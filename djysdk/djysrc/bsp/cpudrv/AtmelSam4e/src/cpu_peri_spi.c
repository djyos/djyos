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
// 这份授权条款，在使用者符合以下二条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、此三条件表列，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、此三条件表列，以及下述
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
// 文件名     ：cpu_peri_spi.c
// 模块描述: SPI模块底层硬件驱动模块
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 9/12.2014
// =============================================================================

#include "string.h"
#include "cpu_peri.h"
#include "spibus.h"

// =============================================================================
#define CN_SPI_BASE        0x40008000

//定义中断中需使用的静态量结构体
struct SPI_IntParamSet
{
//  struct semaphore_LCB *pDrvPostSemp; //信号量
    u32 SendDataLen;
    u32 RecvDataLen;
    u32 RecvOffset;
//  u8 CurrentCS;
//  u8 block_option;
};
// =============================================================================
//定义SPI控制块和中断静态量
#define CN_SPI_BUF_LEN              128
#define CN_SPI_DMA_BUF_LEN          128
static struct SPI_CB s_SPI_CB;
static u8 s_SPI_Buf[CN_SPI_BUF_LEN];
static u8 s_SPI_DmaSendBuf[CN_SPI_DMA_BUF_LEN];
static u8 s_SPI_DmaRecvBuf[CN_SPI_DMA_BUF_LEN];
struct SPI_IntParamSet IntParamset0;

static const Pin SPI_PINS[] = {
    {PIO_PA12A_MISO,  PIOA, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP},
    {PIO_PA13A_MOSI,  PIOA, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP},
    {PIO_PA14A_SPCK,  PIOA, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP},
    {PIO_PA11A_NPCS0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP},
    {PIO_PB14A_NPCS1, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_PULLUP}
};

//static u8 s_ChipSelect = 0x00;

// =============================================================================
// 功能: 中断使能和失能函数
// 参数: tpSPI,被操作的寄存器组指针
// 返回: 无
// =============================================================================
static void __SPI_IntEnable(volatile tagSpiReg *Reg,u32 IntSrc)
{
    Reg->SPI_IER = IntSrc;
}
static void __SPI_IntDisable(volatile tagSpiReg *Reg,u32 IntSrc)
{
    Reg->SPI_IDR = IntSrc;
}

// =============================================================================
// 功能：SPI时钟配置函数，时钟来源为50M，经SCR和CPSR分频得到时钟，时钟计算公式为：
//       SCK =  PCLK / (CPSDVSR *[SCR+1])
// 参数：spi_dev，设备句柄
//      spiclk，欲配置的时钟速度，单位为Hz
// 返回：true=成功，false=失败
// 说明：此驱动固定SCR = 1;而根据手册，CPSDVSR必须为2-254的偶数，因此，频率范围为
//       12.5M ~ 100kHz
// =============================================================================
static void __SPI_SetClk(volatile tagSpiReg *Reg,u8 cs,u32 Fre)
{
    if(cs > CN_CS_MAX)
        return;

    Reg->SPI_CSR[cs] |= SPI_CSR_SCBR(CN_CFG_MCLK/Fre);
}

// =============================================================================
// 功能：SPI引脚初始化，初始化为外设使用，包括时钟和引脚
// 参数：spi_no,SPI控制器号
// 返回：无
// =============================================================================
static void __SPI_GpioInit(void)
{
//  GPIO_CfgPinFunc(SPI_PINS,PIO_LISTSIZE(SPI_PINS));
    PIO_Configure(SPI_PINS,PIO_LISTSIZE(SPI_PINS));
}

// =============================================================================
// 功能：SPI控制寄存器参数配置，如PHA和CPOL、时钟等，根据各种寄存器而异
// 参数：tpSPI,SPI控制器基址
//       ptr,参数指针
// 返回：无
// =============================================================================
static void __SPI_Config(volatile tagSpiReg *Reg,u8 cs,tagSpiConfig *ptr)
{
    if(cs > CN_CS_MAX)
        return;
    __SPI_SetClk(Reg,cs,ptr->Freq);

    //set the PHA
    if(ptr->Mode & SPI_CPHA)
    {
        Reg->SPI_CSR[cs] |= SPI_CSR_NCPHA;
    }
    else
    {
        Reg->SPI_CSR[cs] &= ~SPI_CSR_NCPHA;
    }
    //set the cpol
    if(ptr->Mode & SPI_CPOL)
    {
        Reg->SPI_CSR[cs] |= SPI_CSR_CPOL;
    }
    else
    {
        Reg->SPI_CSR[cs] &= ~SPI_CSR_CPOL;
    }

    if(ptr->CharLen <= 16 && ptr->CharLen >= 4)
    {
        Reg->SPI_CSR[cs] &= ~SPI_CSR_BITS_Msk;
        Reg->SPI_CSR[cs] |= ptr->CharLen - 8;
    }
}

