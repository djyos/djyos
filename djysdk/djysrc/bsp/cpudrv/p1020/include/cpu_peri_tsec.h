//----------------------------------------------------
//Copyright (C), 2013-2020,  luoshitian.
//版权所有 (C), 2013-2020,   罗侍田.
//所属模块: dma驱动模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: tsec网口驱动文件
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-11-14
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

#include "config-prj.h"
#ifndef __TSEC_H__
#define __TSEC_H__

/* tsec */

#define CONFIG_TSEC1    1
#define CONFIG_TSEC1_NAME    "eTSEC1"
#define CONFIG_TSEC2    1
#define CONFIG_TSEC2_NAME    "eTSEC2"
#define CONFIG_TSEC3    1
#define CONFIG_TSEC3_NAME    "eTSEC3"
#define CONFIG_TSEC4    0
#define CONFIG_TSEC4_NAME    "eTSEC3"

#define TSEC1_PHY_ADDR    2
#define TSEC2_PHY_ADDR    0
#define TSEC3_PHY_ADDR    0

#define TSEC1_FLAGS    (TSEC_GIGABIT | TSEC_REDUCED)
#define TSEC2_FLAGS    (TSEC_GIGABIT | TSEC_SGMII)
#define TSEC3_FLAGS    (TSEC_GIGABIT | TSEC_REDUCED)

#define TSEC1_PHYIDX    0
#define TSEC2_PHYIDX    0
#define TSEC3_PHYIDX    0

#define CONFIG_ETHPRIME    "eTSEC1"

#define CONFIG_PHY_GIGE    1    /* Include GbE speed/duplex detection */

#define CONFIG_HAS_ETH0
#define CONFIG_HAS_ETH1
#define CONFIG_HAS_ETH2


