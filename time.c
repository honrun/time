/*
 * Author: honrun
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "time.h"

static int64_t st_lUNIXTimeStamp = 0;
const static uint8_t st_ucMonthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};



/* ���չ�ʽ�������ڼ� Christian Zeller */
uint8_t cTimeToWeek(int32_t iYear, uint8_t ucMonth, uint8_t ucDay)
{
    int32_t iCentury = 0;
    int8_t cWeek = 0;


    /* �����1��2�·ݣ�����Ҫ���·ݵ���������ȵ�13��14�·� */
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
 * Parameters:  lStamp: UNIXʱ���; ptypeTime:ʱ��ṹ��ָ��; cUTC: ʱ������ʱ��Ϊ������ʱ��Ϊ����
 * Description: ��UNIXʱ���ת����ʱ��ṹ��
 */
void vStampToTime(int64_t lStamp, TimeType *ptypeTime, int8_t cUTC)
{
    int64_t lDaysNumber = 0;
    int32_t lYearTemp = 1970;
    uint8_t ucMonthTemp = 1;

    /* ����ʱ��ֵ */
    lStamp += cUTC * 3600;
    ptypeTime->cUTC = cUTC;

	/* ��ȥȫ����ݵ�����������1����86400�룩 */
    for(lDaysNumber = lStamp / 86400; lDaysNumber >= 0; ++lYearTemp)
        lDaysNumber -= DAYS_OF_THE_YEAR(lYearTemp);

	/* ����ѭ���������1�� */
    ptypeTime->year = lYearTemp - 1;

	/* ��������ѭ�����ȥ��1�꣬��ȥ��ǰ�����ȫ���·ݵ��������� */
    for(lDaysNumber += DAYS_OF_THE_YEAR(ptypeTime->year); lDaysNumber >= 0; ++ucMonthTemp)
        lDaysNumber -= DAYS_OF_THE_MONTH(ptypeTime->year, ucMonthTemp);

	/* ����ѭ���������1�� */
    ptypeTime->month = ucMonthTemp - 1;

	/* ��������ѭ�����ȥ��1�� */
    lDaysNumber += DAYS_OF_THE_MONTH(ptypeTime->year, ptypeTime->month);

	/* ���1��ʼ���� */
    ptypeTime->day = lDaysNumber + 1;

    ptypeTime->hour = lStamp / 3600 % 24;

    ptypeTime->minute = lStamp / 60 % 60;

    ptypeTime->second = lStamp % 60;

	ptypeTime->week = cTimeToWeek(ptypeTime->year, ptypeTime->month, ptypeTime->day);
}

/*
 * Return:      UNIXʱ���
 * Parameters:  ptypeTime:ʱ��ṹ��;
 * Description: ��ʱ��ṹ��ת����UNIXʱ���
 */
int64_t lTimeToStamp(TimeType typeTime)
{
    int64_t lDaysNumber = 0, lStamp = 0;
    int32_t lYearTemp = 1970;
    uint8_t ucMonthTemp = 1;

    for(lYearTemp = 1970; lYearTemp < typeTime.year; ++lYearTemp)
        lDaysNumber += DAYS_OF_THE_YEAR(lYearTemp);

    for(ucMonthTemp = 1; ucMonthTemp < typeTime.month; ++ucMonthTemp)
        lDaysNumber += DAYS_OF_THE_MONTH(typeTime.year, ucMonthTemp);

    lDaysNumber += typeTime.day - 1;

    lStamp = lDaysNumber * 86400;
    lStamp += typeTime.hour * 3600;
    lStamp += typeTime.minute * 60;
    lStamp += typeTime.second;

    /* �������ʱ��ֵ */
    lStamp -= typeTime.cUTC * 3600;

    return lStamp;
}

int64_t lTimeGetStamp(void)
{
    return st_lUNIXTimeStamp;
}

void vTimeSetStamp(int64_t lStamp)
{
    st_lUNIXTimeStamp = lStamp;
}


char *pcStampToTimeStrings(int64_t lStamp)
{
    TimeType typeTime = {0};
    static char cTimeStrings[16] = {0};

    /* ��8�� */
    vStampToTime(lStamp, &typeTime, 8);

    memset(cTimeStrings, 0, sizeof(cTimeStrings));
    sprintf(cTimeStrings, "%02d:%02d:%02d", typeTime.hour, typeTime.minute, typeTime.second);

    return cTimeStrings;
}

char *pcStampToDateStrings(int64_t lStamp)
{
    TimeType typeTime = {0};
    static char cDateStrings[16] = {0};

    /* ��8�� */
    vStampToTime(lStamp, &typeTime, 8);

    memset(cDateStrings, 0, sizeof(cDateStrings));
    sprintf(cDateStrings, "%d/%02d/%02d", (int)typeTime.year, typeTime.month, typeTime.day);

    return cDateStrings;
}
