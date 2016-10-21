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
// -----------------------------------------------------------------------------
// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：sysinit.c
// 模块描述: CPU时钟的初始化和片内片外RAM等的初始化
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 08/13.2015
// =============================================================================
#include "stdint.h"
#include "cpu_peri.h"
#include "arch_feature.h"
#include "mpu.h"

// =============================================================================
// ATMEL 时钟配置，OSC = 10MHz,PLLA = 10*(0x1D+1)=300M, HCLK300M，外设时钟150M Hz
/* Clock Settings (300MHz HCLK, 150MHz MCK)=> PRESC = 1, MDIV = 2 */
#define SYS_BOARD_OSCOUNT   (CKGR_MOR_MOSCXTST(0x8U))

#if (CN_CFG_MCLK == 300*Mhz)
#define SYS_BOARD_PLLAR     (CKGR_PLLAR_ONE | CKGR_PLLAR_MULA(0x1DU) | \
                CKGR_PLLAR_PLLACOUNT(0x3fU) | CKGR_PLLAR_DIVA(0x1U))
#elif (CN_CFG_MCLK == 200*Mhz)
#define SYS_BOARD_PLLAR     (CKGR_PLLAR_ONE | CKGR_PLLAR_MULA(19U) | \
                CKGR_PLLAR_PLLACOUNT(0x3fU) | CKGR_PLLAR_DIVA(0x1U))
#endif

#define SYS_BOARD_MCKR      (PMC_MCKR_PRES_CLK_1 | PMC_MCKR_CSS_PLLA_CLK \
                | PMC_MCKR_MDIV_PCK_DIV2)


void TCM_Disable(void)
{

    __DSB();
    __ISB();
    SCB->ITCMCR &= ~(uint32_t)SCB_ITCMCR_EN_Msk;
    SCB->DTCMCR &= ~(uint32_t)SCB_ITCMCR_EN_Msk;
    __DSB();
    __ISB();
}

// =============================================================================
// 功能：系统时钟初始化，主时钟配置为300MHz，外设时钟为150M
// 参数：无
// 返回：无
// =============================================================================
void SysClk_Init(void)
{
	uint32_t read_MOR;

	EFC->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_CGPB | EEFC_FCR_FARG(8));
	EFC->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_CGPB | EEFC_FCR_FARG(7));

	TCM_Disable();

	/* Set FWS according to SYS_BOARD_MCKR configuration */
	EFC->EEFC_FMR = EEFC_FMR_FWS(5);

	 /* Before switching MAIN OSC on external crystal : enable it and don't
	 * disable at the same time RC OSC in case of if MAIN OSC is still using RC
	 * OSC
	 */

	read_MOR = PMC->CKGR_MOR;
	 /* enable external crystal - enable RC OSC */
	read_MOR |= (CKGR_MOR_KEY_PASSWD |CKGR_MOR_XT32KFME);
	PMC->CKGR_MOR = read_MOR;

	/* Select XTAL 32k instead of internal slow RC 32k for slow clock */
//	if ( (SUPC->SUPC_SR & SUPC_SR_OSCSEL) != SUPC_SR_OSCSEL_CRYST )
//	{
//		SUPC->SUPC_CR = SUPC_CR_KEY_PASSWD | SUPC_CR_XTALSEL_CRYSTAL_SEL;
//
//		while( !(SUPC->SUPC_SR & SUPC_SR_OSCSEL) );
//	}

	/* Initialize main oscillator */
	if ( !(PMC->CKGR_MOR & CKGR_MOR_MOSCSEL) )
	{
	  PMC->CKGR_MOR = CKGR_MOR_KEY_PASSWD | SYS_BOARD_OSCOUNT
					| CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCXTEN;

	  while ( !(PMC->PMC_SR & PMC_SR_MOSCXTS) )
	  {
	  }
	}

	/* Switch to 3-20MHz Xtal oscillator */
	PMC->CKGR_MOR = CKGR_MOR_KEY_PASSWD | SYS_BOARD_OSCOUNT
					| CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCXTEN | CKGR_MOR_MOSCSEL;

	while ( !(PMC->PMC_SR & PMC_SR_MOSCSELS) )
	{
	}

	PMC->PMC_MCKR = (PMC->PMC_MCKR & ~(uint32_t)PMC_MCKR_CSS_Msk)
					| PMC_MCKR_CSS_MAIN_CLK;

	while ( !(PMC->PMC_SR & PMC_SR_MCKRDY) )
	{
	}

	/* Initialize PLLA */
	PMC->CKGR_PLLAR = SYS_BOARD_PLLAR;
	while ( !(PMC->PMC_SR & PMC_SR_LOCKA) )
	{
	}

	/* Switch to main clock */
	PMC->PMC_MCKR = (SYS_BOARD_MCKR & ~PMC_MCKR_CSS_Msk) | PMC_MCKR_CSS_MAIN_CLK;
	while ( !(PMC->PMC_SR & PMC_SR_MCKRDY) )
	{
	}

	/* Switch to PLLA */
	PMC->PMC_MCKR = SYS_BOARD_MCKR;
	while ( !(PMC->PMC_SR & PMC_SR_MCKRDY) )
	{
	}

//	SystemCoreClock = CHIP_FREQ_CPU_MAX;
}








