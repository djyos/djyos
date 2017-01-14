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
// 模块描述: UART模块底层硬件驱动，寄存器级别的操作
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 05/15.2014
// =============================================================================

#include "cpu_peri.h"
#include "stdio.h"
#include "stdlib.h"
#include <driver/include/uart.h>
#include "string.h"
#include "uart_dma.h"
#include "uart_dma.h"
#include "xdmad.h"
#include "xdmac.h"

extern const char *gc_pCfgStdinName;    //标准输入设备
extern const char *gc_pCfgStdoutName;   //标准输出设备
extern const char *gc_pCfgStderrName;   //标准错误输出设备

extern void Board_UartHalfDuplexSend(u8 SerialNo);
extern void Board_UartHalfDuplexRecv(u8 SerialNo);
// =============================================================================
//base address
#define CN_UART0_BASE   (0x400E0800)
#define CN_UART1_BASE   (0x400E0A00)
#define CN_UART2_BASE   (0x400E1A00)
#define CN_UART3_BASE   (0x400E1C00)
#define CN_UART4_BASE   (0x400E1E00)

#define CN_USART0_BASE  (0x40024000)
#define CN_USART1_BASE  (0x40028000)
#define CN_USART2_BASE  (0x4002C000)

static u32 TxByteTime;                    //正常发送一个字节所需要的时间
static tagUartReg *PutStrDirectReg;     //用于printk发送的串口寄存器
static tagUartReg *GetCharDirectReg;     //用于直接接收的串口寄存器
static u32 TxDirectPort;                  //用于printk发送的串口号
static u32 RxDirectPort;                  //用于直接接收的串口号
//==============================================================================
static tagUartReg volatile * const tg_UART_Reg[] = {(tagUartReg *)CN_UART0_BASE,
                                                    (tagUartReg *)CN_UART1_BASE,
                                                    (tagUartReg *)CN_UART2_BASE,
                                                    (tagUartReg *)CN_UART3_BASE,
                                                    (tagUartReg *)CN_UART4_BASE,
                                                    (tagUartReg *)CN_USART0_BASE,
                                                    (tagUartReg *)CN_USART1_BASE,
                                                    (tagUartReg *)CN_USART2_BASE};

#define UART_SendBufLen  256
#define UART_RecvBufLen  256
#define UART_DMA_BUF_LEN  64

static bool_t s_UART_DmaUsed[CN_UART_NUM]={0,0,0,0,0,0,0,0};

//串口使用DMA时候，使用LLI0和LLI1,由于UART没有超时中断，因此UART 的LLI0 长度定义为1;
//串口UASART有超时中断因此，因此将LLI0的长度适当拉长
//在配置LLI的长度时，使用32位数，高16位存储LLI1的长度，低16位存储LLI0的长度，如下所示：
#define UART_LLI_LEN     (1 + ((UART_DMA_BUF_LEN-1) << 16) )
#define USART_LLI_LEN    ((UART_DMA_BUF_LEN - 16 ) + (16 << 16))
const u32 s_UART_DmaRcvLen[CN_UART_NUM] ={UART_LLI_LEN,UART_LLI_LEN,UART_LLI_LEN
        ,UART_LLI_LEN,UART_LLI_LEN,USART_LLI_LEN,USART_LLI_LEN,USART_LLI_LEN};

u32 s_UART_TimeOut[] = {115200,115200,115200};// USART超时中断时间配置

extern sXdmad dmad;
static UartDma Uartd0;
static UartChannel Uart0Tx, Uart0Rx;
static UartDma Uartd1;
static UartChannel Uart1Tx, Uart1Rx;
static UartDma Uartd2;
static UartChannel Uart2Tx, Uart2Rx;
static UartDma Uartd3;
static UartChannel Uart3Tx, Uart3Rx;
static UartDma Uartd4;
static UartChannel Uart4Tx, Uart4Rx;
static UartDma Usartd0;
static UartChannel Usart0Tx, Usart0Rx;
static UartDma Usartd1;
static UartChannel Usart1Tx, Usart1Rx;
static UartDma Usartd2;
static UartChannel Usart2Tx, Usart2Rx;

ptu32_t UART_ISR(ptu32_t IntLine);

static struct UartCB *pUartCB[CN_UART_NUM];

//用于标识串口是否初始化标记，第0位表示UART0，第一位表UART1....
//依此类推，1表示初始化，0表示未初始化
static u8 sUartInited = 0;

//----使能接收中断-------------------------------------------------------------
//功能: 使能uart的接收中断,DMA中断使用的是endrx
//参数: reg,被操作的寄存器组指针
//返回: 无
//-----------------------------------------------------------------------------
static void __UART_RecvIntEnable(tagUartReg volatile *reg,u8 enable)
{
    if(enable)
        reg->UART_IER = (1<<0);//rxrdy int
    else
        reg->UART_IDR = (1<<0);//rxrdy int
}

