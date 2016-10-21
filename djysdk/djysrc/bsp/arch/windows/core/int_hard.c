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
// 这份授权条款?在使用者符合下列条件的情形下?授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利?无论此包装是否经改作皆然?
//
// 1. 对于本软件源代码的再散播?必须保留上述的版权宣告、本条件列表?以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播?必须连带以文件以及?或者其他附
//    于散播包装中的媒介方式?重制上述之版权宣告、本条件列表?以及下述
//    的免责声明。

// 免责声明?本软件是本软件版权持有人以及贡献者以现状?"as is"?提供?
// 本软件包装不负任何明示或默示之担保责任?包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者?无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权?包括过失或其他原因等?而起?对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害?包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等??
// 不负任何责任?即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------
//所属模块:中断模块
//作者?lst
//版本?V1.0.0
//文件描述: 中断模块与硬件相关的代码?包含异步信号与实时中断
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2010-01-04
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 从原int.c中把硬件无关代码剥离后而成
//------------------------------------------------------
#include "windows.h"
#include "cpu.h"
#include "stdint.h"
#include "stdlib.h"
#include "cpu_peri.h"
#include "int.h"
#include "int_hard.h"
#include "critical.h"
#include "djyos.h"

CRITICAL_SECTION  int_cs;
ufast_t ufg_int_number;
HANDLE win32_int_event;
HANDLE win32_int_engine;

extern struct IntLine *tg_pIntLineTable[];       //中断线查找表
extern struct IntMasterCtrl  tg_int_global;          //定义并初始化总中断控制结构
extern void __Djy_ScheduleAsynSignal(void);
void __Djy_EventReady(struct EventECB *event_ready);

//----进入高级原子操作---------------------------------------------------------
//功能?读当前总中断状态?然后禁止总中断。高级原子操作是指期间不容许任何原因
//      打断的操作。
//      Int_HighAtomStart--int_high_atom_end必须配套使用?
//      这两函数可视作"Int_SaveTrunk--Int_RestoreTrunk"的快速版本?在被他们套
//      住的代码块内?不允许调用"Int_SaveTrunk--Int_RestoreTrunk",更不允许
//      调用可能引起线程切换的函数?建议不要调用任何系统调用。
//参数?无
//返回?原子状态
//备注?移植敏感
//-----------------------------------------------------------------------------
atom_high_t Int_HighAtomStart(void)
{
    EnterCriticalSection(&int_cs);
    return true;
}

//----离开高级原子操作---------------------------------------------------------
//功能?恢复进入前总中断状态?本函数应与int_high_atom_start函数配套
//      Int_HighAtomStart--int_high_atom_end必须配套使用?
//      这两函数可视作"Int_SaveTrunk--Int_RestoreTrunk"的快速版本?在被他们套
//      住的代码块内?不允许调用"Int_SaveTrunk--Int_RestoreTrunk",更不允许
//      调用可能引起线程切换的函数?建议不要调用任何系统调用。
//参数?high?int_high_atom_start保存的原子状态
//返回?无
//备注?移植敏感
//-----------------------------------------------------------------------------
void Int_HighAtomEnd(atom_high_t high)
{
    LeaveCriticalSection(&int_cs);

}

//----进入低级原子操作---------------------------------------------------------
//功能?读取当前异步信号开关状态?然后禁止异步信号。低级级原子操作可用来防止临界
//      数据的并发访问?但可能会被实时中断打断。
//      Int_LowAtomStart---int_low_atom_end必须配套使用?这两函数可视作
//      "Int_SaveAsynSignal--Int_RestoreAsynSignal"的快速版本?在被他们套住
//      的代码块内?不允许调用int_restore_asyn_signal、int_save_asyn_signal中的
//      任何一个。更不允许调用可能引起线程切换的函数?建议不要调用任何系统调用。
//参数?无
//返回?原子状态
//备注?移植敏感
//-----------------------------------------------------------------------------
atom_low_t Int_LowAtomStart(void)
{
    EnterCriticalSection(&int_cs);
    return true;
}

//----离开低级原子操作---------------------------------------------------------
//功能?恢复进入前异步信号许可状态?本函数应与int_low_atom_start函数配套。
//      Int_LowAtomStart---int_low_atom_end必须配套使用?这两函数可视作
//      "Int_SaveAsynSignal--Int_RestoreAsynSignal"的快速版本?在被他们套住
//      的代码块内?不允许调用int_restore_asyn_signal、int_save_asyn_signal中的
//      任何一个。更不允许调用可能引起线程切换的函数?建议不要调用任何系统调用。
//参数?low?int_low_atom_start保存的原子状态
//返回?无
//备注?移植敏感
//-----------------------------------------------------------------------------
void Int_LowAtomEnd(atom_low_t low)
{
    LeaveCriticalSection(&int_cs);
}

