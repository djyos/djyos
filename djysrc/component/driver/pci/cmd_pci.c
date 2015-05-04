//----------------------------------------------------
/*
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>
 *
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 * Wolfgang Grandegger, DENX Software Engineering, wg@denx.de.
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
//------------------------------------------------------
#include "stdint.h"
#include "ctype.h"
#include "stdio.h"
#include "string.h"
#include "pci.h"
#include "shell.h"

bool_t sh_cmd_pcie(char *param);
struct tagShellCmdTab const pcie_cmd_table[] =
{
    {
        "pcie",
        sh_cmd_pcie,
        "查看指定bus号的pcie设备或主桥的信息,当不输入pcie号时，打印所有pcie设备或主桥的信息",
        "命令格式: pcie bus号（bus号省略则信息全打印）"
    }
};
static struct tagShellCmdRsc tg_pcie_cmd_rsc
        [sizeof(pcie_cmd_table)/sizeof(struct tagShellCmdTab)];

//----添加pcie shell 命令------------------------------------------------------
//功能：添加pcie shell 命令
//参数：无
//返回：无
//-----------------------------------------------------------------------------
void pcie_sh_install(void)
{
    Sh_InstallCmd(pcie_cmd_table,tg_pcie_cmd_rsc,
                      sizeof(pcie_cmd_table)/sizeof(struct tagShellCmdTab));
}

/*
 * Follows routines for the output of infos about devices on PCI bus.
 */
/*
static unsigned int simple_guess_base(const char *cp)
{
    if (cp[0] == '0')
    {
        if (tolower((int)cp[1]) == 'x' && isxdigit((int)cp[2]))
            return 16;
        else
            return 8;
    }
    else
    {
        return 10;
    }
}
int djy_strtoul(const char *cp, char **endp, unsigned int base)
{
    int result = 0;
    if (!base)
        base = simple_guess_base(cp);
    if (base == 16 && cp[0] == '0' && tolower((int)cp[1]) == 'x')
        cp += 2;
    while (isxdigit((int)*cp))
    {
        unsigned int value;
        value = isdigit((int)*cp) ? *cp - '0' : tolower((int)*cp) - 'a' + 10;
        if (value >= base)
            break;
        result = result * base + value;
        cp++;
    }
    if (endp)
        *endp = (char *)cp;
    return result;
}
*/

void pci_header_show(pci_dev_t dev);
void pci_header_show_brief(pci_dev_t dev);
/*
 * Subroutine:  pciinfo
 *
 * Description: Show information about devices on PCI bus.
 *                Depending on the define CONFIG_SYS_SHORT_PCI_LISTING
 *                the output will be more or less exhaustive.
 *
 * Inputs:    bus_no        the number of the bus to be scanned.
 *
 * Return:      None
 *
 */
void pciinfo(u32 BusNum)
{
    int Device;
    int Function;
    unsigned char HeaderType;
    unsigned short VendorID;
    pci_dev_t dev;
    int ShortPCIListing;

    printf("Scanning PCI devices on bus %d\r\n", BusNum);
    ShortPCIListing = 0;
    if (ShortPCIListing)
    {
        printf("BusDevFun  VendorId   DeviceId   Device Class       Sub-Class\r\n");
        printf("_____________________________________________________________\r\n");
    }

    for (Device = 0; Device < PCI_MAX_PCI_DEVICES; Device++)
    {
        HeaderType = 0;
        VendorID = 0;
        for (Function = 0; Function < PCI_MAX_PCI_FUNCTIONS; Function++)
        {
            /*
             * If this is not a multi-function device, we skip the rest.
             */
            if (Function && !(HeaderType & 0x80))
                break;

            dev = PCI_BDF(BusNum, Device, Function);

            pci_read_config_word(dev, PCI_VENDOR_ID, &VendorID);
            if ((VendorID == 0xFFFF) || (VendorID == 0x0000))
                continue;

            if (!Function) pci_read_config_byte(dev, PCI_HEADER_TYPE, &HeaderType);

            if (ShortPCIListing)
            {
                printf("%02x %02x %02x   ", BusNum, Device, Function);
                pci_header_show_brief(dev);
            }
            else
            {
                printf("\nFound PCI device %02x %02x %02x:\r\n",
                           BusNum, Device, Function);
                pci_header_show(dev);
            }
        }
    }
}