//----使能发送中断-------------------------------------------------------------
//功能: 使能uart的发送中断,DMA中断使用的是endtx 中断
//参数: reg,被操作的寄存器组指针
//返回: 无
//-----------------------------------------------------------------------------
static void __UART_SendIntEnable(tagUartReg volatile *reg,u8 enable)
{
    if(enable)
        reg->UART_IER = (1<<9);//tx int
    else
        reg->UART_IDR = (1<<9);//tx int
}

//----使能串口发送功能------------------------------------------------------------
//功能: 使能串口控制器的发送数据功能
//参数: reg,被操作的寄存器组指针
//      enable,1，使能，0，失能
//返回: 无
//-----------------------------------------------------------------------------
static void __UART_TxEnable(tagUartReg volatile *reg,u8 enable)
{
    if(enable)
    {
        reg->UART_CR = (1<<6);//TXEN
    }
    else
    {
        reg->UART_CR = (1<<7);
    }
}

//----使能串口接收功能------------------------------------------------------------
//功能: 使能串口控制器的接收数据功能
//参数: reg,被操作的寄存器组指针
//      enable,1，使能，0，失能
//返回: 无
//-----------------------------------------------------------------------------
static void __UART_RxEnable(tagUartReg volatile *reg,u8 enable)
{
    if(enable)
    {
        reg->UART_CR = (1<<4);//RXEN
    }
    else
    {
        reg->UART_CR = (1<<5);
    }
}
//----检查串口发送寄存器是否空-------------------------------------------------
//功能: 检查发送寄存器的状态，如果空就返回true，若为空，则移位寄存器和thr都为空
//参数：无。
//返回: true = 缓冲区空，false = 非空
//-----------------------------------------------------------------------------
static bool_t __UART_TxTranEmpty(volatile tagUartReg *reg)
{
    return (0 != (reg->UART_SR & (1<<9)));//TXR EMPTY
}

// =============================================================================
// 功能: 检查接收寄存器的状态，有数据就返回true，否则返回false
// 参数：reg,被操作的寄存器组指针
// 返回: true = 有数据，false = 无数据
// =============================================================================
static bool_t __UART_RxHadChar(tagUartReg volatile *reg)
{
    return reg->UART_SR & (0x01<<0);//RXRDY
}

//----设置baud-----------------------------------------------------------------
//功能: 设置串口baud,串口属于外设，时钟源为MCK，即150M未分频的情况
//参数: reg,被操作的寄存器组指针
//返回: 无
//-----------------------------------------------------------------------------
static void __UART_SetBaud(tagUartReg volatile *reg,u32 baud)
{
	if(baud != 0)
	{
		reg->UART_BRGR = ((CN_CFG_MCLK/2)/baud)/16;
		if(tg_UART_Reg[TxDirectPort] == reg)
		{
			TxByteTime = 11000000/baud;     //1个字节传输时间，按10bit，+10%计算
		}
	}
}

//----设置COM-----------------------------------------------------------------
//功能: 设置串口相关的参数，包括波特率、数据位、停止位、奇校验位
//参数: reg,被操作的寄存器组指针
//返回: 无
//注意：ATMEL M7的UART不能修改数据位、停止位，但是在寄存器上面这些位预留，配置对UART无效
//     ，只对USART有效
//-----------------------------------------------------------------------------
static void __UART_SetCom(tagUartReg volatile *reg,struct COMParam *Com)
{
    u32 mr;
    __UART_TxEnable(reg,0);
    __UART_RxEnable(reg,0);

    __UART_SetBaud(reg,Com->BaudRate);

    mr = (reg->UART_MR) & ( ~(7 << 9));
    //奇偶校验
    switch(Com->Parity)
    {
    case CN_UART_PARITY_NONE:
        mr |=  (4 << 9);
        break;
    case CN_UART_PARITY_ODD:
        mr |= (1 << 9);
        break;
    case CN_UART_PARITY_EVEN:
        mr |= (0 << 9);
        break;
    case CN_UART_PARITY_MARK:
        mr |= (3 << 9);
        break;
    case CN_UART_PARITY_SPACE:
        mr |= (2 << 9);
        break;
    default:
        break;
    }
    reg->UART_MR = mr;

    mr = (reg->UART_MR) & ( ~(3 << 6));
    //UART不能修改databits
    switch(Com->DataBits)
    {
    case CN_UART_DATABITS_5:
        mr |= (0 << 6);
        break;
    case CN_UART_DATABITS_6:
        mr |= (1 << 6);
        break;
    case CN_UART_DATABITS_7:
        mr |= (2 << 6);
        break;
    case CN_UART_DATABITS_8:
        mr |= (3 << 6);
        break;
    default:
        break;
    }
    reg->UART_MR = mr;


    mr = (reg->UART_MR) & ( ~(3 << 12));
    //UART不能修改停止位
    switch(Com->StopBits)
    {
    case CN_UART_STOPBITS_1:
        mr |= (0 << 12);
        break;
    case CN_UART_STOPBITS_1_5:
        mr |= (1 << 12);
        break;
    case CN_UART_STOPBITS_2:
        mr |= (2 << 12);
        break;
    default:
        break;
    }
    reg->UART_MR = mr;

    __UART_TxEnable(reg,1);
    __UART_RxEnable(reg,1);
}

