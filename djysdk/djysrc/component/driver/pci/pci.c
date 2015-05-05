/*
 * Support for indirect PCI bridges.
 *
 * Copyright (C) 1998 Gabriel Paubert.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
/*
 *
 * PCI autoconfiguration library
 *
 * Author: Matt Porter <mporter@mvista.com>
 *
 * Copyright 2000 MontaVista Software Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */
/*
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>
 *
 * (C) Copyright 2002, 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
/*
 * Copyright 2007-2012 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
//------------------------------------------------------
#include "stdint.h"
#include "pci.h"
#include <stdio.h>

#define max(x, y)    (((x) < (y)) ? (y) : (x))
#define min(x, y)    (((x) < (y)) ? (x) : (y))

#define MAP_NOCACHE    (0)
#define MAP_WRCOMBINE    (0)
#define MAP_WRBACK    (0)
#define MAP_WRTHROUGH    (0)

#define    PCIAUTO_IDE_MODE_MASK        0x05

/* the user can define CONFIG_SYS_PCI_CACHE_LINE_SIZE to avoid problems */
#ifndef CONFIG_SYS_PCI_CACHE_LINE_SIZE
#define CONFIG_SYS_PCI_CACHE_LINE_SIZE    8
#endif
s32 pci_hose_read_config_byte(struct pci_controller *hose,
                              pci_dev_t dev, s32 where, u8 *val)
{
    return indirect_read_config_byte(hose, dev, where, val);
}

s32 pci_hose_read_config_word(struct pci_controller *hose,
                              pci_dev_t dev, s32 where, u16 *val)
{
    return indirect_read_config_word(hose, dev, where, val);
}

s32 pci_hose_read_config_dword(struct pci_controller *hose,
                               pci_dev_t dev, s32 where, u32 *val)
{
    return indirect_read_config_dword(hose, dev, where, val);
}

s32 pci_hose_write_config_byte(struct pci_controller *hose,
                               pci_dev_t dev, s32 where, u8 val)
{
    return indirect_write_config_byte(hose, dev, where, val);
}

s32 pci_hose_write_config_word(struct pci_controller *hose,
                               pci_dev_t dev, s32 where, u16 val)
{
    return indirect_write_config_word(hose, dev, where, val);
}


s32 pci_hose_write_config_dword(struct pci_controller *hose,
                                pci_dev_t dev, s32 where, u32 val)
{
    return indirect_write_config_dword(hose, dev, where, val);
}

s32 pci_read_config_byte(pci_dev_t dev, s32 offset, u8* value)
{
    struct pci_controller *hose = pci_bus_to_hose(PCI_BUS(dev));

    if (!hose)
    {
        return -1;
    }

    return pci_hose_read_config_byte(hose, dev, offset, value);
}

s32 pci_read_config_word(pci_dev_t dev, s32 offset, u16* value)
{
    struct pci_controller *hose = pci_bus_to_hose(PCI_BUS(dev));

    if (!hose)
    {
        return -1;
    }

    return pci_hose_read_config_word(hose, dev, offset, value);
}

s32 pci_read_config_dword(pci_dev_t dev, s32 offset, u32* value)
{
    struct pci_controller *hose = pci_bus_to_hose(PCI_BUS(dev));

    if (!hose)
    {
        return -1;
    }

    return pci_hose_read_config_dword(hose, dev, offset, value);
}

s32 pci_write_config_byte(pci_dev_t dev, s32 offset, u8 value)
{
    struct pci_controller *hose = pci_bus_to_hose(PCI_BUS(dev));

    if (!hose)
    {
        return -1;
    }

    return pci_hose_write_config_byte(hose, dev, offset, value);
}

s32 pci_write_config_word(pci_dev_t dev, s32 offset, u16 value)
{
    struct pci_controller *hose = pci_bus_to_hose(PCI_BUS(dev));

    if (!hose)
    {
        return -1;
    }

    return pci_hose_write_config_word(hose, dev, offset, value);
}

s32 pci_write_config_dword(pci_dev_t dev, s32 offset, u32 value)
{
    struct pci_controller *hose = pci_bus_to_hose(PCI_BUS(dev));

    if (!hose)
    {
        return -1;
    }

    return pci_hose_write_config_dword(hose, dev, offset, value);
}

s32 pci_hose_read_config_byte_via_dword(struct pci_controller *hose,pci_dev_t dev,s32 offset, u8 * val)
{
    u32 val32;

    if (pci_hose_read_config_dword(hose, dev, offset & 0xfc, &val32) < 0)
    {
        *val = -1;
        return -1;
    }

    *val = (val32 >> ((offset & (s32)0x03) * 8));

    return 0;
}

s32 pci_hose_read_config_word_via_dword(struct pci_controller *hose,pci_dev_t dev,s32 offset, u16 * val)
{
    u32 val32;

    if (pci_hose_read_config_dword(hose, dev, offset & 0xfc, &val32) < 0)
    {
        *val = -1;
        return -1;
    }

    *val = (val32 >> ((offset & (s32)0x02) * 8));

    return 0;
}

