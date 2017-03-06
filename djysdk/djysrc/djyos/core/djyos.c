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
//所属模块:核心模块
//作者:  罗侍田.
//版本：V1.1.0
//文件描述:事件类型、事件管理以及多事件调度相关的代码全在这里了。
//其他说明:
//修订历史:
//8. 日期:2013-2-7
//   作者:  罗侍田.
//   新版本号：1.3.3
//   修改说明: 修正了djy_event_exit函数的一处错误，该bug由裴乐提交
//7. 日期:2013-1-18
//   作者:  罗侍田.
//   新版本号：1.3.2
//   修改说明: 修正了djy_event_pop函数的一处错误，该bug由QQ群友
//              "ring wang(23424710)"提交
//6. 日期:2012-12-18
//   作者:  罗侍田.
//   新版本号：1.3.1
//   修改说明: 修正了djy_event_complete和djy_wait_event_completed函数的一处错误，
//      该bug由网友"王毛磊"提交
//5. 日期: 2011-07-19
//   作者:  罗侍田.
//   新版本号: V1.3.0
//   修改说明:解决一个可能导致内存泄漏的bug。该bug发作条件：参数尺寸大于32字节，
//   且在调用djy_event_pop时要求动态分配参数所需内存空间，djy_task_completed和
//   djy_event_exit函数释放参数时将不释放该动态分配的内存。
//   修改方法是在struct  para_record结构中增加成员dynamic_mem，用于表示参数是否
//   动态分配得到的，如是，则在djy_task_completed和djy_event_exit函数释放参数时
//   调用m_free释放之。
//4. 日期: 2009-12-15
//   作者:  罗侍田.
//   新版本号: V1.3.0
//   修改说明:
//   1、djy_evtt_regist中判断同名事件类型时，无论该事件类型控制块有效否，都调用
//      strcmp函数比较，效率较低，改为先判断是否空，然后才比较。
//   2、原来在djy_evtt_regist中因不能创建线程导致登记失败时，没有处理事件类型
//      控制块，导致仍然处于registered=1的状态。
//3. 日期: 2009-10-05
//   作者:  罗侍田.
//   新版本号: V1.2.0
//   修改说明:
//   1、不再以tg_int_global.nest_asyn_signal==0作为是否允许调度的依据，这会导致
//      中断模块和调度模块间隐含的耦合关系。
//      以全局变量bg_schedule_enable作为是否允许调度的标志
//   2、改掉了一些gcc（以及c99）才支持的特征，以适应更广泛的编译器
//   3、前面的版本并无实现djy_evtt_unregist的全部功能，本版本补全之。
//   4、__djy_resume_delay函数中这一行：
//      g_ptEventDelay = g_ptEventDelay->previous;改为
//      g_ptEventDelay = g_ptEventDelay->next;
//      否则会导致闹钟同步队列错乱
//   5、原版本中,__turnto_context函数身兼2职，内核初始化完成后，首次调用本函数还
//      有启动线程调度的功能，这在arm中是可以的，但不确保其他cpu也可以，例如cm3
//      上就不行，先把这个功能分离出来，增加一个函数__asm_start_thread。但在arm
//      版本中用宏定义：#define __asm_start_thread __asm_turnto_context
//   6、int_restore_asyn_signal函数中，先执行__int_contact_asyn_signal打开中断，
//      然后再执行：
//            if(g_ptEventRunning != g_ptEventReady)
//                __Djy_Schedule();
//      在打开中断与__djy_schedule函数重新关闭中断之间，如果有中断抢占的话，可能
//      导致严重错误，int_restore_trunk函数也有同样的问题。
//      新版本中，__djy_schedule函数调用__asm_switch_context时才打开中断，杜绝了
//      上述问题，__djy_schedule函数整个都是在关闭中断的条件下执行的。
//2. 日期: 2009-03-18
//   作者:  罗侍田.
//   新版本号: V1.1.0
//   修改说明:
//   1、根据无为小僧（曾波）发现的隐患，事件类型的名字如果用指针索引的话，如果
//      用户调用y_evtt_regist时使用的是局部数组，将存在保存名字的内存被释放的危
//      险。改为数组存储，最长31字符，在struct event_type结构中把evtt_name由指针
//      改为数组，修改涉及下列函数：
//          __djy_init_sys、y_evtt_regist、y_evtt_unregist、y_get_evtt_id
//   2、在y_evtt_unregist函数中释放线程处发现一个bug，存在访问非法内存的危险，
//      改之。
//   3、y_evtt_done_sync函数中处理同步队列处存在严重bug，可能导致访问非法内存，
//      且使同步队列混乱，改之。
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "board-config.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "cpu.h"
#include "object.h"
#include "lock.h"
#include "int.h"
#include "pool.h"
#include "systime.h"
#include "djyos.h"
#include "exp.h"
#include "core_config.h"

ptu32_t __Djy_Service(void);

struct ProcessVm *  g_ptMyProcess;
//为cn_events_limit条事件控制块分配内存
struct EventECB *g_ptECB_Table;
//static struct ParaPCB  s_tEventParaTable[CN_CFG_PARAS_LIMIT];
//为cn_evtts_limit个事件类型控制块分配内存
struct EventType *g_ptEvttTable;
struct EventECB  *s_ptEventFree; //空闲链表头,不排序
//static struct ParaPCB  *s_ptParaFree; //空闲链表头,不排序
//轮转调度时间片，0表示禁止轮转调度，默认1，RRS = "round robin scheduling"缩写。
static u32  s_u32RRS_Slice = 1;

struct EventECB  *g_ptEventReady;      //就绪队列头
struct EventECB  *g_ptEventRunning;    //当前正在执行的事件
struct EventECB  *g_ptEventDelay;      //闹钟同步队列表头
s64 g_s64RunningStartTime;        //当前运行中事件的开始执行时间.
s64  g_s64OsTicks;            //操作系统运行ticks
bool_t g_bScheduleEnable;     //系统当前运行状态是否允许调
bool_t g_bMultiEventStarted = false;    //多事件(线程)调度是否已经开始
extern u32 g_u32CycleSpeed; //for(i=j;i>0;i--);每循环纳秒数*1.024

u32 (*g_fnEntryLowPower)(struct ThreadVm *vm) = NULL;  //进入低功耗状态的函数指针。
void __Djy_SelectEventToRun(void);
void __Djy_EventReady(struct EventECB *event_ready);
void __Djy_ResumeDelay(struct EventECB *delay_event);
extern void __DjyInitTick(void);
extern void __M_WaitMemory(struct EventECB *event);
extern void __M_CleanUp(uint16_t event_id);
extern struct ThreadVm *__CreateThread(struct EventType *evtt,u32 *stack_size);
extern void __asm_reset_switch(ptu32_t (*thread_routine)(void),
                               struct ThreadVm *new_vm,struct ThreadVm *old_vm);
extern void __asm_turnto_context(struct ThreadVm  *new_vm);
extern void __asm_start_thread(struct ThreadVm  *new_vm);
extern void __asm_switch_context_int(struct ThreadVm *new_vm,struct ThreadVm *old_vm);
extern void __asm_switch_context(struct ThreadVm *new_vm,struct ThreadVm *old_vm);
extern void __Int_ResetAsynSignal(void);


//----微秒级延时-------------------------------------------------------------
//功能：利用循环实现的微秒分辨率延时，调用__DjySetDelay函数后才能使用本函数，
//      否则在不同优化级别和不同编译器下,延时数不同.
//参数：time，延时时间，单位为微秒
//返回：无
//注意：不建议使用此函数做太长延时，长延时请使用函数 Djy_EventDelay,
//-----------------------------------------------------------------------------
void Djy_DelayUs(u32 time)
{
    volatile u32 i;

    i = (time << 10) / g_u32CycleSpeed;
    for(; i >0 ; i--);
}

//----线程栈检查---------------------------------------------------------------
//功能: 检查一个事件的线程是否有栈溢出风险，方法:检测栈最低1/16空间内，内容是否
//      改变，若改变，则可能发生了栈溢出。
//参数: event_id,被检查的事件id
//返回: true=no overflow,false = overflow
//-----------------------------------------------------------------------------
bool_t __Djy_CheckStack(s16 event_id)
{
    struct ExpThrowPara  parahead;
    char StackExp[128];
    u32 loop;
    volatile ptu32_t level;
    u32 *stack;
    u32 pads;
    bool_t result=true;

    pads = 0x64646464;
    if(g_ptECB_Table[event_id].vm == NULL)
        return true;
    if(g_ptECB_Table[event_id].previous ==
                    (struct EventECB*)&s_ptEventFree)
        return true;

    level = g_ptECB_Table[event_id].vm->stack_size>>6;

    if((ptu32_t)(g_ptECB_Table[event_id].vm->stack - (u32*)(g_ptECB_Table[event_id].vm))
                                                            < level)
    {
        result = false;   //栈指针已经低于安全值，有溢出风险
    }
    else
    {
        stack = (u32*)(&(g_ptECB_Table[event_id].vm[1]));
        for(loop = 0; loop < level; loop++)
        {
            if(stack[loop] != pads)
            {
                result = false;   //安全区内发生了改变，有溢出风险
                break;
            }
        }
    }
    if(result == false)
    {
        parahead.DecoderName = NULL;
        parahead.ExpAction = EN_EXP_DEAL_RESTART;
        sprintf(StackExp,"栈溢出风险,栈底:0x%x,栈尺寸:0x%x,剩余:0x%x,事件号:%d,类型号%d,类型名:%s",
                            (ptu32_t)stack,g_ptECB_Table[event_id].vm->stack_size,loop,
                            event_id, g_ptECB_Table[event_id].evtt_id&(~CN_EVTT_ID_MASK),
                            g_ptEvttTable[g_ptECB_Table[event_id].evtt_id&(~CN_EVTT_ID_MASK)].evtt_name);
        parahead.ExpInfo = (u8*)StackExp;
        parahead.ExpInfoLen = sizeof(StackExp);
        parahead.ExpType = CN_EXP_TYPE_STACK_OVER;
        printf("%s\n\r",StackExp);
        Exp_Throw(&parahead);
    }
    return result;
}

//以下为多任务管理函数

//----tick中断-----------------------------------------------------------------
//功能：为操作系统产生时钟源，并比较闹钟同步队列中的事件到期时间，如果有闹铃时间到的
//      事件，就把他们激活到事件就绪队列中。如果时间片轮转调度被允许，还要看是否要
//      执行轮转。ticks被设置为异步信号。
//参数：inc_ticks，ticks中断的中断线号，实际上不用。
//返回：无
//-----------------------------------------------------------------------------
void __DjyMaintainSysTime(void);
void  Djy_IsrTick(u32 inc_ticks)
{
    struct EventECB *pl_ecb,*pl_ecbp,*pl_ecbn;
    g_s64OsTicks += (s64)inc_ticks;    //系统时钟,默认永不溢出
    //用于维护系统时钟运转，使读系统时间的间隔，小于硬件定时器循环周期。
    __DjyMaintainSysTime( );
    if(g_ptEventDelay != NULL)
    {
        pl_ecb = g_ptEventDelay;
        while(1)
        {
            if(pl_ecb->delay_end_tick <= g_s64OsTicks) //默认64位ticks不会溢出
            {
                //事件在某同步队列中，应该从该队列取出
                if(pl_ecb->sync_head != NULL)
                {
                    if(*(pl_ecb->sync_head) == pl_ecb)//本事件位于队列头部
                    {
                        if(pl_ecb->multi_next == pl_ecb)//该队列中只有一个事件
                        {
                            *(pl_ecb->sync_head) = NULL;

                            pl_ecb->multi_next = NULL;
                            pl_ecb->multi_previous = NULL;
                        }else                   //该同步队列中有多个事件
                        {
                            //头指针指向下一个事件
                            *pl_ecb->sync_head = pl_ecb->multi_next;
                            pl_ecb->multi_previous->multi_next
                                                = pl_ecb->multi_next;
                            pl_ecb->multi_next->multi_previous
                                                = pl_ecb->multi_previous;
                        }
                    }else           //本事件不是队列头
                    {
                        pl_ecb->multi_previous->multi_next
                                            = pl_ecb->multi_next;
                        pl_ecb->multi_next->multi_previous
                                            = pl_ecb->multi_previous;
                    }
                    pl_ecb->sync_head = NULL;   //事件头指针置空
                }

                pl_ecb->wakeup_from = pl_ecb->event_status;
                pl_ecb->event_status = CN_STS_EVENT_READY;
                if(pl_ecb->next == pl_ecb)      //这是闹钟同步队列最后一个结点.
                {
                    g_ptEventDelay = NULL;
                    __Djy_EventReady(pl_ecb);
                    break;
                }else
                {
                    g_ptEventDelay = pl_ecb->next;
                    pl_ecb->next->previous = pl_ecb->previous;
                    pl_ecb->previous->next = pl_ecb->next;
                    __Djy_EventReady(pl_ecb);
                    pl_ecb = g_ptEventDelay;
                }
            }else
                break;
        }
    }

    //下面处理时间片轮转调度.
    //因在开异步信号(允许调度)才可能进入__djy_isr_tick，即使因闹钟响导致新事件加
    //入，pg_event_running也必定在优先级单调队列中，但可能不等于pg_event_ready
    if(s_u32RRS_Slice != 0)      //允许轮转调度
    {
        if( (g_ptEventRunning->prio == g_ptEventRunning->next->prio)
                    &&(g_ptEventRunning != g_ptEventRunning->next) )
        {//该优先级有多个事件，看轮转时间是否到
            if((u32)g_s64OsTicks % s_u32RRS_Slice == 0) //时间片用完
            {
                //先处理优先级单调队列，把pg_event_running从队列中取出，代之以
                //g_ptEventRunning->next。
                pl_ecb = g_ptEventRunning->next;
                pl_ecbn = g_ptEventRunning->multi_next;
                pl_ecbp = g_ptEventRunning->multi_previous;
                pl_ecbp->multi_next = pl_ecb;
                pl_ecb->multi_previous = pl_ecbp;
                pl_ecbn->multi_previous = pl_ecb;
                pl_ecb->multi_next = pl_ecbn;
                g_ptEventRunning->multi_next = NULL;
                g_ptEventRunning->multi_previous = NULL;

                //再处理就绪链表,把pg_event_running从就绪队列中拿出来，然后把它
                //放到同优先级的最后。

                //如果没有高优先级事件因同步时间到而加入ready队列，条件将成立
                if(g_ptEventReady == g_ptEventRunning)
                    g_ptEventReady = pl_ecb;
//                pl_ecbn = g_ptEventReady->multi_next;
//                pl_ecbp = g_ptEventRunning->previous;
                //这两行把pg_event_running从队列中取出
                g_ptEventRunning->previous->next = pl_ecb;
                pl_ecb->previous = g_ptEventRunning->previous;
                //这两行把pg_event_running插入目标位置
                g_ptEventRunning->previous = pl_ecbn->previous;
                g_ptEventRunning->next = pl_ecbn;
                pl_ecbn->previous->next = g_ptEventRunning;
                pl_ecbn->previous = g_ptEventRunning;
            }
        }
    }
    return;
}

void Djy_SaveLastError(u32 ErrorCode)
{
    g_ptEventRunning->error_no = ErrorCode;
}

//----提取最后一条错误信息-----------------------------------------------------
//功能：如名
//参数：无
//返回：错误号
//还需添加文本串和时间的功能，时间使用ticks数---db
//-----------------------------------------------------------------------------
u32 Djy_GetLastError(void)
{
    return g_ptEventRunning->error_no;
}

u32 *DjyCurrentErrnoAddr(void)
{
	static u32 err;
	if(NULL == g_ptEventRunning)
	{
		return &err;
	}
	else
	{
		return &g_ptEventRunning->error_no;
	}
}


