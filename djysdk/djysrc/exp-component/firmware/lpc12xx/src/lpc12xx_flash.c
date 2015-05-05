/**************************************************************************//**
 * $Id: lpc12xx_flash.c 387 2010-08-27 03:15:52Z cnh20509 $
 *
 * @file     lpc12xx_flash.c
 * @brief    Contains all functions support for FLASH firmware library on LPC12xx.
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
/** @addtogroup FLASH
 * @{
 */

/* Includes ----------------------------------------------------------------- */
#include "lpc12xx_flash.h"

/* If this source file built with example, the LPC12xx FW library configuration
 * file in each example directory ("lpc12xx_libcfg.h") must be included,
 * otherwise the default FW library configuration file must be included instead
 */
#ifdef __BUILD_WITH_EXAMPLE__
#include "lpc12xx_libcfg.h"
#else
#include "lpc12xx_libcfg_default.h"
#endif /* __BUILD_WITH_EXAMPLE__ */

#ifdef _FLASH

/** @defgroup FLASH_Private_Macros
  * @{
  */

#define IAP_CLK SystemCoreClock
#define IAP_CLK_KHZ     (IAP_CLK / 1000)

#define IAP_LOCATION 0x1FFF1FF1
#define iap_entry(a, b)     ((void (*)())(IAP_LOCATION))(a, b)

/**
 * @}
 */
 
/** @defgroup FLASH_Private_Variables
  * @{
  */
  
static IAP_U32 command[5] = {0,0,0,0,0};
static IAP_U32 result[5]  = {0,0,0,0,0};

/**
 * @}
 */
 
/** @defgroup FLASH_Private_Functions
 * @{
 */
 
/**
  * @brief  Prepare sector(s) for erase/write operation.
  *
  * @param  Start: The start sector number.
  * @param  End  : The end sector number.
  *         It should be greater than or equal to start sector number.
  * @retval Status code of this operation, see @ref IAP_Status_Codes.
  *         This parameter can be one of the following values:
  *             @arg IAP_STAT_CMD_SUCCESS
  *             @arg IAP_STAT_INVALID_SECTOR    
  *             @arg IAP_STAT_BUSY  
  *
  * This command must be executed before executing "Copy RAM to flash" 
  * or "Erase Sector(s)" command.
  */ 
IAP_Status IAP_PrepareSector (IAP_U32 Start,  IAP_U32 End)
{
	command[0] = IAP_CMD_PrepareSecor;
	command[1] = Start;
	command[2] = End;
	iap_entry(command, result);

	return result[0];
}

/**
  * @brief  Program the flash program.
  *
  * @param  Dst: Destination flash address. Should be a 4 byte boundary.
  * @param  Src: Source RAM address. Should be a word boundary.   
  * @param  Number: Number of byte to be written. Must be a multiple of 4 bytes.
  * @retval Status code of this operation, see @ref IAP_Status_Codes.
  *         This parameter can be one of the following values:  
  *             @arg IAP_STAT_CMD_SUCCESS
  *             @arg IAP_STAT_SRC_ADDR_ERROR
  *             @arg IAP_STAT_DST_ADDR_ERROR
  *             @arg IAP_STAT_SRC_ADDR_NOT_MAPPED
  *             @arg IAP_STAT_DST_ADDR_NOT_MAPPED
  *             @arg IAP_STAT_COUNT_ERROR
  *             @arg IAP_STAT_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION  
  *             @arg IAP_STAT_BUSY   
  *
  * The affected sectors should be prepared first by calling 
  * "Prepare Sector for Write Operation" command. 
  * The boot sector can not be written by this command.
  */
IAP_Status IAP_CopyRAMToFlash (IAP_U32 Dst,  IAP_U32 Src, IAP_U32 Number)
{
	command[0] = IAP_CMD_CopyRAMToFlash;
	command[1] = Dst;
	command[2] = Src;
	command[3] = Number;
	command[4] = IAP_CLK_KHZ;	// Fcclk in KHz
	iap_entry(command, result);

	return result[0];
}

