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
//todo: 贺敏,修改注释\文件名等.
// 模块描述: PIC控制器上的TIMER的驱动
// 模块版本: V1.00
// 创建人员: zhangqf_cyg
// 创建时间: 1:55:12 PM/Mar 4, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 备注：由于提供给timer_core.c使用，所以不再做参数检查之类的
#include "stdint.h"
#include "stddef.h"
#include "cpu_peri.h"
#include "cfg/misc_config.h"
#include "int.h"
#include "cpu.h"
#include "cpu_peri_int_line.h"
#include "timer_hard.h"

#define CN_PIT_MAX_COUNTER     0xFFFF
#define CN_PIT_MAX_TIME_US     (0xFFFF)

// =============================================================================
#define CN_TIMER6_BASE          0x40001000
#define CN_TIMER7_BASE          0x40001400
// =============================================================================
static tagTimerReg volatile * const tg_TIMER_Reg[] = {
                                            (tagTimerReg *)CN_TIMER6_BASE,
                                            (tagTimerReg *)CN_TIMER7_BASE};

enum ENUM_STM32_TIMER
{
    EN_STM32TIMER_6=0,
    EN_STM32TIMER_7,
};

//各个定时器芯片的定时器应该有自己的句柄
struct STM32TimerHandle
{
    u32     timerno;          //定时器号
    u32     irqline;          //中断号
    u32     cycle;            //定时周期
    u32     timerstate;       //定时器标识
};
#define CN_STM32TIMER_NUM   (EN_STM32TIMER_7 +1)
#define CN_STM32TIMER_MAX    EN_STM32TIMER_7

static struct STM32TimerHandle  stgTimerHandle[CN_STM32TIMER_NUM];

//最高位代表timer0 依次类推
static u32  gs_dwSTM32TimerBitmap;  //对于定时器这种东西，一般的不会很多，32个应该足够
#define CN_STM32TIMER_BITMAP_MSK  (0x80000000)  //最高位为1，依次右移即可

//timer0..timern的irq
static u32 sgHaltimerIrq[CN_STM32TIMER_NUM]={
                                            //CN_INT_LINE_TIM2,
                                            //CN_INT_LINE_TIM3,
                                            //CN_INT_LINE_TIM4,
                                            //CN_INT_LINE_TIM5,
                                            CN_INT_LINE_TIM6,\
                                            CN_INT_LINE_TIM7};
