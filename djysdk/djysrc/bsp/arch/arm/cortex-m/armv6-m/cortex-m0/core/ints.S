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
//版本：      V1.00
//初始版本完成日期：2009-08-10
//文件描述:  中断模块中的汇编部分
//其他说明:  无
//修订历史:
//2. ...
//1. 日期:
//   作者:
//   新版本号：
//   修改说明:
//------------------------------------------------------

.syntax		unified
.cpu		cortex-m0
.fpu		softvfp
.thumb
.file	"ints.S"

.extern  __Int_EngineAsynSignal

@为什么要保存r4-r11呢，如果被中断的是线程a，而中断服务函数使高优先级的线程b就绪，
@就得返回到线程b中，而把线程a的r4-r11保存到线程a的上下文中，这两个操作都需要使用
@r4-r11，为啥不保存在栈中呢?，因为__asm_switch_context_int函数不知道栈顶在哪里。
@这在si版本中是没问题的，但可能在dlsp版本中出问题
.text
.align 2
.global __start_int
.thumb
.thumb_func
__start_int:
    push    {r0,lr}				@r0只是为了满足栈指针8字节对齐的填充物
    mrs     r1,psp
    subs    r1,r1,#8*4          @后退8个寄存器,用于保存r4~r11
    msr     psp,r1
    stmia   r1!,{r4-r7}         @保护寄存器,调用tick异常可能引起线程切换。
    mov     r2,r8
    mov     r3,r9
    mov     r4,r10
    mov     r5,r11
    stmia   r1!,{r2-r5}         @保护寄存器,中断可能引起线程切换。
    bl      __Int_EngineAsynSignal
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
.global __start_int_0
.thumb
.thumb_func
__start_int_0:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#0              @取异常号
    
    ldr     r1,=(vector_table+0*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_1
.thumb
.thumb_func
__start_int_1:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#1              @取异常号
    
    ldr     r1,=(vector_table+1*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_2
.thumb
.thumb_func
__start_int_2:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#2              @取异常号
    
    ldr     r1,=(vector_table+2*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_3
.thumb
.thumb_func
__start_int_3:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#3              @取异常号
    
    ldr     r1,=(vector_table+3*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_4
.thumb
.thumb_func
__start_int_4:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#4              @取异常号
    
    ldr     r1,=(vector_table+4*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_5
.thumb
.thumb_func
__start_int_5:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#5              @取异常号
    
    ldr     r1,=(vector_table+5*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_6
.thumb
.thumb_func
__start_int_6:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#6              @取异常号
    
    ldr     r1,=(vector_table+6*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_7
.thumb
.thumb_func
__start_int_7:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#7              @取异常号
    
    ldr     r1,=(vector_table+7*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_8
.thumb
.thumb_func
__start_int_8:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#8              @取异常号
    
    ldr     r1,=(vector_table+8*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_9
.thumb
.thumb_func
__start_int_9:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#9              @取异常号
    
    ldr     r1,=(vector_table+9*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_10
.thumb
.thumb_func
__start_int_10:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#10              @取异常号
    
    ldr     r1,=(vector_table+10*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_11
.thumb
.thumb_func
__start_int_11:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#11              @取异常号
    
    ldr     r1,=(vector_table+11*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_12
.thumb
.thumb_func
__start_int_12:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#12              @取异常号
    
    ldr     r1,=(vector_table+12*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_13
.thumb
.thumb_func
__start_int_13:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#13              @取异常号
    
    ldr     r1,=(vector_table+13*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_14
.thumb
.thumb_func
__start_int_14:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#14              @取异常号
    
    ldr     r1,=(vector_table+14*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_15
.thumb
.thumb_func
__start_int_15:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#15              @取异常号
    
    ldr     r1,=(vector_table+15*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_16
.thumb
.thumb_func
__start_int_16:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#16              @取异常号
    
    ldr     r1,=(vector_table+16*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_17
.thumb
.thumb_func
__start_int_17:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#17              @取异常号
    
    ldr     r1,=(vector_table+17*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_18
.thumb
.thumb_func
__start_int_18:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#18              @取异常号
    
    ldr     r1,=(vector_table+18*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_19
.thumb
.thumb_func
__start_int_19:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#19              @取异常号
    
    ldr     r1,=(vector_table+19*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_20
.thumb
.thumb_func
__start_int_20:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#20              @取异常号
    
    ldr     r1,=(vector_table+20*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_21
.thumb
.thumb_func
__start_int_21:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#21              @取异常号
    
    ldr     r1,=(vector_table+21*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_22
.thumb
.thumb_func
__start_int_22:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#22              @取异常号
    
    ldr     r1,=(vector_table+22*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_23
.thumb
.thumb_func
__start_int_23:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#23              @取异常号
    
    ldr     r1,=(vector_table+23*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_24
.thumb
.thumb_func
__start_int_24:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#24              @取异常号
    
    ldr     r1,=(vector_table+24*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_25
.thumb
.thumb_func
__start_int_25:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#25              @取异常号
    
    ldr     r1,=(vector_table+25*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_26
.thumb
.thumb_func
__start_int_26:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#26              @取异常号
    
    ldr     r1,=(vector_table+26*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_27
.thumb
.thumb_func
__start_int_27:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#27              @取异常号
    
    ldr     r1,=(vector_table+27*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_28
.thumb
.thumb_func
__start_int_28:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#28              @取异常号
    
    ldr     r1,=(vector_table+28*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_29
.thumb
.thumb_func
__start_int_29:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#29              @取异常号
    
    ldr     r1,=(vector_table+29*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_30
.thumb
.thumb_func
__start_int_30:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#30              @取异常号
    
    ldr     r1,=(vector_table+30*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.text
.align 2
.global __start_int_31
.thumb
.thumb_func
__start_int_31:
    push    {r0,lr}			    @r0只是为了满足栈指针8字节对齐的填充物
    movs    r0,#31              @取异常号
    
    ldr     r1,=(vector_table+31*4) 
    ldr     r1,[r1]
    blx     r1
    pop     {r0,pc}

.end
