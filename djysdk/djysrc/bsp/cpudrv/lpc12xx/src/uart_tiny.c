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
//所属模块: uart驱动程序
//作者：lst
//版本：V1.0.0
//文件描述: 驱动44b0的uart
//其他说明:
//修订历史:
//1. 日期:2009-03-10
//   作者:lst
//   新版本号：1.0.0
//   修改说明: 移植字44b0的1.0.1版
//------------------------------------------------------
#include "stdint.h"
#include <string.h>
#include "cpu_peri.h"
#include "uart_tiny.h"


//#define UART0_BRD 57600   //串口波特率
//#define UART1_BRD 57600   //串口波特率

struct uart_line_buf
{
    uint8_t tagLineBuf[uart1_buf_len];
    u32     pt;
};

#define UART0_BRD   2500000 //串口波特率
#define UART1_BRD   2500000 //串口波特率


#define UART1_RTS_HIGH()    GPIO_SettoHigh(0,1<<27) // set_pin(0,27)
#define UART1_RTS_LOW()     GPIO_SettoLow(0,1<<27)      //clr_pin(0,27)

#if(CN_CFG_UART0_USED == 1)
static struct tagUartUCB tg_uart0_cb;
uint8_t uart0_drv_send_buf[uart0_buf_len];
uint8_t uart0_drv_recv_buf[uart0_buf_len];
#endif

#if(CN_CFG_UART1_USED == 1)
static struct tagUartUCB tg_uart1_cb;
static uint8_t uart1_drv_send_buf[uart1_buf_len];
static uint8_t uart1_drv_recv_buf[uart1_buf_len];
static struct uart_line_buf uart1_line_buf1,uart1_line_buf2,*pg_inuse_line_buf;
#endif
u32 __uart0_int(ufast_t uart_int_line);
u32 __uart1_int(ufast_t uart_int_line);
void __uart0_pause_send_int(void);
void __uart1_pause_send_int(void);

#if(CN_CFG_UART0_USED == 1)

//----设置uart0 baud--------------------------------------------------------
//功能: 设置uart0 baud,注:本函数未处理小数baud
//参数: baud
//返回: 无
//-----------------------------------------------------------------------------
void __uart0_set_baud(u32 baud)
{
    u32 n;
    n = (CN_CFG_MCLK / baud)>>4;
    pg_uart_reg0->LCR |= UART_LCR_DLAB_EN;
    pg_uart_reg0->RTD.DLL = (u8)(n % 256);
    pg_uart_reg0->DI.DLM = (u8)(n / 256);
    pg_uart_reg0->LCR &= (~UART_LCR_DLAB_EN) & 0xff;
    pg_uart_reg0->FDR = 0;    //暂未处理小数
}

