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
// 文件名     ：cpu_peri_gmac.c
// 模块描述: GMAC模块的初始化的操作，包含PHY初始化和MII初始化
//           按DJYOS的以太网协议实现要求实现的网卡底层驱动
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 25/11.2015
// =============================================================================
#if 0 // 前福最新修改了以太网协议，因此驱动部分未来得及修改

#include "cpu_peri.h"
#include "stdio.h"
#include "string.h"
#include <tcpip/netdev.h>
#include <tcpip/rout.h>
#include <tcpip/tcpip_cdef.h>
#include "ksz8051mnl.h"

// =============================================================================
#if(DEBUG == 1)
#define GMAC_DBG   printk
#else
#define GMAC_DBG(...)
#endif

#define RAW_GMAC

#define enableCAF GMAC_CAF_ENABLE
#define enableNBC GMAC_NBC_DISABLE          //不屏蔽广播报文
// =============================================================================
#define CN_HARD_BOARD            0x03
static u8  sgNetHardMac[6] = {0x00,0x01, 0x02, 0x03, 0x04, CN_HARD_BOARD};
static u32  sgNetHardIpAddrMain = 0xC0A80100;
static u32  sgNetHardIpMsk = 0xFFFFFF00 ; //255.255.255.0
static u32  sgNetHardGateWay = 0xC0A80101; //192.168.1.1
static struct tagSemaphoreLCB GMAC_RcvSemp;

#define CN_PKG_MAX_LEN  1500
static tagRoutTab  *ptNetRout = NULL;
static tagNetDev   *ptNetDev = NULL;

// =============================================================================
// 该指针初始化到按8字节对齐的BD表
#ifdef __GNUC__
static volatile GRxTd GrxTd __attribute__((aligned(8)));
static volatile GTxTd GtxTd __attribute__((aligned(8)));
static volatile u8 RxBuffer[GRX_BUFFERS * GMAC_RX_UNITSIZE] __attribute__((aligned(8)));
static volatile u8 TxBuffer[GMAC_TX_UNITSIZE] __attribute__((aligned(8)));
#else
#error "bd must be 8-byte aligned"
#endif

#define FRAME_START_BUF     (1<<0)          //首包标记
#define FRAME_END_BUF       (1<<1)          //尾包标记

#define  SendPkgWaitMs  1000

#define NEW_PRO 0
/* Return count up to the end of the buffer. */
/* Carefully avoid accessing head and tail more than once, */
/* so they can change underneath us without returning inconsistent results */
#define GCIRC_CNT_TO_END(head, tail, size) \
    ({int end = (size) - (tail); \
      int n = ((head) + end) & ((size) - 1); \
      n < end ? n : end; } \
    )

/* Return space available up to the end of the buffer */
#define GCIRC_SPACE_TO_END(head, tail, size) \
    ({int end = (size) - 1 - (head); \
      int n = (end + (tail)) & ((size) - 1); \
      n <= end ? n : end + 1; } \
    )

/* Increment head or tail */
#define GCIRC_INC(headortail, size) \
    headortail++; \
    if (headortail >= size) { \
        headortail = 0; \
    }

#define GCIRC_EMPTY(circ)     ((circ)->head == (circ)->tail)
#define GCIRC_CLEAR(circ)     ((circ)->head = (circ)->tail = 0)
/* Return count in buffer */
#define GCIRC_CNT(head, tail, size) (((head) - (tail)) & ((size) - 1))

/* Return space available, 0..size-1 */
/* We always leave one free char as a completely full buffer */
/* has head == tail, which is the same as empty */
#define GCIRC_SPACE(head, tail, size) GCIRC_CNT((tail), ((head) + 1), (size))

static GMacb gMacb;

static void gmac_disable_txrx(void)
{
    GMAC->GMAC_NCR &=  ~(GMAC_NCR_RXEN | GMAC_NCR_TXEN);
}

/**
 * \brief Gmac, enable receiver
 */
static void gmac_enable_txrx(void)
{
    GMAC->GMAC_NCR |=  (GMAC_NCR_RXEN | GMAC_NCR_TXEN);
}

// =============================================================================
// 功能：网卡等待PHY
// 参数：retry，尝试次数
// 返回：1，成功；0失败
// =============================================================================
static u8 __GMAC_WaitPhy( u32 retry )
{
    volatile uint32_t ul_retry_count = 0;

    while ((GMAC->GMAC_NSR & GMAC_NSR_IDLE) == 0)
    {
        /* Dead LOOP! */
        if (retry == 0)
        {
            continue;
        }

        /* Timeout check */
        ul_retry_count++;
        if (ul_retry_count >= retry)
        {
            GMAC_DBG("Error: Wait PHY time out\n\r");
            return 0;
        }
    }

    return 1;
}

