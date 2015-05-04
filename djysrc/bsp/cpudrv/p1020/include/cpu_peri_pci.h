//----------------------------------------------------
//Copyright (C), 2013-2020,  luoshitian.
//版权所有 (C), 2013-2020,   罗侍田.
//所属模块: pcie控制器驱动模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: freescale p1020中pcie控制器模块驱动文件
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-11-14
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __ppc1020_pci_h__
#define __ppc1020_pci_h__

#include "pci.h"
#include "cpu_peri_serdes.h"
#include "board-config.h"

#define FSLPCIEDEBUG
#ifdef FSLPCIEDEBUG
#define _DEBUG    1
#else
#define _DEBUG    0
#endif
#define debug_cond(cond, fmt, args...)        \
    do {                    \
        if (cond)            \
            printf(fmt, ##args);    \
    } while (0)

#define fslpcie_debug(fmt, args...)        debug_cond(_DEBUG, fmt, ##args)

/*设置寄存器关闭ASPM（激活状态电源管理）功能,1表示关闭*/
#define CONFIG_FSL_PCIE_DISABLE_ASPM      1

#define CONFIG_PCIE1   1        /* PCIE controler 1 (slot 1) */
#define CONFIG_PCIE2   1        /* PCIE controler 2 (slot 2) */
#define CONFIG_PCIE3   0        /* PCIE controler 3 (slot 3) */
#define CONFIG_PCIE4   0        /* PCIE controler 4 (slot 4) */

//#define CONFIG_SYS_PCI_64BIT        /* enable 64-bit PCI resources */

/*
 * PCI Windows
 * Memory space is mapped 1-1, but I/O space must start from 0.
 */
/* controller 1, Slot 1, tgtid 1, Base address 8000 */
#define CONFIG_SYS_PCIE1_NAME        "PCIe SLOT1"
#define CONFIG_SYS_PCIE1_MEM_VIRT    0x80000000
#if CONFIG_PHYS_64BIT == 1
#define CONFIG_SYS_PCIE1_MEM_BUS    0x80000000
#define CONFIG_SYS_PCIE1_MEM_PHYS    0x800000000ull
#else
#define CONFIG_SYS_PCIE1_MEM_BUS    0x80000000
#define CONFIG_SYS_PCIE1_MEM_PHYS    0x80000000
#endif
#define CONFIG_SYS_PCIE1_MEM_SIZE    (0x20000000-0x00010000)    /* 512M-64k*/

#define CONFIG_SYS_PCIE1_IO_VIRT     (0x80000000+CONFIG_SYS_PCIE1_MEM_SIZE)
#define CONFIG_SYS_PCIE1_IO_BUS      0x00000000
#define CONFIG_SYS_PCIE1_IO_PHYS     CONFIG_SYS_PCIE1_IO_VIRT
#define CONFIG_SYS_PCIE1_IO_SIZE     0x00010000    /* 64k */


/* controller 2, direct to uli, tgtid 2, Base address a000 */
#define CONFIG_SYS_PCIE2_NAME        "PCIe SLOT2"
#define CONFIG_SYS_PCIE2_MEM_VIRT    0xa0000000
#if CONFIG_PHYS_64BIT == 1
#define CONFIG_SYS_PCIE2_MEM_BUS    0xa0000000
#define CONFIG_SYS_PCIE2_MEM_PHYS    0xa00000000ull
#else
#define CONFIG_SYS_PCIE2_MEM_BUS    0xa0000000
#define CONFIG_SYS_PCIE2_MEM_PHYS    0xa0000000
#endif
#define CONFIG_SYS_PCIE2_MEM_SIZE    (0x20000000-0x00010000)    /* 512M -64K*/

#define CONFIG_SYS_PCIE2_IO_VIRT    (0xa0000000+CONFIG_SYS_PCIE2_MEM_SIZE)
#define CONFIG_SYS_PCIE2_IO_BUS        0x00000000
#define CONFIG_SYS_PCIE2_IO_PHYS    CONFIG_SYS_PCIE2_IO_VIRT
#define CONFIG_SYS_PCIE2_IO_SIZE    0x00010000    /* 64k */


/* PCI Registers */
typedef struct ccsr_pcix
{
    u32    cfg_addr;    /* PCIX Configuration Addr */
    u32    cfg_data;    /* PCIX Configuration Data */
    u32    int_ack;    /* PCIX IRQ Acknowledge */
    u8    res000c[52];
    u32    liodn_base;    /* PCIX LIODN base register */
    u8    res0044[3004];
    u32    potar0;        /* PCIX Outbound Transaction Addr 0 */
    u32    potear0;    /* PCIX Outbound Translation Extended Addr 0 */
    u32    powbar0;    /* PCIX Outbound Window Base Addr 0 */
    u32    powbear0;    /* PCIX Outbound Window Base Extended Addr 0 */
    u32    powar0;        /* PCIX Outbound Window Attrs 0 */
    u8    res2[12];
    u32    potar1;        /* PCIX Outbound Transaction Addr 1 */
    u32    potear1;    /* PCIX Outbound Translation Extended Addr 1 */
    u32    powbar1;    /* PCIX Outbound Window Base Addr 1 */
    u32    powbear1;    /* PCIX Outbound Window Base Extended Addr 1 */
    u32    powar1;        /* PCIX Outbound Window Attrs 1 */
    u8    res3[12];
    u32    potar2;        /* PCIX Outbound Transaction Addr 2 */
    u32    potear2;    /* PCIX Outbound Translation Extended Addr 2 */
    u32    powbar2;    /* PCIX Outbound Window Base Addr 2 */
    u32    powbear2;    /* PCIX Outbound Window Base Extended Addr 2 */
    u32    powar2;        /* PCIX Outbound Window Attrs 2 */
    u8    res4[12];
    u32    potar3;        /* PCIX Outbound Transaction Addr 3 */
    u32    potear3;    /* PCIX Outbound Translation Extended Addr 3 */
    u32    powbar3;    /* PCIX Outbound Window Base Addr 3 */
    u32    powbear3;    /* PCIX Outbound Window Base Extended Addr 3 */
    u32    powar3;        /* PCIX Outbound Window Attrs 3 */
    u8    res5[12];
    u32    potar4;        /* PCIX Outbound Transaction Addr 4 */
    u32    potear4;    /* PCIX Outbound Translation Extended Addr 4 */
    u32    powbar4;    /* PCIX Outbound Window Base Addr 4 */
    u32    powbear4;    /* PCIX Outbound Window Base Extended Addr 4 */
    u32    powar4;        /* PCIX Outbound Window Attrs 4 */
    u8    res6[268];
    u32    pitar3;        /* PCIX Inbound Translation Addr 3 */
    u32    pitear3;    /* PCIX Inbound Translation Extended Addr 3 */
    u32    piwbar3;    /* PCIX Inbound Window Base Addr 3 */
    u32    piwbear3;    /* PCIX Inbound Window Base Extended Addr 3 */
    u32    piwar3;        /* PCIX Inbound Window Attrs 3 */
    u8    res7[12];
    u32    pitar2;        /* PCIX Inbound Translation Addr 2 */
    u32    pitear2;    /* PCIX Inbound Translation Extended Addr 2 */
    u32    piwbar2;    /* PCIX Inbound Window Base Addr 2 */
    u32    piwbear2;    /* PCIX Inbound Window Base Extended Addr 2 */
    u32    piwar2;        /* PCIX Inbound Window Attrs 2 */
    u8    res8[12];
    u32    pitar1;        /* PCIX Inbound Translation Addr 1 */
    u32    pitear1;    /* PCIX Inbound Translation Extended Addr 1 */
    u32    piwbar1;    /* PCIX Inbound Window Base Addr 1 */
    u8    res9[4];
    u32    piwar1;        /* PCIX Inbound Window Attrs 1 */
    u8    res10[12];
    u32    pedr;        /* PCIX Error Detect */
    u32    pecdr;        /* PCIX Error Capture Disable */
    u32    peer;        /* PCIX Error Enable */
    u32    peattrcr;    /* PCIX Error Attrs Capture */
    u32    peaddrcr;    /* PCIX Error Addr Capture */
    u32    peextaddrcr;    /* PCIX Error Extended Addr Capture */
    u32    pedlcr;        /* PCIX Error Data Low Capture */
    u32    pedhcr;        /* PCIX Error Error Data High Capture */
    u32    gas_timr;    /* PCIX Gasket Timer */
    u8    res11[476];
} ccsr_pcix_t;

#define PCIX_COMMAND    0x62
#define POWAR_EN    0x80000000
#define POWAR_IO_READ    0x00080000
#define POWAR_MEM_READ    0x00040000
#define POWAR_IO_WRITE    0x00008000
#define POWAR_MEM_WRITE    0x00004000
#define POWAR_MEM_512M    0x0000001c
#define POWAR_IO_1M    0x00000013

#define PIWAR_EN    0x80000000
#define PIWAR_PF    0x20000000
#define PIWAR_LOCAL    0x00f00000
#define PIWAR_READ_SNOOP    0x00050000
#define PIWAR_WRITE_SNOOP    0x00005000
#define PIWAR_MEM_2G        0x0000001e


#define CONFIG_SYS_MPC85xx_PCI1_OFFSET        0x8000
#define CONFIG_SYS_MPC85xx_PCIX_OFFSET        0x8000
#define CONFIG_SYS_MPC85xx_PCI2_OFFSET        0x9000
#define CONFIG_SYS_MPC85xx_PCIX2_OFFSET        0x9000
#define CONFIG_SYS_MPC85xx_PCIE1_OFFSET         0xa000
#define CONFIG_SYS_MPC85xx_PCIE2_OFFSET         0x9000

#define CONFIG_SYS_MPC85xx_PCIX_ADDR \
    (CN_CCSR_NEWADDR_V + CONFIG_SYS_MPC85xx_PCIX_OFFSET)
#define CONFIG_SYS_MPC85xx_PCIX2_ADDR \
    (CN_CCSR_NEWADDR_V + CONFIG_SYS_MPC85xx_PCIX2_OFFSET)



#define CONFIG_SYS_PCI1_ADDR \
    (CN_CCSR_NEWADDR_V + CONFIG_SYS_MPC85xx_PCI1_OFFSET)
#define CONFIG_SYS_PCI2_ADDR \
    (CN_CCSR_NEWADDR_V + CONFIG_SYS_MPC85xx_PCI2_OFFSET)
#define CONFIG_SYS_PCIE1_ADDR \
    (CN_CCSR_NEWADDR_V + CONFIG_SYS_MPC85xx_PCIE1_OFFSET)
#define CONFIG_SYS_PCIE2_ADDR \
    (CN_CCSR_NEWADDR_V + CONFIG_SYS_MPC85xx_PCIE2_OFFSET)
#define CONFIG_SYS_PCIE3_ADDR \
    (CN_CCSR_NEWADDR_V + CONFIG_SYS_MPC85xx_PCIE3_OFFSET)
#define CONFIG_SYS_PCIE4_ADDR \
    (CN_CCSR_NEWADDR_V + CONFIG_SYS_MPC85xx_PCIE4_OFFSET)


#ifndef CONFIG_SYS_CCSRBAR_PHYS_HIGH

#define CONFIG_SYS_CCSRBAR_PHYS_HIGH    0

#endif

#define CONFIG_SYS_CCSRBAR_PHYS ((CONFIG_SYS_CCSRBAR_PHYS_HIGH * 1ull) << 32 | \
        CN_CCSR_NEWADDR_V)

