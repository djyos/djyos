// =============================================================================
// Copyright (C) 2012-2020 都江堰操作系统研发团队  All Rights Reserved
// 文件名     ：cpu_peri_iic.c
// 模块描述: FRAM模块的器件驱动程序，由器件驱动程序员完成
// 模块版本: V1.00
// 创建人员: hm
// 创建时间: 20/06.2014
// =============================================================================
#include "stdint.h"
#include "cpu_peri.h"
#include "stdio.h"
#include "endian.h"
#include "fram_fm24cl64b.h"
#include "iicbus.h"
#include "endian.h"
#include "fram_fm24cl64b.h"

#define FRAM0_Address                 (0x50)
#define FRAM1_Address                 (0x54)//铁电设备的地址,根据物理地址定
#define FRAM_MAX_ADDR                 (0x20000) //铁电大小，字节单位

struct tagMutexLCB *  ptSemID_Fram0;       // IIC操作互斥标志
struct tagMutexLCB *  ptSemID_Fram1;

static struct tagIIC_Device pg_FRAM0_Dev;
static struct tagIIC_Device pg_FRAM1_Dev;

u32 timeout = CN_TIMEOUT_FOREVER;//2000*mS;//;100*mS

// =============================================================================
// 功能：读铁电指定地址开始Dstlen长度的数据
// 参数：fram_no,铁电号，为0或1
//       Fram_DstAddr，目标存储器预读取数据的地址
//       pDstBuf，存放接收到的数据缓存地址
//       DstLen， 预读取数据的长度
// 返回：ret, 实际读到的字节数
// =============================================================================
u32 FRAM_Read_Data(u8 fram_no,u32 Fram_DstAddr,u8 *pDstBuf,u32 DstLen)
{
    u32 ret = 0,TempLen;

    if(Fram_DstAddr + DstLen> FRAM_MAX_ADDR)
        return 0;
    if(fram_no == 1)
    {
        GPIO_CfgPinFunc(GPIO_3,cn_gpio_mode_out_ad);
        GPIO_SettoLow(GPIO_3);//拉低FRAM写保护引脚

        //若操作的区间跨越两个page
        if((Fram_DstAddr < 0x10000) && (Fram_DstAddr + DstLen > 0x10000))
        {
            TempLen = 0x10000 - Fram_DstAddr - 1;
            IIC_Read(&pg_FRAM0_Dev, Fram_DstAddr, pDstBuf, TempLen,timeout);

            IIC_Read(&pg_FRAM0_Dev, 0x10000, pDstBuf+TempLen, DstLen-TempLen,
                    timeout);
        }
        else
        {
            IIC_Read(&pg_FRAM0_Dev, Fram_DstAddr, pDstBuf, DstLen, timeout);
        }
    }
    else if(fram_no == 2)
    {
        GPIO_CfgPinFunc(GPIO_4,cn_gpio_mode_out_ad);
        GPIO_SettoLow(GPIO_4);//拉低FRAM写保护引脚

        //若操作的区间跨越两个page
        if((Fram_DstAddr < 0x10000) && (Fram_DstAddr + DstLen > 0x10000))
        {
            TempLen = 0x10000 - Fram_DstAddr - 1;
            IIC_Read(&pg_FRAM1_Dev, Fram_DstAddr, pDstBuf, TempLen,timeout);

            IIC_Read(&pg_FRAM1_Dev, 0x10000, pDstBuf+TempLen, DstLen-TempLen,
                    timeout);
        }
        else
        {
            IIC_Read(&pg_FRAM1_Dev, Fram_DstAddr, pDstBuf, DstLen, timeout);
        }
    }
    else
        ret = 0;
    return ret;
}
// =============================================================================
// 功能：写铁电指定地址开始Dstlen长度的数据
// 参数：fram_no,铁电号，为0或1
//      Fram_DstAddr，目标存储器预写数据地址
//      pSrcBuf，发送数据缓存地址
//      SrcLen， 发送数据长度
// 返回：ret, 实际写入的字节数
// =============================================================================
u32 FRAM_Write_Data(u8 fram_no, u32 Fram_DstAddr,u8 *pSrcBuf,u32 SrcLen)
{
    u32 ret = 0,TempLen;

    if(Fram_DstAddr + SrcLen > FRAM_MAX_ADDR)
        return 0;
    if(fram_no == CN_FRAM0_NO)
    {
        //配置写保护的引脚
        GPIO_CfgPinFunc(GPIO_3,cn_gpio_mode_out_ad);
        GPIO_SettoLow(GPIO_3);//拉低FRAM写保护引脚

        //若操作的区间跨越两个page
        if((Fram_DstAddr < 0x10000) && (Fram_DstAddr + SrcLen > 0x10000))
        {
            TempLen = 0x10000 - Fram_DstAddr - 1;
            IIC_Write(&pg_FRAM0_Dev, Fram_DstAddr, pSrcBuf, TempLen,1,timeout);

            IIC_Write(&pg_FRAM0_Dev, 0x10000, pSrcBuf+TempLen, SrcLen-TempLen,
                    0,timeout);
        }
        else
        {
            IIC_Write(&pg_FRAM0_Dev, Fram_DstAddr, pSrcBuf, SrcLen,0, timeout);
        }
    }
    else if(fram_no == CN_FRAM1_NO)
    {
        GPIO_CfgPinFunc(GPIO_4,cn_gpio_mode_out_ad);
        GPIO_SettoLow(GPIO_4);//拉低FRAM写保护引脚

        //若操作的区间跨越两个page
        if((Fram_DstAddr < 0x10000) && (Fram_DstAddr + SrcLen > 0x10000))
        {
            TempLen = 0x10000 - Fram_DstAddr - 1;
            IIC_Write(&pg_FRAM0_Dev, Fram_DstAddr, pSrcBuf, TempLen,1,timeout);

            IIC_Write(&pg_FRAM0_Dev, 0x10000, pSrcBuf+TempLen, SrcLen-TempLen,
                    0,timeout);
        }
        else
        {
            IIC_Write(&pg_FRAM0_Dev, Fram_DstAddr, pSrcBuf, SrcLen,0, timeout);
        }
    }
    else
        ret = 0;

    return ret;
}
// =============================================================================
// 功能：写字到铁电。读铁电指定地址双字长度的数据
// 参数：fram_no,铁电号，为0或1
//       Fram_DstAddr，目标存储器预读地址
// 返回：u32data,返回读到的数据
// =============================================================================
u32 FRAM_Read_DWord(u8 fram_no,u32 Fram_DstAddr)
{
    u32 u32data,buf[4];
    u32  rLen = 4;
    u32 ret = 0;

    ret = FRAM_Read_Data(fram_no,Fram_DstAddr,(u8 *)&buf,rLen);
    if(ret != rLen)
        return 0;
    //大小端转换
    u32data = pick_little_32bit((u8 *)&buf,0);
    return u32data;
}
// =============================================================================
// 功能：写字到铁电，写双字数据到指定地址的FRAM
// 参数：fram_no,铁电号，为0或1
//       Fram_DstAddr，目标存储器预写地址
// 返回：ret, 操作错误码，无错误为ERR_N0_ERROR
// =============================================================================
u32 FRAM_Write_DWord(u8 fram_no,u32 Fram_DstAddr,u32 u32data)
{
    u32  rLen = 4;
    u8 buf[4];
    u32 ret = 0;

    fill_little_32bit((u8 *)&buf,0,u32data);
    ret = FRAM_Write_Data(fram_no,Fram_DstAddr,(u8 *)&buf,rLen);
    return ret;
}

