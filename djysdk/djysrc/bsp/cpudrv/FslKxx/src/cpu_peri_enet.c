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
// 文件名     ：cpu_peri_enet.c
// 模块描述: ENET模块的初始化的操作，包含PHY初始化和MII初始化
//           按DJYOS的以太网协议实现要求实现的网卡底层驱动
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 28/07.2014
// =============================================================================
#if 1
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>

#include "cpu_peri.h"

// =============================================================================
#define CN_SYS_CLK_MHZ          120         // 系统时钟频率MHZ
#define CN_PHY_ADDR             0x01        // PHY的物理地址
#define CN_PHY_IDR1             0X02        // PHY的ID寄存器1
#define CN_PHY_IDR2             0X03        // PHY的ID寄存器2

#define MII_TIMEOUT             0x1FFFF     // MII超时时间参数（非精准）
#define MII_LINK_TIMEOUT        0x1FFFF     // MII的LINK超时参数（非精准）

#if(DEBUG == 1)
#define ENET_DBG   printk
#else
#define ENET_DBG
#endif
// =============================================================================
// 定义MAC参数配置，默认配置为RMII，自适应，100M，全双工
static tagEnetCfg s_EnetConfig = {"ENET",MAC_RMII,AUTONEG_ON,MII_100BASET,
                                    MII_FDX,NO_LOOPBACK,EXTERNAL_NONE,
                                    0,{0x00,0x01,0x02,0x03,0x04,0x02}};

#define CN_PKG_MAX_LEN  1522
// =============================================================================
static struct SemaphoreLCB *pEnetRcvSync;
static struct MutexLCB     *pEnetSndSync;
static ptu32_t              gEnetHandle = NULL;
// 该指针初始化到按16字节对齐的BD表
#ifdef __GNUC__
static u8 txbd[(sizeof(NBUF) * NUM_TXBDS)] __attribute__ ((aligned(16)));
static u8 rxbd[(sizeof(NBUF) * NUM_RXBDS)] __attribute__ ((aligned(16)));
static u8 RxBuffer[(RX_BUFFER_SIZE * NUM_RXBDS)] __attribute__ ((aligned(16)));
static u8 TxBuffer[(TX_BUFFER_SIZE * NUM_TXBDS)] __attribute__ ((aligned(16)));
#else
#error "bd must be 16-byte aligned"
#endif

// 该指针初始化到按16字节对齐的BD表
static NBUF *spTxBdTab;
static NBUF *spRxBdTab;

// 标识下一个BD表标识，即下一个可用的BD表
static int sNextTxBd=0;
static int sNextRxBd=0;

// =============================================================================
// 功能：将32位value高低字节转换，主要用于大小端变换
// 参数：value,转换值
// 返回：result,转换结果
// =============================================================================
static u32 ENET_REV(u32 value)
{
  u32 result;

  __asm__ volatile ("rev %0, %1" : "=r" (result) : "r" (value) );

  return(result);
}

// =============================================================================
// 功能：将16位的value高低字节转换，主要用于大小端变换
// 参数：value,转换值
// 返回：result,转换结果
// =============================================================================
static int32_t ENET_REVSH(int32_t value)
{
  u32 result;

  __asm__ volatile ("revsh %0, %1" : "=r" (result) : "r" (value) );

  return(result);
}

// =============================================================================
// 功能：MII串行接口的时钟初始化，MDC的时钟频率该值必须低于2.5MHz，MSCR_MII_SPEED的
//       计算公式为 25 MHz / ((4 + 1) x 2) = 2.5 MHz
// 参数：value,转换值
// 返回：result,转换结果
// =============================================================================
static void __MII_Init(int sys_clk_mhz)
{
    ENET->MSCR = 0
#ifdef TSIEVB/*TSI EVB requires a longer hold time than default 10 ns*/
                      | ENET_MSCR_HOLDTIME(2)
#endif
                      | ENET_MSCR_MII_SPEED((2*sys_clk_mhz/5)+1)
                      ;
}

