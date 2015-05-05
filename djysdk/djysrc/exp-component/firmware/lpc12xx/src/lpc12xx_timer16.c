/**************************************************************************//**
 * $Id: lpc12xx_timer16.c 475 2010-09-10 02:44:48Z cnh00813 $
 *
 * @file     lpc12xx_timer16.c
 * @brief    Contains all functions support for TIMER16 firmware library on LPC12xx.
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
/** @addtogroup TIMER16
 * @{
 */

/* Includes ----------------------------------------------------------------- */
#include "lpc12xx_timer16.h"

/* If this source file built with example, the LPC12xx FW library configuration
 * file in each example directory ("lpc12xx_libcfg.h") must be included,
 * otherwise the default FW library configuration file must be included instead
 */
#ifdef __BUILD_WITH_EXAMPLE__
#include "lpc12xx_libcfg.h"
#else
#include "lpc12xx_libcfg_default.h"
#endif /* __BUILD_WITH_EXAMPLE__ */

#ifdef _TIMER16

/** @defgroup TIM16_Private_Macros
  * @{
  */

  /*Macro define for Timer16 control register  */ 
#define TIM16_ENABLE	((uint32_t)(1<<0))  /* Timer/counter enable bit */
#define TIM16_RESET	((uint32_t)(1<<1))   /* Timer/counter reset bit */
#define TIM16_TCR_MASKBIT	((uint32_t)(3))     /* Timer control bit mask */
                               
/*Macro define for Timer16 match control register  */
/* Bit location for interrupt on MRx match, n = 0 to 1   */
#define TIM16_INT_ON_MATCH(n)      	(_BIT((n * 3)))
/* Bit location for reset on MRx match, n = 0 to 1  */
#define TIM16_RESET_ON_MATCH(n)    	(_BIT(((n * 3) + 1)))
/* Bit location for stop on MRx match, n = 0 to 1  */
#define TIM16_STOP_ON_MATCH(n)     	(_BIT(((n * 3) + 2)))
/* Timer Match control bit mask */
#define TIM16_MCR_MASKBIT		((uint32_t)(0x03F))
/* Timer Match control bit mask for specific channel*/
#define	TIM16_MCR_CHANNEL_MASKBIT(n)		((uint32_t)(7<<(n*3)))   

/*Macro define for Timer16 capture control register   */
/* Bit location for CAP.n on CRx rising edge, n = 0 to 3 */
#define TIM16_CAP_RISING(n)   	(_BIT((n * 3)))
/* Bit location for CAP.n on CRx falling edge, n = 0 to 3 */
#define TIM16_CAP_FALLING(n)   	(_BIT(((n * 3) + 1)))
/* Bit location for CAP.n on CRx interrupt enable, n = 0 to 3 */
#define TIM16_INT_ON_CAP(n)    	(_BIT(((n * 3) + 2)))
/* Mask bit for rising and falling edge bit */
#define TIM16_EDGE_MASK(n)		(_SBF((n * 3), 0x03))
/* Timer capture control bit mask */
#define TIM16_CCR_MASKBIT		((uint32_t)(0x0FFF))    
/* Timer Capture control bit mask for specific channel*/
#define	TIM16_CCR_CHANNEL_MASKBIT(n)	((uint32_t)(7<<(n*3)))  

/*Macro define for Timer16 external match control register */ 
/*Bit location for output state change of MAT.n when ext. match happens, n:0~1*/
#define TIM16_EM(n)    		 _BIT(n)
/* Output state change of MAT.n when external match happens: no change */
#define TIM16_EM_NOTHING    	((uint8_t)(0x0))
/* Output state change of MAT.n when external match happens: low */
#define TIM16_EM_LOW         	((uint8_t)(0x1))
/* Output state change of MAT.n when external match happens: high */
#define TIM16_EM_HIGH        	((uint8_t)(0x2))
/* Output state change of MAT.n when external match happens: toggle */
#define TIM16_EM_TOGGLE      	((uint8_t)(0x3))
/* Macro for setting for the MAT.n change state bits */
#define TIM16_EM_SET(n,s) 	(_SBF(((n << 1) + 4), (s & 0x03)))
/* Mask for the MATn change state bits */
#define TIM16_EM_MASK(n) 		(_SBF(((n << 1) + 4), 0x03))
/* Timer external match bit mask */
#define TIM16_EMR_MASKBIT	0x0FF

