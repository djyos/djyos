// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：brdwdt.c
// 模块描述: SR3200的看门狗，包括初始化和喂狗函数
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 09/04.2015
// =============================================================================

#if 0
#include <stdint.h>
#include "os.h"
#include <samv7/samv71.h>
#include "cpu_peri_int_line.h"
#include "cpu_peri_pio.h"
#include "supc.h"
#include "pmc.h"

//所有引脚都配置为开出，输出低电平
static const Pin AllPin[] = {
		{0xFFFFFFFF,PIOA,ID_PIOA,PIO_OUTPUT_0,PIO_DEFAULT},
		{0xFFFFFFFF,PIOB,ID_PIOB,PIO_OUTPUT_0,PIO_DEFAULT},
		{0xFFFFFFFF,PIOC,ID_PIOC,PIO_OUTPUT_0,PIO_DEFAULT},
		{0xFFFFFFFF,PIOD,ID_PIOD,PIO_OUTPUT_0,PIO_DEFAULT},
		{0xFFFFFFFF,PIOE,ID_PIOE,PIO_OUTPUT_0,PIO_DEFAULT},
};

#define SCR_SLEEPDEEP   (0x1 <<  2)
const uint32_t gWakeUpPinId = (1 << 7);			//唤醒功能Pin号
// 唤醒引脚配置为输入
static const Pin VccWkup_Pin[] = {
		{PIO_PA11, PIOA, ID_PIOA, PIO_INPUT,    PIO_DEFAULT}
};


// =============================================================================
// 功能：进入低功耗模式，先关闭电源监测功能（很重要，否则无法唤醒），将所有引脚配置为输出
//      低电平，将唤醒引脚配置为输入，并配置唤醒引脚
// 参数：无
// 返回：无
// 注释：唤醒采用唤醒引脚输入，唤醒引脚上出现上升沿时，CPU从back up mode唤醒
// =============================================================================
void BackupMode_Entry(void)
{
	u32 smmr;
	u32 SupcRegSr;

	SupcRegSr = SUPC->SUPC_SR ;

	//关闭 supply monitor功能
	smmr = SUPC_SMMR_SMTH(0x0A) | SUPC_SMMR_SMSMPL_SMD |
			SUPC_SMMR_SMRSTEN_NOT_ENABLE;

	//enable ext32k
	SUPC_ConfigSupplyMonitor(smmr);

	MATRIX->CCFG_SYSIO |= (1 << 4) | (1 << 5) |(1 << 6) |(1 << 7);
	PIO_Configure(AllPin,PIO_LISTSIZE(AllPin));

	/* Disable UTMI PLL clock */
	PMC->CKGR_UCKR &= ~CKGR_UCKR_UPLLEN;

	/* Disable PCK */
	PMC->PMC_SCDR = 0xFFFFFFFF;

	/* Disable all the peripheral clocks */
	PMC_DisableAllPeripherals();

	PIO_Configure(VccWkup_Pin,1);

	/* Enable the PIO for wake-up */
	SUPC->SUPC_WUIR = (gWakeUpPinId << 16) | gWakeUpPinId ;
	PMC->PMC_FSMR   = PMC_FSMR_FLPM_FLASH_DEEP_POWERDOWN | gWakeUpPinId;
	/* Set the SLEEPDEEP bit of Cortex-M processor. */
	SCB->SCR |= SCR_SLEEPDEEP ;
	/* Set the VROFF bit of SUPC_CR. */
	SUPC->SUPC_CR = SUPC_CR_KEY_PASSWD | SUPC_CR_VROFF_STOP_VREG;
	SUPC->SUPC_WUMR = SUPC_WUMR_WKUPDBC_3_SLCK;
}

u32 Supc_IRQHandler(ptu32_t para)
{
	BackupMode_Entry();
	Int_ClearLine(CN_INT_LINE_SUPC);
	return 0;
}

// =============================================================================
// 功能：低功耗模块模块配置，配置电压水平，配置中断，当电压降低到电压水平时，发生中断，在中断
//       服务函数中，进入back up mode
// 参数：
// 返回：
// =============================================================================
void BackupMode_Config(void)
{
	u32 smmr;

	u32 SupcRegSr;

	SupcRegSr = SUPC->SUPC_SR ;

	SUPC->SUPC_MR |= SUPC_MR_BKUPRETON;				//backup sram on

	//配置电压threshold、监测采样周期、复位使能、中断使能
	//电压降到2.68V以下时，产生中断
	//0x08 ----2.56V; 0x09 --- 2.68V ; 0x0A ---- 2.8V; 0x0B ----3.04V
	smmr = SUPC_SMMR_SMTH(0x0A) | SUPC_SMMR_SMSMPL_CSM |
			SUPC_SMMR_SMRSTEN_NOT_ENABLE | SUPC_SMMR_SMIEN;

	//enable ext32k
	SUPC_ConfigSupplyMonitor(smmr);

	//监测电压过低后，进入中断，配置进入BACKUPMODE
	Int_Register(CN_INT_LINE_SUPC);
    Int_IsrConnect(CN_INT_LINE_SUPC,Supc_IRQHandler);
    Int_SettoReal(CN_INT_LINE_SUPC);
    Int_ClearLine(CN_INT_LINE_SUPC);
    Int_RestoreRealLine(CN_INT_LINE_SUPC);
}
#endif


