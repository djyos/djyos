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
// 模块描述: 软件定时器模块
// 模块版本: V1.00
// 创建人员: zhangqf_cyg
// 创建时间: 2:36:18 PM/Apr 1, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 备注:该文件需要提供一个硬件定时器，需要一个不间断走时的64位定时器
#include "config-prj.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#include "os.h"

#include "timer.h"
#include "timer_hard.h"

#define CN_TIMER_ALARMNEVER      CN_LIMIT_SINT64             //挂起时限
#define CN_TIMERSOFT_PRECISION   ((1E+10)/CN_CFG_MCLK)       //软件定时器精度，单位微秒

static bool_t  sbUsingHardTimer = false;
static tagTimerSoft* ptTimerSoftHead = NULL;                 //软定时器队列头
static tagTimerSoft* ptTimerSoftTail = NULL;                 //软件定时器队尾
static tagTimerSoft           *ptTimerSoftMem = NULL;        //动态分配内存
static struct tagMemCellPool  *ptTimerSoftMemPool = NULL;    //内存池头指针。
static struct tagMutexLCB     *ptTimerSoftQSync =NULL;       //API资源竞争锁

//使用的硬件定时器（指定使用硬件定时器）
static ptu32_t sgTimerHardDefault = (ptu32_t)NULL;           //用作定时的32位定时器
//使用线程模拟硬件定时器
enum __EN_TIMERSOFT_CMD
{
    EN_TIMERSOFT_ADD = EN_TIMER_LASTOPCODE+1,
    EN_TIMERSOFT_REMOVE,
};

static struct tagMsgQueue    *ptTimerSoftMsgQ = NULL;
typedef struct
{
    tagTimerSoft*     timer;
    u32               type;
    u32               para;
}tagTimerSoftMsg;
#define CN_TIMERSOFT_MSGLEN  0X10
// =============================================================================
// 函数功能：__TimerSoft_Get64Time
//           获取S64timer的时间
// 输入参数: 无
// 输出参数：time,单位为微秒
// 返回值  ：true 成功  false失败
// 说明    ：内部调用函数,注意该函数是移植敏感函数，S64timer,必须为不停表不被打断的
//           64位定时器，每个系统或者CPU可能用的都不一样
// =============================================================================
bool_t __TimerSoft_Get64Time(s64 *time)
{
    *time = DjyGetTime();
    return true;
}
// =============================================================================
// 函数功能：__TimerSoft_ExeAlarmHandler
//          执行timersoft的钩子函数
// 输入参数：timersoft,待执行的定时器.
// 输出参数：无
// 返回值  ：true 成功  false失败
// 说明    ：内部调用函数，没有HOOK则不执行
// =============================================================================
bool_t __TimerSoft_ExeAlarmHandler(tagTimerSoft *timer)
{
    if(NULL != timer->isr)
    {
        timer->isr(timer);
    }
    return true;
}

// =============================================================================
// 函数功能：__TimerSoft_Remove
//          将指定的定时器从定时器队列中移除
// 输入参数：timer,待移除的定时器
// 输出参数：无
// 返回值  ：调整的定时器
// 说明    ：
// =============================================================================
void __TimerSoft_Remove(tagTimerSoft *timer)
{
    if((timer == ptTimerSoftHead)&&(timer == ptTimerSoftTail)) //only one
    {
        ptTimerSoftHead = NULL;
        ptTimerSoftTail = NULL;
    }
    else if(timer == ptTimerSoftHead)  //head to remove
    {
        ptTimerSoftHead = ptTimerSoftHead->nxt;
        ptTimerSoftHead->pre = NULL;
    }
    else if(timer == ptTimerSoftTail) //tail to remove
    {
        ptTimerSoftTail = ptTimerSoftTail->pre;
        ptTimerSoftTail->nxt = NULL;
    }
    else //normal to remove
    {
        timer->pre->nxt = timer->nxt;
        timer->nxt->pre = timer->pre;
    }
    return;
}
// =============================================================================
// 函数功能：__TimerSoft_Add
//          添加定时器到定时器队列
// 输入参数：timer,待添加的定时器
// 输出参数：无
// 返回值  ：
// 说明    ：
// =============================================================================
void __TimerSoft_Add(tagTimerSoft *timer)
{
    tagTimerSoft *tmp;
    if(NULL == ptTimerSoftHead) //the queue is empty
    {
        ptTimerSoftHead = timer;
        ptTimerSoftTail = timer;
        timer->pre = NULL;
        timer->nxt = NULL;
    }
    else        //find proper place to add
    {
        tmp = ptTimerSoftHead;
        while((NULL != tmp)&&(tmp->deadline < timer->deadline))
        {
            tmp = tmp->nxt;
        }
        if(NULL == tmp) //add the timer to the tail
        {
            timer->pre = ptTimerSoftTail;
            timer->nxt = NULL;
            ptTimerSoftTail->nxt = timer;
            ptTimerSoftTail = timer;
        }
        else
        {
            timer->pre = tmp->pre;
            timer->nxt = tmp;
            if(NULL != tmp->pre)
            {
                tmp->pre->nxt = timer;
            }
            else
            {
                ptTimerSoftHead = timer;
            }
            tmp->pre = timer;
        }
    }
    return;
}