// =============================================================================
// 功能: SPI默认硬件初始化配置，主要是时钟配置和GPIO写保护引脚配置
// 参数: RegBaseAddr,寄存器基址
// 返回: 无
// =============================================================================
static void __SPI_HardConfig(void)
{
    tagSpiReg *Reg;
    u32 temp,Fre;

    Reg = (tagSpiReg *)CN_SPI_BASE;
    //配置SPI使用GPIO引脚
    __SPI_GpioInit();

    Reg->SPI_CR = SPI_CR_SPIDIS|SPI_CR_SWRST;           //复位控制器
    Reg->SPI_MR = SPI_MR_MSTR|SPI_MR_MODFDIS|SPI_MR_DLYBCS(128);

    //默认使用4M波特率
    Fre = 4*1000*1000;
    for(temp = 0; temp < 4; temp ++)
    {
        Reg->SPI_CSR[temp] = SPI_CSR_CSAAT|SPI_CSR_SCBR(CN_CFG_MCLK/Fre);
    }
    //使用DMA方式
//    Reg->SPI_PTCR = SPI_PTCR_RXTEN | SPI_PTCR_TXTEN;
    Reg->SPI_TCR   = 0;
    Reg->SPI_TNCR  = 0;
    Reg->SPI_RCR   = 0;
    Reg->SPI_RNCR  = 0;

    Reg->SPI_CR = SPI_CR_SPIEN;
}
// =============================================================================
// 功能：SPI接收与发送中断服务函数。该函数实现的功能如下：
//       1.由于收发一体，因此发送空中断中也收到了数据；
//       2.SPI_PortRead()中读不到数据时，说明发送的数据已经完成，可关发送中断
//       3.若param->RecvDataLen大于0，即需要接收数据，则从接收到数据领衔offset字节
//         开始存储数据，即调用SPI_PortRead()
//       4.
// 参数：spi_int_line,中断号，本函数没用到
// 返回：无意义
// =============================================================================
u32 SPI_ISR(ptu32_t IntLine)
{
    struct SPI_CB *pSCB=NULL;
    struct SPI_IntParamSet *param;
    tagSpiReg *Reg;
    u32 spi_sr,spi_imr,temp;

    if(IntLine != CN_INT_LINE_SPI)
        return 0;

    pSCB = &s_SPI_CB;
    Reg = (tagSpiReg *)CN_SPI_BASE;
    param = &IntParamset0;

    spi_sr = Reg->SPI_SR;
    spi_imr = Reg->SPI_IMR;
    //发送中断
    if( (spi_imr & SPI_IMR_TXBUFE) && (spi_sr & SPI_SR_TXBUFE)) //使能了发送中断，且TXRIS
    {
        Reg->SPI_PTCR = SPI_PTCR_TXTDIS|SPI_PTCR_RXTDIS;
        temp = SPI_PortRead(pSCB,s_SPI_DmaSendBuf,CN_SPI_DMA_BUF_LEN);//是否还有数据需要发送

        if(temp >0)
        {
            param->SendDataLen -= CN_SPI_DMA_BUF_LEN;
            param->RecvOffset  -= CN_SPI_DMA_BUF_LEN;

            Reg->SPI_TPR = (u32)s_SPI_DmaSendBuf;
            Reg->SPI_TCR = temp;
            Reg->SPI_RPR = (u32)s_SPI_DmaRecvBuf;
            Reg->SPI_RCR = temp;
            Reg->SPI_PTCR = SPI_PTCR_TXTEN|SPI_PTCR_RXTEN;
        }
        else                                            //表明发送结束
        {
            //关发送中断
            while(!(Reg->SPI_SR & SPI_SR_TXEMPTY));
            while(Reg->SPI_SR & SPI_SR_RDRF)
            {
                temp = Reg->SPI_RDR;
            }
            __SPI_IntDisable(Reg,SPI_IDR_TXBUFE);
            param->RecvOffset -= param->SendDataLen;
            param->SendDataLen = 0;
            //发送完成了，判断是否需要接收
            if(param->RecvDataLen > 0)
            {
                //MASTER模式下，发送和接收同时进行
                memset(s_SPI_DmaSendBuf,0x00,CN_SPI_DMA_BUF_LEN);
                if(param->RecvDataLen > CN_SPI_DMA_BUF_LEN)
                    temp = CN_SPI_DMA_BUF_LEN;
                else
                    temp = param->RecvDataLen;

                Reg->SPI_TPR = (u32)s_SPI_DmaSendBuf;
                Reg->SPI_TCR = temp;
                Reg->SPI_RPR = (u32)s_SPI_DmaRecvBuf;
                Reg->SPI_RCR = temp;

                //同时使能接收和发送DMA
                Reg->SPI_PTCR = SPI_PTCR_TXTEN | SPI_PTCR_RXTEN;
                __SPI_IntEnable(Reg,SPI_IER_RXBUFF);    //使能接收中断
            }
        }
    }
    else if((spi_imr & SPI_IMR_RXBUFF) && (spi_sr & SPI_SR_RXBUFF))    //接收中断
    {
        Reg->SPI_PTCR = SPI_PTCR_TXTDIS|SPI_PTCR_RXTDIS;
        //计算接收了多少数据
        if(param->RecvDataLen > CN_SPI_DMA_BUF_LEN)
            temp = CN_SPI_DMA_BUF_LEN;
        else
            temp = param->RecvDataLen;

        SPI_PortWrite(pSCB,s_SPI_DmaRecvBuf,temp);
        param->RecvDataLen -= temp;

        //判断是否还需接收数据
        if(param->RecvDataLen > 0)
        {
            if(param->RecvDataLen > CN_SPI_DMA_BUF_LEN)
                temp = CN_SPI_DMA_BUF_LEN;
            else
                temp = param->RecvDataLen;
            Reg->SPI_TPR = (u32)s_SPI_DmaSendBuf;
            Reg->SPI_TCR = temp;
            Reg->SPI_RPR = (u32)s_SPI_DmaRecvBuf;
            Reg->SPI_RCR = temp;

            //同时使能接收和发送DMA
            Reg->SPI_PTCR = SPI_PTCR_TXTEN | SPI_PTCR_RXTEN;
        }
        else                    //表明接收已经完成
        {
            param->RecvDataLen = 0;
            __SPI_IntDisable(Reg,SPI_IDR_RXBUFF);
            Reg->SPI_PTCR = SPI_PTCR_TXTDIS | SPI_PTCR_RXTDIS;
        }
    }

    if(param->RecvDataLen + param->SendDataLen == 0)
    {

    }

    return 0;
}

