/*================================================================================
 * 文件名称：Ax88796.h
 * 文件描述：本文件主要用于光通讯模块。
 * 文件版本: v1.00
 * 开发人员: 刘宏君
 * 创建时间: 2006-01
 * Copyright(c) 2004-2008  Shenzhen NARI Limited Co.
 *================================================================================
 * 程序修改记录(最新的放在最前面):
 *  <修改日期>, <修改人员>: <修改功能概述>
 *
 */
#ifndef _Ax88796B_h_
#define _Ax88796B_h_

#include "stdint.h"
#include "stddef.h"

typedef unsigned char    INT8U;
typedef unsigned char    BYTE;
typedef unsigned short   INT16U;
typedef unsigned int     INT32U;
typedef int              DWORD;
typedef short int        WORD;

#ifndef TRUE
#define TRUE   true
#endif
#ifndef TRUE
#define FALSE  false
#endif

/*================================================================================*/
#define CHIP_CONFIG_PAGE      (0x00)    /* where the Ethernet address is */
#define CHIP_TSTART           (0x40)
#define CHIP_PSTART           (0x46)    // page start value
#define CHIP_PSTOP            (0x80)    //0x80 for 16 mode,60 for 8 mode   page stop value

/* page-0, read */

#define PORT_CMD    (0x00)      //command register
#define PORT_TRINCRL    (0x01)  //page start register
#define PORT_TRINCRH    (0x02)  //page stop register
#define PORT_BOUND  (0x03)      //Boundary Pointer
#define PORT_TSTAT  (0x04)      //Transmit Status Register
#define PORT_COLCNT (0x05)      //Number of Collisions Register
#define PORT_INTSTAT    (0x07)  //Interrupt Status Register
#define PORT_RSTAT  (0x0c)      //Receive Status Register
#define PORT_ALICNT (0x0d)      //
#define PORT_CRCCNT (0x0e)      //
#define PORT_MPCNT  (0x0f)      //
#define MII_REG         (0x14)  //MII/EEPROM Access

/* page-0, write */

#define PORT_RSTART (0x01)          //page start register
#define PORT_RSTOP  (0x02)          //page stop register
#define PORT_TSTART (0x04)          //Transmit Page Start Address
#define PORT_TCNTL  (0x05)          //Transmit Byte Count Register 0
#define PORT_TCNTH  (0x06)          //Transmit Byte Count Register 1
#define PORT_RSAR0  (0x08)          //Remote Start Address Register 0
#define PORT_RSAR1  (0x09)          //Remote Start Address Register 1
#define PORT_RBCR0  (0x0a)          //Remote Byte Count 0
#define PORT_RBCR1  (0x0b)          //Remote Byte Count 1
#define PORT_RCON   (0x0c)          //Receive Configuration Register
#define PORT_TCON   (0x0d)          //Transmit Configuration Register
#define PORT_DCON   (0x0e)          //Data Configuration Register
#define PORT_INTMASK    (0x0f)      // Interrupt Mask Register

/* page-1, read and write */

#define PORT_STA0   (0x01)  //Physical Address Register 0
#define PORT_STA1   (0x02)  //Physical Address Register 1
#define PORT_STA2   (0x03)  //Physical Address Register 2
#define PORT_STA3   (0x04)  //Physical Address Register 3
#define PORT_STA4   (0x05)  //Physical Address Register 4
#define PORT_STA5   (0x06)  //Physical Address Register 5
#define PORT_CURR   (0x07)  //Current Page Register
#define PORT_MAR0   (0x08)  //Multicast Address Register 0
#define PORT_MAR1   (0x09)  //Multicast Address Register 1
#define PORT_MAR2   (0x0a)  //Multicast Address Register 2
#define PORT_MAR3   (0x0b)  //Multicast Address Register 3
#define PORT_MAR4   (0x0c)  //Multicast Address Register 4
#define PORT_MAR5   (0x0d)  //Multicast Address Register 5
#define PORT_MAR6   (0x0e)  //Multicast Address Register 6
#define PORT_MAR7   (0x0f)  //Multicast Address Register 7

/* CHIP board registers */

#define PORT_DATA   (0x10)  /* data window */ //Data Port
#define PORT_RESET  (0x1f)  /* read here to reset */// Reset

