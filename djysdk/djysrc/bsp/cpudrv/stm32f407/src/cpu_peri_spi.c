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
#include "djyos.h"
#include "cpu_peri.h"
#include "int.h"
#include "spibus.h"

// =============================================================================
#define CN_TIMEOUT  (20*1000)
#define tagSpiReg SPI_TypeDef
static tagSpiReg volatile * const tg_SpiReg[] = {(tagSpiReg *)SPI1_BASE,
                                                (tagSpiReg *)SPI2_BASE,
                                                (tagSpiReg *)SPI3_BASE};

//定义中断中需使用的静态量结构体
struct SPI_IntParamSet
{
    u32 SendDataLen;
    u32 RecvDataLen;
    u32 RecvOffset;
};
// =============================================================================
//定义SPI控制块和中断静态量
#define CN_SPI_BUF_LEN              128
#define CN_SPI_DMA_BUF_LEN          128
static struct SPI_CB s_Spi1_CB;
static u8 s_Spi1_Buf[CN_SPI_BUF_LEN];
struct SPI_IntParamSet IntParamset1;

static struct SPI_CB s_Spi2_CB;
static u8 s_Spi2_Buf[CN_SPI_BUF_LEN];
struct SPI_IntParamSet IntParamset2;

static struct SPI_CB s_Spi3_CB;
static u8 s_Spi3_Buf[CN_SPI_BUF_LEN];
struct SPI_IntParamSet IntParamset3;

// =============================================================================
// 功能: 中断使能和失能函数
// 参数: tpSPI,被操作的寄存器组指针
// 返回: 无
// =============================================================================
static void __SPI_IntEnable(volatile tagSpiReg *Reg,u32 IntSrc)
{
    Reg->CR2 |= IntSrc;
}
static void __SPI_IntDisable(volatile tagSpiReg *Reg,u32 IntSrc)
{
    Reg->CR2 &= ~IntSrc;
}

// =============================================================================
// 功能: 根据APB2的时钟和Fre计算BR寄存器的写入值
// 参数: Fre,想要配置的SPI时钟
// 返回: BR值
// =============================================================================
static u32 __SPI_BrCal(u32 Fre)
{
    u32 temp;
    temp = CN_CFG_PCLK2/Fre;
    if(temp >= 256)         temp = 7;
    else if(temp >= 128)    temp = 6;
    else if(temp >= 64)     temp = 5;
    else if(temp >= 32)     temp = 4;
    else if(temp >= 16)     temp = 3;
    else if(temp >= 8)      temp = 2;
    else if(temp >= 4)      temp = 1;
    else                    temp = 0;

    return temp;
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
static void __SPI_SetClk(volatile tagSpiReg *Reg,u32 Fre)
{
    u8 br;
    br = (u8)__SPI_BrCal(Fre);
    Reg->CR1 &= ~SPI_CR1_SPE;
    Reg->CR1 &= ~SPI_CR1_BR;
    Reg->CR1 |= br<<3;
}

// =============================================================================
// 功能：SPI引脚初始化，初始化为外设使用，包括时钟和引脚
// 参数：spi_no,SPI控制器号
// 返回：无
// =============================================================================
static void __SPI_GpioInit(u32 BaseAddr)
{
#if 0
    u8 i;
    for(i = 0; i < CN_SPI_NUM; i++)
    {
        if(BaseAddr == (u32)tg_SpiReg[i])
            break;
    }
    if(i == CN_SPI_NUM)
        return;

    extern bool_t BoardBoard_SpiGpioInit(u8 SPIx);
    Board_SpiGpioInit(i); //
#else
    u8 SPIPort;

    // 根据基地址找对应的端口号
    for(SPIPort = 0; SPIPort < CN_SPI_NUM; SPIPort++)
    {
        if(BaseAddr == (u32)tg_SpiReg[SPIPort])
            break;
    }

    // 当前只支持了SPI1,其他暂不管
    if(SPIPort == CN_SPI1)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // 使能PORTB时钟
        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // 使能SPI1时钟

        RCC->APB2RSTR |= RCC_APB2RSTR_SPI1; // 复位SPI1
        RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1; // 停止复位SPI1

        GPIO_CfgPinFunc(GPIO_B, PIN4|PIN5|PIN3, GPIO_MODE_AF, GPIO_OTYPE_PP,
                        GPIO_SPEED_50M, GPIO_PUPD_PU); // GPB3、4和5为SCK、MISO和MOSI
        GPIO_AFSet(GPIO_B, 3, 5);
        GPIO_AFSet(GPIO_B, 4, 5);
        GPIO_AFSet(GPIO_B, 5, 5);
    }
#endif
}

