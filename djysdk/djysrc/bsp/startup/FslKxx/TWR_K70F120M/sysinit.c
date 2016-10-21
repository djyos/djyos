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
// 文件名     ：sysinit.c
// 模块描述: 系统初始化部分，主要是内核启动相关的初始化，如时钟，总线等
// 模块版本: V1.00
// 创建人员: hm
// 创建时间: 06/06.2014
// =============================================================================

#include "stdint.h"
#include "cpu_peri.h"
#include "board-config.h"

#define CLK0_FREQ_HZ        50000000    //主时钟晶振
#define PLL0_PRDIV          5           //PLL分频
#define PLL0_VDIV           24          //PLL倍频

// =============================================================================
// 功能：PLL初始化部分，主要对PLL和晶振初始化，是时间从内部时钟切换到外部时钟的主要部
//       分，此处使用PLL0和OSC0作为内部时钟源，经分频和倍频，使内核时钟达到150M
// 参数：crystal_fre_hz,外部晶振频率
//       prdiv,PLL分频参数
//       vdiv,PLL倍频参数
// 返回：实际配置的MCG输出频率
// =============================================================================
u32 PLL_Init(u32 crystal_fre_hz, u8 prdiv, u8 vdiv)
{
    //reset后，系统处于FEI模式，时钟配置过程为：
    //FEI--->FBE--->PBE--->PEE(手册640页table25-22)

    //默认使用振荡器0，设置振荡器0
    // 配置控制寄存器MCG_C2
    // 先清bit位，配置高速晶振，high-gain operation，外部晶振
    MCG->C2 &= ~(MCG_C2_RANGE0_MASK | MCG_C2_HGO0_MASK | MCG_C2_EREFS0_MASK);
    MCG->C2 |= (MCG_C2_RANGE0(1)
                | (0 << MCG_C2_HGO0_SHIFT)
                | (0 << MCG_C2_EREFS0_SHIFT));

    // 配置控制寄存器MCG_C1
    //先清bit位，CLK为外部时钟，FRDIV为5时，配置分频为1024，即配置FLL处于
    //31.25-39.0625 kHz 之间
    MCG->C1 &= ~(MCG_C1_CLKS_MASK | MCG_C1_FRDIV_MASK | MCG_C1_IREFS_MASK);
    MCG->C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(5);

    while((MCG->S & MCG_S_IREFST_MASK));    //等待FLL时钟转为外部源
    while(((MCG->S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2);

    // Now in FBE
    MCG->C6 |= MCG_C6_CME0_MASK;
    // start Configure PLL0
    MCG->C5 &= ~MCG_C5_PLLREFSEL0_MASK;         // ensure OSC0
    MCG->C11 &= ~MCG_C11_PLLCS_MASK;            // select PLL0

    // Configure MCG_C5
    MCG->C5 &= ~MCG_C5_PRDIV0_MASK;             // clear settings
    MCG->C5 |= MCG_C5_PRDIV0(prdiv - 1);            //set PLL ref divider

    // Configure MCG_C6
    MCG->C6 &= ~MCG_C6_VDIV0_MASK;                          // clear settings
    MCG->C6 |= MCG_C6_PLLS_MASK | MCG_C6_VDIV0(vdiv - 16); // write new VDIV

    while(!(MCG->S & MCG_S_PLLST_MASK));        // wait for PLLST status bit to set
    while(!(MCG->S & MCG_S_LOCK0_MASK));        // Wait for LOCK bit to set

    // Use actual PLL settings to calculate PLL frequency
    prdiv = ((MCG->C5 & MCG_C5_PRDIV0_MASK) + 1);
    vdiv = ((MCG->C6 & MCG_C6_VDIV0_MASK) + 16);

    // now in PBE
    MCG->C1 &= ~MCG_C1_CLKS_MASK; // 清CLKS，选择MCG_OUT源为PLL
    while(((MCG->S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3);

    // start Configure PLL1 if needed, for DDR,so, CLK = 100MHz
#if (CN_CFG_DDR_USED == 1)
    // Configure MCG_C12
    MCG->C11 &= ~MCG_C11_PLLREFSEL1_MASK;       // Clear select bit to choose OSC0
    MCG->C11 &= ~MCG_C11_PRDIV1_MASK;
    MCG->C11 |= MCG_C11_PRDIV1(5 - 1);
    MCG->C12 &= ~MCG_C12_VDIV1_MASK;            // clear VDIV settings
    MCG->C12 |=  MCG_C12_VDIV1(24 - 16); // write new VDIV and enable PLL
    // Now enable the PLL
    MCG->C11 |= MCG_C11_PLLCLKEN1_MASK; // Set PLLCLKEN2 to enable PLL1

    while(!(MCG->S2 & MCG_S2_LOCK1_MASK));  // Wait for PLL1 locked
#endif

    //MCGOUT equals PLL output frequency/2
    return (((crystal_fre_hz / prdiv) * vdiv) / 2);
}

// =============================================================================
// 功能：系统时钟初始化，从系统上电使用FEI到切换到使用外部的50M时钟合为时钟源，并经过
//       PLL分频和倍频，使内核时钟为120M，bus时钟为2分频，FlexBus3分频，Flash6分频，
//       各个模块（如portA~E）的时钟各自使用时配置，此处只配置系统时钟
// 参数：无
// 返回：无
// =============================================================================
void SysClockInit(void)
{
    //初始化端口时钟，在系统初始化时全部打开
    SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK
                  | SIM_SCGC5_PORTB_MASK
                  | SIM_SCGC5_PORTC_MASK
                  | SIM_SCGC5_PORTD_MASK
                  | SIM_SCGC5_PORTE_MASK
                  | SIM_SCGC5_PORTF_MASK );

    //初始化PLL前，先初始化系统时钟分步器，分别用于配置
    //core/system时钟、bus时钟、FlexBus时钟、Flash时钟
    SIM->CLKDIV1 = ( 0
                    | SIM_CLKDIV1_OUTDIV1(0)
                    | SIM_CLKDIV1_OUTDIV2(1)
                    | SIM_CLKDIV1_OUTDIV3(2)
                    | SIM_CLKDIV1_OUTDIV4(5) );

    //系统的PLL时钟分频配置，用于生成MCGOUTCLK = 120M
    //对于50M的crystal，分频为5，PLL倍频为24，则主时钟计算公式为：
    //MCGOUTCLK = ((50M/5) * 24)/2 = 120M
    PLL_Init(CLK0_FREQ_HZ,PLL0_PRDIV,PLL0_VDIV);
}

// =============================================================================
// 功能：DDR2初始化，底层硬件的初始化必须保证正确，否则将一直等待
// 参数：无
// 返回：无
// =============================================================================
void SDRAM_Init(void)
{
#if (CN_CFG_DDR_USED == 1)
    /* Enable DDR controller clock */
    SIM->SCGC3 |= SIM_SCGC3_DDR_MASK;

    /* Enable DDR pads and set slew rate */
    SIM->MCR |= 0xC4;   // bits were left out of the manual so there isn't a macro right now

    DDR->RCR |= DDR_RCR_RST_MASK;

    * (vu32 *)(0x400Ae1ac) = 0x01030203;

    DDR->CR00 = 0x00000400;    // DDRCLS = 4 is reserved??
    DDR->CR02 = 0x02000031;
    DDR->CR03 = 0x02020506;
    DDR->CR04 = 0x06090202;
    DDR->CR05 = 0x02020302;
    DDR->CR06 = 0x02904002;
    DDR->CR07 = 0x01000303;
    DDR->CR08 = 0x05030201;
    DDR->CR09 = 0x020000c8;
    DDR->CR10 = 0x03003207;
    DDR->CR11 = 0x01000000;
    DDR->CR12 = 0x04920031;
    DDR->CR13 = 0x00000005;
    DDR->CR14 = 0x00C80002;
    DDR->CR15 = 0x00000032; //  | DDR->CR15_SREF_MASK ;
    DDR->CR16 = 0x00000001;
    DDR->CR20 = 0x00030300;
    DDR->CR21 = 0x00040232;
    DDR->CR22 = 0x00000000;
//    DDR->CR23 = 0x00040302;
    DDR->CR25 = 0x0A010201;
    DDR->CR26 = 0x0101FFFF;
    DDR->CR27 = 0x01010101;
    DDR->CR28 = 0x00000003;
    DDR->CR29 = 0x00000000;
    DDR->CR30 = 0x00000001;
    DDR->CR34 = 0x02020101;
//    DDR->CR36 = 0x01010201;
    DDR->CR37 = 0x00000200;
    DDR->CR38 = 0x00200000;
    DDR->CR39 = 0x01010020;
    DDR->CR40 = 0x00002000;
    DDR->CR41 = 0x01010020;
    DDR->CR42 = 0x00002000;
    DDR->CR43 = 0x01010020;
    DDR->CR44 = 0x00000000;
    DDR->CR45 = 0x03030303;
    DDR->CR46 = 0x02006401;
    DDR->CR47 = 0x01020202;
    DDR->CR48 = 0x01010064;
    DDR->CR49 = 0x00020101;
    DDR->CR50 = 0x00000064;
    DDR->CR52 = 0x02000602;
    DDR->CR53 = 0x03c80000;
    DDR->CR54 = 0x03c803c8;
    DDR->CR55 = 0x03c803c8;
    DDR->CR56 = 0x020303c8;
    DDR->CR57 = 0x01010002;

    asm("NOP");

    DDR->CR00 |= 0x00000001;

    while ((DDR->CR30 & 0x400) != 0x400);

    MCM->CR |= MCM_CR_DDRSIZE(1);
#endif
}

// =============================================================================
// 功能：CACHE初始化，并使能buffer 和cache。K70的cache分为16个region，分别代表了不
//       的地址范围，如手册K70 Sub-Family Reference Manual page 126.将其各个区域
//       配置为默认的cache特性（Write-through、Write-back、non-cacheable）
// 参数：无
// 返回：无
// =============================================================================
void Cache_Init(void)
{
#if (CN_CFG_DDR_USED == 1)
    // Code Cache Init
    /* Cache Set Command: set command bits in CCR */
    /* set invalidate way 1 and invalidate way 0 bits */
    LMEM->PCCCR = LMEM_PCCCR_INVW0_MASK | LMEM_PCCCR_INVW1_MASK;
    /* set ccr[go] bit to initiate command to invalidate cache */
    LMEM->PCCCR |= LMEM_PCCCR_GO_MASK;

//    LMEM->PCCRMR |= LMEM_PCCRMR_R0(0x03) | LMEM_PCCRMR_R8(0x03);
    /* wait until the ccr[go] bit clears to indicate command complete */
    while(((LMEM->PCCCR & LMEM_PCCCR_GO_MASK)>>LMEM_PCCCR_GO_SHIFT)== 0x1)
    {};
    /* enable write buffer */
    LMEM->PCCCR |= LMEM_PCCCR_ENWRBUF_MASK;
    /* enable cache */
    LMEM->PCCCR |= LMEM_PCCCR_ENCACHE_MASK;

    // System Cache Init
    /* Cache Set Command: set command bits in CCR */
    /* set invalidate way 1 and invalidate way 0 bits */
    LMEM->PSCCR = LMEM_PSCCR_INVW0_MASK | LMEM_PSCCR_INVW1_MASK;
    /* set ccr[go] bit to initiate command to invalidate cache */
    LMEM->PSCCR |= LMEM_PSCCR_GO_MASK;

//    LMEM->PSCRMR |= LMEM_PSCRMR_R0(0x03) | LMEM_PSCRMR_R8(0x03);
    /* wait until the ccr[go] bit clears to indicate command complete */
    while(((LMEM->PSCCR & LMEM_PSCCR_GO_MASK)>>LMEM_PSCCR_GO_SHIFT)== 0x1)
    {};
    /* enable write buffer */
    LMEM->PSCCR |= LMEM_PSCCR_ENWRBUF_MASK;
    /* enable cache */
    LMEM->PSCCR |= LMEM_PSCCR_ENCACHE_MASK;

#endif
}