// =============================================================================
// 功能：MII接口写函数，通过MDIO与PHY芯片通信
// 参数：phy_addr, PHY地址
//       reg_addr,寄存器地址
//       data,写入的数据
// 返回：0,正常返回；1,失败
// =============================================================================
static u32 __MII_Write(u32 phy_addr, u32 reg_addr, u32 data)
{
    int timeout;

    /* Clear the MII interrupt bit */
    ENET->EIR = ENET_EIR_MII_MASK;

    /* Initiatate the MII Management write */
    ENET->MMFR = ENET_MMFR_ST(0x01)
                | ENET_MMFR_OP(0x01)
                | ENET_MMFR_PA(phy_addr)
                | ENET_MMFR_RA(reg_addr)
                | ENET_MMFR_TA(0x02)
                | ENET_MMFR_DATA(data);

    /* Poll for the MII interrupt (interrupt should be masked) */
    for (timeout = 0; timeout < MII_TIMEOUT; timeout++)
    {
        if (ENET->EIR & ENET_EIR_MII_MASK)
            break;
    }

    if(timeout == MII_TIMEOUT)
        return 1;

    /* Clear the MII interrupt bit */
    ENET->EIR = ENET_EIR_MII_MASK;

    return 0;
}

// =============================================================================
// 功能：MII接口读数据，通过MDIO接口与PHY芯片通信
// 参数：phy_addr, PHY地址
//       reg_addr,寄存器地址
//       data,读取到的数据指针
// 返回：0,正常返回；1,失败
// =============================================================================
static u32 __MII_Read(u32 phy_addr, u32 reg_addr, u32 *data)
{
    int timeout;

    /* Clear the MII interrupt bit */
    ENET->EIR = ENET_EIR_MII_MASK;

    /* Initiatate the MII Management read */
    ENET->MMFR = ENET_MMFR_ST(0x01)
                | ENET_MMFR_OP(0x2)
                | ENET_MMFR_PA(phy_addr)
                | ENET_MMFR_RA(reg_addr)
                | ENET_MMFR_TA(0x02);

    /* Poll for the MII interrupt (interrupt should be masked) */
    for (timeout = 0; timeout < MII_TIMEOUT; timeout++)
    {
        if (ENET->EIR & ENET_EIR_MII_MASK)
            break;
    }

    if(timeout == MII_TIMEOUT)
        return 1;

    /* Clear the MII interrupt bit */
    ENET->EIR = ENET_EIR_MII_MASK;

    *data = ENET->MMFR & 0x0000FFFF;

    return 0;
}

// =============================================================================
// 功能：复位PHY芯片，需调用MII接口函数
// 参数：phy_addr, PHY地址
// 返回：0,正常返回；1,失败
// =============================================================================
static int __PHY_Reset(int phy_addr)
{
#if MII_CHECK_TIMEOUT
    int timeout;
#endif
    u32 settings;

    /* Reset the PHY */
    if (__MII_Write(phy_addr, PHY_BMCR, PHY_BMCR_RESET))
        return 1;
    /* Wait for reset to complete */
#if MII_CHECK_TIMEOUT
    for (timeout = 0; timeout < MII_LINK_TIMEOUT; ++timeout)
#endif
    while(1)
    {
      /* Read back the contents of the CTRL register and verify
       * that RESET is not set - this is a sanity check to ensure
       * that we are talking to the PHY correctly. RESET should
       * always be cleared. */
      if (!(__MII_Read(phy_addr, PHY_BMCR, &settings)) && !(settings & PHY_BMCR_RESET))
          break;
    }
#if MII_CHECK_TIMEOUT
    if (timeout == MII_LINK_TIMEOUT || (settings & PHY_BMCR_RESET))
        return 1;
    else
#endif
        return 0;
}

