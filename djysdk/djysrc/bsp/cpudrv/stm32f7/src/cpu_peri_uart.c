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
// 创建人员:
// 创建时间:
// =============================================================================
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "errno.h"
#include "systime.h"
#include "cpu_peri.h"
#include <driver/include/uart.h>
#include "int.h"
#include "djyos.h"
#include "cpu_peri_uart.h"
#include "cpu_peri_int_line.h"
//#include "cpu_peri_dma.h"

extern const char *gc_pCfgStdinName;    //标准输入设备
extern const char *gc_pCfgStdoutName;   //标准输出设备
//extern const char *gc_pCfgStderrName;   //标准错误输出设备
// =============================================================================
#define CN_UART1_BASE USART1_BASE//0x40013800
#define CN_UART2_BASE USART2_BASE
#define CN_UART3_BASE USART3_BASE
#define CN_UART4_BASE UART4_BASE
#define CN_UART5_BASE UART5_BASE
#define CN_UART6_BASE USART6_BASE
#define CN_UART7_BASE UART7_BASE
#define CN_UART8_BASE UART8_BASE

static u32 TxByteTime;                    //正常发送一个字节所需要的时间
static tagUartReg *PutStrDirectReg;     //用于printk发送的串口寄存器
static tagUartReg *GetCharDirectReg;     //用于直接接收的串口寄存器
static u32 TxDirectPort;                  //用于printk发送的串口号
static u32 RxDirectPort;                  //用于直接接收的串口号
// =============================================================================
static tagUartReg volatile * const tg_UART_Reg[] = {(tagUartReg *)CN_UART1_BASE,
                                                    (tagUartReg *)CN_UART2_BASE,
                                                    (tagUartReg *)CN_UART3_BASE,
                                                    (tagUartReg *)CN_UART4_BASE,
													(tagUartReg *)CN_UART5_BASE,
													(tagUartReg *)CN_UART6_BASE,
													(tagUartReg *)CN_UART7_BASE,
													(tagUartReg *)CN_UART8_BASE,};
#define CN_DMA_UNUSED   0
#define CN_DMA_USED     1

// UART对应的DMA Stream中断号
static u8 const UartDmaRxInt[] = {CN_INT_LINE_DMA2_Stream5,CN_INT_LINE_DMA1_Stream5,
								  CN_INT_LINE_DMA1_Stream1,CN_INT_LINE_DMA1_Stream2,
								  CN_INT_LINE_DMA1_Stream0,CN_INT_LINE_DMA2_Stream2,
								  CN_INT_LINE_DMA1_Stream3,CN_INT_LINE_DMA1_Stream6};

static u8 const UartDmaTxInt[] = {CN_INT_LINE_DMA2_Stream7,CN_INT_LINE_DMA1_Stream6,
								  CN_INT_LINE_DMA1_Stream3,CN_INT_LINE_DMA1_Stream4,
								  CN_INT_LINE_DMA1_Stream7,CN_INT_LINE_DMA2_Stream6,
								  CN_INT_LINE_DMA1_Stream1,CN_INT_LINE_DMA1_Stream0};

static DMA_Stream_TypeDef * const UartDmaRxStream[] =
 	 	 	 	 	 	 	 	 {DMA2_Stream5,DMA1_Stream5,
								  DMA1_Stream1,DMA1_Stream2,
								  DMA1_Stream0,DMA2_Stream2,
								  DMA1_Stream3,DMA1_Stream6};

static DMA_Stream_TypeDef * const UartDmaTxStream[] =
								 {DMA2_Stream7,DMA1_Stream6,
								  DMA1_Stream3,DMA1_Stream4,
								  DMA1_Stream7,DMA2_Stream6,
								  DMA1_Stream1,DMA1_Stream0};
// DMA正在使用标记，是否使用DMA标记
static bool_t s_UART_DmaSending[] = {false,false,false,false,false,false,false,false};
static bool_t s_UART_DmaUsed[]    = {false,false,false,false,false,false,false,false};

#define UART1_SendBufLen            32
#define UART1_RecvBufLen            32
#define UART1_DmaBufLen             32
static u8  UART1_DmaSendBuf[UART1_DmaBufLen]__attribute__ ((section(".nocacheram")));
static u8  UART1_DmaRecvBuf[UART1_DmaBufLen]__attribute__ ((section(".nocacheram")));

#define UART2_SendBufLen            32
#define UART2_RecvBufLen            32
#define UART2_DmaBufLen             32
static u8  UART2_DmaSendBuf[UART2_DmaBufLen]__attribute__ ((section(".nocacheram")));
static u8  UART2_DmaRecvBuf[UART2_DmaBufLen]__attribute__ ((section(".nocacheram")));

#define UART3_SendBufLen            32
#define UART3_RecvBufLen            32
#define UART3_DmaBufLen             32
static u8  UART3_DmaSendBuf[UART3_DmaBufLen]__attribute__ ((section(".nocacheram")));
static u8  UART3_DmaRecvBuf[UART3_DmaBufLen]__attribute__ ((section(".nocacheram")));

#define UART4_SendBufLen            32
#define UART4_RecvBufLen            32
#define UART4_DmaBufLen             32
static u8  UART4_DmaSendBuf[UART4_DmaBufLen]__attribute__ ((section(".nocacheram")));
static u8  UART4_DmaRecvBuf[UART4_DmaBufLen]__attribute__ ((section(".nocacheram")));

#define UART5_SendBufLen            32
#define UART5_RecvBufLen            32
#define UART5_DmaBufLen             32
static u8  UART5_DmaSendBuf[UART5_DmaBufLen]__attribute__ ((section(".nocacheram")));
static u8  UART5_DmaRecvBuf[UART5_DmaBufLen]__attribute__ ((section(".nocacheram")));

