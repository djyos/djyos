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
//module:p1020-core0
//author：zqf
//version：V1.0.0 11:40:37 AM/Nov 4, 2013
//description:this file defines the details for the sprs
//modified history: NULL
//------------------------------------------------------

#ifndef __E500V2REGBITS_H
#define __E500V2REGBITS_H


//CORE  id
#define cn_pir_core0_id  0
#define cn_pir_core1_id  1

//msr的一些重要标志位
#define  cn_msr_de    (1<<9)
#define  cn_msr_ee    (1<<15)
#define  cn_msr_ce    (1<<17)
#define  cn_msr_me    (1<<12)
#define  cn_msr_spe   (1<<25)
//TCR的一些重要标志位
#define  cn_tcr_die   (1<<26)
#define  cn_tcr_are   (1<<22)
#define  cn_tcr_wp_msk          (0x03<<30)   //only two bits
#define  cn_tcr_wrc_dis_reset   (0x00<<28)
#define  cn_tcr_wrc_en_reset    (0x02<<28)
#define  cn_tcr_wie             (1<<27)
#define  cn_tcr_wpext_msk       (0x0f<<17)  //4bits
#define  cn_tcr_fp_msk          (0x03<<24)
#define  cn_tcr_fie             (1<<23)
#define  cn_tcr_fpext_msk       (0x0f<<13)

#define  CN_TCR_DEFAULT         (cn_tcr_die|cn_tcr_are)


//TSR 的一些标志位
#define  cn_tsr_enw             (1<<31)
#define  cn_tsr_wis             (1<<30)
#define  cn_tsr_wrs             (3<<28)
#define  cn_tsr_dis             (1<<27)
#define  cn_tsr_fis             (1<<26)

//spefscr的一些重要标志位
#define   cn_spefscr_frmc   (0x00)  //round to nearest
#define   cn_spefscr_finve  (1<<5)  //invalid operating/input
#define   cn_spefscr_fdbze  (1<<4)  //divided by zero exception
#define   cn_spefscr_funfe  (1<<3)  //underflow exception
#define   cn_spefscr_fovfe  (1<<2)  //overflow exception
#define   cn_spefscr_finv   (1<<11) //input error
#define   cn_spefscr_fdbz   (1<<10) //divided by zero
#define   cn_spefscr_funf   (1<<9)  //underflow error
#define   cn_spefscr_fovf   (1<<8)  //overflow error

//dbcr0寄存器的一些重要位置
#define  cn_dbcr0_idm   (1<<30)
#define  cn_dbcr0_rst   (1<<29)
#define  cn_dbcr0_icmp  (1<<27)
#define  cn_dbcr0_trap  (1<<24)
//l1 cache 控制器重要标志位
/*数据cache*/
#define  cn_l1csr0_ce    (1<<0)
#define  cn_l1csr0_cfi   (1<<1)
#define  cn_l1csr0_clfr  (1<<8)
#define  cn_l1csr0_clo   (1<<9)
#define  cn_l1csr0_cul   (1<<10)
#define  cn_l1csr0_cslc  (1<<11)
#define  cn_l1csr0_cpi   (1<<15)
#define  cn_l1csr0_cpe   (1<<16)

#define  L1_CACHE_SHIFT             5
#define  L1_CACHE_LINESIZE          32    
/*指令cache*/
#define  cn_l1csr1_ice    (1<<0)
#define  cn_l1csr1_icfi   (1<<1)
#define  cn_l1csr1_iclfr  (1<<8)
#define  cn_l1csr1_iclo   (1<<9)
#define  cn_l1csr1_icul   (1<<10)
#define  cn_l1csr1_icslc  (1<<11)
#define  cn_l1csr1_icpi   (1<<15)
#define  cn_l1csr1_icpe   (1<<16)

