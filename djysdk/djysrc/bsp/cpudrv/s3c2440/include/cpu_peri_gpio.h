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
//所属模块:IO初始化
//作者：lst
//版本：V1.0.0
//文件描述:IO初始化
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-006-10
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __GPIO_H__
#define __GPIO_H__
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

struct gpio_reg         //地址在0x56000000
{
    vu32 GPACON;     //0x56000000
    vu32 GPADAT;     //0x56000004
    vu32 rev1;      //0x56000008
    vu32 rev2;      //0x5600000c
    vu32 GPBCON;     //0x56000010
    vu32 GPBDAT;     //0x56000014
    vu32 GPBUP ;     //0x56000018
    vu32 rev4;      //0x5600001c
    vu32 GPCCON;     //0x56000020
    vu32 GPCDAT;     //0x56000024
    vu32 GPCUP ;     //0x56000028
    vu32 rev5;      //0x5600002c
    vu32 GPDCON;     //0x56000030
    vu32 GPDDAT;     //0x56000034
    vu32 GPDUP ;     //0x56000038
    vu32 rev6;      //0x5600003c
    vu32 GPECON;     //0x56000040
    vu32 GPEDAT;     //0x56000044
    vu32 GPEUP ;     //0x56000048
    vu32 rev7;      //0x5600001c
    vu32 GPFCON;     //0x56000050
    vu32 GPFDAT;     //0x56000054
    vu32 GPFUP ;     //0x56000058
    vu32 rev8;      //0x5600005c
    vu32 GPGCON;     //0x56000060
    vu32 GPGDAT;     //0x56000064
    vu32 GPGUP ;     //0x56000068
    vu32 rev9;      //0x5600006c
    vu32 GPHCON;     //0x56000070
    vu32 GPHDAT;     //0x56000074
    vu32 GPHUP ;     //0x56000078
    vu32 rev10;      //0x5600007c
    vu32 MISCCR;     //0x56000080
    vu32 DCLKCON;     //0x56000084
    vu32 EXTINT0;     //0x56000088
    vu32 EXTINT1;      //0x5600008c
    vu32 EXTINT2;      //0x56000090
    vu32 EINTFLT0;     //0x56000094
    vu32 EINTFLT1;     //0x56000098
    vu32 EINTFLT2;      //0x5600009c
    //vu32 EINTFLT3;      //0x4C0000a0???
    vu32 rev11;         //0x560000a0
    vu32 EINTMASK;      //0X560000a4
    vu32 EINTPEND;      //0X560000a8
};

#ifdef __cplusplus
}
#endif

#endif /*__GPIO_H__*/
