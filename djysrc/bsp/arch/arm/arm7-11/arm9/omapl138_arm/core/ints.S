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
//所属模块:  中断模块
//作者：     lst
//版本：      V1.0.1
//文件描述:  中断模块中的汇编部分
//其他说明:  无
//修订历史:
//3. 日期:2010-3-06
//   作者:lst
 //   新版本号：1.0.2
//   修改说明:     stmfd   sp!,{r0-r3,lr}         保存lr_svc,
//           改为  stmfd   sp!,{r0,lr}             保存lr_svc,
//2. 日期:2010-01-04
//   作者:lst
//   新版本号：1.0.1
//   修改说明: 从gcc移植到keil，并无修改代码
//1. 日期:2009-03-10
//   作者:lst
//   新版本号：1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
.cpu	arm926ej
.fpu	softvfp
.file	"ints.S"

/* *********************** ********** ***********************
 *在2440中谨慎使用fiq中断,除非你只有一个中断被设置成fiq,否则,没有任何直接的信息
 *会告诉你正在响应哪个中断.FIQ模式中断号寄存器，你只能通过F_ISPR来确认中断源，
 *ARM9没有直接返回前导0个数的指令，只能用循环移位的方法。确定中断源所需的时间开
 *销很大，故2440中FIQ变得毫无意义，因此djyos在2440版本并不使用FIQ中断.
*********************** ********** *********************** */

#include "arm_mode_s_file.h"
.equ    INTOFFSET, 0x4a000014
.equ    TOFF_AINTC_HIPIR1, 0x900
.equ    TOFF_AINTC_HIPIR2, 0x904

//cpsr的IF位清0，允许全部中断
.section .text.int_enable_arm_int, "ax", %progbits
.align 2
.global int_enable_arm_int
int_enable_arm_int:
	mrs r0, cpsr
	bic r0, r0, #SF_IRQ_BIT|SF_FIQ_BIT
	msr cpsr_cxsf, r0
	bx	lr

//cpsr的IF位置1，禁止全部中断
.section .text.int_disable_arm_int, "ax", %progbits
.align 2
.global int_disable_arm_int
int_disable_arm_int:
	mrs  r0, cpsr
	orr  r0, r0, #SF_IRQ_BIT|SF_FIQ_BIT
	msr  cpsr_cxsf, r0
	bx	lr

//cpsr的I位置1，禁止irq中断
.section .text.__int_disable_irq, "ax", %progbits
.align 2
.global __int_disable_irq
__int_disable_irq:
	mrs  r0, cpsr
	orr  r0, r0, #SF_IRQ_BIT
	msr  cpsr_cxsf, r0
	bx	lr

//cpsr的I位清0，允许irq中断
.section .text.__int_enable_irq, "ax", %progbits
.align 2
.global __int_enable_irq
__int_enable_irq:
	mrs  r0, cpsr
	bic  r0, r0, #SF_IRQ_BIT
	msr  cpsr_cxsf, r0
	bx	lr

//cpsr的F位置1，禁止fiq中断
.section .text.__int_disable_fiq, "ax", %progbits
.align 2
.global __int_disable_fiq
__int_disable_fiq:
	mrs r0,cpsr
	orr r0,r0,#SF_FIQ_BIT
	msr cpsr_cxsf,r0
	bx	lr

//cpsr的F位清0，允许fiq中断
.section .text.__int_enable_fiq, "ax", %progbits
.align 2
.global __int_enable_fiq
__int_enable_fiq:
	mrs r0, cpsr
	bic r0, r0, #SF_FIQ_BIT
	msr cpsr_cxsf, r0
	bx	lr

/* *********************** ********** ***********************
 *中断相关的栈安排：
 *1、irq_stack放被中断的上下文，顺序:lr，r12-r0，cpsr，共14字
 *2、svc_stack运行中断服务函数，包括用户编写的中断服务函数。
 *3、SYS_stack，除非要在中断里切换上下文，否则无关
*********************** ********** *********************** */
.section .text.start_int, "ax", %progbits
.align 2
.global start_int
start_int:
    stmfd   sp!,{r0-r12,lr}          /* 保护寄存器,以及返回地址 */
    ldr r0,=0xFFFEE000
    ldr r0,[r0, #TOFF_AINTC_HIPIR2]

    mrs     r1,spsr
    stmfd   sp!,{r1}                 /* 保护SPSR_irq，以支持中断嵌套 */
    msr     cpsr_c,#SF_SVCMODE|SF_IRQ_BIT   /* 进入SVCMODE,以便允许中断嵌套 */
    stmfd   sp!,{r0,lr}             /* 保存lr_svc,r0是保持栈8字节对齐的填充物 */

    ldr		r2, =__Int_EngineAsynSignal

    mov     lr,pc               /* 这执行4G空间内的函数调用 */
    mov     pc,r2				/* intn用r0传递 */

    ldmfd   sp!,{r0,lr}     /* 恢复lr_svc, */
    msr     cpsr_c,#SF_IRQMODE|SF_IRQ_BIT   /* 更新cpsr,进入IRQ模式并禁止中断,由于lr寄存
                             		器的问题,进入irq或者fiq模式，均要禁止中断以防嵌套，
                             		只有在进入svc状态才允许嵌套 */
    ldmfd   sp!,{r0}        /* spsr->r0 */
    msr     spsr_cxsf,r0    /* 恢复spsr */
    ldmfd   sp!,{r0-r12,lr}
    subs    pc,lr,#4        /* 此后，中断被重新打开 */

.section .text.start_fiq, "ax", %progbits
.align 2
.global start_fiq
start_fiq:
    stmfd   sp!,{r0-r12,lr}         /* 保护寄存器,以及返回地址 */

	LDR		r0, =0xFFFEE000
	LDR		r0, [r0, #TOFF_AINTC_HIPIR1]

    mrs     r1,spsr
    stmfd   sp!,{r1}               	/* 保护SPSR_fiq，以支持中断嵌套 */
    msr     cpsr_c,#SF_SVCMODE 	    /* 进入SVCMODE,同时允许中断嵌套 */
                                    /* 因arm的lr寄存器的问题，在fiq模式是不能实现 */
                                    /* 中断嵌套的。 */
    stmfd   sp!,{r0,lr}            	/* 保存lr_svc,r0是保持栈8字节对齐的填充物 */

    ldr     r2,=__Int_EngineReal    /* 取异步信号地址 */

    mov     lr,pc           /* 这两条指令模拟函数调用(4G空间)，调用用户中断处理函数, */
    mov     pc,r2           /* int_isr_real和int_isr_asyn_signal分别是实时中断和异步 */
                            /* 事件处理函数的入口地址,该函数原型为 */
                            /* void int_isr_asyn_signal(ucpu_t intn);intn为中断号, */
                            /* 根据atpcs,intn用r0传递 */

    ldmfd   sp!,{r0,lr}     /* 恢复lr_svc, */
    msr     cpsr_c,#SF_IRQMODE|SF_IRQ_BIT    /* 更新cpsr,进入IRQ模式并禁止中断 */
    ldmfd   sp!,{r0}        /* spsr->r0 */
    msr     spsr_cxsf,r0    /* 恢复spsr */
    ldmfd   sp!,{r0-r12,lr}
    subs    pc,lr,#4        /* 此后，中断被重新打开 */

.end
