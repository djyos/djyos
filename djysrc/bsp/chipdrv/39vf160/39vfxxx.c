//----------------------------------------------------
//Copyright (C), 2004-2009,  都江堰操作系统开发团队
//版权所有 (C), 2004-2009,   都江堰操作系统开发团队
//所属模块:sst39vf160驱动
//作者：lst
//版本：V1.0.0
//文件描述:sst39vf160 flash驱动程序，提供flash的原始操作函数
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "stddef.h"
#include "board-cfg.h"
#include "int.h"

#include "39vfxxx.h"

#define SST_ID                  0x00BF  // SST Manufacturer's ID code

#define SST_39VF160             0x2782
#define SST_39VF1601            0x234B
#define SST_39VF1602            0x234A
#define SST_39VF3201            0x235B
#define SST_39VF3202            0x235A
#define SST_39VF6401            0x236B
#define SST_39VF6402            0x236A

#define SST_39VF1601C           0x234F
#define SST_39VF1602C           0x234E
#define SST_39VF3201B           0x235D
#define SST_39VF3202B           0x235C
#define SST_39VF3201C           0x235F
#define SST_39VF3202C           0x235E
#define SST_39VF6401B           0x236D
#define SST_39VF6402B           0x236C

struct flash_info tg_flash_info;
static u16 cmd_erase_block,cmd_erase_sector;

void __get_device_id(u16 *mft_id,u16 *device_id)
{

    if((mft_id == NULL) || (device_id == NULL))
        return;
    Flash39VFxxxAddress[0x5555]=0xaa;
    Flash39VFxxxAddress[0x2aaa]=0x55;
    Flash39VFxxxAddress[0x5555]=0x90;
    Djy_DelayUs(1);   // Tida Max 150ns for 39VF640XB

    *mft_id  = Flash39VFxxxAddress[0];
    *device_id  = Flash39VFxxxAddress[1];

    Flash39VFxxxAddress[0x5555]=0xaa;
    Flash39VFxxxAddress[0x2aaa]=0x55;
    Flash39VFxxxAddress[0x5555]=0xF0;
    Djy_DelayUs(1);   // Tida Max 150ns for 39VF640XB

    return ;
}

