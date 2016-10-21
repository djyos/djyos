#include "E500v2Reg.h"
#include "E500v2RegBits.h"
#include "SysSpace.h"

.file "RoutSetUps.S"
.globl  RoutSetUpAsm
RoutSetUpAsm:
    bl     RoutSetUp
    /* switch back to AS = 0 */
    //turn back to space 0, the tlb law and ddr is ready now
    lis    r3,(MSR_ME|MSR_DE)@h
    ori    r3,r3,(MSR_ME|MSR_DE)@l
    mtmsr  r3
    isync 
//test the ddr
    lis  r3, 0x5555555@h
    ori  r3, r3,  0x5555555@l
    lis  r4, CN_DDR_ADDR_V@h
    ori  r4, r4, CN_DDR_ADDR_V@l
    stw  r3, 0x10(r4)
    isync
    msync
    lwz  r5, 0x10(r4)
    cmpw r3, r5
DDR_FAILED:
    bne  DDR_FAILED
//almost successfull now, then goto the CPU init
//set the stack to the ram
    lis  sp, usr_top@h
    ori  sp, sp, usr_top@l
    subi sp, sp, 0x10
    //before we cpy the code, we must unlock the l1 cache, else any cpy will be invalid
    bl   __AsmEnableL1Cache   
    bl   LoadLoader2Mem
    lis  r3, __AsmRamStart@h
    ori  r3, r3, __AsmRamStart@l
    mtlr r3
    blr
    isync
    //never come back
    /*
@-------------------------------------------------------------------------------
@功能: 开L1cache
@参数:
@返回:
@函数原型:__AsmEnableL1Cache(void)
@-------------------------------------------------------------------------------
*/
 //enable the l1 as the cache
/*first invalidate, then enable*/
//first icache, the dcache
//invalidate l1icache
/*---not stable now, maybe need some time delay---TODO*/
.global  __AsmEnableL1Cache
__AsmEnableL1Cache:
    mfspr    r0,l1csr1
    ori      r0,r0,cn_l1csr1_icfi@l
    msync
    isync
    mtspr    l1csr1,r0
    isync
    //enable l1icache
    mfspr  r4, l1csr1
    ori    r4, r4, cn_l1csr1_ice@l
    mtspr  l1csr1, r4
    isync
    msync

    //now the l1dcache
    //invalidate l1dcache
    mfspr    r0,l1csr0
    ori      r0,r0,cn_l1csr0_cfi@l
    mtspr    l1csr0,r0
    isync
    msync

    //enable l1dcache
    mfspr  r4, l1csr0
    ori    r4, r4, cn_l1csr0_ce
    mtspr  l1csr0, r4
    isync
    msync
    
    blr     

/*
@-------------------------------------------------------------------------------
@功能: 创建TLB条目
@参数: void
@返回:
@函数原型:void write_tlb(mas0, mas1, mas2, mas3, mas7)
@-------------------------------------------------------------------------------
*/
.globl  write_tlb
write_tlb:
    mtspr   MAS0,r3
    mtspr   MAS1,r4
    mtspr   MAS2,r5
    mtspr   MAS3,r6
    mtspr   MAS7,r7
    isync
    tlbwe
    msync
    isync
    blr