// =============================================================================
// 功能: SPI中断配置函数
// 参数: IntLine,中断线
// 返回: 无
// =============================================================================
static void __SPI_IntConfig(void)
{
    u8 IntLine;

    IntLine = CN_INT_LINE_SPI;
    //中断线的初始化
    Int_Register(IntLine);
    Int_SetClearType(IntLine,CN_INT_CLEAR_AUTO);
    Int_IsrConnect(IntLine,SPI_ISR);
    Int_SettoAsynSignal(IntLine);
    Int_ClearLine(IntLine);
    Int_RestoreAsynLine(IntLine);
}

// =============================================================================
// 功能：SPI片选使能，使片选有效,若控制器有具体的配置片选寄存器，可直接返回
// 参数：Reg，本模块内即SPI寄存器基址
//       cs,片选线
// 返回：无
// 说明：SPI控制器上只引出了一个CS，若需要接多个CS，则必须应用程序自己控制CS
// =============================================================================
static bool_t __SPI_BusCsActive(tagSpiReg *Reg, u8 cs)
{
//  s_ChipSelect = cs;
    Reg->SPI_TDR |= SPI_TDR_PCS(cs);
    Reg->SPI_MR  &= ~SPI_MR_PCS(0x0F);
    Reg->SPI_MR  |= SPI_MR_PCS(cs);
    return true;
}

