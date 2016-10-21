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
//所属模块: SD卡驱动模块
//作者：wjz
//版本：V1.0.0
//文件描述: 用于tq2440的sd卡驱动。不支持sdv2.0以上的sd卡(2G以上不支持)
//其他说明:本驱动模块源码来自三星出厂板子的sd卡驱动修改而成
//修订历史:
// 1. 日期: 2011-06-19
//   作者: wjz
//   新版本号: V1.0.0
//   修改说明: 移植sd卡驱动到tq2440
//------------------------------------------------------

#include "stdint.h"
#include <stdio.h>
#include <string.h>
#include "cpu_peri.h"

/************************SD卡命令说明****************************************
Class0 :(卡的识别、初始化等基本命令集)
cmd0:复位SD 卡.
CMD1:读OCR寄存器.
cmd9:读CSD寄存器.
CMD10:读CID寄存器.
CMD12:停止读多块时的数据传输
cmd13:读 Card_Status 寄存器

Class2 (读卡命令集):
CMD16:设置块的长度
CMD17:读单块.
CMD18:读多块,直至主机发送CMD12为止 .

Class4(写卡命令集) :
CMD24:写单块.
CMD25:写多块.
CMD27:写CSD寄存器 .

Class5 (擦除卡命令集):
CMD32:设置擦除块的起始地址.
CMD33:设置擦除块的终止地址.
CMD38: 擦除所选择的块.

Class6(写保护命令集):
CMD28:设置写保护块的地址.
CMD29:擦除写保护块的地址.
CMD30: Ask the card for the status of the write protection bits

class7：卡的锁定，解锁功能命令集
class8：申请特定命令集 。
class10 －11 ：保留
*/

unsigned int Tx_buf[128];   //128[word]*4bit=512[byte]
unsigned int Rx_buf[128];   //128[word]*4bit=512[byte]
volatile unsigned int rd_cnt;
volatile unsigned int wt_cnt;
int MMC=0;  // 0:SD  , 1:MMC 留着，以后MMC卡使用
volatile int RCA;

//----大小端相互转换---------------------------------------------------
//功能：int类型的，大小端随意转换(即小端可以转为大端，大端也可以转为小端)
//参数：inta， int类型参数
//      intb， int类型参数
//返回：
//---------------------------------------------------------------------
void byte_conversion(uint32_t *inta,uint32_t *intb)
{
    uint8_t *a_byte;
    uint8_t *b_byte;

    a_byte=(uint8_t *)inta;
    b_byte=(uint8_t *)intb;
    b_byte[3]=a_byte[0];
    b_byte[2]=a_byte[1];
    b_byte[1]=a_byte[2];
    b_byte[0]=a_byte[3];

}


//----SD卡选择--------------------------------------------------------
//功能：本函数是用来选择一张SD卡
//参数：sel_desel
//返回：1，成功
//      0，失败
//---------------------------------------------------------------------

void card_sel_desel(char sel_desel)
{
    //-- Card select or deselect
    if(sel_desel)
    {
RECMDS7:
    rSDICARG=RCA<<16;   // CMD7(RCA,stuff bit)
    rSDICCON= (0x1<<9)|(0x1<<8)|0x47;   // sht_resp, wait_resp, start, CMD7
    //发送CMD7，实现对卡的选择
    if(!chk_cmd_end(7, 1))
        goto RECMDS7;
    rSDICSTA=0xa00; // Clear cmd_end(with rsp)
    //--State(transfer) check
    if( (rSDIRSP0 & 0x1e00) !=0x800 )
        goto RECMDS7;
    }
    else
    {
RECMDD7:
        rSDICARG=0<<16;     //CMD7(RCA,stuff bit)
        rSDICCON=(0x1<<8)|0x47; //no_resp, start, CMD7
        //发送CMD7，实现对卡的选择
        if(!chk_cmd_end(7, 0))
        goto RECMDD7;
        rSDICSTA=0x800; // Clear cmd_end(no rsp)
    }
}
//----读一扇区--------------------------------------------------------
//功能：本函数是用来读取SD卡指定某一扇区
//参数：addr，扇区号
//      buf，缓冲区
//返回：
//---------------------------------------------------------------------

