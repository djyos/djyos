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

#ifndef __CARD_H__
#define __CARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stddef.h"

//
// SD、SDIO、MMC等抽象结构体
//

// CardDescription.Type值
#define MMC			((u8)0)
#define SDIO		((u8)1)
#define SDSC		((u8)2)
#define SDHC		((u8)3)
#define SDXC		((u8)4)
								
// CardDescription.SpecVer值
#define V1			((u8)0) 	// Version 1.0 and 1.01
#define V1_1		((u8)1) 	// Version 1.10
#define V2			((u8)2) 	// Version 2.00
#define V3			((u8)3) 	// Version 3.0X
#define V4			((u8)4) 	// Version 4.XX
#define UNKNOWN		((u8)255) 	// 未知,出错了
								
// CardDescription.State值
#define IDLE		((u8)0)
#define IDENT		((u8)1)
#define STANDBY		((u8)2)
#define TRANSFER	((u8)3)
#define PROGRAM		((u8)4)

struct CardDescription{
	u8 Type;					// 卡的类型
	u8 SpecVer;					// Physical Layer Specification Version
	u8 State;					// 卡的当前状态
	u8 TransSpeed;				// 最大传输速度(0x32 = 25MBit/s)
	u32 CSize;					// 容量(byte为单位)
	u8 ReadBlLen;				// 块大小,2次方
	u8 ReadBlPartial;			// 最小读写量(0:块为单位,1:Byte为单位)
	u8 CSizeMult;				// todo: 这个参数可能无太多实际用途
	u8 BusWidth;				//
	u8 DataStatusAfterErases;	// 块被擦除后的状态，为1或0
	u16 RCA;					// 器件的相对地址
	u16 BlkLenSettings;			// 实际读写块大小,与器件和控制器都相关,2次方
	u64 CapacityInBytes;		// 块容量, 字节为单位.
};

#ifdef __cplusplus
}
#endif

#endif //__CARD_H__
