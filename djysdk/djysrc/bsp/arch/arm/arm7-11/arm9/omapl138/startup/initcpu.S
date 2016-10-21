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
//所属模块:  CPU初始化
//作者：     lst
//版本：      V1.1.0
//文件描述:  CPU初始化必须用汇编语言实现的部分
//其他说明:  无
//修订历史:
//2. 日期: 2009-10-30
//   作者: lst
//   新版本号：V1.1.0
//   修改说明: 把内存和时钟用C语言初始化
//1. 日期: 2009-03-10
//   作者: lst
//   新版本号：V1.0.0
//   修改说明: s3c2440的原始版本
//------------------------------------------------------
.cpu	arm926ej-s
.fpu	softvfp
.file	"initcpu.S"

#include "arm_mode_s_file.h"

.section .text._start, "ax", %progbits
.align  2
.global _start

.long 0x11
_start:        /* 复位向量地址 */
	ldr		r0, =0x68000010		@cs2(flash)配置地址
	ldr		r1, =0x00a0050d		@cs2设置为13个访问周期，默认值是88个周期，启动太慢
	str		r1, [r0]

    mrs     r0, cpsr                  /* 取CPSR */
    bic     r0, r0, #SF_MODEMASK   /* 清模式位 */
    orr     r1, r0, #SF_SVCMODE|SF_IRQ_BIT|SF_FIQ_BIT    /* 设置为管理态，并禁止中断 */
    msr     cpsr_cxsf,r1              /* 切换到管理态,可防止意外返回0地址时出错. */
	LDR     r0, =0xffff2000
	mov		sp, r0

    mrc     p15,0,r0,c1,c0,0
    orr     r0,r0,#0x00002000       @使能高端向量
    bic     r0,r0,#0x1000           @禁止Icache
    bic     r0,r0,#0x1              @禁止mmu
    bic     r0,r0,#0x4              @禁止Dcache
    mcr     p15,0,r0,c1,c0,0

#ifdef release
	BL	    config_clocks
	BL	    config_ddr
#endif

/* 初始化栈指针 */
    mrs     r0, cpsr
    bic     r0, r0, #SF_MODEMASK

    orr		r0, r0, #SF_IRQ_BIT|SF_FIQ_BIT

    orr     r1, r0, #SF_UNDEFMODE
    msr     cpsr_cxsf, r1
    ldr     sp, =undef_top

    orr     r1, r0, #SF_ABORTMODE
    msr     cpsr_cxsf, r1
    ldr     sp, =abort_top

    orr     r1, r0, #SF_IRQMODE
    msr     cpsr_cxsf, r1
    ldr     sp, =irq_top

    orr     r1, r0, #SF_FIQMODE
    msr     cpsr_cxsf, r1
    ldr     sp, =fiq_top

    orr     r1, r0, #SF_SVCMODE
    msr     cpsr_cxsf, r1
    ldr     sp, =svc_top

    orr     r1, r0, #SF_SYSMODE
    msr     cpsr_cxsf, r1

	LDR     r0, =0xffff2000
	mov		sp, r0
    bl		MMU_Init
    ldr     sp, =usr_top

    bl      Load_Preload

@----热启动-----------------------------------------------------------------
@功能: 热启动地址，用于DSP给ARM软复位
@参数: 无
@返回: 不返回
@函数原型:void __asm_hot_restart(void );
@-----------------------------------------------------------------------------
.section .text.__asm_hot_restart, "ax", %progbits
.global __asm_hot_restart
__asm_hot_restart:
    mrs     r0, cpsr                  /* 取CPSR */
    bic     r0, r0, #SF_MODEMASK   /* 清模式位 */
    orr     r1, r0, #SF_SVCMODE|SF_IRQ_BIT|SF_FIQ_BIT    /* 设置为管理态，并禁止中断 */
    msr     cpsr_cxsf,r1              /* 切换到管理态,可防止意外返回0地址时出错. */
	LDR     r0, =0xffff2000
	mov		sp, r0

/* 初始化栈指针 */
    mrs     r0, cpsr
    bic     r0, r0, #SF_MODEMASK

    orr		r0, r0, #SF_IRQ_BIT|SF_FIQ_BIT

    orr     r1, r0, #SF_UNDEFMODE
    msr     cpsr_cxsf, r1
    ldr     sp, =undef_top

    orr     r1, r0, #SF_ABORTMODE
    msr     cpsr_cxsf, r1
    ldr     sp, =abort_top

    orr     r1, r0, #SF_IRQMODE
    msr     cpsr_cxsf, r1
    ldr     sp, =irq_top

    orr     r1, r0, #SF_FIQMODE
    msr     cpsr_cxsf, r1
    ldr     sp, =fiq_top

    orr     r1, r0, #SF_SVCMODE
    msr     cpsr_cxsf, r1
    ldr     sp, =svc_top

    orr     r1, r0, #SF_SYSMODE
    msr     cpsr_cxsf, r1
    ldr     sp, =usr_top

    bl      Load_Preload

@ end of __asm_hot_restart

.end