//----初始化uart对应的中断-----------------------------------------------------
//功能: 初始化uart对应的中断线，并初始化中断入口函数
//参数：SerialNo,串口号
//返回: 无
//-----------------------------------------------------------------------------
static void __UART_IntInit(u32 SerialNo)
{
    uint32_t int_line;
    if(SerialNo == CN_UART0)
        int_line = CN_INT_LINE_UART0;
    else if(SerialNo == CN_UART1)
        int_line = CN_INT_LINE_UART1;
    else if(SerialNo == CN_UART2)
        int_line = CN_INT_LINE_UART2;
    else if(SerialNo == CN_UART3)
        int_line = CN_INT_LINE_UART3;
    else if(SerialNo == CN_UART4)
        int_line = CN_INT_LINE_UART4;
    else if(SerialNo == CN_USART0)
        int_line = CN_INT_LINE_USART0;
    else if(SerialNo == CN_USART1)
        int_line = CN_INT_LINE_USART1;
    else
        int_line = CN_INT_LINE_USART2;

    Int_Register(int_line);
    Int_SetClearType(int_line,CN_INT_CLEAR_AUTO);
    Int_IsrConnect(int_line,UART_ISR);
    Int_SettoAsynSignal(int_line);
    Int_ClearLine(int_line);
    Int_RestoreAsynLine(int_line);
}

//----初始化uart时钟使能-----------------------------------------------------
//功能: 使能相应uart外设的时钟
//参数：SerialNo,串口号
//返回: 无
//-----------------------------------------------------------------------------
static void __UART_PmcInit(u32 SerialNo)
{
    if(SerialNo == CN_UART0)
        PMC_EnablePeripheral(ID_UART0);
    else if(SerialNo == CN_UART1)
        PMC_EnablePeripheral(ID_UART1);
    else if(SerialNo == CN_UART2)
        PMC_EnablePeripheral(ID_UART2);
    else if(SerialNo == CN_UART3)
        PMC_EnablePeripheral(ID_UART3);
    else if(SerialNo == CN_UART4)
        PMC_EnablePeripheral(ID_UART4);
    else if(SerialNo == CN_USART0)
        PMC_EnablePeripheral(ID_USART0);
    else if(SerialNo == CN_USART1)
        PMC_EnablePeripheral(ID_USART1);
    else
        PMC_EnablePeripheral(ID_USART2);
}

// =============================================================================
// 功能: 快速配置DMA串口收发，因为前面已经将其他的控制器部分已经配置好，此次，只需要将地址
//       和需要发送的数据重新配置一下便可
// 参数：pXdmac,DMA寄存器指针
//      iChannel,通道号
//      sa,da,len,源地址，目的地址，数据字节数
// 返回: 无
// =============================================================================
static void __UART_DmaTxConfig(Xdmac *pXdmac,u8 iChannel,u32 sa,u32 da,u32 len)
{
    XDMAC_SetSourceAddr(pXdmac, iChannel, sa);
    XDMAC_SetDestinationAddr(pXdmac, iChannel, da);
    XDMAC_SetMicroblockControl(pXdmac, iChannel, len);
}

// =============================================================================
// 功能：配置DMA 的LLI，串口使用LLI0和LLI1两个LIST
// 参数：pUartRx，配置的接收通道
//      sa，数据源地址，即对应UART或者USART的 RHR寄存器
//      da,目标地址，即DMA的搬运的缓冲区
//      len,32位的数据 ，高16位代表LLI1的字节数，低16位表示LLI0的字节长度
// 返回：无
// =============================================================================
static void __UART_DmaRxLLIConfig(UartChannel *pUartRx,u32 sa,u32 da,u32 len)
{
    u8 i,LliNo= 2,xdmaCndc,xdmaIntEn;
    u16 BuffSize[2];
    u8 *pbuf;

    pbuf = (uint8_t *)da;
    BuffSize[0] = (u16)len;
    BuffSize[1] = (u16)(len >> 16);
    for (i = 0; i < LliNo; i++)
    {
         pUartRx->pLLIview[i].mbr_ubc = XDMA_UBC_NVIEW_NDV1
                                        | XDMA_UBC_NSEN_UNCHANGED
                                        | XDMA_UBC_NDEN_UPDATED
                                        | ((i== LliNo- 1)?
                                        ( (pUartRx->dmaRingBuffer)?
                                                XDMA_UBC_NDE_FETCH_EN : 0):
                                                XDMA_UBC_NDE_FETCH_EN)
                                        | BuffSize[i];
        pUartRx->pLLIview[i].mbr_sa = (uint32_t)sa;
        pUartRx->pLLIview[i].mbr_da = (uint32_t)pbuf;
        pUartRx->pLLIview[i].mbr_nda = (i == ( LliNo - 1))?
                ( (pUartRx->dmaRingBuffer)? (uint32_t)pUartRx->pLLIview : 0 ):
                (uint32_t)&pUartRx->pLLIview[ i + 1 ];

        pbuf += BuffSize[i];
    }

    __DMB();
    xdmaCndc = XDMAC_CNDC_NDVIEW_NDV1 |
               XDMAC_CNDC_NDE_DSCR_FETCH_EN |
               XDMAC_CNDC_NDSUP_SRC_PARAMS_UPDATED|
               XDMAC_CNDC_NDDUP_DST_PARAMS_UPDATED ;
    xdmaIntEn = XDMAC_CIE_BIE;//XDMAC_CIE_LIE
    XDMAC_SetDescriptorAddr(dmad.pXdmacs, pUartRx->ChNum, (u32)(pUartRx->pLLIview), 0);
    XDMAC_SetDescriptorControl(dmad.pXdmacs, pUartRx->ChNum, xdmaCndc);
    XDMAC_DisableChannelIt (dmad.pXdmacs, pUartRx->ChNum, 0xFF);
    XDMAC_EnableChannelIt (dmad.pXdmacs,pUartRx->ChNum,xdmaIntEn  );
}

