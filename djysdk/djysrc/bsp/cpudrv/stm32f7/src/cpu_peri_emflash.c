//-----------------------------------------------------------------------------
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

#include <string.h>
#include <stdlib.h>
#include <driver.h>
#include <driver/flash/flash.h>
#include <cpu_peri.h>
#include <stm32f7xx_hal_flash.h>
#include <int.h>

static struct EmbdFlashDescr{
	u16     BytesPerPage;      		     // 页中包含的字节数
	u16     PagesPerSmallSect;      	 // small sector的页数
	u16 	PagesPerLargeSect;           // large sector的页数
	u16     PagesPerNormalSect;          // Normal sector的页数
	u16     SmallSectorsPerPlane;        // 每plane的small sector数
	u16     LargeSectorsPerPlane;        // 每plane的large sector数
	u16     NormalSectorsPerPlane;       // 每plane的normal sector数
	u16 	Planes;                      // plane数
	u32     MappedStAddr;
} *s_ptEmbdFlash;

extern u32 gc_ptIbootSize;
extern u32 gc_ptFlashOffset;
extern u32 gc_ptFlashRange;
//-----------------------------------------------------------------------------
//功能: 获取内置FLASH的信息
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static s32 EmFlash_Init(struct EmbdFlashDescr *Description)
{
	Description->BytesPerPage = 512;
	Description->PagesPerSmallSect = 64;
	Description->PagesPerLargeSect = 256;
	Description->PagesPerNormalSect = 512;
	Description->SmallSectorsPerPlane = 4;
	Description->LargeSectorsPerPlane = 1;
	Description->NormalSectorsPerPlane = 7; // STM32F767
	Description->Planes = 1;
	Description->MappedStAddr = gc_ptFlashOffset;
	return (0);
}

//-----------------------------------------------------------------------------
//功能: 获取内置FLASH的信息
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static s32 Flash_GetDescr(struct EmFlashDescr *Description)
{
	Description->BytesPerPage = s_ptEmbdFlash->BytesPerPage;
	Description->TotalPages = (s_ptEmbdFlash->PagesPerSmallSect *
						s_ptEmbdFlash->SmallSectorsPerPlane +
						s_ptEmbdFlash->PagesPerLargeSect *
						s_ptEmbdFlash->LargeSectorsPerPlane +
						s_ptEmbdFlash->PagesPerNormalSect *
						s_ptEmbdFlash->NormalSectorsPerPlane) *
					   s_ptEmbdFlash->Planes;
	Description->ReservedPages = gc_ptIbootSize / s_ptEmbdFlash->BytesPerPage;
	Description->MappedStAddr = s_ptEmbdFlash->MappedStAddr;
	return (0);
}
//-----------------------------------------------------------------------------
//功能: 擦除块
//参数: BlkNo -- 块号;
//返回: "0" -- 成功;"-1" -- 失败;
//备注:
//-----------------------------------------------------------------------------
static s32 Flash_SectorEarse(u32 SectorNo)
{
	s32 Ret;
	u32 SECTORError=0;//保存出错类型信息
	static FLASH_EraseInitTypeDef EraseInitStruct;

	HAL_FLASH_Unlock();

	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector        = SectorNo;
	EraseInitStruct.NbSectors     = 1;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
	 	Ret=-1;
	else
		Ret=0;
	HAL_FLASH_Lock();
	return Ret;
}
//-----------------------------------------------------------------------------
//功能: 写某页
//参数: PageNo -- 页号;
//      Data -- 写缓冲;
//      Flags -- 写操作控制位;
//返回: ">0" -- 正常，写入的字节数;
//      "-1" -- 参数错误;
//      "-2" -- 写失败;
//备注:
//-----------------------------------------------------------------------------
static s32 Flash_PageProgram(u32 Page, u8 *Data, u32 Flags)
{
	u32 Ret,i;
	u32 *pData = (u32*)Data;

	u32 Addr = Page * s_ptEmbdFlash->BytesPerPage + s_ptEmbdFlash->MappedStAddr;

	if(!Data)
		return (-1);

	HAL_FLASH_Unlock();

	for(i = 0; i < s_ptEmbdFlash->BytesPerPage;)
	{
    	if(*(u32*)Addr != *pData)
    	{
    		Ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Addr,*pData);
    		if(Ret != HAL_OK)
    			break;
    	}
		pData++;
		i += 4;
		Addr += 4;
	}

	HAL_FLASH_Lock();

	return i;
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static s32 Flash_PageRead(u32 Page, u8 *Data, u32 Flags)
{
	u32 Addr = Page * s_ptEmbdFlash->BytesPerPage + s_ptEmbdFlash->MappedStAddr;

	if(!Data)
		return (-1);

	memcpy(Data, (u8*)Addr, s_ptEmbdFlash->BytesPerPage);
	return (s_ptEmbdFlash->BytesPerPage);
}

