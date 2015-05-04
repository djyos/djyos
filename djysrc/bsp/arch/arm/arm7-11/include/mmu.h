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

#ifndef __MMU_H_
#define __MMU_H_

#ifdef __cplusplus
extern "C" {
#endif

extern u32 mmu_page_table[];

// cp15协处理器中的寄存器定义
// c0 - ID编码（只读）

// c1 - 控制寄存器
#define CN_BM_CP15_C1_MMU_EN 0x00000001 //使能MMU或者PU
#define CN_CP15_C1_MMU_EN 0
#define CN_CP15_C1_MEM_ALIGN_CHECK_EN 0x00000002 //使能内存访问时的地址对齐检查
#define CN_BO_CP15_C1_MEM_ALIGN_CHECK_EN 1
#define CN_BM_CP15_C1_CACHE_DATA_EN 0x00000004 //使能数据Cache（如果指令和数据Cache统一时，设置这个位会同时禁止指令和数据Cache）
#define CN_BO_CP15_C1_CACHE_DATA_EN 2
#define CN_BM_CP15_C1_WRITE_BUFFER_EN 0x00000008 //使能写缓冲
#define CN_BO_CP15_C1_WRITE_BUFFER_EN 3
#define CN_BM_CP15_C1_MEM_BIG_ENDIAN 0x00000080 //选择使用big-endian内存模式（仅对于两种端模式都支持的ARM才有效）
#define CN_BO_CP15_C1_MEM_BIG_ENDIAN 7
#define CN_BM_CP15_C1_SYS_PROTECTION 0x00000100 //在基于MMU的存储系统中，本控制位用作系统保护
#define CN_BO_CP15_C1_SYS_PROTECTION 8
#define CN_BM_CP15_C1_ROM_PROTECTION 0x00000200 //在基于MMU的存储系统中，本控制位用作ROM保护
#define CN_BO_CP15_C1_ROM_PROTECTION 9
#define CN_BM_CP15_C1_BR_PREDICTION_EN 0x00000800 //使能跳转预测功能（如果支持）
#define CN_BO_CP15_C1_BR_PREDICTION_E
#define CN_BM_CP15_C1_CACHE_INSTRUCTION_EN 0x00001000 //使能指令Cache
#define CN_BO_CP15_C1_CACHE_INSTRUCTION_EN 12
#define CN_BM_CP15_C1_EXCEPTIONS_LOCATION 0x00002000 //异常中断向量表的位置选择（0:0x0-0x1c;1:0xFFFF0000-0xFFFF001C）
#define CN_BO_CP15_C1_EXCEPTIONS_LOCATION 13
#define CN_BM_CP15_C1_RR_STRATEGY 0x00004000 //系统中Cache的淘汰算法（0:随机淘汰；1:Round-robin淘汰算法）
#define CN_BO_CP15_C1_RR_STRATEGY 14

// 一级描述符中各个位的定义（段定义）
#define CN_BM_FIRST_LEVEL_DES_AP 0x00000C00 //AP两个位
#define CN_BO_FIRST_LEVEL_DES_AP 10
#define CN_BM_FIRST_LEVEL_DES_DOMAIN 0x000001E0 //域
#define CN_BO_FIRST_LEVEL_DES_DOMAIN 5
#define CN_BM_FIRST_LEVEL_DES_CACHE 0x00000008 //C位
#define CN_BO_FIRST_LEVEL_DES_CACHE 3 //C位
#define CN_BM_FIRST_LEVEL_DES_BUFFER 0x00000004 //B位
#define CN_BO_FIRST_LEVEL_DES_BUFFER 2 //B位

u32 MMU_MapSection(u32 vaddr, u32 paddr, int properties);
void MMU_LockEntry();
void MMU_InvalidateTlbInstructionEntry(void);
void MMU_InvalidateTlbDatasEntry(void);
void MMU_InvalidateTlbInstructionsAll(void);
void MMU_InvalidateTlbDatasAll(void);
void MMU_InvalidateTlbsAll(void);
void MMU_SetSectionsBase(u32 *base);
void MMU_SetDomainAccess(u32 authorization);
void MMU_Disable(void);
void MMU_Enable(void); 

#ifdef __cplusplus
}
#endif

#endif /* __MMU_H_ */