// =============================================================================
// 功能：SPI控制寄存器参数配置，如PHA和CPOL、时钟等，根据各种寄存器而异
// 参数：tpSPI,SPI控制器基址
//       ptr,参数指针
// 返回：无
// =============================================================================
static void __SPI_Config(volatile tagSpiReg *Reg,tagSpiConfig *ptr)
{
    __SPI_SetClk(Reg,ptr->Freq);

    //set the PHA
    if(ptr->Mode & SPI_CPHA)
    {
        Reg->CR1 |= SPI_CR1_CPHA;
    }
    else
    {
        Reg->CR1 &= ~SPI_CR1_CPHA;
    }
    //set the cpol
    if(ptr->Mode & SPI_CPOL)
    {
        Reg->CR1 |= SPI_CR1_CPOL;
    }
    else
    {
        Reg->CR1 &= ~SPI_CR1_CPOL;
    }

    if(ptr->CharLen == 16)
    {
        Reg->CR1 |= SPI_CR1_DFF;
    }
    else
    {
        Reg->CR1 &= ~SPI_CR1_DFF;
    }
    if(ptr->ShiftDir == SPI_SHIFT_LSB)
    {
        Reg->CR1 |= SPI_CR1_LSBFIRST;
    }
    else
    {
        Reg->CR1 &= ~SPI_CR1_LSBFIRST;
    }
}

// =============================================================================
// 功能: SPI默认硬件初始化配置，主要是时钟配置和GPIO写保护引脚配置
// 参数: RegBaseAddr,寄存器基址
// 返回: 无
// =============================================================================
static void __SPI_HardConfig(u32 BaseAddr)
{
    tagSpiReg *Reg;
    u32 temp = 0;

    Reg = (tagSpiReg *)BaseAddr;
    //配置SPI使用GPIO引脚
    __SPI_GpioInit(BaseAddr);

    //这里只针对SPI口初始化
    temp &= ~SPI_CR1_RXONLY;                   // 全双工模式
    temp |= SPI_CR1_SSM;                       // 软件nss管理
    temp |= SPI_CR1_SSI;
    temp |= SPI_CR1_MSTR;                      // SPI主机
    temp &= ~SPI_CR1_DFF;                      // 8位数据格式
    temp |= SPI_CR1_CPOL;                      // 空闲模式下SCK为1 CPOL=1
    temp |= SPI_CR1_CPHA;                      // 数据采样从第2个时间边沿开始,CPHA=1
    //对SPI1属于APB2的外设.时钟频率最大为84Mhz频率.
    temp |= 7<<3;                              // Fsck=Fpclk1/256
    temp &= ~SPI_CR1_LSBFIRST;                 // MSB First
    temp |= SPI_CR1_SPE;                       // SPI启动
    Reg->CR1 = (u16)temp ;                     // 设置CR1
    Reg->I2SCFGR &= (u16)(~SPI_I2SCFGR_I2SMOD);// 选择SPI模式
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
    return false;
}

