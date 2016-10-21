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
/*========================================================*
 * 文件模块说明: omapl138的uart驱动，移植自都江堰操作系统作者拥有版权
 * 的S3C2440 uart驱动文件，针对omapl138做了一些修改
 * 文件版本: v1.00                                               *
 * 开发人员: lst                              *
 * 创建时间: 2011.06.01                                          *
 * Copyright(c) 2011-2011  深圳鹏瑞软件有限公司             *
 *========================================================*
 * 程序修改记录(最新的放在最前面):                               *
 *  <版本号> <修改日期>, <修改人员>: <修改功能概述>              *
 *========================================================*
* 例如：
*  V1.02 2004-07-14, 张三: 接收函数增加对报文长度有效性校核
*
*/
//以下是原作者版权声明
//----------------------------------------------------
//Copyright (C), 2004-2009,  都江堰操作系统开发团队
//版权所有 (C), 2004-2009,   都江堰操作系统开发团队
//所属模块: uart驱动程序
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 
//其他说明:
//修订历史:
//1. 日期:2009-03-10
//   作者:  罗侍田.
//   新版本号：1.0.0
//   修改说明: 移植字44b0的1.0.1版
//------------------------------------------------------
#ifndef __CPU_PERI_UART_H__
#define __CPU_PERI_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

//base address
#define cn_uart0_base   0x01c42000
#define cn_uart1_base   0x01d0c000
#define cn_uart2_base   0x01d0d000
typedef struct         //offset
{
    vu32 RBR_THR;         // 0x0000
    vu32 IER;             // 0x0004
    vu32 IIR_FCR;         // 0x0008
    vu32 LCR;             // 0x000C
    vu32 MCR;             // 0x0010
    vu32 LSR;             // 0x0014
    vu32 MSR;             // 0x0018
    vu32 SCR;             // 0x001C
    vu32 DLL;             // 0x0020
    vu32 DLH;             // 0x0024
    vu32 REV_ID1;         // 0x0028
    vu32 REV_ID2;         // 0x002C
    vu32 PWREMU_MGMT;     // 0x0030
    vu32 MDR;             // 0x0034
}tag_UartReg;


#define uart0_buf_len  512
#define uart1_buf_len  512
#define uart2_buf_len  512

//串口状态控制结构
typedef struct UartUCB
{
    struct RingBuf send_ring_buf;           //环形发送缓冲区.
    struct RingBuf recv_ring_buf;           //环形接收缓冲区.

    struct SemaphoreLCB *send_buf_semp;     //发送缓冲区锁
    struct SemaphoreLCB *recv_buf_semp;     //接收缓冲区锁

    u16 evtt_right_write;      //设备右手被写后弹出的事件类型
    u16 evtt_left_write;       //设备左手被写后弹出的事件类型
    u16 recv_trigger_level;    //串口接收触发水平,即收到多少数据时弹出事件
    u32 timeout;               //未到触发水平,持续设定时间内没有收到数据,
                                    //也弹出事件.
    u16 send_trigger_level;    //发送同步触发水平，当发送环形缓冲区满，将
                                    //用send_buf_semp阻塞当前线程，直到发送缓冲
                                    //区数据量降至send_trigger_level时释放之。
    tag_UartReg volatile *my_reg;
    u32 baud;
    ufast_t status;
    ufast_t serial_no;
};

//通信状态变量各位定义
#define CN_SER_RXBUF_BUSY         (1<<0)
#define CN_SER_RXBUF_EMPTY     (1<<1)
#define CN_SER_RECEIVE_OVER    (1<<2)

//串口错误类型
enum _UART_ERROR_NO_
{
    EN_UART_RECV_FIFO_OVER,         //接收硬件fifo溢出
    EN_UART_RECV_BUF_OVER,          //接收缓冲区溢出
};

//串口设备控制命令常数，用于uart_ctrl函数。
#define cn_uart_connect_recv_evtt     0  //设备被从右手写入的数据超过软件触发水
                                         //平时弹出本类型事件
#define cn_uart_disconnect_recv_evtt  1  //断开右手事件
#define cn_uart_connect_send_evtt     2  //设备被从左手写入数据时弹出本类型事件
#define cn_uart_disconnect_send_evtt  3  //断开左手事件
#define CN_UART_START                 4  //启动串口
#define CN_UART_STOP                  5  //停止串口
#define CN_UART_SET_BAUD              6  //设置uartBaud.
#define CN_UART_SEND_DATA             7  //发送数据
#define CN_UART_RECV_DATA             8  //接收数据
#define CN_UART_COMPLETED_SEND        9  //完成发送工作
#define CN_UART_RX_PAUSE              10 //暂停接收数据到缓冲区
#define CN_UART_RX_RESUME             11 //重新开始接收数据到缓冲区
#define CN_UART_RX_OVERTIME              12 //设置接收超时时间，毫秒数
#define CN_UART_RECV_SOFT_LEVEL       13 //设置接收软件缓冲区触发水平
#define CN_UART_SEND_SOFT_LEVEL       14 //设置接收软件缓冲区触发水平
#define CN_UART_RECV_HARD_LEVEL       15 //设置接收fifo触发水平
#define CN_UART_SEND_HARD_LEVEL       16 //设置发送fifo触发水平

ptu32_t module_init_uart0(ptu32_t para);
ptu32_t module_init_uart1(ptu32_t para);
ptu32_t module_init_uart2(ptu32_t para);
ptu32_t module_init_uart3(ptu32_t para);

#ifdef __cplusplus
}
#endif

#endif //__CPU_PERI_UART_H__