ptu32_t module_init_sst39vfxxx(ptu32_t para)
{
    Flash39VFxxxAddress = (u16 *)0x60000000;
    __get_device_id(&(tg_flash_info.mft_id),&(tg_flash_info.device_id));
    if(tg_flash_info.mft_id != SST_ID)
        return 0;
    switch(tg_flash_info.device_id)
    {
        case SST_39VF160:
        case SST_39VF1601:
        case SST_39VF1602:
        case SST_39VF3201:
        case SST_39VF3202:
        case SST_39VF6401:
        case SST_39VF6402:
            cmd_erase_sector = 0x30;
            cmd_erase_block = 0x50;
            break;
        case SST_39VF1601C:
        case SST_39VF1602C:
        case SST_39VF3201B:
        case SST_39VF3202B:
        case SST_39VF3201C:
        case SST_39VF3202C:
        case SST_39VF6401B:
        case SST_39VF6402B:
            cmd_erase_sector = 0x50;
            cmd_erase_block = 0x30;
            break;
    }
    return 0;
}
//----等待操作完成------------------------------------------------------------
//功能：利用39vf160的toggle功能，如果内部写或擦除操作正在进行，连续两次读（任意
//      地址）将得到0/1交替的结果。
//参数：无
//全局：chip_addr，芯片基地址
//返回：true=完成查询，false=超时不结束。
//-----------------------------------------------------------------------------
bool_t wait_end_39vfxxx(void)
{
    static u32 i=0;
    volatile u16 a,b;
    u16 c;
    i++;
    for(c=0;c<700;c++)
    {
        a=*Flash39VFxxxAddress;
        b=*Flash39VFxxxAddress;
        if(a==b)
            return(true);
        Djy_DelayUs(100);   //延时100uS
    }
    return(false);  //若70毫秒内仍然不能完成查询，则出错
}
//----块擦除------------------------------------------------------------------
//功能：擦除1块,39VF160每64K字节为1块，使用的命令序列见源程序
//参数：block_addr，块地址，64K字节对齐，若输入非64K对齐地址，将忽略低16位地址。
//全局：chip_addr，芯片基地址
//返回：true=成功擦除，false=失败，应被标记为坏块。nor flash不在擦除时检测坏
//      块，而是用写入回读的方法，故总是返回true
//----------------------------------------------------------------------------
bool_t block_erase_39vfxxx(u16 block_no)
{
    atom_low_t atom_low;
    u16 *block;
    if(block_no >= flash_block_sum)
        return false;
    block=(u16*)(block_no*flash_block_length+flash_base_addr);
    wait_end_39vfxxx();   //等待上一次操作完成
    atom_low = Int_LowAtomStart();  //此序列不能被针对此器件的其他操作打断
    Flash39VFxxxAddress[0x5555]=0xaa;
    Flash39VFxxxAddress[0x2aaa]=0x55;
    Flash39VFxxxAddress[0x5555]=0x80;
    Flash39VFxxxAddress[0x5555]=0xaa;
    Flash39VFxxxAddress[0x2aaa]=0x55;

    *block = cmd_erase_block;
//    wait_end_39vfxxx(); //不等待操作完成，而是在下一次使用前等待
                                    //这样可以节省时间。
    Int_LowAtomEnd(atom_low);
    return true;
}
//----扇区擦除-----------------------------------------------------------------
//功能：擦除一个扇区,39VF160每4K字节为1扇区.
//参数：start， 扇区地址，4K字节对齐，若输入非4K对齐地址，将忽略低12位地址。
//全局：chip_addr，芯片基地址
//返回：true=成功擦除，false=失败，应被标记为坏块。nor flash不在擦除时检测坏
//      块，而是用写入回读的方法，故总是返回true
//----------------------------------------------------------------------------
bool_t sector_erase_39vfxxx(u16 sector_no)
{
    atom_low_t atom_low;
    u16 *sector;
    if(sector_no >= flash_sector_sum)
        return false;
    sector=(u16*)(sector_no*flash_sector_length+flash_base_addr);
    wait_end_39vfxxx();   //等待上一次操作完成
    atom_low = Int_LowAtomStart();  //此序列不能被针对此器件的其他操作打断
    Flash39VFxxxAddress[0x5555]=0xaa;
    Flash39VFxxxAddress[0x2aaa]=0x55;
    Flash39VFxxxAddress[0x5555]=0x80;
    Flash39VFxxxAddress[0x5555]=0xaa;
    Flash39VFxxxAddress[0x2aaa]=0x55;
    *sector = cmd_erase_sector;
//    wait_end_39vfxxx(); //不等待操作完成，而是在下一次使用前等待
                                    //这样可以节省时间。
    Int_LowAtomEnd(atom_low);
    return true;
}

//----擦除整片flash------------------------------------------------------------
//功能：擦除整片flash,入口:当前操作的flash地址
//参数：无
//全局：chip_addr，芯片基地址
//返回：无
//-----------------------------------------------------------------------------
void chip_erase_39vfxxx(void)
{
    atom_low_t atom_low;
    wait_end_39vfxxx();
    atom_low = Int_LowAtomStart();  //此序列不能被针对此器件的其他操作打断
    Flash39VFxxxAddress[0x5555]=0xaa;
    Flash39VFxxxAddress[0x2aaa]=0x55;
    Flash39VFxxxAddress[0x5555]=0x80;
    Flash39VFxxxAddress[0x5555]=0xaa;
    Flash39VFxxxAddress[0x2aaa]=0x55;
    Flash39VFxxxAddress[0x5555]=0x10;
    Int_LowAtomEnd(atom_low);
//    wait_end_39vfxxx();
}

//----写一个字节--------------------------------------------------------------
//功能：写Flash一个字节,写入地址应该是已经擦除的
//参数：addr,字对齐的写入地址,
//      Data:写入的字
//全局：chip_addr，芯片基地址
//返回：无
//备注：本函数的存在必要性有待商榷，因写flash的主要时间花在等待flash操作上，
//      故本函数加速作用非常有限，暂且留着吧
//---------------------------------------------------------------------------
bool_t byte_write_39vfxxx(u8 *addr,u8 data)
{
    bool_t result;
    u16 wd;
    u16 *wd_addr;
    wait_end_39vfxxx();
    Int_SaveAsynSignal();      //禁止调度也就是禁止异步事件
    wd_addr =(u16*)((u32)addr&(~1));
    if((u32)addr&1)
        wd = (data<<8) + 0xff;
    else
        wd = data+0xff00;
    Flash39VFxxxAddress[0x5555]=0xaa;
    Flash39VFxxxAddress[0x2aaa]=0x55;
    Flash39VFxxxAddress[0x5555]=0xa0;
    *wd_addr=wd;
    wait_end_39vfxxx();
    if((u32)addr&1)
        result = (u8)(*wd_addr>>8) == data;
    else
        result = (u8)*wd_addr == data;
    Int_RestoreAsynSignal();
    return result;
}