/* command register  */
#define CMD_PAGE3   (0xC0)      //page 3       page select
#define CMD_PAGE2   (0x80)      //page 2       page select
#define CMD_PAGE1   (0x40)      //page 1       page select
#define CMD_PAGE0   (0x00)      //page 0       page select
#define CMD_NODMA   (0x20)      //Remote DMA Command: Abort / Complete Remote DMA
#define CMD_RWRITE  (0x10)      //Remote DMA Command: Remote Write
#define CMD_RREAD   (0x08)      //Remote DMA Command: Remote Read
#define CMD_TXP     (0x04)      //This bit could be set to initiate transmission of a packet
#define CMD_START   (0x02)      //This bit is used to active AX88796 operation.
#define CMD_STOP    (0x01)      //This bit is used to stop the AX88796 operation.

/* RCON: Receive Configuration Register */
#define RCON_MON    (0x20)      //Monitor Mode
#define RCON_PROM   (0x10)      //Enable the receiver to accept all packets with a physical address.
#define RCON_GROUP  (0x08)      //Enable the receiver to accept packets with a multicast address.
#define RCON_BROAD  (0x04)      //Enable the receiver to accept broadcast packet.
#define RCON_RUNTS  (0x02)      //Enable the receiver to accept runt packet.
#define RCON_SEP    (0x01)      //Enable the receiver to accept and save packets with error.

/* TCON: Transmit Configuration Register */
#define TCON_LB1    (0x04)
#define TCON_LB0    (0x02)
#define TCON_CRCN   (0x01)
#define TCON_NORMAL (0x00)           /* normal operation mode */

/* DCON: Data Configuration Register */
#define DCON_BSIZE1     (0x40)
#define DCON_BSIZE0     (0x20)
#define DCON_AUTO_INIT      (0x10)
#define DCON_LOOPBK_OFF     (0x08)
#define DCON_LOOPBK_ON      (0x00)
#define DCON_DMA_32     (0x04)
#define DCON_DMA_16     (0x00)
#define DCON_BIG_ENDIAN     (0x02)
#define DCON_LITTLE_ENDIAN  (0x00)
#define DCON_BUS16      (0x01)
#define DCON_BUS_8      (0x00)

/* INTMASK: Interrupt Mask Register */
#define IM_XDCE         (0x40)
#define IM_CNTE         (0x20)
#define IM_OVWE         (0x10)
#define IM_TXEE         (0x08)
#define IM_RXEE         (0x04)
#define IM_PTXE         (0x02)
#define IM_PRXE         (0x01)

/* INTSTAT: Interrupt Status Register */
#define ISTAT_RST       (0x80)
#define ISTAT_RDC       (0x40)      /* remote DMA complete */
#define ISTAT_CNT       (0x20)
#define ISTAT_OVW       (0x10)
#define ISTAT_TXE       (0x08)
#define ISTAT_RXE       (0x04)
#define ISTAT_PTX       (0x02)
#define ISTAT_PRX       (0x01)

/* TSTAT: Transmit Status Register */
#define TSTAT_CDH       (0x40)
#define TSTAT_UNDER     (0x20)
#define TSTAT_CRL       (0x10)
#define TSTAT_ABORT     (0x08)

#define TSTAT_OWC       (1<<7)
#define TSTAT_ABT       (1<<3)
#define TSTAT_COL       (1<<2)
#define TSTAT_PTX       (1<<0)

#define TSTAT_TWC       (0x04)
#define TSTAT_NDT       (0x02)
#define TSTAT_PTX       (0x01)

/* RSTAT: Receive Status Register */
#define RSTAT_DFR       (0x80)
#define RSTAT_DIS       (0x40)
#define RSTAT_GROUP     (0x20)
#define RSTAT_MPA       (0x10)
#define RSTAT_OVER      (0x08)
#define RSTAT_FAE       (0x04)
#define RSTAT_CRC       (0x02)
#define RSTAT_PRX       (0x01)

/* ENH: Enable Features */
#define ENH_WAIT1       (0x80)
#define ENH_WAIT0       (0x40)
#define ENH_SLOT1       (0x10)
#define ENH_SLOT0       (0x08)

/* flags - software synchronize bit definitions */
#define TXING           (0x01)
#define RXING           (0x02)
#define TXREQ           (0x04)