/*Macro define for Timer16 Count control register  */
#define TIM16_CTCR_MODE_MASK  0x3  /* Mask to get the Counter/timer mode bits */
#define TIM16_CTCR_INPUT_MASK 0xC  /* Mask to get the count input select bits */
#define TIM16_CTCR_MASKBIT    0xF     /* Timer Count control bit mask */
#define TIM16_COUNTER_MODE ((uint8_t)(1))


/*Macro define for PWM control register, n = 0 to 1 */ 
#define TIM16_PWM_SET(n)  _BIT(n)  /* Bit location for PWM output state 
                                      change of MAT.n */
#define TIM16_PWM_MASK(n) _BIT(n)  /*Mask for PWMn change state bits */
#define TIM16_PWM_MASKBIT    0x3   /*PWM control register bit mask */

/**
 * @}
 */
 

/** @defgroup TIM16_Private_Functions
 * @{
 */ 

/**
  * @brief  Convert a time to a timer count value.
  *
  * @param  usec: usec Time in microseconds. 
  * @retval The number of required clock ticks to give the time delay.   
  */ 
uint16_t TIM16_ConverUSecToVal (uint32_t USec)
{
    uint64_t clkdlycnt;
    
    /* Get Pclock of timer */
    clkdlycnt = (uint64_t) SystemCoreClock ;
    
    clkdlycnt = (clkdlycnt * USec) / 1000000;
    return (uint16_t) clkdlycnt;
}

/**
  * @brief  Get timer16 Interrupt Status.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR16B0: timer/counter CT16B0
  *             @arg LPC_TMR16B1: timer/counter CT16B1
  * @param  IntFlag: Timer interrupt flag.
  *         This parameter can be one of the following values:
  *             @arg TIM16_INT_MR0: interrupt for Match channel 0
  *             @arg TIM16_INT_MR1: interrupt for Match channel 1
  *             @arg TIM16_INT_CR0: interrupt for Capture channel 0
  *             @arg TIM16_INT_CR1: interrupt for Capture channel 1
  *             @arg TIM16_INT_CR2: interrupt for Capture channel 2
  *             @arg TIM16_INT_CR3: interrupt for Capture channel 3
  * @retval Interrupt flag status (SET or RESET).
  *             @arg SET: interrupt
  *             @arg RESET: no interrupt 
  */
IntStatus TIM16_GetIntStatus(LPC_TMR_TypeDef *TIMx, uint8_t IntFlag)
{
    uint8_t temp;
	 
    CHECK_PARAM(PARAM_TIM16x(TIMx));
    CHECK_PARAM(PARAM_TIM16_INT_TYPE(IntFlag));
	 
    temp = (TIMx->IR)& (IntFlag);
    if (temp)
    	return SET;
    
    return RESET;
    
}

/**
  * @brief  Get timer16 Interrupt Status register value.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR16B0: timer/counter CT16B0
  *             @arg LPC_TMR16B1: timer/counter CT16B1
  * @retvalInterrupt Status register (MIS) value.
  *         This parameter can be any combination of the following values.
  *             @arg TIM16_INT_MR0: interrupt for Match channel 0
  *             @arg TIM16_INT_MR1: interrupt for Match channel 1
  *             @arg TIM16_INT_CR0: interrupt for Capture channel 0
  *             @arg TIM16_INT_CR1: interrupt for Capture channel 1
  *             @arg TIM16_INT_CR2: interrupt for Capture channel 2
  *             @arg TIM16_INT_CR3: interrupt for Capture channel 3
  */

uint32_t TIM16_GetIntStatusReg(LPC_TMR_TypeDef *TIMx)
{
    return (TIMx->IR);
}

