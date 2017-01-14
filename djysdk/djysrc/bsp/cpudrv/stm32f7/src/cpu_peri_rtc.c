//-----------------------------------------------------------------------------
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块版本: V1.00
// 创建人员:czz
// 创建时间:2017
// =============================================================================

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os.h"
#include "cpu_peri.h"
#include "time.h"

#define HexToBcd(x) ((((x) / 10) <<4) + ((x) % 10))
#define BcdToHex(x) ((((x) & 0xF0) >>4) * 10 + ((x) & 0x0F))

static struct SemaphoreLCB *pRtcSemp  = NULL;
static s64  UpdateTime = 0;                     //需更新的时间

// =============================================================================
// 功能：RTC读后备区域SRAM
// 参数：BKPx:后备区寄存器编号,范围:0~19
// 返回：后备区域数据
// =============================================================================

u32 BKP_ReadBackupRegister(u32 BKRx)
{
	u32 temp=0;
	temp=RTC_BASE+0x50+BKRx*4;
	return (*(u32*)temp);		//返回读取到的值
}

// =============================================================================
// 功能：RTC写入后备区域SRAM
// 参数：BKPx:后备区寄存器编号,范围:0~19
//	   data:要写入的数据,32位长度
// 返回：true,正常操作，否则出错
// =============================================================================

void BKP_WriteBackupRegister(u32 BKRx,u32 data)
{
	u32 temp=0;
	temp=RTC_BASE+0x50+BKRx*4;
	(*(u32*)temp)=data;
}
// =============================================================================
// 功能：等待RSF同步 防止在同步时读取造成误差
// 参数：void
// 返回：
// =============================================================================
bool_t RTC_Wait_Rsf(void)
{
	u32 retry=0XFFFFF;

	RTC->WPR=0xCA;
	RTC->WPR=0x53;
	RTC->ISR&=~(1<<5);		//清除RSF位

	while(retry&&((RTC->ISR&(1<<5))==0x00))
	{
		retry--;
	}

    if(retry==0)
    	return false; //同步失败
	RTC->WPR=0xFF;	 //使能RTC寄存器写保护
	return true;
}
// =============================================================================
// 功能：从RTC进入初始化模式
// 参数：无
// 返回：true,正常操作，否则出错
// =============================================================================
static bool_t RTC_Init_Mode(void)
{
	u32 retry=0XFFFFF;
	if(RTC->ISR&(1<<6))
		return true;
	RTC->ISR|=1<<7;
	while(retry&&((RTC->ISR&(1<<6))==0x00))
		retry--;
    if(retry==0)
    	return false;
	return true;
}
// =============================================================================
// 功能：从RTC设备中读取RTC时间，单位微秒,取从1970年1月1日0:0:0到现在的时间差
// 参数：time, 时间值，需把日历时间转换成1970年1月1日0:0:0到现在的时间差
// 返回：true,正常操作，否则出错
// =============================================================================
bool_t Rtc_GetTime(s64 *time)
{
    struct tm dtm;
    u32 year,month,date,hour,min,sec;
    u32 DR_bak=0,TR_bak=0;

    u8 timeout=10;
	while(timeout&&(false==RTC_Wait_Rsf()))
	{
		timeout--;
	}
 	if(0==timeout)	//等待同步
 		return false;

 	DR_bak=RTC->DR;//将年月日。。一次读出来防止进位误差
 	TR_bak=RTC->TR;

	year=BcdToHex((DR_bak>>16)&0XFF)+1970;
	month=BcdToHex((DR_bak>>8)&0X1F);
	date=BcdToHex(DR_bak&0X3F);

	hour=BcdToHex((TR_bak>>16)&0X3F);
	min=BcdToHex((TR_bak>>8)&0X7F);
	sec=BcdToHex(TR_bak&0X7F);

    dtm.tm_year = year;
    dtm.tm_mon  = month;
    dtm.tm_mday = date;
    dtm.tm_hour = hour;
    dtm.tm_min  = min;
    dtm.tm_sec  = sec;

    *time = 1000000 * Tm_MkTime(&dtm);
    return true;
}

// =============================================================================
// 功能：设置RTC设备RTC时间，单位微秒，该时间从1970年1月1日0:0:0到现在的时间差
// 参数：time, 时间值
// 返回：true,正常操作，否则出错
// =============================================================================
bool_t Rtc_SetTime(s64 time)
{
    atom_low_t  atom_bak;

    atom_bak = Int_LowAtomStart();
    UpdateTime = time;
    Int_LowAtomEnd(atom_bak);
    Lock_SempPost(pRtcSemp);
    return true;
}

