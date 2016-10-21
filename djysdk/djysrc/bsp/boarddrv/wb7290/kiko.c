// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：KiKo.c
// 模块描述: 开入开出驱动
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 10/05.2016
// =============================================================================

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "os.h"
#include "cpu_peri.h"
#include "kiko.h"
// BIT位定义
#define DB0                 (0x01)
#define DB1                 (0x02)
#define DB2                 (0x04)
#define DB3                 (0x08)
#define DB4                 (0x10)
#define DB5                 (0x20)
#define DB6                 (0x40)
#define DB7                 (0x80)
#define DB8                 (0x100)
#define DB9                 (0x200)
#define DB10                (0x400)
#define DB11                (0x800)
#define DB12                (0x1000)
#define DB13                (0x2000)
#define DB14                (0x4000)
#define DB15                (0x8000)
#define DB16                (0x10000)
#define DB17                (0x20000)
#define DB18                (0x40000)
#define DB19                (0x80000)
#define DB20                (0x100000)
#define DB21                (0x200000)
#define DB22                (0x400000)
#define DB23                (0x800000)
#define DB24                (0x1000000)
#define DB25                (0x2000000)
#define DB26                (0x4000000)
#define DB27                (0x8000000)
#define DB28                (0x10000000)
#define DB29                (0x20000000)
#define DB30                (0x40000000)
#define DB31                (0x80000000)

#define CPU_KO1     {PIO_PD10, PIOD, ID_PIOD, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO2     {PIO_PD11, PIOD, ID_PIOD, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO3     {PIO_PD13, PIOD, ID_PIOD, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO4     {PIO_PD14, PIOD, ID_PIOD, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO5     {PIO_PD17, PIOD, ID_PIOD, PIO_OUTPUT_1, PIO_DEFAULT}

#define CPU_KO6     {PIO_PD29, PIOD, ID_PIOD, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO7     {PIO_PC23, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO8     {PIO_PC24, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO9     {PIO_PC25, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO10    {PIO_PC26, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}

#define CPU_KO11    {PIO_PA25, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO12    {PIO_PC27, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO13    {PIO_PC28, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO14    {PIO_PC29, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO15    {PIO_PC30, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}

#define CPU_KO16    {PIO_PB1,  PIOB, ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO17    {PIO_PE1,  PIOE, ID_PIOE, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO18    {PIO_PC12, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO19    {PIO_PA23, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO20    {PIO_PD24, PIOD, ID_PIOD, PIO_OUTPUT_1, PIO_DEFAULT}

#define CPU_KO21    {PIO_PA19, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO22    {PIO_PD31, PIOD, ID_PIOD, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO23    {PIO_PA9,  PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO24    {PIO_PA10, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO25    {PIO_PA0,  PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}

#define CPU_KO26    {PIO_PD12, PIOD, ID_PIOD, PIO_OUTPUT_1, PIO_DEFAULT}
#define CPU_KO27    {PIO_PA1,  PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}

#define ENKO        {PIO_PA29, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}
#define KO_RUN		{PIO_PA20, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}

#define LED_A1		{PIO_PE0, PIOE, ID_PIOE, PIO_OUTPUT_1, PIO_DEFAULT}
#define LED_A2		{PIO_PE3, PIOE, ID_PIOE, PIO_OUTPUT_1, PIO_DEFAULT}
#define LED_A3		{PIO_PE4, PIOE, ID_PIOE, PIO_OUTPUT_1, PIO_DEFAULT}
#define LED_OUT		{PIO_PA18, PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}

static const Pin KO_Pins[] = {
        CPU_KO1,
        CPU_KO2,
        CPU_KO3,
        CPU_KO4,
        CPU_KO5,
        CPU_KO6,
        CPU_KO7,
        CPU_KO8,
        CPU_KO9,
        CPU_KO10,
        CPU_KO11,
        CPU_KO12,
        CPU_KO13,
        CPU_KO14,
        CPU_KO15,
        CPU_KO16,
        CPU_KO17,
        CPU_KO18,
        CPU_KO19,
        CPU_KO20,
        CPU_KO21,
        CPU_KO22,
        CPU_KO23,
        CPU_KO24,
        CPU_KO25,
        CPU_KO26,
        CPU_KO27,
        ENKO,
		KO_RUN
};


static const Pin Led_Pins[] = {
		LED_A1,
		LED_A2,
		LED_A3,
		LED_OUT,
};
static const Pin Cpu_Led[] = {
		{PIO_PA27, PIOA, ID_PIOA, PIO_OUTPUT_0,    PIO_DEFAULT}
};

