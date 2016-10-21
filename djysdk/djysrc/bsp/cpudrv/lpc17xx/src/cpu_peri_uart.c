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
// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：cpu_peri_uart.c
// 模块描述: DJYOS串口模块的底层驱动部分，主要实现寄存器级别的操作，如中断等
// 模块版本: V1.10
// 创建人员: hm
// 创建时间: 14/08.2014
// =============================================================================

#include "cpu_peri.h"
#include "string.h"
#include <driver/include/uart.h>
#include "stdio.h"

extern const char *gc_pCfgStdinName;    //标准输入设备
extern const char *gc_pCfgStdoutName;   //标准输出设备
extern const char *gc_pCfgStderrName;   //标准错误输出设备

extern bool_t Board_UartGpioInit(u8 SerialNo);
extern void Board_UartHalfDuplexSend(u8 SerialNo);
extern void Board_UartHalfDuplexRecv(u8 SerialNo);
// =============================================================================
#define tagUartReg              LPC_UART_TypeDef
#define CN_UART0_BASE           LPC_UART0_BASE
#define CN_UART1_BASE           LPC_UART1_BASE
#define CN_UART2_BASE           LPC_UART2_BASE
#define CN_UART3_BASE           LPC_UART3_BASE

static u32 TxByteTime;                    //正常发送一个字节所需要的时间
static tagUartReg *PutStrDirectReg;     //用于printk发送的串口寄存器
static tagUartReg *GetCharDirectReg;     //用于直接接收的串口寄存器
static u32 TxDirectPort;                  //用于printk发送的串口号
static u32 RxDirectPort;                  //用于直接接收的串口号
// =============================================================================
static tagUartReg volatile * const tg_UART_Reg[] = {(tagUartReg *)CN_UART0_BASE,
                                                    (tagUartReg *)CN_UART1_BASE,
                                                    (tagUartReg *)CN_UART2_BASE,
                                                    (tagUartReg *)CN_UART3_BASE};
#define UART0_SendBufLen            32
#define UART0_RecvBufLen            32

#define UART1_SendBufLen            32
#define UART1_RecvBufLen            32

#define UART2_SendBufLen            32
#define UART2_RecvBufLen            32

#define UART3_SendBufLen            32
#define UART3_RecvBufLen            32

static struct UartCB *pUartCB[CN_UART_NUM];
//用于标识串口是否初始化标记，第0位表示UART0，第一位表UART1....
//依此类推，1表示初始化，0表示未初始化
static u8 sUartInited = 0;
// =============================================================================
static ptu32_t UART_ISR(ptu32_t IniLine);

// ----使能接收中断-------------------------------------------------------------
// 功能: 使能uart的接收中断
// 参数: reg,被操作的寄存器组指针
// 返回: 无
//-----------------------------------------------------------------------------
static void __UART_RxIntEnable(tagUartReg volatile *reg)
{
    reg->IER |= (0x01 << 0);
}

// ----禁止接收中断-------------------------------------------------------------
// 功能: 禁止uart的接收中断
// 参数: reg,被操作的寄存器组指针
// 返回: 无
//-----------------------------------------------------------------------------
static void __UART_RxIntDisable(tagUartReg volatile *reg)
{
    reg->IER &= ~(0x01 << 0);
}

// ----使能发送中断-------------------------------------------------------------
// 功能: 使能uart的发送中断
// 参数: reg,被操作的寄存器组指针
// 返回: 无
// -----------------------------------------------------------------------------
static void __UART_TxIntEnable(tagUartReg volatile *reg)
{
    reg->IER |= (0x01 << 1);
}

// ----禁止发送中断-------------------------------------------------------------
// 功能: 禁止uart的发送中断
// 参数: reg,被操作的寄存器组指针
// 返回: 无
// -----------------------------------------------------------------------------
static void __UART_TxIntDisable(tagUartReg volatile *reg)
{
    reg->IER &= ~(0x01 << 1);
}

// ----检查串口发送寄存器是否空--------------------------------------------------
// 功能: 检查发送寄存器的状态，如果空就返回true，若为空，否则返回false
// 参数：reg,被操作的寄存器组指针
// 返回: true = 缓冲区空，false = 非空
// -----------------------------------------------------------------------------
static bool_t __UART_TxTranEmpty(tagUartReg volatile *reg)
{
    return (0 != (reg->LSR & (1<<5)));
}

