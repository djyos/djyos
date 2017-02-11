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
//author:zhangqf
//date  :下午6:28:01/2017年1月3日
//usage :
//------------------------------------------------------

//standard includes
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <os.h>
//add your own specified header here
//add your own specified header here
#include "linkhal.h"
#include "rout.h"
//data flow in the loop
//ipout->linkout->linkoutloop->loopout->loopin->LinkIn->linkInLoop->link2Ip

#define CN_LOOP_MTU      4*1024        //4KB
#define CN_LOOP_DEVICE   "loop"
static  ptu32_t          LoopHandle = NULL;
//-----------------------------------------------------------------------------
//功能:the loop device in function
//参数:
//返回:
//备注:
//作者:zhangqf@上午9:28:15/2016年12月29日
//-----------------------------------------------------------------------------
static bool_t __LoopIn(tagNetPkg *pkg)
{
	return LinkPost(LoopHandle,pkg);
}
//-----------------------------------------------------------------------------
//功能:the linkoutloop call this function to pass the package to the stack
//参数:
//返回:
//备注:
//作者:zhangqf@上午9:24:20/2016年12月29日
//-----------------------------------------------------------------------------
static bool_t __LoopOut(ptu32_t dev,tagNetPkg *pkg,u32 framlen,u32 netdevtask)
{
    bool_t  result;
    tagNetPkg *tmp;
    tagNetPkg *sndpkg;
    u8 *src;
    u8 *dst;

    result = false;
    if(NULL != pkg)
    {
        sndpkg = PkgMalloc(framlen,0);
        if(NULL != sndpkg)
        {
            tmp = pkg;
            while(NULL != tmp)
            {
                src = (u8 *)(tmp->buf + tmp->offset);
                dst = (u8 *)(sndpkg->buf + sndpkg->offset + sndpkg->datalen);
                memcpy(dst, src, tmp->datalen);
                sndpkg->datalen += tmp->datalen;
                if(tmp->pkgflag & CN_PKLGLST_END)
                {
                    tmp = NULL;
                }
                else
                {
                    tmp = tmp->partnext;
                }
            }
            result = __LoopIn(sndpkg);
            PkgTryFreePart(sndpkg);
        }
    }
    return result;
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//作者:zhangqf@上午9:26:04/2016年12月29日
//-----------------------------------------------------------------------------
bool_t LoopInit(void)
{
    tagNetDevPara   devpara;
    tagHostAddrV4   devaddr;
    //then we will register a loop device to the stack
    memset((void *)&devpara,0,sizeof(devpara));
    devpara.ifsend = __LoopOut;
    devpara.iftype = EN_LINK_RAW;
    devpara.name = CN_LOOP_DEVICE;
    devpara.private = 0;
    devpara.mtu = CN_LOOP_MTU;
    devpara.devfunc = CN_IPDEV_ALL;
    memcpy(devpara.mac,CN_MAC_BROAD,CN_MACADDR_LEN);
    LoopHandle = NetDevInstall(&devpara);
    if(NULL == LoopHandle)
    {
    	goto EXIT_LOOPDEV;
    }

    //then we will create a loop rout to the stack
    devaddr.ip      = inet_addr("127.0.0.1");
    devaddr.gatway  = inet_addr("127.0.0.1");
    devaddr.submask = inet_addr("255.255.255.0");
    devaddr.dns     = inet_addr("127.0.0.1");
    devaddr.broad   = inet_addr("127.0.0.255");
    //loop could receive the broad mail,but never send the broad mail
    if(false == RoutCreate(CN_LOOP_DEVICE,EN_IPV_4,(void *)&devaddr,CN_ROUT_NONE))
    {
    	printf("%s:NetDevInstall Err\n\r",__FUNCTION__);
    	goto EXIT_LOOPROUT;
    }
    //here means we are successful
    return true;

EXIT_LOOPROUT:
	NetDevUninstall(CN_LOOP_DEVICE);
EXIT_LOOPDEV:
    return false;
}