//----初始化uart0模块----------------------------------------------------------
//功能：初始化uart0模块，初始化状态为不使用dma传送的方式。
//参数：para，在本函数中无效
//返回：true = 成功初始化，false = 初始化失败
//-----------------------------------------------------------------------------
ptu32_t module_init_uart0(ptu32_t para)
{
    //保护缓冲区的信号量，使缓冲区中数据量为0时阻塞写入线程，读取线程使缓冲区中
    //数据降至trigger_level以下时释放信号量，使写入线程解除阻塞
    tg_uart0_cb.send_buf_semp = Lock_SempCreate(1,0,"uart0 tx buf");
    if(tg_uart0_cb.send_buf_semp == NULL)
        goto exit_from_left_buf_semp;
    tg_uart0_cb.recv_buf_semp = Lock_SempCreate(1,0,"uart0 rx buf");
    if(tg_uart0_cb.recv_buf_semp == NULL)
        goto exit_from_right_buf_semp;
    //初始化IO端口位uart功能
    sys_enable_peri(cn_peri_uart0);
    sys_enable_peri_clk(cn_peri_clksrc_uart0);
    sys_set_uart0_clkdiv(1);


    __uart0_set_baud(UART0_BRD);     //设置波特率为宏定义中的数值

    pg_uart_reg0->IF.IIR;     //读一下清除所有中断标志
//    LPC_UART0->IIR_FCR |= 1;   //set FIFOEN
    pg_uart_reg0->LCR |= UART_LCR_DLAB_EN;
    pg_uart_reg0->IF.FCR = 0x1;   //使能ifo。
    pg_uart_reg0->IF.FCR = 0x47;  //接收4字节触发中断，清收发fifo。
    pg_uart_reg0->LCR = 3 | (0<<2) | (000<<3) | (0<<6) | (0<<7);
    pg_uart_reg0->MCR = 0;
    pg_uart_reg0->TER = UART_TER_TXEN;
    pg_uart_reg0->DI.IER =  UART_INTCFG_RBR;       //使能收发中断，
    pg_uart_reg0->RS485CTRL =      UART_RS485_RECEIVER_ENABLE
                            |   UART_RS485_AUTO_DIRECTION_ENABLE
                            |   UART_RS485_DIRPIN_HIGH
                            |   UART_RS485_DIRPIN_RTS;


    tg_uart0_cb.baud = UART0_BRD;

    tg_uart0_cb.sendding = false;
    Ring_Init(  &tg_uart0_cb.send_ring_buf,
                uart0_drv_send_buf,
                uart0_buf_len);
    Ring_Init(  &tg_uart0_cb.recv_ring_buf,
                uart0_drv_recv_buf,
                uart0_buf_len);

    Int_IsrConnect(cn_int_line_uart0,__uart0_int);
    Int_SettoAsynSignal(cn_int_line_uart0);

    Int_ClearLine(cn_int_line_uart0);
    Int_RestoreAsynLine(cn_int_line_uart0);
    return true;

exit_from_right_buf_semp:
    Lock_SempDelete(tg_uart0_cb.send_buf_semp);
exit_from_left_buf_semp:
    return false;
}

//----uart1中断---------------------------------------------------------------
//功能: 如果是接收信号触发的中断:
//      1.检查接收fifo中的数据量.把数据从接收寄存器全部copy到物理层缓冲区中.
//      2.如果物理层缓冲区满.置串口控制块状态字的溢出位,并发出uart错误事件.
//      3.发送串口接收事件.两个事件都是状态量事件
//      如果是发送信号引发的中断:
//      1.检查发送fifo有多少空位
//      2.从设备右手接口读出适量数据，
//      3.把读出的数据写入发送fifo
//      如果是出错信号引发中断:弹出串口出错事件
//参数: 中断函数没有参数.
//返回: 中断函数没有返回值.
//-----------------------------------------------------------------------------
u32 __uart0_int(ufast_t uart_int_line)
{
    u32 recv_trans,num;
    u32 IIR;
    uint8_t ch[16];
    while(1)
    {
        IIR = pg_uart_reg0->IF.IIR;
        if(IIR & 1)
            break;
        IIR = (IIR>>1)&0x7;
        switch(IIR)
        {
            case 1:     //发送fifo寄存器空
            {
                recv_trans = Ring_Read(&tg_uart0_cb.send_ring_buf,ch,16);
                if(recv_trans != 0)
                {
                    for(num = 0; num < recv_trans; num++)
                    {
                        pg_uart_reg0->RTD.THR = ch[num];
                    }
                }
                if(recv_trans < 16)
                    __uart0_pause_send_int();
                if(Ring_Check(&tg_uart0_cb.send_ring_buf) < 16)
                    Lock_SempPost(tg_uart0_cb.send_buf_semp);
            }break;
            case 2:     //接收触发水平到
            case 6:     //接收超时，fifo中至少有一个数据，连续4字符时间无新数据
            {
                for(num = 0; num<16; num++)
                {
                    if(pg_uart_reg0->LSR & 1)
                        ch[num] = pg_uart_reg0->RTD.THR;
                    else
                        break;
                }
                Ring_Write(&tg_uart0_cb.recv_ring_buf,ch,num);
                Lock_SempPost(tg_uart0_cb.recv_buf_semp);
            }break;
            case 3:     //出错标志
            {
            }break;
            default: return 0;
        }
    }
    return 0;
}

