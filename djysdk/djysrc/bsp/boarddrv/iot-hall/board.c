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

// =============================================================================
// ===========================板上IIC的IO模拟使用=================================
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))

#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //out
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //in
// =============================================================================


// =============================================================================
// 功能：根据具体的板件配置串口的GPIO的引脚功能，这是与板件相关，所以该函数放在该文件，CPU
//      串口驱动直接调用该函数来初始化串口的GPIO引脚
//      主要包括GPIO端口和串口时钟使能、GPIO配置、重映射、时钟等
// 参数：无
// 返回：true
// =============================================================================
bool_t Board_UartGpioInit(u8 SerialNo)
{
    //初始化IO端口位uart功能
    //Ports  :  GPA10 GPA9 GPA8 GPA7 GPA6 GPA5 GPA4 GPA3 GPA2 GPA1 GPA0
    //Signal :   RXD1 TXD1  xx   xx   xx   xx   xx   xx   xx   xx   xx
    switch(SerialNo)
    {
    case CN_UART1:
    	RCC->AHB1ENR|=1<<0;   	//使能PORTA口时钟
    	RCC->APB2ENR|=1<<4;  	//使能串口1时钟
    	GPIO_CfgPinFunc(GPIO_A,PIN9|PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,
    			GPIO_SPEED_50M,GPIO_PUPD_PU);//PA9,PA10,复用功能,上拉输出
     	GPIO_AFSet(GPIO_A,9,7);	//PA9,AF7
    	GPIO_AFSet(GPIO_A,10,7);//PA10,AF7
        break;
    case CN_UART2:
        RCC->APB1ENR |= (1<<17);
        RCC->AHB1ENR |= (1<<0);//port A clock enable
        GPIO_CfgPinFunc(GPIO_A,PIN2|PIN3,GPIO_MODE_AF,GPIO_OTYPE_PP,
        		GPIO_SPEED_50M,GPIO_PUPD_PU);//PA9,PA10,复用功能,上拉输出
     	GPIO_AFSet(GPIO_A,2,7);	//PA2,AF7
    	GPIO_AFSet(GPIO_A,3,7);//PA3,AF7
        break;
    case CN_UART3:
        RCC->APB1ENR |= (1<<18);//uart3 clock enable
        RCC->AHB1ENR |= (1<<1);// port b clock enable
        GPIO_CfgPinFunc(GPIO_B,PIN10|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,
        		GPIO_SPEED_50M,GPIO_PUPD_PU);
     	GPIO_AFSet(GPIO_B,10,7);	//PB10,AF7
    	GPIO_AFSet(GPIO_B,11,7);//PB11,AF7
        break;
    case CN_UART4:
    	RCC->APB1ENR |= (1<<19);//uart4 clock enable
        RCC->AHB1ENR |= (1<<0);//port A clock enable
        GPIO_CfgPinFunc(GPIO_A,PIN0|PIN1,GPIO_MODE_AF,GPIO_OTYPE_PP,
        		GPIO_SPEED_50M,GPIO_PUPD_PU);//PA9,PA10,复用功能,上拉输出
     	GPIO_AFSet(GPIO_A,0,7);	//PA2,AF7
    	GPIO_AFSet(GPIO_A,1,7);//PA3,AF7
        break;
    case CN_UART5:
    	RCC->APB1ENR |= (1<<20);//uart5 clock enable
        RCC->AHB1ENR |= (1<<2);//port C clock enable
        GPIO_CfgPinFunc(GPIO_C,PIN11|PIN12,GPIO_MODE_AF,GPIO_OTYPE_PP,
        		GPIO_SPEED_50M,GPIO_PUPD_PU);//PA9,PA10,复用功能,上拉输出
     	GPIO_AFSet(GPIO_C,11,7);	//PA2,AF7
    	GPIO_AFSet(GPIO_C,12,7);//PA3,AF7
        break;
    case CN_UART6:
    	RCC->APB2ENR|= (1<<5);  	//使能串口6时钟
        RCC->AHB1ENR |= (1<<2);//port C clock enable
        GPIO_CfgPinFunc(GPIO_C,PIN6|PIN7,GPIO_MODE_AF,GPIO_OTYPE_PP,
        		GPIO_SPEED_50M,GPIO_PUPD_PU);//PA9,PA10,复用功能,上拉输出
     	GPIO_AFSet(GPIO_C,6,7);	//PA2,AF7
    	GPIO_AFSet(GPIO_C,7,7);//PA3,AF7
    	break;
    default:
        break;
    }
	return true;
}

