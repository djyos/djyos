/**************************************************************************//**
 * $Id: lpc12xx_timer16.h 416 2010-09-02 03:56:10Z cnh00813 $
 *
 * @file     lpc12xx_timer16.h
 * @brief    Contains all macro definitions and function prototypes
 *           support for TIMER16 firmware library on LPC12xx.
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
/** @defgroup TIMER16
 * @ingroup LPC1200CMSIS_FwLib_Drivers 
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LPC12xx_TIMER16_H
#define __LPC12xx_TIMER16_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include "LPC12xx.h"
#include "lpc_types.h"


/* Public Types ------------------------------------------------------------- */
/** @defgroup TIM16_Public_Types
 * @{
 */
 
/**
 * @brief TIMER16 Init structure definition
 */
typedef struct
{
    
    uint8_t PrescaleOption;	  /* !< Specifies timer Prescale option. 
                                   This parameter can be a value of 
                                   @ref TIM16_Prescale_Option  */
    uint8_t Reserved[3];	  /* Reserved */
    uint32_t PrescaleValue;	  /* Prescale value */  
} TIM16_InitTypeDef;

/**
 * @brief Configuration structure definition in COUNTER mode
 */
typedef struct {
    uint8_t CounterOption;	/* !< Specifies the Counter Option. 
    				   This parameter can be a value of 
    				   @ref TIM16_Capture_Mode_Option */
    uint8_t CountInputSelect;  /* !< Specifies counter input select 
				   This parameter can be a value of 
				   @ref TIM16_CounterInput_Option */
    uint8_t Reserved[2];
} TIM16_COUNTERTypeDef;

/**
 * @brief TIMER16 Match structure definition 
 */
typedef struct {
    uint8_t MatchChannel;	/* !< Specifies the Match channel.
                                    This parameter can be a value of 
                                    @ref TIM16_Match_Channel  */
    uint8_t IntOnMatch;	        /* !< Specifies the interrupt on match.
    			           - ENABLE: Enable this function.
    			           - DISABLE: Disable this function.
    			        */
    uint8_t StopOnMatch;	/* !< Specifies Stop On match.
    			           - ENABLE: Enable this function.
    			           - DISABLE: Disable this function.
    			        */
    uint8_t ResetOnMatch;	/* !< Specifies Reset On match.
    			           - ENABLE: Enable this function.
    			           - DISABLE: Disable this function.
    			        */
    uint8_t ExtMatchOutputType;	/* !< Specifies External Match Output type.
                                   This parameter can be a value of 
                                   @ref TIM16_ExtMatch_Option  */
    uint8_t Reserved[3];	/* Reserved */
    uint16_t MatchValue;	/* 16-bit Match value */
} TIM16_MATCHTypeDef;

/**
 * @brief TIMER16 Capture structure definition
 */
typedef struct {
    uint8_t CaptureChannel;	  /* !< Specifies Capture channel.
                                    This parameter can be a value of 
                                    @ref TIM16_Capture_Channel  */
    uint8_t RisingEdge;	         /* !< Specifies caputre on rising edge.
    			            - ENABLE: Enable rising edge.
    			            - DISABLE: Disable this function.
    				 */
    uint8_t FallingEdge;	 /* !< Specifies capture on falling edge.
    				    - ENABLE: Enable falling edge.
    				    - DISABLE: Disable this function.
    				 */
    uint8_t IntOnCapture;	 /* !< Specifies Interrupt On capture.
    				    - ENABLE: Enable interrupt function.
    				    - DISABLE: Disable this function.
    				 */
} TIM16_CAPTURETypeDef;

/**
 * @brief PWM structure definition 
 */
typedef struct {
    uint8_t MatchChannel;     /* !< Specifies the Match channel.
                                 This parameter can be a value of 
                                 @ref TIM16_PWM_OPT  */
    uint8_t IntOnMatch;	      /* !< Specifies Interrupt On match.
    			         - ENABLE: Enable this function.
    			         - DISABLE: Disable this function.
    			      */
    uint8_t StopOnMatch;      /* !< Specifies Stop On match.
    			         - ENABLE: Enable this function.
    			         - DISABLE: Disable this function.
    			      */
    uint8_t ResetOnMatch;     /* !< Specifies Reset On match.
    			         - ENABLE: Enable this function.
    			         - DISABLE: Disable this function.
    			      */
    uint8_t ExtPWMOutput;     /* !< Specifies External PWM Output enable or not.
    				 - DISABLE:disable PWM output on Match ch x
    				 - ENABLE:Enable PWM output on Match ch x  */
    uint8_t Reserved[3];      /* Reserved */
    uint16_t MatchValue;      /* 16-bit Match value */
} TIM16_PWMTypeDef;