// =============================================================================
// 功能: 检查接收寄存器的状态，有数据就返回true，否则返回false
// 参数：reg,被操作的寄存器组指针
// 返回: true = 有数据，false = 无数据
// =============================================================================
static bool_t __UART_RxHadChar(tagUartReg volatile *reg)
{
    return (reg->LSR & 0x01);
}

// =============================================================================
// 功能: 设置串口baud,PCLK为25M，CPU主频为100M计算,该驱动只提供9600、19200、57600、
//       115200四种波特率设置,时钟改成100M，根据手册318页所示，
//       2*PCLK/(16*2^15) <= BAUD <= PCLK/(16*(2+data bits+parity bits +stop bits))
//       可算出，当PCLK为100M时，最大波特率不超过560K
// 参数: Reg,被操作的寄存器组指针
//       baud,波特率
// 返回: 无
// =============================================================================
static void __UART_BaudSet(tagUartReg volatile *Reg,u32 baud)
{
    u8 DLL,DLM;
    u32 temp;
    if(Reg == NULL)
        return;

    temp = ( CN_CFG_CCLK / 16 ) / baud ;	/*baud rate */
    DLM = temp / 256;
    DLL = temp % 256;

    Reg->LCR |= (1<<7);         //  DLAB = 1
    Reg->DLL  = DLL;            // 115200
    Reg->DLM  = DLM;
    Reg->LCR &= ~(1<<7);        // DLAB = 0

    if(tg_UART_Reg[TxDirectPort] == Reg)
    {
        TxByteTime = 11000000/baud;     //1个字节传输时间，按10bit，+10%计算
    }
}

// =============================================================================
// 功能: 设置对应UART的IO口，包括时钟和IO配置
// 参数: SerialNo,串口号
// 返回: 无
// =============================================================================
static void __UART_GpioConfig(u8 SerialNo)
{
    //UART 的GPIO初始化应根据板件来定，因此该函数在boarddrv文件夹的board.c中
    Board_UartGpioInit(SerialNo);
}

// =============================================================================
// 功能: 对串口传输参数配置，包括波特率、奇偶校验、数据位、停止位
// 参数:  Reg,被操作的寄存器组指针
//        data,结构体tagCOMParam类型的指针数值
// 返回: 无
// =============================================================================
static void __UART_ComConfig(tagUartReg volatile *Reg,ptu32_t data)
{
    struct COMParam *COM;
    if((data == 0) || (Reg == NULL))
        return;
    COM = (struct COMParam *)data;
    __UART_BaudSet(Reg,COM->BaudRate);

    switch(COM->DataBits)               // data bits
    {
    case CN_UART_DATABITS_5:    Reg->LCR &= ~0x3; Reg->LCR |= 0x00;break;
    case CN_UART_DATABITS_6:    Reg->LCR &= ~0x3; Reg->LCR |= 0x01;break;
    case CN_UART_DATABITS_7:    Reg->LCR &= ~0x3; Reg->LCR |= 0x02;break;
    case CN_UART_DATABITS_8:    Reg->LCR &= ~0x3; Reg->LCR |= 0x03;break;
    default:break;
    }

    switch(COM->Parity)                 // parity
    {
    case CN_UART_PARITY_ODD:    Reg->LCR &=~(0x03<<4);Reg->LCR |=0x00<<4;break;
    case CN_UART_PARITY_EVEN:   Reg->LCR &=~(0x03<<4);Reg->LCR |=0x01<<4;break;
    case CN_UART_PARITY_MARK:   Reg->LCR &=~(0x03<<4);Reg->LCR |=0x02<<4;break;
    case CN_UART_PARITY_SPACE:  Reg->LCR &=~(0x03<<4);Reg->LCR |=0x03<<4;break;
    default:break;
    }

    switch(COM->StopBits)               // stop bits
    {
    case CN_UART_STOPBITS_1:    Reg->LCR &= ~(0x01<<2);break;
    case CN_UART_STOPBITS_1_5:  break;
    case CN_UART_STOPBITS_2:    Reg->LCR |= (0x01<<2);break;
    default:break;
    }
}

