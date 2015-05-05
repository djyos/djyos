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
//所属模块: CPU系统控制
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 包含时钟、电源等控制代码
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

#include "stdint.h"
#include "cpu_peri.h"

//----使能一个外设-----------------------------------------------------------
//功能: 使能外设,
//参数: module，待操作的外设编号，在cpu_peri_sysctrl.h文件中定义
//返回: 无
//-----------------------------------------------------------------------------
void sys_enable_peri(u32 module)
{
    pg_sysctrl_reg->PRESETCTRL |= (u32)1 << module;
}

//----禁止一个外设-----------------------------------------------------------
//功能: 使能外设,
//参数: module，待操作的外设时钟源编号，在cpu_peri_sysctrl.h文件中定义
//返回: 无
//-----------------------------------------------------------------------------
void sys_disable_peri(u32 module)
{
    pg_sysctrl_reg->PRESETCTRL &= ~((u32)1 << module);
}

//----使能一个外设时钟---------------------------------------------------------
//功能: 使能外设时钟,
//参数: module，待操作的外设时钟源编号，在cpu_peri_sysctrl.h文件中定义
//返回: 无
//-----------------------------------------------------------------------------
void sys_enable_peri_clk(u32 module)
{
    pg_sysctrl_reg->SYSAHBCLKCTRL |= (u32)1 << module;
}

//----禁止一个外设时钟-----------------------------------------------------------
//功能: 使能外设时钟,
//参数: module，待操作的外设编号，在cpu_peri_sysctrl.h文件中定义
//返回: 无
//-----------------------------------------------------------------------------
void sys_disable_peri_clk(u32 module)
{
    pg_sysctrl_reg->SYSAHBCLKCTRL &= ~((u32)1 << module);
}

//----设置uart0时钟除数--------------------------------------------------------
//功能: 设置uart0时钟除数，uart的输入时钟= cn_mclk / dev_num
//参数: dev_num
//返回: 无
//-----------------------------------------------------------------------------
void sys_set_uart0_clkdiv(u32 dev_num)
{
    pg_sysctrl_reg->UART0CLKDIV = (dev_num & 0xFF);
}

//----设置uart1时钟除数--------------------------------------------------------
//功能: 设置uart1时钟除数，uart的输入时钟= cn_mclk / dev_num，0=禁止该时钟
//参数: dev_num
//返回: 无
//-----------------------------------------------------------------------------
void sys_set_uart1_clkdiv(u32 dev_num)
{
    pg_sysctrl_reg->UART1CLKDIV = (dev_num & 0xFF);
}

//----设置io滤波采样时钟除数0--------------------------------------------------
//功能: lpc12xx有7个时钟用于io口输入抗干扰滤波，每个时钟从主频分频，分频数由本组
//      函数配置，每个io口可以独立选择其中一个时钟，0=禁止该时钟
//参数: dev_num
//返回: 无
//-----------------------------------------------------------------------------
void sys_set_iofilter0_clkdiv(u32 dev_num)
{
    pg_sysctrl_reg->FILTERCLKCFG0 = (dev_num & 0xFF);
}

//----设置io滤波采样时钟除数1--------------------------------------------------
//功能: lpc12xx有7个时钟用于io口输入抗干扰滤波，每个时钟从主频分频，分频数由本组
//      函数配置，每个io口可以独立选择其中一个时钟，0=禁止该时钟
//参数: dev_num
//返回: 无
//-----------------------------------------------------------------------------
void sys_set_iofilter1_clkdiv(u32 dev_num)
{
    pg_sysctrl_reg->FILTERCLKCFG1 = (dev_num & 0xFF);
}

//----设置io滤波采样时钟除数2--------------------------------------------------
//功能: lpc12xx有7个时钟用于io口输入抗干扰滤波，每个时钟从主频分频，分频数由本组
//      函数配置，每个io口可以独立选择其中一个时钟，0=禁止该时钟
//参数: dev_num
//返回: 无
//-----------------------------------------------------------------------------
void sys_set_iofilter2_clkdiv(u32 dev_num)
{
    pg_sysctrl_reg->FILTERCLKCFG2 = (dev_num & 0xFF);
}

//----设置io滤波采样时钟除数3--------------------------------------------------
//功能: lpc12xx有7个时钟用于io口输入抗干扰滤波，每个时钟从主频分频，分频数由本组
//      函数配置，每个io口可以独立选择其中一个时钟，0=禁止该时钟
//参数: dev_num
//返回: 无
//-----------------------------------------------------------------------------
void sys_set_iofilter3_clkdiv(u32 dev_num)
{
    pg_sysctrl_reg->FILTERCLKCFG3 = (dev_num & 0xFF);
}

//----设置io滤波采样时钟除数4--------------------------------------------------
//功能: lpc12xx有7个时钟用于io口输入抗干扰滤波，每个时钟从主频分频，分频数由本组
//      函数配置，每个io口可以独立选择其中一个时钟，0=禁止该时钟
//参数: dev_num
//返回: 无
//-----------------------------------------------------------------------------
void sys_set_iofilter4_clkdiv(u32 dev_num)
{
    pg_sysctrl_reg->FILTERCLKCFG4 = (dev_num & 0xFF);
}

//----设置io滤波采样时钟除数5--------------------------------------------------
//功能: lpc12xx有7个时钟用于io口输入抗干扰滤波，每个时钟从主频分频，分频数由本组
//      函数配置，每个io口可以独立选择其中一个时钟，0=禁止该时钟
//参数: dev_num
//返回: 无
//-----------------------------------------------------------------------------
void sys_set_iofilter5_clkdiv(u32 dev_num)
{
    pg_sysctrl_reg->FILTERCLKCFG5 = (dev_num & 0xFF);
}

//----设置io滤波采样时钟除数6--------------------------------------------------
//功能: lpc12xx有7个时钟用于io口输入抗干扰滤波，每个时钟从主频分频，分频数由本组
//      函数配置，每个io口可以独立选择其中一个时钟，0=禁止该时钟
//参数: dev_num
//返回: 无
//-----------------------------------------------------------------------------
void sys_set_iofilter6_clkdiv(u32 dev_num)
{
    pg_sysctrl_reg->FILTERCLKCFG6 = (dev_num & 0xFF);
}

//----读器件ID-----------------------------------------------------------
//功能: 读器件ID
//参数: 无
//返回: 器件编号:
//          LPC1227FBD64/301 = 0x3670 002B
//          LPC1227FBD48/301 = 0x3670 002B
//          LPC1226FBD64/301 = 0x3660 002B
//          LPC1226FBD48/301 = 0x3660 002B
//          LPC1225FBD64/321 = 0x3652 002B
//          LPC1225FBD64/301 = 0x3650 002B
//          LPC1225FBD48/321 = 0x3652 002B
//          LPC1225FBD48/301 = 0x3650 002B
//          LPC1224FBD64/121 = 0x3642 C02B
//          LPC1224FBD64/101 = 0x3640 C02B
//          LPC1224FBD48/121 = 0x3642 C02B
//          LPC1224FBD48/101 = 0x3640 C02B
//-----------------------------------------------------------------------------
u32 sys_get_deviceID(void)
{
    return(pg_sysctrl_reg->DEVICE_ID);
}