// =============================================================================
// 功能：读双字，读铁电指定地址双字长度的数据
// 参数：fram_no,铁电号，为0或1
//       Fram_DstAddr，目标存储器预读地址
// 返回：u16data,返回读到的数据
// =============================================================================
u16 FRAM_Read_Word(u8 fram_no,u32 Fram_DstAddr)
{
    u16 u16data,buf[2];
    u8  rLen = 2;
    u32 ret = 0;

    ret = FRAM_Read_Data(fram_no,Fram_DstAddr,(u8 *)&buf,rLen);
    if(ret != rLen)
        return 0;
    //从小端存储中读取到的数据，转化为大端模式
    u16data = pick_little_16bit((u8 *)&buf,0);
    return u16data;
}

// =============================================================================
// 功能：写字到铁电，写字数据到指定地址的FRAM
// 参数：fram_no,铁电号，为0或1
//       Fram_DstAddr，目标存储器预写地址
//       u16data,写入的数据
// 返回：ret, 操作错误码，无错误为ERR_N0_ERROR
// =============================================================================
u32 FRAM_Write_Word(u8 fram_no, u32 Fram_DstAddr,u16 u16data)
{
    u8  rLen = 2,buf[2];
    u32 ret = 0;

    //将大端改成小端 ，以小端方式存储在FRAM
    fill_little_16bit((u8 *)&buf,0,u16data);
    ret = FRAM_Write_Data(fram_no,Fram_DstAddr,(u8 *)&buf,rLen);
    return ret;
}
// =============================================================================
// 功能：带保护写字到铁电，按单个WORD读取铁电数据
// 参数：读取WORD地址
// 返回：读取后WORD数据
// =============================================================================
u16 FRAM_Read_Word_r(u8 fram_no, u16 wAddress )
{
    u16    wData = 0;

    if(fram_no == CN_FRAM0_NO)
    {
        if(-1 == Lock_MutexPend( ptSemID_Fram0, CN_TIMEOUT_FOREVER))  // 获取写权限
        {
            printf("FRAM1_Read_Word_r 获取写权限失败\n");
            return  false;
        }
        wData = FRAM_Read_Word(fram_no, wAddress );   // 写铁电
        Lock_MutexPost( ptSemID_Fram0 );
    }
    else if(fram_no == CN_FRAM1_NO)
    {
        if(-1 == Lock_MutexPend( ptSemID_Fram1, CN_TIMEOUT_FOREVER))   // 获取写权限
        {
            printf("FRAM2_Read_Word_r 获取写权限失败\n");
            return  false;
        }
        wData = FRAM_Read_Word(fram_no, wAddress);   // 写铁电
        Lock_MutexPost( ptSemID_Fram1 );
    }
    else
        ;

    return    wData;
}