void __uart0_pause_send_int(void)
{
    pg_uart_reg0->DI.IER &=  ~(u32)UART_INTCFG_THRE;          //禁止发送中断，
}
void __uart0_resume_send_int(void)
{
    pg_uart_reg0->DI.IER |=  (u32)UART_INTCFG_THRE;           //使能发送中断，
}
void uart0_send(u8 *buf,u32 len)
{
    u32 completed = 0,sended;
    if((len==0) || (buf == NULL))
        return;
    __uart0_pause_send_int();
    if(Ring_IsEmpty(&tg_uart0_cb.send_ring_buf))
    {
        if(pg_uart_reg0->LSR &UART_LS_TX_EMPTY)
        {
            if(len < 16)
            {
                for(completed = 0; completed < len; completed++)
                    pg_uart_reg0->RTD.THR = buf[completed];
            }else
            {
                for(completed = 0; completed < 16; completed++)
                    pg_uart_reg0->RTD.THR = buf[completed];
            }
        }
    }
    if(completed < len)
    {
        while(1)
        {
            sended = Ring_Write(&tg_uart0_cb.send_ring_buf,
                                (uint8_t*)buf+completed,len-completed);
            __uart0_resume_send_int();
            if(sended != len-completed)     //缓冲区满，没有送出全部数据
            {
                completed += sended;
                Lock_SempPend(tg_uart0_cb.send_buf_semp,CN_TIMEOUT_FOREVER);
                __uart0_pause_send_int();
            }else
            {
                break;
            }
        }
    }
}

//========================================================
// 函数功能：串口0接收数据
// 输入参数：无
// 输出参数：无
// 返回值  ：无
//========================================================
u32 uart0_read(u8 *buf,u32 len,u32 timeout)
{
    u32 readed,completed=0,start_time,time=0;
    start_time = DjyGetTime();
    while(1)
    {

        readed = Ring_Read(&tg_uart0_cb.recv_ring_buf,buf+completed,len-completed);

        if(readed != len-completed)
        {
            completed += readed;
            if(Lock_SempPend(tg_uart0_cb.recv_buf_semp,timeout-time) == false)
            {
                break;
            }else
            {
                time = DjyGetTime() - start_time;     //回到while处再次读串口。
            }
        }else
        {
            completed = len;                            //已经完成，
            break;
        }

    }
//  if(completed>len)
//      time=0;
    return completed;
}
#endif  //for (CN_CFG_UART0_USED == 1)

#if(CN_CFG_UART1_USED == 1)

u32 __uart1_tx(ufast_t uart_tx_line);
u32 __uart1_rx(ufast_t uart_rx_line);

//----设置uart0 baud--------------------------------------------------------
//功能: 设置uart0 baud,注:本函数未处理小数baud
//参数: baud
//返回: 无
//-----------------------------------------------------------------------------
void __uart1_set_baud(u32 baud)
{
    u32 n;
    n = (CN_CFG_MCLK / baud)>>4;
    pg_uart_reg1->LCR |= UART_LCR_DLAB_EN;
    pg_uart_reg1->RTD.DLL = (u8)(n % 256);
    pg_uart_reg1->DI.DLM = (u8)(n / 256);
    pg_uart_reg1->LCR &= (~UART_LCR_DLAB_EN) & 0xff;
    pg_uart_reg1->FDR = 0;    //暂未处理小数
}

