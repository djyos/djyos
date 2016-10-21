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
// 文件名     ：cpu_peri_rtc.h
// 模块描述: RTC时钟模块
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 25/09.2014
// =============================================================================
#ifndef __CPU_PERI_RTC_H__
#define __CPU_PERI_RTC_H__
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

// RTC
#ifdef __BIG_ENDIAN
#define rRTCCON    (*(volatile unsigned char *)0x57000043)  //RTC control
#define rTICNT     (*(volatile unsigned char *)0x57000047)  //Tick time count
#define rRTCALM    (*(volatile unsigned char *)0x57000053)  //RTC alarm control
#define rALMSEC    (*(volatile unsigned char *)0x57000057)  //Alarm second
#define rALMMIN    (*(volatile unsigned char *)0x5700005b)  //Alarm minute
#define rALMHOUR   (*(volatile unsigned char *)0x5700005f)  //Alarm Hour
#define rALMDATE   (*(volatile unsigned char *)0x57000063)  //Alarm date   //edited by junon
#define rALMMON    (*(volatile unsigned char *)0x57000067)  //Alarm month
#define rALMYEAR   (*(volatile unsigned char *)0x5700006b)  //Alarm year
#define rRTCRST    (*(volatile unsigned char *)0x5700006f)  //RTC round reset
#define rBCDSEC    (*(volatile unsigned char *)0x57000073)  //BCD second
#define rBCDMIN    (*(volatile unsigned char *)0x57000077)  //BCD minute
#define rBCDHOUR   (*(volatile unsigned char *)0x5700007b)  //BCD hour
#define rBCDDATE   (*(volatile unsigned char *)0x5700007f)  //BCD date  //edited by junon
#define rBCDDAY    (*(volatile unsigned char *)0x57000083)  //BCD day   //edited by junon
#define rBCDMON    (*(volatile unsigned char *)0x57000087)  //BCD month
#define rBCDYEAR   (*(volatile unsigned char *)0x5700008b)  //BCD year

#else //Little Endian
#define rRTCCON    (*(volatile unsigned char *)0x57000040)  //RTC control
#define rTICNT     (*(volatile unsigned char *)0x57000044)  //Tick time count
#define rRTCALM    (*(volatile unsigned char *)0x57000050)  //RTC alarm control
#define rALMSEC    (*(volatile unsigned char *)0x57000054)  //Alarm second
#define rALMMIN    (*(volatile unsigned char *)0x57000058)  //Alarm minute
#define rALMHOUR   (*(volatile unsigned char *)0x5700005c)  //Alarm Hour
#define rALMDATE   (*(volatile unsigned char *)0x57000060)  //Alarm date  // edited by junon
#define rALMMON    (*(volatile unsigned char *)0x57000064)  //Alarm month
#define rALMYEAR   (*(volatile unsigned char *)0x57000068)  //Alarm year
#define rRTCRST    (*(volatile unsigned char *)0x5700006c)  //RTC round reset
#define rBCDSEC    (*(volatile unsigned char *)0x57000070)  //BCD second
#define rBCDMIN    (*(volatile unsigned char *)0x57000074)  //BCD minute
#define rBCDHOUR   (*(volatile unsigned char *)0x57000078)  //BCD hour
#define rBCDDATE   (*(volatile unsigned char *)0x5700007c)  //BCD date  //edited by junon
#define rBCDDAY    (*(volatile unsigned char *)0x57000080)  //BCD day   //edited by junon
#define rBCDMON    (*(volatile unsigned char *)0x57000084)  //BCD month
#define rBCDYEAR   (*(volatile unsigned char *)0x57000088)  //BCD year
#endif  //RTC

#ifdef __cplusplus
}
#endif

#endif // __CPU_PERI_RTC_H__

