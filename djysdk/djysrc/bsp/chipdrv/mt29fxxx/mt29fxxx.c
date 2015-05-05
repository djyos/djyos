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
// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：MT29Fxxx.c
// 模块描述: 基于DJYOS文件系统设计的NANDFALSH驱动调用了NFC模块的接口函数
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 20/10.2014
// =============================================================================

#include "stdint.h"
#include "verify.h"
#include "file.h"
#include "flashfile.h"
#include "cpu_peri.h"
#include <string.h>
#include "MT29Fxxx.h"


//扇区缓冲区指针，扇区读写函数内部专用，djyffs中，扇区不是所有flash的共性，故不
//出现在struct flash_chip 结构中。动态分配内存而不定义成静态数组，是因为chip作
//为一个资源，是可以删除的，删除时可以回收资源。
static u8 *pg_sector_buf;      //扇区缓冲区指针
static u32 u32g_sectors_per_block; //每块包含的扇区数
static u32 u32g_sector_size;      //扇区数尺寸
static u16 u16g_oob_size;

static  struct tagFlashChip tg_micron_nand;    //芯片

struct nand_table tg_nand_table[] =
{
    {0x9876,16,32,4096,16384,"Toshiba TH58512FT,1.8v,64Mbytes"},
    {0xec36,16,32,4096,16384,"samsung k9f1208,1.8v,64Mbytes"},
    {0xec76,16,32,4096,16384,"samsung k9f1208,3.3v,64Mbytes"},
    {0xec73,16,32,1024,16384,"samsung k9f2808,3.3v,16Mbytes"},
    {0xec33,16,32,1024,16384,"samsung k9f2808,1.8v,16Mbytes"},
    {0xecda,64,64,2048,131072,"samsung K9F2G08,3.3v,256Mbytes"},
    {0x2CCA,64,64,2048,131072,"Micron MT29F2G16,1.8V,256Mbytes"}
};
//----写入命令-----------------------------------------------------------------
//功能: 写入芯片写命令字
//参数: val，写命令字，
//返回: 无
//-----------------------------------------------------------------------------
#define __write_command_nand(cmd)   //pg_nand_reg->NFCMD = cmd

//----写入地址-----------------------------------------------------------------
//功能: 写入芯片写命令字,5行分别写入A0 ~ A7，A9 ~ A11，A12 ~ A19,A20~A27
//参数: val，写命令字，
//返回: 无
//----------------------------------------------------------------------------
void __AddrToRowCol(u32 Addr,u32* RowAddr,u32 *ColAddr)
{
    *ColAddr = Addr & 0x7FF;
    *RowAddr = (Addr >> 11) & 0x1FFFF;
}

//----读扇区(含oob)------------------------------------------------------------
//功能: 读一个扇区，含oob中的内容一起读
//参数: sector，被读的扇区号
//      data，读出数据的缓冲区
//返回: 无
//-----------------------------------------------------------------------------
void __read_sector_and_oob(u32 sector,u8 *data)
{
//    u32 i;
    u32 address,RowAddr,ColAddr;
    address =u32g_sector_size*sector;     //计算实际地址

    __AddrToRowCol(address,&RowAddr,&ColAddr);
    NFC_CsActive(NFC_CE0);
    NFC_PageRead(BufferNo,RowAddr,ColAddr);
    __wait_ready_nand( );

    NFC_ReadBuffer(data,0,2048+64);
    NFC_CsInactive(NFC_CE0);
    return ;
}
//----写掉电恢复块-------------------------------------------------------------
//功能: 当芯片正在写入数据时掉电或者复位，将使正在写入的块数据丢失，为在这种情况
//      下保护数据，采用的策略是:
//      1.如果写入的是新块，则直接写入，因为写入新块失败和写入掉电恢复块失败的
//        后果是一样的。
//      2.如果是修改一块或者从一块后面添加数据，则把数据先写入到掉电恢复块，并
//        在掉电恢复块的适当位置标明掉电恢复块保护的是哪一块数据。
//      3.写入掉电恢复块后，接着把数据写入到目标块中，完成后，擦除掉电恢复块。
//      4.重新启动后，如果检测到掉电恢复块有有效数据，则恢复到目标块
//      本函数的职责:第三种条件下，调用本函数把数据写入掉电恢复块。
//参数: PCRB_block,用于保存掉电恢复数据的块号，芯片的绝对块号
//      protected_block，被保护的目标块号，芯片的绝对块号。
//返回: true = 成功写入，false = 写入失败，可能第PCRB_block块是坏块。
//-----------------------------------------------------------------------------
bool_t write_PCRB_nand(u32 PCRB_block,
                       u32 protected_block,u8 *buf)
{
    u32 cur_sector;    //当前正在写的扇区
    u32 completed = 0;
    u32 verify;
    u32 address,RowAddr,ColAddr;
    u32 loop;

    if(PCRB_block >= tg_micron_nand.block_sum)
        return false;
    cur_sector = PCRB_block * u32g_sectors_per_block;
    for(loop = 0; loop < u32g_sectors_per_block; loop++)
    {
        verify = __write_sector_nand_with_ecc(cur_sector,0,
                                  buf+completed,u32g_sector_size);
        if((verify == cn_all_right_verify) || (verify == cn_ecc_right_verify))
        {
            completed += u32g_sector_size;
        }else
            return false;
        cur_sector++;
    }
    //第一页的最后4字节保存受保护的目标块号。
    address =tg_micron_nand.block_size*PCRB_block +
            (u32g_sector_size + u16g_oob_size-4)/2;//地址线是16比特//todo

    //受保护的目标块号写入flash，注意:没有做ECC校验，以后补上---db
    __AddrToRowCol(address,&RowAddr,&ColAddr);
    NFC_WriteBuffer((u8*)&protected_block,0,4);

    NFC_CsActive(NFC_CE0);
    NFC_PageProgram(BufferNo,RowAddr,ColAddr);
    __wait_ready_nand_slow(CN_WAIT_PAGE_WRITE);

    if(__read_status_nand() & cn_nand_failure)
    {
        NFC_CsInactive(NFC_CE0);
        return false;
    }
    NFC_CsInactive(NFC_CE0);

    return true;
}

