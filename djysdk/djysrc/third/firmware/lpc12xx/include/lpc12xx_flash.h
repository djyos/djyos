/**************************************************************************//**
 * $Id: lpc12xx_flash.h 386 2010-08-27 03:15:29Z cnh20509 $
 *
 * @file     lpc12xx_flash.h
 * @brief    Contains all macro definitions and function prototypes
 *           support for FLASH firmware library on LPC12xx.
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
/** @defgroup FLASH
 * @ingroup LPC1200CMSIS_FwLib_Drivers 
 * @{
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LPC12xx_FLASH_H
#define __LPC12xx_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include "LPC12xx.h"

/* Public Types ------------------------------------------------------------- */
/** @defgroup FLASH_Public_Types
 * @{
 */
 
typedef unsigned int IAP_U32;
typedef unsigned int IAP_Status;

/**
 * @}
 */
 

/* Public Macros -------------------------------------------------------------- */
/** @defgroup FLASH_Public_Macros
 * @{
 */
 
/** @defgroup IAP_Command 
  * @{
  */
  
#define		IAP_CMD_PrepareSecor	    50		/* select sector */
#define		IAP_CMD_CopyRAMToFlash	    51		/* copy data from ram to flash */
#define		IAP_CMD_EraseSecor		    52		/* erase sector */
#define		IAP_CMD_BlankCheckSecor	    53		/* check if sector is blank */
#define		IAP_CMD_ReadPartID		    54		//read chip ID
#define		IAP_CMD_ReadBootVersion	    55		//read BOOT version
#define		IAP_CMD_Compare			    56		//compare
#define		IAP_CMD_ReinvokeISP		    57		//reinvoke ISP command
#define     IAP_CMD_ReadUID             58      // read unique ID
#define     IAP_CMD_ErasePage           59      // erase page
#define     IAP_CMD_EraseInfoPage       60      // erase info page

/**
  * @}
  */ 

/** @defgroup IAP_Status_Codes 
  * @{
  */
    
#define     IAP_STAT_CMD_SUCCESS 								0
#define     IAP_STAT_INVALID_COMMAND 							1
#define     IAP_STAT_SRC_ADDR_ERROR 							2
#define     IAP_STAT_DST_ADDR_ERROR 							3
#define     IAP_STAT_SRC_ADDR_NOT_MAPPED 						4
#define     IAP_STAT_DST_ADDR_NOT_MAPPED 						5
#define     IAP_STAT_COUNT_ERROR 								6
#define     IAP_STAT_INVALID_SECTOR 							7
#define     IAP_STAT_SECTOR_NOT_BLANK							8
#define     IAP_STAT_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION 	9
#define     IAP_STAT_COMPARE_ERROR 							    10
#define     IAP_STAT_BUSY 									    11

/**
  * @}
  */
  
/**
  * @}
  */ 
  
/* Public Functions ----------------------------------------------------------- */
/** @defgroup FLASH_Public_Functions
 * @{
 */  
 
IAP_Status IAP_PrepareSector (IAP_U32 Start,  IAP_U32 End);
IAP_Status IAP_CopyRAMToFlash (IAP_U32 Dst,  IAP_U32 Src, IAP_U32 Number);
IAP_Status IAP_EraseSector (IAP_U32 Start,  IAP_U32 End);
IAP_Status IAP_BlankCheckSecor (IAP_U32 Start,  IAP_U32 End, IAP_U32 * NonBlank);
IAP_Status IAP_ReadParID (IAP_U32 * PartID);
IAP_Status IAP_ReadBootVersion (IAP_U32 * Version);
IAP_Status IAP_Compare (IAP_U32 Dst,  IAP_U32 Src, IAP_U32 Number, IAP_U32 *Offset);
void  IAP_ReinvokeISP(void);
IAP_Status IAP_ReadUID (IAP_U32 * UID);
IAP_Status IAP_ErasePage (IAP_U32 Start,  IAP_U32 End);
IAP_Status IAP_EraseInfoPage (IAP_U32 Start,  IAP_U32 End);	

/**
 * @}
 */
 
#ifdef __cplusplus
}
#endif


/**
 * @}
 */

#endif /* __LPC12xx_FLASH_H */

/* --------------------------------- End Of File ------------------------------ */
