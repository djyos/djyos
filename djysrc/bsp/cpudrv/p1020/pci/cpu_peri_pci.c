//----------------------------------------------------
//Copyright (C), 2013-2020,  luoshitian.
//版权所有 (C), 2013-2020,   罗侍田.
//所属模块: pcie驱动模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: freescale p1020中pcie模块驱动文件，初始化pcie控制器并扫描pcie设备
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-11-14
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include <asm/bitops.h>
#include <asm/io.h>
#include <cpu_peri.h>

#define max(x, y)    (((x) < (y)) ? (y) : (x))
#define min(x, y)    (((x) < (y)) ? (x) : (y))

#ifndef CONFIG_SYS_PCI_MEMORY_BUS
#define CONFIG_SYS_PCI_MEMORY_BUS 0
#endif

#ifndef CONFIG_SYS_PCI_MEMORY_PHYS
#define CONFIG_SYS_PCI_MEMORY_PHYS 0
#endif

#if (CONFIG_SYS_PCI_64BIT==1)
#define CONFIG_SYS_PCI64_MEMORY_BUS (64ull*1024*1024*1024)
#endif

//----设置inbound窗口寄存器-----------------------------------------------------------------
//功能：根据pci_region中配置设置inbound相关的地址和属性寄存器，根据sz设置inbound窗口的size大小
//参数：pit_t *pi,inbound相关的寄存器，pci_region *r，pci中用来存放IO空间、存储空间、预读空间等的结构体，
//size，配置inbound窗口的大小
//返回：无
//-----------------------------------------------------------------------------
static void set_inbound_window(volatile pit_t *pi,
                               struct pci_region *r,
                               u64 size)
{
    u32 sz = (__ilog2_u64(size) - 1);
    u32 flag = PIWAR_EN | PIWAR_LOCAL |
               PIWAR_READ_SNOOP | PIWAR_WRITE_SNOOP;
    out_be32(&pi->pitar, r->phys_start >> 12);
    out_be32(&pi->piwbar, r->bus_start >> 12);
#if (CONFIG_SYS_PCI_64BIT == 1)
    out_be32(&pi->piwbear, r->bus_start >> 44);
#else
    out_be32(&pi->piwbear, 0);
#endif
    if (r->flags & PCI_REGION_PREFETCH)
        flag |= PIWAR_PF;
    out_be32(&pi->piwar, flag | sz);
}
//----初始化pcie主桥控制器配置结构体-----------------------------------------------------------------
//功能：初始化pcie主桥控制器配置结构体pci_controller，存储对应的配置地址
//参数：pci_controller *hose,pcie主桥控制器配置结构体，unsigned long addr，pcie控制器寄存器地址
//返回：PCIE主控制器是配置成主桥还是设备
//-----------------------------------------------------------------------------
int fsl_setup_hose(struct pci_controller *hose, unsigned long addr)
{
    volatile ccsr_fsl_pci_t *pci = (ccsr_fsl_pci_t *) addr;

    /* Reset hose to make sure its in a clean state */
    memset(hose, 0, sizeof(struct pci_controller));

    pci_setup_indirect(hose, (u32)&pci->cfg_addr, (u32)&pci->cfg_data);

    return fsl_is_pci_agent(hose);
}
static unsigned char setInboundPrefetch=0;
//----给inbound窗口设置prefetch模式-----------------------------------------------------------------
//功能：给inbound窗口设置prefetch模式，要在pcie主控制器初始化之前调用
//如果不调用，则默认不用prefetch
//参数：无
//返回：无
//-----------------------------------------------------------------------------
void fsl_pci_set_inbound_prefetch_mode()
{
    setInboundPrefetch = 1;
}
//----设置inbound窗口-----------------------------------------------------------------
//功能：设置inbound窗口，分配相应的地址空间
//参数：pci_controller *hose,pcie主桥控制器配置结构体，u64 out_lo，outbound分配的低地址
//u8 pcie，cap pcie能力寄存器值，pit_t *pi,inbound相关的寄存器
//返回：返回0表示没给inbound分配地址空间，1表示分配了相应空间
//-----------------------------------------------------------------------------
static int fsl_pci_setup_inbound_windows(struct pci_controller *hose,
        u64 out_lo, u8 pcie_cap,
        volatile pit_t *pi)
{
    struct pci_region *r = hose->regions + hose->region_count;
    u64 sz = min((u64)0x10000000, (1ull << 32));

    phys_addr_t phys_start = CONFIG_SYS_PCI_MEMORY_PHYS;
    pci_addr_t bus_start = CONFIG_SYS_PCI_MEMORY_BUS;
    pci_size_t pci_sz;

    /* we have no space available for inbound memory mapping */
    if (bus_start > out_lo)
    {
        printf ("no space for inbound mapping of memory\r\n");
        return 0;
    }

    /* limit size */
    if ((bus_start + sz) > out_lo)
    {
        sz = out_lo - bus_start;
        fslpcie_debug("limiting size to %llx\r\n", sz);
    }

    pci_sz = 1ull << __ilog2_u64(sz);
    /*
     * we can overlap inbound/outbound windows on PCI-E since RX & TX
     * links a separate
     */
    if ((pcie_cap == PCI_CAP_ID_EXP) && (pci_sz < sz))
    {
        fslpcie_debug("ID R0 bus_start: %llx phys_start: %llx size: %llx\r\n",
                      (u64)bus_start, (u64)phys_start, (u64)sz);

        pci_set_region(r, bus_start, phys_start, sz,
                       PCI_REGION_MEM | PCI_REGION_SYS_MEMORY
                      );

        /* if we aren't an exact power of two match, pci_sz is smaller
         * round it up to the next power of two.  We report the actual
         * size to pci region tracking.
         */
        if (pci_sz != sz)
            sz = 2ull << __ilog2_u64(sz);

        set_inbound_window(pi--, r++, sz);
        sz = 0; /* make sure we dont set the R2 window */
    }
    else
    {
        fslpcie_debug("R0 bus_start: %llx phys_start: %llx size: %llx\r\n",
                      (u64)bus_start, (u64)phys_start, (u64)pci_sz);

        if(setInboundPrefetch)
            pci_set_region(r, bus_start, phys_start, pci_sz,
                           PCI_REGION_MEM | PCI_REGION_SYS_MEMORY |
                           PCI_REGION_PREFETCH);
        else
            pci_set_region(r, bus_start, phys_start, pci_sz,
                           PCI_REGION_MEM | PCI_REGION_SYS_MEMORY /*|
                            PCI_REGION_PREFETCH*/);
        set_inbound_window(pi--, r++, pci_sz);

        sz -= pci_sz;
        bus_start += pci_sz;
        phys_start += pci_sz;

        pci_sz = 1ull << __ilog2_u64(sz);
        if (sz)
        {
            fslpcie_debug("R1 bus_start: %llx phys_start: %llx size: %llx\r\n",
                          (u64)bus_start, (u64)phys_start, (u64)pci_sz);
            if(setInboundPrefetch)
                pci_set_region(r, bus_start, phys_start, pci_sz,
                               PCI_REGION_MEM | PCI_REGION_SYS_MEMORY |
                               PCI_REGION_PREFETCH);
            else
                pci_set_region(r, bus_start, phys_start, pci_sz,
                               PCI_REGION_MEM | PCI_REGION_SYS_MEMORY /*|
                                PCI_REGION_PREFETCH*/);
            set_inbound_window(pi--, r++, pci_sz);
            sz -= pci_sz;
            bus_start += pci_sz;
            phys_start += pci_sz;
        }
    }

#if (CONFIG_PHYS_64BIT == 1) && (CONFIG_SYS_PCI_64BIT == 1)
    /*
     * On 64-bit capable systems, set up a mapping for all of DRAM
     * in high pci address space.
     */
    pci_sz = 1ull << __ilog2_u64(PHYS_MEM_SIZE);
    /* round up to the next largest power of two */
    if (PHYS_MEM_SIZE > pci_sz)
        pci_sz = 1ull << (__ilog2_u64(PHYS_MEM_SIZE) + 1);
    fslpcie_debug ("R64 bus_start: %llx phys_start: %llx size: %llx\r\n",
                   (u64)CONFIG_SYS_PCI64_MEMORY_BUS,
                   (u64)CONFIG_SYS_PCI_MEMORY_PHYS,
                   (u64)pci_sz);
    pci_set_region(r,
                   CONFIG_SYS_PCI64_MEMORY_BUS,
                   CONFIG_SYS_PCI_MEMORY_PHYS,
                   pci_sz,
                   PCI_REGION_MEM | PCI_REGION_SYS_MEMORY |
                   PCI_REGION_PREFETCH);
    set_inbound_window(pi--, r++, pci_sz);
#else
    pci_sz = 1ull << __ilog2_u64(sz);
    if (sz)
    {
        fslpcie_debug("R2 bus_start: %llx phys_start: %llx size: %llx\r\n",
                      (u64)bus_start, (u64)phys_start, (u64)pci_sz);
        pci_set_region(r, bus_start, phys_start, pci_sz,
                       PCI_REGION_MEM | PCI_REGION_SYS_MEMORY |
                       PCI_REGION_PREFETCH);
        sz -= pci_sz;
        bus_start += pci_sz;
        phys_start += pci_sz;
        set_inbound_window(pi--, r++, pci_sz);
    }
#endif

#if (CONFIG_PHYS_64BIT == 1)
    if (sz && ((PHYS_MEM_SIZE) < (1ull << 32)))
        printf("Was not able to map all of memory via "
                   "inbound windows -- %lld remaining\r\n", sz);
#endif

    hose->region_count = r - hose->regions;

    return 1;
}


