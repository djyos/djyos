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
//所属模块：网络模块
//作者：Merlin
//版本：V0.1.0
//文件描述：dm9000的驱动文件，向上层提供如读、写包的函数
//其他说明：
//修订历史：
//2. 日期：
//   作者：
//   新版本号：
//   修改说明：
//1. 日期: 2012-02-27
//   作者：Merlin
//   新版本号：V0.1.0
//   修改说明：原始版本
//------------------------------------------------------
#include "stdint.h"
#include "djyos.h"
#include "stdio.h"
#include "stm32f10x.h"
#include "enet.h"
#include "ethernet.h"
#include "nicxx.h"

#define cn_int_line_enet (cn_int_line_EXTI9_5)  // dm9000a的到STM32的入口中断

#define ADDRW8(r)   (*(volatile u8 *)DM9000A_INDEX = r)
#define DATAR8()    (*(volatile u8 *)DM9000A_DATA)
#define DATAW8(d)   (*(volatile u8 *)DM9000A_DATA = d)

#define ADDRW16(r)  (*(volatile u16 *)DM9000A_INDEX = r)
#define DATAR16()   (*(volatile u16 *)DM9000A_DATA)
#define DATAW16(d)  (*(volatile u16 *)DM9000A_DATA = d)

extern struct enet_rcv_packet *__hw_read_in(struct enet_rcv_packet *lst);
extern s32 __hw_write_out(struct enet_send_section *upsec);
extern s32 __hw_ctrl(enum enet_hw_ctrl_opcode opcode);

enum DM9000A_PHY_mode { DM9000A_10MHD = 0, DM9000A_100MHD = 1,
    DM9000A_10MFD = 4, DM9000A_100MFD = 5, DM9000A_AUTO = 8,
    DM9000A_1M_HPNA = 0x10
};
enum DM9000A_NIC_TYPE { FASTETHER_NIC = 0, HOMERUN_NIC = 1, LONGRUN_NIC = 2
};

struct _eth_hw_info {
    u8  io; // IO mode 0:16-bit; 1:32-bit; 2:8-bit
    u8  base; // BASE mode 1:10MHalfDuplex; 2:10MFullDuplex; 4:100MHD; 8:100MFD
};

struct _eth_hw_info ehi;    // ethernet hardware connect information

static int media_mode = DM9000A_AUTO;
// DM9000A的信号量，当一个线程要使用DM9000A发送数据或者接收数据时，首先应取得该信号量
static struct tagSemaphoreLCB* semp_dm9000;

static u8 ior(u8 reg)
{
    ADDRW8(reg);
    return (u8)DATAR8();
}

static void iow(u8 reg, u8 value)
{
    ADDRW8(reg);
    DATAW8(value);
}

//----写PHY（半字）--------------------------------------------------------------
//功能：向PHY中写入16位的数据
//参数：reg，寄存器偏移地址
//      value，要写入的值
//返回：无
//-----------------------------------------------------------------------------
static void phyw_h(u8 reg, u16 value)
{
    iow(DM9000A_EPAR, reg|0x40);// 使用内部PHY时bit[7:6]要为01b
    iow(DM9000A_EPDRL, value&0xFF);
    iow(DM9000A_EPDRH, (value&0xFF00)>>8);
    iow(DM9000A_EPCR, 0x0A);        // 选择PHY功能，写功能
    while (ior(DM9000A_EPCR) & 0x01)
    {
        Djy_DelayUs(10);
    }
    Djy_DelayUs(150);
    iow(DM9000A_EPCR, 0x00);        // 由程序清零
}

//----读PHY（半字）--------------------------------------------------------------
//功能：从PHY中读取16位的数据
//参数：reg，寄存器偏移地址
//返回：读得的寄存器值
//-----------------------------------------------------------------------------
static u16 phyr_h(u8 reg)
{
    u16 tmp;

    iow(DM9000A_EPAR, reg|0x40);    // 使用内部PHY时bit[7:6]要为01b
    iow(DM9000A_EPCR, 0x0C);        // 选择PHY功能，读取功能
    while (ior(DM9000A_EPCR) & 0x01)
    {
        Djy_DelayUs(10);
    }
    Djy_DelayUs(150);
    iow(DM9000A_EPCR, 0x00);        // 由程序清零
    tmp = ior(DM9000A_EPDRL);
    tmp |= ior(DM9000A_EPDRH)<<8;

    return tmp;
}