//读取DMA计数器的值，该计数器是一个递减计数器
static u32 __UART_DmaGetDataLen(Xdmac *pXdmac,u8 iChannel)
{
    return XDMAC_GetMicroBlock(pXdmac,iChannel);
}

// =============================================================================
// 功能：串口DMA接收中断回调函数，初始化时注册，被DMA中断函数回调，进入该函数后，先暂停该
//      通道的DMA，然后将DMA的数据读出来，写入到环形缓冲区，最后再判断是否需启动DMA
// 参数：channel,通道号
//      pArg，串口DMA结构指针
// 返回：无
// =============================================================================
void UART_DmaRxIntIsr(uint32_t channel, UartDma* pArg)
{
    UartChannel *pUartRx;

    static struct UartCB *UCB;
    uint8_t *puart_dma_recv_buf;
    uint32_t num,Port;
    uint32_t len,UartDmaRcvLen;
    tagUartReg* Reg;

    for(Port = 0; Port < CN_UART_NUM; Port++)
    {
        if((u32)(pArg->pUartHw) == (u32)tg_UART_Reg[Port])
        {
            break;
        }
    }
    if(Port == CN_UART_NUM)
        return;

    if( !(sUartInited & (0x01 << Port)) )
    	return ;

    pUartRx = pArg->pRxChannel;
    UCB = pUartCB[Port];
    Reg = tg_UART_Reg[Port];
    UartDmaRcvLen = s_UART_DmaRcvLen[Port];
    puart_dma_recv_buf = pUartRx->pBuff;//UartDmaRecvBuf[Port];

    XDMAC_SoftwareFlushReq(dmad.pXdmacs,pUartRx->ChNum);
    num = __UART_DmaGetDataLen(dmad.pXdmacs,pUartRx->ChNum);

    len = UART_DMA_BUF_LEN - num;		//计数器是递减

    UARTD_RcvPause(pArg);

    UART_PortWrite(UCB,puart_dma_recv_buf,len,0);

    //重新配置一下DMA
//    XDMAC_SetDescriptorAddr(dmad.pXdmacs, pUartRx->ChNum, (u32)(pUartRx->pLLIview), 0);
    __UART_DmaRxLLIConfig(pUartRx,(u32)&Reg->UART_RHR,
            (u32)(pUartRx->pBuff),UartDmaRcvLen);

    UARTD_RcvData(pArg);

    return ;
}

// =============================================================================
// 功能：串口DMA发送中断回调函数，初始化时注册，被DMA中断函数回调，进入该函数后，先暂停该
//      通道的DMA，然后将DMA的数据读出来，写入到环形缓冲区，最后再判断是否需启动DMA
// 参数：channel,通道号
//      pArg，串口DMA结构指针
// 返回：无
// =============================================================================
void UART_DmaTxIntIsr(uint32_t channel, UartDma* pArg)
{
    UartChannel *pUartTx;

    static struct UartCB *UCB;
    volatile tagUartReg *Reg;
    uint8_t *puart_dma_send_buf;
    uint32_t num,Port;

    for(Port = 0; Port < CN_UART_NUM; Port++)
    {
        if((u32)(pArg->pUartHw) == (u32)tg_UART_Reg[Port])
        {
            break;
        }
    }
    if(Port == CN_UART_NUM)
        return;

    if( !(sUartInited & (0x01 << Port)) )
    	return ;

    pUartTx = pArg->pTxChannel;
    UCB = pUartCB[Port];
    Reg = tg_UART_Reg[Port];
    puart_dma_send_buf = pUartTx->pBuff;//UartDmaRecvBuf[Port];

    UARTD_SendPause(pArg);

    num = UART_PortRead(UCB,puart_dma_send_buf, UART_DMA_BUF_LEN,0);
    if(num > 0)
    {
        __UART_DmaTxConfig(dmad.pXdmacs,pUartTx->ChNum,
                (u32)puart_dma_send_buf,(u32)&Reg->UART_THR,num);

        UARTD_SendData(pArg);
    }
    return ;
}

