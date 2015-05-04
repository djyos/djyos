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
// Copyright (C) 2012-2020 深圳鹏瑞软件有限公司 All Rights Reserved
// 模块描述: p1020Tsec.c:
// 模块版本: V1.00
// 创建人员: Zqf
// 创建时间: 3:25:13 PM/Sep 23, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
//The Driver For P1020

//----------------------------------------------------
//Copyright (C), 2013-2020,  luoshitian.
//版权所有 (C), 2013-2020,   罗侍田.
//所属模块: tsec网口驱动模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: tsec网口驱动文件,包括etsec3网口的phy和网口的驱动
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-11-14
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "stdio.h"
#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "os.h"
#include "int_hard.h"

#include <asm/io.h>
#include "printf.h"
#include <cpu_peri.h>
#include "phy_88e1512.h"
#include "string.h"
#include "cpu_peri_tsec.h"
#include "netdev.h"
#include "rout.h"
#include "tcpip_cdef.h"

#pragma pack(1)
typedef struct __tagTsecFcb
{
    u16  status;
    u8   l4os;
    u8   l3os;
    u16  phcs;
    u16  vlctl;
}tagTsecFcb;
#pragma pack()
#define CN_FCB_VLN (1<<15)
#define CN_FCB_IP  (1<<14)
#define CN_FCB_IP6 (1<<13)
#define CN_FCB_TUP (1<<12)
#define CN_FCB_UDP (1<<11)
#define CN_FCB_CIP (1<<10)
#define CN_FCB_CTU (1<<9)
#define CN_FCB_NPH (1<<8)
#define CN_FCB_PTP (1<<0)

typedef struct _tagTsecSndPkgBuf
{
    struct _tagTsecSndPkgBuf *nxt;
    tagNetPkg *plst;
    u8 pkgnum;
    u16 bdno;
}tagTsecSndPkgBuf;

struct tsec_info_struct
{
    tsec_t *regs;
    struct tsec_mii_mng *miiregs_sgmii;
    char *devname;
    char *mii_devname;
    phy_interface_t interface;
    u32 phyaddr;
    u32  flags;
    unsigned char enetaddr[6];
    void *priv;

    u8  devno;
    tagTsecSndPkgBuf   *sndpkgbufhead;
    tagTsecSndPkgBuf   *sndpkgbuftail;
};

/*
 * AS.HARNOIS : Better to set CN_RCVPKG_MAXSIZE to maximum size because
 * traffic type is not always controlled
 * maximum packet size =  1518
 * maximum packet size and multiple of 32 bytes =  1536
 */
#define CN_RCVPKG_ALIGN          ALIGN_SIZE
#define CN_RCVPKG_MAXSIZE        1518
#define CN_RCVPKG_ALIGNSIZE      1536

#define CN_RXBD_NUM              512   //20 bd for snd
#define CN_TXBD_NUM              512   //20 bd for rcv

#define CN_CRCPOLY_LE            0xedb88320

typedef struct _tagTsecBdTab
{
    txbd8_t txbd[CN_TXBD_NUM];
    rxbd8_t rxbd[CN_RXBD_NUM];
    u32 nxtrxbd;        /* index of the read bd  nxttime */
    u32 nxttxbd;        /* index of the write bd nxttime*/
    tagNetPkg *txpkg[CN_TXBD_NUM];
    tagNetPkg *rcvpkg[CN_RXBD_NUM];
} tagTsecBdTab;

#define CN_TSEC_NUM  3
#ifdef __GNUC__
static tagTsecBdTab sgTsecBDTab[CN_TSEC_NUM] __attribute__ ((aligned(8)));
#else
#error "rtx must be 64-bit aligned"
#endif

/* Default initializations for TSEC controllers. */

static struct tsec_info_struct sgTsecInfo[] =
{
#if    (CONFIG_TSEC1 == 1)
    STD_TSEC_INFO(1),    /* TSEC1 */
#endif
#if    (CONFIG_TSEC2 == 1)
    STD_TSEC_INFO(2),    /* TSEC2 */
#endif
#if    (CONFIG_TSEC3 == 1)
    STD_TSEC_INFO(3),    /* TSEC3 */
#endif
#if    (CONFIG_TSEC4 == 1)
    STD_TSEC_INFO(4),    /* TSEC4 */
#endif
};

#define TBIANA_SETTINGS ( \
        TBIANA_ASYMMETRIC_PAUSE \
        | TBIANA_SYMMETRIC_PAUSE \
        | TBIANA_FULL_DUPLEX \
        )

/* By default force the TBI PHY into 1000Mbps full duplex when in SGMII mode */
#ifndef CONFIG_TSEC_TBICR_SETTINGS
#define CONFIG_TSEC_TBICR_SETTINGS ( \
        TBICR_PHY_RESET \
        | TBICR_ANEG_ENABLE \
        | TBICR_FULL_DUPLEX \
        | TBICR_SPEED1_SET \
        )
#endif /* CONFIG_TSEC_TBICR_SETTINGS */

extern s32 phy_startup(struct tsec_private *priv);
extern s32 phy_reset(s32 addr);

