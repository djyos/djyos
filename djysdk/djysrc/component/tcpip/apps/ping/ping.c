//-----------------------------------------------------------------------------
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

#include <sys/socket.h>

#include "shell.h"
#include "../../icmp.h"


#define CN_PING_DEFAULT_TIMEOUT   (1*1000*mS)   //1s
#define CN_PING_DEFAULT_COUNTER   (4)
#define CN_PING_DEFAULT_SIZE      (32)
// =============================================================================
// FUNCTION:This function is used to do the ping echo
// PARA  IN:param
//         :must be in the style:host name   times
// PARA OUT:
// RETURN  :
// INSTRUCT:
// =============================================================================
bool_t Ping(char *param)
{
    u32 i;
    u32 ip;
    u32 timeused;
    u32 timestart;
    u32 timeend;
    u8  datapad[CN_PING_DEFAULT_SIZE];
    struct in_addr ipaddr;
    struct hostent *host;
    u32    times;

    int argc = 2;
    char *argv[2];
    if(NULL == param)
    {
        return false;
    }

    string2arg(&argc,argv,param);
    if((argc != 1)&&(argc != 2))
    {
        return false;
    }
    if(argc == 2)
    {
        times = strtol(argv[1],NULL,0);
    }
    else
    {
        times = CN_PING_DEFAULT_COUNTER;
    }

    if(0 == inet_aton(argv[0],&ipaddr))
    {
        //use the dns to get the ip
        host = gethostbyname(argv[0]);
        if(NULL != host)
        {
            memcpy((void *)&ipaddr,host->h_addr_list[0],sizeof(ipaddr));
        }
        else
        {
            printf("%s:Unknown host:%s\n\r",__FUNCTION__,param);
            return false;
        }
    }
    ip = ipaddr.s_addr;

    memset(datapad,0x55,CN_PING_DEFAULT_SIZE);
    for(i = 0; i <times; i++)
    {
        timestart = (u32)DjyGetSysTime();
        if(Icmp_EchoRequest(ip,datapad,CN_PING_DEFAULT_SIZE,CN_PING_DEFAULT_TIMEOUT))
        {
            timeend = (u32)DjyGetSysTime();
            timeused = (u32)(timeend - timestart);
            printf("0x%08x %s reply:Time = %d ms\n\r",i,inet_ntoa(ipaddr),timeused/1000);
            Djy_EventDelay(CN_PING_DEFAULT_TIMEOUT);
        }
        else
        {
            printf("0x%08x %s reply:Timeout\n\r",i,inet_ntoa(ipaddr));
        }
    }
    return true;
}

struct ShellCmdTab  gServicePing[] =
{
    {
        "ping",
        Ping,
        "usage:ping hostname [times]",
        "usage:ping hostname [times]",
    }
};

#define CN_PINGDEBUG_NUM  ((sizeof(gServicePing))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc gServicePingCmdRsc[CN_PINGDEBUG_NUM];

//THIS IS PING MODULE FUNCTION
bool_t ServicePingInit(ptu32_t para)
{
    bool_t result;

    result = Sh_InstallCmd(gServicePing,gServicePingCmdRsc,CN_PINGDEBUG_NUM);

    return result;
}

