/*
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
@所属模块:  中断模块
@作者：     lst
@版本：      V1.00
@初始版本完成日期：2009-08-10
@文件描述:  中断模块中的汇编部分
@其他说明:  无
@修订历史:
@2. ...
@1. 日期:
@   作者:
@   新版本号：
@   修改说明:
@------------------------------------------------------
*/
.syntax		unified
.cpu		cortex-m4
.fpu		softvfp
.thumb
.file	"ints.S"

/* *********************** ********** ***********************
;为什么要保存r4-r11呢，如果被中断的是线程a，而中断服务函数使高优先级的线程b就绪，
;就得返回到线程b中，而把线程a的r4-r11保存到线程a的上下文中，这两个操作都需要使用
;r4-r11，为啥不保存在栈中呢?，因为__asm_switch_context_int函数不知道栈顶在哪里。
;这在si版本中是没问题的，但可能在dlsp版本中出问题
*********************** ********** *********************** */
.text
.align 2
.global __start_asyn_signal
.thumb
.thumb_func
__start_asyn_signal:
    push    {r0,lr}				/* r0只是为了满足栈指针8字节对齐的填充物 */
    mrs     r0,psp
    stmfd   r0!,{r4-r11}        /* 保护寄存器,以及返回地址 */
    msr     psp,r0
    mrs     r0,ipsr             /* 取异常号 */
    sub     r0,r0,#16
    bl      __Int_EngineAsynSignal
    pop     {r0,lr}
    mrs     r0,psp
    ldmfd   r0!,{r4-r11}
    msr     psp,r0
    bx      lr

.text
.align 2
.global __start_real
.thumb
.thumb_func
__start_real:
    push    {r0,lr}				/* r0只是为了满足栈指针8字节对齐的填充物 */
    mrs     r0,psp
    stmfd   r0!,{r4-r11}        /* 保护寄存器,以及返回地址 */
    msr     psp,r0
    mrs     r0,ipsr             /* 取异常号 */
    sub     r0,r0,#16
    bl      __Int_EngineReal
    pop     {r0,lr}
    mrs     r0,psp
    ldmfd   r0!,{r4-r11}
    msr     psp,r0
    bx      lr

/*
@----进入低级原子操作---------------------------------------------------------
@功能：读取当前异步信号开关状态，然后禁止异步信号。低级级原子操作可用来防止临界
@      数据的并发访问，但可能会被实时中断打断。
@      Int_LowAtomStart---int_low_atom_end必须配套使用，绝对不能嵌套，在可能
@      嵌套的地方，必须使用int_save_asyn_signal来关闭中断(调度)。
@      这两函数可视作"Int_RestoreAsynSignal--Int_SaveAsynSignal"的快速版本，
@      除本身不能嵌套调用外，在被他们套住的代码块内，也不允许调用
@      int_restore_asyn_signal、int_save_asyn_signal中的任何一个。
@参数：无
@返回：原子状态
@备注：移植敏感
@函数原型:atom_low_t Int_LowAtomStart(void)
@-----------------------------------------------------------------------------
*/
.text
.align 2
.global Int_LowAtomStart
.thumb
.thumb_func
Int_LowAtomStart:
    mrs     r0, basepri
    mov     r1, #0xff
    msr     basepri, r1
    bx      lr

/*
@----离开低级原子操作---------------------------------------------------------
@功能：恢复进入前异步信号许可状态，本函数应与int_low_atom_start函数配套。
@      Int_LowAtomStart---int_low_atom_end必须配套使用，绝对不能嵌套，在可能
@      嵌套的地方，必须使用int_save_asyn_signal来关闭中断(调度)。
@      这两函数可视作"Int_RestoreAsynSignal--Int_SaveAsynSignal"的快速版本，
@      除本身不能嵌套调用外，在被他们套住的代码块内，也不允许调用
@      int_restore_asyn_signal、int_save_asyn_signal中的任何一个。
@参数：low，int_low_atom_start保存的原子状态
@返回：无
@备注：移植敏感
@函数原型:void Int_LowAtomEnd(atom_low_t low)
@-----------------------------------------------------------------------------
*/
.text
.align 2
.global Int_LowAtomEnd
.thumb
.thumb_func
Int_LowAtomEnd:
    msr     basepri, r0
    bx      lr

/*           
@----进入高级原子操作---------------------------------------------------------
@功能：读当前总中断状态，然后禁止总中断。高级原子操作是指期间不容许任何原因
@      打断的操作。
@      Int_HighAtomStart--int_high_atom_end必须配套使用，绝对不能嵌套，在可能
@      嵌套的地方，必须使用int_save_trunk来关闭中断(调度)。
@      这两函数可视作"Int_RestoreTrunk--Int_SaveTrunk"的快速版本，除本身不能
@      嵌套调用外，在被他们套住的代码块内，也不允许调用int_restore_trunk和
@      int_save_trunk中的任何一个。
@参数：无
@返回：原子状态
@备注：移植敏感
@函数原型:atom_high_t Int_HighAtomStart(void)
@-----------------------------------------------------------------------------
*/
.text
.align 2
.global Int_HighAtomStart
.thumb
.thumb_func
Int_HighAtomStart:
    mrs     r0, primask
    cpsid   i
    bx      lr

/*                   
@----离开高级原子操作---------------------------------------------------------
@功能：恢复进入前总中断状态，本函数应与int_high_atom_start函数配套
@      Int_HighAtomStart--int_high_atom_end必须配套使用，绝对不能嵌套，在可能
@      嵌套的地方，必须使用int_save_trunk来关闭中断(调度)。
@      这两函数可视作"Int_RestoreTrunk--Int_SaveTrunk"的快速版本，除本身不能
@      嵌套调用外，在被他们套住的代码块内，也不允许调用int_restore_trunk和
@      int_save_trunk中的任何一个。
@参数：high，int_high_atom_start保存的原子状态
@返回：无
@备注：移植敏感
@函数原型:void Int_HighAtomEnd(atom_high_t high)
@-----------------------------------------------------------------------------
*/
.text
.align 2
.global Int_HighAtomEnd
.thumb
.thumb_func
Int_HighAtomEnd:
    msr     primask, r0
    bx      lr


.end