//----从掉电恢复块恢复数据-----------------------------------------------------
//功能: 从掉电恢复块恢复数据，如果掉电恢复块里有有效数据，比较其与目标块的数据是
//      否一致，如果不一致则用掉电恢复块的数据覆盖目标块。无论是否需要恢复，最后
//      均擦除掉电恢复块。
//参数: PCRB_block,掉电恢复块块号，芯片的绝对块号
//      restored，本指针返回被恢复的目标块
//返回: true = 无需恢复或者正确恢复，false = 发生错误，一般是因为目标块是坏块。
//-----------------------------------------------------------------------------
bool_t restore_PCRB_nand(u32 PCRB_block,u32 *restored)
{
    u32 verify;
    u32 address,RowAddr,ColAddr;
    u32 loop;
    u32 protected_block;
    u32 sector_from,sector_to;
    u8 *sector_buf;   //扇区缓冲区，动态分配，不在栈中分配
    bool_t result = true;

    if(PCRB_block >= tg_micron_nand.block_sum)
        return false;
    //第一页的最后4字节保存受保护的目标块号。
    address =tg_micron_nand.block_size*PCRB_block +
            (u32g_sector_size + u16g_oob_size-4)/2;//地址线是16位的

    __AddrToRowCol(address,&RowAddr,&ColAddr);
    NFC_CsActive(NFC_CE0);
    NFC_PageRead(BufferNo,RowAddr,ColAddr);
    __wait_ready_nand( );

    NFC_ReadBuffer((u8*)&protected_block,0,4);

    *restored = protected_block;

    if(protected_block > tg_micron_nand.block_sum)
    {
        erase_block_nand(PCRB_block);
        return true;
    }
    sector_buf = M_MallocLc(u32g_sector_size,0);
    if(sector_buf == NULL)
        return false;
    erase_block_nand(protected_block);
    sector_from = PCRB_block * u32g_sectors_per_block;
    sector_to = protected_block * u32g_sectors_per_block;
    for(loop = 0; loop < u32g_sectors_per_block; loop++)
    {
        verify = __read_sector_nand_with_ecc(
                        sector_from,0,sector_buf,u32g_sector_size);
        if((verify != cn_all_right_verify) && (verify != cn_ecc_right_verify))
        {
            result = false;
            break;
        }
        verify = __write_sector_nand_with_ecc(
                                    sector_to,0,sector_buf,u32g_sector_size);
        if((verify != cn_all_right_verify) && (verify != cn_ecc_right_verify))
        {
            result = false;
            break;
        }
        sector_from++;
        sector_to++;
    }
    free(sector_buf);
    erase_block_nand(PCRB_block);
    NFC_CsInactive(NFC_CE0);
    return result;
}

//----等待芯片内部完成操作----------------------------------------------------
//功能: 对芯片执行写操作后，要效用本函数等待操作完成才能进一步操作。
//参数: 无
//返回: true = 正确完成操作，false = 发生错误
//-----------------------------------------------------------------------------
bool_t __wait_ready_nand(void)
{
    while(!NFC_OptionCompleted());
    return true;
}

//----等待芯片内部慢速操作完成-------------------------------------------------
//功能: 对芯片执行写操作后，要调用本函数等待操作完成才能进一步操作。由于一连串的
//      同步操作将浪费很多时间，对于一些很快就绪的操作，宜使用__wait_ready_nand
//      函数。
//参数: wait_time，估计等待时间，微秒数
//返回: true = 正确完成操作，false = 发生错误
//-----------------------------------------------------------------------------
bool_t __wait_ready_nand_slow(uint16_t wait_time)
{
//    timer_set_counter(1,wait_time);     //计数值设为wait_time
//    timer_reload(1);                    //重载定时值
//    timer_start(1);                     //启动定时器
//    int_asyn_signal_sync(cn_int_line_timer1);
//    timer_stop(1);
//    while((pg_nand_reg->NFSTAT & 1) ==0 );

    u32 timestart,timeend;

    timestart = (u32)DjyGetTime( );
    timeend = timestart;

    while ( (timeend - timestart) < wait_time)
    {
      if(NFC_OptionCompleted())
       {
          break;
       }
      timeend = (u32)DjyGetTime( );
    }

    if( (timeend - timestart) >= wait_time)
    {
        return false;
    }
    else
    {
        return true;
    }
}

