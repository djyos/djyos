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
//版本：V2.0.1
//文件描述: 中断模块的硬件无关部分
//其他说明:
//修订历史:
//3. 日期: 2010-01-12
//   作者:  罗侍田.
//   新版本号: V2.0.1
//   修改说明:
//   1、查到了一个非常隐蔽的问题，在__int_engine_real函数内，这两行
//      if(tg_int_global.en_trunk_counter == 0)
//          tg_int_global.en_trunk = true;      //原误写成false了
//   2、在int_save_trunk,Int_RestoreTrunk,Int_SaveAsynSignal,
//      int_restore_asyn_signal几个函数中增加对bg_schedule_enable的操作
//2. 日期: 2010-01-04
//   作者:  罗侍田.
//   新版本号: V2.0.0
//   修改说明: 从原版本中去掉硬件相关部分而成，存放位置也从arch改到djyos
//1. 日期: 2009-12-10
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 从44b0版本移植而成
//------------------------------------------------------

//实时中断ISR的限制:
// 1、不允许调用任何系统调用。
// 2、不允许改变任何中断线的异步信号还是实时中断属性。
// 3、不允许使能或禁止异步信号，包括信号线和总异步信号开关。
// 4、允许使能或禁止属性为实时中断的中断线

#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stddef.h"
#include "int_hard.h"
#include "int.h"
#include "djyos.h"

struct IntLine *tg_pIntLineTable[CN_INT_LINE_LAST+1];
struct IntMasterCtrl  tg_int_global;

extern void __Djy_CutReadyEvent(struct EventECB *event);
extern bool_t __Djy_Schedule(void);

//----保存当前状态并禁止异步信号------------------------------------------------
//功能：本函数是int_restore_asyn_signal()的姊妹函数，调用本函数使禁止次数增加，
//      调用一次int_restore_asyn_signal()使禁止次数减少。
//      若当前次数为0，增加为1并禁止异步信号，不为0时简单地增1
//参数：无
//返回：无
//------------------------------------------------------------------------------
void Int_SaveAsynSignal(void)
{
    if(tg_int_global.nest_asyn_signal != 0)
        return;

    Int_CutAsynSignal();
    //达上限后再加会回绕到0
    if(tg_int_global.en_asyn_signal_counter != CN_LIMIT_UCPU)
        tg_int_global.en_asyn_signal_counter++;
    //原算法是从0->1的过程中才进入，但如果在en_asyn_signal_counter != 0的状态下
    //因故障使中断关闭，将使用户后续调用的en_asyn_signal_counter起不到作用
    g_bScheduleEnable = false;
    return;
}

//----复位异步信号开关------------------------------------------------
//功能：把异步信号开关恢复到初始状态，即en_asyn_signal_counter=1的状态，初始化
//      中断系统后，还要做大量的模块初始化工作才能启动多事件调度，在启动多事件
//      调度前调用本函数复位异步信号状态，可以防止模块初始化代码的bug意外修改
//      了异步信号使能状态。
//参数：无
//返回：无
//-----------------------------------------------------------------------------
void __Int_ResetAsynSignal(void)
{
    Int_CutAsynSignal();
    tg_int_global.en_asyn_signal_counter =1;
    return;
}

//----查询当前软件级别---------------------------------------------------------
//功能: 用于确定当前是运行在异步信号中断，还是在线程中。
//参数: 无
//返回: 0 = 线程中。1=异步信号中断，大于1表示异步信号嵌套级数。
//-----------------------------------------------------------------------------
u32 Int_GetRunLevel(void)
{
    return tg_int_global.nest_asyn_signal;
}

//----恢复保存的异步信号状态----------------------------------------------------
//功能：本函数是int_save_asyn_signal()的姊妹函数，调用本函数使禁止次数减少，调用
//      一次int_save_asyn_signal()是禁止次数增加。
//      当次数减至0时激活异步信号,否则简单减1
//参数：无
//返回：无
//------------------------------------------------------------------------------
void Int_RestoreAsynSignal(void)
{
    if(tg_int_global.nest_asyn_signal != 0)
        return;
    if(tg_int_global.en_asyn_signal_counter != 0)
        tg_int_global.en_asyn_signal_counter--;
    if(tg_int_global.en_asyn_signal_counter==0)
    {
//        tg_int_global.en_asyn_signal = true;   //异步信号设为使能
//        if(tg_int_global.en_trunk_counter == 0)
//        {
            g_bScheduleEnable = true;
            if(g_ptEventRunning!= g_ptEventReady)
            {
                __Djy_Schedule();
//                Int_ContactAsynSignal();    //汇编中已经打开，无须再调用
            }else
                Int_ContactAsynSignal();
//        }else
//            Int_ContactAsynSignal();
    }else
    {
        Int_CutAsynSignal();    //防止counter>0期间意外(bug)打开
        g_bScheduleEnable = false;
    }
    return;
}

//----开异步信号前半部---------------------------------------------------------
//功能: 这是一个专门为上下文切换准备的函数/
//      在数据结构中异步信号设置为开启状态，但不完成最后一步:合上异步信号开关。
//      用于上下文切换过程中，先行设置好状态，使得上下文切换的最后一步和重新打开
//      中断同时完成。这个过程必然要关中断，有些系统没有能够与操作pc同时完成的、
//      独立的异步信号开关，这个步骤势必会把实时中断也关掉，因此把设置数据结构的
//      部分提出来，以减少关闭实时中断的时间。这使得部分cpu不能实现"实时中断永不
//      关闭"的要求，但是关闭时间是非常短的。
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void Int_HalfEnableAsynSignal(void)
{
    tg_int_global.en_asyn_signal_counter = 0;
}

//----查看异步信号是否允许-----------------------------------------------------
//功能：
//参数：无
//返回：允许返回true,禁止返回false
//注意: 低级原子操作也会控制异步信号开关，本函数没有考虑其影响
//-----------------------------------------------------------------------------
bool_t Int_CheckAsynSignal(void)
{
    if( tg_int_global.en_asyn_signal_counter == 0)
        return true;
    else
        return false;
}

//----保存当前状态并禁止异步信号中断线-----------------------------------------
//功能：本函数是int_restore_asyn_line()的姊妹函数，调用本函数使禁止次数增加，调
//      用一次int_restore_asyn_line是禁止次数减少。
//      若当前次数为0，增加为1并禁止中断线，不为0时简单地增1
//参数：ufl_line
//返回：无
//-----------------------------------------------------------------------------
bool_t Int_SaveAsynLine(ufast_t ufl_line)
{
    if( (ufl_line > CN_INT_LINE_LAST)
            || (tg_pIntLineTable[ufl_line] == NULL)
            || (tg_int_global.nest_real != 0) )
        return false;
    if(tg_pIntLineTable[ufl_line]->int_type == CN_REAL)
        return false;
    Int_CutLine(ufl_line);
    if(tg_pIntLineTable[ufl_line]->en_counter!=CN_LIMIT_UCPU)//达上限后再加会回绕到0
        tg_pIntLineTable[ufl_line]->en_counter++;
    //原算法是从0->1的过程中才进入，但如果在en_counter != 0的状态下
    //因故障使中断关闭，将使用户后续调用的en_counter起不到作用
    tg_int_global.enable_bitmap[ufl_line/CN_CPU_BITS]
                &= ~(1<<(ufl_line % CN_CPU_BITS));
    return true;
}

//----保存当前状态并禁止异步信号中断线-----------------------------------------
//功能：本函数是int_restore_real_line()的姊妹函数，调用本函数使禁止次数增加，调
//      用一次int_restore_real_line是禁止次数减少。
//      若当前次数为0，增加为1并禁止中断线，不为0时简单地增1
//参数：ufl_line
//返回：无
//------------------------------------------------------------------------------
bool_t Int_SaveRealLine(ufast_t ufl_line)
{
    if( (ufl_line > CN_INT_LINE_LAST)
            || (tg_pIntLineTable[ufl_line] == NULL) )
        return false;
    if(tg_pIntLineTable[ufl_line]->int_type == CN_ASYN_SIGNAL)
        return false;
    Int_CutLine(ufl_line);
    if(tg_pIntLineTable[ufl_line]->en_counter!=CN_LIMIT_UCPU)//达上限后再加会回绕到0
        tg_pIntLineTable[ufl_line]->en_counter++;
    //原算法是从0->1的过程中才进入，但如果在en_counter != 0的状态下
    //因故障使中断关闭，将使用户后续调用的en_counter起不到作用
    tg_int_global.enable_bitmap[ufl_line/CN_CPU_BITS]
                &= ~(1<<(ufl_line % CN_CPU_BITS));
    return true;
}

//----恢复保存的异步信号中断线状态---------------------------------------------
//功能：本函数是int_save_line（）的姊妹函数，调用本函数使禁止次数减少，调
//      用一次int_save_line是禁止次数增加。
//      当次数减至0时激活中断线,否则简单减1
//参数：ufl_line
//返回：无
//注: 本函数不允许在实时中断ISR中调用，若调用直接返回false。
//-----------------------------------------------------------------------------
bool_t Int_RestoreAsynLine(ufast_t ufl_line)
{
    if( (ufl_line > CN_INT_LINE_LAST)
            || (tg_pIntLineTable[ufl_line] == NULL)
            || (tg_int_global.nest_real != 0) )
        return false;
    if(tg_pIntLineTable[ufl_line]->int_type == CN_REAL)
        return false;
    if(tg_pIntLineTable[ufl_line]->en_counter != 0)
        tg_pIntLineTable[ufl_line]->en_counter--;
    if(tg_pIntLineTable[ufl_line]->en_counter==0)
    {
        tg_int_global.enable_bitmap[ufl_line/CN_CPU_BITS]
                |= 1<<(ufl_line % CN_CPU_BITS);
        Int_ContactLine(ufl_line);
    }
    return true;
}

//----恢复保存的实时中断线状态-------------------------------------------------
//功能：本函数是int_save_line（）的姊妹函数，调用本函数使禁止次数减少，调
//      用一次int_save_line是禁止次数增加。
//      当次数减至0时激活中断线,否则简单减1
//参数：ufl_line
//返回：无
//注: 与int_restore_asyn_line不一样，本函数在异步信号和实时中断ISR中都允许调用
//-----------------------------------------------------------------------------
bool_t Int_RestoreRealLine(ufast_t ufl_line)
{
    if( (ufl_line > CN_INT_LINE_LAST)
            || (tg_pIntLineTable[ufl_line] == NULL) )
        return false;
    if(tg_pIntLineTable[ufl_line]->int_type == CN_ASYN_SIGNAL)
        return false;
    if(tg_pIntLineTable[ufl_line]->en_counter != 0)
        tg_pIntLineTable[ufl_line]->en_counter--;
    if(tg_pIntLineTable[ufl_line]->en_counter==0)
    {
        tg_int_global.enable_bitmap[ufl_line/CN_CPU_BITS]
                |= 1<<(ufl_line % CN_CPU_BITS);
        Int_ContactLine(ufl_line);
    }
    return true;
}

//----直接禁止异步信号中断线---------------------------------------------------
//功能：本函数是int_enable_asyn_line()的姊妹函数，调用本函数使中断线的使能计数器
//      置位，并掐断中断线
//参数：ufl_line
//返回：无
//------------------------------------------------------------------------------
bool_t Int_DisableAsynLine(ufast_t ufl_line)
{
    if( (ufl_line > CN_INT_LINE_LAST)
            || (tg_pIntLineTable[ufl_line] == NULL)
            || (tg_int_global.nest_real != 0) )
        return false;
    if(tg_pIntLineTable[ufl_line]->int_type == CN_REAL)
        return false;
    Int_CutLine(ufl_line);
    tg_pIntLineTable[ufl_line]->en_counter = 1;
    tg_int_global.enable_bitmap[ufl_line/CN_CPU_BITS]
                &= ~(1<<(ufl_line % CN_CPU_BITS));
    return true;
}

//----直接禁止实时中断线-------------------------------------------------------
//功能：本函数是int_enable_real_line()的姊妹函数，调用本函数使中断线的使能计数器
//      置位，并掐断中断线
//参数：ufl_line
//返回：无
//------------------------------------------------------------------------------
bool_t Int_DisableRealLine(ufast_t ufl_line)
{
    if( (ufl_line > CN_INT_LINE_LAST)
            || (tg_pIntLineTable[ufl_line] == NULL) )
        return false;
    if(tg_pIntLineTable[ufl_line]->int_type == CN_ASYN_SIGNAL)
        return false;
    Int_CutLine(ufl_line);
    tg_pIntLineTable[ufl_line]->en_counter = 1;
    tg_int_global.enable_bitmap[ufl_line/CN_CPU_BITS]
                &= ~(1<<(ufl_line % CN_CPU_BITS));
    return true;
}

//----直接允许异步信号中断线---------------------------------------------------
//功能：本函数是int_disable_asyn_line()的姊妹函数，调用本函数使中断线的使能计数
//      器归零，并接通中断线
//参数：ufl_line
//返回：无
//------------------------------------------------------------------------------
bool_t Int_EnableAsynLine(ufast_t ufl_line)
{
    if( (ufl_line > CN_INT_LINE_LAST)
            || (tg_pIntLineTable[ufl_line] == NULL)
            || (tg_int_global.nest_real != 0) )
        return false;
    if(tg_pIntLineTable[ufl_line]->int_type == CN_REAL)
        return false;
    tg_pIntLineTable[ufl_line]->en_counter = 0;
    tg_int_global.enable_bitmap[ufl_line/CN_CPU_BITS]
                |= 1<<(ufl_line % CN_CPU_BITS);
    Int_ContactLine(ufl_line);
    return true;
}

//----直接允许实时中断线-------------------------------------------------------
//功能：本函数是int_disable_real_line()的姊妹函数，调用本函数使中断线的使能计数
//      器归零，并接通中断线
//参数：ufl_line
//返回：无
//------------------------------------------------------------------------------
bool_t Int_EnableRealLine(ufast_t ufl_line)
{
    if( (ufl_line > CN_INT_LINE_LAST)
            || (tg_pIntLineTable[ufl_line] == NULL) )
        return false;
    if(tg_pIntLineTable[ufl_line]->int_type == CN_ASYN_SIGNAL)
        return false;

    tg_pIntLineTable[ufl_line]->en_counter = 0;
    tg_int_global.enable_bitmap[ufl_line/CN_CPU_BITS]
                |= 1<<(ufl_line % CN_CPU_BITS);
    Int_ContactLine(ufl_line);
    return true;
}

//----查询中断线使能状态-------------------------------------------------------
//功能：查询中断线是否允许
//参数：ufl_line，欲查询的中断线
//返回：true = 使能，false = 禁止。
//-----------------------------------------------------------------------------
bool_t Int_CheckLine(ufast_t ufl_line)
{
    if( (ufl_line > CN_INT_LINE_LAST)
            || (tg_pIntLineTable[ufl_line] == NULL) )
        return false;
    if(tg_pIntLineTable[ufl_line]->en_counter == 0)
        return true;
    else
        return false;
}

//----设置中断应答方式---------------------------------------------------------
//功能: 设置某中断线的应答方式
//参数：ufl_line，欲设置的中断线
//      clear_type, 应答方式，以下三者之一:
//          CN_INT_CLEAR_AUTO    调用ISR之前应答，默认方式
//          CN_INT_CLEAR_USER    系统不应答，由用户在ISR中应答
//返回: true=成功，false=失败，该中断线被设为实时中断，固定为cn_int_clear_user
//-----------------------------------------------------------------------------
bool_t Int_SetClearType(ufast_t ufl_line,ufast_t clear_type)
{
    if( (ufl_line > CN_INT_LINE_LAST)
            || (tg_pIntLineTable[ufl_line] == NULL) )
        return false;
    if(tg_pIntLineTable[ufl_line]->int_type == CN_REAL)
        return false;
    else
        tg_pIntLineTable[ufl_line]->clear_type = clear_type;
    return true;
}

//----关联中断线与ISR----------------------------------------------------------
//功能：指定中断线指定中断响应函数，该函数为普通函数，
//参数：ufl_line,需要设置的中断线号
//      isr，中断响应函数，由用户提供，原型：void isr(ufast_t)
//返回：无
//-----------------------------------------------------------------------------
void Int_IsrConnect(ufast_t ufl_line, u32 (*isr)(ptu32_t))
{
    if( (ufl_line > CN_INT_LINE_LAST)
            || (tg_pIntLineTable[ufl_line] == NULL) )
        return;
    tg_pIntLineTable[ufl_line]->ISR = isr;
    return;
}


