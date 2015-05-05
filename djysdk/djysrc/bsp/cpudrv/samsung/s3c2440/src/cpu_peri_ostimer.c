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
// 模块描述: PIC控制器上的TIMER的驱动
// 模块版本: V1.00
// 创建人员: zhangqf_cyg
// 创建时间: 1:55:12 PM/Mar 4, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 备注：由于提供给timer_core.c使用，所以不再做参数检查之类的

#include "stddef.h"
#include "config-prj.h"

#include "cpu_peri.h"
#include "cpu_peri_timer.h"
#include "int.h"
#include "cpu.h"
#include "cpu_peri_int_line.h"
#include "timer_hard.h"

#define CN_TIMER_MAX_CYCLE (0xFFFFFFFF)

// =============================================================================

enum ENUM_SAMSUNG_TIMER
{
    EN_SAMSUNGTIMER_0=0,
    EN_SAMSUNGTIMER_1,
    EN_SAMSUNGTIMER_2,
    EN_SAMSUNGTIMER_3,
    EN_SAMSUNGTIMER_4,
};

//各个定时器芯片的定时器应该有自己的句柄
struct tagSAMSUNGTimerHandle
{
    u32     timerno;          //定时器号
    u32     irqline;          //中断号
    u32     cycle;            //定时周期
    u32     timerstate;       //定时器标识
};
#define CN_SAMSUNGTIMER_NUM   (EN_SAMSUNGTIMER_4 +1)
#define CN_SAMSUNGTIMER_MAX    EN_SAMSUNGTIMER_4

static struct tagSAMSUNGTimerHandle  stgTimerHandle[CN_SAMSUNGTIMER_NUM];

//最高位代表timer0 依次类推
static u32  gs_dwSAMSUNGTimerBitmap;  //对于定时器这种东西，一般的不会很多，32个应该足够
#define CN_SAMSUNGTIMER_BITMAP_MSK  (0x80000000)  //最高位为1，依次右移即可

//timer0..timern的irq
static u32 sgHaltimerIrq[CN_SAMSUNGTIMER_NUM]={cn_int_line_timer0,\
                                                cn_int_line_timer1,\
                                                cn_int_line_timer2,\
                                                cn_int_line_timer3,\
                                                cn_int_line_timer4};
//获取32位数第一个0bit位置(从高位到低位算起)
u8 __SAMSUNGTimer_GetFirstZeroBit(u32 para)
{
    u8 i;
    for(i = 0; i < 32; i++)
    {
        if(!(para & (1<<(31-i))))
            break;
    }
    return i;
}
// =============================================================================
// 函数功能:__SAMSUNGTimer_PauseCount
//          停止计数
// 输入参数:timer，PIC定时器
// 输出参数:
// 返回值  :true成功 fasle失败
// 说明    :
// =============================================================================
bool_t __SAMSUNGTimer_PauseCount(struct tagSAMSUNGTimerHandle  *timer)
{
    u8 timerno;
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timerno = timer->timerno;
        if(timerno > CN_SAMSUNGTIMER_MAX)
        {
            return false;
        }
        else
        {
            Timer_Stop(timerno);
            timer->timerstate = (timer->timerstate)&(~CN_TIMER_ENCOUNT);
            return true;
        }
    }
    else
    {
        return false;
    }
}
// =============================================================================
// 函数功能:__SAMSUNGTimer_StartCount
//          开始计数
// 输入参数:timer，PIC定时器
// 输出参数:
// 返回值  :true成功 fasle失败
// 说明    :
// =============================================================================
bool_t __SAMSUNGTimer_StartCount(struct tagSAMSUNGTimerHandle  *timer)
{
    u8 timerno;
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timerno = timer->timerno;
        if(timerno > CN_SAMSUNGTIMER_MAX)
        {
            return false;
        }
        else
        {
            Timer_Reload(timerno);
            Timer_Start(timerno);
            timer->timerstate = (timer->timerstate)| (CN_TIMER_ENCOUNT);

            return true;
        }
    }
    else
    {
        return false;
    }

}

