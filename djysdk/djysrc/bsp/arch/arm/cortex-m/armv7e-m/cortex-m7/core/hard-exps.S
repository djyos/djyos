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
.cpu            cortex-m7
//.fpu            fpv4-sp-d16 //定义到IDE配置
.thumb
.file   "hard-exps.S"

//因异步信号使能才允许调度，故上下文切换中恢复异步信号寄存器等同于使能异步信号
.macro restore_asyn_signal_reg
    bl Int_ContactAsynSignal
.endm




/**** 向量处理函数 ****/
.text
.align 2
.global default_handler
.thumb
.thumb_func
default_handler:
    b       default_handler

.global HardExp_NmiHandler
.thumb_set HardExp_NmiHandler, default_handler

/*
@ ------------------------------------------------------------------------------
@ 功能:处理异常时保存现场和恢复现场的工作，压栈R4-R11,sp,视情况压栈s16-s31，并进入异常
@      处理函数handler，将调用异常服务函数，并将fpu_used和现场入栈的起始地址以参数的形式
@      传递到异常服务函数
@ 参数：异常服务函数地址
@ ------------------------------------------------------------------------------
*/
.text
.align 2
.global exp_status_handler
.thumb
.thumb_func
exp_status_handler:
    TST     LR,#0x4
    ITE     EQ
    MRSEQ   R1,MSP              /* 则使用的是主堆栈，故把MSP的值取出 */
    MRSNE   R1,PSP

    ADD     R2,R1,#32           /* R2中存储的是发生异常时的栈指针*/

    STMFD   R1!,{R4-R11}        /* 保存R4-R11到被中断的栈*/
    STMFD   R1!,{R2}            /* 压栈发生异常时的栈指针*/

    TST     R14,#0x10
    ITTTE   EQ
    ADDEQ   R2,#72
    MOVEQ   R0,#1
    VSTMDBEQ R1!,{S16-S31}
    MOVNE   R0,#0

    TST     LR,#0x4
    ITE     EQ
    MSREQ   MSP,R1              /* 则使用的是主堆栈，故把MSP的值取出 */
    MSRNE   PSP,R1

    PUSH    {R0,LR}             /* 保护返回值，可能需要用到 */

    BLX     R3

    POP     {R0,LR}
    TST     LR,#0x4
    ITE     EQ
    MRSEQ   R0,MSP              /* 则使用的是主堆栈，故把MSP的值取出 */
    MRSNE   R0,PSP

    TST     R14,#0x10
    IT      EQ
    VLDMIAEQ R0!,{S16-S31}

    LDMFD   R0!,{R2}
    LDMFD   R0!,{R4-R11}

    TST     LR,#0x4
    ITE     EQ
    MSREQ   MSP,R0              /* 则使用的是主堆栈，故把MSP的值取出 */
    MSRNE   PSP,R0

    BX      LR

/*
@ ------------------------------------------------------------------------------
@ 功能:Hard异常处理，发生异常时，内核会自动保存R0-R3,R12,lr,pc,xpsr,在汇编中 为了保
@      存现场，会将剩下的R4-R11压栈，硬件异常可能由其他异常引起
@ 参数:无
@ ------------------------------------------------------------------------------
*/
.text
.align 2
.global HardExp_HardfaultHandler
.thumb
.thumb_func
HardExp_HardfaultHandler:
    LDR     R3,=Exp_HardFaultHandler
    B       exp_status_handler

/*
@ ------------------------------------------------------------------------------
@ 功能:MEM 异常处理，发生异常时，内核会自动保存R0-R3,R12,lr,pc,xpsr,在汇编中 为了保
@      存现场，会将剩下的R4-R11压栈，一般是由于MPU保护导致的访问违例引起的异常
@ 参数:无
@ ------------------------------------------------------------------------------
*/
.text
.align 2
.global HardExp_MemfaultHandler
.thumb
.thumb_func
HardExp_MemfaultHandler:
    LDR     R3,=Exp_MemManageFaultHandler
    B       exp_status_handler

/*
@ ------------------------------------------------------------------------------
@ 功能:BusFault处理，发生异常时，内核会自动保存R0-R3,R12,lr,pc,xpsr,在汇编中 为了保
@      存现场，会将剩下的R4-R11压栈
@ 参数:无
@ ------------------------------------------------------------------------------
*/
.text
.align 2
.global HardExp_BusfaultHandler
.thumb
.thumb_func
HardExp_BusfaultHandler:
    LDR     R3,=Exp_BusFaultHandler
    B       exp_status_handler

/*
@ ------------------------------------------------------------------------------
@ 功能:Debug异常处理，发生异常时，内核会自动保存R0-R3,R12,lr,pc,xpsr,在汇编中 为了保
@      存现场，会将剩下的R4-R11压栈
@ 参数:无
@ ------------------------------------------------------------------------------
*/
.text
.align 2
.global HardExp_DbgHandler
.thumb
.thumb_func
HardExp_DbgHandler:
    b       HardExp_DbgHandler

.text
.align 2
.global HardExp_PendsvHandler
.thumb
.thumb_func
HardExp_PendsvHandler:
    b       HardExp_PendsvHandler