#define UART6_SendBufLen            32
#define UART6_RecvBufLen            32
#define UART6_DmaBufLen             32
static u8  UART6_DmaSendBuf[UART6_DmaBufLen]__attribute__ ((section(".nocacheram")));
static u8  UART6_DmaRecvBuf[UART6_DmaBufLen]__attribute__ ((section(".nocacheram")));

#define UART7_SendBufLen            32
#define UART7_RecvBufLen            32
#define UART7_DmaBufLen             32
static u8  UART7_DmaSendBuf[UART7_DmaBufLen]__attribute__ ((section(".nocacheram")));
static u8  UART7_DmaRecvBuf[UART7_DmaBufLen]__attribute__ ((section(".nocacheram")));

#define UART8_SendBufLen            32
#define UART8_RecvBufLen            32
#define UART8_DmaBufLen             32
static u8  UART8_DmaSendBuf[UART8_DmaBufLen]__attribute__ ((section(".nocacheram")));
static u8  UART8_DmaRecvBuf[UART8_DmaBufLen]__attribute__ ((section(".nocacheram")));

static struct UartCB *pUartCB[CN_UART_NUM];

//用于标识串口是否初始化标记，第0位表示UART0，第一位表UART1....
//依此类推，1表示初始化，0表示未初始化
static u8 sUartInited = 0;

// =============================================================================
static ptu32_t UART_ISR(ptu32_t IniLine);

uint32_t UART_DmaRx_ISR(ptu32_t IntLine);
uint32_t UART_DmaTx_ISR(ptu32_t IntLine);

// =============================================================================
// 功能: 禁止uart的接收中断。
// 参数: flag,是否使用dma传输
//       port,串口号，1~8
// 返回: 无
// =============================================================================
void __UART_RxIntEnable(u8 flag,u8 port)
{
    if(flag == CN_DMA_USED)
        Int_RestoreAsynLine(UartDmaRxInt[port]);
    else
        tg_UART_Reg[port]->CR1 |= (1<<5);//中断使能
}

// =============================================================================
// 功能: 禁止uart的接收中断
// 参数: reg,被操作的寄存器组指针
//       port,串口号
// 返回: 无
// =============================================================================
void __UART_RxIntDisable(u8 flag, u8 port)
{
    if(flag == CN_DMA_USED)
        Int_SaveAsynLine(UartDmaRxInt[port]);
    else
        tg_UART_Reg[port]->CR1 &= ~(1<<5);
}

// =============================================================================
// 功能: 使能uart的发送中断
// 参数: reg,被操作的寄存器组指针
//       port,串口号
// 返回: 无
// =============================================================================
static void __UART_TxIntEnable(u8 flag,u8 port)
{
    if(flag == CN_DMA_USED)
        Int_RestoreAsynLine(UartDmaTxInt[port]);
    else
        tg_UART_Reg[port]->CR1 |= (1<<7);
}

// =============================================================================
// 功能: 禁止UART的发送中断
// 参数: Reg,被操作的寄存器组指针
//       port,串口号
// 返回: 无
// =============================================================================
static void __UART_TxIntDisable(u8 flag, u8 port)
{
    if(flag == CN_DMA_USED)
        Int_SaveAsynLine(UartDmaTxInt[port]);
    else
        tg_UART_Reg[port]->CR1 &= ~(1<<7);//禁止发送空中断
}

// =============================================================================
// 功能: 检查发送寄存器的状态，
// 参数：reg,被操作的寄存器组指针
// 返回: true = 缓冲区空，false = 非空
// =============================================================================
static bool_t __UART_TxTranEmpty(tagUartReg volatile *reg)
{
    return ((reg->ISR & (1<<7)) !=0);
}

// =============================================================================
// 功能: 检查接收寄存器的状态，有数据就返回true，否则返回false
// 参数：reg,被操作的寄存器组指针
// 返回: true = 有数据，false = 无数据
// =============================================================================
static bool_t __UART_RxHadChar(tagUartReg volatile *reg)
{
    return ((reg->ISR & (1<<5)) !=0);
}

// =============================================================================
// 功能: 设置串口baud,PCLK为25M，CPU主频为216M计算,该驱动只提供9600、19200、57600、
//       115200四种波特率设置
// 参数: Reg,被操作的寄存器组指针
//       port,串口号
//       baud,波特率
// 返回: 无
// =============================================================================
static void __UART_BaudSet(tagUartReg volatile *Reg,u32 port,u32 baud)
{
    u32 mantissa,fraction;
    float temp;
    if((port == CN_UART1) || (port == CN_UART6))
    {
        temp = (float)CN_CFG_PCLK2/(16*baud);
        mantissa = temp;
        fraction = (temp - mantissa)*16;
    }
    else
    {
        temp = (float)CN_CFG_PCLK1/(16*baud);
        mantissa = temp;
        fraction = (temp - mantissa)*16;
    }

    Reg->BRR =  ((u16)mantissa<<4)+(u16)fraction;

    if(TxDirectPort == port)
    {
        TxByteTime = 11000000/baud;     //1个字节传输时间，按10bit，+10%计算
    }
}

