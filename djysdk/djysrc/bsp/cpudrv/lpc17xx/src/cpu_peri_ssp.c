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
// 文件名     ：cpu_peri_ssp.c
// 模块描述: SPI模块底层硬件驱动模块,由于LPC17xx有两个SSP和一个SPI，虽然SSP功能上兼
//          容SPI,但是寄存器不兼容，因此，此模块根据板件使用SSP
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 27/08.2014
// =============================================================================

#include "djyos.h"
#include "cpu_peri.h"
#include "spibus.h"

// =============================================================================
#define CN_SSP0_BASE        LPC_SSP0_BASE
#define CN_SSP1_BASE        LPC_SSP1_BASE
#define tagSpiReg           LPC_SSP_TypeDef
#define CN_FIFO_LEN         8

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
#define CN_SSP0_BUF_LEN      32
struct SPI_IntParamSet IntParamset0;

#define CN_SSP1_BUF_LEN      32
struct SPI_IntParamSet IntParamset1;

static struct SPI_CB *sp_SSP_CB[CN_SSP_NUM];
// =============================================================================
// 功能: 中断使能和失能函数
// 参数: tpSPI,被操作的寄存器组指针
// 返回: 无
// =============================================================================
static void __SSP_TxIntEnable(volatile tagSpiReg *Reg)
{
    Reg->IMSC |= (1<<3);
}
static void __SSP_TxIntDisable(volatile tagSpiReg *Reg)
{
    Reg->IMSC &= ~(1<<3);
}
static void __SSP_RxIntEnable(volatile tagSpiReg *Reg)
{
    Reg->IMSC |= (1<<1)|(1<<2);
}
static void __SSP_RxIntDisable(volatile tagSpiReg *Reg)
{
    Reg->IMSC &= ~((1<<1) | (1<<2));
}
// =============================================================================
// 功能：SPI时钟配置函数，时钟来源为25M，经SCR和CPSR分频得到时钟，时钟计算公式为：
//       SCK =  PCLK / (CPSDVSR *[SCR+1])
// 参数：spi_dev，设备句柄
//      spiclk，欲配置的时钟速度，单位为Hz
// 返回：true=成功，false=失败
// 说明：此驱动固定SCR = 1;而根据手册，CPSDVSR必须为2-254的偶数，因此，频率范围为
//       12.5M ~ 100kHz
// =============================================================================
static void __SSP_SetClk(volatile tagSpiReg *tpSPI,u32 spisck)
{
    u32 temp;
    if(tpSPI == NULL)
        return;
    temp = 25 * 1000000 / spisck;
    temp = temp - (temp%2);
    if(temp > 254)  temp = 254;
    if(temp < 2)    temp = 2;

    tpSPI->CPSR = temp;
}

// =============================================================================
// 功能：SPI引脚初始化，初始化为外设使用，包括时钟和引脚
// 参数：spi_no,SPI控制器号
// 返回：无
// =============================================================================
static void __SSP_GpioInit(u32 spi_no)
{
    if(spi_no == CN_SSP0)
    {
        LPC_SC->PCONP |= (1<<21);

        // MISO,MOSI,SCK,SSEL配置
        LPC_PINCON->PINSEL0  |= (2 << 30);  //sck0
        LPC_SC->PCLKSEL1 &= ~(3<<10);
        LPC_SC->PCLKSEL1 |= (00<<10);   //cclk/2 = 25M
        LPC_PINCON->PINSEL1  |= (2 << 0) | (2 << 2) | (2 << 4);
        LPC_PINCON->PINMODE1 &= ~0x000000FF;
    }
    else
    {
        // Enable clock for SSP1, clock = CCLK / 2
        LPC_SC->PCONP    |= (1<<10);
        LPC_SC->PCLKSEL0 &= ~(3<<20);
        LPC_SC->PCLKSEL0 |= (00<<20);   //cclk/4 = 25M
        /* Connect MOSI, MISO, and SCK to SSP peripheral*/
        LPC_PINCON->PINSEL0  |= (2 << 18) | (2 << 16) | (2 << 14);
        LPC_PINCON->PINMODE0 &= ~0x000FF000;
    }
}

