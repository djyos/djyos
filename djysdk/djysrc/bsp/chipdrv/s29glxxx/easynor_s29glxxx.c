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
//所属模块:s29glxxx驱动
//作者：lst
//版本：V1.0.1
//文件描述:s29glxxx flash驱动程序，提供flash的原始操作函数
//其他说明:
//修订历史:
//2. 日期: 2014-03-28
//   作者: liq
//   新版本号: V1.0.1
//   修改说明: 升级驱动程序，使之支持norFlash的8位与16位两种工作模式；将相关的一些宏定义移至sys_space_config.h中
//1. 日期: 2013-08-04
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "easynor_s29glxxx.h"
#include "SysSpace.h"
#include "easynorflash.h"

static struct EasynorFlashChip norflash_chip;
static u8 *chip_sector_buf;

#define cn_flash_size                   0x2000000    //32M
#define cn_flash_sector_length          0x20000     // 2*65536 bytes/sector 一个扇区有多少个字节
#define cn_flash_sector_sum             (cn_flash_size/cn_flash_sector_length)
#define cn_flash_base_addr              CN_NORFLASH_BASE

#define cn_flash_page_length            cn_norflash_buf_size

#define cmd_erase_sector 0x30
#define cmd_reset_chip   0xF0

#define cn_wr_word_delay_us   500
#define cn_w_buffer_delay_us  1200

//----读写flash等待操作完成-----------------------------------------------------
//功能：利用s29glxxx的toggle功能，如果内部写或擦除操作正在进行，连续两次读（任意
//      地址）将得到0/1交替的结果。
//参数：delay_us,等待操作完成的时间，微秒
//全局：chip_addr，芯片基地址
//返回：true=完成查询，false=超时不结束。
//-----------------------------------------------------------------------------
static bool_t __wait_end_s29glxxx(u32 delay_us)
{
    static u32 i=0;
    volatile u16 a,b;
    u16 c;
    i++;
    for(c=0;c<delay_us;c++)
    {
        a=*(u16 *)cn_flash_base_addr;
        b=*(u16 *)cn_flash_base_addr;
        if(a==b)
            return(true);
        Djy_DelayUs(1);   //延时1uS
    }
    return(false);
}
//----擦除sector等待操作完成-----------------------------------------------------
//功能：利用s29glxxx的toggle功能，如果内部写或擦除操作正在进行，连续两次读（任意
//      地址）将得到0/1交替的结果,s29gl128p最大擦除时间是3.5s，s29gl256s最大擦除
//      时间为1.1s，为了兼容版本，此次取大值
//参数：无
//全局：cn_flash_base_addr，芯片基地址
//返回：true=完成查询，false=超时不结束。
//-----------------------------------------------------------------------------
static bool_t __wait_erase_end_s29glxxx(void)
{
    static u32 i=0;
    volatile u16 a,b;
    u16 c;
    i++;
    for(c=0;c<35;c++)//最大擦除时间为3.5S
    {
        a=*(u16 *)cn_flash_base_addr;
        b=*(u16 *)cn_flash_base_addr;
        if(a==b)
            return(true);
        Djy_DelayUs(100000);
    }
    return(false);
}
 //----写一个字----------------------------------------------------------------
//功能：写Flash一个字,写入地址应该是已经擦除的,
//参数：addr,字对齐的写入地址,
//      Data:写入的字
//全局：chip_addr，芯片基地址
//返回：无
//---------------------------------------------------------------------------
/*
static void __word_write_s29glxxx(u16 * addr,u16 data)
{
    __wait_end_s29glxxx(cn_wr_word_delay_us);
    if(*addr == data)
        return;
#if(cn_norflash_bus_width==16)
    ((u16 *)cn_flash_base_addr)[0x555]=0xaa;
    ((u16 *)cn_flash_base_addr)[0x2aa]=0x55;
    ((u16 *)cn_flash_base_addr)[0x555]=0xa0;
#endif
#if(cn_norflash_bus_width==8)
    ((u16 *)cn_flash_base_addr)[0xaaa]=0xaa;
    ((u16 *)cn_flash_base_addr)[0x555]=0x55;
    ((u16 *)cn_flash_base_addr)[0xaaa]=0xa0;
#endif
    *addr=data;
    return;
}
*/
//----按buffer写flash---------------------------------------------------------
//功能：按buffer的写入方式写flash，地址必须是按页大小对齐
//参数：dst_addr,页对齐的写入地址,该地址为相对地址
//      src_addr,写入数据的地址，函数内部做强制转换
//      len,写入的数据长度，单位为byte
//全局：cn_flash_base_addr，芯片基地址
//返回：无
//---------------------------------------------------------------------------
static void __buf_write_s29glxxx(u32 dst_addr, u32 src_addr, u32 len)
{
    u32 i,sector_addr;
    u16 *src = (u16*)src_addr;
    u16 *dst = (u16*)(dst_addr + cn_flash_base_addr);

    //计算该块的起始地址
    sector_addr = dst_addr - dst_addr%cn_flash_sector_length + cn_flash_base_addr;
    if(!__wait_end_s29glxxx(cn_w_buffer_delay_us))
        return ;

#if(cn_norflash_bus_width==16)
    ((u16 *)cn_flash_base_addr)[0x555]=0xaa;
    ((u16 *)cn_flash_base_addr)[0x2aa]=0x55;
    len /= 2;
#elif(cn_norflash_bus_width==8)
    ((u16 *)cn_flash_base_addr)[0xaaa]=0xaa;
    ((u16 *)cn_flash_base_addr)[0x555]=0x55;
#endif

    *((u16 *)sector_addr) = 0x25;
    *((u16 *)sector_addr) = len-1;

    for(i = 0; i <= len - 1; i++)
    {
        *dst++ = *src++;
    }

    *((u16 *)sector_addr) = 0x29;

    return;
}