s32 pci_hose_write_config_byte_via_dword(struct pci_controller *hose,pci_dev_t dev,s32 offset, u8 val)
{
    u32 val32, mask, ldata, shift;

    if (pci_hose_read_config_dword(hose, dev, offset & 0xfc, &val32) < 0)
        return -1;

    shift = ((offset & (s32)0x03) * 8);
    ldata = (((unsigned long)val) & 0x000000ff) << shift;
    mask = 0x000000ff << shift;
    val32 = (val32 & ~mask) | ldata;

    if (pci_hose_write_config_dword(hose, dev, offset & 0xfc, val32) < 0)
        return -1;

    return 0;
}

s32 pci_hose_write_config_word_via_dword(struct pci_controller *hose,pci_dev_t dev,s32 offset, u16 val)
{
    u32 val32, mask, ldata, shift;

    if (pci_hose_read_config_dword(hose, dev, offset & 0xfc, &val32) < 0)
        return -1;

    shift = ((offset & (s32)0x02) * 8);
    ldata = (((unsigned long)val) & 0x0000ffff) << shift;
    mask = 0x0000ffff << shift;
    val32 = (val32 & ~mask) | ldata;

    if (pci_hose_write_config_dword(hose, dev, offset & 0xfc, val32) < 0)
        return -1;

    return 0;
}

static struct pci_controller* hose_head;
//----pcie主控制器注册-----------------------------------------------------------------
//功能：将配置的PCIE主控制器结构体挂载到链表中，便于查询
//参数：pci_controller *hose,pcie主桥控制器配置结构体
//返回：无
//-----------------------------------------------------------------------------
void pci_register_hose(struct pci_controller* hose)
{
    struct pci_controller **phose = &hose_head;

    while(*phose)
        phose = &(*phose)->next;

    hose->next = NULL;

    *phose = hose;
}
//----通过bus获取pcie主控制器结构体指针-----------------------------------------------------------------
//功能：通过bus获取pcie主控制器结构体指针
//参数：int bus，查询的bus号
//返回：pcie主控制器结构体指针，否则为NULL
//-----------------------------------------------------------------------------
struct pci_controller *pci_bus_to_hose(s32 bus)
{
    struct pci_controller *hose;

    for (hose = hose_head; hose; hose = hose->next)
    {
        if (bus >= hose->first_busno && bus <= hose->last_busno)
            return hose;
    }

    printf("pci_bus_to_hose() failed\r\n");
    return NULL;
}
//----通过配置地址获取pcie主控制器结构体指针-----------------------------------------------------------------
//功能：通过配置地址获取pcie主控制器结构体指针
//参数：void *cfg_addr，pcie主控制器配置地址
//返回：pcie主控制器结构体指针，否则为NULL
//-----------------------------------------------------------------------------
struct pci_controller *find_hose_by_cfg_addr(void *cfg_addr)
{
    struct pci_controller *hose;

    for (hose = hose_head; hose; hose = hose->next)
    {
        if (hose->cfg_addr == cfg_addr)
            return hose;
    }

    return NULL;
}
//----获取pcie总线上最后一个bus号----------------------------------------------------------------
//功能：获取pcie总线上最后一个bus号
//参数：无
//返回：最后一个bus号
//-----------------------------------------------------------------------------
s32 pci_last_busno(void)
{
    struct pci_controller *hose = hose_head;

    if (!hose)
        return -1;

    while (hose->next)
        hose = hose->next;

    return hose->last_busno;
}

pci_dev_t pci_find_devices(struct pci_device_id *ids, s32 index)
{
    struct pci_controller * hose;
    u16 vendor, device;
    u8 header_type;
    pci_dev_t bdf;
    s32 i, bus, found_multi = 0;

    for (hose = hose_head; hose; hose = hose->next)
    {
        for (bus = hose->first_busno; bus <= hose->last_busno; bus++)
            for (bdf = PCI_BDF(bus, 0, 0);
                    bdf < PCI_BDF(bus + 1, 0, 0);
                    bdf += PCI_BDF(0, 0, 1))
            {
                if (!PCI_FUNC(bdf))
                {
                    pci_read_config_byte(bdf,
                                         PCI_HEADER_TYPE,
                                         &header_type);

                    found_multi = header_type & 0x80;
                }
                else
                {
                    if (!found_multi)
                        continue;
                }

                pci_read_config_word(bdf,
                                     PCI_VENDOR_ID,
                                     &vendor);
                pci_read_config_word(bdf,
                                     PCI_DEVICE_ID,
                                     &device);

                for (i = 0; ids[i].vendor != 0; i++)
                {
                    if (vendor == ids[i].vendor &&
                            device == ids[i].device)
                    {
                        if (index <= 0)
                            return bdf;

                        index--;
                    }
                }
            }
    }

    return -1;
}
//----查找pcie总线上的设备----------------------------------------------------------------
//功能：通过vender ID和device ID找到PCIE总线上的设备，返回设备的BDF号
//参数：unsigned s32 vendor, 设备vendor ID，unsigned s32 device, 设备device ID，
//s32 index，总线上第几个该ID的设备，为1，表示第一个。
//返回：设备的BDF号
//-----------------------------------------------------------------------------
pci_dev_t pci_find_device(u32 vendor, u32 device, s32 index)
{
    static struct pci_device_id ids[2] = {{}, {0, 0}};

    ids[0].vendor = vendor;
    ids[0].device = device;

    return pci_find_devices(ids, index);
}

