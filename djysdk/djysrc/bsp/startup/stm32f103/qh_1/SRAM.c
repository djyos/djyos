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
#include "stdint.h"
#include "cpu_peri.h"
#include "stm32f10x.h"
#include "SRAM.h"

#if 0
void SRAM_GPIO_Init(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE );

    GPIO_PowerOn(CN_GPIO_D);
    GPIO_PowerOn(CN_GPIO_E);
    GPIO_PowerOn(CN_GPIO_F);
    GPIO_PowerOn(CN_GPIO_G);

    GPIO_CfgPinFunc(CN_GPIO_D, 14, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);   //d0
    GPIO_CfgPinFunc(CN_GPIO_D, 15, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);   //d1
    GPIO_CfgPinFunc(CN_GPIO_D, 0, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //d2
    GPIO_CfgPinFunc(CN_GPIO_D, 1, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //d3
    GPIO_CfgPinFunc(CN_GPIO_E, 7, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //d4
    GPIO_CfgPinFunc(CN_GPIO_E, 8, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //d5
    GPIO_CfgPinFunc(CN_GPIO_E, 9, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //d6
    GPIO_CfgPinFunc(CN_GPIO_E, 10, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);   //d7
    GPIO_CfgPinFunc(CN_GPIO_E, 11, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);   //d8
    GPIO_CfgPinFunc(CN_GPIO_E, 12, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);   //d9
    GPIO_CfgPinFunc(CN_GPIO_E, 13, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);   //d10
    GPIO_CfgPinFunc(CN_GPIO_E, 14, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);   //d11
    GPIO_CfgPinFunc(CN_GPIO_E, 15, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);   //d12
    GPIO_CfgPinFunc(CN_GPIO_D, 8, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //d13
    GPIO_CfgPinFunc(CN_GPIO_D, 9, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //d14
    GPIO_CfgPinFunc(CN_GPIO_D, 10, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);   //d15

    GPIO_CfgPinFunc(CN_GPIO_F, 0, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //a0
    GPIO_CfgPinFunc(CN_GPIO_F, 1, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //a1
    GPIO_CfgPinFunc(CN_GPIO_F, 2, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //a2
    GPIO_CfgPinFunc(CN_GPIO_F, 3, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //a3
    GPIO_CfgPinFunc(CN_GPIO_F, 4, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //a4
    GPIO_CfgPinFunc(CN_GPIO_F, 5, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //a5
    GPIO_CfgPinFunc(CN_GPIO_F, 12, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);   //a6
    GPIO_CfgPinFunc(CN_GPIO_F, 13, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);   //a7
    GPIO_CfgPinFunc(CN_GPIO_F, 14, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);   //a8
    GPIO_CfgPinFunc(CN_GPIO_F, 15, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);   //a9
    GPIO_CfgPinFunc(CN_GPIO_G, 0, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //a10
    GPIO_CfgPinFunc(CN_GPIO_G, 1, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //a11
    GPIO_CfgPinFunc(CN_GPIO_G, 2, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //a12
    GPIO_CfgPinFunc(CN_GPIO_G, 3, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //a13
    GPIO_CfgPinFunc(CN_GPIO_G, 4, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //a14
    GPIO_CfgPinFunc(CN_GPIO_G, 5, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //a15
    GPIO_CfgPinFunc(CN_GPIO_D, 11, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);   //a16
    GPIO_CfgPinFunc(CN_GPIO_D, 12, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);   //a17
    GPIO_CfgPinFunc(CN_GPIO_D, 13, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);   //a18
    GPIO_CfgPinFunc(CN_GPIO_E, 3, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //a19
    GPIO_CfgPinFunc(CN_GPIO_E, 4, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //a20
    GPIO_CfgPinFunc(CN_GPIO_E, 5, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //a21

    GPIO_CfgPinFunc(CN_GPIO_D, 5, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //we
    GPIO_CfgPinFunc(CN_GPIO_D, 4, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //re
    GPIO_CfgPinFunc(CN_GPIO_D, 7, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //cs1 for sram
    GPIO_CfgPinFunc(CN_GPIO_G, 9, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //cs2 for nand
    GPIO_CfgPinFunc(CN_GPIO_G, 10, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);   //cs3 for lcd
    GPIO_CfgPinFunc(CN_GPIO_G, 12, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);   //cs4 for dm9000a


    GPIO_CfgPinFunc(CN_GPIO_E, 0, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //lb
    GPIO_CfgPinFunc(CN_GPIO_E, 1, CN_GPIO_MODE_PERI_OUT_PP_50Mhz);    //ub

}
#else
void SRAM_GPIO_Init(void)
{
	RCC->AHBENR|=1<<8;     	 	//使能FSMC时钟
	RCC->APB2ENR|=1<<0;
	RCC->APB2ENR|=1<<5;     	//使能PORTD时钟
	RCC->APB2ENR|=1<<6;     	//使能PORTE时钟
	RCC->APB2ENR|=1<<7;     	//使能PORTF时钟
 	RCC->APB2ENR|=1<<8;      	//使能PORTG时钟

	//PORTD复用推挽输出
	GPIOD->CRH &=0X00000000;		//8,9,10,11,12,13,14,15
	GPIOD->CRH |=0XBBBBBBBB;
	GPIOD->CRL &=0X0F00FF00;		//0,1,4,5,7
	GPIOD->CRL |=0XB0BB00BB;

	//PORTE复用推挽输出
	GPIOE->CRH &=0X00000000;		//8~15
	GPIOE->CRH |=0XBBBBBBBB;
	GPIOE->CRL &=0X0F000F00;		//0,1,3,4,5,7
	GPIOE->CRL |=0XB0BBB0BB;

	//PORTF复用推挽输出
	GPIOF->CRH &=0X0000FFFF;		//12,13,14,15
	GPIOF->CRH |=0XBBBB0000;
	GPIOF->CRL &=0XFF000000;		//0,1,2,3,4,5
	GPIOF->CRL |=0X00BBBBBB;

	GPIOG->CRH&=0XFFF0F00F;			//9,10,12
	GPIOG->CRH|=0X000B0BB0;
	GPIOG->CRL&=0XFF000000;			//0,1,2,3,4,5
	GPIOG->CRL|=0X00BBBBBB;
}

#endif

void SRAM_FSMCConfig(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  p;

    /*-- FSMC Configuration ------------------------------------------------------*/
    /*----------------------- SRAM Bank 1 ----------------------------------------*/
      /* FSMC_Bank1_NORSRAM1 configuration */

    p.FSMC_AddressSetupTime = 1;
    p.FSMC_AddressHoldTime = 0;
    p.FSMC_DataSetupTime = 2;
    p.FSMC_BusTurnAroundDuration = 0;
    p.FSMC_CLKDivision = 0;
    p.FSMC_DataLatency = 1;
    p.FSMC_AccessMode = FSMC_AccessMode_A;

    /* Color LCD configuration ------------------------------------
       LCD configured as follow:
          - Data/Address MUX = Disable
          - Memory Type = SRAM
          - Data Width = 16bit
          - Write Operation = Enable
          - Extended Mode = Enable
          - Asynchronous Wait = Disable */
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_PSRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

    /* BANK1 (of NOR/SRAM Bank) is enabled */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

void LCD_FSMCConfig(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
     FSMC_NORSRAMTimingInitTypeDef  p;
    /*-- FSMC Configuration ------------------------------------------------------*/
    /*----------------------- SRAM Bank 3 ----------------------------------------*/
      /* FSMC_Bank1_NORSRAM3 configuration */
      p.FSMC_AddressSetupTime = 1;
      p.FSMC_AddressHoldTime = 0;
      p.FSMC_DataSetupTime = 2;
      p.FSMC_BusTurnAroundDuration = 0;
      p.FSMC_CLKDivision = 0;
      p.FSMC_DataLatency = 0;
      p.FSMC_AccessMode = FSMC_AccessMode_A;
      /* Color LCD configuration ------------------------------------
         LCD configured as follow:
            - Data/Address MUX = Disable
            - Memory Type = SRAM
            - Data Width = 16bit
            - Write Operation = Enable
            - Extended Mode = Enable
            - Asynchronous Wait = Disable */
      FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
      FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
      FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
      FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
      FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
      FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
      FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
      FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
      FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
      FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
      FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
      FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
      FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
      FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
      FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;
      FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
      /* BANK 3 (of NOR/SRAM Bank) is enabled */
      FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);
}

void NandFlash_FSMCConfig(void)
{
  FSMC_NANDInitTypeDef FSMC_NANDInitStructure;
  FSMC_NAND_PCCARDTimingInitTypeDef  p;

  /*-- FSMC Configuration ------------------------------------------------------*/
  p.FSMC_SetupTime = 0x0;
  p.FSMC_WaitSetupTime = 0x3;
  p.FSMC_HoldSetupTime = 0x9;
  p.FSMC_HiZSetupTime = 0x0;

  FSMC_NANDInitStructure.FSMC_Bank = FSMC_Bank3_NAND;
  FSMC_NANDInitStructure.FSMC_Waitfeature = FSMC_Waitfeature_Enable;
  FSMC_NANDInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
  FSMC_NANDInitStructure.FSMC_ECC = FSMC_ECC_Enable;
  FSMC_NANDInitStructure.FSMC_ECCPageSize = FSMC_ECCPageSize_512Bytes;
  FSMC_NANDInitStructure.FSMC_TCLRSetupTime = 0x00;
  FSMC_NANDInitStructure.FSMC_TARSetupTime = 0x00;
  FSMC_NANDInitStructure.FSMC_CommonSpaceTimingStruct = &p;
  FSMC_NANDInitStructure.FSMC_AttributeSpaceTimingStruct = &p;

  FSMC_NANDInit(&FSMC_NANDInitStructure);

  /*!< FSMC NAND Bank Cmd Test */
  FSMC_NANDCmd(FSMC_Bank3_NAND, ENABLE);
}

void Dm9000a_FSMCConfig()
{
    FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef p;

    // DM9000A使用的地址（BANK1，BANK1有4个片选，DM9000A使用第4条片选）
    // 时序设置
    p.FSMC_AddressSetupTime = 1;
    p.FSMC_AddressHoldTime = 1;
    p.FSMC_DataSetupTime = 2;
    p.FSMC_BusTurnAroundDuration = 1;
    p.FSMC_CLKDivision = 1;
    p.FSMC_DataLatency = 1;
    p.FSMC_AccessMode = FSMC_AccessMode_A;
    // 工作模式
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_PSRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
    // 使能DM9000A的FSMC访问
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
}

void SRAM_Init(void)
{
    SRAM_GPIO_Init();
    SRAM_FSMCConfig();
    LCD_FSMCConfig();
    NandFlash_FSMCConfig();
    Dm9000a_FSMCConfig();
}

