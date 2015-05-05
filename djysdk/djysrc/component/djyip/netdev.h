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
#ifndef __NETDEV_H
#define __NETDEV_H
#include "stdint.h"
#include "stddef.h"

#include "os.h"

#include "pkg.h"

enum _EN_LINK_INTERFACE_TYPE
{
    EN_LINK_INTERFACE_ETHERNET = 0,  //以太网卡
    EN_LINK_INTERFACE_SLIP,          //串行
    EN_LINK_INTERFACE_LOOP,          //回环
    EN_LINK_INTERFACE_RAW,           //裸链路层，直接使用IP包发送
    EN_LINK_INTERFACE_LAST,
};
enum _EN_LINK_INTERFACE_STATE        //网卡状态
{
    EN_LINK_INTERFACE_STATE_OK = 0,     //可以正常发送数据
    EN_LINK_INTERFACE_STATE_ERR,        //网卡出错了
    EN_LINK_INTERFACE_STATE_STOP,       //网卡停用，不能收发数据
    EN_LINK_INTERFACE_STATE_RESET,      //重启
};

typedef struct __tagNetDev tagNetDev;

//netdev snd module function
typedef bool_t (*fnIfSend)(tagNetDev *netdev,tagNetPkg *pkglst,u32 netdevtask);

typedef struct _tagNetDevPara   //添加网卡时提供的参数,根据para来建立对应的网卡
{
    char                           *name;    //网卡名字
    u8                             iftype;   //网卡类型
    u8                             mac[6];   //mac地址
    fnIfSend                       ifsend;   //网卡的发送函数，允许建立缓冲表进行发送
                                             //发送函数以及控制函数由网卡驱动负责资源竞争
    u32                            devfunc;  //网卡支持的功能
    u16                            pkglen;   //最大发送包数据
    u16                            linklen;  //链路层数据包长度
    ptu32_t                        private;  //每个网卡的自由标记和私有数据
}tagNetDevPara;

struct __tagNetDev
{
    struct __tagNetDev             *pre;     //前一个网卡
    struct __tagNetDev             *nxt;     //后一个网卡
    char                           *name;    //网卡名字
    u8                             iftype;   //网卡类型
    u8                             ifstate;  //网卡状态
    u8                             mac[6];   //mac地址
    fnIfSend                       ifsend;   //网卡的发送函数，一般而言，执行完代表发送完成

    u32                            devfunc;  //网卡支持的功能,如TCP UDP校验等，见NetCDef.h中定义
    u16                            pkglen;   //网络层发送的最大数据包长度
    u16                            linklen;  //链路层数据包长度
    ptu32_t                        private;  //目前未使用，接口的自己数据

};


//驱动调用函数
tagNetDev  *NetDev_AddDev(tagNetDevPara *para);
bool_t NetDev_DelDev(tagNetDev *netdev);
ptu32_t NetDev_GetDevPrivate(tagNetDev *netdev);

//协议栈调用该函数发送数据到网卡
bool_t NetDev_DevSend(tagNetDev *netdev,tagNetPkg *pkglst,u32 netdevtask);
//网卡驱动调用该函数传递数据包到协议栈
bool_t NetDev_PostPkg(tagNetDev *netdev, tagNetPkg *pkglst);

u8  *NetDev_GetDevMac(tagNetDev *netdev);       //链路层使用
u32 NetDev_GetDevFun(tagNetDev *netdev);        //协议栈使用

typedef struct __tagNetDevPkgMsg
{
    struct __tagNetDevPkgMsg *nxt;    //the nxt node
    tagNetDev                *dev;    //the function the netdev has done
    tagNetPkg                *pkglst; //the rcv pkg of the card
}tagNetDevPkgMsg;


#endif
