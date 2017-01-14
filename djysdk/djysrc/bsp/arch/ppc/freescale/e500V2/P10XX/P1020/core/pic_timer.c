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
#include "stdint.h"
#include "stddef.h"
#include "timer_hard.h"
#include "Pic.h"
#include "misc_config.h"
#include "int.h"
#include "cpu.h"
#include "cpu_peri_int_line.h"

enum ENUM_P1020PIC_TIMER
{
    EN_P1020PICTIMER_A0=0,
    EN_P1020PICTIMER_A1,
    EN_P1020PICTIMER_A2,
    EN_P1020PICTIMER_A3,
    EN_P1020PICTIMER_B0,
    EN_P1020PICTIMER_B1,
    EN_P1020PICTIMER_B2,
    EN_P1020PICTIMER_B3,
};

//各个定时器芯片的定时器应该有自己的句柄
struct P1020PicTimerHandle
{
    u32     timerno;          //定时器号
    u32     irqline;          //中断号
    u32     cycle;            //定时周期
    u32     timerstate;       //定时器标识
};
#define CN_P1020PICTIMER_NUM   (EN_P1020PICTIMER_B3 +1)


static struct P1020PicTimerHandle  stgP1020TimerHandle[CN_P1020PICTIMER_NUM];

//最高位代表timer0 依次类推
static u32  gs_dwP1020PicTimerBitmap;  //对于定时器这种东西，一般的不会很多，32个应该足够
#define CN_P1020PICTIMER_BITMAP_MSK  (0x80000000)  //最高位为1，依次右移即可

//timer0..timern的irq
static u32 sgHaltimerIrq[CN_P1020PICTIMER_NUM]={cn_int_line_gbtimer_a0,\
                                                cn_int_line_gbtimer_a1,\
                                                cn_int_line_gbtimer_a2,\
                                                cn_int_line_gbtimer_a3,\
                                                cn_int_line_gbtimer_b0,\
                                                cn_int_line_gbtimer_b1,\
                                                cn_int_line_gbtimer_b2,\
                                                cn_int_line_gbtimer_b3};
