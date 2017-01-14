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
#include <cfg/misc_config.h>
#include "board-config.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#include "os.h"

#include "timer.h"
#include "timer_hard.h"

//timer的数据结构
struct Timer
{
    struct Timer  *pre;
    struct Timer  *nxt;
    char          *name;
    u32           cycle;        //定时器周期 (单位是微秒)
    fnTimerIsr    isr;          //定时器定时时间节点钩子函数
    u32           stat;         //定时器状态标志，参见CN_TIMER_ENCOUNT等常数
    ptu32_t       TimerTag;     //私有标签
    s64           deadline;     //定时器定时时间(单位是微秒)
};

#define CN_TIMER_ALARMNEVER      CN_LIMIT_SINT64             //挂起时限

// 时精度的算法：
// 果时间基准是tick，则取1个tick对应的uS数。
// 果是硬件定时器，取1000个主频周期对应的uS数取整，若主频大于1G，取1uS。
// 硬件定时器时，限制精度是为了限制定时器中断频繁度
// 意，即使使用了硬件定时器，如果systime没有专用的定时器，而是依赖tick的话，
// 依然无法获得所需精度。
static u32 s_u32TimerPrecision;     //主频1000clk对应的uS数，取整
static u32 s_u32Precision2Tclk;     //取整后的s_u32TimerPrecision对应的定时器的周期数
static u32 s_u32TimerFreq;          //所用的硬件定时器频率
static bool_t  sbUsingHardTimer = false;
static tagTimer* ptTimerHead = NULL;                 //软定时器队列头
static tagTimer* ptTimerTail = NULL;                 //软件定时器队尾
static tagTimer           *ptTimerMem = NULL;        //动态分配内存
static struct MemCellPool  *ptTimerMemPool = NULL;    //内存池头指针。
static struct MutexLCB     *ptTimerQSync =NULL;       //API资源竞争锁

//使用的硬件定时器（指定使用硬件定时器）
static ptu32_t sgHardTimerDefault = (ptu32_t)NULL;           //用作定时的32位定时器
//使用线程模拟硬件定时器
enum __EN_TIMERSOFT_CMD
{
    EN_TIMERSOFT_ADD = EN_TIMER_LASTOPCODE+1,
    EN_TIMERSOFT_REMOVE,
};

static struct MsgQueue    *ptTimerMsgQ = NULL;
typedef struct
{
    tagTimer*     timer;
    u32               type;
    u32               para;
}tagTimerMsg;
#define CN_TIMERSOFT_MSGLEN  0X10
// =============================================================================
// 函数功能：__Timer_Get64Time
//           获取S64timer的时间
// 输入参数: 无
// 输出参数：time,单位为微秒
// 返回值  ：true 成功  false失败
// 说明    ：内部调用函数,注意该函数是移植敏感函数，S64timer,必须为不停表不被打断的
//           64位定时器，每个系统或者CPU可能用的都不一样
// =============================================================================
bool_t __Timer_Get64Time(s64 *time)
{
    *time = DjyGetSysTime();
    return true;
}
// =============================================================================
// 函数功能：__Timer_ExeAlarmHandler
//          执行timersoft的钩子函数
// 输入参数：timersoft,待执行的定时器.
// 输出参数：无
// 返回值  ：true 成功  false失败
// 说明    ：内部调用函数，没有HOOK则不执行
// =============================================================================
bool_t __Timer_ExeAlarmHandler(tagTimer *timer)
{
    if(NULL != timer->isr)
    {
        timer->isr(timer);
    }
    return true;
}