void Int_SetIsrPara(ufast_t ufl_line,ptu32_t para)
{

    if( (ufl_line > CN_INT_LINE_LAST)
            || (tg_pIntLineTable[ufl_line] == NULL) )
        return;
    tg_pIntLineTable[ufl_line]->para = para;
    return;
}

//----关联中断线与事件类型-----------------------------------------------------
//功能：为指定该中断线指定一个事件类型id，如果是异步信号，则在返回主程序前弹出
//      事件类型为该id的事件，如果是实时中断，则不弹出事件。
//参数：ufl_line,需要设置的中断线号
//      my_evtt_id，事件类型id
//返回：true = 成功连接事件类型 , false = 失败。
//-----------------------------------------------------------------------------
bool_t Int_EvttConnect(ufast_t ufl_line,uint16_t my_evtt_id)
{
    if( (ufl_line > CN_INT_LINE_LAST)
            || (tg_pIntLineTable[ufl_line] == NULL) )
        return false;
    if(tg_pIntLineTable[ufl_line]->int_type != CN_ASYN_SIGNAL)
        return false;
    tg_pIntLineTable[ufl_line]->my_evtt_id = my_evtt_id;
    return true;
}

//----断开中断线与中断响应函数的关联-------------------------------------------
//功能：断开指定中断线指定中断响应函数的关联，代之以空函数
//参数：ufl_line,需要设置的中断线号
//返回：无
//-----------------------------------------------------------------------------
void Int_IsrDisConnect(ufast_t ufl_line)
{
    if( (ufl_line > CN_INT_LINE_LAST)
            || (tg_pIntLineTable[ufl_line] == NULL) )
        return;
    tg_pIntLineTable[ufl_line]->ISR = NULL;
    return;
}

//----断开中断线与事件类型的关联-----------------------------------------------
//功能：断开指定中断线指定事件类型的关联，代之以cn_invalid_evtt_id
//参数：ufl_line,需要设置的中断线号
//返回：无
//-----------------------------------------------------------------------------
void Int_EvttDisConnect(ufast_t ufl_line)
{
    if( (ufl_line > CN_INT_LINE_LAST)
            || (tg_pIntLineTable[ufl_line] == NULL) )
        return;
    tg_pIntLineTable[ufl_line]->my_evtt_id = CN_EVTT_ID_INVALID;
    return;
}

//----设定中断同步-------------------------------------------------------------
//功能: 阻塞正在处理的事件的线程，直到指定的中断线的中断发生、响应并返回，然后才
//      激活线程。调用前确保该中断线被禁止，调用本函数将导致中断线被使能(是直接
//      使能，不是调用int_save_asyn_line)，并在返回后恢复禁止状态。
//参数: ufl_line,等待的目标中断线
//返回: false = 该中断已经被其他线程等待，直接返回。
//      true = 成功同步，并在该中断发生后返回。
//备注: 1.中断是一种临界资源，不宜在中断函数中太多的事情，故中断同步的功能比较简
//      单，每条中断线同一时刻只能有一个线程等待，也不允许设置超时等待
//      2.秉承djyos一贯风格，中断同步函数只能把自己置入等待状态，而不能控制别的
//      线程，故函数原型不能是 bool_t int_sync(ufast_t line,uint16_t event_id)
//      3.实时中断设置等待无效，调用本函数时，如果line已经被设置为实时中断，则
//      直接返回false，如果调用本函数后，line不能被设置为实时中断。
//      4.为正确实现功能，须在调用前确保该中断线是被禁止的。
//-----------------------------------------------------------------------------
bool_t Int_AsynSignalSync(ufast_t ufl_line)
{
    if( (ufl_line > CN_INT_LINE_LAST)
            || (tg_pIntLineTable[ufl_line] == NULL) )
        return false;
    if( !Djy_QuerySch())
    {   //禁止调度，不能进入异步信号同步状态。
        Djy_SaveLastError(EN_KNL_CANT_SCHED);
        return false;
    }
    Int_SaveAsynSignal();   //在操作就绪队列期间不能发生中断
    //实时中断不能设置同步，一个中断只接受一个同步事件
    if((tg_pIntLineTable[ufl_line]->int_type == CN_REAL)
            || (tg_pIntLineTable[ufl_line]->sync_event != NULL))
    {
        Int_RestoreAsynSignal();
        return false; //实时中断或已经有同步事件
    }else
    {
        if(Int_QueryLine(ufl_line) == true)    //中断已经发生，同步条件达到
        {
            Int_ClearLine(ufl_line);
            Int_RestoreAsynSignal();
            return true;
        }
        //以下三行从就绪链表中取出running事件
        __Djy_CutReadyEvent(g_ptEventRunning);
        g_ptEventRunning->next = NULL;
        g_ptEventRunning->previous = NULL;
        g_ptEventRunning->event_status = CN_STS_WAIT_ASYN_SIGNAL;
        tg_pIntLineTable[ufl_line]->sync_event = g_ptEventRunning;
    }
    Int_EnableAsynLine(ufl_line);
    Int_RestoreAsynSignal();      //调用本函数将引发线程切换，正在处理的事件被
                                    //挂起。
    Int_DisableAsynLine(ufl_line);
    g_ptEventRunning->wakeup_from = CN_STS_WAIT_ASYN_SIGNAL;
    g_ptEventRunning->event_status = CN_STS_EVENT_READY;
    return true;
}

