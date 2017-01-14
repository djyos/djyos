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
#include "link.h"
#include "rout.h"
#include "arp.h"
#include "ip.h"
#include "tcpipconfig.h"

#pragma pack(1)
typedef struct EthernetHdr
{
    u8 macdst[CN_MACADDR_LEN];
    u8 macsrc[CN_MACADDR_LEN];
    u16 type;
}tagEthernetHdr;
#pragma pack()
#define CN_ETHERNET_HEADLEN  sizeof(tagEthernetHdr)

//the internet proto deal function we need implement
extern bool_t IpPushPkg(tagNetDev *netdev, tagNetPkg *pkg);
extern bool_t ArpPushPkg(tagNetDev *netdev, tagNetPkg *pkg);

typedef struct __LinkRcvHookItem
{
	struct __LinkRcvHookItem *nxt;  //used to do the queue
	const char *      name;         //may be you want to specify an name here
	u16               proto;        //the listen  protocol you have set
	fnLinkProtoDealer hook;         //the listen hook you have set
	ptu32_t           devhandle;    //the listen device you have set
	s64               framenum;     //how many frames has listen
}tagLinkRcvHookItem;

typedef struct
{
	u32 itemtotal;
	s64 framenum;
	s64 frameunknown;
	tagLinkRcvHookItem *list;
	struct MutexLCB    *mutex;
}tagLinkRcvHookModule;
static tagLinkRcvHookModule  gLinkRcvHookModule;
//if you specified the special device, only the same device and the same proto frame
//will passed into the hook;if the devhandler is NULL,then any proto with the same proto
//will passed into the hook.
//you could use this function to register some link protocol to do the functions
bool_t LinkRegisterRcvHook(fnLinkProtoDealer hook, ptu32_t devhandle,u16 proto,const char *hookname)
{
	bool_t result = false;
	tagLinkRcvHookItem *item;

	if(NULL == hook)
	{
		return result;
	}
	item = malloc(sizeof(tagLinkRcvHookItem));
	if(NULL == item)
	{
		return result;
	}
	memset((void *)item,0,sizeof(tagLinkRcvHookItem));
	item->devhandle = devhandle;
	item->hook = hook;
	item->proto = proto;
	item->name = hookname;

	if(Lock_MutexPend(gLinkRcvHookModule.mutex,CN_TIMEOUT_FOREVER))
	{
		item->nxt = gLinkRcvHookModule.list;
		gLinkRcvHookModule.list = item;
		TCPIP_DEBUG_INC(gLinkRcvHookModule.itemtotal);
		result = true;

		Lock_MutexPost(gLinkRcvHookModule.mutex);
	}
	else
	{
		free(item);
	}
	return result;
}

//you could use this function to unregister the hook you got last time
bool_t LinkUnRegisterRcvHook(fnLinkProtoDealer hook, ptu32_t devhandle,u16 proto,const char *hookname)
{
	bool_t result = false;
	tagLinkRcvHookItem *item;
	tagLinkRcvHookItem *nxt;

	if(Lock_MutexPend(gLinkRcvHookModule.mutex,CN_TIMEOUT_FOREVER))
	{
		item = gLinkRcvHookModule.list;
		if((item->hook == hook)&&\
			(item->devhandle == devhandle)&&\
			(item->proto == proto)&&\
			(item->name == hookname))
		{
			//the first match
			gLinkRcvHookModule.list = item->nxt;
			free(item);
			TCPIP_DEBUG_DEC(gLinkRcvHookModule.itemtotal);
			result= true;
		}
		else
		{
			while(NULL != item->nxt)
			{
				nxt = item->nxt;
				if((nxt->hook == hook)&&\
					(nxt->devhandle == devhandle)&&\
					(nxt->proto == proto)&&\
					(nxt->name == hookname))
				{

					item->nxt = nxt->nxt;
					free(nxt);
					TCPIP_DEBUG_DEC(gLinkRcvHookModule.itemtotal);
					result= true;
					break;
				}
				else
				{
					item = nxt;
				}
			}
		}
		Lock_MutexPost(gLinkRcvHookModule.mutex);
	}

	return result;
}
//use this function to do hook protocol deal
bool_t __LinkHookPost(tagNetDev  *dev,tagNetPkg *pkg,u16 proto)
{
	bool_t  result = false;
	fnLinkProtoDealer hook = NULL;
	tagLinkRcvHookItem *item;

	//find the hook first
	if(Lock_MutexPend(gLinkRcvHookModule.mutex,CN_TIMEOUT_FOREVER))
	{
		item = gLinkRcvHookModule.list;
		while(NULL != item)
		{
			if((proto == item->proto)&&\
			  ((dev == (tagNetDev *)item->devhandle)||\
			   (NULL == (tagNetDev *)item->devhandle)))
			{
				hook = item->hook;
				TCPIP_DEBUG_INC(item->framenum);
				break;
			}
			item = item->nxt;
		}
		Lock_MutexPost(gLinkRcvHookModule.mutex);
	}

	if(NULL != hook)
	{
		result = hook((ptu32_t)dev,proto,pkg);
		TCPIP_DEBUG_INC(gLinkRcvHookModule.framenum);
	}
	else
	{
		TCPIP_DEBUG_INC(gLinkRcvHookModule.frameunknown);
	}
	return result;
}