//----读扇区(无ecc校验)----------------------------------------------------
//功能: 不带ecc校验从一扇区内读取数据，地址不能跨扇区边界
//参数: sector，扇区号
//      offset，扇区内偏移地址
//      data，保存读取数据的缓冲区
//      size，读取的尺寸
//返回: 正确读取
//-----------------------------------------------------------------------------
u32 __read_sector_nand_no_ecc(u32 sector,u32 offset,
                                 u8 *data,u32 size)
{
//    u32 i;
    u32 address,RowAddr,ColAddr;
    address =u32g_sector_size*sector;     //计算实际地址
    //wjz code revision 256M的，比64M的，多了个0x30命令。由于这个忘记等待时间了
    //data[i],前面一些数据，都是随机数据.
    NFC_CsActive(NFC_CE0);
    __AddrToRowCol(address,&RowAddr,&ColAddr);
    NFC_PageRead(BufferNo,RowAddr,ColAddr);
    __wait_ready_nand( );

    NFC_ReadBuffer(data,offset,size);
    NFC_CsInactive(NFC_CE0);
    return cn_all_right_verify;
}

//----读块(无ecc校验)----------------------------------------------------
//功能: 不带ecc校验从一块内读取数据，地址不能跨块边界
//参数: sector，扇区号
//      offset，块内偏移地址
//      data，保存读取数据的缓冲区
//      size，读取的尺寸
//返回: 正确读取
//-----------------------------------------------------------------------------
u32 read_block_ss_no_ecc(u32 block,u32 offset,
                         u8 *buf,u32 size)
{
    u32 start_sector;  //首扇区号
    u32 start_offset;  //首地址在首扇区的偏移量
    u32 end_sector;    //结束地址所在扇区
    u32 end_offset;    //结束地址在扇区中的偏移量
    u32 cur_sector;    //当前正在读的扇区
    u32 read_size;     //从当前扇区读取的数据量
    u32 completed = 0;

    if(block >= tg_micron_nand.block_sum)
        return CN_LIMIT_UINT32;
    if((size + offset) > tg_micron_nand.block_size)
        return CN_LIMIT_UINT32;
    if(size == 0)
        return 0;

    //起始扇区号
    start_sector = offset / u32g_sector_size + u32g_sectors_per_block * block;
    //起始地址在起始扇区号中的偏移量
    start_offset = offset % u32g_sector_size;
    //结束扇区号
    end_sector =(offset + size-1)/u32g_sector_size+u32g_sectors_per_block*block;
    //结束地址在结束扇区中的偏移量
    end_offset = (offset + size -1) % u32g_sector_size;
    for(cur_sector = start_sector; cur_sector <= end_sector; cur_sector++)
    {
        if(cur_sector != end_sector)    //当前扇区不是最后一个扇区
            read_size = u32g_sector_size - start_offset;
        else    //当前扇区是最后一个扇区
            //+1是因为end_offset本身是需要写入的
            read_size = end_offset - start_offset +1;
        __read_sector_nand_no_ecc(cur_sector,start_offset,
                                  buf+completed,read_size);
        completed += read_size;
        start_offset = 0;   //从第二个扇区开始，肯定从0开始读
    }
    return completed;
}

//----ECC检查并改正-----------------------------------------------------------
//功能: ecc校验一个扇区
//参数: data，待校验数据的缓冲区
//      ecc，校验码
//返回：0=正确，1=被改正，2=错误且不能修正
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
u32 __correct_sector(u8 *data,const u8 *ecc)
{
    s32 j;
    u32 i,result = cn_all_right_verify;
    for(i = 0; i < u32g_sector_size/256; i++)
    {
        j = Ecc_Corect256(data + i*256, ecc + i*3);
        if(( j == -1) || (j == 2))
            return cn_ecc_error_verify;   //无法修正则直接返回
        if( j == 1)
            result = cn_ecc_right_verify;
    }
    return result;
}

//----读扇区(带ecc校验)----------------------------------------------------
//功能: 带ecc校验从一扇区内读取数据，地址不能跨扇区边界
//参数: sector，扇区号
//      offset，扇区内偏移地址
//      data，保存读取数据的缓冲区
//      size，读取的尺寸
//返回：0=正确，1=被改正，2=错误且不能修正
//-----------------------------------------------------------------------------
u32 __read_sector_nand_with_ecc(u32 sector,u32 offset,
                                     u8 *data,u32 size)
{
    u32 address,result,RowAddr,ColAddr;
    u8 *ecc;
    address =u32g_sector_size*sector;     //计算实际地址
    ecc = pg_sector_buf+u32g_sector_size;

    NFC_CsActive(NFC_CE0);
    __AddrToRowCol(address,&RowAddr,&ColAddr);
    NFC_PageRead(BufferNo,RowAddr,ColAddr);
    __wait_ready_nand( );

    NFC_ReadBuffer(pg_sector_buf,0,u32g_sector_size);

   if(u32g_sector_size > 256)
    {
       //OOB的第一个字节为坏块标记，因此不读
       NFC_ReadBuffer(ecc,u32g_sector_size+1,u32g_sector_size/256*3);
    }
    else
    {
        //读取校验码，只有3字节
        NFC_ReadBuffer(ecc,u32g_sector_size,3);
    }

   NFC_CsInactive(NFC_CE0);
   result = __correct_sector(pg_sector_buf,ecc);
    //无论校验结果如何，均执行数据copy，即使错误，也把错误数据告诉用户，让人知道
    //错在哪里。
    memcpy(data, pg_sector_buf + offset, size);
    return result;
}