// =============================================================================
// 功能：SPI片选失能，使片选无效
// 参数：Reg，本模块内即SPI寄存器基址
//       cs,片选线
// 返回：无
// =============================================================================
static bool_t __SPI_BusCsInActive(tagSpiReg *Reg, u8 cs)
{
    Reg->SPI_CR = SPI_CR_LASTXFER | SPI_CR_SPIDIS;
    return false;
}

// =============================================================================
// 功能：SPI总线控制回调函数，被上层调用，目前只实现对SPI时钟频率配置
// 参数：specific_flag,个性标记，本模块内即IIC寄存器基址
//       cmd,命令
//       data1,data2,数据，与具体命令相关
// 返回：无
// =============================================================================
static s32 __SPI_BusCtrl(tagSpiReg *Reg,u32 cmd,ptu32_t data1,ptu32_t data2)
{
    s32 result = 1;
    if((u32)Reg != CN_SPI_BASE)
        return 0;

    switch(cmd)
    {
    case CN_SPI_SET_CLK:
        __SPI_SetClk(Reg,data1,data2);
        break;
    case CN_SPI_CS_CONFIG:
        __SPI_Config(Reg,data1,(tagSpiConfig *)data2);
        break;
    case CN_SPI_SET_AUTO_CS_EN:
        // 硬件上如果有自动产生CS的配置
        break;
    case CN_SPI_SET_AUTO_CS_DIS:
        break;
    default:
        break;
    }
    return result;
}

// =============================================================================
// 功能：读SPI接收寄存器，轮询方式,被__SPI_TxRxPoll调用
// 参数：Reg,寄存器基址
// 返回：读到的数据
// =============================================================================
static u32 __SPI_Read( tagSpiReg * Reg )
{
    while ( (Reg->SPI_SR & SPI_SR_RDRF) == 0 ) ;

    return Reg->SPI_RDR & 0xFFFF ;
}

// =============================================================================
// 功能：写SPI发送寄存器，轮询方式,被__SPI_TxRxPoll调用
// 参数：Reg,寄存器基址，dwNpcs,片选；wData，写数据
// 返回：无
// =============================================================================
static void __SPI_Write( tagSpiReg * Reg, uint32_t dwNpcs, uint16_t wData )
{
    /* Send data */
    while ( (Reg->SPI_SR & SPI_SR_TXEMPTY) == 0 ) ;
    Reg->SPI_TDR = wData | SPI_PCS( dwNpcs ) ;
    while ( (Reg->SPI_SR & SPI_SR_TDRE) == 0 ) ;
}

// =============================================================================
// 功能：轮询方式读写数据
// 参数：Reg,寄存器基址，srcAddr,数据源指针；wrSize，写数据量，destAddr，读到的数据
//       地址指针；rdSize，读数据量，recvoff,读偏移，暂未用
// 返回：true,正确；false,错误
// =============================================================================
static bool_t __SPI_TxRxPoll(tagSpiReg *Reg,u8 *srcAddr,u32 wrSize,
        u8 *destAddr, u32 rdSize,u32 recvoff,u8 cs)
{
    u32 i;
    if(!srcAddr)
        return false;

    Reg->SPI_CR |= SPI_CR_SPIEN;
    for (i=0;i<(wrSize+rdSize);i++)
    {
        if (i<wrSize)
        {
            __SPI_Write(Reg,cs,srcAddr[i]);
            __SPI_Read(Reg);
        }
        else if((i>=wrSize)&&(i<(wrSize+rdSize)))
        {
          __SPI_Write(Reg,cs,0);
          if(destAddr)
            destAddr[i-wrSize] = __SPI_Read(Reg);
        }
    }

    return true;
}

