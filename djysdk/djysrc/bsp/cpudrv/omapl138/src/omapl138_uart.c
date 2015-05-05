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
 * 文件模块说明:                                                 *
 * omapl138的uart驱动，移植自都江堰操作系统作者拥有版权
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
#include "stdint.h"
#include "lock.h"
#include "int.h"
#include "djyos.h"
#include "cpu_peri.h"

ptu32_t Uart_Ctrl(struct  tagPanDevice *uart_dev,u32 cmd,
                   ptu32_t data1,ptu32_t data2);
ptu32_t uart_left_write(struct  tagPanDevice *uart_dev,ptu32_t src_buf,
                            ptu32_t res,ptu32_t len);
ptu32_t uart_right_write(struct  tagPanDevice *uart_dev,ptu32_t src_buf,
                            ptu32_t res,ptu32_t len);
ptu32_t uart_left_read(struct  tagPanDevice *uart_dev,ptu32_t src_buf,
                            ptu32_t res,ptu32_t len);
ptu32_t uart_right_read(struct  tagPanDevice *uart_dev,ptu32_t src_buf,
                            ptu32_t res,ptu32_t len);
u32  Uart0_Int(ufast_t uart_int_line);
u32  Uart1_Int(ufast_t uart_int_line);
u32  Uart2_Int(ufast_t uart_int_line);
u32  uart3_int(ufast_t uart_int_line);
ptu32_t uart0_error_service(void);
ptu32_t uart1_error_service(void);
ptu32_t uart2_error_service(void);
ptu32_t uart3_error_service(void);
ptu32_t uart_send_service(void);

extern struct st_int_reg volatile * const pg_int_reg;

static djy_handle_t pg_uart0_rhdl;
static djy_handle_t pg_uart1_rhdl;
static djy_handle_t pg_uart2_rhdl;
static struct tagUartUCB tg_uart0_CB,tg_uart1_CB,tg_uart2_CB;

uint16_t u16g_evtt_uart0_error;
uint16_t u16g_evtt_uart1_error;
uint16_t u16g_evtt_uart2_error;

uint8_t uart0_ptcl_send_buf[uart0_buf_len];
uint8_t uart1_ptcl_send_buf[uart1_buf_len];
uint8_t uart2_ptcl_send_buf[uart2_buf_len];
uint8_t uart0_ptcl_recv_buf[uart0_buf_len];
uint8_t uart1_ptcl_recv_buf[uart1_buf_len];
uint8_t uart2_ptcl_recv_buf[uart2_buf_len];

//----使能接收中断-------------------------------------------------------------
//功能: 使能uart的接收中断。
//参数: reg,被操作的寄存器组指针
//返回: 无
//-----------------------------------------------------------------------------
void __Uart_RecvIntEnable(tag_UartReg volatile *reg)
{
    reg->IER |= (u32)1;
}

//----禁止接收中断-------------------------------------------------------------
//功能: 禁止uart的接收中断。
//参数: reg,被操作的寄存器组指针
//返回: 无
//-----------------------------------------------------------------------------
void __Uart_RecvIntDisable(tag_UartReg volatile *reg)
{
    reg->IER &= ~(u32)1;
}

//----使能发送中断-------------------------------------------------------------
//功能: 使能uart的发送中断。
//参数: reg,被操作的寄存器组指针
//返回: 无
//-----------------------------------------------------------------------------
void __Uart_SendIntEnable(tag_UartReg volatile *reg)
{
    reg->IER |= (u32)2;
}

//----禁止发送中断-------------------------------------------------------------
//功能: 禁止uart的发送中断。
//参数: reg,被操作的寄存器组指针
//返回: 无
//-----------------------------------------------------------------------------
void __Uart_SendIntDisable(tag_UartReg volatile *reg)
{
    reg->IER &= ~(u32)2;
}

//----设置baud-----------------------------------------------------------------
//功能: 设置串口baud
//参数: reg,被操作的寄存器组指针
//返回: 无
//-----------------------------------------------------------------------------
void __Uart_SetBaud(tag_UartReg volatile *reg,u32 baud)
{
    u32 temp;
    if((u32)reg == cn_uart0_base)       //串口0,不受ASYNC3_CLKSRC控制
    {
        temp = (u32)(CN_CFG_PLL0_SYSCLK2+(baud>>1))/(baud*16);
        reg->DLL = (u8)temp;
        reg->DLH = (u8)(temp>>8);
    }else                               //串口1和串口2
    {
        if(g_ptSysconfig0Reg->CFGCHIP[3] & 0x10)   //test ASYNC3_CLKSRC bit
        {
            temp = (u32)(CN_CFG_PLL1_SYSCLK2+(baud>>1))/(baud*16);
            reg->DLL = (u8)temp;
            reg->DLH = (u8)(temp>>8);
        }else
        {
            temp = (u32)(CN_CFG_PLL0_SYSCLK2+(baud>>1))/(baud*16);
            reg->DLL = (u8)temp;
            reg->DLH = (u8)(temp>>8);
        }
    }

}
//----检查串口发送寄存器是否空-------------------------------------------------
//功能: 检查发送寄存器的状态，如果空就返回true
//参数：无。
//返回: true = 缓冲区空，false = 非空
//-----------------------------------------------------------------------------
bool_t uart_tx_tran_empty(volatile tag_UartReg *uart_reg)
{
    return (0 != (uart_reg->LSR & 0x40));
}

//----初始化uart0模块----------------------------------------------------------
//功能：初始化uart0模块
//参数：模块初始化函数没有参数
//返回：1=成功，0=失败
//-----------------------------------------------------------------------------
ptu32_t module_init_uart0(ptu32_t para)
{
    struct tagSemaphoreLCB *left_semp,*right_semp;
    struct  tagPanDevice   *uart_dev;
    uint16_t uart_send_evtt;
    vu32 temp;
    //保护缓冲区的信号量，使缓冲区中数据量为0时阻塞写入线程，读取线程使缓冲区中
    //数据降至trigger_level以下时释放信号量，使写入线程解除阻塞
    tg_uart0_CB.send_buf_semp = Lock_SempCreate(1,0,"uart0 tx buf");
    if(tg_uart0_CB.send_buf_semp == NULL)
        goto exit_from_left_buf_semp;
    tg_uart0_CB.recv_buf_semp = Lock_SempCreate(1,0,"uart0 rx buf");
    if(tg_uart0_CB.recv_buf_semp == NULL)
        goto exit_from_right_buf_semp;
    //分别创建左手和右手访问uart0设备的信号量，第一个参数为1表示串口是独占访问
    //的，第二个参数1表示初始状态有一盏信号灯点亮。左手由应用程序使用，右手由硬
    //件操作函数使用。
    left_semp = Lock_SempCreate(1,1,"uart0 device left");
    if(left_semp == NULL)
        goto exit_from_left_semp;
    right_semp = Lock_SempCreate(1,1,"uart0 device right");
    if(right_semp == NULL)
        goto exit_from_right_semp;

    Cpucfg_EnableLPSC(cn_PSC0_UART0);
    GPIO_CfgPinFunc(8,3,cn_p8_3_uart0_txd);
    GPIO_CfgPinFunc(8,4,cn_p8_4_uart0_rxd);
    //系统初始化时已经使中断处于禁止状态，无需再禁止和清除中断。
    //初始化uart硬件控制数据结构
    tg_uart0_CB.my_reg = (tag_UartReg *)cn_uart0_base;
    __Uart_SetBaud(tg_uart0_CB.my_reg,9600);
    temp = tg_uart0_CB.my_reg->IIR_FCR;     //读一下清除所有中断标志
//    tg_uart0_CB.my_reg->IIR_FCR |= 1;   //set FIFOEN
    tg_uart0_CB.my_reg->IIR_FCR = 0xcf; //接收14字节触发中断，清收发fifo。
    tg_uart0_CB.my_reg->LCR = 3 | (0<<2) | (000<<3) | (0<<6) | (0<<7);
    tg_uart0_CB.my_reg->MCR = 0;
    tg_uart0_CB.my_reg->PWREMU_MGMT = 0x7fff;
    tg_uart0_CB.my_reg->MDR = 0;        //16倍时钟
    tg_uart0_CB.my_reg->IER =  3;       //使能收发中断，此时中断线控制尚未使能
    tg_uart0_CB.baud = 9600;

    //初始化通用串口数据结构
    tg_uart0_CB.recv_trigger_level = 0;  //缓冲接收触发水平为0
    tg_uart0_CB.send_trigger_level = uart0_buf_len>>4;  //缓冲发送触发水平为1/16
    tg_uart0_CB.timeout = (u32)500*mS;  //超时时间=500
    tg_uart0_CB.serial_no = 0;  //串口号是0
    Ring_Init(  &tg_uart0_CB.send_ring_buf,
                uart0_ptcl_send_buf,
                uart0_buf_len);
    Ring_Init(  &tg_uart0_CB.recv_ring_buf,
                uart0_ptcl_recv_buf,
                uart0_buf_len);
    tg_uart0_CB.status = CN_SER_RXBUF_EMPTY;

    //以下建立uart0设备
    uart_dev = Driver_DevAddDevice(Driver_DevGetRootDevice(),"uart0",
                               right_semp,left_semp,
                               (dev_write_func) uart_right_write,
                               (dev_read_func ) uart_right_read,
                               (dev_ctrl_func ) Uart_Ctrl,
                               (dev_write_func ) uart_left_write,
                               (dev_read_func  ) uart_left_read,
                               (dev_ctrl_func  ) Uart_Ctrl,
                               (ptu32_t)(&tg_uart0_CB)  //uart0设备的私有数据
                               );
    if(uart_dev == NULL)
        goto exit_from_add_device;
    pg_uart0_rhdl = Driver_DevOpenRight("uart0",0);      //打开右手句柄
    u16g_evtt_uart0_error = Djy_EvttRegist(
                           EN_CORRELATIVE,CN_PRIO_REAL,0,0,
                           uart0_error_service,1024,NULL);
    if(u16g_evtt_uart0_error == CN_INVALID_EVTT_ID)
        goto exit_from_error_evtt;
    uart_send_evtt = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_REAL,0,0,
                                   uart_send_service,20,NULL);
    if(uart_send_evtt == CN_INVALID_EVTT_ID)
        goto exit_from_send_evtt;
    Driver_DevCtrlRight(pg_uart0_rhdl,cn_uart_connect_send_evtt,uart_send_evtt,0);
    Int_IsrConnect(cn_int_line_uart0,Uart0_Int);
    Int_SettoAsynSignal(cn_int_line_uart0);
    Int_ClearLine(cn_int_line_uart0);       //清掉初始化产生的发送fifo空的中断
    Int_RestoreAsynLine(cn_int_line_uart0);
    return 1;
exit_from_send_evtt:
    Djy_EvttUnregist(u16g_evtt_uart0_error);
exit_from_error_evtt:
    Driver_DevDeleteDevice(uart_dev);
exit_from_add_device:
    Lock_SempDelete(right_semp);
exit_from_right_semp:
    Lock_SempDelete(left_semp);
exit_from_left_semp:
    Lock_SempDelete(tg_uart0_CB.recv_buf_semp);
exit_from_right_buf_semp:
    Lock_SempDelete(tg_uart0_CB.send_buf_semp);
exit_from_left_buf_semp:
    return 0;
}

//----初始化uart1模块----------------------------------------------------------
//功能：初始化uart1模块
//参数：模块初始化函数没有参数
//返回：1=成功，0=失败
//-----------------------------------------------------------------------------
ptu32_t module_init_uart1(ptu32_t para)
{
    struct tagSemaphoreLCB *left_semp,*right_semp;
    struct  tagPanDevice   *uart_dev;
    uint16_t uart_send_evtt;
    vu32 temp;
    //保护缓冲区的信号量，使缓冲区中数据量为0时阻塞写入线程，读取线程使缓冲区中
    //数据降至trigger_level以下时释放信号量，使写入线程解除阻塞
    tg_uart1_CB.send_buf_semp = Lock_SempCreate(1,0,"uart1 tx buf");
    if(tg_uart1_CB.send_buf_semp == NULL)
        goto exit_from_left_buf_semp;
    tg_uart1_CB.recv_buf_semp = Lock_SempCreate(1,0,"uart1 rx buf");
    if(tg_uart1_CB.recv_buf_semp == NULL)
        goto exit_from_right_buf_semp;
    //分别创建左手和右手访问uart1设备的信号量，第一个参数为1表示串口是独占访问
    //的，第二个参数1表示初始状态有一盏信号灯点亮。左手由应用程序使用，右手由硬
    //件操作函数使用。
    left_semp = Lock_SempCreate(1,1,"uart1 device left");
    if(left_semp == NULL)
        goto exit_from_left_semp;
    right_semp = Lock_SempCreate(1,1,"uart1 device right");
    if(right_semp == NULL)
        goto exit_from_right_semp;

    Cpucfg_EnableLPSC(cn_PSC1_UART1);
    GPIO_CfgPinFunc(1,0,cn_p1_0_uart1_txd);
    GPIO_CfgPinFunc(1,1,cn_p1_1_uart1_rxd);

    //系统初始化时已经使中断处于禁止状态，无需再禁止和清除中断。
    //初始化uart硬件控制数据结构
    tg_uart1_CB.my_reg = (tag_UartReg *)cn_uart1_base;
    __Uart_SetBaud(tg_uart1_CB.my_reg,9600);
    temp = tg_uart1_CB.my_reg->IIR_FCR;     //读一下清除所有中断标志
//    tg_uart1_CB.my_reg->IIR_FCR |= 1;   //set FIFOEN
    tg_uart1_CB.my_reg->IIR_FCR = 0xcf; //接收14字节触发中断，清收发fifo。
    tg_uart1_CB.my_reg->LCR = 3 | (0<<2) | (000<<3) | (0<<6) | (0<<7);
    tg_uart1_CB.my_reg->MCR = 0;
    tg_uart1_CB.my_reg->PWREMU_MGMT = 0x7fff;
    tg_uart1_CB.my_reg->MDR = 0;        //16倍时钟
    tg_uart1_CB.my_reg->IER =  3;       //使能收发中断，此时中断线控制尚未使能
    tg_uart1_CB.baud = 9600;

    //初始化通用串口数据结构
    tg_uart1_CB.recv_trigger_level = 0;  //缓冲接收触发水平为0
    tg_uart1_CB.send_trigger_level = uart1_buf_len>>4;  //缓冲发送触发水平为1/16
    tg_uart1_CB.timeout = (u32)500*mS;  //超时时间=500
    tg_uart1_CB.serial_no = 1;  //串口号是1
    Ring_Init(  &tg_uart1_CB.send_ring_buf,
                uart1_ptcl_send_buf,
                uart1_buf_len);
    Ring_Init(  &tg_uart1_CB.recv_ring_buf,
                uart1_ptcl_recv_buf,
                uart1_buf_len);
    tg_uart1_CB.status = CN_SER_RXBUF_EMPTY;

    //以下建立uart1设备
    uart_dev = Driver_DevAddDevice(Driver_DevGetRootDevice(),"uart1",
                               right_semp,left_semp,
                               (dev_write_func) uart_right_write,
                               (dev_read_func ) uart_right_read,
                               (dev_ctrl_func ) Uart_Ctrl,
                               (dev_write_func ) uart_left_write,
                               (dev_read_func  ) uart_left_read,
                               (dev_ctrl_func  ) Uart_Ctrl,
                               (ptu32_t)(&tg_uart1_CB)  //uart1设备的私有数据
                               );
    if(uart_dev == NULL)
        goto exit_from_add_device;
    pg_uart1_rhdl = Driver_DevOpenRight("uart1",0);      //打开右手句柄
    u16g_evtt_uart1_error = Djy_EvttRegist(
                           EN_CORRELATIVE,CN_PRIO_REAL,0,0,
                           uart1_error_service,1024,NULL);
    if(u16g_evtt_uart1_error == CN_INVALID_EVTT_ID)
        goto exit_from_error_evtt;
    uart_send_evtt = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_REAL,0,0,
                                   uart_send_service,20,NULL);
    if(uart_send_evtt == CN_INVALID_EVTT_ID)
        goto exit_from_send_evtt;
    Driver_DevCtrlRight(pg_uart1_rhdl,cn_uart_connect_send_evtt,uart_send_evtt,0);
    Int_IsrConnect(cn_int_line_uart1,Uart1_Int);
    Int_SettoAsynSignal(cn_int_line_uart1);
    Int_ClearLine(cn_int_line_uart1);       //清掉初始化产生的发送fifo空的中断
    Int_RestoreAsynLine(cn_int_line_uart1);
    return 1;
exit_from_send_evtt:
    Djy_EvttUnregist(u16g_evtt_uart1_error);
exit_from_error_evtt:
    Driver_DevDeleteDevice(uart_dev);
exit_from_add_device:
    Lock_SempDelete(right_semp);
exit_from_right_semp:
    Lock_SempDelete(left_semp);
exit_from_left_semp:
    Lock_SempDelete(tg_uart1_CB.recv_buf_semp);
exit_from_right_buf_semp:
    Lock_SempDelete(tg_uart1_CB.send_buf_semp);
exit_from_left_buf_semp:
    return 0;
}

//----初始化uart2模块----------------------------------------------------------
//功能：初始化uart2模块
//参数：模块初始化函数没有参数
//返回：1=成功，0=失败
//-----------------------------------------------------------------------------
ptu32_t module_init_uart2(ptu32_t para)
{
    struct tagSemaphoreLCB *left_semp,*right_semp;
    struct  tagPanDevice   *uart_dev;
    uint16_t uart_send_evtt;
    vu32 temp;
    //保护缓冲区的信号量，使缓冲区中数据量为0时阻塞写入线程，读取线程使缓冲区中
    //数据降至trigger_level以下时释放信号量，使写入线程解除阻塞
    tg_uart2_CB.send_buf_semp = Lock_SempCreate(1,0,"uart2 tx buf");
    if(tg_uart2_CB.send_buf_semp == NULL)
        goto exit_from_left_buf_semp;
    tg_uart2_CB.recv_buf_semp = Lock_SempCreate(1,0,"uart2 rx buf");
    if(tg_uart2_CB.recv_buf_semp == NULL)
        goto exit_from_right_buf_semp;
    //分别创建左手和右手访问uart2设备的信号量，第一个参数为1表示串口是独占访问
    //的，第二个参数1表示初始状态有一盏信号灯点亮。左手由应用程序使用，右手由硬
    //件操作函数使用。
    left_semp = Lock_SempCreate(1,1,"uart2 device left");
    if(left_semp == NULL)
        goto exit_from_left_semp;
    right_semp = Lock_SempCreate(1,1,"uart2 device right");
    if(right_semp == NULL)
        goto exit_from_right_semp;

    Cpucfg_EnableLPSC(cn_PSC1_UART2);
    GPIO_CfgPinFunc(1,2,cn_p1_2_uart2_txd);
    GPIO_CfgPinFunc(1,3,cn_p1_3_uart2_rxd);
    //系统初始化时已经使中断处于禁止状态，无需再禁止和清除中断。
    //初始化uart硬件控制数据结构
    tg_uart2_CB.my_reg = (tag_UartReg *)cn_uart2_base;
    __Uart_SetBaud(tg_uart2_CB.my_reg,9600);
    temp = tg_uart2_CB.my_reg->IIR_FCR;     //读一下清除所有中断标志
//    tg_uart2_CB.my_reg->IIR_FCR |= 1;   //set FIFOEN
    tg_uart2_CB.my_reg->IIR_FCR = 0xcf; //接收14字节触发中断，清收发fifo。
    tg_uart2_CB.my_reg->LCR = 3 | (0<<2) | (000<<3) | (0<<6) | (0<<7);
    tg_uart2_CB.my_reg->MCR = 0;
    tg_uart2_CB.my_reg->PWREMU_MGMT = 0x7fff;
    tg_uart2_CB.my_reg->MDR = 0;        //16倍时钟
    tg_uart2_CB.my_reg->IER =  3;       //使能收发中断，此时中断线控制尚未使能
    tg_uart2_CB.baud = 9600;

    //初始化通用串口数据结构
    tg_uart2_CB.recv_trigger_level = 0;  //缓冲接收触发水平为0
    tg_uart2_CB.send_trigger_level = uart2_buf_len>>4;  //缓冲发送触发水平为1/16
    tg_uart2_CB.timeout = (u32)500*mS;  //超时时间=500
    tg_uart2_CB.serial_no = 2;  //串口号是2
    Ring_Init(  &tg_uart2_CB.send_ring_buf,
                uart2_ptcl_send_buf,
                uart2_buf_len);
    Ring_Init(  &tg_uart2_CB.recv_ring_buf,
                uart2_ptcl_recv_buf,
                uart2_buf_len);
    tg_uart2_CB.status = CN_SER_RXBUF_EMPTY;

    //以下建立uart2设备
    uart_dev = Driver_DevAddDevice(Driver_DevGetRootDevice(),"uart2",
                               right_semp,left_semp,
                               (dev_write_func) uart_right_write,
                               (dev_read_func ) uart_right_read,
                               (dev_ctrl_func ) Uart_Ctrl,
                               (dev_write_func ) uart_left_write,
                               (dev_read_func  ) uart_left_read,
                               (dev_ctrl_func  ) Uart_Ctrl,
                               (ptu32_t)(&tg_uart2_CB)  //uart2设备的私有数据
                               );
    if(uart_dev == NULL)
        goto exit_from_add_device;
    pg_uart2_rhdl = Driver_DevOpenRight("uart2",0);      //打开右手句柄
    u16g_evtt_uart2_error = Djy_EvttRegist(
                           EN_CORRELATIVE,CN_PRIO_REAL,0,0,
                           uart2_error_service,1024,NULL);
    if(u16g_evtt_uart2_error == CN_INVALID_EVTT_ID)
        goto exit_from_error_evtt;
    uart_send_evtt = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_REAL,0,0,
                                   uart_send_service,20,NULL);
    if(uart_send_evtt == CN_INVALID_EVTT_ID)
        goto exit_from_send_evtt;
    Driver_DevCtrlRight(pg_uart2_rhdl,cn_uart_connect_send_evtt,uart_send_evtt,0);
    Int_IsrConnect(cn_int_line_uart2,Uart2_Int);
    Int_SettoAsynSignal(cn_int_line_uart2);
    Int_ClearLine(cn_int_line_uart2);       //清掉初始化产生的发送fifo空的中断
    Int_RestoreAsynLine(cn_int_line_uart2);
    return 1;