#define PEX_IP_BLK_REV_2_2    0x02080202
#define PEX_IP_BLK_REV_2_3    0x02080203
#define PEX_IP_BLK_REV_3_0    0x02080300

/* Freescale-specific PCI config registers */
#define FSL_PCI_PBFR        0x44

#ifdef CONFIG_SYS_FSL_PCI_VER_3_X
/* Currently only the PCIe capability is used, so hardcode the offset.
 * if more capabilities need to be justified, the capability link method
 * should be applied here
 */
#define FSL_PCIE_CAP_ID        0x70
#define PCI_DCR        0x78    /* PCIe Device Control Register */
#define PCI_DSR        0x7a    /* PCIe Device Status Register */
#define PCI_LSR        0x82    /* PCIe Link Status Register */
#define PCI_LCR        0x80    /* PCIe Link Control Register */
#else
#define FSL_PCIE_CAP_ID        0x4c
#define PCI_DCR        0x54    /* PCIe Device Control Register */
#define PCI_DSR        0x56    /* PCIe Device Status Register */
#define PCI_LSR        0x5e    /* PCIe Link Status Register */
#define PCI_LCR        0x5c    /* PCIe Link Control Register */
#endif

#define FSL_PCIE_CFG_RDY    0x4b0
#define FSL_PROG_IF_AGENT    0x1

