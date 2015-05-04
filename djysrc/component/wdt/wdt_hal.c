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
// 模块描述: 看门狗硬件抽象层
// 模块版本: V1.00
// 创建人员: zhangqf_cyg
// 创建时间: 7:52:28 PM/Apr 14, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 备注：看门狗硬件抽象层，专门为虚拟看门狗服务
#include "stdint.h"
#include "stddef.h"

#include "os.h"

#include "wdt_hal.h"

#define CN_WDTHAL_HARDCYCLE_SCALE    8/10      //缩放比例
///////禁止其他文件以任何技术手段引用修改该文件定义的全局静态变量（后果自负）///////
static struct tagWdtHalChipInfo    s_tWdtHalChipInfo; //存储注册的WDT信息
static fnWdtBootFeedStart          fnBootFeedStart = NULL;
static fnWdtBootFeedEnd            fnBootFeedEnd = NULL;

// =============================================================================
// 函数功能：WdtHal_BootStart
//          BOOT中喂狗启动
// 输入参数：bootfeedtime,在boot中主动喂狗时间，单位为微秒
// 输出参数：无
// 返回值  ：true成功 false失败(没有看门狗硬件或者无需启动定时器喂狗)
// 说明    ：具体的操作方法由用户自己在注册硬件看门狗的时候提供
// =============================================================================
bool_t WdtHal_BootStart(u32 bootfeedtime)
{
    bool_t result = true;
    if(NULL != fnBootFeedStart)
    {
    	result = fnBootFeedStart(bootfeedtime);
    }
    
    return result;
}

// =============================================================================
// 函数功能：WdtHal_BootEnd
//          BOOT中喂狗结束
// 输入参数：bootfeedtimes
// 输出参数：无
// 返回值  ：true 成功  fasle失败
// 注意    ：
// =============================================================================
bool_t WdtHal_BootEnd(void)
{
	bool_t result = true;
	if(NULL != fnBootFeedEnd)
	{
		result = fnBootFeedEnd();
	}
	return result;
}

// =============================================================================
// 函数功能：WdtHal_GetChipPara
//          获取硬件芯片参数
// 输入参数：
// 输出参数：hardpara,存储获取的看门狗芯片信息
// 返回值  ：true成功 false失败
// 说明    ：务必检查返回值
// =============================================================================
bool_t WdtHal_GetChipPara(struct tagWdtHalChipInfo *hardpara)
{
    if(NULL != s_tWdtHalChipInfo.wdtchip_feed)
    {
        *hardpara = s_tWdtHalChipInfo;
        hardpara->wdtchip_name = s_tWdtHalChipInfo.wdtchip_name;
        hardpara->wdtchip_cycle = s_tWdtHalChipInfo.wdtchip_cycle;
        hardpara->wdtchip_feed = s_tWdtHalChipInfo.wdtchip_feed;
        return true;
    }
    else
    {
        return false;
    }
}

// =============================================================================
// 函数功能：WdtHal_RegisterWdtChip
//          注册硬件看门狗芯片
// 输入参数：chipname,芯片名字
//           yipcycle,芯片的周期，老老实实的交代就OK，内部会做参数评估的
//           可以设置CYCLE的WDT，该值为设置好后的CYCLE
//           wdtchip_feed,硬件看门狗的喂狗方法
//           bootfeedstart,在启动过程中开始喂狗
//           bootfeedend,在启动过程中结束喂狗
// 返回值  ：true成功 false失败
// 说明    ：失败一定是参数不正确，name存储的地方一定是const型，千万别是局部变量
// =============================================================================
bool_t WdtHal_RegisterWdtChip(char *chipname, u32 yipcycle,\
		                      bool_t (*wdtchip_feed)(void),\
		                      fnWdtBootFeedStart bootfeedstart,\
		                      fnWdtBootFeedEnd   bootfeedend)
{
    bool_t  result;

    if(NULL == wdtchip_feed)
    {
        result = false;
    }
    else
    {
        s_tWdtHalChipInfo.wdtchip_name = chipname;
        s_tWdtHalChipInfo.wdtchip_cycle = yipcycle*CN_WDTHAL_HARDCYCLE_SCALE;
        s_tWdtHalChipInfo.wdtchip_feed  = wdtchip_feed;
        fnBootFeedStart = bootfeedstart;
        fnBootFeedEnd = bootfeedend;
        
        result = true;
    }

    return result;
}

