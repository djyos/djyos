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
// 模块描述:P1020rdb板件的MMU映射表
// 模块版本:
// 创建人员: zhangqf
// 创建时间: 9:14:46 AM/Sep 17, 2013
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =======================================================================
#include "stdint.h"
#include "E500v2RegBits.h"
#include "SysSpace.h"
#include "TLB.h"
#include "LAW.h"
#include "ELBC.h"

const tagTlbConfig tgTlbConfig[] = {
	//ddr
    SET_TLB_ENTRY(1, CN_DDR_ADDR_V, CN_DDR_ADDR_P,
				MAS3_SX|MAS3_SW|MAS3_SR, MAS2_W | MAS2_G |MAS2_M,
				0, 0, BOOKE_PAGESZ_1G, 1),	   //DDR WILL BE WRITE THROUGH
	//other device route		
	SET_TLB_ENTRY(1, CN_SRAM_ADDR_V, CN_SRAM_ADDR_P,
				MAS3_SW|MAS3_SR|MAS3_SX, MAS2_I| MAS2_G |MAS2_M ,
				0, 7, BOOKE_PAGESZ_256K, 1),	//SRAM		
				
	//ccsr
	SET_TLB_ENTRY(1, CN_CCSR_NEWADDR_V, CN_CCSR_NEWADDR_PL,
				MAS3_SW|MAS3_SR, MAS2_I | MAS2_G|MAS2_M,
			    0, 11, BOOKE_PAGESZ_1M, 1),

	//nor flash  16MB--storage
	SET_TLB_ENTRY(1, CN_NORFLASH_EXTEND_V, CN_NORFLASH_EXTEND_P,
				MAS3_SX|MAS3_SW|MAS3_SR,MAS2_I |MAS2_G|MAS2_M,
				0, 12, BOOKE_PAGESZ_16M, 1),	
	//nor flash  16MB--code
	SET_TLB_ENTRY(1, CN_NORFLASH_ADDR_V, CN_NORFLASH_ADDR_P,
			    MAS3_SX|MAS3_SW|MAS3_SR,MAS2_G|MAS2_M,
				0, 13, BOOKE_PAGESZ_16M, 1),		
				
    //nand flash
	SET_TLB_ENTRY(1, CN_NANDFLASH_ADDR_V, CN_NANDFLASH_ADDR_P,
			    MAS3_SX|MAS3_SW|MAS3_SR,  MAS2_I |MAS2_G|MAS2_M,
				0, 14, BOOKE_PAGESZ_4K, 1),
};
const int gTlbCfgItem = ARRAY_SIZE(tgTlbConfig);
//LAW config
//0x00000000--0x3fffffff     1GB reserved for the mem
const tagLawConfig tgLawConfig[] = {
	//DDR
	{0x00000000,0x80F0001d},	
    //ELBC
    {0x000fe000,0x80400018},
};
const int gLawCfgItem = ARRAY_SIZE(tgLawConfig);

//elbc config
const tagElbcCfg  tgP1020RDB_ElbcCfg[] =
{
        {0xFE001001, 0xFE000F87},   //NOR FLASH BOOT,GPCM-CS0
        {0,0},
        {0xC0000C21, 0xF80003AE},   //NAND FLASH-CS2
        {0xEFA00801, 0xFFFE09F7},   //CPLD-CS3
};
const int gElbcCfgItem = ARRAY_SIZE(tgLawConfig);

//do the cache init

#define CN_CACHEL2_BASEADDR          (CN_CCSR_NEWADDR_V + 0x20000)
#define CN_CACHEL2_L2CTL             (CN_CACHEL2_BASEADDR + 0x00)
#define CN_CACHEL2_L2CTL_L2E         (1<<31)
#define CN_CACHEL2_L2CTL_L2I         (1<<30)
#define CN_CACHEL2_L2CTL_NOSRAM      (0x0<<16)
#define CN_CACHEL2_L2CTL_ALLSRAM     (0X01<<16)

#define CN_CACHEL2_L2ERRDIS          (CN_CACHEL2_BASEADDR+0xE44)
#define CN_CACHEL2_L2ERRDIS_SBECC    (1<<2)
#define CN_CACHEL2_L2ERRDIS_MBECC    (1<<3)

#define CN_CACHEL2_L2SRBAR0          (CN_CACHEL2_BASEADDR + 0X100)
#define CN_CACHEL2_L2SRBAREA0        (CN_CACHEL2_BASEADDR + 0x104)
#define CN_CACHEL2_SRAM_ADDRL        (0x40000000)
#define CN_CACHEL2_SRAM_ADDRH        (0x0)

/*
@-------------------------------------------------------------------------------
@function:initial the l2 cache
@para:void
@return:void
@-------------------------------------------------------------------------------
*/
void Cache_InitL2(void)
{
	u32 value;
	u32 *addr;
	
	//DISABLE THE SB AND MB ERROR
	addr = (u32 *)CN_CACHEL2_L2ERRDIS;
	value = *addr;
	value |=(CN_CACHEL2_L2ERRDIS_SBECC|CN_CACHEL2_L2ERRDIS_MBECC);
	*addr = value;
	
	//DISABLE THE L2CACHE AND SET TO THE 256KB CACHE
	addr = (u32 *)CN_CACHEL2_L2CTL;
	value = CN_CACHEL2_L2CTL_ALLSRAM;
	*addr = value;
	
	//SET THE  BASE ADDR
	addr = (u32 *)CN_CACHEL2_L2SRBAR0;
	value = CN_CACHEL2_SRAM_ADDRL;
	*addr = value;
	
	addr = (u32 *)CN_CACHEL2_L2SRBAREA0;
	value = CN_CACHEL2_SRAM_ADDRH;
	*addr = value;
	
	//ENABLE THE SRAM
	addr = (u32 *)CN_CACHEL2_L2CTL;
	value = *addr;
	value |= CN_CACHEL2_L2CTL_L2E;
	*addr = value;
	
	return;
}