static bool_t __Rtc_SetTime(s64 time)
{
    struct tm dtm;
    s64 time_s;
    u8 tm_wday;
    time_s = time/1000000;
    Tm_LocalTime_r(&time_s,&dtm);

    //关闭RTC寄存器写保护
	RTC->WPR=0xCA;
	RTC->WPR=0x53;
	if(false==RTC_Init_Mode())
		return false;//进入RTC初始化模式失败

	if(dtm.tm_wday==0)
		tm_wday=7;
	else
		tm_wday=dtm.tm_wday;
	RTC->DR=(((u32)(tm_wday&0X07))<<13)|
			((u32)HexToBcd(dtm.tm_year-1970)<<16)|
			((u32)HexToBcd(dtm.tm_mon)<<8)|
			((u32)HexToBcd(dtm.tm_mday));

	RTC->TR=((u32)HexToBcd(dtm.tm_hour)<<16)|\
			((u32)HexToBcd(dtm.tm_min)<<8)|\
			(u32)(HexToBcd(dtm.tm_sec));

	RTC->ISR&=~(1<<7);			//退出RTC初始化模式

	return true;
}

// =============================================================================
// 功能：RTC时间更新任务，专门用一个低
//     优先级的任务作为更新RTC任务，以防占用其他线程的CPU时间
// 参数：无
// 返回：无
// =============================================================================
ptu32_t Rtc_UpdateTime(void)
{

    while(1)
    {
        if(Lock_SempPend(pRtcSemp,CN_TIMEOUT_FOREVER))
        {
            __Rtc_SetTime(UpdateTime);
        }
    }
    return 0;
}

// =============================================================================
// 功能：RTC硬件初始化首先检查LSE是否能可用如果不行则使用LSI时钟
// 参数：void
// 返回：true,正常操作，否则出错
// =============================================================================

static bool_t RTC_Configuration(void)
{

	u16 retry=200;

	RCC->APB1ENR|=1<<28;		//使能电源接口时钟
	PWR->CR1|=1<<8;				//后备区域访问使能(RTC+SRAM)

	RCC->CSR|=1<<0;				//LSI总是使能
	while(!(RCC->CSR&0x02));	//等待LSI就绪

	RCC->BDCR|=1<<0;			//尝试开启LSE
	while(retry&&((RCC->BDCR&0X02)==0))//等待LSE准备好
	{
		retry--;
		Djy_DelayUs(5);
	}

	RCC->BDCR&=~(3<<8);			//清零8/9位

	if(retry==0)
		RCC->BDCR|=1<<9;	//LSE开启失败,启动LSI.
	else
		RCC->BDCR|=1<<8;			//选择LSE,作为RTC时钟
	RCC->BDCR|=1<<15;				//使能RTC时钟

	if(0xA5A5!=BKP_ReadBackupRegister(1))
	{
		RTC->WPR=0xCA;	//关闭RTC寄存器写保护
		RTC->WPR=0x53;
		RTC->CR=0;
		if(false==RTC_Init_Mode())
		{
			RCC->BDCR=1<<16;		//复位BDCR
			Djy_DelayUs(10);
			RCC->BDCR=0;			//结束复位
			return 2;				//进入RTC初始化模式
		}
		RTC->PRER=0XFF;				//RTC同步分频系数(0~7FFF),必须先设置同步分频,再设置异步分频,Frtc=Fclks/((Sprec+1)*(Asprec+1))
		RTC->PRER|=0X7F<<16;		//RTC异步分频系数(1~0X7F)
		RTC->CR&=~(1<<6);			//RTC设置为,24小时格式
		RTC->ISR&=~(1<<7);			//退出RTC初始化模式
		RTC->WPR=0xFF;				//使能RTC寄存器写保护

		BKP_WriteBackupRegister(1,0xA5A5);			//标记已经初始化过了,使用LSE
	}
	return true;
}
// =============================================================================
// 功能：RTC时间注册及初始化
// 参数：time, 时间值
// 返回：true,正常操作，否则出错
// =============================================================================
ptu32_t ModuleInstall_CpuRtc(ptu32_t para)
{
    s64 rtc_time;
    struct timeval tv;
    u16 evtt;

	RTC_Configuration();    // 配置RTC
    //初始化硬件

    pRtcSemp = Lock_SempCreate(1,0,CN_BLOCK_FIFO,"RTC_SEMP");

    if(NULL == pRtcSemp)
        return false;

    evtt = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_REAL,0,0,
                            Rtc_UpdateTime,NULL,800,
                                "RTC Update Event");

    if(evtt == CN_EVTT_ID_INVALID)
    {
        free(pRtcSemp);
        return false;
    }
    Djy_EventPop(evtt,NULL,0,NULL,0,0);
    Rtc_GetTime(&rtc_time);

    tv.tv_sec = rtc_time/1000000;//us ---> s
    tv.tv_usec = 0;

    settimeofday(&tv,NULL);

    //注册RTC时间
    if(!Rtc_RegisterDev(NULL,Rtc_SetTime))
        return false;
    return true;
}





