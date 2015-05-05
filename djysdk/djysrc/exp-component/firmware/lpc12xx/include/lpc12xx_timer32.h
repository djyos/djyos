/**************************************************************************//**
 * $Id: lpc12xx_timer32.h 415 2010-09-02 02:39:05Z cnh00813 $
 *
 * @file     lpc12xx_timer32.h
 * @brief    Contains all macro definitions and function prototypes
 *           support for TIMER32 firmware library on LPC12xx.
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
/** @defgroup TIMER32
 * @ingroup LPC1200CMSIS_FwLib_Drivers 
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LPC12xx_TIMER32_H
#define __LPC12xx_TIMER32_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include "LPC12xx.h"
#include "lpc_types.h"


/* Public Types ------------------------------------------------------------- */
/** @defgroup TIM32_Public_Types
 * @{
 */

/**
 * @brief TIMER32 Init structure definition 
 */
typedef struct
{
    
    uint8_t PrescaleOption;	/* !< Specifies timer Prescale option. 
                                   This parameter can be a value of 
                                   @ref TIM32_Prescale_Option  */

    uint8_t Reserved[3];	/* Reserved */
    uint32_t PrescaleValue;	/* !< Specifies the prescale value */
} TIM32_InitTypeDef;

/**
 * @brief Configuration structure definition in COUNTER mode
 */
typedef struct {
    uint8_t CounterOption;	/* !< Specifies the Counter Option. 
    				   This parameter can be a value of 
    				   @ref TIM32_Capture_Mode_Option	*/
									  	
    uint8_t CountInputSelect;   /* !< Specifies counter input select 
				   This parameter can be a value of 
				   @ref TIM32_CounterInput_Option */
    uint8_t Reserved[2];
} TIM32_COUNTERTypeDef;

/**
 * @brief TIMER32 Match structure definition 
 */
typedef struct {
    uint8_t MatchChannel;	/* !< Specifies the Match channel.
                                    This parameter can be a value of 
                                    @ref TIM32_Match_Channel  */

    uint8_t IntOnMatch;	        /* !< Specifies the interrupt on match.
    			           - ENABLE: Enable this function.
    			           - DISABLE: Disable this function.   */
    			           
    uint8_t StopOnMatch;	/* !< Specifies Stop On match.
    			           - ENABLE: Enable this function.
    			           - DISABLE: Disable this function.   */
    			           
    uint8_t ResetOnMatch;	/* !< Specifies Reset On match.
    			           - ENABLE: Enable this function.
    			           - DISABLE: Disable this function.   */
    			           
    uint8_t ExtMatchOutputType;	/* !< Specifies External Match Output type.
                                   This parameter can be a value of 
                                   @ref TIM32_ExtMatch_Option  */
														   	
    uint8_t Reserved[3];	/* Reserved */
														   
    uint32_t MatchValue;	/* !< Specifies Match value  */
} TIM32_MATCHTypeDef;

/**
 * @brief TIMER32 Capture structure definition
 */
typedef struct {
    uint8_t CaptureChannel;	 /* !< Specifies Capture channel.
                                    This parameter can be a value of 
                                    @ref TIM32_Capture_Channel  */

    uint8_t RisingEdge;	         /* !< Specifies caputre on rising edge.
    			            - ENABLE: Enable rising edge.
    			            - DISABLE: Disable this function.    */
    			            
    uint8_t FallingEdge;	 /*  !< Specifies capture on falling edge.
    				    - ENABLE: Enable falling edge.
    				    - DISABLE: Disable this function.    */
    				    
    uint8_t IntOnCapture;	 /* !< Specifies Interrupt On capture.
    				    - ENABLE: Enable interrupt function.
    				    - DISABLE: Disable this function.    */
    				    
} TIM32_CAPTURETypeDef;

/**
 * @brief PWM structure definition 
 */
typedef struct {
    uint8_t MatchChannel;	/* !< Specifies the Match channel.
                                   This parameter can be a value of 
                                   @ref TIM32_PWM_OPT  */
											  	
    uint8_t IntOnMatch;		/* !<当匹配的时候是否处罚中断
    				   - ENABLE: Enable this function.
    				   - DISABLE: Disable this function.   	*/
    				   
    uint8_t StopOnMatch;	/* !< 当匹配的时候停止TC计数
    				   - ENABLE: Enable this function.
    				   - DISABLE: Disable this function.   	*/
    				   
    uint8_t ResetOnMatch;	/* !< 当匹配的时候复位TC值
    				   - ENABLE: Enable this function.
    				   - DISABLE: Disable this function.    	*/
    				   
    uint8_t ExtPWMOutput;	/* !< Specifies External PWM Output enable or not.
    				   - DISABLE:disable PWM output on Match ch x
    				   - ENABLE:Enable PWM output on Match ch x  */
    				   
    uint8_t Reserved[3];	/* Reserved  */
    uint32_t MatchValue;	/* !<匹配计数器的值  */
} TIM32_PWMTypeDef;

