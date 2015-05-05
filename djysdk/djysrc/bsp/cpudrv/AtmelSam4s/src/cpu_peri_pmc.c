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
//版本：     V1.0.0
//初始版本完成日期：2014-1-20
//文件描述:  cpu时钟初始化
//其他说明:  无
//修订历史:
//2. ...
//1. 日期: 2014-1-20
//   作者: lst
//   新版本号：V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "os.h"
#include "cpu_peri_pmc.h"

//todo  ---- #if MCK = 100MHz
#define BOARD_OSCOUNT   (CKGR_MOR_MOSCXTST(0x8))
#define BOARD_PLLBR     (CKGR_PLLBR_MULB(24) \
                       | CKGR_PLLBR_PLLBCOUNT(0x1) \
                       | CKGR_PLLBR_DIVB(3))
#define BOARD_MCKR      (PMC_MCKR_PRES_CLK | PMC_MCKR_CSS_PLLB_CLK)

/* Define clock timeout */
#define CLOCK_TIMEOUT    0xFFFFFFFF

#define MASK_STATUS0 0xFFFFFFFC
#define MASK_STATUS1 0xFFFFFFFF

struct atmel_pmc_reg volatile * const SAM_PMC_REG =
                                    (struct atmel_pmc_reg *)0x400E0400;
//------CPU时钟初始化-----------------------------------------------------
//功能: 初始化CPU时钟，上电默认使用的是内部的晶振时钟，该函数将晶振源切换到
 //     外部调整时钟，后面将main clock时钟切换到PLLB分频后的时钟
//参数: 无
//返回: 无
//-----------------------------------------------------------------------
void cpu_clk_init(void)
{
    uint32_t timeout = 0;

    /* Initialize main oscillator */
    if ( !(SAM_PMC_REG->CKGR_MOR & CKGR_MOR_MOSCSEL) )
    {
    	SAM_PMC_REG->CKGR_MOR = CKGR_MOR_KEY(0x37) | BOARD_OSCOUNT |
                                CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCXTEN;
        timeout = 0;
        while (!(SAM_PMC_REG->PMC_SR & PMC_SR_MOSCXTS) &&
                                    (timeout++ < CLOCK_TIMEOUT));
    }

    /* Switch to 3-20MHz Xtal oscillator */
    SAM_PMC_REG->CKGR_MOR = CKGR_MOR_KEY(0x37) | BOARD_OSCOUNT |
                CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCXTEN | CKGR_MOR_MOSCSEL;
    timeout = 0;
    while (!(SAM_PMC_REG->PMC_SR & PMC_SR_MOSCSELS) && (timeout++ < CLOCK_TIMEOUT));
    SAM_PMC_REG->PMC_MCKR = (SAM_PMC_REG->PMC_MCKR & ~(uint32_t)PMC_MCKR_CSS_Msk) |
                                PMC_MCKR_CSS_MAIN_CLK;
    for ( timeout = 0; !(SAM_PMC_REG->PMC_SR & PMC_SR_MCKRDY) &&
                            (timeout++ < CLOCK_TIMEOUT) ; );

    /* Initialize PLLB */
    SAM_PMC_REG->CKGR_PLLBR = BOARD_PLLBR;
    timeout = 0;
    while (!(SAM_PMC_REG->PMC_SR & PMC_SR_LOCKB) && (timeout++ < CLOCK_TIMEOUT));

    /* Switch to main clock */
    SAM_PMC_REG->PMC_MCKR = (BOARD_MCKR & ~PMC_MCKR_CSS_Msk) | PMC_MCKR_CSS_MAIN_CLK;
    for ( timeout = 0; !(SAM_PMC_REG->PMC_SR & PMC_SR_MCKRDY) &&
                                    (timeout++ < CLOCK_TIMEOUT) ; );

    SAM_PMC_REG->PMC_MCKR = BOARD_MCKR ;
    for ( timeout = 0; !(SAM_PMC_REG->PMC_SR & PMC_SR_MCKRDY) &&
                                    (timeout++ < CLOCK_TIMEOUT) ; );
}