void read_one_sector(u32 sector_addr,unsigned int *buf)
{
    unsigned int value;
    int status;

    rd_cnt=0;
    rSDIFSTA=rSDIFSTA|(1<<16);  // FIFO reset
/*
    保留 [31:25]
    Burst4 enable   [24]    在DMA模式下使能Burst4。仅当数据大小是字时该位被
     (Burst4)               置位。0：无效 1：Burst4使能

    Data Size       [23:22] 指出用FIFO传输的大小，哪个类型，半字或字。00 = 字节
    (DataSize)              传输，01 =半字传输 10 = 字传输， 11 = 保留

    SDIO Interrupt  [21]    决定SDIO的中断周期是 2个周期还是外部更多周期，当数据块
    Period Type(PrdType)    最后被发送（对SDIO）。0=正好两个周期 1=更多周期（像单周期）

    Transmit After  [20]    决定数据传输在响应收到后开始或不开始。0= 在DatMode设置后直接
    Response(TARSP)         1= 在响应收到后（假定设置DatMode设为 2b11）

    Receive After   [19]    决定数据传输在命令发出后开始或不开始 0= 在DatMode设置后直接
    Command(RACMD)          1= 在命令发出后（假定设置DatMode设为2b10）

    Busy After      [18]    决定忙接收在命令发出后开始或不开始 0= 在DatMode设置后直接
    Command(BACMD)          1= 在命令发出后（假定设置DatMode设为2b01）

    Block mode      [17]    数据传输模式 0=流数据传输 1=模块数据传输
    (BlkMode)

    Wide bus        [16]    决定使能宽总线模式 0：标准总线模式（仅使用SDIDAT[0]）
    enable(WideBus)         1：宽总线模式（使用SDIDAT[3]）

    DMA Enable      [15]    使能DMA（当DMA操作完成时，该位应该无效）
    (EnDMA)                 0：无效（查询） 1：DMA使能

    Data Transfer   [14]    决定数据传输是否开始，该位自动清除。
    Start(DTST)             0：数据准备好， 1：数据开始

    Data Transfer   [13:12] 决定数据传输的方向 00 =无操作， 01 = 仅忙检测模式
    Mode (DatMode)          10 =数据接收模式，11 =数据发送模式

    BlkNum          [11:0] 模块数(0~4095)，当流模式时不考虑
*/
    rSDIDCON=(2<<22)|(1<<19)|(1<<17)|(1<<16)|(1<<14)|(2<<12)|(1<<0);
    //rSDIDCON，[11:0] 我设置是1，所以这里block_addr，必须是地址，而不是扇区号
    //移动9位，512字节。这样，block_addr就变成了"扇区号"了
    rSDICARG=sector_addr<<9;    // CMD17/18(addr)

    //开始准备读取数据

    rSDICCON=(0x1<<9)|(0x1<<8)|0x51;    // sht_resp, wait_resp, dat, start, CMD17
    if(!chk_cmd_end(17, 1)) //-- Check end of CMD17
        return ;
    rSDICSTA=0xa00; // Clear cmd_end(with rsp)
    while(rd_cnt<128)   // 128word=512byte
    {
        if((rSDIDSTA&0x20)==0x20) // Check timeout
        {
            rSDIDSTA=(0x1<<0x5);  // Clear timeout flag
            break;
        }
        status=rSDIFSTA;
        if((status&0x1000)==0x1000) // Is Rx data?
        {
            value = rSDIDAT;
            byte_conversion(&value,buf);
            *buf++;
            rd_cnt++;
        }
    }
    //-- Check end of DATA
    if(!chk_dat_end())
        return ;

    rSDIDCON=rSDIDCON&~(7<<12);
    rSDIFSTA=rSDIFSTA&0x200;    //Clear Rx FIFO Last data Ready, YH 040221
    rSDIDSTA=0x10;  // Clear data Tx/Rx end detect

}
//----写一扇区--------------------------------------------------------
//功能：本函数是用来把数据写入SD卡指定某一扇区
//参数：addr，扇区号
//      buf，缓冲区
//返回：
//---------------------------------------------------------------------
void write_one_sector(u32 sector_addr,unsigned int *buf)
{
    u32 value;
    int status;

    buf=(u32*)buf;//由于FAT是是以u8传入。不知道上面是否强制转换了
    wt_cnt=0;
    rSDIFSTA=rSDIFSTA|(1<<16);  //YH 040223 FIFO reset
    /*
    保留 [31:25]
    Burst4 enable   [24]    在DMA模式下使能Burst4。仅当数据大小是字时该位被
     (Burst4)               置位。0：无效 1：Burst4使能

    Data Size       [23:22] 指出用FIFO传输的大小，哪个类型，半字或字。00 = 字节
    (DataSize)              传输，01 =半字传输 10 = 字传输， 11 = 保留

    SDIO Interrupt  [21]    决定SDIO的中断周期是 2个周期还是外部更多周期，当数据块
    Period Type(PrdType)    最后被发送（对SDIO）。0=正好两个周期 1=更多周期（像单周期）

    Transmit After  [20]    决定数据传输在响应收到后开始或不开始。0= 在DatMode设置后直接
    Response(TARSP)         1= 在响应收到后（假定设置DatMode设为 2b11）

    Receive After   [19]    决定数据传输在命令发出后开始或不开始 0= 在DatMode设置后直接
    Command(RACMD)          1= 在命令发出后（假定设置DatMode设为2b10）

    Busy After      [18]    决定忙接收在命令发出后开始或不开始 0= 在DatMode设置后直接
    Command(BACMD)          1= 在命令发出后（假定设置DatMode设为2b01）

    Block mode      [17]    数据传输模式 0=流数据传输 1=模块数据传输
    (BlkMode)

    Wide bus        [16]    决定使能宽总线模式 0：标准总线模式（仅使用SDIDAT[0]）
    enable(WideBus)         1：宽总线模式（使用SDIDAT[3]）

    DMA Enable      [15]    使能DMA（当DMA操作完成时，该位应该无效）
    (EnDMA)                 0：无效（查询） 1：DMA使能

    Data Transfer   [14]    决定数据传输是否开始，该位自动清除。
    Start(DTST)             0：数据准备好， 1：数据开始

    Data Transfer   [13:12] 决定数据传输的方向 00 =无操作， 01 = 仅忙检测模式
    Mode (DatMode)          10 =数据接收模式，11 =数据发送模式

    BlkNum          [11:0] 模块数(0~4095)，当流模式时不考虑
*/
    rSDIDCON=(2<<22)|(1<<20)|(1<<17)|(1<<16)|(1<<14)|(3<<12)|(1<<0);
    //扇区号转为地址，移动9位，是512字节的意思
    rSDICARG=sector_addr<<9;        // CMD24/25(addr)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x58;    //sht_resp, wait_resp, dat, start, CMD24
    if(!chk_cmd_end(24, 1)) //-- Check end of CMD24
        return;
    rSDICSTA=0xa00; // Clear cmd_end(with rsp)
    while(wt_cnt<128)
    {
        status=rSDIFSTA;
        if((status&0x2000)==0x2000)
        {
            byte_conversion(buf,&value);
            rSDIDAT=value;
            *buf++;
            wt_cnt++;
        }
    }
    if(!chk_dat_end())
        return;
    //Clear Data Transfer mode => no operation,Cleata
    //Data Transfer start
    rSDIDCON=rSDIDCON&~(7<<12);
    rSDIDSTA=0x10;  // Clear data Tx/Rx end
}