/**
  * @brief   Erase a sector or multiple sectors of on-chip flash memory..
  *
  * @param  Start: The start sector number.
  * @param  End  : The end sector number.
  *         It should be greater than or equal to start sector number.
  * @retval Status code of this operation, see @ref IAP_Status_Codes.
  *         This parameter can be one of the following values:
  *             @arg IAP_STAT_CMD_SUCCESS
  *             @arg IAP_STAT_INVALID_SECTOR  
  *             @arg IAP_STAT_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION     
  *             @arg IAP_STAT_BUSY  
  *
  * The boot sector can not be erased by this command. To erase a single 
  * sector use the same "Start" and "End" sector numbers.
  */ 
IAP_Status IAP_EraseSector (IAP_U32 Start,  IAP_U32 End)
{
	command[0] = IAP_CMD_EraseSecor;
	command[1] = Start;
	command[2] = End;
	command[3] = IAP_CLK_KHZ;   // IAP_CLK / 1000;
	iap_entry(command, result);

	return result[0];
}

/**
  * @brief  Blank check a sector or multiple sectors of on-chip flash memory.
  *
  * @param  Start: The start sector number.
  * @param  End  : The end sector number.
  *         It should be greater than or equal to start sector number.
  * @param  NonBlank: Pointer to buffer which contains the offset of the first 
  *         non blank word location if the Status Code is SECTOR_NOT_BLANK and 
  *         Contents of non blank word location.
  * @retval Status code of this operation, see @ref IAP_Status_Codes.
  *         This parameter can be one of the following values:
  *             @arg IAP_STAT_CMD_SUCCESS
  *             @arg IAP_STAT_INVALID_SECTOR  
  *             @arg IAP_STAT_SECTOR_NOT_BLANK     
  *             @arg IAP_STAT_BUSY  
  *
  * The boot sector can not be erased by this command. To erase a single 
  * sector use the same "Start" and "End" sector numbers.
  */
IAP_Status IAP_BlankCheckSecor (IAP_U32 Start,  IAP_U32 End, IAP_U32 * NonBlank)
{
	command[0] = IAP_CMD_BlankCheckSecor;
	command[1] = Start;
	command[2] = End;

	iap_entry(command, result);

	if (result[0] == IAP_STAT_SECTOR_NOT_BLANK)
	{
		*NonBlank = result[1];
		*(NonBlank+1) = result[2];
	}

	return result[0];
}

/**
  * @brief  Read the part identification number.
  *
  * @param  PartID: Pointer to one-word Part Identification Number
  * @retval Status code of this operation, see @ref IAP_Status_Codes.
  *         This parameter can be one of the following values:
  *             @arg IAP_STAT_CMD_SUCCESS 
  */
IAP_Status IAP_ReadParID (IAP_U32 * PartID)
{

	command[0] = IAP_CMD_ReadPartID;
	iap_entry(command, result);
	*PartID = result[1];

	return result[0];
}

/**
  * @brief  Read  the boot code version number.
  *
  * @param  Version: Pointer to one-word boot code version number in ASCII format. 
  *         It is to be interpreted as <byte1(Major)>.<byte0(Minor)>.
  * @retval Status code of this operation, see @ref IAP_Status_Codes.
  *         This parameter can be one of the following values:
  *             @arg IAP_STAT_CMD_SUCCESS 
  */
IAP_Status IAP_ReadBootVersion (IAP_U32 *Version)
{
	command[0] = IAP_CMD_ReadBootVersion;
	iap_entry(command, result);
    *Version = result[1] & 0xFFFF;

	return result[0];
}

