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
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：cpu_peri_iic.c
// 模块描述: IIC模块底层硬件驱动，寄存器级别的操作
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 05/12.2014
// =============================================================================
#include "cpu_peri.h"
#include <string.h>
#include "endian.h"
#include "iicbus.h"
#include "xdmac.h"
#include "xdmad.h"
#include "twi_dma.h"

//==============================================================================
#define CN_TWI_REG_BADDR0        0x40018000
#define CN_TWI_REG_BADDR1        0x4001C000
#define CN_TWI_REG_BADDR2         0x40060000
#define tagTwiReg Twihs
// =============================================================================
static tagTwiReg volatile * const tg_TWI_Reg[] = {(tagTwiReg *)CN_TWI_REG_BADDR0,
                                                (tagTwiReg *)CN_TWI_REG_BADDR1,
                                                (tagTwiReg *)CN_TWI_REG_BADDR2};

//中断中使用的数据类型结构体
struct IIC_IntParamSet
{
    struct SemaphoreLCB *pDrvPostSemp;   //信号量
    u32 TransCount;                     //传输数据量计数器
    u32 TransTotalLen;
};

#define TWI_BUF_LEN         128
#define TWI_DMA_BUF_LEN     64

//定义静态变量
static struct IIC_CB sTwi0CB;
static u8 sTwi0Buf[TWI_BUF_LEN];
static u8 sTwi0DmaBuf[TWI_DMA_BUF_LEN];
static struct IIC_IntParamSet IntParamset0;

static struct IIC_CB sTwi1CB;
static u8 sTwi1DmaBuf[TWI_DMA_BUF_LEN];
static u8 sTwi1Buf[TWI_BUF_LEN];
static struct IIC_IntParamSet IntParamset1;

static struct IIC_CB sTwi2CB;
static u8 sTwi2DmaBuf[TWI_DMA_BUF_LEN];
static u8 sTwi2Buf[TWI_BUF_LEN];
static struct IIC_IntParamSet IntParamset2;

u8 *TwiDmaBuf[CN_TWI_NUM] = {sTwi0DmaBuf,sTwi1DmaBuf,sTwi2DmaBuf};
struct IIC_IntParamSet *pIntParamset[CN_TWI_NUM] =
                                {&IntParamset0,&IntParamset1,&IntParamset2};

extern sXdmad dmad;
static TwiDma Twid0;
static TwiChannel Twi0Tx, Twi0Rx;
static TwiDma Twid1;
static TwiChannel Twi1Tx, Twi1Rx;
static TwiDma Twid2;
static TwiChannel Twi2Tx, Twi2Rx;

//静态变量，是否使用DMA方式发送接收，默认为否
static volatile bool_t sTwiDmaUsed[CN_TWI_NUM] = {false,false,false};
static struct IIC_CB *pTwi_CB[CN_TWI_NUM]={&sTwi0CB,&sTwi1CB,&sTwi2CB};
// =============================================================================
// 功能：对相应的I2C外设进行软件复位
// 参数：iic相应外设寄存器的基址
// 返回：无
// =============================================================================
static void __TWI_Reset(volatile tagTwiReg *reg)
{
    reg->TWIHS_CR = TWIHS_CR_SWRST;
}

// =============================================================================
// 功能: 使能iic中断,接收与发送共用一个中断源。
// 参数: reg,被操作的寄存器组指针
//       IntSrc,中断源
// 返回: 无
// =============================================================================
static void __TWI_IntDisable(volatile tagTwiReg *reg,u32 IntSrc)
{
    reg->TWIHS_IDR = IntSrc;
}

// =============================================================================
// 功能: 禁止iic中断
// 参数: reg,被操作的寄存器组指针
//       IntSrc,操作的中断源
// 返回: 无
// =============================================================================
static void __TWI_IntEnable(volatile tagTwiReg *reg,u32 IntSrc)
{
    reg->TWIHS_IER = IntSrc;
}

// =============================================================================
// 功能: 禁止iic中断,接收与发送共用一个中断源。
// 参数: reg,被操作的寄存器组指针
// 返回: 无
// =============================================================================
static void __TWI_GenerateStop(volatile tagTwiReg *reg)
{
    reg->TWIHS_CR = TWIHS_CR_STOP;
}