#define PCI_LTSSM    0x404   /* PCIe Link Training, Status State Machine */
#define PCI_LTSSM_L0    0x16    /* L0 state */

s32 fsl_setup_hose(struct pci_controller *hose, unsigned long addr);
s32 fsl_is_pci_agent(struct pci_controller *hose);
void fsl_pci_config_unlock(struct pci_controller *hose);

/*
 * Common PCI/PCIE Register structure for mpc85xx and mpc86xx
 */

/*
 * PCI Translation Registers
 */
typedef struct pci_outbound_window
{
    u32    potar;        /* 0x00 - Address */
    u32    potear;        /* 0x04 - Address Extended */
    u32    powbar;        /* 0x08 - Window Base Address */
    u32    res1;
    u32    powar;        /* 0x10 - Window Attributes */
#define POWAR_EN    0x80000000
#define POWAR_IO_READ    0x00080000
#define POWAR_MEM_READ    0x00040000
#define POWAR_IO_WRITE    0x00008000
#define POWAR_MEM_WRITE    0x00004000
    u32    res2[3];
} pot_t;

typedef struct pci_inbound_window
{
    u32    pitar;        /* 0x00 - Address */
    u32    res1;
    u32    piwbar;        /* 0x08 - Window Base Address */
    u32    piwbear;    /* 0x0c - Window Base Address Extended */
    u32    piwar;        /* 0x10 - Window Attributes */
#define PIWAR_EN        0x80000000
#define PIWAR_PF        0x20000000
#define PIWAR_LOCAL        0x00f00000
#define PIWAR_READ_SNOOP    0x00050000
#define PIWAR_WRITE_SNOOP    0x00005000
    u32    res2[3];
} pit_t;

