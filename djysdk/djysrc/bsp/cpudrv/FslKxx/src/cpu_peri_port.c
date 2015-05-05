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
// 文件名     ：cpu_peri_port.c
// 模块描述: 端口模块配置，端口中断配置和使能
// 模块版本: V1.00
// 创建人员: hm
// 创建时间: 07/06.2014
// =============================================================================

#include "cpu_peri.h"

static PORT_Type volatile * const PTR_PORT_REG[] = {(PORT_Type *)0x40049000u,
                                            (PORT_Type *)0x4004A000u,
                                            (PORT_Type *)0x4004B000u,
                                            (PORT_Type *)0x4004C000u,
                                            (PORT_Type *)0x4004D000u,
                                            (PORT_Type *)0x4004E000u};

// =============================================================================
// 功能：端口引脚利用功能配置，根据不同的外设对端口的需求，mux配置为不同的值，具体值
//       需查看用户手册每个模块对端口的要求
// 参数：port,配置的端口号，PORT_PORT_A~F
//       pin,引脚，PORT_PIN(x),x从0~31
//       mux,与具体模块相关，若配置为GPIO功能，则为PORT_PINMUX_GPIO
// 返回：NONE
// =============================================================================
void PORT_MuxConfig(u32 port,u32 pin,u8 mux)
{
    u32 temp;
    if((port > PORT_PORT_MAX) || (pin > PORT_PIN_MAX))
        return;

    temp = PTR_PORT_REG[port]->PCR[pin];
    temp &= (u32)~0x700u;                           //clear specific bits
    PTR_PORT_REG[port]->PCR[pin] = temp | (mux << 8);//write mux to register
}

// =============================================================================
// 功能：中断配置，包含触发方式，内部的上下拉配置等,调用该函数实质就等同于使能了中断
// 参数：port,配置的端口号，PORT_PORT_A~F
//       pin,引脚，PORT_PIN(x),x从0~31
//       trig,触发方式，分为边沿触发和水平触发，边沿分为上、下方式
//       pull,上下拉方式
// 返回：0无中断标志，非0有中断标志
// =============================================================================
void PORT_IntConfig(u32 port,u32 pin,u8 trig,u8 pull)
{
    u32 temp;
    if((port > PORT_PORT_MAX) || (pin > PORT_PIN_MAX))
        return;

    temp = PTR_PORT_REG[port]->PCR[pin];
    //configure int
    temp &= (u32)~0xF0000u;                             //clear specific bits
    PTR_PORT_REG[port]->PCR[pin] = temp | (trig << 16); //write trig to register

    //pull
    if(pull == PORT_PULL_NONE)
    {
        PTR_PORT_REG[port]->PCR[pin] &= (u32)~0x02u;
    }
    else if(pull == PORT_PULL_UP)
    {
        PTR_PORT_REG[port]->PCR[pin] |= 0x02u;
        PTR_PORT_REG[port]->PCR[pin] |= 0x01u;
    }
    else if(pull == PORT_PULL_DOWN)
    {
        PTR_PORT_REG[port]->PCR[pin] |= 0x02u;
        PTR_PORT_REG[port]->PCR[pin] &= (u32)~0x01u;
    }
    else
    {
        return;
    }
}

// =============================================================================
// 功能：禁止指定中断，使中断请求失能
// 参数：port,配置的端口号，PORT_PORT_A~F
//       pin,引脚，PORT_PIN(x),x从0~31
// 返回：无
// =============================================================================
void PORT_IntDisable(u32 port, u32 pin)
{
    if((port > PORT_PORT_MAX) || (pin > PORT_PIN_MAX))
        return;

    PTR_PORT_REG[port]->PCR[pin] &= (u32)(~0xF0000u);
}

// =============================================================================
// 功能：清除指定中断标志位，往指定寄存器标志位写1，中断退出时必须清标志
// 参数：port,配置的端口号，PORT_PORT_A~F
//       pin,引脚，PORT_PIN(x),x从0~31
// 返回：无
// =============================================================================
void PORT_IntFlagClear(u32 port, u32 pin)
{
    if((port > PORT_PORT_MAX) || (pin > PORT_PIN_MAX))
        return;

    PTR_PORT_REG[port]->PCR[pin] |= (u32)(1<<24);
}

// =============================================================================
// 功能：读中断标志位，每一个引脚都有一个对应的中断标志位，可获得中断是否发生
// 参数：port,配置的端口号，PORT_PORT_A~F
//       pin,引脚，PORT_PIN(x),x从0~31
// 返回：0无中断标志，非0有中断标志
// =============================================================================
u8   PORT_IntStatusGet(u32 port,u32 pin)
{
    if((port > PORT_PORT_MAX) || (pin > PORT_PIN_MAX))
        return 0;

    if(PTR_PORT_REG[port]->ISFR & (1<<pin))
    {
        return 1;
    }else
    {
        return 0;
    }
}
