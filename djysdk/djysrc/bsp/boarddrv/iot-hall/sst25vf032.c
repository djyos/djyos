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
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：SST25VF.c
// 模块描述: SPI FLASH 芯片驱动，调用了SPIBUS总线驱动模型
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 21/10.2015
// =============================================================================

#include "stdint.h"
#include "string.h"
#include "os.h"
#include "cpu_peri.h"
#include "spibus.h"
#include "sst25vf032.h"

// =============================================================================
//操作码定义
#define CMD_RDSR                    0x05  /* 读状态寄存器     */
#define CMD_WRSR                    0x01  /* 写状态寄存器     */
#define CMD_EWSR                    0x50  /* 使能写状态寄存器 */
#define CMD_WRDI                    0x04  /* 关闭写使能       */
#define CMD_WREN                    0x06  /* 打开写使能       */
#define CMD_READ                    0x03  /* 读数据           */
#define CMD_FAST_READ               0x0B  /* 快速读           */
#define CMD_BP                      0x02  /* 字节编程         */
#define CMD_AAIP                    0xAD  /* 自动地址增量编程 */
#define CMD_ERASE_4K                0x20  /* 扇区擦除:4K      */
#define CMD_ERASE_32K               0x52  /* 扇区擦除:32K     */
#define CMD_ERASE_64K               0xD8  /* 扇区擦除:64K     */
#define CMD_ERASE_FULL              0xC7  /* 全片擦除         */
#define CMD_JEDEC_ID                0x9F  /* 读 JEDEC_ID      */
#define CMD_EBSY                    0x70  /* 打开SO忙输出指示 */
#define CMD_DBSY                    0x80  /* 关闭SO忙输出指示 */

#define CMD_SE  CMD_ERASE_4K
#define CHIP_SIZE      (0x3FFFFF+1)
#define SECTOR_SIZE    4096
#define SECTOR_NUM     (CHIP_SIZE/SECTOR_SIZE)
// =============================================================================
#define SST25VF_CS_LOW  GPIO_SettoLow(GPIO_A,PIN4)
#define SST25VF_CS_HIGH GPIO_SettoHigh(GPIO_A,PIN4)
#define SST25VF_TIMEOUT (1*1000*1000)

struct SPI_Device s_SST25VF_Dev;

void __SST25VF_SpiInterfalce(u8* wBuf,u16 wLen,u8* rBuf,u16 rLen)
{
    struct SPI_DataFrame frame;

    frame.RecvBuf = rBuf;
    frame.RecvLen = rLen;
    frame.RecvOff = wLen;
    frame.SendBuf = wBuf;
    frame.SendLen = wLen;

    SST25VF_CS_LOW;
    SPI_Transfer(&s_SST25VF_Dev,&frame,1,SST25VF_TIMEOUT);
    SST25VF_CS_HIGH;
}

// =============================================================================
// 功能：读取FLASH的状态寄存器
//      BIT7    6    5   4   3   2   1   0
//      SPR    RV     TB     BP2   BP1  BP0 WEL  BUSY
//      SPR:默认0,状态寄存器保护位,配合WP使用
//      TB,BP2,BP1,BP0:FLASH区域写保护设置
//      WEL:写使能锁定
//      BUSY:忙标记位(1,忙;0,空闲)
//      默认:0x00
// 参数：无
// 返回：状态寄存器
// =============================================================================
u8 __SST25VF_ReadSR(void)
{
    u8 byte = 0;
    u8 cmd = CMD_RDSR;

    __SST25VF_SpiInterfalce(&cmd,1,&byte,1);
    return byte;
}

// =============================================================================
// 功能：写FLASH的状态寄存器，只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写
// 参数：无
// 返回：状态寄存器
// =============================================================================
void __SST25VF_WriteSR(u8 sr)
{
    u8 data[2] =  {CMD_EWSR,sr};

    __SST25VF_SpiInterfalce(data,1,NULL,0);
    data[0] = CMD_WRSR;
    __SST25VF_SpiInterfalce(data,2,NULL,0);
}

// =============================================================================
// 功能：等待空闲
// 参数：无
// 返回：无
// =============================================================================

void __SST25VF_WaitBusy(void)
{
    while((__SST25VF_ReadSR()&0x01)==0x01);   // 等待BUSY位清空
}

// =============================================================================
// 功能：写使能，将WEL置位
// 参数：无
// 返回：无
// =============================================================================
void __SST25VF_WriteEnable(void)
{
    u8 cmd = CMD_WREN;
    __SST25VF_SpiInterfalce(&cmd,1,NULL,0);
}

// =============================================================================
// 功能：写禁能，将WEL清零
// 参数：无
// 返回：无
// =============================================================================
void __SST25VF_WriteDisable(void)
{
    u8 cmd = CMD_WRDI;
    __SST25VF_SpiInterfalce(&cmd,1,NULL,0);
}

