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
//作者：     Liuwei
//版本：      V1.0.0
//初始版本完成日期：2015-06-01
//文件描述:  cpu时钟初始化
//其他说明:  无
//修订历史:
//2. ...
//1. 日期: 2015-06-01
//   作者: Liuwei
//   新版本号：V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "arch_feature.h"
#include "stdint.h"
#include "cpu_peri.h"
#include "hard-exp.h"
#include "sysctl.h"
#include "MMU.h"



#if 0 //for led_debug
void _start(void);

void    x_led_test(void)
{
    //PB5,6,7,8;
    register struct gpio_reg *gpio;
    register volatile int i;
    ////register volatile int j;

    //i = &_start;

    gpio =(struct gpio_reg*)GPIO_REG_BASE;

    gpio->GPBCON &= ~((3<<16)|(3<<14)|(3<<12)|(3<<10));
    gpio->GPBCON |=  ((1<<16)|(1<<14)|(1<<12)|(1<<10));

    //j=0;
    //while(j++ < 50000)
    while(1)
    {
        gpio->GPBDAT &= ~((1<<8)|(1<<7)|(1<<6)|(1<<5));
        for(i=0;i<0x00040000;i++);

        gpio->GPBDAT |=  ((1<<8)|(1<<7)|(1<<6)|(1<<5));
        for(i=0;i<0x00040000;i++);
    }

}
#endif


// ---- 初始化MMU ----
void MMU_Init(void)
{
        MMU_MapSection(mmu_page_table,0x00000000,0x00000000,4096,
                SECTION_ATTR(AP_USER_RW,DOMAIN_NO_CHECK,NCNB));

        MMU_MapSection(mmu_page_table,0x08000000,0x08000000,32,
                SECTION_ATTR(AP_USER_RW,DOMAIN_NO_CHECK,NCNB)); //BANK1(网卡)

        MMU_MapSection(mmu_page_table,0x10000000,0x10000000,32,
                SECTION_ATTR(AP_USER_RW,DOMAIN_NO_CHECK,NCNB)); //BANK2(CAN)

        MMU_MapSection(mmu_page_table,0x20000000,0x20000000,32,
                SECTION_ATTR(AP_USER_RW,DOMAIN_NO_CHECK,NCNB)); //BANK4

        MMU_MapSection(mmu_page_table,0x30000000,0x30000000,8,
                SECTION_ATTR(AP_USER_RW,DOMAIN_NO_CHECK,CNB));  //ROM1

        MMU_MapSection(mmu_page_table,0x30800000,0x30800000,8,
                SECTION_ATTR(AP_USER_RW,DOMAIN_NO_CHECK,NCNB)); //RAM_nocache

        MMU_MapSection(mmu_page_table,0x31000000,0x31000000,16,
                SECTION_ATTR(AP_USER_RW,DOMAIN_NO_CHECK,CNB));  //RAM1

        MMU_MapSection(mmu_page_table,0x32000000,0x32000000,16,
                SECTION_ATTR(AP_USER_RW,DOMAIN_NO_CHECK,CNB));  //RAM2

        MMU_MapSection(mmu_page_table,0x33000000,0x33000000,16,
                SECTION_ATTR(AP_USER_RW,DOMAIN_NO_CHECK,CNB));  //RAM3

        MMU_MapSection(mmu_page_table,0x48000000,0x48000000,384,
                SECTION_ATTR(AP_USER_RW,DOMAIN_NO_CHECK,NCNB)); //SFR

        MMU_MapSection(mmu_page_table,0xFFF00000,0x33F00000,1,
                SECTION_ATTR(AP_USER_RW,DOMAIN_NO_CHECK,CNB));  //Last 1MB


        MMU_SetSectionsBase((u32*)mmu_page_table);
        MMU_SetDomainAccess(0xffffffff);          //所有域具有管理者权限


}

#if (CN_CFG_FIN_CLK==(8*Mhz))
#define PLL_200MHZ  (192<<12)|(2<<4)|(1<<0)  //For Fin=8M
#endif

#if (CN_CFG_FIN_CLK==(12*Mhz))
#define PLL_200MHZ  (92<<12)|(4<<4)|(0<<0)   //For Fin=12M
#endif


