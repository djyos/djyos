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
#ifndef __ROUT_H
#define __ROUT_H

#include <sys/socket.h>

typedef struct NetDev  tagNetDev;

typedef struct  __Rout
{
   struct __Rout  *nxt;        //rout chain
   tagNetDev      *dev;        //this rout owner
   u32             pro;        //rout property
   u32             life;       //this rout time
   u32             func;       //inherit from the device
   enum_ipv_t      ver;        //ipv4 or v6
   union
   {
       tagHostAddrV4 ipv4;
       tagHostAddrV6 ipv6;
   }ipaddr;
}tagRout;
typedef enum
{
    EN_IPV4_ANY = 0,       //not sure yet
    EN_IPV4_HOSTTARGET,    //which means this is an host address
    EN_IPV4_INSUBNET,      //which means it is in the same sub net
    EN_IPV4_SUBBROAD,      //it is the sub net broad address
    EN_IPV4_BROAD,         //it is the limited broad address
    EN_IPV4_WAN,          //it is the wlan address
    EN_IPV4_MULTI,         //it is an multicast address
    EN_IPV4_NOTDEF,        //not define yet
}enum_ip_class;

typedef struct
{
	bool_t     enable;               //if true the filter item is enable
	bool_t     action;               //which means the action start or not(1 start while 0 stop)
	vu32       cmd;                  //which means the filter start or stop command
	vu32       actiontimes;          //which means the action total times
	vu32       actiontime;           //which means the action time length
	vu32       framcounter;          //which means in the measure time,the specified type frame number
	vu32       framlimit;            //which means the frame limit,if over then the action will start
	vu32       measuretime;          //which means the measure time length
	vu64       framtotal;            //which means the frame of the type total number
	vs64       starttime;            //which means the action start or measure start time
}tagNetDevRcvFilter;
struct NetDev
{
    struct NetDev                 *nxt;                    //dev chain
    char                           name[CN_TCPIP_NAMELEN]; //dev name
    u8                             iftype;   //dev type
    fnIfSend                       ifsend;   //dev snd function
    fnIfCtrl                       ifctrl;   //dev ctrl or stat get fucntion
    u32                            devfunc;  //dev hard function,such as tcp chksum
    u16                            mtu;      //dev mtu
    ptu32_t                        private;  //the dev driver use this to has its owner property
    u8                             mac[CN_MACADDR_LEN];   //mac address
    tagRout                       *routq;    //an net maqybe has many ip address
    //the following used to debug the net device,show some main status
    u32                            fsnd;     //frame send
    u32                            fsnderr;  //frame snd failed
    u32                            frcv;     //frame receive
    u32                            frcverr;  //frame receive err
    tagNetDevRcvFilter             rfilter[EN_NETDEV_FRAME_LAST];  //the recv filter
};

tagNetDev  *NetDevGet(const char  *name);
#define     NetDevGetMac(dev)     (dev->mac)       //get the dev mac(stack use)
#define     NetDevGetFunc(dev)    (dev->devfunc)   //get the dev func(stack use)
#define     RoutFunc(rout)        (rout->func)     //use this to get the rout func
enum_ip_class RoutIpClass(enum_ipv_t ver,ipaddr_t ip,tagRout **rout);//use this function to get the ip net
enum_ip_class RoutTargetClass(tagNetDev *dev,enum_ipv_t ver,ipaddr_t ip,tagRout **rout);
bool_t RoutUpdate(char *name,enum_ipv_t ver,void *netaddr);// use this function to update


#endif /* __ROUT_H */
