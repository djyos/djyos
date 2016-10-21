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
//作者：lst
//版本：V1.1.0
//文件描述: cm3异常处理模块
//其他说明:
//修订历史:
//2. 日期: 2010-01-20
//   作者: lst
//   新版本号: V1.1.0
//   修改说明: 修改 exp_esr_tick函数，否则在tick中断中调用异步信号的使能和禁止
//      函数时，由于en_asyn_signal_counter等参数未正确设置，会出问题。
//1. 日期: 2009-08-30
//   作者: lst
//   新版本号: V1.0.0
//   修改说明:
//------------------------------------------------------
#include "stdint.h"
#include "arch_feature.h"
#include "cortexm0.h"
#include "hard-exp.h"

extern struct IntLine *tg_pIntLineTable[];       //中断线查找表
extern struct IntMasterCtrl  tg_int_global;          //定义并初始化总中断控制结构
extern void __Djy_ScheduleAsynSignal(void);

void (*user_systick)(u32 inc_ticks);
void HardExp_SystickHandler(void);
void HardExp_SvcHandler(void);


void HardExp_Init(void)
{
    pg_scb_reg->pri12_15 |=0xff000000;    //systick设为最低优先级,=异步信号
    pg_scb_reg->pri8_11 |= 0xff000000;    //svc的优先级和异步信号相同。
}
void HardExp_ConnectNmi(void (*esr)(void))
{
}

void HardExp_ConnectSystick(void (*tick)(u32 inc_ticks))
{
    user_systick = tick;
}

void HardExp_EsrTick(void)
{
    if((pg_systick_reg->ctrl & bm_systick_ctrl_tickint) == 0)
        return;
    g_bScheduleEnable = false;
//    tg_int_global.en_asyn_signal = false;
//    tg_int_global.en_asyn_signal_counter = 1;
    tg_int_global.nest_asyn_signal++;
    user_systick(1);

    tg_int_global.nest_asyn_signal--;
//    tg_int_global.en_asyn_signal = true;
//    tg_int_global.en_asyn_signal_counter = 0;
    if(g_ptEventReady != g_ptEventRunning)
        __Djy_ScheduleAsynSignal();       //执行中断内调度
    g_bScheduleEnable = true;
}