//----初始化uart0模块----------------------------------------------------------
//功能：初始化uart0模块，初始化状态为不使用dma传送的方式。
//参数：para，在本函数中无效
//返回：true = 成功初始化，false = 初始化失败
//-----------------------------------------------------------------------------
ptu32_t module_init_uart1(ptu32_t para)
{
    //保护缓冲区的信号量，使缓冲区中数据量为0时阻塞写入线程，读取线程使缓冲区中
    //数据降至trigger_level以下时释放信号量，使写入线程解除阻塞
    tg_uart1_cb.send_buf_semp = Lock_SempCreate(1,0,"uart0 tx buf");
    if(tg_uart1_cb.send_buf_semp == NULL)
        goto exit_from_left_buf_semp;
    tg_uart1_cb.recv_buf_semp = Lock_SempCreate(1,0,"uart0 rx buf");
    if(tg_uart1_cb.recv_buf_semp == NULL)
        goto exit_from_right_buf_semp;
    //初始化IO端口位uart功能
    sys_enable_peri(cn_peri_uart1);
    sys_enable_peri_clk(cn_peri_clksrc_uart1);
    sys_set_uart1_clkdiv(1);


    __uart1_set_baud(UART1_BRD);     //设置波特率为宏定义中的数值

  //设置波特率为1843200
//  LPC_UART1->LCR |= UART_LCR_DLAB_EN;
//  LPC_UART1->RTD.DLL = 1;
//  LPC_UART1->DI.DLM = 0;
//  LPC_UART1->LCR &= (~UART_LCR_DLAB_EN) & 0xff;
//  LPC_UART1->FDR = 0x92; //0x92=1843200   0x01=2.25M
  //结束设置波特率

/*  ////////////ADD BY HUANGCW
    LPC_UART1->FDR = 0x10;
    LPC_UART1->IF.FCR = 0;
    while (LPC_UART1->LSR & 0x01)
    {
        tmp = LPC_UART1->RTD.RBR;        // Dummy reading
    }
    LPC_UART1->TER = UART_TER_TXEN ;
    while (!(LPC_UART1->LSR & UART_LS_TX_EMPTY));  // Wait for current transmit complete
    LPC_UART1->TER = 0;             // Disable Tx
    LPC_UART1->DI.IER = 0;             // Disable interrupt
    LPC_UART1->LCR = 0;             // Set LCR to default state
    LPC_UART1->ACR = 0;             // Set ACR to default state

    tmp = LPC_UART1->LSR;           // Dummy reading

    //配置uart
    uart_set_divisors(LPC_UART1,115200);
    LPC_UART1->LCR = 3 | (0<<2) | (000<<3) | (0<<6) | (0<<7);
    LPC_UART1->IF.FCR = 0x81;
    LPC_UART1->TER |= UART_TER_TXEN;
    LPC_UART1->DI.IER |=  UART_INTCFG_RBR;
    NVIC->ISER[0] = (1 << ((u32)(18) & 0x1F));
    ////////////END
  */
    pg_uart_reg1->IF.IIR;     //读一下清除所有中断标志
//    LPC_UART1->IIR_FCR |= 1;   //set FIFOEN
    pg_uart_reg1->LCR |= UART_LCR_DLAB_EN;
    pg_uart_reg1->IF.FCR = 0x1;   //使能ifo。
    pg_uart_reg1->IF.FCR = 0x47;  //接收4字节触发中断，清收发fifo。
    pg_uart_reg1->LCR = 3 | (0<<2) | (000<<3) | (0<<6) | (0<<7);
    pg_uart_reg1->MCR = 0;
    pg_uart_reg1->TER = UART_TER_TXEN;
    pg_uart_reg1->DI.IER =  UART_INTCFG_RBR;       //使能收发中断，
    pg_uart_reg1->RS485CTRL =      UART_RS485_RECEIVER_ENABLE
                            |   UART_RS485_AUTO_DIRECTION_ENABLE
                            |   UART_RS485_DIRPIN_HIGH
                            |   UART_RS485_DIRPIN_RTS;


    tg_uart1_cb.baud = UART1_BRD;

    tg_uart1_cb.sendding = false;
    Ring_Init(  &tg_uart1_cb.send_ring_buf,
                uart1_drv_send_buf,
                uart1_buf_len);
    Ring_Init(  &tg_uart1_cb.recv_ring_buf,
                uart1_drv_recv_buf,
                uart1_buf_len);

    pg_inuse_line_buf = &uart1_line_buf1;

    Int_IsrConnect(cn_int_line_uart1,__uart1_int);
    Int_SettoReal(cn_int_line_uart1);

    Int_IsrConnect(cn_int_line_wakeup1,__uart1_rx);
    Int_SettoAsynSignal(cn_int_line_wakeup1);
    Int_RestoreAsynLine(cn_int_line_wakeup1);


    Int_IsrConnect(cn_int_line_wakeup2,__uart1_tx);
    Int_SettoAsynSignal(cn_int_line_wakeup2);
    Int_RestoreAsynLine(cn_int_line_wakeup2);

    Int_ClearLine(cn_int_line_uart1);
    Int_RestoreRealLine(cn_int_line_uart1);
    return true;

exit_from_right_buf_semp:
    Lock_SempDelete(tg_uart1_cb.send_buf_semp);
exit_from_left_buf_semp:
    return false;
}

