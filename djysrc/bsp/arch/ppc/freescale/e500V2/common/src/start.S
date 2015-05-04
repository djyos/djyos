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
// 模块描述:PPC最开始映射的4KB代码
// 模块版本:
// 创建人员: Administrator
// 创建时间: 下午4:34:27/2013-9-10
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 备注：建立好空间1的C环境，方便在空间1中进行MMU操作
#include "E500V2Reg.h"
#include "E500V2RegBits.h"
#include "SysSpace.h"  //SYS的空间分配，整个4G空间
/*
 * e500 Startup -- after reset only the last 4KB of the effective
 * address space is mapped in the MMU L2 TLB1 Entry0. The .bootpg
 * section is located at THIS LAST page and basically does three
 * things: clear some registers, set up exception tables and
 * add more TLB entries for 'larger spaces'(e.g. the boot rom) to
 * continue the boot procedure.

 * Once the boot rom is mapped by TLB entries we can proceed
 * with normal startup.
 *
 */
    .macro  create_tlb1_entry esel ts tsize epn wimg rpn perm phy_high scratch
    lis \scratch, FSL_BOOKE_MAS0(1, \esel, 0)@h
    ori \scratch, \scratch, FSL_BOOKE_MAS0(1, \esel, 0)@l
    mtspr   MAS0, \scratch
    lis \scratch, FSL_BOOKE_MAS1(1, 1, 0, \ts, \tsize)@h
    ori \scratch, \scratch, FSL_BOOKE_MAS1(1, 1, 0, \ts, \tsize)@l
    mtspr   MAS1, \scratch
    lis \scratch, FSL_BOOKE_MAS2(\epn, \wimg)@h
    ori \scratch, \scratch, FSL_BOOKE_MAS2(\epn, \wimg)@l
    mtspr   MAS2, \scratch
    lis \scratch, FSL_BOOKE_MAS3(\rpn, 0, \perm)@h
    ori \scratch, \scratch, FSL_BOOKE_MAS3(\rpn, 0, \perm)@l
    mtspr   MAS3, \scratch
    lis \scratch, \phy_high@h
    ori \scratch, \scratch, \phy_high@l
    mtspr   MAS7, \scratch
    isync
    msync
    tlbwe
    isync
    .endm


    .macro  create_tlb0_entry esel ts tsize epn wimg rpn perm phy_high scratch
    lis \scratch, FSL_BOOKE_MAS0(0, \esel, 0)@h
    ori \scratch, \scratch, FSL_BOOKE_MAS0(0, \esel, 0)@l
    mtspr   MAS0, \scratch
    lis \scratch, FSL_BOOKE_MAS1(1, 0, 0, \ts, \tsize)@h
    ori \scratch, \scratch, FSL_BOOKE_MAS1(1, 0, 0, \ts, \tsize)@l
    mtspr   MAS1, \scratch
    lis \scratch, FSL_BOOKE_MAS2(\epn, \wimg)@h
    ori \scratch, \scratch, FSL_BOOKE_MAS2(\epn, \wimg)@l
    mtspr   MAS2, \scratch
    lis \scratch, FSL_BOOKE_MAS3(\rpn, 0, \perm)@h
    ori \scratch, \scratch, FSL_BOOKE_MAS3(\rpn, 0, \perm)@l
    mtspr   MAS3, \scratch
    lis \scratch, \phy_high@h
    ori \scratch, \scratch, \phy_high@l
    mtspr   MAS7, \scratch
    isync
    msync
    tlbwe
    isync
    .endm


    .macro  delete_tlb1_entry esel scratch
    lis \scratch, FSL_BOOKE_MAS0(1, \esel, 0)@h
    ori \scratch, \scratch, FSL_BOOKE_MAS0(1, \esel, 0)@l
    mtspr   MAS0, \scratch
    li  \scratch, 0
    mtspr   MAS1, \scratch
    isync
    msync
    tlbwe
    isync
    .endm


    .macro  delete_tlb0_entry esel epn wimg scratch
    lis \scratch, FSL_BOOKE_MAS0(0, \esel, 0)@h
    ori \scratch, \scratch, FSL_BOOKE_MAS0(0, \esel, 0)@l
    mtspr   MAS0, \scratch
    li  \scratch, 0
    mtspr   MAS1, \scratch
    lis \scratch, FSL_BOOKE_MAS2(\epn, \wimg)@h
    ori \scratch, \scratch, FSL_BOOKE_MAS2(\epn, \wimg)@l
    mtspr   MAS2, \scratch
    isync
    msync
    tlbwe
    isync
    .endm
    