/**
 * @}
 */



/* Public Macros -------------------------------------------------------------- */
/** @defgroup TIM32_Public_Macros
 * @{
 */


/* brief Macro to determine if it is valid TIMER32 peripheral */
#define PARAM_TIM32x(n)	 ((((uint32_t *)n)==((uint32_t *)LPC_TMR32B0)) \
                         || (((uint32_t *)n)==((uint32_t *)LPC_TMR32B1)))


/** @defgroup TIM32_Match_Channel
  * @{
  */

#define TIM32_MATCH_CHANNEL0    0
#define TIM32_MATCH_CHANNEL1    1
#define TIM32_MATCH_CHANNEL2    2
#define TIM32_MATCH_CHANNEL3    3

#define PARAM_TIM32_MATCH_CHANNEL(ch)	((   ch  ==  TIM32_MATCH_CHANNEL0) ||\
                                        (    ch  ==  TIM32_MATCH_CHANNEL1) ||\
					(    ch  ==  TIM32_MATCH_CHANNEL2) ||\
					(    ch  ==  TIM32_MATCH_CHANNEL3))

/**
  * @}
  */


/** @defgroup TIM32_Capture_Channel
  * @{
  */

#define TIM32_CAPTURE_CHANNEL0	    0
#define TIM32_CAPTURE_CHANNEL1      1
#define TIM32_CAPTURE_CHANNEL2      2
#define TIM32_CAPTURE_CHANNEL3      3

#define PARAM_TIM32_CAPTURE_CHANNEL(ch)	((   ch  ==  TIM32_CAPTURE_CHANNEL0) ||\
                                        (    ch  ==  TIM32_CAPTURE_CHANNEL1) ||\
                                        (    ch  ==  TIM32_CAPTURE_CHANNEL2) ||\
                                        (    ch  ==  TIM32_CAPTURE_CHANNEL3))

/**
  * @}
  */


/** @defgroup TIM32_interrupt_type
  * @{
  */
#define    TIM32_INT_MR0     1<<0        /* interrupt for Match channel 0*/
#define    TIM32_INT_MR1     1<<1        /* interrupt for Match channel 1*/
#define    TIM32_INT_MR2     1<<2        /* interrupt for Match channel 2*/
#define    TIM32_INT_MR3     1<<3        /* interrupt for Match channel 3*/
#define    TIM32_INT_CR0     1<<4        /* interrupt for Capture channel 0*/
#define    TIM32_INT_CR1     1<<5        /* interrupt for Capture channel 1*/ 
#define    TIM32_INT_CR2     1<<6        /* interrupt for Capture channel 2*/ 
#define    TIM32_INT_CR3     1<<7        /* interrupt for Capture channel 3*/ 

#define PARAM_TIM32_INT_TYPE(TYPE)	((TYPE ==TIM32_INT_MR0)||\
					(TYPE ==TIM32_INT_MR1)||\
					(TYPE ==TIM32_INT_MR2)||\
					(TYPE ==TIM32_INT_MR3)||\
					(TYPE ==TIM32_INT_CR0)||\
					(TYPE ==TIM32_INT_CR1)||\
					(TYPE ==TIM32_INT_CR2)||\
					(TYPE ==TIM32_INT_CR3))


/**
  * @}
  */
  

/** @defgroup TIM32_mode 
  * @{
  */

#define    TIM32_MODE_TIMER            0	/* Timer mode */
#define    TIM32_MODE_COUNTER_RISING   1	/* Counter rising mode */
#define    TIM32_MODE_COUNTER_FALLING  2	/* Counter falling mode */
#define    TIM32_MODE_COUNTER_ANY      3	/* Counter on both edges */

#define PARAM_TIM32_MODE_OPT(MODE)	(( MODE	==TIM32_MODE_TIMER)|| \
					( MODE ==TIM32_MODE_COUNTER_RISING)|| \
					( MODE ==TIM32_MODE_COUNTER_FALLING)|| \
					( MODE ==TIM32_MODE_COUNTER_ANY))

/**
  * @}
  */
  

/** @defgroup TIM32_Prescale_Option
  * @{
  */

#define    TIM32_PRESCALE_TICKVAL  0	/* Prescale in absolute value */
#define    TIM32_PRESCALE_USVAL	   1	/* Prescale in microsecond value */

#define PARAM_TIM32_PRESCALE_OPT(OPT)	(( OPT == TIM32_PRESCALE_TICKVAL)|| \
					( OPT == TIM32_PRESCALE_USVAL))


/**
  * @}
  */
  

/** @defgroup TIM32_CounterInput_Option
  * @{
  */

#define    TIM32_COUNTER_INCAP0    0		/* CAPn.0 input pin for TIMERn */
#define    TIM32_COUNTER_INCAP1    1		/* CAPn.1 input pin for TIMERn */
#define    TIM32_COUNTER_INCAP2    2		/* CAPn.2 input pin for TIMERn */
#define    TIM32_COUNTER_INCAP3    3		/* CAPn.3 input pin for TIMERn */