//----检查命令发送-----------------------------------------------------
//功能：判断命令是否发送成功
//参数：cmd，sd卡的命令，见本文件头部注释[sd卡命令说明]
//      be_resp，1 有应答，0无应答
//返回：1，成功
//      0，time out
//---------------------------------------------------------------------

int chk_cmd_end(int cmd, int be_resp)
{
    int finish0;

    if(!be_resp)    // No response
    {
        finish0=rSDICSTA;
        while((finish0&0x800)!=0x800)   // Check cmd end
        finish0=rSDICSTA;

        rSDICSTA=finish0;// Clear cmd end state

        return 1;
    }
    else    // With response
    {
        finish0=rSDICSTA;
        // Check cmd/rsp end
        //debug 这个是bug，如果没有SD卡，那么不就是在这里一直循环
        //有空了修改一下
        while( !( ((finish0&0x200)==0x200) | ((finish0&0x400)==0x400) ))
        finish0=rSDICSTA;
        if((cmd==1) | (cmd==41))    // CRC no check, cmd9 is a long Resp. command.
        {
            if( (finish0&0xf00) != 0xa00 )  // Check error
            {
                rSDICSTA=finish0;   // Clear error state

                if(((finish0&0x400)==0x400))
                return 0;   // Timeout error
            }
            rSDICSTA=finish0;   // Clear cmd & rsp end state
        }
        else    // CRC check
        {
            if( (finish0&0x1f00) != 0xa00 ) // Check error
            {
                rSDICSTA=finish0;   // Clear error state

                if(((finish0&0x400)==0x400))
                    return 0;   // Timeout error
            }
            rSDICSTA=finish0;
        }
        return 1;
    }
}
//----检查数据结束-----------------------------------------------------
//功能：检查数据发送或者接收完毕，以及是否超时
//参数：cmd，sd卡的命令，见本文件头部注释[sd卡命令说明]
//      be_resp，是否回应
//返回：1，成功
//      0，失败，
//---------------------------------------------------------------------
int chk_dat_end(void)
{
    int finish;

    finish=rSDIDSTA;
    while( !( ((finish&0x10)==0x10) | ((finish&0x20)==0x20) ))
        // Chek timeout or data end
        finish=rSDIDSTA;

    if( (finish&0xfc) != 0x10 )
    {
        rSDIDSTA=0xec;  // Clear error state
        return 0;
    }
    return 1;
}