/* Configure the TBI for SGMII operation */
//---配置TBI寄存器函数-------------------------------------------
//功能：p1020rdb中采用tsec的sgmill模式与phy进行连接，配置
//      tbi寄存器连接外部phy
//参数：struct tsec_private *priv，tsec网口PHY配置结构体
//返回：无
//---------------------------------------------------------------
static void __TsecSerdesConfig(struct tsec_private *priv)
{
    /* Access TBI PHY registers at given TSEC register offset as
     *opposed to the register offset used for external PHY accesses */
    tsec_local_mdio_write(priv->phyregs_sgmii,
                          in_be32(&priv->regs->tbipa),
                          0, TBI_ANA, TBIANA_SETTINGS);
    tsec_local_mdio_write(priv->phyregs_sgmii,
                          in_be32(&priv->regs->tbipa),
                          0, TBI_TBICON, TBICON_CLK_SELECT);
    tsec_local_mdio_write(priv->phyregs_sgmii,
                          in_be32(&priv->regs->tbipa),
                          0, TBI_CR, CONFIG_TSEC_TBICR_SETTINGS);
}

/* Initialized required registers to appropriate values, zeroing
 * those we don't care about (unless zero is bad, in which case,
 * choose a more appropriate value)
 */
//---初始化tsec寄存器函数---------------------------------------
//功能：初始化tsec寄存器，清零
//参数：struct tsec_t *regs，tsec网口寄存器结构体
//返回：无
//--------------------------------------------------------------

#define CN_TSEC_TCTRL_IPCSEN  (1<<14)
#define CN_TSEC_TCTRL_TUCSEN  (1<<13)
static void __TsecRegsInit(tsec_t *regs)
{
    u32 value;
    /* Clear IEVENT */
    out_be32(&regs->ievent, IEVENT_INIT_CLEAR);
    //anyway, we open the snd and rcv fram interrupt
    value = IEVENT_XFUN|IMASK_RXFEN0;
    out_be32(&regs->imask, value);


    out_be32(&regs->hash.iaddr0, 0);
    out_be32(&regs->hash.iaddr1, 0);
    out_be32(&regs->hash.iaddr2, 0);
    out_be32(&regs->hash.iaddr3, 0);
    out_be32(&regs->hash.iaddr4, 0);
    out_be32(&regs->hash.iaddr5, 0);
    out_be32(&regs->hash.iaddr6, 0);
    out_be32(&regs->hash.iaddr7, 0);

    out_be32(&regs->hash.gaddr0, 0);
    out_be32(&regs->hash.gaddr1, 0);
    out_be32(&regs->hash.gaddr2, 0);
    out_be32(&regs->hash.gaddr3, 0);
    out_be32(&regs->hash.gaddr4, 0);
    out_be32(&regs->hash.gaddr5, 0);
    out_be32(&regs->hash.gaddr6, 0);
    out_be32(&regs->hash.gaddr7, 0);

    out_be32(&regs->rctrl, 0x00000000);

    value = CN_TSEC_TCTRL_IPCSEN|CN_TSEC_TCTRL_TUCSEN;
    out_be32(&regs->tctrl, value);

    /* Init RMON mib registers */
    memset((void *)&(regs->rmon), 0, sizeof(rmon_mib_t));

    out_be32(&regs->rmon.cam1, 0xffffffff);
    out_be32(&regs->rmon.cam2, 0xffffffff);

    out_be32(&regs->mrblr, MRBLR_INIT_SETTINGS);

    out_be32(&regs->minflr, MINFLR_INIT_SETTINGS);

    out_be32(&regs->attr, ATTR_INIT_SETTINGS);
    out_be32(&regs->attreli, ATTRELI_INIT_SETTINGS);

}

/* Configure maccfg2 based on negotiated speed and duplex
 * reported by PHY handling code
 */
//---配置maccfg2寄存器函数-----------------------------------------
//功能：基于之前phy链路连接配置的速度和双工模式，配置maccfg2寄存器
//参数：struct tsec_private *priv，tsec网口PHY配置结构体
//返回：无
//----------------------------------------------------------------
static void __TsecAdjustLink(struct tsec_private *priv)
{
    tsec_t *regs = priv->regs;
    u32  ecntrl, maccfg2;

    if (!priv->link)
    {
        printk("%s: No link.\r\n",priv->name);
        return;
    }

    /* clear all bits relative with interface mode */
    ecntrl = in_be32(&regs->ecntrl);
    ecntrl &= ~ECNTRL_R100;

    maccfg2 = in_be32(&regs->maccfg2);
    maccfg2 &= ~(MACCFG2_IF | MACCFG2_FULL_DUPLEX);

    if (priv->duplex)
        maccfg2 |= MACCFG2_FULL_DUPLEX;

    switch (priv->speed)
    {
    case 1000:
        maccfg2 |= MACCFG2_GMII;
        break;
    case 100:
    case 10:
        maccfg2 |= MACCFG2_MII;

        /* Set R100 bit in all modes although
         * it is only used in RGMII mode
         */
        if (priv->speed == 100)
            ecntrl |= ECNTRL_R100;
        break;
    default:
        printk("%s: Speed was bad\n", priv->name);
        break;
    }

    out_be32(&regs->ecntrl, ecntrl);
    out_be32(&regs->maccfg2, maccfg2);

    printk("Speed: %d, %s duplex\r\n", priv->speed,
               (priv->duplex) ? "full" : "half");
}
/* Set up the buffers and their descriptors, and bring up the
 * interface
 */