/**
  * @brief  Compare the memory contents at two locations.
  *
  * @param  Dst: Starting flash or RAM address of data bytes to be compared. 
  *         This address should be a word boundary.
  * @param  Src: Starting flash or RAM address of data bytes to be compared. 
  *         This address should be a word boundary.
  * @param  Number: Number of bytes to be compared; should be a multiple of 4.
  * @param  Offset: Pointer to one-word offset of the first mismatch if the Status Code 
  *         is COMPARE_ERROR.
  * @retval Status code of this operation, see @ref IAP_Status_Codes.
  *         This parameter can be one of the following values:
  *             @arg IAP_STAT_CMD_SUCCESS
  *             @arg IAP_STAT_COMPARE_ERROR
  *             @arg IAP_STAT_COUNT_ERROR
  *             @arg IAP_STAT_SRC_ADDR_NOT_MAPPED
  *             @arg IAP_STAT_DST_ADDR_NOT_MAPPED
  *             @arg IAP_STAT_SRC_ADDR_NOT_MAPPED
  *             @arg IAP_STAT_DST_ADDR_NOT_MAPPED 
  *
  * The result may not be correct when the source or destination includes any 
  * of the first 512 bytes starting from address zero. The first 512 bytes can 
  * be re-mapped to RAM.
  */
IAP_Status IAP_Compare (IAP_U32 Dst,  IAP_U32 Src, IAP_U32 Number, IAP_U32 *Offset)
{
	command[0] = IAP_CMD_Compare;
	command[1] = Dst;
	command[2] = Src;
	command[3] = Number;
	iap_entry(command, result);

	if (result[0] == IAP_STAT_COMPARE_ERROR)
		*Offset = result[1];

	return result[0];
}

/**
  * @brief  Invoke the bootloader in ISP mode.
  *
  * @param  None
  * @retval None 
  */
void  IAP_ReinvokeISP(void)
{
	command[0] = IAP_CMD_ReinvokeISP;					
	iap_entry(command, result);	
}

/**
  * @brief  Read the unique ID.
  *
  * @param  UID: Pointer to a 4-word unique ID.
  * @retval Status code of this operation, see @ref IAP_Status_Codes.
  *         This parameter can be one of the following values:
  *             @arg IAP_STAT_CMD_SUCCESS 
  */
IAP_Status IAP_ReadUID (IAP_U32 * UID)
{
    command[0] = IAP_CMD_ReadUID;
    iap_entry(command, result);	

    *(UID + 0) = result[1];
    *(UID + 1) = result[2];
    *(UID + 2) = result[3];
    *(UID + 3) = result[4];

    return result[0];
}

/**
  * @brief  Erase page(s).
  *
  * @param  Start: The start page number.
  * @param  End  : The end page number.
  *         It should be greater than or equal to start sector number.
  * @retval Status code of this operation, see @ref IAP_Status_Codes.
  *         This parameter can be one of the following values:
  *             @arg IAP_STAT_CMD_SUCCESS
  *             @arg IAP_STAT_INVALID_SECTOR  
  *             @arg IAP_STAT_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION     
  *             @arg IAP_STAT_BUSY  
  *
  * To erase a single page use the same "Start" and "End" sector numbers.
  */ 
IAP_Status IAP_ErasePage (IAP_U32 Start,  IAP_U32 End)
{
	command[0] = IAP_CMD_ErasePage;
	command[1] = Start;
	command[2] = End;
	command[3] = IAP_CLK_KHZ;   // IAP_CLK / 1000;
	iap_entry(command, result);

	return result[0];
}

/**
  * @brief  Erase a page or multiple pages of the information block. .
  *
  * @param  Start: The start page number.
  * @param  End  : The end page number.
  *         It should be greater than or equal to start sector number.
  * @retval Status code of this operation, see @ref IAP_Status_Codes.
  *         This parameter can be one of the following values:
  *             @arg IAP_STAT_CMD_SUCCESS
  *             @arg IAP_STAT_INVALID_SECTOR  
  *             @arg IAP_STAT_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION     
  *             @arg IAP_STAT_BUSY  
  *
  * To erase a single page use the same "Start" and "End" sector numbers.
  */ 
IAP_Status IAP_EraseInfoPage (IAP_U32 Start,  IAP_U32 End)
{
	command[0] = IAP_CMD_EraseInfoPage;
	command[1] = Start;
	command[2] = End;
	command[3] = IAP_CLK_KHZ;   // IAP_CLK / 1000;
	iap_entry(command, result);

	return result[0];
}

/**
 * @}
 */
 
#endif // _FLASH 

/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */
