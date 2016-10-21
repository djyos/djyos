/**************************************************************************//**
 * $Id: lpc12xx_pmu.h 583 2010-09-25 13:48:34Z cnh20446 $
 *
 * @file     lpc12xx_pmu.h
 * @brief    Contains all macro definitions and function prototypes
 *           support for pmu firmware library on LPC12xx.
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
/** @defgroup PMU
 * @ingroup LPC1200CMSIS_FwLib_Drivers 
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LPC12xx_PMU_H
#define __LPC12xx_PMU_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include "LPC12xx.h"
#include "lpc_types.h"

/* Public Macros -------------------------------------------------------------- */
/** @defgroup PMU_Public_Macros
 * @{
 */


/** @defgroup PMU_sleep_mode
  * @{
  */

#define PMU_SLEEPMODE_SLEEP           0
#define PMU_SLEEPMODE_DEEPSLEEP      1
#define PARAM_PMU_SLEEPMODE(SLEEPMODE)   (((SLEEPMODE) == PMU_SLEEPMODE_SLEEP ) || \
										((SLEEPMODE) == PMU_SLEEPMODE_DEEPSLEEP ))
/**
  * @}
  */ 

/** @defgroup PMU_sleep_configuration
  * @{
  */

#define PMU_PDSLEEPCFG_IRCOUT		(1<<0)
#define PMU_PDSLEEPCFG_IRC			(1<<1)
#define PMU_PDSLEEPCFG_FLASH		(1<<2)
#define PMU_PDSLEEPCFG_BOD			(1<<3)
#define PMU_PDSLEEPCFG_ADC			(1<<4)
#define PMU_PDSLEEPCFG_SYSOSC		(1<<5)
#define PMU_PDSLEEPCFG_WDTOSC		(1<<6)
#define PMU_PDSLEEPCFG_SYSPLL		(1<<7)
#define PMU_PDSLEEPCFG_USBPLL       (1<<8)
#define PMU_PDSLEEPCFG_MAINREGUL	(1<<9)
#define PMU_PDSLEEPCFG_USBPHY		(1<<10)
#define PMU_PDSLEEPCFG_LPREGUL		(1<<12)
#define PMU_PDSLEEPCFG_COMP			(1<<15)
#define PARAM_PMU_PDSLEEPCFG(PMU_PDSLEEPCFG)   (((PMU_PDSLEEPCFG) & (uint32_t)0x97FF) != 0x00)

/**
  * @}
  */ 

/** @defgroup PMU_wakeup_pin
  * @{
  */

#define PMU_WAKEUPPIN_PIO0_0		(1<<0)
#define PMU_WAKEUPPIN_PIO0_1		(1<<1)
#define PMU_WAKEUPPIN_PIO0_2		(1<<2)
#define PMU_WAKEUPPIN_PIO0_3		(1<<3)
#define PMU_WAKEUPPIN_PIO0_4		(1<<4)
#define PMU_WAKEUPPIN_PIO0_5		(1<<5)
#define PMU_WAKEUPPIN_PIO0_6		(1<<6)
#define PMU_WAKEUPPIN_PIO0_7		(1<<7)
#define PMU_WAKEUPPIN_PIO0_8		(1<<8)
#define PMU_WAKEUPPIN_PIO0_9		(1<<9)
#define PMU_WAKEUPPIN_PIO0_10		(1<<10)
#define PMU_WAKEUPPIN_PIO0_11		(1<<11)
#define PARAM_PMU_WAKEUPPIN(PMU_WAKEUPPIN)   (((PMU_WAKEUPPIN) & (uint32_t)0xFFFFF000) == 0x00)

/**
  * @}
  */ 

/** @defgroup PMU_general_purpose_register 
  * @{
  */

#define PMU_GPREG_0			0
#define PMU_GPREG_1			1
#define PMU_GPREG_2			2
#define PMU_GPREG_3			3
#define PMU_GPREG_4			4
#define PARAM_PMU_GPREG(PMU_GPREG) 	(((PMU_GPREG) == PMU_GPREG_0) || \
									((PMU_GPREG) == PMU_GPREG_1) || \
									((PMU_GPREG) == PMU_GPREG_2) || \
									((PMU_GPREG) == PMU_GPREG_3) || \
									((PMU_GPREG) == PMU_GPREG_4))

/**
  * @}
  */ 

/** @defgroup PMU_wakeup_edge 
  * @{
  */

#define PMU_WAKEUPEDGE_FALLINGEDGE		0
#define PMU_WAKEUPEDGE_RISINGEDGE		1
#define PARAM_PMU_WAKEUPEDGE(PMU_WAKEUPEDGE) 	(((PMU_WAKEUPEDGE) == PMU_WAKEUPEDGE_FALLINGEDGE) || \
											((PMU_WAKEUPEDGE) == PMU_WAKEUPEDGE_RISINGEDGE))

/**
  * @}
  */ 

/** @defgroup PMU_power_control_register 
  * @{
  */

#define PMU_PCON_DPDEN				(1<<1)	/*!< Deep power down mode */
#define PMU_PCON_SLEEPFLAG			(1<<8)	/*!< Sleep/deepsleep or Deep powerdown flag */
#define PMU_PCON_DPDFLAG			(1<<11)	/*!< Deep powerdown flag */
#define PARAM_PMU_PCON(PMU_PCON) 	(((PMU_PCON) == PMU_PCON_DPDEN) || \
										((PMU_PCON) == PMU_PCON_DPDFLAG) || \
										((PMU_PCON) == PMU_PCON_DPDEN))

/**
 * @}
 */

/** @defgroup PMU_system_control_register 
  * @{
  */

#define NVIC_LP_SEVONPEND  		 	(1<<4)
#define NVIC_LP_SLEEPDEEP   		(1<<2)
#define NVIC_LP_SLEEPONEXIT 		(1<<1)

/**
 * @}
 */

/** @defgroup PMU_misc_definition 
  * @{
  */
#define PMU_STARTLOGIC_NUMBER		12		/**< PIO0_0 to PIO0_11 */
#define PMU_WAKESOURCE_NUMBER		32

/**
  * @}
  */

/**
 * @}
 */
 
/* Public Functions ----------------------------------------------------------- */
/** @defgroup PMU_Public_Functions
 * @{
 */

void PMU_Init( uint32_t PMU_WAKEUPPIN, uint8_t PMU_WAKEUPEDGE );
void PMU_Sleep( uint32_t PMU_SLEEPMODE, uint32_t PMU_PDSLEEPCFG );
void PMU_DeepPowerDown( void );
void PMU_ResetStartLogic(uint32_t Pin_x);
uint32_t PMU_ReadStartLogic(void);
uint32_t PMU_ReadPCON( uint32_t PMU_PCON );
void PMU_ClearDPDFLAG( void );
void PMU_ClearSLEEPFLAG( void );
void PMU_SetGPREG(uint32_t GPREG, uint32_t Data);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __LPC12xx_PMU_H */

/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */



