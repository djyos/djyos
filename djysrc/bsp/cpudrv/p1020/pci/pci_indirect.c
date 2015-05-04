//----------------------------------------------------
//Copyright (C), 2013-2020,  luoshitian.
//版权所有 (C), 2013-2020,   罗侍田.
//所属模块: pci寄存器读写函数
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: pci寄存器读写函数
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-11-14
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include <asm/io.h>
#include "pci.h"

int indirect_read_config_byte(struct pci_controller *hose,
                              pci_dev_t dev, int offset, u8 * val)
{
    u32 b, d,f;
    b = PCI_BUS(dev); d = PCI_DEV(dev); f = PCI_FUNC(dev);
    b = b - hose->first_busno;
    dev = PCI_BDF(b, d, f);
    *(hose->cfg_addr) = dev | (offset & 0xfc) | ((offset & 0xf00) << 16) | 0x80000000;
    sync();
    *val = in_8((u8*)&hose->cfg_data[offset&3]);
    return 0;
}

int indirect_read_config_word(struct pci_controller *hose,
          pci_dev_t dev, int offset, u16 * val)
{
    u32 b, d,f;
    b = PCI_BUS(dev); d = PCI_DEV(dev); f = PCI_FUNC(dev);
    b = b - hose->first_busno;
    dev = PCI_BDF(b, d, f);
    *(hose->cfg_addr) = dev | (offset & 0xfc) | ((offset & 0xf00) << 16) | 0x80000000;
    sync();
    *val = in_le16((u16*)&hose->cfg_data[offset&2]);
    return 0;
}

int indirect_read_config_dword(struct pci_controller *hose,
          pci_dev_t dev, int offset, u32 * val)
{
    u32 b, d,f;
    b = PCI_BUS(dev); d = PCI_DEV(dev); f = PCI_FUNC(dev);
    b = b - hose->first_busno;
    dev = PCI_BDF(b, d, f);
    *(hose->cfg_addr) = dev | (offset & 0xfc) | ((offset & 0xf00) << 16) | 0x80000000;
    sync();
    *val = in_le32((u32*)&hose->cfg_data[offset&0]);
    return 0;
}

int indirect_write_config_byte(struct pci_controller *hose,
        pci_dev_t dev, int offset, u8 val)
{
    u32 b, d,f;
    b = PCI_BUS(dev); d = PCI_DEV(dev); f = PCI_FUNC(dev);
    b = b - hose->first_busno;
    dev = PCI_BDF(b, d, f);
    *(hose->cfg_addr) = dev | (offset & 0xfc) | ((offset & 0xf00) << 16) | 0x80000000;
    sync();
    out_8((u8*)&hose->cfg_data[offset&3],val);
    return 0;
}

int indirect_write_config_word(struct pci_controller *hose,
        pci_dev_t dev, int offset, u16 val)
{
    u32 b, d,f;
    b = PCI_BUS(dev); d = PCI_DEV(dev); f = PCI_FUNC(dev);
    b = b - hose->first_busno;
    dev = PCI_BDF(b, d, f);
    *(hose->cfg_addr) = dev | (offset & 0xfc) | ((offset & 0xf00) << 16) | 0x80000000;
    sync();
    out_le16((u16*)&hose->cfg_data[offset&2],val);
    return 0;
}

int indirect_write_config_dword(struct pci_controller *hose,
        pci_dev_t dev, int offset, u32 val)
{
    u32 b, d,f;
    b = PCI_BUS(dev); d = PCI_DEV(dev); f = PCI_FUNC(dev);
    b = b - hose->first_busno;
    dev = PCI_BDF(b, d, f);
    *(hose->cfg_addr) = dev | (offset & 0xfc) | ((offset & 0xf00) << 16) | 0x80000000;
    sync();
    out_le32((u32*)&hose->cfg_data[offset&0],val);
    return 0;
}

void pci_setup_indirect(struct pci_controller* hose, u32 cfg_addr, u32 cfg_data)
{
    hose->cfg_addr = (unsigned int *) cfg_addr;
    hose->cfg_data = (unsigned char *) cfg_data;
}

