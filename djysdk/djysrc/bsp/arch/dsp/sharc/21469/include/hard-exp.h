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
// 模块描述:hard-exp.h/p1020-core0
// 模块版本:
// 创建人员: Administrator
// 创建时间: 5:28:05 PM/Sep 16, 2013
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =======================================================================

#ifndef EXCEPTION_H_
#define EXCEPTION_H_
#ifdef __cplusplus
#extern "C" {
#endif

#define monitor_reboot  1
#define release_reboot  2

//reserved 256K (block 27~block 30)for the exception record.20K for the tagSysExpRecordItem.
//#define cn_sys_exp_record_size                     0x40000     //256K for the exception record.
#define cn_sys_exp_record_size_basic               0x10000      //20K for the basic exp information(struct SysExpRecordItem).
#define cn_sys_exp_record_infor_basic_offset_addr  0x10000   //the start address for save the exp information.
#define cn_exp_tab_used_flag              0x00
//Flash中存储exp最大地址
#define cn_sys_exp_record_max_address     0x44FFFFF
#define cn_sys_exp_sector_base_number     0x32
#define cn_sys_exp_sector_max_number      0x39
#define sector_max_offset_addr            0x1FFFF
#define CN_MAJOREXP_INVALID  0xa
#define CN_MINOREXP_INVALID 0x28



#define Exception_StartAddress 0x000E6E05

#define sys_cmode_dealer_result_succeed 0
#define sys_cmode_dealer_result_asmdo   1
#define sys_cmode_dealer_result_restart 2


//declare the function for exception
//ASM
#define  __asm_reset_cpu_hot  __asm_software_reset




//SPERRSTAT 及SPERRCTLx寄存器bit 定义
#define BIT_0   0x00000001
#define BIT_1   0x00000002
#define BIT_2   0x00000004
#define BIT_3   0x00000008
#define BIT_4   0x00000010
#define BIT_5   0x00000020
#define BIT_6   0x00000040
#define BIT_7   0x00000080
#define BIT_8   0x00000100
#define BIT_9   0x00000200
#define BIT_10  0x00000400
#define BIT_11  0x00000800
#define BIT_12  0x00001000
#define BIT_13  0x00002000
#define BIT_14  0x00004000
#define BIT_15  0x00008000
#define BIT_16  0x00010000
#define BIT_17  0x00020000
#define BIT_18  0x00040000
#define BIT_19  0x00080000
#define BIT_20  0x00100000
#define BIT_21  0x00200000
#define BIT_22  0x00400000
#define BIT_23  0x00800000
#define BIT_24  0x01000000
#define BIT_25  0x02000000
#define BIT_26  0x04000000
#define BIT_27  0x08000000
#define BIT_28  0x10000000
#define BIT_29  0x20000000
#define BIT_30  0x40000000
#define BIT_31  0x80000000

//定义SPERRSTAT 寄存器位
#define SP0_DERRA (BIT_0)
#define SP0_DERRB (BIT_1)
#define SP1_DERRA (BIT_2)
#define SP1_DERRB (BIT_3)
#define SP2_DERRA (BIT_4)
#define SP2_DERRB (BIT_5)
#define SP3_DERRA (BIT_6)
#define SP3_DERRB (BIT_7)
#define SP4_DERRA (BIT_8)
#define SP4_DERRB (BIT_9)
#define SP5_DERRA (BIT_10)
#define SP5_DERRB (BIT_11)
#define SP6_DERRA (BIT_12)
#define SP6_DERRB (BIT_13)
#define SP7_DERRA (BIT_14)
#define SP7_DERRB (BIT_15)
#define SP0_FSERR (BIT_16)
#define SP1_FSERR (BIT_17)
#define SP2_FSERR (BIT_18)
#define SP3_FSERR (BIT_19)
#define SP4_FSERR (BIT_20)
#define SP5_FSERR (BIT_21)
#define SP6_FSERR (BIT_22)
#define SP7_FSERR (BIT_23)

//定义SPERRCTLx寄存器位
#define DERRA_STAT (BIT_4)
#define DERRB_STAT (BIT_5)
#define FSERR_STAT (BIT_6)




//外加一些CPU的寄存器信息，此信息是属于该具体CPU型号的（CORE信息+ CPU特有信息）
//系统硬异常--CPU实际异常
 /*struct SysExpCpuInfo
{
    u32    dwSysExpCpuFlag;      //该异常信息的有效编码
    u32    dwExpMajor;            //异常主编号
    u32    dwExpMinor;            //异常分支编号
    u32    dwExpAddr;             //异常在内存中存储的地址
    u32    dwExpLen;              //本次异常存储的信息的长度
};*/

#define cn_sysexp_cpuinfo_validflag     ((u32)(0x87654321))


#ifdef __cplusplus
}
#endif
#endif /* EXCEPTION_H_ */