#define PARAM_TIM32_COUNTER_INPUT_OPT(OPT)	(( OPT == TIM32_COUNTER_INCAP0)|| \
						( OPT == TIM32_COUNTER_INCAP1)|| \
						( OPT == TIM32_COUNTER_INCAP2)|| \
						( OPT == TIM32_COUNTER_INCAP3))


/**
  * @}
  */
  

/** @defgroup TIM32_ExtMatch_Option
  * @{
  */

#define  TIM32_EXTMATCH_NOTHING  0  /* Do nothing for ext. output pin if match*/
#define  TIM32_EXTMATCH_LOW      1  /* Force ext.output pin to low if match*/
#define  TIM32_EXTMATCH_HIGH     2  /* Force ext.output pin to high if match*/
#define  TIM32_EXTMATCH_TOGGLE   3  /* Toggle ext. output pin if match */

#define PARAM_TIM32_EXTMATCH_OPT(OPT)	(( OPT == TIM32_EXTMATCH_NOTHING)|| \
					( OPT == TIM32_EXTMATCH_LOW)|| \
					( OPT == TIM32_EXTMATCH_HIGH)|| \
					( OPT == TIM32_EXTMATCH_TOGGLE))


/**
  * @}
  */
  

/** @defgroup TIM32_Capture_Mode_Option
  * @{
  */

#define    TIM32_CAPTURE_NONE      0	/* No Capture */
#define    TIM32_CAPTURE_RISING    1	/* Rising capture mode */
#define    TIM32_CAPTURE_FALLING   2	/* Falling capture mode */
#define    TIM32_CAPTURE_ANY	   3	/* On both edges */

#define PARAM_TIM32_CAP_MODE_OPT(OPT)	(( OPT == TIM32_CAPTURE_NONE)|| \
					( OPT == TIM32_CAPTURE_RISING)|| \
					( OPT == TIM32_CAPTURE_FALLING)|| \
					( OPT == TIM32_CAPTURE_ANY))



/**
  * @}
  */
  

/** @defgroup TIM32_PWM_Option
  * @{
  */

typedef uint8_t  TIM32_PWM_OPT;

#define    TIM32_PWM_CH0   0      /* PWM output on Match channel 0*/
#define    TIM32_PWM_CH1   1      /* PWM output on Match channel 1*/
#define    TIM32_PWM_CH2   2      /* PWM output on Match channel 2*/
#define    TIM32_PWM_CH3   3      /* PWM output on Match channel 3*/
    
#define PARAM_TIM32_PWM_OPT(OPT)	(( OPT == TIM32_PWM_CH0)|| \
				        ( OPT == TIM32_PWM_CH1)|| \
				        ( OPT == TIM32_PWM_CH2)|| \
				        ( OPT == TIM32_PWM_CH3))
				        
/**
  * @}
  */


/**
 * @}
 */


/* Public Functions ----------------------------------------------------------- */
/** @defgroup TIM32_Public_Functions
 * @{
 */
 
uint32_t TIM32_ConverUSecToVal (uint32_t USec);
IntStatus TIM32_GetIntStatus(LPC_TMR_TypeDef *TIMx, uint8_t IntFlag);
uint32_t TIM32_GetIntStatusReg(LPC_TMR_TypeDef *TIMx);
void TIM32_ClearIntPendingBit(LPC_TMR_TypeDef *TIMx, uint8_t IntFlag);
void TIM32_DeInit(LPC_TMR_TypeDef *TIMx);
void TIM32_Init(LPC_TMR_TypeDef *TIMx, uint8_t TimerCounterMode, void *TIM32_InitStruct);
void TIM32_StructInit(uint8_t TimerCounterMode, void *TIM32_InitStruct);
void TIM32_ConfigMatch(LPC_TMR_TypeDef *TIMx, TIM32_MATCHTypeDef *TIM32_MatchConfigStruct);
void TIM32_ConfigCapture(LPC_TMR_TypeDef *TIMx, TIM32_CAPTURETypeDef *TIM32_CaptureConfigStruct);
void TIM32_ConfigPWM(LPC_TMR_TypeDef *TIMx, TIM32_PWMTypeDef *TIM32_PWMConfigStruct);
void TIM32_Cmd(LPC_TMR_TypeDef *TIMx, FunctionalState NewState);
void TIM32_ResetCounter(LPC_TMR_TypeDef *TIMx);
uint32_t TIM32_GetTCValue(LPC_TMR_TypeDef *TIMx);
uint32_t TIM32_GetCaptureValue(LPC_TMR_TypeDef *TIMx, uint8_t CaptureChannel);
void TIM32_ClearPWMOuptut(LPC_TMR_TypeDef *TIMx, uint8_t PWMChannel);
void TIM32_ClearAllPWM(LPC_TMR_TypeDef *TIMx);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __LPC12xx_TIMER32_H */

/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */



