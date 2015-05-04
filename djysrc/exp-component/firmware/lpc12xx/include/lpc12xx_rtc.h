/**************************************************************************//**
 * $Id: lpc12xx_rtc.h 378 2010-08-26 08:27:13Z cnh20509 $
 *
 * @file     lpc12xx_rtc.h
 * @brief    Contains all macro definitions and function prototypes
 *           support for RTC firmware library on LPC12xx.
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
/** @defgroup RTC
 * @ingroup LPC1200CMSIS_FwLib_Drivers 
 * @{
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LPC12xx_RTC_H
#define __LPC12xx_RTC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include "LPC12xx.h"
#include "lpc_types.h"

/* Public Macros -------------------------------------------------------------- */
/** @defgroup RTC_Public_Macros
 * @{
 */
    
/** @defgroup RTC_clock_source 
  * @{
  */
  
#define RTC_CLKSRC_1HZ              (0x00)
#define RTC_CLKSRC_DELAYED1HZ       (0x01)
#define RTC_CLKSRC_1KHZ             (0x0A) 
#define RTC_CLKSRC_PCLK             (0x04)
#define PARAM_RTC_CLKSRC(CLKSRC) (((CLKSRC) == RTC_CLKSRC_1HZ) || \
                                  ((CLKSRC) == RTC_CLKSRC_DELAYED1HZ) || \
                                  ((CLKSRC) == RTC_CLKSRC_1KHZ) || \
                                  ((CLKSRC) == RTC_CLKSRC_PCLK))

/**
  * @}
  */

/**
  * @}
  */

                                      
/* Public Functions ----------------------------------------------------------- */
/** @defgroup RTC_Public_Functions
 * @{
 */
 
void RTC_ClearIntPendingBit(void);
void RTC_Cmd (FunctionalState NewState);
void RTC_DeInit (void);
uint32_t RTC_GetData (void);
IntStatus RTC_GetIntStatus (void);
uint32_t RTC_GetLoad (void);
IntStatus RTC_GetRawIntStatus (void);
FlagStatus RTC_GetStatus (void);
void RTC_Init (uint8_t RTC_CLKSRC, uint8_t CLKDiv);
void RTC_IntConfig(FunctionalState NewState);
void RTC_SetLoad (uint32_t LoadValue);
void RTC_SetMatch (uint32_t MatchValue);
                                  

/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __LPC12xx_RTC_H */

/* --------------------------------- End Of File ------------------------------ */