// =============================================================================
// 功能：MII接口写函数，通过MDIO与PHY芯片通信
// 参数：phy_addr, PHY地址
//       reg_addr,寄存器地址
//       data,写入的数据
// 返回：1,正常返回；0,失败
// =============================================================================
u32 GMAC_WritePhy(u32 PhyAddress, u32 Address, u32 Value,u32 retry)
{
    GMAC->GMAC_MAN = GMAC_MAN_WTN(GMAC_MAN_CODE_VALUE)
            | GMAC_MAN_CLTTO
            | GMAC_MAN_PHYA(PhyAddress)
            | GMAC_MAN_REGA(Address)
            | GMAC_MAN_OP((0 ? GMAC_MAN_RW_TYPE : GMAC_MAN_READ_ONLY))
            | GMAC_MAN_DATA(Value);

    if (__GMAC_WaitPhy(retry) == 0)
    {
        GMAC_DBG("TimeOut GMAC_WritePhy\n\r");
        return 0;
    }

    return 1;
}

// =============================================================================
// 功能：MII接口读数据，通过MDIO接口与PHY芯片通信
// 参数：PhyAddress, PHY地址
//       Address,寄存器地址
//       pValue,读取到的数据指针
//       retry,失败尝试的次数
// 返回：1,正常返回；0,失败
// =============================================================================
u32 GMAC_ReadPhy(u32 PhyAddress, u32 Address, u32 *pValue,u32 retry)
{
    /* PHY Maintenance Register */
    GMAC->GMAC_MAN = ((~GMAC_MAN_WZO) & GMAC_MAN_CLTTO )
                            | GMAC_MAN_OP(0x02)        /* read */
                            | GMAC_MAN_WTN(0x02)       /* must be */
                            | GMAC_MAN_PHYA(PhyAddress)
                            | GMAC_MAN_REGA(Address);

    if (__GMAC_WaitPhy(retry) == 0)
    {
        GMAC_DBG("TimeOut GMAC_ReadPhy\n\r");
        return 0;
    }

    *pValue = (GMAC->GMAC_MAN & 0x0000ffff);
    return 1;
}

// =============================================================================
// 功能：MDIO管理总线的使能和禁能
// 参数：无
// 返回：无
// =============================================================================
void GMAC_MdioEnable(void)
{
    gmac_disable_txrx();
    GMAC->GMAC_NCR |= GMAC_NCR_MPE;
    gmac_enable_txrx();
}

void GMAC_MdioDisable(void)
{
    gmac_disable_txrx();
    GMAC->GMAC_NCR &= ~GMAC_NCR_MPE;
    gmac_enable_txrx();
}
void GMAC_EnableMii(void)
{
    gmac_disable_txrx();
    GMAC->GMAC_UR  |= GMAC_UR_RMIIMII;
    gmac_enable_txrx();
}

// =============================================================================
// 功能：设置GMAC的连接速度:speed 100M/10M and Full/Half duplex
// 参数：speed, 0 for 10M, 1 for 100M, 2 for 1000M
//      fullduplex ,1 for Full Duplex mode
// 返回：CRC，哈希算法计算的CRC值高8比特值
// =============================================================================
void GMAC_SetLinkSpeed(u8 speed, u8 fullduplex)
{
    unsigned int ncfgr;
    gmac_disable_txrx();

    ncfgr = GMAC->GMAC_NCFGR;
    ncfgr &= ~(GMAC_NCFGR_SPD | GMAC_NCFGR_FD);
    if (speed) {
        ncfgr |= GMAC_NCFGR_SPD;
    }

    if (fullduplex) {
        ncfgr |= GMAC_NCFGR_FD;
    }

    GMAC->GMAC_NCFGR = ncfgr;

    GMAC->GMAC_NCR |=  (GMAC_NCR_RXEN | GMAC_NCR_TXEN);     // Enable Tx Rx
}

// =============================================================================
// 功能：MAC地址配置和地址hash表配置，取计算的哈希CRC最高的6比特
// 参数：pa,MAC地址数据指针
// 返回：无
// =============================================================================
static void __GMAC_AddrSet (u8 *pa)
{
    GMAC->GMAC_SA[0].GMAC_SAB = ( (pa[3] << 24) | (pa[2] << 16)
                                    | (pa[1] << 8)  |  pa[0] );

    GMAC->GMAC_SA[0].GMAC_SAT = ( (pa[5] << 8) | pa[4] );
}

// =============================================================================
// 功能：Buffer Descriptor初始化，用于发送和接收数据，根据手册，BD表必须按8字节对齐
//       并且data指针所指向的地址也必须是按8字节对齐
// 参数：无
// 返回：无
// =============================================================================
static void __GMAC_RxBD_Init(void)
{
    unsigned int Index;
    unsigned int Address;

    /* Disable RX */
    GMAC->GMAC_NCR &= ~GMAC_NCR_RXEN;
    /* Setup the RX descriptors. */
    GrxTd.idx = 0;
    for (Index = 0; Index < GRX_BUFFERS; Index++)
    {
        Address = (unsigned int)(&(RxBuffer[Index * GMAC_RX_UNITSIZE]));
        /* Remove GMAC_RX_OWNERSHIP_BIT and GMAC_RX_WRAP_BIT */
        GrxTd.td[Index].addr.val = Address & 0xFFFFFFFC;
        GrxTd.td[Index].status.val = 0;
    }
    GrxTd.td[GRX_BUFFERS - 1].addr.bm.b_wrap = 1;
    /* Receive Buffer Queue Pointer Register */
    GMAC->GMAC_RBQB = (unsigned int)(GrxTd.td);
}