/**
  * @brief  Clear Interrupt pending bit.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR16B0: timer/counter CT16B0
  *             @arg LPC_TMR16B1: timer/counter CT16B1
  * @param  IntFlag: Timer interrupt flag.
  *         This parameter can be one of the following values:
  *             @arg TIM16_INT_MR0: interrupt for Match channel 0
  *             @arg TIM16_INT_MR1: interrupt for Match channel 1
  *             @arg TIM16_INT_CR0: interrupt for Capture channel 0
  *             @arg TIM16_INT_CR1: interrupt for Capture channel 1
  *             @arg TIM16_INT_CR2: interrupt for Capture channel 2
  *             @arg TIM16_INT_CR3: interrupt for Capture channel 3
  * @retval None
  */
void TIM16_ClearIntPendingBit(LPC_TMR_TypeDef *TIMx, uint8_t IntFlag)
{
    CHECK_PARAM(PARAM_TIM16x(TIMx));
    CHECK_PARAM(PARAM_TIM16_INT_TYPE(IntFlag));

    TIMx->IR |= (IntFlag);
}


/**
  * @brief  Close Timer/Counter peripheral.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR16B0: timer/counter CT16B0
  *             @arg LPC_TMR16B1: timer/counter CT16B1
  * @retval None
  *
  */
void TIM16_DeInit(LPC_TMR_TypeDef *TIMx)
{
    CHECK_PARAM(PARAM_TIM16x(TIMx));

    /* Disable timer/counter */
    TIMx->TCR = 0x00;

    /* Disable clock to TIM16  */
    if (TIMx == LPC_TMR16B0)
    {
	LPC_SYSCON->SYSAHBCLKCTRL &= ~ ( 1<<7);
    }
    else if (TIMx == LPC_TMR16B1)
    {
	LPC_SYSCON->SYSAHBCLKCTRL &= ~ (1<<8);
    }

}

/**
  * @brief  Initial Timer/Counter.
  *
  * @param  TIMx: pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR16B0: timer/counter CT16B0
  *             @arg LPC_TMR16B1: timer/counter CT16B1
  * @param  TimerCounterMode: timer/counter operation mode.
  *         This parameter can be one of the following values:
  *             @arg TIM16_TIMER_MODE: timer mode
  *             @arg TIM16_COUNTER_RISING_MODE: counter rising mode
  *             @arg TIM16_COUNTER_FALLING_MODE: counter falling mode
  *             @arg TIM16_COUNTER_ANY_MODE: counter on both edges
  * @param  TIM16_InitStruct: pointer to TIM16_InitTypeDef that contains
  *         the configuration information for the specified Timer peripheral.
  * @retval None
  *
  * Set Clock frequency for Timer and set initial configuration for Timer.
  */
void TIM16_Init(LPC_TMR_TypeDef *TIMx, uint8_t TimerCounterMode, void *TIM16_InitStruct)
{

    TIM16_InitTypeDef *pTimeCfg;
    TIM16_COUNTERTypeDef *pCounterCfg;
    pCounterCfg = pCounterCfg;

    CHECK_PARAM(PARAM_TIM16x(TIMx));
    CHECK_PARAM(PARAM_TIM16_MODE_OPT(TimerCounterMode));

    /* Enable clock to TIM16  */
    if (TIMx == LPC_TMR16B0)
    {
	LPC_SYSCON->SYSAHBCLKCTRL |= 1<<7;
    }
    else if (TIMx == LPC_TMR16B1)
    {
	LPC_SYSCON->SYSAHBCLKCTRL |= 1<<8;
    }

    TIMx->CCR &= ~TIM16_CTCR_MODE_MASK;
    TIMx->CCR |= TIM16_TIMER_MODE;
    
    TIMx->TC =0;
    TIMx->PC =0;
    TIMx->PR =0;
    if (TimerCounterMode == TIM16_TIMER_MODE )
    {
    	pTimeCfg = (TIM16_InitTypeDef *)TIM16_InitStruct;
    	if (pTimeCfg->PrescaleOption  == TIM16_PRESCALE_TICKVAL)
    	{
            /* PR is 16-bit register  */ 
	    TIMx->PR   = (pTimeCfg->PrescaleValue -1)& 0x0FFFF  ;  
    	}
    	else
    	{
    	    TIMx->PR   = TIM16_ConverUSecToVal (pTimeCfg->PrescaleValue)-1;
    	}
    }
    else
    {
    
    	pCounterCfg = (TIM16_COUNTERTypeDef *)TIM16_InitStruct;
    	TIMx->CCR  &= ~TIM16_CTCR_INPUT_MASK;
    }
    
    /* Clear interrupt pending  */
    TIMx->IR = 0xFFFFFFFF;
    
}


