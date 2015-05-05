// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：cpu_peri_enet.c
// 模块描述: ENET模块的初始化的操作，包含PHY初始化和MII初始化
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 28/07.2014
// =============================================================================

#ifndef CPU_PERI_ENET_H_
#define CPU_PERI_ENET_H_

/* Ethernet standard lengths in bytes*/
#define ETH_ADDR_LEN    (6)
#define ETH_TYPE_LEN    (2)
#define ETH_CRC_LEN     (4)
#define ETH_MAX_DATA    (1500)
#define ETH_MIN_DATA    (46)
#define ETH_HDR_LEN     (ETH_ADDR_LEN * 2 + ETH_TYPE_LEN)

/* Defined Ethernet Frame Types */
#define ETH_FRM_IP      (0x0800)
#define ETH_FRM_ARP     (0x0806)
#define ETH_FRM_RARP    (0x8035)
#define ETH_FRM_TEST    (0xA5A5)

/* Maximum and Minimum Ethernet Frame Sizes */
#define ETH_MAX_FRM     (ETH_HDR_LEN + ETH_MAX_DATA + ETH_CRC_LEN)
#define ETH_MIN_FRM     (ETH_HDR_LEN + ETH_MIN_DATA + ETH_CRC_LEN)
#define ETH_MTU         (ETH_HDR_LEN + ETH_MAX_DATA)


// =============================================================================
// Buffer sizes in bytes (must be divisible by 16)
#define RX_BUFFER_SIZE                                         256//ETH_MAX_FRM
//#define TX_BUFFER_SIZE                                         1520//ETH_MAX_FRM

// Number of Receive and Transmit Buffers and Buffer Descriptors
#define NUM_RXBDS 20
#define NUM_TXBDS 10


typedef struct
{
	uint16_t status;	/* control and status */
	uint16_t length;	/* transfer length */
	uint8_t  *data;	    /* buffer address */
} NBUF;

//For Freescale ARM Architecture(NBUF_LITTLE_ENDIAN)
// ----------------------------------------------------------------------
// TX Buffer Descriptor Bit Definitions
// ----------------------------------------------------------------------
#define TX_BD_R			0x0080
#define TX_BD_TO1		0x0040
#define TX_BD_W			0x0020
#define TX_BD_TO2		0x0010
#define TX_BD_L			0x0008
#define TX_BD_TC		0x0004
#define TX_BD_ABC		0x0002

// ----------------------------------------------------------------------
// RX Buffer Descriptor Bit Definitions
// ----------------------------------------------------------------------

// Offset 0 flags - status: Big Endian
#define RX_BD_E			0x0080
#define RX_BD_R01		0x0040
#define RX_BD_W			0x0020
#define RX_BD_R02		0x0010
#define RX_BD_L			0x0008
#define RX_BD_M			0x0001
#define RX_BD_BC		0x8000
#define RX_BD_MC		0x4000
#define RX_BD_LG		0x2000
#define RX_BD_NO		0x1000
#define RX_BD_CR		0x0400
#define RX_BD_OV		0x0200
#define RX_BD_TR		0x0100


// =============================================================================
/* MII Register Addresses */
#define PHY_BMCR                    (0x00)
#define PHY_BMSR                    (0x01)
#define PHY_PHYIDR1                 (0x02)
#define PHY_PHYIDR2                 (0x03)
#define PHY_ANAR                    (0x04)
#define PHY_ANLPAR                  (0x05)
#define PHY_ANLPARNP                (0x05)
#define PHY_ANER                    (0x06)
#define PHY_ANNPTR                  (0x07)
#define PHY_PHYSTS                  (0x10)
#define PHY_MICR                    (0x11)
#define PHY_MISR                    (0x12)
#define PHY_PAGESEL                 (0x13)

/*TSI-EVB definition: National Semiconductor*/
#define PHY_PHYCR2                  (0x1C)

/*TWR definition: Micrel*/
#define PHY_PHYCTRL1                (0x1E)
#define PHY_PHYCTRL2                (0x1F)