//----PCIE自动扫描配置初始化-----------------------------------------------------------------
//功能：设置PCIE自动扫描配置初始化，为上电PCIE扫描设备，为挂接PCIE设备分配地址空间等做初始化准备
//参数：pci_controller *hose,pcie主桥控制器配置结构体
//返回：无
//-----------------------------------------------------------------------------
void pciauto_config_init(struct pci_controller *hose)
{
    int i;
    hose->pci_io = hose->pci_mem = 0;

    for (i = 0; i < hose->region_count; i++)
    {
        switch(hose->regions[i].flags)
        {
        case PCI_REGION_IO:
            if (!hose->pci_io ||
                    hose->pci_io->size < hose->regions[i].size)
                hose->pci_io = hose->regions + i;
            break;
        case PCI_REGION_MEM:
            if (!hose->pci_mem ||
                    hose->pci_mem->size < hose->regions[i].size)
                hose->pci_mem = hose->regions + i;
            break;
        case (PCI_REGION_MEM | PCI_REGION_PREFETCH):
            if (!hose->pci_prefetch ||
                    hose->pci_prefetch->size < hose->regions[i].size)
                hose->pci_prefetch = hose->regions + i;
            break;
        }
    }


    if (hose->pci_mem)
    {
        pciauto_region_init(hose->pci_mem);

        fslpcie_debug("PCI Autoconfig: Bus Memory region: [0x%llx-0x%llx],\r\n"
                      "\t\tPhysical Memory [%llx-%llx]\r\n",
                      (u64)hose->pci_mem->bus_start,
                      (u64)(hose->pci_mem->bus_start + hose->pci_mem->size - 1),
                      (u64)hose->pci_mem->phys_start,
                      (u64)(hose->pci_mem->phys_start + hose->pci_mem->size - 1));
    }

    if (hose->pci_prefetch)
    {
        pciauto_region_init(hose->pci_prefetch);

        fslpcie_debug("PCI Autoconfig: Bus Prefetchable Mem: [0x%llx-0x%llx],\r\n"
                      "\t\tPhysical Memory [%llx-%llx]\r\n",
                      (u64)hose->pci_prefetch->bus_start,
                      (u64)(hose->pci_prefetch->bus_start +
                            hose->pci_prefetch->size - 1),
                      (u64)hose->pci_prefetch->phys_start,
                      (u64)(hose->pci_prefetch->phys_start +
                            hose->pci_prefetch->size - 1));
    }

    if (hose->pci_io)
    {
        pciauto_region_init(hose->pci_io);

        fslpcie_debug("PCI Autoconfig: Bus I/O region: [0x%llx-0x%llx],\r\n"
                      "\t\tPhysical Memory: [%llx-%llx]\r\n",
                      (u64)hose->pci_io->bus_start,
                      (u64)(hose->pci_io->bus_start + hose->pci_io->size - 1),
                      (u64)hose->pci_io->phys_start,
                      (u64)(hose->pci_io->phys_start + hose->pci_io->size - 1));

    }
}
//----PCIE主控制器初始化-----------------------------------------------------------------
//功能：PCIE主控制器初始化，fsl pcie主控制器初始化和上电扫描pcie设备，并配置pcie设备
//参数：pci_controller *hose,pcie主桥控制器配置结构体，fsl_pci_info *pci_info，用来存储MEM/IO基地址和大小和law寄存器
//返回：无
//-----------------------------------------------------------------------------
void fsl_pci_init(struct pci_controller *hose, struct fsl_pci_info *pci_info)
{
    u32 cfg_addr = (u32)&((ccsr_fsl_pci_t *)pci_info->regs)->cfg_addr;
    u32 cfg_data = (u32)&((ccsr_fsl_pci_t *)pci_info->regs)->cfg_data;
    u16 temp16;
    u32 temp32;
    u32 block_rev;
    int enabled, r, inbound = 0;
    u16 ltssm;
    u8 pcie_cap;
    volatile ccsr_fsl_pci_t *pci = (ccsr_fsl_pci_t *)cfg_addr;
    struct pci_region *reg = hose->regions + hose->region_count;

    pci_dev_t dev = PCI_BDF(hose->first_busno, 0, 0);

    /* Initialize ATMU registers based on hose regions and flags */
    volatile pot_t *po = &pci->pot[1];    /* skip 0 */
    volatile pit_t *pi;

    u64 out_hi = 0, out_lo = -1ULL;
    u32 pcicsrbar, pcicsrbar_sz;

    pci_setup_indirect(hose, cfg_addr, cfg_data);

    block_rev = in_be32(&pci->block_rev1);

    if (PEX_IP_BLK_REV_2_2 <= block_rev)
    {
        pi = &pci->pit[2];    /* 0xDC0 */
    }
    else
    {
        pi = &pci->pit[3];    /* 0xDE0 */
    }
    /* Handle setup of outbound windows first */
    for (r = 0; r < hose->region_count; r++)
    {
        unsigned long flags = hose->regions[r].flags;
        u32 sz = (__ilog2_u64((u64)hose->regions[r].size) - 1);

        flags &= PCI_REGION_SYS_MEMORY|PCI_REGION_TYPE;
        if (flags != PCI_REGION_SYS_MEMORY)
        {
            u64 start = hose->regions[r].bus_start;
            u64 end = start + hose->regions[r].size;

            out_be32(&po->powbar, hose->regions[r].phys_start >> 12);
            out_be32(&po->potar, start >> 12);
#if (CONFIG_SYS_PCI_64BIT == 1)
            out_be32(&po->potear, start >> 44);
#else
            out_be32(&po->potear, 0);
#endif
            if (hose->regions[r].flags & PCI_REGION_IO)
            {
                out_be32(&po->powar, POWAR_EN | sz |
                         POWAR_IO_READ | POWAR_IO_WRITE);
            }
            else
            {
                out_be32(&po->powar, POWAR_EN | sz |
                         POWAR_MEM_READ | POWAR_MEM_WRITE);
                out_lo = min(start, out_lo);
                out_hi = max(end, out_hi);
            }
            po++;
        }
    }
    fslpcie_debug("Outbound memory range: %llx:%llx\r\n", out_lo, out_hi);

    /* setup PCSRBAR/PEXCSRBAR */
    pci_hose_write_config_dword(hose, dev, PCI_BASE_ADDRESS_0, 0xffffffff);
    pci_hose_read_config_dword (hose, dev, PCI_BASE_ADDRESS_0, &pcicsrbar_sz);

    pcicsrbar_sz = ~pcicsrbar_sz + 1;

    if (out_hi < (0x100000000ull - pcicsrbar_sz) ||
            (out_lo > 0x100000000ull))
        pcicsrbar = 0x100000000ull - pcicsrbar_sz;
    else
        pcicsrbar = (out_lo - pcicsrbar_sz) & -pcicsrbar_sz;


    pci_hose_write_config_dword(hose, dev, PCI_BASE_ADDRESS_0, pcicsrbar);

    out_lo = min(out_lo, (u64)pcicsrbar);

    fslpcie_debug("PCICSRBAR @ 0x%x\r\n", pcicsrbar);

    pci_set_region(reg++, pcicsrbar, CONFIG_SYS_CCSRBAR_PHYS,
                   pcicsrbar_sz, PCI_REGION_SYS_MEMORY);
    hose->region_count++;

    /* see if we are a PCIe or PCI controller */
    pci_hose_read_config_byte(hose, dev, FSL_PCIE_CAP_ID, &pcie_cap);


    /* inbound */
    inbound = fsl_pci_setup_inbound_windows(hose, out_lo, pcie_cap, pi);

    for (r = 0; r < hose->region_count; r++)
        fslpcie_debug("PCI reg:%d    %016llx:%016llx %016llx %08lx\r\n", r,
                      (u64)hose->regions[r].phys_start,
                      (u64)hose->regions[r].bus_start,
                      (u64)hose->regions[r].size,
                      hose->regions[r].flags);



    pci_register_hose(hose);
    pciauto_config_init(hose);    /* grab pci_{mem,prefetch,io} */
    hose->current_busno = hose->first_busno;

    out_be32(&pci->pedr, 0xffffffff);    /* Clear any errors */
    out_be32(&pci->peer, ~0x20140);    /* Enable All Error Interrupts except
                                        * - Master abort (pci)
                                        * - Master PERR (pci)
                                        * - ICCA (PCIe)
                                        */
    pci_hose_read_config_dword(hose, dev, PCI_DCR, &temp32);
    temp32 |= 0xf000e;        /* set URR, FER, NFER (but not CER) */
    temp32 = 0x081e;/*MAX_PAYLOAD_SIZE配置为128bytes，MAX_READ_SIZE设置为128字节*/
    pci_hose_write_config_dword(hose, dev, PCI_DCR, temp32);

#if (CONFIG_FSL_PCIE_DISABLE_ASPM == 1)
    temp32 = 0;
    pci_hose_read_config_dword(hose, dev, PCI_LCR, &temp32);
    temp32 &= ~0x03;        /* Disable ASPM  */
    pci_hose_write_config_dword(hose, dev, PCI_LCR, temp32);
    Djy_DelayUs(10);

#endif
    if (pcie_cap == PCI_CAP_ID_EXP)
    {
        if (block_rev >= PEX_IP_BLK_REV_3_0)
        {
#define PEX_CSR0_LTSSM_MASK    0xFC
#define PEX_CSR0_LTSSM_SHIFT   2
            ltssm = (in_be32(&pci->pex_csr0)
                     & PEX_CSR0_LTSSM_MASK) >> PEX_CSR0_LTSSM_SHIFT;
            enabled = (ltssm == 0x11) ? 1 : 0;
        }
        else
        {
            /* pci_hose_read_config_word(hose, dev, PCI_LTSSM, &ltssm); */
            /* enabled = ltssm >= PCI_LTSSM_L0; */
            pci_hose_read_config_word(hose, dev, PCI_LTSSM, &ltssm);

            enabled = ltssm >= PCI_LTSSM_L0;
#if 0
#ifdef CONFIG_FSL_PCIE_RESET
            if (ltssm == 1)
            {
                int i;
                fslpcie_debug("....PCIe link error. " "LTSSM=0x%02x.", ltssm);
                /* assert PCIe reset */
                setbits_be32(&pci->pdb_stat, 0x08000000);
                (void) in_be32(&pci->pdb_stat);
                Djy_DelayUs(100);
                fslpcie_debug("  Asserting PCIe reset @%p = %x\r\n",
                              &pci->pdb_stat, in_be32(&pci->pdb_stat));
                /* clear PCIe reset */
                clrbits_be32(&pci->pdb_stat, 0x08000000);
                __asm__("sync;isync");
                for (i=0; i<100 && ltssm < PCI_LTSSM_L0; i++)
                {
                    pci_hose_read_config_word(hose, dev, PCI_LTSSM,
                                              &ltssm);
                    Djy_DelayUs(1000);
                    fslpcie_debug("....PCIe link error. "
                                  "LTSSM=0x%02x.\r\n", ltssm);
                }
                enabled = ltssm >= PCI_LTSSM_L0;

                /* we need to re-write the bar0 since a reset will
                 * clear it
                 */
                pci_hose_write_config_dword(hose, dev,
                                            PCI_BASE_ADDRESS_0, pcicsrbar);
            }
#endif
#endif
        }

        if (!enabled)
        {
            /* Let the user know there's no PCIe link */
            printf("no link, regs @ 0x%8x\r\n", pci_info->regs);
            hose->last_busno = hose->first_busno;
            return;
        }

        out_be32(&pci->pme_msg_det, 0xffffffff);
        out_be32(&pci->pme_msg_int_en, 0xffffffff);

        /* Print the negotiated PCIe link width */
        pci_hose_read_config_word(hose, dev, PCI_LSR, &temp16);
        printf("x%d, regs @ 0x%8x\r\n", (temp16 & 0x3f0 ) >> 4,
                   pci_info->regs);

        hose->current_busno++; /* Start scan with secondary */
        pciauto_prescan_setup_bridge(hose, dev, hose->current_busno);
    }

    /* Use generic setup_device to initialize standard pci regs,
     * but do not allocate any windows since any BAR found (such
     * as PCSRBAR) is not in this cpu's memory space.
     */
    pciauto_setup_device(hose, dev, 0, hose->pci_mem,
                         hose->pci_prefetch, hose->pci_io);

    if (inbound)
    {
        pci_hose_read_config_word(hose, dev, PCI_COMMAND, &temp16);
        pci_hose_write_config_word(hose, dev, PCI_COMMAND,
                                   temp16 | PCI_COMMAND_MEMORY);
    }

#if (CONFIG_PCI_NOSCAN == 0)
    if (!fsl_is_pci_agent(hose))
    {
        fslpcie_debug("           Scanning PCI bus %02x\r\n",
                      hose->current_busno);
        hose->last_busno = pci_hose_scan_bus(hose, hose->current_busno);
    }
    else
    {
        fslpcie_debug("           Not scanning PCI bus %02x\r\n",
                      hose->current_busno);
        hose->last_busno = hose->current_busno;
    }

    /* if we are PCIe - update limit regs and subordinate busno
     * for the virtual P2P bridge
     */
    if (pcie_cap == PCI_CAP_ID_EXP)
    {
        pciauto_postscan_setup_bridge(hose, dev, hose->last_busno);
    }
#else
    hose->last_busno = hose->current_busno;
#endif

    /* Clear all error indications */
    if (pcie_cap == PCI_CAP_ID_EXP)
        out_be32(&pci->pme_msg_det, 0xffffffff);
    out_be32(&pci->pedr, 0xffffffff);

    pci_hose_read_config_word (hose, dev, PCI_DSR, &temp16);
    if (temp16)
    {
        pci_hose_write_config_word(hose, dev, PCI_DSR, 0xffff);
    }

    pci_hose_read_config_word (hose, dev, PCI_SEC_STATUS, &temp16);
    if (temp16)
    {
        pci_hose_write_config_word(hose, dev, PCI_SEC_STATUS, 0xffff);
    }
}
//----判断pcie主控制器是设置成主桥还是设备-----------------------------------------------------------------
//功能：判断pcie主控制器是设置成主桥还是设备
//参数：pci_controller *hose,pcie主桥控制器配置结构体
//返回：0表示设置为设备，1表示设置为主桥
//-----------------------------------------------------------------------------
int fsl_is_pci_agent(struct pci_controller *hose)
{
    u8 pcie_cap;
    pci_dev_t dev = PCI_BDF(hose->first_busno, 0, 0);

    pci_hose_read_config_byte(hose, dev, FSL_PCIE_CAP_ID, &pcie_cap);

    if (pcie_cap == PCI_CAP_ID_EXP)
    {
        u8 header_type;

        pci_hose_read_config_byte(hose, dev, PCI_HEADER_TYPE,
                                  &header_type);
        return (header_type & 0x7f) == PCI_HEADER_TYPE_NORMAL;
    }
    else
    {
        u8 prog_if;

        pci_hose_read_config_byte(hose, dev, PCI_CLASS_PROG, &prog_if);
        /* Programming Interface (PCI_CLASS_PROG)
         * 0 == pci host or pcie root-complex,
         * 1 == pci agent or pcie end-point
         */
        return (prog_if == FSL_PROG_IF_AGENT);
    }
}
//----初始化PCIE端口-----------------------------------------------------------------
//功能：初始化PCIE主桥控制器，设置pcie region，分配的bus号
//参数：pci_controller *hose,pcie主桥控制器配置结构体，fsl_pci_info *pci_info，用来存储MEM/IO基地址和大小和law寄存器
//int busno，前一个bus号
//返回：最末尾的bus号
//-----------------------------------------------------------------------------
int fsl_pci_init_port(struct fsl_pci_info *pci_info,
                      struct pci_controller *hose, int busno)
{
    volatile ccsr_fsl_pci_t *pci;
    struct pci_region *r;
    pci_dev_t dev = PCI_BDF(busno,0,0);
    u8 pcie_cap;