/**
  * @brief  Configuration for Timer16 at initial time.
  *
  * @param  TimerCounterMode: timer/counter operation mode.
  *         This parameter can be one of the following values:
  *             @arg TIM16_TIMER_MODE: timer mode
  *             @arg TIM16_COUNTER_RISING_MODE: counter rising mode
  *             @arg TIM16_COUNTER_FALLING_MODE: counter falling mode
  *             @arg TIM16_COUNTER_ANY_MODE: counter on both edges
  * @param  TIM16_InitStruct: pointer to TIM16_InitTypeDef or TIM16_COUNTERTypeDef.
  * @retval None
  *
  * If config to timer mode, the initial value of Prescale option is 
  * TIM16_PRESCALE_USVAL and PrescaleValue is 1. If config to counter mode, 
  * the initial capture channel is 0.
  */
void TIM16_StructInit(uint8_t TimerCounterMode, void *TIM16_InitStruct)
{
    CHECK_PARAM(PARAM_TIM16_MODE_OPT(TimerCounterMode));
	
    if (TimerCounterMode == TIM16_TIMER_MODE )
    {
    	TIM16_InitTypeDef * pTimeCfg = (TIM16_InitTypeDef *)TIM16_InitStruct;
    	pTimeCfg->PrescaleOption = TIM16_PRESCALE_USVAL;
    	pTimeCfg->PrescaleValue = 1;
    }
    else
    {
    	TIM16_COUNTERTypeDef * pCounterCfg = (TIM16_COUNTERTypeDef *)TIM16_InitStruct;
    	pCounterCfg->CountInputSelect = TIM16_COUNTER_INCAP0;
    
    }
}


/**
  * @brief  Configuration for Match register.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR16B0: timer/counter CT16B0
  *             @arg LPC_TMR16B1: timer/counter CT16B1
  * @param  TIM16_MatchConfigStruct: Pointer to TIM16_MATCHTypeDef for match
  *         configuration.
  * @retval None
  *
  */
void TIM16_ConfigMatch(LPC_TMR_TypeDef *TIMx, TIM16_MATCHTypeDef *TIM16_MatchConfigStruct)
{
    CHECK_PARAM(PARAM_TIM16x(TIMx));
    CHECK_PARAM(PARAM_TIM16_MATCH_CHANNEL(TIM16_MatchConfigStruct->MatchChannel));
	 
    switch(TIM16_MatchConfigStruct->MatchChannel)
    {
    case TIM16_MATCH_CHANNEL0:
    	TIMx->MR0 = TIM16_MatchConfigStruct->MatchValue;
    	break;
    case TIM16_MATCH_CHANNEL1:
    	TIMx->MR1 = TIM16_MatchConfigStruct->MatchValue;
    	break;
    }
    /* clear the match channel config  */
    TIMx->MCR &=~(TIM16_MCR_CHANNEL_MASKBIT(TIM16_MatchConfigStruct->MatchChannel));
    
    /* interrupt on MRn */
    if (TIM16_MatchConfigStruct->IntOnMatch)
    	TIMx->MCR |= TIM16_INT_ON_MATCH(TIM16_MatchConfigStruct->MatchChannel);
    
    /* reset on MRn */
    if (TIM16_MatchConfigStruct->ResetOnMatch)
    	TIMx->MCR |= TIM16_RESET_ON_MATCH(TIM16_MatchConfigStruct->MatchChannel);
    
    /* stop on MRn */
    if (TIM16_MatchConfigStruct->StopOnMatch)
    	TIMx->MCR |= TIM16_STOP_ON_MATCH(TIM16_MatchConfigStruct->MatchChannel);
    
    /*  match output type */
    TIMx->EMR &= ~(TIM16_EM_MASK(TIM16_MatchConfigStruct->MatchChannel));
    TIMx->EMR = TIM16_EM_SET(TIM16_MatchConfigStruct->MatchChannel, TIM16_MatchConfigStruct->ExtMatchOutputType);
}

