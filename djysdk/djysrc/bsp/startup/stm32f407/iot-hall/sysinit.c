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
// 创建时间: 07/07.2015
// =============================================================================

#include "stdint.h"
#include "cpu_peri.h"
#include "board-config.h"

#define STM32F40_41xxx

// ===================== PLL配置所需的宏定义 =====================================
// PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
// USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ
// //SYSCLK = PLL_VCO / PLL_P
// =============================================================================
#define PLL_M      8
#define PLL_Q      7

#if defined (STM32F40_41xxx)
#define PLL_N      336
#define PLL_P      2
#endif /* STM32F40_41xxx */

#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
#define PLL_N      360
#define PLL_P      2
#endif /* STM32F427_437x || STM32F429_439xx */

#if defined (STM32F401xx)
#define PLL_N      336
#define PLL_P      4
#endif /* STM32F401xx */

// =============================================================================
// 功能：该函数实现系统时钟的初始化，主要包括：1、系统时钟从内部时钟切换到外部时钟；2、配置
//      HCLK、PCLK1、PCLK2、MCLK分频系数；3、使能数据和指令cache；4、选用MCLK为系统时钟
// 参数：无
// 返回：无
// =============================================================================
void SysClockInit(void)
{
	//PLL (clocked by HSE) used as System clock source
	u32 StartUpCounter = 0, HSEStatus = 0;

	/* Enable HSE */
	RCC->CR |= ((uint32_t)RCC_CR_HSEON);

	/* Wait till HSE is ready and if Time out is reached exit */
	do
	{
		HSEStatus = RCC->CR & RCC_CR_HSERDY;
		StartUpCounter++;
	} while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

	if ((RCC->CR & RCC_CR_HSERDY) != RESET)
	{
		HSEStatus = (uint32_t)0x01;
	}
	else
	{
		HSEStatus = (uint32_t)0x00;
	}

	if (HSEStatus == (uint32_t)0x01)
	{
		/* Select regulator voltage output Scale 1 mode */
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
		PWR->CR |= PWR_CR_VOS;

		/* HCLK = SYSCLK / 1*/
		RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

	#if defined (STM32F40_41xxx) || defined (STM32F427_437xx) || defined (STM32F429_439xx)
	/* PCLK2 = HCLK / 2*/
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

	/* PCLK1 = HCLK / 4*/
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
	#endif /* STM32F40_41xxx || STM32F427_437x || STM32F429_439xx */

	#if defined (STM32F401xx)
	/* PCLK2 = HCLK / 2*/
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

	/* PCLK1 = HCLK / 4*/
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
	#endif /* STM32F401xx */

	/* Configure the main PLL */
	RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
				   (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);

	/* Enable the main PLL */
	RCC->CR |= RCC_CR_PLLON;

	/* Wait till the main PLL is ready */
	while((RCC->CR & RCC_CR_PLLRDY) == 0)
	{
	}

	#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
	/* Enable the Over-drive to extend the clock frequency to 180 Mhz */
	PWR->CR |= PWR_CR_ODEN;
	while((PWR->CSR & PWR_CSR_ODRDY) == 0)
	{
	}
	PWR->CR |= PWR_CR_ODSWEN;
	while((PWR->CSR & PWR_CSR_ODSWRDY) == 0)
	{
	}
	/* Configure Flash prefetch, Instruction cache, Data cache and wait state */
	FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;
	#endif /* STM32F427_437x || STM32F429_439xx  */

	#if defined (STM32F40_41xxx)
	/* Configure Flash prefetch, Instruction cache, Data cache and wait state */
	FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;
	#endif /* STM32F40_41xxx  */

	#if defined (STM32F401xx)
	/* Configure Flash prefetch, Instruction cache, Data cache and wait state */
	FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_2WS;
	#endif /* STM32F401xx */

	/* Select the main PLL as system clock source */
	RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	/* Wait till the main PLL is used as system clock source */
	while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
	{
	}
	}
	else
	{ /* If HSE fails to start-up, the application will have wrong clock
		 configuration. User can add here some code to deal with this error */
	}
}