    pci = (ccsr_fsl_pci_t *) pci_info->regs;

    /* on non-PCIe controllers we don't have pme_msg_det so this code
     * should do nothing since the read will return 0
     */
    if (in_be32(&pci->pme_msg_det))
    {
        out_be32(&pci->pme_msg_det, 0xffffffff);
        fslpcie_debug(" with errors.  Clearing.  Now 0x%08x",
                      pci->pme_msg_det);
    }

    r = hose->regions + hose->region_count;

    /* outbound memory */
    pci_set_region(r++,
                   pci_info->mem_bus,
                   pci_info->mem_phys,
                   pci_info->mem_size,
                   PCI_REGION_MEM);

    /* outbound io */
    pci_set_region(r++,
                   pci_info->io_bus,
                   pci_info->io_phys,
                   pci_info->io_size,
                   PCI_REGION_IO);

    hose->region_count = r - hose->regions;
    hose->first_busno = busno;

    fsl_pci_init(hose, pci_info);

    if (fsl_is_pci_agent(hose))
    {
        fsl_pci_config_unlock(hose);
        hose->last_busno = hose->first_busno;

    }

    pci_hose_read_config_byte(hose, dev, FSL_PCIE_CAP_ID, &pcie_cap);
    printf("PCI%s%x: Bus %02x - %02x\r\n", pcie_cap == PCI_CAP_ID_EXP ?
               "e" : "", pci_info->pci_num,
               hose->first_busno, hose->last_busno);

