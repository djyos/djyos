//----------------------------------------------------
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
//------------------------------------------------------
#include "stdint.h"
#include "cpu_peri.h"

//----选择引脚功能-------------------------------------------------------------
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
    pinsel = &pg_pin_config_reg->pinsel0;
    portoffset = (port<<1) + (pinnum>>4);
    bitoffset = pinnum % 16 <<1;
    pinsel[portoffset] = pinsel[portoffset] & (~((u32)0x3<<bitoffset))
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
//      gpio_cfg_pin_mode(0,1,cn_gpio_mode_od_pullup)
//-----------------------------------------------------------------------------
void gpio_cfg_pin_mode(u32 port,u32 pinnum,u32 mode)
{
    vu32  *pinmode;
    u32 bitoffset,portoffset;
    //先设置pinmode寄存器
    pinmode = &pg_pin_config_reg->pinmode0;
    portoffset = (port<<1) + (pinnum>>4);
    bitoffset = pinnum % 16 <<1;
    pinmode[portoffset] = pinmode[portoffset] & (~((u32)0x3<<bitoffset))
                        | (((u32)0x3 & mode)<<bitoffset);
    //再设置pinmode_od寄存器
    pinmode = &pg_pin_config_reg->pinmode_od0;
    portoffset = port;
    bitoffset = pinnum;
    pinmode[port] = pinmode[port] & (~((u32)1<<pinnum))
                        | (((u32)1 & (mode>>2))<<pinnum);
}

//----设置gpio为输入-----------------------------------------------------------
//功能: 把某组gpio中msk=1对应的gpio口设为输入口，引脚选为gpio时才有效。
//参数: port，被操作的port编号，比如要操作P1.5,则port=1.
//      mask，port中需要设为输入的未掩码
//返回: 无
//example:
//      gpio_setto_in(1,0x10000010);        //把p1.4,p1.28设为输入
//-----------------------------------------------------------------------------
void gpio_setto_in(u32 port,u32 msk)
{
    switch(port)
    {
        case 0:
            pg_gpio0_reg->dir &= ~msk;
            break;
        case 1:
            pg_gpio1_reg->dir &= ~msk;
            break;
        case 2:
            pg_gpio2_reg->dir &= ~msk;
            break;
        case 4:
            pg_gpio4_reg->dir &= ~msk;
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
//      gpio_setto_out(1,0x10000010);       //把p1.4,p1.28设为输出
//-----------------------------------------------------------------------------
void gpio_setto_out(u32 port,u32 msk)
{
    switch(port)
    {
        case 0:
            pg_gpio0_reg->dir |= msk;
            break;
        case 1:
            pg_gpio1_reg->dir |= msk;
            break;
        case 2:
            pg_gpio2_reg->dir |= msk;
            break;
        case 4:
            pg_gpio4_reg->dir |= msk;
            break;

        default:break;
    }
}

//----从gpio读取数据-----------------------------------------------------------
//功能: 把某gpio port整体读入
//参数: port，被操作的port编号，比如要操作P1,则port=1.
//返回: 读得的数据
//example:
//      data = GPIO_GetData(1);        //读p1口的数据
//-----------------------------------------------------------------------------
u32 GPIO_GetData(u32 port)
{
    u32 temp;
    switch(port)
    {
        case 0:
            temp = pg_gpio0_reg->pin;
            break;
        case 1:
            temp = pg_gpio1_reg->pin;
            break;
        case 2:
            temp = pg_gpio2_reg->pin;
            break;
        case 4:
            temp = pg_gpio4_reg->pin;
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
//      GPIO_OutData(1,data);      //数据输出到p1口
//-----------------------------------------------------------------------------
void GPIO_OutData(u32 port,u32 data)
{
    switch(port)
    {
        case 0:
            pg_gpio0_reg->pin = data;
            break;
        case 1:
            pg_gpio1_reg->pin = data;
            break;
        case 2:
            pg_gpio2_reg->pin = data;
            break;
        case 4:
            pg_gpio4_reg->pin = data;
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
//      GPIO_SettoHigh(1,0x10000010);      //在p1.4,p1.28输出高电平
//-----------------------------------------------------------------------------
void GPIO_SettoHigh(u32 port,u32 msk)
{
    switch(port)
    {
        case 0:
            pg_gpio0_reg->setpin = msk;
            break;
        case 1:
            pg_gpio1_reg->setpin = msk;
            break;
        case 2:
            pg_gpio2_reg->setpin = msk;
            break;
        case 4:
            pg_gpio4_reg->setpin = msk;
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
//      GPIO_SettoHigh(1,0x10000010);      //在p1.4,p1.28输出低电平
//-----------------------------------------------------------------------------
void GPIO_SettoLow(u32 port,u32 msk)
{
    switch(port)
    {
        case 0:
            pg_gpio0_reg->clrpin = msk;
            break;
        case 1:
            pg_gpio1_reg->clrpin = msk;
            break;
        case 2:
            pg_gpio2_reg->clrpin = msk;
            break;
        case 4:
            pg_gpio4_reg->clrpin = msk;
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
    pg_sysctrl_reg->pconp |= (u32)1<<15;
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