// =============================================================================
// 功能：根据具体的板件配置IIC的GPIO的引脚功能，这是与板件相关，所以该函数放在该文件，CPU
//      IIC驱动直接调用该函数来初始化串口的GPIO引脚
//      主要包括GPIO端口和外设时钟使能、GPIO配置、重映射、时钟等
// 参数：无
// 返回：true
// =============================================================================
bool_t Board_IicGpioInit(u8 I2Cx)
{
	if(I2Cx == CN_I2C1)
	{
		RCC->AHB1ENR|=1<<1;    //使能PORTB时钟

		GPIO_CfgPinFunc(GPIO_B,PIN6|PIN7,GPIO_MODE_OUT,GPIO_OTYPE_PP,
				GPIO_SPEED_50M,GPIO_PUPD_PU);//PA9,PA10,复用功能,上拉输出
		return true;
	}
	return false;
}

void IIC_SetDaIn(u8 IICx)
{
	if(IICx == CN_I2C1)
	{
		GPIOB->MODER&=~(3<<(6*2));
		GPIOB->MODER|=0<<6*2;	//PB6输入模式
	}
	else
	{
		//板上未用到
	}
}
void IIC_SetDaOut(u8 IICx)
{
	if(IICx == CN_I2C1)
	{
		GPIOB->MODER&=~(3<<(6*2));
		GPIOB->MODER|=1<<6*2;//PB6输出模式
	}
	else
	{
		//板上未用到
	}
}
void IIC_Scl(u8 IICx,u8 level)
{
	if(IICx == CN_I2C1)
	{
		PBout(7) = level; //SCL;
	}
}
void IIC_Sda(u8 IICx,u8 level)
{
	if(IICx == CN_I2C1)
	{
		PBout(6) = level; //SDA
	}
}

u8 IIC_ReadSda(u8 IICx)
{
	u8 i = 0;
	if(IICx == CN_I2C1)
	{
		i = PBin(6);
	}
	return i;
}

// =============================================================================
// 功能：根据具体的板件配置SPI的GPIO的引脚功能，这是与板件相关，所以该函数放在该文件，CPU
//      SPI驱动直接调用该函数来初始化串口的GPIO引脚
//      主要包括GPIO端口和外设时钟使能、GPIO配置、重映射、时钟等
// 参数：无
// 返回：true
// =============================================================================
bool_t Board_SpiGpioInit(u8 SPIx)
{
	if(SPIx == CN_SPI1)
	{
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
		RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;		//使能I2C1时钟

		RCC->APB2RSTR	|=	RCC_APB2RSTR_SPI1;	//复位SPI1
		RCC->APB2RSTR	&=	~RCC_APB2RSTR_SPI1;	//停止复位SPI1
		//MOSI,MISO,SCK,CSS
    	GPIO_CfgPinFunc(GPIO_A,PIN5|PIN6|PIN7,GPIO_MODE_AF,GPIO_OTYPE_PP,
    			GPIO_SPEED_50M,GPIO_PUPD_PU);//PA9,PA10,复用功能,上拉输出
//    	GPIO_AFSet(GPIO_A,4,5);
    	GPIO_AFSet(GPIO_A,5,5);
    	GPIO_AFSet(GPIO_A,6,5);
    	GPIO_AFSet(GPIO_A,7,5);
	}
	else
	{
		return false;
	}
	return true;
}
















