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
//Copyright (C), 2004-2009,  lst.
//版权所有 (C), 2004-2009,   lst.
//所属模块:IO初始化
//作者：lst
//版本：V1.0.0
//文件描述:本文件实现两个功能:
//          1、引脚功能选择，现在MCU的引脚，多是复用的，使用前需要正确地选择他们
//             的功能，甚至有些系统在运行过程中还要动态切换功能，有些CPU有独立的
//             模块用于引脚功能选择，有些则把这些功能附加在gpio模块上，djyos统一
//             在gpio模块中实现。
//          2、gpio工作模式设置，这是和具体硬件关联度非常高的功能.许多CPU允许用
//             户选择pin引脚上拉、下拉、工作速度、输出电流等选项。
//          3、都gpio引脚电平，以及控制gpio的输出。
//          5、以上未尽之功能，用户自行添加。
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------------------------------

#include "cpu_peri.h"

//----选择引脚功能--------------------------------------------------------------
//功能: 选择一个引脚的功能，MCU都会把外部引脚划分为若干port，从P0~Px进行编号，每
//      个port有若干引脚。
//参数: port，被操作的port编号，比如要操作P1.5,则port=1.
//      pinnum，port内编号，比如要操作P1.5,则pinnum=5.
//      func_no，功能编号，由具体系统定义
//返回: 无
//example:
//      GPIO_CfgPinFunc(0,1,cn_p0_0_txd3)     //把p0.1设置为uart3的txd
//-----------------------------------------------------------------------------
void GPIO_CfgPinFunc(u32 port,u32 pinnum,u32 func_no)
{
    vu32  *pinsel;
    u32 bitoffset,portoffset;
    pinsel = &LPC_PINCON->PINSEL0;
    portoffset = (port<<1) + (pinnum>>4);
    bitoffset = pinnum % 16 <<1;
    pinsel[portoffset] = (pinsel[portoffset] & (~((u32)0x3<<bitoffset)))
                        | (((u32)0x3 & func_no)<<bitoffset);
}

//----选择引脚模式-------------------------------------------------------------
//功能: 选择一个引脚的模式，MCU都会把外部引脚划分为若干port，从P0~Px进行编号，每
//      个port有若干引脚。
//参数: port，被操作的port编号，比如要操作P1.5,则port=1.
//      pinnum，port内编号，比如要操作P1.5,则pinnum=5.
//      mode，模式编号，由具体系统定义，在175x中，bit0~1用于配置pinmode，bit2用
//            于配置pinmode_od，即是否开漏。
//返回: 无
//example:
//      把p0.1设置为上拉+开漏输出
//      GPIO_CfgPinMode(0,1,cn_gpio_mode_od_pullup)
//-----------------------------------------------------------------------------
void GPIO_CfgPinMode(u32 port,u32 pinnum,u32 mode)
{
    vu32  *pinmode;
    u32 bitoffset,portoffset;
    //先设置pinmode寄存器
    pinmode = &LPC_PINCON->PINMODE0;
    portoffset = (port<<1) + (pinnum>>4);
    bitoffset = pinnum % 16 <<1;
    pinmode[portoffset] = (pinmode[portoffset] & (~((u32)0x3<<bitoffset)))
                        | (((u32)0x3 & mode)<<bitoffset);
    //再设置pinmode_od寄存器
    pinmode = &LPC_PINCON->PINMODE_OD0;
    portoffset = port;
    bitoffset = pinnum;
    pinmode[port] = (pinmode[port] & (~((u32)1<<pinnum)))
                        | (((u32)1 & (mode>>2))<<pinnum);
}

//----设置gpio为输入-----------------------------------------------------------
//功能: 把某组gpio中msk=1对应的gpio口设为输入口，引脚选为gpio时才有效。
//参数: port，被操作的port编号，比如要操作P1.5,则port=1.
//      mask，port中需要设为输入的未掩码
//返回: 无
//example:
//      GPIO_SetToIn(1,0x10000010);        //把p1.4,p1.28设为输入
//-----------------------------------------------------------------------------
void GPIO_SetToIn(u32 port,u32 msk)
{
    switch(port)
    {
        case 0:
        	LPC_GPIO0->FIODIR &= ~msk;
            break;
        case 1:
        	LPC_GPIO1->FIODIR &= ~msk;
            break;
        case 2:
        	LPC_GPIO2->FIODIR &= ~msk;
            break;
        case 3:
        	LPC_GPIO3->FIODIR &= ~msk;
            break;
        case 4:
        	LPC_GPIO4->FIODIR &= ~msk;
            break;
        default:break;
    }
}

//----设置gpio为输出-----------------------------------------------------------
//功能: 把某组gpio中msk中是1的bit对应的gpio口设为输出口，引脚选为gpio时才有效。
//参数: port，被操作的port编号，比如要操作P1.5,则port=1.
//      mask，port中需要设为输入的位掩码
//返回: 无
//example:
//      GPIO_SetToOut(1,0x10000010);       //把p1.4,p1.28设为输出
//-----------------------------------------------------------------------------
void GPIO_SetToOut(u32 port,u32 msk)
{
    switch(port)
    {
        case 0:
        	LPC_GPIO0->FIODIR |= msk;
            break;
        case 1:
        	LPC_GPIO1->FIODIR |= msk;
            break;
        case 2:
        	LPC_GPIO2->FIODIR |= msk;
            break;
        case 3:
        	LPC_GPIO3->FIODIR |= msk;
            break;
        case 4:
        	LPC_GPIO4->FIODIR |= msk;
            break;
        default:break;
    }
}