extern char *Sh_GetWord(char *buf,char **next);
bool_t debug_dm9000a_read_reg(char *param)
{
    char *word, *next_param;
    int i;
    u32 reg = 0xFFFF;

    // 提取为SOCKET号
    if (param)
    {
        next_param = param;
        word = Sh_GetWord(next_param, &next_param);
        reg = __sh_atol(word);
        word = Sh_GetWord(next_param, &next_param);
        if(word != NULL)
        {
            printf("\r\n参数错误\r\n");
            return false;
        }
    }

    if (Lock_SempPend(semp_dm9000, 10000*mS) == true)
    {
        Int_SaveAsynLine(cn_int_line_enet); // 关闭DM9000A的外部中断
        if (reg != 0xFFFF)
        {
            printf("\r\n读取DM9000A的【%4x】寄存器：", reg);
            printf("\r\n %4x:%4x", reg, ior(reg));
        }
        else
        {
            printf("\r\n读取DM9000A的【所有】寄存器：");
            printf("\r\n");
            for (i=0; i<=0x1f; i++)
            {
                printf("\r\n %4x:%4x", i, ior(i));
            }
            printf("\r\n");
            for (i=0x22; i<=0x25; i++)
            {
                printf("\r\n %4x:%4x", i, ior(i));
            }
            printf("\r\n");
            for (i=0x28; i<=0x34; i++)
            {
                printf("\r\n %4x:%4x", i, ior(i));
            }

            printf("\r\n");
            i = 0x38;
            printf("\r\n %4x:%4x", i, ior(i));
            i = 0x39;
            printf("\r\n %4x:%4x", i, ior(i));

            printf("\r\n");
            i = 0x50;
            printf("\r\n %4x:%4x", i, ior(i));
            i = 0x51;
            printf("\r\n %4x:%4x", i, ior(i));

            printf("\r\n");
            i = 0xF0;
            printf("\r\n %4x:%4x", i, ior(i));
            i = 0xF1;
            printf("\r\n %4x:%4x", i, ior(i));
            i = 0xF2;
            printf("\r\n %4x:%4x", i, ior(i));
            i = 0xF4;
            printf("\r\n %4x:%4x", i, ior(i));
            i = 0xF5;
            printf("\r\n %4x:%4x", i, ior(i));
            i = 0xF6;
            printf("\r\n %4x:%4x", i, ior(i));
            i = 0xF8;
            printf("\r\n %4x:%4x", i, ior(i));

            printf("\r\n");
            for (i=0xFA; i<=0xFF; i++)
            {
                printf("\r\n %4x:%4x", i, ior(i));
            }
        }

        printf("\r\n");

        Int_RestoreAsynLine(cn_int_line_enet);  // 打开DM9000A的外部中断
        Lock_SempPost(semp_dm9000);
    }
    return true;
}

bool_t debug_dm9000a_write_reg(char *param)
{
    char *word, *next_param;
    u32 reg = 0;
    u32 value = 0;

    // 提取为SOCKET号
    if (param)
    {
        next_param = param;
        word = Sh_GetWord(next_param, &next_param);
        reg = __sh_atol(word);
        word = Sh_GetWord(next_param, &next_param);
        value = __sh_atol(word);
        word = Sh_GetWord(next_param, &next_param);
        if(word != NULL)
        {
            printf("\r\n参数错误\r\n");
            return false;
        }
    }

    if (Lock_SempPend(semp_dm9000, 10000*mS) == true)
    {
        Int_SaveAsynLine(cn_int_line_enet); // 关闭DM9000A的外部中断
        printf("\r\n写入DM9000A的【%4x】寄存器：%4x", reg, value);
        iow(reg, value);

        printf("\r\n");

        Int_RestoreAsynLine(cn_int_line_enet);  // 打开DM9000A的外部中断
        Lock_SempPost(semp_dm9000);
    }
    return true;
}

