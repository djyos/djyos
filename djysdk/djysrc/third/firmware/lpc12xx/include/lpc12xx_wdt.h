/**************************************************************************//**
 * $Id: lpc12xx_wdt.h 381 2010-08-27 01:47:05Z cnh00813 $
 *
 * @file     lpc12xx_wdt.h
 * @brief    Contains all macro definitions and function prototypes
 *           support for WDT firmware library on LPC12xx.
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
/** @defgroup WDT
 * @ingroup LPC1200CMSIS_FwLib_Drivers 
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LPC12xx_WDT_H
#define __LPC12xx_WDT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include "LPC12xx.h"
#include "lpc_types.h"

/* Public Types ------------------------------------------------------------- */
/** @defgroup WDT_Public_Types
 * @{
 */

/**
 * @brief WDT Init structure definition 
 */
typedef struct {
    uint8_t ClkSel;	  /* !< Specifies the WDT clock source.
                              This parameter can be a value of 
                              @ref WDT_Clock_Selection  */
										  	
    uint8_t Mode;	  /* !< Specifies the WDT operation mode.
                              This parameter can be a value of 
                              @ref WDT_Mode  */
										  	
    uint8_t LockClk;      /* !< Specifies the WDT clock lock function.
    			      - 1: WDT clock is locked.
    			      - 0: WDT clock can be disable at any time. */
    			                       
    uint8_t LockDP;       /* !< Specifies the WDT Deep power-down mode disable 
                              or not.
    			      - 1: entry into deep power-down mode is blocked.
    			      - 0: deep power-down mode can be entered.      */
    			                       
    uint8_t LockWDEN;     /* !< Specifies the WDT WDEN lock or not.
    			      - 1: WDEN bit is locked.
    			      - 0: WDEN can be written by software any time 
    			      to enable/disable WDT. */  
    			                           
    uint8_t LockClkSel;   /* !< Specifies the WDT clock source lock or not.
    			       - 1: WDSEL bits are locked.
    			       - 0: WDSEL bits can be written by software 
    			       any time to change the clock source. */ 
    			                                
    uint8_t SetProtect;   /* !< Specifies the WDT update mode.
    			      - 1: WDTC can be changed only after the counter
    			      is below the value of WDWARNINT and WDWINDOW.
    			      - 0: the WDTC can be changed at any time. */
    			                                      
    uint8_t TimeOutOption;	/* !< Specifies the time out option of WDT.
                                    This parameter can be a value of 
                                    @ref WDT_TimeOut_Option  */
										  	
    uint32_t TimeOutValue;	/* !< Specifies the WDT time out value.  */
										  
    uint32_t WarningCompareValue;    /* !< Specifies the WDT warning interrupt 
                                         compare value.  Should be <=0x3FF  */

    uint32_t WindowValue;	/* !< Specifies the WDT warning interrupt 
                                    compare value.  Reset value: 0xFFFFFF  */
}WDT_InitTypeDef;

/**
 * @}
 */


/* Public Macros -------------------------------------------------------------- */
/** @defgroup WDT_Public_Macros
 * @{
 */ 


/** @defgroup WDT_Clock_Selection 
  * @{
  */
typedef uint8_t WDT_CLKSEL_OPT; 
#define WDT_CLKSEL_IRCOSC  ((uint8_t) 0x00)   /* Clock source:iRC osc.(default)*/
#define WDT_CLKSEL_WDOSC   ((uint8_t) 0x01)   /* Clock source:watchdog osc.  */

#define PARAM_WDT_CLKSEL_OPT(OPTION)   ((OPTION ==WDT_CLKSEL_IRCOSC)||\
                                       (OPTION ==WDT_CLKSEL_WDOSC))
/**
 * @}
 */

/** @defgroup WDT_Mode 
  * @{
  */
typedef uint8_t WDT_MODE_OPT; 
#define WDT_MODE_INT_ONLY  ((uint8_t) 0x00)  /* generate interrupt only */
#define WDT_MODE_RESET     ((uint8_t) 0x01)  /* generate interrupt & reset MCU*/

#define PARAM_WDT_MODE_OPT(OPTION)     ((OPTION ==WDT_MODE_INT_ONLY)||\
                                       (OPTION ==WDT_MODE_RESET))
/**
 * @}
 */


/** @defgroup WDT_TimeOut_Option 
  * @{
  */
typedef uint8_t  WDT_TIMEOUT_OPT;

#define WDT_TIMEOUT_TICKVAL  ((uint8_t) 0x00)  /* timeout in absolute value */
#define WDT_TIMEOUT_USVAL    ((uint8_t) 0x01)  /* timeout in microsecond value*/

#define PARAM_WDT_TIMEOUT_OPT(OPT)	(( OPT == WDT_TIMEOUT_TICKVAL)|| \
					( OPT == WDT_TIMEOUT_USVAL))

/**
 * @}
 */

/* Define divider index for microsecond ( us ) */
#define WDT_US_INDEX	((uint32_t)(1000000))
/* WDT Time out minimum value */
#define WDT_TIMEOUT_MIN	((uint32_t)(0xFF))
/* WDT Time out maximum value */
#define WDT_TIMEOUT_MAX	((uint32_t)(0x00FFFFFF))
/* WDT constant register default value */
#define WDT_WDTC_DEFAULTVALUE  ((uint32_t)(0x00FFFF))


/**
 * @}
 */

/* Public Functions ----------------------------------------------------------- */
/** @defgroup WDT_Public_Functions
 * @{
 */

void WDT_UpdateTimeOut ( uint32_t TimeOut);
void WDT_UpdateTimeOutClk ( uint32_t TimeOutClk);
uint8_t WDT_SetTimeOut (uint32_t TimeOut);
uint8_t WDT_SetTimeOutClk (uint32_t TimeOutClk);
void WDT_DeInit(void);
void WDT_Init (WDT_InitTypeDef* WDT_InitStruct);
void WDT_StructInit(WDT_InitTypeDef* WDT_InitStruct);
void WDT_Start(void);
void WDT_Stop(void);
void WDT_SetProtect(void);
void WDT_SetLockClk(void);
void WDT_SetLockDP(void);
void WDT_SetLockWDEN(void);
void WDT_SetLockClkSel(void);
void WDT_SetWarningIntValue(WDT_InitTypeDef* WDT_InitStruct);
void WDT_SetWindowValue(WDT_InitTypeDef* WDT_InitStruct);
uint32_t WDT_GetWDTV(void);
IntStatus WDT_GetIntStatus (void);
FlagStatus WDT_GetTimeOutFlag (void);
void WDT_ClrIntPendingBit (void);
void WDT_ClrTimeOutFlag (void);
void WDT_Feed (void);


/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __LPC12xx_WDT_H */

/**
 * @}
 */
 
/* --------------------------------- End Of File ------------------------------ */