// =============================================================================
// 功能：SPI底层驱动发送接收函数，本函数完成的功能如下：
//       1.保存结构静态变量，用于中断中使用；
//       2.配置相关的寄存器，使其处于发送接收数据状态;
//       3.开启中断，在中断服务函数中完成数据收发。
// 参数：specific_flag,个性标记，本模块内即IIC寄存器基址
//       sendlen,发送数据长度，字节
//       recvlen,发送数据长度，字节
//       recvoff,接收数据依稀，即从接收到的第几个字节开始保存数据，字节
//       block_opt,阻塞选项，true时，为阻塞，非阻塞方式需底层驱动释放信号量和拉高CS
//       spi_semp,底层可能需要释放的信号量，是否释放由block_opt决定
// 返回：true,无错误;false,失败
// =============================================================================
static bool_t __SPI_TransferTxRx(tagSpiReg *Reg,u32 sendlen,u32 recvlen,
                                u32 recvoff)
{
    struct SPI_IntParamSet *Param=NULL;
    u32 temp;

    if((u32)Reg != CN_SPI_BASE)
        return false;

    Param = &IntParamset0;
    Param->SendDataLen = sendlen;
    Param->RecvDataLen = recvlen;
    Param->RecvOffset  = recvoff;

    __SPI_IntDisable(Reg,SPI_IDR_TXBUFE|SPI_IDR_ENDRX);

    Reg->SPI_CR |= SPI_CR_SPIEN;
    //绝大多数的通信，都是发送完成再接收，因此优先发送
    if(sendlen > 0)
    {
        if(sendlen > CN_SPI_DMA_BUF_LEN)
            temp = CN_SPI_DMA_BUF_LEN;
        else
            temp = sendlen;

        SPI_PortRead(&s_SPI_CB,s_SPI_DmaSendBuf,temp);
        Reg->SPI_TPR = (u32)s_SPI_DmaSendBuf;
        Reg->SPI_TCR = temp;
        Reg->SPI_RPR = (u32)s_SPI_DmaRecvBuf;
        Reg->SPI_RCR = temp;
        Reg->SPI_PTCR = SPI_PTCR_TXTEN|SPI_PTCR_RXTEN;
        __SPI_IntEnable(Reg,SPI_IER_TXBUFE);
    }
    else
    {
        if(recvlen > CN_SPI_DMA_BUF_LEN)
            temp = CN_SPI_DMA_BUF_LEN;
        else
            temp = recvlen;
        Reg->SPI_TPR = (u32)s_SPI_DmaSendBuf;
        Reg->SPI_TCR = temp;
        Reg->SPI_RPR = (u32)s_SPI_DmaRecvBuf;
        Reg->SPI_RCR = temp;
        Reg->SPI_PTCR = SPI_PTCR_TXTEN|SPI_PTCR_RXTEN;
        __SPI_IntEnable(Reg,SPI_IER_RXBUFF);
    }

    return true;
}

// =============================================================================
// 功能：SPI底层驱动的初始化，完成整个SPI总线的初始化，其主要工作如下：
//       1.初始化总线控制块SPI_CB，回调函数和缓冲区的初始化赋值；
//       2.默认的硬件初始化，如GPIO或SPI寄存器等；
//       3.中断初始化，完成读写中断配置；
//       4.调用SPIBusAdd或SPIBusAdd_r增加总线结点；
// 参数：无
// 返回：初始化成功返回1，失败返回0
// =============================================================================
bool_t SPI_Init(void)
{
    struct SPI_Param SPI_Config;

    SPI_Config.BusName          = "SPI";
    SPI_Config.SPIBuf           = (u8*)&s_SPI_Buf;
    SPI_Config.SPIBufLen        = CN_SPI_BUF_LEN;
    SPI_Config.SpecificFlag     = CN_SPI_BASE;
    SPI_Config.MultiCSRegFlag   = true;
    SPI_Config.pTransferPoll    = (TransferPoll)__SPI_TxRxPoll;
    SPI_Config.pTransferTxRx    = (TransferFunc)__SPI_TransferTxRx;
    SPI_Config.pCsActive        = (CsActiveFunc)__SPI_BusCsActive;
    SPI_Config.pCsInActive      = (CsInActiveFunc)__SPI_BusCsInActive;
    SPI_Config.pBusCtrl         = (SPIBusCtrlFunc)__SPI_BusCtrl;

    __SPI_HardConfig();
    __SPI_IntConfig();
    PMC_EnablePeripheral(CN_INT_LINE_SPI);

    if(NULL == SPI_BusAdd_s(&SPI_Config,&s_SPI_CB))
        return 0;
    return 1;
}