exit_from_send_evtt:
    Djy_EvttUnregist(u16g_evtt_uart2_error);
exit_from_error_evtt:
    Driver_DevDeleteDevice(uart_dev);
exit_from_add_device:
    Lock_SempDelete(right_semp);
exit_from_right_semp:
    Lock_SempDelete(left_semp);
exit_from_left_semp:
    Lock_SempDelete(tg_uart2_CB.recv_buf_semp);
exit_from_right_buf_semp:
    Lock_SempDelete(tg_uart2_CB.send_buf_semp);
exit_from_left_buf_semp:
    return 0;
}

ptu32_t uart0_error_service(void)
{
    while(1)
    {
        Djy_WaitEvttPop(Djy_MyEvttId(),NULL,CN_TIMEOUT_FOREVER);
    }
}

ptu32_t uart1_error_service(void)
{
    volatile uint32_t temp;
    while(1)
    {
        Djy_WaitEvttPop(Djy_MyEvttId(),NULL,CN_TIMEOUT_FOREVER);
    }
}

ptu32_t uart2_error_service(void)
{
    volatile uint32_t temp;
    while(1)
    {
        Djy_WaitEvttPop(Djy_MyEvttId(),NULL,CN_TIMEOUT_FOREVER);
    }
}

//----uart0中断---------------------------------------------------------------
//功能: 接收，读取fifo中的数据并从右手写入设备
//      发送，从右手读数据并发送
//参数: 中断号.
//返回: 0.
//-----------------------------------------------------------------------------
uint32_t Uart0_Int(ufast_t uart_int_line)
{
    uint32_t recv_trans,num;
    uint32_t IIR;
    tag_UartReg *reg;
    uint8_t ch[20];
    reg =  (tag_UartReg *)tg_uart0_CB.my_reg;
    while(1)
    {
        IIR = reg->IIR_FCR;
        if(IIR & 1)
            break;
        IIR = (IIR>>1)&0x7;
        switch(IIR)
        {
            case 1:     //发送fifo寄存器空
            {
                recv_trans = Driver_DevReadRight(pg_uart0_rhdl,(ptu32_t)ch,16,0);
                for(num = 0; num < recv_trans; num++)
                {
                    reg->RBR_THR = ch[num];
                }
            }break;
            case 2:     //接收触发水平到
            case 6:     //接收超时，fifo中至少有一个数据，连续4字符时间无新数据
            {
                for(num = 0; num<20; num++)
                {
                    if(reg->LSR & 1)
                        ch[num] = reg->RBR_THR;
                    else
                        break;
                }
                Driver_DevWriteRight(pg_uart0_rhdl,(ptu32_t)ch,num,0);
            }break;
            case 3:     //出错标志
            {
            }break;
            default: return 0;
        }
    }
    return 0;
}

