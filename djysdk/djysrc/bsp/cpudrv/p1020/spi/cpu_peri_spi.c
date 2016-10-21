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
// 文件名     ：cpu_peri_spi.c
// 模块描述: SPI模块硬件驱动，按照SPI BUS模块要求写的驱动
// 模块版本: V1.00
// 创建人员: hm
// 创建时间: 08/10.2014
// =============================================================================


#include "os.h"
#include "cpu_peri.h"
#include "spibus.h"
#include "string.h"
#include "int_hard.h"

// =============================================================================
#define min(x, y)    (((x) < (y)) ? (x) : (y))

#define ESPI_MAX_CS_NUM     4

#define ESPI_EV_RNE                 (1 << 9)
#define ESPI_EV_RXCNT_OFF           (0x3F << 24)
#define ESPI_EV_RXCNT(x)            ((x & ESPI_EV_RXCNT_OFF)>> 24)
#define ESPI_EV_TNF                 (1 << 8)
#define ESPI_EV_TXE                 (1 << 15)

#define ESPI_MODE_EN                (1 << 31)   /* Enable interface */
#define ESPI_MODE_TXTHR(x)          ((x) << 8)  /* Tx FIFO threshold */
#define ESPI_MODE_RXTHR(x)          ((x) << 0)  /* Rx FIFO threshold */

#define ESPI_COM_CS(x)              ((x) << 30)
#define ESPI_COM_RXSKIP(x)          ((x) << 15)
#define ESPI_COM_TRANLEN(x)         ((x) << 0)

#define ESPI_CSMODE_CI_INACTIVEHIGH (1 << 31)
#define ESPI_CSMODE_CP_BEGIN_EDGCLK (1 << 30)
#define ESPI_CSMODE_REV_MSB_FIRST   (1 << 29)
#define ESPI_CSMODE_DIV16           (1 << 28)
#define ESPI_CSMODE_PM(x)           ((x) << 24)
#define ESPI_CSMODE_POL_ASSERTED_LOW    (1 << 20)
#define ESPI_CSMODE_LEN(x)          ((x) << 16)
#define ESPI_CSMODE_CSBEF(x)        ((x) << 12)
#define ESPI_CSMODE_CSAFT(x)        ((x) << 8)
#define ESPI_CSMODE_CSCG(x)         ((x) << 3)

#define ESPI_CSMODE_INIT_VAL (ESPI_CSMODE_POL_ASSERTED_LOW | \
        ESPI_CSMODE_CSBEF(0) | ESPI_CSMODE_CSAFT(0) | \
        ESPI_CSMODE_CSCG(1))

#define ESPI_SPIM_TXE_MASK     (1<<15)
#define ESPI_SPIM_RXT_MASK     (1<<13)

/*一帧数据最多包含多少个数据*/
#define ESPI_MAX_DATA_TRANSFER_LEN 0xFFF0
static u8 spi_cs = 0;/*CS的静态变量，仅供该源文件调用*/

// =============================================================================
//定义中断中需使用的静态量结构体
struct SPI_IntParamSet
{
    u32 SendDataLen;
    u32 RecvDataLen;
    u32 RecvOffset;
};

#define CN_SPI_BASE        cn_spi_baddr
#define CN_SPI_BUF_LEN      128
static struct SPI_CB s_SPI_CB;
static u8 s_SPI_Buf[CN_SPI_BUF_LEN];
struct SPI_IntParamSet IntParamset;





void __SPI_TxRxIntEnable(tagSpiReg *Reg)
{
    Reg->mask |=  (ESPI_SPIM_TXE_MASK);
}

void __SPI_TxRxIntDisable(tagSpiReg *Reg)
{
    Reg->mask &= ~(ESPI_SPIM_TXE_MASK);
}