//---使卡进入IDEL状态-----------------------------------------------------
//功能：使卡进入IDEL状态
//参数：无
//返回：无
//---------------------------------------------------------------------

void cmd0(void)
{
    //-- Make card idle state
    rSDICARG=0x0;       // cmd0(stuff bit)
    rSDICCON=(1<<8)|0x40;   // No_resp, start, cmd0

    //-- Check end of cmd0
    chk_cmd_end(0, 0);
    rSDICSTA=0x800;     // Clear cmd_end(no rsp)
}
//---检查是否是MMC卡---------------------------------------------------
//功能：检查是否是MMC卡，并且设置目标工作电压
//参数：无
//返回：无
//---------------------------------------------------------------------
int chk_MMC_OCR(void)
{
    int i;

    for(i=0;i<100;i++)  //Negotiation time is dependent on CARD Vendors.
    {
        rSDICARG=0xff8000;              //CMD1(SD OCR:2.7V~3.6V)
        rSDICCON=(0x1<<9)|(0x1<<8)|0x41;    //sht_resp, wait_resp, start, CMD1
        //YH 0903 [31]:Card Power up status bit (busy)
        if(chk_cmd_end(1, 1) && (rSDIRSP0>>16)==0x80ff)
        {
            rSDICSTA=0xa00; // Clear cmd_end(with rsp)
            return 1;   // Success
        }
    }
    rSDICSTA=0xa00; // Clear cmd_end(with rsp)
    return 0;       // Fail
}
//---检查是否是SD卡---------------------------------------------------
//功能：检查是否是SD卡，并且设置目标工作电压
//参数：无
//返回：无
//---------------------------------------------------------------------
int chk_SD_OCR(void)
{
    int i;

    //-- Negotiate operating condition for SD, it makes card ready state
    for(i=0;i<50;i++)   //If this time is short, init. can be fail.
    {
        cmd55();    // Make ACMD

        rSDICARG=0xff8000;  //ACMD41(SD OCR:2.7V~3.6V)
        rSDICCON=(0x1<<9)|(0x1<<8)|0x69;//sht_resp, wait_resp, start, ACMD41

        //-- Check end of ACMD41
        if( (chk_cmd_end(41, 1) & rSDIRSP0)==0x80ff8000 )
        {
            rSDICSTA=0xa00; // Clear cmd_end(with rsp)

            return 1;   // Success
        }
        Djy_DelayUs(200000); // Wait Card power up status
    }
    rSDICSTA=0xa00; // Clear cmd_end(with rsp)
    return 0;       // Fail
}
//---检测卡是否插入---------------------------------------------------
//功能：检测卡是否插入
//参数：无
//返回：1 成功，0 失败
//---------------------------------------------------------------------

int cmd55(void)
{
    //--Make ACMD
    rSDICARG=RCA<<16;           //CMD7(RCA,stuff bit)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x77;    //sht_resp, wait_resp, start, cmd55

    //-- Check end of cmd55
    if(!chk_cmd_end(55, 1))
        return 0;

    rSDICSTA=0xa00; // Clear cmd_end(with rsp)
    return 1;
}
//---获取卡内状态---------------------------------------------------
//功能：获取卡内状态
//参数：无
//返回：1 成功
//      0 失败
//---------------------------------------------------------------------

int cmd13(void)//SEND_STATUS
{
    int response0;

    rSDICARG=RCA<<16;           // cmd13(RCA,stuff bit)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x4d;    // sht_resp, wait_resp, start, cmd13

    //-- Check end of cmd13
    if(!chk_cmd_end(13, 1))
        return 0;
    //if(rSDIRSP0&0x100)
    response0=rSDIRSP0;
    response0 &= 0x3c00;
    response0 = response0 >> 9;
    if(response0==6)
        //Test_SDI();

    rSDICSTA=0xa00; // Clear cmd_end(with rsp)
    return 1;
}
//---获取卡的CSD寄存器的值---------------------------------------------------
//功能：获取卡的CSD寄存器的值
//参数：无
//返回：1，成功 0，失败
//---------------------------------------------------------------------

