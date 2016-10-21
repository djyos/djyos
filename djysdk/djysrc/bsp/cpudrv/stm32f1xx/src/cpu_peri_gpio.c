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
//文件描述:IO初始化
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "cpu_peri_gpio.h"
#include "cpu_peri_rcc.h"

//----选择引脚功能-------------------------------------------------------------
//功能: stm32的io设计比较特殊，作为输入端时，不用区分外设输入还是gpio输入，
//      均设置为输入即可。作为输出时，需要区分gpio输出和外设输出，但无需指
//      定具体外设，只需要设置为外设即可。
//参数: port，被操作的port编号，比如要操作P1.5,则port=1.
//      pinnum，port内编号，比如要操作P1.5,则pinnum=5.
//      func_no，功能编号，见 CN_GPIO_MODE_IN_ANALOG 那一组常量。
//返回: 无
//-----------------------------------------------------------------------------
void GPIO_CfgPinFunc(u32 port,u32 pinnum,u32 func_no)
{
    if(port >6)
        return;

    if(pinnum < 8 )
    {
        pg_gpio_reg[port]->CRL  = (pg_gpio_reg[port]->CRL & (~(0xf << (pinnum<<2))))
                                    | ((func_no & 0xf)<< (pinnum<<2));
    }
    else
    {
        pg_gpio_reg[port]->CRH  =(pg_gpio_reg[port]->CRH &(~(0xf <<((pinnum-8)<<2))))
                                    | ((func_no & 0xf)<< ((pinnum-8)<<2));
    }

    //输入功能的上下拉是通过输出寄存器来实现的。
    if(func_no == CN_GPIO_MODE_IN_PULLUP)
    {
        pg_gpio_reg[port]->BSRR = 1<<pinnum;
    }
    else if(func_no == CN_GPIO_MODE_IN_PULLDOWN)
    {
        pg_gpio_reg[port]->BRR = 1<<pinnum;
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
    if(port >6)
        return 0;
    return pg_gpio_reg[port]->IDR;
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
    if(port >6)
        return;
    pg_gpio_reg[port]->ODR = data;
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
    if(port >6)
        return;
    pg_gpio_reg[port]->BSRR = msk & 0xffff;
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
    if(port >6)
        return;
    pg_gpio_reg[port]->BRR = msk;
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
//参数: port，被操作的port编号，比如要操作PA,则port=CN_GPIO_A.
//返回: 无
//example:
//      GPIO_PowerOn(1);      //使能p1，
//-----------------------------------------------------------------------------
void GPIO_PowerOn(u32 port)
{
    //stm32没有关闭gpio电源的功能，只能关掉或打开他们的时钟。
    switch(port)
    {
        case 0:
            bb_rcc_apb2enr_iopaen = 1;
        break;
        case 1:
            bb_rcc_apb2enr_iopben = 1;
            break;
        case 2:
            bb_rcc_apb2enr_iopcen = 1;
            break;
        case 3:
            bb_rcc_apb2enr_iopden = 1;
            break;
        case 4:
            bb_rcc_apb2enr_iopeen = 1;
            break;
        case 5:
            bb_rcc_apb2enr_iopfen = 1;
            break;
        case 6:
            bb_rcc_apb2enr_iopgen = 1;
            break;

        default:break;
    }
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
//      GPIO_PowerOff(1);    //禁止p1
//-----------------------------------------------------------------------------
void GPIO_PowerOff(u32 port)
{
    //stm32没有关闭gpio电源的功能，只能关掉或打开他们的时钟。
    switch(port)
    {
        case 0:
            bb_rcc_apb2enr_iopaen = 0;
        break;
        case 1:
            bb_rcc_apb2enr_iopben = 0;
            break;
        case 2:
            bb_rcc_apb2enr_iopcen = 0;
            break;
        case 3:
            bb_rcc_apb2enr_iopden = 0;
            break;
        case 4:
            bb_rcc_apb2enr_iopeen = 0;
            break;
        case 5:
            bb_rcc_apb2enr_iopfen = 0;
            break;
        case 6:
            bb_rcc_apb2enr_iopgen = 0;
            break;

        default:break;
    }
}