//获取32位数第一个0bit位置(从高位到低位算起)
u8 __STM32Timer_GetFirstZeroBit(u32 para)
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
// 函数功能:__STM32Timer_Time2Counter
//          将提供的时间换算为定时器的计数单位,PIT时钟是50M的定时频率
// 输入参数:time，要换算的时间(单位为微秒)
// 输出参数:counter,对应时间的counter
// 返回值  :true成功 fasle失败
// 说明    :time太大会失败，计算的counter则是定时器的最大值
// =============================================================================
bool_t __STM32Timer_Time2Counter(u32 time, u32 *counter)
{
    u32 time_set;
    time_set = time > CN_PIT_MAX_TIME_US ? CN_PIT_MAX_TIME_US : time;
    if(CN_PIT_MAX_TIME_US > time)
    {
        time_set = time;
        *counter =  time_set;
        return true;
    }
    else
    {
        time_set = CN_PIT_MAX_TIME_US;
        *counter =  time_set;
        return false;
    }
}
// =============================================================================
// 函数功能:__STM32Timer_Counter2Time
//          将定时器的计数单位换算为时间
// 输入参数:counter,定时器的counter单元
// 输出参数:time，换算的时间(单位为微秒)
// 返回值  :true成功 fasle失败
// =============================================================================
bool_t __STM32Timer_Counter2Time(u32 counter,u32 *time)
{
    if(counter > CN_PIT_MAX_COUNTER)
    {
        *time = CN_PIT_MAX_TIME_US;
        return false;
    }
    else
    {
        *time = counter;
        return true;
    }
}
// =============================================================================
// 函数功能:__STM32Timer_PauseCount
//          停止计数
// 输入参数:timer，PIC定时器
// 输出参数:
// 返回值  :true成功 fasle失败
// 说明    :
// =============================================================================
bool_t __STM32Timer_PauseCount(struct STM32TimerHandle  *timer)
{
    u8 timerno;
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timerno = timer->timerno;
        if(timerno > CN_STM32TIMER_MAX)
        {
            return false;
        }
        else
        {
            tg_TIMER_Reg[timerno]->CR1 &= ~TIM_CR1_CEN_MASK;
            tg_TIMER_Reg[timerno]->SR = 0;//清中断标志
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
// 函数功能:__STM32Timer_StartCount
//          开始计数
// 输入参数:timer，PIC定时器
// 输出参数:
// 返回值  :true成功 fasle失败
// 说明    :
// =============================================================================
bool_t __STM32Timer_StartCount(struct STM32TimerHandle  *timer)
{
    u8 timerno;
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timerno = timer->timerno;
        if(timerno > CN_STM32TIMER_MAX)
        {
            return false;
        }
        else
        {
            tg_TIMER_Reg[timerno]->CNT = 0;
            tg_TIMER_Reg[timerno]->SR = 0;//清中断标志
            tg_TIMER_Reg[timerno]->CR1 |= TIM_CR1_CEN_MASK;
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
// 函数功能:__STM32Timer_SetCycle,STM32的timer,最大为0xFFFFFFFF,根据Timer_ModuleInit
//          设定计数器每计一次数为1us，可知最大时间不超-1
//          设定周期
// 输入参数:timer，PIC定时器
//          cycle,周期（定时器时钟个数），
// 输出参数:
// 返回值  :true成功 fasle失败
// 说明    :如果设置周期太大（超过最大定时器能力），则设置为定时器的最大周期
// =============================================================================
bool_t  __STM32Timer_SetCycle(struct STM32TimerHandle  *timer, u32 cycle)
{
    u8 timerno;
    u32 counter,time_set;
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timerno = timer->timerno;
        if(timerno > CN_STM32TIMER_MAX)
        {
            return false;
        }
        else
        {
            __STM32Timer_Time2Counter(cycle,&counter);
            tg_TIMER_Reg[timerno]->ARR = (u16)counter;
            __STM32Timer_Counter2Time(counter,&time_set);
            timer->cycle = time_set;

            return true;
        }
    }
    else
    {
        return false;
    }
}
// =============================================================================
// 函数功能:__STM32Timer_SetAutoReload
//          设定周期
// 输入参数:timer，PIC定时器
//         autoreload, true则自己reload周期，否则手动每次设置周期
// 输出参数:
// 返回值  :true成功 fasle失败
// =============================================================================
bool_t  __STM32Timer_SetAutoReload(struct STM32TimerHandle  *timer, bool_t autoreload)
{
    bool_t result;
    u8 timerno;
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timerno = timer->timerno;
        if(timerno < CN_STM32TIMER_MAX)
        {
            if(autoreload == true)
            {
                tg_TIMER_Reg[timerno]->CR1 |= TIM_CR1_ARPE_MASK;
            }
            else
            {
                tg_TIMER_Reg[timerno]->CR1 &= ~TIM_CR1_ARPE_MASK;
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
// 函数功能:__STM32Timer_Alloc
//          分配定时器
// 输入参数:timerisr,定时器的中断处理函数
// 输出参数:
// 返回值  :分配的定时器句柄，NULL则分配不成功
// 说明    :
// =============================================================================
ptu32_t __STM32Timer_Alloc(fntTimerIsr timerisr)
{
    u8 timerno;
    u8 irqline;
    struct STM32TimerHandle  *timer;
    ptu32_t timerhandle;
    //原子操作，防止资源竞争
    atom_low_t  timeratom;
    timeratom = Int_LowAtomStart();

    //寻找空闲的timer
    timerno = __STM32Timer_GetFirstZeroBit(gs_dwSTM32TimerBitmap);
    if(timerno < CN_STM32TIMER_NUM)//还有空闲的，则设置标志位
    {
        gs_dwSTM32TimerBitmap = gs_dwSTM32TimerBitmap | (CN_STM32TIMER_BITMAP_MSK<< timerno);
        Int_LowAtomEnd(timeratom);  //原子操作完毕
    }
    else//没有的话直接返回就可以了，用不着再啰嗦了
    {
        Int_LowAtomEnd(timeratom);   //原子操作完毕
        return 0;
    }
    irqline = sgHaltimerIrq[timerno];
    timer = &stgTimerHandle[timerno];
    timer->cycle = 0;
    timer->timerno = timerno;
    timer->irqline = irqline;
    timer->timerstate = CN_TIMER_ENUSE;
    //好了，中断号和定时器号码都有了，该干嘛就干嘛了。
    //先设置好定时器周期
    __STM32Timer_PauseCount(timer);
//    __STM32Timer_SetCycle(timer,cycle);
    //设置定时器中断,先结束掉该中断所有的关联相关内容
    Int_Register(irqline);
    Int_CutLine(irqline);
    Int_IsrDisConnect(irqline);
    Int_EvttDisConnect(irqline);
    Int_SettoAsynSignal(irqline);
    Int_IsrConnect(irqline, timerisr);
    timerhandle = (ptu32_t)timer;

    return timerhandle;
}


// =============================================================================
// 函数功能:__STM32Timer_Free
//          释放定时器
// 输入参数:timer，分配的定时器
// 输出参数:
// 返回值  :true成功false失败
// 说明    :
// =============================================================================
bool_t  __STM32Timer_Free(ptu32_t timerhandle)
{
    u8 timerno;
    u8 irqline;
    atom_low_t  timeratom;  //保护公用资源
    struct STM32TimerHandle  *timer;
    timer = (struct STM32TimerHandle  *)timerhandle;

    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timerno = timer->timerno;
        irqline = timer->irqline;
        if(timerno < CN_STM32TIMER_NUM)//还有空闲的，则设置标志位
        {       //修改全局标志一定是原子性的
            timeratom = Int_LowAtomStart();
            gs_dwSTM32TimerBitmap = gs_dwSTM32TimerBitmap &(~(CN_STM32TIMER_BITMAP_MSK<< timerno));
            //解除掉中断所关联的内容
            timer->timerstate = 0;
            Int_CutLine(irqline);
            Int_IsrDisConnect(irqline);
            Int_EvttDisConnect(irqline);
            Int_UnRegister(irqline);
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
// 函数功能:__STM32Timer_SetIntPro
//          设置定时器的中断类型
// 输入参数:timer，待操作的定时器
//          real_prior为true则为实时信号否则为异步信号
// 输出参数:
// 返回值  :分配的定时器，NULL则分配不成功
// 说明    :
// =============================================================================
bool_t  __STM32Timer_SetIntPro(struct STM32TimerHandle  *timer, bool_t real_prior)
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
// 函数功能:__STM32Timer_EnInt
//          使能定时器中断
// 输入参数:timer，待操作的定时器
// 输出参数:
// 返回值  :true成功false失败
// 说明    :
// =============================================================================
bool_t  __STM32Timer_EnInt(struct STM32TimerHandle  *timer)
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
// 函数功能:__STM32Timer_DisInt
//          禁止定时器中断
// 输入参数:timer，待操作的定时器
// 输出参数:
// 返回值  :true成功false失败
// 说明    :
// =============================================================================
bool_t  __STM32Timer_DisInt(struct STM32TimerHandle  *timer)
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
// 函数功能:__STM32Timer_GetTime
//          获取定时器走时
// 输入参数:timer，待操作的定时器
// 输出参数:time，走时（微秒）
// 返回值  :true成功false失败
// 说明    :从设定的周期算起，即cycle-剩余时间,表示已经走掉的时间(单位：定时器主频时钟个数)
// =============================================================================
bool_t __STM32Timer_GetTime(struct STM32TimerHandle  *timer, u32 *time)
{
    u8 timerno;
    u32 counter;
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timerno = timer->timerno;
        if(timerno > CN_STM32TIMER_MAX)
        {
            return false;
        }
        counter = tg_TIMER_Reg[timerno]->CNT;   //CNT即是微秒数
        *time = counter;
        return true;
    }
    else
    {
        return false;
    }
}
// =============================================================================
// 函数功能:__STM32Timer_CheckTimeout
//          看定时器的定时时间是否已经到了
// 输入参数:timer，待操作的定时器
// 输出参数:timeout, true超时，false不超时
// 返回值  :true成功 false失败
// 说明    :
// =============================================================================
bool_t __STM32Timer_CheckTimeout(struct STM32TimerHandle  *timer, bool_t *timeout)
{
    bool_t result=true;
    u8 timerno;
    u32 temp;
    u32 counter;
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timerno = timer->timerno;
        if(timerno > CN_STM32TIMER_MAX)
        {
            result = false;
        }
        else
        {
            temp = tg_TIMER_Reg[timerno]->ARR;
            counter = tg_TIMER_Reg[timerno]->CNT;
            if(temp == counter)
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
// 函数功能：__STM32Timer_GetID
//          获取定时器ID
// 输入参数：timer，待操作定时器，
// 输出参数：timerId,高16位为TIMERNO,低16为对应的IRQNO
// 返回值  ：true 成功 false失败
// 说明    : 本层实现
// =============================================================================
bool_t __STM32Timer_GetID(struct STM32TimerHandle   *timer,u32 *timerId)
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
// 函数功能：__STM32Timer_GetCycle
//          获取定时器周期
// 输入参数：timer，待操作定时器，
// 输出参数：cycle，定时器周期(单位：定时器主频时钟个数)
// 返回值  ：true 成功 false失败
// =============================================================================
bool_t __STM32Timer_GetCycle(struct STM32TimerHandle   *timer, u32 *cycle)
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
// 函数功能：__STM32Timer_GetState
//          获取定时器状态
// 输入参数：timer,待操作的定时器
// 输出参数：timerflag，定时器状态
// 返回值  ：true 成功 false失败
// 说明    : 本层实现
// =============================================================================
bool_t __STM32Timer_GetState(struct STM32TimerHandle   *timer, u32 *timerflag)
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
// 函数功能:__STM32Timer_Ctrl
//          看定时器的定时时间是否已经到了
// 输入参数:timerhandle 待操作的定时器句柄
//         ctrlcmd, 操作命令码
// 输出参数:inoutpara,根据ctrlcmd的不同而不同
// 返回值  :true成功  false失败
// 说明    :
// =============================================================================
bool_t __STM32Timer_Ctrl(ptu32_t timerhandle, \
                         enum TimerCmdCode ctrlcmd, \
                         ptu32_t inoutpara)
{
    bool_t result = false;
    struct STM32TimerHandle  *timer;
    timer = (struct STM32TimerHandle  *)timerhandle;
    if(NULL != timer)
    {
        switch(ctrlcmd)
        {
            case EN_TIMER_STARTCOUNT:
                result = __STM32Timer_StartCount(timer);
                break;
            case EN_TIMER_PAUSECOUNT:
                result = __STM32Timer_PauseCount(timer);
                break;
            case EN_TIMER_SETCYCLE:
                result = __STM32Timer_SetCycle(timer,(u32)inoutpara);
                break;
            case EN_TIMER_SETRELOAD:
                result = __STM32Timer_SetAutoReload(timer, (bool_t)inoutpara);
                break;
            case EN_TIMER_ENINT:
                result = __STM32Timer_EnInt(timer);
                break;
            case EN_TIMER_DISINT:
                result = __STM32Timer_DisInt(timer);
                break;
            case EN_TIMER_SETINTPRO:
                result = __STM32Timer_SetIntPro(timer, (bool_t)inoutpara);
                break;
            case EN_TIMER_GETTIME:
                result = __STM32Timer_GetTime(timer, (u32 *)inoutpara);
                break;
            case EN_TIMER_GETCYCLE:
                result = __STM32Timer_GetCycle(timer, (u32 *)inoutpara);
                break;
            case EN_TIMER_GETID:
                result = __STM32Timer_GetID(timer, (u32 *)inoutpara);
                break;
            case EN_TIMER_GETSTATE:
                result = __STM32Timer_GetState(timer, (u32 *)inoutpara);
                break;
            default:
                break;
        };
    }

    return result;
}
// =============================================================================
// 函数功能:__P1020PicTimer_GetFreq
//       获取定时器主频
// 输入参数:timerhandle 待操作的定时器句柄
// 输出参数:
// 返回值  :定时器主频
// 说明    :单位（HZ）
// =============================================================================
u32  __STM32Timer_GetFreq(ptu32_t timerhandle)
{
    //定时器TIM234567时钟源为低速外设时钟PCLK1，速度为36M
    //定时器分频配置为36，则一个时钟为1uS，定时器主频为1MHz
    return 1000000;
}
// =============================================================================
// 函数功能:module_init_timer
//          P1020的PICtimer初始化,timer时钟源为APB2 = CN_CFG_PCLK1 = 36M(timer234567)
// 输入参数:
// 输出参数:
// 返回值  :
// 说明    :
// =============================================================================
void ModuleInstall_HardTimer(void)
{
    struct TimerChip  STM32timer;
    u32 temp;
    pg_rcc_reg->APB1ENR |= (1<<4)|(1<<5);//时钟使能,timer6/7的使能位为比特4/5
    //定时器TIM234567时钟源为低速外设时钟PCLK1，速度为36M
    for(temp = 0; temp < CN_STM32TIMER_NUM; temp++)
    {
        tg_TIMER_Reg[temp]->CR1 &= ~(TIM_CR1_CEN_MASK); //禁止TIMER
        tg_TIMER_Reg[temp]->CR1 |= TIM_CR1_ARPE_MASK;

        tg_TIMER_Reg[temp]->DIER |= TIM_DIER_UIE_MASK;
        tg_TIMER_Reg[temp]->PSC = 0x23;//配置为36，则一个时钟为1uS
        tg_TIMER_Reg[temp]->ARR = 0xFFFF;
    }

    STM32timer.chipname = "STM32TIMER";
    STM32timer.TimerHardAlloc = __STM32Timer_Alloc;
    STM32timer.TimerHardFree = __STM32Timer_Free;
    STM32timer.TimerHardCtrl = __STM32Timer_Ctrl;
    STM32timer.TimerHardGetFreq = __STM32Timer_GetFreq;
    TimerHard_RegisterChip(&STM32timer);

    return ;
}