// =============================================================================
// 功能：信号保护读数据，按多个u8读取铁电数据
// 参数：读取WORD地址, 保存缓冲区 读取长度
// 返回： 读取数据长度
// =============================================================================
u32 FRAM_Read_Data_r( u8 fram_no,u16 wAddress, u8 *pbyBuf, u32 dwLen )
{
    register u32      dwStatus = 0;

    if(fram_no == CN_FRAM0_NO)
    {
        if(-1 == Lock_MutexPend( ptSemID_Fram0, CN_TIMEOUT_FOREVER))   // 获取写权限
        {
            printf("FRAM1_Read_Data 获取写权限失败\n");
            return  false;
        }
        dwStatus = FRAM_Read_Data(fram_no, wAddress, pbyBuf, dwLen );// 写铁电
        Lock_MutexPost( ptSemID_Fram0 );
    }
    else if(fram_no == CN_FRAM1_NO)
    {
        if(-1 == Lock_MutexPend( ptSemID_Fram1, CN_TIMEOUT_FOREVER))   // 获取写权限
        {
            printf("FRAM2_Read_Data 获取写权限失败\n");
            return  false;
        }
        dwStatus = FRAM_Read_Data(fram_no, wAddress, pbyBuf, dwLen ); // 写铁电
        Lock_MutexPost( ptSemID_Fram1 );
    }
    else
        ;

    return    dwStatus;
}

