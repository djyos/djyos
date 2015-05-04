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
// 模块描述: 异常信息存储方案接口
// 模块版本: V1.00
// 创建人员: zhangqf_cyg
// 创建时间: 10:27:22 AM/Mar 26, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 备注:用户注册的异常信息管理方案

#include "stdint.h"
#include "stddef.h"
#include "exp_api.h"
#include "exp_record.h"

static struct tagExpRecordOperate s_tExpRecordOpt = {NULL};
static struct tagExpRecordOperate s_tExpRecordDefaultOpt = {NULL};

// =============================================================================
// 函数功能：__memcpyByteByByte
//           内存拷贝
// 输入参数：
// 输出参数：
// 返回值  ：见_EN_EXP_RECORDRESULT定义
// =============================================================================
void __memcpyByteByByte(u8 *dest, u8 *src, u32 len)
{
    u32 i = 0;
    while(i < len)
    {
        *dest = *src;
        dest++;
        src++;
        i++;
    }
}

// =============================================================================
// 函数功能：Exp_Record
//           记录一帧异常信息
// 输入参数：recordpara，需要记录的异常信息
// 输出参数：
// 返回值  ：见_EN_EXP_RECORDRESULT定义
// =============================================================================
u32 Exp_Record(struct tagExpRecordPara *recordpara)
{
    u32 result;
    if(NULL != s_tExpRecordOpt.fnexprecord)
    {
        result = (u32)(s_tExpRecordOpt.fnexprecord(recordpara));
    }
    else
    {
        result = (u32)EN_EXP_DEAL_RECORD_NOMETHOD;
    }
    return result;
}
// =============================================================================
// 函数功能：Exp_RecordClear
//          清除所有的异常信息，清除异常信息存储区域
// 输入参数：无
// 输出参数：无
// 返回值  ：true 成功， false失败
// =============================================================================
bool_t Exp_RecordClear(void)
{
    if(NULL != s_tExpRecordOpt.fnexprecordclear)//有主动注册的
    {
        return s_tExpRecordOpt.fnexprecordclear();
    }
    else//默认方式处理
    {
        return false;
    }
}
// =============================================================================
// 函数功能：Exp_RecordCheckNum
//          查看一共存储了多少条异常信息
// 输入参数：无
// 输出参数：recordnum,返回的异常信息条目数
// 返回值  ：false,失败 true成功
// =============================================================================
bool_t Exp_RecordCheckNum(u32 *recordnum)
{
    if(NULL != s_tExpRecordOpt.fnexprecordchecknum)//有主动注册的
    {
        return s_tExpRecordOpt.fnexprecordchecknum(recordnum);
    }
    else//默认方式处理
    {
        *recordnum = 0;
        return false;
    }
}

// =============================================================================
// 函数功能：Exp_RecordCheckLen
//          查看指定条目的异常信息的长度
// 输入参数：assignedno,指定的异常帧条目
// 输出参数：recordlen,用于存储指定异常条目的长度
// 返回值  ：false,失败 true成功
// =============================================================================
bool_t Exp_RecordCheckLen(u32 assignedno, u32 *recordlen)
{
    bool_t result;
    if(NULL != s_tExpRecordOpt.fnexprecordchecklen)//有主动注册的
    {
        result = s_tExpRecordOpt.fnexprecordchecklen(assignedno, recordlen);
    }
    else//默认方式处理
    {
        result = false;
    }
    return result;
}
// =============================================================================
// 函数功能：Exp_RecordGet
//          从存储介质中获取指定条目的异常帧信息
// 输入参数：assignedno,指定的异常帧条目(第几条异常条目)
//          buflenlimit,系统异常帧的长度，限制长度是因为避免拷贝越界
// 输出参数：buf,用于存储获取指定条目的异常信息
//          recordpara,异常信息存储时的参数，在此是对buf的各个部分的定义
// 返回值  ：true 成功 false失败
// =============================================================================
bool_t Exp_RecordGet(u32 assignedno, u32 buflenlimit, u8 *buf, \
                     struct tagExpRecordPara  *recordpara)
{
    bool_t result;
    if(NULL != s_tExpRecordOpt.fnexprecordget)//有主动注册的
    {
        result = s_tExpRecordOpt.fnexprecordget(assignedno, buflenlimit,buf,recordpara);
    }
    else//默认方式处理
    {
        result = false;
    }
    return result;
}
// =============================================================================
// 函数功能：Exp_RegisterRecordOpt
//          注册异常信息处理方法
// 输入参数：opt,需要注册的异常信息处理方法
// 输出参数：无
// 返回值  ：false,失败  true成功，失败的话会使用BSP默认的处理方法
// 注意    ：理论上opt结构里面指定的处理方法都应该提供，否则的话会注册不成功
//           OPT的各个操作方法自己提供参数校验等
// =============================================================================
bool_t  Exp_RegisterRecordOpt(struct tagExpRecordOperate *opt)
{
    bool_t result;
    if(NULL == opt)
    {
        result = false;
    }
    else
    {
        if((NULL == opt->fnexprecord) ||\
            (NULL == opt->fnexprecordchecklen)||\
            (NULL == opt->fnexprecordchecknum)||\
            (NULL == opt->fnexprecordclear)||\
            (NULL == opt->fnexprecordget))
        {
            result = false;
        }
        else
        {
            //注册
            __memcpyByteByByte((u8 *)&s_tExpRecordOpt, \
                               (u8 *)opt, sizeof(s_tExpRecordOpt));
            //第一次的话保留备份
            if(NULL == s_tExpRecordDefaultOpt.fnexprecord)
            {
                __memcpyByteByByte((u8 *)&s_tExpRecordDefaultOpt, \
                                   (u8 *)&s_tExpRecordOpt, \
                                   sizeof(s_tExpRecordOpt));
            }
            //存储方案初始化扫描
            if(NULL != s_tExpRecordOpt.fnexprecordscan)
            {
                s_tExpRecordOpt.fnexprecordscan();
            }
            result = true;
        }
    }
    return result;
}
// =============================================================================
// 函数功能：Exp_UnRegisterRecordOpt
//           注销异常信息处理方法
// 输入参数：无
// 输出参数：无
// 返回值  ：true成功  false失败
// =============================================================================
bool_t Exp_UnRegisterRecordOpt(void)
{
    //恢复成默认的存储方案
    __memcpyByteByByte((u8 *)&s_tExpRecordOpt, \
                       (u8 *)&s_tExpRecordDefaultOpt, \
                       sizeof(s_tExpRecordOpt));
    //存储方案初始化扫描
    if(NULL != s_tExpRecordOpt.fnexprecordscan)
    {
        s_tExpRecordOpt.fnexprecordscan();
    }
    return true;
}