typedef struct ccsr_tsec
{
    u8    res1[16];
    u32    ievent;        /* IRQ Event */
    u32    imask;        /* IRQ Mask */
    u32    edis;        /* Error Disabled */
    u8    res2[4];
    u32    ecntrl;        /* Ethernet Control */
    u32    minflr;        /* Minimum Frame Len */
    u32    ptv;        /* Pause Time Value */
    u32    dmactrl;    /* DMA Control */
    u32    tbipa;        /* TBI PHY Addr */
    u8    res3[88];
    u32    fifo_tx_thr;        /* FIFO transmit threshold */
    u8    res4[8];
    u32    fifo_tx_starve;        /* FIFO transmit starve */
    u32    fifo_tx_starve_shutoff;    /* FIFO transmit starve shutoff */
    u8    res5[96];
    u32    tctrl;        /* TX Control */
    u32    tstat;        /* TX Status */
    u8    res6[4];
    u32    tbdlen;        /* TX Buffer Desc Data Len */
    u8    res7[16];
    u32    ctbptrh;    /* Current TX Buffer Desc Ptr High */
    u32    ctbptr;        /* Current TX Buffer Desc Ptr */
    u8    res8[88];
    u32    tbptrh;        /* TX Buffer Desc Ptr High */
    u32    tbptr;        /* TX Buffer Desc Ptr Low */
    u8    res9[120];
    u32    tbaseh;        /* TX Desc Base Addr High */
    u32    tbase;        /* TX Desc Base Addr */
    u8    res10[168];
    u32    ostbd;        /* Out-of-Sequence(OOS) TX Buffer Desc */
    u32    ostbdp;        /* OOS TX Data Buffer Ptr */
    u32    os32tbdp;    /* OOS 32 Bytes TX Data Buffer Ptr Low */
    u32    os32iptrh;    /* OOS 32 Bytes TX Insert Ptr High */
    u32    os32iptrl;    /* OOS 32 Bytes TX Insert Ptr Low */
    u32    os32tbdr;    /* OOS 32 Bytes TX Reserved */
    u32    os32iil;    /* OOS 32 Bytes TX Insert Idx/Len */
    u8    res11[52];
    u32    rctrl;        /* RX Control */
    u32    rstat;        /* RX Status */
    u8    res12[4];
    u32    rbdlen;        /* RxBD Data Len */
    u8    res13[16];
    u32    crbptrh;    /* Current RX Buffer Desc Ptr High */
    u32    crbptr;        /* Current RX Buffer Desc Ptr */
    u8    res14[24];
    u32    mrblr;        /* Maximum RX Buffer Len */
    u32    mrblr2r3;    /* Maximum RX Buffer Len R2R3 */
    u8    res15[56];
    u32    rbptrh;        /* RX Buffer Desc Ptr High 0 */
    u32    rbptr;        /* RX Buffer Desc Ptr */
    u32    rbptrh1;    /* RX Buffer Desc Ptr High 1 */
    u32    rbptrl1;    /* RX Buffer Desc Ptr Low 1 */
    u32    rbptrh2;    /* RX Buffer Desc Ptr High 2 */
    u32    rbptrl2;    /* RX Buffer Desc Ptr Low 2 */
    u32    rbptrh3;    /* RX Buffer Desc Ptr High 3 */
    u32    rbptrl3;    /* RX Buffer Desc Ptr Low 3 */
    u8    res16[96];
    u32    rbaseh;        /* RX Desc Base Addr High 0 */
    u32    rbase;        /* RX Desc Base Addr */
    u32    rbaseh1;    /* RX Desc Base Addr High 1 */
    u32    rbasel1;    /* RX Desc Base Addr Low 1 */
    u32    rbaseh2;    /* RX Desc Base Addr High 2 */
    u32    rbasel2;    /* RX Desc Base Addr Low 2 */
    u32    rbaseh3;    /* RX Desc Base Addr High 3 */
    u32    rbasel3;    /* RX Desc Base Addr Low 3 */
    u8    res17[224];
    u32    maccfg1;    /* MAC Configuration 1 */
    u32    maccfg2;    /* MAC Configuration 2 */
    u32    ipgifg;        /* Inter Packet Gap/Inter Frame Gap */
    u32    hafdup;        /* Half Duplex */
    u32    maxfrm;        /* Maximum Frame Len */
    u8    res18[12];
    u32    miimcfg;    /* MII Management Configuration */
    u32    miimcom;    /* MII Management Cmd */
    u32    miimadd;    /* MII Management Addr */
    u32    miimcon;    /* MII Management Control */
    u32    miimstat;    /* MII Management Status */
    u32    miimind;    /* MII Management Indicator */
    u8    res19[4];
    u32    ifstat;        /* Interface Status */
    u32    macstnaddr1;    /* Station Addr Part 1 */
    u32    macstnaddr2;    /* Station Addr Part 2 */
    u8    res20[312];
    u32    tr64;        /* TX & RX 64-byte Frame Counter */
    u32    tr127;        /* TX & RX 65-127 byte Frame Counter */
    u32    tr255;        /* TX & RX 128-255 byte Frame Counter */
    u32    tr511;        /* TX & RX 256-511 byte Frame Counter */
    u32    tr1k;        /* TX & RX 512-1023 byte Frame Counter */
    u32    trmax;        /* TX & RX 1024-1518 byte Frame Counter */
    u32    trmgv;        /* TX & RX 1519-1522 byte Good VLAN Frame */
    u32    rbyt;        /* RX Byte Counter */
    u32    rpkt;        /* RX Packet Counter */
    u32    rfcs;        /* RX FCS Error Counter */
    u32    rmca;        /* RX Multicast Packet Counter */
    u32    rbca;        /* RX Broadcast Packet Counter */
    u32    rxcf;        /* RX Control Frame Packet Counter */
    u32    rxpf;        /* RX Pause Frame Packet Counter */
    u32    rxuo;        /* RX Unknown OP Code Counter */
    u32    raln;        /* RX Alignment Error Counter */
    u32    rflr;        /* RX Frame Len Error Counter */
    u32    rcde;        /* RX Code Error Counter */
    u32    rcse;        /* RX Carrier Sense Error Counter */
    u32    rund;        /* RX Undersize Packet Counter */
    u32    rovr;        /* RX Oversize Packet Counter */
    u32    rfrg;        /* RX Fragments Counter */
    u32    rjbr;        /* RX Jabber Counter */
    u32    rdrp;        /* RX Drop Counter */
    u32    tbyt;        /* TX Byte Counter Counter */
    u32    tpkt;        /* TX Packet Counter */
    u32    tmca;        /* TX Multicast Packet Counter */
    u32    tbca;        /* TX Broadcast Packet Counter */
    u32    txpf;        /* TX Pause Control Frame Counter */
    u32    tdfr;        /* TX Deferral Packet Counter */
    u32    tedf;        /* TX Excessive Deferral Packet Counter */
    u32    tscl;        /* TX Single Collision Packet Counter */
    u32    tmcl;        /* TX Multiple Collision Packet Counter */
    u32    tlcl;        /* TX Late Collision Packet Counter */
    u32    txcl;        /* TX Excessive Collision Packet Counter */
    u32    tncl;        /* TX Total Collision Counter */
    u8    res21[4];
    u32    tdrp;        /* TX Drop Frame Counter */
    u32    tjbr;        /* TX Jabber Frame Counter */
    u32    tfcs;        /* TX FCS Error Counter */
    u32    txcf;        /* TX Control Frame Counter */
    u32    tovr;        /* TX Oversize Frame Counter */
    u32    tund;        /* TX Undersize Frame Counter */
    u32    tfrg;        /* TX Fragments Frame Counter */
    u32    car1;        /* Carry One */
    u32    car2;        /* Carry Two */
    u32    cam1;        /* Carry Mask One */
    u32    cam2;        /* Carry Mask Two */
    u8    res22[192];
    u32    iaddr0;        /* Indivdual addr 0 */
    u32    iaddr1;        /* Indivdual addr 1 */
    u32    iaddr2;        /* Indivdual addr 2 */
    u32    iaddr3;        /* Indivdual addr 3 */
    u32    iaddr4;        /* Indivdual addr 4 */
    u32    iaddr5;        /* Indivdual addr 5 */
    u32    iaddr6;        /* Indivdual addr 6 */
    u32    iaddr7;        /* Indivdual addr 7 */
    u8    res23[96];
    u32    gaddr0;        /* Global addr 0 */
    u32    gaddr1;        /* Global addr 1 */
    u32    gaddr2;        /* Global addr 2 */
    u32    gaddr3;        /* Global addr 3 */
    u32    gaddr4;        /* Global addr 4 */
    u32    gaddr5;        /* Global addr 5 */
    u32    gaddr6;        /* Global addr 6 */
    u32    gaddr7;        /* Global addr 7 */
    u8    res24[96];
    u32    pmd0;        /* Pattern Match Data */
    u8    res25[4];
    u32    pmask0;        /* Pattern Mask */
    u8    res26[4];
    u32    pcntrl0;    /* Pattern Match Control */
    u8    res27[4];
    u32    pattrb0;    /* Pattern Match Attrs */
    u32    pattrbeli0;    /* Pattern Match Attrs Extract Len & Idx */
    u32    pmd1;        /* Pattern Match Data */
    u8    res28[4];
    u32    pmask1;        /* Pattern Mask */
    u8    res29[4];
    u32    pcntrl1;    /* Pattern Match Control */
    u8    res30[4];
    u32    pattrb1;    /* Pattern Match Attrs */
    u32    pattrbeli1;    /* Pattern Match Attrs Extract Len & Idx */
    u32    pmd2;        /* Pattern Match Data */
    u8    res31[4];
    u32    pmask2;        /* Pattern Mask */
    u8    res32[4];
    u32    pcntrl2;    /* Pattern Match Control */
    u8    res33[4];
    u32    pattrb2;    /* Pattern Match Attrs */
    u32    pattrbeli2;    /* Pattern Match Attrs Extract Len & Idx */
    u32    pmd3;        /* Pattern Match Data */
    u8    res34[4];
    u32    pmask3;        /* Pattern Mask */
    u8    res35[4];
    u32    pcntrl3;    /* Pattern Match Control */
    u8    res36[4];
    u32    pattrb3;    /* Pattern Match Attrs */
    u32    pattrbeli3;    /* Pattern Match Attrs Extract Len & Idx */
    u32    pmd4;        /* Pattern Match Data */
    u8    res37[4];
    u32    pmask4;        /* Pattern Mask */
    u8    res38[4];
    u32    pcntrl4;    /* Pattern Match Control */
    u8    res39[4];
    u32    pattrb4;    /* Pattern Match Attrs */
    u32    pattrbeli4;    /* Pattern Match Attrs Extract Len & Idx */
    u32    pmd5;        /* Pattern Match Data */
    u8    res40[4];
    u32    pmask5;        /* Pattern Mask */
    u8    res41[4];
    u32    pcntrl5;    /* Pattern Match Control */
    u8    res42[4];
    u32    pattrb5;    /* Pattern Match Attrs */
    u32    pattrbeli5;    /* Pattern Match Attrs Extract Len & Idx */
    u32    pmd6;        /* Pattern Match Data */
    u8    res43[4];
    u32    pmask6;        /* Pattern Mask */
    u8    res44[4];
    u32    pcntrl6;    /* Pattern Match Control */
    u8    res45[4];
    u32    pattrb6;    /* Pattern Match Attrs */
    u32    pattrbeli6;    /* Pattern Match Attrs Extract Len & Idx */
    u32    pmd7;        /* Pattern Match Data */
    u8    res46[4];
    u32    pmask7;        /* Pattern Mask */
    u8    res47[4];
    u32    pcntrl7;    /* Pattern Match Control */
    u8    res48[4];
    u32    pattrb7;    /* Pattern Match Attrs */
    u32    pattrbeli7;    /* Pattern Match Attrs Extract Len & Idx */
    u32    pmd8;        /* Pattern Match Data */
    u8    res49[4];
    u32    pmask8;        /* Pattern Mask */
    u8    res50[4];
    u32    pcntrl8;    /* Pattern Match Control */
    u8    res51[4];
    u32    pattrb8;    /* Pattern Match Attrs */
    u32    pattrbeli8;    /* Pattern Match Attrs Extract Len & Idx */
    u32    pmd9;        /* Pattern Match Data */
    u8    res52[4];
    u32    pmask9;        /* Pattern Mask */
    u8    res53[4];
    u32    pcntrl9;    /* Pattern Match Control */
    u8    res54[4];
    u32    pattrb9;    /* Pattern Match Attrs */
    u32    pattrbeli9;    /* Pattern Match Attrs Extract Len & Idx */
    u32    pmd10;        /* Pattern Match Data */
    u8    res55[4];
    u32    pmask10;    /* Pattern Mask */
    u8    res56[4];
    u32    pcntrl10;    /* Pattern Match Control */
    u8    res57[4];
    u32    pattrb10;    /* Pattern Match Attrs */
    u32    pattrbeli10;    /* Pattern Match Attrs Extract Len & Idx */
    u32    pmd11;        /* Pattern Match Data */
    u8    res58[4];
    u32    pmask11;    /* Pattern Mask */
    u8    res59[4];
    u32    pcntrl11;    /* Pattern Match Control */
    u8    res60[4];
    u32    pattrb11;    /* Pattern Match Attrs */
    u32    pattrbeli11;    /* Pattern Match Attrs Extract Len & Idx */
    u32    pmd12;        /* Pattern Match Data */
    u8    res61[4];
    u32    pmask12;    /* Pattern Mask */
    u8    res62[4];
    u32    pcntrl12;    /* Pattern Match Control */
    u8    res63[4];
    u32    pattrb12;    /* Pattern Match Attrs */
    u32    pattrbeli12;    /* Pattern Match Attrs Extract Len & Idx */
    u32    pmd13;        /* Pattern Match Data */
    u8    res64[4];
    u32    pmask13;    /* Pattern Mask */
    u8    res65[4];
    u32    pcntrl13;    /* Pattern Match Control */
    u8    res66[4];
    u32    pattrb13;    /* Pattern Match Attrs */
    u32    pattrbeli13;    /* Pattern Match Attrs Extract Len & Idx */
    u32    pmd14;        /* Pattern Match Data */
    u8    res67[4];
    u32    pmask14;    /* Pattern Mask */
    u8    res68[4];
    u32    pcntrl14;    /* Pattern Match Control */
    u8    res69[4];
    u32    pattrb14;    /* Pattern Match Attrs */
    u32    pattrbeli14;    /* Pattern Match Attrs Extract Len & Idx */
    u32    pmd15;        /* Pattern Match Data */
    u8    res70[4];
    u32    pmask15;    /* Pattern Mask */
    u8    res71[4];
    u32    pcntrl15;    /* Pattern Match Control */
    u8    res72[4];
    u32    pattrb15;    /* Pattern Match Attrs */
    u32    pattrbeli15;    /* Pattern Match Attrs Extract Len & Idx */
    u8    res73[248];
    u32    attr;        /* Attrs */
    u32    attreli;    /* Attrs Extract Len & Idx */
    u8    res74[1024];
} ccsr_tsec_t;
#define CONFIG_TSECV2
#define CONFIG_SYS_TSEC1_OFFSET            0xB0000
#define CONFIG_SYS_MDIO1_OFFSET            0x24000