// =============================================================================
// 功能: 配置uart为DMA方式或配置为非DMA方式时，配置各寄存器，
// 参数：reg,串口寄存器指针
//       cmd,命令号
//       Port,端口号，即串口号
// 返回: 无
// =============================================================================
static void __UART_DMA_Config(tagUartReg* Reg,u32 cmd,u32 Port)
{
    UartDma *pUartDma;
    UartChannel *pUartRx,*pUartTx;
    u32 UartId,UartDmaRcvLen;
    u8 *pUartDmaRxBuf,*pUartDmaTxBuf;

    switch(Port)
    {
    case CN_UART0: pUartDma = &Uartd0; pUartRx = &Uart0Rx; pUartTx = &Uart0Tx; UartId = ID_UART0;break;
    case CN_UART1: pUartDma = &Uartd1; pUartRx = &Uart1Rx; pUartTx = &Uart1Tx; UartId = ID_UART1;break;
    case CN_UART2: pUartDma = &Uartd2; pUartRx = &Uart2Rx; pUartTx = &Uart2Tx; UartId = ID_UART2;break;
    case CN_UART3: pUartDma = &Uartd3; pUartRx = &Uart3Rx; pUartTx = &Uart3Tx; UartId = ID_UART3;break;
    case CN_UART4: pUartDma = &Uartd4; pUartRx = &Uart4Rx; pUartTx = &Uart4Tx; UartId = ID_UART4;break;
    case CN_USART0: pUartDma = &Usartd0; pUartRx = &Usart0Rx; pUartTx = &Usart0Tx; UartId = ID_USART0;break;
    case CN_USART1: pUartDma = &Usartd1; pUartRx = &Usart1Rx; pUartTx = &Usart1Tx; UartId = ID_USART1;break;
    case CN_USART2: pUartDma = &Usartd2; pUartRx = &Usart2Rx; pUartTx = &Usart2Tx; UartId = ID_USART2;break;
    default: return;
    }

    //将串口相关的寄存器reset并除能接收和发送
    Reg->UART_CR = ((1<<2)|(1<<3)|(1<<5)|
                (1<<7)|(1<<8));         //接收发送都除能了

    if(cmd == CN_UART_DMA_UNUSED)//不使用DMA
    {
        s_UART_DmaUsed[Port] = false;

         //关闭非dma时使用的中断，开始dma使用的uart接收中断
         //发送中断只在启动发送时开启
        __UART_RecvIntEnable(Reg,1);
        //关闭总线空闲中断，只有USART有效
        Reg->US_RTOR = 0;
        Reg->UART_IDR = (1<<8);//关闭timeout中断

        if( (pUartRx->pBuff) && (pUartTx->pBuff) )
        {
        	free(pUartRx->pBuff);
        	free(pUartTx->pBuff);
        	pUartRx->pBuff = NULL;
        	pUartTx->pBuff = NULL;

            UARTD_DisableRxChannels(pUartDma, pUartRx);
            UARTD_DisableTxChannels(pUartDma, pUartTx);
        }
    }
    else    //使用DMA
    {
        s_UART_DmaUsed[Port] = true;

        //关闭非dma时使用的中断，开始dma使用的uart接收中断
        __UART_RecvIntEnable(Reg, 0);
        __UART_SendIntEnable(Reg, 0);

        //开启receiver time-out中断，只有USART有该功能
        if(Port >= CN_USART0)
        {
            Reg->US_RTOR = s_UART_TimeOut[Port-CN_USART0];// 1*115200,1S
            Reg->UART_CR = (1<<11);//使能setto
            Reg->UART_IER = (1<<8);
        }

        //cache 清cache需保证32字节对齐，因此的地址需32字节对齐
        pUartDmaRxBuf = (u8*)M_Malloc(UART_DMA_BUF_LEN*2 + 32,0);
        pUartDmaRxBuf = (u8*)((u32)pUartDmaRxBuf - ((u32)pUartDmaRxBuf % 32));
        pUartDmaTxBuf = pUartDmaRxBuf + UART_DMA_BUF_LEN;
        if(pUartDmaRxBuf == NULL)
            return ;

        memset(pUartTx, 0, sizeof(UartChannel));
        memset(pUartRx, 0, sizeof(UartChannel));
        pUartTx->BuffSize = UART_DMA_BUF_LEN;
        pUartTx->pBuff = pUartDmaTxBuf;
        pUartRx->BuffSize= UART_DMA_BUF_LEN/2;
        pUartRx->pBuff = pUartDmaRxBuf;
        pUartTx->sempaphore = 1;
        pUartRx->sempaphore = 1;
        pUartTx->pArgument = (void*)pUartDma;
        pUartRx->pArgument = (void*)pUartDma;

        pUartTx->callback = (UartdCallback)UART_DmaTxIntIsr;
        pUartRx->callback = (UartdCallback)UART_DmaRxIntIsr;

        //config the rx LLI
        pUartRx->dmaProgrammingMode = XDMAD_LLI;
        pUartRx->dmaRingBuffer = 1;
        pUartRx->dmaBlockSize = 2;
        UartDmaRcvLen = s_UART_DmaRcvLen[Port];

        pUartDma->pRxChannel = pUartRx;
        pUartDma->pTxChannel = pUartTx;
        pUartDma->pXdmad = &dmad;

        UARTD_Configure(pUartDma,UartId);

        UARTD_EnableRxChannels(pUartDma, pUartRx);
        UARTD_EnableTxChannels(pUartDma, pUartTx);

        UARTD_RcvPause(pUartDma);

        __UART_DmaRxLLIConfig(pUartRx,(u32)&Reg->UART_RHR,
                (u32)(pUartRx->pBuff),UartDmaRcvLen);

        UARTD_RcvData(pUartDma);
    }

    __UART_TxEnable(Reg,1);
    __UART_RxEnable(Reg,1);
}

