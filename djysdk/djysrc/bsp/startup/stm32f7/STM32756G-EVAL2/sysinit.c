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
// 文件名     ：sysinit.c
// 模块描述: 系统初始化部分，主要是内核启动相关的初始化，如时钟，总线等
// 模块版本:
// 创建人员:
// 创建时间:
// =============================================================================

#include "stdint.h"
#include "cpu_peri.h"
#include "board-config.h"

// ===================== PLL配置所需的宏定义 =====================================
//PLL_N:主PLL倍频系数(PLL倍频),取值范围:64~432.
//PLL_M:主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.
//PLL_P:系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)
//PLL_Q:USB/SDIO/随机数产生器等的主PLL分频系数(PLL之后的分频),取值范围:2~15.
//VCO频率=PLL*(PLL_N/PLL_M);
//系统时钟频率=VCO频率/PLL_P=PLL*(PLL_N/(PLL_M*PLL_P));
//USB,SDIO,RNG等的时钟频率=VCO频率/PLL_Q=PLL*(PLL_N/(PLL_M*PLL_Q));
// =============================================================================
/*设置  VCO频率=432Mhz 系统时钟频率=216Mhz USB,SDIO,RNG等的时钟频率=48Mhz*/
#define PLL_N      ((CN_CFG_MCLK/Mhz)*2)

#define PLL_M      25//分频系数等于外部晶振

#define PLL_P      2
#define PLL_Q      9

#define PLLSAI_N      192
#define PLLSAI_R      5
#define PLLSAI_P      2
#define PLLSAI_Q      2
//0 1 2 3对应2 4 8 16分频
#define PLLSAIDIVR    0
// =============================================================================
// 功能：该函数实现系统时钟的初始化，主要包括：1、系统时钟从内部时钟切换到外部时钟；2、配置
//       HCLK、PCLK1、PCLK2、MCLK分频系数；3、用PLL为系统时钟
//       本函数的时钟设置，必须与board-config.h中的CN_CFG_MCLK等常量定义一致。
// 参数：无
// 返回：true false
// =============================================================================
bool_t SysClockInit(void)
{

//相应寄存器初始化
    RCC->CR|=0x00000001;//高速内部时钟使能
    RCC->CFGR=0x00000000;
    RCC->CR&=0xFEF6FFFF;
    RCC->PLLCFGR=0x24003010;
    RCC->CR&=~(1<<18);
    RCC->CIR=0x00000000;//禁止时钟中断
    RCC->CR|=1<<16;//开HSE
    while((RCC->CR&(1<<17))==0);

//配置PLL PLLSAI
    RCC->APB1ENR|=1<<28;    //电源接口时钟使能
    PWR->CR1|=3<<14;        //切换到高性能模式
    RCC->PLLCFGR=PLL_M|(PLL_N<<6)|(((PLL_P>>1)-1)<<16)|(PLL_Q<<24)|(1<<22);//配置主PLL
    RCC->PLLSAICFGR=(PLLSAI_R<<28)|(PLLSAI_Q<<24)|(((PLL_P>>1)-1)<<16)|( PLLSAI_N<<6); //配置主SAI
    RCC->DCKCFGR1 |=(PLLSAIDIVR <<16);
    RCC->CR |=(1<<24);//关闭主PLL

//切换到过驱动模式
    PWR->CR1|=1<<16;        //使能过驱动,频率可到216Mhz
    while(0==(PWR->CSR1&(1<<16)));
    PWR->CR1|=1<<17;        //使能过驱动切换
    while((PWR->CSR1&(1<<17))==0);

//FLASH 时序设置 APB1 APB2分频系数
    FLASH->ACR|=7<<0;       //7个CPU等待周期.
    FLASH->ACR|=1<<8;       //指令预取使能.
    FLASH->ACR|=1<<9;       //使能ART Accelerator
    RCC->CFGR|=(0<<4)|(5<<10)|(4<<13);//HCLK 不分频;APB1 4分频;APB2 2分频.

//切换系统时钟
    RCC->CR|=1<<24;         //打开主PLL
    while((RCC->CR&(1<<25))==0);//等待PLL准备好
    RCC->CFGR&=~(3<<0);     //清零
    RCC->CFGR|=2<<0;        //选择主PLL作为系统时钟
    while((RCC->CFGR&(3<<2))!=(2<<2));//等待主PLL作为系统时钟成功.
    return true;
}