// =============================================================================
// 功能：IIC时钟配置函数，时钟来源为MCK，计算方法可参考技术手册
// 参数：reg，设备句柄
//      iicclk，欲配置的时钟速度，标准的时钟如50KHz,100KHz,200KHz
// 返回：true=成功，false=失败
// =============================================================================
static void __TWI_SetClk(volatile tagTwiReg *reg,u32 iicclk)
{
    uint32_t dwCkDiv = 0 ;
    uint32_t dwClDiv ;
    uint32_t dwOk = 0 ;

    /* Configure clock */
    while ( !dwOk )
    {
        dwClDiv = ((CN_CFG_MCLK/ (2 * iicclk)) - 4) / (1<<dwCkDiv) ;

        if ( dwClDiv <= 255 )
        {
            dwOk = 1 ;
        }
        else
        {
            dwCkDiv++ ;
        }
    }

    reg->TWIHS_CWGR = 0 ;
    reg->TWIHS_CWGR = (dwCkDiv << 16) | (dwClDiv << 8) | dwClDiv ;
}

// =============================================================================
// 功能：快速配置已知通道的源地址、目标地址和数据长度，其他参数项已经配置好
// 参数：pXdmac，XDMAC的控制器外设指针
//      iChannel，通道号
//      sa,da,len，源地址、目标地址和DAM的字节数
// =============================================================================
static void __TWI_DmaFastConfig(Xdmac *pXdmac,u8 iChannel,u32 sa,u32 da,u32 len)
{
    XDMAC_SetSourceAddr(pXdmac, iChannel, sa);
    XDMAC_SetDestinationAddr(pXdmac, iChannel, da);
    XDMAC_SetMicroblockControl(pXdmac, iChannel, len);
}

// =============================================================================
// 功能：TWI接收中断的DMA服务函数，由DMA中断入口函数调用，主要接收TWI数据，并快速配置DMA,
//      多次使用该DMA通道接收数据量，使用该中断函数多次配置DMA
// 参数：channel，通道号
//       pArg，TwiDma
// 返回：无
// =============================================================================
static void TWI_DmaRxIntIsr(uint32_t channel, TwiDma* pArg)
{
    TwiChannel *pTwiRx;
    static struct IIC_IntParamSet *IntParam;

    static struct IIC_CB *pTwiCB;
    volatile tagTwiReg *Reg;
    uint8_t *ptwi_dma_buf;
    uint32_t num,Port,DmaBufLen;

    for(Port = 0; Port < CN_TWI_NUM; Port++)
    {
        if((u32)(pArg->pTwihsHw) == (u32)tg_TWI_Reg[Port])
        {
            break;
        }
    }
    if(Port == CN_UART_NUM)
        return;

    pTwiRx = pArg->pRxChannel;
    pTwiCB = pTwi_CB[Port];
    Reg = tg_TWI_Reg[Port];
    DmaBufLen = TWI_DMA_BUF_LEN;
    ptwi_dma_buf = TwiDmaBuf[Port];
    IntParam = pIntParamset[Port];

    TWID_SendPause(pArg);

    if(IntParam->TransTotalLen - IntParam->TransCount > DmaBufLen)
    {
        num = DmaBufLen;
        IIC_PortWrite(pTwiCB,ptwi_dma_buf,num);
        IntParam->TransCount += num;
        //计算下次DMA接收的数据大小
        if(IntParam->TransTotalLen - IntParam->TransCount > DmaBufLen)
            num = DmaBufLen;
        else
            num = IntParam->TransTotalLen - IntParam->TransCount;
        //配置下次DMA接收
        if(num > 0)
        {
            __TWI_DmaFastConfig(dmad.pXdmacs,pTwiRx->ChNum,
                    (u32)&Reg->TWIHS_RHR,(u32)ptwi_dma_buf,num);

            TWID_RcvData(pArg);
        }
    }
    else                    //表明接收到了全部的数据
    {
        num = IntParam->TransTotalLen - IntParam->TransCount;
        IIC_PortWrite(pTwiCB,ptwi_dma_buf,num);
        __TWI_GenerateStop(Reg);

        Lock_SempPost(IntParam->pDrvPostSemp);
    }

}

