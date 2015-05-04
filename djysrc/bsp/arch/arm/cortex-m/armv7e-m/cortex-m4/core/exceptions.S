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
//所属模块:  异常模块
//作者：     lst
//版本：      V1.00
//初始版本完成日期：2009-08-30
//文件描述:  系统异常模块中的汇编部分
//其他说明:  无
//修订历史:
//2. 日期: 2013-05-24
//   修改人: 裴乐
//   说明: 直接把代码拷了一份,当作cortex-m4的对应部分
//1. 日期:
//   作者:
//   新版本号：
//   修改说明:
//------------------------------------------------------


.syntax         unified
.cpu            cortex-m4
.fpu            softvfp
.thumb
.file   "exceptions.S"

//因异步信号使能才允许调度，故上下文切换中恢复异步信号寄存器等同于使能异步信号
.macro restore_asyn_signal_reg
    bl Int_ContactAsynSignal
.endm

/**** 栈大小配置 ****/
.section        .stack.msp, "aw", %nobits
.align          3
        .space  0x800


/**** 异常向量表 ****/
.section    .isr_vector, "ax", %progbits
.align      3
.global     isr_vector
isr_vector:
    .word   msp_top
    .word   Init_Cpu
    .word   nmi_handler
    .word   hardfault_handler
    .word   exp_memfault_handler      @ MemManage Fault Handler
    .word   exp_busfault_handler      @ Bus Fault Handler
    .word   exp_usagefault_handler    @ Usage Fault Handler
    .word   0                         @ Reserved
    .word   0                         @ Reserved
    .word   0                         @ Reserved
    .word   0                         @ Reserved
    .word   exp_svc_handler           @ SVCall Handler
    .word   exp_debug_handler         @ Reserved
    .word   0                         @ Reserved
    .word   exp_pendsv_handler        @ PendSV Handler
    .word   exp_systick_handler       @ SysTick Handler

/**** 向量处理函数 ****/
.text
.align 2
.global default_handler
.thumb
.thumb_func
default_handler:
    b       default_handler

.global nmi_handler
.thumb_set nmi_handler, default_handler

/*
@ ------------------------------------------------------------------------------
@ 功能:Hard异常处理，发生异常时，内核会自动保存R0-R3,R12,lr,pc,xpsr,在汇编中 为了保
@      存现场，会将剩下的R4-R11压栈，硬件异常可能由其他异常引起
@ 参数:无
@ ------------------------------------------------------------------------------
*/
.text
.align 2
.global hardfault_handler
.thumb
.thumb_func
hardfault_handler:
        //first of all,save the exp state
    TST     LR,#0x4
    ITE     EQ
    MRSEQ   R1,MSP              /* 则使用的是主堆栈，故把MSP的值取出 */
    MRSNE   R1,PSP

    STMFD   R1!,{R4-R11}        /*  保存R4-R11到被中断的栈*/
    ADD     R2,R1,#64           /*  将发生异常的SP压栈*/
    STMFD   R1!,{R2}

    TST     LR,#0x4
    ITE     EQ
    MSREQ   MSP,R1              /* 则使用的是主堆栈，故把MSP的值取出 */
    MSRNE   PSP,R1

    PUSH    {R0,LR}             /* 保护返回值，可能需要用到 */

    MOV     R0,R1
    BL       Exp_HardFaultHandler

    POP     {R0,LR}
    TST     LR,#0x4
    ITE     EQ
    MRSEQ   R0,MSP              /* 则使用的是主堆栈，故把MSP的值取出 */
    MRSNE   R0,PSP
    LDMFD   R0!,{R4-R11}
    LDMFD   R0!,{R2}

    TST     LR,#0x4
    ITE     EQ
    MSREQ   MSP,R0              /* 则使用的是主堆栈，故把MSP的值取出 */
    MSRNE   PSP,R0

    BX      LR

