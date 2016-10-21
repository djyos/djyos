// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名    ：flash.h
// 模块描述: 片内flash操作包括读写等
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 01/06.2015
// =============================================================================

#ifndef __FLASH__H__
#define __FLASH__H__

#define     IAP_STAT_CMD_SUCCESS                                0
#define     IAP_STAT_INVALID_COMMAND                            1
#define     IAP_STAT_SRC_ADDR_ERROR                             2
#define     IAP_STAT_DST_ADDR_ERROR                             3
#define     IAP_STAT_SRC_ADDR_NOT_MAPPED                        4
#define     IAP_STAT_DST_ADDR_NOT_MAPPED                        5
#define     IAP_STAT_COUNT_ERROR                                6
#define     IAP_STAT_INVALID_SECTOR                             7
#define     IAP_STAT_SECTOR_NOT_BLANK                           8
#define     IAP_STAT_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION    9
#define     IAP_STAT_COMPARE_ERROR                              10
#define     IAP_STAT_BUSY                                       11


#define     IAP_CMD_PrepareSecor        50      /* select sector */
#define     IAP_CMD_CopyRAMToFlash      51      /* copy data from ram to flash */
#define     IAP_CMD_EraseSecor          52      /* erase sector */
#define     IAP_CMD_BlankCheckSecor     53      /* check if sector is blank */
#define     IAP_CMD_ReadPartID          54      //read chip ID
#define     IAP_CMD_ReadBootVersion     55      //read BOOT version
#define     IAP_CMD_Compare             56      //compare
#define     IAP_CMD_ReinvokeISP         57      //reinvoke ISP command
#define     IAP_CMD_ReadUID             58      // read unique ID
#define     IAP_CMD_ErasePage           59      // erase page
#define     IAP_CMD_EraseInfoPage       60      // erase info page


u32 IAP_ReadParID (u32 * PartID);
u32 IAP_PrepareSector (u32 Start,  u32 End);
u32 IAP_EraseSector (u32 Start,  u32 End);
u32 IAP_BlankCheckSecor (u32 Start,  u32 End, u32 * NonBlank);
u32 IAP_CopyRAMToFlash (u32 Dst,  u32 Src, u32 Number);
void Flash_ReadData(u32 Addr, u32 *Buf, u32 Len);

#endif