// =============================================================================
// 函数功能：__TimerSoft_AddLast
//          添加定时器到定时器队列尾
// 输入参数：timer,待添加的定时器
// 输出参数：无
// 返回值  ：
// 说明    ：
// =============================================================================
void __TimerSoft_AddLast(tagTimerSoft *timer)
{
    if(NULL == ptTimerSoftHead) //the queue is empty
    {
        ptTimerSoftHead = timer;
        ptTimerSoftTail = timer;
        timer->pre = NULL;
        timer->nxt = NULL;
    }
    else
    {
        timer->pre = ptTimerSoftTail;
        timer->nxt = NULL;
        ptTimerSoftTail->nxt = timer;
        ptTimerSoftTail = timer;
    }
    return;
}

// =============================================================================
// 函数功能：__TimerSoft_ChkTimeout
// 输入参数：无
// 输出参数：无
// 返回值  ：true，该定时器超时，否则该定时器不超时
// 说明     :
// =============================================================================
bool_t  __TimerSoft_ChkTimeout(tagTimerSoft *timer, s64 timenow)
{
    bool_t result = false;
    if((timer->deadline -CN_TIMERSOFT_PRECISION)<timenow)
    {
        result = true;
    }
    return result;
}
// =============================================================================
// 函数功能：__TimerSoft_DealTimeout
//          处理超时的定时器队列
// 输入参数：
// 输出参数：
// 返回值  ：下次等待的时间
// 说明    ：内部调用函数
// =============================================================================
u32 __TimerSoft_DealTimeout(void)
{
    u32           result;
    s64           timenow;
    tagTimerSoft *timer;

    result = CN_TIMEOUT_FOREVER;
    timer = ptTimerSoftHead;
    while(timer) //执行完所有的TIMER ALARM
    {
        __TimerSoft_Get64Time(&timenow);
        if(timer->stat &CN_TIMER_ENCOUNT)
        {
            if(__TimerSoft_ChkTimeout(timer, timenow))
            {
                __TimerSoft_Remove(timer);
                if(timer->stat & CN_TIMER_RELOAD)
                {
                    timer->deadline = timenow + timer->cycle;
                    __TimerSoft_Add(timer);
                }
                else
                {
                    timer->deadline = CN_TIMER_ALARMNEVER;
                    __TimerSoft_AddLast(timer);
                }
                __TimerSoft_ExeAlarmHandler(timer); //execute the hook

                timer = ptTimerSoftHead;
            }
            else
            {
                //compute the wait time
                result = (u32)(timer->deadline - timenow);
                timer = NULL;
            }
        }
        else
        {
            timer = NULL; //跳出循环，证明所有的TIMER都处于PAUSE或者没有定时器状态
        }
    }

    return result;
}
// =============================================================================
// 函数功能：TimerSoft_ISR
//          定时器中断服务HOOK
// 输入参数：irq_no,中断号
// 输出参数：无
// 返回值     ：暂时未定义
// =============================================================================
u32 TimerSoft_ISR(ufast_t irq_no)
{
    u32 waittime;
    //定时器停止计数
    TimerHard_Ctrl(sgTimerHardDefault, EN_TIMER_PAUSECOUNT,(ptu32_t)NULL);

    waittime = __TimerSoft_DealTimeout();
    if(waittime != CN_TIMEOUT_FOREVER)
    {
        TimerHard_Ctrl(sgTimerHardDefault, EN_TIMER_SETCYCLE,(ptu32_t)waittime);
        TimerHard_Ctrl(sgTimerHardDefault, EN_TIMER_STARTCOUNT,(ptu32_t)NULL);
    }
    else
    {
        //all the timer is paused or no timer, so no need to start the counter
    }

    return 0;
}

