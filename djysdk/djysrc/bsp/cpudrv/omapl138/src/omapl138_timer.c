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
//========================================================
// 文件模块说明:
// omapl138定时器操作函数
// 文件版本: v1.00
// 开发人员: lst
// 创建时间: 2011.06.01
// Copyright(c) 2011-2011  深圳鹏瑞软件有限公司
//========================================================
// 程序修改记录(最新的放在最前面):
//  <版本号> <修改日期>, <修改人员>: <修改功能概述>
//========================================================
//例如：
// V1.02 2004-07-14, 张三: 接收函数增加对报文长度有效性校核


//omapl138的定时器功能简述:
//有个64位定时器，每个64位定时器又可以作为2个32位定时器(t12和t34)。
//Timer64P1的64位模式可作为看门狗使用，但32位模式不可以。
//32位方式，又分为两两连接成菊花链的方式和独立32位方式。
//菊花链方式，实际上是把t34作为t12的预分频器。
//独立模式下，t34有一个4bit的预分频器，t12没有。
//非菊花链模式下，t12有8个32位的比较寄存器

tagTimerReg *__timer_get_timer_reg(u32 t64_no)
{
    switch(t64_no)
    {
        case 0:
            return (tagTimerReg *)g_ptTimer0Reg;
        case 1:
            return (tagTimerReg *)g_ptTimer1Reg;
        case 2:
            return (tagTimerReg *)g_ptTimer2Reg;
        case 3:
            return (tagTimerReg *)g_ptTimer3Reg;
        default:
            return NULL;
    }
}


//----设置64还是双32位模式-----------------------------------------------------
//功能: 设置定时器工作在64位模式还是双32位模式下。
//参数: t64_no,被设置的64位timer，0~3
//      mode,工作模式，0=64bit，1=双32模式，2=64位看门狗(仅t64-1),3=32位链接式。
//返回: 无
//-----------------------------------------------------------------------------
void timer_counter_len_set(u32 t64_no,ufast_t mode)
{
    tagTimerReg * timer;
    if(t64_no > 3)
        return;
    timer = __timer_get_timer_reg(t64_no);
    timer->TGCR &= ~((u32)0x0c);
    timer->TGCR |= mode<<2;
}

//----使能/禁止中断------------------------------------------------------------
//功能: 使能或禁止timer产生中断，注意，这里只是控制timer模块本身，要想响应timer
//      中断，还要在中断控制器中也使能相应的中断线。
//参数: t64_no,被设置的64位timer，0~3
//      msk,被操作的中断功能:
//          =EVTINTEN34  t34事件捕获中断
//          =PRDINTEN34  t34周期匹配中断
//          =EVTINTEN12  t12事件捕获中断
//          =PRDINTEN12  t12周期匹配中断
//      en，false=禁止，true=使能
//返回: 无
//-----------------------------------------------------------------------------
void timer_int_ctrl(u32 t64_no,u32 msk,bool_t en)
{
    tagTimerReg * timer;
    if(t64_no > 3)
        return;
    timer = __timer_get_timer_reg(t64_no);
    if(en)
        timer->INTCTLSTAT |= msk;
    else
        timer->INTCTLSTAT &= ~msk;
}

//----设置时钟源---------------------------------------------------------------
//功能: 设置定时器的时钟源，内部晶振或者外部引脚。
//参数: t64_no,被设置的64位timer，0~3
//      sub，被设置的是哪个32位定时器:0=t12或者64位模式，1=t34
//      src，时钟源，0=内部时钟，1=外部时钟
//返回: 无
//-----------------------------------------------------------------------------
void timer_set_clksrc(u32 t64_no,u32 sub,u32 src)
{
    tagTimerReg * timer;
    u32 offset;
    if(t64_no > 3)
        return;
    timer = __timer_get_timer_reg(t64_no);
    if(sub ==0 )
        offset = CLKSRC12_64_OFF;
    else
        offset = CLKSRC34_OFF;

    if(src == 0)
        timer->TCR &= ~(u32)(1<<offset);
    else
        timer->TCR |= 1<<offset;
}

