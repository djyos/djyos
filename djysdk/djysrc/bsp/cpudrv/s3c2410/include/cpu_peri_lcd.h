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

struct LCD_REG
{
   vu32 VIDCON0;             // 0x00
   vu32 VIDCON1;             // 0x04
   vu32 VIDTCON0;             // 0x08
   vu32 VIDTCON1;             // 0x0C
   vu32 VIDTCON2;             // 0x10
   vu32 WINCON0;           // 0x14
   vu32 WINCON1;           // 0x18
   vu32 PAD[3];

   vu32 VIDOSD0A;           // 0x28
   vu32 VIDOSD0B;              // 0x2C
   vu32 VIDOSD0C;            // 0x30
   vu32 VIDOSD1A;             // 0x34
   vu32 VIDOSD1B;             // 0x38
   vu32 VIDOSD1C;             // 0x3C

   vu32 PAD1[9];              // 0x40 ~ 0x60 // PAD

   vu32 VIDW00ADD0B0;             // 0x64
   vu32 VIDW00ADD0B1;             // 0x68
   vu32 VIDW01ADD0;               // 0x6C

   vu32 PAD2[3];              // 0x70 ~ 0x78 // PAD

   vu32 VIDW00ADD1B0;             // 0x7C
   vu32 VIDW00ADD1B1;             // 0x80
   vu32 VIDW01ADD1;               // 0x84

   vu32 PAD3[3];              // 0x88 ~ 0x90 // PAD

   vu32 VIDW00ADD2B0;             // 0x94
   vu32 VIDW00ADD2B1;             // 0x98
   vu32 VIDW01ADD2;               // 0x9C

   vu32 PAD4[3];              // 0xA0 ~ 0xA8 // PAD

   vu32 VIDINTCON;                // 0xAC
   vu32 W1KEYCON0;                // 0xB0
   vu32 W1KEYCON1;                // 0xB4
   vu32 W2KEYCON0;                // 0xB8
   vu32 W2KEYCON1;                // 0xBC
   vu32 W3KEYCON0;                // 0xC0
   vu32 W3KEYCON1;                // 0xC4
   vu32 W4KEYCON0;                // 0xC8
   vu32 W4KEYCON1;                // 0xCC
   vu32 WIN0MAP;                  // 0xD0
   vu32 WIN1MAP;                  // 0xD4

   vu32 PAD5[3];              // 0xD8 ~ 0xE0 // PAD

   vu32 WPALCON;                  // 0xE4

   vu32 PAD6[18];              // 0xE8 ~ 0x12C // PAD

   vu32 SYSIFCON0;                // 0x130
   vu32 SYSIFCON1;                // 0x134
   vu32 DITHMODE;                 // 0x138
};

#define LCD_REG_BASE    ((struct LCD_REG*)0x4C800000)

struct DisplayRsc* ModuleInstall_LCD(const char *DisplayName,const char* HeapName);

#ifdef __cplusplus
}
#endif

#endif   //__LCDDRV_H_