//---启动tsec函数-------------------------------------------------------
//功能：配置tsec网口收发缓冲，使能收发
//参数：struct tsec_info_struct *tsec_info，tsec网口配置结构体
//返回：无
//----------------------------------------------------------------------
static void __TsecConfigBDTab(struct tsec_info_struct *tsec_info)
{
    u32 i;
    tagTsecBdTab *bdtab;
    tsec_t *regs;
    struct tsec_private *priv;

    priv = (struct tsec_private *)tsec_info->priv;
    regs = priv->regs;
    bdtab = &sgTsecBDTab[tsec_info->devno];
    //alloc the mem for the rcv db
    //when we rcv from the db, at the same time we must alloc an pkg to replace the
    //the rcv pkg
    for(i =0; i< CN_RXBD_NUM; i++)
    {
        bdtab->rcvpkg[i] = NULL;
//      bdtab->rcvpkg[i] = Pkg_Alloc(CN_RCVPKG_ALIGNSIZE);
//      if(NULL == bdtab->rcvpkg[i])
//      {
//          printk("p1020Tsec:BD PKG ALLOC FAILED!\n\r");
//      }
//      else
//      {
//          bdtab->rcvpkg[i]->freefun= Pkg_Free;
//      }
    }
    /* Initialize the Rx Buffer descriptors */
//    for (i = 0; i < CN_RXBD_NUM; i++)
//    {
//      bdtab->rxbd[i].status = RXBD_EMPTY|RXBD_INTERRUPT;
//      bdtab->rxbd[i].length = 0;
//        bdtab->rxbd[i].bufPtr = (u32 )(bdtab->rcvpkg[i]->buf+bdtab->rcvpkg[i]->offset);
//    }
    for (i = 0; i < CN_RXBD_NUM; i++)
    {
        bdtab->rxbd[i].status = RXBD_EMPTY|RXBD_INTERRUPT;
        bdtab->rxbd[i].length = 0;
        bdtab->rxbd[i].bufPtr = (u32)(M_Malloc(CN_RCVPKG_ALIGNSIZE,CN_TIMEOUT_FOREVER));
    }
    bdtab->rxbd[CN_RXBD_NUM-1].status |= RXBD_WRAP;
    bdtab->nxtrxbd = 0;

    /* Initialize the TX Buffer Descriptors */
    for (i = 0; i < CN_TXBD_NUM; i++)
    {
        bdtab->txbd[i].status = 0;
        bdtab->txbd[i].length = 0;
        bdtab->txbd[i].bufPtr = 0;
        bdtab->txpkg[i] = NULL;
    }
    bdtab->txbd[CN_TXBD_NUM-1].status |= TXBD_WRAP;
    bdtab->nxttxbd = 0;

    /* Point to the buffer descriptors */
    out_be32(&regs->tbase, (u32)(bdtab->txbd));
    out_be32(&regs->rbase, (u32)(bdtab->rxbd));

//    redundant_init(tsec_info);//回环测试

    /* Enable Transmit and Receive */
    setbits_be32(&regs->maccfg1, MACCFG1_RX_EN | MACCFG1_TX_EN);
    /* Tell the DMA it is clear to go */
    setbits_be32(&regs->dmactrl, DMACTRL_INIT_SETTINGS);
    out_be32(&regs->tstat, TSTAT_CLEAR_THALT);
    out_be32(&regs->rstat, RSTAT_CLEAR_RHALT);
    clrbits_be32(&regs->dmactrl, DMACTRL_GRS | DMACTRL_GTS);
}


/* Stop the interface */
//---tsec网口停止函数---------------------------------------------------
//功能：停止使用tsec网口接口
//参数：struct tsec_info_struct *tsec_info，tsec网口配置结构体
//返回：无
//----------------------------------------------------------------------
static void __TsecHalt(struct tsec_info_struct *tsec_info)
{
    struct tsec_private *priv = (struct tsec_private *)tsec_info->priv;
    tsec_t *regs = priv->regs;

    clrbits_be32(&regs->dmactrl, DMACTRL_GRS | DMACTRL_GTS);
    setbits_be32(&regs->dmactrl, DMACTRL_GRS | DMACTRL_GTS);
#if 1
    while ((in_be32(&regs->ievent) & (IEVENT_GRSC | IEVENT_GTSC))
            != (IEVENT_GRSC | IEVENT_GTSC))
        ;
#endif
    clrbits_be32(&regs->maccfg1, MACCFG1_TX_EN | MACCFG1_RX_EN);

    /* Shut down the PHY, as needed */
}

/* Initializes data structures and registers for the controller,
 * and brings the interface up.     Returns the link status, meaning
 * that it returns success if the link is up, failure otherwise.
 * This allows u-boot to find the first active controller.
 */
