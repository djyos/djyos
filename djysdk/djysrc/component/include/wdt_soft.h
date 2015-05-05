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
// 模块描述: 软件看门狗的接口定义
// 模块版本: V1.00
// 创建人员: zhangqf_cyg
// 创建时间: 3:48:11 PM/Dec 18, 2013
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 备注
#ifndef WDT_SOFT_H_
#define WDT_SOFT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

struct __tagWdt;
typedef u32 (* fnYipHook)(struct __tagWdt *wdt);
//the struct of the wdt
typedef struct __tagWdt
{
    struct __tagWdt *ppre;        //双向不循环链表指针-前
    struct __tagWdt *pnxt;        //双向不循环链表指针-后
    char            *pname;       //看门狗名字，指向静态或者常量字符串
    fnYipHook       fnhook;       //狗叫善后钩子函数
    u32             action;       //狗叫动作
    u32             cycle;        //看门狗周期，单位：微秒
    u32             taskownerid;  //看门狗所属任务ID
    s64             deadtime;     //看门狗喂狗截止时间，到此时间还不喂，则狗叫，单位：微秒
    s64             runtime;      //上次操作看门狗时看门狗所属任务的运行时间，单位：微秒
    u32             timeoutreason;     //看门狗狗叫原因
    u16             shyiptimes;        //调度原因引起的看门狗狗叫次数   ，单位：次
    u32             runtimelevel;      //看门狗狗叫原因判断标准，单位：微秒
    u16             sheduletimeslevel; //看门狗狗叫调度原因连续次数，单位：次
}tagWdt;

typedef struct
{
    u32  runtimelevel;        //当看门狗叫时，用于判断是逻辑错误还是调度延时问题
                              //当任务运行时间大于runtime,判定为逻辑错误，即有运行却不喂狗
                              //当任务运行时间小于runtime,判定为调度问题，没有足够运行时间
    u16  shtimeoutlevel;      //连续的调度问题引起的狗叫次数限制，超过该限制则执行HOOK，否则不执行
}tagWdtTolerate;              //看门狗的容忍限度

enum _EN_WDT_CMD
{
    EN_WDTCMD_ADD =0,
    EN_WDTCMD_DEL,
    EN_WDTCMD_CLEAN,
    //the following the api could use, but the up not
    EN_WDTCMD_PAUSE,
    EN_WDTCMD_RESUME,
    EN_WDTCMD_SETTASKID,
    EN_WDTCMD_SETCYCLE,                  //设置狗叫周期
    EN_WDTCMD_SETYIPACTION,              //设置狗叫动作
    EN_WDTCMD_SETYIPHOOK,                //设置狗叫善后钩子
    EN_WDTCMD_SETCONSUMEDTIMELEVEL,      //设置狗叫逻辑错误时间触发水平
    EN_WDTCMD_SETSCHLEVEL,               //设置连续狗叫逻辑错误容忍限度
    EN_WDTCMD_INVALID,                   //无效操作
};

ptu32_t ModuleInstall_Wdt(ptu32_t para);
tagWdt *Wdt_Create(char *dogname,u32 yip_cycle,\
        fnYipHook yiphook,ptu32_t yip_action, tagWdtTolerate *levelpara);
tagWdt *Wdt_Create_s(tagWdt *wdt, char *dogname,u32 yip_cycle,\
        fnYipHook yiphook,ptu32_t yip_action, tagWdtTolerate *levelpara);
bool_t Wdt_Delete(tagWdt *wdt);
bool_t Wdt_Delete_s(tagWdt *wdt);
bool_t Wdt_Clean(tagWdt *wdt);
bool_t Wdt_Ctrl(tagWdt *wdt, u32 type, ptu32_t para);

#ifdef __cplusplus
}
#endif

#endif //WDTSOFT_H_