//----设置PHY工作模式------------------------------------------------------------
//功能：设置PHY工作模式
//参数：无
//返回：无
//-----------------------------------------------------------------------------
static void set_phy_mode(void)
{
    u16 phy_reg4 = 0x01e1, phy_reg0 = 0x3100;

    // reset PHY
    phyw_h(0, 0x8000);
    do {
        Djy_DelayUs(50);
    } while (phyr_h(0) & 0x8000);

    switch (media_mode) {
    case DM9000A_10MHD:
        phy_reg4 = 0x21;
        phy_reg0 = 0x0000;
        break;
    case DM9000A_10MFD:
        phy_reg4 = 0x41;
        phy_reg0 = 0x1100;
        break;
    case DM9000A_100MHD:
        phy_reg4 = 0x81;
        phy_reg0 = 0x2000;
        break;
    case DM9000A_100MFD:
        phy_reg4 = 0x101;
        phy_reg0 = 0x3100;
        break;
    }
    phyw_h(4, phy_reg4);    // Set PHY media mode
    phyw_h(0, phy_reg0);    // Tmp
}

//----探测芯片的ID是否为DM9000A--------------------------------------------------
//功能：探测芯片的ID是否为DM9000A
//参数：无
//返回：读取正确返回0，错误时返回错误号
//-----------------------------------------------------------------------------
static u8 dm9000a_probe(void)
{
    u32 id;

    // 读取厂商ID和产品ID号
    id = ior(DM9000A_PIDL);
    id |= ior(DM9000A_PIDH)<<8;
    id |= ior(DM9000A_VIDL)<<16;
    id |= ior(DM9000A_VIDH)<<24;
    if (id != DM9000A_VID_PID) // 读出的ID应当为0x0A469000
    {
        return -1;
    }
    return 0;
}

//----设置MAC地址---------------------------------------------------------------
//功能：设置MAC地址
//参数：mac，以太网的地址数组（6个字节）
//返回：无
//-----------------------------------------------------------------------------
void dm9000a_set_macaddr(u8 mac[])
{
    for (int i=0; i<6; i++)
        iow(DM9000A_PAR0+i, mac[i]);
}

//----设置多播地址---------------------------------------------------------------
//功能：设置多播地址
//参数：mulicast，多播地址数组（8个字节）
//返回：无
//-----------------------------------------------------------------------------
void dm9000a_set_multicastaddr(void)
{
    // 设置多播地址
    for (int i=0; i<8; i++)
        iow(DM9000A_MAR0+i, 0xFF);
}

//----DM9000A复位---------------------------------------------------------------
//功能：软件复位DM9000A。某些寄存器的值不受软件复位的影响。
//参数：无
//返回：无
//-----------------------------------------------------------------------------
void dm9000a_reset(void)
{
    iow(DM9000A_NCR, 0x01); // RESET
    do {
        Djy_DelayUs(30);    // 延时30us，应用手册上说至少20us
    } while (ior(DM9000A_NCR) & 0x01);
    iow(DM9000A_NCR, 0x00);
}

//----复位DM9000A到某一特定的原始状态---------------------------------------------
//功能：让DM9000A回到这样一个状态：除了中断未打开之外，其它有关于DM9000A的所有初始化工作
//      均已完成的地步。DM9000A芯片在从内部RAM中读数据时，如果读到的标志位不是1也不是0
//      时，会调用本函数，本函数需要清空RAM中的数据及使读写指针复位。
//参数：无
//返回：无
//-----------------------------------------------------------------------------
void dm9000a_reset_to_new(void)
{
    u8 flags;

    iow(DM9000A_IMR, 0x00); // 禁止发送、接收中断

    ior(DM9000A_ISR);   // 清除所有中断状态

    flags = 0x80;   // 读地址复位到0xC000，写地址复位到0x0000
    flags |= 0x01;  // 使能接收中断
    iow(DM9000A_IMR, flags);

    iow(DM9000A_TCR, 0);
    iow(DM9000A_RCR, 0x3d); // 使能接收（广播、Runt包，不使用混杂模式）
}