static void __GMAC_TxBD_Init(void)
{
    unsigned int Index;
    unsigned int Address;

    /* Disable TX */
    GMAC->GMAC_NCR &= ~GMAC_NCR_TXEN;
    /* Setup the TX descriptors. */
    GCIRC_CLEAR(&GtxTd);
    for (Index = 0; Index < GTX_BUFFERS; Index++)
    {
        Address = 0;                                //等发送的时候再配置
        GtxTd.td[Index].addr = Address;
        GtxTd.td[Index].status.bm.b_used = 1;
    }
    GtxTd.td[GTX_BUFFERS - 1].status.bm.b_wrap = 1;
    GtxTd.td[GTX_BUFFERS - 1].status.bm.b_used = 1;
    /* Transmit Buffer Queue Pointer Register */
    GMAC->GMAC_TBQB = (unsigned int)(GtxTd.td);
}

// =============================================================================
// 功能：GMAC的PORT口初始化，主要是对时钟的使能，GPIO配置，根据RMII和MII
//       分别配置不同的IO引脚功能
// 参数：无
// 返回：无
// =============================================================================
static void __GMAC_PortInit(void)
{
    extern void Board_PhyIoInit(void);
    Board_PhyIoInit();
}

// =============================================================================
// 功能：GMAC中断初始化，使用中断的方式接收数据，用于配置中断寄存器
// 参数：无
// 返回：无
// =============================================================================
static void __GMAC_IntInit(void)
{
    u32 GMAC_IntHandler(ufast_t IntLine);
    Int_Register(CN_INT_LINE_GMAC);
    Int_IsrConnect(CN_INT_LINE_GMAC,GMAC_IntHandler);
    Int_SettoAsynSignal(CN_INT_LINE_GMAC);
    Int_ClearLine(CN_INT_LINE_GMAC);
    Int_ContactLine(CN_INT_LINE_GMAC);
}

// =============================================================================
// 功能：PHY芯片初始化，本函数使用自适应方式，通过读取PHY芯片中SPEED和双工的相关寄存
//       器，来获得配置MAC的相关参数
// 参数：无
// 返回：true,link成功；false,link失败
// =============================================================================
static bool_t __PHY_Init(void)
{
    /* Init MAC PHY driver */
    ksz8051mnl_init(&gMacb, BOARD_GMAC_PHY_ADDR);

    if (!ksz8051mnl_init_phy(&gMacb, CN_CFG_MCLK))
    {
        GMAC_DBG("PHY Initialize ERROR!\r");
        return false;
    }

    /* Auto Negotiate, work in MII mode */
    if (!ksz8051mnl_auto_negotiate(&gMacb/*, BOARD_GMAC_PHY_ADDR*/))
    {
        GMAC_DBG("Auto Negotiate ERROR!\r");
        return false;
    }

    /* Establish ethernet link */
//  while(GMAC_OK != ksz8051mnl_set_link(&gMacb, BOARD_GMAC_PHY_ADDR, 1))
//  {
//      GMAC_DBG("Set link ERROR!\r");
//  }
    return true;
}

// =============================================================================
// 功能：以太网模块MAC初始化，主要是MAC寄存器和BD表的初始化，下几个步骤：
//       1.外设时钟使能，禁止发送接收和中断，清标志，配置MII、RMII模式；
//       2.初始化BD表，包括发送和接收，配置TCR和RCR，即发送接收功能；
//       3.MAC地址初始化，写MAC到寄存器，配置广播包，地址过滤等；
//       4.配置中断使能，最后使能GMAC。
// 参数：无
// 返回：无
// =============================================================================
static void __GMAC_Init(void)
{
    // Enable GMAC clock
    PMC_EnablePeripheral(ID_GMAC);

    // Establish ethernet link
    GMAC->GMAC_NCR = 0;             // Disable TX & RX and more
    GMAC->GMAC_IDR = ~0;            // Disable all INT

    // MII MODE
    GMAC_EnableMii();

    // Receiver Buffer List Init
    __GMAC_RxBD_Init();
    // Transmit Buffer List Init
    __GMAC_TxBD_Init();
    // Set MAC Address
    __GMAC_AddrSet((u8*)&sgNetHardMac);

    GMAC->GMAC_NCR = GMAC_NCR_CLRSTAT;

    /* Clear all status bits in the receive status register. */
    GMAC->GMAC_RSR = (GMAC_RSR_RXOVR | GMAC_RSR_REC
            | GMAC_RSR_BNA | GMAC_RSR_HNO);

    /* Clear all status bits in the transmit status register */
    GMAC->GMAC_TSR = (GMAC_TSR_UBR | GMAC_TSR_COL | GMAC_TSR_RLE
            | GMAC_TSR_TXGO | GMAC_TSR_TFC | GMAC_TSR_TXCOMP
            | GMAC_TSR_UND | GMAC_TSR_HRESP );

    /* Clear interrupts */
    GMAC->GMAC_ISR;

    /* Enable the copy of data into the buffers */
    /* ignore broadcasts, and don't copy FCS. */
    GMAC->GMAC_NCFGR |= GMAC_NCFGR_FD; /* Full Duplex */

    /* Enable Non-VLAN FRAMES */
    GMAC->GMAC_NCFGR &= ~(GMAC_NCFGR_DNVLAN);

    if (enableCAF == GMAC_CAF_ENABLE)
    {
        /* When set to logic one, all valid frames will be accepted. */
        GMAC->GMAC_NCFGR |= GMAC_NCFGR_CAF;
    }

    if (enableNBC == GMAC_NBC_ENABLE)
    {
      /* When set to logic one, frames addressed to the broadcast address
       * of all ones will not be accepted. */
        GMAC->GMAC_NCFGR |= GMAC_NCFGR_NBC;
    }

    /* Setup the interrupts for TX (and errors) */
    GMAC->GMAC_IER = GMAC_IER_PDRSFT | GMAC_IER_PDRQFT | GMAC_IER_PDRSFR |
                    GMAC_IER_PDRQFR | GMAC_IER_SFT   | GMAC_IER_DRQFT |
                    GMAC_IER_SFR    | GMAC_IER_DRQFR | GMAC_IER_EXINT |
                    GMAC_IER_PFTR   | GMAC_IER_PTZ   | GMAC_IER_PFNZ  |
                    GMAC_IER_HRESP  | GMAC_IER_ROVR  | GMAC_IER_TCOMP |
                    GMAC_IER_TFC    | GMAC_IER_RLEX  | GMAC_IER_TUR   |
                    GMAC_IER_TXUBR  | GMAC_IER_RXUBR | GMAC_IER_RCOMP |
                    GMAC_IER_MFS;

    /* Enable Rx and Tx, plus the stats register. */
    GMAC->GMAC_NCR |= (GMAC_NCR_TXEN | GMAC_NCR_RXEN);
}