//----从ready队列中取出一个事件------------------------------------------------
//功能: 把一个事件从ready队列中取出，并使ready队列重新连接
//参数: event，待取出的事件指针
//返回: 无
//注: 调用者请保证在异步信号(调度)被禁止的情况下调用本函数
//-----------------------------------------------------------------------------
void __Djy_CutReadyEvent(struct EventECB *event)
{
    struct EventECB *pl_ecb;
    if(event != g_ptEventReady)         //event不是ready队列头
    {
        if(event->multi_next == NULL)   //不在优先级单调队列中
        {
            event->next->previous = event->previous;
            event->previous->next = event->next;
        }else                           //在优先级单调队列中
        {
            pl_ecb = event->next;
            event->next->previous = event->previous;
            event->previous->next = event->next;
            if(pl_ecb->prio == event->prio)     //相应优先级不止一个事件
            {
                event->multi_previous->multi_next = pl_ecb;
                pl_ecb->multi_previous = event->multi_previous;
                event->multi_next->multi_previous = pl_ecb;
                pl_ecb->multi_next = event->multi_next;
            }else                               //相应优先级只有一个事件
            {
                //pl_ecb即event->multi_next.
                pl_ecb->multi_previous = event->multi_previous;
                event->multi_previous->multi_next = pl_ecb;
            }
        }
    }else                               //event是ready队列头
    {
        g_ptEventReady = event->next;
        pl_ecb = event->next;
        event->next->previous = event->previous;
        event->previous->next = event->next;
        if(pl_ecb->prio == event->prio)     //相应优先级不止一个事件
        {
            event->multi_previous->multi_next = pl_ecb;
            pl_ecb->multi_previous = event->multi_previous;
            event->multi_next->multi_previous = pl_ecb;
            pl_ecb->multi_next = event->multi_next;
        }else                               //相应优先级只有一个事件
        {
            //pl_ecb即event->multi_next.
            pl_ecb->multi_previous = event->multi_previous;
            event->multi_previous->multi_next = pl_ecb;
        }
    }
}

//----设置轮转调度时间片-------------------------------------------------------
//功能: djyos总所有参与轮转调度的事件时间片都是相同的，在这里设置。如果设为0，
//      表示不允许时间片轮转。
//参数: slices，新的轮转调度时间片，微秒数，将被向上取整为整数个ticks时间
//返回: 无
//-----------------------------------------------------------------------------
void Djy_SetRRS_Slice(u32 slices)
{
#if (32 > CN_CPU_BITS)
    atom_low_t atom_low;
    //若处理器字长＜32位,需要多个周期才能更新u32g_RRS_slice,该过程不能被时钟中断打断.
    atom_low = Int_LowAtomStart( );   //本函数对不能嵌套调用
#endif

    s_u32RRS_Slice =(slices + CN_CFG_TICK_US -1)/CN_CFG_TICK_US;

#if (32 > CN_CPU_BITS)
    //若处理器字长＜32位,需要多个周期才能更新u32g_RRS_slice,该过程不能被时钟中断打断.
    Int_LowAtomEnd( atom_low );
#endif
}

//----查询轮转调度时间片-------------------------------------------------------
//功能: djyos总所有参与轮转调度的事件时间片都是相同的，可用本函数查询。
//参数: 无
//返回: 当前时间片长度，微秒数。
//-----------------------------------------------------------------------------
u32 Djy_GetRRS_Slice(void)
{
    u32 temp;
#if (32 > CN_CPU_BITS)
    atom_low_t atom_low;
    //处理器字长＜32位,需要多个周期才能读取u32g_RRS_slice,该过程不能被时钟中断打断.
    atom_low = Int_LowAtomStart( );   //本函数对不能嵌套调用
#endif

    temp = s_u32RRS_Slice;

#if (32 > CN_CPU_BITS)
    //处理器字长＜32位,需要多个周期才能读取u32g_RRS_slice,该过程不能被时钟中断打断.
    Int_LowAtomEnd( atom_low );
#endif
    return temp * CN_CFG_TICK_US;
}

//----查找可运行线程----------------------------------------------------------
//功能: 在 pg_event_ready队列中获得一个可以运行的线程.
//      1.从pg_event_ready队列头开始查找,如果该事件还没有连接线程,则试图创建之.
//      2.如果不能建立,肯定是因内存不足，则把该事件放到内存等待表中,继续往下查找.
//      3.如此反复,直到找到一个可以运行的线程.然后把pg_event_ready指针指向该事件
//参数: 无
//返回: 无
//备注: 本函数由操作系统调用,调用前保证关异步信号。
//      由于系统服务事件总是ready,所以总是能找到可以运行的线程.
//----------------------------------------------------------------------------
void __Djy_SelectEventToRun(void)
{
    struct EventECB *pl_ecb,*temp_var;
   // struct EventType *pl_evtt;  //被操作的事件的类型指针
    struct EventType *pl_evtt;
    while(g_ptEventReady->vm == NULL)
    {
        pl_evtt =& g_ptEvttTable[g_ptEventReady->evtt_id &(~CN_EVTT_ID_MASK)];
        if(pl_evtt->my_free_vm != NULL)     //该事件类型有空闲的线程,直接使用
        {
            g_ptEventReady->vm = pl_evtt->my_free_vm;
            pl_evtt->my_free_vm = pl_evtt->my_free_vm->next;
        }else       //该事件类型没有空闲的线程,试图创建之
        {
            //创建线程
            g_ptEventReady->vm =
                    __CreateThread(pl_evtt,&(g_ptEventReady->wait_mem_size));
            if(g_ptEventReady->vm == NULL)                  //创建线程失败
            {
                pl_ecb = g_ptEventReady;
                __Djy_CutReadyEvent(pl_ecb);
#if (CN_CFG_DYNAMIC_MEM == 1)
                __M_WaitMemory(pl_ecb);//用的仅仅是多功能指针
#endif
                //将ECB放进EVTT的markedevent队列中，注意只是独立事件才会
                //此if语句是新增加内容
                if(pl_evtt->property.correlative == EN_INDEPENDENCE)
                {
                    if(NULL == pl_evtt->mark_event)
                    {
                        pl_ecb->previous = pl_ecb;
                        pl_ecb->next = pl_ecb;
                        pl_evtt->mark_event = pl_ecb;
                    }
                    else//按照优先级排队，第一个是优先级最高的
                    {
                        temp_var = pl_evtt->mark_event;
                        do
                        {
                            if(temp_var->prio > pl_ecb->prio)
                            {
                                break;
                            }
                            else
                            {
                                temp_var = temp_var->next;
                            }
                           }while(temp_var != pl_evtt->mark_event);

                        temp_var->previous->next = pl_ecb;
                        pl_ecb->previous = temp_var->previous;
                        temp_var->previous = pl_ecb;
                        pl_ecb->next = temp_var;
                        if(pl_ecb->prio > pl_ecb->previous->prio)//插入的是最高的优先级
                        {
                            pl_evtt->mark_event = pl_ecb->previous;
                        }
                    }
                }
            }else                                           //成功创建线程
            {
                pl_evtt->vpus++;        //独立事件vpus表示线程数
                                        //关联事件的vpus无意义
            }
        }
    }
}

//----创建进程-----------------------------------------------------------
//功能: 创建进程。
//参数: 无
//返回: 无
//备注: 这只是占一个位而已，在mp模式才有实用价值
//-----------------------------------------------------------------------------
void Djy_CreateProcessVm(void)
{
    static struct ProcessVm my_process;
    g_ptMyProcess = &my_process;
}


//----事件调度-----------------------------------------------------------------
//功能：剥夺running事件的cpu,把cpu交给ready队列的第一个事件.如果ready队列的第一
//      个事件尚未拥有线程,则建立之.建立线程时如果内存不足,则把这个事件放到
//      内存等待链表中,ready指针指向ready队列的下一个事件,依次推之,直到找到一个
//      线程指针非空或者成功创建线程的事件.
//参数：无
//返回：true = 成功切换，false = 未切换直接返回
//备注：1.本函数由操作系统调用
//      2.djyos系统认为,用户禁止中断就是为了能够连续执行,或者保护临界资源.
//      djyos中异步信号相当于高优先级线程,所以全局和异步信号禁止状态
//      下,是不允许事件调度的.
//      3.实时中断是否禁止,与调度无关.
//      4.由于最低优先级的系统服务事件总是ready,因此本函数总是能够找到目标事件
//-----------------------------------------------------------------------------
bool_t __Djy_Schedule(void)
{
    struct EventECB *event;
    u32 time;

//    if(!Djy_QuerySch())
//        return false;
//    Int_CutAsynSignal();
    __Djy_SelectEventToRun();
    if(g_ptEventReady != g_ptEventRunning)
    {//当running事件仍在ready队列中,且内存不足以建立新线程时,可能会出现优先
     //级高于running的事件全部进入内存等待队列的可能.此时执行else子句.
        event = g_ptEventRunning;
//#if(CN_CFG_DEBUG_INFO == 1)
        time = (u32)DjyGetSysTime();
        event->consumed_time += time - g_s64RunningStartTime;
        g_s64RunningStartTime = time;
//#endif
        g_ptEvttTable[event->evtt_id & (~CN_EVTT_ID_MASK)].SchHook(EN_SWITCH_OUT);

        g_ptEventRunning=g_ptEventReady;
        g_ptEvttTable[g_ptEventRunning->evtt_id & (~CN_EVTT_ID_MASK)].SchHook(EN_SWITCH_IN);
        Int_HalfEnableAsynSignal( );
        __asm_switch_context(g_ptEventReady->vm ,event->vm);
    }else
    {//优先级高于running的事件全部进入内存等待队列,下一个要处理的事件就是
     //running事件,无须执行任何操作
        return false;
    }
    return true;
}

//----中断内执行的事件调度------------------------------------------------------
//功能：剥夺running事件的cpu,把cpu交给ready队列的第一个事件.如果ready队列的第一
//      个事件尚不拥有线程,则创建之.建立线程时如果内存不足,则把这个事件放到
//      内存等待链表中,ready指针指向ready队列的下一个事件,依次推之,直到找到一个
//      线程指针非空或者成功创建线程的事件.
//参数：无
//返回：无
//备注：本函数由操作系统在异步信号引擎返回前调用
//      由于最低优先级的y_idle_service事件总是ready,因此总是能够找到调度对象的.
//-----------------------------------------------------------------------------
void __Djy_ScheduleAsynSignal(void)
{
    struct EventECB *event;
    u32 time;

    __Djy_SelectEventToRun();
    if(g_ptEventReady != g_ptEventRunning)
    {//当running事件仍在ready队列中,且内存不足以建立新线程时,可能会出现优先
     //级高于running的事件全部进入内存等待队列的可能.此时执行else子句.
         event=g_ptEventRunning;
//#if(CN_CFG_DEBUG_INFO == 1)
         time = (u32)DjyGetSysTime();
         event->consumed_time += time - g_s64RunningStartTime;
         g_s64RunningStartTime = time;
//#endif
         g_ptEvttTable[event->evtt_id & (~CN_EVTT_ID_MASK)].SchHook(EN_SWITCH_OUT);

         g_ptEventRunning=g_ptEventReady;
         g_ptEvttTable[g_ptEventRunning->evtt_id & (~CN_EVTT_ID_MASK)].SchHook(EN_SWITCH_IN);
         __asm_switch_context_int(g_ptEventReady->vm,event->vm);
    }else
    {//优先级高于running的事件全部进入内存等待队列,下一个要处理的事件就是
     //running事件,无须执行任何操作
    }
    return;
}

//----登记事件类型------------------------------------------------------------
//功能：登记一个事件类型到系统中,事件类型经过登记后,就可以pop了,否则,系统会
//      拒绝pop该类型事件
//参数：relation，取值为EN_INDEPENDENCE或EN_CORRELATIVE
//      default_prio，本事件类型的默认优先级。
//      vpus_res，系统繁忙时为本类型事件保留的线程数量，
//      vpus_limit，本类型事件所能拥有的线程数量的最大值
//      para_limit, 参数队列长度限制，以保护参数池不被撑破
//      thread_routine，线程入口函数(事件处理函数)
//      Stack，用户为处理事件的线程分配的栈，NULL表示从heap中分配栈。如果事件是
//              EN_INDEPENDENCE类型，且有多条事件并行处理导致需要为该类型创建多
//              个线程时，从第二个线程开始，就一定是从heap中申请Stack。
//      stack_size，执行thread_routine需要的栈尺寸，不包括thread_routine函数可能
//          调用的系统服务。
//      evtt_name，事件类型名，不同模块之间要交叉弹出事件的话，用事件类型名。
//          本参数允许是NULL，但只要非NULL，就不允许与事件类型表中已有的名字重名
//          名字不超过31个单字节字符
//返回：新事件类型的类型号
//------------------------------------------------------------------------------
u16 Djy_EvttRegist(enum enEventRelation relation,
                       ufast_t default_prio,
                       u16 vpus_res,
                       u16 vpus_limit,
//                       u16 para_limit,
                       ptu32_t (*thread_routine)(void),
                       void *Stack,
                       u32 StackSize,
                       char *evtt_name)
{
    struct ExpThrowPara  parahead;
    char ExpStr[64] = "事件类型控制块耗尽: ";
    u16 i,evtt_offset;
    u32 temp;
    if((default_prio >= CN_PRIO_SYS_SERVICE) || (default_prio == 0))
    {
        Djy_SaveLastError(EN_KNL_INVALID_PRIO);
        printf("事件类型优先级非法\n\r");
        return CN_EVTT_ID_INVALID;
    }
    Int_SaveAsynSignal();      //禁止调度也就是禁止异步事件
    //查找空闲的事件控制块
    for(evtt_offset=0; evtt_offset<gc_u32CfgEvttLimit; evtt_offset++)
        if( g_ptEvttTable[evtt_offset].property.registered ==0)
            break;
    if(evtt_offset == gc_u32CfgEvttLimit)     //没有空闲事件控制块
    {
        parahead.DecoderName = NULL;
        parahead.ExpAction = EN_EXP_DEAL_RECORD;
        strcat(ExpStr,evtt_name);
        parahead.ExpInfo = (u8*)ExpStr;
        parahead.ExpInfoLen = sizeof(ExpStr);
        parahead.ExpType = CN_EXP_TYPE_ETCB_EXHAUSTED;
        Exp_Throw(&parahead);
        Djy_SaveLastError(EN_KNL_ETCB_EXHAUSTED);
        printf("没有空闲事件控制块: %s\n\r",evtt_name);
        Int_RestoreAsynSignal();
        return CN_EVTT_ID_INVALID;
    }else if(evtt_name != NULL) //新类型有名字，需检查有没有重名
    {
        for(i=0; i<gc_u32CfgEvttLimit; i++)
        {
            if(g_ptEvttTable[i].property.registered == 1)
            {
                if(strcmp(g_ptEvttTable[i].evtt_name,evtt_name) == 0)
                {
                    Djy_SaveLastError(EN_KNL_EVTT_HOMONYMY);
                    printf("事件类型重名: %s\n\r",evtt_name);
                    Int_RestoreAsynSignal();
                    return CN_EVTT_ID_INVALID;
                }
            }
        }
        if(strnlen(evtt_name,32) <= 31)
            strcpy(g_ptEvttTable[evtt_offset].evtt_name,evtt_name);
        else
        {
            memcpy(g_ptEvttTable[evtt_offset].evtt_name,evtt_name,31);
            g_ptEvttTable[evtt_offset].evtt_name[31] = '\0';
        }
    }else
    {
        g_ptEvttTable[evtt_offset].evtt_name[0] = '\0';   //清空事件类型名
    }

    g_ptEvttTable[evtt_offset].default_prio = default_prio;
    g_ptEvttTable[evtt_offset].events = 0;
    g_ptEvttTable[evtt_offset].vpus_limit = vpus_limit;
    if(relation == EN_INDEPENDENCE)
    {
        //高优先级事件类型至少保留一个线程
        if( (vpus_res == 0) && ((default_prio < 0x80)
                              || (CN_CFG_DYNAMIC_MEM == 0) ) )
            g_ptEvttTable[evtt_offset].vpus_res = 1;
        else
            g_ptEvttTable[evtt_offset].vpus_res = vpus_res;
    }else
    {
        g_ptEvttTable[evtt_offset].vpus_res = 0;    //关联型事件，vpus_res无效
    }
    g_ptEvttTable[evtt_offset].SchHook = (SchHookFunc)NULL_func;
    g_ptEvttTable[evtt_offset].thread_routine = thread_routine;
    g_ptEvttTable[evtt_offset].stack_size = StackSize;
    g_ptEvttTable[evtt_offset].mark_event = NULL;
    g_ptEvttTable[evtt_offset].done_sync = NULL;
    g_ptEvttTable[evtt_offset].pop_sync = NULL;
    g_ptEvttTable[evtt_offset].vpus = 0;
//    g_ptEvttTable[evtt_offset].para_limit = para_limit;
    if(Stack == NULL)
    {
        if((default_prio<0x80) ||(CN_CFG_DYNAMIC_MEM == 0))
        {//运行模式为si或dlsp，或该事件类型拥有紧急优先级，需预先创建一个线程
            g_ptEvttTable[evtt_offset].my_free_vm =
                                __CreateThread(&g_ptEvttTable[evtt_offset],&temp);
            if(g_ptEvttTable[evtt_offset].my_free_vm == NULL)
            {//内存不足，不能创建线程
                parahead.DecoderName = NULL;
                parahead.ExpAction = EN_EXP_DEAL_RECORD;
                ExpStr[0] = '\0';
                strcat(ExpStr,"登记事件类型时内存不足: ");
                strcat(ExpStr,evtt_name);
                parahead.ExpInfo = (u8*)ExpStr;
                parahead.ExpInfoLen = sizeof(ExpStr);
                parahead.ExpType = CN_EXP_TYPE_MEM_EVTT;
                Exp_Throw(&parahead);
                Djy_SaveLastError(EN_MEM_TRIED);
                printf("%s\n\r",evtt_name);
                Int_RestoreAsynSignal();
                return CN_EVTT_ID_INVALID;
            }else
            {
                g_ptEvttTable[evtt_offset].vpus = 1;
            }
        }else
        {
            g_ptEvttTable[evtt_offset].my_free_vm = NULL;
        }
    }
    else
    {
        g_ptEvttTable[evtt_offset].my_free_vm =
               __CreateStaticThread(&g_ptEvttTable[evtt_offset],Stack,StackSize);
    }
    g_ptEvttTable[evtt_offset].property.correlative = relation;
    g_ptEvttTable[evtt_offset].property.registered = 1;
    g_ptEvttTable[evtt_offset].property.inuse = 0;
    g_ptEvttTable[evtt_offset].property.deleting = 0;
    g_ptEvttTable[evtt_offset].pop_times = 0;
    Int_RestoreAsynSignal();
    return evtt_offset | CN_EVTT_ID_MASK;
}