// =============================================================================
// 功能: 设置对应UART的IO口，包括时钟和IO配置
// 参数: SerialNo,串口号
// 注 :因串口复用引脚不确定性在需要初始化需要的引脚
// 返回: 无
// =============================================================================
static void __UART_GpioConfig(u8 SerialNo)
{

	switch(SerialNo)
	{
	case CN_UART1:
		RCC->APB2ENR |=(1<<4);//串口时钟使能
		GPIO_PowerOn(GPIO_A);//GPIO时钟使能
		GPIO_AFSet( GPIO_A,PIN10|PIN9, AF7);//设置复用功能
		GPIO_CfgPinFunc(GPIO_A,PIN10|PIN9,GPIO_MODE_AF,
						GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);

//		GPIO_PowerOn(GPIO_B);//GPIO时钟使能
//		GPIO_AFSet( GPIO_B,PIN6|PIN7, AF7);//设置复用功能
//		GPIO_CfgPinFunc(GPIO_B,PIN6|PIN7,GPIO_MODE_AF,
//						GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);

		break;
	case CN_UART2:
		RCC->APB1ENR |=(1<<17);
		GPIO_PowerOn(GPIO_A);//GPIO时钟使能
		GPIO_AFSet( GPIO_A,PIN2|PIN3, AF7);
		GPIO_CfgPinFunc(GPIO_A,PIN2|PIN3,GPIO_MODE_AF,
				GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);

//		GPIO_PowerOn(GPIO_A);//GPIO时钟使能
//		GPIO_AFSet( GPIO_D,PIN5|PIN6, AF7);
//		GPIO_CfgPinFunc(GPIO_A,PIN5|PIN6,GPIO_MODE_AF,
//				GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);

		break;
	case CN_UART3:
		RCC->APB1ENR |=(1<<18);
		GPIO_PowerOn(GPIO_D);//GPIO时钟使能
		GPIO_AFSet( GPIO_D,PIN8|PIN9, AF7);
		GPIO_CfgPinFunc(GPIO_D,PIN8|PIN9,GPIO_MODE_AF,
				GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);

//		GPIO_PowerOn(GPIO_C);//GPIO时钟使能
//		GPIO_AFSet( GPIO_C,PIN10|PIN11, AF7);
//		GPIO_CfgPinFunc(GPIO_C,PIN10|PIN11,GPIO_MODE_AF,
//				GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);

//		GPIO_PowerOn(GPIO_B);//GPIO时钟使能
//		GPIO_AFSet( GPIO_B,PIN10|PIN11, AF7);
//		GPIO_CfgPinFunc(GPIO_B,PIN10|PIN11,GPIO_MODE_AF,
//				GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);

		break;
	case CN_UART4:
		RCC->APB1ENR |=(1<<19);
		GPIO_PowerOn(GPIO_A);//GPIO时钟使能
		GPIO_AFSet( GPIO_A,PIN0|PIN1, AF8);
		GPIO_CfgPinFunc(GPIO_A,PIN0|PIN1,GPIO_MODE_AF,
				GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);

//		GPIO_PowerOn(GPIO_C);//GPIO时钟使能
//		GGPIO_AFSet( GPIO_C,PIN10|PIN11, AF8);
//		GPIO_CfgPinFunc(GPIO_C,PIN10|PIN11,GPIO_MODE_AF,
//				GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);

		break;
	case CN_UART5:
		RCC->APB1ENR |=(1<<20);
		GPIO_PowerOn(GPIO_C);//GPIO时钟使能
		GPIO_AFSet( GPIO_C,PIN12, AF8);
		GPIO_CfgPinFunc(GPIO_C,PIN12,GPIO_MODE_AF,
				GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);

		GPIO_PowerOn(GPIO_D);//GPIO时钟使能
		GPIO_AFSet( GPIO_D,PIN2, AF8);//C12 D2
		GPIO_CfgPinFunc(GPIO_D,PIN2,GPIO_MODE_AF,
				GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);
		break;
	case CN_UART6:
		RCC->APB2ENR |=(1<<5);
		GPIO_PowerOn(GPIO_C);//GPIO时钟使能
		GPIO_AFSet( GPIO_C,PIN6|PIN7, AF8);
		GPIO_CfgPinFunc(GPIO_C,PIN6|PIN7,GPIO_MODE_AF,
				GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);

//		GPIO_PowerOn(GPIO_G);//GPIO时钟使能
//		GPIO_AFSet( GPIO_G,PIN9|PIN14, AF8);
//		GPIO_CfgPinFunc(GPIO_G,PIN9|PIN14,GPIO_MODE_AF,
//				GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);
		break;
	case CN_UART7:
		RCC->APB1ENR |=(1<<30);
		GPIO_PowerOn(GPIO_F);//GPIO时钟使能
		GPIO_AFSet( GPIO_F,PIN6|PIN7, AF8);
		GPIO_CfgPinFunc(GPIO_F,PIN6|PIN7,GPIO_MODE_AF,
				GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);

//		GPIO_PowerOn(GPIO_E);//GPIO时钟使能
//		GPIO_AFSet( GPIO_E,PIN8|PIN7, AF8);
//		GPIO_CfgPinFunc(GPIO_E,PIN8|PIN7,GPIO_MODE_AF,
//				GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);
		break;
	case CN_UART8:
		RCC->APB1ENR |=(1<<31);
		GPIO_PowerOn(GPIO_E);//GPIO时钟使能
		GPIO_AFSet( GPIO_E,PIN0|PIN1, AF8);
		GPIO_CfgPinFunc(GPIO_E,PIN0|PIN1,GPIO_MODE_AF,
				GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);
		break;
	default: break;

	}

}

