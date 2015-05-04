/**************************************************************************//**
 * $Id: lpc12xx_rtc.c 379 2010-08-26 08:27:19Z cnh20509 $
 *
 * @file     lpc12xx_rtc.c
 * @brief    Contains all functions support for RTC firmware library on LPC12xx.
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
/** @addtogroup RTC
 * @{
 */

/* Includes ----------------------------------------------------------------- */
#include "lpc12xx_rtc.h"

/* If this source file built with example, the LPC12xx FW library configuration
 * file in each example directory ("lpc12xx_libcfg.h") must be included,
 * otherwise the default FW library configuration file must be included instead
 */
#ifdef __BUILD_WITH_EXAMPLE__
#include "lpc12xx_libcfg.h"
#else
#include "lpc12xx_libcfg_default.h"    
#endif /* __BUILD_WITH_EXAMPLE__ */


#ifdef _RTC

/** @defgroup RTC_Private_Macros
  * @{
  */
  
/* RTC CR mask */
#define CR_RTC_ENABLE   (uint32_t)(0x1)
#define CR_RTC_DISABLE  (uint32_t)(~0x1)

/* RTC INT mask */
#define RTC_INT         (uint32_t)(0x1)
#define RTC_INT_MASK    (uint32_t)(0x1)

/* RTC clock control bit in Reg. SYSAHBCLKCTRL */
#define SYSAHBCLKCTRL_RTC   19

/**
 * @}
 */


/** @defgroup RTC_Private_Functions
 * @{
 */
 
/**
  * @brief  Initializes the RTC peripheral.
  *
  * @param  RTC_CLKSRC: specifies the RTC clock source.
  *         This parameter can be one of the following values:
  *             @arg RTC_CLKSRC_1HZ: 1 Hz clock
  *             @arg RTC_CLKSRC_DELAYED1HZ: delayed 1 Hz clock
  *             @arg RTC_CLKSRC_1KHZ: 1 KHz clock    
  *             @arg RTC_CLKSRC_PCLK: RTC PCLK  
  * @param CLKDiv: RTC clock divider when select RTC_CLKSRC_PCLK as its source
  *        The RTC_PCLK can be shut down by setting the divider to 0x0. 
  * @retval None  
  *
  * The RTC clock source must be selected before the RTC is enabled in the 
  * AHBCLKCTRL register. The clock source must NOT be changed while the RTC 
  * is running.
  */
void RTC_Init (uint8_t RTC_CLKSRC, uint8_t CLKDiv)
{
    CHECK_PARAM (PARAM_RTC_CLKSRC(RTC_CLKSRC));
    
    /* Select clock source for RTC. This has to be done before RTC clock is
    enabled in SYSAHBCLKCTRL. */
    if (RTC_CLKSRC != RTC_CLKSRC_PCLK)  CLKDiv = 0;                        
    LPC_SYSCON->RTCOSCCTRL = RTC_CLKSRC;
    LPC_SYSCON->RTCCLKDIV = CLKDiv;
    
    /* Enable clock to RTC */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<SYSAHBCLKCTRL_RTC);           
}

/**
  * @brief  Deinitializes the RTC peripheral registers to their default
  *         reset values.
  *
  * @param  None
  * @retval None    
  */
void RTC_DeInit ()
{    
    /* Disable clock to RTC */
    LPC_SYSCON->SYSAHBCLKCTRL &= ~(1<<SYSAHBCLKCTRL_RTC);    
}

/**
  * @brief  Get the current counter value of the RTC.
  *
  * @param  None
  * @retval Current RTC counter    
  */
uint32_t RTC_GetData ()
{
    return LPC_RTC->DR;
}

/**
  * @brief  Get the current match value of the RTC.
  *
  * @param  None
  * @retval Current match value    
  */
uint32_t RTC_GetMatch ()
{
    return LPC_RTC->MR;
}

/**
  * @brief  Set the current match value of the RTC.
  *
  * @param  MatchValue: new match value
  * @retval None    
  */
void RTC_SetMatch (uint32_t MatchValue)
{
    LPC_RTC->MR = MatchValue;
}

/**
  * @brief  Get the current load value of the RTC.
  *
  * @param  None
  * @retval Current load value
  */
uint32_t RTC_GetLoad ()
{
    return LPC_RTC->LR;
}

/**
  * @brief  Set the current load value of the RTC.
  *
  * @param  LoadValue: new load value
  * @retval None    
  */
void RTC_SetLoad (uint32_t LoadValue)
{
    LPC_RTC->LR = LoadValue;
}

/**
  * @brief  Enable or disable the RTC peripheral.
  *
  * @param  NewState: new state of the RTC peripheral. 
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None 
  */
void RTC_Cmd (FunctionalState NewState)
{
    /* Check the parameters */
	CHECK_PARAM(PARAM_FUNCTIONALSTATE(NewState));

	if (NewState == ENABLE)
	{
		LPC_RTC->CR |= CR_RTC_ENABLE;
	}
	else
	{
		LPC_RTC->CR &= CR_RTC_DISABLE;
	}
}

/**
  * @brief  Get the status of RTC (started or stopped).
  *
  * @param  None
  * @retval New State of RTC status flag (SET or RESET).
  *         SET: RTC is started; RESET: RTC is stopped.    
  */
FlagStatus RTC_GetStatus ()
{
    return ((LPC_RTC->CR & CR_RTC_ENABLE) ? SET : RESET);
    
}

/**
  * @brief  Enable or disable the RTC interrupt.
  * 
  * @param  NewState: new state of the RTC interrupt.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RTC_IntConfig(FunctionalState NewState)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_FUNCTIONALSTATE(NewState));
      
	if (NewState == ENABLE)
	{
	    /* Enable RTC interrupt */
		LPC_RTC->IMSC |= RTC_INT;
	}
	else
	{
	    /* Disable RTC interrupt */
		LPC_RTC->IMSC &= (~RTC_INT) & RTC_INT_MASK;
	}
}

/**
  * @brief  Check whether the Raw RTC interrupt status flag is set or not.
  *
  * @param  None
  * @retval New State of the interrupt status flag (SET or RESET).    
  */
IntStatus RTC_GetRawIntStatus ()
{    
    /* Check the raw status of the RTC interrupt */
	return ((LPC_RTC->RIS & RTC_INT) ? SET : RESET);
}

/**
  * @brief  Checks whether the RTC interrupt has occurred or not.
  *
  * @param  None
  * @retval New State of the interrupt status flag (SET or RESET).
  */
IntStatus RTC_GetIntStatus ()
{    
    /* Check the status of the RTC interrupt */
	return ((LPC_RTC->MIS & RTC_INT) ? SET : RESET);
}

/**
  * @brief  Clear the interrupt pending in RTC peripheral bit.
  *
  * @param  None
  * @retval None
  */
void RTC_ClearIntPendingBit(void)
{   
    /* Clear specified interrupt pending */  
	LPC_RTC->ICR = RTC_INT;
}

/**
 * @}
 */
 
 
#endif

/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */
