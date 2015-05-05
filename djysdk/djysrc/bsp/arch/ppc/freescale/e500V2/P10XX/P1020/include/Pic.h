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
// 模块描述:P1020的中断控制器的一些寄存器地址的定义
// 模块版本: V1.00(初始化版本)
// 创建人员:zhangqf_cyg
// 创建时间:2013-7-26--上午10:15:29
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
#ifndef __PIC_H
#define __PIC_H

#include "board-config.h"

#define cn_pic_baddr                 (CN_CCSR_NEWADDR_V + 0x40000)
#define cn_int_register_step         0x10
#define cn_pic_ivpr2idr              0x10

//register that store the interrupt  and priority and configure the dest
#define cn_int_ext_baddr             (cn_pic_baddr + 0x10000)
#define cn_int_interal_baddr         (cn_pic_baddr + 0x10200)
#define cn_int_msg_baddr             (cn_pic_baddr + 0x11600)
#define cn_int_ipi_vp_baddr          (cn_pic_baddr + 0x10A0)
#define cn_int_c0_dispath_baddr      (cn_pic_baddr + 0x20040)
#define cn_int_c1_dispath_baddr      (cn_pic_baddr + 0x21040)
#define cn_int_smsg_baddr            (cn_pic_baddr + 0x11C00)
#define cn_int_gbtimer_ga_baddr      (cn_pic_baddr + 0x1120)
#define cn_int_gbtimer_gb_baddr      (cn_pic_baddr + 0x2120)
//IVPR寄存器的各种屏蔽位
#define cn_ivpr_prior_msk            (0x000f0000)
#define cn_ivpr_vector_msk           (0x0000ffff)
#define cn_ivpr_en_msk               (0x01<<31)
#define cn_ivpr_sense_msk            (0x01<<22)
#define cn_ivpr_polarity_msk         (0x01<<23)
#define cn_ivpr_trigertype_msk       (0x03<<22)
#define cn_ivpr_activity_msk         (0x01<<30)

#define cn_prior_core_event            (0x00)//normal event
#define cn_prior_core_asyn_enable      (0x00)//normal state
#define cn_prior_core_asyn_disable     (0x01)//used to msk the asyn signal
#define cn_prior_asyn_signal           (0x01)//asyn signal state
#define cn_prior_real_int_default      (0x0A)//real int state
#define cn_prior_core_highest          (0x0F) //no higher prior any more
//IDR的各种屏蔽位
#define cn_pic_idr_p0                (0x01<<0)
#define cn_pic_idr_p1                (0x01<<1)
#define CN_PIC_IDR_C0                (0X01<<29)
#define CN_PIC_IDR_C1                (0x01<<30)
#define cn_pic_idr_ep                (0x01<<31)

//each core has one,but the same physical addr,copied to different registers
#define cn_core_ctpr_addr            (cn_pic_baddr + 0x80)
#define cn_core_idnumber_addr        (cn_pic_baddr + 0x90)
#define cn_core_iack_addr            (cn_pic_baddr + 0xA0)
#define cn_int_eoi_addr              (cn_pic_baddr + 0xB0)
#define cn_pic_gcr                   (cn_pic_baddr + 0x1020)
#define cn_pic_svr                   (cn_pic_baddr + 0x10e0)
#define cn_pic_svr_vector            0x00ff


#define cn_pic_gcr_addr              (cn_pic_baddr + 0x1020)
#define cn_pic_gcr_mode_m            (0x01<<29)//mixed mode
#define cn_pic_gcr_mode_p            (0x00<<29)//pass through mode
#define cn_pic_gcr_rst               (0x01<<31)//reset

//defines the PIC timers
#define cn_pic_timer_tfrra         (cn_pic_baddr + 0x10f0)
#define cn_pic_timer_tcra          (cn_pic_baddr + 0x1300)
#define cn_pic_timer_tfrrb         (cn_pic_baddr + 0x20f0)
#define cn_pic_timer_tcrb          (cn_pic_baddr + 0x2300)

#define cn_pic_timer_gtccra_base  (cn_pic_baddr + 0x1100)
#define cn_pic_timer_gtbcra_base  (cn_pic_baddr + 0x1110)
#define cn_pic_timer_gtccrb_base  (cn_pic_baddr + 0x2100)
#define cn_pic_timer_gtbcrb_base  (cn_pic_baddr + 0x2110)
#define cn_pic_timer_gt_step       0x40
#define cn_pic_timer_gnumber       0x04

//PIC INFORMATION
#define cn_pic_frr_reg                (cn_pic_baddr + 0x1000)
#define cn_pic_frr_nirq_shift         16
#define cn_pic_frr_nirq_mask          0x7ff
#define cn_pic_frr_ncpu_shift         8
#define cn_pic_frr_ncpu_mask          0x1f
#define cn_pic_frr_vid_shift          0
#define cn_pic_frr_vid_mask           0xff

//not rollover, not casecaded, use ccb/8, all the group timer are independent
#define cn_pic_timer_tcr_set       0x00
#define cn_pic_timer_msk           (1<<31)
#define cn_pic_timer_countermsk    (0x7fffffff)
#define cn_pic_timer_togmsk        (0x80000000)

#endif /* __PIC_H */