// =============================================================================
// 函数功能：__Timer_Remove
//          将指定的定时器从定时器队列中移除
// 输入参数：timer,待移除的定时器
// 输出参数：无
// 返回值  ：调整的定时器
// 说明    ：
// =============================================================================
void __Timer_Remove(tagTimer *timer)
{
    if((timer == ptTimerHead)&&(timer == ptTimerTail)) //only one
    {
        ptTimerHead = NULL;
        ptTimerTail = NULL;
    }
    else if(timer == ptTimerHead)  //head to remove
    {
        ptTimerHead = ptTimerHead->nxt;
        ptTimerHead->pre = NULL;
    }
    else if(timer == ptTimerTail) //tail to remove
    {
        ptTimerTail = ptTimerTail->pre;
        ptTimerTail->nxt = NULL;
    }
    else //normal to remove
    {
        timer->pre->nxt = timer->nxt;
        timer->nxt->pre = timer->pre;
    }
    return;
}
// =============================================================================
// 函数功能：__Timer_Add
//          添加定时器到定时器队列
// 输入参数：timer,待添加的定时器
// 输出参数：无
// 返回值  ：
// 说明    ：
// =============================================================================
void __Timer_Add(tagTimer *timer)
{
    tagTimer *tmp;
    if(NULL == ptTimerHead) //the queue is empty
    {
        ptTimerHead = timer;
        ptTimerTail = timer;
        timer->pre = NULL;
        timer->nxt = NULL;
    }
    else        //find proper place to add
    {
        tmp = ptTimerHead;
        while((NULL != tmp)&&(tmp->deadline < timer->deadline))
        {
            tmp = tmp->nxt;
        }
        if(NULL == tmp) //add the timer to the tail
        {
            timer->pre = ptTimerTail;
            timer->nxt = NULL;
            ptTimerTail->nxt = timer;
            ptTimerTail = timer;
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
                ptTimerHead = timer;
            }
            tmp->pre = timer;
        }
    }
    return;
}

// =============================================================================
// 函数功能：__Timer_AddLast
//          添加定时器到定时器队列尾
// 输入参数：timer,待添加的定时器
// 输出参数：无
// 返回值  ：
// 说明    ：
// =============================================================================
void __Timer_AddLast(tagTimer *timer)
{
    if(NULL == ptTimerHead) //the queue is empty
    {
        ptTimerHead = timer;
        ptTimerTail = timer;
        timer->pre = NULL;
        timer->nxt = NULL;
    }
    else
    {
        timer->pre = ptTimerTail;
        timer->nxt = NULL;
        ptTimerTail->nxt = timer;
        ptTimerTail = timer;
    }
    return;
}