//----uart1中断---------------------------------------------------------------
//功能: 接收，读取fifo中的数据并从右手写入设备
//      发送，从右手读数据并发送
//参数: 中断号.
//返回: 0.
//-----------------------------------------------------------------------------
uint32_t Uart1_Int(ufast_t uart_int_line)
{
    uint32_t recv_trans,num;
    uint32_t IIR;
    tag_UartReg *reg;
    uint8_t ch[20];
    reg = (tag_UartReg *)tg_uart1_CB.my_reg;
    while(1)
    {
        IIR = reg->IIR_FCR;
        if(IIR & 1)
            break;
        IIR = (IIR>>1)&0x7;
        switch(IIR)
        {
            case 1:     //发送fifo寄存器空
            {
                recv_trans = Driver_DevReadRight(pg_uart1_rhdl,(ptu32_t)ch,16,0);
                for(num = 0; num < recv_trans; num++)
                {
                    reg->RBR_THR = ch[num];
                }
            }break;
            case 2:     //接收触发水平到
            case 6:     //接收超时，fifo中至少有一个数据，连续4字符时间无新数据
            {
                for(num = 0; num<20; num++)
                {
                    if(reg->LSR & 1)
                        ch[num] = reg->RBR_THR;
                    else
                        break;
                }
                Driver_DevWriteRight(pg_uart1_rhdl,(ptu32_t)ch,num,0);
            }break;
            case 3:     //出错标志
            {
            }break;
            default: return 0;
        }
    }
    return 0;
}

//----uart2中断---------------------------------------------------------------
//功能: 接收，读取fifo中的数据并从右手写入设备
//      发送，从右手读数据并发送
//参数: 中断号.
//返回: 0.
//-----------------------------------------------------------------------------
uint32_t Uart2_Int(ufast_t uart_int_line)
{
    uint32_t recv_trans,num;
    uint32_t IIR;
    tag_UartReg *reg;
    uint8_t ch[20];
    reg = (tag_UartReg *)tg_uart2_CB.my_reg;
    while(1)
    {
        IIR = reg->IIR_FCR;
        if(IIR & 1)
            break;
        IIR = (IIR>>1)&0x7;
        switch(IIR)
        {
            case 1:     //发送fifo寄存器空
            {
                recv_trans = Driver_DevReadRight(pg_uart2_rhdl,(ptu32_t)ch,16,0);
                for(num = 0; num < recv_trans; num++)
                {
                    reg->RBR_THR = ch[num];
                }
            }break;
            case 2:     //接收触发水平到
            case 6:     //接收超时，fifo中至少有一个数据，连续4字符时间无新数据
            {
                for(num = 0; num<20; num++)
                {
                    if(reg->LSR & 1)
                        ch[num] = reg->RBR_THR;
                    else
                        break;
                }
                Driver_DevWriteRight(pg_uart2_rhdl,(ptu32_t)ch,num,0);
            }break;
            case 3:     //出错标志
            {
            }break;
            default: return 0;
        }
    }
    return 0;
}

//----uart设备右手写函数-------------------------------------------------------
//功能: 从右手写入uart设备,由uart设备连接的串口模块调用.
//      1.把数据写入串口serial_DCB_t的环形发送缓冲区中.
//      2.如果写入设备的数据未达到触发水平，则继续收集数据
//      3.达到出发水平后，按设定的方式把数据传递给上层设备
//参数: uart_dev,被操作的设备的右手指针.
//      buf，数据源指针。
//      len，数据量(bytes)
//返回: 实际写入环形缓冲区的字符数
//-----------------------------------------------------------------------------
ptu32_t uart_right_write(struct  tagPanDevice *uart_dev,ptu32_t buf,
                            ptu32_t len,u32 res)
{
    struct tagUartUCB *uart_port;
    uint16_t recv_bytes,valid_bytes,error_evtt;
    enum _UART_ERROR_NO_ uart_error;

    uart_port = (struct tagUartUCB*)Driver_DevGetMyTag(uart_dev);

    //copy整个硬件缓冲区到协议缓冲区
    recv_bytes = Ring_Write(&uart_port->recv_ring_buf, (uint8_t*)buf,len);
    if(recv_bytes != len)
    {
        switch(uart_port->serial_no)
        {
            case 0:
                error_evtt = u16g_evtt_uart0_error;
                break;
            case 1:
                error_evtt = u16g_evtt_uart1_error;
                break;
            case 2:
                error_evtt = u16g_evtt_uart2_error;
                break;
            default:
                error_evtt = u16g_evtt_uart0_error;
        }
        uart_error = EN_UART_RECV_BUF_OVER;
        Djy_EventPop(error_evtt,NULL,0,(ptu32_t)uart_error,0,0);
    }
    valid_bytes = Ring_Check(&uart_port->recv_ring_buf);
    if(valid_bytes >= uart_port->recv_trigger_level)
    {
        //如果不登记右手写事件，就只能查询方式从设备读取数据了。
        Djy_EventPop(uart_port->evtt_right_write, NULL,0,0,0,0);
    }

    return (ptu32_t)recv_bytes;


}