//---tsec网口初始化函数-------------------------------------------------
//功能：tsec网口接口初始化
//参数：struct tsec_info_struct *tsec_info，tsec网口配置结构体
//返回：返回tsec网口链路连接状态
//----------------------------------------------------------------------
bool_t __TsecInit(u8 devno)
{
    u32 tempval;
    char tmpbuf[MAC_ADDR_LEN];
    s32 i;
    struct tsec_info_struct *tsec_info;
    struct tsec_private *priv;
    tsec_t *regs;
    s32 ret;

    tsec_info = &sgTsecInfo[devno];
    priv = (struct tsec_private *)tsec_info->priv;
    regs = priv->regs;
    /* Make sure the controller is stopped */
    __TsecHalt(tsec_info);

    /* Init MACCFG2.  Defaults to GMII */
    out_be32(&regs->maccfg2, MACCFG2_INIT_SETTINGS);

    /* Init ECNTRL */
    out_be32(&regs->ecntrl, ECNTRL_INIT_SETTINGS);

    /* Copy the station address into the address registers.
     * Backwards, because little endian MACS are dumb */
    for (i = 0; i < MAC_ADDR_LEN; i++)
        tmpbuf[MAC_ADDR_LEN - 1 - i] = tsec_info->enetaddr[i];

    tempval = (tmpbuf[0] << 24) | (tmpbuf[1] << 16) | (tmpbuf[2] << 8) |
              tmpbuf[3];

    out_be32(&regs->macstnaddr1, tempval);

    tempval = *((u32 *) (tmpbuf + 4));

    out_be32(&regs->macstnaddr2, tempval);

    /* Clear out (for the most part) the other registers */
    __TsecRegsInit(regs);

    /* Ready the device for tx/rx */
    __TsecConfigBDTab(tsec_info);

    /* Start up the PHY */
    ret = phy_startup(priv);
    if (ret)
    {
        printk("Could not initialize PHY %s\n",
                   tsec_info->devname);
        return ret;
    }

    __TsecAdjustLink(priv);

    /* If there's no link, fail */
    return priv->link ? true: false;
}
//---获取phy接口类型函数------------------------------------------------
//功能：获取tsec的phy接口连接方式，看是MII、GMII、RGMII、SGMII等哪种模式
//参数：struct tsec_private *priv，tsec网口PHY配置结构体
//返回：返回tsec的PHY接口连接模式
//----------------------------------------------------------------------
static phy_interface_t __TsecGetInterface(struct tsec_private *priv)
{
    tsec_t *regs = priv->regs;
    u32  ecntrl;

    ecntrl = in_be32(&regs->ecntrl);

    if (ecntrl & ECNTRL_SGMII_MODE)
        return PHY_INTERFACE_MODE_SGMII;

    if (ecntrl & ECNTRL_TBI_MODE)
    {
        if (ecntrl & ECNTRL_REDUCED_MODE)
            return PHY_INTERFACE_MODE_RTBI;
        else
            return PHY_INTERFACE_MODE_TBI;
    }

    if (ecntrl & ECNTRL_REDUCED_MODE)
    {
        if (ecntrl & ECNTRL_REDUCED_MII_MODE)
            return PHY_INTERFACE_MODE_RMII;
        else
        {
            phy_interface_t interface = priv->interface;

            /*
             * This isn't autodetected, so it must
             * be set by the platform code.
             */
            if ((interface == PHY_INTERFACE_MODE_RGMII_ID) ||
                    (interface == PHY_INTERFACE_MODE_RGMII_TXID) ||
                    (interface == PHY_INTERFACE_MODE_RGMII_RXID))
                return interface;

            return PHY_INTERFACE_MODE_RGMII;
        }
    }

    if (priv->flags & TSEC_GIGABIT)
        return PHY_INTERFACE_MODE_GMII;

    return PHY_INTERFACE_MODE_MII;
}


/* Discover which PHY is attached to the device, and configure it
 * properly.  If the PHY is not recognized, then return 0
 * (failure).  Otherwise, return 1
 */
//---phy初始化函数-----------------------------------------------------
//功能：检查PHY链路连接状态，并配置phy寄存器
//参数：struct tsec_info_struct *tsec_info，tsec网口配置结构体
//返回：PHY设备正常返回1，否则0
//---------------------------------------------------------------------
bool_t __TsecInitPhy(struct tsec_info_struct *tsec_info)
{
    struct tsec_private *priv = (struct tsec_private *)tsec_info->priv;
    tsec_t *regs = priv->regs;
    u32  supported = (SUPPORTED_10baseT_Half |
                               SUPPORTED_10baseT_Full |
                               SUPPORTED_100baseT_Half |
                               SUPPORTED_100baseT_Full);

    if (priv->flags & TSEC_GIGABIT)
        supported |= SUPPORTED_1000baseT_Full;

    /* Assign a Physical address to the TBI */
    out_be32(&regs->tbipa, CONFIG_SYS_TBIPA_VALUE);

    priv->interface = __TsecGetInterface(priv);

    if (priv->interface == PHY_INTERFACE_MODE_SGMII)
        __TsecSerdesConfig(priv);

    /* Wait 15ms to make sure the PHY has come out of hard reset */
    Djy_DelayUs(15000);

    //config the PHY
    if(priv->interface == PHY_INTERFACE_MODE_GMII)//eTSEC1,RMII
    {
        /* Soft Reset the PHY */
        phy_reset(priv->phyaddr);
//        extern s32 phy_config_ar8021(s32 addr,s32 supported);
//        phy_config_ar8021(priv->phyaddr,supported);
    }
    else if(priv->interface == PHY_INTERFACE_MODE_RGMII)//eTSEC3,RGMII
    {
//        phy_reset_88e1512(priv->phyaddr);
//        phy_config_88e1512(priv->phyaddr,supported);
    }
    else if(priv->interface == PHY_INTERFACE_MODE_SGMII)//eTSEC2,SGMII
    {
        //eTSEC2直接接到光纤，没经过phy
    }
    return 1;
}
static struct tsec_private sgTsecPrivate[CN_TSEC_NUM]= {{0}};