//added by wang hua ,this is for MII and EEPROM Programing
#define MII_CLK     (0x01)
#define MII_MDIR        (0x02)
#define MII_MDI         (0x04)
#define MII_MDO         (0x08)
#define MII_EECS        (0x10)
#define MII_EEI         (0x20)
#define MII_EEO         (0x40)
#define MII_EECLK       (0x80)

#define MII_STARTDELIM  (0x01)
#define MII_READOP  (0x02)
#define MII_WRITEOP (0x01)
#define MII_TURNAROUND  (0x02)

#define PHY_BMCR    (0x00)
#define PHY_BMSR    (0x01)
#define PHY_VENID   (0x02)
#define PHY_DEVID   (0x03)
#define PHY_ANAR    (0x04)
#define PHY_LPAR    (0x05)
#define PHY_ANEXP   (0x06)

#define PHY_ANAR_NEXTPAGE   (0x8000)
#define PHY_ANAR_RSVD0      (0x4000)
#define PHY_ANAR_TLRFLT     (0x2000)
#define PHY_ANAR_RSVD1      (0x1000)
#define PHY_ANAR_RSVD2      (0x0800)
#define PHY_ANAR_RSVD3      (0x0400)
#define PHY_ANAR_100BT4     (0x0200)
#define PHY_ANAR_100BTXFULL (0x0100)
#define PHY_ANAR_100BTXHALF (0x0080)
#define PHY_ANAR_10BTFULL   (0x0040)
#define PHY_ANAR_10BTHALF   (0x0020)
#define PHY_ANAR_PROTO4     (0x0010)
#define PHY_ANAR_PROTO3     (0x0008)
#define PHY_ANAR_PROTO2     (0x0004)
#define PHY_ANAR_PROTO1     (0x0002)
#define PHY_ANAR_PROTO0     (0x0001)


#define PHY_BMCR_RESET          (0x8000)
#define PHY_BMCR_LOOPBK         (0x4000)
#define PHY_BMCR_SPEEDSEL       0x2000
#define PHY_BMCR_AUTONEGENBL        0x1000
#define PHY_BMCR_RSVD0          0x0800
#define PHY_BMCR_ISOLATE        0x0400
#define PHY_BMCR_AUTONEGRSTR        0x0200
#define PHY_BMCR_DUPLEX         0x0100
#define PHY_BMCR_COLLTEST       0x0080
#define PHY_BMCR_RSVD1          0x0040
#define PHY_BMCR_RSVD2          0x0020
#define PHY_BMCR_RSVD3          0x0010
#define PHY_BMCR_RSVD4          0x0008
#define PHY_BMCR_RSVD5          0x0004
#define PHY_BMCR_RSVD6          0x0002
#define PHY_BMCR_RSVD7          0x0001

#define PHY_BMSR_100BT4         0x8000
#define PHY_BMSR_100BTXFULL     0x4000
#define PHY_BMSR_100BTXHALF     0x2000
#define PHY_BMSR_10BTFULL       0x1000
#define PHY_BMSR_10BTHALF       0x0800
#define PHY_BMSR_RSVD1          0x0400
#define PHY_BMSR_RSVD2          0x0200
#define PHY_BMSR_RSVD3          0x0100
#define PHY_BMSR_RSVD4          0x0080
#define PHY_BMSR_MFPRESUP       0x0040
#define PHY_BMSR_AUTONEGCOMP        0x0020
#define PHY_BMSR_REMFAULT       0x0010
#define PHY_BMSR_CANAUTONEG     0x0008
#define PHY_BMSR_LINKSTAT       0x0004
#define PHY_BMSR_JABBER         0x0002
#define PHY_BMSR_EXTENDED       0x0001
/* ethernet media */

#define IFM_ETHER       0x00000020
#define IFM_10_T        3
#define IFM_10_2        4
#define IFM_10_5        5
#define IFM_100_TX      6
#define IFM_100_FX      7
#define IFM_100_T4      8
#define IFM_100_VG      9
#define IFM_100_T2      10


#define IFM_AUTO        0
#define IFM_MANUAL      1
#define IFM_NONE        2

#define IFM_FDX         0x00100000
#define IFM_HDX         0x00200000
#define IFM_FLAG0       0x01000000
#define IFM_FLAG1       0x02000000
#define IFM_FLAG2       0x04000000
#define IFM_LOOP        0x08000000


