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
// 模块描述: 异常处理的C部分，这部分涉及具体CPU的功能
// 模块版本: V1.00
// 创建人员: zhangqf_cyg
// 创建时间: 9:30:54 AM/Apr 18, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 备注
// 除了SC TICK EXTERNAL异常以外，其余的所有的异常都不会使用任何的系统调用（会引
// 起调度的API，所以写其他异常代码时务必小心，不要做任何的系统调用，记录异常写介质函数
// 务必保证是直接写，不经过任何的调度这样的代码）

#include "stdint.h"
#include "stddef.h"

#include "os.h"

#include "int_hard.h"

extern struct IntMasterCtrl  tg_int_global;         //定义并初始化总中断控制结构
//int
extern void __Int_EngineReal(ufast_t ufl_line);
extern void __Int_EngineAsynSignal(ufast_t ufl_line);
extern void __Djy_ScheduleAsynSignal(void);
// =============================================================================
// 函数功能：Exp_DTimer  系统TICK中断
// 输入参数：
// 输出参数：无
// 返回值  ：true 成功 false失败
// 说明    ：在E500核心下，该部分作为系统的TICK进行处理
// =============================================================================
void (*user_systick)(void) = NULL_func;
void HardExp_ConnectSystick(void (*tick)(void))
{
    user_systick = tick;
}

extern struct EventECB  *g_ptEventReady;      //就绪队列头
extern struct EventECB  *g_ptEventRunning;    //当前正在执行的事件
extern struct EventECB  *g_ptEventDelay;      //闹钟同步队列表头
extern bool_t  g_bScheduleEnable;                 //系统当前运行状态是否允许调
extern struct IntLine *tg_pIntLineTable[CN_INT_LINE_LAST+1];
extern void __Djy_EventReady(struct EventECB *event_ready);

void Exp_DTimer(void)
{
    g_bScheduleEnable = false;
    tg_int_global.nest_asyn_signal=1;
    user_systick();

    if(g_ptEventReady != g_ptEventRunning)
        __Djy_ScheduleAsynSignal();       //执行中断内调度
    g_bScheduleEnable = true;
    tg_int_global.nest_asyn_signal = 0;
}


// =============================================================================
// 函数功能：Exp_EInt  异步信号的处理引擎
// 输入参数：无
// 输出参数：无
// 返回值     ：
// 说明          ：异步信号不支持嵌套，上层做参数检查处理
// =============================================================================
void Exp_EInt(void)
{
    ufast_t ufl_line;
    struct EventECB *event;
    struct IntLine *ptIntLine;
    u32 isr_result[2]={0,0};

    ufl_line = Int_GetEIntNumber();
    if(ufl_line < CN_INT_LINE_LAST)
    {
        //valid int number
           g_bScheduleEnable = false;
           ptIntLine = tg_pIntLineTable[ufl_line];
            tg_int_global.nest_asyn_signal=1;

            if(ptIntLine->clear_type == CN_INT_CLEAR_AUTO)
                Int_ClearLine(ufl_line);        //中断应答,
            if(ptIntLine->ISR != NULL)
            {
                isr_result[0] = ptIntLine->ISR(ptIntLine->para);
            }
            else
            {
                if(ptIntLine->clear_type == CN_INT_CLEAR_USER)
                    Int_ClearLine(ufl_line);        //中断应答,
            }
//            if(ptIntLine->clear_type == CN_INT_CLEAR_POST)
//                Int_ClearLine(ufl_line);        //中断应答,
            event = ptIntLine->sync_event;
            if(event != NULL)   //看同步指针中有没有事件(注：单个事件，不是队列)
            {
                event->event_result = isr_result[0];
                __Djy_EventReady(event);   //把该事件放到ready队列
                ptIntLine->sync_event = NULL;   //解除同步
            }
            if(ptIntLine->my_evtt_id != CN_EVTT_ID_INVALID)
            {
                isr_result[1] = (u32)ufl_line;
                Djy_EventPop(ptIntLine->my_evtt_id,
                                NULL,0,0,0,0);
            }

            if(g_ptEventReady != g_ptEventRunning)
                __Djy_ScheduleAsynSignal();       //执行中断内调度
            g_bScheduleEnable = true;
            tg_int_global.nest_asyn_signal = 0;
    }
    return;
}

// =============================================================================
// 函数功能：Exp_CInit 实时信号的处理引擎
// 输入参数：中断号
// 输出参数：无
// 返回值     ：
// 说明          ：支持实时信号的嵌套处理;上层做参数检查，本层不用
// =============================================================================
void Exp_CInit(void)
{
    ufast_t ufl_line;
    struct IntLine *ptIntLine;

    ufl_line = Int_GetCIntNumber();
    if((ufl_line > CN_INT_LINE_LAST)&&
        (tg_pIntLineTable[ufl_line] != NULL))
    {
        tg_int_global.nest_real++;
        ptIntLine = tg_pIntLineTable[ufl_line];
        if(ptIntLine->enable_nest == false)
        {
            Int_CutTrunk();
        }

        if(ptIntLine->ISR != NULL)
            ptIntLine->ISR(ptIntLine->para);  //调用用户中断函数

        if(ptIntLine->enable_nest == false)
        {
            Int_ContactTrunk();
        }
        tg_int_global.nest_real--;
    }
    return;
}

// =============================================================================
// 函数功能：异常初始化
// 输入参数：暂时无（后续待定）
// 输出参数：无
// 返回值     :true成功, false失败
// 说明          ：
// =============================================================================
bool_t HardExp_Init(void)
{
    //设置异常向量表
    extern void __AsmSetExpVecTab(void);
    __AsmSetExpVecTab();

    return true;
}

bool_t HardExp_InfoDecoderInit(void)
{
    return true;
}



