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
// 模块描述: 定时器硬件接口层数据定义以及接口声明
// 模块版本: V1.00
// 创建人员: hm
// 创建时间: 18/12, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 备注

#ifndef CPU_PERI_TC_H_
#define CPU_PERI_TC_H_

#include "stdint.h"

#ifndef WoReg
#define WoReg vu32
#define RwReg vu32
#define RoReg vu32
#endif

typedef struct {
  RwReg      TC_CCR;        /**< \brief (TcChannel Offset: 0x0) Channel Control Register */
  RwReg      TC_CMR;        /**< \brief (TcChannel Offset: 0x4) Channel Mode Register */
  RwReg      TC_SMMR;       /**< \brief (TcChannel Offset: 0x8) Stepper Motor Mode Register */
  RoReg      Reserved1[1];
  RwReg      TC_CV;         /**< \brief (TcChannel Offset: 0x10) Counter Value */
  RwReg      TC_RA;         /**< \brief (TcChannel Offset: 0x14) Register A */
  RwReg      TC_RB;         /**< \brief (TcChannel Offset: 0x18) Register B */
  RwReg      TC_RC;         /**< \brief (TcChannel Offset: 0x1C) Register C */
  RwReg      TC_SR;         /**< \brief (TcChannel Offset: 0x20) Status Register */
  RwReg      TC_IER;        /**< \brief (TcChannel Offset: 0x24) Interrupt Enable Register */
  RwReg      TC_IDR;        /**< \brief (TcChannel Offset: 0x28) Interrupt Disable Register */
  RwReg      TC_IMR;        /**< \brief (TcChannel Offset: 0x2C) Interrupt Mask Register */
  RoReg      Reserved2[4];
} tagTcChannel;

#define TCCHANNEL_NUMBER 3
typedef struct {
  tagTcChannel  TC_CHANNEL[TCCHANNEL_NUMBER]; /**< \brief (Tc Offset: 0x0) channel = 0 .. 2 */
  WoReg      TC_BCR;        /**< \brief (Tc Offset: 0xC0) Block Control Register */
  RwReg      TC_BMR;        /**< \brief (Tc Offset: 0xC4) Block Mode Register */
  WoReg      TC_QIER;       /**< \brief (Tc Offset: 0xC8) QDEC Interrupt Enable Register */
  WoReg      TC_QIDR;       /**< \brief (Tc Offset: 0xCC) QDEC Interrupt Disable Register */
  RoReg      TC_QIMR;       /**< \brief (Tc Offset: 0xD0) QDEC Interrupt Mask Register */
  RoReg      TC_QISR;       /**< \brief (Tc Offset: 0xD4) QDEC Interrupt Status Register */
  RwReg      TC_FMR;        /**< \brief (Tc Offset: 0xD8) Fault Mode Register */
  RoReg      Reserved1[2];
  RwReg      TC_WPMR;       /**< \brief (Tc Offset: 0xE4) Write Protect Mode Register */
} tagTcReg;


#endif /* TIMER_HARD_H_ */
