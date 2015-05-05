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
//所属模块: 硬件定时器
//作者：lst
//版本：V1.1.0
//文件描述: 用于44b0硬件定时器操作
//其他说明:
//修订历史:
//2. 日期: 2009-04-24
//   作者: lst
//   新版本号: V1.1.0
//   修改说明: 原djyos.c中定时功能与硬件相关的部分转移到这里。
//1. 日期: 2009-03-10
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 移植自44b0版本
//------------------------------------------------------
#include "stdint.h"
#include "cpu_peri.h"

//2440的定时器功能简述:
//1、减计数方式工作
//2、每个定时器可选5个时钟源，分别为主频的1/2分频~1/16分频，或EXTCLK。
//3、有两个8位预分频器，01共享一个，234共享一个。
//4、每个定时器都有比较寄存器，用于产生PWM波形。
//5、每个定时器可以设定为单次运行和重复运行，可以手动启动和停止。

void Timer_Init(void)
{
    //DMA:No select(All Interrupt),
    pg_timer_reg->TCFG1 = 0;
}

//----设置定时器时钟源----------------------------------------------------------
//功能: 设置各定时器时钟源.
//参数: timer,定时器号
//      value,定时器输入时钟源,timer和value的对应表如下:
//      value:  0           1           2            3          4
//      timer0: 1/2分频     1/4分频     1/8分频      1/16分频   EXTCLK0
//      timer1: 1/2分频     1/4分频     1/8分频      1/16分频   EXTCLK0
//      timer2: 1/2分频     1/4分频     1/8分频      1/16分频   EXTCLK1
//      timer3: 1/2分频     1/4分频     1/8分频      1/16分频   EXTCLK1
//      timer4: 1/2分频     1/4分频     1/8分频      1/16分频   EXTCLK1
//返回: 无
//-----------------------------------------------------------------------------
void Timer_SetClkSource(ufast_t timer,ufast_t value)
{
    pg_timer_reg->TCFG1 &= ~(0xf << (timer<<2));
    pg_timer_reg->TCFG1 |= value << (timer<<2);
}

//----设置定时器预分频数-------------------------------------------------------
//功能: 设置各定时器预分频数.
//参数: group,定时器组号,01为1组,234为1组
//      value,定时器预分频数,0~255对应1~256分频
//返回: 无
//-----------------------------------------------------------------------------
void Timer_SetPrecale(ufast_t group,uint16_t value)
{
    pg_timer_reg->TCFG0 &= ~(0xff << (group<<3));
    pg_timer_reg->TCFG0 |= value << (group<<3);
}

//----设置定时器计数值----------------------------------------------------------
//功能: 设置各定时器计数值.定时器的溢出中断时间为:value*(预分频+1)/时钟源分频数
//参数: timer,定时器号
//      value,计数值
//返回: 无
//-----------------------------------------------------------------------------
void Timer_SetCounter(ufast_t timer,uint16_t value)
{
    switch (timer)
    {
        case 0:
            pg_timer_reg->TCNTB0 = value;
            break;
        case 1:
            pg_timer_reg->TCNTB1 = value;
            break;
        case 2:
            pg_timer_reg->TCNTB2 = value;
            break;
        case 3:
            pg_timer_reg->TCNTB3 = value;
            break;
        case 4:
            pg_timer_reg->TCNTB4 = value;
            break;
        default:break;
    }
}

//----设置定时器比较值----------------------------------------------------------
//功能: 用于pwm占空比设计
//参数: timer,定时器号
//      value,比较值
//返回: 无
//-----------------------------------------------------------------------------
void Timer_SetCompare(ufast_t timer,uint16_t value)
{
    switch (timer)
    {
        case 0:
            pg_timer_reg->TCMPB0 = value;
            break;
        case 1:
            pg_timer_reg->TCMPB1 = value;
            break;
        case 2:
            pg_timer_reg->TCMPB2 = value;
            break;
        case 3:
            pg_timer_reg->TCMPB3 = value;
            break;
        default:break;
    }
}