#define  cn_l1csrn_ce    (1<<0)
#define  cn_l1csrn_cfi   (1<<1)
#define  cn_l1csrn_clfr  (1<<8)
#define  cn_l1csrn_clo   (1<<9)
#define  cn_l1csrn_cul   (1<<10)
#define  cn_l1csrn_cslc  (1<<11)
#define  cn_l1csrn_cpi   (1<<15)
#define  cn_l1csrn_cpe   (1<<16)

//MCSR
//machine check 异常标志位MSR
#define cn_mc_mcp_msk                       (1<<31)
#define cn_mc_icpeer_msk					(1<<30)
#define cn_mc_dcp_perr_msk					(1<<29)
#define cn_mc_dcperr_msk                    (1<<28)
#define cn_mc_bus_iaerr_msk                 (1<<7)
#define cn_mc_bus_raerr_msk					(1<<6)
#define cn_mc_bus_waerr_msk                 (1<<5)
#define cn_mc_bus_iberr_msk					(1<<4)
#define cn_mc_bus_rberr_msk                 (1<<3)
#define cn_mc_bus_wberr_msk                 (1<<2)
#define cn_mc_bus_iperr_msk                 (1<<1)
#define cn_mc_bus_rperr_msk                 (1<<0)

//define for the msr ready for event to run
//define for the default state of the cpu register for the event
#define cn_tcr_for_event_run      (cn_tcr_die | cn_tcr_are)  //ARE 和   DIE
#define cn_spefscr_for_event_run  (cn_spefscr_frmc | cn_spefscr_finve |cn_spefscr_fdbze|\
								   cn_spefscr_funfe | cn_spefscr_fovfe)
#define cn_msr_for_event_run   (cn_msr_me| cn_msr_spe|cn_msr_de |cn_msr_ce |cn_msr_ee  )

#define CN_MSR_INIT            (cn_msr_me| cn_msr_spe|cn_msr_de)
#define CN_MSR_THREAD          (cn_msr_me| cn_msr_spe |cn_msr_de|cn_msr_ce |cn_msr_ee  )
#define CN_MSR_ASYN_EN         (cn_msr_ee )
#define CN_MSR_ASYN_DIS        (~CN_MSR_ASYN_EN)
#define CN_MSR_TRUNK_EN        (cn_msr_ce)
#define CN_MSR_TRUNK_DIS       (~(cn_msr_ce |cn_msr_ee))


#define  cn_dbcr0_debug_mode (cn_dbcr0_idm | cn_dbcr0_trap)

#define MSR_UCLE	(1<<26)		/* User-mode cache lock enable (e500) */
#define MSR_VEC		(1<<25)		/* Enable AltiVec(74xx) */
#define MSR_SPE		(1<<25)		/* Enable SPE(e500) */
#define MSR_POW		(1<<18)		/* Enable Power Management */
#define MSR_WE		(1<<18)		/* Wait State Enable */
#define MSR_TGPR	(1<<17)		/* TLB Update registers in use */
#define MSR_CE		(1<<17)		/* Critical Interrupt Enable */
#define MSR_ILE		(1<<16)		/* Interrupt Little Endian */
#define MSR_EE		(1<<15)		/* External Interrupt Enable */
#define MSR_PR		(1<<14)		/* Problem State / Privilege Level */
#define MSR_FP		(1<<13)		/* Floating Point enable */
#define MSR_ME		(1<<12)		/* Machine Check Enable */
#define MSR_FE0		(1<<11)		/* Floating Exception mode 0 */
#define MSR_SE		(1<<10)		/* Single Step */
#define MSR_DWE		(1<<10)		/* Debug Wait Enable (4xx) */
#define MSR_UBLE	(1<<10)		/* BTB lock enable (e500) */
#define MSR_BE		(1<<9)		/* Branch Trace */
#define MSR_DE		(1<<9)		/* Debug Exception Enable */
#define MSR_FE1		(1<<8)		/* Floating Exception mode 1 */
#define MSR_IP		(1<<6)		/* Exception prefix 0x000/0xFFF */
#define MSR_IR		(1<<5)		/* Instruction Relocate */
#define MSR_IS		(1<<5)		/* Book E Instruction space */
#define MSR_DR		(1<<4)		/* Data Relocate */
#define MSR_DS		(1<<4)		/* Book E Data space */
#define MSR_PE		(1<<3)		/* Protection Enable */
#define MSR_PX		(1<<2)		/* Protection Exclusive Mode */
#define MSR_PMM		(1<<2)		/* Performance monitor mark bit (e500) */
#define MSR_RI		(1<<1)		/* Recoverable Exception */
#define MSR_LE		(1<<0)		/* Little Endian */


