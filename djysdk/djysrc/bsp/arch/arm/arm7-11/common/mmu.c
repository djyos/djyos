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
//所属模块:  ARM9 MMU函数集
//作者：     Liuwei
//版本：      V1.0.0
//初始版本完成日期：2015-05-22
//修订历史:
//-----------------------------------------------------------------------------

#include "stdint.h"

//---- MMU段映射 ----------------------------------------------------------------
//描述: 将物理段地址映射到虚拟地址.
//参数：tt_base: 转换表基址.
//      va_start: 虚拟地址(必须是1MB地址对齐).
//      pa_start: 物理地址(必须是1MB地址对齐).
//      sec_count: 需要映射的段数量.
//		attr: 段属性.
// C B   写通型cache         写回型cache         可选型cache
// 0 0   禁cache，禁写缓冲   禁cache，禁写缓冲   禁cache，禁写缓冲
// 0 1   禁cache，开写缓冲   禁cache，开写缓冲   禁cache，开写缓冲
// 1 0   开cache，禁写缓冲   不可预测            写通开cache，开写缓冲
// 1 1   开cache，开写缓冲   开cache，开写缓冲   写回开cache，开写缓冲
//返回：无.
//------------------------------------------------------------------------------

void MMU_MapSection(u32 *tt_base,u32 va_start,u32 pa_start,u32 sec_count,u32 attr)
{
	u32 *page_tbl;
    int i;

    page_tbl  = (u32 *)tt_base;
    page_tbl += (va_start>>20);
 	pa_start  = pa_start>>20;

    for(i=0;i<sec_count;i++)
    {
   		page_tbl[i]	=((pa_start+i)<<20) | (attr);
    }

}

// ---- mmu段映射--------------------------------------------------------------
//功能: 将虚拟地址vaddr开始的一个段(1M)映射到物理地址paddr中，同时设置该段的
//      cache及写缓冲属性。映射后，访问vaddr将被mmu重定向到paddr中。
//页表条目C(bit3),B(bit2)位含义定义:
// C B   写通型cache         写回型cache         可选型cache
// 0 0   禁cache，禁写缓冲   禁cache，禁写缓冲   禁cache，禁写缓冲
// 0 1   禁cache，开写缓冲   禁cache，开写缓冲   禁cache，开写缓冲
// 1 0   开cache，禁写缓冲   不可预测            写通开cache，开写缓冲
// 1 1   开cache，开写缓冲   开cache，开写缓冲   写回开cache，开写缓冲
//返回：无
//-----------------------------------------------------------------------------
/*
void xxMMU_MapSection(u32 vaddr, u32 paddr, int properties)
{
    mmu_page_table[vaddr>>20] = (paddr & 0xfff00000)|(properties&0xFFF);

}*/

// ---- 将某个entry锁定在TLB中 ----
// 将某些地址变换锁定在TLB中，这样对于这些虚拟地址到物理地址的访问就会变得更加迅速
// 需要加参数
// ---- C10
void MMU_LockEntry()
{

}

// ---- 使指令TLB中的一个条目无效 ----
// ---- C8
void MMU_InvalidateTlbInstructionEntry(void)
{
}

// ---- 使数据TLB中的一个条目无效 ----
// ---- C8
void MMU_InvalidateTlbDatasEntry(void)
{
}

// ---- 使指令TLB中的所有条目无效 ----
// ---- C8
void MMU_InvalidateTlbInstructionsAll(void)
{
}

// ---- 使数据TLB中的所有条目无效 ----
// ---- C8
void MMU_InvalidateTlbDatasAll(void)
{
}

// ---- 使统一TLB中的所有条目无效 ----
// 使统一TLB失效，即指令与数据的TLB都失效
// ---- C8
void MMU_InvalidateTlbsAll(void)
{
}

// ---- 设置MMU一级页表的基址 ----
void MMU_SetSectionsBase(u32 *base)
{
    __asm__ volatile (
            "mcr    p15, 0, r0, c2, c0, 0 @页表基地址 \n\t"
            :
            :"r"(base)
            :"memory"
    );
}

// ---- 设置MMU域访问权限 -----------------------------------------------------
//功能: 设置mmu的域控制，arm可以设置16个域，每个域可以定义访问权限。在页表中，
//      可以指定该段的访问权限受哪个域控制。
//参数: authorization,访问权限控制字，每个域占两位
//返回: 无
//-----------------------------------------------------------------------------
void MMU_SetDomainAccess(u32 authorization)
{
    __asm__ volatile (
            "mcr    p15, 0, r0, c3, c0, 0 @写域寄存器 \n\t"
            :
            :"r"(authorization)
            :"memory"
    );
}

// ---- 禁止MMU ----
void MMU_Disable(void)
{
    __asm__ volatile (
            "mrc    p15, 0, r0, c1, c0, 0 \n\t"
            "bic    r0, r0, #0x1 @禁止mmu \n\t"
            "mcr    p15, 0, r0, c1, c0, 0 \n\t"
            :
            :
            :"memory"
    );
}

// ---- 使能MMU ----
void MMU_Enable(void)
{
    __asm__ volatile (
            "mrc    p15, 0, r0, c1, c0, 0 \n\t"
            "orr    r0, r0, #0x1 @允许mmu \n\t"
            "mcr    p15, 0, r0, c1, c0, 0 \n\t"
            :
            :
            :"memory"
    );
}

