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
#include "SRAM.h"
#include "stm32f1xx_hal_sram.h"

void SRAM_GPIO_Init(void)
{
    RCC->AHBENR|=1<<8;              //使能FSMC时钟
    RCC->APB2ENR|=1<<0;
    RCC->APB2ENR|=1<<5;         //使能PORTD时钟
    RCC->APB2ENR|=1<<6;         //使能PORTE时钟
    RCC->APB2ENR|=1<<7;         //使能PORTF时钟
     RCC->APB2ENR|=1<<8;          //使能PORTG时钟

    //PORTD复用推挽输出
    GPIOD->CRH &=0X00000000;        //8,9,10,11,12,13,14,15
    GPIOD->CRH |=0XBBBBBBBB;
    GPIOD->CRL &=0X0F00FF00;        //0,1,4,5,7
    GPIOD->CRL |=0XB0BB00BB;

    //PORTE复用推挽输出
    GPIOE->CRH &=0X00000000;        //8~15
    GPIOE->CRH |=0XBBBBBBBB;
    GPIOE->CRL &=0X0F000F00;        //0,1,3,4,5,7
    GPIOE->CRL |=0XB0BBB0BB;

    //PORTF复用推挽输出
    GPIOF->CRH &=0X0000FFFF;        //12,13,14,15
    GPIOF->CRH |=0XBBBB0000;
    GPIOF->CRL &=0XFF000000;        //0,1,2,3,4,5
    GPIOF->CRL |=0X00BBBBBB;

    GPIOG->CRH&=0XFFF0F00F;            //9,10,12
    GPIOG->CRH|=0X000B0BB0;
    GPIOG->CRL&=0XFF000000;            //0,1,2,3,4,5
    GPIOG->CRL|=0X00BBBBBB;
}

void SRAM_FSMCConfig(void)
{
    SRAM_HandleTypeDef hsram;

    FSMC_NORSRAM_TimingTypeDef Timing;



    hsram.Instance  = FSMC_NORSRAM_DEVICE;
    hsram.Extended  = FSMC_NORSRAM_EXTENDED_DEVICE;

    Timing. AddressSetupTime = 1;
    Timing. AddressHoldTime = 0;
    Timing. DataSetupTime = 2;
    Timing. BusTurnAroundDuration = 0;
    Timing. CLKDivision = 0;
    Timing. DataLatency = 0;
    Timing. AccessMode = FSMC_ACCESS_MODE_A;

    hsram.Init. NSBank= FSMC_NORSRAM_BANK1;
    hsram.Init. DataAddressMux= FSMC_DATA_ADDRESS_MUX_DISABLE;
    hsram.Init. MemoryType =FSMC_MEMORY_TYPE_SRAM;
    hsram.Init. MemoryDataWidth=FSMC_NORSRAM_MEM_BUS_WIDTH_16;
    hsram.Init. BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
    hsram.Init. WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
    hsram.Init. WrapMode = FSMC_WRAP_MODE_DISABLE;
    hsram.Init. WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
    hsram.Init. WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
    hsram.Init. WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
    hsram.Init. ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
    hsram.Init. AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
    hsram.Init. WriteBurst = FSMC_WRITE_BURST_DISABLE;
    //      /* Initialize the NAND controller */

    if(HAL_SRAM_Init(&hsram, &Timing, &Timing) != HAL_OK)
    {
        return;//出错
    }

}