// =============================================================================
// 功能：信号保护读字；按单个WORD写铁电数据
// 参数：读取WORD地址, 写入数据
// 返回：  写入数据长度
// =============================================================================
s32 FRAM_Write_Word_r( u8 fram_no,u16 wAddress, u16 wData )
{
    s32    Status = 0;

    if(fram_no == CN_FRAM0_NO)
    {
        if(-1 == Lock_MutexPend( ptSemID_Fram0, CN_TIMEOUT_FOREVER))       // 获取写权限
        {
            printf("FRAM1_Write_Word_r 获取写权限失败\n");
            return  false;
        }
        Status = FRAM_Write_Word(fram_no, wAddress, wData);   // 写铁电
        Lock_MutexPost( ptSemID_Fram0 );
    }
    else if(fram_no == CN_FRAM1_NO)
    {
        if(-1 == Lock_MutexPend( ptSemID_Fram1, CN_TIMEOUT_FOREVER))       // 获取写权限
        {
            printf("FRAM2_Write_Word_r 获取写权限失败\n");
            return  false;
        }
        Status = FRAM_Write_Word(fram_no, wAddress, wData);   // 写铁电
        Lock_MutexPost( ptSemID_Fram1 );
    }
    else
        ;

    return    Status;
}

// =============================================================================
// 功能：信号保护写数据，按多个u8写铁电数据
// 参数：读取WORD地址, 写入数据
// 返回：  写入数据长度
// =============================================================================
u32 FRAM_Write_Data_r(u8 fram_no, u16 wAddress, u8 *pbyBuf, u32 dwLen)
{
    register u32      dwStatus = 0;

    if(fram_no == CN_FRAM0_NO)
    {
        if(-1 == Lock_MutexPend( ptSemID_Fram0, CN_TIMEOUT_FOREVER))   // 获取写权限
        {
            printf("FRAM1_Write_Data_r 获取写权限失败\n");
            return  false;
        }
        dwStatus = FRAM_Write_Data(fram_no, wAddress, pbyBuf, dwLen );// 写铁电
        Lock_MutexPost( ptSemID_Fram0 );
    }
    else if(fram_no == CN_FRAM1_NO)
    {
        if(-1 == Lock_MutexPend( ptSemID_Fram1, CN_TIMEOUT_FOREVER))   // 获取写权限
        {
            printf("FRAM2_Write_Data_r 获取写权限失败\n");
            return  false;
        }
        dwStatus = FRAM_Write_Data(fram_no, wAddress, pbyBuf, dwLen );// 写铁电
        Lock_MutexPost( ptSemID_Fram1 );
    }
    else
        ;
    return    dwStatus;
}

// =============================================================================
// 功能：初始化fram芯片，调用IIC驱动初始化函数
// 参数: para,铁电号，0或1
// 返回: true= 成功，false=失败
// =============================================================================
ptu32_t ModuleInit_Fram(ptu32_t para)
{
    struct tagIIC_Device *Dev;
    char *BusName,*DevName;
    switch(para)
    {
    case 0:
        pg_FRAM0_Dev.DevAddr = FRAM0_Address;
        pg_FRAM0_Dev.BitOfMemAddrInDevAddr = 1;
        pg_FRAM0_Dev.BitOfMemAddr = 17;

        BusName = "IIC0";
        DevName = "FRAM0";
        Dev = &pg_FRAM0_Dev;
        IIC0_Init();

        ptSemID_Fram0 = Lock_MutexCreate("Fram0 Lock");
        break;
    case 1:
        pg_FRAM1_Dev.DevAddr = FRAM1_Address;
        pg_FRAM1_Dev.BitOfMemAddrInDevAddr = 1;
        pg_FRAM1_Dev.BitOfMemAddr = 17;

        BusName = "IIC1";
        DevName = "FRAM1";
        Dev = &pg_FRAM1_Dev;
        IIC1_Init();

        ptSemID_Fram1 = Lock_MutexCreate("Fram1 Lock");
        break;
    default:
        return false;
        break;
    }

    if(NULL == IIC_DevAdd_r(BusName,DevName,Dev))
        return false;
    else
        return true;
}