/*
@ ------------------------------------------------------------------------------
@ 功能:MEM 异常处理，发生异常时，内核会自动保存R0-R3,R12,lr,pc,xpsr,在汇编中 为了保
@      存现场，会将剩下的R4-R11压栈，一般是由于MPU保护导致的访问违例引起的异常
@ 参数:无
@ ------------------------------------------------------------------------------
*/
.text
.align 2
.global exp_memfault_handler
.thumb
.thumb_func
exp_memfault_handler:
    //first of all,save the exp state
    TST     LR,#0x4
    ITE     EQ
    MRSEQ   R1,MSP              /* 则使用的是主堆栈，故把MSP的值取出 */
    MRSNE   R1,PSP

    STMFD   R1!,{R4-R11}        /*  保存R4-R11到被中断的栈*/
    ADD     R2,R1,#64           /*  将发生异常的SP压栈*/
    STMFD   R1!,{R2}

    TST     LR,#0x4
    ITE     EQ
    MSREQ   MSP,R1              /* 则使用的是主堆栈，故把MSP的值取出 */
    MSRNE   PSP,R1

    PUSH    {R0,LR}             /* 保护返回值，可能需要用到 */

    MOV     R0,R1
    BL       Exp_MemManageFaultHandler

    POP     {R0,LR}
    TST     LR,#0x4
    ITE     EQ
    MRSEQ   R0,MSP              /* 则使用的是主堆栈，故把MSP的值取出 */
    MRSNE   R0,PSP
    LDMFD   R0!,{R4-R11}
    LDMFD   R0!,{R2}

    TST     LR,#0x4
    ITE     EQ
    MSREQ   MSP,R0              /* 则使用的是主堆栈，故把MSP的值取出 */
    MSRNE   PSP,R0

    BX      LR

/*
@ ------------------------------------------------------------------------------
@ 功能:BusFault处理，发生异常时，内核会自动保存R0-R3,R12,lr,pc,xpsr,在汇编中 为了保
@      存现场，会将剩下的R4-R11压栈
@ 参数:无
@ ------------------------------------------------------------------------------
*/
.text
.align 2
.global exp_busfault_handler
.thumb
.thumb_func
exp_busfault_handler:
    //first of all,save the exp state
    TST     LR,#0x4
    ITE     EQ
    MRSEQ   R1,MSP              /* 则使用的是主堆栈，故把MSP的值取出 */
    MRSNE   R1,PSP

    STMFD   R1!,{R4-R11}        /*  保存R4-R11到被中断的栈*/
    ADD     R2,R1,#64           /*  将发生异常的SP压栈*/
    STMFD   R1!,{R2}

    TST     LR,#0x4
    ITE     EQ
    MSREQ   MSP,R1              /* 则使用的是主堆栈，故把MSP的值取出 */
    MSRNE   PSP,R1

    PUSH    {R0,LR}             /* 保护返回值，可能需要用到 */

    MOV     R0,R1
    BL       Exp_BusFaultHandler

    POP     {R0,LR}
    TST     LR,#0x4
    ITE     EQ
    MRSEQ   R0,MSP              /* 则使用的是主堆栈，故把MSP的值取出 */
    MRSNE   R0,PSP
    LDMFD   R0!,{R4-R11}
    LDMFD   R0!,{R2}

    TST     LR,#0x4
    ITE     EQ
    MSREQ   MSP,R0              /* 则使用的是主堆栈，故把MSP的值取出 */
    MSRNE   PSP,R0

    BX      LR

/*
@ ------------------------------------------------------------------------------
@ 功能:Debug异常处理，发生异常时，内核会自动保存R0-R3,R12,lr,pc,xpsr,在汇编中 为了保
@      存现场，会将剩下的R4-R11压栈
@ 参数:无
@ ------------------------------------------------------------------------------
*/
.text
.align 2
.global exp_debug_handler
.thumb
.thumb_func
exp_debug_handler:
    b       exp_debug_handler

.text
.align 2
.global exp_pendsv_handler
.thumb
.thumb_func
exp_pendsv_handler:
    b       exp_pendsv_handler

