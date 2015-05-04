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
// 模块描述:异常向量表
// 模块版本: V1.00(初始化版本)
// 创建人员:Administrator
// 创建时间:2013-7-26--上午10:15:54
//         ：该模块实现的是机器运行的第一条指令
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================

.file   "Exp_Vec.S"
#include  "E500v2Reg.h"
#include  "E500v2RegBits.h"
#include  "VMStack.h"
 /*
@-------------------------------------------------------------------------------
@功能: 从此开始，按照DJY的需求初始化CPU，做必要的准备
@参数: 无
@返回: 无
@-------------------------------------------------------------------------------
*/

.macro  vector_align
    .align  8
    .endm

/* 异常向量表 */
.align  8
.global   exception_tab
exception_tab:
.long 0x12345678
.align  8

.global IVOR0_OFFSET
IVOR0_OFFSET:
    b     .
    mtspr  spr4, sp
    xor    sp, sp, sp
    li     sp, cn_majorexp_critical_input@l
    mtspr  spr5, sp
    lis    sp, cint_exp_top@h
    ori    sp, sp, cint_exp_top@l
    subi   sp, sp, _IRQ_TOP_OFFSET
    b   Default_cint_exception
    /*CRITICAL INPUT*/

    vector_align
.global IVOR1_OFFSET
IVOR1_OFFSET:
    b  .
    mtspr  spr4, sp
    xor    sp, sp, sp
    li     sp, cn_majorexp_machine_check@l
    mtspr  spr5, sp
    lis    sp, sys_exp_top@h
    ori    sp, sp, sys_exp_top@l
    subi   sp, sp, _IRQ_TOP_OFFSET
    b   Default_machinecheck_exception
    /*MACHINE CHECK */

    vector_align
.global IVOR2_OFFSET
IVOR2_OFFSET:
    b  .
    mtspr  spr4, sp
    xor    sp, sp, sp
    li     sp, cn_majorexp_data_storage@l
    mtspr  spr5, sp
    lis    sp, sys_exp_top@h
    ori    sp, sp, sys_exp_top@l
    subi   sp, sp, _IRQ_TOP_OFFSET
    b   Default_sys_general_exception
     /*DATA STORAGE */

    vector_align
.global IVOR3_OFFSET
IVOR3_OFFSET:
    b  .
    mtspr  spr4, sp
    xor    sp, sp, sp
    li     sp, cn_majorexp_instruction_storage@l
    mtspr  spr5, sp
    lis    sp, sys_exp_top@h
    ori    sp, sp, sys_exp_top@l
    subi   sp, sp, _IRQ_TOP_OFFSET
    b   Default_sys_general_exception
    /*INSTRUCTION STORAGE */

    vector_align
.global IVOR4_OFFSET
IVOR4_OFFSET:
    b   Default_externalint_exception
    /*EXTERNAL INT EXCEPTION */

    vector_align
.global IVOR5_OFFSET
IVOR5_OFFSET:
    b  .
    mtspr  spr4, sp
    xor    sp, sp, sp
    li     sp, cn_majorexp_alignment@l
    mtspr  spr5, sp
    lis    sp, sys_exp_top@h
    ori    sp, sp, sys_exp_top@l
    subi   sp, sp, _IRQ_TOP_OFFSET
    b   Default_sys_general_exception
    /*ALIGNMENT EXCEPTION */

    vector_align
.global IVOR6_OFFSET
IVOR6_OFFSET:
    b      .
    mtspr  spr4, sp
    xor    sp, sp, sp
    li     sp, cn_majorexp_program@l
    mtspr  spr5, sp
    lis    sp, sys_exp_top@h
    ori    sp, sp, sys_exp_top@l
    subi   sp, sp, _IRQ_TOP_OFFSET
    b   Default_sys_general_exception
    /*PROGRAM EXCEPTION*/

    vector_align
.global IVOR7_OFFSET
IVOR7_OFFSET:

    mtspr  spr4, sp
    xor    sp, sp, sp
    li     sp, cn_majorexp_reserved_7@l
    mtspr  spr5, sp
    lis    sp, sys_exp_top@h
    ori    sp, sp, sys_exp_top@l
    subi   sp, sp, _IRQ_TOP_OFFSET
    b   Default_sys_general_exception
    /*RESERVED  EXCEPTION*/

    vector_align