//----注册调度hook函数---------------------------------------------------------
//功能：注册调度hook函数, 该函数在事件上下文切入和切离时调用。
//参数：EvttID, 事件类型ID
//      HookFunc, hook函数指针
//返回：true = 成功注册, false = 失败。
//----------------------------------------------------------------------------
bool_t Djy_RegisterHook(u16 EvttID,SchHookFunc HookFunc)
{
    if(EvttID-CN_EVTT_ID_BASE  >= gc_u32CfgEvttLimit)
    {
        return false;
    }
    else
    {
        //无须判断相应EvttID是否被使用
        g_ptEvttTable[EvttID-CN_EVTT_ID_BASE].SchHook = HookFunc;
    }
    return true;
}

//----取事件类型id-------------------------------------------------------------
//功能：根据事件类型的名字，返回该事件类型的id号，不查找没名字的事件类型。
//参数：evtt_name，事件类型名
//返回：事件类型id号，如果没有找到则返回cn_invalid_id。
//备注：只能找到有名字的事件类型，没名字的事件类型又叫隐身事件类型。
//----------------------------------------------------------------------------
u16 Djy_GetEvttId(char *evtt_name)
{
    u16 loop;
    if(evtt_name == NULL)
        return CN_EVTT_ID_INVALID;
    for(loop = 0; loop < gc_u32CfgEvttLimit; loop++)
    {
        if((strcmp(g_ptEvttTable[loop].evtt_name,evtt_name) ==0)
                &&(g_ptEvttTable[loop].property.registered))
            return loop | CN_EVTT_ID_MASK;
    }
    return CN_EVTT_ID_INVALID;
}

//----删除事件类型-------------------------------------------------------------
//功能: 删除一个事件类型,如果队列中还有该类型事件(inuse == 1),只标记该类型为被
//      删除,真正的删除工作是在 done函数里完成的.如果队列中已经没有该类型事件了,
//      将会执行真正的删除操作.无论哪种情况,系统均会拒绝弹出该类型的新事件.
//参数: evtt_id,待删除的事件类型号
//返回: true = 成功，包括成功标记；false = 失败
//-----------------------------------------------------------------------------
bool_t Djy_EvttUnregist(u16 evtt_id)
{
    struct ThreadVm *next_vm,*temp;
    struct EventType *pl_evtt;
    struct EventECB *pl_ecb,*pl_ecb_temp;
    bool_t result = true;
    if((evtt_id & (~CN_EVTT_ID_MASK)) >= gc_u32CfgEvttLimit)
        return false;
    Int_SaveAsynSignal();
    pl_evtt = &g_ptEvttTable[evtt_id & (~CN_EVTT_ID_MASK)];
    if(pl_evtt->property.registered == 0)
    {//该事件类型是无效事件类型
        result = false;
    }
//  if(pl_evtt->done_sync != NULL)     //若完成同步队列中有事件，取出到就绪队列
//  {
//      pl_ecb = pl_evtt->done_sync;     //取同步队列头
//      while(pl_ecb != NULL)
//      {
//          if(pl_ecb->event_status & CN_STS_SYNC_TIMEOUT)  //是否在超时队列中
//          {
//              __Djy_ResumeDelay(pl_ecb);    //结束超时等待
//          }
//          pl_ecb->wakeup_from = CN_WF_EVTT_DELETED;   //设置唤醒原因
//          pl_ecb->event_status = CN_STS_EVENT_READY;
//          pl_ecb_temp = pl_ecb;
//          if(pl_ecb->multi_next == pl_evtt->done_sync)   //是最后一个事件
//          {
//              pl_evtt->done_sync = NULL;
//              pl_ecb = NULL;
//          }else
//          {
//              pl_ecb = pl_ecb->multi_next;
//          }
//          __Djy_EventReady(pl_ecb_temp);           //把事件加入到就绪队列中
//      }
//  }
    if(pl_evtt->pop_sync != NULL)     //若弹出同步队列中有事件，取出到就绪队列
    {
        pl_ecb = pl_evtt->pop_sync;     //取同步队列头
        while(pl_ecb != NULL)
        {
            if(pl_ecb->event_status & CN_STS_SYNC_TIMEOUT)  //是否在超时队列中
            {
                __Djy_ResumeDelay(pl_ecb);    //结束超时等待
            }
            pl_ecb->wakeup_from = CN_WF_EVTT_DELETED;    //设置唤醒原因
            pl_ecb->event_status = CN_STS_EVENT_READY;
            pl_ecb_temp = pl_ecb;
            if(pl_ecb->multi_next == pl_evtt->done_sync)  //是最后一个事件
            {
                pl_evtt->pop_sync = NULL;  //置空事件同步队列
                pl_ecb = NULL;
            }else
            {
                pl_ecb = pl_ecb->multi_next;      //取队列中下一个事件
            }
            __Djy_EventReady(pl_ecb_temp);       //把事件加入到就绪队列中
        }
    }
    if(pl_evtt->property.inuse)
    {
        //事件类型正在使用，或完成同步和弹出同步队列非空，只标记删除
        pl_evtt->property.deleting = 1;
    }else
    {
        //回收事件类型控制块，只需把registered属性清零。
        pl_evtt->property.registered = 0;
        pl_evtt->evtt_name[0] = '\0';      //清空事件类型名
        next_vm = pl_evtt->my_free_vm;
        while(next_vm != NULL)          //释放该事件类型拥有的空闲线程
        {
            temp = next_vm;
            next_vm = next_vm->next;
            free((void *)temp);
        }
    }
    Int_RestoreAsynSignal();
    return result;
}
const struct EventECB cn_sys_event = {
                        NULL,NULL,//next,previous
                        NULL,NULL,//multi_next,multi_previous
                        NULL,                       //vm
                        0,0,                        //param1,param2
                       NULL,                       //sync
                        NULL,                       //sync_head
                        0,                          //EventStartTime
                        0,                          //consumed_time
//#if(CN_CFG_DEBUG_INFO == 1)
                        0,                          //consumed_time_second
                        0,                          //consumed_time_record
//#endif
                        0,                          //delay_start_tick
                        0,                          //delay_end_tick
                        EN_KNL_NO_ERROR,          //error_no
                        0,                          //event_result
                        0,                          //wait_mem_size
                        CN_WF_EVENT_NORUN,          //wakeup_from
                        CN_STS_EVENT_READY,         //event_status
                        CN_PRIO_SYS_SERVICE,        //prio_base
                        CN_PRIO_SYS_SERVICE,        //prio
                        CN_EVTT_ID_BASE,            //evtt_id
                        0,                          //sync_counter
                        0,                          //event_id
                        0                           //local_memory
                        };

//----建立事件链表-------------------------------------------------------------
//功能：1.根据系统设定初始化操作系统线程和事件表指针
//      2.把 gc_u32CfgEventLimit 个事件控制块结构连接成一个空闲队列,队列的结构参见
//        文档，由指针pg_free_event索引,
//参数：无
//返回：无
//-----------------------------------------------------------------------------
void __Djy_InitSys(void)
{
    u16 i;
    u8 *IdleStack;
    struct ThreadVm *vm;
    g_ptEventDelay=NULL;    //延时事件链表空

    IdleStack = M_Malloc(gc_IdleSeventStackSize,0);
    g_ptECB_Table = M_Malloc(gc_u32CfgEventLimit * sizeof(struct EventECB),0);
    g_ptEvttTable = M_Malloc(gc_u32CfgEvttLimit * sizeof(struct EventType),0);
    if(g_ptEvttTable == NULL)
    {
        while(1);       //此时IO系统未准备好，能跑到这里，开发调试时必然能发现。
    }
    //把事件类型表全部置为没有注册,0为系统服务类型
    for(i=1; i<gc_u32CfgEvttLimit; i++)
    {
        g_ptEvttTable[i].property.registered = 0;
    }
    for(i = 1; i < gc_u32CfgEventLimit; i++)    //i=0为系统服务事件
    {
        if(i==(gc_u32CfgEventLimit-1))
            g_ptECB_Table[i].next = NULL;
        else
            g_ptECB_Table[i].next = &g_ptECB_Table[i+1];
        //向前指针指向pg_event_free的地址,说明这事个空闲事件块.
        //没有别的含义,只是找一个唯一且不变的数值,全局变量地址在整个运行期
        //是不会变化的.
        g_ptECB_Table[i].previous = (struct EventECB*)&s_ptEventFree;
        g_ptECB_Table[i].event_id = i;    //本id号在程序运行期维持不变
        g_ptECB_Table[i].evtt_id = CN_EVENT_ID_INVALID;
    }
    s_ptEventFree = &g_ptECB_Table[1];

    g_ptEventReady = g_ptECB_Table;
    g_ptEventRunning = g_ptEventReady;
    g_ptEvttTable[0].property.correlative = 1;
//    g_ptEvttTable[0].property.overlay = 0;
    g_ptEvttTable[0].property.registered = 1;
    g_ptEvttTable[0].property.inuse = 1;
    g_ptEvttTable[0].property.deleting = 0;
    g_ptEvttTable[0].my_free_vm = NULL;
    g_ptEvttTable[0].SchHook = (SchHookFunc)NULL_func;
   // g_ptEvttTable[0].evtt_name[0] = '\0';

    strcpy(g_ptEvttTable[0].evtt_name,"sys_idle");

    g_ptEvttTable[0].default_prio = CN_PRIO_SYS_SERVICE;
    g_ptEvttTable[0].events = 1;
    g_ptEvttTable[0].vpus_res =0;
    g_ptEvttTable[0].vpus_limit =1;
    g_ptEvttTable[0].vpus = 1;
    g_ptEvttTable[0].thread_routine = __Djy_Service;
    g_ptEvttTable[0].stack_size = gc_IdleSeventStackSize;
    g_ptEvttTable[0].mark_event = g_ptECB_Table;
    g_ptEvttTable[0].done_sync = NULL;
    g_ptEvttTable[0].pop_sync = NULL;

    vm = __CreateStaticThread(&g_ptEvttTable[0],IdleStack,gc_IdleSeventStackSize);
    if(vm == NULL)      //内存不足，不能创建常驻线程
    {
        //此时g_ptEventRunning尚未赋值，无法调用Djy_SaveLastError
//        Djy_SaveLastError(EN_MEM_TRIED);
//        printf"创建线程时内存不足\n\r");
        g_ptEvttTable[0].vpus = 0;
        return ;
    }
    //以下模拟popup函数,弹出系统服务事件.当事件队列为空时,调用popup的结果
    //是不可预知的.由于系统运行时,系统服务事件总在队列中,所以事件队列是不会空
    //的,这里模拟popup事件,而不是直接调用,可以使popup函数中省掉一个判断队列是否
    //空的操作.
    g_ptECB_Table[0] = cn_sys_event;
    g_ptECB_Table[0].vm = vm;
    g_ptECB_Table[0].next = g_ptECB_Table;
    g_ptECB_Table[0].previous = g_ptECB_Table;
    g_ptECB_Table[0].multi_next = g_ptECB_Table;
    g_ptECB_Table[0].multi_previous = g_ptECB_Table;
}

//--------检查是否允许调度------------------------------------------------------
//功能: 检查是否允许调度,允许异步信号且运行在线程态时，允许调度
//参数: 无
//返回: 允许调度返回true,否则返回false
//-----------------------------------------------------------------------------
bool_t Djy_QuerySch(void)
{
    return g_bScheduleEnable;
}

//--------检查调度是否已经开始--------------------------------------------------
//功能: 检查是否已经启动多事件(线程)调度,调用__Djy_StartOs函数后,本函数即返回true,
//      否则返回false
//参数: 无
//返回: true = 调度已经开始,不管是否允许切换.false = 调度尚未开始
//-----------------------------------------------------------------------------
bool_t Djy_IsMultiEventStarted(void)
{
    return g_bMultiEventStarted;
}
//----把事件插入就绪队列中----------------------------------------------------
//功能：把事件插入到就绪队列中合适的位置,该事件原来不在就绪队列中.本函数只处理
//      事件队列，不设置event_status的值
//参数：event_ready,待插入的事件,该事件原来不在就绪队列中
//返回：无
//------------------------------------------------------------------------------
void __Djy_EventReady(struct EventECB *event_ready)
{
    struct EventECB *event;
    atom_low_t atom_low;
    atom_low = Int_LowAtomStart(  );             //本函数对不能嵌套调用
    event = g_ptEventReady;
    do
    { //找到一个优先级低于新事件的事件.由于系统服务事件prio=250总是ready,因此总是能找到.
        if(event->prio <= event_ready->prio)
            event = event->multi_next;
        else
            break;
    }while(event != g_ptEventReady);
    event_ready->next = event;
    event_ready->previous = event->previous;
    event->previous->next = event_ready;
    event->previous = event_ready;

    //新插入的事件在同优先级的最后，故这样能够判断新事件是否该优先级的唯一事件。
    //若是该优先级的唯一事件，就要插入优先级单调队列
    if(event_ready->prio != event_ready->previous->prio)
    {//此时event == event_ready->next;
        event->multi_previous->multi_next = event_ready;
        event_ready->multi_previous = event->multi_previous;
        event->multi_previous = event_ready;
        event_ready->multi_next = event;
    }else       //无须插入优先级单调队列
    {
        event_ready->multi_next = NULL;
        event_ready->multi_previous = NULL;
    }
    if(event_ready->prio < g_ptEventReady->prio)
        g_ptEventReady = event_ready;
    Int_LowAtomEnd( atom_low );
}