//------使能外设时钟-----------------------------------------------------------
//功能: 外设时钟可单独使能和除能，以节约能耗
//参数: dwId，外设ID，外设ID可参考手册39页
//返回: 无
//-----------------------------------------------------------------------------
void PMC_EnablePeripheral( uint32_t dwId )
{
    if(dwId > 34)
    	return;

    if ( dwId < 32 )
    {
        if ( (SAM_PMC_REG->PMC_PCSR0 & ((uint32_t)1 << dwId)) == ((uint32_t)1 << dwId) )
        {
//            djy_printf( "PMC_EnablePeripheral: clock of peripheral"  "
//                        %u is already enabled\n\r", (unsigned int)dwId ) ;
        }
        else
        {
            SAM_PMC_REG->PMC_PCER0 = 1 << dwId ;
        }
    }
    else
    {
        dwId -= 32;
        if ((SAM_PMC_REG->PMC_PCSR1 & ((uint32_t)1 << dwId)) == ((uint32_t)1 << dwId))
        {
//            djy_printf( "PMC_EnablePeripheral: clock of peripheral"  "
//                    %u is already enabled\n\r", (unsigned int)(dwId + 32) ) ;
        }
        else
        {
            SAM_PMC_REG->PMC_PCER1 = 1 << dwId ;
        }
    }
}

//------除能外设时钟-----------------------------------------------------------
//功能: 外设时钟可单独使能和除能，以节约能耗
//参数: dwId，外设ID，外设ID可参考手册39页
//返回: 无
//-----------------------------------------------------------------------------
void PMC_DisablePeripheral( uint32_t dwId )
{
    if(dwId > 34)
    	return;

    if ( dwId < 32 )
    {
        if ( (SAM_PMC_REG->PMC_PCSR0 & ((uint32_t)1 << dwId)) != ((uint32_t)1 << dwId) )
        {
//            djy_printf("PMC_DisablePeripheral: clock of peripheral" "
//                            %u is not enabled\n\r", (unsigned int)dwId ) ;
        }
        else
        {
            SAM_PMC_REG->PMC_PCDR0 = 1 << dwId ;
        }
    }
    else
    {
        dwId -= 32 ;
        if ( (SAM_PMC_REG->PMC_PCSR1 & ((uint32_t)1 << dwId)) != ((uint32_t)1 << dwId) )
        {
//            djy_printf( "PMC_DisablePeripheral: clock of peripheral" "
//                        %u is not enabled\n\r", (unsigned int)(dwId + 32) ) ;
        }
        else
        {
            SAM_PMC_REG->PMC_PCDR1 = 1 << dwId ;
        }
    }
}

//------使能所有外设时钟-------------------------------------------------------
//功能: 外设时钟可单独使能和除能，以节约能耗
//参数: dwId，外设ID，外设ID可参考手册39页
//返回: 无
//-----------------------------------------------------------------------------
void PMC_EnableAllPeripherals( void )
{
    SAM_PMC_REG->PMC_PCER0 = MASK_STATUS0 ;
    while ( (SAM_PMC_REG->PMC_PCSR0 & MASK_STATUS0) != MASK_STATUS0 ) ;

    SAM_PMC_REG->PMC_PCER1 = MASK_STATUS1 ;
    while ( (SAM_PMC_REG->PMC_PCSR1 & MASK_STATUS1) != MASK_STATUS1 ) ;

//    djy_printf( "Enable all periph clocks\n\r" ) ;
}

//------使能外设时钟-----------------------------------------------------------
//功能: 外设时钟可单独使能和除能，以节约能耗
//参数: dwId，外设ID，外设ID可参考手册39页
//返回: 无
//-----------------------------------------------------------------------------
void PMC_DisableAllPeripherals( void )
{
    SAM_PMC_REG->PMC_PCDR0 = MASK_STATUS0 ;
    while ( (SAM_PMC_REG->PMC_PCSR0 & MASK_STATUS0) != 0 ) ;

    SAM_PMC_REG->PMC_PCDR1 = MASK_STATUS1 ;
    while ( (SAM_PMC_REG->PMC_PCSR1 & MASK_STATUS1) != 0 ) ;

//    djy_printf( "Disable all periph clocks\n\r" ) ;
}

//------判断外设时钟是否使能---------------------------------------------------
//功能: 判断指定的外设ID号的外设时钟是否使能
//参数: dwId，外设ID，外设ID可参考手册39页
//返回: 0,除能   1,使能
//-----------------------------------------------------------------------------
uint32_t PMC_IsPeriphEnabled( uint32_t dwId )
{
    if(dwId > 34)
    	return 0xFF;

    if ( dwId < 32 )
    {
        return ( SAM_PMC_REG->PMC_PCSR0 & (1 << dwId) ) ;
    }
    else {
        return ( SAM_PMC_REG->PMC_PCSR1 & (1 << (dwId - 32)) ) ;
    }
}