// =============================================================================
// 功能: 对串口传输参数配置，包括波特率、奇偶校验、数据位、停止位
// 参数:  Reg,被操作的寄存器组指针
//        port,串口号
//        data,结构体tagCOMParam类型的指针数值
// 返回: 无
// =============================================================================
static void __UART_ComConfig(tagUartReg volatile *Reg,u32 port,ptu32_t data)
{
    struct COMParam *COM;
    if((data == 0) || (Reg == NULL))
        return;
    COM = (struct COMParam *)data;
    __UART_BaudSet(Reg,port,COM->BaudRate);

    switch(COM->DataBits)               // data bits
    {
		case CN_UART_DATABITS_7:
			Reg->CR1 |= (1<<28);
			Reg->CR1 &= ~(1<<12);
			break;
		case CN_UART_DATABITS_8:
			Reg->CR1 &= ~(1<<28);
			Reg->CR1 &= ~(1<<12);
		break;
		case CN_UART_DATABITS_9:
			Reg->CR1 &= ~(1<<28);
			Reg->CR1 |= (1<<12);break;
		default:break;
    }

    switch(COM->Parity)                 // parity
    {
    Reg->CR1 |= (1<<10);                // enable parity
    case CN_UART_PARITY_ODD:    Reg->CR1 |= (1<<9);break;//奇效验
    case CN_UART_PARITY_EVEN:   Reg->CR1 &=~(1<<9);break;
    default:break;
    }

    switch(COM->StopBits)  //todu:F7的 0.5个停止位没有写进来
    {
    Reg->CR2 &= ~(3<<12);
//    case CN_UART_STOPBITS_0_5;  Reg->CR2 |= (1<<12);break;
    case CN_UART_STOPBITS_1:    Reg->CR2 |= (0<<12);break;
    case CN_UART_STOPBITS_1_5:  Reg->CR2 |= (3<<12);break;
    case CN_UART_STOPBITS_2:    Reg->CR2 |= (2<<12);break;
    default:break;
    }
}

// =============================================================================
// 功能: 使串口开始工作，时钟和uart模块
// 参数: port,串口号
// 返回: 无
// =============================================================================
void __UART_Enable(u32 port)
{
    switch(port)
    {//使能串口1时钟
		case CN_UART1:
			RCC->APB2ENR |=(1<<4);break;
    	case CN_UART2:
    		RCC->APB1ENR |=(1<<17);break;
    	case CN_UART3:
    		RCC->APB1ENR |=(1<<18);break;
    	case CN_UART4:
    		RCC->APB1ENR |=(1<<19);break;
    	case CN_UART5:
    		RCC->APB1ENR |=(1<<20);break;
    	case CN_UART6:
    		RCC->APB2ENR |=(1<<5);break;
    	case CN_UART7:
    		RCC->APB1ENR |=(1<<30);break;
    	case CN_UART8:
    		RCC->APB1ENR |=(1<<31);break;
		default:
			break;
    }
    tg_UART_Reg[port]->CR1 |= (1<<13);
}

// =============================================================================
// 功能: 使串口停止工作，包括时钟和uart模块
// 参数: port,串口号
// 返回: 无
// =============================================================================
void __UART_Disable(u32 port)
{
    switch(port)
    {
		case CN_UART1: 	RCC->APB2ENR &=~(1<<4);  break;
		case CN_UART2:  RCC->APB1ENR &=~(1<<17); break;
		case CN_UART3:	RCC->APB1ENR &=~(1<<18); break;
		case CN_UART4:	RCC->APB1ENR &=~(1<<19); break;
		case CN_UART5:	RCC->APB1ENR &=~(1<<20); break;
		case CN_UART6:	RCC->APB2ENR &=~(1<<5);  break;
		case CN_UART7: 	RCC->APB1ENR &=~(1<<30); break;
		case CN_UART8:  RCC->APB1ENR &=~(1<<31); break;
		default:break;
    }
    tg_UART_Reg[port]->CR1 &= ~(1);
}

// =============================================================================
// 功能: 初始化UART对应的中断线，并初始化中断入口函数
// 参数：SerialNo,串口号
// 返回: 无
// =============================================================================
static void __UART_IntInit(u32 SerialNo)
{
    u8 IntLine;
    switch (SerialNo)
    {
		case CN_UART1:
					IntLine = CN_INT_LINE_USART1;break;
		case CN_UART2:
					IntLine = CN_INT_LINE_USART2;break;
		case CN_UART3:
					IntLine = CN_INT_LINE_USART3;break;
		case CN_UART4:
					IntLine = CN_INT_LINE_UART4; break;
		case CN_UART5:
					IntLine = CN_INT_LINE_UART5; break;
		case CN_UART6:
					IntLine = CN_INT_LINE_USART6;break;
		case CN_UART7:
					IntLine = CN_INT_LINE_UART7; break;
		case CN_UART8:
					IntLine = CN_INT_LINE_UART8; break;
				default:
					return;
	}
    Int_Register(IntLine);
    Int_SetClearType(IntLine,CN_INT_CLEAR_AUTO);
    Int_IsrConnect(IntLine,UART_ISR);
    Int_SettoAsynSignal(IntLine);
    Int_ClearLine(IntLine);
    Int_RestoreAsynLine(IntLine);
}

// =============================================================================
// 功能: 硬件参数配置和寄存器的初始化，包括波特率、停止位、校验位、数据位，默认情况下:
//       波特率:115200  ； 停止位:1 ; 校验:无 ; 数据位:8bit
// 参数: SerialNo,串口号，0~3
// 返回: 无
// =============================================================================
static void __UART_HardInit(u8 SerialNo)
{
    if(SerialNo > CN_UART_NUM)
        return;
    __UART_GpioConfig(SerialNo);
    //系统初始化时已经使中断处于禁止状态，无需再禁止和清除中断。
   //初始化uart硬件控制数据结构
    tg_UART_Reg[SerialNo]->CR1 = 0x00bd;
    tg_UART_Reg[SerialNo]->CR2 = 0x0;
    tg_UART_Reg[SerialNo]->CR3 = 0x0000;
    __UART_BaudSet(tg_UART_Reg[SerialNo],SerialNo,115200);
}