// =============================================================================
// 功能：GMAC以太网的MAC层发生中断服务函数，包括接收整包数据、发送完整包数据和发生错误等
// 参数：int_line,中断线号
// 返回：0
// =============================================================================
u32 GMAC_IntHandler(ufast_t IntLine)
{
    u32 isr,rsr,tsr,rxStatusFlag,txStatusFlag;
//  volatile gmac_tx_descriptor_t *pTxTd;

    isr = GMAC->GMAC_ISR;
    rsr = GMAC->GMAC_RSR;
    tsr = GMAC->GMAC_TSR;
    isr &= ~(GMAC->GMAC_IMR | 0xF8030300);

    if((isr & GMAC_ISR_RCOMP) || (rsr & GMAC_RSR_REC))
    {
        rxStatusFlag = GMAC_RSR_REC;

        /* Check OVR */
        if (rsr & GMAC_RSR_RXOVR)
        {
            rxStatusFlag |= GMAC_RSR_RXOVR;
//          GmacStatistics.rx_ovrs++;
        }

        /* Check BNA */
        if (rsr & GMAC_RSR_BNA)
        {
            rxStatusFlag |= GMAC_RSR_BNA;
//          GmacStatistics.rx_bnas++;
        }

        /* Check HNO */
        if (rsr & GMAC_RSR_HNO)
        {
            rxStatusFlag |= GMAC_RSR_HNO;
//          GmacStatistics.rx_ovrs++;
        }
        /* Clear status */
        GMAC->GMAC_RSR |= rxStatusFlag;
        Lock_SempPost(&GMAC_RcvSemp);
    }

    /* TX packet */
    if ((isr & GMAC_ISR_TCOMP) || (tsr & GMAC_TSR_TXCOMP))
    {
        txStatusFlag = GMAC_TSR_TXCOMP;
//      GmacStatistics.tx_comp++;

        /* A frame transmitted */
        /* Check RLE = Retry limit exceeded */
        if (tsr & GMAC_TSR_RLE)
        {
            /* Status RLE & Number of discarded buffers */
            txStatusFlag = GMAC_TSR_RLE
                    | GCIRC_CNT(GtxTd.head, GtxTd.tail,GTX_BUFFERS);
            __GMAC_TxBD_Init();
            GMAC_DBG("Tx RLE!!\n\r");
            GMAC->GMAC_NCR |= GMAC_NCR_TXEN;
//          GmacStatistics.tx_errors++;
        }

        /* Check COL */
        if (tsr & GMAC_TSR_COL)
        {
            txStatusFlag |= GMAC_TSR_COL;
//          GmacStatistics.collisions++;
        }

        /* Check TFC */
        if (tsr & GMAC_TSR_TFC)
        {
            txStatusFlag |= GMAC_TSR_TFC;
//          GmacStatistics.tx_exausts++;
        }
        /* Check UND */
        if (tsr & GMAC_TSR_UND)
        {
            txStatusFlag |= GMAC_TSR_UND;
//          GmacStatistics.tx_underruns++;
        }

        /* Check HRESP */
        if (tsr & GMAC_TSR_HRESP)
        {
            txStatusFlag |= GMAC_TSR_HRESP;
//          GmacStatistics.tx_hresp++;
        }

        if (tsr & GMAC_TSR_RLE)
        {

        }
    }

    // Error Handler
//  if()
//  {
//
//  }

    return 0;
}