// =============================================================================
// 功能：TWI的DMA发送服务函数，由DMA中断服务函数调用
// 参数：channel，通道号
//       pArg，TwiDma
// 返回：无
// =============================================================================
static void TWI_DmaTxIntIsr(uint32_t channel, TwiDma* pArg)
{
    TwiChannel *pTwiTx;
    static struct IIC_IntParamSet *IntParam;

    static struct IIC_CB *pTwiCB;
    volatile tagTwiReg *Reg;
    uint8_t *ptwi_dma_send_buf;
    uint32_t num,Port,DmaBufLen;

    for(Port = 0; Port < CN_TWI_NUM; Port++)
    {
        if((u32)(pArg->pTwihsHw) == (u32)tg_TWI_Reg[Port])
        {
            break;
        }
    }
    if(Port == CN_UART_NUM)
        return;

    pTwiTx = pArg->pTxChannel;
    pTwiCB = pTwi_CB[Port];
    Reg = tg_TWI_Reg[Port];
    DmaBufLen = TWI_DMA_BUF_LEN;
    ptwi_dma_send_buf = TwiDmaBuf[Port];
    IntParam = pIntParamset[Port];

    TWID_SendPause(pArg);

    if(IntParam->TransTotalLen - IntParam->TransCount > DmaBufLen)
    {
        IntParam->TransCount += DmaBufLen;
        //计算本次需发送多少数据
        if(IntParam->TransTotalLen - IntParam->TransCount > DmaBufLen)
            num = DmaBufLen;
        else
            num = IntParam->TransTotalLen - IntParam->TransCount;
        IIC_PortRead(pTwiCB,ptwi_dma_send_buf,  DmaBufLen);
        //配置DMA发送
        if(num > 0)
        {
            __TWI_DmaFastConfig(dmad.pXdmacs,pTwiTx->ChNum,
                    (u32)ptwi_dma_send_buf,(u32)&Reg->TWIHS_THR,num);

            TWID_SendData(pArg);
        }
    }
    else                    //表明DMA已经传输结束
    {
        IntParam->TransCount = IntParam->TransTotalLen;
        __TWI_GenerateStop(Reg);
        Lock_SempPost(IntParam->pDrvPostSemp);
    }
    return ;
}
// =============================================================================
// 功能：IIC传输的DMA配置函数，配置控制器使用DMA方式
// 参数：
// 返回：
// =============================================================================
static void __TWI_DMA_Config(volatile tagTwiReg* Reg,u32 cmd,u32 Port)
{
    TwiDma *pTwiDma;
    TwiChannel *pTwiRx,*pTwiTx;
    u32 TwiId;

    switch(Port)
    {
    case CN_TWI0:pTwiDma = &Twid0;  pTwiRx = &Twi0Rx;   pTwiTx = &Twi0Tx; TwiId = ID_TWIHS0;break;
    case CN_TWI1:pTwiDma = &Twid1;  pTwiRx = &Twi1Rx;   pTwiTx = &Twi1Tx; TwiId = ID_TWIHS1;break;
    case CN_TWI2:pTwiDma = &Twid2;  pTwiRx = &Twi2Rx;   pTwiTx = &Twi2Tx; TwiId = ID_TWIHS2;break;
    default: return;
    }

    if(cmd == CN_IIC_DMA_USED)              //配置使用DMA方式发送和接收
    {
        sTwiDmaUsed[Port] = true;           //标记使用DMA
        //关闭不使用DMA时的中断
        __TWI_IntDisable(Reg,TWIHS_IDR_RXRDY|TWIHS_IDR_TXRDY);

        memset(pTwiTx, 0, sizeof(TwiChannel));
        memset(pTwiRx, 0, sizeof(TwiChannel));
        pTwiTx->BuffSize = TWI_DMA_BUF_LEN;
        pTwiTx->pBuff = TwiDmaBuf[Port];
        pTwiRx->BuffSize= TWI_DMA_BUF_LEN;
        pTwiRx->pBuff = TwiDmaBuf[Port];
        pTwiTx->sempaphore = 1;
        pTwiRx->sempaphore = 1;
        pTwiTx->pArgument = (void*)pTwiDma;
        pTwiRx->pArgument = (void*)pTwiDma;

        pTwiTx->callback = (TwiCallback)TWI_DmaTxIntIsr;
        pTwiRx->callback = (TwiCallback)TWI_DmaRxIntIsr;

        pTwiDma->pRxChannel = pTwiRx;
        pTwiDma->pTxChannel = pTwiTx;
        pTwiDma->pXdmad = &dmad;

        TWID_Configure(pTwiDma,TwiId);

        TWID_EnableRxChannels(pTwiDma, pTwiRx);
        TWID_EnableTxChannels(pTwiDma, pTwiTx);
    }
    else                                    //配置使用非DMA方式发送和接收
    {
        sTwiDmaUsed[Port] = false;

        TWID_DisableRxChannels(pTwiDma,pTwiRx);
        TWID_DisableTxChannels(pTwiDma,pTwiTx);
    }
}

