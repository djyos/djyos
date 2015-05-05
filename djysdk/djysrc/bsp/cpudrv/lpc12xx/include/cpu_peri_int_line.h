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
//所属模块:中断模块
//作者：lst
//版本：V1.0.0
//文件描述: 定义中断号
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009--9-28
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __CPU_PERI_INT_LINE_H__
#define __CPU_PERI_INT_LINE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define cn_int_line_wakeup0         0x00  //wakeup from gpio0.0
#define cn_int_line_wakeup1         0x01  //wakeup from gpio0.1
#define cn_int_line_wakeup2         0x02  //wakeup from gpio0.2
#define cn_int_line_wakeup3         0x03  //wakeup from gpio0.3
#define cn_int_line_wakeup4         0x04  //wakeup from gpio0.4
#define cn_int_line_wakeup5         0x05  //wakeup from gpio0.5
#define cn_int_line_wakeup6         0x06  //wakeup from gpio0.6
#define cn_int_line_wakeup7         0x07  //wakeup from gpio0.7
#define cn_int_line_wakeup8         0x08  //wakeup from gpio0.8
#define cn_int_line_wakeup9         0x09  //wakeup from gpio0.9
#define cn_int_line_wakeup10        0x0A  //wakeup from gpio0.10
#define cn_int_line_wakeup11        0x0B  //wakeup from gpio0.11
#define cn_int_line_iic             0x0C
#define cn_int_line_timer16_0       0x0D
#define cn_int_line_timer16_1       0x0E
#define cn_int_line_timer32_0       0x0F
#define cn_int_line_timer32_1       0x10
#define cn_int_line_ssp             0x11
#define cn_int_line_uart0           0x12
#define cn_int_line_uart1           0x13
#define cn_int_line_comparator      0x14
#define cn_int_line_adc             0x15
#define cn_int_line_wdt             0x16
#define cn_int_line_bod             0x17
#define cn_int_line_fmc             0x18
#define cn_int_line_PIOINT0         0x19
#define cn_int_line_PIOINT1         0x1A
#define cn_int_line_PIOINT2         0x1B
#define cn_int_line_pmu             0x1C
#define cn_int_line_dma             0x1D
#define cn_int_line_rtc             0x1E
#define cn_int_line_edm             0x1F
#define CN_INT_LINE_LAST            0x1F

#ifdef __cplusplus
}
#endif

#endif //__CPU_PERI_INT_LINE_H__