    return(hose->last_busno + 1);
}
//----使能PCIe能接收inbound配置请求 -----------------------------------------------------------------
//功能：只有在pcie主控制器被设置为设备时才能使用，使能PCIe能接收inbound配置请求
//参数：pci_controller *hose,pcie主桥控制器配置结构体
//返回：无
//-----------------------------------------------------------------------------
void fsl_pci_config_unlock(struct pci_controller *hose)
{
    pci_dev_t dev = PCI_BDF(hose->first_busno,0,0);
    u8 pcie_cap;
    u16 pbfr;

    if (!fsl_is_pci_agent(hose))
        return;

    pci_hose_read_config_byte(hose, dev, FSL_PCIE_CAP_ID, &pcie_cap);
    if (pcie_cap != 0x0)
    {
        /* PCIe - set CFG_READY bit of Configuration Ready Register */
        pci_hose_write_config_byte(hose, dev, FSL_PCIE_CFG_RDY, 0x1);
    }
    else
    {
        /* PCI - clear ACL bit of PBFR */
        pci_hose_read_config_word(hose, dev, FSL_PCI_PBFR, &pbfr);
        pbfr &= ~0x20;
        pci_hose_write_config_word(hose, dev, FSL_PCI_PBFR, pbfr);
    }
}

#if (CONFIG_PCIE1 == 1) || (CONFIG_PCIE2 == 1) || \
    (CONFIG_PCIE3 == 0) || (CONFIG_PCIE4 == 0)