//----退出闹钟同步队列---------------------------------------------------------
//功能: 把一个事件从闹钟同步队列中取出,不管闹铃时间是否已经到达.用在带超时的同步
//      功能中，当同步条件先于超时时限到达时，从闹钟同步队列取出事件。
//参数: delay_event,待处理的事件.
//返回: 无
//备注: 1.本函数不开放给用户,仅仅是操作系统内部使用.操作系统不给用户提供提前
//      退出延时的功能,这样可以防止事件间互相干涉
//      2.本函数应该在关闭调度条件下调用,调用者保证,函数内部不检查中断状态.
//      3.本函数只把事件从闹钟同步链表中取出，并不放到就绪队列中。
//-----------------------------------------------------------------------------
void __Djy_ResumeDelay(struct EventECB *delay_event)
{
    if(g_ptEventDelay->next == g_ptEventDelay)  //队列中只有一个事件
        g_ptEventDelay = NULL;
    else
    {
        if(delay_event == g_ptEventDelay)
            g_ptEventDelay = g_ptEventDelay->next;
        delay_event->next->previous = delay_event->previous;
        delay_event->previous->next = delay_event->next;
    }
    delay_event->next = NULL;
    delay_event->previous = NULL;
    delay_event->delay_end_tick = DjyGetSysTick();
}

//----加入延时队列------------------------------------------------------------
//功能：由正在执行的事件调用,直接把自己加入延时队列，不引起调度也不操作ready
//      队列，一般由同步函数调用，在timeout!=0时把事件加入闹钟同步队列实现
//      timeout功能，是特定条件下对y_timer_sync函数的简化。
//参数：u32l_uS,延迟时间,单位是微秒，将被向上调整为CN_CFG_TICK_US的整数倍
//返回：无
//备注：1、操作系统内部使用的函数，且需在关闭中断（禁止调度）的条件下使用。
//      2、调用本函数前running事件已经从就绪表中取出，本函数不改变就绪表。
//      3、与其他内部函数一样，由调用方保证参数合理性，即u32l_uS>0.
//-----------------------------------------------------------------------------
//change by lst in 20130922,ticks改为64bit后，删掉处理32位数溢出回绕的代码
void __Djy_AddToDelay(u32 u32l_uS)
{
    struct EventECB * event;

    g_ptEventRunning->delay_start_tick = DjyGetSysTick(); //事件延时开始时间
    g_ptEventRunning->delay_end_tick = g_ptEventRunning->delay_start_tick
                  + ((s64)u32l_uS + CN_CFG_TICK_US -(u32)1)/CN_CFG_TICK_US; //闹铃时间

    if(g_ptEventDelay==NULL)    //延时队列空
    {
        g_ptEventRunning->next = g_ptEventRunning;
        g_ptEventRunning->previous = g_ptEventRunning;
        g_ptEventDelay=g_ptEventRunning;
    }else
    {
        event = g_ptEventDelay;
        do
        {//本循环找到第一个剩余延时时间大于新延时事件的事件.
            if(event->delay_end_tick <= g_ptEventRunning->delay_end_tick)
            {
                event = event->next;
            }
            else
                break;

        }while(event != g_ptEventDelay);
        //如果没有找到剩余延时时间比新延时事件长的事件,新事件插入队列尾,
        //而队列尾部就是pg_event_delay的前面,event恰好等于pg_event_delay
        //如果找到剩余延时时间长于新事件的事件,新事件插入该事件前面.
        //所以无论前面循环找到与否,均可使用下列代码
        g_ptEventRunning->next = event;
        g_ptEventRunning->previous = event->previous;
        event->previous->next = g_ptEventRunning;
        event->previous = g_ptEventRunning;
        if(g_ptEventDelay->delay_end_tick > g_ptEventRunning->delay_end_tick)
            //新事件延时小于原队列中的最小延时.
            g_ptEventDelay = g_ptEventRunning;
    }
}

//----添加处理中事件进阻塞队列-------------------------------------------------
//功能：把正在运行的事件，添加进阻塞队列，只添加队列，不调度。
//      只支持FIFO方式或者优先级排序，其他排序方式不能使用本函数
//      本函数只允许内核模块调用，调用者保证并发安全。
//参数：Head,阻塞队列头
//      Qsort，阻塞队列排队方式，只支持CN_BLOCK_FIFO和CN_BLOCK_PRIO
//      timeout，超时设置,不等于0则进入阻塞队列的同时，还进入超时队列。
//         单位是微秒，timeout==CN_TIMEOUT_FOREVER则不会进入超时队列，
//         timeout ==0则不应调用本函数。非0值将被向上调整为CN_CFG_TICK_US的倍数
//      Status，阻塞状态，参看CN_STS_EVENT_READY系列定义
//返回：无
//-----------------------------------------------------------------------------
void __Djy_AddRunningToBlock(struct EventECB **Head,bool_t Qsort,u32 timeout,u32 Status)
{
    struct EventECB *event;
    __Djy_CutReadyEvent(g_ptEventRunning);
    g_ptEventRunning->previous = NULL;
    g_ptEventRunning->next = NULL;

    g_ptEventRunning->sync_head = Head;
    if(*Head == NULL)
    {//同步队列空,running事件自成双向循环链表
        g_ptEventRunning->multi_next = g_ptEventRunning;
        g_ptEventRunning->multi_previous = g_ptEventRunning;
        *Head = g_ptEventRunning;
    }else
    {
        event = *Head;
        if(Qsort == CN_BLOCK_PRIO)   //同步队列按优先级排序
        {
            do
            {   //找到一个优先级低于新事件的事件.
                //如果找不到，会停在队列头的位置，该位置也是正确的插入点。
                if(event->prio <= g_ptEventRunning->prio)
                    event = event->multi_next;
                else
                    break;
            }while(event != *Head);
            g_ptEventRunning->event_status |= CN_BLOCK_PRIO_SORT;
            g_ptEventRunning->multi_next = event;
            g_ptEventRunning->multi_previous = event->multi_previous;
            event->multi_previous->multi_next = g_ptEventRunning;
            event->multi_previous = g_ptEventRunning;
            if((*Head)->prio > g_ptEventRunning->prio)
                *Head = g_ptEventRunning;
        }else                               //按先后顺序，新事件直接排在队尾
        {
            g_ptEventRunning->event_status &= ~CN_BLOCK_PRIO_SORT;
            g_ptEventRunning->multi_next = event;
            g_ptEventRunning->multi_previous = event->multi_previous;
            event->multi_previous->multi_next = g_ptEventRunning;
            event->multi_previous = g_ptEventRunning;
        }
    }
    if(timeout != CN_TIMEOUT_FOREVER)
    {
        //事件状态设为等待信号量 +  超时
        g_ptEventRunning->event_status = Status + CN_STS_SYNC_TIMEOUT;
        __Djy_AddToDelay(timeout);
    }else
    {
        g_ptEventRunning->event_status = Status;  //事件状态设为等待信号量
    }
}

//----修改阻塞队列-------------------------------------------------------------
//功能：调整指定事件在以PRIO排序的阻塞队列中的位置，在修改了阻塞中的事件优先级后，
//      就需要调用本函数改变其在队列中的位置。
//      本函数只允许内核模块调用，调用者保证并发安全。
//参数：Event，被操作的事件
//返回：无
//-----------------------------------------------------------------------------
void __Djy_ChangeBlockQueue(struct EventECB *Event)
{
    struct EventECB *pl_ecb;
    struct EventECB *Head;
    ufast_t Prio;
    Head = *Event->sync_head;
    pl_ecb = Head;
    Prio = Event->prio;
    //无须移动位置的条件：
    //第一行：处于队列头，且最小。
    //第二行：优先不低于后一个，不高于前一个。
    //第三行：处于队列尾，且最大。
    if( ((Event == pl_ecb)  && (Prio <= Event->multi_next->prio))
       ||((Prio >= Event->multi_previous->prio)&& (Prio <= Event->multi_next->prio))
       ||((Event == pl_ecb->multi_previous)&& (Prio >= Event->multi_previous->prio)) )
        return ;

    //从队列中取出事件控制块
    Event->multi_previous->multi_next = Event->multi_next;
    Event->multi_next->multi_previous = Event->multi_previous;
    do
    {   //找到一个优先级低于新事件的事件.
        //如果找不到，会停在队列头的位置，该位置也是正确的插入点。
        if(pl_ecb->prio < Event->prio)
            pl_ecb = pl_ecb->multi_next;
        else
            break;
    }while(pl_ecb != Head);

    Event->multi_next = pl_ecb;
    Event->multi_previous = pl_ecb->multi_previous;
    pl_ecb->multi_previous->multi_next = Event;
    pl_ecb->multi_previous = pl_ecb;
    if((Head)->prio > Event->prio)
        Head = Event;
}

//----设置事件优先级-----------------------------------------------------------
//功能: 改变事件优先级，如果改变自身的优先级，并把优先级被改低了，可能立即调度，
//      阻塞本事件运行。
//参数: event_id，被操作的事件id
//      new_prio,设置的新优先级
//返回: true = 成功设置，false=失败，一般是优先级不合法
//-----------------------------------------------------------------------------
bool_t Djy_SetEventPrio(u16 event_id,ufast_t new_prio)
{
    struct EventECB * pl_ecb;

    if((new_prio >= CN_PRIO_SYS_SERVICE) || (new_prio == 0)
       || (event_id >= gc_u32CfgEventLimit))
        return false;

    Int_SaveAsynSignal();
    pl_ecb = &g_ptECB_Table[event_id];
    pl_ecb->prio_base = new_prio;
    if(new_prio != pl_ecb->prio)
    {
        //事件原来的状态，可能：1、处于就绪态，2、处于某种阻塞态。
        //如果处于某种阻塞态，假如队列是按优先级排序的，则要修改该队列。
        if(pl_ecb->event_status == CN_STS_EVENT_READY)
        {
            //注：此三句的顺序不能变，因为prio的值对__Djy_CutReadyEvent函数执行
            //    结果有影响
            __Djy_CutReadyEvent(pl_ecb);
            pl_ecb->prio = new_prio;
            __Djy_EventReady(pl_ecb);
        }
        else if(pl_ecb->event_status & CN_BLOCK_PRIO_SORT)
        {
            pl_ecb->prio = new_prio;
            __Djy_ChangeBlockQueue(pl_ecb);
        }
    }
    Int_RestoreAsynSignal();
    return true;
}

//----继承事件优先级-----------------------------------------------------------
//功能: 如果g_ptEventRunning的优先级较高，event_id临时以g_ptEventRunning的优先级
//      运行，直到调用Djy_RestorePrio，否则不改变优先级。
//参数: event_id，被操作的事件id
//返回: true = 成功设置，false=失败，一般是优先级不合法
//-----------------------------------------------------------------------------
bool_t Djy_RaiseTempPrio(u16 event_id)
{
    struct EventECB * pl_ecb;

    if(event_id >= gc_u32CfgEventLimit)
        return false;

    Int_SaveAsynSignal();
    pl_ecb = &g_ptECB_Table[event_id];
    if(g_ptEventRunning->prio < pl_ecb->prio)
    {
        //事件原来的状态，可能：1、处于就绪态，2、处于某种阻塞态。
        //如果处于某种阻塞态，假如队列是按优先级排序的，则要修改该队列。
        if(pl_ecb->event_status == CN_STS_EVENT_READY)
        {
            //注：此三句的顺序不能变，因为prio的值对__Djy_CutReadyEvent函数执行
            //    结果有影响
            __Djy_CutReadyEvent(pl_ecb);
            pl_ecb->prio = g_ptEventRunning->prio;
            __Djy_EventReady(pl_ecb);
        }
        else if(pl_ecb->event_status & CN_BLOCK_PRIO_SORT)
        {
            pl_ecb->prio = g_ptEventRunning->prio;
            __Djy_ChangeBlockQueue(pl_ecb);
        }
    }
    Int_RestoreAsynSignal();
    return true;
}

//----恢复事件优先级-----------------------------------------------------------
//功能: 调用Djy_SetTempPrio后，可以调用本函数恢复g_ptEventRunning的优先级。
//参数: 无，
//返回: true = 成功设置，false=失败，一般是优先级不合法
//-----------------------------------------------------------------------------
bool_t Djy_RestorePrio(void)
{

    Int_SaveAsynSignal();
    if(g_ptEventRunning->prio != g_ptEventRunning->prio_base)
    {
        //注：此三句的顺序不能变，因为prio的值对__Djy_CutReadyEvent函数执行
        //    结果有影响
        __Djy_CutReadyEvent(g_ptEventRunning);
        g_ptEventRunning->prio = g_ptEventRunning->prio_base;
        __Djy_EventReady(g_ptEventRunning);
    }
    Int_RestoreAsynSignal();
    return true;
}

//----闹钟同步-----------------------------------------------------------------
//功能：由正在执行的事件调用,使自己暂停u32l_uS微秒后继续运行.
//参数：u32l_uS,延迟时间,单位是微秒，0且允许轮转调度则把事件放到同优先级的
//      最后。非0值将被向上调整为CN_CFG_TICK_US的整数倍
//返回：实际延时时间us数
//备注：延时队列为双向循环链表
//change by lst in 20130922,ticks改为64bit后，删掉处理32位数溢出回绕的代码
//-----------------------------------------------------------------------------
u32 Djy_EventDelay(u32 u32l_uS)
{
    struct EventECB * event;

    if( !Djy_QuerySch())
    {
//        Djy_SaveLastError(EN_KNL_CANT_SCHED);
        Djy_DelayUs(u32l_uS);
        return u32l_uS;
    }
    Int_SaveAsynSignal();
    //延时量为0的算法:就绪队列中有同优先级的，把本事件放到轮转最后一个，
    //注意:这里不管轮转调度是否允许
    if(u32l_uS == 0)
    {
        if((g_ptEventRunning->prio == g_ptEventRunning->next->prio)
                    && (g_ptEventRunning != g_ptEventRunning->next)   )
        {
            g_ptEventRunning->delay_start_tick = DjyGetSysTick();//闹铃时间
            __Djy_CutReadyEvent(g_ptEventRunning);      //从同步队列取出
            __Djy_EventReady(g_ptEventRunning);            //放回同步队列尾部
        }else
        {
            Int_RestoreAsynSignal();
            return 0;   //延时量为0，且不符合轮转条件
        }
    }else
    {
        g_ptEventRunning->delay_start_tick =DjyGetSysTick();//设定闹铃的时间
        g_ptEventRunning->delay_end_tick = g_ptEventRunning->delay_start_tick
                  + ((s64)u32l_uS + CN_CFG_TICK_US -(u32)1)/CN_CFG_TICK_US; //闹铃时间

        __Djy_CutReadyEvent(g_ptEventRunning);

        g_ptEventRunning->event_status = CN_STS_EVENT_DELAY;
        if(g_ptEventDelay==NULL)    //闹钟同步队列空
        {
            g_ptEventRunning->next = g_ptEventRunning;
            g_ptEventRunning->previous = g_ptEventRunning;
            g_ptEventDelay=g_ptEventRunning;
        }else
        {
            event = g_ptEventDelay;
            do
            {//本循环找到第一个闹铃时间晚于新事件的事件.
                if(event->delay_end_tick <= g_ptEventRunning->delay_end_tick)
                {
                    event = event->next;
                }
                else
                    break;
            }while(event != g_ptEventDelay);
            //下面把新事件插入前述找到的事件前面，如没有找到，则event将等于
            //pg_event_delay，因是双向循环队列，g_event_delay前面也就刚好是队列尾。
            g_ptEventRunning->next = event;
            g_ptEventRunning->previous = event->previous;
            event->previous->next = g_ptEventRunning;
            event->previous = g_ptEventRunning;
            if(g_ptEventDelay->delay_end_tick >g_ptEventRunning->delay_end_tick)
                //新事件延时小于原队列中的最小延时.
                g_ptEventDelay = g_ptEventRunning;
        }
    }
    Int_RestoreAsynSignal();
    return (DjyGetSysTick() -g_ptEventRunning->delay_start_tick)*CN_CFG_TICK_US;
}