// =============================================================================
// 功能: IIC默认硬件初始化配置，主要是时钟配置和GPIO写保护引脚配置
// 参数: RegBaseAddr,寄存器基址
//       iicno,IIC控制器号
// 返回: 无
// =============================================================================
static void __TWI_HardConfig(ptu32_t RegBaseAddr,u8 iicno)
{
    volatile tagTwiReg *reg;
    reg = (volatile tagTwiReg *)RegBaseAddr;


    XDMAD_Initialize(&dmad,0);
    reg->TWIHS_CR = TWIHS_CR_SWRST;//reset the twi
    reg->TWIHS_RHR;        //clear the rhr
    //config master mode
    reg->TWIHS_CR = TWIHS_CR_MSDIS;
    reg->TWIHS_CR = TWIHS_CR_SVDIS;

    /* Set master mode */
    reg->TWIHS_CR = TWIHS_CR_MSEN;

    //config the SCK
    reg->TWIHS_CWGR = 0 ;
    reg->TWIHS_CWGR = (1 << 16) | (248 << 8) | 248 ;//100KHz
}

// =============================================================================
// 功能: IIC中断配置函数
// 参数: IntLine,中断线
//       ISR,中断服务函数指针
// 返回: 无
// =============================================================================
static void __TWI_IntConfig(u32 IntLine,u32 (*ISR)(ptu32_t))
{
    //中断线的初始化
    Int_Register(IntLine);
    Int_IsrConnect(IntLine,ISR);
    Int_SettoAsynSignal(IntLine);
    Int_ClearLine(IntLine);     //清掉初始化产生的发送fifo空的中断
    Int_RestoreAsynLine(IntLine);
}

// =============================================================================
// 功能: TWI总线等待函数，用于轮询发送和接收数据等待
// 参数: Reg，TWI基址
//       wrFlag，读写标记
// 返回: true，正常退出;false，超时退出
// =============================================================================
static bool_t __TWI_Wait(volatile tagTwiReg *Reg,u8 Flag)
{
    u32 status;
    bool_t result = false;
    u32 timeout = 200;

    while(timeout != 0)
    {
    	status = Reg->TWIHS_SR;
    	if( (status & Flag) == Flag )
    	{
    		result = true;
    		break;
    	}
    	timeout--;
    	Djy_DelayUs(10);
    }


    return result;
}

// =============================================================================
// 功能：轮询方式写TWI数据
// 参数：Reg,TWI基址
//      DevAddr,设备地址
//      MemAddr,设备内部地址
//      MemAddrLen,内部地址长度，字节数
//      Buf,缓冲区
//      Length,写字节数
// 返回：true,正常完成，false,超时或错误
// =============================================================================
static bool_t __TWI_WritePoll(volatile tagTwiReg *Reg,u8 DevAddr,u32 MemAddr,\
        u8 MemAddrLen,u8* Buf, u32 Length)
{
    u32 i;

    Reg->TWIHS_MMR = 0;
    Reg->TWIHS_MMR = (MemAddrLen<<8)|(DevAddr<<16);
    Reg->TWIHS_IADR = 0;
    Reg->TWIHS_IADR = TWIHS_IADR_IADR(MemAddr);
    Reg->TWIHS_CR = TWIHS_CR_START;

    Reg->TWIHS_THR = Buf[0];
    for(i = 1; i < Length; i++)
    {
        //wait for
         __TWI_Wait(Reg,TWIHS_SR_TXRDY);
        Reg->TWIHS_THR = Buf[i];
    }

    __TWI_GenerateStop(Reg);

    __TWI_Wait(Reg,TWIHS_SR_TXCOMP);
    return true;
}