// =============================================================================
// 功能：复位PHY，本功能用于热插拔网口时自适应PHY并重新配置网卡，发生PHY LINKUP中断时调用
// 参数：无
// 返回：无
// =============================================================================
void GMAC_PhyIntHandler(void)
{
    if(GMACB_PhyIsLinkUp(&gMacb))
    {
        __GMAC_Init();

        /* Establish ethernet link */
        while(GMAC_OK != ksz8051mnl_set_link(&gMacb, BOARD_GMAC_PHY_ADDR, 1))
        {
            GMAC_DBG("Set link ERROR!\r");
        }
    }
}

#ifndef RAW_GMAC
// =============================================================================
// 功能：以太网模块接收数据包，数据包是字节流。接收数据为如下几个步骤：
//       1.检查是MAC是否有往缓冲区写入数据，即是否有b_ownership == 1；
//       2.查询完整数据帧的首包所在环形BD的位置，即status.bm.b_sof == 1；
//       3.查询结束包的位置status.bm.b_eof，并计算完整数据帧的数据长度；
//       4.分配pkg空间，包大小不超过GMAC_TX_UNITSIZE
//       5.将缓冲区的数据写入pkg，并标记b_ownership，清标志
// 参数：packet,接收到数据的首地址
// 返回：接收到数据包长度，最大不会超过1518字节
// =============================================================================
static tagNetPkg *GMAC_RcvPacket(tagNetDev *NetCard)
{
    tagNetPkg *pkg=NULL;
    u8 LastBuffer=0;
    u8 *rcvbuf = NULL;
    u8 tmpIdx,i;
    u16 totallen = 0,length = 0;
    u8 IsFrame = false;
    volatile gmac_rx_descriptor_t *pRxTd;

    //寻找到第一个包的位置
    for(i = 0;i < GRX_BUFFERS; i++)
    {
        pRxTd = GrxTd.td + GrxTd.idx;
        if(pRxTd->status.bm.b_sof)
            break;
        pRxTd->addr.bm.b_ownership = 0;
        GCIRC_INC(GrxTd.idx, GRX_BUFFERS);      //容错处理
    }

    if(i == GRX_BUFFERS)
    {
        return NULL;
    }

    // 检查是否有数据包
    tmpIdx = GrxTd.idx;
    pRxTd = GrxTd.td + tmpIdx;
    while(pRxTd->addr.bm.b_ownership)
    {
        if(pRxTd->status.bm.b_sof)                  //找到当前帧的首包
        {
            while(GrxTd.idx != tmpIdx)                  //可能两个SOF之间没有EOF
            {
                GCIRC_INC(GrxTd.idx, GRX_BUFFERS);
                pRxTd->addr.bm.b_ownership = 0;
                pRxTd = GrxTd.td + GrxTd.idx;
            }
            IsFrame = FRAME_START_BUF;
        }

        GCIRC_INC(tmpIdx, GRX_BUFFERS);

        if(IsFrame == FRAME_START_BUF)
        {
            if(tmpIdx == GrxTd.idx)         //未找到EOF
            {
                do {
                    pRxTd = GrxTd.td + GrxTd.idx;
                    pRxTd->addr.bm.b_ownership = 0;
                    GCIRC_INC(GrxTd.idx, GRX_BUFFERS);
                } while(tmpIdx != GrxTd.idx);
                break;
            }
            if(pRxTd->status.bm.b_eof)      //找到了一个完整的帧
            {
                IsFrame |= (FRAME_END_BUF);
                totallen = pRxTd->status.bm.len;
                break;
            }
        }
        else
        {
            pRxTd->addr.bm.b_ownership = 0;
            GrxTd.idx = tmpIdx;
        }

        pRxTd = GrxTd.td + tmpIdx;
    }

    //接收到完整的数据帧，则进行数据拷贝
    if( (IsFrame == (FRAME_START_BUF|FRAME_END_BUF)) &&
            (totallen <= CN_PKG_MAX_LEN) && (totallen > 0) )
    {
        pkg =PkgMalloc(totallen,CN_PKLGLST_END);
        if(NULL != pkg)
        {
            rcvbuf = (u8 *)(pkg->buf + pkg->offset);
            pkg->datalen = totallen;
            pkg->partnext= NULL;
        }
        else
        {
            GMAC_DBG("GMAC_RcvPacket() pkg_alloc failed!\r\n ");
        }

        LastBuffer = 0;
        while(!LastBuffer)                             //是否为该Frame的最后一个BD项
        {
            pRxTd = GrxTd.td + GrxTd.idx;
            length = (totallen > GMAC_RX_UNITSIZE) ?
                    GMAC_RX_UNITSIZE : (totallen);

            if( (length > 0) && (NULL != rcvbuf))
            {
               memcpy(rcvbuf,(void *)(pRxTd->addr.val & 0xFFFFFFFC),length);
               rcvbuf = rcvbuf + length;
               totallen -= length;
            }

            LastBuffer = pRxTd->status.bm.b_eof;
            if(pRxTd->status.bm.b_eof)
            {
                pRxTd->status.bm.b_eof = 0;
                pRxTd->status.bm.len = 0;
            }

            if(pRxTd->status.bm.b_sof)
            {
                pRxTd->status.bm.b_sof = 0;
            }
            pRxTd->addr.bm.b_ownership = 0;
            GCIRC_INC(GrxTd.idx, GRX_BUFFERS);
        }
    }

    return pkg;
}