//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static s32 Flash_PageToSector(u32 PageNo, u32 *Remains, u32 *SectorNo)
{
	u32 PagesLeft, PagesDone;
	u32 i;
	u32 Sector;

	if((!Remains) || (!SectorNo))
		return (-1);

	Sector = 0;
	PagesDone = 0;
	PagesLeft = s_ptEmbdFlash->PagesPerSmallSect -
			   (PageNo % s_ptEmbdFlash->PagesPerSmallSect);
	for(i = 1; i <= s_ptEmbdFlash->SmallSectorsPerPlane; i++)
	{
		if(PageNo < (PagesDone + s_ptEmbdFlash->PagesPerSmallSect * i))
			goto DONE;
		Sector++;
	}

	PagesDone += s_ptEmbdFlash->SmallSectorsPerPlane *
			s_ptEmbdFlash->PagesPerSmallSect;
	PagesLeft = s_ptEmbdFlash->PagesPerLargeSect -
				   (PageNo % s_ptEmbdFlash->PagesPerLargeSect);
	for(i = 1; i <= s_ptEmbdFlash->LargeSectorsPerPlane; i++)
	{
		if(PageNo < (PagesDone + s_ptEmbdFlash->PagesPerLargeSect * i))
			goto DONE;
		Sector++;
	}

	PagesDone += s_ptEmbdFlash->LargeSectorsPerPlane *
			s_ptEmbdFlash->PagesPerLargeSect;
	PagesLeft = s_ptEmbdFlash->PagesPerNormalSect -
				   (PageNo % s_ptEmbdFlash->PagesPerNormalSect);
	for(i = 1; i <= s_ptEmbdFlash->NormalSectorsPerPlane; i++)
	{
		if(PageNo < (PagesDone + s_ptEmbdFlash->PagesPerNormalSect * i))
			goto DONE;
		Sector++;
	}

	return (-1);

DONE:
	*SectorNo = Sector; // 从sector零计
	*Remains = PagesLeft; //
	return (0);
}
//-----------------------------------------------------------------------------
//功能:
//参数: ResPages：保留页数
//返回:
//备注:
//-----------------------------------------------------------------------------

s32 ModuleInstall_EmbededFlash(const char *ChipName, u32 Flags, u16 ResPages)
{
	u32 Len;
    struct FlashChip *Chip;
    struct EmFlashDescr FlashDescr;
	struct MutexLCB *FlashLock;
	u8 *Buf;
	s32 Ret = 0;

	if (!ChipName)
		return (-1);

	if(s_ptEmbdFlash)
		return (-4); // 设备已注册

	s_ptEmbdFlash = malloc(sizeof(*s_ptEmbdFlash));
	if(!s_ptEmbdFlash)
		return (-1);

	EmFlash_Init(s_ptEmbdFlash);
    Flash_GetDescr(&FlashDescr);// 获取FLASH信息
    if(ResPages > FlashDescr.TotalPages)
	{
    	Ret = -1;
		goto FAILURE;
    }

    FlashDescr.ReservedPages += ResPages;
    Len = strlen (ChipName) + 1;
    Chip = (struct FlashChip*)malloc(sizeof(struct FlashChip) + Len);
	if (NULL == Chip)
	{
		TraceDrv(FLASH_TRACE_ERROR, "out of memory!\r\n");
		Ret = -2;
		goto FAILURE;
	}

	memset(Chip, 0x00, sizeof(*Chip));
	Chip->Type                    = F_ALIEN;
	Chip->Descr.Embd              = FlashDescr;
	Chip->Ops.ErsBlk              = Flash_SectorEarse;
	Chip->Ops.WrPage              = Flash_PageProgram;
	Chip->Ops.RdPage              = Flash_PageRead;
	Chip->Ops.PageToBlk			  = Flash_PageToSector;
	strcpy(Chip->Name, ChipName); // 设备名
	if(Flags & FLASH_BUFFERED)
	{
		Buf = (u8*)malloc(s_ptEmbdFlash->BytesPerPage); // NAND底层缓冲
		if(!Buf)
		{
			TraceDrv(FLASH_TRACE_ERROR, "out of memory!\r\n");
			Ret = -2;
			goto FAILURE;
		}

		FlashLock = Lock_MutexCreate("Embedded Flash Lock");
		if(!FlashLock)
		{
			Ret = -3;
			goto FAILURE;
		}

		Chip->Buf = Buf;
		Chip->Lock =(void*)FlashLock;
	}

    Driver_DeviceCreate(NULL, Chip->Name, NULL, NULL, NULL, NULL, NULL, NULL, (ptu32_t)Chip); // 设备接入"/dev"
    if(Flags & FLASH_ERASE_ALL)
		EarseWholeChip(Chip);

FAILURE:
	if(Ret)
	{
		if(s_ptEmbdFlash)
			free(s_ptEmbdFlash);
		if(FlashLock)
			Lock_MutexDelete(FlashLock);
		if(Buf)
			free(Buf);
		if(Chip)
			free(Chip);
	}
	return (Ret);
}