//----复位flash芯片---------------------------------------------------------
//功能：复位norflash芯片，停止所有芯片操作
//参数：无
//返回：无
//--------------------------------------------------------------------------
void reset_norflash(void)
{
    *((u16 *)cn_flash_base_addr) = cmd_reset_chip;
}

//----查询sector是准备就绪-------------------------------------------------
//功能：检查参数写入flash之前，是否需要擦除。按字节读写
//      的原因需要计算offset和(offset+size)的奇偶性来决定读取字数，
//      buf为NULL,则检查是否需要擦除，即是否为全F。
//参数：sector:操作的片
//      offset:该片中的偏移量，字节为单位
//      buf:检查的数据
//      size:检查大小，字节为单位
//返回：true = 已准备好，不需要擦除(或空块),false = 需要擦除；
//-----------------------------------------------------------------------------
bool_t query_sector_ready(u32 sector,u32 offset,u8 *buf,u32 size)
{
    u16 *sector_addr;
    u32 i,length;
    u16 wd,wdn;

    if(size == 0)
        return false;

    if(sector >= cn_flash_sector_sum)           //参数错误
        return false;
    if(offset +size > cn_flash_sector_length)   //超出本sector
        size = cn_flash_sector_length - offset;

    //计算从offset到size的所需比较的flash中字长度
    sector_addr = (u16*)(sector*cn_flash_sector_length+cn_flash_base_addr);
    if(!__wait_end_s29glxxx(cn_w_buffer_delay_us))
        return 0;

#if(cn_norflash_bus_width==16)
    length = ((offset + size)/2 + (offset + size)%2 )-offset/2;
    for(i = 0; i < length; i++)
    {
        if((i == 0) &&  (offset%2))  //处理首字,offset是奇数,注意大小端问题
        {
            wd = sector_addr[offset/2 + i] | 0xFF00;//将字中的高字节平移到低8位，高八位写1
            if(((wd | buf[0]) != wd) || ((buf == NULL)&&(wd != 0xFFFF)))
                return false;
        }
        else if((i == length - 1) && ((offset + size)%2))//处理最后一字,offset+size是奇数
        {
            wd = (sector_addr[offset/2 + i]>>8) | 0xFF00;
            if(((wd | buf[size-1]) != wd) || ((buf == NULL)&&(wd != 0xFFFF)))
                return false;
        }
        else
        {
            wdn = (buf[i*2 - offset%2]<<8) +  buf[i*2+1 - offset%2];
            //检查有没有把0改成1的;检查buf为NULL的情况
            if(((sector_addr[offset/2 + i] | wdn) != sector_addr[offset/2 + i])
                        || ((buf == NULL)&&(sector_addr[offset/2 + i] != 0xFFFF)))
                return false;
        }
    }
#elif(cn_norflash_bus_width==8)
    for(i = 0; i < size; i++)
    {
        wd=sector_addr[offset+i];
        wdn = buf[i];
        //检查有没有把0改成1的;检查buf为NULL的情况
        if(((wd | wdn) != wd)
                    || ((buf == NULL)&&(wd != 0xFF)))
            return false;
    }
#endif

    return true;
}

