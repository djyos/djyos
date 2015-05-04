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
// 模块描述:异常处理的汇编部分，该部分主要处理异常本身，不针对片上外设
// 模块版本: V1.00(初始化版本)
// 创建人员:zhangqf_cyg
// 创建时间:2013-7-26--上午10:15:29
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================
.file   "Exp_Asm.S"
#include "E500V2Reg.h"
#include "E500V2RegBits.h"
#include "VMStack.h"
/*
@-----------------------------------------------------------------------------
@功能:中断处理前面部分，进行现场保存
@参数:
@
@-----------------------------------------------------------------------------
*/
//要保存40个寄存器
//sp指向的是可以用的栈空间
//sp-160这个栈地址也应该是空的
//sp---sp-160之间的空间刚好存够40个寄存器
//spsr指的是中断前的那个msr，一般的放在Xsrr1中的
//在此中，我们不应该修改r3/r4/r5的值，这些很可能是传给我们的参数
//PC就是XSRR0， SPSR指的是XSRR1，这个X看你是在什么异常模式中
.macro  push_current_state pc spsr
    subi   sp, sp, CUR_STACK_SIZE //sp-288这个地址空间是空的，可用的下一个sp
    stw    r31, 0(sp)//在此打开浮点支持，因为需要64位操作,
    stw    r30, 4(sp)
    lis    r31, cn_msr_spe@h   //make sure that the spe is open
    ori    r31, r31, cn_msr_spe@l
    mfmsr  r30
    or     r31, r30, r31
    mtmsr  r31
    isync
    lwz    r31,0(sp)
    lwz    r30,4(sp)

    evstdd r0, R0_OFFSET(sp)  //-288--280存放的是r0，依次类推
                  //在特殊寄存器和普通寄存器之间会有4字节的空余
    evstdd r2, R2_OFFSET(sp)
    evstdd r3, R3_OFFSET(sp)
    evstdd r4, R4_OFFSET(sp)
    evstdd r5, R5_OFFSET(sp)
    evstdd r6, R6_OFFSET(sp)
    evstdd r7, R7_OFFSET(sp)
    evstdd r8, R8_OFFSET(sp)
    evstdd r9, R9_OFFSET(sp)
    evstdd r10, R10_OFFSET(sp)
    evstdd r11, R11_OFFSET(sp)
    evstdd r12, R12_OFFSET(sp)
    evstdd r13, R13_OFFSET(sp)
    evstdd r14, R14_OFFSET(sp)
    evstdd r15, R15_OFFSET(sp)
    evstdd r16, R16_OFFSET(sp)
    evstdd r17, R17_OFFSET(sp)
    evstdd r18, R18_OFFSET(sp)
    evstdd r19, R19_OFFSET(sp)
    evstdd r20, R20_OFFSET(sp)
    evstdd r21, R21_OFFSET(sp)
    evstdd r22, R22_OFFSET(sp)
    evstdd r23, R23_OFFSET(sp)
    evstdd r24, R24_OFFSET(sp)
    evstdd r25, R25_OFFSET(sp)
    evstdd r26, R26_OFFSET(sp)
    evstdd r27, R27_OFFSET(sp)
    evstdd r28, R28_OFFSET(sp)
    evstdd r29, R29_OFFSET(sp)
    isync
    xor    r29, r29, r29
    mr    r29, sp
    addi   r29, r29, R30_OFFSET
    isync
    evstdd r30, 0(r29)
    evstdd r31, 8(r29)
    //evstdd r30, R30_OFFSET(sp)
    //evstdd r31, R31_OFFSET(sp) //evstdd connot exceed 248
    mfcr   r6
    stw    r6, CR_OFFSET(sp)     //compute flag register
    
    mfspr  r6, xer
    stw    r6, XER_OFFSET(sp)    //integer compute register

    mfspr  r6, ctr
    stw    r6, CTR_OFFSET(sp)    //couter register

    mflr   r6
    stw    r6, LR_OFFSET(sp)     //function swith register
    
    mfspr  r6, \spsr
    stw    r6, MSR_OFFSET(sp)    //cpu exp state register

    mfspr  r7, \pc
    stw    r7, PC_OFFSET(sp)     //cpu pc register
    isync
    msync
    .endm
