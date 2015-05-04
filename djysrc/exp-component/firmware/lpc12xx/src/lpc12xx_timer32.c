/**************************************************************************//**
 * $Id: lpc12xx_timer32.c 414 2010-09-02 02:38:51Z cnh00813 $
 *
 * @file     lpc12xx_timer32.c
 * @brief    Contains all functions support for TIMER32 firmware library on LPC12xx.
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
/** @addtogroup TIMER32
 * @{
 */

/* Includes ----------------------------------------------------------------- */
#include "lpc12xx_timer32.h"
#include "os_inc.h"

/* If this source file built with example, the LPC12xx FW library configuration
 * file in each example directory ("lpc12xx_libcfg.h") must be included,
 * otherwise the default FW library configuration file must be included instead
 */
#ifdef __BUILD_WITH_EXAMPLE__
#include "lpc12xx_libcfg.h"
#else
#include "lpc12xx_libcfg_default.h"
#endif /* __BUILD_WITH_EXAMPLE__ */

#ifdef _TIMER32


/** @defgroup TIM32_Private_Macros
  * @{
  */

/*Macro define for Timer32 control register  */ 
#define TIM32_ENABLE	((uint32_t)(1<<0))  /* Timer/counter enable bit */
#define TIM32_RESET	((uint32_t)(1<<1))  /* Timer/counter reset bit */
#define TIM32_TCR_MASKBIT    ((uint32_t)(3))    /* Timer control bit mask */
                           
/*Macro define for Timer32 match control register  */
/* Bit location for interrupt on MRx match, n:0 ~ 3   */
#define TIM32_INT_ON_MATCH(n)    (_BIT((n * 3)))   
/* Bit location for reset on MRx match, n:0 ~ 3  */                                                  
#define TIM32_RESET_ON_MATCH(n)  (_BIT(((n * 3) + 1))) 
/* Bit location for stop on MRx match, n:0 ~ 3  */ 
#define TIM32_STOP_ON_MATCH(n)   (_BIT(((n * 3) + 2))) 
/* Timer Match control bit mask */ 
#define TIM32_MCR_MASKBIT        ((uint32_t)(0x0FFF))   
/* Timer Match control bit mask for specific channel*/
#define	TIM32_MCR_CHANNEL_MASKBIT(n)	((uint32_t)(7<<(n*3))) 

/*Macro define for Timer32 capture control register   */
/* Bit location for CAP.n on CRx rising edge, n = 0 to 3 */
#define TIM32_CAP_RISING(n)   	(_BIT((n * 3)))
/* Bit location for CAP.n on CRx falling edge, n = 0 to 3 */  
#define TIM32_CAP_FALLING(n)   	(_BIT(((n * 3) + 1)))  
/* Bit location for CAP.n on CRx interrupt enable, n = 0 to 3 */
#define TIM32_INT_ON_CAP(n)    	(_BIT(((n * 3) + 2)))
/* Mask bit for rising and falling edge bit */  
#define TIM32_EDGE_MASK(n)	(_SBF((n * 3), 0x03))  
/* Timer capture control bit mask */
#define TIM32_CCR_MASKBIT	((uint32_t)(0x0FFF))  
/* Timer Capture control bit mask for specific channel*/
#define	TIM32_CCR_CHANNEL_MASKBIT(n)	((uint32_t)(7<<(n*3)))   

/*Macro define for Timer32 external match control register */ 
/* Bit location for output state change of MAT.n when match happens, n:0 ~ 3  */
#define TIM32_EM(n)    		_BIT(n)
/* Output state change of MAT.n when external match happens: no change */
#define TIM32_EM_NOTHING    	((uint8_t)(0x0))
/* Output state change of MAT.n when external match happens: low */
#define TIM32_EM_LOW         	((uint8_t)(0x1))
/* Output state change of MAT.n when external match happens: high */
#define TIM32_EM_HIGH        	((uint8_t)(0x2))
/* Output state change of MAT.n when external match happens: toggle */
#define TIM32_EM_TOGGLE      	((uint8_t)(0x3))
/* Macro for setting for the MAT.n change state bits */
#define TIM32_EM_SET(n,s) 	(_SBF(((n << 1) + 4), (s & 0x03)))
/* Mask for the MATn change state bits */
#define TIM32_EM_MASK(n) 	(_SBF(((n << 1) + 4), 0x03))
/* Timer external match bit mask */
#define TIM32_EMR_MASKBIT	0x0FFF
  