// =============================================================================
// 功能：SPI控制寄存器参数配置，如PHA和CPOL、时钟等，根据各种寄存器而异
// 参数：tpSPI,SPI控制器基址
//       ptr,参数指针
// 返回：无
// =============================================================================
static void __SSP_Config(volatile tagSpiReg *Reg,tagSpiConfig *ptr)
{
    __SSP_SetClk(Reg,ptr->Freq);

    //set the PHA
    if(ptr->Mode & SPI_CPHA)
    {
        Reg->CR0 |= 1<<7;
    }
    else
    {
        Reg->CR0 &= ~(1<<7);
    }
    //set the cpol
    if(ptr->Mode & SPI_CPOL)
    {
        Reg->CR0 |= 1<<6;
    }
    else
    {
        Reg->CR0 &= ~(1<<6);
    }

    if(ptr->CharLen <= 16 && ptr->CharLen >= 4)
    {
        Reg->CR0 &= ~(0xF);
        Reg->CR0 = ptr->CharLen - 1;
    }
}

// =============================================================================
// 功能: SPI默认硬件初始化配置，主要是时钟配置和GPIO写保护引脚配置
// 参数: RegBaseAddr,寄存器基址
// 返回: 无
// =============================================================================
static void __SSP_HardDefaultSet(u8 port)
{
    tagSpiReg * Reg;
    if(port == CN_SSP0)
        Reg = (tagSpiReg *)CN_SSP0_BASE;
    else if(port == CN_SSP1)
        Reg = (tagSpiReg *)CN_SSP1_BASE;
    else
        return;

    //配置SPI使用GPIO引脚
    __SSP_GpioInit(port);

    Reg->CR0        = 0x00C7;       //SCR = 0;CPOL=1, CPHA=1,8bits
    Reg->CPSR       = 0x02;         //CLK = 50M/(0x02 * (1+SCR)) = 25M
    Reg->CR1        = 0x02;         //ENABLE SSP
}

// =============================================================================
// 功能：SPI片选使能，使片选有效
// 参数：Reg，本模块内即SPI寄存器基址
//       cs,片选线
// 返回：无
// 说明：SSP控制器上只引出了一个CS，若需要接多个CS，则必须应用程序自己控制CS
// =============================================================================
static bool_t __SSP_BusCsActive(tagSpiReg *Reg, u8 cs)
{
    bool_t result = true;
    if((u32)Reg == CN_SSP0_BASE)
        LPC_GPIO0->FIOCLR |= (0x1<<16);
    else if((u32)Reg == CN_SSP1_BASE)
        LPC_GPIO0->FIOCLR |= (0x1<<6);
    else
        result = false;
    return result;
}

// =============================================================================
// 功能：SPI片选失能，使片选无效
// 参数：Reg，本模块内即SPI寄存器基址
//       cs,片选线
// 返回：无
// =============================================================================
static bool_t __SSP_BusCsInActive(tagSpiReg *Reg, u8 cs)
{
    bool_t result = true;
    if((u32)Reg == CN_SSP0_BASE)
        LPC_GPIO0->FIOSET |= (0x1<<16);
    else if((u32)Reg == CN_SSP1_BASE)
        LPC_GPIO0->FIOSET |= (0x1<<6);
    else
        result = false;
    return result;
}