// =============================================================================
// 功能：片外SRAM初始化，包括GPIO和FMC控制器的初始化
// 参数：无
// 返回：无
// =============================================================================
void SRAM_Init(void)
{
	/* Enable GPIOD, GPIOE, GPIOF and GPIOG interface clock */
	RCC->AHB1ENR   |= 0x00000078;

	/* Connect PDx pins to FMC Alternate function: PD0,1,4,5,8~15*/
	GPIOD->AFR[0]  = 0x00cc00cc;
	GPIOD->AFR[1]  = 0xcccccccc;
	/* Configure PDx pins in Alternate function mode */
	GPIOD->MODER   = 0xaaaa0a0a;
	/* Configure PDx pins speed to 100 MHz */
	GPIOD->OSPEEDR = 0xffff0f0f;
	/* Configure PDx pins Output type to push-pull */
	GPIOD->OTYPER  = 0x00000000;
	/* No pull-up, pull-down for PDx pins */
	GPIOD->PUPDR   = 0x55550505;//0x00000000;

	/* Connect PEx pins to FMC Alternate function :PE0,1,7~15*/
	GPIOE->AFR[0]  = 0xc00000cc;
	GPIOE->AFR[1]  = 0xcccccccc;
	/* Configure PEx pins in Alternate function mode */
	GPIOE->MODER   = 0xaaaa800a;
	/* Configure PEx pins speed to 100 MHz */
	GPIOE->OSPEEDR = 0xffffC00f;
	/* Configure PEx pins Output type to push-pull */
	GPIOE->OTYPER  = 0x00000000;
	/* No pull-up, pull-down for PEx pins */
	GPIOE->PUPDR   = 0x55554005;

	/* Connect PFx pins to FMC Alternate function:PF0~5,12~15 */
	GPIOF->AFR[0]  = 0x00cccccc;
	GPIOF->AFR[1]  = 0xcccc0000;
	/* Configure PFx pins in Alternate function mode */
	GPIOF->MODER   = 0xaa000aaa;
	/* Configure PFx pins speed to 100 MHz */
	GPIOF->OSPEEDR = 0xff000fff;
	/* Configure PFx pins Output type to push-pull */
	GPIOF->OTYPER  = 0x00000000;
	/* No pull-up, pull-down for PFx pins */
	GPIOF->PUPDR   = 0x55000555;

	/* Connect PGx pins to FMC Alternate function :PG0~5,10*/
	GPIOG->AFR[0]  = 0x00cccccc;
	GPIOG->AFR[1]  = 0x00000c00;
	/* Configure PGx pins in Alternate function mode */
	GPIOG->MODER   = 0x00200aaa;
	/* Configure PGx pins speed to 100 MHz */
	GPIOG->OSPEEDR = 0x00300fff;
	/* Configure PGx pins Output type to push-pull */
	GPIOG->OTYPER  = 0x00000000;
	/* No pull-up, pull-down for PGx pins */
	GPIOG->PUPDR   = 0x00100555;

	/*-- FMC Configuration ------------------------------------------------------*/
	/* Enable the FMC/FSMC interface clock */
	RCC->AHB3ENR         |= 0x00000001;

	#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
	  /* Configure and enable Bank1_SRAM2 */
	  FMC_Bank1->BTCR[2]  = 0x00001011;
	  FMC_Bank1->BTCR[3]  = 0x00000201;
	  FMC_Bank1E->BWTR[2] = 0x0fffffff;
	#endif /* STM32F427_437xx || STM32F429_439xx */

	#if defined (STM32F40_41xxx)
	  /* Configure and enable Bank1_SRAM3，NE3 */
	//bank1有NE1~4,每一个有一个BCR+TCR，所以总共八个寄存器。
	//这里我们使用NE3 ，也就对应BTCR[4],[5]。
	FSMC_Bank1->BTCR[4]=0X00000000;
	FSMC_Bank1->BTCR[5]=0X00000000;
	FSMC_Bank1E->BWTR[4]=0X00000000;
	//操作BCR寄存器	使用异步模式,模式A(读写共用一个时序寄存器)
	//BTCR[偶数]:BCR寄存器;BTCR[奇数]:BTR寄存器
	FSMC_Bank1->BTCR[4]|=1<<12;//存储器写使能
	FSMC_Bank1->BTCR[4]|=1<<4; //存储器数据宽度为16bit
	//操作BTR寄存器			（HCLK=168M, 1个HCLK=6ns
	FSMC_Bank1->BTCR[5]|=8<<8; //数据保持时间（DATAST）为9个HCLK 6*9=54ns
	FSMC_Bank1->BTCR[5]|=0<<4; //地址保持时间（ADDHLD）未用到
	FSMC_Bank1->BTCR[5]|=6<<0; //地址建立时间（ADDSET）为0个HCLK 0ns
	//闪存写时序寄存器
	FSMC_Bank1E->BWTR[4]=0x0FFFFFFF;//默认值
	//使能BANK1区域3
	FSMC_Bank1->BTCR[4]|=1<<0;
	#endif  /* STM32F40_41xxx */
}