u8 sgMacAddr[CN_TSEC_NUM][6]=
{
    {0x00,0x01,0x02,0x03,0x04,0x00},
    {0x00,0x01,0x02,0x03,0x04,0x03},
    {0x00,0x01,0x02,0x03,0x04,0x03},
};
tagHostIpv4Addr sgIpv4Addr[CN_TSEC_NUM]=
{
    {
        .ip  =  0xC0A80101,
        .ipmsk = 0xFFFFFF00,
        .gateway = 0xC0A80101,
    },
    {
        .ip  =  0xC0A80102,
        .ipmsk = 0xFFFFFF00,
        .gateway = 0xC0A80101,
    },
    {
        .ip  =  0xC0A80103,
        .ipmsk = 0xFFFFFF00,
        .gateway = 0xC0A80101,
    },
};
static tagNetDev *pgTsecNetDev[CN_TSEC_NUM]= {NULL,NULL,NULL};
static tagRoutTab * sgTsecNoMapRout[CN_TSEC_NUM] = {NULL,NULL,NULL};
static ufast_t sgTsecIrqNo[CN_TSEC_NUM][2] = {
        {cn_int_line_etsec1_g0_receive,cn_int_line_etsec1_g0_transmit},
        {cn_int_line_etsec2_g0_receive,cn_int_line_etsec2_g0_transmit},
        {cn_int_line_etsec3_g0_receive,cn_int_line_etsec3_g0_transmit},
};
// =============================================================================
// 函数功能: __TsecDataInitialize
//          网卡的初始化模块
// 输入参数：num,第几块网卡(0,1,2等)
// 输出参数：无
// 返回值  ：true成功 false失败
// 说明    ：
// =============================================================================
bool_t __TsecDataInitialize(u8  devno)
{

    struct tsec_info_struct *tsec_info;
    struct tsec_private *priv;

    tsec_info = &sgTsecInfo[devno];
    priv = &sgTsecPrivate[devno];

    priv->regs = tsec_info->regs;
    priv->phyregs_sgmii = tsec_info->miiregs_sgmii;

    priv->phyaddr = tsec_info->phyaddr;
    priv->flags = tsec_info->flags;
    sprintf(priv->name, tsec_info->devname);
    priv->interface = tsec_info->interface;
    tsec_info->priv = priv;

    /* Tell u-boot to get the addr from the env */
    tsec_info->enetaddr[0] = sgMacAddr[devno][0];
    tsec_info->enetaddr[1] = sgMacAddr[devno][1];
    tsec_info->enetaddr[2] = sgMacAddr[devno][2];
    tsec_info->enetaddr[3] = sgMacAddr[devno][3];
    tsec_info->enetaddr[4] = sgMacAddr[devno][4];
    tsec_info->enetaddr[5] = sgMacAddr[devno][5];

    tsec_info->devno = devno;

    tsec_info->sndpkgbufhead = NULL;
    tsec_info->sndpkgbuftail = NULL;


    /* Reset the MAC */
    setbits_be32(&priv->regs->maccfg1, MACCFG1_SOFT_RESET);
    Djy_DelayUs(2);  /* Soft Reset must be asserted for 3 TX clocks */
    clrbits_be32(&priv->regs->maccfg1, MACCFG1_SOFT_RESET);

    /* Try to initialize PHY here, and return */
    return __TsecInitPhy(tsec_info);
}
// =============================================================================
// 函数功能: __TsecFreeBD()
// 输入参数：
// 输出参数：
// 返回值  ：
// 说明    ：when we snd some new data, we should check any data that BD has been
//        snt, if any snt, we just free it
// =============================================================================
//void __TsecFreeUsedPkg(tagTsecBdTab *txbdtab)
//{
//  u32 curbd;
//  curbd = txbdtab->usedtxbd;
//  while(curbd != txbdtab->nxttxbd)
//  {
//      if(txbdtab->txbd[curbd].status &TXBD_READY)
//      {
//          break;
//      }
//      else
//      {
//          Pkg_PartFlagFree(txbdtab->txpkg[curbd]);
//          txbdtab->txpkg[curbd]= NULL;
//          curbd =(curbd+1)%CN_TXBD_NUM;
//      }
//  }
//  txbdtab->usedtxbd = curbd;
//}
// =============================================================================
// 函数功能: __TsecCheckSndBdValid()
//          net dev snd function
// 输入参数：txbdtab ,the dev db tab to be checked
//          referednum, the num to be checked
// 输出参数：
// 返回值  ：true if there is more than referednum bd to be used, otherwise false
// 说明    ：
// =============================================================================
bool_t __TsecCheckSndBdValid(tagTsecBdTab *txbdtab, u32 referednum)
{
    bool_t result;
    u32 checktimes ;
    u32 curbd;

    result = false;
    curbd = txbdtab->nxttxbd;
    checktimes = 0;
    while(checktimes < referednum)
    {
        if(txbdtab->txbd[curbd].status &TXBD_READY)
        {
            break;
        }
        else
        {
            Pkg_PartFlagFree(txbdtab->txpkg[curbd]);
            txbdtab->txpkg[curbd]= NULL;
            curbd =(curbd+1)%CN_TXBD_NUM;
            checktimes++;
        }
    }
    if(checktimes == referednum)
    {
        result = true;
    }
    return result;
}