// =============================================================================
// 功能: 设置硬件参数，包括波特率、停止位、校验位、数据位，默认情况下:
//      波特率:115200  ； 停止位:1 ; 校验:偶校验 ; 数据位:8bit
// 参数: reg,被操作的寄存器组指针
// 返回: 无
// =============================================================================
static void __UART_ResetReg(tagUartReg *reg)
{
	reg->UART_CR = (1<<2) | (1<<3) | (1<<5) | (1<<7) | (1<<8);
}
static void __UART_HardInit(u8 SerialNo)
{
    tagUartReg *reg;
    if(SerialNo > CN_UART_MAX)
        return;

    XDMAD_Initialize(&dmad,0);				//复位DMA，如果之前配置了，必须复位
    __UART_PmcInit(SerialNo);

    reg = (tagUartReg *)tg_UART_Reg[SerialNo];
    __UART_ResetReg(reg);
    __UART_SetBaud(reg,115200);            //波特率设置
    reg->UART_MR = 0x000;                  //偶校验，正常模式
    reg->UART_CR = ((1<<2)|(1<<3)|(1<<5)|
                    (1<<7)|(1<<8));         //接收发送都除能了

    reg->UART_MR = (3<<6)|(4<<9);//无奇偶校验，时钟源为MCK,正常模式,停止位1，8比特
    reg->UART_CR = ((1<<4)|(1<<6));// tx rx enable
    __UART_RecvIntEnable(reg,1);
}

// =============================================================================
// 功能: 串口设备的控制函数,与具体的硬件寄存器设置相关
// 参数: Reg,UART的寄存器基址.
//       cmd,操作类型
//       data,含义依cmd而定
// 返回: 无意义.
// =============================================================================
ptu32_t __UART_Ctrl(tagUartReg *Reg,u32 cmd, u32 data1,u32 data2)
{
    u8 Port;
    u32 timeout = 10000;
    switch((u32)Reg)
    {
    case CN_UART0_BASE: Port = CN_UART0;break;
    case CN_UART1_BASE: Port = CN_UART1;break;
    case CN_UART2_BASE: Port = CN_UART2;break;
    case CN_UART3_BASE: Port = CN_UART3;break;
    case CN_UART4_BASE: Port = CN_UART4;break;
    case CN_USART0_BASE: Port = CN_USART0;break;
    case CN_USART1_BASE: Port = CN_USART1;break;
    case CN_USART2_BASE: Port = CN_USART2;break;
    default:return 0;
    }

    if( !(sUartInited & (0x01 << Port)) )
    	return 0;

    switch(cmd)
    {
        case CN_UART_START:
            __UART_RxEnable(Reg,1);
            __UART_TxEnable(Reg,1);
            break;
        case CN_UART_STOP:
            __UART_RxEnable(Reg,0);
            __UART_TxEnable(Reg,0);
            break;
        case CN_UART_SET_BAUD:  //设置Baud
            __UART_SetBaud(Reg,data1);
            if(Port >= CN_USART0)
            {
                s_UART_TimeOut[Port-CN_USART0] = data1;//timeout = 500ms
            }
            break;
        case CN_UART_COM_SET:
            __UART_SetCom(Reg,(struct COMParam *)data1);
            break;
        case CN_UART_HALF_DUPLEX_SEND:
            Board_UartHalfDuplexSend(Port);
            break;
        case CN_UART_HALF_DUPLEX_RECV:
            while((false == __UART_TxTranEmpty(Reg))&& (timeout > 10))
            {
                timeout -=10;
                Djy_DelayUs(10);
            }
            Board_UartHalfDuplexRecv(Port);
            break;
        case CN_UART_DMA_USED:
        case CN_UART_DMA_UNUSED:
            __UART_DMA_Config(Reg,cmd,Port);
            break;
        default: break;
    }
    return 0;
}