void LCD_FSMCConfig(void)
{
    SRAM_HandleTypeDef hsram;

    FSMC_NORSRAM_TimingTypeDef Timing;



    hsram.Instance  = FSMC_NORSRAM_DEVICE;
    hsram.Extended  = FSMC_NORSRAM_EXTENDED_DEVICE;

    Timing. AddressSetupTime = 1;
    Timing. AddressHoldTime = 0;
    Timing. DataSetupTime = 2;
    Timing. BusTurnAroundDuration = 0;
    Timing. CLKDivision = 0;
    Timing. DataLatency = 1;
    Timing. AccessMode = FSMC_ACCESS_MODE_A;


    hsram.Init. NSBank= FSMC_NORSRAM_BANK3;
    hsram.Init. DataAddressMux= FSMC_DATA_ADDRESS_MUX_DISABLE;
    hsram.Init. MemoryType =FSMC_MEMORY_TYPE_SRAM;
    hsram.Init. MemoryDataWidth=FSMC_NORSRAM_MEM_BUS_WIDTH_16;
    hsram.Init. BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
    hsram.Init. WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
    hsram.Init. WrapMode = FSMC_WRAP_MODE_DISABLE;
    hsram.Init. WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
    hsram.Init. WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
    hsram.Init. WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
    hsram.Init. ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
    hsram.Init. AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
    hsram.Init. WriteBurst = FSMC_WRITE_BURST_DISABLE;

    if(HAL_SRAM_Init(&hsram, &Timing, &Timing) != HAL_OK)
    {
        return;//出错
    }
}

void NandFlash_FSMCConfig(void)
{

    FSMC_NAND_PCC_TimingTypeDef  Timing;
    NAND_HandleTypeDef nandHandle;

    nandHandle.Instance  = FSMC_NAND_DEVICE;

    nandHandle.Init. NandBank=FSMC_NAND_BANK3;
    nandHandle.Init. Waitfeature =FSMC_NAND_PCC_WAIT_FEATURE_ENABLE;
    nandHandle.Init. MemoryDataWidth=FSMC_NAND_PCC_MEM_BUS_WIDTH_8;
    nandHandle.Init. EccComputation =FSMC_NAND_ECC_ENABLE;
    nandHandle.Init. ECCPageSize=FSMC_NAND_ECC_PAGE_SIZE_512BYTE;
    nandHandle.Init. TCLRSetupTime=0;
    nandHandle.Init. TARSetupTime=0;

    Timing. SetupTime=0;
    Timing. WaitSetupTime=3;
    Timing. HoldSetupTime=9;
    Timing. HiZSetupTime=0;

    if(HAL_NAND_Init(&nandHandle, &Timing, &Timing) != HAL_OK)
    {
        return;/* Initialization Error */
    }

}

void Dm9000a_FSMCConfig()
{

    SRAM_HandleTypeDef hsram;
    FSMC_NORSRAM_TimingTypeDef Timing;


    hsram.Instance  = FSMC_NORSRAM_DEVICE;
    hsram.Extended  = FSMC_NORSRAM_EXTENDED_DEVICE;

    Timing. AddressSetupTime = 1;
    Timing. AddressHoldTime = 1;
    Timing. DataSetupTime = 2;
    Timing. BusTurnAroundDuration = 1;
    Timing. CLKDivision = 1;
    Timing. DataLatency = 1;
    Timing. AccessMode = FSMC_ACCESS_MODE_A;


    hsram.Init. NSBank= FSMC_NORSRAM_BANK4;
    hsram.Init. DataAddressMux= FSMC_DATA_ADDRESS_MUX_DISABLE;
    hsram.Init. MemoryType =FSMC_MEMORY_TYPE_SRAM;
    hsram.Init. MemoryDataWidth=FSMC_NORSRAM_MEM_BUS_WIDTH_16;
    hsram.Init. BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
    hsram.Init. WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
    hsram.Init. WrapMode = FSMC_WRAP_MODE_DISABLE;
    hsram.Init. WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
    hsram.Init. WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
    hsram.Init. WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
    hsram.Init. ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
    hsram.Init. AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
    hsram.Init. WriteBurst = FSMC_WRITE_BURST_DISABLE;

    if(HAL_SRAM_Init(&hsram, &Timing, &Timing) != HAL_OK)
    {
        return;//出错
    }


}

void SRAM_Init(void)
{
    SRAM_GPIO_Init();
    SRAM_FSMCConfig();
    LCD_FSMCConfig();
    NandFlash_FSMCConfig();
    Dm9000a_FSMCConfig();
}