// =============================================================================
// 函数功能: __TsecSnd()
//          net dev snd function
// 输入参数：devindex, the net dev id in the net stack
//          plst, the pkg list to snd
//          netdevtask,the task that the netdev must do
// 输出参数：
// 返回值  ：true if succeed else false
// 说明    ：we must make it clear that because snd function maybe called at any time
//        in any service thread, so we must make sure to protect the common resource
//        when we snd;anyway ,if we want to use the snd chksum of tsec, then we
//        must add an fcb before the fram buf
// =============================================================================

static u32 sgWaitTimes = 0;
#ifdef __GNUC__
static tagTsecFcb sgTsecTcpIpFcb  __attribute__ ((aligned(8)));
static tagTsecFcb sgTsecIpFcb   __attribute__ ((aligned(8)));
#else
#error "rtx must be 64-bit aligned"
#endif
bool_t __TsecSnd(tagNetDev *netdev,tagNetPkg * plst, u32 netdevtask)
{
    u8  devno;
    atom_low_t atomop;
    struct tsec_info_struct *tsec_info;
    tsec_t *regs;
    tagTsecBdTab *txbdtab;
    u32 cursndbd;
    u16 status;
    u32 fbdno;
    tagNetPkg * volatile pkg;
    u32 bufaddr;
    u16 datalen;

    devno = (u8)NetDev_GetDevPrivate(netdev);
    tsec_info = &sgTsecInfo[devno];
    regs = tsec_info->regs;
    //must be sync with the snd isr, so we use the asyn signal to make it sync
    atomop = Int_LowAtomStart();
    //if the pkg snd channel is busy, then add it to the sndbuf queue,
    //else move it to the dev snd bd buf, make sure we have enough valid bd
    //when move it to the dev snd bd buf.
    txbdtab =  &sgTsecBDTab[devno];
//  __TsecFreeUsedPkg(txbdtab);
    while(false ==__TsecCheckSndBdValid(txbdtab,6))
    {
        sgWaitTimes++;
    }
    //now move the pkg to the bd
    cursndbd = txbdtab->nxttxbd;
    pkg = plst;
    fbdno = cursndbd;
    //the first bd status must be set at last
    if((netdevtask&CN_IPDEV_TCPOCHKSUM)&&(netdevtask&CN_IPDEV_IPOCHKSUM))
    {
        //this means that we should do the tcp and ip checksum
        cursndbd = (cursndbd +1)%CN_TXBD_NUM;
        while(NULL != pkg)
        {
            status = TXBD_READY;
            if(NULL == pkg->partnext)
            {
                //this is the last one
                status |= TXBD_LAST ;
            }
            if(CN_TXBD_NUM == (cursndbd+1))
            {
                //comes to the end of the bd tab
                status |= TXBD_WRAP;
            }
            bufaddr = (u32)(pkg->buf+pkg->offset);
            datalen = pkg->datalen;
            //begin to move the sndpkgbuf to the bd
            txbdtab->txbd[cursndbd].bufPtr = bufaddr;
            txbdtab->txbd[cursndbd].length = datalen;
            txbdtab->txbd[cursndbd].status = status;
            //free the old pkg and set it to the new pkg for the bdtab
            txbdtab->txpkg[cursndbd] = pkg;
            cursndbd = (cursndbd +1)%CN_TXBD_NUM;
            pkg = pkg->partnext;
        }
        txbdtab->nxttxbd = cursndbd;
        //the first bd was used to pkg the fcb
        //the bd is free now;
        status = TXBD_CRC| TXBD_UNDERRUN| TXBD_READY;
        if(CN_TXBD_NUM == (fbdno+1))
        {
            //comes to the end of the bd tab
            status |= TXBD_WRAP;
        }
        txbdtab->txbd[fbdno].bufPtr = (u32)(&sgTsecTcpIpFcb);
        txbdtab->txbd[fbdno].length = sizeof(sgTsecTcpIpFcb);
        txbdtab->txbd[fbdno].status = status;
    }
    else if(netdevtask&CN_IPDEV_IPOCHKSUM)
    {
        //this means that we should do the ip checksum
        cursndbd = (cursndbd +1)%CN_TXBD_NUM;
        while(NULL != pkg)
        {
            status = TXBD_READY;
            if(NULL == pkg->partnext)
            {
                //this is the last one
                status |= TXBD_LAST ;
            }
            if(CN_TXBD_NUM == (cursndbd+1))
            {
                //comes to the end of the bd tab
                status |= TXBD_WRAP;
            }
            bufaddr = (u32)(pkg->buf+pkg->offset);
            datalen = pkg->datalen;
            if(0 == datalen)
            {
                printk("Driver Binggo!\n\r");
            }
            //begin to move the sndpkgbuf to the bd
            txbdtab->txbd[cursndbd].bufPtr = bufaddr;
            txbdtab->txbd[cursndbd].length = datalen;
            txbdtab->txbd[cursndbd].status = status;
            //free the old pkg and set it to the new pkg for the bdtab
            txbdtab->txpkg[cursndbd] = pkg;
            cursndbd = (cursndbd +1)%CN_TXBD_NUM;
            pkg = pkg->partnext;
        }
        txbdtab->nxttxbd = cursndbd;
        //the first bd was used to pkg the fcb
        //the bd is free now;
        status = TXBD_CRC| TXBD_UNDERRUN| TXBD_READY;
        if(CN_TXBD_NUM == (fbdno+1))
        {
            //comes to the end of the bd tab
            status |= TXBD_WRAP;
        }
        txbdtab->txbd[fbdno].bufPtr = (u32)(&sgTsecIpFcb);
        txbdtab->txbd[fbdno].length = sizeof(sgTsecIpFcb);
        txbdtab->txbd[fbdno].status = status;
    }
    else
    {
        //this means that we do nothing to help the stack but to snd the
        //pkglst
        pkg=plst;
        status = TXBD_CRC| TXBD_READY;
        while(NULL != pkg)
        {
            if(NULL == pkg->partnext)
            {
                //this is the last one
                status |= TXBD_LAST ;
            }
            if(CN_TXBD_NUM == (cursndbd+1))
            {
                //comes to the end of the bd tab
                status |= TXBD_WRAP;
            }
            bufaddr = (u32)(pkg->buf+pkg->offset);
            datalen = pkg->datalen;
            if(0 == datalen)
            {
                printk("Driver Binggo!\n\r");
            }
            //begin to move the sndpkgbuf to the bd
            txbdtab->txbd[cursndbd].bufPtr = bufaddr;
            txbdtab->txbd[cursndbd].length = datalen;
            txbdtab->txbd[cursndbd].status = status;
            //free the old pkg and set it to the new pkg for the bdtab
            txbdtab->txpkg[cursndbd] = pkg;
            cursndbd = (cursndbd +1)%CN_TXBD_NUM;
            pkg = pkg->partnext;
            status = TXBD_READY;
        }
        txbdtab->nxttxbd = cursndbd;
    }
    //tell the dma to go
    out_be32(&regs->tstat, TSTAT_CLEAR_THALT);
    Int_LowAtomEnd(atomop);

    return true;
}