s32 __pci_hose_phys_to_bus(struct pci_controller *hose,
                           phys_addr_t phys_addr,
                           unsigned long flags,
                           unsigned long skip_mask,
                           pci_addr_t *ba)
{
    struct pci_region *res;
    pci_addr_t bus_addr;
    s32 i;

    for (i = 0; i < hose->region_count; i++)
    {
        res = &hose->regions[i];

        if (((res->flags ^ flags) & PCI_REGION_TYPE) != 0)
            continue;

        if (res->flags & skip_mask)
            continue;

        bus_addr = phys_addr - res->phys_start + res->bus_start;

        if (bus_addr >= res->bus_start &&
                bus_addr < res->bus_start + res->size)
        {
            *ba = bus_addr;
            return 0;
        }
    }

    return 1;
}
//----获取PCIE控制器的MEM/IO的bus地址----------------------------------------------------------------
//功能：通过pcie主控制器的物理地址，获取指定PCIe主控制器中MEM或IO的bus地址
//参数：pci_controller *hose,pcie主桥控制器配置结构体，phys_addr_t phys_addr, pcie控制器空间的物理地址，
//unsigned long flags，指明是获取MEM还是IO的bus地址。
//返回：返回要获取的bus地址
//-----------------------------------------------------------------------------
pci_addr_t pci_hose_phys_to_bus (struct pci_controller *hose,
                                 phys_addr_t phys_addr,
                                 unsigned long flags)
{
    pci_addr_t bus_addr = 0;
    s32 ret;

    if (!hose)
    {
        printf("pci_hose_phys_to_bus: invalid hose\r\n");
        return bus_addr;
    }

    /*
     * if PCI_REGION_MEM is set we do a two pass search with preference
     * on matches that don't have PCI_REGION_SYS_MEMORY set
     */
    if ((flags & PCI_REGION_MEM) == PCI_REGION_MEM)
    {
        ret = __pci_hose_phys_to_bus(hose, phys_addr,
                                     flags, PCI_REGION_SYS_MEMORY, &bus_addr);
        if (!ret)
            return bus_addr;
    }

    ret = __pci_hose_phys_to_bus(hose, phys_addr, flags, 0, &bus_addr);

    if (ret)
        printf("pci_hose_phys_to_bus: invalid physical address\r\n");

    return bus_addr;
}

s32 __pci_hose_bus_to_phys(struct pci_controller *hose,
                           pci_addr_t bus_addr,
                           unsigned long flags,
                           unsigned long skip_mask,
                           phys_addr_t *pa)
{
    struct pci_region *res;
    s32 i;

    for (i = 0; i < hose->region_count; i++)
    {
        res = &hose->regions[i];

        if (((res->flags ^ flags) & PCI_REGION_TYPE) != 0)
            continue;

        if (res->flags & skip_mask)
            continue;

        if (bus_addr >= res->bus_start &&
                bus_addr < res->bus_start + res->size)
        {
            *pa = (bus_addr - res->bus_start + res->phys_start);
            return 0;
        }
    }

    return 1;
}
//----获取PCIE控制器的PHY地址----------------------------------------------------------------
//功能：通过pcie主控制器MEM或IO的bus地址，获取指定PCIe主控制器的phy地址
//参数：pci_controller *hose,pcie主桥控制器配置结构体，pci_addr_t bus_addr, pcie控制器MEM或IO空间的bus地址，
//unsigned long flags，指明是MEM还是IO的bus地址。
//返回：返回要获取的phy地址
//-----------------------------------------------------------------------------
phys_addr_t pci_hose_bus_to_phys(struct pci_controller* hose,
                                 pci_addr_t bus_addr,
                                 unsigned long flags)
{
    phys_addr_t phys_addr = 0;
    s32 ret;

    if (!hose)
    {
        printf("pci_hose_bus_to_phys: invalid hose\r\n");
        return phys_addr;
    }

    /*
     * if PCI_REGION_MEM is set we do a two pass search with preference
     * on matches that don't have PCI_REGION_SYS_MEMORY set
     */
    if ((flags & PCI_REGION_MEM) == PCI_REGION_MEM)
    {
        ret = __pci_hose_bus_to_phys(hose, bus_addr,
                                     flags, PCI_REGION_SYS_MEMORY, &phys_addr);
        if (!ret)
            return phys_addr;
    }

    ret = __pci_hose_bus_to_phys(hose, bus_addr, flags, 0, &phys_addr);

    if (ret)
        printf("pci_hose_bus_to_phys: invalid physical address\r\n");

    return phys_addr;
}