// =============================================================================
// 函数功能：TimerSoft_Create_s
//          创建一个定时器(自己提供资源)
// 输入参数：name,定时器名字
//          cycle,定时器周期,单位微秒
//          isr,定时器定时时间到执行HOOk，中断中可能被调用
//          timer,提供的虚拟定时器的资源空间
// 输出参数：无
// 返回值  ： NULL分配失败  否则返回创建成功的定时器句柄
//           创建的定时器默认的reload模式，如果需要手动的话，那么创建之后自己设置；
//           创建的定时器还是处于pause状态，需要手动开启该定时器
// =============================================================================
tagTimerSoft*  TimerSoft_Create_s(const char *name, u32 cycle, fnTimerSoftIsr isr,\
                                  tagTimerSoft *timer)
{
    tagTimerSoft*      result = NULL;
    u32                waittime;
    tagTimerSoftMsg    msg;
    if(NULL == timer)  //参数检查
    {
        result =NULL;
    }
    else
    {
        if(sbUsingHardTimer)
        {
           if(Lock_MutexPend(ptTimerSoftQSync,CN_TIMEOUT_FOREVER))
            {
                //暂停闹钟，进行中断互斥
                TimerHard_Ctrl(sgTimerHardDefault, EN_TIMER_PAUSECOUNT,(ptu32_t)NULL);
                //开始执行指定任务的操作
                //虚拟定时器的初始化
                timer->name = (char*)name;
                timer->cycle = cycle;
                timer->isr = isr;
                timer->deadline = CN_TIMER_ALARMNEVER;
                timer->stat = CN_TIMER_RELOAD;
                //将创建的虚拟定时器插进timer定时器队列中
                __TimerSoft_AddLast(timer);
                waittime = __TimerSoft_DealTimeout();
                if(waittime != CN_TIMEOUT_FOREVER)
                {
                    TimerHard_Ctrl(sgTimerHardDefault, EN_TIMER_SETCYCLE,(ptu32_t)waittime);
                    TimerHard_Ctrl(sgTimerHardDefault, EN_TIMER_STARTCOUNT,(ptu32_t)NULL);
                }
                Lock_MutexPost(ptTimerSoftQSync);
                result = timer;
            }
        }
        else
        {
            timer->name = (char*)name;
            timer->cycle = cycle;
            timer->isr = isr;
            timer->deadline = CN_TIMER_ALARMNEVER;
            timer->stat = CN_TIMER_RELOAD;
            msg.timer = timer;
            msg.type = EN_TIMERSOFT_ADD;
            if(MsgQ_Send(ptTimerSoftMsgQ,(u8 *)&msg, sizeof(msg),\
                               CN_TIMEOUT_FOREVER, CN_MSGQ_PRIO_NORMAL))
            {
                result = timer;
            }
        }
    }
    return result;
}
// =============================================================================
// 函数功能：TimerSoft_Delete_s
//           删除一个定时器(自己提供资源的定时器)
// 输入参数：timersoft,待删除的定时器
// 输出参数：无
// 返回值  ：被删除的定时器，NULL删除不成功
// =============================================================================
tagTimerSoft* TimerSoft_Delete_s(tagTimerSoft* timer)
{
    tagTimerSoft*      result = NULL;
    u32                waittime;
    tagTimerSoftMsg    msg;
    if(NULL == timer)  //参数检查
    {
        result =NULL;
    }
    else
    {
        if(sbUsingHardTimer)
        {
           if(Lock_MutexPend(ptTimerSoftQSync,CN_TIMEOUT_FOREVER))
            {
                //暂停闹钟，进行中断互斥
                TimerHard_Ctrl(sgTimerHardDefault, EN_TIMER_PAUSECOUNT,(ptu32_t)NULL);

                //将创建的虚拟定时器插进timer定时器队列中
                __TimerSoft_Remove(timer);
                waittime = __TimerSoft_DealTimeout();
                if(waittime != CN_TIMEOUT_FOREVER)
                {
                    TimerHard_Ctrl(sgTimerHardDefault, EN_TIMER_SETCYCLE,(ptu32_t)waittime);
                    TimerHard_Ctrl(sgTimerHardDefault, EN_TIMER_STARTCOUNT,(ptu32_t)NULL);
                }
                Lock_MutexPost(ptTimerSoftQSync);
                result = (tagTimerSoft*)timer;
            }
        }
        else
        {
            msg.timer = timer;
            msg.type = EN_TIMERSOFT_REMOVE;
            if(MsgQ_Send(ptTimerSoftMsgQ,(u8 *)&msg, sizeof(msg),\
                               CN_TIMEOUT_FOREVER, CN_MSGQ_PRIO_NORMAL))
            {
                result = timer;
            }
        }
    }
    return result;
}
// =============================================================================
// 函数功能：TimerSoft_Create
//          创建一个定时器
// 输入参数：name,定时器名字
//          cycle,定时器周期，单位微秒
//          isr,定时器定时时间到执行HOOk，中断中可能被调用
// 输出参数：无
// 返回值  ：NULL分配失败  否则返回分配到的定时器句柄
//           创建的定时器默认的reload模式，如果需要手动的话，那么创建之后自己设置；
//           创建的定时器还是处于pause状态，需要手动开启该定时器
// =============================================================================
tagTimerSoft* TimerSoft_Create(const char *name, u32 cycle,fnTimerSoftIsr isr)
{
    tagTimerSoft *timer;
    tagTimerSoft *result;

    result = NULL;
    timer = Mb_Malloc(ptTimerSoftMemPool,CN_TIMEOUT_FOREVER);
    if(NULL != timer)
    {
        result = TimerSoft_Create_s(name, cycle, isr,timer);
        if(NULL == result)
        {
            Mb_Free(ptTimerSoftMemPool,timer);
        }
    }
    return result;
}
// =============================================================================
// 函数功能：TimerSoft_Delete
//           删除一个定时器
// 输入参数：timersoft,待删除的定时器
// 输出参数：无
// 返回值  ：true 成功 false失败
// =============================================================================
bool_t TimerSoft_Delete(tagTimerSoft* timer)
{
    tagTimerSoft* result;

    result = TimerSoft_Delete_s(timer);
    if(result != (tagTimerSoft*)NULL)
    {
        Mb_Free(ptTimerSoftMemPool, (void *)timer);
        return true;
    }
    else
    {
        return false;
    }
}
// =============================================================================
// 函数功能：TimerSoft_Ctrl
//          设置定时器标识和状态
// 输入参数：timersoft,待设定的定时器.
//          opcode,定时器的操作码
//          inoutpara,根据操作码解析的参数，比方设定cycle的时候，inoutpara代表cyle
// 输出参数：无
// 返回值  ：true 成功 false失败
// 说明：opcode对应的para的属性定义说明
//  EN_TIMER_STARTCOUNT  //使能计数，inoutpara无意义
//  EN_TIMER_PAUSECOUNT, //停止计数，inoutpara无意义
//  EN_TIMER_SETCYCLE,   //设置周期，inoutpara为u32,待设置的周期（微秒）
//  EN_TIMER_SETRELOAD,  //reload模式or not！，inoutpara为bool_t,true代表reload
//  其他CMD不支持
// =============================================================================
bool_t TimerSoft_Ctrl(tagTimerSoft* timer,u32 opcode, u32 para)
{
    bool_t  result = false;
    u32     waittime;
    s64     timenow;               //当前时间
    tagTimerSoftMsg msg;
    if(timer)                      //参数检查
    {
        if(sbUsingHardTimer)
        {
            Lock_MutexPend(ptTimerSoftQSync,CN_TIMEOUT_FOREVER);
            //暂停闹钟，进行中断互斥
            TimerHard_Ctrl(sgTimerHardDefault, EN_TIMER_PAUSECOUNT,(ptu32_t)NULL);
            result = true;
            switch(opcode)
            {
                case EN_TIMER_STARTCOUNT:
                    if(0 ==(CN_TIMER_ENCOUNT & timer->stat))//本来就使能
                    {
                        timer->stat |= CN_TIMER_ENCOUNT;
                        __TimerSoft_Get64Time(&timenow);
                        timer->deadline = timenow + timer->cycle;
                        __TimerSoft_Remove(timer);
                        __TimerSoft_Add(timer);
                    }
                    break;
                case EN_TIMER_PAUSECOUNT:
                    if(CN_TIMER_ENCOUNT & timer->stat)//本来就暂停
                    {
                        timer->stat &= (~CN_TIMER_ENCOUNT);
                        timer->deadline = CN_TIMER_ALARMNEVER;
                        __TimerSoft_Remove(timer);
                        __TimerSoft_AddLast(timer);
                    }
                    break;
                case EN_TIMER_SETCYCLE:
                    timer->cycle = para;
                    if(CN_TIMER_ENCOUNT&timer->stat)
                    {
                        __TimerSoft_Get64Time(&timenow);
                        timer->deadline = timenow + timer->cycle;
                        __TimerSoft_Remove(timer);
                        __TimerSoft_Add(timer);
                    }
                    break;
                case EN_TIMER_SETRELOAD:
                    if(para)
                    {
                        timer->stat |= CN_TIMER_RELOAD;
                    }
                    else
                    {
                        timer->stat &= (~CN_TIMER_RELOAD);
                    }
                    break;
                default:
                    result = false;
                    break;
            }
            //上述操作可能会有定时器超时,做定时器队列的超时处理
            //做定时器队列的超时处理
            waittime = __TimerSoft_DealTimeout();
            if(waittime != CN_TIMEOUT_FOREVER)
            {
                TimerHard_Ctrl(sgTimerHardDefault, EN_TIMER_SETCYCLE,(ptu32_t)waittime);
                TimerHard_Ctrl(sgTimerHardDefault, EN_TIMER_STARTCOUNT,(ptu32_t)NULL);
            }
            Lock_MutexPost(ptTimerSoftQSync);
        }
        else
        {
            msg.timer = timer;
            msg.type = opcode;
            msg.para = para;
            result = MsgQ_Send(ptTimerSoftMsgQ,(u8 *)&msg, sizeof(msg),\
                               CN_TIMEOUT_FOREVER, CN_MSGQ_PRIO_NORMAL);
        }
    }

    return result;
}
// =============================================================================
// 函数功能：TimerSoft_VMTask 线程模仿硬件定时器
// 输入参数：无
// 输出参数：无
// 返回值  ：
// 说明      :当使用线程作为模拟定时器的时候，除了添加和删除以外，所有的对定时器队列的操作都
//         是在该线程中完成
// =============================================================================
ptu32_t  TimerSoft_VMTask(void)
{
    u32              waittime;
    u32              opcode;
    u32              para;
    s64              timenow;
    tagTimerSoftMsg  msg;
    tagTimerSoft     *timer;

    while(1)
    {
        //any way, we scan the queue to chk if any timer is timeout
        waittime = __TimerSoft_DealTimeout();
        if(MsgQ_Receive(ptTimerSoftMsgQ,(u8 *)&msg, sizeof(msg),waittime))
        {
            //has got some msg from the api
            opcode = msg.type;
            timer = msg.timer;
            para = msg.para;
            switch(opcode)
            {
                case EN_TIMER_STARTCOUNT:
                    if(0 ==(CN_TIMER_ENCOUNT & timer->stat))//本来就使能
                    {
                        timer->stat |= CN_TIMER_ENCOUNT;
                        __TimerSoft_Get64Time(&timenow);
                        timer->deadline = timenow + timer->cycle;
                        __TimerSoft_Remove(timer);
                        __TimerSoft_Add(timer);
                    }
                    break;
                case EN_TIMER_PAUSECOUNT:
                    if(CN_TIMER_ENCOUNT & timer->stat)//本来就暂停
                    {
                        timer->stat &= (~CN_TIMER_ENCOUNT);
                        timer->deadline = CN_TIMER_ALARMNEVER;
                        __TimerSoft_Remove(timer);
                        __TimerSoft_AddLast(timer);
                    }
                    break;
                case EN_TIMER_SETCYCLE:
                    timer->cycle = para;
                    if(CN_TIMER_ENCOUNT&timer->stat)
                    {
                        __TimerSoft_Get64Time(&timenow);
                        timer->deadline = timenow + timer->cycle;
                        __TimerSoft_Remove(timer);
                        __TimerSoft_Add(timer);
                    }
                    break;
                case EN_TIMER_SETRELOAD:
                    if(para)
                    {
                        timer->stat |= CN_TIMER_RELOAD;
                    }
                    else
                    {
                        timer->stat &= (~CN_TIMER_RELOAD);
                    }
                    break;
                case EN_TIMERSOFT_ADD:
                    __TimerSoft_Add(timer);
                    break;
                case EN_TIMERSOFT_REMOVE:
                    __TimerSoft_Remove(timer);
                    break;
                default:
                    break;
            }

        }

    }
    return 0;
}
// =============================================================================
// 函数功能：ModuleInstall_TimerSoft
//          虚拟定时器初始化模块
// 输入参数：para,该定时器模块的精度，单位微秒
// 输出参数：无
// 返回值  ：0 成功  -1失败
// =============================================================================
ptu32_t ModuleInstall_TimerSoft(ptu32_t para)
{
    u16 u16EvttId;
    u16 u16EventId;
    printk("TimerSoft:Init Start....\n\r");
    ptTimerSoftMem = M_Malloc(gc_u32CfgTimerLimit * sizeof(tagTimerSoft),\
                              CN_TIMEOUT_FOREVER);
    if(ptTimerSoftMem == NULL)
    {
        goto EXIT_MEMFAILED;
    }
    ptTimerSoftMemPool = Mb_CreatePool(ptTimerSoftMem,gc_u32CfgTimerLimit,
                                sizeof(tagTimerSoft),0,0,"TimerSoft");
    if(NULL ==ptTimerSoftMemPool)
    {
        goto EXIT_POOLFAILED;
    }
    sbUsingHardTimer = (bool_t)para;
    if(sbUsingHardTimer == CN_TIMER_SOURCE_HARD)
    {
        //使用硬件定时器的时候才会使用该同步标记
        ptTimerSoftQSync = Lock_MutexCreate("TimerSoft");
        if(NULL == ptTimerSoftQSync)
        {
            goto EXIT_TIMERFAILED;
        }
        sgTimerHardDefault = TimerHard_Alloc(0,TimerSoft_ISR);
        if((ptu32_t)NULL == sgTimerHardDefault)
        {
            Lock_MutexDelete(ptTimerSoftQSync);
            goto EXIT_TIMERFAILED;
        }
        //使能定时器中断，但是没有使能定时器,坐等API的调用
        TimerHard_Ctrl(sgTimerHardDefault,EN_TIMER_ENINT,(ptu32_t)NULL);
        TimerHard_Ctrl(sgTimerHardDefault,EN_TIMER_SETRELOAD,(ptu32_t)(false));
    }
    else
    {
        //建立通信用的邮箱
        ptTimerSoftMsgQ = MsgQ_Create(CN_TIMERSOFT_MSGLEN, \
                                      sizeof(tagTimerSoftMsg),CN_MSGQ_TYPE_FIFO);
        if(NULL == ptTimerSoftMsgQ)
        {
            goto EXIT_TIMERFAILED;
        }
        else
        {
            u16EvttId = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS-2,0,0,
                                   TimerSoft_VMTask,NULL,0x400,NULL);
            if(CN_EVTT_ID_INVALID == u16EvttId)
            {
                MsgQ_Delete(ptTimerSoftMsgQ);
                ptTimerSoftMsgQ = NULL;
                goto EXIT_TIMERFAILED;
            }
            else
            {
                u16EventId = Djy_EventPop(u16EvttId,NULL,0,0,0,0);
                if(CN_EVENT_ID_INVALID == u16EventId)
                {
                    MsgQ_Delete(ptTimerSoftMsgQ);
                    ptTimerSoftMsgQ = NULL;
                    //注销事件类型，貌似目前没有--TODO
                    goto EXIT_TIMERFAILED;
                }
            }
        }
    }

    printk("TimerSoft:Init Success\n\r");
    return 0;

EXIT_TIMERFAILED:
    Mb_DeletePool(ptTimerSoftMemPool);
EXIT_POOLFAILED:
    free(ptTimerSoftMem);
EXIT_MEMFAILED:
    printk("TimerSoft:Init Failed\n\r");
    return -1;
}
