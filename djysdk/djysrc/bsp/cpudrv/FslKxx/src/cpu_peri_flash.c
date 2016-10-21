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

#include <stdlib.h>
#include <driver.h>
#include <driver/flash/flash.h>
#include <cpu_peri.h>
#include <int.h>

static struct EmbdFlashDescr *sp_tFlashDesrc;

void Cache_Invalidate(void)
{

//  if (cache == CODE_CACHE)
//  {
      /* Cache Set Command: set command bits in CCR */
      /* set invalidate way 1 and invalidate way 0 bits */
      LMEM->PCCCR = LMEM_PCCCR_INVW0_MASK | LMEM_PCCCR_INVW1_MASK;

      /* set ccr[go] bit to initiate command to invalidate cache */
      LMEM->PCCCR |= LMEM_PCCCR_GO_MASK;

      /* wait until the ccr[go] bit clears to indicate command complete */
      while(((LMEM->PCCCR & LMEM_PCCCR_GO_MASK)>>LMEM_PCCCR_GO_SHIFT)== 0x1)
      {};

//  }else if (cache == SYS_CACHE)
//  {
      /* Cache Set Command: set command bits in CCR */
      /* set invalidate way 1 and invalidate way 0 bits */
      LMEM->PSCCR = LMEM_PSCCR_INVW0_MASK | LMEM_PSCCR_INVW1_MASK;

      /* set ccr[go] bit to initiate command to invalidate cache */
      LMEM->PSCCR |= LMEM_PSCCR_GO_MASK;

      /* wait until the ccr[go] bit clears to indicate command complete */
      while(((LMEM->PSCCR & LMEM_PSCCR_GO_MASK)>>LMEM_PSCCR_GO_SHIFT)== 0x1)
      {};

//  } else
//  {
//     return CACHE_INVALID_PARAM;
//  }

//  return CACHE_OK;

}
static void Kinetis_FlashInit(void)
{
//	Cache_Disable();
	//检查Flash访问错误
	if (FTFE->FSTAT & FTFE_FSTAT_ACCERR_MASK)
	{
	FTFE->FSTAT |= FTFE_FSTAT_ACCERR_MASK;//清除错误标志
	}
	//检查保护错误
	else if (FTFE->FSTAT & FTFE_FSTAT_FPVIOL_MASK)
	{
	FTFE->FSTAT |= FTFE_FSTAT_FPVIOL_MASK;
	}
	else if (FTFE->FSTAT & FTFE_FSTAT_RDCOLERR_MASK)
	{
	//检查读冲突错误
	FTFE->FSTAT |= FTFE_FSTAT_RDCOLERR_MASK;
	} /* EndIf */

	//禁用Flash模块的数据缓存
	FMC->PFB01CR &= ~FMC_PFB01CR_B01DCE_MASK;
	FMC->PFB23CR &= ~FMC_PFB23CR_B23DCE_MASK;

	while(!(FTFE->FSTAT & FTFE_FSTAT_CCIF_MASK));    // 等待命令完成
}
static s32 Kinetis_CmdLaunch(void)
{
    // 清除访问错误标志位和非法访问标志位
	FTFE->FSTAT = (1<<5) | (1<<4);

    // 启动命令
    FTFE->FSTAT = FTFE_FSTAT_CCIF_MASK;

    // 等待命令结束
    while(!(FTFE->FSTAT & FTFE_FSTAT_CCIF_MASK));

    u32 temp;
    temp = FTFE->FSTAT;
    // 检查错误标志
    if(temp & ((1<<5) | (1<<4) | 1))
        return 1 ; //执行命令出错

    return 0; //执行命令成功
}
static s32 Kinetis_EraseSector(u16 sectorNo)
{
    union
    {
        u32  word;
        u8   byte[4];
    } dest;

    dest.word    = (u32)(sectorNo*(1<<11));

    FTFE->FCCOB0 = 0x09; //擦除扇区

    // 设置目标地址
    FTFE->FCCOB1 = dest.byte[2];
    FTFE->FCCOB2 = dest.byte[1];
    FTFE->FCCOB3 = dest.byte[0];

    // 执行命令序列
    if(1 == Kinetis_CmdLaunch())    //若执行命令出现错误
        return 1;     //擦除命令错误

    // 若擦除sector0时，则解锁设备
    if(dest.word <= 0x800)
    {

        FTFE->FCCOB0 = 0x06; // 写入4字节
        // 设置目标地址
        FTFE->FCCOB1 = 0x00;
        FTFE->FCCOB2 = 0x04;
        FTFE->FCCOB3 = 0x0C;
        // 数据
        FTFE->FCCOB4 = 0xFF;
        FTFE->FCCOB5 = 0xFF;
        FTFE->FCCOB6 = 0xFF;
        FTFE->FCCOB7 = 0xFE;
        // 执行命令序列
        if(1 == Kinetis_CmdLaunch())  //若执行命令出现错误
            return 2;   //解锁命令错误
    }

    return 0;  //成功返回
}

