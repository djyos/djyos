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
//所属模块: CPU外设定义
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 包含CPU所有外设的定义，
//      1、不包含内核相关的固定的设备，比如m3的nvic等，所有m3内核均一致。
//      2、不包含内核相关的可选配但固定的定义，比如m3的mpu部件，不一定所有m3内核
//         的芯片都有，但只有有无的区别，不存在变化。
//         以上两部分在相应的内核目录下定义。
//      3、包含内核相关的可变部件定义
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __LPC12XX_SYSCTRL_H__
#define __LPC12XX_SYSCTRL_H__

#ifdef __cplusplus
extern "C" {
#endif

//外设时钟编号，开启和关闭外设时钟时用
#define cn_peri_clksrc_apb		0	//clock for apb,使用众多apb外设前，先使能
#define cn_peri_clksrc_rom		1	//clock for rom 
#define cn_peri_clksrc_ram		2	//clock for ram 
#define cn_peri_clksrc_flash1	3	//clock for flash 1 
#define cn_peri_clksrc_flash2	4	//clock for flash 2 
#define cn_peri_clksrc_i2c		5	//clock for i2c 
#define cn_peri_clksrc_crc		6	//clock for crc 
#define cn_peri_clksrc_ct16b0	7	//clock for 16 bit counter/timer 0 
#define cn_peri_clksrc_ct16b1	8	//clock for 16 bit counter/timer 1 
#define cn_peri_clksrc_ct32b0	9	//clock for 32 bit counter/timer 0 
#define cn_peri_clksrc_ct32b1	10	//clock for 32 bit counter/timer 1 
#define cn_peri_clksrc_ssp		11	//clock for ssp 
#define cn_peri_clksrc_uart0	12	//clock for uart0 
#define cn_peri_clksrc_uart1	13	//clock for uart1 
#define cn_peri_clksrc_adc		14	//clock for adc 
#define cn_peri_clksrc_wdt		15	//clock for wdt 
#define cn_peri_clksrc_iocon	16	//clock for io configuration block 
#define cn_peri_clksrc_dma		17	//clock for dma 
#define cn_peri_clksrc_ezh		18	//clock for ezh 
#define cn_peri_clksrc_rtc		19	//clock for rtc 
#define cn_peri_clksrc_cmp		20	//clock for cmp 
#define cn_peri_clksrc_gpio3	28	//clock for gpio3 
#define cn_peri_clksrc_gpio2	29	//clock for gpio2 
#define cn_peri_clksrc_gpio1	30	//clock for gpio1 
#define cn_peri_clksrc_gpio0	31	//clock for gpio0 

//外设编号，使能和禁止外设时用
#define cn_peri_ssp     0 
#define cn_peri_i2c     1 
#define cn_peri_uart0   2 
#define cn_peri_uart1   3 
#define cn_peri_ct16b0  4 
#define cn_peri_ct16b1  5 
#define cn_peri_ct32b0  6 
#define cn_peri_ct32b1  7 
#define cn_peri_cmp     8 
#define cn_peri_crc     9 
#define cn_peri_dma     10

struct st_sysctrl_reg
{
    vu32 SYSMEMREMAP;            /*!< Offset: 0x000 System memory remap (R/W) */
    vu32 PRESETCTRL;             /*!< Offset: 0x004 Peripheral reset control (R/W) */
    vu32 SYSPLLCTRL;             /*!< Offset: 0x008 System PLL control (R/W) */
    vu32 SYSPLLSTAT;             /*!< Offset: 0x00C System PLL status (R/ ) */
    vu32 RESERVED0[4];
    
    vu32 SYSOSCCTRL;             /*!< Offset: 0x020 System oscillator control (R/W) */
    vu32 WDTOSCCTRL;             /*!< Offset: 0x024 Watchdog oscillator control (R/W) */
    vu32 IRCCTRL;                /*!< Offset: 0x028 IRC control (R/W) */
    vu32 RTCOSCCTRL;             /*!< Offset: 0x02C RTC oscillator control (R/W) */
    
    vu32 SYSRESSTAT;             /*!< Offset: 0x030 System reset status Register (R/ ) */
    vu32 RESERVED1[3];
    vu32 SYSPLLCLKSEL;           /*!< Offset: 0x040 System PLL clock source select (R/W) */	
    vu32 SYSPLLCLKUEN;           /*!< Offset: 0x044 System PLL clock source update enable (R/W) */
    vu32 RESERVED2[10];
    
    vu32 MAINCLKSEL;             /*!< Offset: 0x070 Main clock source select (R/W) */
    vu32 MAINCLKUEN;             /*!< Offset: 0x074 Main clock source update enable (R/W) */
    vu32 SYSAHBCLKDIV;           /*!< Offset: 0x078 System AHB clock divider (R/W) */
    vu32 RESERVED3[1];
    
    vu32 SYSAHBCLKCTRL;          /*!< Offset: 0x080 System AHB clock control (R/W) */
    vu32 RESERVED4[4];
    vu32 SSPCLKDIV;              /*!< Offset: 0x094 SSP clock divider (R/W) */               
    vu32 UART0CLKDIV;            /*!< Offset: 0x098 UART0 clock divider (R/W) */
    vu32 UART1CLKDIV;            /*!< Offset: 0x09C UART1 clock divider (R/W) */
    
    vu32 RTCCLKDIV;              /*!< Offset: 0x0A0 RTC clock divider (R/W) */
    vu32 RESERVED5[3];       
    vu32 SYSTICKCLKDIV;          /*!< Offset: 0x0B0 SysTick clock divider (R/W) */          
    vu32 RESERVED6[11];
         
    vu32 CLKOUTCLKSEL;           /*!< Offset: 0x0E0 CLKOUT clock source select (R/W) */
    vu32 CLKOUTUEN;              /*!< Offset: 0x0E4 CLKOUT clock source update enable (R/W) */
    vu32 CLKOUTDIV;              /*!< Offset: 0x0E8 CLKOUT clock divider (R/W) */       
    vu32 RESERVED8[5];   
    
    vu32 PIOPORCAP0;             /*!< Offset: 0x100 POR captured PIO status 0 (R/) */            
    vu32 PIOPORCAP1;             /*!< Offset: 0x104 POR captured PIO status 1 (R/) */ 
    vu32 RESERVED9[11];
    
    vu32 FILTERCLKCFG6;
    vu32 FILTERCLKCFG5;
    vu32 FILTERCLKCFG4;
    vu32 FILTERCLKCFG3;          /* Offset 0x140 */
    vu32 FILTERCLKCFG2;
    vu32 FILTERCLKCFG1;
    vu32 FILTERCLKCFG0;	
    	            
    vu32 BODCTRL;                 /*!< Offset: 0x150 BOD control (R/W) */
    vu32 SYSTCKCAL;               /*!< Offset: 0x154 System tick counter calibration (R/W) */              
    vu32 AHBPRIO;                 /*!< Offset: 0x158 AHB priority setting (R/W) */
    
    vu32 RESERVED10[5];    
    vu32 IRQLATENCY;             /*!< Offset: 0x170 IRQ delay (R/W) */
    vu32 INTNMI;                 /*!< Offset: 0x174 NMI interrupt source configuration control (R/W) */
    vu32 RESERVED11[34];          
    
    vu32 STARTAPRP0;             /*!< Offset: 0x200 Start logic edge control Register 0 (R/W) */   
    vu32 STARTERP0;              /*!< Offset: 0x204 Start logic signal enable Register 0 (R/W) */  
    vu32 STARTRSRP0CLR;          /*!< Offset: 0x208 Start logic reset Register 0  ( /W) */
    vu32 STARTSRP0;              /*!< Offset: 0x20C Start logic status Register 0 (R/) */
    vu32 STARTAPRP1;             /*!< Offset: 0x210 Start logic edge control Register 1 (R/W) */   
    vu32 STARTERP1;              /*!< Offset: 0x214 Start logic signal enable Register 1 (R/W) */  
    vu32 STARTRSRP1CLR;          /*!< Offset: 0x218 Start logic reset Register 1  ( /W) */
    vu32 STARTSRP1;              /*!< Offset: 0x21C Start logic status Register 1 (R/) */  
    vu32 RESERVED12[4];
    
    vu32 PDSLEEPCFG;             /*!< Offset: 0x230 Power-down states in Deep-sleep mode (R/W) */
    vu32 PDAWAKECFG;             /*!< Offset: 0x234 Power-down states after wake-up (R/W) */       
    vu32 PDRUNCFG;               /*!< Offset: 0x238 Power-down configuration Register (R/W) */
    
    vu32 RESERVED13[110];
    
    vu32 DEVICE_ID;              /*!< Offset: 0x3F4 Device ID (R/ ) */
};
void sys_enable_peri(u32 module);
void sys_disable_peri(u32 module);
void sys_enable_peri_clk(u32 module);
void sys_disable_peri_clk(u32 module);
void sys_set_uart0_clkdiv(u32 dev_num);
void sys_set_uart1_clkdiv(u32 dev_num);
void sys_set_iofilter0_clkdiv(u32 dev_num);
void sys_set_iofilter1_clkdiv(u32 dev_num);
void sys_set_iofilter2_clkdiv(u32 dev_num);
void sys_set_iofilter3_clkdiv(u32 dev_num);
void sys_set_iofilter4_clkdiv(u32 dev_num);
void sys_set_iofilter5_clkdiv(u32 dev_num);
void sys_set_iofilter6_clkdiv(u32 dev_num);
u32 sys_get_deviceID(void);

#ifdef __cplusplus
}
#endif

#endif /*__LPC12XX_SYSCTRL_H__*/

