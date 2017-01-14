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
//所属模块：SPI FLASH模块
//作者：liq
//版本：V0.1.0
//文件描述：DJYOS的SPI FLASH AT45DB321D底层驱动
//其他说明：
//修订历史：
//1. 日期: 2014-4-15
//   作者：
//   新版本号：V0.1.0
//   修改说明：原始版本
//------------------------------------------------------

#include "stdint.h"
#include "string.h"
#include "cpu_peri.h"
#include "at45db321.h"
#include "spibus.h"
#include "board-config.h"

/*------------------------Time Symbol--------------------------------
Symbol  Parameter                               Typ     Max     Units
fSCK    SCK Frequency                                   66      MHz
fCAR1   SCK Frequency for Continuous Array Read         66      MHz
fCAR2   SCK Frequency for Continuous Array Read
        (Low Frequency)                                 33      MHz
tXFR    Page to Buffer Transfer Time                    200     μs
tcomp   Page to Buffer Compare Time                     200     μs
tEP     Page Erase and Programming Time         17      40      ms
tP      Page Programming Time                   3       6       ms
tPE     Page Erase Time                         15      35      ms
tBE     Block Erase Time                        45      100     ms
-------------------------------------------------------------------*/

//Read Commands
#define AT45_MainMem_Page_Read          0xD2
#define AT45_Continous_Array_Read_LC    0xE8        //(Legacy Command) up to 66M
#define AT45_Continous_Array_Read_LF    0x03        //(Low Frequency) up to 33M
#define AT45_Continous_Array_Read_HF    0x0B        //(High Frequency) up to 66M
#define AT45_Buff1_Read_LF              0xD1        //(Low Frequency)
#define AT45_Buff2_Read_LF              0xD3        //(Low Frequency)
#define AT45_Buff1_Read                 0xD4
#define AT45_Buff2_Read                 0xD6
//Program and Erase Commands
#define AT45_Buff1_Write                0x84
#define AT45_Buff2_Write                0x87
#define AT45_Buff1_to_MainMem_Page_Prog_vsErase     0x83    //Buffer 1 to Main Memory Page Program with Built-in Erase
#define AT45_Buff2_to_MainMem_Page_Prog_vsErase     0x86    //Buffer 2 to Main Memory Page Program with Built-in Erase
#define AT45_Buff1_to_MainMem_Page_Prog_noErase     0x88    //Buffer 1 to Main Memory Page Program without Built-in Erase
#define AT45_Buff2_to_MainMem_Page_Prog_noErase     0x89    //Buffer 2 to Main Memory Page Program without Built-in Erase
#define AT45_Page_Erase_Cmd             0x81
#define AT45_Block_Erase_Cmd            0x50
#define AT45_Sector_Erase               0x7C
#define AT45_Chip_Erase1                0xC7
#define AT45_Chip_Erase2                0x94
#define AT45_Chip_Erase3                0x80
#define AT45_Chip_Erase4                0x9A
#define AT45_MainMem_Page_Prog_Through_Buff1        0x82
#define AT45_MainMem_Page_Prog_Through_Buff2        0x85
//Additional Commands
#define AT45_MainMem_Page_to_Buff1_Transfer         0x53
#define AT45_MainMem_Page_to_Buff2_Transfer         0x55
#define AT45_MainMem_Page_to_Buff1_Compare          0x60
#define AT45_MainMem_Page_to_Buff2_Compare          0x61
#define AT45_Auto_Page_Rewrite_through_Buff1        0x58
#define AT45_Auto_Page_Rewrite_through_Buff2        0x59
#define AT45_Deep_Power_down                        0xB9
#define AT45_Resume_Power_down                      0xAB
#define AT45_Status_Register_Read                   0xD7
#define AT45_Command_ID                             0x9F    //Manufacturer and Device ID Read
//AT45_Status_Register bit define
#define AT45_Status_Reg_Bit_BUSY        0x80    //1=Ready   0=Busy
#define AT45_Status_Reg_Bit_COMP        0x40    //1=not match   0=match
#define AT45_Status_Reg_Bit_PRTC        0x02    //1=En protect  0=Dis protect
#define AT45_Status_Reg_Bit_PGSZ        0x01    //1=page size 512   0=page size 528

#ifndef AT45_Page_Size
//AT45_Page_Size默认值为528，如不使用默认值，请在config-prj.h中定义
#define AT45_Page_Size      528
#endif