// =============================================================================
// 功能：利用hash算法计算CRC值，取高8位作为返回值
// 参数：addr, MAC地址指针
// 返回：CRC，哈希算法计算的CRC值高8比特值
// =============================================================================
static u8 __Enet_HashAddr(const u8* addr)
{
    u32 crc;
    u8 byte;
    int i, j;

    crc = 0xFFFFFFFF;
    for(i=0; i<6; ++i)
    {
        byte = addr[i];
        for(j=0; j<8; ++j)
        {
            if((byte & 0x01)^(crc & 0x01))
            {
                crc >>= 1;
                crc = crc ^ 0xEDB88320;
            }
            else
                crc >>= 1;
            byte >>= 1;
        }
    }
    return (u8)(crc >> 26);
}

// =============================================================================
// 功能：MAC地址配置和地址hash表配置，取计算的哈希CRC最高的6比特
// 参数：pa,MAC地址数据指针
// 返回：无
// =============================================================================
static void __Enet_MAC_AddrSet (const u8 *pa)
{
    u8 crc;

    // Set the Physical Address
    ENET->PALR = (u32)((pa[0]<<24) | (pa[1]<<16) | (pa[2]<<8) | pa[3]);
    ENET->PAUR = (u32)((pa[4]<<24) | (pa[5]<<16));

    // Calculate and set the hash for given Physical Address
    // in the  Individual Address Hash registers
    crc = __Enet_HashAddr(pa);
    if(crc >= 32)
        ENET->IAUR |= (u32)(1 << (crc - 32));
    else
        ENET->IALR |= (u32)(1 << crc);
}

// =============================================================================
// 功能：Buffer Descriptor初始化，用于发送和接收数据，根据手册，BD表必须按16字节对齐
//       并且data指针所指向的地址也必须是按16字节对齐，地址和length需进行大小端转换
// 参数：无
// 返回：无
// =============================================================================
static void __Enet_BD_Init(void)
{
    int i;

    sNextTxBd = 0;
    sNextRxBd = 0;

    spTxBdTab = (NBUF *)(((u32)(txbd)) & 0xFFFFFFF0);
    spRxBdTab = (NBUF *)(((u32)(rxbd)) & 0xFFFFFFF0);

    // Initialize transmit descriptor ring
    for (i = 0; i < NUM_TXBDS; i++)
    {
        spTxBdTab[i].status = 0x0000;
        spTxBdTab[i].length = 0;
        spTxBdTab[i].data = (u8 *)ENET_REV((u32)&TxBuffer[i * TX_BUFFER_SIZE]);
    }

    // Initialize receive descriptor ring
    for (i = 0; i < NUM_RXBDS; i++)
    {
        spRxBdTab[i].status = RX_BD_E;
        spRxBdTab[i].length = 0;
        spRxBdTab[i].data = (u8 *)ENET_REV((u32)&RxBuffer[i * RX_BUFFER_SIZE]);
    }

    // Set the Wrap bit on the last one in the ring
    spRxBdTab[NUM_RXBDS - 1].status |= RX_BD_W;
    spTxBdTab[NUM_TXBDS - 1].status |= TX_BD_W;

    // Set Receive Buffer Size
    ENET->MRBR = (uint16_t)RX_BUFFER_SIZE;

    // Point to the start of the Tx buffer descriptor queue
    ENET->TDSR = (u32)spTxBdTab;
    // Point to the start of the circular Rx buffer descriptor queue
    ENET->RDSR = (u32)spRxBdTab;
}