// =============================================================================
// 功能: 查询等待串口发送完毕，超时返回
// 参数: sending，dma发送方式，串口是否已经发送完成
// 返回: 无
// =============================================================================
bool_t __uart_dma_timeout(bool_t sending)
{
    u32 timeout = 1000;
    while((sending == true)&& (timeout > 0))//超时
    {
        timeout--;
        Djy_DelayUs(1);
    }
    if(timeout == 0)
        return true;
    else
        return false;
}
// =============================================================================
// 功能: 启动串口DMA发送，使用DMA启动，在发送前需判断上次DMA是否已经发送完成，通过
//       查看标志UARTx_DMA_SENDING来判断UARTx是否已经发送完成
// 参数: PORT,串口号.
// 返回: 发送的个数
// =============================================================================
u32 __UART_DMA_SendStart(u32 port)
{
    u32 num,addr;
    u8 *DmaSendBuf;
    u32 DmaSendBufLen;

    switch(port)
    {
		case CN_UART1:
			DmaSendBuf = UART1_DmaSendBuf;
			DmaSendBufLen = UART1_DmaBufLen;
			break;
		case CN_UART2:
			DmaSendBuf = UART2_DmaSendBuf;
			DmaSendBufLen = UART2_DmaBufLen;
			break;
		case CN_UART3:
			DmaSendBuf = UART3_DmaSendBuf;
			DmaSendBufLen = UART3_DmaBufLen;
			break;
		case CN_UART4:
			DmaSendBuf = UART4_DmaSendBuf;
			DmaSendBufLen = UART4_DmaBufLen;
			break;
		case CN_UART5:
			DmaSendBuf = UART5_DmaSendBuf;
			DmaSendBufLen = UART5_DmaBufLen;
			break;
		case CN_UART6:
			DmaSendBuf = UART6_DmaSendBuf;
			DmaSendBufLen = UART6_DmaBufLen;
			break;
		case CN_UART7:
			DmaSendBuf = UART7_DmaSendBuf;
			DmaSendBufLen = UART7_DmaBufLen;
			break;
		case CN_UART8:
			DmaSendBuf = UART8_DmaSendBuf;
			DmaSendBufLen = UART8_DmaBufLen;
			break;
		 default:	break;
    }

    if(true == __uart_dma_timeout(s_UART_DmaSending[port]))
        return 0;
    num = UART_PortRead(pUartCB[port],DmaSendBuf,DmaSendBufLen,0);
    addr = (u32)DmaSendBuf;

    if(num > 0)
    {
        DMA_Enable(UartDmaTxStream[port],addr,num); //启动DMA
        s_UART_DmaSending[port] = true;
    }
    return 0;
}

// =============================================================================
// 功能: 启动串口发送，其目的是触发中断，用中断方式发送数据。
// 参数: Reg,被操作的串口寄存器指针.
// 返回: 发送的个数
// =============================================================================
static u32 __UART_SendStart (tagUartReg *Reg,u32 timeout)
{
    u8 port;

    switch((u32)Reg)
    {
    case CN_UART1_BASE:        port = CN_UART1;    break;
    case CN_UART2_BASE:        port = CN_UART2;    break;
    case CN_UART3_BASE:        port = CN_UART3;    break;
    case CN_UART4_BASE:        port = CN_UART4;    break;
    case CN_UART5_BASE:        port = CN_UART5;    break;
    case CN_UART6_BASE:        port = CN_UART6;    break;
    case CN_UART7_BASE:        port = CN_UART7;    break;
    case CN_UART8_BASE:        port = CN_UART8;    break;
    default:return 0;
    }

    __UART_TxIntDisable(s_UART_DmaUsed[port],port);
    if(s_UART_DmaUsed[port])
    {
        __UART_DMA_SendStart(port);
    }
    else
    {
        //非DMA方式，只要使能发送空中断，则触发中断
    }

    __UART_TxIntEnable(s_UART_DmaUsed[port],port);
    return 1;
}

// =============================================================================
// 功能: 设置uart使用dma收发，根据stm32各串口的收发dma通道配置寄存器。将重新初
//       始化所有dma寄存器，因为:
//       1、不使用dma期间可能有其他设备使用dma，从而更改了设置。
//       2、即使uart使用dma期间，也可能软件模块配合的问题导致其他设备使用dma.
// 参数: port,端口号，即串口号，从1~3
// 返回: 无
// =============================================================================
void __UART_SetDmaUsed(u32 port)
{
    u8 *DmaRecvBuf,*DmaSendBuf;
    switch(port)
    {
    case CN_UART1:
        DmaRecvBuf = UART1_DmaRecvBuf;
        DmaSendBuf = UART1_DmaSendBuf;
        break;
    case CN_UART2:
        DmaRecvBuf = UART2_DmaRecvBuf;
        DmaSendBuf = UART2_DmaSendBuf;
        break;
    case CN_UART3:
        DmaRecvBuf = UART3_DmaRecvBuf;
        DmaSendBuf = UART3_DmaSendBuf;
        break;
    case CN_UART4:
        DmaRecvBuf = UART4_DmaRecvBuf;
        DmaSendBuf = UART4_DmaSendBuf;
        break;
    case CN_UART5:
        DmaRecvBuf = UART5_DmaRecvBuf;
        DmaSendBuf = UART5_DmaSendBuf;
        break;
    case CN_UART6:
		DmaRecvBuf = UART6_DmaRecvBuf;
		DmaSendBuf = UART6_DmaSendBuf;
		break;
    case CN_UART7:
		DmaRecvBuf = UART7_DmaRecvBuf;
		DmaSendBuf = UART7_DmaSendBuf;
		break;
    case CN_UART8:
		DmaRecvBuf = UART8_DmaRecvBuf;
		DmaSendBuf = UART8_DmaSendBuf;
		break;
    default :        break;
    }
    __UART_RxIntDisable(CN_DMA_UNUSED,port);//串口中断失能
    __UART_TxIntDisable(CN_DMA_UNUSED,port);
    tg_UART_Reg[port]->CR1 |= (1<<4);//enable idle int

    DMA_Config(UartDmaRxStream[port],4,(u32)&(tg_UART_Reg[port]->RDR),
            (u32)DmaRecvBuf,DMA_DIR_P2M,DMA_DATABITS_8,DMA_DATABITS_8,32);

    DMA_Config(UartDmaTxStream[port],4,(u32)&(tg_UART_Reg[port]->TDR),
            (u32)DmaSendBuf,DMA_DIR_M2P,DMA_DATABITS_8,DMA_DATABITS_8,32);





    tg_UART_Reg[port]->CR3 |= 0x00c0;  //设置串口使用dma收发
    s_UART_DmaUsed[port] = CN_DMA_USED;
    DMA_Enable(UartDmaRxStream[port],(u32)DmaRecvBuf,32);//启动dma通道

    Int_Register(UartDmaTxInt[port]);//注册发送DMA中断
    Int_Register(UartDmaRxInt[port]);//注册接收DMA中断

    Int_SetClearType(UartDmaRxInt[port],CN_INT_CLEAR_AUTO);//关联中断
    Int_IsrConnect(UartDmaRxInt[port],UART_DmaRx_ISR);
    Int_SetClearType(UartDmaTxInt[port],CN_INT_CLEAR_AUTO);
    Int_IsrConnect(UartDmaTxInt[port],UART_DmaTx_ISR);

    Int_SettoAsynSignal(UartDmaRxInt[port]);
    Int_SettoAsynSignal(UartDmaTxInt[port]);
    Int_RestoreAsynLine(UartDmaRxInt[port]);
    Int_RestoreAsynLine(UartDmaTxInt[port]);
}