.globl __E500V2_START
__E500V2_START:
    //do some basic register init
    //disable the exp and interrupt
    lis  r3, cn_msr_de@h
    ori  r3, r3, cn_msr_de@l
    mtmsr   r3
    xor   r3, r3, r3
    mtspr tcr, r3
    mtspr mcsr, r3
    mtxer r3
    //clear all the exp flags
    mfspr   r3,dbsr
    mtspr   dbsr,r3  
    isync
    mfspr   r3, tsr
    mtspr   tsr, r3   
    isync

    lis    r3, 0x0000
    mtspr  hid0, r3
    li  r0,(HID1_ASTME|HID1_ABE)@l  
    mtspr   hid1,r0
    isync
    /* Enable Branch Prediction ,*/
    lis r0,BUCSR_BPEN@h
    ori r0,r0,BUCSR_BPEN@l
    mtspr   bucsr,r0
    isync
    //used to fix an BTB bug
    lis r0,BUCSR_BBFI@h
    ori r0,r0,BUCSR_BBFI@l
    mfspr   r3, bucsr
    or      r3, r3, r0
    mtspr   bucsr,r3
    isync
    
//now do the ccsr set and set the tlb
    bl  nexti       /* Find our address */
nexti: 
    mflr    r1      /* R1 = our PC */
    li      r2, 0
    mtspr   MAS6, r2    /* Assume the current PID and AS are 0 */
    isync
    msync
    tlbsx   0, r1       /* This must succeed */

    mfspr   r14, MAS0   /* Save ESEL for later */
    rlwinm  r14, r14, 16, 0xfff

    /* Set the size of the TLB to 4KB */
    mfspr   r3, MAS1
    li  r2, 0xF80
    andc    r3, r3, r2  /* Clear the TSIZE bits */
    ori     r3, r3, MAS1_TSIZE(BOOKE_PAGESZ_4K)@l
    oris    r3, r3, MAS1_IPROT@h
    mtspr   MAS1, r3

    /*
     * Set the base address of the TLB to our PC.  We assume that
     * virtual == physical.  We also assume that MAS2_EPN == MAS3_RPN.
     */
    lis r3, MAS2_EPN@h
    ori r3, r3, MAS2_EPN@l  /* R3 = MAS2_EPN */

    and r1, r1, r3  /* Our PC, rounded down to the nearest page */

    mfspr   r2, MAS2
    andc    r2, r2, r3
    or  r2, r2, r1   //reserve the mas2 some optional bits

    mtspr   MAS2, r2    /* Set the EPN to our PC base address */

    mfspr   r2, MAS3
    andc    r2, r2, r3
    or  r2, r2, r1
    mtspr   MAS3, r2    /* Set the RPN to our PC base address */

    isync
    msync
    tlbwe
    isync
    tlbsync
    msync

    /*
     * Clear out any other TLB entries that may exist, to avoid conflicts.
     * Our TLB entry is in r14.
    */
    li  r0, TLBIVAX_ALL | TLBIVAX_TLB0
    tlbivax 0, r0
    tlbsync

    mfspr   r4, tlb1cfg
    rlwinm  r4, r4, 0, TLBnCFG_NENTRY_MASK

    li  r3, 0
    mtspr   MAS1, r3   //MAS1 has the invalid bit
1:  cmpw    r3, r14
    rlwinm  r5, r3, 16, MAS0_ESEL_MSK
    addi    r3, r3, 1
    beq 2f

    oris    r5, r5, MAS0_TLBSEL(1)@h
    mtspr   MAS0, r5
    isync
    tlbwe
    isync
    msync

2:  cmpw    r3, r4
    blt 1b
    
//setup the ccsr
//when poweron, the ccsr is at 0xff700000, which makes the last space divided, 
//so we move it to another space to make the last space integrated 
ccsr_setup:
//beofore we access the ccsr,we must create the tlb map it first
//map to the old phys
    create_tlb0_entry 0, \
        0, BOOKE_PAGESZ_4K, \
        CN_CCSR_TESTADDR, MAS2_I|MAS2_G, \
        CN_CCSR_RESETADDR, MAS3_SW|MAS3_SR, \
        0, r3 /* The default CCSR address is always a 32-bit number */        
//chk wheather the old phys has the ccsr
    lis     r0, CN_CCSR_RESETADDR@h
    ori     r0, r0, CN_CCSR_RESETADDR@l
    lis     r4, CN_CCSR_TESTADDR@h
    ori     r4, r4, CN_CCSR_TESTADDR@l   
    lwz     r3, 0(r4)        /* CCSRBAR, shifted right by 12 */
    slwi    r3, r3, 12
    cmpl    0, r0, r3
//if the phys don't has the ccsr as we expect, we just hange here  
ccsr_chkfailed:
    bne    ccsr_chkfailed
//ok, now we move the old ccsr addr to new one
    sync
    lwz r0, 0(r4)
    isync
    lis    r0, CN_CCSR_NEWADDR_PL@h
    ori    r0, r0, CN_CCSR_NEWADDR_PL@l
    srwi   r0,r0,12
    stw    r0, 0(r4)
    sync
    isync
    //0xfffffffc must be an valid address
    lis  r3, 0xfffffffc@h
    ori  r3, r3, 0xfffffffc@l
    lwz  r4, 0(r3)
    isync
//the ccsr has been set successful, so we must delete the tlb we create for the test
    delete_tlb0_entry 0, CN_CCSR_TESTADDR, MAS2_I|MAS2_G, r3
    