//----写一个字----------------------------------------------------------------
//功能：写Flash一个字,写入地址应该是已经擦除的
//参数：addr,字对齐的写入地址,
//      Data:写入的字
//全局：chip_addr，芯片基地址
//返回：无
//备注：本函数的存在必要性有待商榷，因写flash的主要时间花在等待flash操作上，
//      故本函数加速作用非常有限，暂且留着吧
//---------------------------------------------------------------------------
bool_t word_write_39vfxxx(u16 * addr,u16 data)
{
    bool_t result;
    wait_end_39vfxxx();
    Int_SaveAsynSignal();      //禁止调度也就是禁止异步事件
    Flash39VFxxxAddress[0x5555]=0xaa;
    Flash39VFxxxAddress[0x2aaa]=0x55;
    Flash39VFxxxAddress[0x5555]=0xa0;
    *addr=data;
    wait_end_39vfxxx();
    result = (*addr == data);
    Int_RestoreAsynSignal();
    return result;
}

//----写一个扇区--------------------------------------------------------------
//功能：写flash一个扇区并校验,该扇区是已经擦除的
//参数：sector_no:扇区号.
//      Buf:写入内容.
//全局：无
//返回：true=校验正确，false=校验错误或地址越界
//----------------------------------------------------------------------------
bool_t sector_write_39vfxxx(u16 sector_no,u8 *buf)
{
    int i;
    u16 *sector;
    u16 wd;
    if(sector_no >= flash_sector_sum)
        return false;
    sector=(u16*)(sector_no*flash_sector_length+flash_base_addr);
    wait_end_39vfxxx();
    for(i=0;i<flash_sector_length/2;i++)
    {
        wd = buf[2*i] + (buf[2*i+1]<<8);    //buf可能不是字对齐的地址
        word_write_39vfxxx(&sector[i],wd);
    }
    wait_end_39vfxxx();
    for(i=0;i<flash_sector_length/2;i++)
    {//特别提示，39vf160是纯16位的，不能做字节访问，最好是做字比较。
        wd = buf[2*i] + (buf[2*i+1]<<8);    //buf可能不是字对齐的地址
        if(sector[i] != wd)
            return false;
    }
    return true;
}

//----写一个块----------------------------------------------------------------
//写flash一个块,该块是已经擦除的
//参数：block_no:块号
//      Buf:写入内容
//全局：无
//返回：true=校验正确，false=校验错误或地址越界
//----------------------------------------------------------------------------
bool_t block_write_39vfxxx(u16 block_no,u8 *buf)
{
    int i;
    u16 *block;
    u16 wd;
    if(block_no >= flash_block_sum)
        return false;
    block=(u16*)(block_no*flash_block_length+flash_base_addr);
    wait_end_39vfxxx();

    for(i=0;i<flash_block_length/2;i++)
    {
        wd = buf[2*i] + (buf[2*i+1]<<8);    //buf可能不是字对齐的地址
        word_write_39vfxxx(&block[i],wd);
    }
    wait_end_39vfxxx();
    for(i=0;i<flash_block_length/2;i++)
    {//特别提示，39vf160是纯16位的，不能做字节访问，最好是做字比较。
        wd = buf[2*i] + (buf[2*i+1]<<8);    //buf可能不是字对齐的地址
        if(block[i] != wd)
            return false;
    }
    return true;
}

//----写flash---------------------------------------------------------------
//功能：写Flash,写入区域需事先擦除
//参数：start，起始地址.
//      len:写入长度,
//      buf:待写入的数据缓冲区
//全局：chip_addr，芯片基地址
//返回：true=正确写入，false=校验错误或地址越界。
//--------------------------------------------------------------------------
//注释备注：这是未经检查的代码。
//void write_flash(u8 * start,u32 length,u8 * buf)
//{
//    u16 * pBuf,*pFlash;
//    u32 i,j;
//    i=length;
//    if((u32)buf&1)
//    {
//        pFlash=(u16*)((u32)start-1);
//        byte_write_39vfxxx(pFlash,(*buf<<8)+0xff); //写未对准的第一个字节
//        pBuf=(u16*)((u32)buf+1);    //使地址字对准
//        i--;    //起始地址未字对准,已经写入1字节.长度减1字节
//    }else
//    {
//        pBuf=(u16*)((u32)buf);
//        pFlash=(u16*)((u32)start);
//    }
//    if (i&1)
//    {
//        byte_write_39vfxxx(&start[i-1],buf[length]+0xff00); //写未对准的最后一个字节
//        i--;    //长度加1后未字对准,已经写入1字节.长度减1字节
//    }
//    //至此,i为偶数,且Temp字对准地址
//    i=i/2;
//    for(j=0;j<i;j++)
//    {
//        byte_write_39vfxxx(&pFlash[j],pBuf[j]);
//    }
//}

