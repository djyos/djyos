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
// 模块描述: 定时器硬件接口层数据定义以及接口声明
// 模块版本: V1.00
// 创建人员: zhangqf_cyg
// 创建时间: 2:39:26 PM/Apr 1, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 备注

#ifndef __CPU_PERI_TIMER_H__
#define __CPU_PERI_TIMER_H__

#include "stdint.h"

//timer位域，bo_为位偏移，1位的用位带地址，bb_前缀，多位用掩码，bm_前缀
//uart1 sr寄存器位定义
#define bb_tim1_cr1_base   (0x42000000 + 0x13800*32)           //位带基址
#define bb_tim1_cr1_cen    (*(vu32*)(bb_tim1_cr1_base+4*0))
#define bb_tim1_cr1_udis   (*(vu32*)(bb_tim1_cr1_base+4*1))
#define bb_tim1_cr1_urs    (*(vu32*)(bb_tim1_cr1_base+4*2))
#define bb_tim1_cr1_opm    (*(vu32*)(bb_tim1_cr1_base+4*3))
#define bb_tim1_cr1_dir    (*(vu32*)(bb_tim1_cr1_base+4*4))

//tim1 at 0x40012c00
//tim2 at 0x40000000
//tim3 at 0x40000400
//tim4 at 0x40000800
//tim5 at 0x40000c00
//tim6 at 0x40001000
//tim7 at 0x40001400
//tim8 at 0x40013400
typedef struct
{//             offset
    u32 CR1;    //0x00
    u32 CR2;    //0x04
    u32 SMCR;   //0x08
    u32 DIER;   //0x0c
    u32 SR;     //0x10
    u32 EGR;    //0x14
    u32 CCMR1;  //0x18
    u32 CCMR2;  //0x1c
    u32 CCER;   //0x20
    u32 CNT;    //0x24
    u32 PSC;    //0x28
    u32 ARR;    //0x2c
    u32 RCR;    //0x30
    u32 CCR1;   //0x34
    u32 CCR2;   //0x38
    u32 CCR3;   //0x3c
    u32 CCR4;   //0x40
    u32 BDTR;   //0x44
    u32 DCR;    //0x48
    u32 DMAR;   //0x4c
}tagTimerReg;

#define TIM_CR1_CEN_MASK        (1<<0)
#define TIM_CR1_OPM_MASK        (1<<3)
#define TIM_CR1_ARPE_MASK       (1<<7)

#define TIM_DIER_UIE_MASK       (1<<0)
#define TIM_SR_UIF_MASK         (1<<0)

#endif /* __CPU_PERI_TIMER_H__ */