#define TSEC_BASE_ADDR        (CN_CCSR_NEWADDR_V + CONFIG_SYS_TSEC1_OFFSET)
#define MDIO_BASE_ADDR        (CN_CCSR_NEWADDR_V + CONFIG_SYS_MDIO1_OFFSET)


typedef enum
{
    PHY_INTERFACE_MODE_MII,
    PHY_INTERFACE_MODE_GMII,
    PHY_INTERFACE_MODE_SGMII,
    PHY_INTERFACE_MODE_QSGMII,
    PHY_INTERFACE_MODE_TBI,
    PHY_INTERFACE_MODE_RMII,
    PHY_INTERFACE_MODE_RGMII,
    PHY_INTERFACE_MODE_RGMII_ID,
    PHY_INTERFACE_MODE_RGMII_RXID,
    PHY_INTERFACE_MODE_RGMII_TXID,
    PHY_INTERFACE_MODE_RTBI,
    PHY_INTERFACE_MODE_XGMII,
    PHY_INTERFACE_MODE_NONE    /* Must be last */
} phy_interface_t;
struct tsec_mii_mng
{
    u32 miimcfg;        /* MII management configuration reg */
    u32 miimcom;        /* MII management command reg */
    u32 miimadd;        /* MII management address reg */
    u32 miimcon;        /* MII management control reg */
    u32 miimstat;        /* MII management status reg  */
    u32 miimind;        /* MII management indication reg */
    u32 ifstat;        /* Interface Status Register */
} __attribute__ ((packed));