/*Macro define for Timer32 Count control register  */
#define TIM32_CTCR_MODE_MASK  0x3  /* Mask to get the Counter/timer mode bits */
#define TIM32_CTCR_INPUT_MASK 0xC  /* Mask to get the count input select bits */
#define TIM32_CTCR_MASKBIT    0xF       /* Timer Count control bit mask */
#define TIM32_COUNTER_MODE ((uint8_t)(1))
  
/*Macro define for PWM control register, n = 0 to 3 */ 
#define TIM32_PWM_SET(n)    _BIT(n)   /* Bit location for PWM output of MAT.n */
#define TIM32_PWM_MASK(n)   _BIT(n)  /*Mask for PWMn change state bits */
#define TIM32_PWM_MASKBIT   0xF   /*PWM control register bit mask */

  
/**
 * @}
 */


/** @defgroup TIM32_Private_Functions
 * @{
 */ 

/**
  * @brief  Convert a time to a timer count value.
  *
  * @param  usec: usec Time in microseconds. 
  * @retval The number of required clock ticks to give the time delay.   
  */ 
uint32_t TIM32_ConverUSecToVal (uint32_t USec)
{
    uint64_t clkdlycnt;
    
    /* Get Pclock of timer */
    clkdlycnt = (uint64_t) cn_mclk;
    
    clkdlycnt = (clkdlycnt * USec) / 1000000;
    return (uint32_t) clkdlycnt;
}

/**
  * @brief  Get timer32 Interrupt Status.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR32B0: timer/counter CT32B0
  *             @arg LPC_TMR32B1: timer/counter CT32B1
  * @param  IntFlag: Timer interrupt flag.
  *         This parameter can be one of the following values:
  *             @arg TIM32_INT_MR0: interrupt for Match channel 0
  *             @arg TIM32_INT_MR1: interrupt for Match channel 1
  *             @arg TIM32_INT_MR2: interrupt for Match channel 2
  *             @arg TIM32_INT_MR3: interrupt for Match channel 3
  *             @arg TIM32_INT_CR0: interrupt for Capture channel 0
  *             @arg TIM32_INT_CR1: interrupt for Capture channel 1
  *             @arg TIM32_INT_CR2: interrupt for Capture channel 2
  *             @arg TIM32_INT_CR3: interrupt for Capture channel 3
  * @retval Interrupt flag status (SET or RESET).
  *             @arg SET: interrupt
  *             @arg RESET: no interrupt 
  */
IntStatus TIM32_GetIntStatus(LPC_TMR_TypeDef *TIMx, uint8_t IntFlag)
{
    uint8_t temp;
	 
    //CHECK_PARAM(PARAM_TIM32x(TIMx));
    //CHECK_PARAM(PARAM_TIM32_INT_TYPE(IntFlag));
	 
    temp = (TIMx->IR)& (IntFlag);
    if (temp)
    	return SET;
    
    return RESET;
    
}

/**
  * @brief  Get timer32 Interrupt Status register value.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR32B0: timer/counter CT32B0
  *             @arg LPC_TMR32B1: timer/counter CT32B1
  * @retvalInterrupt Status register (MIS) value.
  *         This parameter can be any combination of the following values.
  *             @arg TIM32_INT_MR0: interrupt for Match channel 0
  *             @arg TIM32_INT_MR1: interrupt for Match channel 1
  *             @arg TIM32_INT_MR2: interrupt for Match channel 2
  *             @arg TIM32_INT_MR3: interrupt for Match channel 3
  *             @arg TIM32_INT_CR0: interrupt for Capture channel 0
  *             @arg TIM32_INT_CR1: interrupt for Capture channel 1
  *             @arg TIM32_INT_CR2: interrupt for Capture channel 2
  *             @arg TIM32_INT_CR3: interrupt for Capture channel 3
  */

uint32_t TIM32_GetIntStatusReg(LPC_TMR_TypeDef *TIMx)
{
    return (TIMx->IR);
}