//----从gpio读取数据-----------------------------------------------------------
//功能: 把某gpio port整体读入
//参数: port，被操作的port编号，比如要操作P1,则port=1.
//返回: 读得的数据
//example:
//      data = GPIO_DataGet(1);        //读p1口的数据
//-----------------------------------------------------------------------------
u32 GPIO_DataGet(u32 port)
{
    u32 temp;
    switch(port)
    {
        case 0:
            temp = LPC_GPIO0->FIOPIN;
            break;
        case 1:
            temp = LPC_GPIO1->FIOPIN;
            break;
        case 2:
            temp = LPC_GPIO2->FIOPIN;
            break;
        case 3:
            temp = LPC_GPIO3->FIOPIN;
            break;
        case 4:
            temp = LPC_GPIO4->FIOPIN;
            break;
        default:break;
    }
    return temp;
}

//----输出数据到某port---------------------------------------------------------
//功能: 把数据整体输出到某gpio port
//参数: port，被操作的port编号，比如要操作P1,则port=1.
//      data，待输出的数据
//返回: 无
//example:
//      GPIO_DataOut(1,data);      //数据输出到p1口
//-----------------------------------------------------------------------------
void GPIO_DataOut(u32 port,u32 data)
{
    switch(port)
    {
        case 0:
        	LPC_GPIO0->FIOPIN = data;
            break;
        case 1:
        	LPC_GPIO1->FIOPIN = data;
            break;
        case 2:
        	LPC_GPIO2->FIOPIN = data;
            break;
        case 3:
        	LPC_GPIO3->FIOPIN = data;
            break;
        case 4:
        	LPC_GPIO4->FIOPIN = data;
            break;
        default:break;
    }
    return ;
}

//----IO口输出高电平-----------------------------------------------------------
//功能: 在port中msk中是1的bit对应的位输出高电平
//参数: port，被操作的port编号，比如要操作P1.5,则port=1.
//      mask，port中需要输出高电平的位掩码
//返回: 无
//example:
//      GPIO_SetToHigh(1,0x10000010);      //在p1.4,p1.28输出高电平
//-----------------------------------------------------------------------------
void GPIO_SetToHigh(u32 port,u32 msk)
{
    switch(port)
    {
        case 0:
        	LPC_GPIO0->FIOSET = msk;
            break;
        case 1:
        	LPC_GPIO1->FIOSET = msk;
            break;
        case 2:
        	LPC_GPIO2->FIOSET = msk;
            break;
        case 3:
        	LPC_GPIO3->FIOSET = msk;
            break;
        case 4:
        	LPC_GPIO4->FIOSET = msk;
            break;
        default:break;
    }
}

//----IO口输出低电平-----------------------------------------------------------
//功能: 在port中msk中是1的bit对应的位输出低电平
//参数: port，被操作的port编号，比如要操作P1.5,则port=1.
//      mask，port中需要输出高电平的位掩码
//返回: 无
//example:
//      GPIO_SetToLow(1,0x10000010);      //在p1.4,p1.28输出低电平
//-----------------------------------------------------------------------------
void GPIO_SetToLow(u32 port,u32 msk)
{
    switch(port)
    {
        case 0:
        	LPC_GPIO0->FIOCLR = msk;
            break;
        case 1:
        	LPC_GPIO1->FIOCLR = msk;
            break;
        case 2:
        	LPC_GPIO2->FIOCLR = msk;
            break;
        case 3:
        	LPC_GPIO3->FIOCLR = msk;
            break;
        case 4:
        	LPC_GPIO4->FIOCLR = msk;
            break;
        default:break;
    }
}

//----使能gpio模块-------------------------------------------------------------
//功能: 使能gpio模块，许多cpu为了控制功耗，其外设模块都是可以被关闭的，gpio模块
//      也是如此。有些cpu是整个gpio模块一起设置，有些则是不同port独立设置。对于
//      没有节能设计的cpu，本函数保持为空即可。
//      控制功耗的功能实现方法，不外乎两大类:
//      1、有一个统一的功耗管理模块。
//      2、功耗管理功能分散到各功能模块中去。
//      无论有无统一的功耗管理模块，djyos建议各外设驱动程序均自行实现一套控制自
//      身功耗的函数，而统一的功耗控制模块则不提供给用户。这样无论是否有统一的
//      功耗控制模块，应用程序都是一致的。
//参数: port，被操作的port编号，比如要操作P1,则port=1.
//返回: 无
//example:
//      GPIO_PowerOn(1);      //使能p1，但lpc不能按port使能，实际上使能全部gpio
//-----------------------------------------------------------------------------
void GPIO_PowerOn(u32 port)
{
    //使能整个gpio模块，gpio模块只有一个控制位，不能按port使能。
    LPC_SC->PCONP |= (u32)1<<15;
}

