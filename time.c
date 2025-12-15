/*
 * Author: honrun
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "time.h"


/* 实时时钟 */
volatile int64_t g_lTimestamp = 0;
/* 时区：默认东八区 */
volatile static float g_fTimeUTC = 8.0f;

/* 判断是否为闰年 */
#define YEAR_LEAP(year) ((((year) % 4)      != 0) ? 0 : \
                         (((year) % 100)    != 0) ? 1 : \
                         (((year) % 400)    != 0) ? 0 : \
                         (((year) % 3200)   != 0) ? 1 : \
                         (((year) % 172800) != 0) ? 0 : 1)
/* 获取年份天数 */
#define DAYS_OF_THE_YEAR(year) (YEAR_LEAP(year) == 1 ? 366 : 365)
/* 获取月份天数 */
const static uint8_t st_ucMonthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
#define DAYS_OF_THE_MONTH(year, month) ((((month) == 2) && (YEAR_LEAP(year) == 1)) ? 29 : st_ucMonthDays[(month) - 1])


/* 蔡勒公式计算星期几 Christian Zeller */
uint8_t cTimeToWeek(int32_t iYear, uint8_t ucMonth, uint8_t ucDay)
{
    int32_t iCentury = 0;
    int8_t cWeek = 0;

    /* 如果是1、2月份，则需要把月份当作是上年度的13、14月份 */
    if(ucMonth <= 2)
    {
        iYear -= 1;
        ucMonth += 12;
    }

    iCentury = iYear / 100;
    iYear = iYear % 100;

    cWeek = ((iCentury / 4) - (iCentury * 2) + iYear + (iYear / 4) + (13 * (ucMonth + 1) / 5) + ucDay - 1) % 7;
    cWeek = (cWeek < 0) ? (cWeek + 7) : cWeek;

    return (uint8_t)cWeek;
}

/*
 * Return:      void
 * Parameters:  lStamp: UNIX时间戳; ptypeTime:时间结构体指针; cUTC: 时区（东时区为正、西时区为负）
 * Description: 把UNIX时间戳转换成时间结构体
 */
void vStampToTime(TimeType *ptypeTime, int64_t lStamp, float fUTC)
{
    int32_t iYear;
    int8_t cMonth;

    /* 加入时区值 */
    lStamp += (int64_t)(fUTC * 3600.0f);
    ptypeTime->UTC = fUTC;

    if(lStamp >= 0)
    {
        /* 计算全部年份的整年天数（1天有86400秒） */
        for(iYear = 1969; lStamp >= 0; lStamp -= DAYS_OF_THE_YEAR(iYear) * 86400)
            ++iYear;

        /* 计算当前年份下全部月份的整月天数 */
        for(cMonth = 0, lStamp += DAYS_OF_THE_YEAR(iYear) * 86400; lStamp >= 0; lStamp -= DAYS_OF_THE_MONTH(iYear, cMonth) * 86400)
            ++cMonth;

        /* 加上上面循环多减去的1个月 */
        lStamp += DAYS_OF_THE_MONTH(iYear, cMonth) * 86400;
    }
    else
    {
        /* 计算全部年份的整年天数（1天有86400秒） */
        for(iYear = 1970; lStamp < 0; lStamp += DAYS_OF_THE_YEAR(iYear) * 86400)
            --iYear;

        /* 计算当前年份下全部月份的整月天数 */
        for(cMonth = 13, lStamp -= DAYS_OF_THE_YEAR(iYear) * 86400; lStamp < 0; lStamp += DAYS_OF_THE_MONTH(iYear, cMonth) * 86400)
            --cMonth;
    }

    ptypeTime->year = iYear;
    ptypeTime->month = cMonth;
    /* 天从1开始计数 */
    ptypeTime->day = lStamp / 86400 + 1;
    lStamp = (lStamp % 86400) + 86400;
    ptypeTime->hour = lStamp / 3600 % 24;
    ptypeTime->minute = lStamp / 60 % 60;
    ptypeTime->second = lStamp % 60;
    ptypeTime->week = cTimeToWeek(ptypeTime->year, ptypeTime->month, ptypeTime->day);
}

/*
* Return:      UNIX时间戳
* Parameters:  ptypeTime:时间结构体;
* Description: 把时间结构体转换成UNIX时间戳
*/
int64_t lTimeToStamp(TimeType *ptypeTime)
{
    int64_t lDays = 0, lStamp;
    int32_t iYear;
    int8_t cMonth;

    if(ptypeTime->year >= 1970)
    {
        for(iYear = 1970; iYear < ptypeTime->year; ++iYear)
            lDays += DAYS_OF_THE_YEAR(iYear);
    }
    else
    {
        for(iYear = 1969; iYear >= ptypeTime->year; --iYear)
            lDays -= DAYS_OF_THE_YEAR(iYear);
    }

    for(cMonth = 1; cMonth < ptypeTime->month; ++cMonth)
        lDays += DAYS_OF_THE_MONTH(ptypeTime->year, cMonth);

    lDays += ptypeTime->day - 1;

    lStamp  = lDays * 86400;
    lStamp += ptypeTime->hour * 3600;
    lStamp += ptypeTime->minute * 60;
    lStamp += ptypeTime->second;

    /* 加入计算时区值 */
    lStamp -= (int64_t)(ptypeTime->UTC * 3600.0f);

    return lStamp;
}

/* 设置时间戳：单位us */
int64_t lTimestampGet(void)
{
    return g_lTimestamp;
}

/* 获取时间戳：单位us */
void vTimestampSet(int64_t lStamp)
{
    g_lTimestamp = lStamp;
}

/* 时区设置 */
void vTimeUTCSet(float fUTC)
{
    g_fTimeUTC = fUTC;
}

/* 时区获取 */
float fTimeUTCGet(void)
{
    return g_fTimeUTC;
}

char *pcTimeToStrings(int64_t lStamp)
{
    static char cDateStrings[32] = {0};
    TimeType typeTime;
    
    /* 东8区 */
    vStampToTime(&typeTime, lStamp, 8.0f);
    
    memset(cDateStrings, 0, sizeof(cDateStrings));
    sprintf(cDateStrings, "%d/%02d/%02d - %02d:%02d:%02d", (int)typeTime.year, typeTime.month, typeTime.day, typeTime.hour, typeTime.minute, typeTime.second);
    
    return cDateStrings;
}