//----配置fsl PCIE主控制器函数 -----------------------------------------------------------------
//功能：设置pcie相关law寄存器，配置fsl PCIE主控制器
//参数：pci_controller *hose,pcie主桥控制器配置结构体，fsl_pci_info *pci_info，用来存储MEM/IO基地址和大小和law寄存器
//const char *connected，PCIE命名，int busno，分配的bus号
//返回：最末尾的bus号
//-----------------------------------------------------------------------------
int fsl_configure_pcie(struct fsl_pci_info *info,
                       struct pci_controller *hose,
                       const char *connected, int busno)
{
    int is_endpoint;

    is_endpoint = fsl_setup_hose(hose, info->regs);
    printf("PCIe%u: %s ", info->pci_num,
               is_endpoint ? "Endpoint" : "Root Complex");
    if (connected)
        printf(" of %s", connected);
    printf(", ");

    return fsl_pci_init_port(info, hose, busno);
}


#define _DEVDISR_PCIE1 MPC85xx_DEVDISR_PCIE
#define _DEVDISR_PCIE2 MPC85xx_DEVDISR_PCIE2
#define _DEVDISR_PCIE3 MPC85xx_DEVDISR_PCIE3
#define _DEVDISR_PCIE4 0
#define CONFIG_SYS_MPC8xxx_GUTS_ADDR CONFIG_SYS_MPC85xx_GUTS_ADDR