//----接通异步信号开关---------------------------------------------------------
//功能?接通异步信号开关,如果总开关接通且中断线开关接通,该中断将被允许
//      1.当有独立的硬件开关时,把该开关接通即可
//      2.如果没有独立硬件开关,则接通所有被允许的异步信号的线开关.
//      3.2440属于第二种情况,把符合条件的中断线在rINTMSK相应位清0.
//参数?无
//返回?无
//备注?移植敏感
//-----------------------------------------------------------------------------
void Int_ContactAsynSignal(void)
{
    LeaveCriticalSection(&int_cs);
}

//----断开异步信号开关---------------------------------------------------------
//功能?断开异步信号开关,所有的异步信号将被禁止
//      1.当有独立的硬件开关时,把该开关断开即可
//      2.如果没有独立硬件开关,则断开所有异步信号的线开关.
//      3.2440属于第二种情况,把所有异步信号线在rINTMSK相应位置1.
//参数?无
//返回?无
//备注?移植敏感
//-----------------------------------------------------------------------------
void Int_CutAsynSignal(void)
{
    if(tg_int_global.en_asyn_signal_counter == 0)
    {
        EnterCriticalSection(&int_cs);
    }
}

//----接通总中断开关-----------------------------------------------------------
//功能?接通总中断开关,所有cpu都会有一个总开关,直接操作该开关即可.
//参数?无
//返回?无
//备注?移植敏感
//-----------------------------------------------------------------------------
void Int_ContactTrunk(void)
{

}

//----断开总中断开关---------------------------------------------------------
//功能?断开总中断开关,所有cpu都会有一个总开关,直接操作该开关即可.
//参数?无
//返回?无
//备注?移植敏感
//-----------------------------------------------------------------------------
void Int_CutTrunk(void)
{

}

//----接通单个中断线开关-------------------------------------------------------
//功能?接通单个中断线开关,该中断是否允许还要看后面的开关状态
//参数?无
//返回?无
//备注?移植敏感
//-----------------------------------------------------------------------------
bool_t Int_ContactLine(ufast_t ufl_line)
{
    return  true;
}

//----断开单个中断线开关-------------------------------------------------------
//功能?断开单个中断线开关?无论总中断和异步信号开关状态如何?该中断线被禁止
//参数?无
//返回?无
//备注?移植敏感
//-----------------------------------------------------------------------------
bool_t Int_CutLine(ufast_t ufl_line)
{

    return true;
}

//----应答中断?清除相应中断线的中断挂起状态-----------------------------------
//功能?硬件应该有相应的功能?提供清除中断挂起的操作?清除前?不能响应同一中断线
//      的后续中断?清除后?才可以响应后续中断。本函数与该中断线被设置为实时中断
//      还是异步信号无关
//参数?ufast ufl_line?指定应答的中断线号
//返回?无
//备注?有些体系中断响应时硬件应答?本函数为空函数。
//      本函数是移植敏感函数
//说明: windows版本不需要应答中断
//-----------------------------------------------------------------------------
bool_t Int_ClearLine(ufast_t ufl_line)
{

    return true;
}

//----激发中断-----------------------------------------------------------------
//功能: 触发一个中断.如果中断本已悬挂,本函数无影响.本函数与该中断线被设置为实时
//      中断还是异步信号无关
//参数?ufast ufl_line?欲触发的中断线号
//返回?如果相应的中断线硬件不提供用软件触发中断功能,返回 false,否则返回 true
//备注: 本函数实现依赖于硬件,有些硬件系统不支持此功能.
//      本函数是移植敏感函数
//-----------------------------------------------------------------------------
bool_t Int_TapLine(ufast_t ufl_line)
{
    EnterCriticalSection(&int_cs);
    if(tg_int_global.en_trunk_counter == 0) //实时中断使能才可以发出
    {
        //如果中断线是使能的
        if(tg_int_global.enable_bitmap[ufl_line/CN_CPU_BITS]
            & ((ucpu_t)1<<(ufl_line % CN_CPU_BITS) ))
        {
            //中断线属于实时中断,或者异步信号使能
            if( (tg_pIntLineTable[ufl_line]->int_type ==CN_REAL)
                        ||(tg_int_global.en_asyn_signal_counter == 0) )
            {
                ufg_int_number = ufl_line;
                SetEvent(win32_int_event);
            }
        }
    }
    LeaveCriticalSection(&int_cs);
    return true;
}