//we will create all the tlb item in the space 1, so we must first create some
//tlb item that covers for the code txt and program stack for the code run in space1
//CCSR(14)  TEMPSTACK()  TEXTCODE(15)
MMUSETUP_FORSPACE1:
//CREATE THE CCSR FOR SPACE1--tlb1-14
    create_tlb1_entry 15, \
        1, BOOKE_PAGESZ_1M, \
        CN_CCSR_NEWADDR_V, MAS2_I|MAS2_G, \
        CN_CCSR_NEWADDR_PL, MAS3_SW|MAS3_SR, \
        CN_CCSR_NEWADDR_PH, r3
//CREATE THE TXTCODE FOR SPACE1--tlb1 --15        
    create_tlb0_entry 1, \
        1, BOOKE_PAGESZ_4K, \
        CN_BOOT_ADDR4K, MAS2_G, \
        CN_BOOT_ADDR4K, MAS3_SX|MAS3_SW|MAS3_SR, \
        0, r6
//CREATE THE STACK TLB
//CREATE THE STACK FOR BOTH SPACE  1
    create_tlb0_entry 2, \
        1, BOOKE_PAGESZ_4K, \
        CN_SRAM_STACK,0, \
        CN_SRAM_STACK, MAS3_SW|MAS3_SR, \
        0, r6
//NOW TURN TO SPACE 1
    lis r3,MSR_IS|MSR_DS|MSR_DE@h
    ori r3,r3,MSR_IS|MSR_DS|MSR_DE@l
    lis r4,RUNNING_INSPACE1@h
    ori r4,r4,RUNNING_INSPACE1@l
    mtspr   srr0,r4
    mtspr   srr1,r3
    rfi
/////////THE SPACE 1 RUNNING CODE///////////////
RUNNING_INSPACE1:
//make the l1 dcache as the stack 
    li  r0,cn_l1csrn_cfi
    mtspr   l1csr0,r0   /* invalidate d-cache */
    mtspr   l1csr1,r0   /* invalidate i-cache */
    /* Enable/invalidate the I-Cache */
    lis r2,(cn_l1csrn_cfi|cn_l1csrn_clfr)@h
    ori r2,r2,(cn_l1csrn_cfi|cn_l1csrn_clfr)@l
    mtspr   l1csr1,r2
1:
    mfspr   r3,l1csr1
    and.    r1,r3,r2
    bne 1b

    lis r3,(cn_l1csrn_cpe|cn_l1csrn_ce)@h
    ori r3,r3,(cn_l1csrn_cpe|cn_l1csrn_ce)@l
    mtspr   l1csr1,r3
    isync
2:
    mfspr   r3,l1csr1
    andi.   r1,r3,cn_l1csrn_ce@l
    beq 2b

    /* Enable/invalidate the D-Cache */
    lis r2,(cn_l1csrn_cfi|cn_l1csrn_clfr)@h
    ori r2,r2,(cn_l1csrn_cfi|cn_l1csrn_clfr)@l
    mtspr   l1csr0,r2
1:
    mfspr   r3,l1csr0
    and.    r1,r3,r2
    bne 1b

    lis r3,(cn_l1csrn_cpe|cn_l1csrn_ce)@h
    ori r3,r3,(cn_l1csrn_cpe|cn_l1csrn_ce)@l
    mtspr   l1csr0,r3
    isync
2:
    mfspr   r3,l1csr0
    andi.   r1,r3,cn_l1csrn_ce@l
    beq 2b
    
    
    /* lock l1d cahche as the sram */
    lis r3,CN_SRAM_STACK@h
    ori r3,r3,CN_SRAM_STACK@l   
    lis  r2, CN_SRAM_STACK_SIZE@h
    ori  r2, r2, CN_SRAM_STACK_SIZE@l
    srwi r2, r2, L1_CACHE_SHIFT
    mtctr  r2
    li  r0,0
1:
    dcbz    r0,r3
    dcbtls  0,r0,r3
    addi    r3,r3,L1_CACHE_LINESIZE
    bdnz    1b

//test for the cache for sram
    lis r1, 0x55555555@h
    ori r1, r1, 0x55555555@l
    lis r3, CN_SRAM_STACK@h
    ori r3, r3, CN_SRAM_STACK@l
    stw r1, 0xf0(r3)
    xor r4, r4, r4
    lwz r4, 0xf0(r3)
    cmpw  r1, r4
STACKSETUP_FAILED:
    bne  STACKSETUP_FAILED

//the cache for sram test success, so we will setup the stack 
//must be 16 aligned and be an decrement empty stack
    lis   r3,CN_SRAM_STACK@h
    ori   r3,r3,CN_SRAM_STACK@l
    addi  r3,r3,0xff0
    lis   r0,0
    stw   r0,0(r3)    
    stw   r0,+4(r3)   
    mr    r1,r3  
//OK, now we could run in C mode, because we have setup for the c environment
    lis   r3, RoutSetUpAsm@h
    ori   r3, r3, RoutSetUpAsm@l
    mtlr  r3
    blr
   
.end
/*************************file end**********************************/