int cmd9(void)//SEND_CSD
{
    rSDICARG=RCA<<16;               // cmd9(RCA,stuff bit)
    rSDICCON=(0x1<<10)|(0x1<<9)|(0x1<<8)|0x49;  // long_resp, wait_resp, start, cmd9

    if(!chk_cmd_end(9, 1))
        return 0;
    else
        return 1;
}
//---设置卡通信宽度---------------------------------------------------
//功能：设置卡通信宽度为4bit
//参数：无
//返回：无
//---------------------------------------------------------------------

void set_4bit_bus(void)
{
    cmd55();    // Make ACMD
    //-- CMD6 implement
    rSDICARG=1<<1;       //Wide 0: 1bit, 1: 4bit
    rSDICCON=(0x1<<9)|(0x1<<8)|0x46;    //sht_resp, wait_resp, start, cmd55
    if(!chk_cmd_end(6, 1))   // ACMD6
        return;
    rSDICSTA=0xa00;     // Clear cmd_end(with rsp)
}
//----SD卡初始化--------------------------------------------------------
//功能：sd卡控制器初始化，使SD卡可以开始工作
//参数：无
//返回：1，成功
//      0，失败
//备注: MMC测试要注意，Cmd & Data 必须使能，上拉(不过，MMC我没测试过)
//---------------------------------------------------------------------

int module_init_SD(void)
{

    int i;

    RCA=0;
    MMC=0;
    //GPIO初始化
    rGPEUP  = 0xf83f;     // SDCMD, SDDAT[3:0] => PU En.
    rGPECON = 0xaaaaaaaa;   //SDCMD, SDDAT[3:0]
    //设置频率为400KHz
    rSDIPRE=CN_CFG_PCLK/(INICLK)-1;
    rSDICON=(1<<4)|1;   // Type B, clk enable
    rSDIFSTA=rSDIFSTA|(1<<16);  //YH 040223 FIFO reset
    rSDIBSIZE=0x200;        // 512byte(128word)
    rSDIDTIMER=0x7fffff;        // Set timeout count
    // Wait 74SDCLK for MMC card
    for(i=0;i<0x1000;i++);
    //发送命令0，使sd卡进入idle状态
    cmd0();

    //检查是否是MMC卡，并且设置目标工作电压
    //设置MMC，为了以后兼容MMC
    if(chk_MMC_OCR())
    {
        MMC=1;
        goto RECMD2;
    }
    //检查是否是SD卡，并且设置目标工作电压
    if(!chk_SD_OCR())
        return 0;
RECMD2:
    rSDICARG=0x0;   // CMD2(stuff bit)
    rSDICCON=(0x1<<10)|(0x1<<9)|(0x1<<8)|0x42; //lng_resp, wait_resp, start, CMD2
    //发送CMD2，获取SD卡身份信息，并且使卡进入identification模式
    if(!chk_cmd_end(2, 1))
        goto RECMD2;
    rSDICSTA=0xa00; // Clear cmd_end(with rsp)
RECMD3:
    //--Send RCA
    rSDICARG=MMC<<16;       // CMD3(MMC:Set RCA, SD:Ask RCA-->SBZ)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x43;    // sht_resp, wait_resp, start, CMD3
    //发送CMD3，SD卡获取RCA,进入stand_by状态
    if(!chk_cmd_end(3, 1))
        goto RECMD3;

    rSDICSTA=0xa00; // Clear cmd_end(with rsp)
    RCA=( rSDIRSP0 & 0xffff0000 )>>16;
    rSDIPRE=CN_CFG_PCLK/(SDCLK)-1;  // Normal clock=25MHz
    //--State(stand-by) check
    if( (rSDIRSP0 & 0x1e00) !=0x600 )  // CURRENT_STATE check
        goto RECMD3;
    card_sel_desel(1);  // Select
    set_4bit_bus();
    return 1;
}

void test_sdi()
{
    char buf[512] = "djyos test";
    char buf1[512];
    memset(buf1,0,512);
    write_one_sector(1, (unsigned int *)buf);
    read_one_sector(1, (unsigned int *)buf1);
}