//----DM9000A初始化-------------------------------------------------------------
//功能：初始化DM9000A
//参数：无
//返回：正常时返回0，错误时返回错误号
//-----------------------------------------------------------------------------
u32 dm9000a_init(void)
{
    u16 i;

    dm9000a_reset();    // 软件复位

    if (dm9000a_probe() != 0)   // 探查当前操作的芯片是否为DM9000A（读出ID进行对比）
        return -1;
    iow(DM9000A_GPR, 0x00); // PHYPD=0时使能PHY，默认为1表示不使能内部PHY
    Djy_DelayUs(20*mS);

    iow(DM9000A_NCR, 0x03); // 选择内部PHY
    Djy_DelayUs(20);
    iow(DM9000A_NCR, 0x03); // 选择内部PHY（重复一次）
    Djy_DelayUs(20);

    ior(DM9000A_NSR);   // 清除发送状态（读取即可清除）

    dm9000a_set_macaddr(MyMAC); // 设置MAC地址
    dm9000a_set_multicastaddr();    // 设置多播地址

    set_phy_mode(); // 自动协商

    while (!(phyr_h(1) & 0x20))     // 自动协商完成标志位
    {
        Djy_EventDelay(5*mS);   // 50mS
    }

    // 读回IO、Base模式
    ehi.io = (ior(DM9000A_ISR)>>6) & 0x3;
    ehi.base = (phyr_h(17)>>12) & 0xF;

    semp_dm9000 = Lock_SempCreate(1, 1, NULL);
    if (semp_dm9000 == NULL)
    {
        return -1;
    }

    dm9000a_reset_to_new();
    // TODO DM9000A具有IP、UDP、TCP的检验和自动计算功能
    //...

    i = ior(DM9000A_NSR);       // xxx del 需要等于0x40才表示连接成功

    return 0;
}

//----从DM9000A的内部缓冲区中读一块数据--------------------------------------------
//功能：从DM9000A的内部缓冲区中读一块数据
//参数：data_ptr，接收缓冲区头指针
//      count，读取的字节数
//返回：无
//-----------------------------------------------------------------------------
static void dm9000a_inblk_16bit(void *data_ptr, int count)
{
    int i;
    u32 tmplen = (count + 1) / 2;

    for (i = 0; i < tmplen; i++) {
        ((u16 *) data_ptr)[i] = DATAR16();
    }
}

//----从DM9000A中读取一块数据----------------------------------------------------
//功能：分配一块足够大小的内存，然后将DM9000A中的第一包数据拷贝过去，这个新的数据包会链到
//      pg_enet_rcvp_list.process链表中，等待后续的处理
//参数：lst，数据包的接收链表头指针
//返回：正常时为读取的字节数，如果包的长度大于以太网最大长度，返回值为-1
//-----------------------------------------------------------------------------
static int dump_data16(struct enet_rcv_packet *lst, struct enet_rcv_packet **nlst)
{
    u16 status, len;
    u32 align_len;  // 向上对齐后的最终的大小
    struct enet_rcv_packet* pt_rcv_packet;

    ADDRW8(DM9000A_MRCMD);
    status = DATAR16(); // 状态，如得到0x4001：高8位为状态，低8位为0x01标志
    len = DATAR16();    // 本帧数据包的长度
    if (len > DM9000A_PKT_MAX)  // 超过以太网长度
    {
        return -1;
    }

    // 申请的内存，向上对齐
    align_len = sizeof(struct enet_rcv_packet) + len;
    pt_rcv_packet = enet_malloc_rcv_packet(align_len);
    if (pt_rcv_packet)
    {
        dm9000a_inblk_16bit((u16*)pt_rcv_packet->section, len);
        *nlst = enet_add_to_rcv_packet_list(pt_rcv_packet, lst);
    }

    return len;
}

//----从DM9000A中读出所有数据----------------------------------------------------
//功能：把DM9000A中的所有数据都读取出来，这些数据包会组织在lst链表中。
//参数：lst，数据包的接收链表头指针
//返回：数据包的接收链表头指针（可能与输入的lst不同）
//-----------------------------------------------------------------------------
struct enet_rcv_packet *__hw_read_in(struct enet_rcv_packet *lst)
{
    u32 tmp;
    u32 len;
    u32 totlen = 0;
    static u32 times = 0;
    struct enet_rcv_packet *nlst = NULL;