// =============================================================================
// 函数功能: __TsecSndIsr
//          net dev snd interrupt handle
// 输入参数：irqno
// 输出参数：
// 返回值  ：
// 说明    ：
// =============================================================================
u32 __TsecSndIsr(ufast_t irqno)
{
    u8  devno;
    u32 value;
    struct tsec_info_struct *tsec_info;
    struct tsec_private *priv;
    tsec_t *regs;

    switch(irqno)
    {
        case cn_int_line_etsec1_g0_transmit:
            devno =0;
            break;
        case cn_int_line_etsec2_g0_transmit:
            devno =1;
            break;
        case cn_int_line_etsec3_g0_transmit:
            devno =2;
            break;
        default:
            devno = CN_TSEC_NUM;
            break;
    }
    if(devno != CN_TSEC_NUM)
    {
        tsec_info = &sgTsecInfo[devno];
        priv = (struct tsec_private *)tsec_info->priv;
        regs = priv->regs;
        //clear the irq bits
        value = in_be32(&regs->ievent);
//      value &=(IEVENT_RXB0|IEVENT_RXF0);
        out_be32(&regs->ievent, value);
    }

    return 0;
}


// =============================================================================
// 函数功能: __TsecRcv()
//          net dev rcv function
// 输入参数：devindex, the net dev id in the net stack
// 输出参数：
// 返回值  ：the pkg that rcved, NULL if no data in the netdev
// 说明    ：because before we begin to read the data, we have turn off the rcv
//        interrupt, there is no need to worry about the protect in the rcv process
// =============================================================================
tagNetPkg *__TsecRcv(u8 devno)
{
    u16 status;
    u32 rcvbdno;
    tagNetPkg *result;
    rxbd8_t *rcvbd;
    u8 *dst;
    u8 *src;

    result = NULL;

    rcvbdno = sgTsecBDTab[devno].nxtrxbd;
    rcvbd = &sgTsecBDTab[devno].rxbd[rcvbdno];
    if(!(rcvbd->status & RXBD_EMPTY))
    {
        //got some normal data or some error data
        if (!(rcvbd->status & RXBD_STATS))
        {
            //got the normal data, we could mmove it and alloc an new pkg to this buf
            result = Pkg_Alloc(rcvbd->length,CN_PKGFLAG_FREE);
            if(NULL == result)
            {
                printk("No mem for the etsec to replace!\n\r");
            }
            else
            {
                //CPY THE BUF TO THE PKG WE ALLOC
                dst = (u8 *)(result->buf + result->offset);
                src =(u8 *) rcvbd->bufPtr;
                memcpy(dst,src,rcvbd->length);
                result->datalen = rcvbd->length;
            }
        }
        else
        {
            printk("Got error %x\n",(rcvbd->status & RXBD_STATS));
        }
        //set the status
        rcvbd->length = 0;
        status = RXBD_EMPTY|RXBD_INTERRUPT;
        if((rcvbdno+1) == CN_RXBD_NUM)
        {
            status |= RXBD_WRAP;
        }
        rcvbd->status = status;
        sgTsecBDTab[devno].nxtrxbd = (rcvbdno+1)%CN_RXBD_NUM;
    }
    return result;
}
// =============================================================================
// 函数功能: __TsecRcvIsr
//          net dev rcv interrupt handle
// 输入参数：irqno
// 输出参数：
// 返回值  ：
// 说明    ：
// =============================================================================
u32 __TsecRcvIsr(ufast_t irqno)
{
    u8  devno;
    tsec_t *regs;
    u32 value;
    tagNetPkg  *pkglst;

    switch(irqno)
    {
        case cn_int_line_etsec1_g0_receive:
            devno =0;
            break;
        case cn_int_line_etsec2_g0_receive:
            devno =1;
            break;
        case cn_int_line_etsec3_g0_receive:
            devno =2;
            break;
        default:
            devno = CN_TSEC_NUM;
            break;
    }

    if(devno != CN_TSEC_NUM)
    {
        while ((pkglst=__TsecRcv(devno)) != NULL)
        {
            if(false == NetDev_PostPkg(pgTsecNetDev[devno],pkglst))
            {
                Pkg_LstFlagFree(pkglst);
            }
        }

        //anyway, we must clear the interrupt msk
        regs = sgTsecInfo[devno].regs;

        if (in_be32(&regs->ievent) & IEVENT_BSY)
        {
            out_be32(&regs->ievent, IEVENT_BSY);
            value = in_be32(&regs->rstat);
            out_be32(&regs->rstat, value);
        }
        //clear the irq bits
        value =IEVENT_RXF0;
        out_be32(&regs->ievent, value);
    }

    return 0;
}