//----应答全部中断?清除全部中断线的中断挂起状态-------------------------------
//功能?硬件应该有相应的功能?提供清除中断挂起的操作
//参数?ufast ufl_line?指定应答的中断线号
//返回?无
//备注?有些体系中断响应时硬件应答?本函数为空函数。
//      本函数是移植敏感函数
//说明: windows版本不需要应答中断
//-----------------------------------------------------------------------------
void __Int_ClearAllLine(void)
{

}

//----查询中断线请求状态-------------------------------------------------------
//功能?查询并清除相应中断线状态?可用于查询式中断程序
//参数?ufl_line?欲查询的中断线
//返回?若中断挂起?返回true?否则返回false
//备注: 与硬件结构相关,有些结构可能不提供这个功能,慎用!
//      本函数是移植关键函数
//说明: windows版本不能查询中断线请求状态
//-----------------------------------------------------------------------------
bool_t Int_QueryLine(ufast_t ufl_line)
{
    return false;
}

//----把指定中断线设置为异步信号--------???---------------------------------
//功能?把指定中断线设置为异步信号,若中断正在响应,则当前中断返回后生效
//参数?ufast ufl_line?指定被设置的中断线号
//返回?true=成功?false=失败
//      本函数移植敏感
//说明: windows版本的中断只能是异步信号
//-----------------------------------------------------------------------------
bool_t Int_SettoAsynSignal(ufast_t ufl_line)
{
    if( (ufl_line > CN_INT_LINE_LAST)
            || (tg_pIntLineTable[ufl_line] == NULL) )
        return false;
    tg_pIntLineTable[ufl_line]->int_type = CN_ASYN_SIGNAL;   //中断线类型

    tg_int_global.property_bitmap[ufl_line/CN_CPU_BITS]
                    &= ~(1<<(ufl_line % CN_CPU_BITS));   //设置位图
    return true;
}

//----把指定中断线设置为实时中断--------???---------------------------------
//功能?把指定中断线设置为实时中断,若中断正在响应,则当前中断返回后生效
//参数?ufast ufl_line?指定被设置的中断线号
//返回?true=成功?false=失败
//      本函数移植敏感
//-----------------------------------------------------------------------------
bool_t Int_SettoReal(ufast_t ufl_line)
{
    if( (ufl_line > CN_INT_LINE_LAST)
            || (tg_pIntLineTable[ufl_line] == NULL) )
        return false;
    if(tg_pIntLineTable[ufl_line]->sync_event != NULL)
        return false;     //有线程在等待这个中断?不能设为实时中断
    tg_pIntLineTable[ufl_line]->int_type = CN_REAL;    //中断线类型
    tg_pIntLineTable[ufl_line]->enable_nest = false;   //本实现不支持实时中断嵌套
    tg_int_global.property_bitmap[ufl_line/CN_CPU_BITS]
            |= 1<<(ufl_line % CN_CPU_BITS);   //设置位图
    return true;
}

//----使能中断嵌套-------------------------------------------------------------
//功能: 使能一个中断源被抢占?使能后?相应的中断服务期间?可能会被别的中断抢占。
//      本函数不分实时中断还是异步信号?与具体的实现有关?移植者应该根据硬件特性
//      和应用需求?确定是否允许嵌套。
//      嵌套发生在实时中断之间或者异步信号之间。实时中断永远可以打断异步信号?这
//      不算嵌套。
//      对于三星系列的ARM芯片中断管理器的特点?对于实时中断的处理?有三种方式:
//      1、用fiq实现实时中断?INTOFFSET1和INTOFFSET2寄存器提供了当前正在服务的
//         中断号?但该两个寄存器在fiq中无效?如果实时中断用fiq实现?则需要逐位
//         检查被设为实时中断的中断号在INTPND1和INTPND1寄存器中的值?才能确定被
//         服务的中断号?这个过程可能非常漫长。这种情况能够实现实时中断嵌套?但
//         却完全失去了实时中断的设计初衷?djyos for s3c2416不采取这种方式。
//      2、有一个特殊情况是?系统只需要一个实时中断?其他全部设为异步信号?这种
//         情况?用fiq实现实时中断是最为理想的?当然只有一个中断?嵌套也就无从
//         谈起了。
//      3、实时中断和异步信号都是用irq方式实现?cpsr的I位用做trunk开关?异步信号
//         没有独立开关?是通过把所有的属于异步信号的中断号的开关全部关掉来模拟
//         异步信号开关的。这种情况是不能实现实时中断嵌套的。为什么呢?arm被设计
//         为?只要I位为0?新的中断就可以抢占正在服务的中断?也就意味着?异步信
//         号抢占实时中断?在硬件上是允许的。实时中断要实现嵌套?须先关掉异步信
//         号?再设置I=0?如果实时中断响应后?在异步信号被关掉之前?紧接着有异步
//         信号发生的话?该irq请求就会发出?随后关掉异步信号也没用?总中断一旦被
//         允许?该异步信号就会抢占实时中断。这种抢占?不仅把"辈分"搞乱?还会引
//         发严重问题?甚至使系统崩溃。如果抢占实时中断的异步信号?服务期间发生
//         了线程切换?把fiq服务的栈?当作pg_event_running的栈。因此?djyos for
//         2416版本不支持实时中断嵌套。
//参数: ufl_line?被操作的中断线
//返回: 无
//说明: windows版本不支持中断嵌套
//-----------------------------------------------------------------------------
bool_t Int_EnableNest(ufast_t ufl_line)
{

    return false;
}