//----改写flash---------------------------------------------------------------
//功能：改写Flash,自动判断是否需要擦除
//参数：start，起始地址.
//      len:写入长度,
//      buf:待写入的数据缓冲区
//全局：chip_addr，芯片基地址
//返回：true=正确写入，false=校验错误或地址越界。
//--------------------------------------------------------------------------
bool_t modify_flash(u8 * start,u32 length,u8 * buf)
{
    union
    {
        u16 buf_u16[flash_sector_length/2];
        u8 buf_u8[flash_sector_length];
    }tempbuf;
    u32 odd_len,i;
    u16 sector_offset,sector_no;
    u16 *sector_addr;

    if((u32)start+length > flash_base_addr+flash_size)
        return false;
    sector_addr=(u16*)((u32)start&(~(flash_sector_length-1)));
    sector_no = ((u32)sector_addr-flash_base_addr)/flash_sector_length;
    sector_offset=(u32)start&(flash_sector_length-1);
    if(sector_offset != 0)
        odd_len=flash_sector_length-sector_offset;
    else
        odd_len = 0;
    if(sector_offset != 0)
    {
        //读出起始地址所在扇区中本次写操作不覆盖的内容到缓冲区
        for (i=0;i<flash_sector_length/2;i++)
            tempbuf.buf_u16[i]=sector_addr[i];
        //检查有没有把0改成1的
        for(i=sector_offset;(i<flash_sector_length)&&(i-sector_offset<length);i++)
        {
            if((tempbuf.buf_u8[i] | buf[i-sector_offset]) != tempbuf.buf_u8[i])
            {//如果有从0改成1的位，擦除扇区
                sector_erase_39vfxxx(sector_no);
                break;
            }
        }
        //把写入的内容填写到起始扇区缓冲区中
        for (i=sector_offset;(i<flash_sector_length)&&(i-sector_offset<length);i++)
                    tempbuf.buf_u8[i]=buf[i-sector_offset];
        //写入起始扇区
        if( !sector_write_39vfxxx(sector_no,tempbuf.buf_u8) )
            return false;
        odd_len = 0;
        sector_no++;
        sector_addr += flash_sector_length/2;
    }
    //写入所有中间扇区
    while(odd_len+flash_sector_length<=length)
    {
        u16 wd1,wd2;
        for(i=0;i<flash_sector_length/2;i++)
        {
            wd1 = sector_addr[i];
            wd2 = buf[odd_len+2*i] + (buf[odd_len+2*i+1]<<8);//处理非字对齐的地址
            if((wd1 | wd2) != wd1)
            {//如果有从0改成1的位，擦除扇区
                sector_erase_39vfxxx(sector_no);
                break;
            }
        }
        if( !sector_write_39vfxxx(sector_no,&buf[odd_len]) )
            return false;
        odd_len+=flash_sector_length/2;
        sector_no++;
        sector_addr += flash_sector_length/2;
    }
    //写最后一页
    if(odd_len<length)
    {
        sector_addr+=flash_sector_length;
        sector_no++;
        for (i=0;i<flash_sector_length/2;i++)
            tempbuf.buf_u16[i]=sector_addr[i];
        for(i=0;i<(length-odd_len);i++)
        {
            if((tempbuf.buf_u8[i] | buf[i+odd_len]) != tempbuf.buf_u8[i])
            {//如果有从0改成1的位，擦除扇区
                sector_erase_39vfxxx(sector_no);
                break;
            }
        }
        for(i=0;i+odd_len<length;i++)
            tempbuf.buf_u8[i]=buf[i+odd_len];
        if( !sector_write_39vfxxx(sector_no,tempbuf.buf_u8) )
            return false;
    }
    return true;
}


