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
#include "cpu_peri.h"
#include "exception.h"
#include "sysctl.h"
#include "MMU.h"



#if 1 //for led_debug
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

static  void __memcpy(char *dst,char *src,int count)
{
    while(count-->0)
    {
        *dst++ = *src++;
    }
}



// ---- 初始化MMU ----
// 映射控制，将SDRAM的向量表映射到0x00000000的地址上去（注意映射的主语与宾语）
// S3C2440的指令和数据的Cache是分开的。
// 在进行映射控制时，还需要控制C及B位。
// CB:   0xC1E
// NCNB: 0xC12
void MMU_Init(void)
{
    register vu32 i;

  //  MMU_Disable();

    //初始化整个4GB L1页表为平板映射，无Cache,无Buffer.
    for(i=0;i<4096;i++)
    {
        MMU_MapSection(i<<12,i<<12,0xC12);
    }

    // 0~0x2FFFFFFF段的初始化，这里很有可能放置的是ROM，开启Cache、Buffer
    for(i=0x00000000; i<0x20000000; )
    {
        MMU_MapSection(i, i, 0xc1e);    // 平板式的映射
        i += 0x100000;
    }

    // 0x30000000~0x3FFFFFFF段映射
    MMU_MapSection(0x20000000, 0x20000000, 0xc12);  //非cache区，HM add for DM9000
    MMU_MapSection(0x30000000, 0x30000000, 0xc12);  //非cache区
    // 其中0x30100000~0x33FFFFFF的63MB是SDRAM，开启Cache、Buffer
    for(i=0x30100000; i<0x40000000; )
    {
        MMU_MapSection(i, i, 0xC1E);    // 平板式的映射
        i += 0x100000;
    }

    // 0x40000000~0x5FFFFFFF段映射
    // 这里是外设寄存器段的地址，由于操作IO口，因而这里不能使能Cache，但可以开启Buffer
    for(i=0x40000000; i<0x60000000; )
    {
        MMU_MapSection(i, i, 0xc12);    // 平板式的映射
        i += 0x100000;
    }

//    // 其余地址都不可用
//    for(i=0x60000000; i != 0; i += 0x100000) {
//      mmu_map(i, i, 0x016);   // 平板式的映射
//    }

    //映射并复制向量表到0xFFFF0000
//    MMU_MapSection(0xFFF00000, 0x33F00000, 0xC1E);
//    __memcpy((char*)0xFFF000000,(char*)0x00000000,4096);


    MMU_MapSection(0xFFF00000, 0x33F00000, 0xc1e);


    MMU_SetSectionsBase((u32*)mmu_page_table);
    MMU_SetDomainAccess(0xffffffff);          //所有域具有管理者权限

    //Cache_EnableAll();
    //Cache_EnableWriteBuf();
    // MMU_Enable();


     //__memcpy((char*)0xFFFF0000,isr_vector,1024);


}

#define PLL_272MHZ  (173<<12)|(2<<4)|(2<<0)
#define PLL_304MHZ  ( 68<<12)|(1<<4)|(1<<0)
#define PLL_405MHZ  (127<<12)|(2<<4)|(1<<0)
#define PLL_532MHZ  (125<<12)|(1<<4)|(1<<0)

//此时内存未初始化好，不能使用局部变量
void Init_CpuClk(void)
{
    volatile register int i;
    struct syscon_reg *sys =(struct syscon_reg*)SYSCON_REG_BASE;

    sys->LOCKTIME = 0xFFFFFFFF;

    sys->CAMDIVN = 0;
    sys->CLKDIVN =  (1<<bo_clkdivn_udivn)    //uclk = upll输出/2
                   |(2<<bo_clkdivn_hdivn)    //hclk = fclk/4=100M
                   |(1<<bo_clkdivn_pdivn);   //pclk = hckk/2=50M

    for(i=0;i<50;i++);
    sys->MPLLCON =  PLL_405MHZ;
    for(i=0;i<1000;i++);


}
void Mem_BusInit(void)
{
    register struct memory_ctrl_reg volatile * const memory_ctrl
                                = (struct memory_ctrl_reg*)0x48000000;
    register u32 xr0;
    xr0 = memory_ctrl->BWSCON;
    xr0 &= 0x00fffff0;       //清掉bank0,bank6/7
    xr0  |= (0<<bo_bus_ublb_bank7) +(0<<bo_bus_wait_bank7)
                                   +(2<<bo_bus_width_bank7)
                                   +(0<<bo_bus_ublb_bank6)
                                   +(0<<bo_bus_wait_bank6)
                                   +(2<<bo_bus_width_bank6)
                                   +(0<<bo_bus_ublb_bank4)
                                   +(0<<bo_bus_wait_bank4)
                                   +(1<<bo_bus_width_bank4);
    memory_ctrl->BWSCON = xr0;
    memory_ctrl->BANKCON0 = (3<<bo_bankcon_tacs)
                                  +(3<<bo_bankcon_tcos)
                                  +(6<<bo_bankcon_tacc)
                                  +(3<<bo_bankcon_tcoh)
                                  +(3<<bo_bankcon_tcah)
                                  +(1<<bo_bankcon_tacp)
                                  +(0<<bo_bankcon_pmc);
    memory_ctrl->BANKCON4 = (3<<bo_bankcon_tacs)
                                  +(3<<bo_bankcon_tcos)
                                  +(6<<bo_bankcon_tacc)
                                  +(3<<bo_bankcon_tcoh)
                                  +(3<<bo_bankcon_tcah)
                                  +(1<<bo_bankcon_tacp)
                                  +(0<<bo_bankcon_pmc);
    memory_ctrl->BANKCON6 = (3<<bo_b67_mt)
                                  +(0<<bo_b67_trcd)
                                  +(1<<bo_b67_scan);
    memory_ctrl->BANKCON7 = (3<<bo_b67_mt)
                                  +(0<<bo_b67_trcd)
                                  +(1<<bo_b67_scan);
    memory_ctrl->REFRESH = (1<<bo_refresh_refen)
                                 +(0<<bo_refresh_trefmd)
                                 +(0<<bo_refresh_trp)
                                 +(1<<bo_refresh_trc)
                                 +(0<<bo_refresh_tchr)
                                 +(SDRAM_refresh_time<<bo_refresh_refcnt);
    memory_ctrl->BANKSIZE = (0<<bo_banksize_bursten)
                                  +(1<<bo_banksize_sckeen)
                                  +(1<<bo_banksize_sclken)
                                  +(1<<bo_banksize_size);
    memory_ctrl->MRSRB6 = 3<<bo_mrsrb_cl;
    memory_ctrl->MRSRB7 = 3<<bo_mrsrb_cl;
}