/*MDIO*/
#define MDIO_PRTAD_NONE            (-1)
#define MDIO_DEVAD_NONE            (-1)

/* Generic MII registers. */

#define MII_BMCR        0x00    /* Basic mode control register */
#define MII_BMSR        0x01    /* Basic mode status register  */
#define MII_PHYSID1        0x02    /* PHYS ID 1               */
#define MII_PHYSID2        0x03    /* PHYS ID 2               */
#define MII_ADVERTISE        0x04    /* Advertisement control reg   */
#define MII_LPA            0x05    /* Link partner ability reg    */
#define MII_EXPANSION        0x06    /* Expansion register           */
#define MII_CTRL1000        0x09    /* 1000BASE-T control           */
#define MII_STAT1000        0x0a    /* 1000BASE-T status           */
#define MII_ESTATUS        0x0f    /* Extended Status */
#define MII_DCOUNTER        0x12    /* Disconnect counter           */
#define MII_FCSCOUNTER        0x13    /* False carrier counter       */
#define MII_NWAYTEST        0x14    /* N-way auto-neg test reg     */
#define MII_RERRCOUNTER     0x15    /* Receive error counter       */
#define MII_SREVISION        0x16    /* Silicon revision           */
#define MII_RESV1        0x17    /* Reserved...               */
#define MII_LBRERROR        0x18    /* Lpback, rx, bypass error    */
#define MII_PHYADDR        0x19    /* PHY address               */
#define MII_RESV2        0x1a    /* Reserved...               */
#define MII_TPISTATUS        0x1b    /* TPI status for 10mbps       */
#define MII_NCONFIG        0x1c    /* Network interface config    */
/* Basic mode control register. */
#define BMCR_RESV        0x003f    /* Unused...               */
#define BMCR_SPEED1000        0x0040    /* MSB of Speed (1000)           */
#define BMCR_CTST        0x0080    /* Collision test           */
#define BMCR_FULLDPLX        0x0100    /* Full duplex               */
#define BMCR_ANRESTART        0x0200    /* Auto negotiation restart    */
#define BMCR_ISOLATE        0x0400    /* Disconnect DP83840 from MII */
#define BMCR_PDOWN        0x0800    /* Powerdown the DP83840       */
#define BMCR_ANENABLE        0x1000    /* Enable auto negotiation     */
#define BMCR_SPEED100        0x2000    /* Select 100Mbps           */
#define BMCR_LOOPBACK        0x4000    /* TXD loopback bits           */
#define BMCR_RESET        0x8000    /* Reset the DP83840           */

/* Basic mode status register. */
#define BMSR_ERCAP        0x0001    /* Ext-reg capability           */
#define BMSR_JCD        0x0002    /* Jabber detected           */
#define BMSR_LSTATUS        0x0004    /* Link status               */
#define BMSR_ANEGCAPABLE    0x0008    /* Able to do auto-negotiation */
#define BMSR_RFAULT        0x0010    /* Remote fault detected       */
#define BMSR_ANEGCOMPLETE    0x0020    /* Auto-negotiation complete   */
#define BMSR_RESV        0x00c0    /* Unused...               */
#define BMSR_ESTATEN        0x0100    /* Extended Status in R15 */
#define BMSR_100HALF2        0x0200    /* Can do 100BASE-T2 HDX */
#define BMSR_100FULL2        0x0400    /* Can do 100BASE-T2 FDX */
#define BMSR_10HALF        0x0800    /* Can do 10mbps, half-duplex  */
#define BMSR_10FULL        0x1000    /* Can do 10mbps, full-duplex  */
#define BMSR_100HALF        0x2000    /* Can do 100mbps, half-duplex */
#define BMSR_100FULL        0x4000    /* Can do 100mbps, full-duplex */
#define BMSR_100BASE4        0x8000    /* Can do 100mbps, 4k packets  */

/* Advertisement control register. */
#define ADVERTISE_SLCT        0x001f    /* Selector bits           */
#define ADVERTISE_CSMA        0x0001    /* Only selector supported     */
#define ADVERTISE_10HALF    0x0020    /* Try for 10mbps half-duplex  */
#define ADVERTISE_1000XFULL    0x0020    /* Try for 1000BASE-X full-duplex */
#define ADVERTISE_10FULL    0x0040    /* Try for 10mbps full-duplex  */
#define ADVERTISE_1000XHALF    0x0040    /* Try for 1000BASE-X half-duplex */
#define ADVERTISE_100HALF    0x0080    /* Try for 100mbps half-duplex */
#define ADVERTISE_1000XPAUSE    0x0080    /* Try for 1000BASE-X pause    */
#define ADVERTISE_100FULL    0x0100    /* Try for 100mbps full-duplex */
#define ADVERTISE_1000XPSE_ASYM 0x0100    /* Try for 1000BASE-X asym pause */
#define ADVERTISE_100BASE4    0x0200    /* Try for 100mbps 4k packets  */
#define ADVERTISE_PAUSE_CAP    0x0400    /* Try for pause           */
#define ADVERTISE_PAUSE_ASYM    0x0800    /* Try for asymetric pause     */
#define ADVERTISE_RESV        0x1000    /* Unused...               */
#define ADVERTISE_RFAULT    0x2000    /* Say we can detect faults    */
#define ADVERTISE_LPACK        0x4000    /* Ack link partners response  */
#define ADVERTISE_NPAGE        0x8000    /* Next page bit           */

#define ADVERTISE_FULL (ADVERTISE_100FULL | ADVERTISE_10FULL | \
            ADVERTISE_CSMA)
#define ADVERTISE_ALL (ADVERTISE_10HALF | ADVERTISE_10FULL | \
               ADVERTISE_100HALF | ADVERTISE_100FULL)