//----读块(带ecc校验)----------------------------------------------------
//功能: 带ecc校验从一块内读取数据，地址不能跨块边界
//参数: sector，块号
//      offset，块内偏移地址
//      data，保存读取数据的缓冲区
//      size，读取的尺寸
//返回：实际读取的数据量
//-----------------------------------------------------------------------------
u32 read_block_ss_with_ecc(u32 block,u32 offset,
                         u8 *buf,u32 size)
{
    u32 start_sector;  //首扇区号
    u32 start_offset;  //首地址在首扇区的偏移量
    u32 end_sector;    //结束地址所在扇区
    u32 end_offset;    //结束地址在扇区中的偏移量
    u32 cur_sector;    //当前正在读的扇区
    u32 read_size;     //从当前扇区读取的数据量
    u32 completed = 0;
    u32 verify;

    if(block >= tg_micron_nand.block_sum)
        return CN_LIMIT_UINT32;
    if((size + offset) > tg_micron_nand.block_size)
        return CN_LIMIT_UINT32;
    if(size == 0)
        return 0;

    //起始扇区号
    start_sector = offset / u32g_sector_size + u32g_sectors_per_block * block;
    //起始地址在起始扇区号中的偏移量
    start_offset = offset % u32g_sector_size;
    //结束扇区号
    end_sector =(offset + size-1)/u32g_sector_size+u32g_sectors_per_block*block;
    //结束地址在结束扇区中的偏移量
    end_offset = (offset + size -1) % u32g_sector_size;
    for(cur_sector = start_sector; cur_sector <= end_sector; cur_sector++)
    {
        if(cur_sector != end_sector)    //当前扇区不是最后一个扇区
            read_size = u32g_sector_size - start_offset;
        else    //当前扇区是最后一个扇区
            //+1是因为end_offset本身是需要写入的
            read_size = end_offset - start_offset +1;
        verify = __read_sector_nand_with_ecc(cur_sector,start_offset,
                                  buf+completed,read_size);
        if((verify == cn_all_right_verify) || (verify == cn_ecc_right_verify))
        {
            completed += read_size;
            start_offset = 0;   //从第二个扇区开始，肯定从0开始读
        }else
            break;
    }
    return completed;
}

//----读芯片状态-------------------------------------------------------------
//功能: 读芯片状态
//参数: 无
//返回：芯片状态
//-----------------------------------------------------------------------------
u8 __read_status_nand(void)
{
    u8 chip_status ;

    chip_status = NFC_FlashStatusRead();
    return chip_status;
}

//----写一个扇区(no ecc)-------------------------------------------------------------
//功能: 写一个扇区，不做ecc校验
//参数: sector，扇区号
//      offset，扇区内偏移地址
//      data，数据缓冲区
//      size，写入的尺寸
//返回：cn_all_right_verify=正确写入，cn_ecc_error_verify=写入错误
//-----------------------------------------------------------------------------
u32 __write_sector_nand_no_ecc(u32 sector,u32 offset,
                                  u8 *data,u32 size)
{
    u32 address,RowAddr,ColAddr;
    address =u32g_sector_size*sector;     //计算实际地址

    if(offset != 0)
    {
        __read_sector_nand_with_ecc(sector,0,pg_sector_buf,u32g_sector_size);
        memcpy(pg_sector_buf+offset,data,size);
        NFC_WriteBuffer(pg_sector_buf,0,u32g_sector_size);
    }
    else
    {
        memset(pg_sector_buf,0xFF,u32g_sector_size);
        memcpy(pg_sector_buf,data,size);
        NFC_WriteBuffer(pg_sector_buf,0,u32g_sector_size);
    }

    __AddrToRowCol(address,&RowAddr,&ColAddr);

    NFC_CsActive(NFC_CE0);
    NFC_PageProgram(BufferNo,RowAddr,ColAddr);
    __wait_ready_nand_slow(CN_WAIT_PAGE_WRITE);

    if(__read_status_nand() & cn_nand_failure)
    {
        NFC_CsInactive(NFC_CE0);
        return cn_ecc_error_verify;
    }
    NFC_CsInactive(NFC_CE0);
    return cn_all_right_verify;
}

//----写块(不带ecc校验)----------------------------------------------------
//功能: 不带ecc校验把缓冲区写入块内，地址不能跨块边界
//参数: sector，块号
//      offset，块内偏移地址
//      data，保存读取数据的缓冲区
//      size，读取的尺寸
//返回：实际写入的数据量，CN_LIMIT_UINT32表示出错
//-----------------------------------------------------------------------------
u32 write_block_ss_no_ecc(u32 block,u32 offset,
                               u8 *buf,u32 size)
{
    u32 start_sector;  //首扇区号
    u32 start_offset;  //首地址在首扇区的偏移量
    u32 end_sector;    //结束地址所在扇区
    u32 end_offset;    //结束地址在扇区中的偏移量
    u32 cur_sector;    //当前正在读的扇区
    u32 write_size;    //从当前扇区读取的数据量
    u32 completed = 0;
    u32 verify;

    if(block >= tg_micron_nand.block_sum)
        return CN_LIMIT_UINT32;
    if((size + offset) > tg_micron_nand.block_size)
        return CN_LIMIT_UINT32;

    //起始扇区号
    start_sector = offset / u32g_sector_size + u32g_sectors_per_block * block;
    //起始地址在起始扇区号中的偏移量
    start_offset = offset % u32g_sector_size;
    //结束扇区号
    end_sector =(offset + size-1)/u32g_sector_size+u32g_sectors_per_block*block;
    //结束地址在结束扇区中的偏移量
    end_offset = (offset + size -1) % u32g_sector_size;
    for(cur_sector = start_sector; cur_sector <= end_sector; cur_sector++)
    {
        if(cur_sector != end_sector)    //当前扇区不是最后一个扇区
            write_size = u32g_sector_size - start_offset;
        else    //当前扇区是最后一个扇区
            //+1是因为end_offset本身是需要写入的
            write_size = end_offset - start_offset +1;
        verify = __write_sector_nand_no_ecc(cur_sector,start_offset,
                                  buf+completed,write_size);
        if((verify == cn_all_right_verify) || (verify == cn_ecc_right_verify))
        {
            completed += write_size;
            start_offset = 0;   //从第二个扇区开始，肯定从0开始读
        }else
            break;
    }
    return completed;
}