/*
@-----------------------------------------------------------------------------
@功能:中断处理后面部分，进行现场恢复
@参数:无
@    仅仅恢复了一些必要寄存器，考虑到重用性，
@-----------------------------------------------------------------------------
*/
.macro  pop_current_state pc spsr
    lis  r3, cn_msr_spe@h
    ori  r3, r3, cn_msr_spe@l
    mfmsr  r4
    or   r3, r3, r4
    mtmsr  r3
    isync   //sure that the spe is open
    lwz   r3, PC_OFFSET(sp)//284-288存放的是PC，其余的依次类推
    mtspr  \pc, r3              //cpu pc register

    lwz   r3, MSR_OFFSET(sp)    //cpu exp state register
    mtspr \spsr, r3 
    isync
    
    lwz   r3, LR_OFFSET(sp)
    mtlr r3                      //function switch register
    isync

    lwz   r3, CTR_OFFSET(sp)
    mtctr r3                     //counter register
    isync

    lwz  r3, XER_OFFSET(sp)
    mtspr  xer, r3               //integer register
    isync

    lwz  r3, CR_OFFSET(sp)
    mtcr  r3                     //compute flag register
    isync

    evldd r0, R0_OFFSET(sp)  //-288--280存放的是r0，依次类推
                  //在特殊寄存器和普通寄存器之间会有4字节的空余
    evldd r2, R2_OFFSET(sp)
    //evldd r3, R3_OFFSET(sp) //后面恢复大数据的时候要用，没有必要这么早恢复
    evldd r4, R4_OFFSET(sp)
    evldd r5, R5_OFFSET(sp)
    evldd r6, R6_OFFSET(sp)
    evldd r7, R7_OFFSET(sp)
    evldd r8, R8_OFFSET(sp)
    evldd r9, R9_OFFSET(sp)
    evldd r10, R10_OFFSET(sp)
    evldd r11, R11_OFFSET(sp)
    evldd r12, R12_OFFSET(sp)
    evldd r13, R13_OFFSET(sp)
    evldd r14, R14_OFFSET(sp)
    evldd r15, R15_OFFSET(sp)
    evldd r16, R16_OFFSET(sp)
    evldd r17, R17_OFFSET(sp)
    evldd r18, R18_OFFSET(sp)
    evldd r19, R19_OFFSET(sp)
    evldd r20, R20_OFFSET(sp)
    evldd r21, R21_OFFSET(sp)
    evldd r22, R22_OFFSET(sp)
    evldd r23, R23_OFFSET(sp)
    evldd r24, R24_OFFSET(sp)
    evldd r25, R25_OFFSET(sp)
    evldd r26, R26_OFFSET(sp)
    evldd r27, R27_OFFSET(sp)
    evldd r28, R28_OFFSET(sp)
    evldd r29, R29_OFFSET(sp)

    isync
    xor   r3, r3, r3
    mr    r3, sp
    addi  r3, r3, R30_OFFSET
    isync
    evldd r30, 0(r3)
    evldd r31, 8(r3)
    evldd  r3, R3_OFFSET(sp)
    isync
    addi sp, sp, CUR_STACK_SIZE
    .endm


/*
@-----------------------------------------------------------------------------
@功能:外部中断处理
@参数:any way, we must set the msr's ee bit in the stack storage
@    external exception, no nesting permit
@-----------------------------------------------------------------------------
*/
.text
.align 4
.global Default_externalint_exception
Default_externalint_exception:

    push_current_state  srr0,srr1
    // set the vm msr bit in the msr
    lis r3,CN_MSR_THREAD@h
    ori r3,r3, CN_MSR_THREAD@l
    stw r3,MSR_OFFSET(sp) 
    //change the stack here, and save the old sp in ther new stack
    lis  r3, irq_top@h
    ori  r3, r3, irq_top@l    
    stw   sp, _VM_STACK_FROM_TOP(r3)//save sp
    subi  sp, r3, _IRQ_TOP_OFFSET   //must be 0x10 aligned 

    //for the external exception, the ee bit cleard, so the asyn signal also closed
    //the same time
    bl   Exp_EInt

    //open the asyn signal when we recover the msr
    lis   r3, irq_top@h
    ori   r3, r3, irq_top@l
    lwz  sp, _VM_STACK_FROM_TOP(r3)//修改SP为irq_top中内容
    pop_current_state  srr0 srr1
    isync
    rfi