/* 1000BASE-T Control register */
#define ADVERTISE_1000FULL    0x0200    /* Advertise 1000BASE-T full duplex */
#define ADVERTISE_1000HALF    0x0100    /* Advertise 1000BASE-T half duplex */
/* Indicates what features are supported by the interface. */
#define SUPPORTED_10baseT_Half        (1 << 0)
#define SUPPORTED_10baseT_Full        (1 << 1)
#define SUPPORTED_100baseT_Half        (1 << 2)
#define SUPPORTED_100baseT_Full        (1 << 3)
#define SUPPORTED_1000baseT_Half    (1 << 4)
#define SUPPORTED_1000baseT_Full    (1 << 5)
#define SUPPORTED_Autoneg        (1 << 6)
#define SUPPORTED_TP            (1 << 7)
#define SUPPORTED_AUI            (1 << 8)
#define SUPPORTED_MII            (1 << 9)
#define SUPPORTED_FIBRE            (1 << 10)
#define SUPPORTED_BNC            (1 << 11)
#define SUPPORTED_10000baseT_Full    (1 << 12)
#define SUPPORTED_Pause            (1 << 13)
#define SUPPORTED_Asym_Pause        (1 << 14)
#define SUPPORTED_2500baseX_Full    (1 << 15)
#define SUPPORTED_Backplane        (1 << 16)
#define SUPPORTED_1000baseKX_Full    (1 << 17)
#define SUPPORTED_10000baseKX4_Full    (1 << 18)
#define SUPPORTED_10000baseKR_Full    (1 << 19)
#define SUPPORTED_10000baseR_FEC    (1 << 20)
#define PHY_BASIC_FEATURES    (SUPPORTED_10baseT_Half | \
                 SUPPORTED_10baseT_Full | \
                 SUPPORTED_100baseT_Half | \
                 SUPPORTED_100baseT_Full | \
                 SUPPORTED_Autoneg | \
                 SUPPORTED_TP | \
                 SUPPORTED_MII)

#define PHY_GBIT_FEATURES    (PHY_BASIC_FEATURES | \
                 SUPPORTED_1000baseT_Half | \
                 SUPPORTED_1000baseT_Full)

#define PHY_ANEG_TIMEOUT    6000
/* Cicada Auxiliary Control/Status Register */
#define MIIM_CIS82xx_AUX_CONSTAT    0x1c
#define MIIM_CIS82xx_AUXCONSTAT_INIT    0x0004
#define MIIM_CIS82xx_AUXCONSTAT_DUPLEX    0x0020
#define MIIM_CIS82xx_AUXCONSTAT_SPEED    0x0018
#define MIIM_CIS82xx_AUXCONSTAT_GBIT    0x0010
#define MIIM_CIS82xx_AUXCONSTAT_100    0x0008

/* Cicada Extended Control Register 1 */
#define MIIM_CIS82xx_EXT_CON1        0x17
#define MIIM_CIS8201_EXTCON1_INIT    0x0000
/* Indicates what features are advertised by the interface. */
#define ADVERTISED_10baseT_Half        (1 << 0)
#define ADVERTISED_10baseT_Full        (1 << 1)
#define ADVERTISED_100baseT_Half    (1 << 2)
#define ADVERTISED_100baseT_Full    (1 << 3)
#define ADVERTISED_1000baseT_Half    (1 << 4)
#define ADVERTISED_1000baseT_Full    (1 << 5)
#define ADVERTISED_Autoneg        (1 << 6)
#define ADVERTISED_TP            (1 << 7)
#define ADVERTISED_AUI            (1 << 8)
#define ADVERTISED_MII            (1 << 9)
#define ADVERTISED_FIBRE        (1 << 10)
#define ADVERTISED_BNC            (1 << 11)
#define ADVERTISED_10000baseT_Full    (1 << 12)
#define ADVERTISED_Pause        (1 << 13)
#define ADVERTISED_Asym_Pause        (1 << 14)
#define ADVERTISED_2500baseX_Full    (1 << 15)
#define ADVERTISED_Backplane        (1 << 16)
#define ADVERTISED_1000baseKX_Full    (1 << 17)
#define ADVERTISED_10000baseKX4_Full    (1 << 18)
#define ADVERTISED_10000baseKR_Full    (1 << 19)
#define ADVERTISED_10000baseR_FEC    (1 << 20)

/* Link partner ability register. */
#define LPA_SLCT        0x001f    /* Same as advertise selector  */
#define LPA_10HALF        0x0020    /* Can do 10mbps half-duplex   */
#define LPA_1000XFULL        0x0020    /* Can do 1000BASE-X full-duplex */
#define LPA_10FULL        0x0040    /* Can do 10mbps full-duplex   */
#define LPA_1000XHALF        0x0040    /* Can do 1000BASE-X half-duplex */
#define LPA_100HALF        0x0080    /* Can do 100mbps half-duplex  */
#define LPA_1000XPAUSE        0x0080    /* Can do 1000BASE-X pause     */
#define LPA_100FULL        0x0100    /* Can do 100mbps full-duplex  */
#define LPA_1000XPAUSE_ASYM    0x0100    /* Can do 1000BASE-X pause asym*/
#define LPA_100BASE4        0x0200    /* Can do 100mbps 4k packets   */
#define LPA_PAUSE_CAP        0x0400    /* Can pause               */
#define LPA_PAUSE_ASYM        0x0800    /* Can pause asymetrically     */
#define LPA_RESV        0x1000    /* Unused...               */
#define LPA_RFAULT        0x2000    /* Link partner faulted        */
#define LPA_LPACK        0x4000    /* Link partner acked us       */
#define LPA_NPAGE        0x8000    /* Next page bit           */

#define LPA_DUPLEX        (LPA_10FULL | LPA_100FULL)
#define LPA_100            (LPA_100FULL | LPA_100HALF | LPA_100BASE4)

/* phy register offsets */
#define MII_MIPSCR        0x11

/* MII_LPA */
#define PHY_ANLPAR_PSB_802_3    0x0001
#define PHY_ANLPAR_PSB_802_9    0x0002

/* MII_CTRL1000 masks */
#define PHY_1000BTCR_1000FD    0x0200
#define PHY_1000BTCR_1000HD    0x0100