//----设置工作模式-------------------------------------------------------------
//功能: 设置定时器的工作模式。
//参数: t64_no,被设置的64位timer，0~3
//      sub，被设置的是哪个32位定时器:0=t12，1=t34
//      mode,工作模式，0=不工作，1=单次计数，2=周期工作，3=周期工作并重载
//返回: 无
//-----------------------------------------------------------------------------
void timer_set_mode(u32 t64_no,u32 sub,u32 mode)
{
    tagTimerReg * timer;
    u32 offset;
    if(t64_no > 3)
        return;
    timer = __timer_get_timer_reg(t64_no);
    if(sub ==0 )
        offset = ENAMODE12_OFF;
    else
        offset = ENAMODE34_OFF;

    timer->TCR &= ~(u32)(0x3<<offset);
    timer->TCR |= mode<<offset;
}

//----设置定时周期-------------------------------------------------------------
//功能: 设置定时周期
//参数: t64_no,被设置的64位timer，0~3
//      sub，被设置的是哪个32位定时器:0=t12，1=t34
//      value,周期值
//返回: 无
//-----------------------------------------------------------------------------
void timer_set_Period(u32 t64_no,u32 sub,u32 value)
{
    tagTimerReg * timer;
    if(t64_no > 3)
        return;
    timer = __timer_get_timer_reg(t64_no);
    if(sub == 0)
        timer->PRD12 = value;
    else
        timer->PRD34 = value;
}

//----设置重载周期-------------------------------------------------------------
//功能: 设置重载周期，当定时器工作在周期性重载方式时有用
//参数: t64_no,被设置的64位timer，0~3
//      sub，被设置的是哪个32位定时器:0=t12，1=t34
//      value,周期值
//返回: 无
//-----------------------------------------------------------------------------
void timer_set_reload(u32 t64_no,u32 sub,u32 value)
{
    tagTimerReg * timer;
    if(t64_no > 3)
        return;
    timer = __timer_get_timer_reg(t64_no);
    if(sub == 0)
        timer->REL12 = value;
    else
        timer->REL34 = value;
}

//----停止定时器-------------------------------------------------------------
//功能: 停止定时器
//参数: t64_no,被设置的64位timer，0~3
//      sub，被设置的是哪个32位定时器:0=t12，1=t34，2=t64
//返回: 无
//-----------------------------------------------------------------------------
void Timer_Stop(u32 t64_no,u32 sub)
{
    tagTimerReg * timer;
    if(t64_no > 3)
        return;
    timer = __timer_get_timer_reg(t64_no);
    if(sub == 0)
        timer->TGCR &= ~(u32)1;
    else if(sub == 1)
        timer->TGCR &= ~(u32)2;
    else
        timer->TGCR &= ~(u32)3;
}

//----启动定时器-------------------------------------------------------------
//功能: 启动定时器
//参数: t64_no,被设置的64位timer，0~3
//      sub，被设置的是哪个32位定时器:0=t12，1=t34，2=t64
//返回: 无
//-----------------------------------------------------------------------------
void Timer_Start(u32 t64_no,u32 sub)
{
    tagTimerReg * timer;
    if(t64_no > 3)
        return;
    timer = __timer_get_timer_reg(t64_no);
    if(sub == 0)
        timer->TGCR |= 1;
    else if(sub == 1)
        timer->TGCR |= 2;
    else
        timer->TGCR |= 3;
}

//----清除计数值---------------------------------------------------------------
//功能: 清除定时器当前计数值，
//参数: t64_no,被设置的64位timer，0~3
//      sub，清除哪个32位定时器:0=t12，1=t34
//返回: 无
//注: 定时器运行中无法清除计数值，必须先停下。
//-----------------------------------------------------------------------------
void timer_clear(u32 t64_no,u32 sub)
{
    tagTimerReg * timer;
    if(t64_no > 3)
        return;
    timer = __timer_get_timer_reg(t64_no);
    if(sub == 0)
        timer->TIM12 = 0;
    else if(sub == 1)
        timer->TIM34 = 0;
}

//----读计数值---------------------------------------------------------------
//功能: 清除定时器当前计数值，
//参数: t64_no,被设置的64位timer，0~3
//      sub，读哪个32位定时器:0=t12，1=t34
//返回: 计数值
//-----------------------------------------------------------------------------
u32 Timer_Read(u32 t64_no,u32 sub)
{
    tagTimerReg * timer;
    if(t64_no > 3)
        return 0;
    timer = __timer_get_timer_reg(t64_no);
    if(sub == 0)
        return timer->TIM12;
    else
        return timer->TIM34;
}