#define AT45_Block_Size     AT45_Page_Size*8

//buff num define
#define AT45_Buff1  1
#define AT45_Buff2  2
//page erase define
#define vs_Erase    1
#define no_Erase    2

static struct SPI_Device s_AT45_Dev;
static u32 s_AT45_Timeout = CN_TIMEOUT_FOREVER;
#define AT45_SPI_SPEED      (10*1000*1000)
static bool_t sAT45Inited = false;

//Command指令缓存区
u8 _at45db321_Command[10]={0};
//SPI FLASH数据缓存区
u8 _at45db321_buff[AT45_Page_Size] = {0};
//当前可用于写数据的Buff,代表另一Buff有可能正处于向FLASH写数据的阶段
u8 _at45db321_Ready_Buff = AT45_Buff1;

static struct SemaphoreLCB AT45_Semp;   //芯片互斥访问保护

bool_t at45db321_Wait_Ready(u32 Time_Out);
/*---------------------test use only----------------------
#define test_buff_size  10240
section("seg_int_data") u32 Data_write[test_buff_size]={0};
section("seg_int_data") u32 Data_read[test_buff_size]={0};
u32 buff1_cnt=0,buff2_cnt=0;
---------------------test use only----------------------*/
void _at45db321_cs_active(void)
{
    SPI_CsActive(&s_AT45_Dev,s_AT45_Timeout);
}
void _at45db321_cs_inactive(void)
{
    SPI_CsInactive(&s_AT45_Dev);
    Djy_DelayUs(20);
}
u32 _at45db321_TxRx(u8* sdata,u32 slen,u8* rdata, u32 rlen)
{
    struct SPI_DataFrame data;
    s32 result;

    data.RecvBuf = rdata;
    data.RecvLen = rlen;
    data.RecvOff = slen;
    data.SendBuf = sdata;
    data.SendLen = slen;

    result = SPI_Transfer(&s_AT45_Dev,&data,true,s_AT45_Timeout);
    if(result != CN_SPI_EXIT_NOERR)
        return 0;
    return 1;
}

//note: 应用程序中的Address实际为虚拟地址，需要经过转换才能成为AT45中的实际地址
//      AT45用地址线A0-A9代表Page内的偏移地址，A10-A23代表Page地址，而因为AT45的Page大小是528bytes
//      所以用此表示的实际物理地址是不连续的，所以应用程序中使用的连续的虚拟地址需要经过变换方可成为实际物理地址

//----通过虚拟地址计算Page内偏移地址-------------------------------------------
//功能：通过虚拟地址计算Page内偏移地址
//      Address(23bit) = page_addr(13bit) + byte_offset_addr(10bit for 528)
//      1 page = 528 bytes
//参数：Address 虚拟地址
//返回：Page内偏移地址地址byte_offset_addr
//-----------------------------------------------------------------------------
u32 _at45db321_Offset_Caculate(u32 Address)
{
#if AT45_Page_Size==512
    return ( (Address%AT45_Page_Size)&0x1FF );
#elif AT45_Page_Size==528
    return ( (Address%AT45_Page_Size)&0x3FF );
#endif
}

//----通过虚拟地址计算Page地址------------------------------------------------
//功能：通过虚拟地址计算Page地址
//      Address(23bit) = page_addr(13bit) + byte_offset_addr(10bit for 528)
//      1 page = 528 bytes
//参数：Address 虚拟地址
//返回：Page地址
//-----------------------------------------------------------------------------
u32 _at45db321_Page_Caculate(u32 Address)
{
    return (u32)( Address/AT45_Page_Size );
}

//----通过Page地址计算Block地址------------------------------------------------
//功能：通过Page地址计算Block地址
//      page_addr = block_addr(10bit) + page_offset_addr(3bit)
//      1 block = 8 pages
//参数：page_addr   Page地址
//返回：Block地址
//-----------------------------------------------------------------------------
u32 _at45db321_Block_Caculate(u32 page_addr)
{
    return ( page_addr>>3 );
}