/*
@-------------------------------------------------------------------------------
@功能:tick中断处理
@参数:无
@
@-------------------------------------------------------------------------------
*/
.text
.align 4
.global Default_tick_exception
Default_tick_exception:

    push_current_state  srr0,srr1
    // set the vm msr bit in the msr
    lis r3,CN_MSR_THREAD@h
    ori r3,r3, CN_MSR_THREAD@l
    stw r3,MSR_OFFSET(sp) 
    //change the stack here, and save the old sp in ther new stack
    lis  r3, irq_top@h
    ori  r3, r3, irq_top@l    
    stw   sp, _VM_STACK_FROM_TOP(r3)//save sp
    subi  sp, r3, _IRQ_TOP_OFFSET   //must be 0x10 aligned 
    //clear the dtimer flag in the tsr
    lis   r3, cn_tsr_dis@h
    ori   r3, r3,  cn_tsr_dis@l
    mtspr tsr, r3
    isync
    //for the external exception, the ee bit cleard, so the asyn signal also closed
    //the same time
    bl   Exp_DTimer

    //open the asyn signal when we recover the msr
    lis   r3, irq_top@h
    ori   r3, r3, irq_top@l
    lwz  sp, _VM_STACK_FROM_TOP(r3)//修改SP为irq_top中内容
    pop_current_state  srr0 srr1
    isync
    rfi
    
/*
@-----------------------------------------------------------------------------
@功能:sys异常处理
@参数:无
@ 他将会拥有自己的异常栈
@-----------------------------------------------------------------------------
*/
//tlb /isi/ dsi /spe / alignment exp could use this exp handler
//this exception will deal with the normal exception to debug the system
//ofcource, it is the release version,
//it is multi exception handler, which only gather the information at exp
//in the debug version with cw, we will use program exception instead
//any way, you could add more information if you thought it necessary
//most of information for debug the app or the sys
/*
@-----------------------------------------------------------------------------
@功能:sys general exception dealer
@参数:spr4存放的是中断时候的栈指针，spr5存放的是异常号
@-----------------------------------------------------------------------------
*/
.text
.align 4
.global Default_sys_general_exception
Default_sys_general_exception:
    b     .
    rfi

/*
@-----------------------------------------------------------------------------
@功能:Critical Input 异常处理
@参数:实时中断
@-----------------------------------------------------------------------------
*/
.text
.align 4
.global Default_cint_exception
Default_cint_exception:
    b     .
    rfci
/*
@-----------------------------------------------------------------------------
@功能:Machine Check 异常处理
@参数:spr4存放的是中断时候的栈指针，spr5存放的是异常号
@-----------------------------------------------------------------------------
*/
.text
.align 4
.global Default_machinecheck_exception
Default_machinecheck_exception:
    b    .
    //any way, we had done what we should done, its time to go back
    rfmci

/*
@-----------------------------------------------------------------------------
@功能:DEBUG异常处理
@参数:无
@
@-----------------------------------------------------------------------------
*/
.text
.align 4
.global Default_debug_exception
Default_debug_exception:
    subi sp, sp, 0x12
    stw  r3, 0(sp)
    stw  r4, 4(sp)
    stw  r5, 8(sp)
    //up till now it is the test version to compatile with the CW debug
    //any way, we need to clear the esr and dbsr
    xor  r3, r3, r3
    mtspr esr, r3
    isync

    mfspr r3, dbsr
    mr  r4, r3
    //use to check csrr0 to map the address
    rlwinm r4, r4, 1, 31, 31
    cmplwi r4, 0x00
    bgt   csrr0_delt
    mfspr r4, csrr0    //if dbsr[ide]=0, csrr0 contains the ins that cause exp
    addi  r4, r4, 0x04
    mtspr csrr0, r4
    isync
csrr0_delt:
    mtspr dbsr, r3
    isync
    //now recover now ,it is so simple version
    lwz   r3, 0(sp)
    lwz   r4, 4(sp)
    lwz   r5, 8(sp)
    msync
    addi  sp, sp, 0x12
    rfci//do nothing,restore the dsrr0 and dsrr1
    
/*
@----重置线程-------------------------------------------------------------------
@功能:  初始化线程的栈，并挂上线程执行函数，新创建线程时将会调用本函数
@参数:  thread_routine，被重置的线程函数
@       vm，线程指针
@返回:  初始化结束后的当前栈指针
@函数原型:void * __asm_reset_thread(void (*thread_routine)(struct event_script *),
@                                        struct  tagThreadVm  *vm)@
@-------------------------------------------------------------------------------
*/
.align 4
.text
.global __asm_reset_thread
__asm_reset_thread:

    stwu  sp, -32(sp)
    stw   r5, 16(sp)
    stw   r6, 20(sp)
    msync

    //get the vm stack top. then save the value to the stack
    lwz     r5, 0x04(r4)
    subi    r5, r5, CUR_STACK_SIZE
    xor     r6, r6, r6
    stw     r6, CR_OFFSET(r5)    //clear cr
    stw     r6, XER_OFFSET(r5)   //clear xer
    stw     r6, CTR_OFFSET(r5)   //clear counter
    stw     r6, LR_OFFSET(r5)    //clear lr
    lis     r6, CN_MSR_THREAD@h
    ori     r6, r6, CN_MSR_THREAD@l
    stw     r6, MSR_OFFSET(r5)   //reset the msr
    lis     r6, __Djy_VmEngine@h
    ori     r6, r6, __Djy_VmEngine@l
    stw     r6, PC_OFFSET(r5)    //set the pc
    evstdd  r3, R3_OFFSET(r5)    //set the r3(thread_routine) for the parameter of __Djy_VmEngine
    stw     r5, 0(r4)            //save the sp in the member top of vm
    msync
    //recover for the current thread
    lwz r5, 16(sp)
    lwz r6, 20(sp)
    lwz sp, 0(sp)
    blr