/* Bit definitions and macros for PHY_BMCR */
#define PHY_BMCR_RESET              (0x8000)
#define PHY_BMCR_LOOP               (0x4000)
#define PHY_BMCR_SPEED              (0x2000)
#define PHY_BMCR_AN_ENABLE          (0x1000)
#define PHY_BMCR_POWERDOWN          (0x0800)
#define PHY_BMCR_ISOLATE            (0x0400)
#define PHY_BMCR_AN_RESTART         (0x0200)
#define PHY_BMCR_FDX                (0x0100)
#define PHY_BMCR_COL_TEST           (0x0080)

/* Bit definitions and macros for PHY_BMSR */
#define PHY_BMSR_100BT4             (0x8000)
#define PHY_BMSR_100BTX_FDX         (0x4000)
#define PHY_BMSR_100BTX             (0x2000)
#define PHY_BMSR_10BT_FDX           (0x1000)
#define PHY_BMSR_10BT               (0x0800)
#define PHY_BMSR_NO_PREAMBLE        (0x0040)
#define PHY_BMSR_AN_COMPLETE        (0x0020)
#define PHY_BMSR_REMOTE_FAULT       (0x0010)
#define PHY_BMSR_AN_ABILITY         (0x0008)
#define PHY_BMSR_LINK               (0x0004)
#define PHY_BMSR_JABBER             (0x0002)
#define PHY_BMSR_EXTENDED           (0x0001)

/* Bit definitions and macros for PHY_ANAR */
#define PHY_ANAR_NEXT_PAGE          (0x8001)
#define PHY_ANAR_REM_FAULT          (0x2001)
#define PHY_ANAR_PAUSE              (0x0401)
#define PHY_ANAR_100BT4             (0x0201)
#define PHY_ANAR_100BTX_FDX         (0x0101)
#define PHY_ANAR_100BTX             (0x0081)
#define PHY_ANAR_10BT_FDX           (0x0041)
#define PHY_ANAR_10BT               (0x0021)
#define PHY_ANAR_802_3              (0x0001)

/* Bit definitions and macros for PHY_ANLPAR */
#define PHY_ANLPAR_NEXT_PAGE        (0x8000)
#define PHY_ANLPAR_ACK              (0x4000)
#define PHY_ANLPAR_REM_FAULT        (0x2000)
#define PHY_ANLPAR_PAUSE            (0x0400)
#define PHY_ANLPAR_100BT4           (0x0200)
#define PHY_ANLPAR_100BTX_FDX       (0x0100)
#define PHY_ANLPAR_100BTX           (0x0080)
#define PHY_ANLPAR_10BTX_FDX        (0x0040)
#define PHY_ANLPAR_10BT             (0x0020)


/* Bit definitions of PHY_PHYSTS: National */
#define PHY_PHYSTS_MDIXMODE         (0x4000)
#define PHY_PHYSTS_RX_ERR_LATCH     (0x2000)
#define PHY_PHYSTS_POL_STATUS       (0x1000)
#define PHY_PHYSTS_FALSECARRSENSLAT (0x0800)
#define PHY_PHYSTS_SIGNALDETECT     (0x0400)
#define PHY_PHYSTS_PAGERECEIVED     (0x0100)
#define PHY_PHYSTS_MIIINTERRUPT     (0x0080)
#define PHY_PHYSTS_REMOTEFAULT      (0x0040)
#define PHY_PHYSTS_JABBERDETECT     (0x0020)
#define PHY_PHYSTS_AUTONEGCOMPLETE  (0x0010)
#define PHY_PHYSTS_LOOPBACKSTATUS   (0x0008)
#define PHY_PHYSTS_DUPLEXSTATUS     (0x0004)
#define PHY_PHYSTS_SPEEDSTATUS      (0x0002)
#define PHY_PHYSTS_LINKSTATUS       (0x0001)


/* Bit definitions of PHY_PHYCR2 */
#define PHY_PHYCR2_SYNC_ENET_EN     (0x2000)
#define PHY_PHYCR2_CLK_OUT_RXCLK    (0x1000)
#define PHY_PHYCR2_BC_WRITE         (0x0800)
#define PHY_PHYCR2_PHYTER_COMP      (0x0400)
#define PHY_PHYCR2_SOFT_RESET       (0x0200)
#define PHY_PHYCR2_CLK_OUT_DIS      (0x0001)

