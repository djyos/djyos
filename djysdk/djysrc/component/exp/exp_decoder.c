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
// 模块描述: 抛出异常信息解析器注册及其内部调用实现
// 模块版本: V1.00
// 创建人员: Administrator
// 创建时间: 3:16:13 PM/May 8, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 备注:

#include "stdint.h"
#include "stddef.h"
#include "stdio.h"
#include "int.h"
#include "string.h"

#include "exp_decoder.h"

static fnExp_ThrowinfoDecoderModule s_fnExpThrowinfoDecoderTab[CN_EXP_DECODERNUM_LIMIT] ={NULL};
//解析器名字,所有的异常，都使用异常名字来搜索decoder，
//如果觉得很多异常处理方法比较类似，可以注册一个NAME,然后在异常信息内部自己做
//异常号的区分,如所有的硬件异常可以注册为CPU_HARD，然后自己根据MAJOR和MINOR自己区分
static const char *s_pcExpThrowinfoDecoderNameTab[CN_EXP_DECODERNUM_LIMIT]= {NULL};

// =============================================================================
// 函数功能:__Exp_CheckDecoderName
//           看该异常名字是否已经被注册
// 输入参数:name,解析器名字
// 输出参数:
// 返回值  :可以被注册的异常号
// 说明    :如果该decoder的名字已经被注册，则返回无效的解析器号
// =============================================================================
u32  __Exp_CheckDecoderName(const char *name)
{
    u32 i = 0;
    u32 result = 0;
    int cmpresult;
    while(i < CN_EXP_DECODERNUM_LIMIT)
    {
        if(NULL == s_pcExpThrowinfoDecoderNameTab[i])//可用的
        {
            result = i;
        }
        else
        {
            cmpresult = strcmp(s_pcExpThrowinfoDecoderNameTab[i],name);
            if(0 == cmpresult)
            {
                break;
            }
        }
        i++;
    }
    if(i < CN_EXP_DECODERNUM_LIMIT)//有重名的
    {
        result = CN_EXP_DECODERNUM_LIMIT;
    }
    return result;
}
// =============================================================================
// 函数功能：__Exp_FindDecoderNoByName
//          用名字查找异常解析器号
// 输入参数:name,注册的异常名字
// 输出参数：
// 返回值  :对应名字的异常号，否则无效异常号
// 说明    ：
// =============================================================================
u32  __Exp_FindDecoderNoByName(char *name)
{
    u32 i = 0;
    int cmpresult;
    while(i < CN_EXP_DECODERNUM_LIMIT)
    {
        if(NULL != s_pcExpThrowinfoDecoderNameTab[i])//可用的
        {
            cmpresult = strcmp(s_pcExpThrowinfoDecoderNameTab[i],name);
            if(0 == cmpresult)
            {
                break;
            }
        }
        i++;
    }
    return i;
}
// =============================================================================
// 函数功能:Exp_Throwinfodecoder
//          抛出异常信息解析
// 输入参数:parahead,抛出异常时参数
//          endian, 信息的大小端
// 输出参数：
// 返回值  :true,成功设置;false,设置失败，主要是因为参数错误
// 说明    :内部调用
// =============================================================================
bool_t  Exp_Throwinfodecoder(struct tagExpThrowPara *parahead,u32 endian)
{
    bool_t result;
    u32 decoder_num;

    decoder_num = __Exp_FindDecoderNoByName(parahead->name);
    if(decoder_num != CN_EXP_DECODERNUM_LIMIT)//注册有，那么就解析吧
    {
        result = s_fnExpThrowinfoDecoderTab[decoder_num](parahead,endian);
    }
    else
    {
        result = false;
    }
    return result;
}
// =============================================================================
// 函数功能：Exp_RegisterThrowinfoDecoder
//          注册软件异常信息解析器
// 输入参数：decoder,异常解析器
//          name,异常解析器名字,至少保证是全局的且不会变的
// 输出参数：
// 返回值  ：true,成功注册;false,注册失败，
// 说明    ：
//          1,当对应的异常号已经被注册了的时候，会查找未被注册的异常号进行注册
//          2,无名或者已经存在对应命名的异常会导致注册失败澹(只是‘\0’也是无效的)
//          3,有注册失败的可能，因此注意检查返回结果
//          4,为了保证该函数的通用性，将会采用原子操作而非同步量
// =============================================================================
bool_t Exp_RegisterThrowinfoDecoder(fnExp_ThrowinfoDecoderModule decoder,const char *name)
{
    bool_t  result;
    u32 num_search;
    atom_low_t  atom2operate;

    if((NULL == name)||(NULL == decoder) ||('\0')== *name)
    {
        printk("Exp_RegisterThrowinfoDecoder:Invalid parameter!\n\r");
        return false;
    }
    atom2operate = Int_LowAtomStart();

    num_search = __Exp_CheckDecoderName(name);
    if(num_search != CN_EXP_DECODERNUM_LIMIT)//名字有效
    {
        s_fnExpThrowinfoDecoderTab[num_search] = decoder;
        s_pcExpThrowinfoDecoderNameTab[num_search] = name;
        result = true;
    }
    else
    {
        result = false;
    }
    Int_LowAtomEnd(atom2operate);

    return result;
}

// =============================================================================
// 函数功能:Exp_UnRegisterThrowinfoDecoder
//          注销软件异常信息解析器
// 输入参数:name,已经被注册的异常名字
// 输出参数：
// 返回值  :true,成功注销;false,注销失败，
// 说明    ：
// =============================================================================
bool_t Exp_UnRegisterThrowinfoDecoder(char *name)
{
    bool_t  result;
    u32 num_search;
    atom_low_t  atom2operate;

    if(NULL == name)
    {
        printk("Exp_UnRegisterThrowinfoDecoder:Invalid parameter!\n\r");
        result = false;
    }
    else
    {
        atom2operate = Int_LowAtomStart();

        num_search = __Exp_FindDecoderNoByName(name);
        if(num_search != CN_EXP_DECODERNUM_LIMIT)//名字有效
        {
            s_fnExpThrowinfoDecoderTab[num_search] = NULL;
            s_pcExpThrowinfoDecoderNameTab[num_search] = NULL;
            result = true;
        }
        else
        {
            result = false;
        }
        Int_LowAtomEnd(atom2operate);
    }
    return result;
}