//----写一个扇区(with ecc)-------------------------------------------------------------
//功能: 写一个扇区，带ecc校验
//参数: sector，扇区号
//      offset，扇区内偏移地址
//      data，数据缓冲区
//      size，写入的尺寸
//返回：cn_all_right_verify=正确写入，cn_ecc_error_verify=写入错误
//-----------------------------------------------------------------------------
u32 __write_sector_nand_with_ecc(u32 sector,u32 offset,
                                      u8 *data,u32 size)
{
    u32 i;
    u32 address,RowAddr,ColAddr;
    u8 *ecc,temp;
    if((offset != 0) || (size != u32g_sector_size))
    {
        __read_sector_nand_with_ecc(sector,0,pg_sector_buf,u32g_sector_size);
        //执行ECC校验，但是不判断校验结果，因为扇区写入前可能是随机数据，校验错
        //误并不能说明发生了错误
    }
    else
        memset(pg_sector_buf,0xFF,u32g_sector_size+u16g_oob_size);

    ecc = pg_sector_buf+u32g_sector_size;
    memcpy(pg_sector_buf+offset,data,size);
    __make_sector_ecc(pg_sector_buf, ecc);               //计算ecc代码

    address=u32g_sector_size*sector;  //写入起始地址

    NFC_WriteBuffer(pg_sector_buf,0,u32g_sector_size);//从缓冲区中读出数据
    if(u32g_sector_size > 256)
    {
        temp = 0xFF;                                //写0xFF不会造成flash修改
        NFC_WriteBuffer(&temp,u32g_sector_size,1);
        NFC_WriteBuffer(ecc,u32g_sector_size+1,u32g_sector_size/256*3);//写ECC
        i = u32g_sector_size/256*3 + 1;
        for(; i < 64; i ++)                         //64字节中剩下的写0xFF
            NFC_WriteBuffer(&temp,u32g_sector_size+i,1);
    }
    else
    {
        NFC_WriteBuffer(ecc,u32g_sector_size,3);
    }

    NFC_CsActive(NFC_CE0);
    __AddrToRowCol(address,&RowAddr,&ColAddr);
    NFC_PageProgram(BufferNo,RowAddr,ColAddr);
    __wait_ready_nand_slow(CN_WAIT_PAGE_WRITE);

    if(__read_status_nand() & cn_nand_failure)      //判断是否读成功
    {
        NFC_CsInactive(NFC_CE0);
        return cn_ecc_error_verify;
    }
    NFC_CsInactive(NFC_CE0);
    return cn_all_right_verify;
}

//----写块(带ecc校验)----------------------------------------------------
//功能: 带ecc校验把缓冲区写入块内，地址不能跨块边界
//参数: sector，块号
//      offset，块内偏移地址
//      data，保存读取数据的缓冲区
//      size，读取的尺寸
//返回：实际写入的数据量，CN_LIMIT_UINT32表示出错
//-----------------------------------------------------------------------------
u32 write_block_ss_with_ecc(u32 block,u32 offset,
                                 u8 *buf,u32 size)
{
    u32 start_sector;  //首扇区号
    u32 start_offset;  //首地址在首扇区的偏移量
    u32 end_sector;    //结束地址所在扇区
    u32 end_offset;    //结束地址在扇区中的偏移量
    u32 cur_sector;    //当前正在写的扇区
    u32 write_size;     //从当前扇区写入的数据量
    u32 completed = 0;
    u32 verify;

    if(block >= tg_micron_nand.block_sum)
        return CN_LIMIT_UINT32;
    if((size + offset) > tg_micron_nand.block_size)
        return CN_LIMIT_UINT32;

    //起始扇区号
    start_sector = offset / u32g_sector_size + u32g_sectors_per_block * block;
    //起始地址在起始扇区号中的偏移量
    start_offset = offset % u32g_sector_size;
    //结束扇区号
    end_sector =(offset + size-1)/u32g_sector_size+u32g_sectors_per_block*block;
    //结束地址在结束扇区中的偏移量
    end_offset = (offset + size -1) % u32g_sector_size;
    for(cur_sector = start_sector; cur_sector <= end_sector; cur_sector++)
    {
        if(cur_sector != end_sector)    //当前扇区不是最后一个扇区
            write_size = u32g_sector_size - start_offset;
        else    //当前扇区是最后一个扇区
            //+1是因为end_offset本身是需要写入的
            write_size = end_offset - start_offset +1;
        verify = __write_sector_nand_with_ecc(cur_sector,start_offset,
                                  buf+completed,write_size);
        if((verify == cn_all_right_verify) || (verify == cn_ecc_right_verify))
        {
            completed += write_size;
            start_offset = 0;   //从第二个扇区开始，肯定从0开始写
        }else
            break;
    }
    return completed;
}

//----擦除一块-------------------------------------------------------------
//功能: 擦除一块
//参数: block_no，目标块号
//返回: true = 成功擦除，false = 坏块
//-----------------------------------------------------------------------------
bool_t erase_block_nand(u32 block_no)
{
    u32 page_no;
    page_no = block_no * u32g_sectors_per_block;

    NFC_CsActive(NFC_CE0);
    // 行地址，因为行地址里面包含了block号和page号，芯片会省略page号
    NFC_BlockErase(page_no);

    __wait_ready_nand_slow(CN_WAIT_BLOCK_ERASE);
    NFC_CsInactive(NFC_CE0);
    return true;
}

//----擦除整个芯片-------------------------------------------------------------
//功能: 擦除整个芯片。
//参数: 无
//返回: true = 成功擦除，false = 坏块
//-----------------------------------------------------------------------------
bool_t __erase_all_nand(void)
{
    u32 i;
    for(i=0; i<tg_micron_nand.block_sum; i++)
    {
        erase_block_nand(i);
    }
    return true;
}

//说明: 以下三个查询是否需要擦除的函数，基于flash的特征：无论块是否空，只要是1
//      的位就可以改为0，而flash的寿命由擦除次数决定。
//----查询是否需要擦除(with ecc)-----------------------------------------------
//功能: 查询在block的offset处写入buf中的size长度数据前是否需要擦除块。由于擦除
//      既慢又容易磨损flash，故本函数对加快速度和延长flash寿命很有效，在不同情况
//      下，呈现的效果并不一致。
//      1、norflash，多次写入少量数据时非常有效。
//      2、nandflash做高可靠应用，由于存在扇区ecc的问题，需要写入sector边界对齐
//        的数据才有效。
//      3、nandflash做媒体应用，音视频媒体应用可不做ecc，效果与norflash一样。
//      4、故nandflash只判断是否全FF

//      对第二种情况，一个改进方法是每次写零碎数据时均新开一个扇区，待扇区用完以
//      后才整体压缩擦除，但太过麻烦，以后再说吧。
//参数: sector，块号
//      offset，块内偏移地址
//      data，数据缓冲区
//      size，数据尺寸
//返回: flase = 需要擦除，true = 已准备好，不需要擦除
//-----------------------------------------------------------------------------
bool_t query_block_ready_ss_with_ecc(u32 block,u32 offset,
                              u8 *buf,u32 size)
{
    u32 start_sector;  //首扇区号
    u32 start_offset;
    u32 end_sector;    //结束地址所在扇区
    u32 cur_sector;    //当前正在读的扇区
    u32 address,ColAddr,RowAddr;
    u32 loop;
    u8 data;

    if(block >= tg_micron_nand.block_sum)
        return false;
    if((size + offset) > tg_micron_nand.block_size)
        return false;
    if(size == 0)
        return true;
    if(buf == NULL)
        return false;

    //起始扇区号
    start_sector = offset / u32g_sector_size + u32g_sectors_per_block * block;
    //结束扇区号
    end_sector =(offset + size-1)/u32g_sector_size+u32g_sectors_per_block*block;
    address = u32g_sector_size*start_sector;     //计算实际地址
    start_offset = offset;
    for(cur_sector = start_sector; cur_sector <= end_sector; cur_sector++)
    {
        NFC_CsActive(NFC_CE0);                          //激活片选
        __AddrToRowCol(address,&RowAddr,&ColAddr);      //地址转换
        NFC_PageRead(BufferNo,RowAddr,ColAddr);         //写命令
        __wait_ready_nand( );                           //等待完成
        for(loop=0; loop < u32g_sector_size+cn_oob_size; loop++)
        {
            NFC_ReadBuffer(&data,start_offset + loop,1);//第一sector需要偏移
            if(data != 0xff)            //只要有1字节非oxff,即返回错误
            {
                NFC_CsInactive(NFC_CE0);
                return false;
            }
        }
        NFC_CsInactive(NFC_CE0);                        //关闭片选
        address += u32g_sector_size;
        start_offset = 0;                               //从第二个sector，偏移为0
    }
    return true;
}