.global IVOR8_OFFSET
IVOR8_OFFSET:
    b  .
    mtspr  spr4, sp
    xor    sp, sp, sp
    li     sp, cn_majorexp_syscall@l
    mtspr  spr5, sp
    lis    sp, sys_exp_top@h
    ori    sp, sp, sys_exp_top@l
    subi   sp, sp, _IRQ_TOP_OFFSET
    b   Default_sys_general_exception
    /*SYSTEM CALL EXCEPTION*/

    vector_align
.global IVOR9_OFFSET
IVOR9_OFFSET:
    b  .
    mtspr  spr4, sp
    xor    sp, sp, sp
    li     sp, cn_majorexp_reserved_9@l
    mtspr  spr5, sp
    lis    sp, sys_exp_top@h
    ori    sp, sp, sys_exp_top@l
    subi   sp, sp, _IRQ_TOP_OFFSET
    b   Default_sys_general_exception
     /* RESERVED EXCEPTION */

    vector_align
.global IVOR10_OFFSET
IVOR10_OFFSET:
    b   Default_tick_exception
    /*DECREMENT TIMER EXCEPTION*/

    vector_align
.global IVOR11_OFFSET
IVOR11_OFFSET:

    mtspr  spr4, sp
    xor    sp, sp, sp
    li     sp, cn_majorexp_fixed_timer@l
    mtspr  spr5, sp
    lis    sp, sys_exp_top@h
    ori    sp, sp, sys_exp_top@l
    subi   sp, sp, _IRQ_TOP_OFFSET
    b   .
    /*FIXED TIMER EXCEPTION*/

    vector_align
.global IVOR12_OFFSET
IVOR12_OFFSET:
    mtspr  spr4, sp
    xor    sp, sp, sp
    li     sp, cn_majorexp_watch_dog_timer@l
    mtspr  spr5, sp
    lis    sp, cint_exp_top@h
    ori    sp, sp, cint_exp_top@l
    subi   sp, sp, _IRQ_TOP_OFFSET
    b      .  //not implement yet--TODO
    /*WATCH TIMER EXCEPTION*/

    vector_align
.global IVOR13_OFFSET
IVOR13_OFFSET:
    b  .
    mtspr  spr4, sp
    xor    sp, sp, sp
    li     sp, cn_majorexp_data_tlb_error@l
    mtspr  spr5, sp
    lis    sp, sys_exp_top@h
    ori    sp, sp, sys_exp_top@l
    subi   sp, sp, _IRQ_TOP_OFFSET
    b   Default_sys_general_exception
    /*DATA TLB ERROR EXCEPTION*/

    vector_align
.global IVOR14_OFFSET
IVOR14_OFFSET:
    b  .
    mtspr  spr4, sp
    xor    sp, sp, sp
    li     sp, cn_majorexp_instruction_tlb_error@l
    mtspr  spr5, sp
    lis    sp, sys_exp_top@h
    ori    sp, sp, sys_exp_top@l
    subi   sp, sp, _IRQ_TOP_OFFSET
    b   Default_sys_general_exception
    /*INSTRUCTION TLB ERROR EXCEPTION*/

    vector_align
.global IVOR15_OFFSET
IVOR15_OFFSET:
   /*
    mtspr  spr4, sp
    xor    sp, sp, sp
    li     sp, cn_majorexp_debug@l
    mtspr  spr5, sp
    lis    sp, dbg_exp_top@h
    ori    sp, sp, dbg_exp_top@l
    subi   sp, sp, _IRQ_TOP_OFFSET
    b   Default_DEBUG_exception
    */
    /*INSTRUCTION TLB ERROR EXCEPTION*/
    b   Default_debug_exception
    /*DEBUG EXCEPTION*/

    vector_align
.global IVOR32_OFFSET
IVOR32_OFFSET:
    mtspr  spr4, sp
    xor    sp, sp, sp
    li     sp, cn_majorexp_speapu_unavailable@l
    mtspr  spr5, sp
    lis    sp, sys_exp_top@h
    ori    sp, sp, sys_exp_top@l
    subi   sp, sp, _IRQ_TOP_OFFSET
    b   Default_sys_general_exception
    /*SPE APU UNAVAILABLE EXCEPTION*/

    vector_align