// =============================================================================
// 功能：Enet的PORT口初始化，主要是对时钟的使能，MPU失能，和GPIO配置，根据RMII和MII
//       分别配置不同的IO引脚功能
// 参数：无
// 返回：无
// =============================================================================
static void Enet_PortInit(void)
{
    // 使能ENET时钟源
    SIM->SCGC2 |= SIM_SCGC2_ENET_MASK;

    /*FSL: allow concurrent access to MPU controller.
     * Example: ENET uDMA to SRAM, otherwise bus error*/
    // MPU失能，因为ENET需要将uDMA中的数据移到SRAM，如果使能了
    // MPU，将导致总线错误
    MPU->CESR = 0;

    // 配置MDC和MDIO的port口，与PHY通信
    PORTB->PCR[0]   = (PORTB->PCR[0]  & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4)
                      | PORT_PCR_ODE_MASK;
    PORTB->PCR[1]   = (PORTB->PCR[1]  & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);

    // 配置使用的接口方式，可配置为RMII和MII
    #ifdef CN_USE_MII_INTERFACE
    PORTA->PCR[12]  = (PORTA->PCR[12] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);
    PORTA->PCR[13]  = (PORTA->PCR[13] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);
    PORTA->PCR[14]  = (PORTA->PCR[14] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);
    PORTA->PCR[15]  = (PORTA->PCR[15] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);
    PORTA->PCR[16]  = (PORTA->PCR[16] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);
    PORTA->PCR[17]  = (PORTA->PCR[17] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);

    PORTA->PCR[11] = (PORTA->PCR[11] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);
    PORTA->PCR[25] = (PORTA->PCR[25] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);
    PORTA->PCR[9]  = (PORTA->PCR[9] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);
    PORTA->PCR[10] = (PORTA->PCR[10] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);
    PORTA->PCR[28] = (PORTA->PCR[28] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);
    PORTA->PCR[24] = (PORTA->PCR[24] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);
    PORTA->PCR[26] = (PORTA->PCR[26] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);
    PORTA->PCR[27] = (PORTA->PCR[27] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);
    PORTA->PCR[29] = (PORTA->PCR[29] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);
    #else
    PORTA->PCR[12]  = (PORTA->PCR[12] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);
    PORTA->PCR[13]  = (PORTA->PCR[13] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);
    PORTA->PCR[14]  = (PORTA->PCR[14] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);
    PORTA->PCR[15]  = (PORTA->PCR[15] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);
    PORTA->PCR[16]  = (PORTA->PCR[16] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);
    PORTA->PCR[17]  = (PORTA->PCR[17] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);
    #endif
}

// =============================================================================
// 功能：Enet中断初始化，使用中断的方式接收数据，用于配置中断寄存器
// 参数：无
// 返回：无
// =============================================================================
static void Enet_IntInit(ufast_t IntLine,u32 (*isr)(ufast_t))
{
    if(true == Int_Register(IntLine))
    {
        Int_Register(IntLine);
        Int_IsrConnect(IntLine,isr);
        Int_SettoAsynSignal(IntLine);
        Int_ClearLine(IntLine);
        Int_ContactLine(IntLine);
    }
}

// =============================================================================
// 功能：PHY芯片初始化，本函数使用自适应方式，通过读取PHY芯片中SPEED和双工的相关寄存
//       器，来获得配置MAC的相关参数
// 参数：无
// 返回：true,link成功；false,link失败
// =============================================================================
static bool_t PHY_Init(void)
{
    u32 usData,NegTimes = 100;

    __PHY_Reset(CN_PHY_ADDR);
    ENET_DBG("PHY RESET DONE!\r\n");
    // MII初始化
    __MII_Init(CN_SYS_CLK_MHZ);

    /* Can we talk to the PHY? */
//    do
//    {
//      Djy_DelayUs(500);
//      usData = 0xFFFF;
//      __MII_Read( CN_PHY_ADDR, CN_PHY_IDR1, &usData );
//    } while( usData == 0xFFFF );
    ENET_DBG("START TO NEGOTIATION!\r\n");
    /* Start auto negotiate. */
    __MII_Write(CN_PHY_ADDR, PHY_BMCR,PHY_BMCR_AN_ENABLE | PHY_BMCR_AN_RESTART);

    /* Wait for auto negotiate to complete. */
    do
    {
        Djy_DelayUs( 50000);
      __MII_Read( CN_PHY_ADDR, PHY_BMSR, &usData );
      NegTimes--;
    } while(( !( usData & PHY_BMSR_AN_COMPLETE ) ) && (NegTimes > 0));

    if(NegTimes == 0)
    {
        ENET_DBG("NEGOTIATION FAILED, PLEASE CHECK THE CONNECTION!\r\n");
    }

    /* When we get here we have a link - find out what has been negotiated. */
    usData = 0;
    __MII_Read(CN_PHY_ADDR, PHY_BMSR, &usData );
    ENET_DBG("START TO LINK!\r\n");
    // Is Linked ?
    if(usData & PHY_BMSR_LINK)
    {
        // Get the speed
        if ((usData & PHY_BMSR_100BT4)     ||
            (usData & PHY_BMSR_100BTX_FDX) ||
            (usData & PHY_BMSR_100BTX))
            s_EnetConfig.speed = MII_100BASET;
        else
            s_EnetConfig.speed = MII_10BASET;
        // Get the duplex
        if ((usData & PHY_BMSR_100BTX_FDX) ||
            (usData & PHY_BMSR_10BT_FDX))
            s_EnetConfig.duplex = MII_FDX;
        else
            s_EnetConfig.duplex = MII_HDX;
        return true;
    }
    ENET_DBG("PHY INIT DONE!\r\n");
    return false;
}