// =============================================================================
// 函数功能：GMAC_SendPacket,以太网底层驱动发送PKG数据包，步骤如下：
//          1.计算当前数据帧由多少个pkg组成；
//          2.计算当前BD Ring有多少空闲BD；
//          3.若空闲BD数目与帧所需的pkg数目足够，则配置BD，并发送；若不够，则等待
//          4.若在指定的时间内未获得足够的bd，则直接返回失败
// 输入参数：dev,使用的网卡设备
//          pkg,待发送的数据包
// 输出参数：
// 返回值  ：true发送成功  false发送失败。
// =============================================================================
static bool_t GMAC_SendPacket(tagNetDev *dev,tagNetPkg * pkg, u32 netdevtask)
{
    bool_t  result = false;
    tagNetPkg *tmp;
    u32 pkgnum = 0,temp = 0,i;
    u32 SendBdWaitTime = SendPkgWaitMs;
    volatile gmac_tx_descriptor_t *pTxTd;
    u16 totallen=0;
    u8 *SendBuf;

    if((NULL == dev)||(NULL == pkg))
        return result;

    pkgnum = 1;
    //查询是否有足够多的BD表
    do
    {
        temp = GCIRC_SPACE(GtxTd.head, GtxTd.tail, GTX_BUFFERS);
        if(temp >= pkgnum)
            break;
        Djy_EventDelay(1*mS);
    }while(SendBdWaitTime-- > 0);

    if(SendBdWaitTime == 0)
        return result;
    //计算一个完整数据包由多少个pkg组成
    tmp = pkg;
    SendBuf = (u8*)TxBuffer;
    while(NULL != tmp)
    {
        if(totallen <= GMAC_TX_UNITSIZE)
        {
            memcpy(&SendBuf[totallen],(void*)(tmp->buf+tmp->offset),tmp->datalen);
            totallen += tmp->datalen;
            tmp = tmp->partnext;
        }
    }

    //到这一步，说明有足够多的空闲BD发送该帧数据
    for (i = 0; i < pkgnum; i++)
    {
        /* Pointers to the current TxTd */
        pTxTd = GtxTd.td + GtxTd.head;

        pTxTd->status.bm.b_used = 0;
        pTxTd->status.bm.len = totallen & 0x3FFF;
        pTxTd->addr = (u32)SendBuf;//(u32)(tmp->buf + tmp->offset);
        if(i == pkgnum - 1)
        {
            // 当前帧的最后一包
            pTxTd->status.bm.b_last_buffer = 1;
        }

        GCIRC_INC(GtxTd.head, GTX_BUFFERS);
        tmp = tmp->partnext;
    }

    // Indicate that Descriptors are ready to transmit
    GMAC->GMAC_NCR |= GMAC_NCR_TSTART;

    SendBdWaitTime = SendPkgWaitMs;
    // 暂时，发送完成再返回
    do
    {
        if((GMAC->GMAC_TSR & GMAC_TSR_TXCOMP) )
        {
            break;
        }
        Djy_EventDelay(1*mS);
    }while(SendBdWaitTime-- > 0);

    pTxTd = GtxTd.td + GtxTd.tail;
    if (pTxTd->status.bm.b_used ==1)    //发送完时，GMAC会将b_used设置为1
    {
        GCIRC_INC( GtxTd.tail, GTX_BUFFERS );
    }

    return result = true;
}

// =============================================================================
// 功能：接收事件的函数，阻塞等待接收到完整数据包的信号量，并接收读取完网卡缓存的所有数据包
// 参数：无
// 返回：无
// =============================================================================
static ptu32_t GMAC_Rcv(void)
{
    tagNetPkg *pkg;
    tagNetDev *NetCard = ptNetDev;

//    Djy_GetEventPara(NetCard,NULL);
    while(1)
    {
        Lock_SempPend(&GMAC_RcvSemp,CN_TIMEOUT_FOREVER);
        while((pkg = GMAC_RcvPacket(NetCard))!= NULL)//不断读网卡直到没有完整数据包为止
        {
            NetDev_PostPkg(NetCard,pkg);
            PkgTryFreePart(pkg);
        }
    }
    return 0;
}

// =============================================================================
// 功能：GMAC网卡接收数据的任务
// 参数：网卡
// 返回：true
// =============================================================================
static bool_t GMAC_RcvTask(tagNetDev *NetCard)
{
    bool_t result = false;
    u16 evttID;
    u16 eventID;

    evttID = Djy_EvttRegist(EN_CORRELATIVE, CN_PRIO_CRITICAL, 0, 1,
        (ptu32_t (*)(void))GMAC_Rcv,NULL, 0x1000, "GMACRcvTask");
    if (evttID != CN_EVTT_ID_INVALID)
    {
        eventID=Djy_EventPop(evttID, NULL,  0,(ptu32_t)NetCard, 0, 0);
        if(eventID != CN_EVENT_ID_INVALID)
        {
            result = true;
            Lock_SempCreate_s(&GMAC_RcvSemp,1,0,CN_SEMP_BLOCK_FIFO,NULL);
        }
        else
        {
            Djy_EvttUnregist(evttID);
        }
    }
    return result;
}

