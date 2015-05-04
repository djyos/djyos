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
#include "string.h"
#include "flash_s29glxxx.h"
#include "board-config.h"

#define CN_FLASH_SIZE                   0x2000000    //32M
#define CN_FLASH_SECTOR_LENGTH          0x20000     // 2*65536 bytes/sector 一个扇区有多少个字节
#define CN_FLASH_SECTOR_SUM             (CN_FLASH_SIZE/CN_FLASH_SECTOR_LENGTH)
#define CN_FLASH_BASE_ADDR              CN_NORFLASH_BASE

#define CN_FLASH_PAGE_LENGTH            cn_norflash_buf_size //芯片buffer大小

#define cmd_erase_sector 0x30

#define CN_WR_WORD_DELAY_US         500
#define CN_W_BUFFER_DELAY_US        1200

static u8 s_block_buf[CN_FLASH_SECTOR_LENGTH];

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
        a=*(u16 *)CN_FLASH_BASE_ADDR;
        b=*(u16 *)CN_FLASH_BASE_ADDR;
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
        a=*(u16 *)CN_FLASH_BASE_ADDR;
        b=*(u16 *)CN_FLASH_BASE_ADDR;
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
static void __data_write_s29glxxx(u32 addr,u16 data)
{
    __wait_end_s29glxxx(CN_WR_WORD_DELAY_US);
    if(*(u16 *)addr == data)
        return;
#if(cn_norflash_bus_width==16)
    ((u16 *)CN_FLASH_BASE_ADDR)[0x555]=0xaa;
    ((u16 *)CN_FLASH_BASE_ADDR)[0x2aa]=0x55;
    ((u16 *)CN_FLASH_BASE_ADDR)[0x555]=0xa0;
#endif
#if(cn_norflash_bus_width==8)
    ((u16 *)CN_FLASH_BASE_ADDR)[0xaaa]=0xaa;
    ((u16 *)CN_FLASH_BASE_ADDR)[0x555]=0x55;
    ((u16 *)CN_FLASH_BASE_ADDR)[0xaaa]=0xa0;
#endif
    *(u16 *)addr=data;
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
    u16 *dst = (u16*)(dst_addr+CN_FLASH_BASE_ADDR);

    sector_addr = dst_addr - dst_addr%CN_FLASH_SECTOR_LENGTH + CN_FLASH_BASE_ADDR;
    __wait_end_s29glxxx(CN_W_BUFFER_DELAY_US);

#if(cn_norflash_bus_width==16)
    ((u16 *)CN_FLASH_BASE_ADDR)[0x555]=0xaa;
    ((u16 *)CN_FLASH_BASE_ADDR)[0x2aa]=0x55;
    len /= 2;
#elif(cn_norflash_bus_width==8)
    ((u16 *)CN_FLASH_BASE_ADDR)[0xaaa]=0xaa;
    ((u16 *)CN_FLASH_BASE_ADDR)[0x555]=0x55;
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

//----检查写前是否需擦除------------------------------------------------------
//功能：检查在写入数据前是否需要擦除，检查的规则是：根据flash特性，flash地址空间
//      的每一个bit位，只能从1改成为0，而不能从0写成1。因此，若写入数据不会导致
//      flash中的数据比特位从0变成1，则无须擦除，否则，需要擦除
//参数：addr:写入数据的起始地址,相对地址
//      buf:数据缓冲区指针
//      len:写数据量。字节为单位
//返回：True: 不需要擦除
//      false: 写前需要擦除
//--------------------------------------------------------------------------
static bool_t __Flash_WriteISReady(uint32_t addr, uint8_t *buf, uint32_t len)
{
    uint32_t i,length,offset;
    uint16_t wd,*flash_addr;

    flash_addr = (u16*)CN_FLASH_BASE_ADDR;

    //计算从offset到size的所需比较的flash中字长度
    offset = addr;
    if(!__wait_end_s29glxxx(CN_W_BUFFER_DELAY_US))
        return false;

#if(cn_norflash_bus_width==16)
    length = addr%2 + (len - addr%2)/2 + (addr + len)%2;      //计算读几个字
    for(i = 0; i < length; i++)
    {
        if((i == 0) &&  (offset%2))  //处理首字,offset是奇数,注意大小端问题
        {
            wd = flash_addr[offset/2] | 0xFF00;//将字中的高字节平移到低8位，高八位写1
            if(((wd | buf[0]) != wd) || ((buf == NULL)&&(wd != 0xFFFF)))
                return false;
        }
        else if((i == length - 1) && ((offset + len)%2))//处理最后一字,offset+size是奇数
        {
            wd = (flash_addr[offset/2 + i]>>8) | 0xFF00;
            if(((wd | buf[len-1]) != wd) || ((buf == NULL)&&(wd != 0xFFFF)))
                return false;
        }
        else
        {
            wd = (buf[i*2 - offset%2]<<8) +  buf[i*2+1 - offset%2];
            //检查有没有把0改成1的;检查buf为NULL的情况
            if(((flash_addr[offset/2 + i] | wd) != flash_addr[offset/2 + i])
                        || ((buf == NULL)&&(flash_addr[offset/2 + i] != 0xFFFF)))
                return false;
        }
    }

#elif(cn_norflash_bus_width==8)
    for(i = 0; i < len; i++)
    {
        wd=flash_addr[offset+i];
        //检查有没有把0改成1的;检查buf为NULL的情况
        if(((wd | buf[i]) != wd)
                    || ((buf == NULL)&&(wd != 0xFFFF)))
            return false;
    }
#endif

    return true;
}


//----写flash---------------------------------------------------------------
//功能：带检查的写flash，调用该函数应用程序不需要判断是否需要擦除flash块，若需要
//      擦除块，则先将先读出块内的数据，擦除该块后，再将数据和现数据全部写入flash,
//      若不需要擦除块，则可直接调用写数据函数
//参数：addr:写入数据的起始地址,相对地址
//      buf:数据缓冲区指针
//      len:写数据量。字节为单位
//返回：CN_FLASH_PARAM_ERROR: 参数错误
//      其他值: 实际写入数据量
//--------------------------------------------------------------------------
uint32_t Flash_WriteDataCheck(uint32_t addr, uint8_t *buf, uint32_t len)
{
    u32 sector_addr,sector,offset;
    u32 sector_start,sector_end;
    u32 completed = 0,size_t;
    u8 *block_buf;

    if(len == 0)
        return 0;

    if((addr  +  len) > (CN_FLASH_SIZE - 1))
        return CN_FLASH_PARAM_ERROR;

    //计算起始块
    sector_start = addr/CN_FLASH_SECTOR_LENGTH;
    sector_end   = (addr + len - 1)/CN_FLASH_SECTOR_LENGTH;

    block_buf = s_block_buf;

    for(sector = sector_start; sector <= sector_end; sector++)
    {
        //计算该块起始地址
        sector_addr = sector*CN_FLASH_SECTOR_LENGTH;
        if(sector == sector_start)
        {
            //计算该块写入偏移
            offset = addr - sector_addr;
            //计算该块写入长度
            size_t = (addr + len) > (sector_addr + CN_FLASH_SECTOR_LENGTH - 1)
                            ? (sector_addr + CN_FLASH_SECTOR_LENGTH - addr)
                            : len;
        }
        else if(sector == sector_end)
        {
            offset = 0;
            size_t = (addr + len)%CN_FLASH_SECTOR_LENGTH;
        }
        else
        {
            offset = 0;
            size_t = CN_FLASH_SECTOR_LENGTH;
        }

        if(__Flash_WriteISReady(addr+completed,&buf[completed],size_t)==false)
        {
            //先要读出来，再写进去
            Flash_ReadData(sector_addr,block_buf,CN_FLASH_SECTOR_LENGTH);
            if(Flash_EraseSector(sector)==false)
                return 0;
            memcpy(block_buf+offset, &buf[completed],size_t);
            Flash_WriteData(sector_addr,block_buf,CN_FLASH_SECTOR_LENGTH);
            completed += size_t;
        }
        else
        {
            completed += Flash_WriteData(addr+completed,&buf[completed],size_t);
        }
    }
    return completed;
}

//----写flash---------------------------------------------------------------
//功能：写Flash,不擦除块，该函数只负责往指定地址写入指定的数据，由应用层检查是在
//      写之前是否需要擦除，驱动认为在调用该函数时，写入flash地址的地址段内全全0xFF
//参数：addr:写入数据的起始地址,相对地址
//      buf:数据缓冲区指针
//      len:写数据量。字节为单位
//返回：CN_FLASH_PARAM_ERROR: 参数错误
//      其他值: 实际写入数据量
//--------------------------------------------------------------------------
/*
uint32_t Flash_WriteData(uint32_t addr, uint8_t *buf, uint32_t len)
{
    uint32_t loop,length,offset,waddr;
    uint16_t wd;

    if(len == 0)
        return 0;

    addr = addr + CN_FLASH_BASE_ADDR;

    if((addr < CN_FLASH_BASE_ADDR) ||
        (addr +  len > CN_FLASH_BASE_ADDR + (CN_FLASH_SIZE - 1)))
        return CN_FLASH_PARAM_ERROR;

    offset = addr - CN_FLASH_BASE_ADDR;

    //flash中的存储一律按小端存储
#if(cn_norflash_bus_width==16)
    length = addr%2 + (len - addr%2)/2 + (addr + len)%2;      //计算读几个字
    for(loop = 0;loop < length; loop++)
    {
        //可根据往flash中写0xFF不影响flash数据的特性，wd的高8位写入0xFF
        if((loop==0) && (offset%2))
            wd = 0xFF00 | buf[loop];
        else if((loop ==length -1) && ((offset + len)%2))
            wd = 0x00FF | (buf[len-1]<<8);
        else
            wd = (buf[loop*2 - offset%2]<<8) +  buf[loop*2+1 - offset%2];

        waddr = addr + loop * 2 - (offset%2);
        __data_write_s29glxxx((u32)waddr,wd);
    }
#elif(cn_norflash_bus_width==8)
    for(loop = 0;loop < len; loop++)
    {
        __data_write_s29glxxx(addr+loop, buf[loop]);
    }
#endif

    return len;
}
*/

//----写flash---------------------------------------------------------------
//功能：写Flash,不擦除块，该函数只负责往指定地址写入指定的数据，由应用层检查是在
//      写之前是否需要擦除，驱动认为在调用该函数时，写入flash地址的地址段内全全0xFF
//参数：addr:写入数据的起始地址,相对地址
//      buf:数据缓冲区指针
//      len:写数据量。字节为单位
//返回：CN_FLASH_PARAM_ERROR: 参数错误
//      其他值: 实际写入数据量
//--------------------------------------------------------------------------
uint32_t Flash_WriteData(uint32_t addr, uint8_t *buf, uint32_t len)
{
    u8 page_buf[CN_FLASH_PAGE_LENGTH];
    u32 i,page_start,page_end;
    u32 page_offset,page_addr;
    u32 w_size,completed = 0;

    if(len == 0)
        return 0;

    //计算需要按buffer写入的页范围
    page_start = addr/CN_FLASH_PAGE_LENGTH;
    page_end   = (addr+len-1)/CN_FLASH_PAGE_LENGTH;

    for(i = page_start; i <= page_end; i++)
    {
        //计算第i页起始地址
        page_addr = i * CN_FLASH_PAGE_LENGTH;
        if((i == page_start) && (addr%CN_FLASH_PAGE_LENGTH))//操作起始页
        {
            //计算写入的该页数据的偏移
            page_offset = addr%CN_FLASH_PAGE_LENGTH;
            Flash_ReadData(page_addr,page_buf,CN_FLASH_PAGE_LENGTH);

            //计算可写入该页数据大小
            w_size = CN_FLASH_PAGE_LENGTH - page_offset;
            w_size = (len > w_size)?(w_size):len;
            memcpy(page_buf+page_offset,buf,w_size);

            completed += w_size;
        }
        else if((i == page_end) && ((addr+len)%CN_FLASH_PAGE_LENGTH))
        {
            //计算页偏移量和大小
            page_offset = 0;
            w_size = (addr+len)%CN_FLASH_PAGE_LENGTH;

            //读出该页数据
            Flash_ReadData(page_addr,page_buf,CN_FLASH_PAGE_LENGTH);
            memcpy(page_buf,buf+completed,w_size);
            completed += w_size;
        }
        else
        {
            memcpy(page_buf,buf+completed,CN_FLASH_PAGE_LENGTH);
            completed += CN_FLASH_PAGE_LENGTH;
        }
        //将数据以buffer的方式写入到flash
        __buf_write_s29glxxx(page_addr,
                (u32)page_buf,CN_FLASH_PAGE_LENGTH);
    }
    return completed;
}
//----读flash---------------------------------------------------------------
//功能：读flash中指定地址的数据
//参数：addr:读数据的起始地址,相对地址
//      buf:将读到的数据存放在buf指针
//      len:读数据大小，字节为单位
//返回：CN_FLASH_PARAM_ERROR:参数错误
//      其它:实际读到的数据量
//--------------------------------------------------------------------------
uint32_t Flash_ReadData(uint32_t addr, uint8_t *buf, uint32_t len)
{
    u32 length,i;
    u16 *flash_addr;

    if(len == 0)
        return 0;

    if((addr +  len) > (CN_FLASH_SIZE - 1))//参数错误
        return CN_FLASH_PARAM_ERROR;

    flash_addr = (u16*)CN_FLASH_BASE_ADDR;
    //检查上次操作是否完成
    if(!__wait_end_s29glxxx(CN_W_BUFFER_DELAY_US))
        return 0;

#if(cn_norflash_bus_width==16)
    length = addr%2 + (len - addr%2)/2 + (addr + len)%2;      //计算读几个字
    for(i = 0; i < length; i++)
    {
        if((i == 0) &&  (addr%2))                           //处理首字,offset是奇数
            buf[0] = (u8)flash_addr[addr/2] ;
        else if((i == length - 1) && ((addr + len)%2))    //处理最后一字,offset+size是奇数
            buf[len-1] = (u8)(flash_addr[addr/2 + length - 1]>>8);
        else    //offset的奇偶性会影响buf下标
        {
           buf[i*2 - addr%2]    = (u8)((flash_addr[i+addr/2])>>8);
           buf[i*2 + 1 - addr%2]= (u8)flash_addr[i+addr/2];
        }
    }
#elif(cn_norflash_bus_width==8)
    for(i = 0; i < len; i++)
    {
        buf[i]=(u8)flash_addr[addr+i];
    }
#endif
    return len;
}

//----扇区擦除-----------------------------------------------------------
//功能：擦除一个扇区,s29gl128每128k字节为1扇区.根据s29gl128p技术文档，
//      擦除128K的sector正常开销0.5S，因此需要在擦除0.5S后才能读写
//参数：sector， 扇区号，128k字节对齐，
//返回：true=成功擦除，false=失败。
//----------------------------------------------------------------------------
uint8_t Flash_EraseSector(uint32_t sector)
{
    u16 *sector_addr;
    if(sector >= CN_FLASH_SECTOR_SUM)
        return false;
    sector_addr = (u16*)(sector*CN_FLASH_SECTOR_LENGTH+CN_FLASH_BASE_ADDR);
    if(!__wait_end_s29glxxx(CN_W_BUFFER_DELAY_US))   //等待上一次操作完成
        return false;
#if(cn_norflash_bus_width==16)
    ((u16 *)CN_FLASH_BASE_ADDR)[0x555]=0xaa;
    ((u16 *)CN_FLASH_BASE_ADDR)[0x2aa]=0x55;
    ((u16 *)CN_FLASH_BASE_ADDR)[0x555]=0x80;
    ((u16 *)CN_FLASH_BASE_ADDR)[0x555]=0xaa;
    ((u16 *)CN_FLASH_BASE_ADDR)[0x2aa]=0x55;
#elif(cn_norflash_bus_width==8)
    ((u16 *)CN_FLASH_BASE_ADDR)[0xaaa]=0xaa;
    ((u16 *)CN_FLASH_BASE_ADDR)[0x555]=0x55;
    ((u16 *)CN_FLASH_BASE_ADDR)[0xaaa]=0x80;
    ((u16 *)CN_FLASH_BASE_ADDR)[0xaaa]=0xaa;
    ((u16 *)CN_FLASH_BASE_ADDR)[0x555]=0x55;
#endif
    *sector_addr = cmd_erase_sector;

    __wait_erase_end_s29glxxx();
    return true;
}

//----扇区地址段-----------------------------------------------------------
//功能：擦除起始地址为addr，结束地址为len所在的空间
//参数：addr， 起始地址,相对地址
//      len,长度，字节为单位
//返回：true=成功擦除，false=失败。
//----------------------------------------------------------------------------
/*
uint8_t Flash_EraseAddr(u32 addr, u32 len)
{
    u32 sector_addr,sector,offset;
    u32 sector_start,sector_end;
    u32 size_t,addr_map;
    u8 *block_buf;

    if(len == 0)
        return 0;

    addr_map = addr + CN_FLASH_BASE_ADDR;//转换成绝对地址

    //判断参数是否正确
    if((addr_map < CN_FLASH_BASE_ADDR) ||
        (addr_map +  len > CN_FLASH_BASE_ADDR + (CN_FLASH_SIZE - 1)))
        return false;

    //计算需要擦除的起始块
    sector_start = (addr_map - CN_FLASH_BASE_ADDR)/CN_FLASH_SECTOR_LENGTH;
    sector_end   = (addr_map + len - CN_FLASH_BASE_ADDR - 1)/CN_FLASH_SECTOR_LENGTH;

    block_buf = s_block_buf;

    for(sector = sector_start; sector <= sector_end; sector++)
    {
        //计算该块的起始地址
        sector_addr = (sector*CN_FLASH_SECTOR_LENGTH+CN_FLASH_BASE_ADDR);
        //处理初始块操作
        if(sector == sector_start)
        {
            //该块内擦除起始地址
            offset = addr_map - sector_addr;
            //该块内擦除长度
            size_t = (addr_map + len) > (sector_addr + CN_FLASH_SECTOR_LENGTH - 1)
                        ? (sector_addr + CN_FLASH_SECTOR_LENGTH - addr_map)
                        : len;

            //擦除前先读出来
            Flash_ReadData(addr-offset,block_buf,offset);//读出前部分
            Flash_ReadData(addr+size_t,block_buf + (offset+size_t),
                    CN_FLASH_SECTOR_LENGTH - (size_t + offset));//读出后部分

            Flash_EraseSector(sector);
            //写入数据
            Flash_WriteData(addr-offset,block_buf,offset);//写入前部分
            Flash_WriteData(addr+size_t,block_buf + (offset+size_t),
                    CN_FLASH_SECTOR_LENGTH - (size_t + offset));//写入后部分
        }
        else if(sector == sector_end)//处理结束块操作
        {
            //该块内擦除起始地址
            offset = 0;
            //该块内擦除长度
            size_t = addr_map + len - sector_addr;

            //只需读出后部分
            Flash_ReadData(addr+len,block_buf,
                    CN_FLASH_SECTOR_LENGTH - size_t);
            Flash_EraseSector(sector);
            //写入后部分保护的数据
            Flash_WriteData(addr+len,block_buf,
                    CN_FLASH_SECTOR_LENGTH - size_t);
        }
        else    //中间地址段直接擦除
        {
            Flash_EraseSector(sector);
        }
    }
    return true;
}
*/
uint8_t Flash_EraseAddr(u32 addr, u32 len)
{
    u32 sector_addr,sector,offset;
    u32 sector_start,sector_end;
    u32 size_t;
    u8 *block_buf;

    if(len == 0)
        return 0;

    //判断参数是否正确
    if((addr + len) > (CN_FLASH_SIZE - 1))
        return false;

    //计算需要擦除的起始块
    sector_start = addr/CN_FLASH_SECTOR_LENGTH;
    sector_end   = (addr + len - 1)/CN_FLASH_SECTOR_LENGTH;

    block_buf = s_block_buf;

    for(sector = sector_start; sector <= sector_end; sector++)
    {
        //计算该块的起始地址
        sector_addr = sector*CN_FLASH_SECTOR_LENGTH;
        //处理初始块操作
        if(sector == sector_start)
        {
            //该块内擦除起始地址
            offset = addr - sector_addr;
            //该块内擦除长度
            size_t = (addr + len) > (sector_addr + CN_FLASH_SECTOR_LENGTH - 1)
                        ? (sector_addr + CN_FLASH_SECTOR_LENGTH - addr)
                        : len;

            //擦除前先读出来
            Flash_ReadData(sector_addr,block_buf,offset);//读出前部分
            Flash_ReadData(addr+size_t,block_buf + (offset+size_t),
                    CN_FLASH_SECTOR_LENGTH - (size_t + offset));//读出后部分

            Flash_EraseSector(sector);
            //写入数据
            Flash_WriteData(sector_addr,block_buf,offset);//写入前部分
            Flash_WriteData(addr+size_t,block_buf + (offset+size_t),
                    CN_FLASH_SECTOR_LENGTH - (size_t + offset));//写入后部分
        }
        else if(sector == sector_end)//处理结束块操作
        {
            //该块内擦除起始地址
            offset = 0;
            //该块内擦除长度
            size_t = addr + len - sector_addr;

            //只需读出后部分
            Flash_ReadData(addr+len,block_buf,
                    CN_FLASH_SECTOR_LENGTH - size_t);
            Flash_EraseSector(sector);
            //写入后部分保护的数据
            Flash_WriteData(addr+len,block_buf,
                    CN_FLASH_SECTOR_LENGTH - size_t);
        }
        else    //中间地址段直接擦除
        {
            Flash_EraseSector(sector);
        }
    }
    return true;
}

//----检查Flash操作是否完成---------------------------------------------------
//功能：查看Flash操作是否完成，超时返回
//参数：us，超时时间
//返回：true,操作成功，正确返回；false,操作失败，错误返回
//--------------------------------------------------------------------------
bool_t Flash_OpetionIsOK(void)
{
    return __wait_end_s29glxxx(CN_W_BUFFER_DELAY_US);
}
//----复位flash芯片---------------------------------------------------------
//功能：复位norflash芯片，停止所有芯片操作
//参数：无
//返回：无
//--------------------------------------------------------------------------
void Flash_ResetChip(void)
{
    *((u16 *)CN_FLASH_BASE_ADDR) = 0x00F0;
}

//----读取flash芯片DeviceID---------------------------------------------------------
//功能：读取flash芯片Device ID
//参数：无
//返回：Device ID
//--------------------------------------------------------------------------
u32 Flash_GetDevID(void)
{
    u32 pnDevCode;
    u16 tmp;
#if(cn_norflash_bus_width==16)
    ((u16 *)CN_FLASH_BASE_ADDR)[0x555]=0xaa;
    ((u16 *)CN_FLASH_BASE_ADDR)[0x2aa]=0x55;
    ((u16 *)CN_FLASH_BASE_ADDR)[0x555]=0x90;
    tmp=*(u16 *)(CN_FLASH_BASE_ADDR+0x201);
    pnDevCode = (tmp&0xFFFF) << 16;
    tmp=*(u16 *)(CN_FLASH_BASE_ADDR+0x20E);
    pnDevCode += (tmp&0xFF) << 8;
    tmp=*(u16 *)(CN_FLASH_BASE_ADDR+0x20F);
    pnDevCode += tmp&0xFF;
#elif(cn_norflash_bus_width==8)
    ((u16 *)CN_FLASH_BASE_ADDR)[0xaaa]=0xaa;
    ((u16 *)CN_FLASH_BASE_ADDR)[0x555]=0x55;
    ((u16 *)CN_FLASH_BASE_ADDR)[0xaaa]=0x90;
    tmp=*(u16 *)(CN_FLASH_BASE_ADDR+0x002);
    pnDevCode = (tmp&0xFFFF) << 16;
    tmp=*(u16 *)(CN_FLASH_BASE_ADDR+0x01C);
    pnDevCode += (tmp&0xFF) << 8;
    tmp=*(u16 *)(CN_FLASH_BASE_ADDR+0x01E);
    pnDevCode += tmp&0xFF;
#endif
    return pnDevCode;
}