//=============================================================================
// 功能: 设置不使用DMA，UART不使用DMA收发，每收发一个字节就必须中断一次，不建议在高速
//      连续通信中使用。
// 参数: port,端口号，即串口号，为1~3
// 返回: 无
// =============================================================================
void __UART_SetDmaUnUsed(u32 port)
{
    u8 IntLine;
    if(port == CN_UART1)             IntLine = CN_INT_LINE_USART1;
    else if(port == CN_UART2)        IntLine = CN_INT_LINE_USART2;
    else if(port == CN_UART3)        IntLine = CN_INT_LINE_USART3;
    else if(port == CN_UART4)        IntLine = CN_INT_LINE_UART4;
    else if(port == CN_UART5)        IntLine = CN_INT_LINE_UART5;
    else if(port == CN_UART6)        IntLine = CN_INT_LINE_USART6;
    else if(port == CN_UART7)        IntLine = CN_INT_LINE_UART7;
    else if(port == CN_UART8)        IntLine = CN_INT_LINE_UART8;
	else
	return;

    tg_UART_Reg[port]->CR3 &= ~0x00c0;     //设置不使用dma传输
    Int_RestoreAsynLine(IntLine);
    if(s_UART_DmaUsed[port] == CN_DMA_USED)
    {
        Int_SaveAsynLine(UartDmaRxInt[port]);
        Int_SaveAsynLine(UartDmaTxInt[port]);
        s_UART_DmaUsed[port] = CN_DMA_UNUSED;
    }
    __UART_RxIntEnable(CN_DMA_USED,port);
    __UART_TxIntEnable(CN_DMA_USED,port);
    tg_UART_Reg[port]->CR1 &= ~(1<<4);//disable idle int
}
// =============================================================================
// 功能: 串口设备的控制函数,与具体的硬件寄存器设置相关
// 参数: Reg,被操作的串口寄存器指针.
//       cmd,操作类型
//       data1,data2,含义依cmd而定
// 返回: 无意义.
// =============================================================================
static ptu32_t __UART_Ctrl(tagUartReg *Reg,u32 cmd, u32 data1,u32 data2)
{
    ptu32_t result = 0;
    u32 port;
    if(Reg == NULL)
        return 0;

    switch((u32)Reg)
    {
    case CN_UART1_BASE: port = CN_UART1;break;
    case CN_UART2_BASE: port = CN_UART2;break;
    case CN_UART3_BASE: port = CN_UART3;break;
    case CN_UART4_BASE: port = CN_UART4;break;
    case CN_UART5_BASE: port = CN_UART5;break;
    case CN_UART6_BASE: port = CN_UART6;break;
    default:return 0;
    }

    switch(cmd)
    {
        case CN_UART_START:
            __UART_Enable(port);
            break;
        case CN_UART_STOP:
            __UART_Disable(port);
            break;
        case CN_UART_SET_BAUD:  //设置Baud
             __UART_BaudSet(Reg,port, data1);
            break;
        case CN_UART_EN_RTS:
            Reg->CR3 |= 0x100;
            break;
        case CN_UART_DIS_RTS:
            Reg->CR3 &= ~0x100;
            break;
        case CN_UART_EN_CTS:
            Reg->CR3 |= 0x200;
            break;
        case CN_UART_DIS_CTS:
            Reg->CR3 &= ~0x200;
            break;
        case CN_UART_DMA_USED:
            __UART_SetDmaUsed(port);
            break;
        case CN_UART_DMA_UNUSED:
            __UART_SetDmaUnUsed(port);
            break;
        case CN_UART_COM_SET:
            __UART_ComConfig(Reg,port,data1);
            break;
        default: break;
    }
    return result;
}