//----闹钟同步2----------------------------------------------------------------
//功能：由正在执行的事件调用,使自己暂停至s64l_uS微秒后继续运行.
//参数：s64l_uS,延迟结束时刻,单位是微秒，将被向上调整为CN_CFG_TICK_US的整数倍
//返回：实际延时时间us数
//备注：延时队列为双向循环链表
//add by lst in 20130922
//-----------------------------------------------------------------------------
u32 Djy_EventDelayTo(s64 s64l_uS)
{
    struct EventECB * event;

    if( !Djy_QuerySch())
    {
//      Djy_SaveLastError(EN_KNL_CANT_SCHED);
        Djy_DelayUs((u32)(s64l_uS - DjyGetSysTime()));
        return 0;
    }
    Int_SaveAsynSignal();
    g_ptEventRunning->delay_start_tick =DjyGetSysTick();//设定闹铃的时间
    g_ptEventRunning->delay_end_tick =(s64l_uS +CN_CFG_TICK_US -1)/CN_CFG_TICK_US;
    if(g_ptEventRunning->delay_end_tick <= g_ptEventRunning->delay_start_tick)
    {
        Int_RestoreAsynSignal();
        return 0;
    }

    __Djy_CutReadyEvent(g_ptEventRunning);

    g_ptEventRunning->event_status = CN_STS_EVENT_DELAY;
    if(g_ptEventDelay==NULL)    //闹钟同步队列空
    {
        g_ptEventRunning->next = g_ptEventRunning;
        g_ptEventRunning->previous = g_ptEventRunning;
        g_ptEventDelay=g_ptEventRunning;
    }else
    {
        event = g_ptEventDelay;
        do
        {//本循环找到第一个闹铃时间晚于新事件的事件.
            if(event->delay_end_tick <= g_ptEventRunning->delay_end_tick)
            {
                event = event->next;
            }
            else
                break;
        }while(event != g_ptEventDelay);
        //下面把新事件插入前述找到的事件前面，如没有找到，则event将等于
        //pg_event_delay，因是双向循环队列，g_event_delay前面也就刚好是队列尾。
        g_ptEventRunning->next = event;
        g_ptEventRunning->previous = event->previous;
        event->previous->next = g_ptEventRunning;
        event->previous = g_ptEventRunning;
        if(g_ptEventDelay->delay_end_tick >g_ptEventRunning->delay_end_tick)
            //新事件延时小于原队列中的最小延时.
            g_ptEventDelay = g_ptEventRunning;
    }
    Int_RestoreAsynSignal();
    return (DjyGetSysTick() -g_ptEventRunning->delay_start_tick)*CN_CFG_TICK_US;
}

//----同步事件----------------------------------------------------------------
//功能: 把正在运行的事件加入到指定事件的同步队列中去,然后重新调度。当指定事件
//      处理完成，或者超时时间到，将唤醒当前事件。
//参数: event_id,目标事件id号
//      timeout，超时设置,单位是微秒，CN_TIMEOUT_FOREVER=无限等待，0则立即按
//      超时返回。非0值将被向上调整为CN_CFG_TICK_US的整数倍
//返回: CN_SYNC_SUCCESS=同步条件成立返回，
//      CN_SYNC_TIMEOUT=超时返回，
//      EN_KNL_CANT_SCHED=禁止调度时不可执行同步操作
//      EN_KNL_EVENT_FREE,event_id是空闲id
//特注: 本函数不像semp_pend一样，会追溯历史的semp_post。如果调用本函数时，
//      completed已经发生，则只能等到下一次completed才会唤醒。
//----------------------------------------------------------------------------
u32 Djy_WaitEventCompleted(u16 event_id,u32 timeout)
{
    struct EventECB * pl_ecb;
    pl_ecb = &g_ptECB_Table[event_id];

    if (event_id == g_ptEventRunning->event_id)
        return CN_EVENT_ID_INVALID;
    if(Djy_QuerySch() == false)  //不能在禁止调度的情况下执行同步操作
        return (u32)EN_KNL_CANT_SCHED;
    if(timeout == 0)
        return (u32)CN_SYNC_TIMEOUT;
    Int_SaveAsynSignal();
    if(pl_ecb->previous == (struct EventECB *)&s_ptEventFree)
    {//目标事件控制块是空闲事件控制块
        Int_RestoreAsynSignal();
        return (u32)EN_KNL_EVENT_FREE;
    }
    __Djy_CutReadyEvent(g_ptEventRunning);
    g_ptEventRunning->next = NULL;
    g_ptEventRunning->previous = NULL;

    //以下把pg_event_running加入到目标事件的同步队列中

    g_ptEventRunning->sync_head = &pl_ecb->sync;
    if(pl_ecb->sync != NULL)
    {//被同步事件的同步队列不是空的
        g_ptEventRunning->multi_next = pl_ecb->sync;
        g_ptEventRunning->multi_previous = pl_ecb->sync->multi_previous;
        pl_ecb->sync->multi_previous->multi_next = g_ptEventRunning;
        pl_ecb->sync->multi_previous = g_ptEventRunning;
    }else
    {//被同步事件的同步队列是空的
        pl_ecb->sync = g_ptEventRunning;
        g_ptEventRunning->multi_next = g_ptEventRunning;
        g_ptEventRunning->multi_previous = g_ptEventRunning;
    }

    if(timeout != CN_TIMEOUT_FOREVER)
    {
        g_ptEventRunning->event_status = CN_STS_WAIT_EVENT_DONE
                                        +CN_STS_SYNC_TIMEOUT;
        __Djy_AddToDelay(timeout);
    }
    else
    {
        g_ptEventRunning->event_status = CN_STS_WAIT_EVENT_DONE;
    }
    Int_RestoreAsynSignal();  //恢复中断会引发重新调度

    //检查从哪里返回，是超时还是同步事件完成。
    if(g_ptEventRunning->wakeup_from & CN_STS_SYNC_TIMEOUT)
    {//说明同步条件未到，从超时返回。
        return (u32)CN_SYNC_TIMEOUT;
    }else if(g_ptEventRunning->wakeup_from & CN_STS_EVENT_EXP_EXIT)
    {//被同步事件被异常终止，可能没有被正确执行
        return (u32)EN_KNL_EVENT_SYNC_EXIT;
    }else
    {
        return (u32)CN_SYNC_SUCCESS;
    }
}

//----事件类型完成同步----------------------------------------------------------
//功能: 把正在运行的事件加入到指定事件类型的前同步队列中去,然后重新调度。完成
//      同步以目标事件类型的完成次数为同步条件。
//参数: evtt_id,目标事件类型号
//      done_times,完成次数，0表示待最后一条该类型事件完成.
//      timeout，超时设置,单位是微秒，CN_TIMEOUT_FOREVER=无限等待，0则立即按
//      超时返回。非0值将被向上调整为CN_CFG_TICK_US的整数倍
//返回: CN_SYNC_SUCCESS=同步条件成立返回，
//      CN_SYNC_TIMEOUT=超时返回，
//      EN_KNL_CANT_SCHED=禁止调度时不可执行同步操作
//      EN_KNL_EVTT_FREE=evtt_id是空闲id
//      本函数必须联系共享文档的相关章节才容易读懂，注释难于解释那么清楚的。
//特注: 本函数不像semp_pend一样，会追溯历史的semp_post。如果调用本函数时，
//      completed已经发生，则只能等到下一次completed才会唤醒。
//----------------------------------------------------------------------------
u32 Djy_WaitEvttCompleted(u16 evtt_id,u16 done_times,u32 timeout)
{
    struct EventType *pl_evtt;
    struct EventECB *pl_ecb;
    u16 evttoffset;
    evttoffset = evtt_id & (~CN_EVTT_ID_MASK);
    if(evttoffset >= gc_u32CfgEvttLimit)
        return EN_KNL_EVTTID_LIMIT;
    pl_evtt = &g_ptEvttTable[evttoffset];
    if(Djy_QuerySch() == false)  //不能在禁止调度的情况下执行同步操作
        return (u32)EN_KNL_CANT_SCHED;
    if(timeout == 0)
        return (u32)CN_SYNC_TIMEOUT;
    Int_SaveAsynSignal();
    if((pl_evtt->property.registered == 0)     //事件类型未注册
        ||(pl_evtt->property.deleting == 1))       //事件类型正在等待注销
    {
        Int_RestoreAsynSignal();
        return (u32)EN_KNL_EVTT_FREE;
    }
    __Djy_CutReadyEvent(g_ptEventRunning);
    g_ptEventRunning->next = NULL;
    g_ptEventRunning->previous = NULL;
    g_ptEventRunning->sync_counter = done_times;

    //以下把pg_event_running加入到目标事件的同步队列中
    g_ptEventRunning->sync_head = &pl_evtt->done_sync;
    if(pl_evtt->done_sync != NULL)
    {//被同步事件类型的同步队列不是空的
        pl_ecb = pl_evtt->done_sync;
        do
        {//本循环找到第一个剩余完成次数不小于新事件的事件.
            if(pl_ecb->sync_counter < done_times)
                pl_ecb = pl_ecb->multi_next;
            else
                break;
        }while(pl_ecb != pl_evtt->done_sync);
        //下面把新事件(g_ptEventRunning)插入到队列中
        g_ptEventRunning->multi_next = pl_ecb;
        g_ptEventRunning->multi_previous = pl_ecb->multi_previous;
        pl_ecb->multi_previous->multi_next = g_ptEventRunning;
        pl_ecb->multi_previous = g_ptEventRunning;
        if(pl_evtt->done_sync->sync_counter > done_times)
            pl_evtt->done_sync = g_ptEventRunning;
    }else
    {//被同步事件类型的同步队列是空的
        pl_evtt->done_sync = g_ptEventRunning;
        g_ptEventRunning->multi_next = g_ptEventRunning;
        g_ptEventRunning->multi_previous = g_ptEventRunning;
    }

    if(timeout != CN_TIMEOUT_FOREVER)
    {
        g_ptEventRunning->event_status = CN_STS_WAIT_EVTT_DONE
                                         +CN_STS_SYNC_TIMEOUT;
        __Djy_AddToDelay(timeout);
    }else
    {
        g_ptEventRunning->event_status = CN_STS_WAIT_EVTT_DONE;
    }
    Int_RestoreAsynSignal();  //恢复调度会引发事件切换

    //检查从哪里返回，是超时还是同步事件完成。
    if(g_ptEventRunning->wakeup_from & CN_STS_SYNC_TIMEOUT)
    {//说明同步条件未到，从超时返回，此时，被同步的事件肯定还没有完成。
        return (u32)CN_SYNC_TIMEOUT;
    }else if(g_ptEventRunning->wakeup_from & CN_STS_EVENT_EXP_EXIT)
    {//被同步事件被异常终止，可能没有被正确执行
        return (u32)EN_KNL_EVENT_SYNC_EXIT;
    }else
    {
        return (u32)CN_SYNC_SUCCESS;
    }
}

//----事件类型弹出同步---------------------------------------------------------
//功能: 把正在运行的事件加入到指定事件类型的弹出同步队列中去,然后重新调度。弹出
//      同步是指以该事件类型的事件弹出若干次为同步条件。
//      pop_times输入作为计算同步的起始次数，同时用于输出同步条件达到时，目标事
//      件类型的弹出次数。
//参数: evtt_id,目标事件类型号
//      base_times,弹出次数起始值，目标事件累计弹出*base_times+1作为同步条件，
//          同步条件达到时，返回实际弹出次数。如果给NULL,则从调用时的弹出次数+1
//          做同步条件，不能得到实际弹出次数。
//      timeout，超时设置,单位是微秒，CN_TIMEOUT_FOREVER=无限等待，0则立即按
//      超时返回。非0值将被向上调整为CN_CFG_TICK_US的整数倍
//返回: CN_SYNC_SUCCESS=同步条件成立返回，
//      CN_SYNC_TIMEOUT=超时返回，
//      CN_SYNC_ERROR=出错，
//      本函数必须联系共享文档的相关章节才容易读懂，注释难于解释那么清楚的。
//----------------------------------------------------------------------------
u32 Djy_WaitEvttPop(u16 evtt_id,u32 *base_times, u32 timeout)
{
    struct EventType *pl_evtt;
    struct EventECB *pl_ecb;
    u32 popt;
    u16 evttoffset;
    evttoffset = evtt_id & (~CN_EVTT_ID_MASK);
    if(evttoffset >= gc_u32CfgEvttLimit)
        return EN_KNL_EVTTID_LIMIT;
    pl_evtt = &g_ptEvttTable[evttoffset];
    //不能在禁止调度的情况下执行同步操作
    if(Djy_QuerySch() == false)
        return CN_SYNC_ERROR;
    if(timeout == 0)
        return CN_SYNC_TIMEOUT;
    Int_SaveAsynSignal();
    if((pl_evtt->property.registered == 0)  //事件类型未注册
        ||(pl_evtt->property.deleting == 1))   //事件类型正在等待注销
    {
        Int_RestoreAsynSignal();
        return CN_SYNC_ERROR;
    }

    if(base_times != NULL)
        popt = *base_times - pl_evtt->pop_times +1;
    else
        popt = 1;
    //这个判断很重要，条件成立说明调用本函数时，等待条件已经成立，应按照条件成立
    //返回，不然会没完没了地等。
    if((popt >= 0x80000000) || (popt == 0))
    {
        if(base_times != NULL)
            *base_times = pl_evtt->pop_times;
        Int_RestoreAsynSignal();
        return CN_SYNC_SUCCESS;
    }
    __Djy_CutReadyEvent(g_ptEventRunning);
    g_ptEventRunning->next = NULL;
    g_ptEventRunning->previous = NULL;
    g_ptEventRunning->sync_counter = popt;
    //以下把pg_event_running加入到目标事件的同步队列中
    g_ptEventRunning->sync_head = &pl_evtt->pop_sync;
    if(pl_evtt->pop_sync != NULL)
    {//被同步事件类型的同步队列不是空的
        pl_ecb = pl_evtt->pop_sync;
        do
        {//本循环找到第一个剩余弹出次数不小于新事件的事件.
            if(pl_ecb->sync_counter < g_ptEventRunning->sync_counter)
                pl_ecb = pl_ecb->multi_next;
            else
                break;
        }while(pl_ecb != pl_evtt->pop_sync);
        //下面把新事件(g_ptEventRunning)插入到队列中
        g_ptEventRunning->multi_next = pl_ecb;
        g_ptEventRunning->multi_previous = pl_ecb->multi_previous;
        pl_ecb->multi_previous->multi_next = g_ptEventRunning;
        pl_ecb->multi_previous = g_ptEventRunning;
        if(pl_evtt->pop_sync->sync_counter > g_ptEventRunning->sync_counter)
            pl_evtt->pop_sync = g_ptEventRunning;
    }else
    {//被同步事件的同步队列是空的
        pl_evtt->pop_sync = g_ptEventRunning;
        g_ptEventRunning->multi_next = g_ptEventRunning;
        g_ptEventRunning->multi_previous = g_ptEventRunning;
    }
    if(timeout != CN_TIMEOUT_FOREVER)
    {
        g_ptEventRunning->event_status = CN_STS_WAIT_EVTT_POP
                                         + CN_STS_SYNC_TIMEOUT;
        __Djy_AddToDelay(timeout);
    }else
    {
        g_ptEventRunning->event_status = CN_STS_WAIT_EVTT_POP;
    }

    Int_RestoreAsynSignal();  //恢复中断会引发重新调度
    if(base_times != NULL)
        *base_times = pl_evtt->pop_times;

    //检查从哪里返回，是超时还是同步事件完成。
    if(g_ptEventRunning->wakeup_from & CN_STS_SYNC_TIMEOUT)
    {//说明同步条件未到，从超时返回，此时，同步条件肯定还没有达成。
        return CN_SYNC_TIMEOUT;
    }else
    {
        return CN_SYNC_SUCCESS;
    }
}