/*
 * Subroutine:  pci_header_show_brief
 *
 * Description: Reads and prints the header of the
 *        specified PCI device in short form.
 *
 * Inputs:    dev      Bus+Device+Function number
 *
 * Return:      None
 *
 */
void pci_header_show_brief(pci_dev_t dev)
{
    u16 vendor, device;
    u8 class, subclass;

    pci_read_config_word(dev, PCI_VENDOR_ID, &vendor);
    pci_read_config_word(dev, PCI_DEVICE_ID, &device);
    pci_read_config_byte(dev, PCI_CLASS_CODE, &class);
    pci_read_config_byte(dev, PCI_CLASS_SUB_CODE, &subclass);

    printf("0x%4x     0x%4x     %-23s 0x%2x\r\n",
               vendor, device,
               pci_class_str(class), subclass);
}
//----pcie打印函数-----------------------------------------------------------------
//功能：读指定设备的pcie配置寄存器，然后串口打印信息。
//参数：const char *msg,要打印的字符串信息；pci_dev_t dev,挂载设备的BDF号
//            int type,读出数据类型，2表示byte型，4表示word型，8表示dword型；int reg，寄存器偏移量
//返回：无
//-----------------------------------------------------------------------------
void cmd_pcie_print(const char *msg,pci_dev_t dev,int type,int reg)
{
    u8 pcie_byte = 0;
    u16 pcie_word = 0;
    u32 pcie_dword = 0;
    if(type == 2)
    {
        pci_read_config_byte(dev, reg, &pcie_byte);
        printf(msg, pcie_byte);
    }
    else if(type == 4)
    {
        pci_read_config_word(dev, reg, &pcie_word);
        printf(msg, pcie_word);
    }
    else if(type == 8)
    {
        pci_read_config_dword(dev, reg, &pcie_dword);
        printf(msg, pcie_dword);
    }
}
/*
 * Subroutine:  PCI_Header_Show
 *
 * Description: Reads the header of the specified PCI device.
 *
 * Inputs:        BusDevFunc      Bus+Device+Function number
 *
 * Return:      None
 *
 */