// =============================================================================
// 函数功能：__Timer_ChkTimeout
// 输入参数：无
// 输出参数：无
// 返回值  ：true，该定时器超时，否则该定时器不超时
// 说明     :
// =============================================================================
bool_t  __Timer_ChkTimeout(tagTimer *timer, s64 timenow)
{
    bool_t result = false;
    if((timer->deadline -s_u32TimerPrecision)<timenow)
    {
        result = true;
    }
    return result;
}
// =============================================================================
// 函数功能：__Timer_DealTimeout
//          处理超时的定时器队列
// 输入参数：
// 输出参数：
// 返回值  ：下次等待的时间
// 说明    ：内部调用函数
// =============================================================================
u32 __Timer_DealTimeout(void)
{
    u32           result;
    s64           timenow;
    tagTimer *timer;

    result = CN_TIMEOUT_FOREVER;
    timer = ptTimerHead;
    while(timer) //执行完所有的TIMER ALARM
    {
        if(timer->stat &CN_TIMER_ENCOUNT)
        {
            timenow = DjyGetSysTime();     //使用系统64位不停表的定时器，可消除
                                        //定时器中断启停之间产生的积累误差
            if(__Timer_ChkTimeout(timer, timenow))
            {
                __Timer_Remove(timer);
                if(timer->stat & CN_TIMER_RELOAD)
                {
                    timer->deadline = timenow + timer->cycle;
                    __Timer_Add(timer);
                }
                else
                {
                    timer->deadline = CN_TIMER_ALARMNEVER;
                    __Timer_AddLast(timer);
                }
                __Timer_ExeAlarmHandler(timer); //execute the hook

                timer = ptTimerHead;
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

u32 __GetTclkCycle(u32 waittime)
{
    u32 result;
    result = waittime *(s_u32TimerFreq / 1E6);
    if(result < s_u32Precision2Tclk)      //中断间隔小于最小间隔
        result = s_u32Precision2Tclk;
    return result;
}
// =============================================================================
// 函数功能：Timer_ISR
//          定时器中断服务HOOK
// 输入参数：irq_no,中断号
// 输出参数：无
// 返回值     ：暂时未定义
// =============================================================================
u32 Timer_ISR(ptu32_t irq_no)
{
    u32 waittime;
    //定时器停止计数
    HardTimer_Ctrl(sgHardTimerDefault, EN_TIMER_PAUSECOUNT,(ptu32_t)NULL);

    waittime = __Timer_DealTimeout();
    if(waittime != CN_TIMEOUT_FOREVER)
    {
        waittime = __GetTclkCycle(waittime);
        HardTimer_Ctrl(sgHardTimerDefault, EN_TIMER_SETCYCLE,(ptu32_t)waittime);
        HardTimer_Ctrl(sgHardTimerDefault, EN_TIMER_STARTCOUNT,(ptu32_t)NULL);
    }
    else
    {
        //all the timer is paused or no timer, so no need to start the counter
    }

    return 0;
}

// =============================================================================
// 函数功能：Timer_Create_s
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
tagTimer*  Timer_Create_s(tagTimer *timer,const char *name,
                                  u32 cycle, fnTimerIsr isr)
{
    tagTimer*      result = NULL;
    u32                waittime;
    tagTimerMsg    msg;
    if(NULL == timer)  //参数检查
    {
        result =NULL;
    }
    else
    {
        if(sbUsingHardTimer)
        {
           if(Lock_MutexPend(ptTimerQSync,CN_TIMEOUT_FOREVER))
            {
                //暂停闹钟，进行中断互斥
                HardTimer_Ctrl(sgHardTimerDefault, EN_TIMER_PAUSECOUNT,(ptu32_t)NULL);
                //开始执行指定任务的操作
                //虚拟定时器的初始化
                timer->name = (char*)name;
                timer->cycle = cycle;
                timer->isr = isr;
                timer->deadline = CN_TIMER_ALARMNEVER;
                timer->stat = CN_TIMER_RELOAD;
                //将创建的虚拟定时器插进timer定时器队列中
                __Timer_AddLast(timer);
                waittime = __Timer_DealTimeout();
                if(waittime != CN_TIMEOUT_FOREVER)
                {
                    __GetTclkCycle(waittime);
                    HardTimer_Ctrl(sgHardTimerDefault, EN_TIMER_SETCYCLE,(ptu32_t)waittime);
                    HardTimer_Ctrl(sgHardTimerDefault, EN_TIMER_STARTCOUNT,(ptu32_t)NULL);
                }
                Lock_MutexPost(ptTimerQSync);
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
            if(MsgQ_Send(ptTimerMsgQ,(u8 *)&msg, sizeof(msg),\
                               CN_TIMEOUT_FOREVER, CN_MSGQ_PRIO_NORMAL))
            {
                result = timer;
            }
        }
    }
    return result;
}
// =============================================================================
// 函数功能：Timer_Delete_s
//           删除一个定时器(自己提供资源的定时器)
// 输入参数：timersoft,待删除的定时器
// 输出参数：无
// 返回值  ：被删除的定时器，NULL删除不成功
// =============================================================================
tagTimer* Timer_Delete_s(tagTimer* timer)
{
    tagTimer*      result = NULL;
    u32                waittime;
    tagTimerMsg    msg;
    if(NULL == timer)  //参数检查
    {
        result =NULL;
    }
    else
    {
        if(sbUsingHardTimer)
        {
           if(Lock_MutexPend(ptTimerQSync,CN_TIMEOUT_FOREVER))
            {
                //暂停闹钟，进行中断互斥
                HardTimer_Ctrl(sgHardTimerDefault, EN_TIMER_PAUSECOUNT,(ptu32_t)NULL);

                //将创建的虚拟定时器插进timer定时器队列中
                __Timer_Remove(timer);
                waittime = __Timer_DealTimeout();
                if(waittime != CN_TIMEOUT_FOREVER)
                {
                    __GetTclkCycle(waittime);
                    HardTimer_Ctrl(sgHardTimerDefault, EN_TIMER_SETCYCLE,(ptu32_t)waittime);
                    HardTimer_Ctrl(sgHardTimerDefault, EN_TIMER_STARTCOUNT,(ptu32_t)NULL);
                }
                Lock_MutexPost(ptTimerQSync);
                result = (tagTimer*)timer;
            }
        }
        else
        {
            msg.timer = timer;
            msg.type = EN_TIMERSOFT_REMOVE;
            if(MsgQ_Send(ptTimerMsgQ,(u8 *)&msg, sizeof(msg),\
                               CN_TIMEOUT_FOREVER, CN_MSGQ_PRIO_NORMAL))
            {
                result = timer;
            }
        }
    }
    return result;
}
// =============================================================================
// 函数功能：Timer_Create
//          创建一个定时器
// 输入参数：name,定时器名字
//          cycle,定时器周期，单位微秒
//          isr,定时器定时时间到执行HOOk，中断中可能被调用
// 输出参数：无
// 返回值  ：NULL分配失败  否则返回分配到的定时器句柄
//           创建的定时器默认的reload模式，如果需要手动的话，那么创建之后自己设置；
//           创建的定时器还是处于pause状态，需要手动开启该定时器
// =============================================================================
tagTimer* Timer_Create(const char *name, u32 cycle,fnTimerIsr isr)
{
    tagTimer *timer;
    tagTimer *result;

    result = NULL;
    timer = Mb_Malloc(ptTimerMemPool,CN_TIMEOUT_FOREVER);
    if(NULL != timer)
    {
        result = Timer_Create_s(timer,name, cycle, isr);
        if(NULL == result)
        {
            Mb_Free(ptTimerMemPool,timer);
        }
    }
    return result;
}
// =============================================================================
// 函数功能：Timer_Delete
//           删除一个定时器
// 输入参数：timersoft,待删除的定时器
// 输出参数：无
// 返回值  ：true 成功 false失败
// =============================================================================
bool_t Timer_Delete(tagTimer* timer)
{
    tagTimer* result;

    result = Timer_Delete_s(timer);
    if(result != (tagTimer*)NULL)
    {
        Mb_Free(ptTimerMemPool, (void *)timer);
        return true;
    }
    else
    {
        return false;
    }
}
// =============================================================================
// 函数功能：Timer_Ctrl
//          设置定时器标识和状态
// 输入参数：timersoft,待设定的定时器.
//          opcode,定时器的操作码
//          inoutpara,根据操作码解析的参数，比方设定cycle的时候，inoutpara代表cyle
// 输出参数：无
// 返回值  ：true 成功 false失败
// 说明：opcode对应的para的属性定义见enum TimerCmdCode说明
// =============================================================================
bool_t Timer_Ctrl(tagTimer* timer,u32 opcode, u32 para)
{
    bool_t  result = false;
    u32     waittime;
    s64     timenow;               //当前时间
    tagTimerMsg msg;
    if(timer)                      //参数检查
    {
        if(sbUsingHardTimer)
        {
            Lock_MutexPend(ptTimerQSync,CN_TIMEOUT_FOREVER);
            //暂停闹钟，进行中断互斥
            HardTimer_Ctrl(sgHardTimerDefault, EN_TIMER_PAUSECOUNT,(ptu32_t)NULL);
            result = true;
            switch(opcode)
            {
                case EN_TIMER_SOFT_START:
                    if(0 ==(CN_TIMER_ENCOUNT & timer->stat))    //本来未使能
                    {
                        timer->stat |= CN_TIMER_ENCOUNT;
                        timenow = DjyGetSysTime(); //使用系统64位不停表的定时器，可消除
                                                //定时器中断启停之间产生的积累误差
                        timer->deadline = timenow + timer->cycle;
                        __Timer_Remove(timer);
                        __Timer_Add(timer);
                    }
                    break;
                case EN_TIMER_SOFT_PAUSE:
                    if(CN_TIMER_ENCOUNT & timer->stat)          //本来在运行态
                    {
                        timer->stat &= (~CN_TIMER_ENCOUNT);
                        timer->deadline = CN_TIMER_ALARMNEVER;
                        __Timer_Remove(timer);
                        __Timer_AddLast(timer);
                    }
                    break;
                case EN_TIMER_SOFT_SETCYCLE:
                    timer->cycle = para;
                    if(CN_TIMER_ENCOUNT&timer->stat)
                    {
                        timenow = DjyGetSysTime(); //使用系统64位不停表的定时器，可消除
                                                //定时器中断启停之间产生的积累误差
                        timer->deadline = timenow + timer->cycle;
                        __Timer_Remove(timer);
                        __Timer_Add(timer);
                    }
                    break;
                case EN_TIMER_SOFT_SETRELOAD:
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
            waittime = __Timer_DealTimeout();
            if(waittime != CN_TIMEOUT_FOREVER)
            {
                __GetTclkCycle(waittime);
                HardTimer_Ctrl(sgHardTimerDefault, EN_TIMER_SETCYCLE,(ptu32_t)waittime);
                HardTimer_Ctrl(sgHardTimerDefault, EN_TIMER_STARTCOUNT,(ptu32_t)NULL);
            }
            Lock_MutexPost(ptTimerQSync);
        }
        else
        {
            msg.timer = timer;
            msg.type = opcode;
            msg.para = para;
            result = MsgQ_Send(ptTimerMsgQ,(u8 *)&msg, sizeof(msg),\
                               CN_TIMEOUT_FOREVER, CN_MSGQ_PRIO_NORMAL);
        }
    }

    return result;
}
// =============================================================================
// 函数功能：Timer_VMTask 线程模仿硬件定时器
// 输入参数：无
// 输出参数：无
// 返回值  ：
// 说明    :当使用线程作为模拟定时器的时候，除了添加和删除以外，所有的对定时器
//          队列的操作都是在该线程中完成
// =============================================================================
ptu32_t  Timer_VMTask(void)
{
    u32              waittime;
    u32              opcode;
    u32              para;
    s64              timenow;
    tagTimerMsg  msg;
    tagTimer     *timer;

    while(1)
    {
        //any way, we scan the queue to chk if any timer is timeout
        waittime = __Timer_DealTimeout();
        if(MsgQ_Receive(ptTimerMsgQ,(u8 *)&msg, sizeof(msg),waittime))
        {
            //has got some msg from the api
            opcode = msg.type;
            timer = msg.timer;
            para = msg.para;
            switch(opcode)
            {
                case EN_TIMER_SOFT_START:
                    if(0 ==(CN_TIMER_ENCOUNT & timer->stat))    //本来未使能
                    {
                        timer->stat |= CN_TIMER_ENCOUNT;
                        timenow = DjyGetSysTime(); //使用系统64位不停表的定时器，可消除
                                                //错过tick中断产生的积累误差
                        timer->deadline = timenow + timer->cycle;
                        __Timer_Remove(timer);
                        __Timer_Add(timer);
                    }
                    break;
                case EN_TIMER_SOFT_PAUSE:
                    if(CN_TIMER_ENCOUNT & timer->stat)      //本来在运行态
                    {
                        timer->stat &= (~CN_TIMER_ENCOUNT);
                        timer->deadline = CN_TIMER_ALARMNEVER;
                        __Timer_Remove(timer);
                        __Timer_AddLast(timer);
                    }
                    break;
                case EN_TIMER_SOFT_SETCYCLE:
                    timer->cycle = para;
                    if(CN_TIMER_ENCOUNT&timer->stat)
                    {
                        timenow = DjyGetSysTime(); //使用系统64位不停表的定时器，可消除
                                                //错过tick中断产生的积累误差
                        timer->deadline = timenow + timer->cycle;
                        __Timer_Remove(timer);
                        __Timer_Add(timer);
                    }
                    break;
                case EN_TIMER_SOFT_SETRELOAD:
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
                    __Timer_Add(timer);
                    break;
                case EN_TIMERSOFT_REMOVE:
                    __Timer_Remove(timer);
                    break;
                default:
                    break;
            }

        }

    }
    return 0;
}

//-----------------------------------------------------------------------------
//功能：取定时器私有标签
//参数：timersoft,定时器指针.
//返回：定时器的私有标签
//-----------------------------------------------------------------------------
ptu32_t Timer_GetTag(tagTimer* timer)
{
    if(timer != NULL)
        return timer->TimerTag;
    else
        return 0;
}

//-----------------------------------------------------------------------------
//功能：取定时器名字
//参数：timersoft,定时器指针.
//返回：定时器名字
//-----------------------------------------------------------------------------
char *Timer_GetName(tagTimer* timer)
{
    if(timer != NULL)
        return timer->name;
    else
        return NULL;
}


// =============================================================================
// 函数功能：ModuleInstall_Timer
//          虚拟定时器初始化模块
// 输入参数：para,表示定时器模块使用硬件定时器还是tick做定时基准，取值为
//              CN_TIMER_SOURCE_TICK或CN_TIMER_SOURCE_HARD
// 输出参数：无
// 返回值  ：0 成功  -1失败
// =============================================================================
ptu32_t ModuleInstall_Timer(ptu32_t para)
{
    u16 u16EvttId;
    u16 u16EventId;
    printk("Timer:Init Start....\n\r");
    ptTimerMem = M_Malloc(gc_u32CfgTimerLimit * sizeof(tagTimer),\
                              CN_TIMEOUT_FOREVER);
    if(ptTimerMem == NULL)
    {
        goto EXIT_MEMFAILED;
    }
    ptTimerMemPool = Mb_CreatePool(ptTimerMem,gc_u32CfgTimerLimit,
                                sizeof(tagTimer),0,0,"Timer");
    if(NULL ==ptTimerMemPool)
    {
        goto EXIT_POOLFAILED;
    }
    sbUsingHardTimer = (bool_t)para;
    if(sbUsingHardTimer == CN_TIMER_SOURCE_HARD)
    {
        //使用硬件定时器的时候才会使用该同步标记
        ptTimerQSync = Lock_MutexCreate("Timer");
        if(NULL == ptTimerQSync)
        {
            goto EXIT_TIMERFAILED;
        }
        sgHardTimerDefault = HardTimer_Alloc(Timer_ISR);
        if((ptu32_t)NULL == sgHardTimerDefault)
        {
            Lock_MutexDelete(ptTimerQSync);
            goto EXIT_TIMERFAILED;
        }
        s_u32TimerPrecision = 1E9/CN_CFG_MCLK;     //计算1000个CPU周期对应的uS数，取整
        if(s_u32TimerPrecision == 0)               //主频可能超过1G
            s_u32TimerPrecision = 1;
        s_u32TimerFreq = s_u32TimerPrecision*CN_CFG_MCLK/1000000;    //取整后s_u32TimerPrecision uS对应的主频周期数
        s_u32Precision2Tclk = HardTimer_GetFreq(sgHardTimerDefault);
        //算对应的定时器周期数，这是timer模块中定时器设计的最小定时间隔
        s_u32Precision2Tclk =s_u32Precision2Tclk*(s_u32TimerFreq/ 1E6);
        //使能定时器中断，但是没有使能定时器,坐等API的调用
        HardTimer_Ctrl(sgHardTimerDefault,EN_TIMER_ENINT,(ptu32_t)NULL);
        HardTimer_Ctrl(sgHardTimerDefault,EN_TIMER_SETRELOAD,(ptu32_t)false);
    }
    else
    {
        //建立通信用的消息队列
        ptTimerMsgQ = MsgQ_Create(CN_TIMERSOFT_MSGLEN, \
                                      sizeof(tagTimerMsg),CN_MSGQ_TYPE_FIFO);
        if(NULL == ptTimerMsgQ)
        {
            goto EXIT_TIMERFAILED;
        }
        else
        {
            u16EvttId = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS-2,0,0,
                                   Timer_VMTask,NULL,0x400,NULL);
            if(CN_EVTT_ID_INVALID == u16EvttId)
            {
                MsgQ_Delete(ptTimerMsgQ);
                ptTimerMsgQ = NULL;
                goto EXIT_TIMERFAILED;
            }
            else
            {
                s_u32TimerPrecision = CN_CFG_TICK_US;   //精度=1个tick
                u16EventId = Djy_EventPop(u16EvttId,NULL,0,0,0,0);
                if(CN_EVENT_ID_INVALID == u16EventId)
                {
                    MsgQ_Delete(ptTimerMsgQ);
                    ptTimerMsgQ = NULL;
                    Djy_EvttUnregist(u16EventId);
                    goto EXIT_TIMERFAILED;
                }
            }
        }
    }

    printk("Timer:Init Success\n\r");
    return 0;

EXIT_TIMERFAILED:
    Mb_DeletePool(ptTimerMemPool);
EXIT_POOLFAILED:
    free(ptTimerMem);
EXIT_MEMFAILED:
    printk("Timer:Init Failed\n\r");
    return -1;
}
