//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
// Copyright (c) 2014 著作权由深圳鹏瑞软件有限公司所有。著作权人保留一切权利。
//
// 这份授权条款，在使用者符合下列条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------
// 文件名     ：
// 模块描述:	片内flash操作
// 模块版本:
// 创建人员: HM
// 创建时间:
// =============================================================================

#include "stdint.h"
#include "stddef.h"
#include "os.h"
#include "cpu_peri.h"
#include "flashd.h"
#include "flashdrv.h"
#include "mpu.h"
#include "int.h"

#define CN_FLASH_START_ADDRESS   (0x00400000)
#define CN_FLASH_SIZE			 (0x200000)		//FLASH大小为2Mbytes
#define CN_APP_START_ADDRESS	 (0x00500000)
#define CN_APP_END_ADDRESS		 (CN_APP_START_ADDRESS + CN_FLASH_SIZE/2)

#define MPU_APP_FLASH_REGION	 (12)
#define MPU_APP_START_ADDRESS    CN_APP_START_ADDRESS
#define MPU_APP_END_ADDRESS      CN_APP_END_ADDRESS

// ============================================================================
// 功能：使能和禁能APP段的cache功能，升级APP时，该段地址不能开cache
// 参数：
// 返回：
// ============================================================================
void Flash_CacheMpuEnable(u8 IsEnable)
{
	uint32_t dwRegionBaseAddr;
	uint32_t dwRegionAttr;

	Int_CutTrunk();

	/* Clean up data and instruction buffer */
	__DSB();
	__ISB();

	if(!IsEnable)
	{
		dwRegionBaseAddr =
			MPU_APP_START_ADDRESS |
			MPU_REGION_VALID |
			MPU_APP_FLASH_REGION;      //12

		//在升级程序时，可对flash 1M的位置读写
		dwRegionAttr =
			MPU_AP_FULL_ACCESS |
			INNER_OUTER_NORMAL_NOCACHE_TYPE( NON_SHAREABLE ) |
			MPU_CalMPURegionSize(IFLASH_END_ADDRESS - IFLASH_START_ADDRESS) |
			MPU_REGION_ENABLE;

		MPU_SetRegion(dwRegionBaseAddr,dwRegionAttr);
	}
	else
	{
		dwRegionBaseAddr =
				MPU_APP_START_ADDRESS |
				MPU_REGION_VALID |
				MPU_APP_FLASH_REGION;      //12

		dwRegionAttr =
			MPU_AP_READONLY |
			INNER_NORMAL_WB_NWA_TYPE( NON_SHAREABLE ) |
			MPU_CalMPURegionSize(IFLASH_END_ADDRESS - IFLASH_START_ADDRESS) |
			MPU_REGION_ENABLE;

		MPU_UpdateRegions(MPU_APP_FLASH_REGION, dwRegionBaseAddr, dwRegionAttr);
	}

	/* Clean up data and instruction buffer to make the new region taking
	   effect at once */
	__DSB();
	__ISB();

	Int_ContactTrunk();
}
// =============================================================================
// 功能: 写数据，只允许写片内后面512k字节
// 参数：参数分别是地址，缓冲区指针，字节数
// 返回：0表示无错误，否则出错
// =============================================================================
u32 Flash_Write(u32 addr, u8 *buf, u32 nbytes)
{
	u32 err;
	if( (addr < CN_APP_START_ADDRESS) ||
			(addr + nbytes > CN_APP_END_ADDRESS)
			|| (buf == NULL) )
	{
		return 0;
	}

	//写flash，因为设备是用电池供电，所以用电压要求最低的字节编程
	err = FLASHD_Write(addr,buf,nbytes);
	if(err == 0)
		return nbytes;
	return err;
}


// =============================================================================
// 功能: 写之前需要解锁芯片
// 参数：无
// 返回：无
// =============================================================================
void Flash_Unlock(void)
{
	u32 start_addr = CN_APP_START_ADDRESS;
	u32 end_addr = CN_APP_START_ADDRESS + CN_FLASH_SIZE/2 - 1;

	if(FLASHD_IsLocked(start_addr,end_addr) != 0)
	{
		FLASHD_Unlock(start_addr,end_addr,NULL,NULL);
	}
}
// =============================================================================
// 功能: 写完之后，需要锁住芯片
// 参数：无
// 返回：无
// =============================================================================
void Flash_Lock(void)
{
	u32 start_addr = CN_APP_START_ADDRESS;
	u32 end_addr = CN_APP_START_ADDRESS + CN_FLASH_SIZE/2 - 1;

	FLASHD_Lock(start_addr,end_addr,NULL,NULL);
}

// =============================================================================
// 功能: 读数据，
// 参数：参数分别是地址，缓冲区指针，字节数
// 返回：实际读到的字节数
// =============================================================================
u32 Flash_Read(u32 addr, u8 *buf, u32 nbytes)
{
	u32 loop;

	if( (addr < CN_APP_START_ADDRESS) ||
			(addr + nbytes > CN_APP_END_ADDRESS)
			|| (buf == NULL) )
	{
		return 0;
	}

	for(loop = 0; loop < nbytes; loop++)
	{
		buf[loop] = *((u8 *)(addr + loop));
	}

	return loop;
}

// =============================================================================
// 功能:擦除片内flash的前1024M字节
// 参数：无
// 返回：无
// =============================================================================
void Flash_Erase(void)
{
	u32 sector_addr;
	u32 start_addr,end_addr;

	u32 sector_length = 128*1024;		//每个sector是128k
	start_addr = CN_APP_START_ADDRESS;
	end_addr = CN_APP_START_ADDRESS + CN_FLASH_SIZE/2;

	for(sector_addr = start_addr; sector_addr < end_addr; )
	{
		FLASHD_EraseSector(sector_addr);
		sector_addr += sector_length;
	}
}



