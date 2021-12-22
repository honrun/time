#ifndef _DevicesTime_H_
#define _DevicesTime_H_
/*
 * Author: honrun
 */


#include <stdint.h>



/* 闰年 */
#ifndef USER_LEAP_YEAR
#define USER_LEAP_YEAR   1
#endif // LEAP_YEAR

/* 平年 */
#ifndef USER_COMMON_YEAR
#define USER_COMMON_YEAR 0
#endif // COMMON_YEAR


/* 判断是否为闰年 */
#define YEAR_LEAP(year) (((((year) % 4 == 0) && ((year) % 100 != 0)) || (((year) % 400 == 0) && ((year) % 3200 != 0)) || ((year) % 172800 == 0)) ? USER_LEAP_YEAR : USER_COMMON_YEAR)
/* 获取年份天数 */
#define DAYS_OF_THE_YEAR(year) (YEAR_LEAP(year) == USER_LEAP_YEAR ? 366 : 365)
/* 获取月份天数 */
#define DAYS_OF_THE_MONTH(year, month) ((((month) == 2) && (YEAR_LEAP(year) == USER_LEAP_YEAR)) ? 29 : st_ucMonthDays[(month) - 1])



typedef struct{
    int32_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
	uint8_t week;
    int8_t cUTC;
}TimeType;



void vStampToTime(int64_t lStamp, TimeType *ptypeTime, int8_t cUTC);
int64_t lTimeToStamp(TimeType typeTime);
int64_t lTimeGetStamp(void);
void vTimeSetStamp(int64_t lStamp);

char *pcStampToTimeStrings(int64_t lStamp);
char *pcStampToDateStrings(int64_t lStamp);


#endif
