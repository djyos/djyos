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
// Copyright (C) 2012-2020 深圳鹏瑞软件有限公司 All Rights Reserved
// 模块描述: NetDevHal.c:
// 模块版本: V1.00
// 创建人员: Zqf
// 创建时间: 9:45:16 AM/Sep 9, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "os.h"

#include "link.h"
#include "netdev.h"

static tagNetDev  *pgNetDevLstH =NULL;

// =============================================================================
// 函数功能：NetDev_AddDev
//           添加一个网卡设备
// 输入参数：netdevpara,网卡设备的特性参数
// 输出参数：errno,出错原因
// 返回值  ：网卡句柄
// =============================================================================
tagNetDev  *NetDev_AddDev(tagNetDevPara *para)
{

    tagNetDev  *result;
    atom_low_t atom;

    result = M_Malloc(sizeof(tagNetDev),CN_TIMEOUT_FOREVER);
    if(NULL != result)
    {
        result->ifsend = para->ifsend;
        result->private = para->private;
        memcpy(result->mac, para->mac, 6);
        result->iftype = para->iftype;
        result->linklen= para->linklen;
        result->pkglen= para->pkglen;
        result->devfunc= para->devfunc;
        result->ifstate = EN_LINK_INTERFACE_STATE_OK;

        atom = Int_LowAtomStart();
        if(NULL == pgNetDevLstH)
        {
            result->nxt = NULL;
            result->pre = NULL;
            pgNetDevLstH = result;
        }
        else
        {
            result->nxt = pgNetDevLstH;
            pgNetDevLstH->pre = result;
            result->pre = NULL;
            pgNetDevLstH = result;
        }
        Int_LowAtomEnd(atom);
    }
    return result;
}

// =============================================================================
// 函数功能：NetDev_DelDev 删除一个网络设备
// 输入参数：netdev,待删除的网卡
// 输出参数：
// 返回值  ：true成功  false失败
// 说明    ：
// =============================================================================
bool_t NetDev_DelDev(tagNetDev *netdev) //暂时不提供
{
    bool_t   result = false;

    return result;
}



// =============================================================================
// 函数功能：NetDev_GetDevPrivate
//          清除网卡接收到数据信息
// 输入参数：netdev,注册成功的网卡
// 输出参数：
// 返回值  ：if para err or something unnormal then return zero,otherwise return the
//          private property that applied when registered
// 说明    ：
// =============================================================================
ptu32_t NetDev_GetDevPrivate(tagNetDev *netdev)
{
    ptu32_t result = 0;

    if(NULL != netdev)
    {
        result= netdev->private;
    }

    return result;
}

// =============================================================================
// 函数功能：NetDev_DevSend
//          网络设备发送PKG包
// 输入参数：netdev,使用的网卡
//        pkglst,待发送的数据包链表
//        netdevtask,需要网卡完成的工作
// 输出参数：
// 返回值  ：true发送成功  false发送失败。
// 说明    ：当网卡不存在或者即将被注销或者处于停用状态时将发送失败
// =============================================================================
bool_t NetDev_DevSend(tagNetDev *netdev,tagNetPkg *pkglst,u32 netdevtask)
{
    bool_t result = false;

    if((NULL != netdev)&&(NULL != netdev->ifsend))
    {
        result = netdev->ifsend(netdev,pkglst,netdevtask);
    }

    return result;
}

// =============================================================================
// 函数功能：NetDev_PostPkg,网卡驱动调用该接口发送数据包到协议栈
// 输入参数：netdev,使用的网卡
//        pkglst,待发送的数据包链表
// 输出参数：
// 返回值  ：true发送成功  false发送失败。
// 说明    ：当网卡不存在或者即将被注销或者处于停用状态时将发送失败
// =============================================================================
bool_t NetDev_PostPkg(tagNetDev *netdev,tagNetPkg *pkglst)
{
    return Link_Process(netdev,pkglst);
}
// =============================================================================
// 函数功能：NetDev_GetDevMac
//          获取网络设备的MAC地址
// 输入参数：netdev,待获取的网卡
// 输出参数：
// 返回值  ：网卡的MAC地址，NULL表示为获取不到
// 说明    ：对于网卡而言，大部分是存在唯一的MAC;获取不到一半是因为参数错误
// =============================================================================
u8 *NetDev_GetDevMac(tagNetDev *netdev)
{
    u8 *result = NULL;

    if(NULL != netdev)
    {
        result = netdev->mac;
    }

    return result;
}

// =============================================================================
// 函数功能：NetDev_GetDevProperty
//          获取网络设备的功能属性
// 输入参数：netdev,待获取的网卡
// 输出参数：
// 返回值  ：网卡的功能属性
// 说明    ：
// =============================================================================
u32 NetDev_GetDevFun(tagNetDev *netdev)
{
    u32 devfun = 0;

    if(NULL != netdev)
    {
        devfun = netdev->devfunc;
    }

    return devfun;
}

// =============================================================================
// 函数功能：NetDev_Init
//          网络设备模块初始化
// 输入参数：无
// 输出参数：无
// 返回值  ：true初始化成功 ，false 初始化失败
// 说明    ：失败的可能性太小，唯一的可能是未知原因
// =============================================================================
bool_t NetDev_Init(void)
{
    return true;
}