/*
@----复位老线程，切换到新线程-------------------------------------------------
@功能:  把old_vm的上下文重新初始化到新创建的状态，然后切换到新线程的
@       上下文中继续执行。
@       当一个在常驻内存的线程中处理的事件完成后，不能删除该线程，必须复位
@       该线程，然后切换到其他就绪线程，这时候就需要调用本函数。因此本函数在
@       被复位的线程上下文里面执行。
@参数:  thread_routine，被重置的线程函数
@       new_vm，目标线程的线程
@       old_vm，被复位的线程
@返回:  无
@函数原型:void __asm_reset_switch(void (*thread_routine)(void),
@                           struct  tagThreadVm *new_vm,struct  tagThreadVm *old_vm)@
@-----------------------------------------------------------------------------
*/
.text
.global __asm_reset_switch
__asm_reset_switch:

    //reset the old vm thread
    //get the vm stack top. then save the value to the stack
    lwz     r7, 0x04(r5)
    subi    r7, r7, CUR_STACK_SIZE
    xor     r6, r6, r6
    stw     r6, CR_OFFSET(r7)    //clear cr
    stw     r6, XER_OFFSET(r7)   //clear xer
    stw     r6, CTR_OFFSET(r7)   //clear counter
    stw     r6, LR_OFFSET(r7)    //clear lr
    lis     r6, CN_MSR_THREAD@h
    ori     r6, r6, CN_MSR_THREAD@l
    stw     r6, MSR_OFFSET(r7)   //reset the msr
    lis     r6, __Djy_VmEngine@h
    ori     r6, r6, __Djy_VmEngine@l
    stw     r6, PC_OFFSET(r7)    //set the pc
    evstdd  r3, R3_OFFSET(r7)    //set the r3(thread_routine) for the parameter of __Djy_VmEngine
    stw     r7, 0(r5)            //save the sp in the member top of vm
    msync
    //ok, now we'd better to recover the new vm stack to the registers.
    lwz     sp, 0(r4)            //get the new sp
    pop_current_state  srr0 srr1 //recover the registers here
    isync
    rfi                          //turn back to the new thread
    

/*
@----启动多事件调度-----------------------------------------------------------
@功能: 系统初始化完成后，启动多事件调度并切入第一个事件.
@参数: new_vm，首次启动的线程控制块指针
@返回: 无
@函数原型: void __asm_start_thread(struct  tagThreadVm  *new_vm)@
@-----------------------------------------------------------------------------
*/
.text
.global __asm_turnto_context
__asm_turnto_context:
.global __asm_start_thread
__asm_start_thread:
    //ok, now we'd better to recover the new vm stack to the registers.
    lwz     sp, 0(r3)            //get the new sp
    pop_current_state  srr0 srr1 //recover the registers here
    isync
    rfi                          //turn back to the new thread
/*
@----上下文切换---------------------------------------------------------------
@功能:  保存当前线程的上下文，切换到新线程的上下文中继续执行。
@参数:  new_sp，新上下文的栈指针
@参数:  old_sp，旧上下文的栈指针的指针，即&vm->stack。无需提供旧上下文栈指针，
@              sp寄存器的当前值就是
@返回:  无
@函数原型:
void __asm_switch_context(struct  tagThreadVm *new_vm,struct  tagThreadVm *old_vm)
@-----------------------------------------------------------------------------
*/
.text
.global __asm_switch_context
__asm_switch_context:
    push_current_state  srr0,srr1
    // set the vm msr bit in the msr
    lis r5,CN_MSR_THREAD@h
    ori r5,r5, CN_MSR_THREAD@l
    stw r5,MSR_OFFSET(sp)   
    //modified the pc register
    lis r5, ASM_THREAD_BACK@h
    ori r5, r5, ASM_THREAD_BACK@l
    stw r5, PC_OFFSET(sp)
    //save the sp in the old vm stack
    stw sp, 0(r4)
    msync
    //now, pop the new thread
    lwz sp, 0(r3)
    pop_current_state  srr0 srr1 //recover the registers here
    isync
    rfi                          //turn back to the new thread
    
