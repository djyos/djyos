/**************************************************************************//**
 * $Id: lpc12xx_pmu.c 588 2010-09-28 01:47:33Z cnh20446 $
 *
 * @file     lpc12xx_pmu.c
 * @brief    Contains all functions support for PMU firmware library on LPC12xx.
 * @version  1.0
 * @date     26. Sep. 2010
 * @author   NXP MCU Team
 *
 * @note
 * Copyright (C) 2010 NXP Semiconductors(NXP). All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 ******************************************************************************/
 
/* Peripheral group --------------------------------------------------------- */
/** @addtogroup PMU
 * @{
 */

/* Includes ----------------------------------------------------------------- */
#include "lpc12xx_pmu.h"


/* If this source file built with example, the LPC12xx FW library configuration
 * file in each example directory ("lpc12xx_libcfg.h") must be included,
 * otherwise the default FW library configuration file must be included instead
 */
#ifdef __BUILD_WITH_EXAMPLE__
#include "lpc12xx_libcfg.h"
#else
#include "lpc12xx_libcfg_default.h"
#endif /* __BUILD_WITH_EXAMPLE__ */

#ifdef _PMU

/** @defgroup PMU_Private_Functions
 * @{
 */

/**
  * @brief  Initialize PMU and setup wakeup source.
  * @param  PMU_WAKEUPPIN: Pin used to wake up
  * 		This parameter could be one or combination of the following values:
  * 			@arg PMU_WAKEUPPIN_PIO0_0: PIO0_0 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_1: PIO0_1 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_2: PIO0_2 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_3: PIO0_3 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_4: PIO0_4 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_5: PIO0_5 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_6: PIO0_6 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_7: PIO0_7 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_8: PIO0_8 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_9: PIO0_9 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_10: PIO0_10 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_11: PIO0_11 as wake up pin.
  * @param  PMU_WAKEUPEDGE: specifies the wakeup edge
  * 		This parameter could be one or combination of the following values:
  * 			@arg PMU_WAKEUPEDGE_FALLINGEDGE: Falling edge wake up.
  * 			@arg PMU_WAKEUPEDGE_RISINGEDGE: Rising edge wake up.
  * @retval None   
  */
void PMU_Init( uint32_t PMU_WAKEUPPIN, uint8_t PMU_WAKEUPEDGE )
{
	uint32_t i;
	
	CHECK_PARAM(PARAM_PMU_WAKEUPPIN(PMU_WAKEUPPIN));
	CHECK_PARAM(PARAM_PMU_WAKEUPEDGE(PMU_WAKEUPEDGE));
	
	for ( i = 0; i < PMU_STARTLOGIC_NUMBER; i++ )
	{
		NVIC_EnableIRQ((IRQn_Type)(WAKEUP0_IRQn+i));
	}
	
	for (i = 0; i <PMU_STARTLOGIC_NUMBER; i++ )
	{
		if (PMU_WAKEUPPIN & i)
		{
			/* use port0_x as wakeup source, i/o pin */
			LPC_IOCON->PIO0_0 &= ~0x07;	
			LPC_GPIO0->DIR &= ~i;	/* Input P0.x */
			/* Only edge trigger. activation polarity on P0.x is FALLING EDGE/RISINGEDGE. */
			if (PMU_WAKEUPEDGE == PMU_WAKEUPEDGE_FALLINGEDGE)	LPC_SYSCON->STARTAPRP0 &= ~i;
			if (PMU_WAKEUPEDGE == PMU_WAKEUPEDGE_RISINGEDGE)	LPC_SYSCON->STARTAPRP0 |= i;
			/* Clear all wakeup source */ 
			LPC_SYSCON->STARTRSRP0CLR = i;
			/* Enable Port 0.x as wakeup source. */
			LPC_SYSCON->STARTERP0 = i;
		}
	}
  return;
}

/**
  * @brief  Enter Sleep mode with co-operated instruction by the Cortex-M0.
  *
  * @param  PMU_SLEEPMODE: specifies the sleep mode to enter
  * 			@arg PMU_SLEEPMODE_SLEEP: Enter sleep mode.
  * 			@arg PMU_SLEEPMODE_DEEP_SLEEP: Enter deep sleep mode.
  *			PMU_PDSLEEPCFG: specifies the analog blocks to be powerd down 
  * 		This parameter can be one or combination of the following values:
  *				@arg PMU_PDSLEEPCFG_IRCOUT: Power down IRCOUT block.  
  *				@arg PMU_PDSLEEPCFG_IRC: Power down IRC block.
  *				@arg PMU_PDSLEEPCFG_FLASH: Power down FLASH block.
  *				@arg PMU_PDSLEEPCFG_BOD: Power down BOD block.
  *				@arg PMU_PDSLEEPCFG_ADC: Power down ADC block.
  *				@arg PMU_PDSLEEPCFG_SYSOSC: Power down SYSOSC block.
  *				@arg PMU_PDSLEEPCFG_WDTOSC: Power down WDTOSC block.
  *				@arg PMU_PDSLEEPCFG_SYSPLL: Power down SYSPLL block.
  *				@arg PMU_PDSLEEPCFG_USBPLL: Power down USBPLL block.
  *				@arg PMU_PDSLEEPCFG_MAINREGUL: Power down MAINREGUL block.
  *				@arg PMU_PDSLEEPCFG_USBPHY: Power down USBPHY block.
  *				@arg PMU_PDSLEEPCFG_LPREGUL: Power down LPREGUL block.
  *				@arg PMU_PDSLEEPCFG_COMP: Power down COMPARATOR block.
  * @retval None   
  */