void pci_header_show(pci_dev_t dev)
{
    u8 _byte, header_type;

    pci_read_config_byte(dev, PCI_HEADER_TYPE, &header_type);

    cmd_pcie_print ("  vendor ID =                   0x%4x\r\n",dev, 4, PCI_VENDOR_ID);
    cmd_pcie_print ("  device ID =                   0x%4x\r\n",dev, 4, PCI_DEVICE_ID);
    cmd_pcie_print ("  command register =            0x%4x\r\n",dev, 4, PCI_COMMAND);
    cmd_pcie_print ("  status register =             0x%4x\r\n",dev, 4, PCI_STATUS);
    cmd_pcie_print ("  revision ID =                 0x%2x\r\n",dev, 2, PCI_REVISION_ID);
    pci_read_config_byte(dev, PCI_CLASS_CODE, &_byte);
    printf("  class code =                  0x%2x (%s)\r\n", _byte,pci_class_str(_byte));
    cmd_pcie_print ("  sub class code =              0x%2x\r\n",dev, 2, PCI_CLASS_SUB_CODE);
    cmd_pcie_print ("  programming interface =       0x%2x\r\n",dev, 2, PCI_CLASS_PROG);
    cmd_pcie_print ("  cache line =                  0x%2x\r\n",dev, 2, PCI_CACHE_LINE_SIZE);
    cmd_pcie_print ("  latency time =                0x%2x\r\n",dev, 2, PCI_LATENCY_TIMER);
    cmd_pcie_print ("  header type =                 0x%2x\r\n",dev, 2, PCI_HEADER_TYPE);
    cmd_pcie_print ("  BIST =                        0x%2x\r\n",dev, 2, PCI_BIST);
    cmd_pcie_print ("  base address 0 =              0x%8x\r\n",dev, 8, PCI_BASE_ADDRESS_0);
    switch (header_type & 0x03)
    {
    case PCI_HEADER_TYPE_NORMAL:    /* "normal" PCI device */
        cmd_pcie_print ("  base address 1 =              0x%8x\r\n",dev, 8, PCI_BASE_ADDRESS_1);
        cmd_pcie_print ("  base address 2 =              0x%8x\r\n",dev, 8, PCI_BASE_ADDRESS_2);
        cmd_pcie_print ("  base address 3 =              0x%8x\r\n",dev, 8, PCI_BASE_ADDRESS_3);
        cmd_pcie_print ("  base address 4 =              0x%8x\r\n",dev, 8, PCI_BASE_ADDRESS_4);
        cmd_pcie_print ("  base address 5 =              0x%8x\r\n",dev, 8, PCI_BASE_ADDRESS_5);
        cmd_pcie_print ("  cardBus CIS pointer =         0x%8x\r\n",dev, 8, PCI_CARDBUS_CIS);
        cmd_pcie_print ("  sub system vendor ID =        0x%4x\r\n",dev, 4, PCI_SUBSYSTEM_VENDOR_ID);
        cmd_pcie_print ("  sub system ID =               0x%4x\r\n",dev, 4, PCI_SUBSYSTEM_ID);
        cmd_pcie_print ("  expansion ROM base address =  0x%8x\r\n",dev, 8, PCI_ROM_ADDRESS);
        cmd_pcie_print ("  interrupt line =              0x%2x\r\n",dev, 2, PCI_INTERRUPT_LINE);
        cmd_pcie_print ("  interrupt pin =               0x%2x\r\n",dev, 2, PCI_INTERRUPT_PIN);
        cmd_pcie_print ("  min Grant =                   0x%2x\r\n",dev, 2, PCI_MIN_GNT);
        cmd_pcie_print ("  max Latency =                 0x%2x\r\n",dev, 2, PCI_MAX_LAT);
        break;

    case PCI_HEADER_TYPE_BRIDGE:    /* PCI-to-PCI bridge */

        cmd_pcie_print ("  base address 1 =              0x%8x\r\n",dev, 8, PCI_BASE_ADDRESS_1);
        cmd_pcie_print ("  primary bus number =          0x%2x\r\n",dev, 2, PCI_PRIMARY_BUS);
        cmd_pcie_print ("  secondary bus number =        0x%2x\r\n",dev, 2, PCI_SECONDARY_BUS);
        cmd_pcie_print ("  subordinate bus number =      0x%2x\r\n",dev, 2, PCI_SUBORDINATE_BUS);
        cmd_pcie_print ("  secondary latency timer =     0x%2x\r\n",dev, 2, PCI_SEC_LATENCY_TIMER);
        cmd_pcie_print ("  IO base =                     0x%2x\r\n",dev, 2, PCI_IO_BASE);
        cmd_pcie_print ("  IO limit =                    0x%2x\r\n",dev, 2, PCI_IO_LIMIT);
        cmd_pcie_print ("  secondary status =            0x%4x\r\n",dev, 4, PCI_SEC_STATUS);
        cmd_pcie_print ("  memory base =                 0x%4x\r\n",dev, 4, PCI_MEMORY_BASE);
        cmd_pcie_print ("  memory limit =                0x%4x\r\n",dev, 4, PCI_MEMORY_LIMIT);
        cmd_pcie_print ("  prefetch memory base =        0x%4x\r\n",dev, 4, PCI_PREF_MEMORY_BASE);
        cmd_pcie_print ("  prefetch memory limit =       0x%4x\r\n",dev, 4, PCI_PREF_MEMORY_LIMIT);
        cmd_pcie_print ("  prefetch memory base upper =  0x%8x\r\n",dev, 8, PCI_PREF_BASE_UPPER32);
        cmd_pcie_print ("  prefetch memory limit upper = 0x%8x\r\n",dev, 8, PCI_PREF_LIMIT_UPPER32);
        cmd_pcie_print ("  IO base upper 16 bits =       0x%4x\r\n",dev, 4, PCI_IO_BASE_UPPER16);
        cmd_pcie_print ("  IO limit upper 16 bits =      0x%4x\r\n",dev, 4, PCI_IO_LIMIT_UPPER16);
        cmd_pcie_print ("  expansion ROM base address =  0x%8x\r\n",dev, 8, PCI_ROM_ADDRESS1);
        cmd_pcie_print ("  interrupt line =              0x%2x\r\n",dev, 2, PCI_INTERRUPT_LINE);
        cmd_pcie_print ("  interrupt pin =               0x%2x\r\n",dev, 2, PCI_INTERRUPT_PIN);
        cmd_pcie_print ("  bridge control =              0x%4x\r\n",dev, 4, PCI_BRIDGE_CONTROL);
        break;

    case PCI_HEADER_TYPE_CARDBUS:    /* PCI-to-CardBus bridge */

        cmd_pcie_print ("  capabilities =                0x%2x\r\n",dev, 2, PCI_CB_CAPABILITY_LIST);
        cmd_pcie_print ("  secondary status =            0x%4x\r\n",dev, 4, PCI_CB_SEC_STATUS);
        cmd_pcie_print ("  primary bus number =          0x%2x\r\n",dev, 2, PCI_CB_PRIMARY_BUS);
        cmd_pcie_print ("  CardBus number =              0x%2x\r\n",dev, 2, PCI_CB_CARD_BUS);
        cmd_pcie_print ("  subordinate bus number =      0x%2x\r\n",dev, 2, PCI_CB_SUBORDINATE_BUS);
        cmd_pcie_print ("  CardBus latency timer =       0x%2x\r\n",dev, 2, PCI_CB_LATENCY_TIMER);
        cmd_pcie_print ("  CardBus memory base 0 =       0x%8x\r\n",dev, 8, PCI_CB_MEMORY_BASE_0);
        cmd_pcie_print ("  CardBus memory limit 0 =      0x%8x\r\n",dev, 8, PCI_CB_MEMORY_LIMIT_0);
        cmd_pcie_print ("  CardBus memory base 1 =       0x%8x\r\n",dev, 8, PCI_CB_MEMORY_BASE_1);
        cmd_pcie_print ("  CardBus memory limit 1 =      0x%8x\r\n",dev, 8, PCI_CB_MEMORY_LIMIT_1);
        cmd_pcie_print ("  CardBus IO base 0 =           0x%4x\r\n",dev, 4, PCI_CB_IO_BASE_0);
        cmd_pcie_print ("  CardBus IO base high 0 =      0x%4x\r\n",dev, 4, PCI_CB_IO_BASE_0_HI);
        cmd_pcie_print ("  CardBus IO limit 0 =          0x%4x\r\n",dev, 4, PCI_CB_IO_LIMIT_0);
        cmd_pcie_print ("  CardBus IO limit high 0 =     0x%4x\r\n",dev, 4, PCI_CB_IO_LIMIT_0_HI);
        cmd_pcie_print ("  CardBus IO base 1 =           0x%4x\r\n",dev, 4, PCI_CB_IO_BASE_1);
        cmd_pcie_print ("  CardBus IO base high 1 =      0x%4x\r\n",dev, 4, PCI_CB_IO_BASE_1_HI);
        cmd_pcie_print ("  CardBus IO limit 1 =          0x%4x\r\n",dev, 4, PCI_CB_IO_LIMIT_1);
        cmd_pcie_print ("  CardBus IO limit high 1 =     0x%4x\r\n",dev, 4, PCI_CB_IO_LIMIT_1_HI);
        cmd_pcie_print ("  interrupt line =              0x%2x\r\n",dev, 2, PCI_INTERRUPT_LINE);
        cmd_pcie_print ("  interrupt pin =               0x%2x\r\n",dev, 2, PCI_INTERRUPT_PIN);
        cmd_pcie_print ("  bridge control =              0x%4x\r\n",dev, 4, PCI_CB_BRIDGE_CONTROL);
        cmd_pcie_print ("  subvendor ID =                0x%4x\r\n",dev, 4, PCI_CB_SUBSYSTEM_VENDOR_ID);
        cmd_pcie_print ("  subdevice ID =                0x%4x\r\n",dev, 4, PCI_CB_SUBSYSTEM_ID);
        cmd_pcie_print ("  PC Card 16bit base address =  0x%8x\r\n",dev, 8, PCI_CB_LEGACY_MODE_BASE);
        break;

    default:
        printf("unknown header\r\n");
        break;
    }
}

//------pcie命令----------------------------------
//功能：shell命令，查看pcie设备信息
//参数：param: 字符串
//返回：true:  打印完成
//----------------------------------------------------------
bool_t sh_cmd_pcie(char *param)
{
    ptu32_t busno;
    char *next_param;
    char *str_busno;
    int i;
    if(param == NULL)
    {
        for(i =0; i<=pci_last_busno(); i++)
            pciinfo(i);
        return true;
    }
    str_busno = Sh_GetWord(param,&next_param);
    if(str_busno == NULL)
    {
        for(i =0; i<=pci_last_busno(); i++)
            pciinfo(i);
        return true;
    }
    busno = strtol(str_busno, (char **)NULL, 0);
    if(busno>pci_last_busno())
    {
        printf("输入的pcie设备 bus号超过了当前总线上的最大bus号\r\n");
        return false;
    }
    else
        pciinfo(busno);
    return true;

}