/* PCI/PCI Express Registers */
typedef struct ccsr_pci
{
    u32    cfg_addr;    /* 0x000 - PCI Configuration Address Register */
    u32    cfg_data;    /* 0x004 - PCI Configuration Data Register */
    u32    int_ack;    /* 0x008 - PCI Interrupt Acknowledge Register */
    u32    out_comp_to;    /* 0x00C - PCI Outbound Completion Timeout Register */
    u32    out_conf_to;    /* 0x010 - PCI Configuration Timeout Register */
    u32    config;        /* 0x014 - PCIE CONFIG Register */
    u32    int_status;    /* 0x018 - PCIE interrupt status register */
    char    res2[4];
    u32    pme_msg_det;    /* 0x020 - PCIE PME & message detect register */
    u32    pme_msg_dis;    /* 0x024 - PCIE PME & message disable register */
    u32    pme_msg_int_en;    /* 0x028 - PCIE PME & message interrupt enable register */
    u32    pm_command;    /* 0x02c - PCIE PM Command register */
    char    res4[3016];    /*     (- #xbf8     #x30)3016 */
    u32    block_rev1;    /* 0xbf8 - PCIE Block Revision register 1 */
    u32    block_rev2;    /* 0xbfc - PCIE Block Revision register 2 */

    pot_t    pot[5];        /* 0xc00 - 0xc9f Outbound ATMU's 0, 1, 2, 3, and 4 */
    u32    res5[24];
    pit_t    pmit;        /* 0xd00 - 0xd9c Inbound ATMU's MSI */
    u32    res6[24];
    pit_t    pit[4];        /* 0xd80 - 0xdff Inbound ATMU's 3, 2, 1 and 0 */

#define PIT3 0
#define PIT2 1
#define PIT1 2

#if 0
    u32    potar0;        /* 0xc00 - PCI Outbound Transaction Address Register 0 */
    u32    potear0;    /* 0xc04 - PCI Outbound Translation Extended Address Register 0 */
    char    res5[8];
    u32    powar0;        /* 0xc10 - PCI Outbound Window Attributes Register 0 */
    char    res6[12];
    u32    potar1;        /* 0xc20 - PCI Outbound Transaction Address Register 1 */
    u32    potear1;    /* 0xc24 - PCI Outbound Translation Extended Address Register 1 */
    u32    powbar1;    /* 0xc28 - PCI Outbound Window Base Address Register 1 */
    char    res7[4];
    u32    powar1;        /* 0xc30 - PCI Outbound Window Attributes Register 1 */
    char    res8[12];
    u32    potar2;        /* 0xc40 - PCI Outbound Transaction Address Register 2 */
    u32    potear2;    /* 0xc44 - PCI Outbound Translation Extended Address Register 2 */
    u32    powbar2;    /* 0xc48 - PCI Outbound Window Base Address Register 2 */
    char    res9[4];
    u32    powar2;        /* 0xc50 - PCI Outbound Window Attributes Register 2 */
    char    res10[12];
    u32    potar3;        /* 0xc60 - PCI Outbound Transaction Address Register 3 */
    u32    potear3;    /* 0xc64 - PCI Outbound Translation Extended Address Register 3 */
    u32    powbar3;    /* 0xc68 - PCI Outbound Window Base Address Register 3 */
    char    res11[4];
    u32    powar3;        /* 0xc70 - PCI Outbound Window Attributes Register 3 */
    char    res12[12];
    u32    potar4;        /* 0xc80 - PCI Outbound Transaction Address Register 4 */
    u32    potear4;    /* 0xc84 - PCI Outbound Translation Extended Address Register 4 */
    u32    powbar4;    /* 0xc88 - PCI Outbound Window Base Address Register 4 */
    char    res13[4];
    u32    powar4;        /* 0xc90 - PCI Outbound Window Attributes Register 4 */
    char    res14[268];
    u32    pitar3;        /* 0xda0 - PCI Inbound Translation Address Register 3 */
    char    res15[4];
    u32    piwbar3;    /* 0xda8 - PCI Inbound Window Base Address Register 3 */
    u32    piwbear3;    /* 0xdac - PCI Inbound Window Base Extended Address Register 3 */
    u32    piwar3;        /* 0xdb0 - PCI Inbound Window Attributes Register 3 */
    char    res16[12];
    u32    pitar2;        /* 0xdc0 - PCI Inbound Translation Address Register 2 */
    char    res17[4];
    u32    piwbar2;    /* 0xdc8 - PCI Inbound Window Base Address Register 2 */
    u32    piwbear2;    /* 0xdcc - PCI Inbound Window Base Extended Address Register 2 */
    u32    piwar2;        /* 0xdd0 - PCI Inbound Window Attributes Register 2 */
    char    res18[12];
    u32    pitar1;        /* 0xde0 - PCI Inbound Translation Address Register 1 */
    char    res19[4];
    u32    piwbar1;    /* 0xde8 - PCI Inbound Window Base Address Register 1 */
    char    res20[4];
    u32    piwar1;        /* 0xdf0 - PCI Inbound Window Attributes Register 1 */
    char    res21[12];
#endif
    u32    pedr;        /* 0xe00 - PCI Error Detect Register */
    u32    pecdr;        /* 0xe04 - PCI Error Capture Disable Register */
    u32    peer;        /* 0xe08 - PCI Error Interrupt Enable Register */
    u32    peattrcr;    /* 0xe0c - PCI Error Attributes Capture Register */
    u32    peaddrcr;    /* 0xe10 - PCI Error Address Capture Register */
    /*    u32    perr_disr     * 0xe10 - PCIE Erorr Disable Register */
    u32    peextaddrcr;    /* 0xe14 - PCI    Error Extended Address Capture Register */
    u32    pedlcr;        /* 0xe18 - PCI Error Data Low Capture Register */
    u32    pedhcr;        /* 0xe1c - PCI Error Error Data High Capture Register */
    u32    gas_timr;    /* 0xe20 - PCI Gasket Timer Register */
    /*    u32    perr_cap_stat;     * 0xe20 - PCIE Error Capture Status Register */
    char    res22[4];
    u32    perr_cap0;    /* 0xe28 - PCIE Error Capture Register 0 */
    u32    perr_cap1;    /* 0xe2c - PCIE Error Capture Register 1 */
    u32    perr_cap2;    /* 0xe30 - PCIE Error Capture Register 2 */
    u32    perr_cap3;    /* 0xe34 - PCIE Error Capture Register 3 */
    char    res23[200];
    u32    pdb_stat;    /* 0xf00 - PCIE Debug Status */
    char    res24[16];
    u32    pex_csr0;    /* 0xf14 - PEX Control/Status register 0*/
    u32    pex_csr1;    /* 0xf18 - PEX Control/Status register 1*/
    char    res25[228];
} ccsr_fsl_pci_t;
#define PCIE_CONFIG_PC    0x00020000
#define PCIE_CONFIG_OB_CK    0x00002000
#define PCIE_CONFIG_SAC    0x00000010
#define PCIE_CONFIG_SP    0x80000002
#define PCIE_CONFIG_SCC    0x80000001

