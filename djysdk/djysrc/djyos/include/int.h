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
#ifndef __INT_H__
#define __INT_H__


#include "arch_feature.h"
#include "cpu_peri_int_line.h"

#ifdef __cplusplus
extern "C" {
#endif

#define  CN_ASYN_SIGNAL     (0)
#define  CN_REAL            (1)

//清中断方式
#define  CN_INT_CLEAR_PRE    0   //调用ISR之前由系统自动清
#define  CN_INT_CLEAR_USER   1   //系统不清，由用户在ISR中清，默认方式
#define  CN_INT_CLEAR_POST   2   //调用ISR返回之后、中断返回前由系统自动清

//表示各中断线状态的位图占ucpu_t类型的字数
#define CN_INT_BITS_WORDS   ((CN_INT_LINE_LAST+CN_CPU_BITS)/CN_CPU_BITS)

//中断线数据结构，每中断一个
//移植敏感
struct tagIntLine
{
    u32 (*ISR)(ufast_t line);
    struct  tagEventECB *sync_event;       //正在等待本中断发生的事件
    ucpu_t en_counter;          //禁止次数计数,等于0时表示允许中断
    ufast_t int_type;           //1=实时中断,0=异步信号
    ufast_t clear_type;          //应答方式
    bool_t enable_nest;         //true=本中断响应期间允许嵌套，对硬件能力有依赖
                                //性，也与软件设置有关。例如cortex-m3版本，异步
                                //信号被设置为最低优先级，从而所有异步信号都不
                                //允许嵌套。
                                //特别注意，实时中断能够无条件嵌套异步信号。
                                //中断响应后，由中断引擎根据enable_nest的值使能
                                //或禁止中断来控制是否允许嵌套，如果在响应中断
                                //后，硬件没有立即禁止中断，将有一个小小的"窗口"
                                //，在该窗口内，是允许嵌套的。例如cm3的实时中断
    uint16_t my_evtt_id;
    u32  prio;                  //优先级，含义由使用者解析
};

//中断总控数据结构.
struct tagIntMasterCtrl
{
    //中断线属性位图，0=异步信号，1=实时中断,数组的位数刚好可以容纳中断数量,与
    //中断线数据结构的int_type成员含义相同。
    ucpu_t  property_bitmap[CN_INT_BITS_WORDS];
    ucpu_t nest_asyn_signal;   //中断嵌套深度,主程序=0,第一次进入中断=1,依次递加
    ucpu_t nest_real;   //中断嵌套深度,主程序=0,第一次进入中断=1,依次递加
    //中断线使能位图,1=使能,0=禁止,反映相应的中断线的控制状态,
    //与总开关/异步信号开关的状态无关.
    ucpu_t  enable_bitmap[CN_INT_BITS_WORDS];
//    bool_t  en_trunk;           //1=总中断使能,  0=总中断禁止
//    bool_t  en_asyn_signal;         //1=异步信号使能,0=异步信号禁止
    ucpu_t en_trunk_counter;   //全局中断禁止计数,=0表示允许全局中断
    ucpu_t en_asyn_signal_counter; //异步信号禁止计数,=0表示允许异步信号
};

atom_high_t Int_HighAtomStart(void);
void Int_HighAtomEnd(atom_high_t high);
atom_low_t Int_LowAtomStart(void);
void Int_LowAtomEnd(atom_low_t low);
void Int_SaveTrunk(void);
void Int_RestoreTrunk(void);
bool_t Int_CheckTrunk(void);
void Int_SaveAsynSignal(void);
u32 Int_GetRunLevel(void);
void Int_RestoreAsynSignal(void);
void Int_HalfEnableAsynSignal(void);
bool_t Int_CheckAsynSignal(void);
bool_t Int_SaveAsynLine(ufast_t ufl_line);
bool_t Int_SaveRealLine(ufast_t ufl_line);
bool_t Int_RestoreAsynLine(ufast_t ufl_line);
bool_t Int_RestoreRealLine(ufast_t ufl_line);
bool_t Int_DisableAsynLine(ufast_t ufl_line);
bool_t Int_DisableRealLine(ufast_t ufl_line);
bool_t Int_EnableAsynLine(ufast_t ufl_line);
bool_t Int_EnableRealLine(ufast_t ufl_line);
bool_t Int_CheckLine(ufast_t ufl_line);
bool_t Int_SetClearType(ufast_t ufl_line,ufast_t clear_type);
void Int_IsrConnect(ufast_t ufl_line, u32 (*isr)(ufast_t));
bool_t Int_EvttConnect(ufast_t ufl_line,uint16_t my_evtt_id);
void Int_IsrDisConnect(ufast_t ufl_line);
void Int_EvttDisConnect(ufast_t ufl_line);
void Int_ResetAsynSync(ufast_t ufl_line);
bool_t Int_AsynSignalSync(ufast_t ufl_line);

//以下函数在int_hard.c中实现
void Int_ContactAsynSignal(void);
void Int_CutAsynSignal(void);
void Int_ContactTrunk(void);
void Int_CutTrunk(void);
bool_t Int_ContactLine(ufast_t ufl_line);
bool_t Int_CutLine(ufast_t ufl_line);
bool_t Int_ClearLine(ufast_t ufl_line);
bool_t Int_TapLine(ufast_t ufl_line);
bool_t Int_QueryLine(ufast_t ufl_line);
bool_t Int_SettoAsynSignal(ufast_t ufl_line);
bool_t Int_SettoReal(ufast_t ufl_line);
bool_t Int_SetPrio(ufast_t ufl_line,u32 prio);
void Int_Init(void);


#ifdef __cplusplus
}
#endif

#endif //__INT_H__
