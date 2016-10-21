//-----------------------------------------------------------------------------
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：cpu_peri_busram.c
// 模块描述: CPU片内backup 1K SRAM读写操作
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 09/11.2015
// =============================================================================

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "os.h"
#include "cpu_peri.h"

#define BACKUPSRAM_ADDR     (0x40074000)        //片内的1K Backup SRAM起始地址
#define BACKUPSRAM_SIZE     (0x400)             //1K字节的数据

// =============================================================================
// 功能：读片内的1K Bytes地址空间的数据，该区域数据掉电保护
// 参数：rBuffer，存储数据的缓冲区
//      offset，相对存储起始位置偏移，32位对齐
//      len，存储的数据长度
// 返回：写入的数据DWORD（4字节）数
// =============================================================================
u32 BackUpSram_Read(u8 *rBuffer,u32 offset,u32 len)
{
    u32 i,addr,dw;
    if( (NULL == rBuffer) || (offset + len >= BACKUPSRAM_SIZE) || (offset%4 != 0))
        return 0;

    addr = BACKUPSRAM_ADDR + offset;
    for(i = 0; i < len/4; i++)      //copy 整4倍数的部分
    {
        dw = *((u32*)(addr + i*4));
        rBuffer[4*i] = dw&0xff;
        rBuffer[4*i+1] = (dw>>8)&0xff;
        rBuffer[4*i+2] = (dw>>16)&0xff;
        rBuffer[4*i+3] = (dw>>24)&0xff;
    }
    
    dw = *((u32*)(addr + i*4));
    for(i = i*4; i < len; i++)      //copy剩余部分
    {
        rBuffer[i] = dw & 0xff;
        dw >>= 8;
    }
    return i;
}

// =============================================================================
// 功能：写片内的1K Bytes地址空间的数据，该区域数据掉电保护
// 参数：rBuffer，存储数据的缓冲区
//      offset，相对存储起始位置偏移，32位对齐
//      len，存储的数据长度
// 返回：写入的数据DWORD（4字节）数
// =============================================================================
u32 BackUpSram_Write(u8 *wBuffer,u32 offset,u32 len)
{
    u32 i,addr,dw;
    if( (NULL == wBuffer) || (offset + len >= BACKUPSRAM_SIZE) || (offset%4 != 0))
        return 0;

    addr = BACKUPSRAM_ADDR + offset;
    for(i = 0; i < len/4; i++)      //copy 整4倍数的部分
    {
        dw =  wBuffer[4*i]
            + (wBuffer[4*i+1]<<8)
            + (wBuffer[4*i+2]<<16)
            + (wBuffer[4*i+3]<<24);
        *(u32*)(addr+4*i) = dw;
    }
    
    for(i = i*4; i < len; i++)      //copy剩余部分
    {
        dw = wBuffer[i]<<(i*8);
    }
    *(u32*)(addr+(i&~0x3)) = dw;
    return i;
}













