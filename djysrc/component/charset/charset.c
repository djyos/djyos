//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. As a constituent part of djyos,do not transplant it to other software
//    without specific prior written permission.

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
// 这份授权条款，在使用者符合以下三条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。
// 3. 本软件作为都江堰操作系统的组成部分，未获事前取得的书面许可，不允许移植到非
//    都江堰操作系统环境下运行。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------

//所属模块: 字符编码驱动
//作者：mjh
//版本：V1.0.0
//文件描述: 字符编码资源初始化
//其他说明:
//修订历史:
//    2. ...
//    1. 日期:2011-05-17
//       作者:mjh
//       新版本号：
//       修改说明:初始版本
//------------------------------------------------------
#include "stdint.h"
#include "rsc.h"
#include "stdio.h"

#include "charset.h"

static struct tagCharset *s_ptCurCharEncoding;    // 编码资源
//----安装字符编码-------------------------------------------------------------
//功能: 把新字符编码安装到系统资源队列中
//参数: encoding，新增的字符编码资源指针
//      name，新增字符编码名
//返回: true
//-----------------------------------------------------------------------------
bool_t  Charset_NlsInstallCharset(struct tagCharset *encoding, char* name)
{
    struct tagRscNode *rsc;
    rsc = Rsc_SearchTree(CN_CHAR_ENCODING_RSC_TREE);
    if(rsc == NULL)
        return false;       //字符资源根结点未创建

    Rsc_AddSon(rsc,&encoding->node,sizeof(struct tagCharset),RSC_CHARSET,name);

    return true;
}

//----字符编码模块初始化-------------------------------------------------------
//功能: 初始化字符编码模块
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_Charset(ptu32_t para)
{
    static struct tagRscNode encoding_root_rsc;    // 编码资源

    s_ptCurCharEncoding = NULL;
    // 添加字符编码资源根节点
    if(Rsc_AddTree(&encoding_root_rsc,
                            sizeof(struct tagRscNode),RSC_RSCNODE,
                            CN_CHAR_ENCODING_RSC_TREE))
    {
        return 1;
    }else
    {
        printf("install charset module fail\r\n");
        return 0;
    }
}

//----获取当前字符编码---------------------------------------------------------
//功能: 获取当前使用的字符编码
//返回: 当前字符编码
//-----------------------------------------------------------------------------
struct tagCharset* Charset_NlsGetCurCharset(void)
{
    return s_ptCurCharEncoding;
}

//----设定当前字符编码---------------------------------------------------------
//功能: 把新字符编码设为当前使用的字符编码,新字符编码必须事先安装到系统中，如果
//      encoding资源未安装，本函数什么都不做，返回原来的当前字符编码
//参数: encoding，指定的字符编码
//返回: NULL，设定失败
//      设定之前的字符编码
//-----------------------------------------------------------------------------
struct tagCharset* Charset_NlsSetCurCharset(
                                struct tagCharset* encoding)
{
    struct tagRscNode *rsc;
    if(encoding == NULL)
        return NULL;
    rsc = Rsc_SearchTree(CN_CHAR_ENCODING_RSC_TREE);
    if(rsc == NULL)
        return NULL;       //字符资源树未创建
    rsc = Rsc_SearchSon(rsc,encoding->node.name);
    if(rsc != NULL)
    {
        s_ptCurCharEncoding = (struct tagCharset*)rsc;
    }
    return s_ptCurCharEncoding;
}

//----搜索字符编码资源---------------------------------------------------------
//功能: 根据名称搜索字符编码资源
//参数: name，指定的字符编码名称
//返回: NULL，无此编码资源
//      要找的字符编码资源
//-----------------------------------------------------------------------------
struct tagCharset* Charset_NlsSearchCharset(const char* name)
{
    struct tagRscNode *rsc;

    rsc = Rsc_SearchTree(CN_CHAR_ENCODING_RSC_TREE);
    if(rsc == NULL)
        return NULL;       //字符资源树未创建

    return (struct tagCharset*)Rsc_SearchSon(rsc, (char *)name);
}