//use this function to send the raw package for the device
//the handle is got by the install device or by name got
bool_t LinkSendRaw(ptu32_t devhandle,tagNetPkg *pkg,u32 framlen,u32 devtask)
{
    bool_t result = false;
    tagNetDev *dev;

    dev = (tagNetDev *)devhandle;
    if((NULL != dev) &&(NULL != dev->ifsend)&&(NULL != pkg))
    {
    	result = dev->ifsend((ptu32_t)dev,pkg,framlen,devtask);
        TCPIP_DEBUG_INC(dev->fsnd);
        if(result == false)
        {
            TCPIP_DEBUG_INC(dev->fsnderr);
        }
    }
    return result;
}
//-----------------------------------------------------------------------------
//功能:use this function to send data in buffer to the specified net device
//参数:devhandle, the net device;
//     buf, the data buffer
//     len, the data len
//返回:true success while false failed
//备注:
//-----------------------------------------------------------------------------
bool_t LinkSendBufRaw(ptu32_t devhandle,u8 *buf, u32 len)
{
    bool_t result = false;
    tagNetDev *dev;
    tagNetPkg *pkg;

    pkg = PkgMalloc(len,CN_PKLGLST_END);
    if(NULL != pkg)
    {
        dev = (tagNetDev *)devhandle;
        if((NULL != dev) &&(NULL != dev->ifsend)&&(NULL != pkg))
        {
        	memcpy(pkg->buf+pkg->offset,buf,len);
        	pkg->datalen = len;
        	result = dev->ifsend((ptu32_t)dev,pkg,len,CN_IPDEV_NONE);
            TCPIP_DEBUG_INC(dev->fsnd);
            if(result == false)
            {
                TCPIP_DEBUG_INC(dev->fsnderr);
            }
        }
        PkgTryFreeLst(pkg);
    }
    return result;
}
//do the hook debug module
bool_t LinkHookShell(char *param)
{
	int i = 0;
	tagLinkRcvHookItem *item;

	printf("LinkHookModule\n\r");
	if(Lock_MutexPend(gLinkRcvHookModule.mutex,CN_TIMEOUT_FOREVER))
	{

		printf("Item:Total:%d  FrameRcvTotal:%lld FrameRcvUnknown:%lld\n\r",\
				gLinkRcvHookModule.itemtotal,gLinkRcvHookModule.framenum,gLinkRcvHookModule.frameunknown);

		printf("%-10s%-10s%-10s%-10s%-10s%-10s\n\r",\
				"Number","HookName","Protocol","Function","DevHandle","FrameNum");

		item = gLinkRcvHookModule.list;
		while(NULL != item)
		{
			printf("No.%-4d   %-8s  %-8x  %-8x  %-8x  %llx\n\r",\
					i++,item->name?item->name:"NULL",\
					item->proto,item->hook,\
					item->devhandle,item->framenum);
			item = item->nxt;
		}
		Lock_MutexPost(gLinkRcvHookModule.mutex);
	}
	return true;
}


//use this function to make an ethernet package and send it to the netdev
bool_t __EthernetSend(tagNetDev *dev,tagNetPkg *pkg,u32 framlen,u32 devtask,u16 proto,\
                     enum_ipv_t ver,ipaddr_t ip)
{
    bool_t            result;
    tagEthernetHdr   *hdr;
    tagNetPkg        *ethpkg;

    result = false;
    ethpkg = PkgMalloc(CN_ETHERNET_HEADLEN,0);
    if(NULL != ethpkg)
    {
        hdr = (tagEthernetHdr  *)(ethpkg->buf + ethpkg->offset);
        memcpy(hdr->macsrc, NetDevGetMac(dev), CN_MACADDR_LEN);
        hdr->type = htons(proto);
        if(ver == EN_IPV_4)
        {
            result = ArpFindMac(ip,hdr->macdst);  //the ipv4 support
        }
        else
        {
            //not support the ipv6 yet
        }
        if(result)
        {
            ethpkg->datalen = CN_ETHERNET_HEADLEN;
            ethpkg->partnext = pkg;
            framlen += CN_ETHERNET_HEADLEN;
            result = dev->ifsend((ptu32_t)dev,ethpkg,framlen, devtask);
        }
        else
        {
             //can't find the mac
        }
        PkgTryFreePart(ethpkg);
    }
    return result;
}