// =============================================================================
// 功能：SPI总线控制回调函数，被上层调用，目前只实现对SPI时钟频率配置
// 参数：Reg,个性标记，本模块内即IIC寄存器基址
//       cmd,命令
//       data1,data2,数据，与具体命令相关
// 返回：无
// =============================================================================
static s32 __SSP_BusCtrl(tagSpiReg *Reg,u32 cmd,ptu32_t data1,ptu32_t data2)
{
    s32 result = 1;
    if(Reg == NULL)
        return 0;

    switch(cmd)
    {
    case CN_SPI_SET_CLK:
        __SSP_SetClk(Reg,data1);
        break;
    case CN_SPI_CS_CONFIG:
        __SSP_Config(Reg,(tagSpiConfig *)data1);
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
// 功能：轮询方式SSP读写数据，接收偏移此处作了简化处理，直接认为与sendlen相同，即发送
//       完后再接收数据
// 参数：Reg,个性标记，本模块内即IIC寄存器基址
//       srcaddr,发送数据源
//       sendlen,发送数据长度，字节单位
//       destaddr,接收数据缓冲区
//       recvlen,接收数据长度，字节单位
//       recvoff,接收依稀字节，很多时候为0
//       Byte,发送的字节
// 返回：true,成功;false,失败
// =============================================================================
static bool_t __SSP_TxRxPoll(tagSpiReg *Reg,u8* srcaddr,u32 sendlen,
                            u8* destaddr,u32 recvlen,u32 recvoff,u8 cs)
{
    u32 i,j,len;
    u8 ch =0xaa;
    __SSP_TxIntDisable(Reg);
    __SSP_RxIntDisable(Reg);

    while(Reg->SR & (1<<2))     //SSP_SR_RNE
    {
        i = Reg->DR;            //读空FIFO
    }

    //此处简化处理，发送后再接收数据
    for(i = 0; i < sendlen/CN_FIFO_LEN; i++)            //发送数据
    {
        if(i == sendlen/CN_FIFO_LEN - 1)
        {
            len = sendlen%CN_FIFO_LEN;
        }
        else
        {
            len = CN_FIFO_LEN;
        }
        for(j = 0;j < len; j++)
        {
            Reg->DR = srcaddr[i*CN_FIFO_LEN + j];
        }
        while (!(Reg->SR & 0x04));
    }

    //如果需要接收数据
    for(i = 0; i < recvlen/CN_FIFO_LEN; i++)            //发送数据
    {
        if(i == sendlen/CN_FIFO_LEN - 1)
        {
            len = sendlen % CN_FIFO_LEN;
        }
        else
        {
            len = CN_FIFO_LEN;
        }
        for(j = 0;j < len; j++)
        {
            Reg->DR = 0xFF;
        }
        while (!(Reg->SR & 0x04));
        for(j = 0; j < len; j ++)
        {
            destaddr[i*8+j]=Reg->DR;
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
// 返回：true,无错误;false,失败
// =============================================================================
static bool_t __SSP_TransferTxRx(tagSpiReg *Reg,u32 sendlen,u32 recvlen,
                                u32 recvoff)
{
    struct SPI_IntParamSet *Param=NULL;

    if((u32)Reg == CN_SSP0_BASE)
        Param = &IntParamset0;

    if((u32)Reg == CN_SSP1_BASE)
        Param = &IntParamset1;

    __SSP_TxIntDisable(Reg);
    __SSP_RxIntDisable(Reg);

    while(Reg->SR & (1<<2))//RNE,清空接收缓冲区
        Reg->DR;

    Param->SendDataLen = sendlen;
    Param->RecvDataLen = recvlen;
    Param->RecvOffset  = recvoff;

    __SSP_RxIntEnable(Reg);
    __SSP_TxIntEnable(Reg);//使能发送空中断，中断中接收发送数据

    return true;
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
u32 SSP_ISR(ptu32_t IntLine)
{
    struct SPI_CB *SPI_SCB=NULL;
    struct SPI_IntParamSet *param;
    tagSpiReg *Reg;
    u8 ch[8],num=0,trans;

    if(IntLine == CN_INT_LINE_SSP0)
    {
        SPI_SCB = sp_SSP_CB[CN_SSP0];
        Reg = (tagSpiReg *)SPI_SCB->SpecificFlag;
        param = &IntParamset0;
    }

    if(IntLine == CN_INT_LINE_SSP1)
    {
        SPI_SCB = sp_SSP_CB[CN_SSP1];
        Reg = (tagSpiReg *)SPI_SCB->SpecificFlag;
        param = &IntParamset1;
    }

    if(SPI_SCB == NULL)
        return 0;

    //发送中断
    if( (Reg->IMSC & (1<<3)) && (Reg->RIS & (1<<3)))    //使能了发送中断，且TXRIS
    {
        if(Reg->SR & (1<<0))        //TNF
        {
            trans = SPI_PortRead(SPI_SCB,ch,4);
            if(trans >0)
            {
                for(num = 0; num < trans; num++)
                {
                    Reg->DR = ch[num];
                }
                param->SendDataLen -= trans;
            }
            else
            {
                if(param->SendDataLen == 0)
                __SSP_TxIntDisable(Reg);
//              break;
            }
        }
    }


    //接收中断
    if((Reg->IMSC & ((1<<2) | (1<<1)))  && (Reg->RIS & ((1<<2) | (1<<1))))
    {
        if(param->RecvDataLen > 0)
        {
            while(Reg->SR & (1<<2))//RNE
            {
                ch[0] = Reg->DR;
                if(param->RecvOffset > 0)
                    param->RecvOffset--;
                else
                {
                    if(param->RecvDataLen > 0)
                    {
                        SPI_PortWrite(SPI_SCB,ch,1);
                        param->RecvDataLen--;
                    }
                    else
                        break;
                }
            }
            if((param->SendDataLen == 0) && (param->RecvDataLen > 0))
            {
                //发送完成，还需要接收数据，因此要往总线上发送垃圾数据
                Reg->DR = 0xaa;
            }
        }
        else
        {
            __SSP_RxIntDisable(Reg);
        }
    }

    if(param->SendDataLen + param->RecvDataLen == 0)
    {
//      __SSP_TxIntDisable(Reg);
        __SSP_RxIntDisable(Reg);
    }


    return 0;
}

// =============================================================================
// 功能: SPI中断配置函数
// 参数: IntLine,中断线
// 返回: 无
// =============================================================================
static void __SSP_IntConfig(u8 port)
{
    u8 IntLine;
    if(port == CN_SSP0)
        IntLine = CN_INT_LINE_SSP0;
    else if(port == CN_SSP1)
        IntLine = CN_INT_LINE_SSP1;
    else
        return;
    //中断线的初始化
    Int_Register(IntLine);
    Int_SetClearType(IntLine,CN_INT_CLEAR_AUTO);
    Int_IsrConnect(IntLine,SSP_ISR);
    Int_SettoAsynSignal(IntLine);
    Int_ClearLine(IntLine);
    Int_RestoreAsynLine(IntLine);
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
bool_t SSP0_Init(void)
{
    struct SPI_Param SSP_Config;
    struct SPI_CB *SPI_SCB;
    static struct SPI_CB s_SSP0_CB;
    static u8 s_SSP0_Buf[CN_SSP0_BUF_LEN];

    SSP_Config.BusName          = "SSP0";
    SSP_Config.SPIBuf           = (u8*)&s_SSP0_Buf;
    SSP_Config.SPIBufLen        = CN_SSP0_BUF_LEN;
    SSP_Config.SpecificFlag     = CN_SSP0_BASE;
    SSP_Config.MultiCSRegFlag   = false;
    SSP_Config.pTransferTxRx    = (TransferFunc)__SSP_TransferTxRx;
    SSP_Config.pCsActive        = (CsActiveFunc)__SSP_BusCsActive;
    SSP_Config.pCsInActive      = (CsInActiveFunc)__SSP_BusCsInActive;
    SSP_Config.pBusCtrl         = (SPIBusCtrlFunc)__SSP_BusCtrl;
    SSP_Config.pTransferPoll    = (TransferPoll)__SSP_TxRxPoll;
    SPI_SCB = &s_SSP0_CB;
    sp_SSP_CB[CN_SSP0] = SPI_SCB;

    __SSP_HardDefaultSet(CN_SSP0);
    __SSP_IntConfig(CN_SSP0);

    if(NULL == SPI_BusAdd_s(SPI_SCB,&SSP_Config))
        return 0;
    return 1;
}
bool_t SSP1_Init(void)
{
    struct SPI_Param SSP_Config;
    struct SPI_CB *SPI_SCB;
    static struct SPI_CB s_SSP1_CB;
    static u8 s_SSP1_Buf[CN_SSP1_BUF_LEN];

    SSP_Config.BusName          = "SSP1";
    SSP_Config.SPIBuf           = (u8*)&s_SSP1_Buf;
    SSP_Config.SPIBufLen        = CN_SSP1_BUF_LEN;
    SSP_Config.SpecificFlag     = CN_SSP1_BASE;
    SSP_Config.MultiCSRegFlag   = false;
    SSP_Config.pTransferTxRx    = (TransferFunc)__SSP_TransferTxRx;
    SSP_Config.pCsActive        = (CsActiveFunc)__SSP_BusCsActive;
    SSP_Config.pCsInActive      = (CsInActiveFunc)__SSP_BusCsInActive;
    SSP_Config.pBusCtrl         = (SPIBusCtrlFunc)__SSP_BusCtrl;
    SSP_Config.pTransferPoll    = (TransferPoll)__SSP_TxRxPoll;
    SPI_SCB = &s_SSP1_CB;
    sp_SSP_CB[CN_SSP1] = SPI_SCB;

    __SSP_HardDefaultSet(CN_SSP1);
    __SSP_IntConfig(CN_SSP1);

    if(NULL == SPI_BusAdd_s(SPI_SCB,&SSP_Config))
        return 0;
    return 1;
}


