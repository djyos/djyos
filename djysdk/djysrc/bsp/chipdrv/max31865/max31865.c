// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：max31865.c
// 模块描述: 温度检测芯片max31865的驱动
// 模块版本: V1.00
// 创建人员:
// 创建时间: 1/20.2017
// =============================================================================

#include "string.h"
#include "os.h"
#include "spibus.h"
#include "cpu_peri.h"
#include "max31865.h"

static struct SPI_Device s_Max3_Dev;
static u32 s_Max3_Timeout = CN_TIMEOUT_FOREVER;
#define Max3_SPI_SPEED      (200*1000)
static bool_t sMax3Inited = false;


static struct SemaphoreLCB Max3_Semp;   //芯片互斥访问保护
static u8 MaxSampleMode = MAX31865_CONTINOUS_MODE;

extern bool_t Max31865_DataReady(void);			//与板件相关，在board.c中
//max31865  温度
//static const Pin Max3_pin[] = {
//		{GPIO_A,PIN4,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_H,GPIO_PUPD_NONE,AF_NUll},//cs
//		{GPIO_A,PIN5,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_H,GPIO_PUPD_PU,AF5},
//		{GPIO_A,PIN6,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_H,GPIO_PUPD_PU,AF5},
//		{GPIO_B,PIN5,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_H,GPIO_PUPD_PU,AF5},
//		{GPIO_I,PIN11,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_H,GPIO_PUPD_NONE,AF_NUll},//data ready
//};

static bool_t Max31865_Ready(u32 WaitTime)
{
	u32 timecnt = 0;
	while(timecnt < WaitTime)
	{
		Djy_DelayUs(10);
//		if(!PIO_Get(&Max3_pin[4]))
		if(!Max31865_DataReady())
			return true;
		timecnt += 10;
	}
	return false;
}

static void Max31865_CsActive(void)
{
    SPI_CsActive(&s_Max3_Dev,s_Max3_Timeout);
//	PIO_Clear(&Max3_pin[0]);
}
static void Max31865_CsInActive(void)
{
    SPI_CsInactive(&s_Max3_Dev);
//	PIO_Set(&Max3_pin[0]);
}
static u32 Max31865_TxRx(u8* sdata,u32 slen,u8* rdata, u32 rlen,u32 RecvOff)
{
    struct SPI_DataFrame data;
    s32 result;

    data.RecvBuf = rdata;
    data.RecvLen = rlen;
    data.RecvOff = RecvOff;
    data.SendBuf = sdata;
    data.SendLen = slen;

    result = SPI_Transfer(&s_Max3_Dev,&data,true,s_Max3_Timeout);
    if(result != CN_SPI_EXIT_NOERR)
        return 0;
    return 1;
}

// =============================================================================
// 函数功能：触发一次采样
// 输入参数：无
// 输出参数：无
// 返回值  ：true，配置成功；false，配置不成功
// =============================================================================
static bool_t Max31865_TrigSingleSample(void)
{
	u8 cfgData[] = {0x80,0xA0};
	u8 cfgRead = 0x00,cfgcnt = 0;
	bool_t ret = false;

    do
    {
    	cfgData[0] = 0x80;
    	cfgData[1] = 0xA0;
        Max31865_CsActive();
        Max31865_TxRx(cfgData,2,NULL,0,0);
        Djy_EventDelay(15*1000);
        Max31865_CsInActive();

    	cfgData[0] = 0x00;
        Max31865_CsActive();
        Max31865_TxRx(cfgData,1,&cfgRead,1,1);
        Max31865_CsInActive();
        if(cfgRead == 0xA0)
        {
        	ret = true;
        	break;
        }
        cfgcnt ++;
        if(cfgcnt == 20)
        {
        	break;
        }
        Djy_EventDelay(1*mS);
    }while(1);

	return ret;
}

// =============================================================================
// 函数功能：配置Max采样为连续采样模式
// 输入参数：无
// 输出参数：无
// 返回值  ：true，配置成功；false，配置不成功
// =============================================================================
static bool_t Max31865_ContinousModeCfg(void)
{
	u8 cfgData[] = {0x80,0xC1};
	u8 cfgRead = 0x00,cfgcnt = 0;
	bool_t ret = false;

    do
    {
    	cfgData[0] = 0x80;
    	cfgData[1] = 0xC1;
        Max31865_CsActive();
        Max31865_TxRx(cfgData,2,NULL,0,0);
        Djy_EventDelay(15*mS);
        Max31865_CsInActive();

    	cfgData[0] = 0x00;
        Max31865_CsActive();
        Max31865_TxRx(cfgData,1,&cfgRead,1,1);
        Max31865_CsInActive();
        if(cfgRead == 0xC1)
        {
        	ret = true;
        	break;
        }
        cfgcnt ++;
        if(cfgcnt == 20)
        {
        	break;
        }
        Djy_EventDelay(10*mS);
    }while(1);

    return ret;
}

