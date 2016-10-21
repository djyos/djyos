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
// 3. 本软件作为都江堰操作系统的组成部分，未获事前取得书面许可，不允许移植到非
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
// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: TCP服务器发送测试
// 模块版本: V1.00
// 创建人员: Administrator
// 创建时间: 9:23:45 AM/Jul 7, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
#include "stdint.h"
#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "endian.h"

#include "os.h"

#include <tcpip/comport/sys/socket.h>

#define CN_TCP_BLOCKSERVERPORT  2048
#define CN_SNDBUF_LEN           0x1000    //4KB
#define CN_SNDBUF_STACKLEN      (CN_SNDBUF_LEN*64)
u8   sBlockSendBuf[CN_SNDBUF_LEN];

static s64   apptimestart;
static s64   apptimeend;
static u32   apptimeused;
static u32   appsndtimes = 0;
ptu32_t TcpServer_BlockSend(void)
{
    int server;
    int client;
    int addrlen;
    struct sockaddr_in addr;
    int sndlen;
    s64 sndtotal;
    int sndopt;
    //INITIAL LIZE THE DATA
    printk("\n\r%s:TASK BEGIN\n\r",__FUNCTION__);

    for(sndlen = 0; sndlen <CN_SNDBUF_LEN; sndlen ++ )
    {
        sBlockSendBuf[sndlen] = (u8)(sndlen%0x100);
    }

    server = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == server)
    {
        printk("%s:socket:err!\n\r",__FUNCTION__);
        return 0;
    }
    else
    {
        printk("%s:socket:ok!\n\r",__FUNCTION__);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(CN_TCP_BLOCKSERVERPORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(-1==bind(server, &addr, sizeof(addr)))
    {
        printk("%s:bind:err!\n\r",__FUNCTION__);
        return 0;
    }
    else
    {
        printk("%s:bind:ok!\n\r",__FUNCTION__);
    }

    if(-1 == listen(server, 5))
    {
        printk("%s:listen err!\n\r",__FUNCTION__);
        return 0;
    }
    else
    {
        printk("%s:listen ok!\n\r",__FUNCTION__);
    }

    while(1)
    {
        client = accept(server, &addr, &addrlen);
        if(client != -1)
        {
            printk("%s:ACCEPT OK: CLIEN :ip = %08x  port = %04x\n\r",__FUNCTION__,\
                    ntohl(addr.sin_addr.s_addr),ntohs(addr.sin_port));
            //设置发送缓冲区
            sndopt = CN_SNDBUF_STACKLEN;
            if(0 == setsockopt(client,SOL_SOCKET ,SO_SNDBUF,&sndopt,sizeof(sndopt)))
            {
                printk("%s:setsockopt:sndbuf = %d  OK\n\r",__FUNCTION__,sndopt);

            }
            else
            {
                printk("%s:setsockopt:sndbuf = %d  failed\n\r",__FUNCTION__,sndopt);
                closesocket(client);
                continue;
            }

            while(1)
            {
//              apptimestart = DjyGetSysTime();
                sndlen = send(client,sBlockSendBuf,CN_SNDBUF_LEN,0);
                if(sndlen > 0)
                {
                    sndtotal += sndlen;
                }
                else
                {
                    printk("%s:send: len = %d  failed\n\r",__FUNCTION__,CN_SNDBUF_LEN);
//                  closesocket(client);
                    break;
                }
//              apptimeend = DjyGetSysTime();
//              apptimeused = (u32)(apptimeend - apptimestart);
//              printk("%s:apptime:No.%d used = %d us\n\r",__FUNCTION__,\
//                      appsndtimes++,apptimeused);
//              Djy_EventDelay(1000*mS);
            }
        }
        else
        {
            printk("%s:accept:bad!\n\r",__FUNCTION__);
        }
    }
    closesocket(server);
    printk("%s:TASK END\n\r",__FUNCTION__);

    return 0;
}


bool_t TcpServer_BlockSendTest(char *param)
{
    u16  evttID;
    u16  eventID;
    evttID = Djy_EvttRegist(EN_CORRELATIVE, CN_PRIO_RRS, 0, 1,\
        (ptu32_t (*)(void))TcpServer_BlockSend,NULL, 0x1000, "TcpServer_BlockSend");
    if (evttID != CN_EVTT_ID_INVALID)
    {
        eventID = Djy_EventPop(evttID, NULL, 0, 0, 0, 0);
        if(eventID == CN_EVENT_ID_INVALID)
        {
            Djy_EvttUnregist(evttID);
            printk("%s:BlockSendTask  Create Failed\n\r",__FUNCTION__);
        }
        else
        {
            printk("%s:BlockSendTask  Create SUCCESS\n\r",__FUNCTION__);
        }
    }
    return true;
}