//----写入数据长度计算--------------------------------------------------------
//功能：通过比较Page的大小确定本次可以写入的数据长度
//参数：byte_offset_addr    Page内的偏移地址
//      data_len            期望写入的数据长度
//返回：本次实际能够写入的数据长度
//-----------------------------------------------------------------------------
u32 _at45db321_Written_Caculate(u32 byte_offset_addr,u32 data_len)
{
    u32 written_data_len;
    written_data_len = AT45_Page_Size-byte_offset_addr;
    if( written_data_len>data_len )
        written_data_len = data_len;
    return written_data_len;
}

//----SPI FLASH连续数据读取----------------------------------------------------
//功能：从AT45的地址Address处开始，连续读取data_len长度的数据，存入data
//      若地址累计到FLASH末尾则返回开头处继续读取
//参数：page_addr   Page地址
//      byte_offset_addr    Page内的偏移地址
//      *data       数据存储首地址
//      data_len    数据长度
//返回：实际读取的数据长度
//-----------------------------------------------------------------------------
u32 _at45db321_Continuous_Array_Read(u32 page_addr,u32 byte_offset_addr,u8 *data,u32 data_len)
{
    //array_addr
    _at45db321_Command[0] = AT45_Continous_Array_Read_HF;
#if AT45_Page_Size==512
    _at45db321_Command[1] = (page_addr>>7)&0xFF;
    _at45db321_Command[2] = ((page_addr<<1)|(byte_offset_addr>>8))&0xFF;
#elif AT45_Page_Size==528
    _at45db321_Command[1] = (page_addr>>6)&0xFF;
    _at45db321_Command[2] = ((page_addr<<2)|(byte_offset_addr>>8))&0xFF;
#endif
    _at45db321_Command[3] = byte_offset_addr & 0xFF;
    _at45db321_Command[4] = 0xFF;

    _at45db321_cs_active();

    _at45db321_TxRx(_at45db321_Command,5,data,data_len);

    _at45db321_cs_inactive();

    return data_len;
}

//----SPI FLASH写数据到缓存区--------------------------------------------------
//功能：SPI FLASH写数据到缓存区
//参数：buff_num    目标缓存区选择
//      buff_addr   缓存区内偏移地址
//      *data       数据获取首地址
//      data_len    数据长度
//返回：实际写入的数据长度
//-----------------------------------------------------------------------------
u32 _at45db321_Buff_Write(u32 buff_num,u32 buff_addr,u8 *data,u32 data_len)
{
    if(buff_num == AT45_Buff1)
        _at45db321_Command[0] = AT45_Buff1_Write;
    else if(buff_num == AT45_Buff2)
        _at45db321_Command[0] = AT45_Buff2_Write;
    else
    {
        return 0;
    }
    //buff_addr
    _at45db321_Command[1] = 0xFF;
    _at45db321_Command[2] = (buff_addr>>8)&0xFF;
    _at45db321_Command[3] = (buff_addr)&0xFF;

    _at45db321_cs_active();

    _at45db321_TxRx(_at45db321_Command,4,NULL,0);
    _at45db321_TxRx(data,data_len,NULL,0);

    _at45db321_cs_inactive();

    return data_len;
}

//----SPI FLASH读缓存区数据----------------------------------------------------
//功能：SPI FLASH读缓存区数据
//参数：buff_num    目标缓存区选择
//      buff_addr   缓存区内偏移地址
//      *data       数据接收首地址
//      data_len    数据长度
//返回：实际读取的数据长度
//-----------------------------------------------------------------------------
u32 _at45db321_Buff_Read(u32 buff_num,u32 buff_addr,u8 *data,u32 data_len)
{
    if(buff_num == AT45_Buff1)
        _at45db321_Command[0] = AT45_Buff1_Read;
    else if(buff_num == AT45_Buff2)
        _at45db321_Command[0] = AT45_Buff2_Read;
    else
    {
        return 0;
    }
    //buff_addr
    _at45db321_Command[1] = 0xFF;
    _at45db321_Command[2] = (buff_addr>>8)&0xFF;
    _at45db321_Command[3] = (buff_addr)&0xFF;
    _at45db321_Command[4] = 0xFF;

    _at45db321_cs_active();

    _at45db321_TxRx(_at45db321_Command,5,data,data_len);

    _at45db321_cs_inactive();

    return data_len;
}

