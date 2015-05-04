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
// 模块描述:P1020CPU的一些功能函数
// 模块版本: V1.00(初始化版本)
// 创建人员:zhangqf_cyg
// 创建时间:2013-7-26--上午10:15:29
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 说明：本文件实现一些在C函数中不好实现的函数，特别是涉及到寄存器操作
// 关于线程切换：在E500v2的核心当中，我们是依赖于SC异常来实现的，
// 后续考虑不使用SC异常，把该异常释放给别的重要操作使用，敬请期待--TODO
.file   "cpus.S"

#include "E500v2Reg.h"
#include "E500v2RegBits.h"
#include "Guts.h"
/*
@-------------------------------------------------------------------------------
@功能: 复位tb，并使能，主要是测延时使用，好吧我们使用的是TBCLK，
@参数:  无()
@参数:  无
@返回:  无
@函数原型: void __asm_reset_the_tb(void)
@          use the tbclk
@-------------------------------------------------------------------------------
*/
//hid0---BITS 50 SEL,0 USE BUSCLK/8 to update, 1 USE  TBCLK(rtc)
//    ---BITS 49 EN, 0 DISABLE, 1 ENBAEL
.text
.global __asm_reset_the_tb
__asm_reset_the_tb:

    stwu   sp, -32(sp)
    stw    r3, 8(sp)
    stw    r4, 12(sp)
    stw    r5, 16(sp)
    msync

    mfspr  r3, hid0
    lis    r4, HID0_TBEN@h
    ori    r4, r4, HID0_TBEN@l
    andc   r3, r3, r4 //stop the tb

    isync
    msync
    mtspr  hid0, r3
    isync//OPERATE THE HIDX NEED THE SYNC OPERATION BEFORE AND POST

    xor     r5, r5, r5
    mtspr   tbl_write, r5
    isync
    mtspr   tbu_write, r5
    isync
    //enable the tb
    or      r3, r4,  r3
    isync
    msync
    mtspr  hid0, r3
    isync//OPERATE THE HIDX NEED THE SYNC OPERATION BEFORE AND POST

    lwz  r3,  8(sp)
    lwz  r4,  12(sp)
    lwz  r5,  16(sp)
    lwz  sp,  0(sp)

    blr               //return

 /*
@-------------------------------------------------------------------------------
@功能: 获取TB时间
@参数: value_h32, value_l32,分别用于存储TB的高32位和低32位
@返回:  无
@函数原型: void __asm_get_tb_value(u32 *value_h32, u32 *value_l32)
@-------------------------------------------------------------------------------
*/
.text
.global __asm_get_tb_value
__asm_get_tb_value:

    stwu   sp, -32(sp)
    stw    r6, 8(sp)
    stw    r7,12(sp)
    stw    r8,16(sp)
    msync

 get_tb_loop:
    mfspr   r6, tbu_read
    mfspr   r7, tbl_read
    mfspr   r8, tbu_read
    cmplw   r6, r8
    bgt     get_tb_loop  //发生进位，再读一次呗

    stw     r6, 0(r3)
    stw     r7, 0(r4)

    lwz    r6, 8(sp)
    lwz    r7, 12(sp)
    lwz    r8, 16(sp)
    lwz    sp, 0(sp)

    //获取TB时间不应该停表，不然这时候要发生异常，则表不走，TICK不会产生，则系统会发生
    //瘫痪
    blr               //return
 /*
@-------------------------------------------------------------------------------
@功能: 初始化减计数器
@参数: value_tick
@返回:  无
@函数原型: void __asm_init_tick(u32 value_tick)
@-------------------------------------------------------------------------------
*/
.text
.global __asm_init_Dtimer
__asm_init_Dtimer:

    stwu  sp, -32(sp)
    stw   r4, 8(sp)
    stw   r5, 12(sp)
    stw   r6, 16(sp)
    stw   r7, 20(sp)
    stw   r8, 24(sp)
    msync

    //diable the tb
    mfspr r6, hid0
    lis   r7, HID0_TBEN@h
    ori   r7, r7, HID0_TBEN@l
    andc  r6, r6, r7

    isync
    msync
    mtspr  hid0, r6
    isync//OPERATE THE HIDX NEED THE SYNC OPERATION BEFORE AND POST

    //set the tick value
    //for the test,make the counter bigger
    mtspr  dec, r3
    isync
    mtspr  decar, r3
    isync

    //no need to set the tcr,the vm will do it itself
    //just enable reload, and tick int will be dealed in asyn signal
    lis    r3,  CN_TCR_DEFAULT@h  //bit37 IE  bit41 ARE
    ori    r3, r3, CN_TCR_DEFAULT@l
    mfspr  r4, tcr
    or     r4, r4, r3
    mtspr  tcr, r4
    isync

    mfspr   r6, hid0
    or      r6, r6, r7
    isync
    msync
    mtspr  hid0, r6
    isync             //OPERATE THE HIDX NEED THE SYNC OPERATION BEFORE AND POST

    lwz   r4, 8(sp)
    lwz   r5, 12(sp)
    lwz   r6, 16(sp)
    lwz   r7, 20(sp)
    lwz   r8, 24(sp)
    lwz   sp, 0(sp)

    blr               //return  