/* MII_STAT1000 masks */
#define PHY_1000BTSR_MSCF    0x8000
#define PHY_1000BTSR_MSCR    0x4000
#define PHY_1000BTSR_LRS    0x2000
#define PHY_1000BTSR_RRS    0x1000
#define PHY_1000BTSR_1000FD    0x0800
#define PHY_1000BTSR_1000HD    0x0400

/* phy EXSR */
#define ESTATUS_1000XF        0x8000
#define ESTATUS_1000XH        0x4000

/* PHY register offsets */
#define PHY_EXT_PAGE_ACCESS    0x1f

/* MII Management Configuration Register */
#define MIIMCFG_RESET_MGMT          0x80000000
#define MIIMCFG_MGMT_CLOCK_SELECT   0x00000007
#define MIIMCFG_INIT_VALUE        0x00000003

/* MII Management Command Register */
#define MIIMCOM_READ_CYCLE    0x00000001
#define MIIMCOM_SCAN_CYCLE    0x00000002

/* MII Management Address Register */
#define MIIMADD_PHY_ADDR_SHIFT    8

/* MII Management Indicator Register */
#define MIIMIND_BUSY        0x00000001
#define MIIMIND_NOTVALID    0x00000004

void tsec_local_mdio_write(struct tsec_mii_mng *phyregs, s32 port_addr,
                           s32 dev_addr, s32 reg, s32 value);
s32 tsec_local_mdio_read(struct tsec_mii_mng *phyregs, s32 port_addr,
                         s32 dev_addr, s32 regnum);
s32 tsec_phy_read(s32 addr, s32 dev_addr, s32 regnum);
s32 tsec_phy_write(s32 addr, s32 dev_addr, s32 regnum,
                   u16 value);


struct fsl_pq_mdio_info
{
    struct tsec_mii_mng *regs;
    char *name;
};

static inline s32 phy_read(s32 addr, s32 devad, s32 regnum)
{

    return tsec_phy_read( addr, devad, regnum);
}

static inline s32 phy_write(s32 addr, s32 devad, s32 regnum,
                            u16 val)
{

    return tsec_phy_write(addr, devad, regnum, val);
}
#define TSEC_SIZE           0x01000
#define TSEC_MDIO_OFFSET    0x01000

#define CONFIG_SYS_MDIO_BASE_ADDR (MDIO_BASE_ADDR + 0x520)

#define DEFAULT_MII_NAME "FSL_MDIO"

#define STD_TSEC_INFO(num) \
{            \
    .regs = (tsec_t *)(TSEC_BASE_ADDR + ((num - 1) * TSEC_SIZE)), \
    .miiregs_sgmii = (struct tsec_mii_mng *)(CONFIG_SYS_MDIO_BASE_ADDR \
                     + (num - 1) * TSEC_MDIO_OFFSET), \
    .devname = CONFIG_TSEC##num##_NAME, \
    .phyaddr = TSEC##num##_PHY_ADDR, \
    .flags = TSEC##num##_FLAGS, \
    .mii_devname = DEFAULT_MII_NAME \
}

#define SET_STD_TSEC_INFO(x, num) \
{            \
    x.regs = (tsec_t *)(TSEC_BASE_ADDR + ((num - 1) * TSEC_SIZE)); \
    x.miiregs_sgmii = (struct tsec_mii_mng *)(CONFIG_SYS_MDIO_BASE_ADDR \
                      + (num - 1) * TSEC_MDIO_OFFSET); \
    x.devname = CONFIG_TSEC##num##_NAME; \
    x.phyaddr = TSEC##num##_PHY_ADDR; \
    x.flags = TSEC##num##_FLAGS;\
    x.mii_devname = DEFAULT_MII_NAME;\
}

#define MAC_ADDR_LEN 6

/* #define TSEC_TIMEOUT    1000000 */
#define TSEC_TIMEOUT 1000
#define TOUT_LOOP    1000000

/* TBI register addresses */
#define TBI_CR              0x00
#define TBI_SR              0x01
#define TBI_ANA             0x04
#define TBI_ANLPBPA         0x05
#define TBI_ANEX            0x06
#define TBI_TBICON          0x11

/* TBI MDIO register bit fields*/
#define TBICON_CLK_SELECT    0x0020
#define TBIANA_ASYMMETRIC_PAUSE 0x0100
#define TBIANA_SYMMETRIC_PAUSE  0x0080
#define TBIANA_HALF_DUPLEX    0x0040
#define TBIANA_FULL_DUPLEX    0x0020
#define TBICR_PHY_RESET        0x8000
#define TBICR_ANEG_ENABLE    0x1000
#define TBICR_RESTART_ANEG    0x0200
#define TBICR_FULL_DUPLEX    0x0100
#define TBICR_SPEED1_SET    0x0040


/* MAC register bits */
#define MACCFG1_SOFT_RESET     0x80000000
#define MACCFG1_RESET_RX_MC    0x00080000
#define MACCFG1_RESET_TX_MC    0x00040000
#define MACCFG1_RESET_RX_FUN   0x00020000
#define MACCFG1_RESET_TX_FUN   0x00010000
#define MACCFG1_LOOPBACK       0x00000100
#define MACCFG1_RX_FLOW        0x00000020
#define MACCFG1_TX_FLOW        0x00000010
#define MACCFG1_SYNCD_RX_EN    0x00000008
#define MACCFG1_RX_EN          0x00000004
#define MACCFG1_SYNCD_TX_EN    0x00000002
#define MACCFG1_TX_EN          0x00000001

#define MACCFG2_INIT_SETTINGS    0x00007205
#define MACCFG2_FULL_DUPLEX    0x00000001
#define MACCFG2_IF        0x00000300
#define MACCFG2_GMII        0x00000200
#define MACCFG2_MII        0x00000100

#define ECNTRL_INIT_SETTINGS    0x00001000
#define ECNTRL_TBI_MODE        0x00000020
#define ECNTRL_REDUCED_MODE    0x00000010
#define ECNTRL_R100        0x00000008
#define ECNTRL_REDUCED_MII_MODE    0x00000004
#define ECNTRL_SGMII_MODE    0x00000002