//----手动配置挂载的PCIE设备----------------------------------------------------------------
//功能：直接给io、mem、指令等参数，手动配置PCIE设备。因为这里采用上电自动配置，所以该函数没用
//参数：pci_controller *hose,pcie主桥控制器配置结构体，pci_dev_t dev,挂载设备的BDF号
//unsigned long io,配置的IO地址，pci_addr_t mem,配置的MEM地址，unsigned long command，配置的指令
//返回：返回0表示配置完成
//-----------------------------------------------------------------------------
s32 pci_hose_config_device(struct pci_controller *hose,
                           pci_dev_t dev,
                           unsigned long io,
                           pci_addr_t mem,
                           unsigned long command)
{
    u32 bar_response;
    u32 old_command;
    pci_addr_t bar_value;
    pci_size_t bar_size;
    u8 pin;
    s32 bar, found_mem64;

    pcie_debug("PCI Config: I/O=0x%lx, Memory=0x%llx, Command=0x%lx\r\n", io,
               (u64)mem, command);

    pci_hose_write_config_dword(hose, dev, PCI_COMMAND, 0);

    for (bar = PCI_BASE_ADDRESS_0; bar <= PCI_BASE_ADDRESS_5; bar += 4)
    {
        pci_hose_write_config_dword(hose, dev, bar, 0xffffffff);
        pci_hose_read_config_dword(hose, dev, bar, &bar_response);

        if (!bar_response)
            continue;

        found_mem64 = 0;

        /* Check the BAR type and set our address mask */
        if (bar_response & PCI_BASE_ADDRESS_SPACE)
        {
            bar_size = ~(bar_response & PCI_BASE_ADDRESS_IO_MASK) + 1;
            /* round up region base address to a multiple of size */
            io = ((io - 1) | (bar_size - 1)) + 1;
            bar_value = io;
            /* compute new region base address */
            io = io + bar_size;
        }
        else
        {
            if ((bar_response & PCI_BASE_ADDRESS_MEM_TYPE_MASK) ==
                    PCI_BASE_ADDRESS_MEM_TYPE_64)
            {
                u32 bar_response_upper;
                u64 bar64;
                pci_hose_write_config_dword(hose, dev, bar + 4,
                                            0xffffffff);
                pci_hose_read_config_dword(hose, dev, bar + 4,
                                           &bar_response_upper);

                bar64 = ((u64)bar_response_upper << 32) | bar_response;

                bar_size = ~(bar64 & PCI_BASE_ADDRESS_MEM_MASK) + 1;
                found_mem64 = 1;
            }
            else
            {
                bar_size = (u32)(~(bar_response & PCI_BASE_ADDRESS_MEM_MASK) + 1);
            }

            /* round up region base address to multiple of size */
            mem = ((mem - 1) | (bar_size - 1)) + 1;
            bar_value = mem;
            /* compute new region base address */
            mem = mem + bar_size;
        }

        /* Write it out and update our limit */
        pci_hose_write_config_dword (hose, dev, bar, (u32)bar_value);

        if (found_mem64)
        {
            bar += 4;
#if (CONFIG_SYS_PCI_64BIT == 1)
            pci_hose_write_config_dword(hose, dev, bar,
                                        (u32)(bar_value >> 32));
#else
            pci_hose_write_config_dword(hose, dev, bar, 0x00000000);
#endif
        }
    }

    /* Configure Cache Line Size Register */
    pci_hose_write_config_byte(hose, dev, PCI_CACHE_LINE_SIZE, 0x08);

    /* Configure Latency Timer */
    pci_hose_write_config_byte(hose, dev, PCI_LATENCY_TIMER, 0x80);

    /* Disable interrupt line, if device says it wants to use interrupts */
    pci_hose_read_config_byte(hose, dev, PCI_INTERRUPT_PIN, &pin);
    if (pin != 0)
    {
        pci_hose_write_config_byte(hose, dev, PCI_INTERRUPT_LINE, 0xff);
    }

    pci_hose_read_config_dword(hose, dev, PCI_COMMAND, &old_command);
    pci_hose_write_config_dword(hose, dev, PCI_COMMAND,
                                (old_command & 0xffff0000) | command);

    return 0;
}

/*获取pci设备配置表，只有用手动配置挂载PCIE设备时采用，这里采用PNP模式，且用自动扫描配置模式
 * 故没有设置pcie配置表，该函数没用上。
 */