// =============================================================================
// 函数功能：判断是否有错误，并清错误标记
// 输入参数：无
// 输出参数：无
// 返回值  ：true，有错误；false，无错误
// =============================================================================
static bool_t Max31865_FaultAndClear(u8 *Reg)
{
	bool_t ret = false;
	u8 cfgData[2];

	if(Reg[7] & 0x04)					//错误标记
	{
		cfgData[0] = 0x80;
		cfgData[1] = Reg[0] & (~((1<<5) | (3<<2)));	//清标记
		cfgData[1] |= (1<<1);
        Max31865_CsActive();
        Max31865_TxRx(cfgData,2,NULL,0,0);
        Max31865_CsInActive();
        ret = true;
	}
	return ret;
}
// =============================================================================
// 函数功能：读温度(注意：函数类型为float，必须声名，否则返回值出错)
// 输入参数：无
// 返回值  ：温度值，为float类型；-1，表示出错
// =============================================================================
float Max31865_TemperatureGet(void)
{
	float temp;
	u8 RegMatrix[8] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};
	u8 Data[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    if(!sMax3Inited)
    {
        return -1;
    }

    if(false == Lock_SempPend(&Max3_Semp,25*mS))
    {
        return -1;
    }

    if(MaxSampleMode == MAX31865_SINGLE_MODE)
    {
    	if(false == Max31865_TrigSingleSample())
    	{
    		Lock_SempPost(&Max3_Semp);
    		return -1;
    	}
    }

    if(false == Max31865_Ready(500000))       //查忙，若超时则返回false
    {
    	Lock_SempPost(&Max3_Semp);
    	return -1;
    }

    Max31865_CsActive();
    Max31865_TxRx(RegMatrix,8,Data,8,1);
    Max31865_CsInActive();

    if(Max31865_FaultAndClear(Data))
    {
    	Lock_SempPost(&Max3_Semp);
    	return -1;
    }

    Lock_SempPost(&Max3_Semp);

    //计算温度
    temp = ((Data[1] <<8) + Data[2]) >> 1;
    temp = (temp / 32 - 256);

	return temp;
}

// =============================================================================
//功能：初始化Max31865芯片，若采用连续读模式，则配置其模式为连续采样
//参数：参数：
//     BusName,总结名称，如“SPI1”
//     Mode，连续模式或单次采样模式
//返回：true = 成功初始化，false = 初始化失败
// =============================================================================
bool_t ModuleInstall_Max31865(char *BusName,u8 SampleMode)
{
//	PIO_Configure(Max3_pin,PIO_LISTSIZE(Max3_pin));
//	PIO_Set(&Max3_pin[0]);

    if(NULL == Lock_SempCreate_s(&Max3_Semp,1,1,CN_BLOCK_FIFO,"MAX31865_semp"))
        return false;

    /* setup baud rate */
    s_Max3_Dev.AutoCs = false;
    s_Max3_Dev.CharLen = 8;
    s_Max3_Dev.Cs = 0;
    s_Max3_Dev.Freq = Max3_SPI_SPEED;
    s_Max3_Dev.Mode = SPI_MODE_1;
    s_Max3_Dev.ShiftDir = SPI_SHIFT_MSB;

    if(NULL != SPI_DevAdd_s(&s_Max3_Dev,BusName,"MAX31865"))
    {
        SPI_BusCtrl(&s_Max3_Dev,CN_SPI_SET_POLL,0,0);
        if(SampleMode == MAX31865_CONTINOUS_MODE)
        {
        	MaxSampleMode = MAX31865_CONTINOUS_MODE;
        	if(Max31865_ContinousModeCfg())
        	{
        		sMax3Inited = true;
        	}
        }
        else
        {
        	MaxSampleMode = MAX31865_SINGLE_MODE;
        	sMax3Inited = true;
        }
    }

    return sMax3Inited;
}