//----读flash---------------------------------------------------------------
//功能：读flash中的数据
//参数：sector:扇区号
//      offset:该片中的偏移量，字节为单位
//      buf:将读到的数据存放在buf
//      size:读数据大小，字节为单位
//返回：cn_limit_uint32:参数错误
//      其它:实际读到的数据量
//--------------------------------------------------------------------------
u32 read_data(u32 sector,u32 offset,u8 *buf,u32 size)
{
    u16 *sector_addr;
    u32 i,length;

    if(size == 0)
        return 0;

    if(sector >= cn_flash_sector_sum)           //参数错误
        return CN_LIMIT_UINT32;
    if(offset +size > cn_flash_sector_length)   //超出本sector
        size = cn_flash_sector_length - offset;

    //计算从offset到size的所需比较的flash中字长度
    sector_addr = (u16*)(sector*cn_flash_sector_length+cn_flash_base_addr);
    if(!__wait_end_s29glxxx(cn_w_buffer_delay_us))
        return 0;

#if(cn_norflash_bus_width==16)
    length = ((offset + size)/2 + (offset + size)%2 )-offset/2;
    for(i = 0; i < length; i++)
    {
        if((i == 0) &&  (offset%2))  //处理首字,offset是奇数
            buf[0] = (u8)sector_addr[offset/2] ;
        else if((i == length - 1) && ((offset + size)%2))    //处理最后一字,offset+size是奇数
            buf[size-1] = (u8)(sector_addr[offset/2 + length - 1]>>8);
        else    //offset的奇偶性会影响buf下标
        {
           buf[i*2 - offset%2]    = (u8)((sector_addr[i+offset/2])>>8);
           buf[i*2 + 1 - offset%2]= (u8)sector_addr[i+offset/2];
        }
    }
#elif(cn_norflash_bus_width==8)
    for(i = 0; i < size; i++)
    {
        buf[i]=(u8)sector_addr[offset+i];
    }
#endif
    return size;
}