//----弹出事件-----------------------------------------------------------------
//功能：向操作系统报告发生事件,可以带一个参数缓冲区，提供处理事件所需要的附加信
//      息。操作系统接报后,将分配事件控制块，或者把参数加入到已存在事件的参数队
//      列中.
//
//参数：hybrid_id，id，可能是事件类型id，也可能是事件id。
//          如果是事件id，系统会查找该事件对应的事件类型id。
//          如果是关联/串行型事件类型，则事件id和事件类型id是等效的。
//          如果是独立/并行型事件类型，则根据hybrid_id传入的id类型做分别处理:
//              1、事件类型id，将新分配一个事件类型控制块，处理事件时，将会分配/
//                 创建一个新的线程。
//              2、事件id，则只是把新参数加入事件的参数队列中。
//      timeout,定义超时时间，如果=0，则弹出事件后，立即返回，何时处理事件将由
//          调度器决定；若!=0，则等待事件处理完成后才返回，并且在pop_result中返
//          回处理结果。单位是us
//      pop_result,事件弹出或处理状态，如果函数返回了合法的事件id，
//          且timeout !=0，则
//              pop_result = cn_sync_success，表示事件被处理完成后返回
//              pop_result = cn_sync_timeout，表示事件未被处理完，超时返回
//              pop_result = enum_knl_event_sync_exit，事件处理被异常终止
//              如果timeout ==0，则pop_result无意义。
//
//          如果函数返回了cn_invalid_event_id，则返回具体的出错信息
//      PopPrarm1，PopPrarm2：传递给事件的参数，如果连续弹出关联型事件，事件控
//          制块中总是保存最后一次弹出的参数。
//      prio,事件优先级,对于需要创建新事件的情况，0表示新事件使用默认值(存在事件
//          类型控制块中)。
//          对于无需创建新事件的情况，分两种情况:
//              1、如果没有携带参数，则不作任何处理，不会改变已经进入事件队列的
//                 事件的优先级。
//              2、如果携带参数，prio不高于已有优先级则新参数置入task_low_prio队
//                 列，prio高于已有优先级则新参数置入task_high_prio队列，数越大，
//                 优先级越低。
//返回：若不能获取事件控制块，返回cn_invalid_event_id，否则返回事件id。
//注: 不会因不能获得事件控制块而阻塞。
//-----------------------------------------------------------------------------
u16 Djy_EventPop(   u16  hybrid_id,
                    u32 *pop_result,
                    u32 timeout,    //如果阻塞，定义超时时间，
                    ptu32_t PopPrarm1,
                    ptu32_t PopPrarm2,
                    ufast_t prio)
{
    struct ExpThrowPara  parahead;
    char ExpStr[32];
    struct EventECB *pl_ecb;
    struct EventType *pl_evtt;
    struct EventECB *pl_ecb_temp;
    u16 evtt_offset;
    u16 return_result;
    bool_t schbak;          //是否允许调度
    if(hybrid_id >= CN_EVTT_ID_BASE)
    {
        evtt_offset = hybrid_id & (~CN_EVTT_ID_MASK);
        if(evtt_offset >= gc_u32CfgEvttLimit)
        {
            Djy_SaveLastError(EN_KNL_EVTTID_LIMIT);
            if(pop_result != NULL)
                *pop_result = (ptu32_t)EN_KNL_EVTTID_LIMIT;
            return CN_EVENT_ID_INVALID;
        }
    }
    else
    {
        if(hybrid_id >= gc_u32CfgEventLimit)
        {
            Djy_SaveLastError(EN_KNL_EVENT_FREE);
            if(pop_result != NULL)
                *pop_result = (ptu32_t)EN_KNL_EVENT_FREE;
            return CN_EVENT_ID_INVALID;
        }
        else
        {
            evtt_offset = g_ptECB_Table[hybrid_id].evtt_id & (~CN_EVTT_ID_MASK);
        }
    }
    schbak = Djy_QuerySch( );
    if((timeout != 0) && (!schbak))
    {   //要求事件处理完才返回，但因不允许调度而无法执行
        Djy_SaveLastError(EN_KNL_CANT_SCHED);
        if(pop_result != NULL)
            *pop_result = (ptu32_t)EN_KNL_CANT_SCHED;
        return CN_EVENT_ID_INVALID;
    }
    pl_evtt =&g_ptEvttTable[evtt_offset];
    if((pl_evtt->property.registered == 0)      //类型未登记
        ||(pl_evtt->property.deleting == 1))    //事件类型正在等待注销
    {
        Djy_SaveLastError(EN_KNL_EVTT_UNREGISTER);
        if(pop_result != NULL)
            *pop_result = (u32)EN_KNL_EVTT_UNREGISTER;
        return CN_EVENT_ID_INVALID;
    }else if(prio >= CN_PRIO_SYS_SERVICE)       //非法优先级
    {
        Djy_SaveLastError(EN_KNL_INVALID_PRIO);
        if(pop_result != NULL)
            *pop_result = (u32)EN_KNL_INVALID_PRIO;
        return CN_EVENT_ID_INVALID;
    }

//    malloc_para = para_options && CN_MALLOC_OVER_32;
    Int_SaveAsynSignal();                     //关异步信号(关调度)
    //下面处理事件类型弹出同步，参看Djy_WaitEvttPop的说明
    //弹出同步中的事件，处理方式如下:
    //1、如果是关联型事件，则激活所有sync_counter减至0的事件。
    //2、如果是独立型事件，则要分两种情况:
    //  a、如果hybrid_id是事件id，则不激活同类型的其他事件。
    //  b、如果hybrid_id是事件类型id，则激活所有sync_counter减至0的事件。
    pl_ecb = pl_evtt->pop_sync;
    if(pl_ecb != NULL)
    {
        do
        {
            //sync_counter减量的条件:
            //1、关联型事件，或者:
            //2、独立型事件，hybrid_id是事件id，且等于pl_ecb->event_id，或者:
            //3、独立型事件，且类型和pl_ecb->evtt_id相同
            if( (pl_evtt->property.correlative == EN_CORRELATIVE)
                    || (pl_ecb->event_id == hybrid_id)
                    || ((evtt_offset|CN_EVTT_ID_MASK) != pl_ecb->evtt_id) )
            {
                //同步条件达成的条件: 1、同步计数器已减至1。
                if(pl_ecb->sync_counter <= 1)
                {
                    pl_ecb->sync_head = NULL;
                    //指定的超时未到
                    if(pl_ecb->event_status & CN_STS_SYNC_TIMEOUT)
                    {
                        __Djy_ResumeDelay(pl_ecb);     //从闹钟队列中移除事件
                    }
                    pl_ecb->wakeup_from = CN_STS_WAIT_EVTT_POP;
                    pl_ecb->event_status = CN_STS_EVENT_READY;
                    pl_ecb_temp = pl_ecb;
                    if(pl_ecb_temp == pl_ecb_temp->multi_next)  //是最后一个事件
                    {
                        pl_ecb = NULL;
                        pl_evtt->pop_sync = NULL;
                    }else
                    {
                        pl_ecb = pl_ecb->multi_next;
                        pl_ecb_temp->multi_previous->multi_next
                                            = pl_ecb_temp->multi_next;
                        pl_ecb_temp->multi_next->multi_previous
                                            = pl_ecb_temp->multi_previous;
                    }
                    __Djy_EventReady(pl_ecb_temp);
//                  if(pl_evtt->pop_sync != NULL)
//                      pl_evtt = NULL;
                }else
                {
                    pl_ecb->sync_counter--;
                    pl_ecb = pl_ecb->multi_next;
                }
            }
            else
            {
                pl_ecb = pl_ecb->multi_next;
            }
        }while(pl_ecb != pl_evtt->pop_sync);
    }

    //需要创建新事件的情况为两者之一:
    //1、事件队列中尚无该类型事件。
    //2、独立型事件且hybrid_id是事件类型id
    if((pl_evtt->property.inuse == 0)
                || ((pl_evtt->property.correlative == EN_INDEPENDENCE)
                    && (hybrid_id & CN_EVTT_ID_MASK)) )
    {
        if(s_ptEventFree==NULL)            //没有空闲的事件控制块
        {
            parahead.DecoderName = NULL;
            parahead.ExpAction = EN_EXP_DEAL_RECORD;
            itoa(hybrid_id,ExpStr,16);
            strcat(ExpStr,": 分配不到事件控制块");
            parahead.ExpInfo = (u8*)ExpStr;
            parahead.ExpInfoLen = sizeof(ExpStr);
            parahead.ExpType = CN_EXP_TYPE_ECB_EXHAUSTED;
            Exp_Throw(&parahead);
            Djy_SaveLastError(EN_KNL_ECB_EXHAUSTED);
            if(pop_result != NULL)
                *pop_result = (u32)EN_KNL_ECB_EXHAUSTED;
            return_result = CN_EVENT_ID_INVALID;
            goto end_pop;
        }else if((pl_evtt->property.correlative == EN_INDEPENDENCE)
                && ((pl_evtt->vpus)> pl_evtt->vpus_limit))
        {
            Djy_SaveLastError(EN_KNL_VPU_OVER);
            return_result = CN_EVENT_ID_INVALID;
            if(pop_result != NULL)
                *pop_result = (u32)EN_KNL_VPU_OVER;
            return_result = CN_EVENT_ID_INVALID;
            goto end_pop;
        }else                       //有空闲事件控制块
        {
            pl_ecb = s_ptEventFree;         //从空闲链表中提取一个事件控制块
            s_ptEventFree=s_ptEventFree->next;  //空闲事件控制块数量减1
            pl_evtt->events++;
            //设置新事件的事件控制块
            pl_ecb->next = NULL;
            pl_ecb->previous = NULL;
            pl_ecb->multi_next = NULL;
            pl_ecb->multi_previous = NULL;
            pl_ecb->vm = NULL;
            pl_ecb->param1 = PopPrarm1;
            pl_ecb->param2 = PopPrarm2;
            pl_ecb->sync = NULL;
            pl_ecb->sync_head = NULL;

            pl_ecb->EventStartTime = DjyGetSysTime();   //事件发生时间
            pl_ecb->consumed_time = 0;
//#if(CN_CFG_DEBUG_INFO == 1)
            pl_ecb->consumed_time_second = 0;
            pl_ecb->consumed_time_record = 0;
//#endif
            pl_ecb->delay_start_tick = 0;
            pl_ecb->delay_end_tick = 0;
            pl_ecb->error_no = 0;
            pl_ecb->wait_mem_size = 0;
            pl_ecb->wakeup_from = CN_WF_EVENT_NORUN;
            pl_ecb->event_status = CN_STS_EVENT_READY;
            pl_ecb->evtt_id = evtt_offset | CN_EVTT_ID_MASK;    //设置事件类型
            pl_ecb->sync_counter = 0;
            pl_ecb->local_memory = 0;
            pl_evtt->property.inuse = 1;
            if(pl_evtt->property.correlative == EN_CORRELATIVE)
            {
                pl_evtt->mark_event = pl_ecb;
            }
            else
            {

            }
            if(prio != 0)
            {
                pl_ecb->prio = prio;                    //设置事件优先级,
                pl_ecb->prio_base = pl_ecb->prio;       //设置事件优先级,
            }else
            {
                pl_ecb->prio =pl_evtt->default_prio;    //从事件类型中继承优先级
                pl_ecb->prio_base = pl_ecb->prio;       //设置事件优先级,
            }
            __Djy_EventReady(pl_ecb);
            return_result = pl_ecb->event_id;
        }
    }
    else  //关联型事件，且队列中已经有该类型事件 或 独立型且hybrid_id是事件id
    {
        if(pl_evtt->property.correlative == EN_INDEPENDENCE)
        {
            //此时hybrid_id一定是事件ID,而mark_event并不指向相应事件
            pl_ecb = &g_ptECB_Table[hybrid_id];
        }
        else
        {
            pl_ecb = pl_evtt->mark_event;
        }
        pl_ecb->param1 = PopPrarm1;     //参数替换
        pl_ecb->param2 = PopPrarm2;
        return_result = pl_ecb->event_id;

    }
    //设定了超时时间，将重新调度
    //否则进入事件同步队列，事件处理函数自然返回或异常退出才解除阻塞
    pl_evtt->pop_times++;
    if(timeout != 0)
    {
        __Djy_AddRunningToBlock(&pl_ecb->sync, CN_BLOCK_FIFO, timeout, CN_STS_WAIT_EVENT_DONE);
        Int_RestoreAsynSignal();  //恢复中断会引发重新调度

        //注:事件处理结果在djy_task_completed函数中给event_result赋值
        //检查从哪里返回，是超时还是同步事件完成。
        if(g_ptEventRunning->wakeup_from & CN_STS_SYNC_TIMEOUT)
        {//说明同步条件未到，从超时返回。
            if(pop_result != NULL)
                *pop_result = (u32)CN_SYNC_TIMEOUT;
        }else if(g_ptEventRunning->wakeup_from & CN_STS_EVENT_EXP_EXIT)
        {//被同步事件被异常终止，可能没有被正确执行
            if(pop_result != NULL)
                *pop_result = (u32)CN_STS_EVENT_EXP_EXIT;
        }else
        {
            if(pop_result != NULL)
                *pop_result = (u32)CN_SYNC_SUCCESS;
        }
    }
end_pop:
    Int_RestoreAsynSignal();  //恢复中断状态
    return return_result;
}

//----取事件类型弹出次数-------------------------------------------------------
//功能: 取当前某类型事件弹出总次数
//参数: evtt_id，事件类型id
//返回: 事件处理结果。
//-----------------------------------------------------------------------------
u32 Djy_GetEvttPopTimes(u16 evtt_id)
{
    return g_ptEvttTable[evtt_id &(~CN_EVTT_ID_MASK)].pop_times;
}

//----取事件处理结果-----------------------------------------------------------
//功能: 一个事件在处理过程中，如果弹出了新事件，并且等待事件处理结果(调用
//      djy_event_pop函数是sync=true)。如果调用djy_event_pop时返回了合法的事件
//      id，且不是超时返回，则可以用本函数获取新事件的处理结果。只能取最后一次
//      成功处理的事件结果。
//参数: 无
//返回: 事件处理结果。
//-----------------------------------------------------------------------------
ptu32_t Djy_GetEventResult(void)
{
    return g_ptEventRunning->event_result;
}

//----提取事件参数-------------------------------------------------------------
//功能: 提取处理中事件的参数，规则顺序:1、高优先级的队列非空，取第一个
//      并把它从队列中删除。2、ppara指针非空,取其值并清空。3、低优先级队列非空，
//      取第一个并从队列删除
//参数: time，如果非NULL，则返回所获取的参数进入队列的时间，uS数
//返回: 事件参数指针
//注: 本函数只能取正在处理中的事件自己的参数
//-----------------------------------------------------------------------------
void Djy_GetEventPara(ptu32_t *Param1,ptu32_t *Param2)
{
    if(Param1 != NULL)
        *Param1 = g_ptEventRunning->param1;
    if(Param2 != NULL)
        *Param2 = g_ptEventRunning->param2;
}

//----取自身的事件类型id-------------------------------------------------------
//功能: 由应用程序调用，取正在处理的事件的事件类型id
//参数: 无
//返回: 事件类型id，如果当前运行在异步信号中断中，则返回 CN_EVTT_ID_ASYN
//-----------------------------------------------------------------------------
u16 Djy_MyEvttId(void)
{
    if(Int_GetRunLevel() >0)
    {
        return CN_EVTT_ID_ASYN;
    }
    else
    {
        return g_ptEventRunning->evtt_id;
    }
}

//----取自身的事件id-------------------------------------------------------
//功能: 由应用程序调用，取正在处理的事件的事件id
//参数: 无
//返回: 事件id，如果当前运行在异步信号中断中，则返回 CN_EVENT_ID_ASYN
//-----------------------------------------------------------------------------
u16 Djy_MyEventId(void)
{
    if(Int_GetRunLevel() >0)
    {
        return CN_EVENT_ID_ASYN;
    }
    else
    {
        return g_ptEventRunning->event_id;
    }
}

//----启动操作系统--------------------------------------------------------------
//功能: 初始化时钟嘀嗒，复位异步信号使能状态，选出最高优先级事件，调用
//      __asm_turnto_context把上下文切换到这个事件
//参数: 无
//返回: 无
//_____________________________________________________________________________
void __Djy_StartOs(void)
{
    //本句为容错性质，以防用户模块初始化过程中没有成对调用异步信号使能与禁止函数
    __Int_ResetAsynSignal();
    __DjyInitTick();
    __Djy_SelectEventToRun();
    g_ptEventRunning = g_ptEventReady;
    g_bScheduleEnable = true;
    g_bMultiEventStarted = true;
    Int_HalfEnableAsynSignal( );
    __asm_start_thread(g_ptEventRunning->vm);
}