/*
@-------------------------------------------------------------------------------
@功能:获取MC相关寄存器,用于调试诊断
@参数:存储地址
@返回:无
@函数原型:
@-------------------------------------------------------------------------------
*/
.global __asm_get_mc_registers
__asm_get_mc_registers:
    mfspr r4, mcsrr0
    stw   r4, 0(r3)
    mfspr r4, mcsrr1
    stw   r4, 0x04(r3)
    mfspr r4, mcsr
    stw   r4, 0x08(r3)
    mfspr r4, mcar
    stw   r4, 0x0c(r3)
    mfspr r4, hid0
    stw   r4, 0x10(r3)
    mfspr r4, hid1
    stw   r4, 0x14(r3)
    isync
    msync
    blr

/*
@-------------------------------------------------------------------------------
@功能:汇编，重新启动
@参数:void
@返回:void
@函数原型: void __asm_reboot(void )
@说明：机器软重启，即启动到版本检测位置，即__check_boot_version
@      记得window装系统的时候，一直按着某一个功能键，然后进入到选择启动位置界面，
@      这就是跳到哪一个选择哪一个系统启动的地方，这么说你明白了吧
@-------------------------------------------------------------------------------
*/
.global reboot
reboot:
    isync
    msync
    wrteei 0x00  //must disable all the unpredict interrupts
    lis r3,  __AsmRamStart@h
    ori r3, r3, __AsmRamStart@l
    mtlr  r3
    isync
    dcbst r0, r3
    msync
    icbi  r0, r3
    msync
    isync
    blr
    b .     //不可能到达的位置
    blr     //保持程序的一致性
/*
@-------------------------------------------------------------------------------
@功能:汇编，复位
@参数:void
@返回:void
@函数原型: void __asm_reset(void )
@说明：那个我们按下我们的power键，那个什么这个就是那个效果，我想你懂得
@-------------------------------------------------------------------------------
*/
.global reset
reset:
    isync
    msync
    wrteei 0x00  //must disable all the unpredict interrupts
//利用全局复位模块进行复位
    lis r3, cn_p1020_guts_rstcr@h
    ori r3, r3, cn_p1020_guts_rstcr@l
    li  r4, cn_p1020_guts_rstcr_hardreset_r
    stw r4, 0(r3)

    msync
    isync
    lwz r4, 0(r3)
    isync
    msync    //OPERATE THE CCSRBAR REGISTER NEED SOME SYNC OPERATION:SYNC AND READ
    b   .  //hung here will do some wait until the power reset
    blr