//----写flash---------------------------------------------------------------
//功能：写Flash,文件系统在调用该函数前，需判断是否需要擦除
//参数：sector:扇区号
//      offset:该片中的偏移量，字节为单位
//      buf:将要发送的数据地址
//      size:写数据量。字节为单位
//返回：cn_limit_uint32: 参数错误
//      其他值: 实际写入数据量
//--------------------------------------------------------------------------
/*
u32 write_data(u32 sector,u32 offset,u8 *buf,u32 size)
{
    u16 *sector_addr;
    u32 i,length;
    u16 wd;

    if(size == 0)
        return 0;

    if(sector >= cn_flash_sector_sum)       //参数错误
        return CN_LIMIT_UINT32;
    if(offset +size > cn_flash_sector_length)//超出本sector
        size = cn_flash_sector_length - offset;

    sector_addr = (u16*)(sector*cn_flash_sector_length+cn_flash_base_addr);

    //把写入的内容填写到起始扇区缓冲区中
#if(cn_norflash_bus_width==16)
    length = ((offset + size)/2 + (offset + size)%2 )-offset/2;
    for(i = 0;i < length; i++)
    {
        if((i==0) && (offset%2))
            wd = 0xFF00 | buf[i];
        else if((i ==length -1) && ((offset + size)%2))
            wd = 0x00FF | (buf[size-1]<<8);
        else
            wd = (buf[i*2 - offset%2]<<8) +  buf[i*2+1 - offset%2];

        __data_write_s29glxxx(&sector_addr[i+offset/2], wd);
    }
#elif(cn_norflash_bus_width==8)
    for(i = 0;i < size; i++)
    {
        __data_write_s29glxxx(sector_addr+i+offset, buf[i]);
    }
#endif

    return size;
}
*/
//----写flash---------------------------------------------------------------
//功能：写Flash,文件系统在调用该函数前，需判断是否需要擦除
//参数：sector:扇区号
//      offset:该片中的偏移量，字节为单位
//      buf:将要发送的数据地址
//      size:写数据量。字节为单位
//返回：cn_limit_uint32: 参数错误
//      其他值: 实际写入数据量
//--------------------------------------------------------------------------
u32 write_data(u32 sector,u32 offset,u8 *buf,u32 size)
{
    u32 i,completed = 0,w_size;
    u8 page_buf[cn_flash_page_length];
    u32 page_addr,page_offset;
    u32 page_start,page_end;

    if(size == 0)
        return 0;
    if(sector >= cn_flash_sector_sum)       //参数错误
        return CN_LIMIT_UINT32;
    if(offset +size > cn_flash_sector_length)//超出本sector
        size = cn_flash_sector_length - offset;

    //计算本次操作的起始和结束页号
    page_start = offset / cn_flash_page_length;
    page_end   = (offset + size - 1) / cn_flash_page_length;

    for(i = page_start; i <= page_end; i++)
    {
        page_addr = i * cn_flash_page_length;
        //处理前面没有与页大小对齐的情况
        if((i == page_start) && (offset%cn_flash_page_length))
        {
            page_offset = offset%cn_flash_page_length;//在该页内的偏移
            read_data(sector,page_addr,page_buf,cn_flash_page_length);//读出该页

            //该页内可写入数据量
            w_size = cn_flash_page_length-page_offset;
            w_size = (size > w_size)?w_size:size;//计算写入长度
            memcpy(page_buf+page_offset,buf,w_size);

            completed += w_size;
        }
        else if((i == page_end) && ((offset+size)%cn_flash_page_length))//后面的字节
        {
            page_offset = 0;
            w_size = (offset+size)%cn_flash_page_length;

            read_data(sector,page_addr,page_buf,cn_flash_page_length);//读出该页
            memcpy(page_buf,buf+completed,w_size);
            completed += w_size;
        }
        else//处理中间的512字节
        {
            memcpy(page_buf,buf+completed,cn_flash_page_length);
            completed += cn_flash_page_length;
        }

        //使用写缓冲区的方式将数据写入flash
        page_addr = sector*cn_flash_sector_length + page_addr;//计算出该页所处的相对地址
        __buf_write_s29glxxx(page_addr,
                (u32)page_buf,cn_flash_page_length);
    }
    return completed;
}
//----扇区擦除-----------------------------------------------------------
//功能：擦除一个扇区,s29gl128每128k字节为1扇区.根据s29gl128p技术文档，
//      擦除128K的sector正常开销0.5S，因此需要在擦除0.5S后才能读写
//参数：sector， 扇区号，128k字节对齐
//返回：true=成功擦除，false=失败。
//-----------------------------------------------------------------------
bool_t erase_sector(uint32_t sector)
{
    u16 *sector_addr;
    if(sector >= cn_flash_sector_sum)
        return false;
    sector_addr = (u16*)(sector*cn_flash_sector_length+cn_flash_base_addr);
    if(!__wait_end_s29glxxx(cn_w_buffer_delay_us))   //等待上一次操作完成
        return false;
#if(cn_norflash_bus_width==16)
    ((u16 *)cn_flash_base_addr)[0x555]=0xaa;
    ((u16 *)cn_flash_base_addr)[0x2aa]=0x55;
    ((u16 *)cn_flash_base_addr)[0x555]=0x80;
    ((u16 *)cn_flash_base_addr)[0x555]=0xaa;
    ((u16 *)cn_flash_base_addr)[0x2aa]=0x55;
#elif(cn_norflash_bus_width==8)
    ((u16 *)cn_flash_base_addr)[0xaaa]=0xaa;
    ((u16 *)cn_flash_base_addr)[0x555]=0x55;
    ((u16 *)cn_flash_base_addr)[0xaaa]=0x80;
    ((u16 *)cn_flash_base_addr)[0xaaa]=0xaa;
    ((u16 *)cn_flash_base_addr)[0x555]=0x55;
#endif
    *sector_addr = cmd_erase_sector;

    __wait_erase_end_s29glxxx();
    return true;
}

//----初始化norflash芯片----------------------------------------------------
//功能：初始化norflash芯片用于文件系统
//参数: para,磁盘符名
//返回: 0= 成功，1=失败
//--------------------------------------------------------------------------
ptu32_t ModuleInstall_fs_s29glxxx(ptu32_t para)
{
    reset_norflash();//复位norflash芯片
    chip_sector_buf = (u8*)M_Malloc(cn_flash_sector_length,0);
    if(chip_sector_buf == NULL)
        return 0;

    strcpy(norflash_chip.PTT_name,(const char*)para);
    norflash_chip.block_size     = cn_flash_sector_length;//块大小
    norflash_chip.start_block     = CN_CFG_EASYNOR_START_SECTOR;//该文件系统起始sector
    norflash_chip.block_sum     = CN_CFG_EASYNOR_LENGTH_SECTOR;//sector用于存放该文件系统
    norflash_chip.block_buf     = chip_sector_buf;
    norflash_chip.files_sum     = 0;
    norflash_chip.erase_block         = erase_sector;
    norflash_chip.query_block_ready   = query_sector_ready;
    norflash_chip.read_data           = read_data;
    norflash_chip.write_data          = write_data;

    if(Easynor_InstallChip(&norflash_chip) == 0)
    {
        extern ptu32_t Cmd_InstallEasynorFs(ptu32_t para);
        Cmd_InstallEasynorFs(0);
        return 0;
    }
    else
    {
        free(chip_sector_buf);
        return 1;
    }
}