void PMU_Sleep( uint32_t PMU_SLEEPMODE, uint32_t PMU_PDSLEEPCFG )
{
	CHECK_PARAM(PARAM_PMU_SLEEPMODE(PMU_SLEEPMODE));	
	CHECK_PARAM(PARAM_PMU_PDSLEEPCFG(PMU_PDSLEEPCFG));
	
	LPC_SYSCON->PDAWAKECFG = LPC_SYSCON->PDRUNCFG;
	LPC_SYSCON->PDSLEEPCFG = PMU_PDSLEEPCFG;
	
	/* If normal sleep, bit 2(SLEEPDEEP) needs to be cleared, otherwise, 
	set in DEEPSLEEP mode. */
	if ( PMU_SLEEPMODE )
	{
		SCB->SCR |= NVIC_LP_SLEEPDEEP;
	}
	else
	{
		SCB->SCR &= ~NVIC_LP_SLEEPDEEP;
	}
	__WFI();
	return;
}

/**
  * @brief  Enter DeepPowerDown mode with co-operated instruction by the Cortex-M0.
  *
  * @param  None 
  * @retval None   
  */
void PMU_DeepPowerDown( void )
{
	SCB->SCR |= NVIC_LP_SLEEPDEEP;
	LPC_PMU->PCON = 0x2;
	__WFI();
	return;
}

/**
  * @brief  Reset start logic signal.
  *
  * @param  PMU_WAKEUPPIN: Start signal input pin.
  * 		This parameter could be one or combination of the following values:
  * 			@arg PMU_WAKEUPPIN_PIO0_0: PIO0_0 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_1: PIO0_1 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_2: PIO0_2 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_3: PIO0_3 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_4: PIO0_4 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_5: PIO0_5 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_6: PIO0_6 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_7: PIO0_7 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_8: PIO0_8 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_9: PIO0_9 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_10: PIO0_10 as wake up pin.
  * 			@arg PMU_WAKEUPPIN_PIO0_11: PIO0_11 as wake up pin.
  * @retval None.
  */
void PMU_ResetStartLogic(uint32_t PMU_WAKEUPPIN)
{
	CHECK_PARAM(PARAM_PMU_WAKEUPPIN(PMU_WAKEUPPIN));

	LPC_SYSCON->STARTRSRP0CLR |= PMU_WAKEUPPIN; 
}

/**
  * @brief  Read start logic0 signal.
  *
  * @param  None  
  * @retval The start logic signal.
  */
uint32_t PMU_ReadStartLogic()
{
	return(LPC_SYSCON->STARTSRP0); 
}

/**
  * @brief  Read PCON register.
  *
  * @param  PMU_PCON: specifies the bits will read from PCON register
  * 		This parameter can be one or combination of the following values:
  * 			@arg PMU_PCON_DPDEN: Deep power down mode enable.
  * 			@arg PMU_PCON_SLEEPFLAG: Sleep mode flag.
  * 			@arg PMU_PCON_DPDFLAG: Deep power down mode flag.
  * @retval The read bits value   
  */
uint32_t PMU_ReadPCON( uint32_t PMU_PCON )
{
	CHECK_PARAM(PARAM_PMU_PCON(PMU_PCON));

	return(LPC_PMU->PCON & PMU_PCON);
}

/**
  * @brief  Clear DPDFLAG in PCON register.
  *
  * @param  None 
  * @retval None   
  */
void PMU_ClearDPDFLAG( void )
{
	LPC_PMU->PCON = PMU_PCON_DPDFLAG;
}

/**
  * @brief  Clear SLEEPFLAG in PCON register.
  *
  * @param  None 
  * @retval None   
  */
void PMU_ClearSLEEPFLAG( void )
{
	LPC_PMU->PCON = PMU_PCON_SLEEPFLAG;
}

/**
  * @brief  Set GPREG registers to restore data when in deep power down mode.
  *
  * @param  GPREG: specifies the GPREG to be set
  * 			@arg PMU_GPREG_0: General purpose register 0.
  * 			@arg PMU_GPREG_1: General purpose register 1.
  * 			@arg PMU_GPREG_2: General purpose register 2.
  * 			@arg PMU_GPREG_3: General purpose register 3.
  * 			@arg PMU_GPREG_4: General purpose register 4.
  * @param  Data: specifies the value will set to GPREGx. 
  * @retval None   
  */
void PMU_SetGPREG(uint32_t GPREG, uint32_t Data)
{
	CHECK_PARAM(PARAM_PMU_GPREG(GPREG));

	if (GPREG == PMU_GPREG_0)
	{
		LPC_PMU->GPREG0 = Data;
	}
	if (GPREG == PMU_GPREG_1)
	{
		LPC_PMU->GPREG1 = Data;
	}
	if (GPREG == PMU_GPREG_2)
	{
		LPC_PMU->GPREG2 = Data;
	}
	if (GPREG == PMU_GPREG_3)
	{
		LPC_PMU->GPREG3 = Data;
	}
}

/**
 * @}
 */

#endif // _PMU

/**
 * @}
 */
/* --------------------------------- End Of File ------------------------------ */
