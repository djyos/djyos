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

#include <stddef.h>
#include <endian.h>
#include "flash.h"


//-----------------------------------------------------------------------------
//功能: 解析NAND FLASH的ONFI参数页。
//参数: OnfiPage -- NandParamterPage
//      Onfi -- Nand描述结构体
//      Little -- NandParamterPage的格式,"1"表示32位小端。
//返回: 0 -- 成功; -1 -- 失败;
//备注: 参数页的具体数据结构,要看注意下NAND FLASH的参数手册
//-----------------------------------------------------------------------------
s32 DecodeONFI(const char *OnfiPage, struct NandDescr *Onfi, u8 Little)
{
   u8 i;
   u32 Offset;
   u8 Temp[20] = {0};
   char Signatrue[4] = {'O', 'N', 'F', 'I'};

   //ONFI标记
   Offset = 0;
   if(Little)
   {
	   *(u32 *)Signatrue = swapl(*(u32 *)Signatrue);
   }
   for (i = 0; i < 4; i++)
   {
	   if (Signatrue[i] != (char)OnfiPage[Offset+i])
		   return (-1);// 标记内容不符要求
   }

   //NOFI版本号
   Offset = 4;
   for (i = 0; i < 4; i++)
   {
       Temp[i] = (u8)OnfiPage[Offset+i];
   }
   if(Little)
	   Onfi->Revision = (u16)swapl(*(u32 *)Temp);
   else
	   Onfi->Revision = (u16)(*(u32 *)Temp);

   //Device Manufacturer
   Offset = 32;
   for (i = 0; i < 12; i++)
   {
       Temp[i] = (u8)OnfiPage[Offset+i];
   }
   if(Little)
   {
	   for (i = 0; i < 3; i++)
	   {
		   ((u32 *)Temp)[i] = swapl(((u32 *)Temp)[i]);
	   }
   }
   for (i = 0; i < 12; i++)
   {
       Onfi->Manufacturer[i] = (char)Temp[i];
   }
   Onfi->Manufacturer[i] = '\0';

   //Device Model
   Offset = 44;
   for (i = 0; i < 20; i++)
   {
       Temp[i] = (u8)OnfiPage[Offset+i];
   }
   if(Little)
   {
	   for (i = 0; i < 5; i++)
	   {
		   ((u32 *)Temp)[i] = swapl(((u32 *)Temp)[i]);
	   }
   }
   for (i = 0; i < 20; i++)
   {
       Onfi->DeviceModel[i] = (char)Temp[i];
   }
   Onfi->DeviceModel[i] = '\0';

   //Number of data bytes per page
   Offset = 80;
   for (i = 0; i < 4; i++)
   {
       Temp[i] = (u8)OnfiPage[Offset+i];
   }
   if(Little)
   {
	   Onfi->BytesPerPage = swapl(*(u32 *)Temp);
   }
   else
   {
	   Onfi->BytesPerPage = *(u32 *)Temp;
   }

   //Number of spare bytes per page
   Offset = 84;
   for (i = 0; i < 4; i++)
   {
       Temp[i] = (u8)OnfiPage[Offset+i];
   }
   if(Little)
   {
	   Onfi->OOB_Size = swapl(*(u32 *)Temp);
   }
   else
   {
	   Onfi->OOB_Size = *(u32 *)Temp;
   }

   //Number of pages per block
   Offset = 92;
   for (i = 0; i < 4; i++)
   {
       Temp[i] = (u8)OnfiPage[Offset+i];
   }
   if(Little)
   {
	   Onfi->PagesPerBlk = swapl(*(u32 *)Temp);
   }
   else
   {
	   Onfi->PagesPerBlk = *(u32 *)Temp;
   }

   //Number of blocks per LUN
   Offset = 96;
   for (i = 0; i < 4; i++)
   {
       Temp[i] = (u8)OnfiPage[Offset+i];
   }
   if(Little)
   {
	   Onfi->BlksPerLUN = swapl(*(u32 *)Temp);
   }
   else
   {
	   Onfi->BlksPerLUN = *(u32 *)Temp;
   }

   //Number of LUNs
   Offset = 100;
   for (i = 0; i < 4; i++)
   {
       Temp[i] = (u8)OnfiPage[Offset+i];
   }
   if(Little)
   {
	   Onfi->LUNs = (u8)swapl(*(u32 *)Temp);
   }
   else
   {
	   Onfi->LUNs = (u8)(*(u32 *)Temp);
   }

#if 0
   //Guaranteed valid block at the beginning of target
   for (i = 0; i < 4; i++)
   {
       Temp[i] = (u8)OnfiPage[104+i];
   }
   *(u32 *)Temp = swapl(*(u32 *)Temp);
   Onfi->SafeBlkNo = Temp[3];
#endif

   TraceDrv(FLASH_TRACE_DEBUG, "ONFI revision:0x\%x\r\n", Onfi->Revision);
   TraceDrv(FLASH_TRACE_DEBUG, "Manufacturer:%s\r\n", Onfi->Manufacturer);
   TraceDrv(FLASH_TRACE_DEBUG, "Device Model:%s\r\n", Onfi->DeviceModel);
   TraceDrv(FLASH_TRACE_DEBUG, "bytes per page:0x%x\r\n", Onfi->BytesPerPage);
   TraceDrv(FLASH_TRACE_DEBUG, "oob size:0x%x\r\n", Onfi->OOB_Size);
   TraceDrv(FLASH_TRACE_DEBUG, "pages per block:0x%x\r\n", Onfi->PagesPerBlk);
   TraceDrv(FLASH_TRACE_DEBUG, "blocks per LUN:0x%x\r\n", Onfi->BlksPerLUN);
   TraceDrv(FLASH_TRACE_DEBUG, "LUNs:0x%x\r\n", Onfi->LUNs);
#if 0
   TraceDrv(FLASH_TRACE_DEBUG, "Guaranteed block:0x%x\r\n", Onfi->SafeBlkNo );
#endif

   return (0);
}
//-----------------------------------------------------------------------------
//功能: 擦除整颗芯片(保留块除外)
//参数:
//返回: >=0 -- 存在的坏块数；-1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
s32 EarseWholeChip(struct FlashChip *Chip)
{
    u32 i;
    u32 Blks;
    s32 BadBlks = 0;

    if(NULL == Chip)
    	return (-1);

    if(F_NAND == Chip->Type)
    {
    	Blks = Chip->Descr.Nand.BlksPerLUN * Chip->Descr.Nand.LUNs;

		for(i = Chip->Descr.Nand.ReservedBlks; i < Blks; i++)
		{
			if(0 != Chip->Ops.ErsBlk(i))
			{
				// 擦除失败
				BadBlks++;
				TraceDrv(FLASH_TRACE_ERROR, "block %d cannot be erased!\r\n", i);
			}
		}
    }
    else if(F_NOR == Chip->Type)
    {
    	if(0 == Chip->Descr.Nor.ReservedBlks)
    	{
			if(0 != Chip->Ops.Special.Nor.ErsChip())
			{
				// 擦除失败
				BadBlks = -1;
				TraceDrv(FLASH_TRACE_ERROR, "chip %d cannot be erased!\r\n", i);
			}
    	}
		else
		{
			for(i = Chip->Descr.Nand.ReservedBlks; i < Chip->Descr.Nor.Blks; i++)
			{
				if(0 != Chip->Ops.ErsBlk(i))
				{
					// 擦除失败
					BadBlks++;
					TraceDrv(FLASH_TRACE_ERROR, "block %d cannot be erased!\r\n", i);
				}
			}
		}

    }
    else
    {
    	BadBlks = -1;
    }
    return (BadBlks);
}
