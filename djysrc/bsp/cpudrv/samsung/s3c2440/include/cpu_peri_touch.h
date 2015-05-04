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

#ifndef __CPU_PERI_TOUCH_H__
#define __CPU_PERI_TOUCH_H__
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
  此处为新增加的定义，用于触摸屏
  2012.09.04
*/

// 引入LCD的驱动，借用LCD_Type的定义，从而确定触摸屏的物理定义
#include "cpu_peri_lcd.h"


#define TOUCH_X1_BIT            5               // X1的ADC端口
#define TOUCH_X2_BIT            7               // X2的ADC端口
#define TOUCH_Y1_BIT            4               // Y1的ADC端口
#define TOUCH_Y2_BIT            6               // Y2的ADC端口

#if (LCD_Type == LCDW35)
#define TOUCH_WIDTH             320             // 触摸屏的水平分辨率
#define TOUCH_HEIGHT            240             // 触摸屏的垂直分辨率
#define TOUCH_DEFAULT_LB        80              // 左边缘对应的默认AD转换值(16bit)
#define TOUCH_DEFAULT_RB        940             // 右边缘对应的默认AD转换值(16bit)
#define TOUCH_DEFAULT_TB        130             // 上边缘对应的默认AD转换值(16bit)
#define TOUCH_DEFAULT_BB        900             // 下边缘对应的默认AD转换值(16bit)

#elif (LCD_Type == LCDW43)
#define TOUCH_WIDTH             480             // 触摸屏的水平分辨率
#define TOUCH_HEIGHT            272             // 触摸屏的垂直分辨率
#define TOUCH_DEFAULT_LB        70   //80       // 左边缘对应的默认AD转换值(16bit)
#define TOUCH_DEFAULT_RB        970  //950      // 右边缘对应的默认AD转换值(16bit)
#define TOUCH_DEFAULT_TB        150  //200      //x, 上边缘对应的默认AD转换值(16bit)
#define TOUCH_DEFAULT_BB        900  //880      //x, 下边缘对应的默认AD转换值(16bit)

#elif (LCD_Type == LCDA70)
#define TOUCH_WIDTH             800             // 触摸屏的水平分辨率
#define TOUCH_HEIGHT            480             // 触摸屏的垂直分辨率
#define TOUCH_DEFAULT_LB        65              // 左边缘对应的默认AD转换值(16bit)
#define TOUCH_DEFAULT_RB        960             // 右边缘对应的默认AD转换值(16bit)
#define TOUCH_DEFAULT_TB        135             // 上边缘对应的默认AD转换值(16bit)
#define TOUCH_DEFAULT_BB        905             // 下边缘对应的默认AD转换值(16bit)
#endif


#define TOUCH_DEBOUNCE_PIXEL    8  // 去抖动像素数，即触摸坐标稳定在半径为多少像素的范围内才输出结果
#define TOUCH_DEBOUNCE_TIME             5       // 去抖动时间，单位为32*ADC转换时间

///////////////////////////////////////////////////////////////////
//      以下固定
#define TOUCH_X1                        (0x0001<<TOUCH_X1_BIT)
#define TOUCH_X2                        (0x0001<<TOUCH_X2_BIT)
#define TOUCH_Y1                        (0x0001<<TOUCH_Y1_BIT)
#define TOUCH_Y2                        (0x0001<<TOUCH_Y2_BIT)

/*
bool_t touch_hard_init(void);
ufast_t touch_scan_hard(struct tagSingleTouchMsg *touch_data);
void touch_ratio_adjust(char *display_name,struct tagST_TouchAdjust *adjust);
*/

#ifdef __cplusplus
}
#endif

#endif /* __CPU_PERI_TOUCH_H__ */
