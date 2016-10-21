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

#include    "gkernel.h"


#define LCDCON4_MVAL        (13)
#define LCDCON1_MMODE       (0)         //0=每帧   1=由MVAL定义此频率
#define LCDCON5_INVVDEN     (1)         //0=正常       1=反转
#define LCDCON5_BSWP        (0)         //字节交换控制位 
#define LCDCON5_HWSWP       (1)         //半字节交换控制位 
#define LCDCON1_PNRMODE     (3)         // 设置为TFT屏
#define LCDCON1_BPPMODE     (12)        // 设置为16bpp模式

#define LCDCON2_VBPD        (12)        //垂直同步信号的后肩
#define LCDCON2_VFPD        (4)         //垂直同步信号的前肩
#define LCDCON2_VSPW        (5)         //垂直同步信号的脉宽

#define LCDCON3_HBPD        (22)        //水平同步信号的后肩
#define LCDCON3_HFPD        (33)        //水平同步信号的前肩
#define LCDCON4_HSPW        (44)        //水平同步信号的脉宽

#define LCDCON1_CLKVAL      (6) 

//hjj
#define HOZVAL_TFT          (cn_lcd_xsize-1)
#define LINEVAL_TFT         (cn_lcd_ysize-1)

// LCD CONTROLLER
struct LCD_REG            //0x4d000000
{
    vu32 LCDCON1;            //0x00,LCD控制1寄存器
    vu32 LCDCON2;            //0x04,LCD控制2寄存器
    vu32 LCDCON3;            //0x08,LCD控制3寄存器
    vu32 LCDCON4;            //0x0c,LCD控制4寄存器
    vu32 LCDCON5;            //0x10,LCD控制5寄存器
    vu32 LCDSADDR1;          //0x14,帧缓冲器开始地址1寄存器
    vu32 LCDSADDR2;          //0x18,帧缓冲器开始地址2寄存器
    vu32 LCDSADDR3;          //0x1c,虚拟屏地址设置
    vu32 REDLUT;             //0x20,红色查找表寄存器
    vu32 GREENLUT;           //0x24,绿色查找表寄存器
    vu32 BLUELUT;            //0x28,蓝色查找表寄存器
    vu32 revs[8];
    vu32 DITHMODE;           //0x4c,抖动模式寄存器
    vu32 TPAL;               //0x50,临时调色板寄存器
    vu32 LCDINTPND;          //0x54,LCD中断挂起寄存器
    vu32 LCDSRCPND;          //0x58,LCD源挂起寄存器
    vu32 LCDINTMSK;          //0x5c,LCD中断屏蔽寄存器
    vu32 TCONSEL;            //0x60,控制 LPC3600/LCC3600模式寄存器
    
};

#define LCD_REG_BASE    ((struct LCD_REG*)0x4D000000)

struct DisplayRsc* ModuleInstall_LCD(const char *DisplayName,const char* HeapName);

#ifdef __cplusplus
}
#endif

#endif   //__LCDDRV_H_
