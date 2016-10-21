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
//所属模块:  CPU初始化
//作者：     lst
//版本：      V1.0.0
//初始版本完成日期：2009-08-05
//文件描述:  cpu时钟初始化
//其他说明:  无
//修订历史:
//2. ...
//1. 日期: 2009-08-05
//   作者: lst
//   新版本号：V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "cortexm0.h"
#include "cpu_peri.h"
#include "LPC12xx.h"
#include "core_cmFunc.h"

#define SYSOSCCTRL_Val      0x00000000      //bit0:0=晶体，1=晶振
                                            //bit1:0=1~20M,1=15~25M
#define SYSPLLCLKSEL_Val    0x00000001      //选pll时钟，0=rc,1=晶振，2=wdtclk

#define cn_SYSPLLCTRL_MSEL  3               //pll除数=MSEL+1
#define cn_SYSPLLCTRL_PSEL  2               //pll乘数
//主频配置为40M，配置公式较复杂，参看nxp的手册
#define SYSPLLCTRL_Val      ((cn_SYSPLLCTRL_PSEL<<5) + cn_SYSPLLCTRL_MSEL)

//选主时钟，0=rc，1=pll输入，2=wdt振荡器，3=pll输出
#define MAINCLKSEL_Val        0x00000003

//AHB总线时钟除数，1~255,0=禁止总线时钟
#define SYSAHBCLKDIV_Val      0x00000001

#define CLKOUTCLKSEL_Val      0x00000003    //clkout选择主时钟
#define CLKOUTCLKDIV_Val      0x00000001    //clkout时钟除法
extern   uint32_t   msp_top[ ];

void Init_Cpu (void)
{
    u32 i;

    __set_PSP(msp_top);
    __set_MSP(msp_top);
    __set_PRIMASK(1);
    __set_CONTROL(0);

    pg_sysctrl_reg->SYSMEMREMAP = 0x2;    // remap to internal FLASH
    pg_sysctrl_reg->PRESETCTRL &= ~(1<<15); //ADD BY HUANGCW  设置flash为多周期读取。周期数配置FLASHCFG寄存器

#if 1
//    First, below lines are for debugging only. For future release, WDT is
//    enabled by bootrom, thus, unless a feed to WDT continuously, or WDT timeout
//    will occur. If it's happen, WDT interrupt will be pending until a INT_CLEAR
//    is applied. Below logic is to prevent system from going to the WDT interrupt
//    during debugging.
//    Second, all the peripheral clocks seem to be enabled by bootrom, it's
//    not consistent with the UM. In below lines, only SYS, ROM, RAM, FLASHREG,
//    FLASHARRAY, and I2C are enabled per UM dated July 14th.
    pg_wdt_reg->MOD = 0x00;
    pg_wdt_reg->FEED = 0xAA;        // Feeding sequence
    pg_wdt_reg->FEED = 0x55;

    pg_nvic_reg->clrpend |= 0xFFFFFFFF;
    pg_sysctrl_reg->SYSAHBCLKCTRL = 0x00000001F;
#endif

    // bit 0 default is crystal bypass,
    //bit1 0=0~20Mhz crystal input, 1=15~50Mhz crystal input.
    pg_sysctrl_reg->SYSOSCCTRL = 0x00;
    // main system OSC run is cleared, bit 5 in PDRUNCFG register
    pg_sysctrl_reg->PDRUNCFG     &= ~(1 << 5);          // Power-up System Osc
    pg_sysctrl_reg->SYSOSCCTRL    = SYSOSCCTRL_Val;
    // Wait 200us for OSC to be stablized, 此时用的是rc振荡，200us是可信的.
    for (i = 0; i < 400; i++) ;

    pg_sysctrl_reg->SYSPLLCLKSEL  = SYSPLLCLKSEL_Val;   // Select PLL Input
    pg_sysctrl_reg->SYSPLLCLKUEN  = 0x01;               // Update Clock Source
    pg_sysctrl_reg->SYSPLLCLKUEN  = 0x00;               // Toggle Update Register
    pg_sysctrl_reg->SYSPLLCLKUEN  = 0x01;
    while (!(pg_sysctrl_reg->SYSPLLCLKUEN & 0x01));     // Wait Until Updated
                                // System PLL Setup
    pg_sysctrl_reg->SYSPLLCTRL    = SYSPLLCTRL_Val;
    pg_sysctrl_reg->PDRUNCFG     &= ~(1 << 7);          // Power-up SYSPLL
    while (!(pg_sysctrl_reg->SYSPLLSTAT & 0x01));         // Wait Until PLL Locked

    pg_sysctrl_reg->MAINCLKSEL    = MAINCLKSEL_Val;     // Select PLL Clock Output
    pg_sysctrl_reg->MAINCLKUEN    = 0x01;               // Update MCLK Clock Source
    pg_sysctrl_reg->MAINCLKUEN    = 0x00;               // Toggle Update Register
    pg_sysctrl_reg->MAINCLKUEN    = 0x01;
    while (!(pg_sysctrl_reg->MAINCLKUEN & 0x01));       // Wait Until Updated

    pg_sysctrl_reg->SYSAHBCLKDIV  = SYSAHBCLKDIV_Val;

    pg_sysctrl_reg->CLKOUTCLKSEL = CLKOUTCLKSEL_Val;
    pg_sysctrl_reg->CLKOUTDIV = CLKOUTCLKDIV_Val;

    // System clock to the IOCON needs to be enabled or
    //most of the I/O related peripherals won't work.
    pg_sysctrl_reg->SYSAHBCLKCTRL |= (1<<16);

    Pre_Start();
}

