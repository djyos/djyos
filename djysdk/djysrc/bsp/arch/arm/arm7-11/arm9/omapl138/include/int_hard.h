/*========================================================*
 * 文件模块说明:                                                 *
 * omapl138的中断硬件相关文件，移植自都江堰操作系统作者拥有版权
 * 的S3C2440中断硬件相关文件，针对omapl138做了一些修改
 * 文件版本: v1.00                                               *
 * 开发人员: lst                              *
 * 创建时间: 2011.06.01                                          *
 * Copyright(c) 2011-2011  深圳鹏瑞软件有限公司             *
 *========================================================*
 * 程序修改记录(最新的放在最前面):                               *
 *  <版本号> <修改日期>, <修改人员>: <修改功能概述>              *
 *========================================================*
* 例如：
*  V1.02 2004-07-14, 张三: 接收函数增加对报文长度有效性校核
*
*/
//以下是原作者版权声明
//----------------------------------------------------
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
//所属模块:中断模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 与中断相关的代码，包含异步信号与实时中断
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-03-10
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __INT_HARD_H__
#define __INT_HARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

//定义原子保护函数中用来保存原子状态的变类型，与中断控制器有关，通常是cpu的字长，
//量但有例外，例如在2416中，atom_low_t被定义为u64。在adsp21469中，甚至被
//定义为结构。
typedef u32 atom_high_t;
typedef u32 atom_low_t;

#define reg_hier_fiq_bit    0x01    //hier寄存器的bit0=1使能fiq，=0禁止fiq
#define reg_hier_irq_bit    0x02    //hier寄存器的bit1=1使能fiq，=0禁止fiq
//注:"引用"的意思是按中断号对个别中断执行一定的操作
//   "掩码"的意思是用位掩码的方式同时对多个中断执行操作
//注1:中断响应后，因不冻结GPIR和GPVR，存在读取其中一个时，另一个发生变化的可能，
//    在现实中很难应用
struct st_int_reg  //基址 0xfffee000 偏移
{
    vu32 REVID;      //0000 硬件版本寄存器
    vu32 CR;         //0004 控制寄存器
    vu8 RSVD0[8];
    vu32 GER;        //0010 全局使能寄存器
    vu8 RSVD1[8];
    vu32 GNLR;       //001c 全局嵌套级别寄存器
    vu32 SISR;       //0020 中断状态引用设置寄存器
    vu32 SICR;       //0024 中断状态引用清除寄存器
    vu32 EISR;       //0028 中断状态引用使能寄存器
    vu32 EICR;       //002c 中断状态引用除能寄存器
    vu8 RSVD2[4];
    vu32 HIEISR;     //0034 写0使能fiq，写1使能irq
    vu32 HIDISR;     //0038 写0除能fiq，写1除能irq
    vu8 RSVD3[20];
    vu32 VBR;        //0050 向量表基址寄存器
    vu32 VSR;        //0054 向量表中每项尺寸
    vu32 VNR;        //0058 空向量寄存器--------指向空函数即可
    vu8 RSVD4[36];
    vu32 GPIR;       //0080 正悬挂的最高优先级中断----无意义，见注1
    vu32 GPVR;       //0084 正悬挂的最高优先级中断的向量
    vu8 RSVD5[376];
    vu32 SRSR[4];    //0200 悬挂状态掩码寄存器，写入相应位可引起中断
    vu8 RSVD6[112];
    vu32 SECR[4];    //0280 位掩码，写1清除中断标志，(读无效，ti文档有误)
    vu8 RSVD7[112];
    vu32 ESR[4];     //0300 位掩码，只写，写1使能相应中断，写0无效
    vu8 RSVD8[112];
    vu32 ECR[4];     //0380 位掩码，只写，写1除能相应中断，写0无效
    vu8 RSVD9[112];
    vu32 CMR[26];    //0400 通道设置，设置中断号与中断通道的对应关系，
                             //     共26个寄存器，每个中断占用8位，通道号对应
                             //     的是优先级，通道号低优先级高。同一个通道
                             //     中，中断号低的优先级高。
                             //     实时中断的通道号可任意设置为0或1.
                             //     异步信号的通道号可任意设置为2~31
    vu8 RSVD10[1176];
    vu32 HIPIR1;     //0900 悬挂的fiq中断中最高优先级中断号
    vu32 HIPIR2;     //0904 悬挂的irq中断中最高优先级中断号
    vu8 RSVD11[2040];
    vu32 HINLR1;     //1100 fiq中断嵌套级别寄存器，一般不用
    vu32 HINLR2;     //1104 irq中断嵌套级别寄存器，一般不用
    vu8 RSVD14[1016];
    vu32 HIER;       //1500 bit0写1使能fiq，写0除能，bit1操作irq。
    vu8 RSVD15[252];
    vu32 HIPVR1;     //1600 正在响应的fiq中断向量地址
    vu32 HIPVR2;     //1604 正在响应的irq中断向量地址
};

#define cn_noirq    0x80
#define cn_nofiq    0x40
#define cn_noint    0xc0

void __Int_ClearAllLine(void);
void __Int_InitHard(void);
void __Int_EngineReal(ufast_t ufl_line);
void __Int_EngineAsynSignal(ufast_t ufl_line);
void __Int_EngineAll(ufast_t ufl_line);

void int_enable_arm_int(void);
void int_disable_arm_int(void);
void __int_enable_irq(void);
void __int_disable_irq(void);
void __int_enable_fiq(void);
void __int_disable_fiq(void);

#ifdef __cplusplus
}
#endif

#endif //__INT_HARD_H__
