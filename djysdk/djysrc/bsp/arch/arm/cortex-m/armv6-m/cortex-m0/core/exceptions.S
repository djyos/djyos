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
//2. ...
//1. 日期:
//   作者:
//   新版本号：
//   修改说明:
//------------------------------------------------------
.syntax         unified
.cpu            cortex-m0
.fpu            softvfp
.thumb
.file   "exceptions.S"

//因异步信号使能才允许调度，故上下文切换中恢复异步信号寄存器等同于使能异步信号
.macro restore_asyn_signal_reg
    bl Int_ContactAsynSignal
.endm

/**** 栈大小配置 ****/
.section        .stack.msp, "aw", %note
.align          3
        .space  0x300

.extern  Exp_EsrTick
.extern  Int_ContactAsynSignal

/**** 异常向量表 ****/
.section    .isr_vector, "ax", %progbits
.align          3
.global         isr_vector
isr_vector:
        .word   msp_top
        .word   Init_Cpu
        .word   nmi_handler
        .word   hardfault_handler
    .word   0                         @ Reserved
    .word   0                         @ Reserved
    .word   0                         @ Reserved
    .word   0                         @ Reserved
    .word   0                         @ Reserved
    .word   0                         @ Reserved
    .word   0                         @ Reserved
    .word   exp_svc_handler           @ SVCall Handler
    .word   0                         @ Reserved
    .word   0                         @ Reserved
    .word   0                         @ PendSV Handler
    .word   exp_systick_handler       @ SysTick Handler

    @ External Interrupts
    .word     __start_int_0           @ 0
    .word     __start_int_1           @ 1
    .word     __start_int_2           @ 2
    .word     __start_int_3           @ 3
    .word     __start_int_4           @ 4
    .word     __start_int_5           @ 5
    .word     __start_int_6           @ 6
    .word     __start_int_7           @ 7
    .word     __start_int_8           @ 8
    .word     __start_int_9           @ 9
    .word     __start_int_10          @ 10
    .word     __start_int_11          @ 11
    .word     __start_int_12          @ 12
    .word     __start_int_13          @ 13
    .word     __start_int_14          @ 14
    .word     __start_int_15          @ 15
    .word     __start_int_16          @ 16
    .word     __start_int_17          @ 17
    .word     __start_int_18          @ 18
    .word     __start_int_19          @ 19
    .word     __start_int_20          @ 20
    .word     __start_int_21          @ 21
    .word     __start_int_22          @ 22
    .word     __start_int_23          @ 23
    .word     __start_int_24          @ 24
    .word     __start_int_25          @ 25
    .word     __start_int_26          @ 26
    .word     __start_int_27          @ 27
    .word     __start_int_28          @ 28
    .word     __start_int_29          @ 29
    .word     __start_int_30          @ 30
    .word     __start_int_31          @ 31



/**** 向量处理函数 ****/
.text
.align 2
.global default_handler
.thumb
.thumb_func
default_handler:
        b               default_handler

.global nmi_handler
.thumb_set nmi_handler, default_handler

.global hardfault_handler
.thumb_set hardfault_handler, default_handler





.text
.align 2
.global exp_systick_handler
.thumb
.thumb_func
exp_systick_handler:
    push    {r0,lr}                             @r0只是为了满足栈指针8字节对齐的填充物
    ldr     r0,=0xe000e010
    ldr     r0,[r0]             @读一下清除中断标志
    mrs     r0,psp              @取线程栈指针
    subs    r0,r0,#8*4          @后退8个寄存器,用于保存r4~r11
    msr     psp,r0
    stmia   r0!,{r4-r7}         @保护寄存器,调用tick异常可能引起线程切换。
    mov     r1,r8
    mov     r2,r9
    mov     r3,r10
    mov     r4,r11
    stmia   r0!,{r1-r4}         @保护寄存器,调用tick异常可能引起线程切换。
    bl      Exp_EsrTick
    pop     {r0,r1}
    mov     lr,r1
    mrs     r0,psp
    ldmfd   r0!,{r4-r7}
    ldmfd   r0!,{r2,r3}
    mov     r8,r2
    mov     r9,r3
    ldmfd   r0!,{r2,r3}
    mov     r10,r2
    mov     r11,r3
    msr     psp,r0
    bx      lr

.text
.align 2
.global exp_svc_handler
.thumb
.thumb_func
exp_svc_handler:
    push    {r0,lr}
    restore_asyn_signal_reg
    pop     {r0,r1}
    mov     lr,r1
    movs    r0,#4
    TST     r1,r0             @测试EXC_RETURN的比特2
    bne     isne
    MRS     R0,MSP              @则使用的是主栈，故把MSP的值取出
    beq     iseq
isne:
    MRS   R0,PSP              @否则, 使用的是线程栈，故把MSP的值取出
iseq:
    LDR     R1,[R0,#24]       @从栈中读取PC的值
    subs    r1,r1,#2
    LDRB    R1,[R1]           @从SVC指令中读取立即数放到R1
    cmp     r1,#0
    bne     svc_error        @目前只支持0号svc调用
    adds     r0,#32              @0号调用用于上下文切换，调整psp指针
    ldmfd   r0!,{r4-r7}        @手工弹出r4-r11
    ldmfd   r0!,{r2,r3}
    mov     r8,r2
    mov     r9,r3
    ldmfd   r0!,{r2,r3}
    mov     r10,r2
    mov     r11,r3
    msr     psp,r0              @psp指向待切入的上下文
svc_error:                       @直接返回
    bx      lr                  @返回，实际弹出的将是待切入上下文

.end
