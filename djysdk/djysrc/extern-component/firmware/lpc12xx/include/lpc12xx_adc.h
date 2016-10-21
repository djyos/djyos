/**************************************************************************//**
 * $Id: lpc12xx_adc.h 488 2010-09-10 08:36:04Z cnh20509 $
 *
 * @file     lpc12xx_adc.h
 * @brief    Contains all macro definitions and function prototypes
 *           support for ADC firmware library on LPC12xx.
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
/** @defgroup ADC
 * @ingroup LPC1200CMSIS_FwLib_Drivers 
 * @{
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LPC12xx_ADC_H
#define __LPC12xx_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include "LPC12xx.h"
#include "lpc_types.h"

/* Public Types ------------------------------------------------------------- */


/* Public Macros -------------------------------------------------------------- */
/** @defgroup ADC_Public_Macros
 * @{
 */
 
    
/** @defgroup ADC_channels 
  * @{
  */

#define ADC_CHANNEL_0      (0x0)
#define ADC_CHANNEL_1      (0x1)
#define ADC_CHANNEL_2      (0x2)
#define ADC_CHANNEL_3      (0x3)
#define ADC_CHANNEL_4      (0x4)
#define ADC_CHANNEL_5      (0x5)
#define ADC_CHANNEL_6      (0x6)
#define ADC_CHANNEL_7      (0x7)
#define PARAM_ADC_CHANNEL(CHANNEL)   (((CHANNEL) == ADC_CHANNEL_0) || ((CHANNEL) == ADC_CHANNEL_1) || \
                                      ((CHANNEL) == ADC_CHANNEL_2) || ((CHANNEL) == ADC_CHANNEL_3) || \
                                      ((CHANNEL) == ADC_CHANNEL_4) || ((CHANNEL) == ADC_CHANNEL_5) || \
                                      ((CHANNEL) == ADC_CHANNEL_6) || ((CHANNEL) == ADC_CHANNEL_7))                                   
                                      
/**
  * @}
  */  
  
/** @defgroup ADC_start_options
  * @{
  */

#define ADC_START_STOP_CONTINUOUS   ((uint32_t) 0x00000000)  /*!< Continuous mode */
#define ADC_START_NOW               ((uint32_t) 0x01000000)  /*!< Start conversion now */
#define ADC_START_ON_CT16B0_CAP0    ((uint32_t) 0x02000000)  /*!< Start conversion when the edge selected by 
                                                                  bit 27 occurs on PIO0_2/SSEL/CT16B0_CAP0. */
#define ADC_START_ON_CT32B0_CAP0    ((uint32_t) 0x03000000)  /*!< Start conversion when the edge selected by 
                                                                  bit 27 occurs on PIO1_5/DIR/CT32B0_CAP0. */
#define ADC_START_ON_CT32B0_MAT0    ((uint32_t) 0x04000000)  /*!< Start conversion when the edge selected by 
                                                                  bit 27 occurs on CT32B0_MAT0. */
#define ADC_START_ON_CT32B0_MAT1    ((uint32_t) 0x05000000)  /*!< Start conversion when the edge selected by 
                                                                  bit 27 occurs on CT32B0_MAT1. */
#define ADC_START_ON_CT16B0_MAT0    ((uint32_t) 0x06000000)  /*!< Start conversion when the edge selected by 
                                                                  bit 27 occurs on CT16B0_MAT0. */ 
#define ADC_START_ON_CT16B0_MAT1    ((uint32_t) 0x07000000)  /*!< Start conversion when the edge selected by                              
                                                                  bit 27 occurs on CT16B0_MAT1. */
#define PARAM_ADC_START(START)   (((START) == ADC_START_STOP_CONTINUOUS) || ((START) == ADC_START_NOW) || \
                                  ((START) == ADC_START_ON_CT16B0_CAP0) || ((START) == ADC_START_ON_CT32B0_CAP0) || \
                                  ((START) == ADC_START_ON_CT32B0_MAT0) || ((START) == ADC_START_ON_CT32B0_MAT1) || \
                                  ((START) == ADC_START_ON_CT16B0_MAT0) || ((START) == ADC_START_ON_CT16B0_MAT1))
                                      