#define IFM_NMASK       0x000000e0
#define IFM_TMASK       0x0000000f
#define IFM_IMASK       0xf0000000
#define IFM_ISHIFT      28
#define IFM_OMASK       0x0000ff00
#define IFM_GMASK       0x0ff00000

#define IFM_AVALID      0x00000001
#define IFM_ACTIVE      0x00000002

#define IFM_TYPE(x) ((x) & IFM_NMASK)
#define IFM_SUBTYPE(x)  ((x) & IFM_TMASK)
#define IFM_INST(x) (((x) & IFM_IMASK) >> IFM_ISHIFT)

#define FLAG_FORCEDELAY 1
#define FLAG_SCHEDDELAY 2
#define FLAG_DELAYTIMEO 3

#pragma pack(1)
typedef struct
{
    u8 rstat;
    u8 next;
    u8 lowByteCnt;
    u8 uppByteCnt;
}tagAx88796FH;   //88796的数据头
#pragma pack()


#include <tcpip/tcpip_cdef.h>

#define CN_DEV_TERR                (1<<0)   //发送出错
#define CN_DEV_TOVR                (1<<1)   //发送溢出
#define CN_DEV_RERR                (1<<2)   //接收出错
#define CN_DEV_ROVR                (1<<3)   //接收溢出
#define CN_DEV_RBF                 (1<<4)   //接收大数据包（超过设定值）
#define CN_DEV_OK                  (0)      //设备正常

#define CN_STACK_SIZEDEFAULT       0X1000
#define CN_BUF_LEN                 1600
#define CN_FRAME_MAX               1514
#define CN_FRAME_MIN               54
#define CN_NAME_LEN                32
#define CN_AX88796_MAGIC           0x88796000

//需要用户实现的钩子函数原型                                                 dev为返回的安装网卡
typedef bool_t (*rstDealer)(void *dev);
typedef bool_t (*rcvDealer)(void *dev,u8 *buf, u16 len); //RAW接收
typedef bool_t (*sndDealer)(void *dev,u8 *buf, u16 len); //RAW发送

typedef struct
{
    bool_t                  useirq;     //使用中断，务必要提供中断号
    u32                     loopcycle;  //当不使用中断时,轮询的周期
    u32                     irqno;      //中断号
    u8                     *mac;        //mac地址
    char                   *name;                  //网卡名字
    void                   *devbase;               //控制器的基地址
    u32                     stacksize;             //任务的栈大小
    rstDealer               fnRstDealer;           //网卡的硬件复位函数
    rcvDealer               fnRcvDealer;           //网卡Raw接收数据处理,当为空时交给协议栈处理
}tagAx88796Para;

typedef struct
{
    u32                     magic;                 //网卡标志，避免被修改
    //系统申请量
    void                   *devhandle;             //注册的网卡句柄
    struct MutexLCB        *devsync;               //用于同步网卡
    struct SemaphoreLCB    *rcvsync;               //接收信号
    u16                     taskRcvID;             //接收任务ID
    u16                     taskMonitorID;         //件事网卡任务ID
    //网卡的配置量
    u8                     *devbase;               //网卡控制器地址
    u32                     stacksize;             //接收线程栈大小
    u32                     irqno;                 //中断号
    u32                     loopcycle;             //当不使用中断时,轮询的周期
    u8                      mac[CN_MACADDR_LEN];   //MAC地址
    char                    name[CN_NAME_LEN];     //网卡名字
    rstDealer               fnRstDealer;           //网卡的硬件复位函数
    sndDealer               fnSndDealer;           //网卡RAW发送，提供给用户使用
    rcvDealer               fnRcvDealer;           //网卡Raw接收数据处理,当为空时交给协议栈处理
    //网卡的标记量
    u32                     devflag;               //网卡标记
    u32                     monitorRcv;            //网卡接收量

    u32                     cnt;   //counter over  num
    u32                     ovw;   //the receive ring over num
    u32                     txe;   //the transmit error num
    u32                     rxe;   //the receive error num

    u8                      rcvbuf[CN_BUF_LEN];    //发送缓存
    u8                      sndbuf[CN_BUF_LEN];    //接收缓存
}tagAx88796Pri;  //88796网卡的私有数据，每个网卡有一个

//安装88796网卡的唯一函数
tagAx88796Pri *Ax88796Install(tagAx88796Para *para);


#endif
