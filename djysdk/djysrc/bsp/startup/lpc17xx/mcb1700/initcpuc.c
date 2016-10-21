// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：initcpuc.c
// 模块描述: 系统启动模块，主要是内核启动相关的初始化，包括时钟，总线等
// 模块版本: V1.10
// 创建人员: hm
// 创建时间: 11/08.2014
// =============================================================================
#include "arch_feature.h"
#include "stdint.h"
#include "cpu_peri.h"
#include "hard-exp.h"
#include "arch_feature.h"
#include "lpc17xx.h"
#include "core_cmFunc.h"

#include "core_cm3.h"

// 外部变量和函数声明
// =============================================================================
extern   uint32_t   msp_top[ ];
extern void __set_PSP(uint32_t topOfProcStack);
extern void __set_PRIMASK(uint32_t priMask);
extern void __set_FAULTMASK(uint32_t faultMask);
extern void __set_CONTROL(uint32_t control);

struct ScbReg volatile * const startup_scb_reg
                        = (struct ScbReg *)0xe000ed00;
void Startup_NMI(void)
{
    while(1);
}
void Startup_Hardfault(void)
{
    while(1);
}
void Init_Cpu(void);
//为什么是256-3？ 编译生成rodata时，会在gc_u32StartupExpTable后面增加12字节的rodata
//不知道为什么？如果
const u32 gc_u32StartupExpTable[4] =
{
    (u32)msp_top,
    (u32)Init_Cpu,
    (u32) Startup_NMI,
    (u32) Startup_Hardfault
};
// =============================================================================

// 系统时钟模块初始化宏定义
#define CLOCK_SETUP           1
#define SCS_Val               0x00000020
#define CLKSRCSEL_Val         0x00000001
#define PLL0_SETUP            1
#define PLL0CFG_Val           0x00050063
#define PLL1_SETUP            1
#define PLL1CFG_Val           0x00000023
#define CCLKCFG_Val           0x00000003
#define USBCLKCFG_Val         0x00000000
#define PCLKSEL0_Val          0x00000000
#define PCLKSEL1_Val          0x00000000
#define PCONP_Val             0x042887DE
#define CLKOUTCFG_Val         0x00000000

#define FLASH_SETUP           1
#define FLASHCFG_Val          0x00005000

// =============================================================================
// 功能：系统时钟初始化，配置OSC和系统主时钟，PLL时钟分频，使提供系统各个模块时钟
//       PLL0输出时钟计算公式：FCCO0 = 2*M*OSC_IN/N = 2*100*12M/6 = 400M
//       CPU的时钟计算公式（PLL0）：CCLK = FCCO0/DIV = 400M/4 = 100M
//       USB的时钟输出：USBCLK = M * F_osc or F_usb = F_cco1 / (2 * P) = 48M
//                     F_cco1 = F_osc * M * 2 * P (156 MHz to 320 MHz)
// 参数：无
// 返回：无
// =============================================================================
void SystemInit (void)
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
    switch(startup_scb_reg->CPUID)
    {
        case CN_M3_REVISION_R0P0:
            break;                              //市场没有版本0的芯片
        case CN_M3_REVISION_R1P0:
        	startup_scb_reg->CCR |= 1<<bo_scb_ccr_stkalign;
            break;
        case CN_M3_REVISION_R1P1:
        	startup_scb_reg->CCR |= 1<<bo_scb_ccr_stkalign;
            break;
        case CN_M3_REVISION_R2P0:break;            //好像没什么要做的
    }
    SystemInit();

    IAP_SelectLoadProgam();
}



extern char g_cIbootFlag[];
const char bootflag[]="RunIboot";//要弄成const，若是局部变量，编译器将其放在ROM
void IAP_GpioPinInit(void)
{
}

bool_t IAP_IsForceIboot(void)
{
	return false;
}
bool_t IAP_IsRamIbootFlag(void)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		if(g_cIbootFlag[i]!=bootflag[i])
		{
			return false;
		}
	}
	return true;
}


