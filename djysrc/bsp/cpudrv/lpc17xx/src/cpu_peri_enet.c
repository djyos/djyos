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
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 19/08.2014
// =============================================================================

#include "cpu_peri.h"
#include "string.h"

// 定义MAC参数配置，默认配置为RMII，100M，全双工,网卡
static ENET_CONFIG s_EnetConfig = {MAC_RMII,AUTONEG_ON,MII_100BASET,MII_FDX,
                            NO_LOOPBACK,{0x1E,0x30,0x6C,0xA2,0x45,0x5E}};

// =============================================================================
// 功能：MII接口写函数，通过MDIO与PHY芯片通信
// 参数：phy_addr, PHY地址
//       PhyReg,寄存器地址
//       Value,写入的数据
// 返回：0,正常返回；1,失败
// =============================================================================
static void __EMAC_PHY_Write (int PhyAddr,int PhyReg, int Value)
{
    u32 tout;

    LPC_EMAC->MADR = PhyAddr | PhyReg;
    LPC_EMAC->MWTD = Value;

    /* Wait utill operation completed */
    tout = 0;
    for (tout = 0; tout < MII_WR_TOUT; tout++)
    {
        if ((LPC_EMAC->MIND & MIND_BUSY) == 0)
        {
            break;
        }
    }
}

// =============================================================================
// 功能：MII接口读数据，通过MDIO接口与PHY芯片通信
// 参数：phy_addr, PHY地址
//       reg_addr,寄存器地址
//       data,读取到的数据指针
// 返回：0,正常返回；1,失败
// =============================================================================
static u16 __EMAC_PHY_Read (int PhyAddr,unsigned char PhyReg)
{
    u32 val,tout;

    LPC_EMAC->MADR = PhyAddr | PhyReg;
    LPC_EMAC->MCMD = MCMD_READ;

    /* Wait until operation completed */
    tout = 0;
    for (tout = 0; tout < MII_RD_TOUT; tout++)
    {
        if ((LPC_EMAC->MIND & MIND_BUSY) == 0)
        {
            break;
        }
    }
    LPC_EMAC->MCMD = 0;
    val = LPC_EMAC->MRDD;
    return (val);
}

// =============================================================================
// 功能：PHY芯片初始化，本函数使用自适应方式，通过读取PHY芯片中SPEED和双工的相关寄存
//       器，来获得配置MAC的相关参数
// 参数：无
// 返回：true,link成功；false,link失败
// =============================================================================
static bool_t __EMAC_PHY_Init(void)
{
    u32 regv,tout;
    bool_t result = false;

    __EMAC_PHY_Write (DP83848C_DEF_ADR,PHY_REG_BMCR, 0x8000);//Reset PHY

    //Wait for hardware reset to end.
    for (tout = 0; tout < 0x100000; tout++)
    {
        regv = __EMAC_PHY_Read (DP83848C_DEF_ADR,PHY_REG_BMCR);
        if (!(regv & 0x8000))
            break;                          //Reset complete
    }

    if(s_EnetConfig.autoneg == AUTONEG_ON)
    {
        //Set Auto negotiation
        __EMAC_PHY_Write (DP83848C_DEF_ADR,PHY_REG_BMCR, PHY_AUTO_NEG);
        tout = 100;
        while(tout--)
        {
            regv = __EMAC_PHY_Read (DP83848C_DEF_ADR,PHY_REG_BMSR);
            if(regv & 0x0020)   break;          //Auto negotiation Complete.
            Djy_DelayUs(1*mS);
        }
    }
    else
    {
        //Manual Configure The Speed And Half/Full Duplex Mode
    }

    //Check the link status
    tout = 100;
    while(tout--)
    {
        regv = __EMAC_PHY_Read (DP83848C_DEF_ADR,PHY_REG_STS);
        if (regv & 0x0001)  break;              //Link is on
        Djy_DelayUs(1*mS);
    }
    if(tout != 0)
    {
        if (regv & 0x0004)
            s_EnetConfig.duplex = MII_FDX;
        else
            s_EnetConfig.duplex = MII_HDX;
        if (regv & 0x0002)
            s_EnetConfig.speed = MII_10BASET;
        else
            s_EnetConfig.speed = MII_100BASET;
        result = true;
    }
    return result;
}

// =============================================================================
// 功能：接收BD表初始化，发送描述符数组、接收描述符数组，以及发送状态数组都必须与一个
//       4 字节（32  位）的地址边界对齐，而接收状态数组则必须与一个 8 字节（64 位）的
//       地址边界对齐.ENET的BD表使用了SRAM的从0x20080000开始的16K独立地址空间
// 参数：无
// 返回：无
// =============================================================================
static void __EMAC_RXBD_Init(void)
{
    u32 i;
    for (i = 0; i < NUM_RX_FRAG; i++)
    {
        RX_DESC_PACKET(i)  = RX_BUF(i);
        RX_DESC_CTRL(i)    = RCTRL_INT | (ETH_FRAG_SIZE-1);
        RX_STAT_INFO(i)    = 0;
        RX_STAT_HASHCRC(i) = 0;
    }

    /* Set EMAC Receive Descriptor Registers. */
    LPC_EMAC->RxDescriptor    = RX_DESC_BASE;
    LPC_EMAC->RxStatus        = RX_STAT_BASE;
    LPC_EMAC->RxDescriptorNumber = NUM_RX_FRAG-1;

    /* Rx Descriptors Point to 0 */
    LPC_EMAC->RxConsumeIndex  = 0;
}