//exception state register(ESR)
#define ESR_PIL  (1<<27)
#define ESR_PPR  (1<<26)
#define ESR_PTR  (1<<25)
#define ESR_ST   (1<<23)
#define ESR_DLK  (1<<21)
#define ESR_ILK  (1<<20)
#define ESR_BO   (1<<17)
#define ESR_SPE  (1<<7)
//exception state register(TSR)

//exception state register (DBSR)
#define  cn_dbsr_ide   (1<<31)

//exception state register (MSR)

//exception state register (SPFSCR)


#define HID0_ICE_SHIFT		15
#define HID0_DCE_SHIFT		14
#define HID0_DLOCK_SHIFT	12

#define   HID0_EMCP	(1<<31)		/* Enable Machine Check pin */
#define   HID0_EBA	(1<<29)		/* Enable Bus Address Parity */
#define   HID0_EBD	(1<<28)		/* Enable Bus Data Parity */
#define   HID0_SBCLK	(1<<27)
#define   HID0_EICE	(1<<26)
#define   HID0_ECLK	(1<<25)
#define   HID0_PAR	(1<<24)
#define   HID0_DOZE	(1<<23)
#define   HID0_NAP	(1<<22)
#define   HID0_SLEEP	(1<<21)
#define   HID0_DPM	(1<<20)
#define   HID0_ICE	(1<<HID0_ICE_SHIFT)	/* Instruction Cache Enable */
#define   HID0_DCE	(1<<HID0_DCE_SHIFT)	/* Data Cache Enable */
#define   HID0_TBEN	(1<<14)		/* Time Base Enable */
#define   HID0_ILOCK	(1<<13)		/* Instruction Cache Lock */
#define   HID0_DLOCK	(1<<HID0_DLOCK_SHIFT)	/* Data Cache Lock */
#define   HID0_ICFI	(1<<11)		/* Instr. Cache Flash Invalidate */
#define   HID0_DCFI	(1<<10)		/* Data Cache Flash Invalidate */
#define   HID0_DCI	HID0_DCFI
#define   HID0_SPD	(1<<9)		/* Speculative disable */
#define   HID0_ENMAS7	(1<<7)		/* Enable MAS7 Update for 36-bit phys */
#define   HID0_SGE	(1<<7)		/* Store Gathering Enable */
#define   HID0_SIED	HID_SGE		/* Serial Instr. Execution [Disable] */
#define   HID0_DCFA	(1<<6)		/* Data Cache Flush Assist */
#define   HID0_BTIC	(1<<5)		/* Branch Target Instruction Cache Enable */
#define   HID0_ABE	(1<<3)		/* Address Broadcast Enable */
#define   HID0_BHTE	(1<<2)		/* Branch History Table Enable */
#define   HID0_BTCD	(1<<1)		/* Branch target cache disable */
#define	  HID1_RFXE	(1<<17)		/* Read Fault Exception Enable */
#define	  HID1_ASTME	(1<<13)		/* Address bus streaming mode */
#define	  HID1_ABE	(1<<12)		/* Address broadcast enable */
#define	  HID1_MBDD	(1<<6)		/* optimized sync instruction */