#define MAX_KO_NUM   28     //最大开出量
#define MAX_KI_NUM   8      //最大开入量
#define MAX_CS_NUM   4      //开入的译码器输入引脚数

//=========================================================================
//#define SLOT_NUM     4
//static const u32 KoSlotMsk[SLOT_NUM] = {
//((1 << 19) | ( 1 << 24) | (1 << 25) | (1 << 26)),//KO20,KO25 K026 KO27
//(1 << 0) | (1 << 1) | ( 1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6)| (1 << 7),//KO1-KO8
//(1 << 8) | (1 << 9) | ( 1 << 10) | (1 << 11) | (1 << 12) | (1 << 13) | (1 << 14)| (1 << 15)
//| (1 << 16) | (1 << 17) | (1 << 20) | (1 << 21) | (1 << 22) | (1 << 23), //KO9-KO18 ,KO21-KO24
//        0x00000000,
//        0x00000000
//};


// =========================================================================
// 函数功能：开出引脚初始化
// 输入参数：无
// 输出参数：无
// 返回值  ：无
// =========================================================================
void KO_Init(void)
{
    PIO_Configure(KO_Pins,  PIO_LISTSIZE(KO_Pins));
    PIO_Configure(Led_Pins, PIO_LISTSIZE(Led_Pins));
    PIO_Configure(Cpu_Led,  PIO_LISTSIZE(Cpu_Led));
}