/**
  * @brief  Clear Interrupt pending bit.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR32B0: timer/counter CT32B0
  *             @arg LPC_TMR32B1: timer/counter CT32B1
  * @param  IntFlag: Timer interrupt flag.
  *         This parameter can be one of the following values:
  *             @arg TIM32_INT_MR0: interrupt for Match channel 0
  *             @arg TIM32_INT_MR1: interrupt for Match channel 1
  *             @arg TIM32_INT_MR2: interrupt for Match channel 2
  *             @arg TIM32_INT_MR3: interrupt for Match channel 3
  *             @arg TIM32_INT_CR0: interrupt for Capture channel 0
  *             @arg TIM32_INT_CR1: interrupt for Capture channel 1
  *             @arg TIM32_INT_CR2: interrupt for Capture channel 2
  *             @arg TIM32_INT_CR3: interrupt for Capture channel 3
  * @retval None
  */
void TIM32_ClearIntPendingBit(LPC_TMR_TypeDef *TIMx, uint8_t IntFlag)
{
    //CHECK_PARAM(PARAM_TIM32x(TIMx));
    //CHECK_PARAM(PARAM_TIM32_INT_TYPE(IntFlag));

    TIMx->IR |= (IntFlag);
}


/**
  * @brief  Close Timer/Counter peripheral.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR32B0: timer/counter CT32B0
  *             @arg LPC_TMR32B1: timer/counter CT32B1
  * @retval None
  *
  */
void TIM32_DeInit(LPC_TMR_TypeDef *TIMx)
{
    //CHECK_PARAM(PARAM_TIM32x(TIMx));

    /* Disable timer/counter  */
    TIMx->TCR = 0x00;
	
    /* Disable clock to TIM32  */
   if (TIMx == LPC_TMR32B0)
   {
	LPC_SYSCON->SYSAHBCLKCTRL &= ~ ( 1<<9);
   }
   else if (TIMx == LPC_TMR32B1)
   {
	LPC_SYSCON->SYSAHBCLKCTRL &= ~ (1<<10);
   }

}

/**
  * @brief  Initial Timer/Counter.
  *
  * @param  TIMx: pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR32B0: timer/counter CT32B0
  *             @arg LPC_TMR32B1: timer/counter CT32B1
  * @param  TimerCounterMode: timer/counter operation mode.
  *         This parameter can be one of the following values:
  *             @arg TIM32_MODE_TIMER: timer mode
  *             @arg TIM32_MODE_COUNTER_RISING: counter rising mode
  *             @arg TIM32_MODE_COUNTER_FALLING: counter falling mode
  *             @arg TIM32_MODE_COUNTER_ANY: counter on both edges
  * @param  TIM32_InitStruct: pointer to TIM32_InitTypeDef that contains
  *         the configuration information for the specified Timer peripheral.
  * @retval None
  *
  * Set Clock frequency for Timer and set initial configuration for Timer.
  */
void TIM32_Init(LPC_TMR_TypeDef *TIMx, uint8_t TimerCounterMode, void *TIM32_InitStruct)
{
    TIM32_InitTypeDef *pTimeCfg;
    TIM32_COUNTERTypeDef *pCounterCfg;
    pCounterCfg = pCounterCfg;

    //CHECK_PARAM(PARAM_TIM32x(TIMx));
    //CHECK_PARAM(PARAM_TIM32_MODE_OPT(TimerCounterMode));

    /* Enable clock to TIM32  */
    if (TIMx == LPC_TMR32B0)
    {
	 	LPC_SYSCON->SYSAHBCLKCTRL |= 1<<9;
    }
    else if (TIMx == LPC_TMR32B1)
    {
	 	LPC_SYSCON->SYSAHBCLKCTRL |= 1<<10;
    }
    
    TIMx->CCR &= ~TIM32_CTCR_MODE_MASK;
    TIMx->CCR |= TIM32_MODE_TIMER;
    
    TIMx->TC =0;
    TIMx->PC =0;
    TIMx->PR =0;
	
    if (TimerCounterMode == TIM32_MODE_TIMER )
    {
    	pTimeCfg = (TIM32_InitTypeDef *)TIM32_InitStruct;
    	if (pTimeCfg->PrescaleOption  == TIM32_PRESCALE_TICKVAL)
    	{
    	    TIMx->PR   = pTimeCfg->PrescaleValue -1  ;
    	}
    	else
    	{
    	    TIMx->PR   = TIM32_ConverUSecToVal (pTimeCfg->PrescaleValue)-1;
    	}
    }
    else
    {
    
    	pCounterCfg = (TIM32_COUNTERTypeDef *)TIM32_InitStruct;
    	TIMx->CCR  &= ~TIM32_CTCR_INPUT_MASK;
    }
    
    /* Clear interrupt pending   */
    TIMx->IR = 0xFFFFFFFF;
    
}