//----SPI FLASH将Page数据读到缓存区--------------------------------------------
//功能：SPI FLASH将Page数据读到缓存区
//参数：buff_num    目标缓存区选择
//      page_addr   目标Page地址
//返回：false:  失败
//      true:   成功
//-----------------------------------------------------------------------------
bool_t _at45db321_Page_to_Buff(u32 buff_num,u32 page_addr)
{
    if(buff_num == AT45_Buff1)
        _at45db321_Command[0] = AT45_MainMem_Page_to_Buff1_Transfer;
    else if(buff_num == AT45_Buff2)
        _at45db321_Command[0] = AT45_MainMem_Page_to_Buff2_Transfer;
    else
    {
        return 0;
    }
#if AT45_Page_Size==512
    _at45db321_Command[1] = (page_addr>>7)&0xFF;
    _at45db321_Command[2] = (page_addr<<1)&0xFF;
#elif AT45_Page_Size==528
    _at45db321_Command[1] = (page_addr>>6)&0xFF;
    _at45db321_Command[2] = (page_addr<<2)&0xFF;
#endif
    _at45db321_Command[3] = 0xFF;

    _at45db321_cs_active();

    _at45db321_TxRx(_at45db321_Command,4,NULL,0);

    _at45db321_cs_inactive();

    return true;
}

//----SPI FLASH将缓存区数据写到Page--------------------------------------------
//功能：SPI FLASH将缓存区数据写到Page
//参数：buff_num    目标缓存区选择
//      page_addr   目标Page地址
//      With_Erase  写Page之前是否需要擦除
//返回：false:  失败
//      true:   成功
//-----------------------------------------------------------------------------
bool_t _at45db321_Buff_to_Page(u32 buff_num,u32 page_addr,u32 With_Erase)
{
    if(buff_num == AT45_Buff1)
    {
        if(With_Erase == no_Erase)
            _at45db321_Command[0] = AT45_Buff1_to_MainMem_Page_Prog_noErase;
        else if(With_Erase == vs_Erase)
            _at45db321_Command[0] = AT45_Buff1_to_MainMem_Page_Prog_vsErase;
        else
        {
            return 0;
        }
    }
    else if(buff_num == AT45_Buff2)
    {
        if(With_Erase == no_Erase)
            _at45db321_Command[0] = AT45_Buff2_to_MainMem_Page_Prog_noErase;
        else if(With_Erase == vs_Erase)
            _at45db321_Command[0] = AT45_Buff2_to_MainMem_Page_Prog_vsErase;
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }

#if AT45_Page_Size==512
    _at45db321_Command[1] = (page_addr>>7)&0xFF;
    _at45db321_Command[2] = (page_addr<<1)&0xFF;
#elif AT45_Page_Size==528
    _at45db321_Command[1] = (page_addr>>6)&0xFF;
    _at45db321_Command[2] = (page_addr<<2)&0xFF;
#endif
    _at45db321_Command[3] = 0xFF;

    _at45db321_cs_active();

    _at45db321_TxRx(_at45db321_Command,4,NULL,0);

    _at45db321_cs_inactive();

    return true;
}

//----SPI FLASH Page擦除需求判断-----------------------------------------------
//功能：判断FLASH中已有的数据与需要写入的数据段之间的匹配度，判断是否需要擦除
//      FLASH相应PAGE后再写入需要写入的数据已经写入Buff，Flash中读出的数据在_at45db321_buff中
//参数：*data       需要写入的数据的起始地址
//      data_len    需要写入的数据长度
//返回：false:  需要擦除
//      true:   不需要擦除
//-----------------------------------------------------------------------------
bool_t _at45db321_Need_Erase_orNot(u8 *data,u32 data_len)
{
    u32 i;

    for(i=0;i<data_len;i++)
    {
        if(_at45db321_buff[i] != 0xFF)
            return false;
    }

    return true;
}

//----SPI FLASH Page擦除-------------------------------------------------------
//功能：SPI FLASH Page擦除
//参数：无
//返回：无
//-----------------------------------------------------------------------------
bool_t AT45_Page_Erase(u32 Address)
{
    u32 page_addr;

    if(false == Lock_SempPend(&AT45_Semp,50*mS))
    {
        return false;
    }

    if(false == at45db321_Wait_Ready(500000))       //查忙，若超时则返回false
        return false;//超时，退出

    page_addr = _at45db321_Page_Caculate(Address);

    _at45db321_Command[0] = AT45_Page_Erase_Cmd;
#if AT45_Page_Size==512
    _at45db321_Command[1] = (page_addr>>7)&0xFF;
    _at45db321_Command[2] = (page_addr<<1)&0xFF;
#elif AT45_Page_Size==528
    _at45db321_Command[1] = (page_addr>>6)&0xFF;
    _at45db321_Command[2] = (page_addr<<2)&0xFF;
#endif
    _at45db321_Command[3] = 0xFF;

    _at45db321_cs_active();

    _at45db321_TxRx(_at45db321_Command,4,NULL,0);

    _at45db321_cs_inactive();

    Lock_SempPost(&AT45_Semp);
    return true;
}

//----SPI FLASH Block擦除------------------------------------------------------
//功能：SPI FLASH Block擦除
//参数：无
//返回：无
//-----------------------------------------------------------------------------
bool_t AT45_Block_Erase(u32 Address)
{
    u32 block_addr;
    if(false == Lock_SempPend(&AT45_Semp,100*mS))
    {
        return false;
    }

    if(false == at45db321_Wait_Ready(500000))       //查忙，若超时则返回false
        return false;//超时，退出

    block_addr = _at45db321_Page_Caculate(Address);
    block_addr = _at45db321_Block_Caculate(block_addr);

    _at45db321_Command[0] = AT45_Block_Erase_Cmd;
    _at45db321_Command[1] = (block_addr>>4)&0xFF;
    _at45db321_Command[2] = (block_addr<<4)&0xFF;
    _at45db321_Command[3] = 0xFF;

    _at45db321_cs_active();

    _at45db321_TxRx(_at45db321_Command,4,NULL,0);

    _at45db321_cs_inactive();

    Lock_SempPost(&AT45_Semp);

    return true;
}

//----SPI FLASH全片擦除--------------------------------------------------------
//功能：SPI FLASH全片擦除
//参数：无
//返回：无
//-----------------------------------------------------------------------------
bool_t AT45_Chip_Erase(void)
{
    if(false == Lock_SempPend(&AT45_Semp,200*mS))
    {
        return false;
    }

    _at45db321_Command[0] = AT45_Chip_Erase1;
    _at45db321_Command[1] = AT45_Chip_Erase2;
    _at45db321_Command[2] = AT45_Chip_Erase3;
    _at45db321_Command[3] = AT45_Chip_Erase4;

    if(false == at45db321_Wait_Ready(500000))       //查忙，若超时则返回false
        return false;//超时，退出

    _at45db321_cs_active();

    _at45db321_TxRx(_at45db321_Command,4,NULL,0);

    _at45db321_cs_inactive();

    Lock_SempPost(&AT45_Semp);

    return true;
}

//----SPI FLASH Page Size设置--------------------------------------------------
//功能：SPI FLASH Page Size设置为512。调用本函数后需要重新上电，否则操作FLASH会得到错误数据
//      注意：该过程不可逆！！AT45默认PageSize为528bytes，调用本函数可以修改为512
//参数：无
//返回：无
//-----------------------------------------------------------------------------
void _at45db321_Binary_Page_Size_512(void)
{
    _at45db321_Command[0] = 0x3D;
    _at45db321_Command[1] = 0x2A;
    _at45db321_Command[2] = 0x80;
    _at45db321_Command[3] = 0xA6;

    _at45db321_cs_active();

    _at45db321_TxRx(_at45db321_Command,4,NULL,0);

    _at45db321_cs_inactive();

//  while(1);
}

//----SPI FLASH读取Status Register---------------------------------------------
//功能：SPI FLASH读取Status Register值
//参数：无
//返回：Status Register值
//-----------------------------------------------------------------------------
u32 _at45db321_Read_Status(void)
{
    u8 Data;

    _at45db321_Command[0] = AT45_Status_Register_Read;

    _at45db321_cs_active();

    _at45db321_TxRx(_at45db321_Command,1,&Data,1);

    _at45db321_cs_inactive();

    return Data;
}

void _at45db321_Check_SP(void)
{
    u8 Data[64],i;

    if(false == at45db321_Wait_Ready(500000))       //查忙，若超时则返回false
        return ;//超时，退出

    _at45db321_Command[0] = 0x32;
    _at45db321_Command[1] = 0xFF;
    _at45db321_Command[2] = 0xFF;
    _at45db321_Command[3] = 0xFF;


    _at45db321_cs_active();
    _at45db321_TxRx(_at45db321_Command,4,Data,64);
    _at45db321_cs_inactive();

    for(i = 0; i < 64; i ++)
    {
        if(Data[i] != 0)
            break;
    }

    if(i != 64)
    {

        if(false == at45db321_Wait_Ready(500000))       //查忙，若超时则返回false
            return ;//超时，退出

        //erase sector protect
        _at45db321_Command[0] = 0x3D;
        _at45db321_Command[1] = 0x2A;
        _at45db321_Command[2] = 0x7F;
        _at45db321_Command[3] = 0xCF;



        _at45db321_cs_active();
        _at45db321_TxRx(_at45db321_Command,4,NULL,0);
        _at45db321_cs_inactive();

        //programe sector protect
        memset(Data,0x00,64);
        if(false == at45db321_Wait_Ready(500000))       //查忙，若超时则返回false
            return ;//超时，退出
        _at45db321_Command[0] = 0x3D;
        _at45db321_Command[1] = 0x2A;
        _at45db321_Command[2] = 0x7F;
        _at45db321_Command[3] = 0xFC;

        _at45db321_cs_active();
        _at45db321_TxRx(_at45db321_Command,4,NULL,0);
        _at45db321_TxRx(Data,64,NULL,0);
        _at45db321_cs_inactive();

        if(false == at45db321_Wait_Ready(500000))       //查忙，若超时则返回false
            return ;//超时，退出
        //check it
        _at45db321_Command[0] = 0x32;
        _at45db321_Command[1] = 0xFF;
        _at45db321_Command[2] = 0xFF;
        _at45db321_Command[3] = 0xFF;

        _at45db321_cs_active();
        _at45db321_TxRx(_at45db321_Command,4,Data,64);
        _at45db321_cs_inactive();
    }

}

void _at45db321_Check_Lock(void)
{
    u8 Data[64];

    _at45db321_Command[0] = AT45_Status_Register_Read;
    _at45db321_cs_active();
    _at45db321_TxRx(_at45db321_Command,1,Data,2);
    _at45db321_cs_inactive();

    if(Data[1] & (0x08))//means locked down
    {
        //then read it
        _at45db321_Command[0] = 0x35;
        _at45db321_Command[1] = 0xFF;
        _at45db321_Command[2] = 0xFF;
        _at45db321_Command[3] = 0xFF;


        _at45db321_cs_active();
        _at45db321_TxRx(_at45db321_Command,4,Data,64);
        _at45db321_cs_inactive();
    }
}
//----SPI FLASH校验芯片ID------------------------------------------------------
//功能：SPI FLASH校验芯片ID
//参数：无
//返回：true = 校验正常，false = 校验不成功
//-----------------------------------------------------------------------------
bool_t _at45db321_Check_ID(void)
{
    u8 Data[5];

    _at45db321_Command[0] = AT45_Command_ID;

    _at45db321_cs_active();

    _at45db321_TxRx(_at45db321_Command,1,Data,5);

    _at45db321_cs_inactive();

    if( (Data[0] != 0x1F) ||            //Manufacturer_ID
        (Data[1] != 0x27) ||            //Device_ID1
        (Data[2] != 0x01) ||            //Device_ID2
        (Data[3] != 0x01) ||
        (Data[4] != 0x00))              //Extended_Info
    {
        return false;
    }

    return true;    //Match SPI Flash ID successful
}

/*--------------------应用程序调用接口函数定义由此开始------------------------*/

//----SPI FLASH模块查忙--------------------------------------------------------
//功能：SPI FLASH模块查忙，读取AT45_Status，返回BUSY标志位的当前值
//参数：无
//返回：true = FLASH空闲，false = FLASH忙
//-----------------------------------------------------------------------------
bool_t at45db321_Check_Busy(void)
{
    u8 Data;

    Data = _at45db321_Read_Status();

    if( AT45_Status_Reg_Bit_BUSY != (AT45_Status_Reg_Bit_BUSY & Data) )
        return false;
    else
        return true;
}
//----等待FLASH模块空闲--------------------------------------------------------
//功能：等待FLASH模块空闲，函数内部判断是否超时
//参数：无
//返回：true = FLASH空闲，false = FLASH超时异常
//-----------------------------------------------------------------------------
bool_t at45db321_Wait_Ready(u32 Time_Out)
{
    u8 Data[2],result = true;


    _at45db321_Command[0] = AT45_Status_Register_Read;

    do
    {
        _at45db321_cs_active();
        _at45db321_TxRx(_at45db321_Command,1,Data,2);
        _at45db321_cs_inactive();

        Time_Out -= 2;
        Djy_DelayUs(2);
        if(Time_Out == 0)
        {
            result = false;
            break;
        }
    }while( AT45_Status_Reg_Bit_BUSY != (AT45_Status_Reg_Bit_BUSY & Data[0]) );

    return result;
}