/*
@-------------------------------------------------------------------------------
@功能:汇编，复位
@参数:void
@返回:void
@函数原型: void __asm_reset(void )
@说明：那个我们按下我们的power键，那个什么这个就是那个效果，我想你懂得
@-------------------------------------------------------------------------------
*/
.global restart_system
restart_system:
    isync
    msync
    wrteei 0x00  //must disable all the unpredict interrupts
//利用全局复位模块进行复位
    lis r3, cn_p1020_guts_rstcr@h
    ori r3, r3, cn_p1020_guts_rstcr@l
    li  r4, cn_p1020_guts_rstcr_hardreset_r
    stw r4, 0(r3)

    msync
    isync
    lwz r4, 0(r3)
    isync
    msync    //OPERATE THE CCSRBAR REGISTER NEED SOME SYNC OPERATION:SYNC AND READ
    b   .  //hung here will do some wait until the power reset
    blr
/*
@-------------------------------------------------------------------------------
@function:找出U32中第一个为1的bit位（从高位到低位寻找）
@para:para,待分析的数据
@return: r3 返回读取的数据
@function modle:unsigned char   __asm_find_f1bits(u32 para)
@-------------------------------------------------------------------------------
*/
.global __asm_find_f1bits
__asm_find_f1bits:
    cntlzw  r3, r3
    blr
    
/*
@-------------------------------------------------------------------------------
@function:从CCSR寄存器读取数据或者 写入数据（8 16 32）
@para:用R3传递地址和返回结果（如果有），R4传输数据（写入的时候）
@return:
@-------------------------------------------------------------------------------
*/
.global  read8
read8:
    isync
    msync
    lbz  r3, 0(r3)
    isync
    msync
    blr  
.global  read16
read16:
    isync
    msync
    lhz  r3, 0(r3)
    isync
    msync
    blr 
.global  read32
read32:
    isync
    msync
    lwz  r3, 0(r3)
    isync
    msync
    blr    
    
.global  write8
write8:
    isync
    msync
    stb  r4, 0(r3)
    isync
    msync
    blr  
.global  write16
write16:
    isync
    msync
    sth  r4, 0(r3)
    isync
    msync
    blr  
.global  write32
write32:
    isync
    msync
    stw  r4, 0(r3)
    isync
    msync
    blr  
   
/*
@-------------------------------------------------------------------------------
@function:set the mas3 property of assigned tlb by the address
@para:property: to set to mas3, address, the tlb block in the address
@return:NULL
@function modle:void __AsmSetTlbMas3Pro(u32 property, u32 address)
@-------------------------------------------------------------------------------
*/
.global  __AsmSetTlbMas3Pro
__AsmSetTlbMas3Pro:

    stwu  sp, -16(sp)
    stw   r5,  4(sp)
    msync
    
    xor  r5, r5, r5
    mtspr   MAS6, r5    /* Assume the current PID and AS are 0 */
    isync
    msync
    tlbsx   0, r4       /* This must succeed */
    isync
    //set to the property we need
    mtspr   MAS3, r3
    isync
    msync
    tlbwe
    isync
    msync

    lwz r5, 4(sp)
    lwz sp, 0(sp)
    blr
/*
@-------------------------------------------------------------------------------
@function:get the mas3 property of assigned tlb by address
@para:address, used to assign the tlb
@return:the property of mas3
@function modle:u32 __AsmGetTlbMas3Pro( u32 address)
@-------------------------------------------------------------------------------
*/
.global  __AsmGetTlbMas3Pro
__AsmGetTlbMas3Pro:

    stwu  sp, -16(sp)
    stw   r4,  4(sp)
    msync
    isync

    xor r4, r4, r4
    mtspr   MAS6, r4    /* Assume the current PID and AS are 0 */
    isync
    msync
    tlbsx   0, r3      /* This must succeed */
    isync
    //get to the property we need
    mfspr   r3, MAS3
    isync

    lwz r4, 4(sp)
    lwz sp, 0(sp)
    blr
    
.end
/****************************file end*********************************/