// =============================================================================
// 功能: 当uart2配置成dma方式是，dma1 ch7负责发送，ch6负责接收
// 参数: 中断函数没有参数.
// 返回: 中断函数没有返回值.
// =============================================================================
uint32_t UART_DmaTx_ISR(ptu32_t tagIntLine)
{
    uint16_t num,port;
    u8 *DmaSendBuf;
    u32 DmaSendBufLen;

    for(port = CN_UART1; port <= CN_UART8;port++)
    {
        if(tagIntLine == UartDmaTxInt[port])
            break;
    }
    switch(port)
    {
    case CN_UART1:
        DmaSendBuf = UART1_DmaSendBuf;
        DmaSendBufLen = UART1_DmaBufLen;
        break;
    case CN_UART2:
        DmaSendBuf = UART2_DmaSendBuf;
        DmaSendBufLen = UART2_DmaBufLen;
        break;
    case CN_UART3:
        DmaSendBuf = UART3_DmaSendBuf;
        DmaSendBufLen = UART3_DmaBufLen;
        break;
    case CN_UART4:
        DmaSendBuf = UART4_DmaSendBuf;
        DmaSendBufLen = UART4_DmaBufLen;
        break;
    case CN_UART5:
        DmaSendBuf = UART5_DmaSendBuf;
        DmaSendBufLen = UART5_DmaBufLen;
        break;
    case CN_UART6:
        DmaSendBuf = UART6_DmaSendBuf;
        DmaSendBufLen = UART6_DmaBufLen;
        break;
    case CN_UART7:
        DmaSendBuf = UART7_DmaSendBuf;
        DmaSendBufLen = UART7_DmaBufLen;
        break;
    case CN_UART8:
        DmaSendBuf = UART8_DmaSendBuf;
        DmaSendBufLen = UART8_DmaBufLen;
        break;
    default:
        break;
    }

    DMA_ClearIntFlag(UartDmaTxStream[port]);
    num = UART_PortRead(pUartCB[port],DmaSendBuf,DmaSendBufLen,0);
    if(num == 0)
    {
        s_UART_DmaSending[port] = false;
    }
    else
    {
        DMA_Enable(UartDmaTxStream[port],(u32)DmaSendBuf,num);
    }
    return 0;
}

