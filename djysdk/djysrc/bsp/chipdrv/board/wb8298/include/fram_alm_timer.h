/*
 * fram_alm_timer.h
 *
 *  Created on: Sep 26, 2013
 *      Author: Administrator
 */

#ifndef FRAM_ALM_TIMER_H_
#define FRAM_ALM_TIMER_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "sunri_types.h"

typedef struct                  // 结构中数据以BCD码形式表示，如day＝0x16，代表16号，而不是22号
{
    UINT8      byAlmHundredSec;          // 秒的百分之一
    UINT8      byAlmSecond;              // 秒
    UINT8      byAlmMinute;            // 分
    UINT8      byAlmHour;                // 小时
    UINT8      byAlmDate;              // 日 2009-9-14 14:18:18
//  UINT8      byAlmDay;                //
//    UINT8      byAlmMonth;                // 月
//  UINT8      byAlmYearL;                // 年
} tagAlmTime;



STATUS FRAM_Set_Alm_Time(tagAlmTime *pTime);
STATUS FRAM_Get_Alm_Time(tagAlmTime *pTime);

#ifdef __cplusplus
}
#endif


#endif