static void Led_Decode(u8 Code)
{
	u8 i,flag;

	for(i = 0; i < 3; i ++)
	{
		flag=((Code)>>i)&0x1;
		if(flag)
		{
			PIO_Set(&Led_Pins[i]);
		}
		else
		{
			PIO_Clear(&Led_Pins[i]);
		}
	}
}
// =========================================================================
// 函数功能：写开出值，开出值从低到高在一个32位的数组里面
// 输入参数：无
// 输出参数：无
// 返回值  ：无
// =========================================================================
void KO_Write(u32 *p)
{
    u32 *SlotPin = p;
    static u8  uledcnt=0;

    PIO_Clear(&KO_Pins[27]);        //ENKO 置低

    // 1号插槽出口 KO20,KO25 K026 KO27 （，，，KO_27 PWR脉冲[DB4]） LED1~8
    (SlotPin[0]&DB0 )?PIO_Clear(&KO_Pins[19]):PIO_Set(&KO_Pins[19]); // XJ1[DB0]
    (SlotPin[0]&DB1 )?PIO_Clear(&KO_Pins[24]):PIO_Set(&KO_Pins[24]); // XJ2[DB1]
    (SlotPin[0]&DB3 )?PIO_Clear(&KO_Pins[25]):PIO_Set(&KO_Pins[25]); // KO_FG[DB3]
//    (SlotPin[0]&DB4 )?PIO_Clear(&KO_Pins[26]):PIO_Set(&KO_Pins[26]); // KO_27(无用)
    (SlotPin[0]&DB4 )?PIO_Clear(&KO_Pins[28]):PIO_Set(&KO_Pins[28]); // PWR脉冲[DB4]
    	
	// LED DB8  ~ DB16  
	//(SlotPin[0]&DB8 )                    //run 灯被run信号点了
    if( (SlotPin[0]&DB9 )&&(uledcnt==0) )  		{Led_Decode(7);PIO_Clear(&Led_Pins[3]);}
    else if( (SlotPin[0]&DB10)&&(uledcnt==1) )  {Led_Decode(6);PIO_Clear(&Led_Pins[3]);}
    else if( (SlotPin[0]&DB11)&&(uledcnt==2) )  {Led_Decode(5);PIO_Clear(&Led_Pins[3]);}
    else if( (SlotPin[0]&DB12)&&(uledcnt==3) )  {Led_Decode(4);PIO_Clear(&Led_Pins[3]);}
    else if( (SlotPin[0]&DB13)&&(uledcnt==4) )  {Led_Decode(3);PIO_Clear(&Led_Pins[3]);}
    else if( (SlotPin[0]&DB14)&&(uledcnt==5) )  {Led_Decode(2);PIO_Clear(&Led_Pins[3]);}
    else if( (SlotPin[0]&DB15)&&(uledcnt==6) )  {Led_Decode(1);PIO_Clear(&Led_Pins[3]);}
    else if( (SlotPin[0]&DB16)&&(uledcnt==7) )  {Led_Decode(0);PIO_Clear(&Led_Pins[3]);}
    else										{PIO_Set(&Led_Pins[3]);				  }

	uledcnt++;
	uledcnt = uledcnt&7;

    // 2号插槽出口 KO1-KO8
    (SlotPin[1]&DB0 )?PIO_Clear(&KO_Pins[0]):PIO_Set(&KO_Pins[0]);
    (SlotPin[1]&DB1 )?PIO_Clear(&KO_Pins[1]):PIO_Set(&KO_Pins[1]);
    (SlotPin[1]&DB2 )?PIO_Clear(&KO_Pins[2]):PIO_Set(&KO_Pins[2]);
    (SlotPin[1]&DB3 )?PIO_Clear(&KO_Pins[3]):PIO_Set(&KO_Pins[3]);
    (SlotPin[1]&DB4 )?PIO_Clear(&KO_Pins[4]):PIO_Set(&KO_Pins[4]);
    (SlotPin[1]&DB5 )?PIO_Clear(&KO_Pins[5]):PIO_Set(&KO_Pins[5]);
    (SlotPin[1]&DB6 )?PIO_Clear(&KO_Pins[6]):PIO_Set(&KO_Pins[6]);
    (SlotPin[1]&DB7 )?PIO_Clear(&KO_Pins[7]):PIO_Set(&KO_Pins[7]);  
    
    // 3号插槽出口 KO9-KO18 ,KO21-KO24       	
    (SlotPin[2]&DB0 )?PIO_Clear(&KO_Pins[8 ]):PIO_Set(&KO_Pins[8 ]);
    (SlotPin[2]&DB1 )?PIO_Clear(&KO_Pins[9 ]):PIO_Set(&KO_Pins[9 ]);
    (SlotPin[2]&DB2 )?PIO_Clear(&KO_Pins[10]):PIO_Set(&KO_Pins[10]);
    (SlotPin[2]&DB3 )?PIO_Clear(&KO_Pins[11]):PIO_Set(&KO_Pins[11]);
    (SlotPin[2]&DB4 )?PIO_Clear(&KO_Pins[12]):PIO_Set(&KO_Pins[12]);
    (SlotPin[2]&DB5 )?PIO_Clear(&KO_Pins[13]):PIO_Set(&KO_Pins[13]);
    (SlotPin[2]&DB6 )?PIO_Clear(&KO_Pins[14]):PIO_Set(&KO_Pins[14]);
    (SlotPin[2]&DB7 )?PIO_Clear(&KO_Pins[15]):PIO_Set(&KO_Pins[15]);
    (SlotPin[2]&DB8 )?PIO_Clear(&KO_Pins[16]):PIO_Set(&KO_Pins[16]);
    (SlotPin[2]&DB9 )?PIO_Clear(&KO_Pins[17]):PIO_Set(&KO_Pins[17]);
    	
    (SlotPin[2]&DB10)?PIO_Clear(&KO_Pins[20]):PIO_Set(&KO_Pins[20]);
    (SlotPin[2]&DB11)?PIO_Clear(&KO_Pins[21]):PIO_Set(&KO_Pins[21]);
    (SlotPin[2]&DB12)?PIO_Clear(&KO_Pins[22]):PIO_Set(&KO_Pins[22]);
    (SlotPin[2]&DB13)?PIO_Clear(&KO_Pins[23]):PIO_Set(&KO_Pins[23]);


}
// =========================================================================
// 函数功能：启动继电器控制K019
// 输入参数：打开 关闭
// 输出参数：无
// 返回值  ：无
// =========================================================================
void KO_StartRly(u8 flag)
{
	if( flag )
	{
		PIO_Clear(&KO_Pins[18]);
	}
	else
	{
		PIO_Set(&KO_Pins[18]);
	}
}
// =========================================================================
// 函数功能：CPU板led控制
// 输入参数：打开 关闭
// 输出参数：无
// 返回值  ：无
// =========================================================================
void CpuLedCtrl(u8 flag,u8 Num)
{
	if( flag )
	{
		PIO_Set(&Cpu_Led[Num]);
	}
	else
	{
		PIO_Clear(&Cpu_Led[Num]);
	}
}

//================KI ===========================================================
#define KI_EBI_CS       2
#define KI_ADDR     EBI_CS2_ADDR

#define EBIDATA     {0xFF, PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP }    // D0-D7 管脚
#define EKI_CS      {PIO_PA22C_NCS2,PIOA, ID_PIOA, PIO_PERIPH_C, PIO_PULLUP}    // CS2  配置
#define KIAD1       {PIO_PC19, PIOC, ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT}
#define KIAD2       {PIO_PC20, PIOC, ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT}
#define KIAD3       {PIO_PC21, PIOC, ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT}
#define KIAD4       {PIO_PC22, PIOC, ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT}

static const Pin ki_pin[] = {
        EBIDATA,
        EKI_CS
};