/**
  * @brief  Configuration for Capture register.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR16B0: timer/counter CT16B0
  *             @arg LPC_TMR16B1: timer/counter CT16B1
  * @param  TIM16_CaptureConfigStruct: Pointer to TIM16_CAPTURETypeDef for capture
  *         configuration.
  * @retval None
  *
  */
void TIM16_ConfigCapture(LPC_TMR_TypeDef *TIMx, TIM16_CAPTURETypeDef *TIM16_CaptureConfigStruct)
{
    CHECK_PARAM(PARAM_TIM16x(TIMx));
    CHECK_PARAM(PARAM_TIM16_CAPTURE_CHANNEL(TIM16_CaptureConfigStruct->CaptureChannel));
	 
    TIMx->CCR &= ~(TIM16_CCR_CHANNEL_MASKBIT(TIM16_CaptureConfigStruct->CaptureChannel));
    
    if (TIM16_CaptureConfigStruct->RisingEdge)
    	TIMx->CCR |= TIM16_CAP_RISING(TIM16_CaptureConfigStruct->CaptureChannel);
    
    if (TIM16_CaptureConfigStruct->FallingEdge)
    	TIMx->CCR |= TIM16_CAP_FALLING(TIM16_CaptureConfigStruct->CaptureChannel);
    
    if (TIM16_CaptureConfigStruct->IntOnCapture)
    	TIMx->CCR |= TIM16_INT_ON_CAP(TIM16_CaptureConfigStruct->CaptureChannel);
}

/**
  * @brief  Configuration for PWM function.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR16B0: timer/counter CT16B0
  *             @arg LPC_TMR16B1: timer/counter CT16B1
  * @param  TIM16_PWMConfigStruct: Pointer to TIM16_PWMTypeDef for PWM
  *         function configuration.
  * @retval None
  *
  */
void TIM16_ConfigPWM(LPC_TMR_TypeDef *TIMx, TIM16_PWMTypeDef *TIM16_PWMConfigStruct)
{
    CHECK_PARAM(PARAM_TIM16x(TIMx));
    CHECK_PARAM(PARAM_TIM16_PWM_OPT(TIM16_PWMConfigStruct->MatchChannel));
	 
    switch(TIM16_PWMConfigStruct->MatchChannel)
    {
    case TIM16_PWM_CH0:
    	TIMx->MR0 = TIM16_PWMConfigStruct->MatchValue;
    	break;
    case TIM16_PWM_CH1:
    	TIMx->MR1 = TIM16_PWMConfigStruct->MatchValue;
    	break;
    }
    /* clear the match channel config  */
    TIMx->MCR &=~(TIM16_MCR_CHANNEL_MASKBIT(TIM16_PWMConfigStruct->MatchChannel));
    
    /* interrupt on MRn */
    if (TIM16_PWMConfigStruct->IntOnMatch)
    	TIMx->MCR |= TIM16_INT_ON_MATCH(TIM16_PWMConfigStruct->MatchChannel);
    
    /* reset on MRn  */
    if (TIM16_PWMConfigStruct->ResetOnMatch)
    	TIMx->MCR |= TIM16_RESET_ON_MATCH(TIM16_PWMConfigStruct->MatchChannel);
    
    /* stop on MRn  */
    if (TIM16_PWMConfigStruct->StopOnMatch)
    	TIMx->MCR |= TIM16_STOP_ON_MATCH(TIM16_PWMConfigStruct->MatchChannel);
    
    /*  PWM output type  */
    if (TIM16_PWMConfigStruct->ExtPWMOutput)
    {
    	TIMx->PWMC |=TIM16_PWM_SET(TIM16_PWMConfigStruct->MatchChannel);
    }
    else
    {
        TIMx->PWMC &=~(TIM16_PWM_MASK(TIM16_PWMConfigStruct->MatchChannel));
    }
    
}

/**
  * @brief  Start/Stop Timer/Counter peripheral.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR16B0: timer/counter CT16B0
  *             @arg LPC_TMR16B1: timer/counter CT16B1
  * @param  NewState: enable or disable timer.
  *         This parameter can be one of the following values:
  *             @arg ENABLE: set timer/counter enable
  *             @arg DISABLE: disable timer/counter 
  * @retval None
  *
  */
