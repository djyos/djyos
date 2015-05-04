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
//所属模块：RTC时钟模块
//作者：lst
//版本：V0.1.0
//文件描述：DJYOS与硬件RTC连接及RTC底层驱动
//其他说明：
//修订历史：
//1. 日期: 2013-10-28
//   作者：
//   新版本号：V0.1.0
//   修改说明：原始版本
//------------------------------------------------------

#ifndef _RTC_DS1390_H_
#define _RTC_DS1390_H_

#ifdef  CN_CFG_DJYOS_USED
#define rtc_tm tm

uint32_t rtc_time_get(struct rtc_tm *DateTime);
bool_t rtc_update_time(struct rtc_tm *DateTime);
ptu32_t ModuleInstall_RTC(ptu32_t para);
#endif

#ifdef  cfg_no_os_used
#include "stdint.h"

struct rtc_tm
{
    s32 tm_us;      // 微秒-取值区间[0,999999]
    s32 tm_sec;     // 秒–取值区间为[0,59]
    s32 tm_min;     // 分 - 取值区间为[0,59]
    s32 tm_hour;    // 时 - 取值区间为[0,23]
    s32 tm_mday;    // 一个月中的日期 - 取值区间为[1,31]
    s32 tm_mon;     // 月份（从一月开始，0代表一月） - 取值区间为[0,11]
    s32 tm_year;    // 年份，其值从1900开始
    s32 tm_wday;    // 星期–取值区间为[0,6]，其中0代表星期天，1代表星期一，
                    // 以此类推
    s32 tm_yday;    // 从每年的1月1日开始的天数–取值区间为[0,365]，其中0代表
                    //1月1日，1代表1月2日，以此类推
    s32 tm_isdst;   // 夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行
                    //夏令时的进候，tm_isdst为0；不了解情况时，tm_isdst()为负。
};

uint32_t rtc_time_get(struct rtc_tm *DateTime);
bool_t rtc_update_time(struct rtc_tm *DateTime);
u32 rtc_init(void);
#endif

#endif