/**
  * @brief  Configuration for Timer32 at initial time.
  *
  * @param  TimerCounterMode: timer/counter operation mode.
  *         This parameter can be one of the following values:
  *             @arg TIM32_MODE_TIMER: timer mode
  *             @arg TIM32_MODE_COUNTER_RISING: counter rising mode
  *             @arg TIM32_MODE_COUNTER_FALLING: counter falling mode
  *             @arg TIM32_MODE_COUNTER_ANY: counter on both edges
  * @param  TIM32_InitStruct: pointer to TIM32_InitTypeDef or TIM32_COUNTERTypeDef.
  * @retval None
  *
  * If config to timer mode, the initial value of Prescale option is 
  * TIM32_PRESCALE_USVAL and PrescaleValue is 1. If config to counter mode, 
  * the initial capture channel is 0.
  */
void TIM32_StructInit(uint8_t TimerCounterMode, void *TIM32_InitStruct)
{
    //CHECK_PARAM(PARAM_TIM32_MODE_OPT(TimerCounterMode));
	
    if (TimerCounterMode == TIM32_MODE_TIMER )
    {
    	TIM32_InitTypeDef * pTimeCfg = (TIM32_InitTypeDef *)TIM32_InitStruct;
    	pTimeCfg->PrescaleOption = TIM32_PRESCALE_USVAL;
    	pTimeCfg->PrescaleValue = 1;
    }
    else
    {
    	TIM32_COUNTERTypeDef * pCounterCfg = (TIM32_COUNTERTypeDef *)TIM32_InitStruct;
    	pCounterCfg->CountInputSelect = TIM32_COUNTER_INCAP0;
    
    }
}


/**
  * @brief  Configuration for Match register.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR32B0: timer/counter CT32B0
  *             @arg LPC_TMR32B1: timer/counter CT32B1
  * @param  TIM32_MatchConfigStruct: Pointer to TIM32_MATCHTypeDef for match
  *         configuration.
  * @retval None
  *
  */
void TIM32_ConfigMatch(LPC_TMR_TypeDef *TIMx, TIM32_MATCHTypeDef *TIM32_MatchConfigStruct)
{
    //CHECK_PARAM(PARAM_TIM32x(TIMx));
    //CHECK_PARAM(PARAM_TIM32_MATCH_CHANNEL(TIM32_MatchConfigStruct->MatchChannel));
	 
    switch(TIM32_MatchConfigStruct->MatchChannel)
    {
    case TIM32_MATCH_CHANNEL0:
    	TIMx->MR0 = TIM32_MatchConfigStruct->MatchValue;
    	break;
    case TIM32_MATCH_CHANNEL1:
    	TIMx->MR1 = TIM32_MatchConfigStruct->MatchValue;
    	break;
    case TIM32_MATCH_CHANNEL2:
    	TIMx->MR2 = TIM32_MatchConfigStruct->MatchValue;
    	break;
    case TIM32_MATCH_CHANNEL3:
    	TIMx->MR3 = TIM32_MatchConfigStruct->MatchValue;
    	break;
    
    }
    /* clear the match channel config  */
    TIMx->MCR &=~(TIM32_MCR_CHANNEL_MASKBIT(TIM32_MatchConfigStruct->MatchChannel));
    
    /* interrupt on MRn  */
    if (TIM32_MatchConfigStruct->IntOnMatch)
    	TIMx->MCR |= TIM32_INT_ON_MATCH(TIM32_MatchConfigStruct->MatchChannel);
    
    /* reset on MRn  */
    if (TIM32_MatchConfigStruct->ResetOnMatch)
    	TIMx->MCR |= TIM32_RESET_ON_MATCH(TIM32_MatchConfigStruct->MatchChannel);
    
    /* stop on MRn  */
    if (TIM32_MatchConfigStruct->StopOnMatch)
    	TIMx->MCR |= TIM32_STOP_ON_MATCH(TIM32_MatchConfigStruct->MatchChannel);
    
    /*  match output type  */
    TIMx->EMR 	&= ~(TIM32_EM_MASK(TIM32_MatchConfigStruct->MatchChannel));
    TIMx->EMR    = TIM32_EM_SET(TIM32_MatchConfigStruct->MatchChannel,TIM32_MatchConfigStruct->ExtMatchOutputType);
}

