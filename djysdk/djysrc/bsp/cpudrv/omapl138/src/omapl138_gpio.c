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
//          4、以上未尽之功能，用户自行添加。
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

const u16 u16c_pinmux_table[] =
{
     60, 56, 52, 48, 44, 40, 36, 32, 28, 24, 20, 16, 12,  8,  4,  0,
    156,152,148,144,140,136,132,128, 96, 88, 84, 80, 76, 72, 68, 64,
    220,216,212,208,204,200,196,192,188,184,180,176,172,168,164,160,
    284,280,276,272,268,264,260,256,252,248,244,240,236,232,228,224,
    348,344,340,336,332,328,324,320,316,312,308,304,300,296,292,288,
    412,408,404,400,396,392,388,384,380,376,372,368,364,360,356,352,
    632,628,624,620,616,516,452,448,444,440,436,432,428,424,420,416,
    580,576,572,568,564,560,556,552,548,544,540,536,532,528,524,520,
    636,124,120,116,112,108,104, 92,612,608,604,600,596,592,588,584,
    //以下引脚无gpio功能。
    512,508,504,500,496,492,488,484,480,476,472,468,464,460,456,100,
};

//----选择引脚功能-------------------------------------------------------------
//功能: 选择一个引脚的功能，MCU都会把外部引脚划分为若干port，从P0~Px进行编号，每
//      个port有若干引脚。
//参数: port，被操作的port编号，比如要操作P1.5,则port=1.
//      pinnum，port内编号，比如要操作P1.5,则pinnum=5.
//      func_no，功能编号，由具体系统定义
//返回: 无
//example:
//      gpio_cfg_pin_mode(0,1,cn_p0_0_txd3)     //把p0.1设置为uart3的txd
//-----------------------------------------------------------------------------
void GPIO_CfgPinFunc(u32 port,u32 pinnum,u32 func_no)
{
    vu32  *pinsel;
    u32 bitoffset,portoffset,temp;
    g_ptSysconfig0Reg->KICKR[0] = KICK0R_UNLOCK;
    g_ptSysconfig0Reg->KICKR[1] = KICK1R_UNLOCK;

    pinsel = g_ptSysconfig0Reg->PINMUX;
    if(port <= 8)
    {
        temp = u16c_pinmux_table[port*16+pinnum];
    }else
    {
        temp = u16c_pinmux_table[9*16+pinnum];
    }
    portoffset = temp/32;
    bitoffset = temp % 32;
    pinsel[portoffset] = (pinsel[portoffset] & (~((u32)0xf<<bitoffset)))
                        | (((u32)0xf & func_no)<<bitoffset);

    g_ptSysconfig0Reg->KICKR[0] = KICK0R_LOCK;
    g_ptSysconfig0Reg->KICKR[1] = KICK1R_LOCK;
}

//----选择引脚模式-------------------------------------------------------------
//功能: 选择一个引脚的模式，MCU都会把外部引脚划分为若干port，从P0~Px进行编号，每
//      个port有若干引脚。
//参数: port，被操作的port编号，比如要操作P1.5,则port=1.
//      pinnum，port内编号，比如要操作P1.5,则pinnum=5.
//      mode，模式编号，由具体系统定义
//返回: 无
//example:
//      把p0.1设置为上拉+开漏输出
//      gpio_cfg_pin_mode(0,1,cn_pin_pull_up+cn_gpio_od_en)
//-----------------------------------------------------------------------------
void gpio_cfg_pin_mode(u32 port,u32 pinnum,u32 mode)
{
    //L138的上下拉模式，实在难写，就算了吧。
    //138的管脚模式不是独立控制的，而是把160个功能pin，分为32组，组内的上下拉方
    //式由一个bit控制。但其每组的数量和排列，并无一定的规律，很难写出规范的函数，
    //需要查数据手册直接控制寄存器。
    //g_ptSysconfig1Reg->PUDN_ENA控制各组上下拉使能。
    //g_ptSysconfig1Reg->PUDN_SEL控制各组上下拉方式。
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
    g_ptGpioReg->bank[(port)>>1].dir |=(u32)(msk)<<(((port)&1)*16);
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
    g_ptGpioReg->bank[(port)>>1].dir&=~((u32)(msk)<<(((port)&1)*16));
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
    return ((g_ptGpioReg->bank[(port)>>1].in_data)>>(((port)&1)*16));
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
    g_ptGpioReg->bank[(port)>>1].out_data = (data)<<(((port)&1)*16);
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
    g_ptGpioReg->bank[(port)>>1].set_data=(u32)(msk)<<(((port)&1)*16);
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
    g_ptGpioReg->bank[(port)>>1].clr_data=(u32)(msk)<<(((port)&1)*16);
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
    Cpucfg_EnableLPSC(cn_PSC1_GPIO);
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
    //禁止整个gpio模块，gpio模块只有一个控制位，不能按port使能。
    Cpucfg_DisableLPSC(cn_PSC1_GPIO);
}