//----uart设备左手写函数-------------------------------------------------------
//功能: 从左手写入uart设备,一般由uart的使用者调用.
//      1.把数据写入串口serial_DCB_t的环形发送缓冲区中.
//      2.如果串口硬件空闲,便把数据写入硬件发送缓冲区,并启动发送,否则不操作
//参数: uart_dev,被操作的设备的左手指针.
//      src_buf,数据源指针
//      len,要发送的序列长度
//返回: 实际写入环形缓冲区的字符数
//-----------------------------------------------------------------------------
ptu32_t uart_left_write(struct  tagPanDevice *uart_dev,ptu32_t src_buf,
                            ptu32_t len,u32 res)
{
    uint32_t result;
    struct tagUartUCB *uart_port;
    uint32_t completed = 0;

    if(len == 0)
        return 0;
    uart_port = (struct tagUartUCB*)Driver_DevGetMyTag(uart_dev);
    while(1)
    {
        __Uart_SendIntDisable((tag_UartReg *)uart_port->my_reg);
        result = Ring_Write(&uart_port->send_ring_buf,
                            (uint8_t*)src_buf+completed,len-completed);
        __Uart_SendIntEnable((tag_UartReg *)uart_port->my_reg);
        Djy_EventPop(uart_port->evtt_left_write,NULL,0,(ptu32_t)&uart_dev,0,0);
        if(result != len-completed)     //缓冲区满，没有送出全部数据
        {
            completed += result;
            Lock_SempPend(uart_port->send_buf_semp,CN_TIMEOUT_FOREVER);
        }else
            break;
    }
    return result;
}

//----串口设备左手读函数------------------------------------------------------
//功能: 左手读用于上层设备,读取串口接收环形缓冲区的数据
//参数: uart_dev,被操作的设备的左手指针.
//      dst_buf,读缓冲区
//      len,读入长度,
//返回: 实际读出长度
//----------------------------------------------------------------------------
ptu32_t uart_left_read(struct  tagPanDevice *uart_dev,ptu32_t dst_buf,
                                        ptu32_t len,u32 res)
{
    struct tagUartUCB *uart_port;
    uint32_t result;
    uart_port = (struct tagUartUCB*)Driver_DevGetMyTag(uart_dev);
    __Uart_RecvIntDisable((tag_UartReg *)uart_port->my_reg);
    result = Ring_Read(&uart_port->recv_ring_buf,(uint8_t*)dst_buf,len);
    __Uart_RecvIntEnable((tag_UartReg *)uart_port->my_reg);
    return result;
}

//----串口设备右手读函数------------------------------------------------------
//功能: 右手读用于硬件,硬件发送服务函数 ser_send_service从串口发送环形缓冲区
//      读取数据到串口硬件缓冲区并启动发送.
//参数: uart_dev,被操作的设备的右手指针.
//      dst_buf,读缓冲区
//      len,读入长度,
//返回: 实际读出长度
//----------------------------------------------------------------------------
ptu32_t uart_right_read(struct  tagPanDevice *uart_dev,ptu32_t dst_buf,
                                        ptu32_t len,u32 res)
{
    struct tagUartUCB *uart_port;
    uint32_t result;
    uart_port = (struct tagUartUCB*)Driver_DevGetMyTag(uart_dev);
    result = (ptu32_t)Ring_Read(&uart_port->send_ring_buf,(uint8_t *)dst_buf,len);
    if(Ring_Check(&uart_port->send_ring_buf) <= uart_port->send_trigger_level)
        Lock_SempPost(uart_port->send_buf_semp);
    return result;
}