// =============================================================================
// 功能：FSL以太网模块MAC初始化，主要是MAC寄存器和BD表的初始化，下几个步骤：
//       1.复位以太网模块，主要将相关的寄存器复位，复位需等待一定时间；
//       2.初始化BD表，配置TCR和RCR，即发送接收功能；
//       3.MAC地址初始化，写MAC到寄存器，可配置为是否强制替换发送包中的源地址；
//       4.使能ENET。
// 参数：无
// 返回：无
// =============================================================================
static void Enet_MAC_Init(void)
{
    int i;

    // Set the Reset bit and clear the Enable bit
    ENET->ECR = ENET_ECR_RESET_MASK;

    // Wait at least 8 clock cycles
    for (i=0; i<10; ++i)
        asm( "NOP" );

    __Enet_BD_Init();                   // Init BD表

    // Clear the Individual and Group Address Hash registers
    ENET->IALR = 0;
    ENET->IAUR = 0;
    ENET->GALR = 0;
    ENET->GAUR = 0;

    __Enet_MAC_AddrSet(s_EnetConfig.mac);   // Set the Physical Address
    ENET->EIMR = ENET_EIMR_RXF_MASK         // Enable RX and err interrupts
                | ENET_EIMR_BABR_MASK
                | ENET_EIMR_BABT_MASK
                | ENET_EIMR_EBERR_MASK
                | ENET_EIMR_UN_MASK;
    ENET->EIR = 0xFFFFFFFF;                 // Clear all FEC interrupt events

    // Initialize the Receive Control Register
    ENET->RCR = ENET_RCR_MAX_FL(ETH_MAX_FRM)
//                | ENET_RCR_BC_REJ_MASK        // 不接收广播帧
                | ENET_RCR_MII_MODE_MASK    // always
                //| ENET_RCR_CRCFWD_MASK;   // no CRC pad required
                ;

    if ( s_EnetConfig.interface == MAC_RMII )
    {
        ENET->RCR |= ENET_RCR_RMII_MODE_MASK;
        if( s_EnetConfig.speed == MII_10BASET ) // only set speed in RMII mode
        {
            ENET->RCR |= ENET_RCR_RMII_10T_MASK;
        }
    }
    // no need to configure MAC MII interface
    ENET->TCR = 0;
    // config the duplex
    if (s_EnetConfig.duplex == MII_HDX)
    {
        ENET->RCR |= ENET_RCR_DRT_MASK;
        ENET->TCR &= (u32)~ENET_TCR_FDEN_MASK;
    }
    else
    {
        ENET->RCR &= ~ENET_RCR_DRT_MASK;
        ENET->TCR |= ENET_TCR_FDEN_MASK;
    }

    if (s_EnetConfig.prom)
    {
        ENET->RCR |= ENET_RCR_PROM_MASK;        // All frames are accepted
    }

#ifdef ENHANCED_BD
    ENET_ECR = ENET_ECR_EN1588_MASK;
#else
    ENET->ECR = 0;                          //clear register
#endif

    if(s_EnetConfig.loopback == INTERNAL_LOOPBACK)
    {
        // seems like RMII internal loopback works,
        // even if it's not supported
        ENET->RCR |= ENET_RCR_LOOP_MASK;
    }

    // Enable FEC
    ENET->ECR |= ENET_ECR_ETHEREN_MASK;
    // Enable RX
    ENET->RDAR = ENET_RDAR_RDAR_MASK;
}