// =============================================================================
// 功能: 初始化UART对应的中断线，并初始化中断入口函数
// 参数：SerialNo,串口号
// 返回: 无
// =============================================================================
static void __UART_IntInit(u32 SerialNo)
{
    u8 IntLine;
    if(SerialNo == CN_UART0)
        IntLine = CN_INT_LINE_UART0;
    else if(SerialNo == CN_UART1)
        IntLine = CN_INT_LINE_UART1;
    else if(SerialNo == CN_UART2)
        IntLine = CN_INT_LINE_UART2;
    else if(SerialNo == CN_UART3)
        IntLine = CN_INT_LINE_UART3;
    else
        return;
    Int_Register(IntLine);
    Int_SetClearType(IntLine,CN_INT_CLEAR_AUTO);
    Int_IsrConnect(IntLine,UART_ISR);
    Int_SettoAsynSignal(IntLine);
    Int_ClearLine(IntLine);
    Int_RestoreAsynLine(IntLine);
}

// =============================================================================
// 功能: 硬件参数配置和寄存器的初始化，包括波特率、停止位、校验位、数据位，默认情况下:
//       波特率:115200  ； 停止位:1 ; 校验:偶校验 ; 数据位:8bit
// 参数: SerialNo,串口号，0~3
// 返回: 无
// =============================================================================
static void __UART_HardInit(u8 SerialNo)
{
    if(SerialNo > CN_UART3)
        return;
    __UART_GpioConfig(SerialNo);
    if(SerialNo == CN_UART0)
    {
        LPC_SC->PCONP |= (1<<3);
        LPC_SC->PCLKSEL0 &= ~(0x03<<6);
        LPC_SC->PCLKSEL0 |= (0x01<<6);  //对PCLK 1 分频
    }
    else if(SerialNo == CN_UART1)
    {
        LPC_SC->PCONP |= (1<<4);
        LPC_SC->PCLKSEL0 &= ~(0x03<<8);
        LPC_SC->PCLKSEL0 |= (0x01<<8);
    }
    else if(SerialNo == CN_UART2)
    {
        LPC_SC->PCONP |= (1<<24);
        LPC_SC->PCLKSEL1 &= ~(0x03<<16);
        LPC_SC->PCLKSEL1 |= (0x01<<16);
    }
    else
    {
        LPC_SC->PCONP |= (1<<25);
        LPC_SC->PCLKSEL1 &= ~(0x03<<18);
        LPC_SC->PCLKSEL1 |= (0x01<<18);
    }
    __UART_BaudSet(tg_UART_Reg[SerialNo],115200);

    tg_UART_Reg[SerialNo]->IIR;             // 读一下清除所有中断标志
    tg_UART_Reg[SerialNo]->LCR |= 0x83;     // 8 bits, no Parity, 1 Stop bit
    tg_UART_Reg[SerialNo]->LCR  = 0x03;     // DLAB = 0
    tg_UART_Reg[SerialNo]->FCR  = (0x07 | (0x02<<6)); // 使能FIFO，且配置触发水平8Bytes
    tg_UART_Reg[SerialNo]->TER  = 0x80;     // 发送使能
    tg_UART_Reg[SerialNo]->IER  = 0x01;     // 使能接收中断

}

// =============================================================================
// 功能: 直接发送函数，使用轮询方式。
// 参数: PrivateTag,被操作的串口设备指针.
//       send_buf,发送数据缓冲区
//       len,数据长度，字节
//       timeout,超时时间，us
// 返回: 发送字节数
// =============================================================================
//static u32 __UART_SendDirectly(tagUartReg *Reg,u8 *send_buf,u32 len,u32 timeout)
//{
//    u32  result = 0;
//
//    if(Reg == NULL)
//        return result;
//    __UART_TxIntDisable(Reg);
//    for(result=0; result < len; result ++)
//    {
//        // 超时或者发送缓冲为空时退出
//        while((false == __UART_TxTranEmpty(Reg))&& (timeout > 0))
//        {
//            timeout--;
//            Djy_DelayUs(1);
//        }
//        if(timeout == 0)
//            break;
//        Reg->THR = send_buf[result];
//    }
//    __UART_TxIntEnable(Reg);
//    return result;
//}