//----串口设备控制函数---------------------------------------------------------
//功能: 串口设备的控制函数
//参数: uart_handle,被操作的串口设备指针.
//      cmd,操作类型
//      data,含义依cmd而定
//返回: 无意义.
//-----------------------------------------------------------------------------
ptu32_t Uart_Ctrl(struct tagPanDevice *uart_dev,uint32_t cmd,
                   uint32_t data1,uint32_t data2)
{
    struct tagUartUCB *uart_port;
    ptu32_t result = 0;

    uart_port = (struct tagUartUCB*)Driver_DevGetMyTag(uart_dev);
    switch(cmd)
    {
        case cn_uart_connect_recv_evtt:
            result = (ptu32_t)uart_port->evtt_right_write;
            uart_port->evtt_right_write = (uint16_t)data1;
            break;
        case cn_uart_disconnect_recv_evtt:
            result = (ptu32_t)uart_port->evtt_right_write;
            uart_port->evtt_right_write = CN_INVALID_EVTT_ID;
            break;
        case cn_uart_connect_send_evtt:
            result = (ptu32_t)uart_port->evtt_left_write;
            uart_port->evtt_left_write = (uint16_t)data1;
            break;
        case cn_uart_disconnect_send_evtt:
            result = (ptu32_t)uart_port->evtt_left_write;
            uart_port->evtt_left_write = CN_INVALID_EVTT_ID;
            break;
        case CN_UART_START:
            __Uart_RecvIntEnable((tag_UartReg *)uart_port->my_reg);
            __Uart_SendIntEnable((tag_UartReg *)uart_port->my_reg);
            break;
        case CN_UART_STOP:
            __Uart_RecvIntDisable((tag_UartReg *)uart_port->my_reg);
            __Uart_SendIntDisable((tag_UartReg *)uart_port->my_reg);
            break;
        case CN_UART_SET_BAUD:  //设置Baud
            if(uart_port->baud !=data1)
            {
                uart_port->baud = data1;
                __Uart_SetBaud((tag_UartReg *)uart_port->my_reg,data1);
            }
            break;
        case CN_UART_SEND_DATA:     //启动发送
        //用IO控制半双工通信的发送使能的话,在此转换收发.
            break;
        case cn_uart_recv_data:     //启动接收
        //用IO控制半双工通信的发送使能的话,在此转换收发.
            break;
        case CN_UART_COMPLETED_SEND:
        //用IO控制半双工通信的发送使能的话,在此转换收发,注意要查询UTRSTAT寄存器
        //的Transmit shifter empty位判断是否已经完成.
            break;
        case CN_UART_RX_PAUSE:      //暂停接收
            __Uart_RecvIntDisable((tag_UartReg *)uart_port->my_reg);
            break;
        case CN_UART_RX_RESUME:      //恢复接收
            __Uart_RecvIntEnable((tag_UartReg *)uart_port->my_reg);
            break;
        case CN_UART_RECV_SOFT_LEVEL:    //设置接收软件缓冲区触发水平
            uart_port->recv_trigger_level = (uint16_t)data1;
            break;
        case CN_UART_SEND_SOFT_LEVEL:    //设置接收软件缓冲区触发水平
            uart_port->send_trigger_level = (uint16_t)data1;
            break;
        case CN_UART_RX_OVERTIME:   //data1为毫秒数
            uart_port->timeout = data1;
            break;
        case CN_UART_RECV_HARD_LEVEL:    //设置接收fifo触发水平
        {
            uart_port->my_reg->IIR_FCR &= 0x3f;
            switch (data1)
            {
                case 1:
                    uart_port->my_reg->IIR_FCR |= 0x00;
                    break;
                case 4:
                    uart_port->my_reg->IIR_FCR |= 0x40;
                    break;
                case 8:
                    uart_port->my_reg->IIR_FCR |= 0x80;
                    break;
                case 14:
                    uart_port->my_reg->IIR_FCR |= 0xc0;
                    break;
                default :break;
            }
            break;
        }
        default: break;
    }
    return result;
}

//----串行发送服务-------------------------------------------------------------
//功能: 事件服务函数,当应用程序从设备左手写入待发送的数据时，就会弹出本事件.
//参数: 事件服务函数没有参数.
//返回: 事件服务函数可以不返回,即使返回也没有返回值.
//-----------------------------------------------------------------------------
ptu32_t uart_send_service(void)
{
    struct tagUartUCB uart_port;
    struct tagPanDevice ser_dev;
    djy_handle_t uart_rhdl;
    void *my_para;
    uint8_t ch[16];
    ufast_t trans,num;

    while(1)
    {
        Djy_GetEventPara(&my_para,NULL);
        memcpy(&ser_dev,my_para,sizeof(struct tagPanDevice));
//      ser_dev = (struct  tagPanDevice *)(*(void**)my_para);
        uart_port = (struct tagUartUCB *)Driver_DevGetMyTag(ser_dev);
        __Uart_SendIntDisable((tag_UartReg *)uart_port->my_reg);
        if( uart_tx_tran_empty((tag_UartReg *)uart_port->my_reg))
        {
            if(uart_port->serial_no == 0)
                uart_rhdl = pg_uart0_rhdl;
            else if(uart_port->serial_no == 1)
                uart_rhdl = pg_uart1_rhdl;
            else if(uart_port->serial_no == 2)
                uart_rhdl = pg_uart2_rhdl;
            trans = Driver_DevReadRight(uart_rhdl,(ptu32_t)ch,16,0);
            for(num = 0; num < trans; num++)
            {
                uart_port->my_reg->RBR_THR = ch[num];
            }
        }
        __Uart_SendIntEnable((tag_UartReg *)uart_port->my_reg);
        //等待自身类型的事件再次发生。
        Djy_WaitEvttPop(Djy_MyEvttId(),NULL,CN_TIMEOUT_FOREVER);
    }
}


