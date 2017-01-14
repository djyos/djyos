//-----------------------------------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. As a constituent part of djyos,do not transplant it to other software
//    without specific prior written permission.

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
// 这份授权条款，在使用者符合以下三条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。
// 3. 本软件作为都江堰操作系统的组成部分，未获事前取得的书面许可，不允许移植到非
//    都江堰操作系统环境下运行。

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
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <systime.h>
#include <lock.h>
#include <driver/flash/flash.h>

struct __LowLevel{
	u32 StartPage;
	u32 EndPage;
	u32 PageBytes;
	u32 EraseBytes;
	s32 (*WritePage)(u32 PageNo, u8 *Data, u32 Flags);
	s32 (*ReadPage)(u32 PageNo, u8 *Data, u32 Flags);
	s32 (*Erase)(u32 BlkNo);
	s32 (*GetSector)(u32 Page, u32 *Remain, u32 *Sector);
	struct MutexLCB *Lock;
	u8 *Buf;
};

extern u32 gc_pAppRange; // 来自于LDS定义
extern u32 gc_pAppOffset; // 来自于LDS定义
//-----------------------------------------------------------------------------
//功能: IAP的volume初始化
//参数: Dev -- flash设备
//      Base -- 返回IAP在volume的偏置。
//返回: IAP的volume
//备注: 
//-----------------------------------------------------------------------------
void *LowLevelInit(void *Dev, u32 *Base)
{
	struct __LowLevel *LowLevel;
	struct EmbdFlashDescr *Embd;
	u32 Capacity; 
	struct FlashChip *Flash = (struct FlashChip *)Dev;
	u32 IAP_Capacity = gc_pAppRange;
	u32 IAP_Start = gc_pAppOffset;
	u32 Unalign = 0;
	
	LowLevel = malloc(sizeof(struct __LowLevel));
	if(!LowLevel)
		return (NULL);
	
	Embd = &Flash->Descr.Embd;
	IAP_Start -= Embd->MappedStAddr; // flash空间范围为操作基准
	if(IAP_Start < Embd->ReservedPages * Embd->BytesPerPage)
	{
		IAP_Start = Embd->ReservedPages;
	}
	else
	{
		if(IAP_Start % Embd->BytesPerPage)	
			Unalign	= 1;	
		IAP_Start = IAP_Start / Embd->BytesPerPage + Unalign; // 转化为页号
	}

	Capacity = (Embd->PagesPerSmallSect * Embd->SmallSectorsPerPlane +
			    Embd->PagesPerLargeSect * Embd->LargeSectorsPerPlane +
		        Embd->PagesPerNormalSect * Embd->NormalSectorsPerPlane) *
		       Embd->Planes - Embd->ReservedPages;
	Unalign = 0;		  
	if(IAP_Capacity % Embd->BytesPerPage)	
		Unalign	= 1; // 容量不对齐，则去掉不对齐的部分
			
	IAP_Capacity = IAP_Capacity / Embd->BytesPerPage - Unalign;	// 转换为页号
	if(Capacity < IAP_Capacity)
		IAP_Capacity = Capacity;
	
	LowLevel->StartPage = IAP_Start;
	LowLevel->EndPage = IAP_Capacity + IAP_Start - 1; // 页号从零计
	LowLevel->PageBytes = Embd->BytesPerPage;
	LowLevel->WritePage = Flash->Ops.WrPage;
	LowLevel->ReadPage = Flash->Ops.RdPage;
	LowLevel->Erase = Flash->Ops.ErsBlk;
	LowLevel->GetSector = Flash->Ops.PageToBlk;
	if(Flash->Lock)
	{
		LowLevel->Lock = (struct MutexLCB*)Flash->Lock;
	}
	else
	{
		free(LowLevel);
		return (NULL); // 失败，驱动必须提供互斥锁
	}

		
	if(Flash->Buf)
	{
		LowLevel->Buf = Flash->Buf;
	}
	else
	{
		free(LowLevel);
		return (NULL); // 失败，驱动必须提供缓冲
	}	

	*Base = 0; //
	return ((void*)LowLevel);
}

//-----------------------------------------------------------------------------
//功能: IAP的volume卸载
//参数:
//返回:
//备注: 
//-----------------------------------------------------------------------------
void LowLevelDeInit(void *LowLevel)
{
	free(LowLevel);
}