// =============================================================================
// 功能：FSL以太网模块接收数据包，数据包是字节流。接收数据为如下几个步骤：
//       1.BD表为环形表，首先判断是否接收到数据，即BD表是否为空；
//       2.读BD表中的所有数据，直到RX_BD_L标号，即该帧最后一包；
//       3.更新BD表，并重新使能接收数据；
// 参数：packet,接收到数据的首地址
// 返回：接收到数据包长度，最大不会超过1518字节
// =============================================================================
static tagNetPkg *__Enet_RcvPacket(ptu32_t handle)
{
    tagNetPkg *pkg=NULL;
    int LastBuffer,length,len,RxBdIndex;
    u16 status;
    u8 *rcvbuf = NULL;
    atom_low_t atomop;

    LastBuffer = 0;
    length = 0;

    RxBdIndex = sNextRxBd;
    atomop = Int_LowAtomStart();
    if(spRxBdTab[RxBdIndex].status & RX_BD_E)             //BD表是否为空
    {
        Int_LowAtomEnd(atomop);
        return NULL;
    }
    Int_LowAtomEnd(atomop);

    //计算接收到数据包有几个BD表，todo----须做容错处理，如没有结束符
    while(!LastBuffer)
    {
        length = ENET_REVSH(spRxBdTab[RxBdIndex].length);
        status = spRxBdTab[RxBdIndex].status;
        if(status & RX_BD_W)
        {
            RxBdIndex = 0;
        }
        else
        {
            RxBdIndex++;
        }
        LastBuffer = (status & RX_BD_L);
    }

    if(length >0)
    {
        pkg =PkgMalloc(length,CN_PKLGLST_END);
        if(NULL != pkg)
        {
            rcvbuf = (u8 *)(pkg->buf + pkg->offset);
            pkg->datalen = length;
            pkg->partnext= NULL;
        }
        else
        {
            ENET_DBG("Enet_RcvPacket() pkg_alloc failed!\r\n ");//todo
        }

    }

    LastBuffer = 0;
    RxBdIndex = sNextRxBd;
    while(!LastBuffer)                             //是否为该Frame的最后一个BD项
    {
        status = spRxBdTab[RxBdIndex].status;
        len = ENET_REVSH(spRxBdTab[RxBdIndex].length);
        len = (length > len) ? len : length;
        length -= len;

        //--TODO, this code has something err, you could't memcpy when
        //        there is no pkg for you~!
        if(len > 0)
        {
            memcpy(rcvbuf,(uint8_t *)ENET_REV((u32)spRxBdTab[RxBdIndex].data),len);
            rcvbuf = rcvbuf + len;
        }
        if(status & RX_BD_W)
        {
            spRxBdTab[RxBdIndex].status = (RX_BD_W | RX_BD_E);
            RxBdIndex = 0;
        }
        else
        {
            spRxBdTab[RxBdIndex].status = RX_BD_E;
            RxBdIndex++;
        }
        LastBuffer = (status & RX_BD_L);
    }

    sNextRxBd = RxBdIndex;                     // 更新读NEXT BD表标志
    ENET->RDAR = ENET_RDAR_RDAR_MASK;           // 更新RX BD列表
    return pkg;
}

// =============================================================================
// 功能：FSL以太网的MAC层发生中断服务函数，包括接收整包数据、发送完整包数据和发生错误
// 参数：int_line,中断线号
// 返回：0
// =============================================================================
u32 Enet_RxISR(ufast_t IntLine)
{
    u32 event;

    event = ENET->EIR;         // 获取中断寄存器
    ENET->EIR = ENET_EIR_RXF_MASK;                      // 清中断标志位
    if(event & ENET_EIR_RXF_MASK)
    {
        Lock_SempPost(pEnetRcvSync);
    }
    return 0;
}