//-----计算时钟信号---------------------------------------------------------
//功能：计算时钟信号，根据输入的clk计算写入寄存器中的值
//参数：freq_hz，时钟频率
//      div16，返回值，用于配置寄存器
//返回：pm,配置寄存器的值
//----------------------------------------------------------------------
u8 __spi_clk_cal(u32 freq_hz, u32 *div16)
{
    u32 spibrg;
    u8 pm;

    /* Set eSPI BRG clock source */
    spibrg = cfg_core_ccb_clk / 2;
    *div16 = 0;
    if ((spibrg / freq_hz) > 32)
        {
        *div16 = ESPI_CSMODE_DIV16;
        pm = spibrg / (freq_hz * 16 * 2);
        if (pm > 16)
            {
            pm = 16;
//          djy_printf("Requested speed is too low: %d Hz, %ld Hz "
//              "is used.\r\n", max_hz, spibrg / (32 * 16));
        }
    }
        else
        pm = spibrg / (freq_hz * 2);
    if (pm)
        pm--;

    return pm;
}

//-----片选配置---------------------------------------------------------
//功能：片选使能，并初始化寄存器cs对应的寄存器
//参数：tpSpi，被操作的spi控制结构的地址
//      data_len,传送的数据长度
//返回：无
//----------------------------------------------------------------------
void __spi_cs_activate(volatile tagSpiReg * tpSpi,u32 data_len)
{
    unsigned int com = 0;

    com &= ~(ESPI_COM_CS(0x3) | ESPI_COM_TRANLEN(0xFFFF));
    com |= ESPI_COM_CS(spi_cs);
    com |= ESPI_COM_TRANLEN(data_len - 1);
    tpSpi->com = com;
}