static s32 Kinetis_PageWrite(u16 sectNo,u8 *buf)
{
    u32 size;
    u32 offset;
    u8 *Data;

    union
    {
        u32   word;
        u8  byte[4];
    } dest;

    // 设置写入命令
    FTFE->FCCOB0 = 0x07;
    size = sp_tFlashDesrc->BytesPerPage;
    offset = 0;
    Data = buf;
    while(size)
    {
    	dest.word = (u32)(sectNo*(1<<11) + offset);//计算地址
        // 设置目标地址
        FTFE->FCCOB1 = dest.byte[2];
        FTFE->FCCOB2 = dest.byte[1];
        FTFE->FCCOB3 = dest.byte[0];

        // 拷贝数据
        FTFE->FCCOB4 = Data[3];
        FTFE->FCCOB5 = Data[2];
        FTFE->FCCOB6 = Data[1];
        FTFE->FCCOB7 = Data[0];
        FTFE->FCCOB8 = Data[7];
        FTFE->FCCOB9 = Data[6];
        FTFE->FCCOBA = Data[5];
        FTFE->FCCOBB = Data[4];

        if(1 == Kinetis_CmdLaunch())
            break;  //写入命令错误
        Cache_Invalidate();
        offset += 8;
        size   -= 8;
        Data   += 8;
    }

    return offset;  //成功执行
}
//-----------------------------------------------------------------------------
//功能: 获取内置FLASH的信息
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static s32 Flash_GetDescr(struct EmbdFlashDescr *Description)
{
	Kinetis_FlashInit();

	Description->BytesPerPage = 2048;
	Description->PagesPerSmallSect = 0;
	Description->PagesPerLargeSect = 0;
	Description->PagesPerNormalSect = 1;
	Description->SmallSectorsPerPlane = 0;
	Description->LargeSectorsPerPlane = 0;
	Description->NormalSectorsPerPlane = 512;
	Description->Planes = 1;
	Description->ReservedPages = 256; // 2个sector为256KB大小,最终由lds提供
	Description->MappedStAddr = 0x000000;
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

	Ret = Kinetis_EraseSector(SectorNo);

	if(Ret != 0)
		Ret = -1;
	return (Ret);
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
	u32 Ret;

	if(!Data)
		return (-1);

	Ret = Kinetis_PageWrite(Page,Data);
	return Ret;
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static s32 Flash_PageRead(u32 Page, u8 *Data, u32 Flags)
{
	u32 Addr = Page * sp_tFlashDesrc->BytesPerPage + sp_tFlashDesrc->MappedStAddr;

	if(!Data)
		return (-1);

	memcpy(Data, (u8*)Addr, sp_tFlashDesrc->BytesPerPage);
	return (sp_tFlashDesrc->BytesPerPage);

}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
s32 ModuleInstall_EmbededFlash(const char *ChipName, u32 Flags, u16 Start)
{

	u32 Len;
    struct FlashChip *Chip;
	struct MutexLCB *FlashLock;
	u8 *Buf;
	s32 Ret = 0;

	if (!ChipName)
		return (-1);

	if(sp_tFlashDesrc)
		return (-4); // 设备已注册

	sp_tFlashDesrc = malloc(sizeof(*sp_tFlashDesrc));
	if(!sp_tFlashDesrc)
		return (-1);

    // 获取FLASH信息
    if(Flash_GetDescr(sp_tFlashDesrc))
    {
        TraceDrv(FLASH_TRACE_ERROR, "解析内置FLASH信息失败\r\n");
		Ret = -3;
		goto FAILURE;
    }

    if(Start > ((sp_tFlashDesrc->NormalSectorsPerPlane+1)*
    		sp_tFlashDesrc->Planes))
	{
    	Ret = -1;
		goto FAILURE;
    }

	sp_tFlashDesrc->ReservedPages += Start;
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
	Chip->Descr.Embd              = *sp_tFlashDesrc;
	Chip->Ops.ErsBlk              = Flash_SectorEarse;
	Chip->Ops.WrPage              = Flash_PageProgram;
	Chip->Ops.RdPage              = Flash_PageRead;
	strcpy(Chip->Name, ChipName); // 设备名
	if(Flags & FLASH_BUFFERED)
	{
		Buf = (u8*)malloc(sp_tFlashDesrc->BytesPerPage); // NAND底层缓冲
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
		if(sp_tFlashDesrc)
			free(sp_tFlashDesrc);
		if(FlashLock)
			Lock_MutexDelete(FlashLock);
		if(Buf)
			free(Buf);
		if(Chip)
			free(Chip);
	}
	return (Ret);
}