// =============================================================================
// 功能：轮询方式读TWI数据
// 参数：Reg,TWI基址
//      DevAddr,设备地址
//      MemAddr,设备内部地址
//      MemAddrLen,内部地址长度，字节数
//      Buf,缓冲区
//      Length,写字节数
// 返回：true,正常完成，false,超时或错误
// =============================================================================
static bool_t __TWI_ReadPoll(volatile tagTwiReg *Reg,u8 DevAddr,u32 MemAddr,\
        u8 MenAddrLen,u8* Buf, u32 Length)
{
    u32 i;
    Reg->TWIHS_RHR;

    //TWIHS_MMR_IADRSZ_2_BYTE; len ; device addr
    Reg->TWIHS_MMR = 0;
    Reg->TWIHS_MMR = (MenAddrLen<<8)|(TWIHS_MMR_MREAD)|(DevAddr<<16);
    Reg->TWIHS_IADR = 0;
    Reg->TWIHS_IADR = TWIHS_IADR_IADR(MemAddr);

    if(Length == 1)
    	Reg->TWIHS_CR = TWIHS_CR_START | TWIHS_CR_STOP;
    else
    	Reg->TWIHS_CR = TWIHS_CR_START;

    for(i = 0; i < Length; i ++)
    {
    	if(i == Length - 1)
    	{
    		__TWI_GenerateStop(Reg);
    	}
        __TWI_Wait(Reg,TWIHS_SR_RXRDY);
        Buf[i] = Reg->TWIHS_RHR;
    }

    __TWI_Wait(Reg,TWIHS_SR_TXCOMP);
    return true;
}

// =============================================================================
// 功能：轮询方式读写IIC设备
// 参数：SpecificFlag,寄存器基址
//       DevAddr,设备地址
//       MemAddr,设备内部地址
//       MemAddrLen,设备内部地址长度
//       Buf,存储数据
//       Length,数据长度，字节
//       WrRdFlag,读写标记，为0时写，1时为读
// 返回：len,读取成功;-1,读取失败
// =============================================================================
static bool_t __TWI_WriteReadPoll(ptu32_t  SpecificFlag,u8 DevAddr,u32 MemAddr,\
                                u8 MemAddrLen,u8* Buf, u32 Length,u8 WrRdFlag)
{
    bool_t result;
    if(WrRdFlag == CN_IIC_WRITE_FLAG)//轮询写
    {
        result =  __TWI_WritePoll((volatile tagTwiReg *)SpecificFlag,DevAddr,
                                MemAddr,MemAddrLen,Buf,Length);
    }
    else                            //轮询读
    {
        result = __TWI_ReadPoll((volatile tagTwiReg *)SpecificFlag,DevAddr,
                                MemAddr,MemAddrLen,Buf,Length);
    }

    return result;
}

// =============================================================================
// 功能: 启动写时序，启动写时序的过程为：器件地址（写）、存储地址（写），当存储地址完
//       成时，需打开中断，重新配置寄存器为接收模式，之后将会发生发送中断，在中断服务
//       函数中，每次发送一定的字节数，直到len数据量，post信号量iic_bus_semp，并产生
//       停止时序
// 参数: specific_flag,个性标记，本模块内即IIC寄存器基址
//      dev_addr,器件地址的前7比特，最后的三个比特位已经更新，该函数内部需将该地址左
//               移一位，并修改最后的读写比特位，最后一比特写0，表示写，最后一比特写
//               1,表示读;
//      mem_addr,存储器内部地址,该函数需发送低maddr_len字节到总线
//      maddr_len,存储器内部地址的长度，字节单位；
//      length,发送的数据总量，接收数据的倒数第一字节，即count-1，停止产生ACK信号，
//          当接收的字节数为count时，产生停止时序，并释放信号量iic_semp;
//      iic_semp,发送完成时驱动需释放的信号量。发送程序读IIC_PortRead时，若读不到数
//          则需释放该信号量，产生结束时序
// 返回: TRUE，启动读时序成功，FALSE失败
// =============================================================================
static bool_t __TWI_GenerateWriteStart(volatile tagTwiReg *Reg,
                                       u8 dev_addr,
                                       u32 mem_addr,
                                       u8 maddr_len,
                                       u32 length,
                                       struct SemaphoreLCB *iic_semp)
{
    TwiDma *pTwiDma;
    TwiChannel *pTxCh;
    struct IIC_CB *IIC;
    struct IIC_IntParamSet *IntParam;
    u8 ch,*pDmaBuf,Port,DmaBufLen;

    //通过specific_flag区分是哪条总线
    switch((u32)Reg)
    {
    case CN_TWI_REG_BADDR0:
        Port = CN_TWI0; pTwiDma = &Twid0;   pTxCh = &Twi0Tx;    break;
    case CN_TWI_REG_BADDR1:
        Port = CN_TWI1; pTwiDma = &Twid1;   pTxCh = &Twi1Tx;    break;
    case CN_TWI_REG_BADDR2:
        Port = CN_TWI2; pTwiDma = &Twid2;   pTxCh = &Twi2Tx;    break;
    default: return false;
    }