//use this function to do the ethernetII frames
bool_t __LinkEthernetIIPost( tagNetDev  *dev,tagNetPkg *pkg)
{
    bool_t          result=false;
    tagEthernetHdr *hdr;
    u16             proto;
    //we analyze the ethernet header first, which type it has
    hdr = (tagEthernetHdr *)(pkg->buf + pkg->offset);
    memcpy(&proto,&hdr->type,sizeof(proto));
    proto = ntohs(proto);
    switch(proto)
    {
     //uptil now, we support the ipv4
        case EN_NET_PROTO_IP:
            pkg->offset += CN_ETHERNET_HEADLEN;
            pkg->datalen -= CN_ETHERNET_HEADLEN;
            result = IpPushPkg(dev,pkg);
            break;
        case EN_NET_PROTO_ARP:
            pkg->offset += CN_ETHERNET_HEADLEN;
            pkg->datalen -= CN_ETHERNET_HEADLEN;
            result = ArpPushPkg(dev,pkg);
            break;
        default:
        	result = __LinkHookPost(dev,pkg,proto);
            break;
    }
    return result;
}
//use this function to do the SNAP frames
bool_t __LinkSnapPost( tagNetDev  *dev,tagNetPkg *pkg)
{
	return false;
}
//use this function to do the wireless frames
bool_t __Link80211Post( tagNetDev  *dev,tagNetPkg *pkg)
{
	return false;
}
// =============================================================================
// FUNCTION   :the driver should call this function to post the package here
// PARAMS IN  :
// PARAMS OUT :
// RETURN     :true success while false failed
// DESCRIPTION:
// =============================================================================
bool_t LinkPost(ptu32_t devhandle,tagNetPkg *pkg)
{
    bool_t          result=false;
    tagNetDev      *dev;

    dev = (tagNetDev *)devhandle;
    if((NULL != dev)&&(NULL != pkg)&&(pkg->datalen > 0))
    {
        switch(dev->iftype)
        {
            case EN_LINK_ETHERNET:
            	result = __LinkEthernetIIPost(dev,pkg);
                break;
            case EN_LINK_RAW:      //we could send the package to the net device directly
                //this only support the ip frame,for the raw type, no mac need
                result = IpPushPkg(dev,pkg);
                break;
            default:
                break;
        }
    }
    TCPIP_DEBUG_INC(dev->frcv);
    if(result == false)
    {
        TCPIP_DEBUG_INC(dev->frcverr);
    }
    return result;
}

// =============================================================================
// FUNCTION   :you could use this function to send the package
// PARAMS IN  :para, the net device function parameters as tagNetDevPara defined
// PARAMS OUT :NULL
// RETURN     :the dev handle
// DESCRIPTION:NO DUPLICATION NAME PERMITTED
// =============================================================================
bool_t LinkSend(tagRout *rout,tagNetPkg *pkg,u32 framlen,u32 devtask,u16 proto,\
                enum_ipv_t ver,ipaddr_t ip)
{
    bool_t result = false;
    tagNetDev *dev;

    if((NULL != rout) &&(NULL != rout->dev))
    {
        dev = rout->dev;
        switch(dev->iftype)
        {
            case EN_LINK_ETHERNET: // we need make a ethernet header
                //first we need to know the
                result = __EthernetSend(dev,pkg,framlen,devtask,proto,ver,ip);
                break;
            case EN_LINK_RAW:      // we could send the package to the net device directly
            	result = dev->ifsend((ptu32_t)dev,pkg,framlen,devtask);
                break;
            default:
                break;
        }

        TCPIP_DEBUG_INC(dev->fsnd);
        if(result == false)
        {
            TCPIP_DEBUG_INC(dev->fsnderr);
        }
    }
    return result;
}


// =============================================================================
// FUNCTION   :this is the link module initialize function
// PARAMS IN  :
// PARAMS OUT :
// RETURN     :
// DESCRIPTION:uptils now, we do nothing here
// =============================================================================
bool_t LinkInit(void)
{
	bool_t result = false;
	//the gLinkHook module must be initialized
	memset((void *)&gLinkRcvHookModule,0,sizeof(gLinkRcvHookModule));

	gLinkRcvHookModule.mutex = Lock_MutexCreate(NULL);
	if(NULL == gLinkRcvHookModule.mutex)
	{
		goto EXIT_MUTEX;
	}
	gLinkRcvHookModule.itemtotal = 0;
	result = true;
	return result;

EXIT_MUTEX:
    return result;
}


