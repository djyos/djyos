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


#ifndef __LCDDRV_H_
#define __LCDDRV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include    "stdint.h"
#include    "gkernel.h"
#include    <gui/gkernel/gk_display.h>

/*===========================================================================*/
#define  LCD_WIDTH    ((uint16_t)640)             /* LCD PIXEL WIDTH */
#define  LCD_HEIGHT   ((uint16_t)480)             /* LCD PIXEL HEIGHT*/
#define CN_LCD_XSIZE  LCD_WIDTH
#define CN_LCD_YSIZE  LCD_HEIGHT

/**
  * @brief  LCD Timing
  */
#define  LCD_HSYNC            ((uint16_t)30)      /* Horizontal synchronization */
#define  LCD_HBP              ((uint16_t)114)     /* Horizontal back porch      */
#define  LCD_HFP              ((uint16_t)16)      /* Horizontal front porch     */
#define  LCD_VSYNC            ((uint16_t)3)       /* Vertical synchronization   */
#define  LCD_VBP              ((uint16_t)32)      /* Vertical back porch        */
#define  LCD_VFP              ((uint16_t)10)      /* Vertical front porch       */


#define  AMPIRE640480_WIDTH    ((uint16_t)640)             /* LCD PIXEL WIDTH            */
#define  AMPIRE640480_HEIGHT   ((uint16_t)480)             /* LCD PIXEL HEIGHT           */

/**
  * @brief  AMPIRE640480 Timing
  */
#define  AMPIRE640480_HSYNC            ((uint16_t)30)      /* Horizontal synchronization */
#define  AMPIRE640480_HBP              ((uint16_t)114)     /* Horizontal back porch      */
#define  AMPIRE640480_HFP              ((uint16_t)16)      /* Horizontal front porch     */
#define  AMPIRE640480_VSYNC            ((uint16_t)3)       /* Vertical synchronization   */
#define  AMPIRE640480_VBP              ((uint16_t)32)      /* Vertical back porch        */
#define  AMPIRE640480_VFP              ((uint16_t)10)      /* Vertical front porch       */

//#define LCD_FB_START_ADDRESS       ((uint32_t)0xC0000000)
#define CN_LCD_PIXEL_FORMAT CN_SYS_PF_ERGB4444

struct DisplayRsc* ModuleInstall_LCD(const char *DisplayName,const char* HeapName);

#ifdef __cplusplus
}
#endif

#endif   //__LCDDRV_H_
