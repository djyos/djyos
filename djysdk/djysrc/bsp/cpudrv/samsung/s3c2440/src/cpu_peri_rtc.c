//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
// Copyright (c) 2014 著作权由深圳鹏瑞软件有限公司所有。著作权人保留一切权利。
//
// 这份授权条款，在使用者符合下列条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------
// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：cpu_peri_rtc.c
// 模块描述: RTC模块，当系统走时时间配置为RTC时，通过RTC秒中断，更新系统时间，使系统
//           走时时基在断电情况下依然正常运行
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 25/09.2014
// =============================================================================

#include "stdint.h"
#include "os.h"
#include "cpu_peri_rtc.h"

#define HexToBcd(x) ((((x) / 10) <<4) + ((x) % 10))
#define BcdToHex(x) ((((x) & 0xF0) >>4) * 10 + ((x) & 0x0F))

// =============================================================================
// 功能：RTC中断，使用RTC模块的TICK计数器，配置为每一秒中断一次，并在中断中读取RTC
//       时钟，并配置系统时钟
// 参数：中断线号
// 返回：0
// =============================================================================
u32 RTC_ISR(ufast_t rtc_int_line)
{
    struct tm dtm;

    rRTCCON = 1 ;       //RTC read and write enable
    dtm.tm_year = BcdToHex(rBCDYEAR) + 2000;
    dtm.tm_mon  = BcdToHex(rBCDMON);
    dtm.tm_mday = BcdToHex(rBCDDATE);
    dtm.tm_hour = BcdToHex(rBCDHOUR);
    dtm.tm_min  = BcdToHex(rBCDMIN);
    dtm.tm_sec  = BcdToHex(rBCDSEC);
    rRTCCON &= ~1 ;     //RTC read and write disable

    Tm_SetDateTime(&dtm); // 向OS的Clock模块传递新的时间值
    return 0;
}

// =============================================================================
// 功能：RTC时钟更新，将系统时间更新到RTC模块，由于时钟模块的年存储的最大值为99，因此
//       本RTC模块设计从2000为起始值，如2014，则RTC模块的rBCDYEAR存储的是14
// 参数：dtm，更新时间
// 返回：true,成功;false,失败
// =============================================================================
bool_t RTC_TimeUpdate(s64 time)
{
    bool_t result = false;
    struct tm dtm;
    s64 time_s;

    time_s = time/1000000;
    Tm_LocalTime_r(&time_s,&dtm);

    if((dtm.tm_year > 2000) && (dtm.tm_year < 2099))
    {
        rRTCCON = 1 ;       //RTC read and write enable
        rBCDYEAR = HexToBcd(dtm.tm_year - 2000);
        rBCDMON  = HexToBcd(dtm.tm_mon);
        rBCDDATE = HexToBcd(dtm.tm_mday);
        rBCDHOUR = HexToBcd(dtm.tm_hour);
        rBCDMIN  = HexToBcd(dtm.tm_min);
        rBCDSEC  = HexToBcd(dtm.tm_sec);
        rRTCCON &= ~1 ;     //RTC read and write disable
        result = true;
    }
    return result;
}

// =============================================================================
// 功能：获取RTC时钟，读取RTC模块的时间，由于时钟模块的年存储的最大值为99，因此需将读
//       到的年份加基数年2000，如，读RTC模块的rBCDYEAR为14，则为2014年
// 参数：dtm，更新时间
// 返回：true
// =============================================================================
bool_t RTC_TimeGet(s64 *time)
{
    struct tm dtm;

    rRTCCON = 1 ;       //RTC read and write enable
    dtm.tm_year = BcdToHex(rBCDYEAR) + 2000;
    dtm.tm_mon  = BcdToHex(rBCDMON);
    dtm.tm_mday = BcdToHex(rBCDDATE);
    dtm.tm_hour = BcdToHex(rBCDHOUR);
    dtm.tm_min  = BcdToHex(rBCDMIN);
    dtm.tm_sec  = BcdToHex(rBCDSEC);
    rRTCCON &= ~1 ;     //RTC read and write disable

    *time = 1000000 * Tm_MkTime(&dtm);
    return true;
}

// =============================================================================
// 功能：初始化RTC模块.若时钟计数器复位过，则设置默认时间，否则读RTC时间，并配置系统
//       时间
// 参数：模块初始化函数没有参数
// 返回：true = 成功初始化，false = 初始化失败
// =============================================================================
ptu32_t RTC_ModuleInit(ptu32_t para)
{
    struct tm DateTime;
    bool_t RtcIsReset = false;

    //判断时钟计数器是否被复位，即是否断电池
    rRTCCON = 1 ;
    RtcIsReset = rRTCCON & (1<<3);
    rRTCCON &= ~1 ;

    if(RtcIsReset == true)
    {
        DateTime.tm_sec = 0;
        DateTime.tm_min = 0;
        DateTime.tm_hour = 0;
        DateTime.tm_mday = 1;
        DateTime.tm_mon  = 1;
        DateTime.tm_yday = 2000;
        RTC_TimeUpdate(&DateTime);
        Tm_SetDateTime(&DateTime);
    }
    Rtc_RegisterDev(RTC_TimeGet,RTC_TimeUpdate);
    return true;
}


