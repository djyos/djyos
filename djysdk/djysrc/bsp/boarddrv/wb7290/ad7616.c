// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：AD7616.c
// 模块描述: AD7616采集模块的底层驱动，调用了SPI接口通信函数读取AD采集值
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 09/04.2015
// =============================================================================
#include "stdint.h"
#include "stddef.h"
#include "endian.h"
#include "djyos.h"
#include "spibus.h"
#include "cpu_peri.h"
//#include "iodefine.h"

static const Pin ADRST[] = {
    {PIO_PB5, PIOB, ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT},//ADRST
};

static const Pin ADSH[] = {
    {PIO_PB4, PIOB, ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT},//ADSH
};

static const Pin ADBUSY[] = {
    {PIO_PA16, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT},//ADBUSY
};
static const Pin ADCS[] = {
        {PIO_PA31, PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT},//ADCS
};
// =============================================================================
#define AD7616_SSP_BAUD     (20000000)      //AD7616的SSP传输速度
#define AD7616_SSP_TIMEOUT  (5*1000)        //AD7616 超时配置为5mS
#define AD7616_CHANNEL_NUM  (16)            //AD7616通道数量

struct SPI_Device s_AD7616_Dev;

// =============================================================================
// 功能：AD7616与CPU接口函数，采用SPI通信，硬件注意拉高拉低片选
// 参数：wBuf,写缓冲区
//      wLen,写数据长度，字节单位
//      rBuf,读缓冲区
//      rLen,读字节量
// 返回：无
// =============================================================================
void __AD7616_SpiInterfalce(u8* wBuf,u16 wLen,u8* rBuf,u16 rLen)
{
    struct SPI_DataFrame frame;

    frame.RecvBuf = rBuf;
    frame.RecvLen = rLen;
    frame.RecvOff = wLen;
    frame.SendBuf = wBuf;
    frame.SendLen = wLen;

    PIO_Clear(ADCS);     // 拉低片选信号
    SPI_Transfer(&s_AD7616_Dev,&frame,1,AD7616_SSP_TIMEOUT);
    PIO_Set(ADCS);        // 拉高片选信号
}
// =============================================================================
// 功能：初始化AD7616，包括引脚配置和初始化配置，接口的初始化等
// 参数：无
// 返回：无
// =============================================================================
void AD7616_Init(void)
{
    PMC_EnablePeripheral(ID_PIOA);
    PMC_EnablePeripheral(ID_PIOB);

    //将PB4 和PB5配置为通用IO
    MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4 | CCFG_SYSIO_SYSIO5;

    PIO_Configure(ADRST,PIO_LISTSIZE(ADRST));
    PIO_Configure(ADSH,PIO_LISTSIZE(ADSH));
    PIO_Configure(ADBUSY,PIO_LISTSIZE(ADBUSY));

    PIO_Clear(ADRST);

    Djy_DelayUs(2*mS);
    PIO_Clear(ADSH);                //设置=0不启动AD转换
    PIO_Set(ADRST);                 //初始化过程RST状态
    Djy_DelayUs(16*mS);             //FULL RESET >= 15mS
    // 以下初始化与SSP相关
    //CS初始化
    PIO_Configure(ADCS,1);
    PIO_Set(ADCS);

    //SSP控制器初始化
    s_AD7616_Dev.Cs = CN_SPINCS1;
    s_AD7616_Dev.AutoCs = false;
    s_AD7616_Dev.CharLen = 8;
    s_AD7616_Dev.Mode = SPI_MODE_3;
    s_AD7616_Dev.ShiftDir = SPI_SHIFT_MSB;
    s_AD7616_Dev.Freq = AD7616_SSP_BAUD;

    if(NULL != SPI_DevAdd_s(&s_AD7616_Dev,"SPI0","SPI_Dev_AD7616"))
    {
        SPI_BusCtrl(&s_AD7616_Dev,CN_SPI_SET_POLL,0,0);
    }
}

// =============================================================================
// 功能：停止AD转换，通过配置IO电平的方式配置
// 参数：无
// 返回：无
// =============================================================================
//void __AD7616_Stop(void)
//{
//    PIO_Clear(ADSH);
//}

// =============================================================================
// 功能：启动AD开始采样，通过配置IO引脚的高低电平，并判断BUSY位
// 参数：无
// 返回：无
// =============================================================================
void AD7616_Start(void)
{
    u32 i = 0;
    PIO_Set(ADSH);
//    while(!PIO_Get(ADBUSY));
    for(i = 0; i < 0x2; i++)    //AD7616的问题，需延时100ns时间，立即停止CONV
    {

    }
    PIO_Clear(ADSH);
//  __AD7616_Stop();
}

// =============================================================================
// 功能：判断AD7616是否已经转换结束，AD7616不采用过采样的时间为4us，64倍数的过采样最大时间
//      为315us
// 参数：无
// 返回：无
// =============================================================================
static bool_t _AD7616_ConvFinished(void)
{
    u32 timeout = 10;

//  Djy_DelayUs(2);         //过采样的最大采样时间是315uS

    while(timeout)
    {
        if(!PIO_Get(ADBUSY))
            break;
        Djy_DelayUs(1);
        timeout--;
    }
    if(timeout)
        return true;
    else
        return false;
}

// =============================================================================
// 功能：获取AD7616的采集值，不包括启动转换，转换结束后再读取AD转换的值
// 参数：无
// 返回：无
// =============================================================================
bool_t AD7616_GetValue(uint16_t *AinBuf)
{
    bool_t result = false;
    u16 i;

//  __AD7616_Start();
    if(_AD7616_ConvFinished())
    {
//          __AD7616_Stop();
        __AD7616_SpiInterfalce(NULL,0,(u8*)AinBuf,AD7616_CHANNEL_NUM*2);
        result = true;
#if(CN_CFG_BYTE_ORDER == CN_CFG_LITTLE_ENDIAN)   //小端需要转换
        for(i = 0; i < AD7616_CHANNEL_NUM; i ++)
        {
            AinBuf[i] = swaps(AinBuf[i]);
        }
#endif
        return true;
    }
    else
        return false;
}


// =============================================================================
// 功能：获取AD7616的采集值，包括启动转换，转换结束后再读取AD转换的值
// 参数：无
// 返回：无
// =============================================================================
bool_t AD7616_Convert(uint16_t *AinBuf)
{
    bool_t result = false;
    u16 i;

  __AD7616_Start();
    if(_AD7616_ConvFinished())
    {
//          __AD7616_Stop();
        __AD7616_SpiInterfalce(NULL,0,(u8*)AinBuf,AD7616_CHANNEL_NUM*2);
        result = true;
#if(CN_CFG_BYTE_ORDER == CN_CFG_LITTLE_ENDIAN)   //小端需要转换

        for(i = 0; i < AD7616_CHANNEL_NUM; i ++)
        {
            AinBuf[i] = swaps(AinBuf[i]);
        }
#endif
        return true;
    }
    else
        return false;
}

