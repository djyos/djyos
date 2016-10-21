// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：AD7606.c
// 模块描述: AD7606采集模块的底层驱动，调用了SPI接口通信函数读取AD采集值
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 09/04.2015
// =============================================================================
#include "stdint.h"
#include "stddef.h"
#include "djyos.h"
#include "spibus.h"
#include "cpu_peri.h"
#include "iodefine.h"
// =============================================================================
#define AD7606_SSP_BAUD  10000000       //AD7606的SSP传输速度
#define AD7606_SSP_TIMEOUT (5*1000)     //ad7606 超时配置为5mS

struct SPI_Device s_AD7606_Dev;

// =============================================================================
// 功能：AD7606与CPU接口函数，采用SPI通信，硬件注意拉高拉低片选
// 参数：wBuf,写缓冲区
//      wLen,写数据长度，字节单位
//      rBuf,读缓冲区
//      rLen,读字节量
// 返回：无
// =============================================================================
void __AD7606_SpiInterfalce(u8* wBuf,u16 wLen,u8* rBuf,u16 rLen)
{
    struct SPI_DataFrame frame;

    frame.RecvBuf = rBuf;
    frame.RecvLen = rLen;
    frame.RecvOff = wLen;
    frame.SendBuf = wBuf;
    frame.SendLen = wLen;

    GPIO_SetToLow( ADCS_PORT, (1 << ADCS_PIN));     // 拉低片选信号
    SPI_Transfer(&s_AD7606_Dev,&frame,1,AD7606_SSP_TIMEOUT);
    GPIO_SetToHigh( ADCS_PORT, (1 << ADCS_PIN));        // 拉高片选信号
}
// =============================================================================
// 功能：初始化AD7606，包括引脚配置和初始化配置，接口的初始化等
// 参数：无
// 返回：无
// =============================================================================
void AD7606_Init(void)
{
    //设置AD7606是否采用过采样
    GPIO_CfgPinFunc(ADOS0_PORT,ADOS0_PIN,CN_GPIO_MODE_OD_PULLUP);
    GPIO_CfgPinFunc(ADOS1_PORT,ADOS1_PIN,CN_GPIO_MODE_OD_PULLUP);
    GPIO_CfgPinFunc(ADOS2_PORT,ADOS2_PIN,CN_GPIO_MODE_OD_PULLUP);

	//关闭过采样，即000
	GPIO_SetToOut(ADOS0_PORT,(1 << ADOS0_PIN));
	GPIO_SetToOut(ADOS1_PORT,(1 << ADOS1_PIN));
	GPIO_SetToOut(ADOS2_PORT,(1 << ADOS2_PIN));

	GPIO_SetToLow(ADOS2_PORT, (1 << ADOS2_PIN));
	GPIO_SetToLow(ADOS1_PORT, (1 << ADOS1_PIN));
	GPIO_SetToLow(ADOS0_PORT, (1 << ADOS0_PIN));

    //GPIO初始化，包括RST，SH，BUSY，并配置输入输出
    GPIO_CfgPinFunc(ADRST_PORT,ADRST_PIN,CN_GPIO_MODE_OD_PULLUP);
    GPIO_CfgPinFunc(ADCONVS_PORT,ADCONVS_PIN,CN_GPIO_MODE_OD_PULLUP);
    GPIO_CfgPinFunc(ADBUSY_PORT,ADBUSY_PIN,CN_GPIO_MODE_OD_PULLUP);

    GPIO_SetToOut(ADRST_PORT,(1 << ADRST_PIN));         //初始化过程RST状态
    GPIO_SetToHigh(ADRST_PORT, (1 << ADRST_PIN));

    GPIO_SetToOut( ADCONVS_PORT, (1 << ADCONVS_PIN));       //设置=0不启动AD转换
    GPIO_SetToLow( ADCONVS_PORT, (1 << ADCONVS_PIN));

    GPIO_SetToIn( ADBUSY_PORT, (1 << ADBUSY_PIN));      //BUSY管脚为输入

    GPIO_SetToLow(ADRST_PORT,(1 << ADRST_PIN));

    // 以下初始化与SSP相关
    //CS初始化
    GPIO_CfgPinFunc(ADCS_PORT,ADCS_PIN,CN_GPIO_MODE_OD_PULLUP);
    GPIO_SetToOut(ADCS_PORT,(1 << ADCS_PIN));

    //SSP控制器初始化
    s_AD7606_Dev.Cs = 1;
    s_AD7606_Dev.AutoCs = false;
    s_AD7606_Dev.CharLen = 8;
    s_AD7606_Dev.Mode = SPI_MODE_3;
    s_AD7606_Dev.ShiftDir = SPI_SHIFT_MSB;
    s_AD7606_Dev.Freq = AD7606_SSP_BAUD;

	if(NULL != SPI_DevAdd_s(&s_AD7606_Dev,"SSP0","SPI_Dev_AD7606"))
	{
		SPI_BusCtrl(&s_AD7606_Dev,CN_SPI_SET_POLL,0,0);
		SPI_BusCtrl(&s_AD7606_Dev,CN_SPI_SET_CLK,AD7606_SSP_BAUD,0);
	}
}

// =============================================================================
// 功能：启动AD开始采样，通过配置IO引脚的高低电平，并判断BUSY位
// 参数：无
// 返回：无
// =============================================================================
void __AD7606_Start(void)
{
    GPIO_SetToHigh( ADCONVS_PORT, (1 << ADCONVS_PIN));
    while( GPIO_DataGet( ADBUSY_PORT) & (1 << ADBUSY_PIN));
}

// =============================================================================
// 功能：停止AD转换，通过配置IO电平的方式配置
// 参数：无
// 返回：无
// =============================================================================
void __AD7606_Stop(void)
{
    GPIO_SetToLow( ADCONVS_PORT, (1 << ADCONVS_PIN));
}

// =============================================================================
// 功能：判断AD7606是否已经转换结束，AD7606不采用过采样的时间为4us，64倍数的过采样最大时间
//      为315us
// 参数：无
// 返回：无
// =============================================================================
static bool_t _AD7606_ConvFinished(void)
{
    u32 timeout = 500;
    u32 data;

	Djy_DelayUs(4);			//过采样的最大采样时间是315uS

    while(1)
    {
        data = GPIO_DataGet(ADBUSY_PORT);
        if( (!(data & (1 << ADBUSY_PIN))) || (timeout == 0) )
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
// 功能：获取AD7606的采集值，包括启动转换，转换结束后再读取AD转换的值
// 参数：无
// 返回：无
// =============================================================================
bool_t AD7606_GetValue(uint16_t *AinBuf)
{
    bool_t result = false;
    u8 *pAinBuf = (u8*)AinBuf;
    u8 temp,i;

    __AD7606_Start();
    if(_AD7606_ConvFinished())
    {
        __AD7606_SpiInterfalce(NULL,0,pAinBuf,16);
        result = true;
    }
    __AD7606_Stop();
    if(result == true)
    {
        if(CN_CFG_BYTE_ORDER == CN_CFG_LITTLE_ENDIAN)   //大小端转换
        {
            for(i = 0; i < 8; i ++)
            {
                temp = pAinBuf[2*i];
                pAinBuf[2*i] = pAinBuf[2*i + 1];
                pAinBuf[2*i + 1] = temp;
            }
        }
    }

    return result;
}
