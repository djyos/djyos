// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名    ：flash.c
// 模块描述: 片内flash操作包括读写等
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 01/06.2015
// =============================================================================

#include "stdint.h"
#include "flash.h"

#define IAP_CLK 80000000
#define IAP_CLK_KHZ     (IAP_CLK / 1000)

#define IAP_LOCATION 0x1FFF1FF1
#define iap_entry(a, b)     ((void (*)())(IAP_LOCATION))(a, b)

// =============================================================================
// 功能：读取器件ID，读取的ID存储到参数指针地址位置
// 参数：PartID,器件ID存储地址
// 返回： IAP_STAT_CMD_SUCCESS
//		 IAP_STAT_INVALID_SECTOR
//		 IAP_STAT_BUSY
// =============================================================================
u32 IAP_ReadParID (u32 * PartID)
{
	u32 command[5]={0,0,0,0,0};
	u32 result[5]={0,0,0,0,0};

	command[0] = IAP_CMD_ReadPartID;
	iap_entry(command, result);
	*PartID = result[1];

	return result[0];
}

// =============================================================================
// 功能：判断指定的起始sector是否已经准备好可供擦除或写数据
// 参数：Start、End，起始块号
// 返回： IAP_STAT_CMD_SUCCESS
//		 IAP_STAT_INVALID_SECTOR
//		 IAP_STAT_BUSY
// =============================================================================
u32 IAP_PrepareSector (u32 Start,  u32 End)
{
	u32 command[5]={0,0,0,0,0};
	u32 result[5]={0,0,0,0,0};

	command[0] = IAP_CMD_PrepareSecor;
	command[1] = Start;
	command[2] = End;
	iap_entry(command, result);

	return result[0];
}

// =============================================================================
// 功能：擦除指定起始块号
// 参数：Start、End，起始块号
// 返回： IAP_STAT_CMD_SUCCESS
//		 IAP_STAT_INVALID_SECTOR
//		 IAP_STAT_BUSY
// =============================================================================
u32 IAP_EraseSector (u32 Start,u32 End)
{
	u32 command[5]={0,0,0,0,0};
	u32 result[5]={0,0,0,0,0};

	command[0] = IAP_CMD_EraseSecor;
	command[1] = Start;
	command[2] = End;
	command[3] = IAP_CLK_KHZ;   // IAP_CLK / 1000;
	iap_entry(command, result);

	return result[0];
}

// =============================================================================
// 功能：检查指定起始sector是否为空，一般会在擦除完时进行一次检查
// 参数：Start，End，起始扇区
//       NonBlank，第一个非空值的位置
// 返回：IAP_STAT_SECTOR_NOT_BLANK
//      IAP_STAT_CMD_SUCCESS
//		IAP_STAT_INVALID_SECTOR
//		IAP_STAT_BUSY
// =============================================================================
u32 IAP_BlankCheckSecor (u32 Start,  u32 End, u32 * NonBlank)
{
	u32 command[5]={0,0,0,0,0};
	u32 result[5]={0,0,0,0,0};

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

// =============================================================================
// 功能：将RAM中的数据写入到FLASH
// 参数：Dst，待写入数据字节的目标 Flash 地址。该地址的边界应为 256 字节。
//      Src，待读出数据字节的源 RAM 地址。该地址应以字为边界。
//      Number，待写入的字节数。应当为 256 | 512 | 1024 | 4096
// 返回： IAP_STAT_CMD_SUCCESS
//		IAP_STAT_SRC_ADDR_ERROR
//		STAT_DST_ADDR_ERROR
//		IAP_STAT_SRC_ADDR_NOT_MAPPED
//		IAP_STAT_DST_ADDR_NOT_MAPPED
//		IAP_STAT_COUNT_ERROR
//		IAP_STAT_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION
//		IAP_STAT_BUSY
// =============================================================================
u32 IAP_CopyRAMToFlash (u32 Dst,  u32 Src, u32 Number)
{
	u32 command[5]={0,0,0,0,0};
	u32 result[5]={0,0,0,0,0};

	command[0] = IAP_CMD_CopyRAMToFlash;
	command[1] = Dst;
	command[2] = Src;
	command[3] = Number;
	command[4] = IAP_CLK_KHZ;	// Fcclk in KHz
	iap_entry(command, result);

	return result[0];
}

// =============================================================================
// 功能：从flash中读数据
// 参数：Addr,地址
//      Buf,存储位置
//      Len,长度，四字节为单位
// 返回：无返回
// =============================================================================
void Flash_ReadData(u32 Addr, u32 *Buf, u32 Len)
{
	u32 i;

	for(i = 0; i < Len; i++)
	{
		Buf[i] = *(u32*)(Addr + i*4);
	}
}