//----禁止gpio模块-------------------------------------------------------------
//功能: 使能gpio模块，许多cpu为了控制功耗，其外设模块都是可以被关闭的，gpio模块
//      也是如此。有些cpu是整个gpio模块一起设置，有些则是不同port独立设置。对于
//      没有节能设计的cpu，本函数保持为空即可。
//      控制功耗的功能实现方法，不外乎两大类:
//      1、有一个统一的功耗管理模块。
//      2、功耗管理功能分散到各功能模块中去。
//      无论有无统一的功耗管理模块，djyos建议各外设驱动程序均自行实现一套控制自
//      身功耗的函数，而统一的功耗控制模块则不提供给用户。这样无论是否有统一的
//      功耗控制模块，应用程序都是一致的。
//参数: port，被操作的port编号，比如要操作P1,则port=1.
//返回: 无
//example:
//      GPIO_PowerOff(1);    //禁止p1，但lpc不能按port使能，实际上禁止全部gpio
//-----------------------------------------------------------------------------
void GPIO_PowerOff(u32 port)
{
    //lpc_17xx没有但lpc不能按port使能，实际上禁止全部gpio,为了不影响其他模块，
    //本函数不执行任何操作
    return;
}

// =============================================================================
// 功能：配置GPIO外部中断的触发模式为上升沿或下降沿触发
// 参数： port，被操作的port编号，比如要操作P1.5,则port=1.
//      pinnum，port内编号，比如要操作P1.5,则pinnum=5.
//      mode，1表示上升沿触发，0表示下降沿触发
// 返回：无
// =============================================================================
void GPIO_CfgIntMode(u32 port,u32 pinnum,u32 mode)
{
	if((port != 0) && (port != 2))
		return;
	if(port == 0)
	{
		if(mode == CN_GPIO_INT_RISING)			//rising mode
		{
			LPC_GPIOINT->IO0IntEnR = 1<<pinnum;
		}
		else
		{
			LPC_GPIOINT->IO0IntEnF = 1<<pinnum;
		}
	}
	else
	{
		if(mode == CN_GPIO_INT_RISING)			//rising mode
		{
			LPC_GPIOINT->IO2IntEnR = 1<<pinnum;
		}
		else
		{
			LPC_GPIOINT->IO2IntEnF = 1<<pinnum;
		}
	}
}

// =============================================================================
// 功能：获取外部中断的状态，0表示无中断挂起，非0表示中断挂起
// 参数： port，被操作的port编号，比如要操作P1.5,则port=1.
//      msk，掩码，如操作1.5，msk = 1<<5
//      mode，1表示上升沿触发，0表示下降沿触发
// 返回：0，无中断；非0，中断挂起
// =============================================================================
u32 GPIO_GetIntStat(u32 port,u32 msk,u32 mode)
{
	u32 result = 0xFF;
	if(port == 0)
	{
		if(mode == CN_GPIO_INT_RISING)			//rising mode
		{
			result = LPC_GPIOINT->IO0IntStatR & msk;
		}
		else
		{
			result = LPC_GPIOINT->IO0IntStatF & msk;
		}
	}
	else
	{
		if(mode == CN_GPIO_INT_RISING)			//rising mode
		{
			result = LPC_GPIOINT->IO2IntStatR & msk;
		}
		else
		{
			result = LPC_GPIOINT->IO2IntStatF & msk;
		}
	}
	return result;
}

// =============================================================================
// 功能：清中断状态位
// 参数： port，被操作的port编号，比如要操作P1.5,则port=1.
//      msk，掩码，如操作1.5，msk = 1<<5
// 返回：无
// =============================================================================
void GPIO_ClearIntStat(u32 port,u32 msk)
{
	if(port == 0)
	{
		LPC_GPIOINT->IO0IntClr = msk;
	}
	else
	{
		LPC_GPIOINT->IO2IntClr = msk;
	}
}

// =============================================================================
// 功能：外部中断初始化
// 参数： exti,包括外部中断0，1，2，3
//       mode,包括1，边沿触发，0，水平触发
//       edge,1,高电平断或上升沿触发中，0，反之
// 返回：无
// =============================================================================
void ExtInt_Cfg(u8 exti,u8 mode,u8 edge)
{
	if(exti > 3)
		return ;

	if(mode)
		mode = 1;
	else
		mode = 0;

	if(edge)
		edge = 1;
	else
		edge = 0;

	LPC_SC->EXTMODE &= ~(1<<exti);
	LPC_SC->EXTPOLAR &= ~(1<<exti);

	LPC_SC->EXTMODE |= (mode << exti);
	LPC_SC->EXTPOLAR |= (edge << exti);
}

// =============================================================================
// 功能：清外部中断
// 参数： 外部中断号，0，1，2，3
// 返回：无
// =============================================================================
void ExtInt_FlagClear(u8 exti)
{
	if(exti > 3)
		return ;
	LPC_SC->EXTINT = (1 << exti);		//写1清除中断
}