void TIM16_Cmd(LPC_TMR_TypeDef *TIMx, FunctionalState NewState)
{
    CHECK_PARAM(PARAM_TIM16x(TIMx));

    if (NewState == ENABLE)
    {
    	TIMx->TCR |=  TIM16_ENABLE;
    }
    else
    {
    	TIMx->TCR &= ~TIM16_ENABLE;
    }
}

/**
  * @brief  Reset Timer/Counter peripheral.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR16B0: timer/counter CT16B0
  *             @arg LPC_TMR16B1: timer/counter CT16B1
  * @retval None
  *
  * Make TC and PC are synchronously reset on the next positive edge of PCLK.
  */
void TIM16_ResetCounter(LPC_TMR_TypeDef *TIMx)
{
    CHECK_PARAM(PARAM_TIM16x(TIMx));

    TIMx->TCR |= TIM16_RESET;
    TIMx->TCR &= ~TIM16_RESET;
}

/**
  * @brief  get TC value.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR16B0: timer/counter CT16B0
  *             @arg LPC_TMR16B1: timer/counter CT16B1
  * @retval value of TC register. 
  *
  */
uint32_t TIM16_GetTCValue(LPC_TMR_TypeDef *TIMx)
{
    CHECK_PARAM(PARAM_TIM16x(TIMx));

    return ((TIMx->TC) & 0x0FFFF);
}


/**
  * @brief  Read value of capture register in timer/counter peripheral.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR16B0: timer/counter CT16B0
  *             @arg LPC_TMR16B1: timer/counter CT16B1
  * @param  CaptureChannel: 0, 1, 2 or 3.
  * @retval Value of the corresponding capture register.
  *
  */
uint32_t TIM16_GetCaptureValue(LPC_TMR_TypeDef *TIMx, uint8_t CaptureChannel)
{
    CHECK_PARAM(PARAM_TIM16x(TIMx));
    CHECK_PARAM(PARAM_TIM16_CAPTURE_CHANNEL(CaptureChannel));

    if (CaptureChannel==TIM16_CAPTURE_CHANNEL0)
    return TIMx->CR0;
    else if (CaptureChannel==TIM16_CAPTURE_CHANNEL1)
    	return TIMx->CR1;
    else if (CaptureChannel==TIM16_CAPTURE_CHANNEL2)
    	return TIMx->CR2;
    else if (CaptureChannel==TIM16_CAPTURE_CHANNEL3)
    	return TIMx->CR3;
    
    return ERROR; 	
}



/**
  * @brief  disable PWM output.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR16B0: timer/counter CT16B0
  *             @arg LPC_TMR16B1: timer/counter CT16B1
  * @param  TIM16_PWM_Ch: PWM channel to disable.
  *         This parameter can be one of the following values:
  *             @arg TIM16_PWM_CH0: PWM output on Match channel 0
  *             @arg TIM16_PWM_CH1: PWM output on Match channel 1
  * @retval None
  *
  */
void TIM16_ClearPWMOuptut(LPC_TMR_TypeDef *TIMx, uint8_t PWMChannel)
{
    CHECK_PARAM(PARAM_TIM16x(TIMx));
    CHECK_PARAM(PARAM_TIM16_PWM_OPT(PWMChannel));

    TIMx->PWMC &= ~(TIM16_PWM_MASK(PWMChannel));
}

/**
  * @brief  disable all PWM output.
  *
  * @param  TIMx: Pointer to specified timer/counter. TIMx Timer selection.
  *         This parameter can be one of the following values:
  *             @arg LPC_TMR16B0: timer/counter CT16B0
  *             @arg LPC_TMR16B1: timer/counter CT16B1
  * @retval None
  * 
  * All PWM outputs will be disabled.
  */
void TIM16_ClearAllPWM(LPC_TMR_TypeDef *TIMx)
{
    CHECK_PARAM(PARAM_TIM16x(TIMx));

    TIMx->PWMC 	&= ~TIM16_PWM_MASKBIT;
}


/**
 * @}
 */
 
#endif // _TIMER16
 
/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */
