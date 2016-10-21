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
// 模块描述: SysExp_Shell.c
// 模块版本: V1.00
// 创建人员: zhangqf_cyg
// 创建时间: 9:20:36 AM/Mar 27, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 备注:该文件主要是为了实现SHELL的一些接口功能


#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "exp.h"
#include "exp_record.h"
#include "shell.h"
#include "systime.h"
bool_t  Exp_InfoDecoder(struct ExpRecordPara *recordpara);

extern char *Sh_GetWord(char *buf,char **next);
// =============================================================================
// 函数功能：Exp_ShellBrowseAssignedRecord
//           shell查看指定条目的异常信息
// 输入参数：param,指定条目字符串
// 输出参数：无
// 返回值  ：true成功  false失败
// 说明    ：
// =============================================================================
bool_t Exp_ShellBrowseAssignedRecord(char *param)
{
    bool_t   result;
    char *word,*next_param;
    u32  item_num;
    u32  item_len;
    u8   *infomem;
    struct ExpRecordPara recordpara;

    if(NULL == param)
    {
        result = false;
        goto browse_end;
    }

    word = Sh_GetWord(param,&next_param);
    item_num = strtol(word, (char **)NULL, 0);

    result = Exp_RecordCheckLen(item_num, &item_len);
    if(false == result)
    {
        printf("请指定正确的条目号\n\r");
        return true;
    }

    infomem = M_MallocLc(item_len, CN_TIMEOUT_FOREVER);
    if(NULL == infomem)
    {
        printf("请指定正确的条目号\n\r");
        return true;
    }

    result = Exp_RecordGet(item_num,item_len,infomem, &recordpara);
    if(false == result)
        printf("读取异常信息失败\n\r");
    else
        Exp_InfoDecoder(&recordpara);

    free(infomem);
    return true;

browse_end:
    return result;

}
// =============================================================================
// 函数功能：Exp_ShellBrowseRecordNum
//           shell查看存储的异常条目数
// 输入参数：param,无
// 输出参数：无
// 返回值  ：true成功  false失败
// 说明    ：
// =============================================================================
bool_t Exp_ShellBrowseRecordNum(char *param)
{
    bool_t  result;

    u32 expnum;

    result = Exp_RecordCheckNum(&expnum);
    if(true == result)
    {
        printf("ShellSysExp:There has been 0x%08x Exp record!\n\r",expnum);
        if(expnum > 0)
        {
            printf("ShellSysExp:VALID NO.(1-%d)\n\r",expnum);
        }
        else
        {
            printf("There has been no exception record yet!\n\r");
        }
    }
    else
        printf("Get Exception Number Failed\n\r");
    return result;
}

// =============================================================================
// 函数功能：Exp_ShellRecordClear
//           shell清除所有的异常信息
// 输入参数：param,无
// 输出参数：无
// 返回值  ：true成功  false失败
// 说明    ：
// =============================================================================
bool_t Exp_ShellRecordClear(char *param)
{
    bool_t  result;

    result = Exp_RecordClear();

    if(true == result)
    {
        printf("\n\rShellSysExp:Clear SysExp info success!");
    }
    else
    {
        printf("\n\rShellSysExp:Clear SysExp info failed!");
    }

    return result;
}
struct ShellCmdTab  gtExpShellCmdTab[] =
{
    {
        "expi",
        Exp_ShellBrowseAssignedRecord,
        "browse the specified exception item",
        "expi item\n\r"
    },
    {
        "expn",
        Exp_ShellBrowseRecordNum,
        "browse the exception number",
        "expn"
    },
    {
        "expc",
        Exp_ShellRecordClear,
        "clear all the exception item",
        "expc"
    }
};
#define CN_EXPSHELL_NUM  ((sizeof(gtExpShellCmdTab))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc sgExpShellRsc[CN_EXPSHELL_NUM];
// =============================================================================
// 函数功能：Exp_ShellInit
//          expshell模块初始化
// 输入参数：param,无
// 输出参数：无
// 返回值  ：true成功  false失败
// 说明    ：
// =============================================================================
bool_t Exp_ShellInit()
{
    bool_t result;
    if(Sh_InstallCmd(gtExpShellCmdTab,sgExpShellRsc,CN_EXPSHELL_NUM))
    {
        printf("Install Exception Module Success\n\r");
        result = true;
    }
    else
    {
        printf("Install Exception Module Failed\n\r");
        result = false;
    }
    return result;
}

