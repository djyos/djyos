//----------------------------------------------------
//Copyright (C), 2013-2020,  luoshitian.
//版权所有 (C), 2013-2020,   罗侍田.
//所属模块: serdes驱动模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: serdes串口驱动文件
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-11-14
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

#ifndef __FSL_SERDES_H
#define __FSL_SERDES_H

#include <stdint.h>
enum srds_prtcl {
    NONE = 0,
    PCIE1,
    PCIE2,
    PCIE3,
    PCIE4,
    SATA1,
    SATA2,
    SRIO1,
    SRIO2,
    SGMII_FM1_DTSEC1,
    SGMII_FM1_DTSEC2,
    SGMII_FM1_DTSEC3,
    SGMII_FM1_DTSEC4,
    SGMII_FM1_DTSEC5,
    SGMII_FM1_DTSEC6,
    SGMII_FM1_DTSEC9,
    SGMII_FM1_DTSEC10,
    SGMII_FM2_DTSEC1,
    SGMII_FM2_DTSEC2,
    SGMII_FM2_DTSEC3,
    SGMII_FM2_DTSEC4,
    SGMII_FM2_DTSEC5,
    SGMII_FM2_DTSEC6,
    SGMII_FM2_DTSEC9,
    SGMII_FM2_DTSEC10,
    SGMII_TSEC1,
    SGMII_TSEC2,
    SGMII_TSEC3,
    SGMII_TSEC4,
    XAUI_FM1,
    XAUI_FM2,
    AURORA,
    CPRI1,
    CPRI2,
    CPRI3,
    CPRI4,
    CPRI5,
    CPRI6,
    CPRI7,
    CPRI8,
    XAUI_FM1_MAC9,
    XAUI_FM1_MAC10,
    XAUI_FM2_MAC9,
    XAUI_FM2_MAC10,
    HIGIG_FM1_MAC9,
    HIGIG_FM1_MAC10,
    HIGIG_FM2_MAC9,
    HIGIG_FM2_MAC10,
    QSGMII_FM1_A,        /* A indicates MACs 1-4 */
    QSGMII_FM1_B,        /* B indicates MACs 5,6,9,10 */
    QSGMII_FM2_A,
    QSGMII_FM2_B,
    XFI_FM1_MAC9,
    XFI_FM1_MAC10,
    XFI_FM2_MAC9,
    XFI_FM2_MAC10,
    INTERLAKEN,
    SGMII_SW1_DTSEC1,    /* SW indicates on L2 switch */
    SGMII_SW1_DTSEC2,
    SGMII_SW1_DTSEC3,
    SGMII_SW1_DTSEC4,
    SGMII_SW1_DTSEC5,
    SGMII_SW1_DTSEC6,
    QSGMII_SW1_A,        /* SW indicates on L2 swtich */
    QSGMII_SW1_B,
};

enum srds {
    FSL_SRDS_1  = 0,
    FSL_SRDS_2  = 1,
    FSL_SRDS_3  = 2,
    FSL_SRDS_4  = 3,
};
#define CONFIG_SYS_HAS_SERDES        /* common SERDES init code */
#define CONFIG_SYS_MPC85xx_GUTS_OFFSET        0xE0000
#define CONFIG_SYS_MPC85xx_SERDES2_OFFSET    0xE3100
#define CONFIG_SYS_MPC85xx_SERDES1_OFFSET    0xE3000

#define CONFIG_SYS_MPC85xx_GUTS_ADDR \
    (CN_CCSR_NEWADDR_V + CONFIG_SYS_MPC85xx_GUTS_OFFSET)

#define CONFIG_SYS_MPC85xx_SERDES1_ADDR \
    (CN_CCSR_NEWADDR_V + CONFIG_SYS_MPC85xx_SERDES1_OFFSET)
#define CONFIG_SYS_MPC85xx_SERDES2_ADDR \
    (CN_CCSR_NEWADDR_V + CONFIG_SYS_MPC85xx_SERDES2_OFFSET)