/**
  * @brief  Configuration for Capture register.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR32B0: timer/counter CT32B0
  *             @arg LPC_TMR32B1: timer/counter CT32B1
  * @param  TIM32_CaptureConfigStruct: Pointer to TIM32_CAPTURETypeDef for capture
  *         configuration.
  * @retval None
  *
  */
void TIM32_ConfigCapture(LPC_TMR_TypeDef *TIMx, TIM32_CAPTURETypeDef *TIM32_CaptureConfigStruct)
{
    //CHECK_PARAM(PARAM_TIM32x(TIMx));
    //CHECK_PARAM(PARAM_TIM32_CAPTURE_CHANNEL(TIM32_CaptureConfigStruct->CaptureChannel));
	 
    TIMx->CCR &= ~(TIM32_CCR_CHANNEL_MASKBIT(TIM32_CaptureConfigStruct->CaptureChannel));
    
    if (TIM32_CaptureConfigStruct->RisingEdge)
    	TIMx->CCR |= TIM32_CAP_RISING(TIM32_CaptureConfigStruct->CaptureChannel);
    
    if (TIM32_CaptureConfigStruct->FallingEdge)
    	TIMx->CCR |= TIM32_CAP_FALLING(TIM32_CaptureConfigStruct->CaptureChannel);
    
    if (TIM32_CaptureConfigStruct->IntOnCapture)
    	TIMx->CCR |= TIM32_INT_ON_CAP(TIM32_CaptureConfigStruct->CaptureChannel);
}

/**
  * @brief  Configuration for PWM function.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR32B0: timer/counter CT32B0
  *             @arg LPC_TMR32B1: timer/counter CT32B1
  * @param  TIM_PWMConfigStruct: Pointer to TIM_PWMCFG_Type for PWM
  *         function configuration.
  * @retval None
  *
  */
void TIM32_ConfigPWM(LPC_TMR_TypeDef *TIMx, TIM32_PWMTypeDef *TIM32_PWMConfigStruct)
{
    //CHECK_PARAM(PARAM_TIM32x(TIMx));
    //CHECK_PARAM(PARAM_TIM32_PWM_OPT(TIM32_PWMConfigStruct->MatchChannel));
	 
    switch(TIM32_PWMConfigStruct->MatchChannel)
    {
    case TIM32_PWM_CH0:
    	TIMx->MR0 = TIM32_PWMConfigStruct->MatchValue;
    	break;
    case TIM32_PWM_CH1:
    	TIMx->MR1 = TIM32_PWMConfigStruct->MatchValue;
    	break;
    case TIM32_PWM_CH2:
    	TIMx->MR2 = TIM32_PWMConfigStruct->MatchValue;
    	break;
    case TIM32_PWM_CH3:
    	TIMx->MR3 = TIM32_PWMConfigStruct->MatchValue;
    	break;
    }
    /*  clear the match channel config   */
    TIMx->MCR &=~(TIM32_MCR_CHANNEL_MASKBIT(TIM32_PWMConfigStruct->MatchChannel));
    
    /* interrupt on MRn    */
    if (TIM32_PWMConfigStruct->IntOnMatch)
    	TIMx->MCR |= TIM32_INT_ON_MATCH(TIM32_PWMConfigStruct->MatchChannel);
    
    /* reset on MRn    */
    if (TIM32_PWMConfigStruct->ResetOnMatch)
    	TIMx->MCR |= TIM32_RESET_ON_MATCH(TIM32_PWMConfigStruct->MatchChannel);
    
    /* stop on MRn    */
    if (TIM32_PWMConfigStruct->StopOnMatch)
    	TIMx->MCR |= TIM32_STOP_ON_MATCH(TIM32_PWMConfigStruct->MatchChannel);
    
    /* PWM output type    */
    if (TIM32_PWMConfigStruct->ExtPWMOutput)
    {
    	TIMx->PWMC       |=TIM32_PWM_SET(TIM32_PWMConfigStruct->MatchChannel);
    }
    else
    {
        TIMx->PWMC      &=~(TIM32_PWM_MASK(TIM32_PWMConfigStruct->MatchChannel));
    }
    
}

