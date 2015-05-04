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
// 模块描述: 看门狗硬件抽象层数据结构以及接口声明
// 模块版本: V1.00
// 创建人员: zhangqf_cyg
// 创建时间: 7:53:48 PM/Apr 14, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 备注

#ifndef WDT_HAL_H_
#define WDT_HAL_H_
#include "stdint.h"

//看门狗硬件的基本信息
 struct tagWdtHalChipInfo
{
	char*  wdtchip_name;             //chip名字             
	u32    wdtchip_cycle;            //chip cycle,硬件周期
	bool_t (*wdtchip_feed)(void);    //chip feed	
};

//在启动阶段，实时中断都可以使用，因此用户完全可以自己申请使用硬件定时来喂狗
//甚至有些外设的喂狗是可以自己自动设置的，因此WDTHAL不再主动申请定时器喂狗，完全由
//用户自己决定采用何种方式，只给用户提供注册HOOK的接口。
typedef bool_t (*fnWdtBootFeedStart)(u32 bootfeedtime);  //在启动阶段喂狗开始HOOK
typedef bool_t (*fnWdtBootFeedEnd)(void);                //在启动阶段喂狗结束HOOK
 
bool_t WdtHal_BootStart(u32 bootfeedtimes);
bool_t WdtHal_BootEnd(void);
bool_t WdtHal_GetChipPara(struct tagWdtHalChipInfo *hardpara);
bool_t WdtHal_RegisterWdtChip(char *chipname, u32 yipcycle,\
		                      bool_t (*wdtchip_feed)(void),\
		                      fnWdtBootFeedStart bootfeedstart,\
		                      fnWdtBootFeedEnd   bootfeedend);

#endif /* WDT_HAL_H_ */