// =============================================================================
// 函数功能:__SAMSUNGTimer_SetCycle,SAMSUNG的timer,最大为0xFFFFFFFF,根据Timer_ModuleInit
//          设定计数器每计一次数为1us，可知最大时间不超-1
//          设定周期
// 输入参数:timer，PIC定时器
//          cycle,周期（微秒），
// 输出参数:
// 返回值  :true成功 fasle失败
// 说明    :如果设置周期太大（超过最大定时器能力），则设置为定时器的最大周期
// =============================================================================
bool_t  __SAMSUNGTimer_SetCycle(struct tagSAMSUNGTimerHandle  *timer, u32 cycle)
{
    u8 timerno;
    u16 rel_cycle;
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timerno = timer->timerno;
        if(timerno > CN_SAMSUNGTIMER_MAX)
        {
            return false;
        }
        else
        {
            rel_cycle = cycle > 65535 ? 65535 : cycle;
            Timer_SetCounter(timerno,rel_cycle);
            timer->cycle = rel_cycle;

            return true;
        }
    }
    else
    {
        return false;
    }
}
// =============================================================================
// 函数功能:__SAMSUNGTimer_SetAutoReload
//          设定周期
// 输入参数:timer，PIC定时器
//         autoreload, true则自己reload周期，否则手动每次设置周期
// 输出参数:
// 返回值  :true成功 fasle失败
// =============================================================================
bool_t  __SAMSUNGTimer_SetAutoReload(struct tagSAMSUNGTimerHandle  *timer, bool_t autoreload)
{
    bool_t result;
    u8 timerno;
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timerno = timer->timerno;
        if((timerno >= EN_SAMSUNGTIMER_0) &&(timerno < CN_SAMSUNGTIMER_MAX))
        {
            if(autoreload == true)
            {
                Timer_SetType(timerno,1);
            }
            else
            {
                Timer_SetType(timerno,0);
            }
        }
        else
        {
            result = false;
        }
    }
    else
    {
        result = false;
    }

    return result;
}
// =============================================================================
// 函数功能:__SAMSUNGTimer_Alloc
//          分配定时器
// 输入参数:cycle，定时器周期
//          timerisr,定时器的中断处理函数
// 输出参数:
// 返回值  :分配的定时器句柄，NULL则分配不成功
// 说明    :
// =============================================================================
ptu32_t __SAMSUNGTimer_Alloc(u32 cycle,fnTimerIsr timerisr)
{
    u8 timerno;
    u8 irqline;
    struct tagSAMSUNGTimerHandle  *timer;
    ptu32_t timerhandle;
    //原子操作，防止资源竞争
    atom_low_t  timeratom;
    timeratom = Int_LowAtomStart();

    //寻找空闲的timer
    timerno = __SAMSUNGTimer_GetFirstZeroBit(gs_dwSAMSUNGTimerBitmap);
    if(timerno < CN_SAMSUNGTIMER_NUM)//还有空闲的，则设置标志位
    {
        gs_dwSAMSUNGTimerBitmap = gs_dwSAMSUNGTimerBitmap | (CN_SAMSUNGTIMER_BITMAP_MSK<< timerno);
        Int_LowAtomEnd(timeratom);  //原子操作完毕
    }
    else//没有的话直接返回就可以了，用不着再啰嗦了
    {
        Int_LowAtomEnd(timeratom);   //原子操作完毕
        return NULL;
    }
    irqline = sgHaltimerIrq[timerno];
    timer = &stgTimerHandle[timerno];
    timer->cycle = cycle;
    timer->timerno = timerno;
    timer->irqline = irqline;
    timer->timerstate = CN_TIMER_ENUSE;
    //好了，中断号和定时器号码都有了，该干嘛就干嘛了。
    //先设置好定时器周期
    __SAMSUNGTimer_PauseCount(timer);
    __SAMSUNGTimer_SetCycle(timer,cycle);
    //设置定时器中断,先结束掉该中断所有的关联相关内容
    Int_IsrConnect(irqline,timerisr);
    Int_SettoAsynSignal(irqline);
    Int_ClearLine(irqline);       //清掉初始化产生的发送fifo空的中断
    Int_RestoreAsynLine(irqline);
    timerhandle = (ptu32_t)timer;

    return timerhandle;
}


// =============================================================================
// 函数功能:__SAMSUNGTimer_Free
//          释放定时器
// 输入参数:timer，分配的定时器
// 输出参数:
// 返回值  :true成功false失败
// 说明    :
// =============================================================================
bool_t  __SAMSUNGTimer_Free(ptu32_t timerhandle)
{
    u8 timerno;
    u8 irqline;
    atom_low_t  timeratom;  //保护公用资源
    struct tagSAMSUNGTimerHandle  *timer;
    timer = (struct tagSAMSUNGTimerHandle  *)timerhandle;

    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timerno = timer->timerno;
        irqline = timer->irqline;
        if(timerno < CN_SAMSUNGTIMER_NUM)//还有空闲的，则设置标志位
        {       //修改全局标志一定是原子性的
            timeratom = Int_LowAtomStart();
            gs_dwSAMSUNGTimerBitmap = gs_dwSAMSUNGTimerBitmap &(~(CN_SAMSUNGTIMER_BITMAP_MSK<< timerno));
            //解除掉中断所关联的内容
            timer->timerstate = 0;
            Int_CutLine(irqline);
            Int_IsrDisConnect(irqline);
            Int_EvttDisConnect(irqline);

            Int_LowAtomEnd(timeratom);  //原子操作完毕

            return true;
        }
        else//没有的话直接返回就可以了，用不着再啰嗦了
        {
            return false;
        }

    }
    else
    {
        return false;
    }
}