struct fsl_pci_info
{
    unsigned long regs;
    pci_addr_t mem_bus;
    phys_size_t mem_phys;
    pci_size_t mem_size;
    pci_addr_t io_bus;
    phys_size_t io_phys;
    pci_size_t io_size;
    s32 pci_num;
};

void fsl_pci_init(struct pci_controller *hose, struct fsl_pci_info *pci_info);
s32 fsl_pci_init_port(struct fsl_pci_info *pci_info,
                      struct pci_controller *hose, s32 busno);
s32 fsl_pcie_init_ctrl(s32 busno, u32 devdisr, enum srds_prtcl dev,
                       struct fsl_pci_info *pci_info);
s32 module_init_fsl_pcie(s32 busno);

#define SET_STD_PCIE_INFO(x, num) \
{            \
    x.regs = CONFIG_SYS_PCIE##num##_ADDR;    \
    x.mem_bus = CONFIG_SYS_PCIE##num##_MEM_BUS; \
    x.mem_phys = CONFIG_SYS_PCIE##num##_MEM_PHYS; \
    x.mem_size = CONFIG_SYS_PCIE##num##_MEM_SIZE; \
    x.io_bus = CONFIG_SYS_PCIE##num##_IO_BUS; \
    x.io_phys = CONFIG_SYS_PCIE##num##_IO_PHYS; \
    x.io_size = CONFIG_SYS_PCIE##num##_IO_SIZE; \
    x.pci_num = num; \
}
#define SET_STD_PCIE1_INFO(x) \
{            \
    x.regs = CONFIG_SYS_PCIE1_ADDR;    \
    x.mem_bus = CONFIG_SYS_PCIE1_MEM_BUS; \
    x.mem_phys = CONFIG_SYS_PCIE1_MEM_PHYS; \
    x.mem_size = CONFIG_SYS_PCIE1_MEM_SIZE; \
    x.io_bus = CONFIG_SYS_PCIE1_IO_BUS; \
    x.io_phys = CONFIG_SYS_PCIE1_IO_PHYS; \
    x.io_size = CONFIG_SYS_PCIE1_IO_SIZE; \
    x.pci_num = 1; \
}
#define SET_STD_PCIE2_INFO(x) \
{            \
    x.regs = CONFIG_SYS_PCIE2_ADDR;    \
    x.mem_bus = CONFIG_SYS_PCIE2_MEM_BUS; \
    x.mem_phys = CONFIG_SYS_PCIE2_MEM_PHYS; \
    x.mem_size = CONFIG_SYS_PCIE2_MEM_SIZE; \
    x.io_bus = CONFIG_SYS_PCIE2_IO_BUS; \
    x.io_phys = CONFIG_SYS_PCIE2_IO_PHYS; \
    x.io_size = CONFIG_SYS_PCIE2_IO_SIZE; \
    x.pci_num = 2; \
}