//----查询是否需要擦除(no ecc)-----------------------------------------------
//功能: 同上一个函数，不带ecc
//参数: sector，块号
//      offset，块内偏移地址
//      data，数据缓冲区
//      size，数据尺寸
//返回: flase = 需要擦除，true = 已准备好，不需要擦除
//-----------------------------------------------------------------------------
bool_t query_block_ready_nand_no_ecc(u32 block,u32 offset,
                              u8 *buf,u32 size)
{
    u32 start_sector;  //首扇区号
    u32 start_offset;  //首地址在首扇区的偏移量
    u32 end_sector;    //结束地址所在扇区
    u32 end_offset;    //结束地址在扇区中的偏移量
    u32 cur_sector;    //当前正在读的扇区
    u32 read_size;
    u32 address,ColAddr,RowAddr;
    u32 completed = 0;
    u32 loop;
    u8 data;

    if(block >= tg_micron_nand.block_sum)
        return false;
    if((size + offset) > tg_micron_nand.block_size)
        return false;
    if(size == 0)
        return true;
    if(buf == NULL)
        return false;

    //起始扇区号
    start_sector = offset / u32g_sector_size + u32g_sectors_per_block * block;
    //起始地址在起始扇区号中的偏移量
    start_offset = offset % u32g_sector_size;
    //结束扇区号
    end_sector =(offset + size-1)/u32g_sector_size+u32g_sectors_per_block*block;
    //结束地址在结束扇区中的偏移量
    end_offset = (offset + size -1) % u32g_sector_size;
    address = block * tg_micron_nand.block_size;
    for(cur_sector = start_sector; cur_sector <= end_sector; cur_sector++)
    {
        if(cur_sector != end_sector)    //当前扇区不是最后一个扇区
            read_size = u32g_sector_size - start_offset;
        else    //当前扇区是最后一个扇区
            //+1是因为end_offset本身是需要写入的
            read_size = end_offset - start_offset +1;

        NFC_CsActive(NFC_CE0);
        __AddrToRowCol(address,&RowAddr,&ColAddr);
        NFC_PageRead(BufferNo,RowAddr,ColAddr);
        __wait_ready_nand( );

        for(loop=0; loop < read_size; loop++)
        {
            NFC_ReadBuffer(&data,start_offset + loop,1);//读取数据
            if((data | buf[completed]) != data)         //查看有否0->1的位
            {
                NFC_CsInactive(NFC_CE0);                //关闭片选
                return false;                           //有0->1的位，返回需要擦除
            }
            completed++;
        }

        NFC_CsInactive(NFC_CE0);                        //关闭片选
        address += read_size;
        start_offset = 0;                       //从第二个扇区开始，肯定从0开始读
    }
    return true;
}

//----查询是否需要擦除(buf)-----------------------------------------------
//功能: flash中的原来的内容在org_data中，如果要改成如new_data中的数据，查看是否
//      需要擦除。
//参数: new_data，新数据缓冲区
//      new_data，数据缓冲区
//      size，数据尺寸
//返回: flase = 需要擦除，true = 已准备好，不需要擦除
//-----------------------------------------------------------------------------
bool_t query_ready_with_data_nand(u8 *new_data,u8 *org_data,
                                        u32 size)
{
    u32 loop;
    if(org_data == NULL)
        return false;
    if(new_data != NULL)
    {
        for(loop = 0; loop < size; loop++)
        {
            if((org_data[loop] | new_data[loop]) != org_data[loop])
                return false;   //如果存在由0改1的数据，返回false
        }
    }else
    {
        for(loop = 0; loop < size; loop++)
        {
            if(org_data[loop] != 0xFF)
                return false;
        }
    }
    return true;
}
//坏块标记算法:
//1、三星出厂时，坏块OOB的头两页第6个字节被写入非0xFF数据，好块则保证是0xFF。
//2、使用过程中的新增坏块，也在头两页的第6字节写入0.
//3、永远不要擦除坏块，否则用于坏块标记的第6字节也被擦除。
//----有效块检查--------------------------------------------------------------
//功能: 检查一个块是否有效块，参考坏块标记算法。
//参数: block_no，块号
//返回: true=好块，false=坏块。
//----------------------------------------------------------------------------
bool_t check_block_nand(u32 block_no)
{
    u32 address,RowAddr,ColAddr;
    u8 data;

    NFC_CsActive(NFC_CE0);

    //OBB的第一个字节为0xFF则为好块，16位数据线时，计算列地址除以2
    address = tg_micron_nand.block_size * block_no + u32g_sector_size/2;

    __AddrToRowCol(address,&RowAddr,&ColAddr);
    NFC_PageRead(BufferNo,RowAddr,ColAddr);
    __wait_ready_nand( );

    NFC_ReadBuffer(&data,0,1);                  //todo

    if(data != 0xFF)
    {
        NFC_CsInactive(NFC_CE0);
        return  false;
    }

    NFC_CsInactive(NFC_CE0);
    return true;
}
//----标记一个坏块-----------------------------------------------------------
//功能：在读、写、擦除等函数中调用，如果操作过程中发现某块是坏块，立即调用本函数
//      把它标记为坏块，即把每块首扇区oob的第1字节写0.
//参数: block_no，块号
//返回: true=成功标记，false=标记失败。
//----------------------------------------------------------------------------
bool_t __mark_invalid_block(u32 block_no)
{
    u32 address,RowAddr,ColAddr;
    u8 temp = 0,i;

    address = tg_micron_nand.block_size * block_no + u32g_sector_size/2;
    __AddrToRowCol(address,&RowAddr,&ColAddr);

    NFC_WriteBuffer(&temp,0,1);                     //写0时标记为坏块
    temp = 0xFF;
    for(i = 0; i < u16g_oob_size-1;i++)
    {
        NFC_WriteBuffer(&temp,i+1,1);
    }
    NFC_CsActive(NFC_CE0);
    NFC_PageProgram(BufferNo,RowAddr,ColAddr);      //编程
    __wait_ready_nand_slow(CN_WAIT_PAGE_WRITE);     //等待芯片内部操作完成

    if(__read_status_nand() & cn_nand_failure)
    {
        NFC_CsInactive(NFC_CE0);
        return false;
     }
    NFC_CsInactive(NFC_CE0);                        //除能片选
    return true;
}

//----查flash中好块数量--------------------------------------------------------
//功能: 查flash中好块数量
//参数: 无
//返回: 好块数量
//-----------------------------------------------------------------------------
u32 __check_all_ss(void)
{
    u32 i,sum = 0;
    u32 address,RowAddr,ColAddr;

    for(i=0; i<tg_micron_nand.block_sum; i++)
    {
        NFC_CsActive(NFC_CE0);
        address = tg_micron_nand.block_size * i + u32g_sector_size/2;

        __AddrToRowCol(address,&RowAddr,&ColAddr);

        NFC_PageRead(BufferNo,RowAddr,ColAddr);
        __wait_ready_nand( );
    }

    NFC_CsInactive(NFC_CE0);
    return sum;
}