#ifndef CONFIG_SYS_TBIPA_VALUE
#define CONFIG_SYS_TBIPA_VALUE    0x1f
#endif

#define MRBLR_INIT_SETTINGS    1536

#define MINFLR_INIT_SETTINGS    0x00000040

#define DMACTRL_INIT_SETTINGS    0x000000c3
#define DMACTRL_GRS        0x00000010
#define DMACTRL_GTS        0x00000008

#define TSTAT_CLEAR_THALT    0x80000000
#define RSTAT_CLEAR_RHALT    0x00800000


#define IEVENT_INIT_CLEAR    0xffffffff
#define IEVENT_BABR        0x80000000
#define IEVENT_RXC        0x40000000
#define IEVENT_BSY        0x20000000
#define IEVENT_EBERR        0x10000000
#define IEVENT_MSRO        0x04000000
#define IEVENT_GTSC        0x02000000
#define IEVENT_BABT        0x01000000
#define IEVENT_TXC        0x00800000
#define IEVENT_TXE        0x00400000
#define IEVENT_TXB        0x00200000
#define IEVENT_TXF        0x00100000
#define IEVENT_IE        0x00080000
#define IEVENT_LC        0x00040000
#define IEVENT_CRL        0x00020000
#define IEVENT_XFUN        0x00010000
#define IEVENT_RXB0        0x00008000
#define IEVENT_GRSC        0x00000100
#define IEVENT_RXF0        0x00000080

#define IMASK_INIT_CLEAR    0x00000000
#define IMASK_TXEEN        0x00400000
#define IMASK_TXBEN        0x00200000
#define IMASK_TXFEN        0x00100000
#define IMASK_RXFEN0        0x00000080


/* Default Attribute fields */
#define ATTR_INIT_SETTINGS     0x000000c0
#define ATTRELI_INIT_SETTINGS  0x00000000


/* TxBD status field bits */
#define TXBD_READY        0x8000
#define TXBD_PADCRC        0x4000
#define TXBD_WRAP        0x2000
#define TXBD_INTERRUPT        0x1000
#define TXBD_LAST        0x0800
#define TXBD_CRC        0x0400
#define TXBD_DEF        0x0200
#define TXBD_HUGEFRAME        0x0080
#define TXBD_LATECOLLISION    0x0080
#define TXBD_RETRYLIMIT        0x0040
#define    TXBD_RETRYCOUNTMASK    0x003c
#define TXBD_UNDERRUN        0x0002
#define TXBD_STATS        0x03ff

/* RxBD status field bits */
#define RXBD_EMPTY        0x8000
#define RXBD_RO1        0x4000
#define RXBD_WRAP        0x2000
#define RXBD_INTERRUPT        0x1000
#define RXBD_LAST        0x0800
#define RXBD_FIRST        0x0400
#define RXBD_MISS        0x0100
#define RXBD_BROADCAST        0x0080
#define RXBD_MULTICAST        0x0040
#define RXBD_LARGE        0x0020
#define RXBD_NONOCTET        0x0010
#define RXBD_SHORT        0x0008
#define RXBD_CRCERR        0x0004
#define RXBD_OVERRUN        0x0002
#define RXBD_TRUNCATED        0x0001
#define RXBD_STATS        0x003f

typedef struct txbd8
{
    u16         status;         /* Status Fields */
    u16         length;         /* Buffer length */
    u32         bufPtr;         /* Buffer Pointer */
} txbd8_t;

typedef struct rxbd8
{
    u16         status;         /* Status Fields */
    u16         length;         /* Buffer Length */
    u32         bufPtr;         /* Buffer Pointer */
} rxbd8_t;

typedef struct rmon_mib
{
    /* Transmit and Receive Counters */
    u32    tr64;        /* Transmit and Receive 64-byte Frame Counter */
    u32    tr127;        /* Transmit and Receive 65-127 byte Frame Counter */
    u32    tr255;        /* Transmit and Receive 128-255 byte Frame Counter */
    u32    tr511;        /* Transmit and Receive 256-511 byte Frame Counter */
    u32    tr1k;        /* Transmit and Receive 512-1023 byte Frame Counter */
    u32    trmax;        /* Transmit and Receive 1024-1518 byte Frame Counter */
    u32    trmgv;        /* Transmit and Receive 1519-1522 byte Good VLAN Frame */
    /* Receive Counters */
    u32    rbyt;        /* Receive Byte Counter */
    u32    rpkt;        /* Receive Packet Counter */
    u32    rfcs;        /* Receive FCS Error Counter */
    u32    rmca;        /* Receive Multicast Packet (Counter) */
    u32    rbca;        /* Receive Broadcast Packet */
    u32    rxcf;        /* Receive Control Frame Packet */
    u32    rxpf;        /* Receive Pause Frame Packet */
    u32    rxuo;        /* Receive Unknown OP Code */
    u32    raln;        /* Receive Alignment Error */
    u32    rflr;        /* Receive Frame Length Error */
    u32    rcde;        /* Receive Code Error */
    u32    rcse;        /* Receive Carrier Sense Error */
    u32    rund;        /* Receive Undersize Packet */
    u32    rovr;        /* Receive Oversize Packet */
    u32    rfrg;        /* Receive Fragments */
    u32    rjbr;        /* Receive Jabber */
    u32    rdrp;        /* Receive Drop */
    /* Transmit Counters */
    u32    tbyt;        /* Transmit Byte Counter */
    u32    tpkt;        /* Transmit Packet */
    u32    tmca;        /* Transmit Multicast Packet */
    u32    tbca;        /* Transmit Broadcast Packet */
    u32    txpf;        /* Transmit Pause Control Frame */
    u32    tdfr;        /* Transmit Deferral Packet */
    u32    tedf;        /* Transmit Excessive Deferral Packet */
    u32    tscl;        /* Transmit Single Collision Packet */
    /* (0x2_n700) */
    u32    tmcl;        /* Transmit Multiple Collision Packet */
    u32    tlcl;        /* Transmit Late Collision Packet */
    u32    txcl;        /* Transmit Excessive Collision Packet */
    u32    tncl;        /* Transmit Total Collision */

    u32    res2;

    u32    tdrp;        /* Transmit Drop Frame */
    u32    tjbr;        /* Transmit Jabber Frame */
    u32    tfcs;        /* Transmit FCS Error */
    u32    txcf;        /* Transmit Control Frame */
    u32    tovr;        /* Transmit Oversize Frame */
    u32    tund;        /* Transmit Undersize Frame */
    u32    tfrg;        /* Transmit Fragments Frame */
    /* General Registers */
    u32    car1;        /* Carry Register One */
    u32    car2;        /* Carry Register Two */
    u32    cam1;        /* Carry Register One Mask */
    u32    cam2;        /* Carry Register Two Mask */
} rmon_mib_t;