// =============================================================================
// 功能：打开SO忙输出指示
// 参数：无
// 返回：无
// =============================================================================
void __SST25VF_EBSY(void)
{
    u8 cmd = CMD_EBSY;
    __SST25VF_SpiInterfalce(&cmd,1,NULL,0);
}

// =============================================================================
// 功能：关闭SO忙输出指示
// 参数：无
// 返回：无
// =============================================================================
void __SST25VF_DBSY(void)
{
    u8 cmd = CMD_DBSY;
    __SST25VF_SpiInterfalce(&cmd,1,NULL,0);
}

// =============================================================================
// 功能：擦除一个扇区
// 参数：Addr:扇区大小 4K，扇区地址 0~511,擦除一个山区的最少时间:150ms
// 返回：无
// =============================================================================
void SST25VF_EraseSector(u32 Addr)
{
    u8 data[4] ;

    data[0] = CMD_SE;
    data[1] = (u8)((Addr)>>16);
    data[2] = (u8)((Addr)>>8);
    data[3] = (u8)((Addr)>>0);

    __SST25VF_WriteEnable();                  //SET WEL
    __SST25VF_WaitBusy();
    __SST25VF_SpiInterfalce(data,4,NULL,0);
    __SST25VF_WaitBusy();                      //等待擦除完成
}

// =============================================================================
// 功能：擦除整个芯片
// 参数：Addr:扇区大小 4K，扇区地址 0~511,擦除一个山区的最少时间:150ms
// 返回：无
// =============================================================================
void SST25VF_EraseChip(void)
{
    u8 cmd = CMD_ERASE_FULL;
    __SST25VF_WriteEnable();                  //SET WEL
    __SST25VF_WaitBusy();
    __SST25VF_SpiInterfalce(&cmd,1,NULL,0);
    __SST25VF_WaitBusy();                      //等待芯片擦除结束
}

// =============================================================================
// 功能：读取JEDEC-ID，为0xBF，0x25，0x4A
// 参数：调用者提供的缓冲区
// 返回：无
// =============================================================================
void SST25VF_ReadID(u8 *id)
{
    u8 cmd = CMD_JEDEC_ID;
    __SST25VF_SpiInterfalce(&cmd,1,id,3);
}

// =============================================================================
// 功能：读FLASH
// 参数：ReadAddr，读数据的地址，范围是0x00-0x3FFFFF
//      pBuffer，数据缓冲区
//      NumByteToRead，读数据的字节数
// 返回：无
// =============================================================================
bool_t SST25VF_Read(u32 ReadAddr,u8* pBuffer,u16 NumByteToRead)
{
    u8 data[] = {CMD_READ,(u8)((ReadAddr)>>16),(u8)((ReadAddr)>>8),(u8)ReadAddr};

    if ((ReadAddr + NumByteToRead > CHIP_SIZE-1)||(NumByteToRead == 0))
        return false;

     __SST25VF_SpiInterfalce(data,4,pBuffer,NumByteToRead);
     return true;
}


void __SST25VF_AutoIncProgramA(u8 Byte1, u8 Byte2, u32 Addr)
{
    u8 data[6] = {CMD_AAIP,(Addr & 0xFF0000) >> 16,(Addr & 0xFF00) >> 8,
        Addr & 0xFF,Byte1,Byte2};

    __SST25VF_WriteEnable();
    __SST25VF_SpiInterfalce(data,6,NULL,0);
    __SST25VF_WaitBusy();
}

void __SST25VF_AutoIncProgramB(u8 Byte1, u8 Byte2)
{
    u8 data[3] = {CMD_AAIP,Byte1,Byte2};

    __SST25VF_WriteEnable();

    __SST25VF_SpiInterfalce(data,3,NULL,0);
    __SST25VF_WaitBusy();
}

//
u8 SectorBuf[SECTOR_SIZE];
bool_t __SST25VF_SectorProgram(u16 sector,u32 offset,u8* pbuffer, u16 bytes)
{
    bool_t result = false;
    u32 i,secAddr;
    u8* pwBuf;

    if( (sector >= SECTOR_NUM) || (bytes == 0) || (pbuffer == NULL))
        return false;

    secAddr = sector*SECTOR_SIZE;
    //如果不是整片擦除第一个sector和最后一个sector,需读取sector，再擦除
    if( (offset) || (offset+bytes != SECTOR_SIZE))
    {
        SST25VF_Read(secAddr,SectorBuf,SECTOR_SIZE);
        memcpy(SectorBuf+offset,pbuffer,bytes);
        pwBuf = SectorBuf;
    }
    else
    {
        pwBuf = pbuffer;
    }

    //擦除整个sector
    SST25VF_EraseSector(secAddr);

    //写整个sector
    __SST25VF_AutoIncProgramA(pwBuf[0],pwBuf[1],secAddr);
    for(i = 1; i < SECTOR_SIZE/2; i++)
    {
        __SST25VF_AutoIncProgramB(pwBuf[i*2],pwBuf[i*2+1]);
    }

    __SST25VF_WriteDisable();
    return result;
}