//-----片选关闭---------------------------------------------------------
//功能：片选失能，并初始化寄存器cs对应的寄存器
//参数：tpSpi，被操作的spi控制结构的地址
//返回：无
//----------------------------------------------------------------------
void __spi_cs_deactivate(volatile tagSpiReg * tpSpi)
{
    /* clear the RXCNT and TXCNT */
    tpSpi->mode &= ~ESPI_MODE_EN;   /*结束本次传输过程*/
    tpSpi->mode |= ESPI_MODE_EN;    /*重新使能SPI*/
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
void __SPI_SetClk(volatile tagSpiReg *tpSPI,u32 spisck)
{
//  u32 temp;
//  if(tpSPI == NULL)
//      return;
}

// =============================================================================
// 功能：SPI控制寄存器参数配置，如PHA和CPOL、时钟等，根据各种寄存器而异
// 参数：tpSPI,SPI控制器基址
//       ptr,参数指针
// 返回：无
// =============================================================================
void __SPI_Config(volatile tagSpiReg *tpSpi,u8 cs,tagSpiConfig *tagpInConfig)
{
    u8 pm;
    u32 mode,div16;

    mode = tagpInConfig->Mode;
    pm = __spi_clk_cal(tagpInConfig->Freq,&div16);

    /* Enable eSPI interface */
    tpSpi->mode = ESPI_MODE_RXTHR(3) | ESPI_MODE_TXTHR(4) | ESPI_MODE_EN;

    tpSpi->event  = 0xffffffff; /* Clear all eSPI events */
    tpSpi->mask  = 0x00000000; /* Mask  all eSPI interrupts */

    /* Init CS0,1,2,3 mode interface */
    if(cs < 4)
    {
        tpSpi->csmode[cs] = ESPI_CSMODE_INIT_VAL;

        tpSpi->csmode[cs] = tpSpi->csmode[cs] &
            ~(ESPI_CSMODE_PM(0xF) | ESPI_CSMODE_DIV16
            | ESPI_CSMODE_CI_INACTIVEHIGH | ESPI_CSMODE_CP_BEGIN_EDGCLK
            | ESPI_CSMODE_REV_MSB_FIRST | ESPI_CSMODE_LEN(0xF));

        /* Set eSPI BRG clock source */
        tpSpi->csmode[cs] = tpSpi->csmode[cs] | ESPI_CSMODE_PM(pm) | div16;

        /* Set eSPI mode */
        if (mode & SPI_CPHA)
            tpSpi->csmode[cs] = tpSpi->csmode[cs] | ESPI_CSMODE_CP_BEGIN_EDGCLK;
        if (mode & SPI_CPOL)
            tpSpi->csmode[cs] = tpSpi->csmode[cs] | ESPI_CSMODE_CI_INACTIVEHIGH;

        /* Character bit order: msb/lsb first */
        if(tagpInConfig->ShiftDir == SPI_SHIFT_MSB)
            tpSpi->csmode[cs] = tpSpi->csmode[cs] | ESPI_CSMODE_REV_MSB_FIRST;
        else
            tpSpi->csmode[cs] = tpSpi->csmode[cs] & (~ESPI_CSMODE_REV_MSB_FIRST);

        /* Character length in bits, between 0x3~0xf, i.e. 4bits~16bits */
        tpSpi->csmode[cs] = tpSpi->csmode[cs] | ESPI_CSMODE_LEN(7);
    }
}

// =============================================================================
// 功能: SPI默认硬件初始化配置，主要是时钟配置和GPIO写保护引脚配置
// 参数: RegBaseAddr,寄存器基址
// 返回: 无
// =============================================================================
static void __SPI_HardDefaultSet(u32 BaseAddr)
{
    u8 i;
    tagSpiConfig    pg_spi_Config = {SPI_MODE_3,SPI_SHIFT_MSB,8,600000};
    for(i = 0; i < 4; i++)
    {
        __SPI_Config((volatile tagSpiReg *)BaseAddr,i,&pg_spi_Config);
    }
}

// =============================================================================
// 功能：SPI片选使能，使片选有效
// 参数：Reg，本模块内即SPI寄存器基址
//       cs,片选线
// 返回：无
// 说明：SPI控制器上只引出了一个CS，若需要接多个CS，则必须应用程序自己控制CS
// =============================================================================
static bool_t __SPI_BusCsActive(tagSpiReg *Reg, u8 cs)
{
//  unsigned int com = 0;

//  com &= ~(ESPI_COM_CS(0x3) | ESPI_COM_TRANLEN(0xFFFF));
//  com |= ESPI_COM_CS(cs);
//  com |= ESPI_COM_TRANLEN(data_len - 1);
//  Reg->com = com;
    spi_cs = cs;
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
    /* clear the RXCNT and TXCNT */
//  Reg->mode &= ~ESPI_MODE_EN; /*结束本次传输过程*/
//  Reg->mode |= ESPI_MODE_EN;  /*重新使能SPI*/
    return true;
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
    if(Reg == NULL)
        return 0;

    switch(cmd)
    {
    case CN_SPI_SET_CLK:
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
    u32 datalen;
    if((u32)Reg == CN_SPI_BASE)
        Param = &IntParamset;
    else
        return false;

    Param->SendDataLen = sendlen;
    Param->RecvDataLen = recvlen;
    Param->RecvOffset  = recvoff;

    datalen = sendlen+recvlen;
    __SPI_TxRxIntDisable(Reg);
    __spi_cs_activate(Reg,datalen); //配置传输数据量，并使能CS

    Reg->event = 0xffffffff;        //清除所有事件标志
//    while(!(Reg->event & ESPI_EV_TXE));   //等待发送FIFO为空
//    while(ESPI_MODE_TXTHR(Reg->event))
//      Reg->tx;
//    while()

    __SPI_TxRxIntEnable(Reg);

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
    struct SPI_CB *SCB=NULL;
    struct SPI_IntParamSet *param;
    tagSpiReg *Reg;
    u8 ch[32],num,i,j,*tempdst,*tempdin,store_num;
    u32 event,temp;
    static u8 s_TxNum = 0;

    Reg = (tagSpiReg *)CN_SPI_BASE;
    SCB = &s_SPI_CB;
    param = &IntParamset;

    //P1020的FIFO为32字节深度，访问FIFO寄存器为32bit宽度
    event = 0;
    while(ESPI_EV_RXCNT(event) != s_TxNum)//发送，接收是否完成
    {
        event = Reg->event;
    }
    event = Reg->event;
    if((event & ESPI_EV_RNE) && (param->RecvDataLen))       //接收数据
    {
        num = ESPI_EV_RXCNT(event);
        if(param->RecvOffset >= num)                //接收到无效数据，无需存储
        {
            param->RecvOffset -= num;
            while(Reg->event & ESPI_EV_RNE)
                temp = Reg->rx;
            Reg->event |= ESPI_EV_RNE;
        }
        else                                        //减去无效数据，存储有效数据
        {
            tempdst = (u8*)&ch;
            Reg->event |= ESPI_EV_RNE;
            while(param->RecvOffset >4)
            {
                temp = Reg->rx;
                param->RecvOffset -= 4;
                num -= 4;
            }

            num = param->RecvDataLen > num ? num:param->RecvDataLen;
            i = num/4 + (num%4>0 ?1:0);
            for(j=0; j < i;j++)             //将接收到的数据存储
            {
                temp = Reg->rx;
                tempdin = (u8*)&temp;
                if(j == 0)                  //首个有效字节
                {
                    if( num > (4 - param->RecvOffset))
                        store_num = 4 - param->RecvOffset;
                    else
                        store_num = num;
                    param->RecvOffset = 0;
                }
                else if(j == i-1)
                {
                    store_num = num - 4*j;
                }
                else
                {
                    store_num = 4;
                }

                param->RecvDataLen -= store_num;
                while(store_num--)
                    *tempdst++ = *tempdin++;
            }
            SPI_PortWrite(SCB,ch,num);
        }
    }

    if(event & ESPI_EV_TXE)         //发送空中断
    {
        Reg->event = ESPI_EV_TXE;
        num = SPI_PortRead(SCB,ch,32);
        if(num > 0)
        {
            i = num/4 + (num%4 > 0 ?1:0);
            for(j = 0; j < i; j ++)
            {
                temp = *(u32*)&ch[j*4];
                Reg->tx = temp;
            }
            param->SendDataLen -= num;
        }
        else if(param->RecvDataLen > 0) //发送完数据，还需要提供接收时钟
        {
            temp = 0xaabbcc55;      //发无效数据，提供时钟
            Reg->tx = temp;
            num = param->RecvDataLen >4 ? 4:param->RecvDataLen;
        }
        else if(param->RecvDataLen + param->SendDataLen == 0)
        {
            __SPI_TxRxIntDisable(Reg);
            __spi_cs_deactivate(Reg);
        }
    }
    s_TxNum = num;
    return 0;
}


// =============================================================================
// 功能: SPI中断配置函数
// 参数: IntLine,中断线
// 返回: 无
// =============================================================================
static void __SPI_IntConfig(void)
{
    u8 IntLine = cn_int_line_espi;
    //中断线的初始化
    Int_Register(IntLine);
    Int_IsrConnect(IntLine,SPI_ISR);
    Int_SettoAsynSignal(IntLine);
    Int_SetLineTrigerType(IntLine,EN_INT_TRIGER_HIGHLEVEL);
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
ptu32_t SPI_ModuleInit(ptu32_t para)
{
    struct SPI_Param SPI_Config;
    struct SPI_CB *SCB;

    SCB = &s_SPI_CB;
    SPI_Config.BusName          = "SPI0";
    SPI_Config.SPIBuf           = (u8*)&s_SPI_Buf;
    SPI_Config.SPIBufLen        = CN_SPI_BUF_LEN;
    SPI_Config.SpecificFlag     = CN_SPI_BASE;
    SPI_Config.MultiCSRegFlag   = false;
    SPI_Config.pTransferTxRx    = (TransferFunc)__SPI_TransferTxRx;
    SPI_Config.pCsActive        = (CsActiveFunc)__SPI_BusCsActive;
    SPI_Config.pCsInActive      = (CsInActiveFunc)__SPI_BusCsInActive;
    SPI_Config.pBusCtrl         = (SPIBusCtrlFunc)__SPI_BusCtrl;

    __SPI_HardDefaultSet(SPI_Config.SpecificFlag);
    __SPI_IntConfig();

    if(NULL == SPI_BusAdd_s(SCB,&SPI_Config))
        return 0;
    return 1;
}