ASM_THREAD_BACK: //when turn back ,the old thread will run here to turn back
    blr    
    
    
/*
@----中断中的上下文切换-------------------------------------------------------
@功能:  保存被中断线程的上下文，切换到新线程的上下文中继续执行。本函数虽然在中
@       断服务函数(非用户用int_isr_connect函数连接的中断服务函数)中，但在ARM
@       中，却运行在svc态
@参数:  new_sp，切换目标线程
@参数:  old_sp，被中断线程
@返回:  无
@函数原型: void __asm_switch_context_int(struct tagThreadVm *new_vm,struct tagThreadVm *old_vm)@
@-----------------------------------------------------------------------------
*/
.text
.global __asm_switch_context_int
__asm_switch_context_int:

    stwu sp, -32(sp)
    stw  r5, 16(sp)
    stw  r6, 20(sp)
    msync
    
    //just save the sp in the old vm stack, and changge it with the new sp
    lis r5, irq_top@h
    ori r5, r5, irq_top@l
    lwz r6, _VM_STACK_FROM_TOP(r5)
    stw r6, 0(r4)
    lwz r6, 0(r3)
    stw r6, _VM_STACK_FROM_TOP(r5)

    lwz  r5, 16(sp)
    lwz  r6, 20(sp)
    lwz  sp, 0(sp)

    blr  
/*
@-------------------------------------------------------------------------------
@功能: 汇编原子操作原型_start
@参数:
@返回:
@函数原型: hight_atom_t Int_HighAtomEnd()
@-------------------------------------------------------------------------------
*/
.text
.global Int_HighAtomStart
Int_HighAtomStart:
    stwu  sp, -32(sp)
    stw   r4, 4(sp)
    
    mfmsr r3
    lis   r4, (CN_MSR_TRUNK_DIS)@h
    ori   r4, r4, (CN_MSR_TRUNK_DIS)@l
    and   r4, r4, r3
    mtmsr r4
    
    lwz r4, 4(sp)
    lwz sp, 0(sp)
    blr

/*
@-------------------------------------------------------------------------------
@功能: 汇编原子操作原型_end
@参数:
@返回:
@函数原型: Int_HighAtomEnd(high_atom_t)
@-------------------------------------------------------------------------------
*/
.text
.global Int_HighAtomEnd
Int_HighAtomEnd:
    stwu  sp, -32(sp)
    
    mtmsr r3
    isync
    
    lwz sp, 0(sp)
    blr
/*
@-------------------------------------------------------------------------------
@功能: 汇编原子操作原型_start:低级原子操作
@参数:
@返回:
@函数原型: low_atom_t Int_LowAtomStart()
@-------------------------------------------------------------------------------
*/
.text
.global Int_LowAtomStart
Int_LowAtomStart:

    stwu  sp, -32(sp)
    stw   r4, 4(sp)
    
    mfmsr r3
    lis   r4, (CN_MSR_ASYN_DIS)@h
    ori   r4, r4, (CN_MSR_ASYN_DIS)@l
    and   r4, r4, r3
    mtmsr r4
    
    lwz r4, 4(sp)
    lwz sp, 0(sp)
    blr
/*
@-------------------------------------------------------------------------------
@功能: 汇编原子操作原型_end:低级原子操作
@参数:
@返回:
@函数原型:void Int_LowAtomEnd(low_atom_t)
@-------------------------------------------------------------------------------
*/
.text
.global Int_LowAtomEnd
Int_LowAtomEnd:
    stwu  sp, -32(sp)
    
    mtmsr r3
    isync
    
    lwz sp, 0(sp)
    blr


/*
@-------------------------------------------------------------------------------
@功能: 关异步信号
@参数:
@返回:
@函数原型:void Int_CutAsynSignal(void)
@-------------------------------------------------------------------------------
*/
.text
.global Int_CutAsynSignal
Int_CutAsynSignal:

    isync
    msync
    wrteei 0x00
    isync
    msync
    blr
/*
@-------------------------------------------------------------------------------
@功能: 开异步信号
@参数:
@返回:
@函数原型:void Int_ContactAsynSignal(void)
@-------------------------------------------------------------------------------
*/
.text
.global Int_ContactAsynSignal
Int_ContactAsynSignal:

    isync
    msync
    wrteei 0x01
    isync
    msync
    blr    
    
.end
//***************************file end*************************//