    // 请求DM9000A硬件操作的信号量，等待5S秒如果没有发出去，则返回
    if (Lock_SempPend(semp_dm9000, 0) == true)
    {
        // 接收过程中如果有中断发生，中断响应函数读 写DM9000的其他寄存器会打断接收过程。
        Int_SaveAsynLine(cn_int_line_enet); // 关闭DM9000A的外部中断
        ior(DM9000A_MRCMDX);    // dummy read
        tmp = (u8)DATAR16();
        if (tmp == 0x01)    // 第一个字节读出为01h
        {
            switch (ehi.io)
            {
            case ENUM_DM9000A_IO_16BIT: // 16-bit
                while (tmp == 0x01)
                {
                    len = dump_data16(lst, &nlst);
                    if (len == -1)
                    {
                        dm9000a_reset_to_new();
                        totlen = 0;
                    }
                    else
                    {
                        totlen += len;
                        // 试读下一帧数据，若为01h则继续读数，若为0则表示没有有效数据
                        ADDRW8(DM9000A_MRCMDX);
                        tmp = (u8)DATAR16();
                        lst = nlst;
                    }
                }
                __hw_ctrl(enum_enet_hw_ctrl_clear_rx_int);  // 清除接收中断
                break;
            case ENUM_DM9000A_IO_32BIT: // 32-bit
                break;
            case ENUM_DM9000A_IO_8BIT:  // 8-bit
                break;
            default:
                break;
            }
        }
        else if (tmp != 0)
        {
            times++;
            if (times > 5)
            {
                debug_dm9000a_read_reg(NULL);
            }
            dm9000a_reset_to_new();
            totlen = 0;
            if (times > 5)
            {
                debug_dm9000a_read_reg(NULL);
                times = 0;
            }
        }

        Int_RestoreAsynLine(cn_int_line_enet);  // 打开DM9000A的外部中断
        Lock_SempPost(semp_dm9000);
    }

    return nlst;
}

//----发送一帧数据---------------------------------------------------------------
//功能：发送一个数据包到以太网上
//参数：upsec，上层数据包（一个链表结构）的头指针
//返回：正常时为发送的字节数；错误时为-1
//-----------------------------------------------------------------------------
s32 __hw_write_out(struct enet_send_section *upsec)
{
    u16 dat;
    u32 i, len, times, totlen;
    struct enet_send_section* pt_ssec;

    // 请求DM9000A硬件操作的信号量，如果没有发出去，则返回
    if (Lock_SempPend(semp_dm9000, 0) == true)
    {
        Int_SaveAsynLine(cn_int_line_enet); // 关闭DM9000A的外部中断
        totlen = 0;
        pt_ssec = upsec;

        ADDRW8(DM9000A_MWCMD);
        if (ehi.io == 0)    // 16-bit
        {
            u8* pt_data;    // 定义字节为8位的，下面进行强制转换
            while (pt_ssec != NULL)
            {
                pt_data = (u8*)pt_ssec->data;
                len = pt_ssec->count;
                totlen += len;
                if ((ptu32_t)pt_data & 0x01)    //判断指针是否双字节对齐
                {
                    for (i=0; i<len; i+=2)
                    {
                        dat = pt_data[i] + (pt_data[i+1]<<8);
                        DATAW16(dat);
                    }
                }
                else
                {
                    times = (len+1)/2;
                    for (i=0; i<times; i++)
                        DATAW16(((u16*)pt_data)[i]);
                }

                pt_ssec = pt_ssec->next_data_ssection;
            }
        }
        else if (ehi.io == 1) // 32-bit
        {
        }
        else if (ehi.io == 2) // 8-bit
        {
        }

        if (totlen > 0)
        {
            iow(DM9000A_TXPLH, (totlen>>8) & 0xff);
            iow(DM9000A_TXPLL, totlen & 0xff);
            iow(DM9000A_NSR, 0x2c);     // 清除状态寄存器
            iow(DM9000A_TCR, ior(DM9000A_TCR) | 0x01);      //发送数据到以太网上

            // 等待发送完成
            // NSR中TX1END及TX2END（发送完成）
            // ISR中PTM（发送完成）
            times = 100;    // 相当等待于3S
            while (!(ior(DM9000A_NSR) & 0x0C) || !(ior(DM9000A_ISR) & 0x02))
            {
                if (times == 0)
                {
                    dm9000a_reset_to_new();
                    break ;
                }
                times--;
                Djy_DelayUs(10);
            }
        }

        Int_RestoreAsynLine(cn_int_line_enet);  // 打开DM9000A的外部中断
        Lock_SempPost(semp_dm9000);
    }

    if (times == 0)
        return -1;

    return totlen;
}