    IIC         = pTwi_CB[Port];
    IntParam    = pIntParamset[Port];
    pDmaBuf     = TwiDmaBuf[Port];
    DmaBufLen   = TWI_DMA_BUF_LEN;

    IntParam->TransTotalLen = length;
    IntParam->TransCount = 0;
    IntParam->pDrvPostSemp = iic_semp;

    Reg->TWIHS_MMR = 0;
    Reg->TWIHS_MMR = (maddr_len<<8)|(dev_addr<<16);
    Reg->TWIHS_IADR = 0;
    Reg->TWIHS_IADR = TWIHS_IADR_IADR(mem_addr);
    Reg->TWIHS_CR = TWIHS_CR_START;

    if(sTwiDmaUsed[Port] == false)      //使用非DMA方式 发送
    {
        //发送一个字节的数据
        while(!(Reg->TWIHS_SR & TWIHS_SR_TXRDY));
        IIC_PortRead(IIC,&ch,1);

        Reg->TWIHS_THR = ch;
        IntParam->TransCount++;
        __TWI_IntEnable(Reg,TWIHS_IER_TXRDY);
    }
    else                                    //配置DMA发送时的参数
    {
        while(!(Reg->TWIHS_SR & TWIHS_SR_TXRDY));
        ch = IIC_PortRead(IIC,pDmaBuf,DmaBufLen);
        if(ch)
        {
            __TWI_DmaFastConfig(dmad.pXdmacs,pTxCh->ChNum,
                    (u32)pDmaBuf,(u32)&Reg->TWIHS_THR,ch);

            TWID_SendData(pTwiDma);
        }
    }

    return true;
}

// =============================================================================
// 功能: 启动读时序，启动读时序的过程为：器件地址（写）、存储地址（写）、器件地址（读）
//       当器件地址（读）完成时，需打开中断，重新配置寄存器为接收模式，之后将会发生
//       接收数据中断，在中断中将接收到的数据调用IIC_PortWrite写入缓冲，接收到len字
//       节数的数据后，释放信号量iic_semp
// 参数: specific_flag,个性标记，本模块内即IIC寄存器基址
//      dev_addr,器件地址的前7比特，最后的三个比特位已经更新，该函数内部需将该地址左
//               移一位，并修改最后的读写比特位，最后一比特写0，表示写，最后一比特写
//               1,表示读;
//      mem_addr,存储器内部地址,该函数需发送低maddr_len字节到总线
//      maddr_len,存储器内部地址的长度，字节单位；
//      len,接收的数据总量，接收数据的倒数第一字节，即count-1，停止产生ACK信号，当接
//          收的字节数为count时，产生停止时序，并释放信号量iic_semp;
//      iic_semp,读完成时，驱动需释放的信号量（缓冲区信号量）
// 返回: TRUE，启动读时序成功，FALSE失败
// =============================================================================
static bool_t __TWI_GenerateReadStart( volatile tagTwiReg *Reg,
                                       u8 dev_addr,
                                       u32 mem_addr,
                                       u8 maddr_len,
                                       u32 length,
                                       struct SemaphoreLCB *iic_semp)
{
    TwiDma *pTwiDma;
    TwiChannel *pRxCh;
    struct IIC_IntParamSet *IntParam;
    u8 DmaRcvLen,*pDmaBuf,Port,DmaBufLen;

    //通过specific_flag区分是哪条总线
    switch((u32)Reg)
    {
    case CN_TWI_REG_BADDR0:
        Port = CN_TWI0; pTwiDma = &Twid0;   pRxCh = &Twi0Rx;    break;
    case CN_TWI_REG_BADDR1:
        Port = CN_TWI1; pTwiDma = &Twid1;   pRxCh = &Twi1Rx;    break;
    case CN_TWI_REG_BADDR2:
        Port = CN_TWI2; pTwiDma = &Twid2;   pRxCh = &Twi2Rx;    break;
    default: return false;
    }

    IntParam    = pIntParamset[Port];
    pDmaBuf     = TwiDmaBuf[Port];
    DmaBufLen   = TWI_DMA_BUF_LEN;

    IntParam->TransTotalLen = length;
    IntParam->TransCount = 0;
    IntParam->pDrvPostSemp = iic_semp;

    Reg->TWIHS_RHR;