//----计算扇区ecc码-----------------------------------------------------------
//功能: data中包含256字节数据，计算它的ecc码
//参数: data，数据缓冲区
//      ecc: 3字节的缓冲区，返回ecc码
//返回: 无
//-----------------------------------------------------------------------------
void __make_sector_ecc(u8 *data,u8 *ecc)
{
    u32 i;
    for(i = 0; i < u32g_sector_size/256; i++)
    {
        Ecc_Make256(data + i*256, ecc + i*3);
    }
}

//----读flash 芯片id-----------------------------------------------------------
//功能: 读取flash芯片的id
//参数: 无
//返回: 芯片id
//-----------------------------------------------------------------------------
uint16_t __read_chip_id (void)
{
    uint16_t id;
    u32 id0;
    NFC_CsActive(NFC_CE0);

    NFC_FlashIDRead(BufferNo,&id0,NULL);
    id = (uint16_t)(id0>>16);

    NFC_CsInactive(NFC_CE0);
    return id ;
}

//----解码芯片id---------------------------------------------------------------
//功能: 根据芯片id识别芯片,识别结果填写到tg_micron_nand中
//参数: id,待识别的芯片id
//返回: true = 正确识别，false = 不能识别
//-----------------------------------------------------------------------------
bool_t __parse_chip(uint16_t id,char **name)
{
    u32 no,sum;
    sum = sizeof(tg_nand_table)/sizeof(struct nand_table);
    for(no = 0; no < sum; no++)
    {
        if(id == tg_nand_table[no].vendor_chip_id)
        {
            u32g_sectors_per_block = tg_nand_table[no].pages_per_block;
            tg_micron_nand.block_sum = tg_nand_table[no].blocks_sum;
            tg_micron_nand.block_size = tg_nand_table[no].block_size;
            tg_micron_nand.PTT_read_buf_size = tg_nand_table[no].block_size;
            tg_micron_nand.PTT_write_buf_size = tg_nand_table[no].block_size;
            *name = tg_nand_table[no].chip_name;
            u16g_oob_size = tg_nand_table[no].oob_size;
            u32g_sector_size =tg_micron_nand.block_size/u32g_sectors_per_block;
            return true;
        }
    }
    return false;
}

//----复位芯片-----------------------------------------------------------------
//功能: 复位芯片
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void __reset_nand(void)
{
    NFC_CsActive(NFC_CE0);

    // 发送Reset 命令
    NFC_Reset();

    __wait_ready_nand_slow(CN_WAIT_RESET);  //等待芯片内部操作完成
    NFC_CsInactive(NFC_CE0);
}

//----初始化nand芯片-----------------------------------------------------------
//功能：初始化nand芯片用于文件系统
//参数: 无
//返回: 1= 成功，0=失败
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_FsNandFlash(const char *FlashHeapName)
{
    uint16_t chip_id;
    char *name;
    pHeap_t MyHeap;

    // 定时器初始化，用于等待操作完成定时---todo
    NFC_ModuleInit();

    __reset_nand();
    chip_id = __read_chip_id();
    if( __parse_chip(chip_id,&name) == false)
        return 0;
    MyHeap = M_FindHeap(FlashHeapName);
    if(MyHeap == NULL)
        return 0;
    tg_micron_nand.ChipHeap = MyHeap;

    pg_sector_buf = (u8*)M_MallocHeap(u32g_sector_size+u16g_oob_size,MyHeap,0);
    if(pg_sector_buf == NULL)
    {
        return 0;
    }
    tg_micron_nand.query_block_ready_with_ecc= query_block_ready_ss_with_ecc;
    tg_micron_nand.query_block_ready_no_ecc = query_block_ready_nand_no_ecc;
    tg_micron_nand.query_ready_with_data = query_ready_with_data_nand;
    tg_micron_nand.erase_block = erase_block_nand;
    tg_micron_nand.check_block = check_block_nand;
    tg_micron_nand.read_data_with_ecc = read_block_ss_with_ecc;
    tg_micron_nand.write_data_with_ecc = write_block_ss_with_ecc;
    tg_micron_nand.read_data_no_ecc = read_block_ss_no_ecc;
    tg_micron_nand.write_data_no_ecc = write_block_ss_no_ecc;
    tg_micron_nand.write_PCRB = write_PCRB_nand;
    tg_micron_nand.restore_PCRB = restore_PCRB_nand;
    if(DFFSD_InstallChip(&tg_micron_nand,name,cn_reserve_blocks))
        return 1;
    else
    {
        free(pg_sector_buf);
        return 0;
    }
}
bool_t nand_test(void)
{
    ModuleInstall_FsNandFlash(0);
    u8 buf[1024];
    u32 i;

    __reset_nand();

    erase_block_nand(10);

    __read_sector_nand_with_ecc(10*0x40,0,buf,1024);

    for(i=0;i<1024;i++)
        buf[i] = i;

    __write_sector_nand_with_ecc(10*0x40,0,buf,1024);

    __read_sector_nand_with_ecc(10*0x40,0,buf,1024);

    return true;
}

bool_t nand_reset(void)
{
    NFC_ModuleInit();
    return true;
}
