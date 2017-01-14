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

#ifndef __FLASH_H__
#define __FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "nor.h"
#include "nand.h"
#include "embedded_flash.h"
#include "ecc_sw.h"
#include "flash_trace.h"


//
// Flags
//
#define FLASH_ERASE_ALL						((u32)0x1)
#define FLASH_BUFFERED						((u32)0x2)

// 
// 读写FLAG标志
// 
#define SPARE_REQ               			((u32)0x00000001)
#define HW_ECC                  			((u32)0x00010000)
#define SW_ECC                  			((u32)0x00020000)
#define NO_ECC                  			((u32)0x00040000)
#define MASK_ECC                			((u32)0x00070000)

//
// FLASH操作方式
//
struct FlashOps{
    s32 (*RdPage)(u32 PageNo, u8 *Data, u32 Flags); 			// 读页
    s32 (*WrPage)(u32 PageNo, u8 *Data, u32 Flags); 			// 写页,去掉const 因为存在ECC数据填充
    s32 (*ErsBlk)(u32 BlkNo);	 								// 擦除块
    s32 (*PageToBlk)(u32 PageNo, u32 *Remains, u32 *BlkNo);			// 通过页
    union{
    	struct NandSpecialOps Nand;								// Nand器件特别操作
    	struct NorSpecialOps Nor;                               // Nor器件特别操作
    }Special;

};

// 
// FLASH设备底层管理信息
// 
struct FlashChip{
	union {
		struct NandDescr Nand; 									// 描述NAND信息
		struct NorDescr Nor; 									// 描述NOR信息
		struct EmbdFlashDescr Embd;				     		    // 描述特殊的FLASH
	}Descr;
    struct FlashOps Ops; 										// 操作函数表,用于实现找到节点即会操作的功能
    void *Lock; 												// todo: 由于存在缓冲,需要底层互斥锁。
    u8 *Buf; 													// 底层缓冲
	u8 Type; 													// Flash的类型
#define F_NAND         (0x0)
#define F_NOR          (0x1)
#define F_ALIEN        (0x2)
    char Name[]; 												// 器件的别名
};

//
//
//
s32 ModuleInstall_NAND(const char *ChipName, u32 Flags, u16 StartBlk);
s32 ModuleInstall_NOR(const char *ChipName, u32 Flags, u8 StartBlk);
s32 DecodeONFI(const char *OnfiPage, struct NandDescr *Onfi, u8 Little);
s32 EarseWholeChip(struct FlashChip *Chip);

#ifdef __cplusplus
}
#endif

#endif // ifndef __FLASH_H__