// =============================================================================
// 功能：发送BD表初始化，发送描述符数组、接收描述符数组，以及发送状态数组都必须与一个
//       4 字节（32  位）的地址边界对齐，而接收状态数组则必须与一个 8 字节（64 位）的
//       地址边界对齐.ENET的BD表使用了SRAM的从0x20080000开始的16K独立地址空间
// 参数：无
// 返回：无
// =============================================================================
static void __EMAC_TXBD_Init(void)
{
    u32 i;
    for (i = 0; i < NUM_TX_FRAG; i++)
    {
        TX_DESC_PACKET(i) = TX_BUF(i);
        TX_DESC_CTRL(i)   = 0;
        TX_STAT_INFO(i)   = 0;
    }

    /* Set EMAC Transmit Descriptor Registers. */
    LPC_EMAC->TxDescriptor    = TX_DESC_BASE;
    LPC_EMAC->TxStatus        = TX_STAT_BASE;
    LPC_EMAC->TxDescriptorNumber = NUM_TX_FRAG-1;

    /* Tx Descriptors Point to 0 */
    LPC_EMAC->TxProduceIndex  = 0;
}

// =============================================================================
// 功能：以太网模块MAC初始化，主要是MAC寄存器和BD表的初始化，下几个步骤：
//       1.ENET模块时钟使能，引脚配置，包括ENET和MDC等引脚；
//       2.复位以太网模块，并配置其寄存器，复位包括TX、RX等
//       3.PHY初始化，主要包括LINK、SPEED、DUPLEX
//       4.初始化BD表，配置TCR和RCR，即发送接收功能；
//       3.MAC地址初始化，写MAC到寄存器，可配置为是否强制替换发送包中的源地址；
//       4.使能ENET。
// 参数：无
// 返回：无
// =============================================================================
static void  __EMAC_Init(void)
{
    LPC_SC->PCONP |= (1<<30);                   //Power Up the EMAC controller.
    LPC_PINCON->PINSEL2 = 0x50150105;           //Enable P1 Ethernet Pins.
    LPC_PINCON->PINSEL3 = (LPC_PINCON->PINSEL3 & ~0xF) | 0x5;   //MDC MDIO Pins

    //Reset all EMAC internal modules.
    LPC_EMAC->MAC1 = MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX
                    | MAC1_RES_MCS_RX | MAC1_SIM_RES | MAC1_SOFT_RES;
    LPC_EMAC->Command = CR_REG_RES | CR_TX_RES | CR_RX_RES | CR_PASS_RUNT_FRM;

    Djy_DelayUs(1*mS);                          //A short delay after reset.

    //Initialize MAC control registers.
    LPC_EMAC->MAC1 = MAC1_PASS_ALL;
    LPC_EMAC->MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;
    LPC_EMAC->MAXF = ETH_MAX_FLEN;
    LPC_EMAC->CLRT = CLRT_DEF;
    LPC_EMAC->IPGR = IPGR_DEF;

    //Enable Reduced MII interface.
    if(s_EnetConfig.interface == MAC_RMII)
        LPC_EMAC->Command = CR_RMII | CR_PASS_RUNT_FRM;
    else
    {
        LPC_EMAC->Command &= ~CR_RMII;
        LPC_EMAC->Command |= CR_PASS_RUNT_FRM;
    }

    if(!__EMAC_PHY_Init())
        return;
    if(s_EnetConfig.duplex == MII_FDX)          //Full Duplex Mode
    {
        LPC_EMAC->MAC2    |= MAC2_FULL_DUP;
        LPC_EMAC->Command |= CR_FULL_DUP;
        LPC_EMAC->IPGT     = IPGT_FULL_DUP;
    }
    else
    {
        LPC_EMAC->IPGT = IPGT_HALF_DUP;         //Half Duplex Mode
    }

    if(s_EnetConfig.speed == MII_100BASET)
        LPC_EMAC->SUPP = SUPP_SPEED;            //100Mbit Speed
    else
        LPC_EMAC->SUPP = 0;                     //10Mbit Speed

    //Set the Ethernet MAC Address registers
    LPC_EMAC->SA0 = (s_EnetConfig.mac[0] << 8) | s_EnetConfig.mac[1];
    LPC_EMAC->SA1 = (s_EnetConfig.mac[2] << 8) | s_EnetConfig.mac[3];
    LPC_EMAC->SA2 = (s_EnetConfig.mac[4] << 8) | s_EnetConfig.mac[5];

    //Initialize Tx and Rx DMA Descriptors
    __EMAC_RXBD_Init();
    __EMAC_TXBD_Init();

    //Receive Broadcast and Perfect Match Packets
    LPC_EMAC->RxFilterCtrl = RFC_BCAST_EN | RFC_PERFECT_EN;

    //Enable EMAC interrupts.
    LPC_EMAC->IntEnable = INT_RX_DONE | INT_TX_DONE;

    //Reset all interrupts
    LPC_EMAC->IntClear  = 0xFFFF;

    /* Enable receive and transmit mode of MAC Ethernet core */
    LPC_EMAC->Command  |= (CR_RX_EN | CR_TX_EN);
    LPC_EMAC->MAC1     |= MAC1_REC_EN;
}