// =============================================================================
// 函数功能: module_NetHardDev_init
//          网卡的初始化模块
// 输入参数：para,第几块网卡(0,1,2等)
// 输出参数：无
// 返回值  ：true成功 false失败
// 说明    ：
// =============================================================================
bool_t ModuleInstall_Tsec(ptu32_t para)
{
    bool_t result;
    u8  devno;
    tagNetDevPara  devpara;

    result = false;
    devno = para;

    sgTsecTcpIpFcb.status = CN_FCB_IP|CN_FCB_TUP|CN_FCB_CIP|CN_FCB_CTU;
    sgTsecTcpIpFcb.l3os = 14;
    sgTsecTcpIpFcb.l4os = 20;
    sgTsecTcpIpFcb.phcs = 0;
    sgTsecTcpIpFcb.vlctl = 0;

    sgTsecIpFcb.status = CN_FCB_IP|CN_FCB_CIP;
    sgTsecIpFcb.l3os = 14;
    sgTsecIpFcb.l4os = 0;
    sgTsecIpFcb.phcs = 0;
    sgTsecIpFcb.vlctl = 0;

    if(devno < CN_TSEC_NUM)
    {
        //config the tsec it self
        if(__TsecDataInitialize(devno))
        {
            //config the tsec hard interface
            if(__TsecInit(devno))
            {
                //communicate the driver isr with interrupt
                //rcv interrupt
                Int_IsrConnect(sgTsecIrqNo[devno][0],__TsecRcvIsr);
                Int_SettoAsynSignal(sgTsecIrqNo[devno][0]);
                Int_SetLineTrigerType(sgTsecIrqNo[devno][0],EN_INT_TRIGER_HIGHLEVEL);
                Int_ContactLine(sgTsecIrqNo[devno][0]);
                //snd interrupt
                Int_IsrConnect(sgTsecIrqNo[devno][1],__TsecSndIsr);
                Int_SettoAsynSignal(sgTsecIrqNo[devno][1]);
                Int_SetLineTrigerType(sgTsecIrqNo[devno][1],EN_INT_TRIGER_HIGHLEVEL);
                Int_ContactLine(sgTsecIrqNo[devno][1]);

                //register the dev into the netstack
                //register a rout for communicating
                devpara.ifsend = __TsecSnd;
                devpara.linklen = 14;
                devpara.pkglen = 1500;
                devpara.devfunc = CN_IPDEV_TCPOCHKSUM|CN_IPDEV_IPOCHKSUM;

                devpara.iftype = EN_LINK_INTERFACE_ETHERNET;
                //TCP 收发校验和 IP收发校验和
                memcpy(devpara.mac, sgMacAddr[devno],6);
                devpara.name = "TsecDriver";
                devpara.private = devno;

                pgTsecNetDev[devno] = NetDev_AddDev(&devpara);
                if(pgTsecNetDev[devno]!= NULL)
                {
                    sgTsecNoMapRout[devno] = Rout_AddRout(pgTsecNetDev[devno], \
                                             &sgIpv4Addr[devno]);
                    if(NULL != sgTsecNoMapRout[devno])
                    {
                        result = true;
                    }
                }
            }
        }
    }

    return result;
}