struct pci_config_table *pci_find_config(struct pci_controller *hose,
        u16 class,
        u32 vendor,
        u32 device,
        u32 bus,
        u32 dev,
        u32 func)
{
    struct pci_config_table *table;

    for (table = hose->config_table; table && table->vendor; table++)
    {
        if ((table->vendor == PCI_ANY_ID || table->vendor == vendor) &&
                (table->device == PCI_ANY_ID || table->device == device) &&
                (table->class  == PCI_ANY_ID || table->class  == class)  &&
                (table->bus    == PCI_ANY_ID || table->bus    == bus)    &&
                (table->dev    == PCI_ANY_ID || table->dev    == dev)    &&
                (table->func   == PCI_ANY_ID || table->func   == func))
        {
            return table;
        }
    }

    return NULL;
}
//----手动配置挂载的PCIE设备----------------------------------------------------------------
//功能：通过pci设备配置表，手动配置PCIE设备。因为这里采用上电自动配置，所以该函数没用上
//参数：pci_controller *hose,pcie主桥控制器配置结构体，pci_dev_t dev,挂载设备的BDF号
//struct pci_config_table *entry，pci设备配置表
//返回：无
//-----------------------------------------------------------------------------
void pci_cfgfunc_config_device(struct pci_controller *hose,
                               pci_dev_t dev,
                               struct pci_config_table *entry)
{
    pci_hose_config_device(hose, dev, entry->priv[0], entry->priv[1],
                           entry->priv[2]);
}

/*
 * HJF: Changed this to return s32. I think this is required
 * to get the correct result when scanning bridges
 */
extern s32 pciauto_config_device(struct pci_controller *hose, pci_dev_t dev);

#if (CONFIG_PCI_SCAN_SHOW==1)
const char * pci_class_str(u8 class)
{
    switch (class)
    {
    case PCI_CLASS_NOT_DEFINED:
        return "Build before PCI Rev2.0";
        break;
    case PCI_BASE_CLASS_STORAGE:
        return "Mass storage controller";
        break;
    case PCI_BASE_CLASS_NETWORK:
        return "Network controller";
        break;
    case PCI_BASE_CLASS_DISPLAY:
        return "Display controller";
        break;
    case PCI_BASE_CLASS_MULTIMEDIA:
        return "Multimedia device";
        break;
    case PCI_BASE_CLASS_MEMORY:
        return "Memory controller";
        break;
    case PCI_BASE_CLASS_BRIDGE:
        return "Bridge device";
        break;
    case PCI_BASE_CLASS_COMMUNICATION:
        return "Simple comm. controller";
        break;
    case PCI_BASE_CLASS_SYSTEM:
        return "Base system peripheral";
        break;
    case PCI_BASE_CLASS_INPUT:
        return "Input device";
        break;
    case PCI_BASE_CLASS_DOCKING:
        return "Docking station";
        break;
    case PCI_BASE_CLASS_PROCESSOR:
        return "Processor";
        break;
    case PCI_BASE_CLASS_SERIAL:
        return "Serial bus controller";
        break;
    case PCI_BASE_CLASS_INTELLIGENT:
        return "Intelligent controller";
        break;
    case PCI_BASE_CLASS_SATELLITE:
        return "Satellite controller";
        break;
    case PCI_BASE_CLASS_CRYPT:
        return "Cryptographic device";
        break;
    case PCI_BASE_CLASS_SIGNAL_PROCESSING:
        return "DSP";
        break;
    case PCI_CLASS_OTHERS:
        return "Does not fit any class";
        break;
    default:
        return  "???";
        break;
    };
}
#endif /* CONFIG_PCI_SCAN_SHOW */

s32 __pci_skip_dev(struct pci_controller *hose, pci_dev_t dev)
{
    /*
     * Check if pci device should be skipped in configuration
     */
    if (dev == PCI_BDF(hose->first_busno, 0, 0))
    {
        return 1;
    }

    return 0;
}

#if (CONFIG_PCI_SCAN_SHOW==1)
s32 __pci_print_dev(struct pci_controller *hose, pci_dev_t dev)
{
    if (dev == PCI_BDF(hose->first_busno, 0, 0))
        return 0;

    return 1;
}
#endif /* CONFIG_PCI_SCAN_SHOW */

//----设置PCIe设备相关配置寄存器----------------------------------------------------------------
//功能：用于给PCIe设备分配地址空间和设置相关配置寄存器
//参数：pci_controller *hose,pcie主桥控制器配置结构体，pci_dev_t dev,挂载设备的BDF号
//s32 bars_num,PCI基地址的总数，一般是6个，主桥的话为0，
//pci_region *mem，*prefetch,*io，分别为mem、prefetch、io空间的地址配置结构体
//返回：无
//-----------------------------------------------------------------------------
void pciauto_setup_device(struct pci_controller *hose,
                          pci_dev_t dev, s32 bars_num,
                          struct pci_region *mem,
                          struct pci_region *prefetch,
                          struct pci_region *io)
{
    u32 bar_response;
    pci_size_t bar_size;
    u16 cmdstat = 0;
    s32 bar, bar_nr = 0;
    pci_addr_t bar_value;
    struct pci_region *bar_res;
    s32 found_mem64 = 0;


