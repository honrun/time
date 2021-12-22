#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "time.h"

int main()
{
    int64_t lTimeNow = 0, lTimeOld = 0;

    printf("Time now is:\r\n\n");

    while(1)
    {
        time(&lTimeNow);
        if(lTimeOld != lTimeNow)
        {
            lTimeOld = lTimeNow;

            printf("\r%s - %s", pcStampToDateStrings(lTimeNow), pcStampToTimeStrings(lTimeNow));
        }
    }

    return 0;
}
