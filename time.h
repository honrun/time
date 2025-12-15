/*
* Author: honrun
*/
#ifndef _Time_H_
#define _Time_H_

#include <stdint.h>


typedef struct{
    float UTC;
    int32_t year;
    uint8_t month   : 4;
    uint8_t day     : 5;
    uint8_t hour    : 5;
    uint8_t minute  : 6;
    uint8_t second  : 6;
    uint8_t week    : 3;
}TimeType;


/* 系统计时时基，不可更改 */
extern volatile int64_t g_lTimeBase;


void vStampToTime(TimeType *ptypeTime, int64_t lStamp, float fUTC);
int64_t lTimeToStamp(TimeType *ptypeTime);

void vTimestampSet(int64_t lUNIXTimeStamp);
int64_t lTimestampGet(void);

void vTimeUTCSet(float cUTC);
float fTimeUTCGet(void);

char *pcTimeToStrings(int64_t lStamp);


#endif