#define SET_STD_PCIE3_INFO(x) \
{            \
    x.regs = CONFIG_SYS_PCIE3_ADDR;    \
    x.mem_bus = CONFIG_SYS_PCIE3_MEM_BUS; \
    x.mem_phys = CONFIG_SYS_PCIE3_MEM_PHYS; \
    x.mem_size = CONFIG_SYS_PCIE3_MEM_SIZE; \
    x.io_bus = CONFIG_SYS_PCIE3_IO_BUS; \
    x.io_phys = CONFIG_SYS_PCIE3_IO_PHYS; \
    x.io_size = CONFIG_SYS_PCIE3_IO_SIZE; \
    x.pci_num = 3; \
}
#define SET_STD_PCIE4_INFO(x) \
{            \
    x.regs = CONFIG_SYS_PCIE4_ADDR;    \
    x.mem_bus = CONFIG_SYS_PCIE4_MEM_BUS; \
    x.mem_phys = CONFIG_SYS_PCIE4_MEM_PHYS; \
    x.mem_size = CONFIG_SYS_PCIE4_MEM_SIZE; \
    x.io_bus = CONFIG_SYS_PCIE4_IO_BUS; \
    x.io_phys = CONFIG_SYS_PCIE4_IO_PHYS; \
    x.io_size = CONFIG_SYS_PCIE4_IO_SIZE; \
    x.pci_num = 4; \
}

#endif /*__ppc1020_pci_h__*/