//----uart1中断---------------------------------------------------------------
//功能: 如果是接收信号触发的中断:
//      1.检查接收fifo中的数据量.把数据从接收寄存器全部copy到物理层缓冲区中.
//      2.如果物理层缓冲区满.置串口控制块状态字的溢出位,并发出uart错误事件.
//      3.发送串口接收事件.两个事件都是状态量事件
//      如果是发送信号引发的中断:
//      1.检查发送fifo有多少空位
//      2.从设备右手接口读出适量数据，
//      3.把读出的数据写入发送fifo
//      如果是出错信号引发中断:弹出串口出错事件
//参数: 中断函数没有参数.
//返回: 中断函数没有返回值.
//-----------------------------------------------------------------------------
u32 __uart1_int(ufast_t uart_int_line)
{
    u32 recv_trans,num;
    u32 IIR;
    uint8_t ch[16];
    GPIO_SettoLow(2,1<<14);   //dbg
    while(1)
    {
        IIR = pg_uart_reg1->IF.IIR;
        if(IIR & 1)
            break;
        IIR = (IIR>>1)&0x7;
        switch(IIR)
        {
            case 1:     //发送fifo寄存器空
            {
                recv_trans = Ring_Read(&tg_uart1_cb.send_ring_buf,ch,16);
                if(recv_trans != 0)
                {
                    for(num = 0; num < recv_trans; num++)
                    {
                        pg_uart_reg1->RTD.THR = ch[num];
                    }
                }
                if(recv_trans < 16)
                    __uart1_pause_send_int();
                if(Ring_Check(&tg_uart1_cb.send_ring_buf) < 16)
                    Int_TapLine(cn_int_line_wakeup2);
//                    Lock_SempPost(tg_uart1_cb.send_buf_semp);
            }break;
            case 2:     //接收触发水平到
            case 6:     //接收超时，fifo中至少有一个数据，连续4字符时间无新数据
            {
                for(num = 0; num<16; num++)
                {
                    if(pg_uart_reg1->LSR & 1)
                    {
                        pg_inuse_line_buf->tagLineBuf[pg_inuse_line_buf->pt]
                                        = pg_uart_reg1->RTD.THR;
                        pg_inuse_line_buf->pt++;
                    }else
                        break;
                }
//                Ring_Write(&tg_uart1_cb.recv_ring_buf,ch,num);                  dbg
                Int_TapLine(cn_int_line_wakeup1);
//                Lock_SempPost(tg_uart1_cb.recv_buf_semp);
            }break;
            case 3:     //出错标志
            {
            }break;
            default: return 0;
        }
    }
    GPIO_SettoHigh(2,1<<14);   //dbg
    return 0;
}

void __uart1_pause_send_int(void)
{
    pg_uart_reg1->DI.IER &=  ~(u32)UART_INTCFG_THRE;          //禁止发送中断，
}
void __uart1_resume_send_int(void)
{
    pg_uart_reg1->DI.IER |=  (u32)UART_INTCFG_THRE;           //使能发送中断，
}