//----设置定时器工作方式-------------------------------------------------------
//功能: 设定定时器是连续工作还是单次工作
//参数: timer,定时器号
//      type,0=单次工作,1=自动加载连续工作
//返回: 无
//-----------------------------------------------------------------------------
void Timer_SetType(ufast_t timer,ufast_t type)
{
    type &= 0x1;
    switch (timer)
    {
        case 0:
            pg_timer_reg->TCON &= ~(1<<3);
            pg_timer_reg->TCON |= type<<3;
            break;
        case 1:
            pg_timer_reg->TCON &= ~(1<<11);
            pg_timer_reg->TCON |= type<<11;
            break;
        case 2:
            pg_timer_reg->TCON &= ~(1<<15);
            pg_timer_reg->TCON |= type<<15;
            break;
        case 3:
            pg_timer_reg->TCON &= ~(1<<19);
            pg_timer_reg->TCON |= type<<19;
            break;
        case 4:
            pg_timer_reg->TCON &= ~(1<<22);
            pg_timer_reg->TCON |= type<<22;
            break;
        default:break;
    }
}

//----重载定时器计数和比较值---------------------------------------------------
//功能: 手动重新加载定时器的计数和比较寄存器值
//参数: timer,定时器号
//返回: 无
//-----------------------------------------------------------------------------
void Timer_Reload(ufast_t timer)
{
    switch (timer)
    {
        case 0:
            pg_timer_reg->TCON |= 1<<1;
            pg_timer_reg->TCON &= ~(1<<1);
            break;
        case 1:
            pg_timer_reg->TCON |= 1<<9;
            pg_timer_reg->TCON &= ~(1<<9);
            break;
        case 2:
            pg_timer_reg->TCON |= 1<<13;
            pg_timer_reg->TCON &= ~(1<<13);
            break;
        case 3:
            pg_timer_reg->TCON |= 1<<17;
            pg_timer_reg->TCON &= ~(1<<17);
            break;
        case 4:
            pg_timer_reg->TCON |= 1<<21;
            pg_timer_reg->TCON &= ~(1<<21);
            break;
        default:break;
    }
}

//----启动定时器--------------------------------------------------------
//功能: 启动定时器
//参数: timer,定时器号
//返回: 无
//-----------------------------------------------------------------------------
void Timer_Start(ufast_t timer)
{
    switch (timer)
    {
        case 0:
            pg_timer_reg->TCON |=1;
            break;
        case 1:
            pg_timer_reg->TCON |=1<<8;
            break;
        case 2:
            pg_timer_reg->TCON |=1<<12;
            break;
        case 3:
            pg_timer_reg->TCON |=1<<16;
            break;
        case 4:
            pg_timer_reg->TCON |=1<<20;
            break;
        default:break;
    }
}

//----停止定时器--------------------------------------------------------
//功能: 停止定时器
//参数: timer,定时器号
//返回: 无
//-----------------------------------------------------------------------------
void Timer_Stop(ufast_t timer)
{
    switch (timer)
    {
        case 0:
            pg_timer_reg->TCON &=~1;
            break;
        case 1:
            pg_timer_reg->TCON &=~(1<<8);
            break;
        case 2:
            pg_timer_reg->TCON &=~(1<<12);
            break;
        case 3:
            pg_timer_reg->TCON &=~(1<<16);
            break;
        case 4:
            pg_timer_reg->TCON &=~(1<<20);
            break;
        default:break;
    }
}

//----读定时器当前值--------------------------------------------------------
//功能: 读出定时器的当前计数值
//参数: timer,定时器号
//返回: 当前计数值
//-----------------------------------------------------------------------------
uint16_t Timer_Read(ufast_t timer)
{
    switch (timer)
    {
        case 0:
            return pg_timer_reg->TCNTO0;
            break;
        case 1:
            return pg_timer_reg->TCNTO1;
            break;
        case 2:
            return pg_timer_reg->TCNTO2;
            break;
        case 3:
            return pg_timer_reg->TCNTO3;
            break;
        case 4:
            return pg_timer_reg->TCNTO4;
            break;
        default:break;
    }
    return 0;
}