typedef struct tsec_hash_regs
{
    u32    iaddr0;        /* Individual Address Register 0 */
    u32    iaddr1;        /* Individual Address Register 1 */
    u32    iaddr2;        /* Individual Address Register 2 */
    u32    iaddr3;        /* Individual Address Register 3 */
    u32    iaddr4;        /* Individual Address Register 4 */
    u32    iaddr5;        /* Individual Address Register 5 */
    u32    iaddr6;        /* Individual Address Register 6 */
    u32    iaddr7;        /* Individual Address Register 7 */
    u32    res1[24];
    u32    gaddr0;        /* Group Address Register 0 */
    u32    gaddr1;        /* Group Address Register 1 */
    u32    gaddr2;        /* Group Address Register 2 */
    u32    gaddr3;        /* Group Address Register 3 */
    u32    gaddr4;        /* Group Address Register 4 */
    u32    gaddr5;        /* Group Address Register 5 */
    u32    gaddr6;        /* Group Address Register 6 */
    u32    gaddr7;        /* Group Address Register 7 */
    u32    res2[24];
} tsec_hash_t;

typedef struct tsec
{
    /* General Control and Status Registers (0x2_n000) */
    u32    res000[4];

    u32    ievent;        /* Interrupt Event */
    u32    imask;        /* Interrupt Mask */
    u32    edis;        /* Error Disabled */
    u32    res01c;
    u32    ecntrl;        /* Ethernet Control */
    u32    minflr;        /* Minimum Frame Length */
    u32    ptv;        /* Pause Time Value */
    u32    dmactrl;    /* DMA Control */
    u32    tbipa;        /* TBI PHY Address */

    u32    res034[3];
    u32    res040[48];

    /* Transmit Control and Status Registers (0x2_n100) */
    u32    tctrl;        /* Transmit Control */
    u32    tstat;        /* Transmit Status */
    u32    res108;
    u32    tbdlen;        /* Tx BD Data Length */
    u32    res110[5];
    u32    ctbptr;        /* Current TxBD Pointer */
    u32    res128[23];
    u32    tbptr;        /* TxBD Pointer */
    u32    res188[30];
    /* (0x2_n200) */
    u32    res200;
    u32    tbase;        /* TxBD Base Address */
    u32    res208[42];
    u32    ostbd;        /* Out of Sequence TxBD */
    u32    ostbdp;        /* Out of Sequence Tx Data Buffer Pointer */
    u32    res2b8[18];

    /* Receive Control and Status Registers (0x2_n300) */
    u32    rctrl;        /* Receive Control */
    u32    rstat;        /* Receive Status */
    u32    res308;
    u32    rbdlen;        /* RxBD Data Length */
    u32    res310[4];
    u32    res320;
    u32    crbptr;    /* Current Receive Buffer Pointer */
    u32    res328[6];
    u32    mrblr;    /* Maximum Receive Buffer Length */
    u32    res344[16];
    u32    rbptr;    /* RxBD Pointer */
    u32    res388[30];
    /* (0x2_n400) */
    u32    res400;
    u32    rbase;    /* RxBD Base Address */
    u32    res408[62];

    /* MAC Registers (0x2_n500) */
    u32    maccfg1;    /* MAC Configuration #1 */
    u32    maccfg2;    /* MAC Configuration #2 */
    u32    ipgifg;        /* Inter Packet Gap/Inter Frame Gap */
    u32    hafdup;        /* Half-duplex */
    u32    maxfrm;        /* Maximum Frame */
    u32    res514;
    u32    res518;

    u32    res51c;

    u32    resmdio[6];

    u32    res538;

    u32    ifstat;        /* Interface Status */
    u32    macstnaddr1;    /* Station Address, part 1 */
    u32    macstnaddr2;    /* Station Address, part 2 */
    u32    res548[46];

    /* (0x2_n600) */
    u32    res600[32];

    /* RMON MIB Registers (0x2_n680-0x2_n73c) */
    rmon_mib_t    rmon;
    u32    res740[48];

    /* Hash Function Registers (0x2_n800) */
    tsec_hash_t    hash;

    u32    res900[128];

    /* Pattern Registers (0x2_nb00) */
    u32    resb00[62];
    u32    attr;       /* Default Attribute Register */
    u32    attreli;       /* Default Attribute Extract Length and Index */

    /* TSEC Future Expansion Space (0x2_nc00-0x2_nffc) */
    u32    resc00[256];
} tsec_t;

#define TSEC_GIGABIT (1 << 0)

/* These flags currently only have meaning if we're using the eTSEC */
#define TSEC_REDUCED    (1 << 1)    /* MAC-PHY interface uses RGMII */
#define TSEC_SGMII    (1 << 2)    /* MAC-PHY interface uses SGMII */

struct tsec_private
{
    tsec_t *regs;
    struct tsec_mii_mng *phyregs_sgmii;
    phy_interface_t interface;
    u32 phyaddr;
    s32 speed;
    s32 duplex;
    s32 link;
    char name[16];
    char mii_devname[16];
    u32  flags;
};



//提供给phy驱动调用的API
s32 tsec_phy_read(s32 addr, s32 dev_addr, s32 regnum);
s32 tsec_phy_write(s32 addr, s32 dev_addr, s32 regnum,u16 value);

//提供应用层调用的API
bool_t module_tsec_init(s32 num,u8 last_mac);
u32 Net_SendPacket(unsigned char bychip,unsigned char* packet, u16 length);
u32 Net_RecvPacket(unsigned char  bychip,unsigned char* packet) ;

#endif /* __TSEC_H__ */