    pci_hose_read_config_word(hose, dev, PCI_COMMAND, &cmdstat);
    cmdstat = (cmdstat & ~(PCI_COMMAND_IO | PCI_COMMAND_MEMORY)) | PCI_COMMAND_MASTER;
    for (bar = PCI_BASE_ADDRESS_0;
            bar < PCI_BASE_ADDRESS_0 + (bars_num * 4); bar += 4)
    {
        /* Tickle the BAR and get the response */
        pci_hose_write_config_dword(hose, dev, bar, 0xffffffff);
        pci_hose_read_config_dword(hose, dev, bar, &bar_response);

        /* If BAR is not implemented go to the next BAR */
        if (!bar_response)
            continue;

        found_mem64 = 0;
        pcie_debug("PCI Autoconfig First: BAR %d, I/O, bar_response=0x%llx, ", bar_nr, (u64)bar_response);
        /* Check the BAR type and set our address mask */
        if (bar_response & PCI_BASE_ADDRESS_SPACE)
        {
            bar_size = ((~(bar_response & PCI_BASE_ADDRESS_IO_MASK))
                        & 0xffff) + 1;
            bar_res = io;

            pcie_debug("PCI Autoconfig: BAR %d, I/O, size=0x%llx, ", bar_nr, (u64)bar_size);
        }
        else
        {
            if ((bar_response & PCI_BASE_ADDRESS_MEM_TYPE_MASK) ==
                    PCI_BASE_ADDRESS_MEM_TYPE_64)
            {
                u32 bar_response_upper;
                u64 bar64;

                pci_hose_write_config_dword(hose, dev, bar + 4,
                                            0xffffffff);
                pci_hose_read_config_dword(hose, dev, bar + 4,
                                           &bar_response_upper);

                bar64 = ((u64)bar_response_upper << 32) | bar_response;

                bar_size = ~(bar64 & PCI_BASE_ADDRESS_MEM_MASK) + 1;
                found_mem64 = 1;
            }
            else
            {
                bar_size = (u32)(~(bar_response & PCI_BASE_ADDRESS_MEM_MASK) + 1);
            }
            if (prefetch && (bar_response & PCI_BASE_ADDRESS_MEM_PREFETCH))
                bar_res = prefetch;
            else
                bar_res = mem;

            pcie_debug("PCI Autoconfig: BAR %d, Mem, size=0x%llx, ", bar_nr, (u64)bar_size);

        }
        pcie_debug("bar_res bus_start : %llx, size=0x%llx, ", (u64)bar_res->bus_start, (u64)bar_res->size);
        if (pciauto_region_allocate(bar_res, bar_size, &bar_value) == 0)
        {
            /* Write it out and update our limit */
            pci_hose_write_config_dword(hose, dev, bar, (u32)bar_value);

            if (found_mem64)
            {
                bar += 4;
#if (CONFIG_SYS_PCI_64BIT == 1)
                pci_hose_write_config_dword(hose, dev, bar, (u32)(bar_value>>32));
#else
                /*
                 * If we are a 64-bit decoder then increment to the
                 * upper 32 bits of the bar and force it to locate
                 * in the lower 4GB of memory.
                 */
                pci_hose_write_config_dword(hose, dev, bar, 0x00000000);
#endif
            }

        }
        cmdstat |= (bar_response & PCI_BASE_ADDRESS_SPACE) ?
                   PCI_COMMAND_IO : PCI_COMMAND_MEMORY;

        pcie_debug("\r\n");

        bar_nr++;
    }

    pci_hose_write_config_word(hose, dev, PCI_COMMAND, cmdstat);
    pci_hose_write_config_byte(hose, dev, PCI_CACHE_LINE_SIZE,
                               CONFIG_SYS_PCI_CACHE_LINE_SIZE);
    pci_hose_write_config_byte(hose, dev, PCI_LATENCY_TIMER, 0x80);
}