typedef struct ccsr_gur {
    u32    porpllsr;    /* POR PLL ratio status */

#define MPC85xx_PORPLLSR_DDR_RATIO    0x00003e00

#define MPC85xx_PORPLLSR_DDR_RATIO_SHIFT    9

#define MPC85xx_PORPLLSR_QE_RATIO    0x3e000000
#define MPC85xx_PORPLLSR_QE_RATIO_SHIFT        25
#define MPC85xx_PORPLLSR_PLAT_RATIO    0x0000003e
#define MPC85xx_PORPLLSR_PLAT_RATIO_SHIFT    1
    u32    porbmsr;    /* POR boot mode status */
#define MPC85xx_PORBMSR_HA        0x00070000
#define MPC85xx_PORBMSR_HA_SHIFT    16
    u32    porimpscr;    /* POR I/O impedance status & control */
    u32    pordevsr;    /* POR I/O device status regsiter */

#define MPC85xx_PORDEVSR_SGMII1_DIS    0x20000000
#define MPC85xx_PORDEVSR_SGMII2_DIS    0x10000000

#define MPC85xx_PORDEVSR_SGMII3_DIS    0x08000000
#define MPC85xx_PORDEVSR_SGMII4_DIS    0x04000000
#define MPC85xx_PORDEVSR_SRDS2_IO_SEL    0x38000000
#define MPC85xx_PORDEVSR_PCI1        0x00800000


#define MPC85xx_PORDEVSR_IO_SEL        0x00780000
#define MPC85xx_PORDEVSR_IO_SEL_SHIFT    19

#define MPC85xx_PORDEVSR_PCI2_ARB    0x00040000
#define MPC85xx_PORDEVSR_PCI1_ARB    0x00020000
#define MPC85xx_PORDEVSR_PCI1_PCI32    0x00010000
#define MPC85xx_PORDEVSR_PCI1_SPD    0x00008000
#define MPC85xx_PORDEVSR_PCI2_SPD    0x00004000
#define MPC85xx_PORDEVSR_DRAM_RTYPE    0x00000060
#define MPC85xx_PORDEVSR_RIO_CTLS    0x00000008
#define MPC85xx_PORDEVSR_RIO_DEV_ID    0x00000007
    u32    pordbgmsr;    /* POR debug mode status */
    u32    pordevsr2;    /* POR I/O device status 2 */
/* The 8544 RM says this is bit 26, but it's really bit 24 */
#define MPC85xx_PORDEVSR2_SEC_CFG    0x00000080
    u8    res1[8];
    u32    gpporcr;    /* General-purpose POR configuration */
    u8    res2[12];

    u32    gpiocr;        /* GPIO control */

    u8    res3[12];

    u32    gpoutdr;    /* General-purpose output data */
    u8    res4[12];

    u32    gpindr;        /* General-purpose input data */
    u8    res5[12];
    u32    pmuxcr;        /* Alt. function signal multiplex control */


#define MPC85xx_PMUXCR_SD_DATA        0x80000000
#define MPC85xx_PMUXCR_SDHC_CD        0x40000000
#define MPC85xx_PMUXCR_SDHC_WP        0x20000000
#define MPC85xx_PMUXCR_ELBC_OFF_USB2_ON    0x01000000
#define MPC85xx_PMUXCR_TDM_ENA        0x00800000
#define MPC85xx_PMUXCR_QE0        0x00008000
#define MPC85xx_PMUXCR_QE1        0x00004000
#define MPC85xx_PMUXCR_QE2        0x00002000
#define MPC85xx_PMUXCR_QE3        0x00001000
#define MPC85xx_PMUXCR_QE4        0x00000800
#define MPC85xx_PMUXCR_QE5        0x00000400
#define MPC85xx_PMUXCR_QE6        0x00000200
#define MPC85xx_PMUXCR_QE7        0x00000100
#define MPC85xx_PMUXCR_QE8        0x00000080
#define MPC85xx_PMUXCR_QE9        0x00000040
#define MPC85xx_PMUXCR_QE10        0x00000020
#define MPC85xx_PMUXCR_QE11        0x00000010
#define MPC85xx_PMUXCR_QE12        0x00000008

    u32    pmuxcr2;    /* Alt. function signal multiplex control 2 */

    u8    res6[8];

    u32    devdisr;    /* Device disable control */
#define MPC85xx_DEVDISR_PCI1        0x80000000
#define MPC85xx_DEVDISR_PCI2        0x40000000
#define MPC85xx_DEVDISR_PCIE        0x20000000
#define MPC85xx_DEVDISR_LBC        0x08000000
#define MPC85xx_DEVDISR_PCIE2        0x04000000
#define MPC85xx_DEVDISR_PCIE3        0x02000000
#define MPC85xx_DEVDISR_SEC        0x01000000
#define MPC85xx_DEVDISR_SRIO        0x00080000
#define MPC85xx_DEVDISR_RMSG        0x00040000
#define MPC85xx_DEVDISR_DDR        0x00010000
#define MPC85xx_DEVDISR_CPU        0x00008000
#define MPC85xx_DEVDISR_CPU0        MPC85xx_DEVDISR_CPU
#define MPC85xx_DEVDISR_TB        0x00004000
#define MPC85xx_DEVDISR_TB0        MPC85xx_DEVDISR_TB
#define MPC85xx_DEVDISR_CPU1        0x00002000
#define MPC85xx_DEVDISR_TB1        0x00001000
#define MPC85xx_DEVDISR_DMA        0x00000400
#define MPC85xx_DEVDISR_TSEC1        0x00000080
#define MPC85xx_DEVDISR_TSEC2        0x00000040
#define MPC85xx_DEVDISR_TSEC3        0x00000020
#define MPC85xx_DEVDISR_TSEC4        0x00000010
#define MPC85xx_DEVDISR_I2C        0x00000004
#define MPC85xx_DEVDISR_DUART        0x00000002
    u8    res7[12];
    u32    powmgtcsr;    /* Power management status & control */
    u8    res8[12];
    u32    mcpsumr;    /* Machine check summary */
    u8    res9[12];
    u32    pvr;        /* Processor version */
    u32    svr;        /* System version */
    u8    res10[8];
    u32    rstcr;        /* Reset control */

    u8    res11a[1868];

    u32    clkdvdr;    /* Clock Divide register */
    u8    res12[1532];
    u32    clkocr;        /* Clock out select */
    u8    res13[12];
    u32    ddrdllcr;    /* DDR DLL control */
    u8    res14[12];
    u32    lbcdllcr;    /* LBC DLL control */

    u8    res15[248];

    u32    lbiuiplldcr0;    /* LBIU PLL Debug Reg 0 */
    u32    lbiuiplldcr1;    /* LBIU PLL Debug Reg 1 */
    u32    ddrioovcr;    /* DDR IO Override Control */
    u32    tsec12ioovcr;    /* eTSEC 1/2 IO override control */
    u32    tsec34ioovcr;    /* eTSEC 3/4 IO override control */
    u8      res16[52];
    u32    sdhcdcr;    /* SDHC debug control register */
    u8      res17[61592];
} ccsr_gur_t;
s32 is_serdes_configured(enum srds_prtcl device);
void fsl_serdes_init(void);



#endif /* __FSL_SERDES_H */
