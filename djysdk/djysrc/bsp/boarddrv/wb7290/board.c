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
// 文件名     ：board.c
// 模块描述: 板件相关部分初始化或配置等
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 5/11.2015
// =============================================================================

#include "stdint.h"
#include "stddef.h"
#include "os.h"
#include "cpu_peri.h"

//板件：DBG调试串口使用
static const Pin uart1_pin[] = {
    {PIO_PA5C_URXD1, PIOA, ID_PIOA, PIO_PERIPH_C, PIO_DEFAULT},
    {PIO_PA6C_UTXD1, PIOA, ID_PIOA, PIO_PERIPH_C, PIO_DEFAULT}};

//板件：print功能，连接打印机
static const Pin uart3_pin[] = {
    {PIO_PD28A_URXD3, 	PIOD, ID_PIOD, PIO_PERIPH_A, PIO_PULLUP},
    {PIO_PD30A_UTXD3, 	PIOD, ID_PIOD, PIO_PERIPH_A, PIO_PULLUP}
};

//板件使用485功能
static const Pin usart1_pin[] = {
    {PIO_PA21A_RXD1, 	PIOA, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP},
    {PIO_PB4D_TXD1,		PIOB, ID_PIOB, PIO_PERIPH_D, PIO_PULLUP},
	{PIO_PA24, 			PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}	//485 RX TX
};
static const Pin usart2_pin[] = {
    {PIO_PD15B_RXD2, 	PIOD, ID_PIOD, PIO_PERIPH_B, PIO_PULLUP},
    {PIO_PD16B_TXD2,	PIOD, ID_PIOD, PIO_PERIPH_B, PIO_PULLUP},
	{PIO_PD18, 			PIOD, ID_PIOD, PIO_OUTPUT_0, PIO_DEFAULT}	//485 RX TX
};


//AT45用到QSPI的SPI MODE
static const Pin Qspi_pin[] = {
		{PIO_PA11A_QCS,PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT},
		{PIO_PA13A_QIO0,PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT},
		{PIO_PA12A_QIO1,PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT},
//		{PIO_PA17A_QIO2,PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT},
//		{PIO_PD31A_QIO3,PIOD, ID_PIOD, PIO_PERIPH_A, PIO_DEFAULT},
		{PIO_PA14A_QSCK,PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT},
};

static const Pin Spi0_pin[] = {
		{PIO_PD20B_SPI0_MISO,PIOD, ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT},
		{PIO_PD21B_SPI0_MOSI,PIOD, ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT},
		{PIO_PD22B_SPI0_SPCK,PIOD, ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT},
};

static const Pin AT45_WP_pin [] = {
		{PIO_PE5, PIOE, ID_PIOE, PIO_OUTPUT_0, PIO_DEFAULT},
};

//定义IIC的GPIO引脚数组
static const Pin Twi0_Pins[]={
        {PIO_PA3A_TWD0,PIOA,ID_PIOA,PIO_PERIPH_A,PIO_DEFAULT},
        {PIO_PA4A_TWCK0,PIOA,ID_PIOA,PIO_PERIPH_A,PIO_DEFAULT}
};

static const Pin AT24_WP_pin [] = {
		{PIO_PA2,PIOA,ID_PIOA,PIO_OUTPUT_0,PIO_DEFAULT},
};

static const Pin Gmac_pin[] = {
		{(PIO_PD0A_GTXCK | PIO_PD1A_GTXEN | PIO_PD2A_GTX0 | PIO_PD3A_GTX1
		| PIO_PD4A_GRXDV | PIO_PD5A_GRX0 | PIO_PD6A_GRX1 | PIO_PD7A_GRXER
		| PIO_PD8A_GMDC | PIO_PD9A_GMDIO ),PIOD, ID_PIOD, PIO_PERIPH_A, PIO_DEFAULT},
};

static const Pin Phy_Interrupt_Pin[] = {
		{PIO_PC13, PIOC, ID_PIOC, PIO_INPUT,    PIO_IT_FALL_EDGE}
};

// ============================================================================
// 功能：设置串口编号为SerialNo的串口为半双功发送功能
// 参数：SerialNo,串口号
// 返回：无
// ============================================================================
void Board_UartHalfDuplexSend(u8 SerialNo)
{
	switch(SerialNo)
	{
	case CN_USART1:
		PIO_Set(&usart1_pin[2]);
		break;
	case CN_USART2:
		PIO_Set(&usart2_pin[2]);
		break;
	default:
		break;
	}
}

// ============================================================================
// 功能：设置串口编号为SerialNo的串口为半双功接收功能
// 参数：SerialNo,串口号
// 返回：无
// ============================================================================
void Board_UartHalfDuplexRecv(u8 SerialNo)
{
	//拉低为485接收
	switch(SerialNo)
	{
	case CN_USART1:
		PIO_Clear(&usart1_pin[2]);
		break;
	case CN_USART2:
		PIO_Clear(&usart2_pin[2]);
		break;
	default:
		break;
	}
}


extern u32 *pProgRomEnd;
void ProgRegionGet(u32 *pStart, u32 *pEnd)
{
	*pStart = 0x00400000;		//程序起始地址
	*pEnd = (u32)(&pProgRomEnd);	//程序结束地址
}

void Board_GpioInit(void)
{
	PIO_Configure(uart1_pin, PIO_LISTSIZE(uart1_pin));		//UART
	PIO_Configure(uart3_pin, PIO_LISTSIZE(uart3_pin));
	MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;
	PIO_Configure(usart1_pin,PIO_LISTSIZE(usart1_pin));
	PIO_Configure(usart2_pin, PIO_LISTSIZE(usart2_pin));

	PIO_Configure(Qspi_pin, PIO_LISTSIZE(Qspi_pin));		//AT45
	PIO_Configure(AT45_WP_pin, PIO_LISTSIZE(AT45_WP_pin));
	PIO_Set(AT45_WP_pin);

	PIO_Configure(Spi0_pin, PIO_LISTSIZE(Spi0_pin));		//SPI0

	PIO_Configure(Twi0_Pins,PIO_LISTSIZE(Twi0_Pins));		//AT24
	PIO_Configure(AT24_WP_pin,PIO_LISTSIZE(AT24_WP_pin));
	PIO_Clear(AT24_WP_pin);

	PIO_Configure(Gmac_pin, PIO_LISTSIZE(Gmac_pin));		//GMAC
//	extern void LS_IntHandler(void);
//	PIO_InitInterrupts(Phy_Interrupt_Pin);
//	PIO_ConfigureIt(Phy_Interrupt_Pin,LS_IntHandler);
//	PIO_EnableIt(Phy_Interrupt_Pin);

}