// =============================================================================
// 功能：写FLASH
// 参数：wAddr,地址范围为0-0x3FFFFF
//      pbuffer,数据缓冲区
//      bytes,字节数
// 返回：true,正确，false,错误
// =============================================================================
bool_t SST25VF_Write(u32 wAddr,u8* pbuffer, u16 bytes)
{
    bool_t result = false;
    u16 i,temp;
    u16 secoff,secremain,sector;

    if( (wAddr + bytes > CHIP_SIZE-1) || (bytes == 0))
        return false;

    sector = wAddr / SECTOR_SIZE;       //扇区号
    secoff = wAddr % SECTOR_SIZE;       //扇区偏移
    secremain = SECTOR_SIZE - secoff;   //扇区余量

    if(bytes < secremain)
        temp = 1;
    else
    {
        i = bytes - secremain;                            /* 判断还占了几个扇区*/
        if (i % 4096 == 0)
           temp = i / 4096 + 1;
        else
           temp = i / 4096 + 2;
    }

    //写所有需要写的块
    for(i = 0; i < temp; i++)
    {
        if(i == 0)
        {
            __SST25VF_SectorProgram(sector,secoff,pbuffer,
                    (bytes>secremain)? secremain:bytes);
        }
        else if(i == temp-1)
        {
            __SST25VF_SectorProgram(sector+i,0,
                pbuffer+secremain+i*SECTOR_SIZE,(bytes-secremain)%SECTOR_SIZE);
        }
        else
        {
            __SST25VF_SectorProgram(sector+i,0,
                    pbuffer+secremain+i*SECTOR_SIZE,SECTOR_SIZE);
        }
    }
    return result;
}

// =============================================================================
// 功能：写FLASH指定地址一个字节数据
// 参数：wAddr,地址范围为0-0x3FFFFF
//      Data,字节数
// 返回：无
// =============================================================================
void SST25VF_WriteByte(u32 Addr, u8 Data)
{
    u32 sector;
    u8 data[5] = {CMD_BP,(Addr & 0xFF0000) >> 16,
                (Addr & 0xFF00) >> 8,Addr & 0xFF,Data};

//  sector = Addr / 4096;                /* 扇区地址 0~511 for w25x16  4096=4k */
//
//    SST25VF_Read(sector*SECTOR_SIZE,SectorBuf,SECTOR_SIZE);
//    SectorBuf[Addr-sector*SECTOR_SIZE] = Data;
//
//    SST25VF_EraseSector(sector);       /* 擦除这个扇区*/
    __SST25VF_WriteEnable();
    __SST25VF_SpiInterfalce(data,5,NULL,0);
    __SST25VF_WaitBusy();                          /* 等待写完成*/
}

// =============================================================================
// 功能：初始化函数，硬件初始化，装载设备并配置，验证ID
// 参数：无
// 返回：无
// =============================================================================
bool_t SST25VF_ModuleInit(ptu32_t para)
{
    u8 id[3];

    GPIO_CfgPinFunc(GPIO_A,PIN4,GPIO_MODE_OUT,GPIO_OTYPE_PP,
            GPIO_SPEED_100M,GPIO_PUPD_PU);//防止干扰

    s_SST25VF_Dev.Cs = 1;
    s_SST25VF_Dev.AutoCs = false;
    s_SST25VF_Dev.CharLen = 8;
    s_SST25VF_Dev.Mode = SPI_MODE_3;
    s_SST25VF_Dev.ShiftDir = SPI_SHIFT_MSB;
    s_SST25VF_Dev.Freq = 10*1000*1000;

    if(NULL != SPI_DevAdd_s(&s_SST25VF_Dev,"SPI1","SPI_Dev_SST25VF"))
    {
        SPI_BusCtrl(&s_SST25VF_Dev,CN_SPI_SET_POLL,0,0);
    }

    __SST25VF_WriteSR(0x02);                    /* 使能状态寄存器中的写存储器*/
    __SST25VF_DBSY();

    SST25VF_ReadID(id);
    printk("id = 0x%x,0x%x,0x%x\r\n",id[0],id[1],id[2]);
    if( (id[0] == 0xBF) && (id[1] == 0x25) && (id[2] == 0x4A))
    {
        return true;
    }
    else
        return false;
}





