// =============================================================================
// 功能：Enet中断初始化，使用中断的方式接收数据，用于配置中断寄存器
// 参数：无
// 返回：无
// =============================================================================
static void __EMAC_IntInit(void)
{
    ufast_t IntLine = CN_INT_LINE_ENET;
    u32 Enet_ISR(ufast_t IntLine);
    Int_SetClearType(IntLine,CN_INT_CLEAR_PRE);
    Int_IsrConnect(IntLine,Enet_ISR);
    Int_SettoAsynSignal(IntLine);
    Int_ClearLine(IntLine);
    Int_RestoreAsynLine(IntLine);
}

// =============================================================================
// 功能：以太网模块中断服务函数，包括发送、接收帧完成和错误中断，对错误帧的处理表现在
//       是否需对硬件复位，而发生溢出错误时，必须重新配置BD表
// 参数：int_line,中断线号
// 返回：0
// =============================================================================
u32 Enet_ISR(ufast_t IntLine)
{
    u32 idx;
    //Tx Frame Done Int
    if(LPC_EMAC->IntStatus & INT_TX_DONE)
    {

    }

    //Rx Frame Done Int
    if(LPC_EMAC->IntStatus & INT_RX_DONE)
    {
        idx = LPC_EMAC->RxConsumeIndex;
        if(++idx == NUM_RX_FRAG) idx = 0;
        LPC_EMAC->RxConsumeIndex = idx;
    }

    //Error Int
    if(LPC_EMAC->IntStatus & (INT_RX_ERR | INT_TX_ERR))
    {

    }

    LPC_EMAC->IntClear = 0xFFFF;        //Clear The INT Flag
    return 0;
}

u32 Enet_Ctrl(u32 cmd,ptu32_t data1, ptu32_t data2)
{
    return 0;
}

// =============================================================================
// 功能：以太网数据包发送函数，若检测到此时冲突或BD表满，则立即返回0，表示本次发送失
//       败，一帧数据发送完成后，会产生TX_DONE中断
// 参数：packet,待发送数据包指针
//       length,数据包长度，字节单位，不得超过FRAME最大长度（1536）
// 返回：true,成功;false,失败
// =============================================================================
bool_t Enet_SendPacket(u8 *packet,u32 length)
{
    u8 *tptr,idx;
    u32 FrameSize;

    FrameSize = length - 1;
    idx  = LPC_EMAC->TxProduceIndex;
    if(LPC_EMAC->TxProduceIndex == LPC_EMAC->TxConsumeIndex - 1)    //FULL
    {
        return false;
    }
    tptr = (u8 *)TX_DESC_PACKET(idx);
    TX_DESC_CTRL(idx) = FrameSize | TCTRL_LAST | TCTRL_INT;

    memcpy(tptr,packet,length);
    if (++idx == NUM_TX_FRAG)
        idx = 0;
    LPC_EMAC->TxProduceIndex = idx;
    return true;
}

// =============================================================================
// 功能：以太网数据包读取，当接收到一个完成以太网包，全产生帧完成中断
// 参数：packet,数据存储指针
// 返回：length,数据包长度，字节单位，不得超过FRAME最大长度（1536）
// =============================================================================
u32 Enet_RecvPacket(u8* packet)
{
    u32 idx,len,length = 0;
    u8* rptr;
//  if(s_RxFrameDoneFlash == false)
//      return length;
//  s_RxFrameDoneFlash = false;
    while(LPC_EMAC->RxProduceIndex != LPC_EMAC->RxConsumeIndex)
    {
        idx = LPC_EMAC->RxConsumeIndex;
        rptr = (u8*)RX_DESC_PACKET(idx);
        len = RX_STAT_INFO(idx) & RINFO_SIZE;
        memcpy(packet,rptr,len);
        packet += len;                          //指针左移
        length += len;
        if(++idx == NUM_RX_FRAG)    idx = 0;
        LPC_EMAC->RxConsumeIndex = idx;
        //每次读一帧数据
        if(RX_STAT_INFO(idx) & RINFO_LAST_FLAG)
        {
            break;
        }
    }
    return length;
}

// =============================================================================
// 功能：初始化以太网模块，该模块初始化时采用默认的参数配置，即PHY自适应模式、RMII接
//       口、100MBIT、全双工方式。初始化包括了PHY的初始化及通信和以太网MAC寄存器初始
//       化，其中，BD表尤其重要。采用中断方式发送和接收。
// 参数：para,未使用
// 返回：0
// =============================================================================
ptu32_t ModuleInstall_Enet(ptu32_t para)
{
    __EMAC_Init();
    __EMAC_IntInit();
    return 0;
}