//----禁止中断嵌套-------------------------------------------------------------
//功能: 清除某中断号的中断嵌套使能。
//参数: ufl_line?被操作的中断线
//返回: 无
//-----------------------------------------------------------------------------
void Int_DisableNest(ufast_t ufl_line)
{

}

//----设定嵌套优先级-----------------------------------------------------------
//功能: 设定指定中断线的嵌套优先级?本函数严重依赖硬件功能。如果硬件不支持?可
//      保持空函数。
//参数?ufast ufl_line?指定被设置的中断线号
//返回?无
//注: 本函数移植敏感
//说明: windows版本中断不分优先级
//-----------------------------------------------------------------------------
bool_t Int_SetPrio(ufast_t ufl_line,u32 prio)
{
    return false;
}
u32 WINAPI win32_switch( LPVOID lpParameter )
{
    while(1)
    {
        WaitForSingleObject(win32_int_event,INFINITE );
        __Int_EngineAll(ufg_int_number);
    }
}


//----初始化中断硬件相关部分---------------------------------------------------
//功能: 如标题
//参数: 无
//返回: 无
//注: 移植敏感?与硬件相关?也与软件策略有关
//-----------------------------------------------------------------------------
void __Int_InitHard(void)
{
    win32_int_event  = CreateEvent(NULL,FALSE,FALSE,NULL);
    win32_int_engine = CreateThread( NULL, 0, win32_switch, 0,
                                     0, NULL );

    SetPriorityClass(win32_int_engine,THREAD_PRIORITY_HIGHEST);
    SetThreadAffinityMask( win32_int_engine, 1 );
    SetThreadPriority(win32_int_engine,THREAD_PRIORITY_TIME_CRITICAL);
}
//----初始化中断---------------------------------------------------------------
//功能?初始化中断硬件,初始化中断线数据结构
//      2.异步信号保持禁止,它会在虚拟机启动引擎中打开.
//      3.总中断允许?
//      用户初始化过程应该遵守如下规则:
//      1.系统开始时就已经禁止所有异步信号,用户初始化时无须担心异步信号发生.
//      2.初始化过程中如果需要操作总中断/实时中断/异步信号,应该成对使用.禁止使
//        异步信号实际处于允许状态(即异步和总中断开关同时允许).
//      3.可以操作中断线,比如连接、允许、禁止等,但应该成对使用.
//      4.建议使用save/restore函数对,不要使用enable/disable函数对.
//参数?无
//返回?无
//-----------------------------------------------------------------------------
void Int_Init(void)
{
    ufast_t ufl_line;
    InitializeCriticalSection(&int_cs);

    __Int_InitHard();
    __Int_ClearAllLine();
    for(ufl_line=0;ufl_line <= CN_INT_LINE_LAST;ufl_line++)
    {
        tg_pIntLineTable[ufl_line] = (ufast_t)NULL;
    }

    for(ufl_line=0; ufl_line < CN_INT_BITS_WORDS; ufl_line++)
    {
        //属性位图清零,全部置为异步信号方式
        tg_int_global.property_bitmap[ufl_line] = 0;
        //中断使能位图清0,全部处于禁止状态
        tg_int_global.enable_bitmap[ufl_line] = 0;
    }
    tg_int_global.nest_asyn_signal = 0;
    tg_int_global.nest_real = 0;
//    tg_int_global.en_asyn_signal = false;
    Int_CutAsynSignal();
    tg_int_global.en_asyn_signal_counter = 1;   //异步信号计数
//    tg_int_global.en_trunk = true;
    tg_int_global.en_trunk_counter = 0;   //总中断计数
    Int_ContactTrunk();                //接通总中断开关
}

