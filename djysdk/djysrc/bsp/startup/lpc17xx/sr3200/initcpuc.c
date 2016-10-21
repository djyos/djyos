// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：initcpuc.c
// 模块描述: 系统启动模块，主要是内核启动相关的初始化，包括时钟，总线等
// 模块版本: V1.10
// 创建人员: hm
// 创建时间: 11/08.2014
// =============================================================================

#include "stdint.h"
#include "cpu_peri.h"
#include "hard-exp.h"
#include "arch_feature.h"
#include "lpc17xx.h"
#include "core_cmFunc.h"

// 外部变量和函数声明
// =============================================================================
extern   uint32_t   msp_top[ ];
extern void __set_PSP(uint32_t topOfProcStack);
extern void __set_PRIMASK(uint32_t priMask);
extern void __set_FAULTMASK(uint32_t faultMask);
extern void __set_CONTROL(uint32_t control);
extern void Load_Preload(void);
// =============================================================================

// 系统时钟模块初始化宏定义
#define CLOCK_SETUP           1
#define SCS_Val               0x00000020
#define CLKSRCSEL_Val         0x00000001
#define PLL0_SETUP            1
#define PLL0CFG_Val           0x00040077//0x00040063//0x000000013//0x00040063//
#define PLL1_SETUP            1
#define PLL1CFG_Val           0x00000023
#define CCLKCFG_Val           0x00000003
#define USBCLKCFG_Val         0x00000000
#define PCLKSEL0_Val          0x00000000
#define PCLKSEL1_Val          0x00000000
#define PCONP_Val             0x042887DE
#define CLKOUTCFG_Val         0x00000000

#define FLASH_SETUP           1
#define FLASHCFG_Val          0x00004000//0x00005000

// =============================================================================
// 功能：系统时钟初始化，配置OSC和系统主时钟，PLL时钟分频，使提供系统各个模块时钟
//       PLL0输出时钟计算公式：FCCO0 = 2*M*OSC_IN/N = 2*120*10M/5 = 480M
//       CPU的时钟计算公式（PLL0）：CCLK = FCCO0/DIV = 480M/4 = 120M
//       USB的时钟输出：USBCLK = M * F_osc or F_usb = F_cco1 / (2 * P) = 48M
//                     F_cco1 = F_osc * M * 2 * P (156 MHz to 320 MHz)
// 参数：无
// 返回：无
// =============================================================================
void CpuClockInit (void)
{
#if (CLOCK_SETUP)                       // Clock Setup
    LPC_SC->SCS       = SCS_Val;
    if (LPC_SC->SCS & (1 << 5))
    {                                   // If Main Oscillator is enabled
        while ((LPC_SC->SCS & (1<<6)) == 0)
            ;                           // Wait for Oscillator to be ready
    }

    LPC_SC->CCLKCFG   = CCLKCFG_Val;      // Setup Clock Divider
    /* Periphral clock must be selected before PLL0 enabling and connecting
     * - according errata.lpc1768-16.March.2010 -
     */
    LPC_SC->PCLKSEL0  = PCLKSEL0_Val;     // Peripheral Clock Selection */
    LPC_SC->PCLKSEL1  = PCLKSEL1_Val;

    LPC_SC->CLKSRCSEL = CLKSRCSEL_Val;    // Select Clock Source sysclk / PLL0

#if (PLL0_SETUP)
    LPC_SC->PLL0CFG   = PLL0CFG_Val;      // configure PLL0
    LPC_SC->PLL0FEED  = 0xAA;
    LPC_SC->PLL0FEED  = 0x55;

    LPC_SC->PLL0CON   = 0x01;             // PLL0 Enable
    LPC_SC->PLL0FEED  = 0xAA;
    LPC_SC->PLL0FEED  = 0x55;
    while (!(LPC_SC->PLL0STAT & (1<<26)));// Wait for PLOCK0

    LPC_SC->PLL0CON   = 0x03;             // PLL0 Enable & Connect
    LPC_SC->PLL0FEED  = 0xAA;
    LPC_SC->PLL0FEED  = 0x55;
    // Wait for PLLC0_STAT & PLLE0_STAT
    while ((LPC_SC->PLL0STAT & ((1<<25) | (1<<24))) != ((1<<25) | (1<<24)));
#endif

#if (PLL1_SETUP)
    LPC_SC->PLL1CFG   = PLL1CFG_Val;
    LPC_SC->PLL1FEED  = 0xAA;
    LPC_SC->PLL1FEED  = 0x55;

    LPC_SC->PLL1CON   = 0x01;             // PLL1 Enable
    LPC_SC->PLL1FEED  = 0xAA;
    LPC_SC->PLL1FEED  = 0x55;
    while (!(LPC_SC->PLL1STAT & (1<<10)));// Wait for PLOCK1

    LPC_SC->PLL1CON   = 0x03;             // PLL1 Enable & Connect
    LPC_SC->PLL1FEED  = 0xAA;
    LPC_SC->PLL1FEED  = 0x55;
     // Wait for PLLC1_STAT & PLLE1_STAT
    while ((LPC_SC->PLL1STAT & ((1<< 9) | (1<< 8))) != ((1<< 9) | (1<< 8)));
#else
    LPC_SC->USBCLKCFG = USBCLKCFG_Val;    //Setup USB Clock Divider
#endif

    LPC_SC->PCONP     = PCONP_Val;        // Power Control for Peripherals

    LPC_SC->CLKOUTCFG = CLKOUTCFG_Val;    // Clock Output Configuration
#endif

#if (FLASH_SETUP == 1)                      // Flash Accelerator Setup
    LPC_SC->FLASHCFG  = (LPC_SC->FLASHCFG & ~0x0000F000) | FLASHCFG_Val;
#endif
}

// =============================================================================
// 功能：CORTEX M3内核启动函数，系统复位启动后，从汇编向量表跳转到C的第一个函数。函数
//       会初始化主堆栈和用户堆栈，配置内核寄存器，然后初始化时钟模块。
// 参数：无
// 返回：无
// =============================================================================
void Init_Cpu(void)
{
    __set_PSP((uint32_t)msp_top);
    __set_PRIMASK(1);
    __set_FAULTMASK(1);
    __set_CONTROL(0);
    switch(pg_scb_reg->CPUID)
    {
    	break;
    }
    CpuClockInit();

    Load_Preload();
    return;
}