/* Implement a dummy function for those platforms w/o SERDES */
static const char *__board_serdes_name(enum srds_prtcl device)
{
    switch (device)
    {
#ifdef CONFIG_SYS_PCIE1_NAME
    case PCIE1:
        return CONFIG_SYS_PCIE1_NAME;
#endif
#ifdef CONFIG_SYS_PCIE2_NAME
    case PCIE2:
        return CONFIG_SYS_PCIE2_NAME;
#endif
#ifdef CONFIG_SYS_PCIE3_NAME
    case PCIE3:
        return CONFIG_SYS_PCIE3_NAME;
#endif
#ifdef CONFIG_SYS_PCIE4_NAME
    case PCIE4:
        return CONFIG_SYS_PCIE4_NAME;
#endif
    default:
        return 0;
    }

    return 0;
}

__attribute__((weak, alias("__board_serdes_name"))) const char *
board_serdes_name(enum srds_prtcl device);

static u32 devdisr_mask[] =
{
    _DEVDISR_PCIE1,
    _DEVDISR_PCIE2,
    _DEVDISR_PCIE3,
    _DEVDISR_PCIE4,
};
//----配置对应fsl PCIE主控制器的主函数 -----------------------------------------------------------------
//功能：配置pcie对应serders寄存器，配置PCIE主控制器
//参数：int busno，分配的bus号，u32 devdisr，记录对应PCIE是否禁用serdes的参数，
//srds_prtcl dev，serdes枚举变量，fsl_pci_info *pci_info，用来存储MEM/IO基地址和大小和law寄存器
//返回：最末尾的bus号
//-----------------------------------------------------------------------------
int fsl_pcie_init_ctrl(int busno, u32 devdisr, enum srds_prtcl dev,
                       struct fsl_pci_info *pci_info)
{
    struct pci_controller *hose;
    int num = dev - PCIE1;