#define CN_KEEP         0   //线程保留不删除
#define CN_DELETE       1   //线程应该被删除
#define CN_DELETED      2   //线程已经被删除

//----激活事件的同步队列---------------------------------------------------
//功能: 事件处理结束时，激活该事件同步队列中的所有事件，返回
//      给同步中的事件的"执行结果"被置为0，被同步事件异常返回位被置位
//参数: para，参数队列的队列头，参数构成一个双向链表。
//返回: 无
//-----------------------------------------------------------------------------
void __Djy_ActiveEventSyncExp(struct EventECB *sync)
{
    struct EventECB *event_temp;
    if(sync == NULL)
        return ;

    sync->multi_previous->multi_next = NULL;
    do{
        sync->event_result = 0;
        sync->sync_head = NULL;
        //是否在超时队列中
        if(sync->event_status & CN_STS_SYNC_TIMEOUT)
        {
            __Djy_ResumeDelay(sync);                   //结束超时等待
        }
        sync->event_status = CN_STS_EVENT_READY;        //取消"同步中"状态
        sync->wakeup_from = CN_STS_EVENT_EXP_EXIT;      //置位异常结束标志
        event_temp = sync->multi_next;
        __Djy_EventReady(sync);           //把事件加入到就绪队列中
        sync = event_temp;
    }while(sync != NULL);
    return;
}

//----异常激活事件参数的同步队列-----------------------------------------------
//功能: 事件异常结束时，激活一个事件所有参数的同步队列中的所有事件，并释放参数控
//      制块.返回给同步中的事件的"执行结果"被置为0，被同步事件异常返回位被置位
//参数: para，参数队列的队列头，参数构成一个双向链表。
//返回: 无
//-----------------------------------------------------------------------------
//void __Djy_ActiveEventParaSyncExp(struct ParaPCB *event_para)
//{
//    struct ParaPCB *para_temp;
//    struct EventECB *pl_ecb,*event_temp;
//    if(event_para == NULL)
//        return ;
//
//    event_para->previous->next = NULL;
//    do{
//        pl_ecb = event_para->sync;
//        if(pl_ecb != NULL)
//        {
//            pl_ecb->multi_previous->multi_next = NULL;
//            do{
//                pl_ecb->event_result = 0;
//                pl_ecb->sync_head = NULL;
//                //是否在超时队列中
//                if(pl_ecb->event_status & CN_STS_SYNC_TIMEOUT)
//                {
//                    __Djy_ResumeDelay(pl_ecb);               //结束超时等待
//                }
//                pl_ecb->event_status = CN_STS_EVENT_READY;    //取消"同步中"状态
//                pl_ecb->wakeup_from = CN_STS_EVENT_EXP_EXIT;  //置位异常结束标志
//                event_temp = pl_ecb->multi_next;
//                __Djy_EventReady(pl_ecb);           //把事件加入到就绪队列中
//                pl_ecb = event_temp;
//            }while(pl_ecb != NULL);
//        }
//
//        if(event_para->dynamic_mem == true)
//        {
//            free(event_para->event_para);
//        }
//        para_temp = event_para;
//        event_para = event_para->next;                  //取队列中下一个事件
//        para_temp->sync = NULL;
//        para_temp->next = s_ptParaFree;     //释放参数控制块
//        s_ptParaFree = para_temp;
//    }while(event_para != NULL);
//    return;
//}

//----从同步队列中移除该ECB-----------------------------------------------
//功能: //此函数不做任何的检查，内部调用，确认此ecb在sync队列中
//返回: 无
//      该同步队列是多功能指针的双向循环队列
//-----------------------------------------------------------------------------
void __Djy_CutEcbFromSync(struct EventECB  *event)
{
    struct EventECB  **syn_head;

    syn_head = event->sync_head;
    if(NULL == syn_head)
    {
        return;
    }
    if(event->multi_next == event)
    {//是最后一个事件
        *syn_head  = NULL;
    }else
    {
        if(*syn_head == event)  //被移除的是队列头
            *syn_head = event->multi_next;
        event->multi_next->multi_previous = event->multi_previous;
        event->multi_previous->multi_next = event->multi_next;
        event->multi_next = NULL;
        event->multi_previous = NULL;
    }
    event->sync_head = NULL;
}

//----从同步队列中移除该ECB-----------------------------------------------
//功能: 将event 从ECB对应的EVTT的marked队列中删除，应该确认event在marked队列中
//返回: 无
//      该队列是前后指针的双向循环队列,顺序是按照优先级高低顺序
//      仅仅是功能性代码，不做安全检查
//-----------------------------------------------------------------------------
void __Djy_CutEventFromEvttMarked(struct EventECB *event)
{
    struct EventType  *pl_evtt;

    pl_evtt =& g_ptEvttTable[event->evtt_id &(~CN_EVTT_ID_MASK)];
    if(event == event->previous)//maked队列中只有一个
    {
        pl_evtt->mark_event = NULL;
    }
    else//从marked队里中移除
    {
        if(event == pl_evtt->mark_event)//队列头，其实一般的都是从头移走
        {
            pl_evtt->mark_event = event->next;
        }
        event->previous->next = event->next;
        event->next->previous = event->previous;
    }
}

//----事件逸出-----------------------------------------------------------------
//功能: 事件处理函数异常返回,一般在看门狗等监视机制监察到系统错误时，做善后处理，
//      或删除事件，并结束线程运行，或复位线程重新开始运行并把线程重新初始化。
//参数: event_ECB，目标事件
//      exit_code，出错码
//      action，出错后的动作
//返回: 本函数不返回
//todo: 未完成
//-----------------------------------------------------------------------------
void Djy_EventExit(struct EventECB *event, u32 exit_code,u32 action)
{
    struct ThreadVm *next_vm,*temp;
    struct EventECB *pl_ecb;
    struct EventType *pl_evtt;
    struct EventECB *pl_ecb_temp;
    struct ExpThrowPara  parahead;
    char ExpStr[32];
    ucpu_t  vm_final = CN_DELETE;

    parahead.DecoderName = NULL;
    parahead.ExpAction = EN_EXP_DEAL_RECORD;
    itoa(event->event_id,ExpStr,16);
    strcat(ExpStr,"事件处理意外结束");
    parahead.ExpInfo = (u8*)ExpStr;
    parahead.ExpInfoLen = sizeof(ExpStr);
    parahead.ExpType = CN_EXP_TYPE_EVENT_EXIT;
    Exp_Throw(&parahead);
    //此处不用int_save_asyn_signal函数，可以在应用程序有bug，没有成对调用
    //int_save_asyn_signal和int_restore_asyn_signal的情况下，确保错误到此为止。
    __Int_ResetAsynSignal();  //直到__vm_engine函数才再次打开.

    //激活事件同步队列中的所有事件
    __Djy_ActiveEventSyncExp(g_ptEventRunning->sync);

//    if(g_ptEventRunning->para_current != NULL)
//        __Djy_ActiveEventSyncExp(g_ptEventRunning->para_current->sync);
    //以下激活参数的同步队列中的事件,并释放参数队列中的全部参数
//  __Djy_ActiveEventParaSyncExp(g_ptEventRunning->para_high_prio);
//  __Djy_ActiveEventParaSyncExp(g_ptEventRunning->para_low_prio);

#if (CN_CFG_DYNAMIC_MEM == 1)
    if(g_ptEventRunning->local_memory != 0)
    {
        __M_CleanUp(g_ptEventRunning->event_id);    //
    }
#endif
    __Djy_CutReadyEvent(g_ptEventRunning);
    g_ptEventRunning->previous
                    = (struct EventECB*)&s_ptEventFree;//表示本控制块空闲
    g_ptEventRunning->next = s_ptEventFree;     //pg_event_free是单向非循环队列
    g_ptEventRunning->evtt_id = CN_EVTT_ID_INVALID;     //todo
    s_ptEventFree = g_ptEventRunning;
    s_ptEventFree->event_id = s_ptEventFree - g_ptECB_Table;   //容错用

    //下面处理完成同步队列，必须联系共享文档的相关章节才容易读懂，注释难于解释
    //那么清楚的。
    pl_evtt =&g_ptEvttTable[g_ptEventRunning->evtt_id & (~CN_EVTT_ID_MASK)];
    pl_evtt->events--;
    pl_ecb = pl_evtt->done_sync;
    if(pl_ecb != NULL)
    {
        do
        {//链表中的事件都是要么没有指定超时，要么时限未到，其他情况不会在此链表
         //中留下痕迹，evtt_done_sync位也已经清除

            //同步条件达成的条件: 1、同步计数器已减至1。
            //2、同步计数器为0且本类型最后一条事件已经处理完
            if((pl_ecb->sync_counter == 1)
                    ||((pl_ecb->sync_counter == 0) && (pl_evtt->vpus == 1)) )
            {
                pl_ecb->sync_head = NULL;
                if(pl_ecb->event_status & CN_STS_SYNC_TIMEOUT)//指定的超时未到
                {
                    __Djy_ResumeDelay(pl_ecb);       //从闹钟队列中移除事件
                }
                pl_ecb->event_status = CN_STS_EVENT_READY;
                pl_ecb->wakeup_from = CN_STS_EVENT_EXP_EXIT;
                pl_ecb_temp = pl_ecb;
                if(pl_ecb_temp == pl_ecb_temp->multi_next)    //是最后一个事件
                {
                    pl_ecb = NULL;
                    pl_evtt->done_sync = NULL;
                }else
                {
                    pl_ecb = pl_ecb->multi_next;
                    pl_ecb_temp->multi_previous->multi_next
                                        = pl_ecb_temp->multi_next;
                    pl_ecb_temp->multi_next->multi_previous
                                        = pl_ecb_temp->multi_previous;
                }
                __Djy_EventReady(pl_ecb_temp);
            }else
            {
                if(pl_ecb->sync_counter != 0)
                    pl_ecb->sync_counter--;
                pl_ecb = pl_ecb->multi_next;
            }
        }while(pl_ecb != pl_evtt->done_sync);
    }

    //线程和事件控制块的处理方式
    //1、若线程数量小于等于vpus_res，则保留.
    //2、其他情况，销毁线程
    //3、事件控制块则直接收回

    if(pl_evtt->property.correlative == EN_CORRELATIVE)      //关联型事件
    {
        pl_evtt->property.inuse = 0;
        if(pl_evtt->default_prio<0x80)
        {//运行模式为si或dlsp，或者该事件类型拥有紧急优先级时，
         //须保留线程
            vm_final = CN_KEEP;
        }else
        {
            vm_final = CN_DELETE;
        }
    }else   //独立型事件
    {
        //有未得到线程的事件，保留之
        if(pl_evtt->events >= pl_evtt->vpus)
        {
            vm_final = CN_KEEP;
        }else   //没有未得到线程的事件，再看是否需保留
        {
            if(pl_evtt->vpus<=pl_evtt->vpus_res)//该类型事件拥有的线程数已经低于
                                                    //或等于最低保留量，保留之
            {
                vm_final = CN_KEEP;
            }else                                   //可能删除线程
            {
                if(pl_evtt->vpus == 1)   //这是最后一个事件
                {
                    pl_evtt->property.inuse = 0;
                    if(pl_evtt->default_prio<0x80)
                    {
                 //运行模式为si或dlsp，或该事件类型拥有紧急优先级时，须保留线程
                        vm_final = CN_KEEP;
                    }else
                    {
                        vm_final = CN_DELETE;
                    }
                }else
                {
                    vm_final = CN_DELETE;
                }
            }
        }
    }

//看事件类型控制块是否要删除，用户注销一个事件类型，如果该事件类型正在使用，则不
//会立即注销，只是不能弹出新事件，要等属于该事件类型的所有事件处理完才能注销。
    if((pl_evtt->property.inuse == 0) && (pl_evtt->property.deleting == 1))
    {
        next_vm = pl_evtt->my_free_vm;
        //回收事件类型控制块，只需把registered属性清零。
        pl_evtt->property.registered = 0;
        pl_evtt->evtt_name[0] = '\0';   //清空事件类型名
        while(next_vm != NULL)          //释放该事件类型拥有的空闲线程
        {
            temp = next_vm;
            next_vm = next_vm->next;
            free((void *)temp);
        }
        vm_final = CN_DELETED;          //线程已经被删除
    }

    __Djy_SelectEventToRun();
    if(vm_final == CN_DELETE)                   //删除线程
    {
        free((void*)g_ptEventRunning->vm);    //删除线程
        pl_evtt->vpus--;
        g_ptEventRunning = g_ptEventReady;
        g_ptEvttTable[g_ptEventRunning->evtt_id & (~CN_EVTT_ID_MASK)].SchHook(EN_SWITCH_IN);
        g_s64RunningStartTime = DjyGetSysTime();
        Int_HalfEnableAsynSignal( );
        __asm_turnto_context(g_ptEventRunning->vm);
    }else if(vm_final == CN_KEEP)    //保留线程
    {
        //本事件类型有等待的VM的ECB,将vm转交给响应队列中的ECB就可以。
        if((pl_evtt->property.correlative == EN_INDEPENDENCE)&&\
                (pl_evtt->mark_event != NULL))
        {
            pl_ecb = pl_evtt->mark_event;
            if(pl_evtt->mark_event->previous == pl_evtt->mark_event)//只有一个
            {
                pl_evtt->mark_event = NULL;
            }
            else
            {
                pl_ecb->previous->next = pl_ecb->next;
                pl_ecb->next->previous = pl_ecb->previous;
                pl_evtt->mark_event = pl_ecb->next;
            }
            //从同步队列中移除,可以肯定的是内存同步队列
            if(NULL != pl_ecb->sync_head)
            {
                __Djy_CutEcbFromSync(pl_ecb);
            }
            //接受旧线程的VM
            pl_ecb->vm = g_ptEventRunning->vm;
            __Djy_EventReady(pl_ecb);
            pl_ecb = g_ptEventRunning;
            pl_evtt->SchHook(EN_SWITCH_OUT);

            g_ptEventRunning=g_ptEventReady;
            g_ptEvttTable[g_ptEventRunning->evtt_id & (~CN_EVTT_ID_MASK)].SchHook(EN_SWITCH_IN);
            g_s64RunningStartTime = DjyGetSysTime();
            //下面一句很关键，running和pl_ecb可能是同一个，所以一定要先reset，然
            //后switch
            Int_HalfEnableAsynSignal( );
            __asm_reset_switch(pl_evtt->thread_routine,
                            g_ptEventRunning->vm,pl_ecb->vm);
        }
        else//这种情况下还是放在free队列吧
        {
            g_ptEventRunning->vm->next = pl_evtt->my_free_vm;
            pl_evtt->my_free_vm = g_ptEventRunning->vm;
            pl_ecb = g_ptEventRunning;
            pl_evtt->SchHook(EN_SWITCH_OUT);

            g_ptEventRunning=g_ptEventReady;
            g_ptEvttTable[g_ptEventRunning->evtt_id & (~CN_EVTT_ID_MASK)].SchHook(EN_SWITCH_IN);
            g_s64RunningStartTime = DjyGetSysTime();
            Int_HalfEnableAsynSignal( );
            __asm_reset_switch(pl_evtt->thread_routine,
                            g_ptEventRunning->vm,pl_ecb->vm);
        }
    }else               //线程已经被删除
    {
//        pl_ecb = g_ptEventRunning;
        g_ptEventRunning = g_ptEventReady;
        g_ptEvttTable[g_ptEventRunning->evtt_id & (~CN_EVTT_ID_MASK)].SchHook(EN_SWITCH_IN);
        g_s64RunningStartTime = DjyGetSysTime();
        Int_HalfEnableAsynSignal( );
        __asm_turnto_context(g_ptEventRunning->vm);
    }
}