.global IVOR33_OFFSET
IVOR33_OFFSET:
    mtspr  spr4, sp
    xor    sp, sp, sp
    li     sp, cn_majorexp_embeded_fp_data@l
    mtspr  spr5, sp
    lis    sp, sys_exp_top@h
    ori    sp, sp, sys_exp_top@l
    subi   sp, sp, _IRQ_TOP_OFFSET
    b   Default_sys_general_exception
    /*EMBEDED FLOAT POINT DATA EXCEPTION*/

    vector_align
.global IVOR34_OFFSET
IVOR34_OFFSET:
    mtspr  spr4, sp
    xor    sp, sp, sp
    li     sp, cn_majorexp_embeded_fp_round@l
    mtspr  spr5, sp
    lis    sp, sys_exp_top@h
    ori    sp, sp, sys_exp_top@l
    subi   sp, sp, _IRQ_TOP_OFFSET
    b   Default_sys_general_exception
    /*EMBEDED FLOAT POINT ROUND EXCEPTION*/

    vector_align
.global IVOR35_OFFSET
IVOR35_OFFSET:
    b   .
    /*PERFORMANCE MONITOR EXCEPTION*/
    
    
/*
@-------------------------------------------------------------------------------
@功能:设置异常向量表
@参数:无
@返回:无
@函数原型: void __AsmSetExpVecTab(void)
@-------------------------------------------------------------------------------
*/
.global __AsmSetExpVecTab
__AsmSetExpVecTab:
    //set the exception tab
    //so we should better disable all the interrupt anyway
    stwu sp, -32(sp)
    stw  r3, 8(sp)

    xor   r3, r3, r3
    li r3, IVOR0_OFFSET@l
    mtspr  IVOR0, r3

    li r3, IVOR1_OFFSET@l
    mtspr  IVOR1, r3

    li r3, IVOR2_OFFSET@l
    mtspr  IVOR2, r3

    li r3, IVOR3_OFFSET@l
    mtspr  IVOR3, r3

    li r3, IVOR4_OFFSET@l
    mtspr  IVOR4, r3

    li r3, IVOR5_OFFSET@l
    mtspr  IVOR5, r3

    li r3, IVOR6_OFFSET@l
    mtspr  IVOR6, r3

    li r3, IVOR7_OFFSET@l
    mtspr  IVOR7, r3

    li r3, IVOR8_OFFSET@l
    mtspr  IVOR8, r3

    li r3, IVOR9_OFFSET@l
    mtspr  IVOR9, r3

    li r3, IVOR10_OFFSET@l
    mtspr  IVOR10, r3

    li r3, IVOR11_OFFSET@l
    mtspr  IVOR11, r3

    li r3, IVOR12_OFFSET@l
    mtspr  IVOR12, r3

    li r3, IVOR13_OFFSET@l
    mtspr  IVOR13, r3

    li r3, IVOR14_OFFSET@l
    mtspr  IVOR14, r3

    li r3, IVOR15_OFFSET@l
    mtspr  IVOR15, r3

    li r3, IVOR32_OFFSET@l
    mtspr  IVOR32, r3

    li r3, IVOR33_OFFSET@l
    mtspr  IVOR33, r3

    li r3, IVOR34_OFFSET@l
    mtspr  IVOR34, r3

    li r3, IVOR35_OFFSET@l
    mtspr  IVOR35, r3

    isync
    msync

    //设置主偏移量
    lis   r3, exception_tab@h
    ori   r3, r3, exception_tab@l
    mtspr   IVPR,r3
    isync
    msync

    lwz sp, 0(sp)
    lwz r3, 8(sp)
    blr    

.align 16
.section    .stack.usr, "aw", %nobits
.align  16
    .space  0x10000
.section    .stack.irq, "aw", %nobits
.align  16
    .space  0x10000

.section    .stack.prg_exp, "aw", %nobits
.align  16
    .space  0x10000

.section    .stack.dbg_exp, "aw", %nobits
.align  16
    .space  0x10000

.section    .stack.cint_exp, "aw", %nobits
.align  16
    .space  0x10000

.section    .stack.mc_exp, "aw", %nobits
.align  16
    .space  0x10000

.section    .stack.sys_exp, "aw", %nobits
.align  16
    .space  0x10000
    
.section    .stack.core1_stack, "aw", %nobits
.align  16
    .space  0x10000

.end