// =============================================================================
// 函数功能：NetHard_AddNetDev
//          向协议栈添加一个网卡设备
// 输入参数：无
// 输出参数：无
// 返回值  ：true成功  false失败。
// 说明    ：1,添加设备
//        2,添加路由
// =============================================================================
tagNetDev *GMAC_AddNetDev(void)
{
    tagNetDevPara  devpara;
    tagHostIpv4Addr devaddr;

    devpara.ifsend = GMAC_SendPacket;
    devpara.iftype = EN_LINK_INTERFACE_ETHERNET;
    devpara.devfunc = 0x00;//0xFF;//所有属性都配置上
    memcpy(devpara.mac, sgNetHardMac,6);
    devpara.name = "SAM4E_MAC";
    devpara.private = 0;
    devpara.linklen = 14;
    devpara.pkglen = 1500;

    ptNetDev = NetDev_AddDev(&devpara);
    if(ptNetDev != NULL)
    {
        devaddr.ip = sgNetHardIpAddrMain;
        devaddr.gateway = sgNetHardGateWay;
        devaddr.ipmsk = sgNetHardIpMsk;
        ptNetRout = Rout_AddRout(ptNetDev, &devaddr);
        if(NULL != ptNetRout)
        {
            Dhcp_AddTask(ptNetRout);
        }
        if(false == GMAC_RcvTask(ptNetDev))
        {
            printk("%s:Create Rcv Task Failed\n\r",__FUNCTION__);
        }

    }
// NetDev_AddDev，Rout_AddRout，Dhcp_AddTask这些函数执行失败怎么办，这里都没有处理
    return ptNetDev;
}

// =============================================================================
// 功能：GMAC网卡和DJYIP驱动初始化函数
// 参数：para
// 返回值  ：true成功  false失败。
// =============================================================================
bool_t ModuleInstall_GMAC(ptu32_t para)
{
    bool_t result = false;

    __GMAC_PortInit();                               // PORT初始化
    __GMAC_Init();                                  // MAC初始化
    __PHY_Init();                                   // PHY初始化
    __GMAC_IntInit();                               // INT SYS初始化

    if(NULL == GMAC_AddNetDev())
        result = false;

    return result;
}
#else
// =============================================================================
// ============================= FUNCTIONS API FOR RAW =========================
// =============================================================================

static u8 RecvBuffer[1500];
static bool_t sGmacIsInited = false;
// =============================================================================
// Function: GMAC API FOR SEND PACKET DIRECTLY BY GMAC, NO DJYIP STACK
// PARAM: BUF,THE BUFFER OF DATE TO BE SENDING
//        LEN，THE LENGTH OF BYTES TO BE SENDING
// RETURN: TRUE,SUCCESSED; OTHERWISE,FAILED
// NOTE: return after packet be sent.
// =============================================================================
bool_t GMAC_Send(u8 *buf,u32 len)
{
    u32 pkgnum = 0,temp = 0,i;
    u32 SendBdWaitTime = SendPkgWaitMs;
    volatile gmac_tx_descriptor_t *pTxTd;
    u16 totallen=len;
    u8 *SendBuf;

    if(sGmacIsInited == false)
        return false;

    if( (NULL == buf) || (len == 0) || ( len > GMAC_TX_UNITSIZE))
        return false;

    pkgnum = 1;
    //查询是否有足够多的BD表
    do
    {
        temp = GCIRC_SPACE(GtxTd.head, GtxTd.tail, GTX_BUFFERS);
        if(temp >= pkgnum)
            break;
        Djy_EventDelay(1*mS);
    }while(SendBdWaitTime-- > 0);

    SendBuf = (u8*)TxBuffer;
    if(totallen <= GMAC_TX_UNITSIZE)
    {
        memcpy(SendBuf,(void*)buf,len);
    }
    else
        return false;

    //到这一步，说明有足够多的空闲BD发送该帧数据
    for (i = 0; i < pkgnum; i++)
    {
        /* Pointers to the current TxTd */
        pTxTd = GtxTd.td + GtxTd.head;

        pTxTd->status.bm.b_used = 0;
        pTxTd->status.bm.len = totallen & 0x3FFF;
        pTxTd->addr = (u32)SendBuf;//(u32)(tmp->buf + tmp->offset);
        if(i == pkgnum - 1)
        {
            // 当前帧的最后一包
            pTxTd->status.bm.b_last_buffer = 1;
        }

        GCIRC_INC(GtxTd.head, GTX_BUFFERS);
    }

    // Indicate that Descriptors are ready to transmit
    GMAC->GMAC_NCR |= GMAC_NCR_TSTART;
    SendBdWaitTime = SendPkgWaitMs;
    // 暂时，发送完成再返回
    do
    {
        if((GMAC->GMAC_TSR & GMAC_TSR_TXCOMP) )
        {
            break;
        }
        Djy_EventDelay(1*mS);
    }while(SendBdWaitTime-- > 0);

    pTxTd = GtxTd.td + GtxTd.tail;
    if (pTxTd->status.bm.b_used ==1)    //发送完时，GMAC会将b_used设置为1
    {
        GCIRC_INC( GtxTd.tail, GTX_BUFFERS );
    }
    return true;
}