/**
  * @brief  Start/Stop Timer/Counter peripheral.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR32B0: timer/counter CT32B0
  *             @arg LPC_TMR32B1: timer/counter CT32B1
  * @param  NewState: enable or disable timer.
  *         This parameter can be one of the following values:
  *             @arg ENABLE: set timer/counter enable
  *             @arg DISABLE: disable timer/counter 
  * @retval None
  *
  */
void TIM32_Cmd(LPC_TMR_TypeDef *TIMx, FunctionalState NewState)
{
    //CHECK_PARAM(PARAM_TIM32x(TIMx));

    if (NewState == ENABLE)
    {
    	TIMx->TCR	|=  TIM32_ENABLE;
    }
    else
    {
    	TIMx->TCR &= ~TIM32_ENABLE;
    }
}

/**
  * @brief  Reset Timer/Counter peripheral.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR16B0: timer/counter CT16B0
  *             @arg LPC_TMR16B1: timer/counter CT16B1
  *             @arg LPC_TMR32B0: timer/counter CT32B0
  *             @arg LPC_TMR32B1: timer/counter CT32B1
  * @retval None
  *
  * Make TC and PC are synchronously reset on the next positive edge of PCLK.
  */
void TIM32_ResetCounter(LPC_TMR_TypeDef *TIMx)
{
    //CHECK_PARAM(PARAM_TIM32x(TIMx));

    TIMx->TCR |= TIM32_RESET;
    TIMx->TCR &= ~TIM32_RESET;
}

/**
  * @brief  get TC value.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR32B0: timer/counter CT32B0
  *             @arg LPC_TMR32B1: timer/counter CT32B1
  * @retval value of TC register. 
  *
  */
uint32_t TIM32_GetTCValue(LPC_TMR_TypeDef *TIMx)
{
    //CHECK_PARAM(PARAM_TIM32x(TIMx));

    return (TIMx->TC) ;
}


/**
  * @brief  Read value of capture register in timer/counter peripheral.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR32B0: timer/counter CT32B0
  *             @arg LPC_TMR32B1: timer/counter CT32B1
  * @param  CaptureChannel: 0, 1, 2 or 3.
  * @retval Value of the corresponding capture register.
  *
  */
uint32_t TIM32_GetCaptureValue(LPC_TMR_TypeDef *TIMx, uint8_t CaptureChannel)
{
    //CHECK_PARAM(PARAM_TIM32x(TIMx));
    //CHECK_PARAM(PARAM_TIM32_CAPTURE_CHANNEL(CaptureChannel));

    if (CaptureChannel==TIM32_CAPTURE_CHANNEL0)
    	return TIMx->CR0;
    else if (CaptureChannel==TIM32_CAPTURE_CHANNEL1)
    	return TIMx->CR1;
    else if (CaptureChannel==TIM32_CAPTURE_CHANNEL2)
    	return TIMx->CR2;
    else if (CaptureChannel==TIM32_CAPTURE_CHANNEL3)
    	return TIMx->CR3;
    
    return ERROR; 	
}



/**
  * @brief  disable PWM output.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR32B0: timer/counter CT32B0
  *             @arg LPC_TMR32B1: timer/counter CT32B1
  * @param  TIM32_PWM_Ch: PWM channel to disable.
  *         This parameter can be one of the following values:
  *             @arg TIM32_PWM_CH0: PWM output on Match channel 0
  *             @arg TIM32_PWM_CH1: PWM output on Match channel 1
  *             @arg TIM32_PWM_CH2: PWM output on Match channel 2
  *             @arg TIM32_PWM_CH3: PWM output on Match channel 3
  * @retval None
  *
  */
void TIM32_ClearPWMOuptut(LPC_TMR_TypeDef *TIMx, uint8_t PWMChannel)
{
    TIMx->PWMC 	&= ~(TIM32_PWM_MASK(PWMChannel));
}

/**
  * @brief  disable all PWM output.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR32B0: timer/counter CT32B0
  *             @arg LPC_TMR32B1: timer/counter CT32B1
  * @retval None
  * 
  * All PWM outputs will be disabled.
  */
void TIM32_ClearAllPWM(LPC_TMR_TypeDef *TIMx)
{
    //CHECK_PARAM(PARAM_TIM32x(TIMx));

    TIMx->PWMC 	&= ~TIM32_PWM_MASKBIT;
}


/**
 * @}
 */
 
#endif // _TIMER32
 
/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */
