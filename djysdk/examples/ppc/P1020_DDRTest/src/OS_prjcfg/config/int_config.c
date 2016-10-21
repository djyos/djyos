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
//所属模块:工程配置
//作者：lst
//版本：V1.0.0
//文件描述: 本文件罗列了具体工程所启用的中断，对使用了的每个中断，都会分配一个
//      struct int_line结构。这对于内存比较充足的系统来说，不成为负担，对这类系
//      统，把系统所有中断线都加上就可以了。
//          但对于内存非常有限且中断线众多的单片机来说，不管该中断是否被使用，都
//      分配一个struct int_line结构的话，内存表示亚历山大。实际工程中所用到的中
//      断线往往不多，在这里只把使用到了的中断线罗列一下就好。
//其他说明:
//修订历史:
//2. ...
//1. 日期: 20130907
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "int.h"
//this assay indicate which int line could be used by the user
//you must obey this, else due to yourself
const ufast_t tg_IntUsed[] =
{
//internal chip interrupt---64,also means the interrupt number

//external interrupt
//     cn_int_line_extbase                   (0x00)
     cn_int_line_irq0                      ,
     cn_int_line_irq1                      ,
     cn_int_line_irq2                      ,
     cn_int_line_irq3                      ,
     cn_int_line_irq4                      ,
     cn_int_line_irq5                      ,
     cn_int_line_irq6                      ,
     cn_int_line_irq7                      ,
     cn_int_line_irq8                      ,
     cn_int_line_irq9                      ,
     cn_int_line_irqa                      ,
     cn_int_line_irqb                      ,

//msg interrupt

     cn_int_line_msg0                      ,
     cn_int_line_msg1                      ,
     cn_int_line_msg2                      ,
     cn_int_line_msg3                      ,
     cn_int_line_msg4                      ,
     cn_int_line_msg5                      ,
     cn_int_line_msg6                      ,
     cn_int_line_msg7                      ,

//shared msg interrupt
     cn_int_line_smsg0                      ,
     cn_int_line_smsg1                     ,
     cn_int_line_smsg2                     ,
     cn_int_line_smsg3                    ,
     cn_int_line_smsg4                      ,
     cn_int_line_smsg5                     ,
     cn_int_line_smsg6                      ,
     cn_int_line_smsg7                      ,

//inter-processor interrupt,core0
     cn_int_line_c0_ipi0                    ,
     cn_int_line_c0_ipi1                   ,
     cn_int_line_c0_ipi2                    ,
     cn_int_line_c0_ipi3                   ,
     cn_int_line_c0_ipi4                    ,
//inter-processor interrupt,core1
     cn_int_line_c1_ipi0                    ,
     cn_int_line_c1_ipi1                    ,
     cn_int_line_c1_ipi2                    ,
     cn_int_line_c1_ipi3                    ,
     cn_int_line_c1_ipi4                    ,

//pic global timer interrupt
     cn_int_line_gbtimer_a0               ,
     cn_int_line_gbtimer_a1               ,
     cn_int_line_gbtimer_a2               ,
     cn_int_line_gbtimer_a3               ,

//     cn_int_line_gbtimer_bbase            (0x28)
     cn_int_line_gbtimer_b0               ,
     cn_int_line_gbtimer_b1               ,
     cn_int_line_gbtimer_b2               ,
     cn_int_line_gbtimer_b3,

//     CN_INT_LINE_LAST                  ,
    cn_int_line_error                     ,  //
    cn_int_line_etsec1_g1_transmit        ,  //
    cn_int_line_etsec1_g1_receive         ,  //
    cn_int_line_elbc_general_interrupt    ,  //
    cn_int_line_DMA_ch1                   ,  //
    cn_int_line_DMA_ch2                  ,  //
    cn_int_line_DMA_ch3                  , //
    cn_int_line_DMA_ch4                   , //
    cn_int_line_etsec1_g1_error           , //
    cn_int_line_etsec3_g3_transmit        ,  //
    cn_int_line_etsec3_g3_receive         , //
    cn_int_line_etsec3_g1_error           ,  //
    cn_int_line_usb1                     , //
    cn_int_line_etsec1_g0_transmit       ,  //
    cn_int_line_etsec1_g0_receive        ,  //
    cn_int_line_etsec3_g0_transmit       ,  //
    cn_int_line_etsec3_g0_receive        ,  //
    cn_int_line_etsec3_g0_error           ,  //
    cn_int_line_etsec1_g0_error           , //
    cn_int_line_etsec2_g0_transmit        ,  //
    cn_int_line_etsec2_g0_receive         ,  //
    cn_int_line_reserverd_21              ,  //
    cn_int_line_reserverd_22              ,  //
    cn_int_line_reserverd_23              , //
    cn_int_line_etsec2_g0_error           , //
    cn_int_line_reserverd_25              ,  //
    cn_int_line_duart                     ,  //
    cn_int_line_iic_controllers           ,  //
    cn_int_line_performance_monitor       , //
    cn_int_line_security_interrupt1       ,  //
    cn_int_line_usb2                      ,  //
    cn_int_line_gpio                     , //
    cn_int_line_reserverd_32             ,  //
    cn_int_line_reserverd_33             ,  //
    cn_int_line_reserverd_34             ,  //
    cn_int_line_etsec2_g1_transmit        , //
    cn_int_line_etsec2_g1_receive        , //
    cn_int_line_reserverd_37             ,  //
    cn_int_line_reserverd_38             , //
    cn_int_line_reserverd_39             , //
    cn_int_line_reserverd_40               , //
    cn_int_line_reserverd_41             , //
    cn_int_line_security_interrupt2       ,  //
    cn_int_line_espi                     ,  //
    cn_int_line_reserverd_44            ,  //
    cn_int_line_reserverd_45            ,//
    cn_int_line_TDM                     ,  //
    cn_int_line_TDM_error                ,  //
    cn_int_line_reserverd_48              ,  //
    cn_int_line_reserverd_49              , //
    cn_int_line_reserverd_50              ,  //
    cn_int_line_etsec2_g1_error           ,  //
    cn_int_line_etsec1_1588timer          , //
    cn_int_line_etsec2_1588timer          , //
    cn_int_line_etsec3_1588timer         ,  //
    cn_int_line_reserverd_55              , //
    cn_int_line_eSDHC                    ,  //
    cn_int_line_reserverd_57             ,  //
    cn_int_line_reserverd_58             , //
    cn_int_line_reserverd_59            ,  //
    cn_int_line_reserverd_60            ,  //
    cn_int_line_reserverd_61             ,  //
    cn_int_line_reserverd_62             , //
    cn_int_line_reserverd_63              //
};

const ufast_t tg_IntUsedNum = sizeof(tg_IntUsed)/sizeof(ufast_t);
//定义中断线控制数据结构
struct IntLine tg_int_table[sizeof(tg_IntUsed)/sizeof(ufast_t)];


