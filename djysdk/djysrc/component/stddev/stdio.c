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
//所属模块：标准IO模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 标准输入输出管理
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2015-09-11
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 创建文件
//------------------------------------------------------
#include "stdint.h"
#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "ring.h"
#include "object.h"
#include "driver.h"
#include "systime.h"

void NULL_Func(void){ }
struct Object g_tStdNotInit = {0};
s32 (* PutStrDirect)(const char *buf,u32 len) = (s32 (* )(const char *,u32 ))NULL_Func;
char (* GetCharDirect)(void) = (char (* )(void))NULL_Func;
#define CN_STDIN_DEV_BUFLIMIT   64  //如果stdin是设备，定义其缓冲区尺寸
struct RingBuf *g_ptStdinDevBuf = NULL;

//StdNotInit在stdio.h中定义，这里玩了个小技巧，给stdin/out/err赋一个不会与任意
//实际的FILE指针重复的值，指针正好有这个特点。
FILE *stdin = StdNotInit;
FILE *stderr= StdNotInit;
FILE *stdout= StdNotInit;

s32 skip_atoi(const char **s)
{
    register s32 i=0;
    while (isdigit((s32)**s)) i = i*10 + *((*s)++) - '0';

    return i;
}

bool_t OpenStdin(const char *Name)
{
    stdin = fopen(Name,"r");
    if (stdin != NULL)
    {
        if (IS_DEV(stdin)) //Name是一个设备名
        {
            if (g_ptStdinDevBuf == NULL)     //设备输入缓冲区尚未初始化
            {
                g_ptStdinDevBuf = (struct RingBuf*)malloc(
                                    sizeof(struct RingBuf)+CN_STDIN_DEV_BUFLIMIT);
                Ring_Init(g_ptStdinDevBuf,(u8*)(g_ptStdinDevBuf+1),CN_STDIN_DEV_BUFLIMIT);
            }
            else
                Ring_Init(g_ptStdinDevBuf,(u8*)(g_ptStdinDevBuf+1),CN_STDIN_DEV_BUFLIMIT);
        }
        return true;
    }
    else
        return false;
}

bool_t OpenStdout(const char *Name)
{
    stdout = fopen(Name,"a");
    if (stdout != NULL)
    {
        return true;
    }
    else
        return false;
}
//todo 句柄被意外修改的安全性。
bool_t OpenStderr(const char *Name)
{
    stderr = fopen(Name,"a");
    if (stderr != NULL)
    {
        return true;
    }
    else
        return false;
}