//异常号的定义
//系统主异常号
#define cn_majorexp_critical_input            0
#define cn_majorexp_machine_check             1
#define cn_majorexp_data_storage              2
#define cn_majorexp_instruction_storage       3
#define cn_majorexp_external_input            4
#define cn_majorexp_alignment                 5
#define cn_majorexp_program                   6
#define cn_majorexp_reserved_7                7   //not supported by e500v2
#define cn_majorexp_syscall                   8
#define cn_majorexp_reserved_9                9   //not supported by e500v2
#define cn_majorexp_systick                   10  //used as ticker
#define cn_majorexp_fixed_timer               11  //used as normal timer
#define cn_majorexp_watch_dog_timer           12  //dog
#define cn_majorexp_data_tlb_error            13
#define cn_majorexp_instruction_tlb_error     14
#define cn_majorexp_debug                     15
#define cn_majorexp_speapu_unavailable        16
#define cn_majorexp_embeded_fp_data           17
#define cn_majorexp_embeded_fp_round          18
#define cn_majorexp_performance_monitor       19
#define cn_majorexp_cpulast                   19
#define cn_majorexp_cpuinvalid                20



////////////////////////////////////////////////////////////////////////////////
//MMU registers and operations
////////////////////////////////////////////////////////////////////////////////

/* Number of TLB CAM entries we have on FSL Book-E chips */
#define  CONFIG_E500
#define  CONFIG_P1020RDB

#define CONFIG_SYS_NUM_TLBCAMS		16
#define CONFIG_MAX_CPUS	    	2
#define CONFIG_SYS_FSL_NUM_LAWS		12
#define CONFIG_SYS_PPC_E500_DEBUG_TLB	2
#define CONFIG_TSECV2
#define CONFIG_SYS_FSL_SEC_COMPAT	2

#define CONFIG_SYS_FSL_ERRATUM_ELBC_A001
#define CONFIG_SYS_FSL_ERRATUM_ESDHC111

//关键性操作
#define MAS0_TLBSEL_MSK	0x30000000
#define MAS0_TLBSEL(x)    (((x) << 28) & MAS0_TLBSEL_MSK)
#define MAS0_ESEL_MSK	0x0FFF0000
#define MAS0_ESEL(x)    (((x) << 16) & MAS0_ESEL_MSK)
#define MAS0_NV(x)    ((x) & 0x00000FFF)

#define MAS1_VALID	0x80000000
#define MAS1_IPROT	0x40000000
#define MAS1_TID(x)    (((x) << 16) & 0x3FFF0000)
#define MAS1_TS		0x00001000
#define MAS1_TSIZE(x)    (((x) << 7) & 0x00000F80)
#define TSIZE_TO_BYTES(x) (1ULL << ((x) + 10))

#define MAS2_EPN	0xFFFFF000
#define MAS2_X0		0x00000040
#define MAS2_X1		0x00000020
#define MAS2_W		0x00000010
#define MAS2_I		0x00000008
#define MAS2_M		0x00000004
#define MAS2_G		0x00000002
#define MAS2_E		0x00000001
#define MAS2_PP_MASK 0x000000FF

#define MAS3_RPN	0xFFFFF000
#define MAS3_U0		0x00000200
#define MAS3_U1		0x00000100
#define MAS3_U2		0x00000080
#define MAS3_U3		0x00000040
#define MAS3_UX		0x00000020
#define MAS3_SX		0x00000010
#define MAS3_UW		0x00000008
#define MAS3_SW		0x00000004
#define MAS3_UR		0x00000002
#define MAS3_SR		0x00000001
#define MAS3_PP_MASK  0x00000FFF

#define MAS4_TLBSELD(x) MAS0_TLBSEL(x)
#define MAS4_TIDDSEL	0x000F0000
#define MAS4_TSIZED(x)	MAS1_TSIZE(x)
#define MAS4_X0D	0x00000040
#define MAS4_X1D	0x00000020
#define MAS4_WD		0x00000010
#define MAS4_ID		0x00000008
#define MAS4_MD		0x00000004
#define MAS4_GD		0x00000002
#define MAS4_ED		0x00000001

