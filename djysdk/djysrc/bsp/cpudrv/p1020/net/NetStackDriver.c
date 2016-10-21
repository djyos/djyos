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
// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: NetStackDriver.c
// 模块版本: V1.00
// 创建人员: Administrator
// 创建时间: 10:12:46 AM/Aug 18, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================

#include "stdint.h"
#include "string.h"

#include "os.h"

#include "cpu_peri_tsec.h"
#include <tcpip/netdev.h>

#define CN_HARD_BOARD            0x02
u8  sgNetHardMac[6] = {0x00,0x01, 0x02, 0x03, 0x04, CN_HARD_BOARD};
#define CN_PKG_LEN  1536
static tagNetDev *pgChkNetDev = NULL;

static u8  sgSndBuf[CN_PKG_LEN];
// =============================================================================
// 函数功能：NetHard_Send
//          网卡发送数据包
// 输入参数：netdev,使用的网卡
//          pkg,待发送的数据包
//          netdevtask,需要网卡完成的工作
// 输出参数：
// 返回值  ：true发送成功  false发送失败。
// 说明    ：采用拷贝的方式发送，后续考虑使用链表发送
// =============================================================================
bool_t NetHard_Send(tagNetDev *netdev,tagNetPkg *pkg,u32 netdevtask)
{
    bool_t  result;
    tagNetPkg *tmp;
    u8 *src;
    u8 *dst;
    u32 sndlen;
    
    result = false;
    if((pgChkNetDev == netdev)&&(NULL != pkg))
    {
        sndlen = 0;
        tmp = pkg;
        //拷贝完毕之后记得释放
        while(NULL != tmp)
        {
            src = (u8 *)(tmp->buf + tmp->offset);
            dst = (u8 *)(sgSndBuf + sndlen);
            memcpy(dst, src, tmp->datalen);
            sndlen += tmp->datalen;
            tmp = tmp->partnext;
        }
        if(sndlen < 60)//小于60的包，记得填充
        {
            dst = (u8 *)(sgSndBuf + sndlen);
            memset(dst,0 ,60-sndlen);
            sndlen = 60;
        }
        Net_SendPacket(1,sgSndBuf,sndlen);
        result = true;
    }
    return result;
}

// =============================================================================
// 函数功能：NetHard_RcvIntIsr
//          网卡接收数据时的中断服务函数
// 输入参数：无
// 输出参数：
// 返回值  ：
// 说明    ：目前采用轮询的方式模拟读中断
// =============================================================================
void NetHard_RcvIntIsr(void)
{
    bool_t      newpkg;
    s32         rcvlen;
    u8          *rcvbuf;
    tagNetPkg *pkg;

    newpkg = true;
    pkg =NULL;
    while(1)
    {
        if(newpkg)
        {
            pkg =PkgMalloc(CN_PKG_LEN, CN_PKLGLST_END);
        }

        if(NULL != pkg)
        {
            rcvbuf = (u8 *)(pkg->buf + pkg->offset);

            rcvlen = Net_RecvPacket(1,rcvbuf);//轮询，里面添加了延时
            if(0 != rcvlen)
            {
                pkg->datalen = rcvlen;
                pkg->partnext = NULL;
                newpkg = true;
                NetDev_PostPkg(pgChkNetDev,pkg);
                PkgTryFreePart(pkg);
            }
            else
            {
                newpkg = false;
            }
        }
        Djy_EventDelay(1*mS);
    }
    return;
}


// =============================================================================
// 函数功能：NetHard_AddNetDev
//          向协议栈添加一个网卡设备
// 输入参数：无
// 输出参数：无
// 返回值  ：true成功  false失败。
// 说明    ：1,添加设备
//        2,添加路由
// =============================================================================
bool_t NetHard_AddNetDev(void)
{
    u16   evtt_id;
    bool_t  result;
    tagNetDevPara  devpara;

//初始化网卡    
    extern bool_t module_tsec_init(s32 num,u8 last_mac);//eTSECx初始化
    module_tsec_init(1,10);
    
    devpara.ifsend = NetHard_Send;
    devpara.iftype = EN_LINK_INTERFACE_ETHERNET;
    memcpy(devpara.mac, sgNetHardMac,6); 
    devpara.name = "TestDriver";
    devpara.private = 0;
    devpara.linklen = 14;
    devpara.pkglen = 1500;
    devpara.devfunc = 0;
    
    result = false;
    pgChkNetDev = NetDev_InstallDev(&devpara);
    if(pgChkNetDev != NULL)
    {
        result = true;
    }
    //添加接收线程
    evtt_id = Djy_EvttRegist(EN_CORRELATIVE, CN_PRIO_RRS-2, 0, 1,
            (ptu32_t (*)(void))NetHard_RcvIntIsr,NULL, 0x100, "NetHard_RcvIsr");
    if (evtt_id != CN_EVTT_ID_INVALID)
    {
        evtt_id = Djy_EventPop(evtt_id, NULL, 0,0, 0, 0);
    }
    return result;
}