    //TWIHS_MMR_IADRSZ_2_BYTE; len ; device addr
    Reg->TWIHS_MMR = 0;
    Reg->TWIHS_MMR = (maddr_len<<8)|(TWIHS_MMR_MREAD)|(dev_addr<<16);
    Reg->TWIHS_IADR = 0;
    Reg->TWIHS_IADR = TWIHS_IADR_IADR(mem_addr);
    if(length == 1)
    	Reg->TWIHS_CR = TWIHS_CR_START | TWIHS_CR_STOP;
    else
    	Reg->TWIHS_CR = TWIHS_CR_START;

    if(sTwiDmaUsed[Port] == false)
    {
        __TWI_IntEnable(Reg,TWIHS_IER_RXRDY|TWIHS_IER_TXRDY);
    }
    else
    {
        DmaRcvLen = length>DmaBufLen ? DmaBufLen:length;
        if(DmaRcvLen)
        {
            __TWI_DmaFastConfig(dmad.pXdmacs,pRxCh->ChNum,
                    (u32)&Reg->TWIHS_RHR,(u32)pDmaBuf,DmaRcvLen);

            TWID_RcvData(pTwiDma);
        }
    }

    return true;
}

// =============================================================================
// 功能: 结束本次读写回调函数，区分读写的不同停止时序，当属于发送时，则直接停止时序，
//      若为读，则先停止回复ACK，再停止
// 参数: specific_flag,个性标记，本模块内即IIC寄存器基址
// 返回: 无
// =============================================================================
static void __TWI_GenerateEnd(volatile tagTwiReg *Reg)
{
    if(((u32)Reg != CN_TWI_REG_BADDR0)
            && (u32)Reg == CN_TWI_REG_BADDR1
            && (u32)Reg == CN_TWI_REG_BADDR2)
        return;

    __TWI_IntDisable(Reg, TWIHS_IDR_TXRDY|TWIHS_IDR_RXRDY|
            TWIHS_IDR_TXCOMP);
    Djy_EventDelay(100);
    __TWI_GenerateStop(Reg);

    __TWI_Reset(Reg);
}

// =============================================================================
// 功能：IIC总线控制回调函数，被上层调用，目前只实现对IIC时钟频率配置
// 参数：specific_flag,个性标记，本模块内即IIC寄存器基址
//       cmd,命令
//       data1,data2,数据，与具体命令相关
// 返回：无
// =============================================================================
static s32 __TWI_BusCtrl(volatile tagTwiReg *Reg,u32 cmd,u32 data1,u32 data2)
{
    u8 port;
    if(((u32)Reg != CN_TWI_REG_BADDR0) &&
            (u32)Reg != CN_TWI_REG_BADDR1)
        return 0;
    if((u32)Reg == CN_TWI_REG_BADDR0)
        port = CN_TWI0;
    else if((u32)Reg == CN_TWI_REG_BADDR1)
        port = CN_TWI1;
    else if((u32)Reg == CN_TWI_REG_BADDR2)
        port = CN_TWI2;
    else
        return 0;
    switch(cmd)
    {
    case CN_IIC_SET_CLK:
        __TWI_SetClk(Reg,data1);
        break;
    case CN_IIC_DMA_USED:
    case CN_IIC_DMA_UNUSED:
        __TWI_DMA_Config(Reg,cmd,port);
        break;
    default:
        break;
    }
    return 1;
}