//----SPI FLASH模块读函数------------------------------------------------------
//功能：从SPI FLASH的Address地址处开始，读出data_len长度的数据，数据存储首地址为data
//      若地址累加到FLASH的末尾，则会返回FLASH开头处继续读取，直到读满data_len个数据为止
//参数：Address     需要写入的地址（注意，此地址为应用程序所使用的虚拟地址，需要经过变换成为物理地址：page_addr和byte_offset_addr）
//      *data       读取数据存储位置的起始地址
//      data_len    读取数据长度
//返回：data_len = 实际读取的数据长度，false = 读取失败
//-----------------------------------------------------------------------------
u32 AT45_FLASH_Read(u32 Address,u8 *data,u32 data_len)
{
    u32 page_addr,byte_offset_addr;

    ////判断是否需要初始化SPI，以防module_init_at45db321未被调用
    if(!sAT45Inited)
    {
        return 0;
    }

    if(false == Lock_SempPend(&AT45_Semp,25*mS))
    {
        return 0;
    }

    page_addr=_at45db321_Page_Caculate(Address);
    byte_offset_addr=_at45db321_Offset_Caculate(Address);

    if(false == at45db321_Wait_Ready(500000))       //查忙，若超时则返回false
        return false;//超时，退出

    _at45db321_Continuous_Array_Read(page_addr,byte_offset_addr,data,data_len);

    Lock_SempPost(&AT45_Semp);

    return data_len;
}

//----SPI FLASH模块写函数------------------------------------------------------
//功能：从SPI FLASH的Address地址处开始，写入data_len长度的数据，数据获取首地址为data
//      本函数在写入连续的、数据长度为多个Page时可以使FLASH的效率达到最优。
//      应用程序无需关心FLASH是否需要擦除后再写入
//      注意：应用程序希望写入的数据长度为data_len，但本程序不保证实际写入data_len长度的数据，
//      实际写入的数据长度会通过written_data_len返回给应用程序（根据Page大小决定的）
//参数：Address     需要写入的地址（注意，此地址为应用程序所使用的虚拟地址）
//      *data       写入数据源的起始地址
//      data_len    写入数据长度
//返回：written_data_len = 实际写入的数据长度，false = 写入失败
//-----------------------------------------------------------------------------
u32 _at45_flash_write(u32 Address,u8 *data,u32 data_len)
{
    u32 written_data_len=0,Erase_orNot=vs_Erase;
    u32 page_addr,byte_offset_addr;

    //计算Page地址及数据在Page中的偏移地址
    page_addr = _at45db321_Page_Caculate(Address);
    byte_offset_addr = _at45db321_Offset_Caculate(Address);

    //确认本次可以写入的数据长度
    written_data_len = _at45db321_Written_Caculate(byte_offset_addr,data_len);

    //1.首先，将需要写入的数据写入处于空闲状态的Buff中
    if( false == at45db321_Check_Busy() )
    {//当前Buff正在执行写入FLASH的操作，换一块Buff写入数据
        if( _at45db321_Ready_Buff==AT45_Buff1 )
            _at45db321_Ready_Buff = AT45_Buff2;
        else
            _at45db321_Ready_Buff = AT45_Buff1;
    }
    _at45db321_Buff_Write(_at45db321_Ready_Buff,byte_offset_addr,data,written_data_len);

    if(false == at45db321_Wait_Ready(500000))       //查忙，若超时则返回false
        return false;//超时，退出

    //2.其次，从FLASH区中读出已经存在的数据,判断是否需要擦除
    _at45db321_Continuous_Array_Read(page_addr,byte_offset_addr,_at45db321_buff,written_data_len);//读出并存入_at45db321_buff
    if( false == _at45db321_Need_Erase_orNot(data,written_data_len))
    {//需要擦除
        Erase_orNot = vs_Erase;
    }
    else
    {//不需要擦除
        Erase_orNot = no_Erase;
    }

    //3.最后，完成写入操作
    //写入时，必须将flash中的数据读入到片内RAM，否则，将会将RAM中未确定的数据刷入到FLASH
//  if((( byte_offset_addr != 0 )||( written_data_len != AT45_Page_Size ))&&(Erase_orNot == vs_Erase))
    {//非整个Page的写入，擦除前需要保存
        _at45db321_Page_to_Buff(_at45db321_Ready_Buff,page_addr);
        if(false == at45db321_Wait_Ready(500000))       //查忙，若超时则返回false
            return false;//超时，退出
        _at45db321_Buff_Write(_at45db321_Ready_Buff,byte_offset_addr,data,written_data_len);
    }

    _at45db321_Buff_to_Page(_at45db321_Ready_Buff,page_addr,Erase_orNot);
    //此处，不需要等待完成。在进行下一轮FLASH操作时再判断

    return written_data_len;
}