#if 1

#include <stdint.h>
#include <stdio.h>

#include <cpu_peri_int_line.h>
#include <samv7/samv71.h>
#include <cpu_peri_pio.h>

//所有引脚都配置为开出，输出低电平
static const Pin AllPin[] = {
		{0xFFFFFFFF,PIOA,ID_PIOA,PIO_OUTPUT_0,PIO_DEFAULT},
		{0xFFFFFFFF,PIOB,ID_PIOB,PIO_OUTPUT_0,PIO_DEFAULT},
		{0xFFFFFFFF,PIOC,ID_PIOC,PIO_OUTPUT_0,PIO_DEFAULT},
		{0xFFFFFFFF,PIOD,ID_PIOD,PIO_OUTPUT_0,PIO_DEFAULT},
		{0xFFFFFFFF,PIOE,ID_PIOE,PIO_OUTPUT_0,PIO_DEFAULT},
};

#define SCR_SLEEPDEEP   (0x1 <<  2)
const uint32_t gWakeUpPinId = (1 << 12);			//唤醒功能Pin号
// 唤醒引脚配置为输入
static const Pin VccWkup_Pin[] = {
		{PIO_PB3, PIOB, ID_PIOB, PIO_INPUT,    PIO_DEFAULT}
};

static const Pin LpEntry_Pin[] = {
		{PIO_PB3, PIOB, ID_PIOB, PIO_INPUT,    PIO_IT_FALL_EDGE}
};
// =============================================================================
// 功能：进入低功耗模式，先关闭电源监测功能（很重要，否则无法唤醒），将所有引脚配置为输出
//      低电平，将唤醒引脚配置为输入，并配置唤醒引脚
// 参数：无
// 返回：无
// 注释：唤醒采用唤醒引脚输入，唤醒引脚上出现上升沿时，CPU从back up mode唤醒
// =============================================================================
void BackupMode_Entry(void)
{
	MATRIX->CCFG_SYSIO |= (1 << 4) | (1 << 5) |(1 << 6) |(1 << 7);
	PIO_Configure(AllPin,PIO_LISTSIZE(AllPin));

	/* Disable UTMI PLL clock */
	PMC->CKGR_UCKR &= ~CKGR_UCKR_UPLLEN;

	/* Disable PCK */
	PMC->PMC_SCDR = 0xFFFFFFFF;

	/* Disable all the peripheral clocks */
	PMC_DisableAllPeripherals();

	PIO_Configure(VccWkup_Pin,1);

	/* Enable the PIO for wake-up */
	SUPC->SUPC_WUIR = (gWakeUpPinId << 16) | gWakeUpPinId ;
	PMC->PMC_FSMR   = PMC_FSMR_FLPM_FLASH_DEEP_POWERDOWN | gWakeUpPinId;
	/* Set the SLEEPDEEP bit of Cortex-M processor. */
	SCB->SCR |= SCR_SLEEPDEEP ;
	/* Set the VROFF bit of SUPC_CR. */
	SUPC->SUPC_CR = SUPC_CR_KEY_PASSWD | SUPC_CR_VROFF_STOP_VREG;
	SUPC->SUPC_WUMR = SUPC_WUMR_WKUPDBC_3_SLCK;
}

u32 Lp_IRQHandler(ptu32_t para)
{
	BackupMode_Entry();
//	Int_ClearLine(CN_INT_LINE_SUPC);
	return 0;
}

// =============================================================================
// 功能：低功耗模块模块配置，配置电压水平，配置中断，当电压降低到电压水平时，发生中断，在中断
//       服务函数中，进入back up mode
// 参数：
// 返回：
// =============================================================================
void BackupMode_Config(void)
{
	SUPC->SUPC_MR |= SUPC_MR_BKUPRETON;
	PIO_InitInterrupts(LpEntry_Pin);
	PIO_ConfigureIt(LpEntry_Pin,Lp_IRQHandler);

	Int_SettoReal(CN_INT_LINE_PIOB);
	PIO_EnableIt(LpEntry_Pin);
}

#endif