u32 Enet_TxISR(ufast_t IntLine)
{
    return 0;
}

u32 Enet_ErrISR(ufast_t IntLine)
{
    u32 event;

    event = ENET->EIR;                      // 获取中断寄存器
    ENET->EIR = event;                      // 清中断标志位

    ENET_DBG("ENET error occurred, event = %8x !\r\n" ,event);

    return 0;
}

// =============================================================================
// 函数功能：Enet_SendPacket,以太网底层驱动发送PKG数据包，步骤如下：
//          1.释放已经发送完毕的bd所使用的pkg；
//          2.计算当前数据帧由多少个pkg组成；
//          3.计算当前BD Ring有多少空闲BD；
//          4.若空闲BD数目与帧所需的pkg数目足够，则配置BD，并发送；若不够，则等待
//          5.若在指定的时间内未获得足够的bd，则直接返回失败
// 输入参数：devindex,使用的网卡
//          pkg,待发送的数据包
// 输出参数：
// 返回值  ：true发送成功  false发送失败。
// =============================================================================
static bool_t Enet_SendPacket(ptu32_t hanlde,tagNetPkg * pkglst,u32 framlen, u32 netdevtask)
{
    bool_t  result = false;
    tagNetPkg *tmppkg;
    u32 TxBdIndex;

    u8  *dst,*src;
    u32 len,pkglen;

    if((NULL == hanlde)||(NULL == pkglst))
        return result;

    if(Lock_MutexPend(pEnetSndSync,CN_TIMEOUT_FOREVER))
    {
        TxBdIndex = sNextTxBd;

        if( !(spTxBdTab[TxBdIndex].status & TX_BD_R) )      //当前BD已准备就绪
        {
            dst = (u8 *)ENET_REV((u32)spTxBdTab[TxBdIndex].data);

            pkglen = 0;
            len =0;
            tmppkg = pkglst;
            do{
                //拷贝数据
                src = (tmppkg->buf + tmppkg->offset);
                len = tmppkg->datalen;
                memcpy((void *)dst,(void *)src,len);
                dst += len;
                pkglen+=len;
                if(pkglen > TX_BUFFER_SIZE)
                {
                    break;//溢出
                }

                if(PKG_ISLISTEND(tmppkg))
                {
                    tmppkg = NULL;
                    break;
                }
                else
                {
                    tmppkg = tmppkg->partnext;
                }
            }while(tmppkg != NULL);

            spTxBdTab[TxBdIndex].status = TX_BD_TC | TX_BD_R | TX_BD_L;
            spTxBdTab[TxBdIndex].length = ENET_REVSH(pkglen);

            // Wrap if this was last TxBD
            if(++TxBdIndex == NUM_TXBDS)
            {
                spTxBdTab[NUM_TXBDS - 1].status |= TX_BD_W;
                TxBdIndex = 0;
            }

            // Update the global txbd index
            sNextTxBd = TxBdIndex;

            // Indicate that Descriptors are ready to transmit
            ENET->TDAR = ENET_TDAR_TDAR_MASK;

            result = true;
        }
        else
        {
            //no bd here
        }
        Lock_MutexPost(pEnetSndSync);
    }

    return result;
}
// =============================================================================
// 功能：接收事件的函数，阻塞等待接收到完整数据包的信号量，并接收读取完网卡缓存的所有数据包
// 参数：无
// 返回：无
// =============================================================================
static ptu32_t __GmacRcvTask(void)
{
    tagNetPkg *pkg;
    while(1)
    {
        Lock_SempPend(pEnetRcvSync,CN_TIMEOUT_FOREVER);
        while((pkg = __Enet_RcvPacket(gEnetHandle))!= NULL)//不断读网卡直到没有完整数据包为止
        {
            LinkPost(gEnetHandle,pkg);
            PkgTryFreePart(pkg);
        }
    }
    return 0;
}
// =============================================================================
// 功能：网卡接收数据的任务
// 参数：网卡
// 返回：true
// =============================================================================
static bool_t __CreateRcvTask(ptu32_t nethandle)
{
    bool_t result = false;
    u16 evttID;
    u16 eventID;

    evttID = Djy_EvttRegist(EN_CORRELATIVE, CN_PRIO_RRS, 0, 1,
        (ptu32_t (*)(void))__GmacRcvTask,NULL, 0x400, "ENETRcvTask");
    if (evttID != CN_EVTT_ID_INVALID)
    {
        eventID=Djy_EventPop(evttID, NULL,  0,(ptu32_t)nethandle, 0, 0);
        if(eventID != CN_EVENT_ID_INVALID)
        {
            result = true;
        }
        else
        {
            Djy_EvttUnregist(evttID);
        }
    }
    return result;
}