/**
 * @}
 */


/* Public Macros -------------------------------------------------------------- */
/** @defgroup TIM16_Public_Macros
 * @{
 */


/* @brief Macro to determine if it is valid TIMER16 peripheral */
#define PARAM_TIM16x(n)	 ((((uint32_t *)n)==((uint32_t *)LPC_TMR16B0)) \
                         || (((uint32_t *)n)==((uint32_t *)LPC_TMR16B1)))



/** @defgroup TIM16_Match_Channel
  * @{
  */

#define TIM16_MATCH_CHANNEL0    0
#define TIM16_MATCH_CHANNEL1    1

#define PARAM_TIM16_MATCH_CHANNEL(ch)	((   ch  ==  TIM16_MATCH_CHANNEL0) ||\
                                        (    ch  ==  TIM16_MATCH_CHANNEL1))

/**
  * @}
  */


/** @defgroup TIM16_Capture_Channel
  * @{
  */

#define TIM16_CAPTURE_CHANNEL0	    0
#define TIM16_CAPTURE_CHANNEL1      1
#define TIM16_CAPTURE_CHANNEL2      2
#define TIM16_CAPTURE_CHANNEL3      3

#define PARAM_TIM16_CAPTURE_CHANNEL(ch)	((   ch  ==  TIM16_CAPTURE_CHANNEL0) ||\
                                        (    ch  ==  TIM16_CAPTURE_CHANNEL1) ||\
                                        (    ch  ==  TIM16_CAPTURE_CHANNEL2) ||\
                                        (    ch  ==  TIM16_CAPTURE_CHANNEL3))
                                                               
/**
  * @}
  */


/** @defgroup TIM16_interrupt_type
  * @{
  */
#define    TIM16_INT_MR0      1<<0         /* interrupt for Match channel 0*/
#define    TIM16_INT_MR1      1<<1         /* interrupt for Match channel 1*/
#define    TIM16_INT_CR0      1<<4         /* interrupt for Capture channel 0*/
#define    TIM16_INT_CR1      1<<5         /* interrupt for Capture channel 1*/ 
#define    TIM16_INT_CR2      1<<6         /* interrupt for Capture channel 2*/ 
#define    TIM16_INT_CR3      1<<7         /* interrupt for Capture channel 3*/ 

#define PARAM_TIM16_INT_TYPE(TYPE)	((TYPE ==TIM16_INT_MR0)||\
					(TYPE ==TIM16_INT_MR1)||\
					(TYPE ==TIM16_INT_CR0)||\
					(TYPE ==TIM16_INT_CR1)||\
					(TYPE ==TIM16_INT_CR2)||\
					(TYPE ==TIM16_INT_CR3))

/**
  * @}
  */
  

/** @defgroup TIM16_mode 
  * @{
  */

#define    TIM16_TIMER_MODE                  0	     /* Timer mode */
#define    TIM16_COUNTER_RISING_MODE         1	     /* Counter rising mode */
#define    TIM16_COUNTER_FALLING_MODE        2	     /* Counter falling mode */
#define    TIM16_COUNTER_ANY_MODE	     3       /* Counter on both edges */

#define PARAM_TIM16_MODE_OPT(MODE)    (( MODE	==TIM16_TIMER_MODE)|| \
				      ( MODE ==TIM16_COUNTER_RISING_MODE)|| \
				      ( MODE ==TIM16_COUNTER_FALLING_MODE)|| \
				      ( MODE ==TIM16_COUNTER_ANY_MODE))

/**
  * @}
  */
  

/** @defgroup TIM16_Prescale_Option
  * @{
  */

#define    TIM16_PRESCALE_TICKVAL     0	     /* Prescale in absolute value */
#define    TIM16_PRESCALE_USVAL	      1	     /* Prescale in microsecond value */

#define PARAM_TIM16_PRESCALE_OPT(OPT)	(( OPT == TIM16_PRESCALE_TICKVAL)|| \
					( OPT == TIM16_PRESCALE_USVAL))

/**
  * @}
  */
  

/** @defgroup TIM16_CounterInput_Option
  * @{
  */

#define    TIM16_COUNTER_INCAP0   0	/* CAPn.0 input pin for TIMERn */
#define    TIM16_COUNTER_INCAP1   1	/* CAPn.1 input pin for TIMERn */
#define    TIM16_COUNTER_INCAP2   2     /* Comparator n, level output as the 
                                           input for TIMERn */
#define    TIM16_COUNTER_INCAP3   3     /* Comparator n, edge output as the 
                                           input for TIMERn */