// =============================================================================
// 函数功能:__SAMSUNGTimer_SetIntPro
//          设置定时器的中断类型
// 输入参数:timer，待操作的定时器
//          real_prior为true则为实时信号否则为异步信号
// 输出参数:
// 返回值  :分配的定时器，NULL则分配不成功
// 说明    :
// =============================================================================
bool_t  __SAMSUNGTimer_SetIntPro(struct tagSAMSUNGTimerHandle  *timer, bool_t real_prior)
{
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        if(real_prior)
        {
            timer->timerstate = (timer->timerstate)| (CN_TIMER_REALINT);
            Int_EvttDisConnect(timer->irqline);
            return Int_SettoReal(timer->irqline);
        }
        else
        {
            timer->timerstate = (timer->timerstate)&(~CN_TIMER_REALINT);
            return Int_SettoAsynSignal(timer->irqline);
        }
    }
    else
    {
        return false;
    }
}

// =============================================================================
// 函数功能:__SAMSUNGTimer_EnInt
//          使能定时器中断
// 输入参数:timer，待操作的定时器
// 输出参数:
// 返回值  :true成功false失败
// 说明    :
// =============================================================================
bool_t  __SAMSUNGTimer_EnInt(struct tagSAMSUNGTimerHandle  *timer)
{
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timer->timerstate = (timer->timerstate)| (CN_TIMER_ENINT);
        return Int_ContactLine(timer->irqline);
    }
    else
    {
        return false;
    }
}
// =============================================================================
// 函数功能:__SAMSUNGTimer_DisInt
//          禁止定时器中断
// 输入参数:timer，待操作的定时器
// 输出参数:
// 返回值  :true成功false失败
// 说明    :
// =============================================================================
bool_t  __SAMSUNGTimer_DisInt(struct tagSAMSUNGTimerHandle  *timer)
{
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timer->timerstate = (timer->timerstate)&(~CN_TIMER_ENINT);
        return Int_CutLine(timer->irqline);
    }
    else
    {
        return false;
    }
}
// =============================================================================
// 函数功能:__SAMSUNGTimer_GetTime
//          获取定时器走时
// 输入参数:timer，待操作的定时器
// 输出参数:time，走时（微秒）
// 返回值  :true成功false失败
// 说明    :从设定的周期算起，即cycle-剩余时间,表示已经走掉的时间
// =============================================================================
bool_t __SAMSUNGTimer_GetTime(struct tagSAMSUNGTimerHandle  *timer, u32 *time)
{
    u8 timerno;
    u32 counter;
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timerno = timer->timerno;
        if(timerno > CN_SAMSUNGTIMER_MAX)
        {
            return false;
        }
        counter = timer->cycle - Timer_Read(timerno);   //TCR即是微秒数
        return counter;
    }
    else
    {
        return false;
    }
}
// =============================================================================
// 函数功能:__SAMSUNGTimer_CheckTimeout
//          看定时器的定时时间是否已经到了
// 输入参数:timer，待操作的定时器
// 输出参数:timeout, true超时，false不超时
// 返回值  :true成功 false失败
// 说明    :
// =============================================================================
bool_t __SAMSUNGTimer_CheckTimeout(struct tagSAMSUNGTimerHandle  *timer, bool_t *timeout)
{
    bool_t result=true;
    u8 timerno;
    u32 counter;
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timerno = timer->timerno;
        if(timerno > CN_SAMSUNGTIMER_MAX)
        {
            result = false;
        }
        else
        {
            counter = Timer_Read(timerno);
            if(0 == counter)
            {
                *timeout = true;
            }
            else
            {
                *timeout = false;
            }
        }
    }
    else
    {
        result = false;
    }
    return result;
}

// =============================================================================
// 函数功能：__SAMSUNGTimer_GetID
//          获取定时器ID
// 输入参数：timer，待操作定时器，
// 输出参数：timerId,高16位为TIMERNO,低16为对应的IRQNO
// 返回值  ：true 成功 false失败
// 说明    : 本层实现
// =============================================================================
bool_t __SAMSUNGTimer_GetID(struct tagSAMSUNGTimerHandle   *timer,u32 *timerId)
{
    u16 irqno;
    u16 timerno;

    if(NULL == timer)
    {
        return false;
    }
    else
    {
        irqno = (u16)timer->timerno;
        timerno = (u16)timer->irqline;
        *timerId = (timerno<<16)|(irqno);
        return true;
    }
}