//----总中断引擎---------------------------------------------------------------
//功能?有些系统?在中断向量表部分难于区分实时中断还是异步信号的?或者不希望在汇
//      编阶段使用过于复杂的代码的?比如2416、2440等?则在汇编阶段获取中断号后?
//      直接调用本函数?由本函数再区别调用异步信号引擎或实时中断引擎。
//      像cortex-m3、omapl138这样?在中断向量表部分就可以区别实时中断还是异步信
//      号?则无须提供本函数?而是在汇编部分直接调用相应的引擎。
//参数?ufast ufl_line?响应的中断线号
//返回?无
//-----------------------------------------------------------------------------

void __Int_EngineAll(ufast_t ufl_line)
{
    if(tg_pIntLineTable[ufl_line]->int_type == CN_REAL)
        __Int_EngineReal(ufl_line);                //是实时中断
    else
        __Int_EngineAsynSignal(ufl_line);         //是异步信号
}

//----实时中断引擎-------------------------------------------------------------
//功能?响应实时中断?根据中断号调用用户ISR
//参数?ufast ufl_line?响应的中断线号
//返回?无
//-----------------------------------------------------------------------------
void __Int_EngineReal(ufast_t ufl_line)
{
    struct IntLine *ptIntLine;
    tg_int_global.nest_real++;
    ptIntLine = tg_pIntLineTable[ufl_line];

    //本if语句与移植敏感?跟cpu的中断管理器的几个特性有关:
    //1、异步信号是否有独立的开关?例如cortex-m3、omapl138等是有的?2440、2416、
    //   2410等是没有的。如果没有独立开关?则在打开总中断前须断掉异步信号线开关
    //2、异步信号和实时中断都处于开启状态的情况下?异步信号是否可能抢占实时中断。
    //   如果可以，是不能实现实时中断嵌套的。
    //3、实时中断响应后?是否自动关闭实时中断
    //4、该具体实现是否支持实时中断嵌套
    //5、本实现支持实时中断嵌套?故整个注释掉
//    if(tg_pIntLineTable[ufl_line]->enable_nest == true)
//    {
//        real_save_asyn = Int_LowAtomStart();
//        Int_ContactTrunk();
//    }
    Int_ClearLine(ufl_line);        //中断应答
    if(tg_pIntLineTable[ufl_line]->ISR != NULL)
        tg_pIntLineTable[ufl_line]->ISR(ptIntLine->para);  //调用用户中断函数


    //如果该实现支持实时中断嵌套?启用以下4句
    tg_int_global.nest_real--;

}

//----异步事件中断引擎---------------------------------------------------------
//功能?响应异步信号?根据中断号调用用户ISR?随后弹出中断线控制块的my_evtt_id
//      成员指定的事件类型?最后在返回前查看是否需要做上下文切换?如需要则切换
//      之。
//参数?ufast ufl_line?响应的中断线号
//返回?无
//-----------------------------------------------------------------------------
void __Int_EngineAsynSignal(ufast_t ufl_line)
{
    struct EventECB *event;
    struct IntLine *ptIntLine;
    u32 isr_result;

    g_bScheduleEnable = false;
    ptIntLine = tg_pIntLineTable[ufl_line];
    tg_int_global.nest_asyn_signal=1;
    if(ptIntLine->clear_type == CN_INT_CLEAR_AUTO)
        Int_ClearLine(ufl_line);        //中断应答,
    if(ptIntLine->ISR != NULL)
        isr_result = ptIntLine->ISR(ptIntLine->para);
    else
    {
        if(ptIntLine->clear_type == CN_INT_CLEAR_USER)
            Int_ClearLine(ufl_line);        //中断应答,
    }
//    if(ptIntLine->clear_type == CN_INT_CLEAR_POST)
//        Int_ClearLine(ufl_line);        //中断应答,
    event = ptIntLine->sync_event;
    if(event != NULL)   //看同步指针中有没有事件(注?单个事件?不是队列)
    {
        event->event_result = isr_result;
        __Djy_EventReady(event);   //把该事件放到ready队列
        ptIntLine->sync_event = NULL;   //解除同步
    }
    if(ptIntLine->my_evtt_id != CN_EVTT_ID_INVALID)
    {
        Djy_EventPop(ptIntLine->my_evtt_id,
                        NULL,0,(ptu32_t)isr_result, (ptu32_t)ufl_line,0);
    }
    tg_int_global.nest_asyn_signal = 0;

    if(g_ptEventReady != g_ptEventRunning)
        __Djy_ScheduleAsynSignal();       //执行中断内调度
    g_bScheduleEnable = true;
    return;
}