//----设置PCIe设备或桥的相关配置寄存器----------------------------------------------------------------
//功能：用于给PCIe设备或桥分配地址空间和设置相关配置寄存器
//参数：pci_controller *hose,pcie主桥控制器配置结构体，pci_dev_t dev,挂载设备的BDF号
//返回：返回设备的bus号，如果是遇到桥的话，则返回桥下最深设备的bus号
//-----------------------------------------------------------------------------
s32 pciauto_config_device(struct pci_controller *hose, pci_dev_t dev)
{
    u32 sub_bus = PCI_BUS(dev),prebus, found_multi = 0;
    u16 vendor, device, class;
    u8 prg_iface;
    u8 BridgeNum=0;
    u8 header_type;
    #if (CONFIG_PCI_PNP==0)
        struct pci_config_table *cfg;
    #endif
    #if (CONFIG_PCI_SCAN_SHOW==1)
        static s32 indent = 0;
    #endif

    pci_hose_read_config_word(hose, dev, PCI_CLASS_DEVICE, &class);
    switch (class)
    {
    case PCI_CLASS_BRIDGE_PCI:
        prebus = hose->current_busno;
        //pcie_debug("bridge prebus:%d\r\n",prebus);
        while(1)
        {
            hose->current_busno++;
            BridgeNum = 0;
            pciauto_setup_device(hose, dev, 2, hose->pci_mem,
                             hose->pci_prefetch, hose->pci_io);

            pcie_debug("PCI Autoconfig: Found P2P bridge, device %d\r\n", PCI_DEV(dev));

            /* Passing in current_busno allows for sibling P2P bridges */
            pciauto_prescan_setup_bridge(hose, dev, hose->current_busno);
            /*
             * need to figure out if this is a subordinate bridge on the bus
             * to be able to properly set the pri/sec/sub bridge registers.
             */
            class = PCI_CLASS_NOT_DEFINED;
            for (dev =  PCI_BDF(hose->current_busno,0,0);
                        dev <  PCI_BDF(hose->current_busno, PCI_MAX_PCI_DEVICES - 1,
                                       PCI_MAX_PCI_FUNCTIONS - 1);
                        dev += PCI_BDF(0, 0, 1))
            {

                    if (__pci_skip_dev(hose, dev))
                        continue;

                    if (PCI_FUNC(dev) && !found_multi)
                        continue;

                    pci_hose_read_config_byte(hose, dev, PCI_HEADER_TYPE, &header_type);

                    pci_hose_read_config_word(hose, dev, PCI_VENDOR_ID, &vendor);

                    if (vendor == 0xffff || vendor == 0x0000)
                        continue;

                    if (!PCI_FUNC(dev))
                        found_multi = header_type & 0x80;

                    pcie_debug("PCI Scan: Found Bus %d, Device %d, Function %d\r\n",
                               PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev));

                    pci_hose_read_config_word(hose, dev, PCI_DEVICE_ID, &device);
                    pci_hose_read_config_word(hose, dev, PCI_CLASS_DEVICE, &class);

                    switch(class)
                    {
                        case PCI_CLASS_NOT_DEFINED:
                             break;
                        case PCI_CLASS_BRIDGE_PCI:
                             BridgeNum++;;
                             break;
                        case PCI_CLASS_STORAGE_IDE:
                             pci_hose_read_config_byte(hose, dev, PCI_CLASS_PROG, &prg_iface);
                             if (!(prg_iface & PCIAUTO_IDE_MODE_MASK))
                             {
                                  pcie_debug("PCI Autoconfig: Skipping legacy mode IDE controller\r\n");
                                  break;
                             }

                             pciauto_setup_device(hose, dev, 6, hose->pci_mem,
                                                         hose->pci_prefetch, hose->pci_io);
                             break;

                        case PCI_CLASS_BRIDGE_CARDBUS:
                             /*
                             * just do a minimal setup of the bridge,
                             * let the OS take care of the rest
                             */
                             pciauto_setup_device(hose, dev, 0, hose->pci_mem,
                                                         hose->pci_prefetch, hose->pci_io);

                             pcie_debug("PCI Autoconfig: Found P2CardBus bridge, device %d\r\n",
                                               PCI_DEV(dev));
                                break;
                        case PCI_CLASS_PROCESSOR_POWERPC: /* an agent or end-point */
                             pcie_debug("PCI AutoConfig: Found PowerPC device\r\n");

                        default:
                             pciauto_setup_device(hose, dev, 6, hose->pci_mem,
                                                         hose->pci_prefetch, hose->pci_io);
                             break;
                    }
                    #if (CONFIG_PCI_SCAN_SHOW==1)
                       indent++;

                    /* Print leading space, including bus indentation */
                    printf("%*c", indent + 1, ' ');

                    if (__pci_print_dev(hose, dev))
                    {

                        printf("%02x:%02x.%-*x - %04x:%04x - %s\r\n",
                                   PCI_BUS(dev), PCI_DEV(dev), 6 - indent, PCI_FUNC(dev),
                                   vendor, device, pci_class_str(class >> 8));
                    }

                    #endif
                    #if (CONFIG_PCI_SCAN_SHOW==1)
                    indent--;
                    #endif

            }
            if(!BridgeNum)
            {
                if(class == PCI_CLASS_NOT_DEFINED)
                    hose->current_busno--;
                sub_bus = hose->current_busno;
                if(class != PCI_CLASS_NOT_DEFINED)
                    hose->current_busno--;
                //pcie_debug("sub_bus:%d\r\n",sub_bus);
                break;
            }
            //最多扫描128次，表示pcie设备扫描的最深深度还是主桥的情况下，不再进行扫描
            //返回
            if(hose->current_busno >= 128)
                break;
        }

        for(;hose->current_busno >= prebus;hose->current_busno--)
            pciauto_postscan_setup_bridge(hose, dev, hose->current_busno);
        //pcie_debug("hose->current_busno :%d\r\n",hose->current_busno);
        hose->current_busno = sub_bus;
        break;

    case PCI_CLASS_STORAGE_IDE:
        pci_hose_read_config_byte(hose, dev, PCI_CLASS_PROG, &prg_iface);
        if (!(prg_iface & PCIAUTO_IDE_MODE_MASK))
        {
            pcie_debug("PCI Autoconfig: Skipping legacy mode IDE controller\r\n");
            return sub_bus;
        }

        pciauto_setup_device(hose, dev, 6, hose->pci_mem,
                             hose->pci_prefetch, hose->pci_io);
        break;

    case PCI_CLASS_BRIDGE_CARDBUS:
        /*
         * just do a minimal setup of the bridge,
         * let the OS take care of the rest
         */
        pciauto_setup_device(hose, dev, 0, hose->pci_mem,
                             hose->pci_prefetch, hose->pci_io);

        pcie_debug("PCI Autoconfig: Found P2CardBus bridge, device %d\r\n",
                   PCI_DEV(dev));

        hose->current_busno++;
        break;
    case PCI_CLASS_PROCESSOR_POWERPC: /* an agent or end-point */
        pcie_debug("PCI AutoConfig: Found PowerPC device\r\n");

    default:
        pciauto_setup_device(hose, dev, 6, hose->pci_mem,
                             hose->pci_prefetch, hose->pci_io);
        break;
    }

    return sub_bus;
}
//#undef CONFIG_PCI_SCAN_SHOW
//----pcie主控制器扫描bus----------------------------------------------------------------
//功能：pcie主控制器扫描总线，并自动配置挂载的设备
//参数：pci_controller *hose,pcie主桥控制器配置结构体，int bus，分配的bus号
//返回：返回最深设备的bus号
//-----------------------------------------------------------------------------
s32 pci_hose_scan_bus(struct pci_controller *hose, s32 bus)
{
    u32 sub_bus,tempbus, found_multi = 0;
    u16 vendor, device, class;
    u8 header_type;
#if (CONFIG_PCI_PNP==0)
    struct pci_config_table *cfg;
#endif
    pci_dev_t dev;
#if (CONFIG_PCI_SCAN_SHOW==1)
    static s32 indent = 0;
#endif

    sub_bus = bus;

    for (dev =  PCI_BDF(bus,0,0);
            dev <  PCI_BDF(bus, PCI_MAX_PCI_DEVICES - 1,
                           PCI_MAX_PCI_FUNCTIONS - 1);
            dev += PCI_BDF(0, 0, 1))
    {

        if (__pci_skip_dev(hose, dev))
            continue;

        if (PCI_FUNC(dev) && !found_multi)
            continue;

        pci_hose_read_config_byte(hose, dev, PCI_HEADER_TYPE, &header_type);

        pci_hose_read_config_word(hose, dev, PCI_VENDOR_ID, &vendor);

        if (vendor == 0xffff || vendor == 0x0000)
            continue;

        if (!PCI_FUNC(dev))
            found_multi = header_type & 0x80;

        pcie_debug("PCI Scan: Found Bus %d, Device %d, Function %d\r\n",
                   PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev));

        pci_hose_read_config_word(hose, dev, PCI_DEVICE_ID, &device);
        pci_hose_read_config_word(hose, dev, PCI_CLASS_DEVICE, &class);

#if (CONFIG_PCI_SCAN_SHOW==1)
        indent++;

        /* Print leading space, including bus indentation */
        printf("%*c", indent + 1, ' ');

        if (__pci_print_dev(hose, dev))
        {
            printf("%02x:%02x.%-*x - %04x:%04x - %s\r\n",
                       PCI_BUS(dev), PCI_DEV(dev), 6 - indent, PCI_FUNC(dev),
                       vendor, device, pci_class_str(class >> 8));
        }

#endif

#if (CONFIG_PCI_PNP==1)
        tempbus = pciauto_config_device(hose, dev);
        sub_bus = max(tempbus, sub_bus);
#else
        cfg = pci_find_config(hose, class, vendor, device,
                              PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev));
        if (cfg)
        {
            cfg->config_device(hose, dev, cfg);
            sub_bus = max(sub_bus, hose->current_busno);
        }