// =============================================================================
// 功能: 启动串口发送，直接发送FIFO大小的数据，并产生发送空中断，在中断中将缓冲区中的
//      数据完成所有数据的发送，相当于启动了一次发送功能；分为DMA方式和非DMA方式
// 参数: Reg,被操作的串口寄存器基址
//       timeout,超时时间，微秒
// 返回: 发送的字节数
// =============================================================================
u32 __UART_SendStart(tagUartReg *Reg,u32 timeout)
{
    u8 Port,num;
    struct UartCB *UCB;
    UartDma *pUartd;
    u8 *UartDmaSendBuf;

    switch((u32)Reg)
    {
    //UART0和UART1的FIFO深度为8，而其他的为1
    case CN_UART0_BASE:UCB = pUartCB[CN_UART0];Port = CN_UART0; pUartd = &Uartd0;break;
    case CN_UART1_BASE:UCB = pUartCB[CN_UART1];Port = CN_UART1;pUartd = &Uartd1;break;
    case CN_UART2_BASE:UCB = pUartCB[CN_UART2];Port = CN_UART2;pUartd = &Uartd2;break;
    case CN_UART3_BASE:UCB = pUartCB[CN_UART3];Port = CN_UART3;pUartd = &Uartd3;break;
    case CN_UART4_BASE:UCB = pUartCB[CN_UART4];Port = CN_UART4;pUartd = &Uartd4;break;
    case CN_USART0_BASE:UCB = pUartCB[CN_USART0];Port = CN_USART0;pUartd = &Usartd0;break;
    case CN_USART1_BASE:UCB = pUartCB[CN_USART1];Port = CN_USART1;pUartd = &Usartd1;break;
    case CN_USART2_BASE:UCB = pUartCB[CN_USART2];Port = CN_USART2;pUartd = &Usartd2;break;
    default:return 0;
    }

    if( !(sUartInited & (0x01 << Port)) )
    	return 0;

    if(s_UART_DmaUsed[Port])
    {
        if(pUartd->pTxChannel->sempaphore==1)
        {
        	UartDmaSendBuf = pUartd->pTxChannel->pBuff;
            UARTD_SendPause(pUartd);
            num = UART_PortRead(UCB,UartDmaSendBuf,UART_DMA_BUF_LEN,0);
            if(num > 0)
            {
                __UART_DmaTxConfig(dmad.pXdmacs,pUartd->pTxChannel->ChNum,
                		(u32)UartDmaSendBuf,(u32)&Reg->UART_THR,num);

                UARTD_SendData(pUartd);
            }
        }
    }
    else
    {
        __UART_SendIntEnable(Reg,1);
    }
    return 1;
}

// =============================================================================
// 功能: 使用中断方式效率能够大大提升软件执行效率；包括接收、发送和异常等，接收到数据
//       后，调用通用接口模块，写入软件缓冲区；发送数据时，调用通用接口模块，从软件缓
//       冲区读出数据，写入硬件发送寄存器；此外，根据芯片，可通过DMA方式发送
// 参数: 中断号.
// 返回: 0.
// =============================================================================
uint32_t UART_ISR(ptu32_t IntLine)
{
    static struct UartCB *UCB;
    volatile tagUartReg *Reg;
    uint8_t ch,Port;
    uint32_t IIR=0,num,timeout = 1000;
    UartDma *pUartDma = NULL;

    switch(IntLine)
    {
        case CN_INT_LINE_UART0:         Port = CN_UART0;            break;
        case CN_INT_LINE_UART1:         Port = CN_UART1;            break;
        case CN_INT_LINE_UART2:         Port = CN_UART2;            break;
        case CN_INT_LINE_UART3:         Port = CN_UART3;            break;
        case CN_INT_LINE_UART4:         Port = CN_UART4;            break;
        case CN_INT_LINE_USART0:        Port = CN_USART0;
                                        pUartDma = &Usartd0;        break;
        case CN_INT_LINE_USART1:        Port = CN_USART1;
                                        pUartDma = &Usartd1;        break;
        case CN_INT_LINE_USART2:        Port = CN_USART2;
                                        pUartDma = &Usartd2;        break;
        default:    return 0;
    }

    if( !(sUartInited & (0x01 << Port)) )
    	return 0;

    Reg = tg_UART_Reg[Port];
    UCB = pUartCB[Port];
    IIR = Reg->UART_SR;

    if((IIR & (1<<0)) && (Reg->UART_IMR &(1<<0)))//rxrdy int
    {
        ch = Reg->UART_RHR;
        num = UART_PortWrite(UCB,&ch,1,0);
        if(num != 1)
        {
            UART_ErrHandle(UCB,CN_UART_BUF_OVER_ERR);
        }
    }
    //tx empty int
    if((IIR &(1<<9)) && (Reg->UART_IMR &(1<<9)))
    {
        num = UART_PortRead(UCB,&ch,1,0);
        while((!__UART_TxTranEmpty(Reg)) && (timeout-- > 0));
        if(num != 0)
            Reg->UART_THR = ch;
        else
        {
            __UART_SendIntEnable(Reg,0);
        }
    }


    if( (IIR & (1<<8) ) && (Reg->UART_IMR &(1<<8)) )//timeout int
    {
        if( (Port >= CN_USART0) && (s_UART_DmaUsed[Port] == true) )
        {
            Reg->UART_CR = (1<<11);//STTTO,启动USART的超时功能
            UART_DmaRxIntIsr(0,pUartDma);
        }
    }

    if(IIR &(0xE0)) //其他中断
    {
        UART_ErrHandle(UCB,CN_UART_HARD_COMM_ERR);
    }

    return 0;
}