#define PARAM_TIM16_COUNTER_INPUT_OPT(OPT)  (( OPT == TIM16_COUNTER_INCAP0)||\
					    ( OPT == TIM16_COUNTER_INCAP1)|| \
					    ( OPT == TIM16_COUNTER_INCAP2)|| \
					    ( OPT == TIM16_COUNTER_INCAP3))


/**
  * @}
  */
  

/** @defgroup TIM16_ExtMatch_Option
  * @{
  */

#define  TIM16_EXTMATCH_NOTHING  0  /* Do nothing for ext. output pin if match*/
#define  TIM16_EXTMATCH_LOW      1  /* Force ext. output pin to low if match */
#define  TIM16_EXTMATCH_HIGH     2  /* Force ext. output pin to high if match */
#define  TIM16_EXTMATCH_TOGGLE   3  /* Toggle ext. output pin if match */

#define PARAM_TIM16_EXTMATCH_OPT(OPT)	(( OPT == TIM16_EXTMATCH_NOTHING)|| \
					( OPT == TIM16_EXTMATCH_LOW)|| \
					( OPT == TIM16_EXTMATCH_HIGH)|| \
					( OPT == TIM16_EXTMATCH_TOGGLE))

/**
  * @}
  */
  

/** @defgroup TIM16_Capture_Mode_Option
  * @{
  */

#define    TIM16_CAPTURE_NONE  	   0	/* No Capture */
#define    TIM16_CAPTURE_RISING    1	/* Rising capture mode */
#define    TIM16_CAPTURE_FALLING   2    /* Falling capture mode */
#define    TIM16_CAPTURE_ANY	   3	/* On both edges */

#define PARAM_TIM16_CAP_MODE_OPT(OPT)	(( OPT == TIM16_CAPTURE_NONE)|| \
					( OPT == TIM16_CAPTURE_RISING)|| \
					( OPT == TIM16_CAPTURE_FALLING)|| \
					( OPT == TIM16_CAPTURE_ANY))

/**
  * @}
  */
  

/** @defgroup TIM16_PWM_Option
  * @{
  */
  
typedef uint8_t  TIM16_PWM_OPT;
#define    TIM16_PWM_CH0     0      /* PWM output on Match channel 0*/
#define    TIM16_PWM_CH1     1      /* PWM output on Match channel 1*/

#define PARAM_TIM16_PWM_OPT(OPT)	(( OPT == TIM16_PWM_CH0)|| \
				        ( OPT == TIM16_PWM_CH1))


/**
  * @}
  */


/**
 * @}
 */


/* Public Functions ----------------------------------------------------------- */
/** @defgroup TIM16_Public_Functions
 * @{
 */

uint16_t TIM16_ConverUSecToVal (uint32_t USec);
IntStatus TIM16_GetIntStatus(LPC_TMR_TypeDef *TIMx, uint8_t IntFlag);
uint32_t TIM16_GetIntStatusReg(LPC_TMR_TypeDef *TIMx);
void TIM16_ClearIntPendingBit(LPC_TMR_TypeDef *TIMx, uint8_t IntFlag);
void TIM16_DeInit(LPC_TMR_TypeDef *TIMx);
void TIM16_Init(LPC_TMR_TypeDef *TIMx, uint8_t TimerCounterMode, void *TIM16_InitStruct);
void TIM16_StructInit(uint8_t TimerCounterMode, void *TIM16_InitStruct);
void TIM16_ConfigMatch(LPC_TMR_TypeDef *TIMx, TIM16_MATCHTypeDef *TIM16_MatchConfigStruct);
void TIM16_ConfigCapture(LPC_TMR_TypeDef *TIMx, TIM16_CAPTURETypeDef *TIM16_CaptureConfigStruct);
void TIM16_ConfigPWM(LPC_TMR_TypeDef *TIMx, TIM16_PWMTypeDef *TIM16_PWMConfigStruct);
void TIM16_Cmd(LPC_TMR_TypeDef *TIMx, FunctionalState NewState);
void TIM16_ResetCounter(LPC_TMR_TypeDef *TIMx);
uint32_t TIM16_GetTCValue(LPC_TMR_TypeDef *TIMx);
uint32_t TIM16_GetCaptureValue(LPC_TMR_TypeDef *TIMx, uint8_t CaptureChannel);
void TIM16_ClearPWMOuptut(LPC_TMR_TypeDef *TIMx, uint8_t PWMChannel);
void TIM16_ClearAllPWM(LPC_TMR_TypeDef *TIMx);


/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __LPC12xx_TIMER16_H */

/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */



