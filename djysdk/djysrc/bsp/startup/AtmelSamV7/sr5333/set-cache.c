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
// -----------------------------------------------------------------------------
// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：sysinit.c
// 模块描述: CPU时钟的初始化和片内片外RAM等的初始化
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 08/13.2015
// =============================================================================
#include "stdint.h"
#include "cpu_peri.h"
#include "arch_feature.h"
#include "mpu.h"

#define memory_sync()        __DSB();__ISB();
#define memory_barrier()    __DSB()
// =============================================================================
// 功能：配置MPU，主要是配置了cache的区域，具体配置信息如下：
/* Default memory map
   Address range        Memory region      Memory type   Shareability  Cache policy
   0x00000000- 0x1FFFFFFF Code             Normal        Non-shareable  WT
   0x20000000- 0x3FFFFFFF SRAM             Normal        Non-shareable  WBWA
   0x40000000- 0x5FFFFFFF Peripheral       Device        Non-shareable  -
   0x60000000- 0x7FFFFFFF RAM              Normal        Non-shareable  WBWA
   0x80000000- 0x9FFFFFFF RAM              Normal        Non-shareable  WT
   0xA0000000- 0xBFFFFFFF Device           Device        Shareable
   0xC0000000- 0xDFFFFFFF Device           Device        Non Shareable
   0xE0000000- 0xFFFFFFFF System           -                  -
   */
// 参数：无
// 返回：无
// =============================================================================
void Cache_config( void )
{

    uint32_t dwRegionBaseAddr;
    uint32_t dwRegionAttr;

    memory_barrier();
/***************************************************
    ITCM memory region --- Normal
    START_Addr:-  0x00000000UL
    END_Addr:-    0x00400000UL
****************************************************/
    dwRegionBaseAddr =
        ITCM_START_ADDRESS |
        MPU_REGION_VALID |
        MPU_DEFAULT_ITCM_REGION;        // 1

    dwRegionAttr =
        MPU_AP_PRIVILEGED_READ_WRITE |
        MPU_REGION_EXECUTE_NEVER |
        MPU_CalMPURegionSize(ITCM_END_ADDRESS - ITCM_START_ADDRESS) |
        MPU_REGION_ENABLE;

    MPU_SetRegion( dwRegionBaseAddr, dwRegionAttr);

/****************************************************
    Internal flash memory region --- Normal read-only
    (update to Strongly ordered in write accesses)
    START_Addr:-  0x00400000UL
    END_Addr:-    0x00600000UL
******************************************************/

    dwRegionBaseAddr =
        IFLASH_START_ADDRESS |
        MPU_REGION_VALID |
        MPU_DEFAULT_IFLASH_REGION;      //2

    dwRegionAttr =
        MPU_AP_READONLY |
        INNER_NORMAL_WB_NWA_TYPE( NON_SHAREABLE ) |
        MPU_CalMPURegionSize(IFLASH_END_ADDRESS - IFLASH_START_ADDRESS) |
        MPU_REGION_ENABLE;

    MPU_SetRegion( dwRegionBaseAddr, dwRegionAttr);

/****************************************************
    DTCM memory region --- Normal
    START_Addr:-  0x20000000L
    END_Addr:-    0x20400000UL
******************************************************/

    /* DTCM memory region */
    dwRegionBaseAddr =
        DTCM_START_ADDRESS |
        MPU_REGION_VALID |
        MPU_DEFAULT_DTCM_REGION;         //3

    dwRegionAttr =
        MPU_AP_PRIVILEGED_READ_WRITE |
        MPU_REGION_EXECUTE_NEVER |
        MPU_CalMPURegionSize(DTCM_END_ADDRESS - DTCM_START_ADDRESS) |
        MPU_REGION_ENABLE;

    MPU_SetRegion( dwRegionBaseAddr, dwRegionAttr);

/****************************************************
    SRAM Cacheable memory region --- Normal
    START_Addr:-  0x20400000UL
    END_Addr:-    0x2043FFFFUL
******************************************************/
    /* SRAM memory  region */
    dwRegionBaseAddr =
        SRAM_FIRST_START_ADDRESS |
        MPU_REGION_VALID |
        MPU_DEFAULT_SRAM_REGION_1;         //4

    dwRegionAttr =
        MPU_AP_FULL_ACCESS    |
		MPU_REGION_EXECUTE_NEVER |
        INNER_NORMAL_WB_NWA_TYPE( NON_SHAREABLE ) |
        MPU_CalMPURegionSize(SRAM_FIRST_END_ADDRESS - SRAM_FIRST_START_ADDRESS)
        | MPU_REGION_ENABLE;

    MPU_SetRegion( dwRegionBaseAddr, dwRegionAttr);


/****************************************************
    Internal SRAM second partition memory region --- Normal
    START_Addr:-  0x20440000UL
    END_Addr:-    0x2045FFFFUL
******************************************************/
    /* SRAM memory region */
    dwRegionBaseAddr =
        SRAM_SECOND_START_ADDRESS |
        MPU_REGION_VALID |
        MPU_DEFAULT_SRAM_REGION_2;         //5

    dwRegionAttr =
        MPU_AP_FULL_ACCESS    |
        MPU_REGION_EXECUTE_NEVER |
        INNER_NORMAL_WB_NWA_TYPE( NON_SHAREABLE ) |
        MPU_CalMPURegionSize(SRAM_SECOND_END_ADDRESS - SRAM_SECOND_START_ADDRESS) |
        MPU_REGION_ENABLE;

    MPU_SetRegion( dwRegionBaseAddr, dwRegionAttr);

#ifdef MPU_HAS_NOCACHE_REGION
    dwRegionBaseAddr =
        SRAM_NOCACHE_START_ADDRESS |
        MPU_REGION_VALID |
        MPU_NOCACHE_SRAM_REGION;          //11

    dwRegionAttr =
        MPU_AP_FULL_ACCESS    |
        MPU_REGION_EXECUTE_NEVER |
        INNER_OUTER_NORMAL_NOCACHE_TYPE( SHAREABLE ) |
        MPU_CalMPURegionSize(NOCACHE_SRAM_REGION_SIZE) |
        MPU_REGION_ENABLE;

    MPU_SetRegion( dwRegionBaseAddr, dwRegionAttr);
#endif

/****************************************************
    Peripheral memory region --- DEVICE Shareable
    START_Addr:-  0x40000000UL
    END_Addr:-    0x5FFFFFFFUL
******************************************************/
    dwRegionBaseAddr =
        PERIPHERALS_START_ADDRESS |
        MPU_REGION_VALID |
        MPU_PERIPHERALS_REGION;          //6

    dwRegionAttr = MPU_AP_FULL_ACCESS |
        MPU_REGION_EXECUTE_NEVER |
        SHAREABLE_DEVICE_TYPE |
        MPU_CalMPURegionSize(PERIPHERALS_END_ADDRESS - PERIPHERALS_START_ADDRESS)
        |MPU_REGION_ENABLE;

    MPU_SetRegion( dwRegionBaseAddr, dwRegionAttr);


/****************************************************
    External EBI memory  memory region --- Strongly Ordered
    START_Addr:-  0x60000000UL
    END_Addr:-    0x6FFFFFFFUL
******************************************************/
    dwRegionBaseAddr =
            EXT_EBI_START_ADDRESS |
            MPU_REGION_VALID |
            MPU_EXT_EBI_REGION;          //11

	dwRegionAttr =
		MPU_AP_FULL_ACCESS |
		MPU_REGION_EXECUTE_NEVER |
		/* External memory Must be defined with 'Device' or 'Strongly Ordered'
		attribute for write accesses (AXI) */
		STRONGLY_ORDERED_SHAREABLE_TYPE |
		MPU_CalMPURegionSize(EXT_EBI_END_ADDRESS - EXT_EBI_START_ADDRESS) |
		MPU_REGION_ENABLE;

	MPU_SetRegion( dwRegionBaseAddr, dwRegionAttr);
/****************************************************
    SDRAM Cacheable memory region --- Normal
    START_Addr:-  0x70000000UL
    END_Addr:-    0x7FFFFFFFUL
******************************************************/
    dwRegionBaseAddr =
        SDRAM_START_ADDRESS |
        MPU_REGION_VALID |
        MPU_DEFAULT_SDRAM_REGION;        //7

    dwRegionAttr =
        MPU_AP_FULL_ACCESS    |
        MPU_REGION_EXECUTE_NEVER |
        //INNER_NORMAL_WB_NWA_TYPE( SHAREABLE ) |
        INNER_NORMAL_NOCACHE_TYPE(NON_SHAREABLE)|
        MPU_CalMPURegionSize(SDRAM_END_ADDRESS - SDRAM_START_ADDRESS) |
        MPU_REGION_ENABLE;

    MPU_SetRegion( dwRegionBaseAddr, dwRegionAttr);
/****************************************************
    QSPI memory region --- Strongly ordered
    START_Addr:-  0x80000000UL
    END_Addr:-    0x9FFFFFFFUL
******************************************************/
    dwRegionBaseAddr =
        QSPI_START_ADDRESS |
        MPU_REGION_VALID |
        MPU_QSPIMEM_REGION;              //8

    dwRegionAttr =
        MPU_AP_FULL_ACCESS |
        MPU_REGION_EXECUTE_NEVER |
        STRONGLY_ORDERED_SHAREABLE_TYPE |
        MPU_CalMPURegionSize(QSPI_END_ADDRESS - QSPI_START_ADDRESS) |
        MPU_REGION_ENABLE;

    MPU_SetRegion( dwRegionBaseAddr, dwRegionAttr);


/****************************************************
    USB RAM Memory region --- Device
    START_Addr:-  0xA0100000UL
    END_Addr:-    0xA01FFFFFUL
******************************************************/
    dwRegionBaseAddr =
        USBHSRAM_START_ADDRESS |
        MPU_REGION_VALID |
        MPU_USBHSRAM_REGION;              //9

    dwRegionAttr =
        MPU_AP_FULL_ACCESS |
        MPU_REGION_EXECUTE_NEVER |
        SHAREABLE_DEVICE_TYPE |
        MPU_CalMPURegionSize(USBHSRAM_END_ADDRESS - USBHSRAM_START_ADDRESS) |
        MPU_REGION_ENABLE;

    MPU_SetRegion( dwRegionBaseAddr, dwRegionAttr);
    /* Enable the memory management fault , Bus Fault, Usage Fault exception */
    SCB->SHCSR |= (SCB_SHCSR_MEMFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk
                    | SCB_SHCSR_USGFAULTENA_Msk);

    /* Enable the MPU region */
    MPU_Enable( MPU_ENABLE | MPU_PRIVDEFENA);

    memory_sync();
}


