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
#include "list.h"
#include "exp_decoder.h"

struct dListNode ExpDecoderListHead;
// =============================================================================
// 函数功能: 看该异常名字是否已经被注册,返回解析器结构,未被注册则返回NULL
// 输入参数: name,解析器名字
// 输出参数:
// 返回值  :解析器结构指针,没找到就返回NULL
// =============================================================================
struct ExpInfoDecoder *  __Exp_FindDecoderNoByName(const char *name)
{
    struct ExpInfoDecoder *result = NULL;
    struct ExpInfoDecoder *Decoder;
    struct dListNode *Pos;
    int cmpresult;
    dListForEach(Pos,&ExpDecoderListHead)
    {
        Decoder = dListEntry(Pos,struct ExpInfoDecoder,DecoderList);
        cmpresult = strcmp(Decoder->DecoderName,name);
        if(0 == cmpresult)
        {
            result = Decoder;
            break;
        }
    }
    return result;
}

// =============================================================================
// 函数功能:Exp_ThrowInfodecode
//          抛出异常信息解析
// 输入参数:parahead,抛出异常时参数
//          endian, 信息的大小端
// 输出参数：
// 返回值  :true,成功设置;false,设置失败，主要是因为参数错误
// 说明    :内部调用
// =============================================================================
bool_t  Exp_ThrowInfodecode(struct ExpThrowPara *parahead,u32 endian)
{
    bool_t result;
    u32 n;
    struct ExpInfoDecoder *Decoder;

    if((parahead->DecoderName == NULL) || (strlen(parahead->DecoderName) == 0))
    {
        printf("%s",parahead->ExpInfo);
        printf("\n\r");
        result = true;
    }
    else
    {
        Decoder = __Exp_FindDecoderNoByName(parahead->DecoderName);
        if(Decoder != NULL) //注册有，那么就解析吧
        {
            result = Decoder->MyDecoder(parahead,endian);
        }
        else                //DecoderName非空，但却找不到解析器
        {
            printf("异常信息解析器未找到,显示16进制信息");
            for(n = 0; n < parahead->ExpInfoLen; n++)
            {
                printf("%x ",parahead->ExpInfo[n]);
                if((n & 0xf) == 0)
                    printf("\n\r");
            }
            result = false;
        }
    }
    return result;
}
// =============================================================================
//功能：注册异常信息解析器,如果异常发生时收集的信息比较多而且复杂,可注册一个
//      解析器,用于把该信息解析成可读信息以供分析。如果异常信息本身就是一个字符串,
//      则无须注册解析器.
//      若存储的异常有名字,但却未注册,则按照16进制显示.
//参数：decoder,异常解析器函数指针
//输出：无
//返回：true,成功注册;false,注册失败，
//说明：为什么使用名字而不用函数指针, 原因如下:
//      1.异常是在设备正常运行时记录的,分析和查找问题时,往往加载一个专门的诊断
//        程序,此时用于解析异常的函数,其一般跟记录异常时的地址不一致.
//      2.异常记录有可能被copy到其他计算机上分析,解析器函数地址坑定不一致了.
// =============================================================================
bool_t Exp_RegisterThrowInfoDecoder(struct ExpInfoDecoder *Decoder)
{
    bool_t  result;
    struct ExpInfoDecoder *dcd;
    atom_low_t  atom2operate;

    if(Decoder == NULL)
    {
        printk("Exp_RegisterThrowInfoDecoder:Invalid parameter!\n\r");
        return false;
    }
    atom2operate = Int_LowAtomStart();

    dcd = __Exp_FindDecoderNoByName(Decoder->DecoderName);
    if(dcd == NULL)//名字尚未被注册
    {
        dListInsertAfter(&ExpDecoderListHead,&(Decoder->DecoderList));
        result = true;
    }
    else
    {
        result = false;
    }
    Int_LowAtomEnd(atom2operate);

    return result;
}