//获取32位数第一个0bit位置(从高位到低位算起)
u8 __P1020PicTimer_GetFirstZeroBit(u32 para)
{
   //采用硬件指令来获取
    para = ~para;
    extern u8 __asm_find_f1bits(u32 para);
    return  __asm_find_f1bits(para);
}
// =============================================================================
// 函数功能:__P1020PicTimer_Time2Counter
//          将提供的时间换算为定时器的计数单位
// 输入参数:time，要换算的时间(单位为微秒)
// 输出参数:counter,对应时间的counter
// 返回值  :true成功 fasle失败
// 说明    :time太大会失败，计算的counter则是定时器的最大值
// =============================================================================
bool_t __P1020PicTimer_Time2Counter(u32 time, u32 *counter)
{
    u32 time_max;
    u32 time_set;
    time_max = (cn_pic_timer_countermsk/1000) *cfg_core_tb_clk_time;
    time_set = time > time_max ? time_max : time;
    if(time_max > time)
    {
        time_set = time;
        *counter =  time_set *(1000/cfg_core_tb_clk_time);
        return true;
    }
    else
    {
        time_set = time_max;
        *counter =  time_set *(1000/cfg_core_tb_clk_time);
        return false;
    }
}
// =============================================================================
// 函数功能:__P1020PicTimer_Counter2Time
//          将定时器的计数单位换算为时间
// 输入参数:counter,定时器的counter单元
// 输出参数:time，换算的时间(单位为微秒)
// 返回值  :true成功 fasle失败
// 说明    :
// =============================================================================
bool_t __P1020PicTimer_Counter2Time(u32 counter,u32 *time)
{
    u32 time_max;
    u64 time_ns;
    time_max = (cn_pic_timer_countermsk/1000) *cfg_core_tb_clk_time;

    if(counter > cn_pic_timer_countermsk)
    {
        *time = time_max;
        return false;
    }
    else
    {
        time_ns = (u64)counter*cfg_core_tb_clk_time;
        *time = (u32)(time_ns/1000);
        return true;
    }
}
// =============================================================================
// 函数功能:__P1020PicTimer_PauseCount
//          停止计数
// 输入参数:timer，PIC定时器
// 输出参数:
// 返回值  :true成功 fasle失败
// 说明    :
// =============================================================================
bool_t __P1020PicTimer_PauseCount(struct P1020PicTimerHandle  *timer)
{
    u8 timerno;
    u32 temp;
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timerno = timer->timerno;
        if(timerno > EN_P1020PICTIMER_B3)
        {
            return false;
        }
        else
        {
            if(timerno > EN_P1020PICTIMER_A3)
            {
                temp = cn_pic_timer_gtbcrb_base + \
                        (timerno - EN_P1020PICTIMER_B0)*cn_pic_timer_gt_step;
                *((u32 *)temp) = *((u32 *)temp) | cn_pic_timer_msk;
            }
            else
            {
                temp = cn_pic_timer_gtbcra_base + \
                        (timerno - EN_P1020PICTIMER_A0)*cn_pic_timer_gt_step;
                *((u32 *)temp) = *((u32 *)temp) | cn_pic_timer_msk;
            }
            temp = *((u32 *)temp);//do the sync
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
// 函数功能:__P1020PicTimer_StartCount
//          开始计数
// 输入参数:timer，PIC定时器
// 输出参数:
// 返回值  :true成功 fasle失败
// 说明    :
// =============================================================================
bool_t __P1020PicTimer_StartCount(struct P1020PicTimerHandle  *timer)
{
    u8 timerno;
    u32 temp;
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timerno = timer->timerno;
        if(timerno > EN_P1020PICTIMER_B3)
        {
            return false;
        }
        else
        {
            if(timerno > EN_P1020PICTIMER_A3)
            {
                temp = cn_pic_timer_gtbcrb_base + \
                        (timerno - EN_P1020PICTIMER_B0)*cn_pic_timer_gt_step;
                *((u32 *)temp) = *((u32 *)temp) &(~cn_pic_timer_msk);
            }
            else
            {
                temp = cn_pic_timer_gtbcra_base + \
                        (timerno - EN_P1020PICTIMER_A0)*cn_pic_timer_gt_step;
                *((u32 *)temp) = *((u32 *)temp) &(~cn_pic_timer_msk);
            }
            temp = *((u32 *)temp);//do the sync
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
// 函数功能:__P1020PicTimer_SetCycle
//          设定周期
// 输入参数:timer，PIC定时器
//          cycle,周期（定时器时钟个数），
// 输出参数:
// 返回值  :true成功 fasle失败
// 说明    :如果设置周期太大（超过最大定时器能力），则设置为定时器的最大周期
// =============================================================================
bool_t  __P1020PicTimer_SetCycle(struct P1020PicTimerHandle  *timer, u32 cycle)
{
    u8 timerno;
    u32 temp;
    u32 counter;
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
//        __P1020PicTimer_Time2Counter(cycle,&counter);
        counter = cycle;
        timerno = timer->timerno;
        if(timerno > EN_P1020PICTIMER_B3)
        {
            return false;
        }
        else
        {
            if(timerno > EN_P1020PICTIMER_A3)
            {
                temp = cn_pic_timer_gtbcrb_base + \
                        (timerno - EN_P1020PICTIMER_B0)*cn_pic_timer_gt_step;
                *((u32 *)temp) = (*((u32 *)temp) &(~cn_pic_timer_countermsk)) |
                        (counter & cn_pic_timer_countermsk);
            }
            else
            {
                temp = cn_pic_timer_gtbcra_base + \
                        (timerno - EN_P1020PICTIMER_A0)*cn_pic_timer_gt_step;
                *((u32 *)temp) = (*((u32 *)temp) &(~cn_pic_timer_countermsk)) |
                        (counter & cn_pic_timer_countermsk);
            }
            temp = *((u32 *)temp);//do the sync

            timer->cycle = cycle;

            return true;
        }
    }
    else
    {
        return false;
    }
}
// =============================================================================
// 函数功能:__P1020PicTimer_SetAutoReload
//          设定周期
// 输入参数:timer，PIC定时器
//         autoreload, true则自己reload周期，否则手动每次设置周期
// 输出参数:
// 返回值  :true成功 fasle失败
// =============================================================================
bool_t  __P1020PicTimer_SetAutoReload(struct P1020PicTimerHandle  *timer, bool_t autoreload)
{
    bool_t result;
    u8 timerno;
    ptu32_t temp;
    u32 value;
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timerno = timer->timerno;
        if((timerno >= EN_P1020PICTIMER_A0) &&(timerno < EN_P1020PICTIMER_A3))
        {
            result = true;
            timerno = timerno - EN_P1020PICTIMER_A0;
            temp = (ptu32_t)cn_pic_timer_tcra;
            value = read32(temp);
            if(autoreload)
            {
                value = value &(u32)(~((1<<24)<<timerno));
                timer->timerstate = timer->timerstate | CN_TIMER_RELOAD;
            }
            else
            {
                value = value |(u32)((1<<24)<<timerno);
                timer->timerstate = timer->timerstate &(~CN_TIMER_RELOAD);
            }
            write32(temp, value);
        }
        else if((timerno >= EN_P1020PICTIMER_B0) &&(timerno < EN_P1020PICTIMER_B3))
        {
            result = true;
            timerno = timerno - EN_P1020PICTIMER_B0;
            temp = (ptu32_t)cn_pic_timer_tcrb;
            value = read32(temp);
            if(autoreload)
            {
                value = value &(u32)(~((1<<24)<<timerno));
                timer->timerstate = timer->timerstate | CN_TIMER_RELOAD;
            }
            else
            {
                value = value |(u32)((1<<24)<<timerno);
                timer->timerstate = timer->timerstate &(~CN_TIMER_RELOAD);
            }
            write32(temp, value);
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
// 函数功能:__P1020PicTimer_Alloc
//          分配定时器
// 输入参数:timerisr,定时器的中断处理函数
// 输出参数:
// 返回值  :分配的定时器句柄，NULL则分配不成功
// 说明    :
// =============================================================================
ptu32_t __P1020PicTimer_Alloc(fntTimerIsr timerisr)
{
    u8 timerno;
    u8 irqline;
    struct P1020PicTimerHandle  *timer;
    ptu32_t timerhandle;
    //原子操作，防止资源竞争
    atom_low_t  timeratom;
    timeratom = Int_LowAtomStart();

    //寻找空闲的timer
    timerno = __P1020PicTimer_GetFirstZeroBit(gs_dwP1020PicTimerBitmap);
    if(timerno < CN_P1020PICTIMER_NUM)//还有空闲的，则设置标志位
    {
        gs_dwP1020PicTimerBitmap = gs_dwP1020PicTimerBitmap | (CN_P1020PICTIMER_BITMAP_MSK<< timerno);
        Int_LowAtomEnd(timeratom);  //原子操作完毕
    }
    else//没有的话直接返回就可以了，用不着再啰嗦了
    {
        Int_LowAtomEnd(timeratom);   //原子操作完毕
        return NULL;
    }
    irqline = sgHaltimerIrq[timerno];
    timer = &stgP1020TimerHandle[timerno];
    timer->cycle = 0;
    timer->timerno = timerno;
    timer->irqline = irqline;
    timer->timerstate = CN_TIMER_ENUSE;
    //好了，中断号和定时器号码都有了，该干嘛就干嘛了。
    //先设置好定时器周期
    __P1020PicTimer_PauseCount(timer);
//    __P1020PicTimer_SetCycle(timer,cycle);
    //设置定时器中断,先结束掉该中断所有的关联相关内容
    Int_Register(irqline);
    Int_CutLine(irqline);
    Int_IsrDisConnect(irqline);
    Int_EvttDisConnect(irqline);
    Int_SettoAsynSignal(irqline);
    Int_SetClearType(irqline,CN_INT_CLEAR_AUTO);
    Int_IsrConnect(irqline, timerisr);
    timerhandle = (ptu32_t)timer;

    return timerhandle;
}


// =============================================================================
// 函数功能:__P1020PicTimer_Free
//          释放定时器
// 输入参数:timer，分配的定时器
// 输出参数:
// 返回值  :true成功false失败
// 说明    :
// =============================================================================
bool_t  __P1020PicTimer_Free(ptu32_t timerhandle)
{
    u8 timerno;
    u8 irqline;
    atom_low_t  timeratom;  //保护公用资源
    struct P1020PicTimerHandle  *timer;
    timer = (struct P1020PicTimerHandle  *)timerhandle;

    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timerno = timer->timerno;
        irqline = timer->irqline;
        if(timerno < CN_P1020PICTIMER_NUM)//还有空闲的，则设置标志位
        {       //修改全局标志一定是原子性的
            timeratom = Int_LowAtomStart();
            gs_dwP1020PicTimerBitmap = gs_dwP1020PicTimerBitmap &(~(CN_P1020PICTIMER_BITMAP_MSK<< timerno));
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
// 函数功能:__P1020PicTimer_SetIntPro
//          设置定时器的中断类型
// 输入参数:timer，待操作的定时器
//          real_prior为true则为实时信号否则为异步信号
// 输出参数:
// 返回值  :分配的定时器，NULL则分配不成功
// 说明    :
// =============================================================================
bool_t  __P1020PicTimer_SetIntPro(struct P1020PicTimerHandle  *timer, bool_t real_prior)
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
// 函数功能:__P1020PicTimer_EnInt
//          使能定时器中断
// 输入参数:timer，待操作的定时器
// 输出参数:
// 返回值  :true成功false失败
// 说明    :
// =============================================================================
bool_t  __P1020PicTimer_EnInt(struct P1020PicTimerHandle  *timer)
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
// 函数功能:__P1020PicTimer_DisInt
//          禁止定时器中断
// 输入参数:timer，待操作的定时器
// 输出参数:
// 返回值  :true成功false失败
// 说明    :
// =============================================================================
bool_t  __P1020PicTimer_DisInt(struct P1020PicTimerHandle  *timer)
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
// 函数功能:__P1020PicTimer_GetTime
//          获取定时器走时
// 输入参数:timer，待操作的定时器
// 输出参数:time，走时（微秒）
// 返回值  :true成功false失败
// 说明    :从设定的周期算起，即cycle-剩余时间,表示已经走掉的时间(单位：定时器主频时钟个数)
// =============================================================================
bool_t __P1020PicTimer_GetTime(struct P1020PicTimerHandle  *timer, u32 *time)
{
    u8 timerno;
    u32 temp;
    u32 counter;
    u32 basecounter;
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timerno = timer->timerno;
        if(timerno > EN_P1020PICTIMER_B3)
        {
            return false;
        }
        else
        {
            if(timerno > EN_P1020PICTIMER_A3)
            {
                //got the current counter
                temp = cn_pic_timer_gtccrb_base + \
                        (timerno - EN_P1020PICTIMER_B0)*cn_pic_timer_gt_step;
                counter = (*(u32 *)temp)&cn_pic_timer_countermsk;
                //got the base counter
                temp = cn_pic_timer_gtbcrb_base + \
                        (timerno - EN_P1020PICTIMER_B0)*cn_pic_timer_gt_step;
                basecounter = (*(u32 *)temp)&cn_pic_timer_countermsk;

                counter = basecounter - counter;
            }
            else
            {
                temp = cn_pic_timer_gtccra_base + \
                        (timerno - EN_P1020PICTIMER_A0)*cn_pic_timer_gt_step;
                counter = (*(u32 *)temp)&cn_pic_timer_countermsk ;
                //got the base counter
                temp = cn_pic_timer_gtbcra_base + \
                        (timerno - EN_P1020PICTIMER_A0)*cn_pic_timer_gt_step;
                basecounter = (*(u32 *)temp)&cn_pic_timer_countermsk;

                counter = basecounter - counter;

            }
//            return __P1020PicTimer_Counter2Time(counter, time);
            *time = counter;
            return true;
        }
    }
    else
    {
        return false;
    }
}
// =============================================================================
// 函数功能:__P1020PicTimer_CheckTimeout
//          看定时器的定时时间是否已经到了
// 输入参数:timer，待操作的定时器
// 输出参数:timeout, true超时，false不超时
// 返回值  :true成功 false失败
// 说明    :
// =============================================================================
bool_t __P1020PicTimer_CheckTimeout(struct P1020PicTimerHandle  *timer, bool_t *timeout)
{
    bool_t result;
    u8 timerno;
    u32 temp;
    u32 counter;
    if(timer->timerstate & CN_TIMER_ENUSE)
    {
        timerno = timer->timerno;
        if(timerno > EN_P1020PICTIMER_B3)
        {
            result = false;
        }
        else
        {
            result = true;
            if(timerno > EN_P1020PICTIMER_A3)
            {
                //got the current counter
                temp = cn_pic_timer_gtccrb_base + \
                        (timerno - EN_P1020PICTIMER_B0)*cn_pic_timer_gt_step;
            }
            else
            {
                temp = cn_pic_timer_gtccra_base + \
                        (timerno - EN_P1020PICTIMER_A0)*cn_pic_timer_gt_step;
            }
            counter = (*(u32 *)temp)&cn_pic_timer_togmsk;
            if(counter)
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
// 函数功能：__P1020PicTimer_GetID
//          获取定时器ID
// 输入参数：timer，待操作定时器，
// 输出参数：timerId,高16位为TIMERNO,低16为对应的IRQNO
// 返回值  ：true 成功 false失败
// 说明    : 本层实现
// =============================================================================
bool_t __P1020PicTimer_GetID(struct P1020PicTimerHandle   *timer,u32 *timerId)
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
// 函数功能：__P1020PicTimer_GetCycle
//          获取定时器周期
// 输入参数：timer，待操作定时器，
// 输出参数：cycle，定时器周期(单位：定时器主频时钟个数)
// 返回值  ：true 成功 false失败
// =============================================================================
bool_t __P1020PicTimer_GetCycle(struct P1020PicTimerHandle   *timer, u32 *cycle)
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
// 函数功能：__P1020PicTimer_GetState
//          获取定时器状态
// 输入参数：timer,待操作的定时器
// 输出参数：timerflag，定时器状态
// 返回值  ：true 成功 false失败
// 说明    : 本层实现
// =============================================================================
bool_t __P1020PicTimer_GetState(struct P1020PicTimerHandle   *timer, u32 *timerflag)
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
// 函数功能:__P1020PicTimer_Ctrl
//          看定时器的定时时间是否已经到了
// 输入参数:timerhandle 待操作的定时器句柄
//         ctrlcmd, 操作命令码
// 输出参数:inoutpara,根据ctrlcmd的不同而不同
// 返回值  :true成功  false失败
// 说明    :
// =============================================================================
bool_t __P1020PicTimer_Ctrl(ptu32_t timerhandle, \
                         enum TimerCmdCode ctrlcmd, \
                         ptu32_t inoutpara)
{
    bool_t result;
    struct P1020PicTimerHandle  *timer;
    timer = (struct P1020PicTimerHandle  *)timerhandle;
    if(NULL == timer)
    {
        result = false;
    }
    else
    {
        switch(ctrlcmd)
        {
            case EN_TIMER_STARTCOUNT:
                result = __P1020PicTimer_StartCount(timer);
                break;
            case EN_TIMER_PAUSECOUNT:
                result = __P1020PicTimer_PauseCount(timer);
                break;
            case EN_TIMER_SETCYCLE:
                result = __P1020PicTimer_SetCycle(timer,(u32)inoutpara);
                break;
            case EN_TIMER_SETRELOAD:
                result = __P1020PicTimer_SetAutoReload(timer, (bool_t)inoutpara);
                break;
            case EN_TIMER_ENINT:
                result = __P1020PicTimer_EnInt(timer);
                break;
            case EN_TIMER_DISINT:
                result = __P1020PicTimer_DisInt(timer);
                break;
            case EN_TIMER_SETINTPRO:
                result = __P1020PicTimer_SetIntPro(timer, (bool_t)inoutpara);
                break;
            case EN_TIMER_GETTIME:
                result = __P1020PicTimer_GetTime(timer, (u32 *)inoutpara);
                break;
            case EN_TIMER_GETCYCLE:
                result = __P1020PicTimer_GetCycle(timer, (u32 *)inoutpara);
                break;
            case EN_TIMER_GETID:
                result = __P1020PicTimer_GetID(timer, (u32 *)inoutpara);
                break;
            case EN_TIMER_GETSTATE:
                result = __P1020PicTimer_GetState(timer, (u32 *)inoutpara);
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
u32  __P1020PicTimer_GetFreq(ptu32_t timerhandle)
{

    return cfg_core_tb_clk;
}

// =============================================================================
// 函数功能:module_init_timer
//          P1020的PICtimer初始化
// 输入参数:
// 输出参数:
// 返回值  :
// 说明    :
// =============================================================================
void module_init_pictimer(void)
{
    struct TimerChip  p1020pictimer;
    u32 temp;
    //做基本的初始化
    *((u32 *)cn_pic_timer_tcra) = cn_pic_timer_tcr_set;
    temp = *((u32 *)cn_pic_timer_tcra);//sync
    *((u32 *)cn_pic_timer_tcrb) = cn_pic_timer_tcr_set;
    temp = *((u32 *)cn_pic_timer_tcrb);//sync

    *((u32 *)cn_pic_timer_tfrra) = cfg_core_tb_clk;  //same as the tb
    temp = *((u32 *)cn_pic_timer_tfrra);
    *((u32 *)cn_pic_timer_tfrrb) = cfg_core_tb_clk;  //same as the tb
    temp = *((u32 *)cn_pic_timer_tfrra);

    //all the timers are disabled,not count
    //initial the group a
    for(temp =0; temp < cn_pic_timer_gnumber; temp++)
    {
         *((u32 *)(cn_pic_timer_gtbcra_base + temp * cn_pic_timer_gt_step)) = cn_pic_timer_msk;
    }

    //all the timers are disabled,not count
    //initial the group b
    for(temp =0; temp < cn_pic_timer_gnumber; temp++)
    {
         *((u32 *)(cn_pic_timer_gtbcrb_base + temp * cn_pic_timer_gt_step)) = cn_pic_timer_msk;
    }
    p1020pictimer.chipname = "P1020PICTIMER";
    p1020pictimer.HardTimerAlloc = __P1020PicTimer_Alloc;
    p1020pictimer.HardTimerFree = __P1020PicTimer_Free;
    p1020pictimer.HardTimerCtrl = __P1020PicTimer_Ctrl;
    p1020pictimer.HardTimerGetFreq = __P1020PicTimer_GetFreq;

    HardTimer_RegisterChip(&p1020pictimer);

    return ;
}