#endif

#if (CONFIG_PCI_SCAN_SHOW==1)
        indent--;
#endif

        if (hose->fixup_irq)
            hose->fixup_irq(hose, dev);
    }

    return sub_bus;
}
//----pcie主控制器扫描bus----------------------------------------------------------------
//功能：pcie主控制器扫描总线，并自动配置挂载的设备，这里是提供的手动扫描接口，非上电扫描。
//参数：pci_controller *hose,pcie主桥控制器配置结构体
//返回：返回最深设备的bus号
//-----------------------------------------------------------------------------
s32 pci_hose_scan(struct pci_controller *hose)
{
    /*
     * Start scan at current_busno.
     * PCIe will start scan at first_busno+1.
     */
    /* For legacy support, ensure current >= first */
    if (hose->first_busno > hose->current_busno)
        hose->current_busno = hose->first_busno;
#if (CONFIG_PCI_PNP==1)
    pciauto_config_init(hose);
#endif
    return pci_hose_scan_bus(hose, hose->current_busno);
}

void pcie_sh_install(void);
//----pcie模块初始化总函数-----------------------------------------------------------------
//功能：pcie模块初始化，pcie指令装入shell下载文件命令
//参数：无
//返回：true：pcie指令装入shell下载文件命令成功
//      false：pcie指令装入shell下载文件命令失败
//-----------------------------------------------------------------------------
ptu32_t module_init_pcie(void)
{
    pcie_sh_install( );
    return true;
}

