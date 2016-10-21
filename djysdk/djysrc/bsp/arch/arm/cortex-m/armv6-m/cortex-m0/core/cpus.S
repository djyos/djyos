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
//所属模块:调度器
//作者：lst
//版本：V1.0.0
//文件描述:调度器中与CPU直接相关的汇编代码，主要是线程切换方面的代码。
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
.extern      __Djy_VmEngine
.extern      Int_HalfEnableAsynSignal

@注: psp并不保存在栈中，而是在vm->stack中!!

.syntax		unified
.cpu		cortex-m0
.fpu		softvfp
.thumb
.file	"cpus.S"

@----重置线程-----------------------------------------------------------------
@功能:  初始化线程的栈，并挂上线程执行函数，新创建线程时将会调用本函数
@参数:  thread_routine，被重置的线程函数
@       vm，线程虚拟机指针
@返回:  初始化结束后的当前栈指针
@函数原型:void * __asm_reset_thread(void (*thread_routine)(struct event_script *),
@                                        struct  tagThreadVm  *vm)@
@注:根据atpcs，只能使用r0~r3,否则必须压栈
@-----------------------------------------------------------------------------
.text
.align 2
.global __asm_reset_thread
.thumb
.thumb_func
__asm_reset_thread:
    ldr     r2,[r1,#4]          @取虚拟机栈顶指针
    subs    r2,r2,#8
    ldr     r3,=0x01000000      @xpsr的初始值
    str     r3,[r2,#4]
    ldr     r3,=__Djy_VmEngine @取虚拟机引擎指针
    str     r3,[r2]
    subs    r2,r2,#14*4        @后退14个寄存器,初始状态r0-r12中，除r0外均无意义，
                                @__vm_engine函数不返回，lr也无意义
    @存在r0中的thread_routine是__vm_engine的参数，切换上下文时，thread_routine将
    @恢复到r0中，根据调用约定，r0的值就是__vm_engine函数的参数。
    str     r0,[r2,#8*4]        @保存 thread_routine指针到r0的位置.
    str     r2,[r1]             @保存vm的当前栈指针到vm->stack中
    bx      lr
@ end of __asm_reset_thread

@----复位老线程，切换到新线程-------------------------------------------------
@功能:  把old_vm的上下文重新初始化到新创建的状态，然后切换到新线程的
@       上下文中继续执行。
@       当一个在常驻内存的虚拟机中处理的事件完成后，不能删除该虚拟机，必须复位
@       该虚拟机，然后切换到其他就绪线程，这时候就需要调用本函数。因此本函数在
@       被复位的线程上下文里面执行。
@参数:  thread_routine，被重置的线程函数
@       new_vm，目标线程的虚拟机
@       old_vm，被复位的虚拟机
@返回:  无
@函数原型:void __asm_reset_switch(void (*thread_routine)(void),
@                           struct  tagThreadVm *new_vm,struct  tagThreadVm *old_vm)@
@注:老线程将被重置，寄存器使用无限制，无须保存
@-----------------------------------------------------------------------------
.text
.align 2
.global __asm_reset_switch
.thumb
.thumb_func
__asm_reset_switch:
    ldr     r7,[r2,#4]          @取老虚拟机栈顶指针
    mov     sp,r7
    ldr     r7,=0x01000000      @xpsr的初始值
    ldr     r6,=__Djy_VmEngine    @取虚拟机引擎指针
    push    {r6,r7}             @存pc,xpsr
    sub     sp,sp,#14*4         @后退14个寄存器,初始状态r0-r12中，除r0外均无意义，
                                @__vm_engine函数不返回，lr也无意义
                                @至此，完成老虚拟机复位
    str     r0,[sp,#8*4]        @保存 thread_routine指针.至此，完成老线程重置
    mov     r5,sp
    str     r5,[r2]             @保存当前栈指针到old_vm->stack中

    ldr     r5,[r1]             @取得新上下文指针
    mov     sp,r5
@    bl      Int_HalfEnableAsynSignal @对应done函数开头的 __Int_ResetAsynSignal 调用
    svc     0                   @切入新上下文
    bx      lr
@ end of __asm_reset_switch

@----启动多事件调度-----------------------------------------------------------
@功能: 系统初始化完成后，启动多事件调度并切入第一个事件.
@参数: new_vm，首次启动的虚拟机控制块指针
@返回: 无
@函数原型: void __asm_start_thread(struct  tagThreadVm  *new_vm)@
@-----------------------------------------------------------------------------
.text
.align 2
.global __asm_start_thread
.thumb
.thumb_func
__asm_start_thread:
    ldr     r1,=msp_top
    msr     msp,r1			    @重置主栈指针，主栈将用于异常和中断
    movs    r1,#2
    msr     control,r1          @切换到特权线程模式，栈指针用psp
    ldr     r1,[r0]             @取得新上下文指针
    mov     sp,r1
@    bl      Int_HalfEnableAsynSignal @线程启动后，调度应该是允许的
    svc     0                   @切入新上下文
    bx      lr

@----切入上下文---------------------------------------------------------------
@功能:  不保存原上下文，直接切入新的上下文执行
@参数:  new_vm，待切入的虚拟机控制块指针
@返回:  无
@函数原型: void __asm_turnto_context(struct  tagThreadVm  *new_vm)@
@说明:  当事件完成,就没有必要保存旧事件的上下文,直接切换到新事件即可.
@-----------------------------------------------------------------------------
.text
.align 2
.global __asm_turnto_context
.thumb
.thumb_func
__asm_turnto_context:
    ldr     r1,[r0]             @取得新上下文指针
    mov     sp,r1
@    bl      Int_HalfEnableAsynSignal @对应done函数开头的 Int_SaveAsynSignal 调用
    svc     0                   @切入新上下文
    bx      lr
@ end of __asm_turnto_context

@----上下文切换---------------------------------------------------------------
@功能:  保存当前线程的上下文，切换到新线程的上下文中继续执行。
@参数:  new_sp，新上下文的栈指针
@参数:  old_sp，旧上下文的栈指针的指针，即&vm->stack。无需提供旧上下文栈指针，
@               sp寄存器的当前值就是
@返回:  无
@函数原型: void __asm_switch_context(struct  tagThreadVm *new_vm,struct  tagThreadVm *old_vm)@
@-----------------------------------------------------------------------------
.text
.align 2
.global __asm_switch_context
.thumb
.thumb_func
__asm_switch_context:
    mrs     r3,xpsr
    ldr     r2,=0x01000000
	orrs    r3,r3,r2            @xpsr的T标志读不出来，得手工置位。
    push    {r3}                @保存xpsr
    push    {lr}                @保存PC,从其他线程切换回来时，相当于原线程调用
                                @本函数返回，故用lr替代pc
    mov     r3,r12
    push    {r3,lr}             @保存r12,lr
    push    {r0-r3}             @保存r0-r3
    mov     r2,r10
    mov     r3,r11
    push    {r2,r3}             @根据atpcs，r4~r7入栈前，不能使用，故只能用r2，
                                @r3做临时变量，分两次保存r8~r11
    mov     r2,r8
    mov     r3,r9
    push    {r2,r3}
    push    {r4-r7}
    mov     r2,sp
    str     r2,[r1]             @保存旧上下文栈指针到old_vm->stack

    ldr     r2,[r0]             @取得新上下文指针
    mov     sp,r2
@    bl      Int_HalfEnableAsynSignal
    svc     0                   @切入新上下文
    bx      lr
@ end of __asm_switch_context

@----中断中的上下文切换-------------------------------------------------------
@功能:  保存被中断线程的上下文，切换到新线程的上下文中继续执行。本函数虽然在中
@       断服务函数(非用户用int_isr_connect函数连接的中断服务函数)中，但在ARM
@       中，却运行在svc态
@参数:  new_sp，切换目标虚拟机
@参数:  old_sp，被中断线程虚拟机
@返回:  无
@函数原型: void __asm_switch_context_int(struct tagThreadVm *new_vm,struct tagThreadVm *old_vm)@
@-----------------------------------------------------------------------------
.text
.align 2
.global __asm_switch_context_int
.thumb
.thumb_func
__asm_switch_context_int:
    mrs     r2,psp      @取被中断线程的psp
    str     r2,[r1]     @把psp写入虚拟机数据结构中
    ldr     r2,[r0]     @取待切入线程的psp
    msr     psp,r2      @待切入线程的当前栈指针写入psp，中断返回恢复上下文将以此为准
    bx      lr

.text
.align 2
.global __asm_reset_cpu
.global __asm_reset_cpu_hot
.thumb
.thumb_func
__asm_reset_cpu:
__asm_reset_cpu_hot:
    movs     r0,#0
    mov     pc,r0


.end