//----事件阶段性完成----------------------------------------------------------
//功能：处理完成一次EventPop,但事件未终止时,调用本函数表示该次EventPop已经完成,
//      将激活事件同步队列中的事件.一般用于调用Djy_WaitEvttPop且返回
//      CN_SYNC_SUCCESS后,用于通知调用Djy_EventPop的事件"该次弹出已经被处理".
//      如果该次Djy_EventPop设定了同步(即timeout !=0),则同步条件达成。
//
//      在以下情况下,本函数可能有歧义:
//      A事件调用Djy_EventPop弹出C事件,timeout = 0,因C被阻塞,未切换到C.
//      B事件调用Djy_EventPop弹出C事件,timeout = 无穷,切换到C.
//      C 调用Djy_EventSessionComplete,B也将激活.
//      此种情况下,不能认为C该调用两次Djy_EventSessionComplete分别对应两
//      次弹出,第二次弹出时才能激活B.因为,关联型事件所完成的,总是最后一次
//      弹出,因为前后弹出是有关联的.
//
//参数：result，事件处理结果，这个结果将返回给弹出该事件的事件(如果设定了同步)
//返回：无
//-----------------------------------------------------------------------------
void Djy_EventSessionComplete(ptu32_t result)
{
    struct EventECB *pl_ecb,*event_temp;
    Int_SaveAsynSignal();
    pl_ecb = g_ptEventRunning->sync;     //取同步队列头
    if(pl_ecb != NULL)
    {
        pl_ecb->multi_previous->multi_next = NULL;
        do{
            pl_ecb->sync_head = NULL;
            pl_ecb->event_result = result;
            if(pl_ecb->event_status & CN_STS_SYNC_TIMEOUT)  //是否在超时队列中
            {
                __Djy_ResumeDelay(pl_ecb);                 //结束超时等待
            }
            pl_ecb->event_status = CN_STS_EVENT_READY;
            pl_ecb->wakeup_from = CN_STS_WAIT_EVENT_DONE;
            event_temp = pl_ecb->multi_next;
            __Djy_EventReady(pl_ecb);           //把事件加入到就绪队列中
            pl_ecb = event_temp;
        }while(pl_ecb != NULL);
    }
    g_ptEventRunning->sync = NULL;
    Int_RestoreAsynSignal();

}

//----事件完成-----------------------------------------------------------------
//功能：事件处理函数自然返回,完成清理工作.
//      1.如果事件同步队列非空，把同步事件放到ready表。
//      2.处理事件类型完成同步队列
//      3.如果未释放的动态分配内存，释放之。
//      4.把事件控制块各分量设置到初始态,并放入free表。
//  如果是线程需保留，则:
//      5.复位线程。
//      6.不保存上下文，直接切换到新的ready表头
//  如果线程无需保留
//      5.释放线程内存。
//      6.不保存上下文，直接切换到新的ready表头
//参数：result，事件处理结果，这个结果将返回给弹出该事件的事件(如果设定了同步)
//返回：无
//备注: 调用本函数的必定是running事件,在running事件上下文中执行，不可以调用
//      __asm_reset_thread函数。
//-----------------------------------------------------------------------------
void Djy_EventComplete(ptu32_t result)
{
    struct ThreadVm *next_vm,*temp;
    struct EventECB *pl_ecb,*event_temp;
    struct EventType   *pl_evtt;
    struct EventECB *pl_ecb_temp;
    ucpu_t  vm_final = CN_DELETE;

    pl_evtt =&g_ptEvttTable[g_ptEventRunning->evtt_id & (~CN_EVTT_ID_MASK)];
    __Int_ResetAsynSignal();  //直到__vm_engine函数才再次打开.
    //下面处理同步队列，必须联系共享文档的相关章节才容易读懂，注释难于解释
    //那么清楚的。
    pl_ecb = g_ptEventRunning->sync;     //取同步队列头
    if(pl_ecb != NULL)
    {
        pl_ecb->multi_previous->multi_next = NULL;
        do{
            pl_ecb->sync_head = NULL;
            pl_ecb->event_result = result;
            if(pl_ecb->event_status & CN_STS_SYNC_TIMEOUT)  //是否在超时队列中
            {
                __Djy_ResumeDelay(pl_ecb);                 //结束超时等待
            }
            pl_ecb->event_status = CN_STS_EVENT_READY;
            pl_ecb->wakeup_from = CN_STS_WAIT_EVENT_DONE;
            event_temp = pl_ecb->multi_next;
            __Djy_EventReady(pl_ecb);           //把事件加入到就绪队列中
            pl_ecb = event_temp;
        }while(pl_ecb != NULL);
    }

    //下面处理完成同步队列，必须联系共享文档的相关章节才容易读懂，注释难于解释
    //那么清楚的。
    pl_ecb = pl_evtt->done_sync;
    while(pl_ecb != NULL)
    {
        do
        {//链表中的事件都是要么没有指定超时，要么时限未到，其他情况不会在此链表
         //中留下痕迹，evtt_done_sync位也已经清除

            //同步条件达成的条件: 1、同步计数器已减至1。
            //2、同步计数器为0且本类型最后一条事件已经处理完
         if((pl_ecb->sync_counter == 1)
             ||((pl_ecb->sync_counter == 0) && (pl_evtt->vpus == 1)) )
            {
                pl_ecb->sync_head = NULL;
                if(pl_ecb->event_status & CN_STS_SYNC_TIMEOUT) //指定的超时未到
                {
                    __Djy_ResumeDelay(pl_ecb);       //从闹钟队列中移除事件
                }
                pl_ecb->event_status = CN_STS_EVENT_READY;
                pl_ecb->wakeup_from = CN_STS_WAIT_EVTT_DONE;
                pl_ecb_temp = pl_ecb;
                if(pl_ecb_temp == pl_ecb_temp->multi_next)    //是最后一个事件
                {
                    pl_ecb = NULL;
                    pl_evtt->done_sync = NULL;
                }else
                {
                    pl_ecb = pl_ecb->multi_next;
                    pl_ecb_temp->multi_previous->multi_next
                                        = pl_ecb_temp->multi_next;
                    pl_ecb_temp->multi_next->multi_previous
                                        = pl_ecb_temp->multi_previous;
                }
                __Djy_EventReady(pl_ecb_temp);
            }else
            {
                if(pl_ecb->sync_counter != 0)
                    pl_ecb->sync_counter--;
                pl_ecb = pl_ecb->multi_next;
            }
        }while(pl_ecb != pl_evtt->done_sync);
    }

    //以下看事件的线程如何处理。
#if (CN_CFG_DYNAMIC_MEM == 1)
    if(g_ptEventRunning->local_memory != 0)
    {
        __M_CleanUp(g_ptEventRunning->event_id);//强行清除事件运行中申请的内存
    }
#endif
    __Djy_CutReadyEvent(g_ptEventRunning);
    g_ptEventRunning->previous
                    = (struct EventECB*)&s_ptEventFree;//表示本控制块空闲
    g_ptEventRunning->next = s_ptEventFree;     //pg_event_free是单向非循环队列
    g_ptEventRunning->evtt_id = CN_EVTT_ID_INVALID;     //todo
    s_ptEventFree = g_ptEventRunning;
    s_ptEventFree->event_id = s_ptEventFree - g_ptECB_Table;   //容错用

    pl_evtt->events--;

    if(pl_evtt->property.correlative == EN_CORRELATIVE)      //关联型事件
    {
        pl_evtt->property.inuse = 0;
        if(pl_evtt->default_prio<0x80)
        {//运行模式为si或dlsp，或者该事件类型拥有紧急优先级时，
         //须保留线程
            vm_final = CN_KEEP;
        }else
        {
            vm_final = CN_DELETE;
        }
    }else   //独立型事件
    {
        //有未得到线程的事件，保留之
        if(pl_evtt->events >= pl_evtt->vpus)
        {
            vm_final = CN_KEEP;
        }else   //没有未得到线程的事件，再看是否需保留
        {
            if(pl_evtt->vpus<=pl_evtt->vpus_res)//该类型事件拥有的线程数已经低于
                                                    //或等于最低保留量，保留之
            {
                vm_final = CN_KEEP;
            }else                                   //可能删除线程
            {
                if(pl_evtt->vpus == 1)   //这是最后一个事件
                {
                    pl_evtt->property.inuse = 0;
                    if(pl_evtt->default_prio<0x80)
                    {
                 //运行模式为si或dlsp，或该事件类型拥有紧急优先级时，须保留线程
                        vm_final = CN_KEEP;
                    }else
                    {
                        vm_final = CN_DELETE;
                    }
                }else
                {
                    vm_final = CN_DELETE;
                }
            }
        }
    }

    if((pl_evtt->property.inuse == 0) && (pl_evtt->property.deleting == 1))
    {
        next_vm = pl_evtt->my_free_vm;
        //回收事件类型控制块，只需把registered属性清零。
        pl_evtt->property.registered = 0;
        pl_evtt->evtt_name[0] = '\0';   //清空事件类型名
        while(next_vm != NULL)          //释放该事件类型拥有的空闲线程
        {
            temp = next_vm;
            next_vm = next_vm->next;
            free((void *)temp);
        }
        vm_final = CN_DELETED;
    }

    __Djy_SelectEventToRun();
    if(vm_final == CN_DELETE)      //删除线程
    {
        free((void*)g_ptEventRunning->vm);    //删除线程
        pl_evtt->vpus--;
        g_ptEventRunning = g_ptEventReady;
        g_ptEvttTable[g_ptEventRunning->evtt_id & (~CN_EVTT_ID_MASK)].SchHook(EN_SWITCH_IN);
        g_s64RunningStartTime = DjyGetSysTime();
        Int_HalfEnableAsynSignal( );
        __asm_turnto_context(g_ptEventRunning->vm);
    }else if(vm_final == CN_KEEP)    //保留线程,和exit一样
    {
        //本事件类型有等待的VM的ECB,将vm转交给响应队列中的ECB就可以。
        if((pl_evtt->property.correlative == EN_INDEPENDENCE)&&\
                (pl_evtt->mark_event != NULL))
        {
            pl_ecb = pl_evtt->mark_event;
            if(pl_evtt->mark_event->previous == pl_evtt->mark_event)//只有一个
            {
                pl_evtt->mark_event = NULL;
            }
            else
            {
                pl_ecb->previous->next = pl_ecb->next;
                pl_ecb->next->previous = pl_ecb->previous;
                pl_evtt->mark_event = pl_ecb->next;
            }
            //从同步队列中移除,可以肯定的是内存同步队列
            if(NULL != pl_ecb->sync_head)
            {
                __Djy_CutEcbFromSync(pl_ecb);
            }
            //接受旧线程的VM
            pl_ecb->vm = g_ptEventRunning->vm;
            __Djy_EventReady(pl_ecb);
            pl_ecb = g_ptEventRunning;
            pl_evtt->SchHook(EN_SWITCH_OUT);
            g_ptEventRunning = g_ptEventReady;
            g_ptEvttTable[g_ptEventRunning->evtt_id & (~CN_EVTT_ID_MASK)].SchHook(EN_SWITCH_IN);
            g_s64RunningStartTime = DjyGetSysTime();
            //下面一句很关键，running和pl_ecb可能是同一个，所以一定要先reset，然
            //后switch
            Int_HalfEnableAsynSignal( );
            __asm_reset_switch(pl_evtt->thread_routine,
                            g_ptEventRunning->vm,pl_ecb->vm);
        }
        else//这种情况下还是放在free队列吧
        {
            g_ptEventRunning->vm->next = pl_evtt->my_free_vm;
            pl_evtt->my_free_vm = g_ptEventRunning->vm;
            pl_ecb = g_ptEventRunning;
            pl_evtt->SchHook(EN_SWITCH_OUT);
            g_ptEventRunning = g_ptEventReady;
            g_ptEvttTable[g_ptEventRunning->evtt_id & (~CN_EVTT_ID_MASK)].SchHook(EN_SWITCH_IN);
            g_s64RunningStartTime = DjyGetSysTime();
            Int_HalfEnableAsynSignal( );
            __asm_reset_switch(pl_evtt->thread_routine,
                            g_ptEventRunning->vm,pl_ecb->vm);
        }
    }else               //线程已经被删除
    {
//        pl_ecb = g_ptEventRunning;
        g_ptEventRunning = g_ptEventReady;
        g_ptEvttTable[g_ptEventRunning->evtt_id & (~CN_EVTT_ID_MASK)].SchHook(EN_SWITCH_IN);
        g_s64RunningStartTime = DjyGetSysTime();
        Int_HalfEnableAsynSignal( );
        __asm_turnto_context(g_ptEventRunning->vm);
    }
}

//----查询唤醒原因-------------------------------------------------------------
//功能: 查询正在执行的事件被执行的原因.
//参数: 无
//返回: 唤醒原因,见头文件union event_status定义处注释.
//-----------------------------------------------------------------------------
u32 Djy_WakeUpFrom(void)
{
    return g_ptEventRunning->wakeup_from;
}

//----获取事件信息-------------------------------------------------------------
//功能: 根据事件号获取事件信息
//参数: id,事件id; struct EventInfo *info,存储事件信息，输出参数
//返回: true成功，false失败
//-----------------------------------------------------------------------------
bool_t Djy_GetEventInfo(u16 id, struct EventInfo *info)
{
    struct EventECB *event;
    if(id >=gc_u32CfgEventLimit)
    {
        return false;
    }
    else
    {
        event = &g_ptECB_Table[id];
        info->consumed_time = event->consumed_time;
        info->error_no = event->error_no;
        info->EventStartTime = event->EventStartTime;
        return true;
    }
}
//----获取事件类型名字-----------------------------------------------------------
//功能: 根据事件类型号获取事件类型名字
//参数: evtt_id,事件类型ID,dest,存储空间，len,最大存储长度
//返回: true成功，false失败
//-----------------------------------------------------------------------------
bool_t Djy_GetEvttName(u16 evtt_id, char *dest, u32 len)
{
    struct EventType *pl_evtt;
    u16 evttoffset;
    if((NULL == dest)||(0 ==len))
    {
        return false;
    }
    evttoffset = evtt_id & (~CN_EVTT_ID_MASK);
    if(evttoffset >= gc_u32CfgEvttLimit)
        return false;
    pl_evtt = &g_ptEvttTable[evttoffset];

    if(NULL == pl_evtt->evtt_name)
    {
         dest[0] = '\0';
    }
    else
    {
        strncpy(dest, pl_evtt->evtt_name, len);
    }

    return true;
}
//----线程引擎---------------------------------------------------------------
//功能: 启动线程,执行线程入口函数,事件完成后执行清理工作
//参数: thread_routine 处理该事件类型的函数指针.
//返回: 无
//-----------------------------------------------------------------------------
void __Djy_VmEngine(ptu32_t (*thread_routine)(void))
{
    Djy_EventComplete( thread_routine() );
}

//----系统服务-----------------------------------------------------------------
//功能: 操作系统的系统服务功能,提供调试,统计等服务.
//参数: 无
//返回: 永不返回.
//惨痛教训:这是一个不允许阻塞的函数，深夜3点调出来的教训。
//-----------------------------------------------------------------------------
ptu32_t __Djy_Service(void)
{
    u32 loop;
    while(1)
    {
        for(loop = 0; loop<gc_u32CfgEventLimit; loop++)
        {
            __Djy_CheckStack(loop);
//            {
//                Djy_SaveLastError(EN_KNL_STACK_OVERFLOW);
//              printk("%d 号事件有栈溢出风险,类型=%d Name = %s\n\r",loop,
//                     g_ptEvttTable[loop],g_ptEvttTable[loop].evtt_name);
//            }
        }
        if(g_fnEntryLowPower != NULL)
            g_fnEntryLowPower(g_ptEventRunning->vm);      //进入低功耗状态
    }
    return 0;//消除编译警告
}


//----api启动函数--------------------------------------------------------------
//功能: 根据api号调用相应的api函数.
//参数: api_no,api号
//返回: mp模式才用，暂且放在这里
//-----------------------------------------------------------------------------
void Djy_ApiStart(u32 api_no)
{
    switch(api_no)
    {
        default :break;
    }
    return;
}
//空函数,未初始化的函数指针指向.
void NULL_func(void)
{}

