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
//所属模块: uart驱动程序(公共接口部分)
//作者:  罗侍田.
//版本：V1.0.0
//文件描述:
//其他说明:
//修订历史:
//1. 日期:2013-11-10
//   作者:  罗侍田.
//   新版本号：1.0.0
//   修改说明: 移植字44b0的1.0.1版
//-----------------------------------------------------------------------------
#ifndef __UARTCTRL_H__
#define __UARTCTRL_H__
#include "driver.h"
#include "ring.h"

#ifdef __cplusplus
extern "C" {
#endif
// 串口波特率定义
enum tagUartBaud
{
    CN_UART_BAUD_110     =  110,
    CN_UART_BAUD_300     =  300,
    CN_UART_BAUD_600     =  600,
    CN_UART_BAUD_1200    =  1200,
    CN_UART_BAUD_2400    =  2400,
    CN_UART_BAUD_4800    =  4800,
    CN_UART_BAUD_9600    =  9600,
    CN_UART_BAUD_19200   =  19200,
    CN_UART_BAUD_38400   =  38400,
    CN_UART_BAUD_57600   =  57600,
    CN_UART_BAUD_115200  =  115200,
    CN_UART_BAUD_230400  =  230400,
    CN_UART_BAUD_460800  =  460800,
    CN_UART_BAUD_921600  =  921600,
};

// 串口数据位
enum tagUartDataBits
{
    CN_UART_DATABITS_5  = 5,
    CN_UART_DATABITS_6  = 6,
    CN_UART_DATABITS_7  = 7,
    CN_UART_DATABITS_8  = 8,
    CN_UART_DATABITS_9  = 9
};

// 串口奇偶校验
enum tagUartParity
{
    CN_UART_PARITY_NONE = 0,
    CN_UART_PARITY_ODD,
    CN_UART_PARITY_EVEN,
    CN_UART_PARITY_MARK,
    CN_UART_PARITY_SPACE
};

// 串口停止位
enum tagUartStopBits
{
    CN_UART_STOPBITS_1 = 0,
    CN_UART_STOPBITS_1_5,
    CN_UART_STOPBITS_2
};

// 串口传输参数结构体
struct COMParam
{
    enum tagUartBaud       BaudRate;
    enum tagUartDataBits   DataBits;
    enum tagUartParity     Parity;
    enum tagUartStopBits   StopBits;
}tagComParam;

//串口错误类型,使用MULPLEX的SENSINGBIT第4个比特到第31个比特
#define CN_UART_FIFO_OVER_ERR    (1<<3) //硬件缓冲区溢出错误
#define CN_UART_BUF_OVER_ERR     (1<<4) //接收缓冲区溢出错误
#define CN_UART_HARD_COMM_ERR    (1<<5) //串口硬件通信错误

//串口设备控制命令常数，用于uart_ctrl函数。
#define CN_UART_START                 (CN_DRV_CTRL_USER+0 )  //启动串口
#define CN_UART_STOP                  (CN_DRV_CTRL_USER+1 )  //停止串口
#define CN_UART_SET_BAUD              (CN_DRV_CTRL_USER+2 )  //设置uartBaud.
#define CN_UART_COM_SET               (CN_DRV_CTRL_USER+3 )  //设置串口参数
#define CN_UART_HALF_DUPLEX_SEND      (CN_DRV_CTRL_USER+4 )  //发送数据
#define CN_UART_HALF_DUPLEX_RECV      (CN_DRV_CTRL_USER+5 )  //接收数据
#define CN_UART_COMPLETED_SEND        (CN_DRV_CTRL_USER+6 )  //完成发送工作
#define CN_UART_RX_PAUSE              (CN_DRV_CTRL_USER+7 )  //暂停接收数据到缓冲区
#define CN_UART_RX_RESUME             (CN_DRV_CTRL_USER+8 )  //重新开始接收数据到缓冲区
#define CN_UART_RX_OVERTIME           (CN_DRV_CTRL_USER+9 )  //设置接收超时时间，毫秒数
#define CN_UART_RECV_SOFT_LEVEL       (CN_DRV_CTRL_USER+10)  //设置接收软件缓冲区触发水平
#define CN_UART_SEND_SOFT_LEVEL       (CN_DRV_CTRL_USER+11)  //设置接收软件缓冲区触发水平
#define CN_UART_RECV_HARD_LEVEL       (CN_DRV_CTRL_USER+12)  //设置接收fifo触发水平
#define CN_UART_SEND_HARD_LEVEL       (CN_DRV_CTRL_USER+13)  //设置发送fifo触发水平
#define CN_UART_RECV_ERROR_EVTT       (CN_DRV_CTRL_USER+14)  //设置错误弹出事件类型
#define CN_UART_EN_RTS                (CN_DRV_CTRL_USER+15)  //使能rts流控
#define CN_UART_DIS_RTS               (CN_DRV_CTRL_USER+16)  //禁止rts流控
#define CN_UART_EN_CTS                (CN_DRV_CTRL_USER+17)  //使能cts流控
#define CN_UART_DIS_CTS               (CN_DRV_CTRL_USER+18)  //禁止cts流控
#define CN_UART_DMA_USED              (CN_DRV_CTRL_USER+19)  //使用dma传输
#define CN_UART_DMA_UNUSED            (CN_DRV_CTRL_USER+20)  //禁止dma传输
#define CN_UART_SETTO_ALL_DUPLEX      (CN_DRV_CTRL_USER+21)  //设置为全双工方式

#ifdef __cplusplus
}
#endif

#endif //__UARTCTRL_H__