// =============================================================================
// 功能：IIC接收与发送中断服务函数。该函数实现的功能如下：
//       1.每发送与接收一个或若干字节发生一次中断；
//       2.若有多个中断使用同一个中断号，则需根据具体情况区分使用的是哪个中断；
//       3.清中断标志，并判断ACK信号，每读写字节，计数器都需相应修改；
//       4.接收达到倒数第一个字节时，需配置不发送ACK信号；
//       5.接收或发送完成时，需post信号量IntParam->pDrvPostSemp；
//       6.接收或发送完成时，需产生停止时序。
// 参数：i2c_int_line,中断号，本函数没用到
// 返回：无意义
// =============================================================================
static u32 __TWI_ISR(ufast_t IntLine)
{
    static struct IIC_CB *ICB;
    static struct IIC_IntParamSet *IntParam;
    volatile tagTwiReg *reg;

    u8 ch,Port;
    u32 twi_sr,twi_imr;

    if(IntLine == CN_INT_LINE_TWIHS0)
        Port = CN_TWI0;
    else if(IntLine == CN_INT_LINE_TWIHS1)
        Port = CN_TWI1;
    else if(IntLine == CN_INT_LINE_TWIHS2)
        Port = CN_TWI2;
    else
        return 0;

    ICB = pTwi_CB[Port];
    reg = tg_TWI_Reg[Port];
    IntParam = pIntParamset[Port];

    twi_sr = reg->TWIHS_SR;
    twi_imr = reg->TWIHS_IMR;

    if(sTwiDmaUsed[Port] == false)      //使用非DMA发送和接收
    {
        if((twi_sr & TWIHS_SR_RXRDY) && (twi_imr & TWIHS_IMR_RXRDY))//接收中断
        {
            ch = reg->TWIHS_RHR;
            if(IIC_PortWrite(ICB,&ch,1))
                IntParam->TransCount ++;
            if(IntParam->TransCount == IntParam->TransTotalLen - 1)
            {
                __TWI_GenerateStop(reg);   //倒数第一个时写stop
            }
            else if(IntParam->TransCount == IntParam->TransTotalLen)
            {
                __TWI_GenerateStop(reg);
                __TWI_IntDisable(reg,TWIHS_IDR_RXRDY);
                __TWI_IntEnable(reg,TWIHS_IER_TXCOMP);
            }
        }
        else if((twi_sr & TWIHS_SR_TXRDY) && (twi_imr & TWIHS_IMR_TXRDY))//发送
        {
            if(IIC_PortRead(ICB,&ch,1))
            {
                reg->TWIHS_THR = ch;
                IntParam->TransCount ++;
            }
            else if(IntParam->TransCount == IntParam->TransTotalLen)
            {
                __TWI_IntDisable(reg,TWIHS_IDR_TXRDY);
                __TWI_IntEnable(reg,TWIHS_IER_TXCOMP);
                __TWI_GenerateStop(reg);
            }
        }
        else if((twi_sr & TWIHS_SR_TXCOMP) && (twi_imr & TWIHS_IMR_TXCOMP))
        {
            __TWI_IntDisable(reg,TWIHS_IDR_TXCOMP);
            Lock_SempPost(IntParam->pDrvPostSemp);
        }
    }
    return 0;
}

// =============================================================================
// 功能：IIC底层驱动的初始化，完成整个IIC总线的初始化，其主要工作如下：
//       1.初始化总线控制块IIC_CB，回调函数和缓冲区的初始化赋值；
//       2.默认的硬件初始化，如GPIO或IIC寄存器等；
//       3.中断初始化，完成读写中断配置；
//       4.调用IICBusAdd或IICBusAdd_r增加总线结点；
// 参数：para,无具体意义
// 返回：无
// =============================================================================
bool_t ModuleInstall_TWI(u8 port)
{
    struct IIC_Param Twi_Config;
    u32 IntLine,TwiId;

    switch(port)
    {
    case CN_TWI0:
        Twi_Config.BusName         = "TWI0";
        Twi_Config.IICBuf          = (u8*)&sTwi0Buf;
        IntLine = CN_INT_LINE_TWIHS0;
        TwiId   = ID_TWIHS0;
        break;
    case CN_TWI1:
        Twi_Config.BusName         = "TWI1";
        Twi_Config.IICBuf          = (u8*)&sTwi1Buf;
        IntLine = CN_INT_LINE_TWIHS1;
        TwiId   = ID_TWIHS1;
        break;
    case CN_TWI2:
        Twi_Config.BusName         = "TWI2";
        Twi_Config.IICBuf          = (u8*)&sTwi2Buf;
        IntLine = CN_INT_LINE_TWIHS2;
        TwiId   = ID_TWIHS2;
        break;
    default: return false;
    }

    Twi_Config.IICBufLen           = TWI_BUF_LEN;
    Twi_Config.SpecificFlag        = (u32)tg_TWI_Reg[port];
    Twi_Config.pWriteReadPoll      = (WriteReadPoll)__TWI_WriteReadPoll;
    Twi_Config.pGenerateWriteStart = (WriteStartFunc)__TWI_GenerateWriteStart;
    Twi_Config.pGenerateReadStart  = (ReadStartFunc)__TWI_GenerateReadStart;
    Twi_Config.pGenerateEnd        = (GenerateEndFunc)__TWI_GenerateEnd;
    Twi_Config.pBusCtrl            = (IICBusCtrlFunc)__TWI_BusCtrl;

    __TWI_HardConfig((u32)tg_TWI_Reg[port],port);
    __TWI_IntConfig(IntLine,__TWI_ISR);
    PMC_EnablePeripheral(TwiId);

    if(NULL == IIC_BusAdd_s(pTwi_CB[port],&Twi_Config))
        return 0;
    return 1;
}