/* Bit definition and macros for PHY_PHYCTRL1 */
#define PHY_PHYCTRL1_LED_MASK       (0xC000)
#define PHY_PHYCTRL1_POLARITY       (0x2000)
#define PHY_PHYCTRL1_MDX_STATE      (0x0800)
#define PHY_PHYCTRL1_REMOTE_LOOP    (0x0080)

/* Bit definition and macros for PHY_PHYCTRL2 */
#define PHY_PHYCTRL2_HP_MDIX        (0x8000)
#define PHY_PHYCTRL2_MDIX_SELECT    (0x4000)
#define PHY_PHYCTRL2_PAIRSWAP_DIS   (0x2000)
#define PHY_PHYCTRL2_ENERGY_DET     (0x1000)
#define PHY_PHYCTRL2_FORCE_LINK     (0x0800)
#define PHY_PHYCTRL2_POWER_SAVING   (0x0400)
#define PHY_PHYCTRL2_INT_LEVEL      (0x0200)
#define PHY_PHYCTRL2_EN_JABBER      (0x0100)
#define PHY_PHYCTRL2_AUTONEG_CMPLT  (0x0080)
#define PHY_PHYCTRL2_ENABLE_PAUSE   (0x0040)
#define PHY_PHYCTRL2_PHY_ISOLATE    (0x0020)
#define PHY_PHYCTRL2_OP_MOD_MASK    (0x001C)
#define PHY_PHYCTRL2_EN_SQE_TEST    (0x0002)
#define PHY_PHYCTRL2_DATA_SCRAM_DIS (0x0001)


/* Bit definitions of PHY_PHYCTRL2_OP_MOD_MASK */
#define PHY_PHYCTRL2_OP_MOD_SHIFT             2
#define PHY_PHYCTRL2_MODE_OP_MOD_STILL_NEG    0
#define PHY_PHYCTRL2_MODE_OP_MOD_10MBPS_HD    1
#define PHY_PHYCTRL2_MODE_OP_MOD_100MBPS_HD   2
#define PHY_PHYCTRL2_MODE_OP_MOD_10MBPS_FD    5
#define PHY_PHYCTRL2_MODE_OP_MOD_100MBPS_FD   6

// =============================================================================
/********INTERFACE**********/
typedef enum
{
	MAC_MII,
	MAC_RMII,
} ENET_INTERFACE;
/********AUTONEG**********/
typedef enum
{
	AUTONEG_ON,
	AUTONEG_OFF
} ENET_AUTONEG;
/********SPEED**********/
typedef enum
{
	MII_10BASET,
	MII_100BASET
} ENET_SPEED;
/********DUPLEX**********/
/* MII Duplex Settings */
typedef enum
{
	MII_HDX,		/*!< half-duplex */
	MII_FDX			/*!< full-duplex */
} ENET_DUPLEX;
/********LOOPBACK**********/
typedef enum
{
	INTERNAL_LOOPBACK,
	EXTERNAL_LOOPBACK,
	NO_LOOPBACK
} ENET_LOOPBACK;
/********EXTERNAL**********/
typedef enum
{
	EXTERNAL_NONE,
	EXTERNAL_YES
} ENET_EXTERNAL_CONN;

/* FEC Configuration Parameters */
typedef struct
{
  ENET_INTERFACE      interface;     /* Transceiver mode  */
  ENET_AUTONEG        neg;           /* FEC autoneg */
  ENET_SPEED          speed;         /* Ethernet Speed           */
  ENET_DUPLEX         duplex;        /* Ethernet Duplex          */
  ENET_LOOPBACK       loopback;      /* Loopback Mode */
  ENET_EXTERNAL_CONN  external;      /* outside test? */
  uint8_t             prom;     /* Promiscuous Mode?        */
  uint8_t             mac[6];   /* Ethernet Address         */
} tagEnetCfg;


#endif /* CPU_PERI_ENET_H_ */