// =============================================================================
// 功能: 当uart1配置成dma方式是，dma2 ch2负责发送，ch7负责接收
// 参数: 中断函数没有参数.
// 返回: 中断函数没有返回值.
// =============================================================================
uint32_t UART_DmaRx_ISR(ptu32_t tagIntLine)
{
    u32 recvs,num,DmaRecvBufLen;
    u8 *DmaRecvBuf;
    u8 port;
    for(port = CN_UART1; port <= CN_UART8;port++)
    {
        if(tagIntLine == UartDmaRxInt[port])
            break;
    }
    switch(port)
    {
    case CN_UART1:
        DmaRecvBuf = UART1_DmaRecvBuf;
        DmaRecvBufLen = UART1_DmaBufLen;
        break;
    case CN_UART2:
        DmaRecvBuf = UART2_DmaRecvBuf;
        DmaRecvBufLen = UART2_DmaBufLen;
        break;
    case CN_UART3:
        DmaRecvBuf = UART3_DmaRecvBuf;
        DmaRecvBufLen = UART3_DmaBufLen;
        break;
    case CN_UART4:
        DmaRecvBuf = UART4_DmaRecvBuf;
        DmaRecvBufLen = UART4_DmaBufLen;
        break;
    case CN_UART5:
        DmaRecvBuf = UART5_DmaRecvBuf;
        DmaRecvBufLen = UART5_DmaBufLen;
        break;
    case CN_UART6:
        DmaRecvBuf = UART6_DmaRecvBuf;
        DmaRecvBufLen = UART6_DmaBufLen;
        break;
    case CN_UART7:
        DmaRecvBuf = UART7_DmaRecvBuf;
        DmaRecvBufLen = UART7_DmaBufLen;
        break;
    case CN_UART8:
        DmaRecvBuf = UART8_DmaRecvBuf;
        DmaRecvBufLen = UART8_DmaBufLen;
        break;
    default:
        break;
    }

    DMA_Disable(UartDmaRxStream[port]);
    DMA_ClearIntFlag(UartDmaRxStream[port]);
    recvs = DmaRecvBufLen - UartDmaRxStream[port]->NDTR;
    if(recvs == DmaRecvBufLen)     //dma缓冲区满，可能有数据丢失
    {
        UART_ErrHandle(pUartCB[port],CN_UART_FIFO_OVER_ERR);
    }else
    {
        num = UART_PortWrite(pUartCB[port],DmaRecvBuf,recvs,0);
        if(num != recvs)
        {
            UART_ErrHandle(pUartCB[port],CN_UART_BUF_OVER_ERR);
        }
    }

    DMA_Enable(UartDmaRxStream[port],(u32)DmaRecvBuf,DmaRecvBufLen);
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
u32 UART_ISR(ptu32_t IntLine)
{
    struct UartCB *UCB;
    tagUartReg *Reg;
    u32 num,port;
    u8 ch;

    switch(IntLine)
    {
		case CN_INT_LINE_USART1:   port = CN_UART1;   break;
		case CN_INT_LINE_USART2:   port = CN_UART2;   break;
		case CN_INT_LINE_USART3:   port = CN_UART3;   break;
		case CN_INT_LINE_UART4:    port = CN_UART4;   break;
		case CN_INT_LINE_UART5:    port = CN_UART5;   break;
		case CN_INT_LINE_USART6:   port = CN_UART6;   break;
		case CN_INT_LINE_UART7:    port = CN_UART7;   break;
		case CN_INT_LINE_UART8:    port = CN_UART8;   break;
		default:return 0;
    }

    UCB = pUartCB[port];
    Reg = (tagUartReg *)tg_UART_Reg[port];
    if( (Reg->ISR & (1<<5)) &&(Reg->CR1 &(1<<5)))           //接收中断
    {
        ch = (u8)Reg->RDR;
        num = UART_PortWrite(UCB,&ch,1,0);
        if(num != 1)
        {
            UART_ErrHandle(UCB,CN_UART_BUF_OVER_ERR);
        }
    }
    if((Reg->ISR & (1<<6)) &&(Reg->CR1 & (1<<7)))    //发送中断
    {
        num = UART_PortRead(UCB,&ch,1,0);
        if(num != 0)
            Reg->TDR = ch;
        else
        {
            Reg->CR1 &= ~(1<<7);        //txeie
        }
    }
    if(Reg->ISR & (1<<3))        //ORE过载错误
    {
        ch = (u8)Reg->RDR;
        num = Reg->ISR;     //读一下sr寄存器
        UART_ErrHandle(UCB,CN_UART_FIFO_OVER_ERR);
    }
    if(Reg->ISR & (1<<4))
    {
        Reg->ICR |=(1<<4);
        Int_TapLine(UartDmaRxInt[port]);
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
        break;
    case CN_UART4://串口4
        UART_Param.Name         = "UART4";
        UART_Param.Baud         = 115200;
        UART_Param.UartPortTag  = CN_UART4_BASE;
        UART_Param.TxRingBufLen = UART4_SendBufLen;
        UART_Param.RxRingBufLen = UART4_RecvBufLen;
        UART_Param.StartSend    = (UartStartSend)__UART_SendStart;
        UART_Param.UartCtrl     = (UartControl)__UART_Ctrl;
        break;
    case CN_UART5://串口5
        UART_Param.Name         = "UART5";
        UART_Param.Baud         = 115200;
        UART_Param.UartPortTag  = CN_UART5_BASE;
        UART_Param.TxRingBufLen = UART5_SendBufLen;
        UART_Param.RxRingBufLen = UART5_RecvBufLen;
        UART_Param.StartSend    = (UartStartSend)__UART_SendStart;
        UART_Param.UartCtrl     = (UartControl)__UART_Ctrl;
        break;
    case CN_UART6://串口6
        UART_Param.Name         = "UART6";
        UART_Param.Baud         = 115200;
        UART_Param.UartPortTag  = CN_UART6_BASE;
        UART_Param.TxRingBufLen = UART6_SendBufLen;
        UART_Param.RxRingBufLen = UART6_RecvBufLen;
        UART_Param.StartSend    = (UartStartSend)__UART_SendStart;
        UART_Param.UartCtrl     = (UartControl)__UART_Ctrl;
        break;
    case CN_UART7://串口7
        UART_Param.Name         = "UART7";
        UART_Param.Baud         = 115200;
        UART_Param.UartPortTag  = CN_UART7_BASE;
        UART_Param.TxRingBufLen = UART7_SendBufLen;
        UART_Param.RxRingBufLen = UART7_RecvBufLen;
        UART_Param.StartSend    = (UartStartSend)__UART_SendStart;
        UART_Param.UartCtrl     = (UartControl)__UART_Ctrl;
        break;
    case CN_UART8://串口8
        UART_Param.Name         = "UART8";
        UART_Param.Baud         = 115200;
        UART_Param.UartPortTag  = CN_UART8_BASE;
        UART_Param.TxRingBufLen = UART8_SendBufLen;
        UART_Param.RxRingBufLen = UART8_RecvBufLen;
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

    CR_Bak = PutStrDirectReg->CR1;                          //Save INT
    PutStrDirectReg->CR1 &= ~(1<<7);                        //disable send INT
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
        PutStrDirectReg->TDR = str[result];
    }
    PutStrDirectReg->CR1 = CR_Bak;                         //restore send INT
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

    CR_Bak = GetCharDirectReg->CR1;                          //Save INT
    GetCharDirectReg->CR1 &= ~(1<<7);                        //disable send INT
    while(__UART_RxHadChar(GetCharDirectReg) == false) ;
    result = GetCharDirectReg->RDR;
    PutStrDirectReg->CR1 = CR_Bak;                         //restore send INT
    return result;
}

//----初始化内核级IO-----------------------------------------------------------
//功能：初始化内核级输入和输出所需的runtime函数指针。
//参数：无
//返回：无
//-----------------------------------------------------------------------------
void Stdio_KnlInOutInit(u32 para)
{
    if(!strcmp(gc_pCfgStdoutName,"/dev/UART1"))
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
    else if(!strcmp(gc_pCfgStdoutName,"/dev/UART4"))
    {
        PutStrDirectReg = (tagUartReg*)CN_UART4_BASE;
        TxDirectPort = CN_UART4;
    }
    else if(!strcmp(gc_pCfgStdoutName,"/dev/UART5"))
    {
        PutStrDirectReg = (tagUartReg*)CN_UART5_BASE;
        TxDirectPort = CN_UART5;
    }
    else if(!strcmp(gc_pCfgStdoutName,"/dev/UART6"))
    {
        PutStrDirectReg = (tagUartReg*)CN_UART6_BASE;
        TxDirectPort = CN_UART6;
    }
    else if(!strcmp(gc_pCfgStdoutName,"/dev/UART7"))
    {
        PutStrDirectReg = (tagUartReg*)CN_UART7_BASE;
        TxDirectPort = CN_UART7;
    }
    else if(!strcmp(gc_pCfgStdoutName,"/dev/UART8"))
    {
        PutStrDirectReg = (tagUartReg*)CN_UART8_BASE;
        TxDirectPort = CN_UART8;
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

    if(!strcmp(gc_pCfgStdinName,"/dev/UART1"))
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
    else if(!strcmp(gc_pCfgStdinName,"/dev/UART4"))
    {
        GetCharDirectReg = (tagUartReg*)CN_UART4_BASE;
        RxDirectPort = CN_UART4;
    }
    else if(!strcmp(gc_pCfgStdinName,"/dev/UART5"))
    {
        GetCharDirectReg = (tagUartReg*)CN_UART5_BASE;
        RxDirectPort = CN_UART5;
    }
    else if(!strcmp(gc_pCfgStdinName,"/dev/UART6"))
    {
        GetCharDirectReg = (tagUartReg*)CN_UART6_BASE;
        RxDirectPort = CN_UART6;
    }
    else if(!strcmp(gc_pCfgStdinName,"/dev/UART7"))
    {
        GetCharDirectReg = (tagUartReg*)CN_UART7_BASE;
        RxDirectPort = CN_UART7;
    }
    else if(!strcmp(gc_pCfgStdinName,"/dev/UART8"))
    {
        GetCharDirectReg = (tagUartReg*)CN_UART8_BASE;
        RxDirectPort = CN_UART8;
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