bool_t Enet_Ctrl(ptu32_t handle,u8 cmd, ptu32_t para)
{
    atom_low_t  atom;
    bool_t result = false;
    if(0 != handle)
    {
        switch(cmd)
        {
            case EN_NETDEV_SETMAC:
                memcpy(s_EnetConfig.mac,(u8 *)para, CN_MACADDR_LEN);
                atom = Int_LowAtomStart();
                Enet_MAC_Init();
                Int_LowAtomEnd(atom);
                result = true;
                break;
            case EN_NETDEV_GTETMAC:
                break;
            default:
                break;
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
static bool_t Enet_AddNetDev(void)
{
    tagNetDevPara   devpara;

    //创建发送和接收同步量
    pEnetRcvSync = Lock_SempCreate(1,1,CN_BLOCK_FIFO,NULL);
    if(NULL == pEnetRcvSync)
    {
        goto RcvSyncFailed;
    }
    pEnetSndSync = Lock_MutexCreate(NULL);
    if(NULL == pEnetSndSync)
    {
        goto SndSyncFailed;
    }

    //注册到网络设备链中
    devpara.ifctrl = Enet_Ctrl;
    devpara.ifsend = Enet_SendPacket;
    devpara.iftype = EN_LINK_ETHERNET;
    devpara.devfunc = 0;    //NO FUNC FOR THE DEV
    memcpy(devpara.mac, s_EnetConfig.mac,6);
    devpara.name = (char *)(s_EnetConfig.name);
    devpara.private = 0;
    devpara.mtu = CN_PKG_MAX_LEN;
    devpara.private = (u32)NULL;
    gEnetHandle = NetDevInstall(&devpara);
    if(0 == gEnetHandle)
    {
        goto NetInstallFailed;
    }
    if(false == __CreateRcvTask(gEnetHandle))
    {
        goto RcvTaskFailed;
    }
    return true;

RcvTaskFailed:
    NetDevUninstall(s_EnetConfig.name);
NetInstallFailed:
    Lock_MutexDelete(pEnetSndSync);
    pEnetSndSync = NULL;
SndSyncFailed:
    Lock_SempDelete(pEnetRcvSync);
    pEnetRcvSync = NULL;
RcvSyncFailed:
    printf("%s:Install gmac failed\n\r",__FUNCTION__);
    return false;
}


// =============================================================================
// 功能：DM9000网卡和DJYIP驱动初始化函数
// 参数：para
// 返回值  ：true成功  false失败。
// =============================================================================
bool_t ModuleInstall_Enet(ptu32_t para)
{
    bool_t result = false;

    if(para == (u32)NULL)
        return false;

    memcpy(&s_EnetConfig,(u8*)para,sizeof(tagEnetCfg));

    Enet_PortInit();                                // PORT初始化
    Enet_IntInit(CN_INT_LINE_ENET_RX,Enet_RxISR);   // INT初始化
    Enet_IntInit(CN_INT_LINE_ENET_ERR,Enet_ErrISR); // INT初始化

    PHY_Init();                         // PHY初始化
    Enet_MAC_Init();                    // MAC初始化

    result = Enet_AddNetDev();

    return result;
}
#endif