// =============================================================================
// 功能: 启动串口发送，其目的是触发中断，用中断方式发送数据。
// 参数: Reg,被操作的串口寄存器指针.
// 返回: 发送的个数
// =============================================================================
static u32 __UART_SendStart (tagUartReg *Reg,u32 timeout)
{
    u8 trans=0,num,ch[16],port;

    switch((u32)Reg)
    {
    case CN_UART0_BASE:   port = CN_UART0;   break;
    case CN_UART1_BASE:   port = CN_UART1;   break;
    case CN_UART2_BASE:   port = CN_UART2;   break;
    case CN_UART3_BASE:   port = CN_UART3;   break;
    default:return 0;
    }

    __UART_TxIntDisable(Reg);
    if(__UART_TxTranEmpty(Reg))
    {
        trans = UART_PortRead(pUartCB[port],ch,16,0);// UART的FIFO大小为16字节
        for(num = 0; num < trans; num++)
        {
            Reg->THR = ch[num];
        }
    }
    __UART_TxIntEnable(Reg);
    return trans;
}

//----串口设备控制函数---------------------------------------------------------
//功能: 串口设备的控制函数,与具体的硬件寄存器设置相关
//参数: Reg,被操作的串口寄存器指针.
//      cmd,操作类型
//      data,含义依cmd而定
//返回: 无意义.
//-----------------------------------------------------------------------------
static ptu32_t __UART_Ctrl(tagUartReg *Reg,u32 cmd, u32 data1,u32 data2)
{
    u8 i;
    for(i =0; i < CN_UART_NUM;i++)
    {
        if(tg_UART_Reg[i] == Reg)
            break;
    }
    if(i == CN_UART_NUM)
        return 0;
    switch(cmd)
    {
        case CN_UART_START:
            __UART_RxIntEnable(Reg);
            __UART_TxIntEnable(Reg);
            break;
        case CN_UART_STOP:
            __UART_RxIntDisable(Reg);
            __UART_TxIntDisable(Reg);
            break;
        case CN_UART_SET_BAUD:  //设置Baud
                __UART_BaudSet(Reg,data1);
            break;
        case CN_UART_RX_PAUSE:      //暂停接收
            __UART_RxIntDisable(Reg);
            break;
        case CN_UART_RX_RESUME:      //恢复接收
            __UART_RxIntEnable(Reg);
            break;
        case CN_UART_RECV_HARD_LEVEL:    //设置接收fifo触发水平
        {
            Reg->FCR &= ~(0x03<<6);
            switch (data1)
            {
                case 1:
                    Reg->FCR |= 0x00<<6;//1字节触发
                    break;
                case 4:
                    Reg->FCR |= 0x01<<6;//4字节触发
                    break;
                case 8:
                    Reg->FCR |= 0x02<<6;//8字节触发
                    break;
                case 14:
                    Reg->FCR |= 0x03<<6;//14字节触发
                    break;
                default :break;
            }
            break;
        }
        case CN_UART_COM_SET:
            __UART_ComConfig(Reg,data1);
            break;
        case CN_UART_HALF_DUPLEX_RECV:
            Board_UartHalfDuplexSend(i);
            break;
        case CN_UART_HALF_DUPLEX_SEND:
            Board_UartHalfDuplexSend(i);
            break;
        default: break;
    }
    return 0;
}

