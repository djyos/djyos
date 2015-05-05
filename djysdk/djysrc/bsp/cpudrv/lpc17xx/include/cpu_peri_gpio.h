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
//------------------------------------------------------------------------------
//Copyright (C), 2005-2009,  lst.
//版权所有 (C), 2005-2009,   lst.
//所属模块:IO初始化
//作者：lst
//版本：V1.0.0
//文件描述:IO初始化
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-03-10
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------------------------------
#ifndef __CPU_PERI_GPIO_REG__
#define __CPU_PERI_GPIO_REG__

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
// 定义IO模式
#define CN_GPIO_MODE_OD_PULLUP            0x00    //开漏，上拉
#define CN_GPIO_MODE_OD_REPEATER          0x01    //开漏，中继(自适应)
#define CN_GPIO_MODE_OD_NOPULL            0x02    //开漏，无上下拉
#define CN_GPIO_MODE_OD_PULLDOWN          0x03    //开漏，下拉
#define CN_GPIO_MODE_NOOD_PULLUP          0x04    //无开漏，上拉
#define CN_GPIO_MODE_NOOD_REPEATER        0x05    //无开漏，中继(自适应)
#define CN_GPIO_MODE_NOOD_NOPULL          0x06    //无开漏，无上下拉
#define CN_GPIO_MODE_NOOD_PULLDOWN        0x07    //无开漏，下拉

// GPIO 模块的API申明
void GPIO_CfgPinFunc(u32 port,u32 pinnum,u32 func_no);
void GPIO_CfgPinMode(u32 port,u32 pinnum,u32 mode);
void GPIO_SetToIn(u32 port,u32 msk);
void GPIO_SetToOut(u32 port,u32 msk);
u32  GPIO_DataGet(u32 port);
void GPIO_DataOut(u32 port,u32 data);
void GPIO_SetToHigh(u32 port,u32 msk);
void GPIO_SetToLow(u32 port,u32 msk);
void GPIO_PowerOn(u32 port);
void GPIO_PowerOff(u32 port);

// GPIO外部中断配置
void GPIO_CfgIntMode(u32 port,u32 pinnum,u32 mode);
u32 GPIO_GetIntStatus(u32 port,u32 msk,u32 mode);
void GPIO_ClearIntStatus(u32 port,u32 msk);

#ifdef __cplusplus
}
#endif

#endif /*__CPU_PERI_GPIO_REG__*/
