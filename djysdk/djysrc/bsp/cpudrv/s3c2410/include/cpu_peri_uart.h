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
//作者：liuwei
//版本：V1.0.1
//文件描述: 驱动44b0的uart
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2015-04-22
//   作者: liuwei
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __CPU_UART_H__
#define __CPU_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

//UTRSTAT状态位定义
#define UartTXShiftEmpty    (1<<2)  //1:发送移位寄存器空,0:非空
#define UartTxBufEmpty      (1<<1)  //1:发送缓冲区空,0:非空
#define UartRxBufNotEmpty   (1<<0)  //1:接受缓冲区有数据,0:无数据
//UFCON FIFO控制寄存器
#define cn_tx_fifo_reset    (1<<2)  //写1复位发送fifo,读总是0
#define cn_rx_fifo_reset    (1<<1)  //写1复位发送fifo,读总是0
//UFSTAT FIFO状态寄存器
#define cn_rx_over          (1<<6)  //1:接收fifo溢出,0:正常

//通信状态变量各位定义
#define CN_SER_RXBUF_BUSY         (1<<0)
#define CN_SER_RXBUF_EMPTY     (1<<1)
#define CN_SER_RECEIVE_OVER    (1<<2)

//中断控制位，在2410的中断控制器中，每个串口只分配了一个中断，具体中断原因是通过
//查询 SUBSRCPND 寄存器相应的位来得到的，控制 INTSUBMSK 中相应位可独立地禁止
//接收/发送中断。
#define BIT_SUB_ERR2    (0x1<<8)
#define BIT_SUB_TXD2    (0x1<<7)
#define BIT_SUB_RXD2    (0x1<<6)
#define BIT_SUB_ERR1    (0x1<<5)
#define BIT_SUB_TXD1    (0x1<<4)
#define BIT_SUB_RXD1    (0x1<<3)
#define BIT_SUB_ERR0    (0x1<<2)
#define BIT_SUB_TXD0    (0x1<<1)
#define BIT_SUB_RXD0    (0x1<<0)
#define BIT_ALL_UART    (0x1ff)

#define SUB_RXD_FLG     0
#define SUB_TXD_FLG     1
#define SUB_ERR_FLG     2
#define BIT_SUB_INT(m,n)   (0x1<<(m*3 + n))
//#define BIT_SUB_INT(m,n)   (0x1<<((m-1)*3 + n))

#define UART_TX_FIFO_SIZE  16  //硬件的发送fifo深度
#define UART_RX_FIFO_SIZE  16  //硬件的接收fifo深度


struct UART_REG{
    vu32 ULCON;                   // line control reg
    vu32 UCON;                    // control reg
    vu32 UFCON;                   // FIFO control reg
    vu32 UMCON;                   // modem control reg
    vu32 UTRSTAT;                 // tx/rx status reg
    vu32 UERSTAT;                 // rx error status reg
    vu32 UFSTAT;                  // FIFO status reg
    vu32 UMSTAT;                  // modem status reg
    vu32 UTXH;                    // tx buffer reg
    vu32 URXH;                    // rx buffer reg
    vu32 UBRDIV;                  // baud rate divisor
    vu32 UDIVSLOT;                // baud rate divisor
};

#define UART0_REG_BASE  ((struct UART_REG*)0x50000000)
#define UART1_REG_BASE  ((struct UART_REG*)0x50004000)
#define UART2_REG_BASE  ((struct UART_REG*)0x50008000)
#define UART3_REG_BASE  ((struct UART_REG*)0x5000C000)

#define CN_UART0       0
#define CN_UART1       1
#define CN_UART2       2
#define CN_UART_NUM    3

ptu32_t UART_ModuleInit(ptu32_t para);

#ifdef __cplusplus
}
#endif

#endif // __CPU_UART_H__