// =============================================================================
// 功能：写flash，分 页写,将data_len的数据写入到Address的起始地址
// 参数：Address，起始地址
//      data，数据指针
//      data_len，数据长度，字节数
// 返回：实际写入的数据长度
// =============================================================================
u32 AT45_FLASH_Write(u32 Address,u8 *data,u32 data_len)
{
    u32 wsize,temp;
    ////判断是否需要初始化SPI，以防module_init_at45db321未被调用
    if(!sAT45Inited)
    {
        return false;
    }
    if(false == Lock_SempPend(&AT45_Semp,25*mS))
    {
        return false;
    }
    temp = data_len;
    while(temp)
    {
        wsize = _at45_flash_write(Address,data,temp);
        if(!wsize)
            break;
        Address += wsize;
        data    += wsize;
        temp -= wsize;
    }
    Lock_SempPost(&AT45_Semp);
    return data_len - temp;
}

// =============================================================================
// 功能：返回AT45DB321是否已经初始化
// 参数：无
// 返回：初始化状态
// =============================================================================
bool_t AT45_FLASH_Ready(void)
{
    return sAT45Inited;
}
//----初始化SPI FLASH模块------------------------------------------------------
//功能：初始化SPI FLASH模块，校验芯片ID是否正确
//参数：模块初始化函数没有参数
//返回：true = 成功初始化，false = 初始化失败
//-----------------------------------------------------------------------------
//#define test_buff_size  2048
//u8 Data_write[test_buff_size];
//u8 Data_read[test_buff_size];
bool_t ModuleInstall_at45db321(void)
{
    if(NULL == Lock_SempCreate_s(&AT45_Semp,1,1,CN_BLOCK_FIFO,"AT45 semp"))
        return false;

    /* setup baud rate */
    s_AT45_Dev.AutoCs = false;
    s_AT45_Dev.CharLen = 8;
    s_AT45_Dev.Cs = 0;
    s_AT45_Dev.Freq = AT45_SPI_SPEED;
    s_AT45_Dev.Mode = SPI_MODE_0;
    s_AT45_Dev.ShiftDir = SPI_SHIFT_MSB;

    if(NULL != SPI_DevAdd_s(&s_AT45_Dev,"QSPI","AT45DB321E"))
    {
        SPI_BusCtrl(&s_AT45_Dev,CN_SPI_SET_POLL,0,0);
    }

    if(false == _at45db321_Check_ID())  //校验芯片ID
        return false;

    if( !(_at45db321_Read_Status() & AT45_Status_Reg_Bit_PGSZ) )//转换成512字节
    {
        _at45db321_Binary_Page_Size_512();//不可逆，且需重启
    }

    sAT45Inited = true;
    return sAT45Inited;

/*---------------------test use only----------------------
    for(i=0;i<test_buff_size;i++) Data_write[i]=i&0xff;

    SPI_FLASH_Read(0,Data_read,test_buff_size);

    j=test_buff_size;
    k=0;
    while(j!=0)
    {
        result = SPI_FLASH_Write(k,Data_write+k,j);
        j = j - result;
        k = k + result;
    }

    SPI_FLASH_Read(0,Data_read,test_buff_size);

    for(i=0;i<test_buff_size;i++)
    {
        if(Data_write[i]!=Data_read[i])
        {
            printk("at45 test error!\r\n");
            break;
        }
    }
    printk("at45 test finished!\r\n");
    return true;
/---------------------test use only----------------------*/
}