// =============================================================================
// 功能：初始化串口模块，使用DJYOS的串口标准模型，包括硬件、中断和串口设备加载
// 参数：串口初始化模块的参数为串口号，如UART0
// 返回：1=成功，0=失败
// =============================================================================
ptu32_t ModuleInstall_UART(ptu32_t SerialNo)
{
    struct UartParam UART_Param;

    switch(SerialNo)
    {
    case CN_UART0:        UART_Param.Name = "UART0";        break;
    case CN_UART1:        UART_Param.Name = "UART1";        break;
    case CN_UART2:        UART_Param.Name = "UART2";        break;
    case CN_UART3:        UART_Param.Name = "UART3";        break;
    case CN_UART4:        UART_Param.Name = "UART4";        break;
    case CN_USART0:       UART_Param.Name = "USART0";        break;
    case CN_USART1:       UART_Param.Name = "USART1";        break;
    case CN_USART2:       UART_Param.Name = "USART2";        break;
    default:        return 0;
    }
    UART_Param.TxRingBufLen = UART_SendBufLen;
    UART_Param.RxRingBufLen = UART_RecvBufLen;
    UART_Param.UartPortTag  = (ptu32_t)tg_UART_Reg[SerialNo];
    UART_Param.Baud         = 115200;
    UART_Param.StartSend    = (UartStartSend)__UART_SendStart;
    UART_Param.UartCtrl     = (UartControl)__UART_Ctrl;
    //硬件初始化
    __UART_HardInit(SerialNo);
    __UART_IntInit(SerialNo);

    sUartInited |= (0x01 << SerialNo);
    pUartCB[SerialNo] = UART_InstallPort(&UART_Param);
    if( pUartCB[SerialNo] == NULL)
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

    CR_Bak = PutStrDirectReg->UART_IER;                          //Save INT
    __UART_SendIntEnable(PutStrDirectReg,0);                    //disable send INT
    for(result=0; result < len+1; result ++)
    {
        // 超时或者发送缓冲为空时退出
        while((false == __UART_TxTranEmpty(PutStrDirectReg))&& (timeout > 10))
        {
            timeout -=10;
            Djy_DelayUs(10);
        }
        if( (timeout <= 10) || (result == len) )
            break;
        PutStrDirectReg->UART_THR = str[result];
    }

    PutStrDirectReg->UART_IER = CR_Bak;                         //restore send INT
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

    CR_Bak = GetCharDirectReg->UART_IER;                          //Save INT
    __UART_SendIntEnable(PutStrDirectReg,0);                      //disable send INT
    while(__UART_RxHadChar(GetCharDirectReg) == false) ;
    result = GetCharDirectReg->UART_RHR;
    PutStrDirectReg->UART_IER = CR_Bak;                         //restore send INT
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
    else if(!strcmp(gc_pCfgStdoutName,"/dev/UART4"))
    {
        PutStrDirectReg = (tagUartReg*)CN_UART4_BASE;
        TxDirectPort = CN_UART4;
    }
    else if(!strcmp(gc_pCfgStdoutName,"/dev/USART0"))
    {
        PutStrDirectReg = (tagUartReg*)CN_USART0_BASE;
        TxDirectPort = CN_USART0;
    }
    else if(!strcmp(gc_pCfgStdoutName,"/dev/USART1"))
    {
        PutStrDirectReg = (tagUartReg*)CN_USART1_BASE;
        TxDirectPort = CN_USART1;
    }
    else if(!strcmp(gc_pCfgStdoutName,"/dev/USART2"))
    {
        PutStrDirectReg = (tagUartReg*)CN_USART2_BASE;
        TxDirectPort = CN_USART2;
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
    else if(!strcmp(gc_pCfgStdinName,"/dev/UART4"))
    {
        GetCharDirectReg = (tagUartReg*)CN_UART4_BASE;
        RxDirectPort = CN_UART4;
    }
    else if(!strcmp(gc_pCfgStdinName,"/dev/USART0"))
    {
        GetCharDirectReg = (tagUartReg*)CN_USART0_BASE;
        RxDirectPort = CN_USART0;
    }
    else if(!strcmp(gc_pCfgStdinName,"/dev/USART1"))
    {
        GetCharDirectReg = (tagUartReg*)CN_USART1_BASE;
        RxDirectPort = CN_USART1;
    }
    else if(!strcmp(gc_pCfgStdinName,"/dev/USART2"))
    {
        GetCharDirectReg = (tagUartReg*)CN_USART2_BASE;
        RxDirectPort = CN_USART2;
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