// =============================================================================
// Function: GMAC API FOR RECV PACKET DIRECTLY BY GMAC, NO DJYIP STACK
// PARAM: *LEN，THE LENGTH OF BYTES RECEIVED
// RETURN: TRUE,SUCCESSED; OTHERWISE,FAILED
// NOTE: return after packet be sent.
// =============================================================================
bool_t GMAC_Recv(u32 *len)
{
    u8 *rcvbuf = NULL;
    u8 tmpIdx,i,result = false,IsFrame = false,LastBuffer=0;
    u16 totallen = 0,length = 0;
    volatile gmac_rx_descriptor_t *pRxTd;

    if(sGmacIsInited == false)
        return false;

    //寻找到第一个包的位置
    for(i = 0;i < GRX_BUFFERS; i++)
    {
        pRxTd = GrxTd.td + GrxTd.idx;
        if(pRxTd->status.bm.b_sof)
            break;
        pRxTd->addr.bm.b_ownership = 0;
        GCIRC_INC(GrxTd.idx, GRX_BUFFERS);      //容错处理
    }

    if(i == GRX_BUFFERS)
    {
        return result;
    }

    // 检查是否有数据包
    tmpIdx = GrxTd.idx;
    pRxTd = GrxTd.td + tmpIdx;
    while(pRxTd->addr.bm.b_ownership)
    {
        if(pRxTd->status.bm.b_sof)                  //找到当前帧的首包
        {
            while(GrxTd.idx != tmpIdx)                  //可能两个SOF之间没有EOF
            {
                GCIRC_INC(GrxTd.idx, GRX_BUFFERS);
                pRxTd->addr.bm.b_ownership = 0;
                pRxTd = GrxTd.td + GrxTd.idx;
            }
            IsFrame = FRAME_START_BUF;
        }

        GCIRC_INC(tmpIdx, GRX_BUFFERS);

        if(IsFrame == FRAME_START_BUF)
        {
            if(tmpIdx == GrxTd.idx)         //未找到EOF
            {
                do {
                    pRxTd = GrxTd.td + GrxTd.idx;
                    pRxTd->addr.bm.b_ownership = 0;
                    GCIRC_INC(GrxTd.idx, GRX_BUFFERS);
                } while(tmpIdx != GrxTd.idx);
                break;
            }
            if(pRxTd->status.bm.b_eof)      //找到了一个完整的帧
            {
                IsFrame |= (FRAME_END_BUF);
                totallen = pRxTd->status.bm.len;
                break;
            }
        }
        else
        {
            pRxTd->addr.bm.b_ownership = 0;
            GrxTd.idx = tmpIdx;
        }

        pRxTd = GrxTd.td + tmpIdx;
    }

    //接收到完整的数据帧，则进行数据拷贝
    if( (IsFrame == (FRAME_START_BUF|FRAME_END_BUF)) &&
            (totallen <= CN_PKG_MAX_LEN) && (totallen > 0) )
    {
        rcvbuf = RecvBuffer;
        LastBuffer = 0;
        while(!LastBuffer)                             //是否为该Frame的最后一个BD项
        {
            pRxTd = GrxTd.td + GrxTd.idx;
            length = (totallen > GMAC_RX_UNITSIZE) ?
                    GMAC_RX_UNITSIZE : (totallen);

            if( (length > 0) && (NULL != rcvbuf))
            {
               memcpy(rcvbuf,(void *)(pRxTd->addr.val & 0xFFFFFFFC),length);
               rcvbuf = rcvbuf + length;
               totallen -= length;
            }

            LastBuffer = pRxTd->status.bm.b_eof;
            if(pRxTd->status.bm.b_eof)
            {
                pRxTd->status.bm.b_eof = 0;
                pRxTd->status.bm.len = 0;
            }

            if(pRxTd->status.bm.b_sof)
            {
                pRxTd->status.bm.b_sof = 0;
            }
            pRxTd->addr.bm.b_ownership = 0;
            GCIRC_INC(GrxTd.idx, GRX_BUFFERS);
        }
        result = true;
    }

    *len = totallen;

    return result;
}
u8* GMAC_ReceiveData(void)
{
    u32 len;
    Lock_SempPend(&GMAC_RcvSemp,CN_TIMEOUT_FOREVER);
    if(true == GMAC_Recv(&len))
    {
        //调用显示用的函数
        //数据放在  RecvBuffer
        return RecvBuffer;
    }
    return NULL;
}

// =============================================================================
// 功能：GMAC网卡和DJYIP驱动初始化函数
// 参数：para
// 返回值  ：true成功  false失败。
// =============================================================================
bool_t ModuleInstall_GMAC(ptu32_t para)
{
    bool_t result = false;

    __GMAC_PortInit();                               // PORT初始化
    __GMAC_Init();                                  // MAC初始化
    __PHY_Init();                                   // PHY初始化
    __GMAC_IntInit();                               // INT SYS初始化

    sGmacIsInited = true;
    Lock_SempCreate_s(&GMAC_RcvSemp,1,0,CN_SEMP_BLOCK_FIFO,NULL);
    return result;
}

#endif
#endif