//----网络硬件的控制函数----------------------------------------------------------
//功能：
//参数：
//返回：正常时返回0，错误时返回错误号
//-----------------------------------------------------------------------------
s32 __hw_ctrl(enum enet_hw_ctrl_opcode opcode)
{
    s32 error = 0;
    u8 flag;

    switch (opcode)
    {
    case enum_enet_hw_ctrl_init:
        error = dm9000a_init();
        break;
    case enum_enet_hw_ctrl_reset:
        error = dm9000a_init();
        break;
    case enum_enet_hw_ctrl_en_tx_int:       //使能发送中断
        flag = ior(DM9000A_IMR);
        iow(DM9000A_IMR, flag | 0x02);
        break;
    case enum_enet_hw_ctrl_en_rx_int:       // 使能接收中断
        flag = ior(DM9000A_IMR);
        iow(DM9000A_IMR, flag | 0x01);
        break;
    case enum_enet_hw_ctrl_dis_tx_int:      // 禁止发送中断
        flag = ior(DM9000A_IMR);
        iow(DM9000A_IMR, flag & ~0x02);
        break;
    case enum_enet_hw_ctrl_dis_rx_int:      // 禁止接收中断
        flag = ior(DM9000A_IMR);
        iow(DM9000A_IMR, flag & ~0x01);
        break;
    case enum_enet_hw_ctrl_clear_tx_int:    // 清除发送中断
        iow(DM9000A_ISR, 0x02);
        break;
    case enum_enet_hw_ctrl_clear_rx_int:    // 清除接收中断
        iow(DM9000A_ISR, 0x01);
        break;
    default :
        break;
    }

    return error;
}

//----发送中断响应函数-----------------------------------------------------------
//功能：目前所使用的DM9000A驱动未使用发送中断
//参数：无
//返回：无
//-----------------------------------------------------------------------------
void dm9000a_txint_handler(void)
{
    __hw_ctrl(enum_enet_hw_ctrl_clear_tx_int);
    __hw_ctrl(enum_enet_hw_ctrl_en_tx_int);
}

//----接收中断函数---------------------------------------------------------------
//功能：禁止接收中断，并释放一个接收信号量
//参数：无
//返回：正常为1
//-----------------------------------------------------------------------------
u32 dm9000a_rxint_handler(void)
{
    __hw_ctrl(enum_enet_hw_ctrl_dis_rx_int);    // 禁止接收中断
    Lock_SempPost(enet_state.semp_recv);    // 释放拷贝数据的信号量

    return 1;
}

//----DM9000A的看门狗超时响应----------------------------------------------------
//功能：
//参数：无
//返回：正常时为1
//-----------------------------------------------------------------------------
void dm9000a_timeout(void)
{
    if (Lock_SempPend(semp_dm9000, CN_TIMEOUT_FOREVER) == true)
    {
        Int_SaveAsynLine(cn_int_line_enet); // 关闭DM9000A的外部中断
        dm9000a_init();
        // fixme 喂狗
        Int_RestoreAsynLine(cn_int_line_enet);
    }
}

