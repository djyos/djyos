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


#ifndef  _CPU_PERI_ADDRESS_H
#define  _CPU_PERI_ADDRESS_H
//------some register that relations to the interrupt
//------makes me not so happy as other codes
//
//the pic base address 0xFF700000(CCSBAR) + 0X00040000(OFFSET)

#define cn_ccsbar_base_addr          0x00000000


//#define cn_gpio_baddr                (cn_ccsbar_base_addr + 0xf000)
#define cn_uart_baddr                (cn_ccsbar_base_addr + 0x3c00)
#define CN_IIC0_BADDR                 (cn_ccsbar_base_addr + 0x4400)
#define cn_spi1_baddr                (cn_ccsbar_base_addr + 0x1000)
#define cn_spi2_baddr                (cn_ccsbar_base_addr + 0x2800)

#define cn_linkport0_baddr           (cn_ccsbar_base_addr + 0x4c00)
#define cn_linkport1_baddr           (cn_ccsbar_base_addr + 0x4c20)


#define cn_dma_linkport0_baddr       (cn_ccsbar_base_addr + 0x4c18)
#define cn_dma_linkport1_baddr       (cn_ccsbar_base_addr + 0x4c38)

#define cn_sport_group1_baddr         (cn_ccsbar_base_addr+0xc00)
#define cn_sport_group2_baddr         (cn_ccsbar_base_addr+0x400)
#define cn_sport_group3_baddr         (cn_ccsbar_base_addr+0x800)
#define cn_sport_group4_baddr         (cn_ccsbar_base_addr+0x4800)

#define cn_dma_sport0_baddr           (cn_ccsbar_base_addr+0xc40)
#define cn_dma_sport1_baddr           (cn_ccsbar_base_addr+0xc48)
#define cn_dma_sport2_baddr           (cn_ccsbar_base_addr+0x440)
#define cn_dma_sport3_baddr           (cn_ccsbar_base_addr+0x448)
#define cn_dma_sport4_baddr           (cn_ccsbar_base_addr+0x840)
#define cn_dma_sport5_baddr           (cn_ccsbar_base_addr+0x848)
#define cn_dma_sport6_baddr           (cn_ccsbar_base_addr+0x4840)
#define cn_dma_sport7_baddr           (cn_ccsbar_base_addr+0x4848)

#define cn_sport_txrx_group1_baddr            (cn_ccsbar_base_addr+0xc60)
#define cn_sport_txrx_group2_baddr            (cn_ccsbar_base_addr+0x460)
#define cn_sport_txrx_group3_baddr            (cn_ccsbar_base_addr+0x860)
#define cn_sport_txrx_group4_baddr            (cn_ccsbar_base_addr+0x4860)

//use core private per-cpu registers,--TODO, may be changed as pic base address 
//#define cn_ctpr_core_addr_p   0xffe60080
//#define cn_iack_core_addr_p   0xffe600a0
//#define cn_eoi_core_addr_p    0xffe600b0   
//#define cn_whoami_core_addr_p 0xffe60090
//
//#define cn_ctpr_core_addr_g   0xffe40080
//#define cn_iack_core_addr_g   0xffe400a0
//#define cn_eoi_core_addr_g    0xffe400b0   
//#define cn_whoami_core_addr_g 0xffe40090



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

//each core has one,but the same physical addr,copied to different registers
#define cn_core_ctpr_addr            (cn_pic_baddr + 0x80)
#define cn_core_idnumber_addr        (cn_pic_baddr + 0x90)
#define cn_core_iack_addr            (cn_pic_baddr + 0xA0)
#define cn_int_eoi_addr              (cn_pic_baddr + 0xB0)
//define for some important register in the PIC
#define ctpr_offset  0x80
#define eoi_offset   0xb0
#define iack_offset  0xA0
#define gcr_offset   0x1020


//the fallowing regs used for importent control
#define cn_pic_gcr_addr              (cn_pic_baddr + 0x1020)
#define cn_pic_gcr_mode_m            (0x01<<29)//mixed mode
#define cn_pic_gcr_mode_p            (0x00<<29)//pass through mode
#define cn_pic_gcr_rst               (0x01<<31)//reset

//global modules
#define cn_guts_rstcr                (cn_ccsbar_base_addr + 0xe00b0)


#endif