// =============================================================================
// 功能：片外SRAM初始化，包括GPIO和FMC控制器的初始化
// 参数：无
// 返回：无
// =============================================================================
static void SRAM_GpioInit(void)
{
    GPIO_PowerOn(GPIO_D);//使能P-DEFGHI时钟
    GPIO_PowerOn(GPIO_E);
    GPIO_PowerOn(GPIO_F);
    GPIO_PowerOn(GPIO_G);
    GPIO_PowerOn(GPIO_H);
    GPIO_PowerOn(GPIO_I);
//初始化总线IO
    GPIO_CfgPinFunc(GPIO_D,
            PIN0|PIN1|PIN4|PIN5|PIN8|PIN9|PIN10|PIN11|PIN12|PIN13|PIN14|PIN15,
            GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);         //PC0/2/3

    GPIO_CfgPinFunc(GPIO_E,
            PIN0|PIN1|PIN2|PIN3|PIN4|PIN5|PIN6|PIN7|PIN8|PIN9|PIN10|PIN11|PIN12|PIN13|PIN14|PIN15,
            GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);     //PD0/1/8/9/10/14/15

    GPIO_CfgPinFunc(GPIO_F,
            PIN0|PIN1|PIN2|PIN3|PIN4|PIN5|PIN12|PIN13|PIN14|PIN15,
            GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);         //PE0/1/7~15

    GPIO_CfgPinFunc(GPIO_G,
            PIN0|PIN1|PIN2|PIN3|PIN4|PIN5|PIN10,
            GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);     //PG0~5/11~15

    GPIO_CfgPinFunc(GPIO_H,
            PIN8|PIN9|PIN10|PIN11|PIN12|PIN13|PIN14|PIN15,
            GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);         //PC0/2/3

    GPIO_CfgPinFunc(GPIO_I,
            PIN0|PIN1|PIN3|PIN6|PIN7|PIN9|PIN10,
            GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE); //PF0~2/4/5/8/15
//////////////////////////////////////
    GPIO_AFSet(GPIO_D,
            PIN0|PIN1|PIN4|PIN5|PIN8|PIN9|PIN10|PIN11|PIN12|PIN13|PIN14|PIN15,AF12);            //PC0/2/3

    GPIO_AFSet(GPIO_E,
            PIN0|PIN1|PIN2|PIN3|PIN4|PIN5|PIN6|PIN7|PIN8|PIN9|PIN10|PIN11|PIN12|PIN13|PIN14|PIN15,AF12);        //PD0/1/8/9/10/14/15

    GPIO_AFSet(GPIO_F,
                    PIN0|PIN1|PIN2|PIN3|PIN4|PIN5|PIN12|PIN13|PIN14|PIN15,AF12);            //PE0/1/7~15

    GPIO_AFSet(GPIO_G,
            PIN0|PIN1|PIN2|PIN3|PIN4|PIN5|PIN10,AF12);      //PG0~5/11~15

    GPIO_AFSet(GPIO_H,
            PIN8|PIN9|PIN10|PIN11|PIN12|PIN13|PIN14|PIN15,AF12);            //PC0/2/3

    GPIO_AFSet(GPIO_I,
            PIN0|PIN1|PIN3|PIN6|PIN7|PIN9|PIN10,AF12);  //PF0~2/4/5/8/15
//功能I/o


}
static void SRAM_FmcInit(void)
{

    RCC->AHB3ENR|=1<<0;         //使能FMC时钟
      /* Configure and enable Bank1_SRAM3，NE3 */
    //bank1有NE1~4,每一个有一个BCR+TCR，所以总共八个寄存器。

    FMC_Bank1->BTCR[4]=0X00000000;
    FMC_Bank1->BTCR[5]=0X00000000;
    FMC_Bank1E->BWTR[4]=0X00000000;
    //操作BCR寄存器 使用异步模式,模式A(读写共用一个时序寄存器)
    //BTCR[偶数]:BCR寄存器;BTCR[奇数]:BTR寄存器
    FMC_Bank1->BTCR[4]|=1<<12;//存储器写使能
    FMC_Bank1->BTCR[4]|=1<<4; //存储器数据宽度为16bit
    //操作BTR寄存器         （HCLK=216M/2, 1个HCLK=2*4.629ns
    FMC_Bank1->BTCR[5]|=8<<5; //数据保持时间（DATAST）
    FMC_Bank1->BTCR[5]|=0<<3; //地址保持时间（ADDHLD）未用到
    FMC_Bank1->BTCR[5]|=6<<0; //地址建立时间（ADDSET）为0个HCLK 0ns
    //闪存写时序寄存器
    FMC_Bank1E->BWTR[4]=0x0FFFFFFF;//默认值
    //使能BANK1区域3
    FMC_Bank1->BTCR[4]|=1<<0;
}

//SDRAM初始化
void SRAM_Init(void)
{
    SRAM_GpioInit();
    SRAM_FmcInit();

}


