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
// 模块描述:p1020_ddrcontroller_ctl.h/WB8298
// 模块版本: V1.0.0
// 创建人员: zhangqf_cyg
// 创建时间: 10:37:15 AM/Dec 4, 2013
// 备         注  ：
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================

#ifndef __DDRC_H
#define __DDRC_H

#include "SysSpace.h"

#define DEBUG_3_ADDR    (CN_CCSR_NEWADDR_V +0x2F08)
#define DEBUG_13_ADDR   (CN_CCSR_NEWADDR_V +0x2F30)
#define DEBUG_22_ADDR   (CN_CCSR_NEWADDR_V +0x2F54)
#define DEBUG_2_ADDR    (CN_CCSR_NEWADDR_V +0x2F04)

#define DDRmc1_CS0_BNDS_ADDR         (CN_CCSR_NEWADDR_V +0x2000)
#define DDRmc1_CS1_BNDS_ADDR         (CN_CCSR_NEWADDR_V +0x2008)
#define DDRmc1_CS0_CONFIG_ADDR       (CN_CCSR_NEWADDR_V +0x2080)
#define DDRmc1_CS1_CONFIG_ADDR       (CN_CCSR_NEWADDR_V +0x2084)
#define DDRmc1_CS0_CONFIG_2_ADDR     (CN_CCSR_NEWADDR_V +0x20C0)
#define DDRmc1_CS1_CONFIG_2_ADDR     (CN_CCSR_NEWADDR_V +0x20C4)
#define DDRmc1_TIMING_CFG_0_ADDR     (CN_CCSR_NEWADDR_V +0x2104)
#define DDRmc1_TIMING_CFG_1_ADDR     (CN_CCSR_NEWADDR_V +0x2108)
#define DDRmc1_TIMING_CFG_2_ADDR     (CN_CCSR_NEWADDR_V +0x210C)
#define DDRmc1_TIMING_CFG_3_ADDR     (CN_CCSR_NEWADDR_V +0x2100)
#define DDRmc1_TIMING_CFG_4_ADDR     (CN_CCSR_NEWADDR_V +0x2160)
#define DDRmc1_TIMING_CFG_5_ADDR     (CN_CCSR_NEWADDR_V +0x2164)
#define DDRmc1_SDRAM_CFG_ADDR        (CN_CCSR_NEWADDR_V +0x2110)
#define DDRmc1_SDRAM_CFG_2_ADDR      (CN_CCSR_NEWADDR_V +0x2114)
#define DDRmc1_SDRAM_INTERVAL_ADDR   (CN_CCSR_NEWADDR_V +0x2124)
#define DDRmc1_SDRAM_CLK_CNTL_ADDR   (CN_CCSR_NEWADDR_V +0x2130)
#define DDRmc1_ZQ_CNTL_ADDR          (CN_CCSR_NEWADDR_V +0x2170)
#define DDRmc1_WRLVL_CNTL_ADDR       (CN_CCSR_NEWADDR_V +0x2174)
#define DDRmc1_WRLVL_CNTL_2_ADDR     (CN_CCSR_NEWADDR_V +0x2190)
#define DDRmc1_WRLVL_CNTL_3_ADDR     (CN_CCSR_NEWADDR_V +0x2194)
#define DDRmc1_SDRAM_MODE_ADDR       (CN_CCSR_NEWADDR_V +0x2118)
#define DDRmc1_SDRAM_MODE_2_ADDR     (CN_CCSR_NEWADDR_V +0x211C)

#define DDRmc1_INIT_ADDR_ADDR        (CN_CCSR_NEWADDR_V +0x2148)

#define DDRmc1_INIT_EXT_ADDR_ADDR    (CN_CCSR_NEWADDR_V +0x214C)
#define DDRmc1_SDRAM_RCW_1_ADDR      (CN_CCSR_NEWADDR_V +0x2180)
#define DDRmc1_SDRAM_RCW_2_ADDR      (CN_CCSR_NEWADDR_V +0x2184)
#define DDRmc1_DATA_INIT_ADDR        (CN_CCSR_NEWADDR_V +0x2128)
#define DDRmc1_SDRAM_MD_CNTL_ADDR    (CN_CCSR_NEWADDR_V +0x2120)
#define DDRmc1_DDRCDR_1_ADDR         (CN_CCSR_NEWADDR_V +0x2B28)
#define DDRmc1_DDRCDR_2_ADDR         (CN_CCSR_NEWADDR_V +0x2B2C)

/* Errata: B4, T4 for v1.0
Before enabling the DDR controller, set the DDR memory mapped register space at offset 0xF70 to a value of 0x30003000."*/

#define DDRmc1_DDR_MEMORY_MAPPED_SPACE        (CN_CCSR_NEWADDR_V +0x2F70)
//#define DDRmc1_DDR_MEMORY_MAPPED_SPACE_VAL      0x30003000
//
//#define SDRAM_CFG_MEM_EN_MASK       0x80000000
//#define SDRAM_CFG2_D_INIT_MASK      0x00000010
//#define TIMING_CFG2_CPO_MASK            0xF07FFFFF
//#define CS0_BNDS_DEBUG3_MASK            0x00000400
//#define CS0_BNDS_DEBUG13_MASK           0x00000015
//#define CS0_BNDS_DEBUG22_MASK           0x24000000
//#define ZQ_CNTL_ZQ_EN_DISABLE_MASK      0x7FFFFFFF
//#define SDRAM_INTERVAL_REFINT_MASK  0x0000FFFF
//#define SDRAM_MD_CNTL_MASK                      0x84080000
//#define SDRAM_MD_CNTL_MD_EN_MASK        0x80000000
//#define SDRAM_MD_CNTL_CKE_CNTL_MASK     0x00200000
//#define WRLVL_CNTL_WRLVL_EN_DISABLE_MASK        0x7FFFFFFF
//#define SDRAM_CFG2_RCW_D_INT_DISABLE_MASK       0xFFFFFFEB
//
///* DDR Controller configured registers' values */
//#define DDRmc1_CS0_BNDS_VAL         0x3F
//#define DDRmc1_CS1_BNDS_VAL         0x00
//#define DDRmc1_CS0_CONFIG_VAL       0x80014302
//#define DDRmc1_CS1_CONFIG_VAL       0x00
//#define DDRmc1_CS0_CONFIG_2_VAL     0x00
//#define DDRmc1_CS1_CONFIG_2_VAL     0x00
//
//#define DDRmc1_TIMING_CFG_0_VAL     0x40210104
//#define DDRmc1_TIMING_CFG_1_VAL     0x5C5BE544
//#define DDRmc1_TIMING_CFG_2_VAL     0x0FA8910B
//#define DDRmc1_TIMING_CFG_3_VAL     0x00020000
//#define DDRmc1_TIMING_CFG_4_VAL     0x01
//#define DDRmc1_TIMING_CFG_5_VAL     0x02401400
//#define DDRmc1_SDRAM_CFG_VAL        0xC70C0008
//#define DDRmc1_SDRAM_CFG_2_VAL      0x24401010
//#define DDRmc1_SDRAM_INTERVAL_VAL   0x0A28028A
//#define DDRmc1_SDRAM_CLK_CNTL_VAL   0x02000000
//#define DDRmc1_ZQ_CNTL_VAL          0x89080600
//#define DDRmc1_WRLVL_CNTL_VAL       0x8655F605
//#define DDRmc1_WRLVL_CNTL_2_VAL     0x00
//#define DDRmc1_WRLVL_CNTL_3_VAL     0x00
//#define DDRmc1_SDRAM_MODE_VAL       0x00041220
//#define DDRmc1_SDRAM_MODE_2_VAL     0x00
//
//#define DDRmc1_DDRCDR_1_VAL         0x80040000
//#define DDRmc1_DDRCDR_2_VAL         0x00
//#define DDRmc1_INIT_ADDR_VAL        0x00000000
//
//#define DDRmc1_INIT_EXT_ADDR_VAL    0x00000000
//#define DDRmc1_SDRAM_RCW_1_VAL      0x00
//#define DDRmc1_SDRAM_RCW_2_VAL      0x00
//#define DDRmc1_DATA_INIT_VAL        0xDEADBEEF
//#define DDRmc1_SDRAM_MD_CNTL_VAL    0x00000000
#define DDRmc1_DDR_MEMORY_MAPPED_SPACE_VAL      0x30003000

#define SDRAM_CFG_MEM_EN_MASK       0x80000000
#define SDRAM_CFG2_D_INIT_MASK      0x00000010
#define TIMING_CFG2_CPO_MASK            0xF07FFFFF
#define CS0_BNDS_DEBUG3_MASK            0x00000400
#define CS0_BNDS_DEBUG13_MASK           0x00000015
#define CS0_BNDS_DEBUG22_MASK           0x24000000
#define ZQ_CNTL_ZQ_EN_DISABLE_MASK      0x7FFFFFFF
#define SDRAM_INTERVAL_REFINT_MASK  0x0000FFFF
#define SDRAM_MD_CNTL_MASK                      0x84080000
#define SDRAM_MD_CNTL_MD_EN_MASK        0x80000000
#define SDRAM_MD_CNTL_CKE_CNTL_MASK     0x00200000
#define WRLVL_CNTL_WRLVL_EN_DISABLE_MASK        0x7FFFFFFF
#define SDRAM_CFG2_RCW_D_INT_DISABLE_MASK       0xFFFFFFEB

/* DDR Controller configured registers' values */
#define DDRmc1_CS0_BNDS_VAL         0x3F
#define DDRmc1_CS1_BNDS_VAL         0x00
#define DDRmc1_CS0_CONFIG_VAL       0x80014302
#define DDRmc1_CS1_CONFIG_VAL       0x00
#define DDRmc1_CS0_CONFIG_2_VAL     0x00
#define DDRmc1_CS1_CONFIG_2_VAL     0x00

#define DDRmc1_TIMING_CFG_0_VAL     0x40210104
#define DDRmc1_TIMING_CFG_1_VAL     0x6D6BE544
#define DDRmc1_TIMING_CFG_2_VAL     0x0FA8918E
#define DDRmc1_TIMING_CFG_3_VAL     0x00020000
#define DDRmc1_TIMING_CFG_4_VAL     0x01
#define DDRmc1_TIMING_CFG_5_VAL     0x02401400
#define DDRmc1_SDRAM_CFG_VAL        0xC70C0008
#define DDRmc1_SDRAM_CFG_2_VAL      0x24401010
#define DDRmc1_SDRAM_INTERVAL_VAL   0x0A28028A
#define DDRmc1_SDRAM_CLK_CNTL_VAL   0x02000000
#define DDRmc1_ZQ_CNTL_VAL          0x89080600
#define DDRmc1_WRLVL_CNTL_VAL       0x8655F605
#define DDRmc1_WRLVL_CNTL_2_VAL     0x00
#define DDRmc1_WRLVL_CNTL_3_VAL     0x00
#define DDRmc1_SDRAM_MODE_VAL       0x00041220
#define DDRmc1_SDRAM_MODE_2_VAL     0x00

#define DDRmc1_DDRCDR_1_VAL         0x80040000
#define DDRmc1_DDRCDR_2_VAL         0x00
#define DDRmc1_INIT_ADDR_VAL        0x00000000

#define DDRmc1_INIT_EXT_ADDR_VAL    0x00000000
#define DDRmc1_SDRAM_RCW_1_VAL      0x00
#define DDRmc1_SDRAM_RCW_2_VAL      0x00
#define DDRmc1_DATA_INIT_VAL        0xDEADBEEF
#define DDRmc1_SDRAM_MD_CNTL_VAL    0x00000000
#endif /* __DDRC_H */