    hose = M_Malloc( sizeof(struct pci_controller),0);
    if (!hose)
        return busno;

    if (is_serdes_configured(dev) && !(devdisr & devdisr_mask[num]))
    {
        busno = fsl_configure_pcie(pci_info, hose,
                                   board_serdes_name(dev), busno);
    }
    else
    {
        printf("PCIe%d: disabled\r\n", num + 1);
    }
    printf("busno = %d\r\n",busno);
    return busno;
}
//----pcie初始化-----------------------------------------------------------------
//功能：fsl所有pcie主控制器初始化
//参数：int busno，分配的bus号，一般取0
//返回：最末尾的bus号
//-----------------------------------------------------------------------------
int module_init_fsl_pcie(int busno)
{
    struct fsl_pci_info pci_info;
    ccsr_gur_t *gur = (void *)CONFIG_SYS_MPC8xxx_GUTS_ADDR;
    u32 devdisr;
    u32 *addr;

    addr = &gur->devdisr;

    devdisr = in_be32(addr);
#if (CONFIG_PCIE1 == 1)
    SET_STD_PCIE1_INFO(pci_info);
    busno = fsl_pcie_init_ctrl(busno, devdisr, PCIE1, &pci_info);
#else
    setbits_be32(addr, _DEVDISR_PCIE1); /* disable */
#endif

#if (CONFIG_PCIE2 == 1)
    SET_STD_PCIE2_INFO(pci_info);
    busno = fsl_pcie_init_ctrl(busno, devdisr, PCIE2, &pci_info);
#else
    setbits_be32(addr, _DEVDISR_PCIE2); /* disable */
#endif

#if (CONFIG_PCIE3 == 1)
    SET_STD_PCIE3_INFO(pci_info);
    busno = fsl_pcie_init_ctrl(busno, devdisr, PCIE3, &pci_info);
#else
    setbits_be32(addr, _DEVDISR_PCIE3); /* disable */
#endif

#if (CONFIG_PCIE4 == 1)
    SET_STD_PCIE4_INFO(pci_info);
    busno = fsl_pcie_init_ctrl(busno, devdisr, PCIE4, &pci_info);
#else
    setbits_be32(addr, _DEVDISR_PCIE4); /* disable */
#endif
    return busno;
}
#endif