#define MAS6_SPID0	0x3FFF0000
#define MAS6_SPID1	0x00007FFE
#define MAS6_SAS	0x00000001
#define MAS6_SPID	MAS6_SPID0

#define MAS7_RPN	0xFFFFFFFF

#define FSL_BOOKE_MAS0(tlbsel,esel,nv) \
        (MAS0_TLBSEL(tlbsel) | MAS0_ESEL(esel) | MAS0_NV(nv))
#define FSL_BOOKE_MAS1(v,iprot,tid,ts,tsize) \
        ((((v) << 31) & MAS1_VALID)             |\
        (((iprot) << 30) & MAS1_IPROT)          |\
        (MAS1_TID(tid))                |\
        (((ts) << 12) & MAS1_TS)                |\
        (MAS1_TSIZE(tsize)))
#define FSL_BOOKE_MAS2(epn, wimge) \
        (((epn) & MAS3_RPN) | (wimge))
#define FSL_BOOKE_MAS3(rpn, user, perms) \
        (((rpn) & MAS3_RPN) | (user) | (perms))
#define FSL_BOOKE_MAS7(rpn) \
        (((u64)(rpn)) >> 32)

//关于LAWS条目
#define CONFIG_SYS_FSL_NUM_LAWS		12
#define FSL_HW_NUM_LAWS CONFIG_SYS_FSL_NUM_LAWS

#define BOOKE_PAGESZ_1K		0
#define BOOKE_PAGESZ_2K		1
#define BOOKE_PAGESZ_4K		2
#define BOOKE_PAGESZ_8K		3
#define BOOKE_PAGESZ_16K	4
#define BOOKE_PAGESZ_32K	5
#define BOOKE_PAGESZ_64K	6
#define BOOKE_PAGESZ_128K	7
#define BOOKE_PAGESZ_256K	8
#define BOOKE_PAGESZ_512K	9
#define BOOKE_PAGESZ_1M		10
#define BOOKE_PAGESZ_2M		11
#define BOOKE_PAGESZ_4M		12
#define BOOKE_PAGESZ_8M		13
#define BOOKE_PAGESZ_16M	14
#define BOOKE_PAGESZ_32M	15
#define BOOKE_PAGESZ_64M	16
#define BOOKE_PAGESZ_128M	17
#define BOOKE_PAGESZ_256M	18
#define BOOKE_PAGESZ_512M	19
#define BOOKE_PAGESZ_1G		20
#define BOOKE_PAGESZ_2G		21
#define BOOKE_PAGESZ_4G		22
#define BOOKE_PAGESZ_8G		23
#define BOOKE_PAGESZ_16GB	24
#define BOOKE_PAGESZ_32GB	25
#define BOOKE_PAGESZ_64GB	26
#define BOOKE_PAGESZ_128GB	27
#define BOOKE_PAGESZ_256GB	28
#define BOOKE_PAGESZ_512GB	29
#define BOOKE_PAGESZ_1TB	30
#define BOOKE_PAGESZ_2TB	31

#define TLBIVAX_ALL	        4
#define TLBIVAX_TLB0		0
#define TLBIVAX_TLB1		8


#define   TLBnCFG_NENTRY_MASK	0x00000fff


#define	  BUCSR_STAC_EN	0x01000000	/* Segment target addr cache enable */
#define	  BUCSR_LS_EN	0x00400000	/* Link stack enable */
#define	  BUCSR_BBFI	0x00000200	/* Branch buffer flash invalidate */
#define	  BUCSR_BPEN	0x00000001	/* Branch prediction enable */
#define   BUCSR_ENABLE (BUCSR_STAC_EN|BUCSR_LS_EN|BUCSR_BBFI|BUCSR_BPEN)


#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#endif