// =============================================================================
// 功能: UART的中断服务函数，包括发送空、接收、错误三种中断。
//      1.接收中断，从接收FIFO中读数据，并调用UART_PortWrite写入接收缓冲区中；
//      2.发送中断，调用UART_PortRead读发送缓冲区中数据，并写入发送FIFO；
//      3.错误中断，弹出错误事件
// 参数: 中断号.
// 返回: 0.
// =============================================================================
static u32 UART_ISR(ptu32_t IntLine)
{
    struct UartCB *UCB;
    volatile tagUartReg *Reg;
    u32 recv_trans,num,IIR;
    u8 ch[16],port;

    switch(IntLine)
    {
    case CN_INT_LINE_UART0:   port = CN_UART0;   break;
    case CN_INT_LINE_UART1:   port = CN_UART1;   break;
    case CN_INT_LINE_UART2:   port = CN_UART2;   break;
    case CN_INT_LINE_UART3:   port = CN_UART3;   break;
    default:return 0;
    }

    UCB = pUartCB[port];
    Reg =  tg_UART_Reg[port];
    while(1)
    {
        IIR = Reg->IIR;
        if(IIR & 0x01)  //无中断
            return 0;
        IIR = (IIR>>1)&0x7;
        switch(IIR)
        {
            case 1:     //发送fifo寄存器空
            {
                recv_trans = UART_PortRead(UCB,ch,16,0);
                if(recv_trans != 0)
                {
                    for(num = 0; num < recv_trans; num++)
                    {
                        Reg->THR = ch[num];
                    }
                }
            }break;
            case 2:     //接收触发水平到
            case 6:     //接收超时，fifo中至少有一个数据，连续4字符时间无新数据
            {
                for(num = 0; num<16; num++)
                {
                    if(Reg->LSR & 0x01)
                        ch[num] = Reg->RBR;
                    else
                        break;
                }
                UART_PortWrite(UCB,ch,num,0);
            }break;
            case 3:     //出错标志
            {
                UART_ErrHandle(UCB,CN_UART_HARD_COMM_ERR);
            }break;
            default: return 0;
        }
    }
    return 0;
}

// =============================================================================
// 功能：初始化uart1模块,并装载串口设备，会调用uart.c的API函数。该函数主要完成的功能
//       如下：
//       1.初始化串口的硬件部分，如GPIO、中断、配置配置的COM参数，配置baud = 115200
//         stop bits = 1,无奇偶校验, data bits = 8
//       2.调用uart.c的API函数UART_InstallPort，装入串口设备
// 参数：串口号，0~3
// 返回：1=成功，0=失败
// =============================================================================
ptu32_t ModuleInstall_UART(u32 serial_no)
{
    struct UartParam UART_Param;

    switch(serial_no)
    {
    case CN_UART0://串口0
        UART_Param.Name         = "UART0";
        UART_Param.Baud         = 115200;
        UART_Param.UartPortTag  = CN_UART0_BASE;
        UART_Param.TxRingBufLen = UART0_SendBufLen;
        UART_Param.RxRingBufLen = UART0_RecvBufLen;
        UART_Param.StartSend    = (UartStartSend)__UART_SendStart;
        UART_Param.UartCtrl     = (UartControl)__UART_Ctrl;
        break;
    case CN_UART1://串口1
        UART_Param.Name         = "UART1";
        UART_Param.Baud         = 115200;
        UART_Param.UartPortTag  = CN_UART1_BASE;
        UART_Param.TxRingBufLen = UART1_SendBufLen;
        UART_Param.RxRingBufLen = UART1_RecvBufLen;
        UART_Param.StartSend    = (UartStartSend)__UART_SendStart;
        UART_Param.UartCtrl     = (UartControl)__UART_Ctrl;
        break;
    case CN_UART2://串口2
        UART_Param.Name         = "UART2";
        UART_Param.Baud         = 115200;
        UART_Param.UartPortTag  = CN_UART2_BASE;
        UART_Param.TxRingBufLen = UART2_SendBufLen;
        UART_Param.RxRingBufLen = UART2_RecvBufLen;
        UART_Param.StartSend    = (UartStartSend)__UART_SendStart;
        UART_Param.UartCtrl     = (UartControl)__UART_Ctrl;
        break;
    case CN_UART3://串口3
        UART_Param.Name         = "UART3";
        UART_Param.Baud         = 115200;
        UART_Param.UartPortTag  = CN_UART3_BASE;
        UART_Param.TxRingBufLen = UART3_SendBufLen;
        UART_Param.RxRingBufLen = UART3_RecvBufLen;
        UART_Param.StartSend    = (UartStartSend)__UART_SendStart;
        UART_Param.UartCtrl     = (UartControl)__UART_Ctrl;
        break;
    default:
        return 0;
    }

    //硬件初始化
    __UART_HardInit(serial_no);
    __UART_IntInit(serial_no);

    sUartInited |= (0x01 << serial_no);
    pUartCB[serial_no] = UART_InstallPort(&UART_Param);
    if( pUartCB[serial_no] == NULL)
        return 0;
    else
        return 1;
}