/*
@ ------------------------------------------------------------------------------
@ 功能:USAGE FAULT 异常处理，发生异常时，内核会自动保存R0-R3,R12,lr,pc,xpsr,在汇编
@      中 为了保存现场，会将剩下的R4-R11压栈，最后将栈顶传参到服务函数
@ 参数:R0中保存了发生异常时的栈顶，即现场
@ ------------------------------------------------------------------------------
*/
.text
.align 2
.global exp_usagefault_handler
.thumb
.thumb_func
exp_usagefault_handler:
    //first of all,save the exp state
    TST     LR,#0x4
    ITE     EQ
    MRSEQ   R1,MSP              /* 则使用的是主堆栈，故把MSP的值取出 */
    MRSNE   R1,PSP

    STMFD   R1!,{R4-R11}        /*  保存R4-R11到被中断的栈*/
    ADD     R2,R1,#64           /*  将发生异常的SP压栈*/
    STMFD   R1!,{R2}

    TST     LR,#0x4
    ITE     EQ
    MSREQ   MSP,R1              /* 则使用的是主堆栈，故把MSP的值取出 */
    MSRNE   PSP,R1

    PUSH    {R0,LR}             /* 保护返回值，可能需要用到 */

    MOV     R0,R1
    BL       Exp_UsageFaultHandler

    POP     {R0,LR}
    TST     LR,#0x4
    ITE     EQ
    MRSEQ   R0,MSP              /* 则使用的是主堆栈，故把MSP的值取出 */
    MRSNE   R0,PSP
    LDMFD   R0!,{R4-R11}
    LDMFD   R0!,{R2}

    TST     LR,#0x4
    ITE     EQ
    MSREQ   MSP,R0              /* 则使用的是主堆栈，故把MSP的值取出 */
    MSRNE   PSP,R0

    BX      LR

/*
@ ------------------------------------------------------------------------------
@ 功能:SYSTICK 异常处理，发生异常时，内核会自动保存R0-R3,R12,lr,pc,xpsr,在汇编
@      中 为了保存现场，会将剩下的R4-R11压栈，服务函数返回后，恢复现场，并返回
@ 参数:无
@ ------------------------------------------------------------------------------
*/
.text
.align 2
.global exp_systick_handler
.thumb
.thumb_func
exp_systick_handler:
    push    {r0,lr}             /* r0只是为了满足栈指针8字节对齐的填充物 */
    ldr     r0,=0xe000e010
    ldr     r0,[r0]
    mrs     r0,psp
    stmfd   r0!,{r4-r11}        /* 保护寄存器,以及返回地址 */
    msr     psp,r0
    bl      Exp_SystickTickHandler
    pop     {r0,lr}
    mrs     r0,psp
    ldmfd   r0!,{r4-r11}
    msr     psp,r0
    bx      lr

/*
@ ------------------------------------------------------------------------------
@ 功能:SVC服务 异常处理，SVC异常主要用于作线程切换，在需要进行线程切换时，使用SVC指令
@      并将相应的参数传递到异常，异常根据SVC号判断SVC服务
@ 参数:SVC号
@ -------------------------------------------------------------------------------
*/
.text
.align 2
.global exp_svc_handler
.thumb
.thumb_func
exp_svc_handler:
    push    {lr}
    restore_asyn_signal_reg
    pop     {lr}
    TST     LR,#0x4             /* 测试EXC_RETURN的比特2 */
    ITE     EQ                  /* 如果为0, */
    MRSEQ   R0,MSP              /* 则使用的是主堆栈，故把MSP的值取出 */
    MRSNE   R0,PSP              /* 否则, 使用的是进程堆栈，故把MSP的值取出 */
    LDR     R1,[R0,#24]         /* 从栈中读取PC的值 */
    LDRB    R1,[R1,#-2]         /* 从SVC指令中读取立即数放到R0 */
    cbnz    r1,svc_error        /* 目前只支持0号svc调用 */
    add     r0,#32              /* 0号调用用于上下文切换，调整psp指针 */
    ldmfd   r0!,{r4-r11}        /* 手工弹出r4-r11 */
    msr     psp,r0              /* psp指向待切入的上下文 */
svc_error:                      /* 直接返回 */
    bx      lr                  /* 返回，实际弹出的将是待切入上下文 */

.end