void __uart1_pause_recv_int(void)
{
    pg_uart_reg1->DI.IER &=  ~(u32)UART_INTCFG_RBR;          //禁止发送中断，
}
void __uart1_resume_recv_int(void)
{
    pg_uart_reg1->DI.IER |=  (u32)UART_INTCFG_RBR;           //使能发送中断，
}
void uart1_send(u8 *buf,u32 len)
{
    u32 completed = 0,sended;
    if((len==0) || (buf == NULL))
        return;

    UART1_RTS_HIGH();   //使能485发送模式 --by huangcw  2012-1-31
    __uart1_pause_send_int();
    if(Ring_IsEmpty(&tg_uart1_cb.send_ring_buf))
    {
        if(pg_uart_reg1->LSR &UART_LS_TX_EMPTY)
        {
            if(len < 16)
            {
                for(completed = 0; completed < len; completed++)
                    pg_uart_reg1->RTD.THR = buf[completed];
            }else
            {
                for(completed = 0; completed < 16; completed++)
                    pg_uart_reg1->RTD.THR = buf[completed];
            }
        }
    }
    if(completed < len)
    {
        while(1)
        {
            sended = Ring_Write(&tg_uart1_cb.send_ring_buf,
                                (uint8_t*)buf+completed,len-completed);
            __uart1_resume_send_int();
            if(sended != len-completed)     //缓冲区满，没有送出全部数据
            {
                completed += sended;
                Lock_SempPend(tg_uart1_cb.send_buf_semp,CN_TIMEOUT_FOREVER);
                __uart1_pause_send_int();
            }else
            {
                break;
            }
        }
    }

    //add by huangcw --2012-1-31
    while( !( pg_uart_reg1->LSR & UART_LS_TX_EMPTY_ALL ) )//等待所有发送完成
            {
            }
        UART1_RTS_LOW();    //关闭485发送模式
    //end  add
}

//========================================================
// 函数功能：串口0接收数据
// 输入参数：无
// 输出参数：无
// 返回值  ：无
//========================================================
u32 uart1_read(u8 *buf,u32 len,u32 timeout)
{
    u32 readed,completed=0,start_time,time=0;
    start_time = DjyGetTime();

    while(1)
    {

        __uart1_pause_recv_int();
        readed = Ring_Read(&tg_uart1_cb.recv_ring_buf,buf+completed,len-completed);
        __uart1_resume_recv_int();

        if(readed != len-completed)
        {
            completed += readed;
            if(Lock_SempPend(tg_uart1_cb.recv_buf_semp,timeout-time) == false)
            {
                break;
            }else
            {
                time = DjyGetTime() - start_time;     //回到while处再次读串口。
            }
        }else
        {
            completed = len;                            //已经完成，
            break;
        }

    }
//  if(completed>len)
//      time=0;
    return completed;
}

u32 __uart1_tx(ufast_t uart_tx_line)
{
    Lock_SempPost(tg_uart1_cb.send_buf_semp);
    Int_ClearLine(uart_tx_line);
}

u32 __uart1_rx(ufast_t uart_rx_line)
{
    struct uart_line_buf *buf;
    buf = pg_inuse_line_buf;
    __uart1_pause_recv_int();
    if(buf == &uart1_line_buf1)
    {
        pg_inuse_line_buf = &uart1_line_buf2;
        uart1_line_buf2.pt = 0;
    }else
    {
        pg_inuse_line_buf = &uart1_line_buf1;
        uart1_line_buf1.pt = 0;
    }
    __uart1_resume_recv_int();
    Ring_Write(&tg_uart1_cb.recv_ring_buf,buf->tagLineBuf,buf->pt);
    Lock_SempPost(tg_uart1_cb.recv_buf_semp);
    Int_ClearLine(uart_rx_line);
    return 0;
}

#endif  //for (CN_CFG_UART1_USED == 1)