// =============================================================================
// 功能：字符终端直接发送函数，采用轮询方式，直接写寄存器，用于printk，或者stdout
//       没有初始化
// 参数：str，发送字符串指针
//      len，发送的字节数
// 返回：0，发生错误；result,发送数据长度，字节单位
// =============================================================================
s32 Uart_PutStrDirect(const char *str,u32 len)
{
    u32 result = 0,timeout = TxByteTime * len;
    u16 CR_Bak;

    CR_Bak = GetCharDirectReg->IER;                          //Save INT
    __UART_TxIntDisable(GetCharDirectReg);                   //disable send INT
    for(result=0; result < len+1; result ++)
    {
        // 超时或者发送缓冲为空时退出
        while((false == __UART_TxTranEmpty(PutStrDirectReg))&& (timeout > 10))
        {
            timeout -=10;
            Djy_DelayUs(10);
        }
        if( (timeout <= 10) || (result == len))
            break;
        PutStrDirectReg->THR = str[result];
    }
    PutStrDirectReg->IER = CR_Bak;                         //restore send INT
    return result;
}

// =============================================================================
// 功能：字符终端直接接收函数，采用轮询方式，直接读寄存器，用于stdin初始化前
// 参数：str，发送字符串指针
//      len，发送的字节数
// 返回：0，发生错误；result,发送数据长度，字节单位
// =============================================================================
char Uart_GetCharDirect(void)
{
    u16 CR_Bak;
    char result;

    CR_Bak = GetCharDirectReg->IER;                          //Save INT
    __UART_TxIntDisable(GetCharDirectReg);                   //disable send INT
    while(__UART_RxHadChar(GetCharDirectReg) == false) ;
    result = GetCharDirectReg->RBR;
    PutStrDirectReg->IER = CR_Bak;                         //restore send INT
    return result;
}

//----初始化内核级IO-----------------------------------------------------------
//功能：初始化内核级输入和输出所需的runtime函数指针。
//参数：无
//返回：无
//-----------------------------------------------------------------------------
void Stdio_KnlInOutInit(u32 para)
{
    if(!strcmp(gc_pCfgStdoutName,"/dev/UART0"))
    {
        PutStrDirectReg = (tagUartReg*)CN_UART0_BASE;
        TxDirectPort = CN_UART0;
    }
    else if(!strcmp(gc_pCfgStdoutName,"/dev/UART1"))
    {
        PutStrDirectReg = (tagUartReg*)CN_UART1_BASE;
        TxDirectPort = CN_UART1;
    }
    else if(!strcmp(gc_pCfgStdoutName,"/dev/UART2"))
    {
        PutStrDirectReg = (tagUartReg*)CN_UART2_BASE;
        TxDirectPort = CN_UART2;
    }
    else if(!strcmp(gc_pCfgStdoutName,"/dev/UART3"))
    {
        PutStrDirectReg = (tagUartReg*)CN_UART3_BASE;
        TxDirectPort = CN_UART3;
    }
    else
    {
        PutStrDirectReg = NULL ;
    }

    if(PutStrDirectReg != NULL)
    {
        __UART_HardInit(TxDirectPort);
        TxByteTime = 95;      //初始化默认115200，发送一个byte是87uS,+10%容限
        PutStrDirect = Uart_PutStrDirect;
    }

    if(!strcmp(gc_pCfgStdinName,"/dev/UART0"))
    {
        GetCharDirectReg = (tagUartReg*)CN_UART0_BASE;
        RxDirectPort = CN_UART0;
    }
    else if(!strcmp(gc_pCfgStdinName,"/dev/UART1"))
    {
        GetCharDirectReg = (tagUartReg*)CN_UART1_BASE;
        RxDirectPort = CN_UART1;
    }
    else if(!strcmp(gc_pCfgStdinName,"/dev/UART2"))
    {
        GetCharDirectReg = (tagUartReg*)CN_UART2_BASE;
        RxDirectPort = CN_UART2;
    }
    else if(!strcmp(gc_pCfgStdinName,"/dev/UART3"))
    {
        GetCharDirectReg = (tagUartReg*)CN_UART3_BASE;
        RxDirectPort = CN_UART3;
    }
    else
    {
        GetCharDirectReg = NULL ;
    }
    if(GetCharDirectReg != NULL)
    {
        if(TxDirectPort != RxDirectPort)
            __UART_HardInit(RxDirectPort);
        GetCharDirect = Uart_GetCharDirect;
    }
    return;
}