// =============================================================================
// 函数功能：__SAMSUNGTimer_GetCycle
//          获取定时器周期
// 输入参数：timer，待操作定时器，
// 输出参数：cycle，定时器周期(微秒)
// 返回值  ：true 成功 false失败
// =============================================================================
bool_t __SAMSUNGTimer_GetCycle(struct tagSAMSUNGTimerHandle   *timer, u32 *cycle)
{
    if(NULL == timer)
    {
        return false;
    }
    else
    {
        *cycle = timer->cycle;
        return true;
    }
}
// =============================================================================
// 函数功能：__SAMSUNGTimer_GetState
//          获取定时器状态
// 输入参数：timer,待操作的定时器
// 输出参数：timerflag，定时器状态
// 返回值  ：true 成功 false失败
// 说明    : 本层实现
// =============================================================================
bool_t __SAMSUNGTimer_GetState(struct tagSAMSUNGTimerHandle   *timer, u32 *timerflag)
{

    if(NULL == timer)
    {
        return false;
    }
    else
    {
        *timerflag = timer->timerstate;
        return true;
    }
}
// =============================================================================
// 函数功能:__SAMSUNGTimer_Ctrl
//          看定时器的定时时间是否已经到了
// 输入参数:timerhandle 待操作的定时器句柄
//         ctrlcmd, 操作命令码
// 输出参数:inoutpara,根据ctrlcmd的不同而不同
// 返回值  :true成功  false失败
// 说明    :
// =============================================================================
bool_t __SAMSUNGTimer_Ctrl(ptu32_t timerhandle, \
                         enum _ENUM_TIMER_CTRL_TYPE_ ctrlcmd, \
                         ptu32_t inoutpara)
{
    bool_t result;
    struct tagSAMSUNGTimerHandle  *timer;
    timer = (struct tagSAMSUNGTimerHandle  *)timerhandle;
    if(NULL == timer)
    {
        result = false;
    }
    else
    {
        switch(ctrlcmd)
        {
            case EN_TIMER_STARTCOUNT:
                result = __SAMSUNGTimer_StartCount(timer);
                break;
            case EN_TIMER_PAUSECOUNT:
                result = __SAMSUNGTimer_PauseCount(timer);
                break;
            case EN_TIMER_SETCYCLE:
                result = __SAMSUNGTimer_SetCycle(timer,(u32)inoutpara);
                break;
            case EN_TIMER_SETRELOAD:
                result = __SAMSUNGTimer_SetAutoReload(timer, (bool_t)inoutpara);
                break;
            case EN_TIMER_ENINT:
                result = __SAMSUNGTimer_EnInt(timer);
                break;
            case EN_TIMER_DISINT:
                result = __SAMSUNGTimer_DisInt(timer);
                break;
            case EN_TIMER_SETINTPRO:
                result = __SAMSUNGTimer_SetIntPro(timer, (bool_t)inoutpara);
                break;
            case EN_TIMER_GETTIME:
                result = __SAMSUNGTimer_GetTime(timer, (u32 *)inoutpara);
                break;
            case EN_TIMER_GETCYCLE:
                result = __SAMSUNGTimer_GetCycle(timer, (u32 *)inoutpara);
                break;
            case EN_TIMER_GETID:
                result = __SAMSUNGTimer_GetID(timer, (u32 *)inoutpara);
                break;
            case EN_TIMER_GETSTATE:
                result = __SAMSUNGTimer_GetState(timer, (u32 *)inoutpara);
                break;
            default:
                break;
        };
    }

    return result;
}

// =============================================================================
// 函数功能:module_init_timer
//          P1020的PICtimer初始化
// 输入参数:
// 输出参数:
// 返回值  :
// 说明    :
// =============================================================================
void Timer_ModuleInit(void)
{
    struct tagTimerChip  SAMSUNGtimer;

    //Only Timer0 used
    gs_dwSAMSUNGTimerBitmap |= (0x80000000 >> EN_SAMSUNGTIMER_1)
                            | (0x80000000 >> EN_SAMSUNGTIMER_2)
                            | (0x80000000 >> EN_SAMSUNGTIMER_3)
                            | (0x80000000 >> EN_SAMSUNGTIMER_4);

    //该模块用到定时器0
    Timer_SetClkSource(EN_SAMSUNGTIMER_0,0);
    //预分频数:设置定时器输入时钟1Mhz
    Timer_SetPrecale(0,(u32)CN_CFG_TIMER_CLK/1000000/2 -1);

    SAMSUNGtimer.chipname = "SAMSUNGTIMER";
    SAMSUNGtimer.timerhardalloc = __SAMSUNGTimer_Alloc;
    SAMSUNGtimer.timerhardfree = __SAMSUNGTimer_Free;
    SAMSUNGtimer.timerhardctrl = __SAMSUNGTimer_Ctrl;
    TimerHard_RegisterChip(&SAMSUNGtimer);

    return ;
}