const static Pin ki_cs[MAX_CS_NUM] = {
        KIAD1,
        KIAD2,
        KIAD3,
        KIAD4
};

static const Pin GPS_Pins[] = {
		{PIO_PA30, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}
};
//ezzyc:
static const Pin EZZYC_Pin[] = {
		{PIO_PB0, PIOB, ID_PIOB, PIO_INPUT, PIO_PULLUP}
};
//EQDJ1:
static const Pin EQDJ1_Pin[] = {
		{PIO_PC31, PIOC, ID_PIOC, PIO_INPUT, PIO_PULLUP}
};


// =========================================================================
// 函数功能：初始化IO引脚
// 输入参数：
// 输出参数：
// 返回值  ：
// =========================================================================
void KI_Init(void)
{
    PIO_Configure (ki_pin, PIO_LISTSIZE(ki_pin));   //总线和片选初始化
    PIO_Configure (ki_cs, PIO_LISTSIZE(ki_cs));     //译码器IO初始化

	// GPS
	PIO_Configure(GPS_Pins, PIO_LISTSIZE(GPS_Pins));

    PIO_Configure(EZZYC_Pin,PIO_LISTSIZE(EZZYC_Pin));
    
    PIO_Configure(EQDJ1_Pin,PIO_LISTSIZE(EQDJ1_Pin));
    
    PMC_EnablePeripheral(ID_SMC ) ;

    SMC->SMC_CS_NUMBER[KI_EBI_CS].SMC_SETUP = (0
                    | (SMC_SETUP_NWE_SETUP(3))
                    | (SMC_SETUP_NCS_WR_SETUP(2))
                    | (SMC_SETUP_NRD_SETUP(3))
                    | (SMC_SETUP_NCS_RD_SETUP(2))
                      );

    SMC->SMC_CS_NUMBER[KI_EBI_CS].SMC_PULSE = (0
                    | (SMC_PULSE_NWE_PULSE(5))
                    | (SMC_PULSE_NCS_WR_PULSE(5))

                    | (SMC_PULSE_NRD_PULSE(10))
                    | (SMC_PULSE_NCS_RD_PULSE(10))
                      );

    SMC->SMC_CS_NUMBER[KI_EBI_CS].SMC_CYCLE = (0
                    | (SMC_CYCLE_NWE_CYCLE(15))
                    | (SMC_CYCLE_NRD_CYCLE(30))
                      );

    SMC->SMC_CS_NUMBER[KI_EBI_CS].SMC_MODE = (0
                    | (SMC_MODE_DBW_8_BIT)
                    | (SMC_MODE_EXNW_MODE_DISABLED)
                    | (SMC_MODE_BAT_BYTE_WRITE)
                    | (SMC_MODE_READ_MODE)
                    | (SMC_MODE_WRITE_MODE)
                      );
}

static void KI_Decode(u8 SlotNo)
{
    u8 i,flag;

    for(i = 0; i < MAX_CS_NUM; i++)
    {
        flag=( SlotNo >> i )&0x1;
        if(flag)
        {
            PIO_Set(&ki_cs[i]);
        }
        else
        {
            PIO_Clear(&ki_cs[i]);
        }
    }
}

static u8 KiRead(u32 CS_Num)
{
    u8 value;
    KI_Decode(CS_Num);
    value = *((volatile unsigned char*)(KI_ADDR));
    value &= 0xff;

    return value;
}

// =============================================================================
// 功能：读对应槽号的开往值
// 参数：SlotNo,槽号
// 返回：读到的开入值，放入32bit的低8位
// =============================================================================
void KI_Read(u8 *buf)
{
    // for 0 槽
    buf[0] = ~KiRead( 0 );
    buf[1] = ~KiRead( 1 );
    buf[2] = ~KiRead( 2 );

    // for 1 槽
    buf[3] = ~KiRead( 3 );
    buf[4] = ~KiRead( 4 );
    buf[5] = ~KiRead( 5 );
    buf[6] = ~KiRead( 6 );
    
    // 按键值存放
    buf[7] = ~KiRead( 7 );

    // for 2 槽
    buf[8] = ~KiRead( 8 );
    buf[9] = ~KiRead( 9 );
    buf[10] = ~KiRead( 10 );
    buf[11] = ~KiRead( 11 );


    return ;
}


u8 GPS_Read(void)
{
	return PIO_Get(GPS_Pins);
}
u8 ZZYC_Read(void)
{
	return PIO_Get(EZZYC_Pin);
}
u8 QDJ1_Read(void)
{ 
	return PIO_Get(EQDJ1_Pin);
}