//      本函数的时钟设置，必须与board-config.h中的CN_CFG_MCLK等常量定义一致。
void CPU_CLKInit(void)
{
    volatile register int i;
    struct SYSCON_REG *sys =SYSCON_REG_BASE;

    sys->LOCKTIME = 0x0FFFFFFF;

    sys->CAMDIVN = 0;
    sys->CLKDIVN =  (1<<bo_clkdivn_udivn)    //uclk = upll输出/2
                   |(1<<bo_clkdivn_hdivn)    //hclk = fclk/2=100M
                   |(1<<bo_clkdivn_pdivn);   //pclk = hckk/2=50M

    for(i=0;i<50;i++);
    sys->MPLLCON =  PLL_200MHZ;
    for(i=0;i<1000;i++);


}



void MEM_BUSInit(void)
{

    struct MEMCON_REG *mem=MEMCON_REG_BASE;
    register vu32 xr0;

    xr0  = mem->BWSCON;
    xr0 &= 0x00FFF000;       //清掉bank0,bank1,bank2,bank6/7

    xr0 |=   (0<<bo_bus_ublb_bank7)
            |(0<<bo_bus_wait_bank7)
            |(2<<bo_bus_width_bank7)

            |(0<<bo_bus_ublb_bank6)
            |(0<<bo_bus_wait_bank6)
            |(2<<bo_bus_width_bank6)

            //BANK2:CAN,8bit
            |(0<<bo_bus_ublb_bank2)
            |(0<<bo_bus_wait_bank2)     //BANK2 wait disable
            |(0<<bo_bus_width_bank2)    //BANK2 0:8bit; 1:16bit;

            //BANK1:网卡,16bit
            |(0<<bo_bus_ublb_bank1)
            |(0<<bo_bus_wait_bank1)     //BANK1 wait disable
            |(1<<bo_bus_width_bank1);   //BANK1 0:8bit; 1:16bit;

    mem->BWSCON = xr0;

    mem->BANKCON0 =  (3<<bo_bankcon_tacs)
                    |(3<<bo_bankcon_tcos)
                    |(6<<bo_bankcon_tacc)
                    |(3<<bo_bankcon_tcoh)
                    |(3<<bo_bankcon_tcah)
                    |(1<<bo_bankcon_tacp)
                    |(0<<bo_bankcon_pmc);

    mem->BANKCON1 =  (3<<bo_bankcon_tacs)
                    |(3<<bo_bankcon_tcos)
                    |(6<<bo_bankcon_tacc)
                    |(3<<bo_bankcon_tcoh)
                    |(3<<bo_bankcon_tcah)
                    |(1<<bo_bankcon_tacp)
                    |(0<<bo_bankcon_pmc);

    mem->BANKCON2 =  (3<<bo_bankcon_tacs)
                    |(3<<bo_bankcon_tcos)
                    |(6<<bo_bankcon_tacc)
                    |(3<<bo_bankcon_tcoh)
                    |(3<<bo_bankcon_tcah)
                    |(1<<bo_bankcon_tacp)
                    |(0<<bo_bankcon_pmc);


    mem->BANKCON4 = (3<<bo_bankcon_tacs)
                                  +(3<<bo_bankcon_tcos)
                                  +(6<<bo_bankcon_tacc)
                                  +(3<<bo_bankcon_tcoh)
                                  +(3<<bo_bankcon_tcah)
                                  +(1<<bo_bankcon_tacp)
                                  +(0<<bo_bankcon_pmc);
    mem->BANKCON6 = (3<<bo_b67_mt)
                                  +(0<<bo_b67_trcd)
                                  +(1<<bo_b67_scan);
    mem->BANKCON7 = (3<<bo_b67_mt)
                                  +(0<<bo_b67_trcd)
                                  +(1<<bo_b67_scan);
    mem->REFRESH = (1<<bo_refresh_refen)
                                 +(0<<bo_refresh_trefmd)
                                 +(0<<bo_refresh_trp)
                                 +(1<<bo_refresh_trc)
                                 +(0<<bo_refresh_tchr)
                                 +(SDRAM_refresh_time<<bo_refresh_refcnt);
    mem->BANKSIZE = (0<<bo_banksize_bursten)
                                  +(1<<bo_banksize_sckeen)
                                  +(1<<bo_banksize_sclken)
                                  +(1<<bo_banksize_size);
    mem->MRSRB6 = 3<<bo_mrsrb_cl;
    mem->MRSRB7 = 3<<bo_mrsrb_cl;
}