/*
@ ------------------------------------------------------------------------------
@ 功能:USAGE FAULT 异常处理，发生异常时，内核会自动保存R0-R3,R12,lr,pc,xpsr,在汇编
@      中 为了保存现场，会将剩下的R4-R11压栈，最后将栈顶传参到服务函数
@ 参数:R0中保存了发生异常时的栈顶，即现场
@ ------------------------------------------------------------------------------
*/
.text
.align 2
.global HardExp_UsagefaultHandler
.thumb
.thumb_func
HardExp_UsagefaultHandler:
    LDR     R3,=Exp_UsageFaultHandler
    B       exp_status_handler
/*
@ ------------------------------------------------------------------------------
@ 功能:SYSTICK 异常处理，发生异常时，内核会自动保存R0-R3,R12,lr,pc,xpsr,在汇编
@      中 为了保存现场，会将剩下的R4-R11压栈，服务函数返回后，恢复现场，并返回
@ 参数:无
@ ------------------------------------------------------------------------------
*/
.text
.align 2
.global HardExp_SystickHandler
.thumb
.thumb_func
HardExp_SystickHandler:
    PUSH    {R0,LR}             /* r0只是为了满足栈指针8字节对齐的填充物 */
    LDR     R0,=0xe000e010
    LDR     R0,[R0]
    MRS     R0,psp

    TST     R14, #0x10          /* 测试EXC_RETURN的比特4 */
    IT      EQ                  /* 如果为0 */
    VSTMDBEQ R0!,{S16-S31}      /* 手动压栈FPU寄存器 */

    STMFD   R0!,{r4-r11}        /* 保护寄存器,以及返回地址 */
    STMFD   R0!,{R14}
    MSR     PSP,R0
    BL      Exp_SystickTickHandler
    POP     {R0,LR}
    mrs     R0,PSP

    LDMFD   R0!,{R14}
    LDMFD   R0!,{R4-R11}

    TST     R14, #0x10          /* 测试EXC_RETURN的比特4 */
    IT      EQ                  /* 如果为0 */
    VLDMIAEQ R0!,{S16-S31}      /* 手动压栈FPU寄存器 */

    MSR     PSP,R0
    BX      LR

/*
@ ------------------------------------------------------------------------------
@ 功能:SVC服务 异常处理，SVC异常主要用于作线程切换，在需要进行线程切换时，使用SVC指令
@      并将相应的参数传递到异常，异常根据SVC号判断SVC服务
@ 参数:SVC号
@ -------------------------------------------------------------------------------
*/
.text
.align 2
.global HardExp_SvcHandler
.thumb
.thumb_func
HardExp_SvcHandler:
//进入SVC时，M4自动压栈了,区分svc 0 and svc 1。
//svc为0时，本svc_handler无须保存现场，因为在进入时已经将现场保存了或无须保存现场，请参见
//cpus.S__asm_start_thread外的函数，sp已经更新为new_vm的栈；
//svc为1时，表示的是线程之间的切换，因此需保存现场，对应__asm_start_thread
    PUSH    {LR}
    restore_asyn_signal_reg
    POP     {LR}
    TST     LR,#0x4             /* 测试EXC_RETURN的比特2 */
    ITE     EQ                  /* 如果为0 */
    MRSEQ   R0,MSP              /* 则使用的是主堆栈，故把MSP的值取出 */
    MRSNE   R0,PSP              /* 否则, 使用的是进程堆栈，故把PSP的值取出 */

    LDR     R2,[R0,#24]         /* 从栈中读取PC的值 */
    LDRB    R2,[R2,#-2]         /* 从SVC指令中读取立即数放到R2 */
    CMP     R2,#1               /* svc号为 1,只有一号SVC才需要压栈 */
    ITTEE   EQ
    LDREQ   R2,[R0]             /* 计算new_vm栈顶，并暂存在R2*/
    LDREQ   R2,[R2]
    ADDNE   R2,R0,#36
    BNE     svc_switch          /* 目前只支持0号svc调用 */

    TST     R14, #0x10          /* 测试EXC_RETURN的比特4 */
    IT      EQ                  /* 如果为0 */
    VSTMDBEQ R0!,{S16-S31}      /* 手动压栈FPU寄存器 */
    STMFD   R0!,{R4-R11}        /* 手动压栈内核寄存器，压栈R14是为了下次切入时查询是否需pop FPU寄存器 */
    STMFD   R0!,{R14}

    STR     R0, [R1]            /* 保存老线程到old_vm->stack */

svc_switch:                     /* svc为0时，只需弹出新线程即可*/
    LDMFD   R2!,{R14}           /* 从R14中可得知是否需pop FPU寄存器 */
    LDMFD   R2!,{R4-R11}        /* 手工弹出r4-r11 */

    TST     R14, #0x10          /* 测试EXC_RETURN的比特4 */
    IT      EQ                  /* 如果为0 */
    VLDMIAEQ R2!,{S16-S31}      /* 手动压栈FPU寄存器 */
    MSR     PSP,R2              /* psp指向待切入的上下文 */

    BX      LR                  /* 返回，实际弹出的将是待切入上下文 */
.end