//-----------------------------------------------------------------------------
//功能: 页偏置
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static u32 __PageFix(struct __LowLevel *LowLevel, u32 PageNo)
{
	return (PageNo + LowLevel->StartPage);
}
//-----------------------------------------------------------------------------
//功能: 写数据
//参数: LowLevel -- IAP的volume
//      Buf -- 数据缓冲
//		Bytes -- 写字节数
//		Addr -- 数据地址
//返回: 实际写入的字节数
//备注: 
//-----------------------------------------------------------------------------
s32 LowLevelWrite(void *LowLevel, u8 *Buf, u32 Bytes, u32 Addr)
{
	u32 i, PageNo, Offset;
    s32 Ret;
    u32 LenToWr, OnceToWr;
	struct __LowLevel *LL = (struct __LowLevel *)LowLevel;
    u8 *DataBuf = (u8*)Buf;

    LenToWr = Bytes;
    Lock_MutexPend(LL->Lock, CN_TIMEOUT_FOREVER);
    PageNo = Addr / LL->PageBytes;
    PageNo = __PageFix(LL, PageNo);
    Offset = Addr % LL->PageBytes;
    while(LenToWr > 0)
    {
        OnceToWr = MIN((LL->PageBytes - Offset), LenToWr);
        if(OnceToWr != LL->PageBytes)
        {
        	Ret = LL->ReadPage(PageNo, LL->Buf, NO_ECC);
            if(!Ret)
                goto FAILURE; //

            // 做简单的检查
            for(i = Offset; i < (Offset + OnceToWr); i++)
            {
                if((0xFF != LL->Buf[i]) && (LL->Buf[i] != DataBuf[i]))
                {
                    printf("data cannot be writen, offset %d, destination 0x%x, source 0x%x\r\n",
                            i, LL->Buf[i], DataBuf[i]);
                    goto FAILURE; //
                }
            }
        }

        memcpy((LL->Buf + Offset), DataBuf, OnceToWr);
        Ret = LL->WritePage(PageNo, LL->Buf, NO_ECC);
        if(Ret != LL->PageBytes)
            goto FAILURE;
        LenToWr -= OnceToWr;
        Offset = 0;
        PageNo++;
        DataBuf += OnceToWr;
    }

FAILURE:	
	Lock_MutexPost(LL->Lock);	
	
	return ((s32)(Bytes - LenToWr));
}

//-----------------------------------------------------------------------------
//功能: 读数据
//参数: LowLevel -- IAP的volume
//      Buf -- 数据缓冲
//		Bytes -- 读字节数
//		Addr -- 数据地址
//返回: 实际读到的字节数
//备注: 
//-----------------------------------------------------------------------------
s32 LowLevelRead(void *LowLevel, u8 *Buf, u32 Bytes, u32 Addr)
{
    s32 Ret;
    u32 PageNo, Offset;
    u32 LenToRd, OnceToRd;
	struct __LowLevel *LL = (struct __LowLevel *)LowLevel;
    u8 *DataBuf = (u8*)Buf;

    LenToRd = Bytes;
    Lock_MutexPend(LL->Lock, CN_TIMEOUT_FOREVER);
    PageNo = Addr / LL->PageBytes;
    PageNo = __PageFix(LL, PageNo);
    Offset = Addr % LL->PageBytes;
    while(LenToRd > 0)
    {
        OnceToRd = MIN((LL->PageBytes - Offset), LenToRd);
        Ret = LL->ReadPage(PageNo, LL->Buf, NO_ECC);
        if(!Ret)
            goto FAILURE; //
        memcpy(DataBuf, (LL->Buf + Offset), OnceToRd);
        LenToRd -= OnceToRd;
        Offset = 0;
        PageNo++;
        DataBuf += OnceToRd;
    }

FAILURE:
    Lock_MutexPost(LL->Lock);
    return((s32)(Bytes - LenToRd));
}

//-----------------------------------------------------------------------------
//功能: 写数据
//参数: LowLevel -- IAP的volume
//      Buf -- 数据缓冲
//		Bytes -- 写字节数
//		Addr -- 数据地址
//返回: 实际写入的字节数
//备注: 
//-----------------------------------------------------------------------------
s32 LowLevelErase(void *LowLevel, u32 Bytes, u32 Addr)
{
	s32 Res;
	u32 PageNo;
	u32 PagesRemain;
	u32 SectorNo;
	u32 Offset;
	u32 BytesLeft = Bytes;
	struct __LowLevel *LL = (struct __LowLevel *)LowLevel;
	
	Lock_MutexPend(LL->Lock, CN_TIMEOUT_FOREVER);
	Offset = Addr % LL->PageBytes;
	while(BytesLeft)
	{
		PageNo = Addr / LL->PageBytes;
		PageNo = __PageFix(LL, PageNo);
		LL->GetSector(PageNo, &PagesRemain, &SectorNo);
		Res = LL->Erase(SectorNo);
		if(Res)
			break;
			
		if(BytesLeft < (PagesRemain) * LL->PageBytes)
			BytesLeft = 0;
		else	
			BytesLeft -= ((PagesRemain) * LL->PageBytes - Offset);
		
		Addr += (PagesRemain * LL->PageBytes - Offset);
		Offset = 0;	
	}
	Lock_MutexPost(LL->Lock);
	return (Bytes - BytesLeft);
}
//-----------------------------------------------------------------------------
//功能: 格式化IAP的volume空间
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
s32 LowLevelFormat(void *LowLevel)
{
	s32 Res;
	u32 PageNo;
	u32 PagesLeft;
	u32 PagesRemain;
	u32 SectorNo;
	struct __LowLevel *LL = (struct __LowLevel *)LowLevel;

	PagesLeft = LL->EndPage - LL->StartPage + 1; // 页号从零计
	PageNo = LL->StartPage;
	Lock_MutexPend(LL->Lock, CN_TIMEOUT_FOREVER);
	while(PagesLeft)
	{
		LL->GetSector(PageNo, &PagesRemain, &SectorNo);
		Res = LL->Erase(SectorNo);
		if(Res)
			break;

		if(PagesLeft < (PagesRemain))
			PagesLeft = 0;
		else
			PagesLeft -= (PagesRemain);

		PageNo += PagesRemain;
	}
	Lock_MutexPost(LL->Lock);

	return(Res);
}

//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
u32 LowLevelCRC32(u32 Offset, u32 Len)
{
	char *Data = (char*)(Offset + gc_pAppOffset);

	extern u32 crc32( char *buf, u32 len);
    return ((u32)crc32(Data, Len));
}