/**
  * @}
  */
     
     
/** @defgroup ADC_edge_options
  * @{
  */

#define ADC_EDGE_RISING         ((uint32_t) (0xF7FFFFFF)) /*!<  Start conversion on a falling edge on the selected CAP/MAT signal. */
#define ADC_EDGE_FALLING        ((uint32_t) (0x08000000)) /*!<  Start conversion on a rising edge on the selected CAP/MAT signal. */
#define PARAM_ADC_EDGE(EDGE)    (((EDGE) == ADC_EDGE_RISING) || ((EDGE) == ADC_EDGE_FALLING))

/**
  * @}
  */     


/** @defgroup ADC_interrupt_type
  * @{
  */

#define ADC_INT_CHANNEL0        ADC_CHANNEL_0     /*!< Interrupt channel 0 */
#define ADC_INT_CHANNEL1        ADC_CHANNEL_1     /*!< Interrupt channel 1 */
#define ADC_INT_CHANNEL2        ADC_CHANNEL_2     /*!< Interrupt channel 2 */
#define ADC_INT_CHANNEL3        ADC_CHANNEL_3     /*!< Interrupt channel 3 */
#define ADC_INT_CHANNEL4        ADC_CHANNEL_4     /*!< Interrupt channel 4 */
#define ADC_INT_CHANNEL5        ADC_CHANNEL_5     /*!< Interrupt channel 5 */
#define ADC_INT_CHANNEL6        ADC_CHANNEL_6     /*!< Interrupt channel 6 */
#define ADC_INT_CHANNEL7        ADC_CHANNEL_7     /*!< Interrupt channel 7 */
#define ADC_INT_GLOBAL          (0x8)     /*!< Individual channel/global flag done generate an interrupt */
#define PARAM_ADC_INT(INT)   (((INT) == ADC_INT_CHANNEL0) || ((INT) == ADC_INT_CHANNEL1) || \
                                      ((INT) == ADC_INT_CHANNEL2) || ((INT) == ADC_INT_CHANNEL3) || \
                                      ((INT) == ADC_INT_CHANNEL4) || ((INT) == ADC_INT_CHANNEL5) || \
                                      ((INT) == ADC_INT_CHANNEL6) || ((INT) == ADC_INT_CHANNEL7) || \
                                      ((INT) == ADC_INT_GLOBAL))  
                           
/**
  * @}
  */
  

/** @defgroup ADC_flag_types
  * @{
  */

#define ADC_FLAG_OVERRUN        ((uint32_t) (0x40000000))   /*!< Overrun bit in burst mode */
#define ADC_FLAG_DONE           ((uint32_t) (0x80000000))   /*!< Done bit */
#define PARAM_ADC_FLAG(FLAG)    (((FLAG) == ADC_FLAG_OVERRUN) || ((FLAG) == ADC_FLAG_DONE))
                           
/**
  * @}
  */


/**
  * @}
  */ 

/* Public Functions ----------------------------------------------------------- */
/** @defgroup ADC_Public_Functions
 * @{
 */

void ADC_BurstCmd (FunctionalState NewState);
void ADC_ChannelCmd (uint32_t ADC_CHANNEL, FunctionalState NewState);
void ADC_DeInit(void);
void ADC_EdgeConfig(uint32_t ADC_EDGE);
uint32_t ADC_GetData (uint8_t ADC_CHANNEL);
FlagStatus ADC_GetStatus(uint8_t ADC_CHANNEL, uint32_t ADC_FLAG);
void ADC_Init(uint32_t ADC_ClockRate);
void ADC_IntConfig (uint16_t ADC_INT, FunctionalState NewState);
void ADC_PowerDownCmd (FunctionalState NewState);
void ADC_StartCmd (uint32_t ADC_START);

/**
 * @}
 */
 
#ifdef __cplusplus
}
#endif

#endif /* __LPC12xx_ADC_H */

/**
 * @}
 */
 
/* --------------------------------- End Of File ------------------------------ */