//----DM9000A的中断响应函数------------------------------------------------------
//功能：处理IO上引发的中断的响应函数，通过读取DM9000A的中断状态位进行分支
//参数：无
//返回：正常时为1
//-----------------------------------------------------------------------------
u32 enet_int_line_handler(ufast_t ufl_line)
{
    u32 flag;

    EXTI_ClearITPendingBit(EXTI_Line6);

    // 中断中不会产生调度，而在读和写DM9000A的函数中都已经禁止了中断
    // 意思就是说，进入本服务函数时不可能在读和写两函数中
    // 反之，在读写DM9000A时也不可能产生中断
    // 因而对DM9000A_ISR的读写就不必进行信号量保护了
    flag = ior(DM9000A_ISR);

    // Packet Received
    if (flag & 0x01)
    {
        dm9000a_rxint_handler();
    }

    // Packet Transmitted
    if (flag & 0x02)
    {
        //dm9000a_txint_handler();
    }

    // Rx Overflow
    if (flag & 0x04)
    {
        iow(DM9000A_ISR, flag|0x04);
    }

    // Rx Overflow Counter Overflow
    if (flag & 0x08)
    {
        iow(DM9000A_ISR, flag|0x08);
    }

    return 1;
}

//----dm9000a的外部中断初始化----------------------------------------------------
//功能：
//参数：
//返回：
//-----------------------------------------------------------------------------
void dm9000a_exti_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    // EXTI6中断为DM9000A的中断（EXTI6中断与EXTI5-9是共用的）
    // 使能GPIOG普通及复用功能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG | RCC_APB2Periph_AFIO, ENABLE);

    // IRQ在设置GPIO时为输入，下面再设置为中断输入引脚
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    //选择中断通道2
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    //抢占式中断优先级设置为0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    //响应式中断优先级设置为0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                   //使能中断
    NVIC_Init(&NVIC_InitStructure);

    // 将GPIOG6引脚设置为外部中断触发引脚
    EXTI_ClearITPendingBit(EXTI_Line6); // 清除外部中断线6
    // 选中GPIOG6管脚为中断线
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, GPIO_PinSource6);
    EXTI_InitStructure.EXTI_Line = EXTI_Line6; //选择中断线路1
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //设置为中断请求，非事件请求
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //设置中断触发方式为上下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;   //外部中断使能
    EXTI_Init(&EXTI_InitStructure);

    Int_SetClearType(cn_int_line_enet,CN_INT_CLEAR_PRE);
    Int_IsrConnect(cn_int_line_enet, enet_int_line_handler);
    Int_SettoAsynSignal(cn_int_line_enet);
    Int_ClearLine(cn_int_line_enet);
    Int_RestoreAsynLine(cn_int_line_enet);
}

//----网络芯片的IO口初始化-------------------------------------------------------
//功能：初始化与网络芯片相关的总线访问接口、中断线、IO口等，该函数应该达到这样的目的：
//      1. 使网络芯片可以通过iow及ior读写寄存器；
//      2. 使网络模块可以接收到中断（当网络芯片本身已经开启了中断）；
//      3. 如网络模块部分还使用了其它IO端口或硬件模块，那么在此函数中也要相应地初始化；
//      4. 初始化网络模块状态结构体中的回调函数
//参数：pt_enet_st，网络状态信息结构体指针
//返回：正常时为1
//fixme lst，修改：应该在enet.c中提供enet_nic_install函数，在module_init_dm9000a中调用
//     该函数。参照gk_display.c中的gk_install_display函数。
//-----------------------------------------------------------------------------
u8 enet_state_nic_init(struct _enet_state *pt_enet_st)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // 设置引脚
    // 使能GPIOG普通及复用功能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG | RCC_APB2Periph_AFIO, ENABLE);
    // RST
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    // dm9000a的外部中断初始化
    dm9000a_exti_init();

    // 复位dm9000a
    GPIO_ResetBits(GPIOG, GPIO_Pin_7);
    Djy_DelayUs(50*mS);
    GPIO_SetBits(GPIOG, GPIO_Pin_7);
    Djy_DelayUs(50*mS); // RST未激活后5uS，DM9000A开始可以操作

    // 对网络状态结构体中的回调函数进行初始化
    if (pt_enet_st)
    {
        pt_enet_st->hw_read_in = __hw_read_in;
        pt_enet_st->hw_write_out = __hw_write_out;
        pt_enet_st->hw_ctrl = __hw_ctrl;

        pt_enet_st->poll = ethernet_poll;
        pt_enet_st->send = ethernet_send;
    }

    return 1;
}

