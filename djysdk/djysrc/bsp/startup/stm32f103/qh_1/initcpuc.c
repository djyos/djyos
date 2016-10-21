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
//所属模块:  CPU初始化
//作者：     lst
//版本：      V1.0.0
//初始版本完成日期：2009-08-05
//文件描述:  cpu时钟初始化
//其他说明:  无
//修订历史:
//2. ...
//1. 日期: 2009-08-05
//   作者: lst
//   新版本号：V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "arch_feature.h"
#include "stdint.h"
#include "int.h"
#include "cpu.h"
#include "cpu_peri.h"
#include "hard-exp.h"
#include "lowpower.h"
#include "djyos.h"
#include "stm32f10x.h"
#include "core_cmFunc.h"

extern   uint32_t   msp_top[ ];
extern void __set_PSP(uint32_t topOfProcStack);
extern void __set_PRIMASK(uint32_t priMask);
extern void __set_FAULTMASK(uint32_t faultMask);
extern void __set_CONTROL(uint32_t control);

extern void SRAM_Init(void);
extern void IAP_SelectLoadProgam(void);
extern char g_cIbootFlag[];

struct ScbReg volatile * const startup_scb_reg
                        = (struct ScbReg *)0xe000ed00;
void Startup_NMI(void)
{
    while(1);
}
void Startup_Hardfault(void)
{
    while(1);
}
void Init_Cpu(void);
const u32 gc_u32StartupExpTable[4] =
{
    (u32)msp_top,
    (u32)Init_Cpu,
    (u32) Startup_NMI,
    (u32) Startup_Hardfault
};

void Init_Cpu(void)
{
    __set_PSP((uint32_t)msp_top);
    __set_PRIMASK(1);
    __set_FAULTMASK(1);
    __set_CONTROL(0);
    switch(startup_scb_reg->CPUID)
    {
        case CN_M3_REVISION_R0P0:
            break;    //市场没有版本0的芯片
        case CN_M3_REVISION_R1P0:
        	startup_scb_reg->CCR |= 1<<bo_scb_ccr_stkalign;
            break;
        case CN_M3_REVISION_R1P1:
        	startup_scb_reg->CCR |= 1<<bo_scb_ccr_stkalign;
            break;
        case CN_M3_REVISION_R2P0:break;    //好像没什么要做的
    }

    FLASH->ACR &= ~(u32)0x1f;
    FLASH->ACR |= (CN_CFG_MCLK-1)/24000000;   //设置等待周期。
    FLASH->ACR |= 0x10;       //开启预取

    if(((RCC->CR & cn_cr_check_mask) != cn_cr_check)
                || ((RCC->CFGR & cn_cfgr_check_mask) != cn_cfgr_check))
    {
        //开始初始化时钟
        //step1:复位时钟控制寄存器
    	RCC->CR |= (uint32_t)0x00000001;
        // 复位 SW[1:0], HPRE[3:0], PPRE1[2:0], PPRE2[2:0], ADCPRE[1:0] MCO[2:0] 位
    	RCC->CFGR &= (uint32_t)0xF8FF0000;
        // 复位 HSEON, CSSON and PLLON 位
    	RCC->CR &= (uint32_t)0xFEF6FFFF;
        // 复位 HSEBYP 位
    	RCC->CR &= (uint32_t)0xFFFBFFFF;
        // 复位 PLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE 位
    	RCC->CFGR &= (uint32_t)0xFF80FFFF;
        // 禁止所有中断
    	RCC->CIR = 0x00000000;

        //step2:设置各时钟控制位以及倍频、分频值
    	RCC->CFGR = cn_cfgr_set+(7<<24);   // set clock configuration register
    	RCC->CR   = cn_cr_set;     // set clock control register

        while(bb_rcc_cr_hserdy ==0);
        while(bb_rcc_cr_pllrdy ==0);
    }
    SRAM_Init();
//    __LP_BSP_HardInit( );
//    if( __LP_BSP_GetSleepLevel() == CN_SLEEP_L3)
//    {
//        __LP_BSP_RestoreRamL3();
//
//        HardExp_Init();
//        Int_ContactTrunk();
//        cm_cpsie_f();
//
//        __asm_start_thread(g_ptEventRunning->vm);
//    }
//    else
//      Load_Preload( );
    IAP_SelectLoadProgam();
}


//-----------------------------------------------------------------
//功能：IAP组件控制运行模式的GPIO引脚初始化SW4-GPIOC_10
//参数：无
//返回：无。
//说明： 具体选择哪个引脚由具体板件确定，选择何种电平对应何种运行状态也随板件确定。
//选择原则是：该GPIO引脚正常情况下不起作用，只有在APP程序出现异常跑飞，无法通过shell
//命令切换到IBOOT状态时，才跳上跳线冒启用该GPIO引脚。具体是高电平运行Iboot还是低电平
//运行IBOOT,由硬件决定。

void IAP_GpioPinInit(void)
{
//   bb_rcc_apb2enr_iopcen=1;//使能gpioc
//   pg_gpio_regc->CRH  =(pg_gpio_regc->CRH &(~(0xf <<((10-8)<<2))))
//                                  | ((CN_GPIO_MODE_IN_PULLUP & 0xf)<< ((10-8)<<2));
//   pg_gpio_regc->BSRR = 1<<10;//上拉输入
//     GPIO_PowerOn(CN_GPIO_C);
//     GPIO_CfgPinFunc(CN_GPIO_C, 10, CN_GPIO_MODE_IN_PULLUP);
}

bool_t IAP_IsForceIboot(void)
{
//    u32 flag;
//    flag=pg_gpio_regc->IDR&(1<<10);
//    if(flag==0)
//        return false;
//    return true;
	return false;

}


bool_t IAP_IsRamIbootFlag(void)
{
    u8 i;
    char bootflag[8]="RunIboot";
    for(i=0;i<8;i++)
    {
        if(g_cIbootFlag[i]!=bootflag[i])
        {
            return false;
        }
    }
    return true;
}