// =============================================================================
// 功能：SPI片选失能，使片选无效
// 参数：Reg，本模块内即SPI寄存器基址
//       cs,片选线
// 返回：无
// =============================================================================
static bool_t __SPI_BusCsInActive(tagSpiReg *Reg, u8 cs)
{
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
    if( (Reg != tg_SpiReg[CN_SPI1]) && (Reg != tg_SpiReg[CN_SPI2])
            && (Reg != tg_SpiReg[CN_SPI3]) )
        return 0;

    switch(cmd)
    {
    case CN_SPI_SET_CLK:
        __SPI_SetClk(Reg,data1);
        break;
    case CN_SPI_CS_CONFIG:
        __SPI_Config(Reg,(tagSpiConfig *)data1);
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
static s32 __SPI_Read( tagSpiReg * Reg )
{
    u32 time = CN_TIMEOUT;
    while ( ( (Reg->SR & SPI_SR_RXNE) == 0 ) && (time > 0))
    {
        time --;
        Djy_DelayUs(1);
    }

    if(time == 0)
        return -1;
    return Reg->DR & 0xFFFF ;
}

// =============================================================================
// 功能：写SPI发送寄存器，轮询方式,被__SPI_TxRxPoll调用
// 参数：Reg,寄存器基址；wData，写数据
// 返回：无
// =============================================================================
static s32 __SPI_Write( tagSpiReg * Reg,uint16_t wData )
{
    u32 time = CN_TIMEOUT;
    /* Send data */
    while ( ( (Reg->SR & SPI_SR_TXE) == 0 ) && (time > 0) )
    {
        time --;
        Djy_DelayUs(1);
    }
    if(time == 0)
        return -1;
    Reg->DR = wData;
    return 0;
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
    u8 data;
    if( (!srcAddr) || ((rdSize != 0) && (!destAddr)))
        return false;

    Reg->CR1 |= SPI_CR1_SPE;
    for (i=0;i<(wrSize+rdSize);i++)
    {
        if (i<wrSize)
        {
            if(-1 == __SPI_Write(Reg,srcAddr[i]))
                return false;
            if(-1 ==__SPI_Read(Reg))
                return false;
        }
        else if(i<(wrSize+rdSize))
        {
          if(-1 == __SPI_Write(Reg,0))
              return false;
          data = __SPI_Read(Reg);
          if(-1 == data)
              return false;
          destAddr[i-wrSize] = data;
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

    if(Reg == tg_SpiReg[CN_SPI1])
        Param = &IntParamset1;
    else if(Reg == tg_SpiReg[CN_SPI2])
        Param = &IntParamset2;
    else
        Param = &IntParamset3;

    Param->SendDataLen = sendlen;
    Param->RecvDataLen = recvlen;
    Param->RecvOffset  = recvoff;

    __SPI_IntEnable(Reg,SPI_CR2_TXEIE);
    Reg->CR1 |= SPI_CR1_SPE;

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
u32 SPI_ISR(ptu32_t IntLine)
{
    struct SPI_CB *pSCB=NULL;
    struct SPI_IntParamSet *param;
    tagSpiReg *Reg;
    u32 spi_sr,spi_cr2;
    u8 ch,temp;
    static u8 firstrcv = 0;

    if(IntLine == CN_INT_LINE_SPI1)
    {
        pSCB = &s_Spi1_CB;
        Reg = (tagSpiReg *)tg_SpiReg[CN_SPI1];
        param = &IntParamset1;
    }
    else if(IntLine == CN_INT_LINE_SPI2)
    {
        pSCB = &s_Spi2_CB;
        Reg = (tagSpiReg *)tg_SpiReg[CN_SPI2];
        param = &IntParamset2;
    }
    else if(IntLine == CN_INT_LINE_SPI3)
    {
        pSCB = &s_Spi3_CB;
        Reg = (tagSpiReg *)tg_SpiReg[CN_SPI3];
        param = &IntParamset3;
    }
    else
        return 0;

    spi_sr = Reg->SR;
    spi_cr2 = Reg->CR2;
    //发送中断
    if( (spi_cr2 & SPI_CR2_TXEIE) && (spi_sr & SPI_SR_TXE)) //使能了发送中断，且TXRIS
    {
        temp = SPI_PortRead(pSCB,&ch,1);//是否还有数据需要发送

        if(temp >0)
        {
            param->SendDataLen --;
            param->RecvOffset  --;
            Reg->DR = ch;
        }
        else                                            //表明发送结束
        {
            //关发送中断
            __SPI_IntDisable(Reg,SPI_CR2_TXEIE);
            param->RecvOffset -= param->SendDataLen;
            param->SendDataLen = 0;

            ch = Reg->DR;

            //发送完成了，判断是否需要接收
            if(param->RecvDataLen > 0)
            {
                //MASTER模式下，发送和接收同时进行
                Reg->DR = 0xAA;
                __SPI_IntEnable(Reg,SPI_CR2_RXNEIE);    //使能接收中断
                firstrcv = 1;
            }
        }
    }
    else if((spi_cr2 & SPI_CR2_RXNEIE) && (spi_sr & SPI_SR_RXNE))    //接收中断
    {
        ch = Reg->DR;
        //计算接收了多少数据
        if(firstrcv != 1)
        {
            SPI_PortWrite(pSCB,&ch,1);
            param->RecvDataLen -= 1;
        }
        else
            firstrcv = 0;

        //判断是否还需接收数据
        if(param->RecvDataLen > 0)
        {
            Reg->DR = 0xAA;
        }
        else                    //表明接收已经完成
        {
            param->RecvDataLen = 0;
            __SPI_IntDisable(Reg,SPI_CR2_RXNEIE);
        }
    }

    if(param->RecvDataLen + param->SendDataLen == 0)
    {
        Reg->CR1 &= ~SPI_CR1_SPE;
    }

    return 0;
}

// =============================================================================
// 功能: SPI中断配置函数
// 参数: IntLine,中断线
// 返回: 无
// =============================================================================
static void __SPI_IntConfig(u8 IntLine)
{
    //中断线的初始化
    Int_Register(IntLine);
    Int_SetClearType(IntLine,CN_INT_CLEAR_AUTO);
    Int_IsrConnect(IntLine,SPI_ISR);
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
bool_t SPI_Initialize(u8 port)
{
    struct SPI_Param SPI_Config;
    struct SPI_CB *pSpiCB = NULL;
    u8 IntLine;

    switch(port)
    {
    case CN_SPI1:
        SPI_Config.BusName          = "SPI1";
        SPI_Config.SPIBuf           = (u8*)&s_Spi1_Buf;
        IntLine = CN_INT_LINE_SPI1;
        pSpiCB = &s_Spi1_CB;
        break;
    case CN_SPI2:
        SPI_Config.BusName          = "SPI2";
        SPI_Config.SPIBuf           = (u8*)&s_Spi2_Buf;
        IntLine = CN_INT_LINE_SPI2;
        pSpiCB = &s_Spi2_CB;
        break;
    case CN_SPI3:
        SPI_Config.BusName          = "SPI3";
        SPI_Config.SPIBuf           = (u8*)&s_Spi3_Buf;
        IntLine = CN_INT_LINE_SPI3;
        pSpiCB = &s_Spi3_CB;
        break;
    default:
        return (0);
    }
    SPI_Config.SpecificFlag     = (ptu32_t)tg_SpiReg[port];
    SPI_Config.SPIBufLen        = CN_SPI_BUF_LEN;
    SPI_Config.MultiCSRegFlag   = false;
    SPI_Config.pTransferPoll    = (TransferPoll)__SPI_TxRxPoll;
    SPI_Config.pTransferTxRx    = (TransferFunc)__SPI_TransferTxRx;
    SPI_Config.pCsActive        = (CsActiveFunc)__SPI_BusCsActive;
    SPI_Config.pCsInActive      = (CsInActiveFunc)__SPI_BusCsInActive;
    SPI_Config.pBusCtrl         = (SPIBusCtrlFunc)__SPI_BusCtrl;

    if(NULL == SPI_BusAdd_s(pSpiCB, &SPI_Config)) // 将SPI端口添加到SPI总线
        return (0); // 失败

    __SPI_HardConfig((u32)SPI_Config.SpecificFlag); // SPI寄存器设置

    __SPI_IntConfig(IntLine); // 注册中断

    return (1); // 成功
}
//-----------------------------------------------------------------------------
//功能: 安装SPI端口
//参数: SPI端口号
//返回: -1 -- 失败; 0 -- 成功;
//备注:
//-----------------------------------------------------------------------------
s32 ModuleInstall_SPIPort(u8 Port)
{
    if(CN_SPI1 != Port) // 板件只支持到SPI1
        return (-1);
    if(0 == SPI_Initialize(Port))
        return (-1);
    return (0);
}

