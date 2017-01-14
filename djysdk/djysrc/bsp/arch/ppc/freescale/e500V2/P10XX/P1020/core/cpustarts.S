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
// 模块描述: 对CPU做几本的初始化，为系统开始加载运行做准备
// 模块版本: V1.00
// 创建人员: Administrator
// 创建时间: 2013--下午5:14:20
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
.file  "CpuStarts.S"
#include "E500V2Reg.h"
#include "E500V2RegBits.h"
.align 4
.text
.global  __AsmRamStart
__AsmRamStart:

    //对于UBOOT给我们的初始化环境极度不信任
    //到此，我们开始运行DJYOS，我们希望一切都是新的，除了MMU
    //清除MSR,关闭一切中断和异常
    lis   r3, CN_MSR_INIT@h  //de  spe
    ori  r3, r3, CN_MSR_INIT@l
    mtmsr r3
    isync

    //设置sp为用户sp
    lis  sp, usr_top@h
    ori  sp,sp, usr_top@l
    subi sp, sp, 0x20


    //设置TCR，关闭对我们有威胁的DTIMER/FIXTIMER
    xor  r5, r5, r5
    mtspr tcr, r5
    //清除TSR标志，主要是怕TIMER以前有异常
    mtspr tsr, r5
    //clear the db exception bits
    mtspr  dbsr , r5
    isync
    //clear the esr exception
    mtspr  esr, r5
    isync

    //开启trap,不用开启trap了，我们根本不需要TRAP了，有SC就够了
    //dbcr0有很多debug异常使能，为了方便，所有的都关闭，包括trap
    lis  r6, cn_dbcr0_debug_mode@h
    ori r6, r6, cn_dbcr0_debug_mode@l
    mtspr  dbcr0,  r6
    isync

    //关于TIMER要用 的寄存器要清理一下吧，免留后患
    //以后需要用什么，自己弄什么，只相信自己
    //clear all the hid
    //OPERATE THE HIDX NEED SYNC OPERATE
    xor  r3, r3, r3
    msync
    isync
    mtspr  hid0, r3
    isync
    msync
    isync
    mtspr  hid1, r3
    isync
    //打开EMCP,使能mcp产生MC异常
    mfspr  r3, hid0
    lis    r4, HID0_EMCP@h
    ori    r4, r4, HID0_EMCP@l
    or     r3, r4, r3
    msync
    isync
    mtspr  hid0, r3
    isync
    //打开总线MC异常，让ASSERT_sreset产生core_fault_in信号进入core产生异常
    mfspr  r3, hid1
    lis    r4, HID1_RFXE@h
    ori    r4, r4, HID1_RFXE@l
    or     r3, r4, r3
    msync
    isync
    mtspr  hid1, r3
    isync
    //清除一些重要的计数器
    xor  r6, r6, r6
    mtspr dec, r6
    isync
    mtspr  decar, r6
    isync
    mtspr tbl_write, r6
    isync
    mtspr tbu_write, r6
    isync
    mtspr tcr, r6
    isync
    //设置spefscr
    xor r3, r3, r3
    li  r3, 0x3C
    mtspr spefscr, r3
    isync


    //do the cpu init
    bl CpuInit
    //飞翔吧，少年！
    //start the os code
    bl  Load_Preload

  /*
@-------------------------------------------------------------------------------
@功能: 关L1cache
@参数:
@返回:
@函数原型:__AsmDisL1Cache(void)
@-------------------------------------------------------------------------------
*/
/*first invalidate, then disable*/
//first icache, the dcache
//invalidate l1icache
.global  __AsmDisL1Cache
__AsmDisL1Cache:

    stwu sp, -32(sp)
    stw  r0, 8(sp)
    stw  r3, 12(sp)
    stw  r4, 16(sp)
    stw  r5, 20(sp)
    msync
    //first disable it, then invalid it
    //icache
    lis      r5, cn_l1csr1_ice@h
    ori      r5, r5, cn_l1csr1_ice@l
    mfspr    r4, l1csr1
    andc     r4, r4, r5
    mtspr    l1csr1, r4
    lis      r3, cn_l1csr1_icfi@h
    ori      r3, r3, cn_l1csr1_icfi@l
    isync
    msync
    mtspr    l1csr1, r3
l1i_invalid_loop:
    mfspr    r4, l1csr1
    and.     r4, r4, r3
    bgt      l1i_invalid_loop

    //dcache
    lis      r5, cn_l1csr0_ce@h
    ori      r5, r5, cn_l1csr0_ce@l
    mfspr    r4, l1csr0
    andc     r4, r4, r5
    mtspr    l1csr0, r4
    lis      r3, cn_l1csr0_cfi@h
    ori      r3, r3, cn_l1csr0_cfi@l
    isync
    msync
    mtspr    l1csr1, r3
l1d_invalid_loop:
    mfspr    r4, l1csr0
    and.     r4, r4, r3
    bgt      l1i_invalid_loop


    lwz r0, 8(sp)
    lwz r3, 12(sp)
    lwz r4, 16(sp)
    lwz r5,20(sp)
    lwz sp,0(sp)
    blr

.end