//----注册中断到中断模块----------------------------------------------------------
//功能: 注册中断到操作系统的中断模块，动态分配结构为struct IntLine，并初始化它，赋值对应
//      的中断位图tg_pIntLineTable[ufl_line ]
//参数: ufl_line,等待的目标中断线
//返回: false = 该中断已经被其他线程等待，直接返回。
//      true = 成功同步，并在该中断发生后返回。
//说明：必须先注册成功，调用其它中断API才有效
//-----------------------------------------------------------------------------
bool_t Int_Register(ufast_t ufl_line)
{
	struct IntLine *pIntLine;
    if(ufl_line > CN_INT_LINE_LAST)
        return false;
    if (tg_pIntLineTable[ufl_line] != NULL) //说明已经注册
	 return true;

    pIntLine = (struct IntLine*)malloc(sizeof(struct IntLine));
    if(NULL == pIntLine)
    {
    	return false;
    }
	memset(pIntLine,0x00,sizeof(struct IntLine));
	pIntLine->para = ufl_line;
	pIntLine->en_counter = 1;               //禁止中断,计数为1
	pIntLine->int_type = CN_ASYN_SIGNAL;    //设为异步信号
	pIntLine->clear_type = CN_INT_CLEAR_AUTO;//设为调用ISR前应答
	//所有中断函数指针指向空函数
	pIntLine->ISR = (u32 (*)(ufast_t))NULL_func;
	pIntLine->sync_event = NULL;                //同步事件空
	pIntLine->my_evtt_id = CN_EVTT_ID_INVALID;  //不弹出事件

	tg_pIntLineTable[ufl_line] = pIntLine;

	return true;
}

//原则上，中断函数一经注册，便不能注销，此处只是与Int_Registerd函数成对出现
bool_t Int_UnRegister(ufast_t ufl_line)
{
	struct IntLine *pIntLine;
    if(ufl_line > CN_INT_LINE_LAST)
        return false;
    if (tg_pIntLineTable[ufl_line] == NULL) //
	 return false;

    pIntLine = tg_pIntLineTable[ufl_line];
    free(pIntLine);
	return true;
}
//特注: 不提供周期性中断同步功能，因为djyos不提供无条件休眠或者挂起的功能，已周
//      期性时钟中断为例，一次时钟中断把线程触发进入ready后，到下次时钟中断到来
//      之前，该线程要么还在ready态(可能被其他线程抢占)，此时不需要再次触发，
//      要么在等待其他触发条件，比如等待内存分配、等待延时到、等待信号量等，此时
//      若被时钟中断触发，则破坏了软件的结构。周期性中断同步可用多次调用单次同步
//      的方法完成，即每次触发后，线程完成了必要的工作以后重新再次调用单次同步
//      函数，这样，程序的每一步都有明确的目标，而不是无目的的休眠或挂起


