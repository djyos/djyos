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

#include "arp.h"
#include "rout.h"
#include "linkhal.h"

//first we should implement the device layer
static tagNetDev        *pNetDevQ        = NULL;
static struct MutexLCB  *pRoutMutex      = NULL;
static tagRout          *pRoutDefaultV4  = NULL;   //this is the default  v4 rout
static tagRout          *pRoutLoop       = NULL;   //this is the loop rout
//this function used to get the specified dev
tagNetDev *__NetDevGet(const char *name)
{
    tagNetDev *result = NULL;
    tagNetDev *tmp = pNetDevQ;

    if(NULL == name)
    {
        result = pNetDevQ;
    }
    else
    {
        while(NULL != tmp)
        {
            if(0 == strcmp(tmp->name,name))
            {
                result = tmp;
                tmp = NULL;//end the search
            }
            else
            {
                tmp = tmp->nxt;
            }
        }
    }

    return result;
}
// =============================================================================
// FUNCTION   :find the name specified net device
// PARAMS IN  :name, the net device name,which specified by installation
// PARAMS OUT :NULL
// RETURN     :the name specified net device while NULL failed
// DESCRIPTION:if name is NULL then return the dev head
// =============================================================================
tagNetDev  *NetDevGet(const char  *name)
{
    tagNetDev *result = NULL;

    if(Lock_MutexPend(pRoutMutex, CN_TIMEOUT_FOREVER))
    {
        result = __NetDevGet(name);
        Lock_MutexPost(pRoutMutex);
    }
    return result;
}

ptu32_t NetDevHandle(const char *name)
{
	return (ptu32_t)NetDevGet(name);
}

// =============================================================================
// FUNCTION   :install an net device to the stack
// PARAMS IN  :para, the net device function paras as tagNetDevPara defined
// PARAMS OUT :NULL
// RETURN     :the dev handle
// DESCRIPTION:NO DUPLICATION NAME PERMITTED
// =============================================================================
ptu32_t NetDevInstall(tagNetDevPara *para)
{
    tagNetDev  *dev = NULL;
    tagLinkOps *ops;

    if(NULL == para)
    {
        return (ptu32_t)dev;
    }
    ops = LinkFindOps(para->iftype);

    if((NULL != ops)&&Lock_MutexPend(pRoutMutex, CN_TIMEOUT_FOREVER))
    {
        dev = __NetDevGet(para->name);
        if(NULL == dev)
        {
            dev = malloc(sizeof(tagNetDev));
            if(NULL != dev)
            {
                //fill the dev
                memset((void *)dev, 0, sizeof(tagNetDev));
                strncpy(dev->name,para->name,CN_TCPIP_NAMELEN);
                memcpy(dev->mac, para->mac, CN_MACADDR_LEN);
                dev->ifsend  = para->ifsend;
                dev->ifctrl  = para->ifctrl;
                dev->private = para->private;
                dev->iftype = para->iftype;
                dev->mtu= para->mtu;
                dev->devfunc= para->devfunc;
                //initialize the filter
                dev->rfilter[EN_NETDEV_FRAME_NOPKG].cmd = EN_NETDEV_SETNOPKG;
                dev->rfilter[EN_NETDEV_FRAME_BROAD].cmd = EN_NETDEV_SETBORAD;
                dev->rfilter[EN_NETDEV_FRAME_POINT].cmd = EN_NETDEV_SETPOINT;
                dev->rfilter[EN_NETDEV_FRAME_MULTI].cmd = EN_NETDEV_SETMULTI;
                //fill the ops
                dev->linkops = ops;
                //send a event to the link
                if(NULL != ops->linkevent)
                {
                	ops->linkevent(dev,EN_LINKEVENT_DEVADD);
                }
                //add it to the dev chain
                dev->nxt = pNetDevQ;
                pNetDevQ = dev;
            }
            else
            {
                //no more mem for the dev, so failed
                printf("%s:failed--no mem\r\n",__FUNCTION__);
            }
        }
        else
        {
             //there has been a dev has the same name, so failed
            printf("%s:failed--corrupt name\r\n",__FUNCTION__);
        }
        Lock_MutexPost(pRoutMutex);
    }
    return (ptu32_t)dev;
}
// =============================================================================
// FUNCTION   :remove the name specified net device
// PARAMS IN  :name, the net device name,which specified by installation
// PARAMS OUT :NULL
// RETURN     :true success while false failed
// DESCRIPTION:
// =============================================================================
bool_t  NetDevUninstall(const char *name)
{
    tagNetDev  *dev = NULL;
    tagNetDev  *tmp;
    tagNetDev  *bak;
    tagRout    *rout;
    bool_t      result = false;

    if(Lock_MutexPend(pRoutMutex, CN_TIMEOUT_FOREVER))
    {
        tmp = pNetDevQ;
        bak = tmp;

        while(NULL != tmp)
        {
            if(0 == strcmp(tmp->name,name))
            {
                //bingo,we got it now
                //remove it from the dev chain
                if(tmp == pNetDevQ)
                {
                    //the head one
                    pNetDevQ = tmp->nxt;
                }
                else
                {
                    bak->nxt = tmp->nxt;
                }
                //send a event to the link
                if(NULL != dev->linkops->linkevent)
                {
                	dev->linkops->linkevent(dev,EN_LINKEVENT_DEVDEL);
                }
                //free all the rout bind to the dev
                rout = dev->routq;
                while(NULL != rout)
                {
                    if(rout == pRoutDefaultV4)
                    {
                        pRoutDefaultV4 = NULL;
                    }
                    if(rout == pRoutLoop)
                    {
                    	pRoutLoop = NULL;
                    }

                    dev->routq = rout->nxt;
                    free((void *)rout);
                    rout = dev->routq;
                }
                //free the device memory now
                free((void *)tmp);
                result = true;
            }
            else
            {
                bak = tmp;
                tmp = tmp->nxt;
            }
        }
        Lock_MutexPost(pRoutMutex);
    }
    return result;
}
// =============================================================================
// FUNCTION   :ctrl the specified netdev
// PARAMS IN  :name, the net device name,which specified by installation
//             cmd, the ctrl cmd. which defined by tagNetDevCmd
//             para,used by the cmd, and its type defined by cmd
// PARAMS OUT :NULL
// RETURN     :true success while false failed
// DESCRIPTION:
// =============================================================================
bool_t  NetDevCtrl(const char *name,enNetDevCmd cmd, ptu32_t para)
{
    bool_t      result = false;
    tagNetDev  *dev;

    dev = NetDevGet((const char *)name);
    if((NULL !=dev)&&(NULL != dev->ifctrl))
    {
        result = dev->ifctrl((ptu32_t)dev,cmd,para);
        if(result &&(cmd == EN_NETDEV_SETMAC))
        {
            memcpy(dev->mac,(u8 *)para,CN_MACADDR_LEN);
            //we also need to send arp  response message to update
            ArpInform(dev->name);
        }
    }
    else
    {
    	printf("%s:no dev matched or no ctrl function\n\r",__FUNCTION__);
    }
    return result;
}
//if you got the handle when install the net device, please use the handle
//it maybe more fast
bool_t  NetDevCtrlByHandle(ptu32_t handle,enNetDevCmd cmd, ptu32_t para)
{
    bool_t      result = false;
    tagNetDev  *dev;

    dev = (tagNetDev *)handle;
    if((NULL !=dev)&&(NULL != dev->ifctrl))
    {
        result = dev->ifctrl((ptu32_t)dev,cmd,para);
        if(result &&(cmd == EN_NETDEV_SETMAC))
        {
            memcpy(dev->mac,(u8 *)para,CN_MACADDR_LEN);
            //we also need to send arp  response message to update
            ArpInform(dev->name);
        }
    }

    return result;
}

// =============================================================================
// FUNCTION   :use this function to initialize the net device filter
// PARAMS IN  :name, the net device you installed
//
// PARAMS OUT :
// RETURN     :
// DESCRIPTION:
// =============================================================================
bool_t NetDevFilterSet(const char *name,enNetDevFramType type,u32 framelimit,\
		u32 actiontime,u32 measuretime)
{
    bool_t      result = false;
    tagNetDev  *dev;
	tagNetDevRcvFilter *filter;

    dev = NetDevGet((const char *)name);
    if((NULL != dev)&&(type < EN_NETDEV_FRAME_LAST))
    {
		 filter = &dev->rfilter[type];
		 filter->framlimit = framelimit;
		 filter->actiontime = actiontime;
		 filter->measuretime = measuretime;
		 result = true;
    }
    return result;
}
bool_t NetDevFilterEnable(const char *name,enNetDevFramType type,bool_t enable)
{
    bool_t      result = false;
    tagNetDev  *dev;
	tagNetDevRcvFilter *filter;

    dev = NetDevGet((const char *)name);
    if((NULL != dev)&&(type < EN_NETDEV_FRAME_LAST))
    {
		 filter = &dev->rfilter[type];
		 filter->enable = enable;
		 result = true;
    }
    return result;
}

// =============================================================================
// FUNCTION   :use this function to do the frame counter
// PARAMS IN  :handle, the net device handle returned when you install the net device
//             type, the receive frame type,defined by tagNetDevFramType
// PARAMS OUT :NULL
// RETURN     :
// DESCRIPTION:when call this function ,then it will add one to the statistics
// =============================================================================
bool_t NetDevFilterCounter(ptu32_t handle,enNetDevFramType type)
{
	 tagNetDev  *dev;
	 tagNetDevRcvFilter *filter;

	 dev = (tagNetDev *)handle;
	 if((NULL != dev)&&(type < EN_NETDEV_FRAME_LAST))
	 {
		 filter = &dev->rfilter[type];
		 if(filter->enable)
		 {
			 filter->framcounter++;
		 }
		 filter->framtotal++;
		 if(type != EN_NETDEV_FRAME_NOPKG)
		 {
			 dev->rfilter[EN_NETDEV_FRAME_NOPKG].framcounter = 0;
		 }
	 }
	 return true;
}
// =============================================================================
// FUNCTION   :use this function to do the frame filter
// PARAMS IN  :handle, the net device handle returned when you install the net device
// PARAMS OUT :NULL
// RETURN     :true success while false failed
// DESCRIPTION:
// =============================================================================
bool_t NetDevFilterCheck(ptu32_t handle)
{
	u32 type;
	tagNetDev  *dev;
	tagNetDevRcvFilter *filter;
	s64                 timenow;

	dev = (tagNetDev *)handle;
	if(NULL == dev)
	{
		return false;
	}

	timenow = DjyGetSysTime();
	for(type =0; type < EN_NETDEV_FRAME_LAST;type++)  //we should
	{
		filter = &dev->rfilter[type];
		if(filter->enable)
		{
			if(filter->action)
			{
				//THE ACTION HAS START, SO WE'D BETTER TO CHECK IF START TIME IS TIMEOUT
				if(timenow > (filter->actiontime+filter->starttime)) //has timeout
				{
					//so restart the logic
					filter->framcounter = 0;
					filter->starttime = timenow;
					filter->action = false;
					NetDevCtrlByHandle(handle,filter->cmd,true);
				}
				else
				{
					//still in the action logic,we do nothing here
				}
			}
			else
			{
				//the action not start, so we should check if should start the action
				if(filter->framcounter >= filter->framlimit)
				{
					//the logic action should be start
					filter->starttime = timenow;
					filter->action = true;
					filter->actiontimes++;
					NetDevCtrlByHandle(handle,filter->cmd,false);
				}
				else if(timenow >(filter->starttime + filter->measuretime))
				{
					//we should clear it, and measure it in the next stage
					filter->framcounter = 0;
					filter->starttime = timenow;
				}
				else
				{
					//we need do nothing here
				}
			}
		}
		else
		{
			//filter not enable yet
		}
	}
	return true;
}

const char *pFilterItemName[EN_NETDEV_FRAME_LAST]=
{
	"NOPKG",
	"BROAD",
	"POINT",
	"MULTI",
};
//we use this function to show the net device filter state
static bool_t NetDevFilterStat(char *name)
{
    bool_t      result = false;
    tagNetDev  *dev;
	tagNetDevRcvFilter *filter;
	u32                 type;

    dev = NetDevGet((const char *)name);
    if(NULL != dev)
    {
    	printf("%-8s%-5s%-6s%-3s%-10s%-10s%-10s%-4s%-4s%-10s%-10s\n\r",\
    			"Name","EN","STAT","CM","MT(us)","AN","AT(us)","FC","FL","FT","ST(us)");
    	char *instruct="CM:command;"
    			"MT:measure time;"
    			"AN:action times;"
    			"AT:action time;"
    			"FC:frame counter in measure time;"
    			"FL:frame limit in measure time;"
    			"FT:frame up till now;"
    			"ST:action start time";
    	for(type =0; type < EN_NETDEV_FRAME_LAST;type++)
    	{
    		filter = &dev->rfilter[type];
    		printf("%-8s%-5s%-6s%-2d %-8x  %-8x  %-8x  %-4x%-4x%-8llx  %-8llx\n\r",\
    				pFilterItemName[type],\
					filter->enable==true?"Yes":"No",\
					filter->action?"start":"stop",\
					filter->cmd,\
					filter->measuretime,\
					filter->actiontimes,\
					filter->actiontime,\
					filter->framcounter,\
					filter->framlimit,\
					filter->framtotal,\
					filter->starttime
					);
    	}
    	printf("%s\n\r",instruct);
    	result =true;
    }
    return result;
}
// =============================================================================
// FUNCTION   :get the netdev private member
// PARAMS IN  :handle, the install returns handle
// PARAMS OUT :NULL
// RETURN     :the private data you installed
// DESCRIPTION:
// =============================================================================
bool_t NetDevPrivate(ptu32_t handle)
{
    ptu32_t result;
    result = ((tagNetDev*)handle)->private;
    return result;
}
//-----------------------------------------------------------------------------
//功能:use this function to send data to the net device
//参数:
//返回:
//备注:
//作者:zhangqf@下午3:49:46/2016年12月29日
//-----------------------------------------------------------------------------
// =============================================================================
// FUNCTION:Use this function to update the net rout malloced by the dhcp client
// PARA  IN:name, the net dev name
//          ver,you must specified the ip version
//          netaddr, a pointer to tagHostAddrV4 if ver is EN_IP_V4 else tagHostAddrV6
// PARA OUT:
// RETURN  :the corresponding rout
// INSTRUCT:
// =============================================================================
bool_t RoutUpdate(char *name,enum_ipv_t ver,void *netaddr)
{
    bool_t     result = false;
    tagNetDev *dev = NULL;
    tagRout   *rout;

    if(Lock_MutexPend(pRoutMutex, CN_TIMEOUT_FOREVER))
    {
        dev = __NetDevGet(name);
        rout = dev->routq;
        while(NULL != rout)
        {
            if(rout->pro & CN_ROUT_DHCP)
            {
                if((EN_IPV_4 == ver)&&(rout->ver == ver))
                {
                    rout->ipaddr.ipv4 = *(tagHostAddrV4 *)netaddr;
                    result = true;
                }
                break;
            }
            else
            {
            	rout = rout->nxt;
            }
        }
        Lock_MutexPost(pRoutMutex);
    }

    return result;
}

// =============================================================================
// FUNCTION:Use this function to create an rout
// PARA  IN:name, the net dev name
//          ver,you must specified the ip version
//          netaddr, a pointer to tagHostAddrV4 if ver is EN_IP_V4 else tagHostAddrV6
// PARA OUT:
// RETURN  :the corresponding rout
// INSTRUCT:
// =============================================================================
bool_t RoutCreate(const char *name,enum_ipv_t ver,void *netaddr,u32 pro)
{
    bool_t     result = false;
    tagNetDev *dev   = NULL;
    tagRout   *rout  = NULL;
    //we find the dev and check if any rout in the dev has the same addr
    if(Lock_MutexPend(pRoutMutex, CN_TIMEOUT_FOREVER))
    {
        dev = __NetDevGet(name);
        if(NULL != dev)
        {
            rout = dev->routq;
            while(NULL != rout)
            {
                if(rout->ver == ver)
                {
                    if((ver == rout->ver)&&(ver == EN_IPV_4)&&\
                       (rout->ipaddr.ipv4.ip == ((tagHostAddrV4 *)netaddr)->ip))
                    {
                        break;
                    }
                    else
                    {
                        rout = rout->nxt;
                    }
                }
                else
                {
                    rout = rout->nxt;
                }
            }
            if(NULL == rout)
            {
                //no rout matched. so we could add the new rout
                rout = (tagRout *) malloc(sizeof(tagRout));
                if((NULL != rout)&&(ver == EN_IPV_4))
                {
                	memset(rout,0,sizeof(tagRout));
                    //fill the net address and add it to the dev rout chain
                    rout->ver = EN_IPV_4;
                    rout->ipaddr.ipv4 = *(tagHostAddrV4 *)netaddr;
                    rout->pro = pro;
                    rout->func = dev->devfunc;
                    //add to the dew rout chain
                    rout->dev = dev;
                    rout->nxt = dev->routq;
                    dev->routq = rout;
                    if(rout->ipaddr.ipv4.ip == htonl(INADDR_LOOPBACK))
                    {
                        pRoutLoop = rout;   //this is the loop rout
                    }
                    result = true;
                }
            }
        }
        Lock_MutexPost(pRoutMutex);
    }
    return result;
}
// =============================================================================
// FUNCTION:please use this function to delete the rout
// PARA  IN:name. the net dev name
//          ver,you must specified the ip version
//          netaddr, a pointer to tagHostAddrV4 if ver is EN_IP_V4 else tagHostAddrV6
// PARA OUT:
// RETURN  :the corresponding rout
// INSTRUCT:
// =============================================================================
bool_t RoutDelete(const char *name,enum_ipv_t ver,ipaddr_t addr)
{
    bool_t     result = false;
    tagNetDev *dev   = NULL;
    tagRout   *rout  = NULL;
    tagRout   *pre   = NULL;

    //we find the dev and check if any rout in the dev has the same addr
    if(Lock_MutexPend(pRoutMutex, CN_TIMEOUT_FOREVER))
    {
        dev = __NetDevGet(name);
        if(NULL != dev)
        {
            rout = dev->routq;
            pre = rout;
            while(NULL != rout)
            {
                if((ver == rout->ver)&&(ver == EN_IPV_4)&&\
                   (rout->ipaddr.ipv4.ip == addr))
                {
                    //find the one, remove it from the chain and free it
                    if(rout == dev->routq)
                    {
                        dev->routq = rout->nxt;
                    }
                    else
                    {
                        pre->nxt = rout->nxt;
                    }
                    if(rout == pRoutDefaultV4)
                    {
                        pRoutDefaultV4 = NULL;
                    }
                    if(rout == pRoutLoop)
                    {
                    	pRoutLoop = NULL;
                    }
                    free((void *)rout);
                    break;
                }
                else
                {
                    pre = rout;
                    rout = rout->nxt;
                }
            }
        }
        Lock_MutexPost(pRoutMutex);
    }
    return result;
}
// =============================================================================
// FUNCTION:Use this function to modify the existed rout
// PARA  IN:name, the net dev name
//          ver,you must specified the ip version
//          netaddr, a pointer to tagHostAddrV4 if ver is EN_IP_V4 else tagHostAddrV6
// PARA OUT:
// RETURN  :the corresponding rout
// INSTRUCT:
// =============================================================================
bool_t RoutSet(const char *name,enum_ipv_t ver,ipaddr_t ipold,void *newaddr)
{
    bool_t     result = false;
    tagNetDev *dev   = NULL;
    tagRout   *rout  = NULL;
    tagHostAddrV4  *newhost;
    //we find the dev and check if any rout in the dev has the same addr
    if(Lock_MutexPend(pRoutMutex, CN_TIMEOUT_FOREVER))
    {
        dev = __NetDevGet(name);
        if(NULL != dev)
        {
            rout = dev->routq;
            while(NULL != rout)
            {
                if((rout->ver == ver)&&(ver == EN_IPV_4)&&\
                   (rout->ipaddr.ipv4.ip == ipold))
                {
                    break;
                }
                else
                {
                    rout = rout->nxt;
                }
            }
            if(NULL != rout)
            {
                //no rout matched. so we could add the new rout
                //fill the net address
                if(ver == EN_IPV_4)
                {
                    rout->ver = EN_IPV_4;
                    newhost = (tagHostAddrV4 *)newaddr;
                    if(newhost->broad != INADDR_ANY)
                    {
                        rout->ipaddr.ipv4.broad = newhost->broad;
                    }
                    if(newhost->dns != INADDR_ANY)
                    {
                        rout->ipaddr.ipv4.dns = newhost->dns;
                    }
                    if(newhost->gatway != INADDR_ANY)
                    {
                        rout->ipaddr.ipv4.gatway = newhost->gatway;
                    }
                    if(newhost->submask != INADDR_ANY)
                    {
                        rout->ipaddr.ipv4.submask = newhost->submask;
                    }
                    if(newhost->ip != INADDR_ANY)
                    {
                        rout->ipaddr.ipv4.ip = newhost->ip;
                    }
                    result = true;
                }
            }
        }
        Lock_MutexPost(pRoutMutex);
    }
    return result;
}
// =============================================================================
// FUNCTION:use this function to set the default rout
// PARA  IN: ip, the specified ip of the rout,if INADDR_ANY, then use the dynamic one
// PARA OUT:
// RETURN  :
// INSTRUCT:
// =============================================================================
bool_t RoutSetDefault(enum_ipv_t ver,ipaddr_t ip)
{
    bool_t     result = false;
    tagNetDev *dev   = NULL;
    tagRout   *rout  = NULL;
    //we find the dev and check if any rout in the dev has the same addr
    if(Lock_MutexPend(pRoutMutex, CN_TIMEOUT_FOREVER))
    {
        dev = __NetDevGet(NULL);
        while(NULL != dev)
        {
            rout = dev->routq;
            while(NULL != rout)
            {
                if((rout->ver == EN_IPV_4)&&(rout->ver == ver))
                {
                    if((ip == INADDR_ANY)&&(rout->pro&CN_ROUT_DHCP))
                    {
                        pRoutDefaultV4 = rout;
                        result = true;
                        rout = NULL;
                        dev = NULL;
                    }
                    else if(rout->ipaddr.ipv4.ip == ip)
                    {
                        pRoutDefaultV4 = rout;
                        result = true;
                        rout = NULL;
                        dev = NULL;  //stop the search
                    }
                    else
                    {
                        rout = rout->nxt;
                    }
                }
                else
                {
                    rout = rout->nxt;
                }
            }
            if(NULL != dev)
            {
                dev = dev->nxt;
            }
        }
        Lock_MutexPost(pRoutMutex);
    }
    return result;
}
// =============================================================================
// FUNCTION:use this function to get the default dns ip
// PARA  IN: ver,you must specified the ip version
// PARA OUT:ip, the specified ip buf
// RETURN  :the ip class
// INSTRUCT:
// =============================================================================
bool_t RoutDns(enum_ipv_t ver, ipaddr_t ip)
{
    bool_t result;
    if((ver == EN_IPV_4)&&(NULL != (u32 *)ip) &&(NULL != pRoutDefaultV4))
    {
        *(u32 *)ip = pRoutDefaultV4->ipaddr.ipv4.dns;
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

//add some rout debug function
//for the debug
static void __RoutShow(tagNetDev *dev)
{
    tagRout       *rout;
    struct in_addr       addr;

    printf("***************************************************************\r\n");
    printf("Device:Name      :%s---LinkType:%s(%d)\r\n",dev->name,\
    		LinkTypeName(dev->iftype),dev->iftype);
    printf("Device:Mac       :%02x-%02x-%02x-%02x-%02x-%02x\r\n",\
            dev->mac[0],dev->mac[1],dev->mac[2],\
            dev->mac[3],dev->mac[4],dev->mac[5]);
    printf("Device:Property  :MTU= %d  Function=0x%08x\r\n",\
            dev->mtu,dev->devfunc);
    printf("Device:STAT      :snd = %d  snderr=%d\r\n",dev->fsnd,dev->fsnderr);
    printf("Device:STAT      :rcv = %d  rcverr=%d\r\n",dev->frcv,dev->frcverr);

    rout = dev->routq;
    while(NULL != rout)
    {
        //print net addr
        if(rout->ver == EN_IPV_4)
        {
            memcpy((void *)&addr,(void *)&rout->ipaddr.ipv4.ip,sizeof(addr));
            printf("IPV4  :ip        :%s(%s)\r\n",inet_ntoa(addr),\
                    rout->pro&CN_ROUT_DHCP?"dynamic":"static");
            memcpy((void *)&addr,(void *)&rout->ipaddr.ipv4.submask,sizeof(addr));
            printf("IPV4  :submask   :%s\r\n",inet_ntoa(addr));
            memcpy((void *)&addr,(void *)&rout->ipaddr.ipv4.gatway,sizeof(addr));
            printf("IPV4  :gateway   :%s\r\n",inet_ntoa(addr));
            memcpy((void *)&addr,(void *)&rout->ipaddr.ipv4.broad,sizeof(addr));
            printf("IPV4  :broad     :%s\r\n",inet_ntoa(addr));
            memcpy((void *)&addr,(void *)&rout->ipaddr.ipv4.dns,sizeof(addr));
            printf("IPV4  :dns       :%s\r\n",inet_ntoa(addr));
        }
        else
        {
            printf("Unknow Host ADDR\r\n");
        }

        printf("RoutSndNum:Total:0x%llx Err:0x%llx\n\r",rout->outnum,rout->outerr);
        printf("RoutRcvNum:Total:0x%llx Err:0x%llx\n\r",rout->innum,rout->inerr);

        rout = rout->nxt;
    }
}


//show or config the rout
//0 -devname
//1 -oldip
//2 -newip
//3 -subnetmask
//4 -gatway
//5 -broad
//6 -dns
static bool_t RoutIfConfig(char *param)
{
    bool_t result;
    int   argc =7;
    char *argv[7];
    tagNetDev  *dev;

    tagHostAddrV4  hostaddr;
    ipaddr_t       ipold;

    result = false;
    string2arg(&argc,argv,param);

    hostaddr.broad = INADDR_ANY;
    hostaddr.dns = INADDR_ANY;
    hostaddr.gatway = INADDR_ANY;
    hostaddr.submask = INADDR_ANY;
    hostaddr.ip = INADDR_ANY;

    switch(argc)
    {
        case 7:
            hostaddr.dns = inet_addr(argv[6]);
            hostaddr.broad = inet_addr(argv[5]);
            hostaddr.gatway = inet_addr(argv[4]);
            hostaddr.submask = inet_addr(argv[3]);
            hostaddr.ip = inet_addr(argv[2]);
            ipold = inet_addr(argv[1]);
            result = RoutSet(argv[0],EN_IPV_4,ipold,&hostaddr);
            break;
        case 6:
            hostaddr.broad = inet_addr(argv[5]);
            hostaddr.gatway = inet_addr(argv[4]);
            hostaddr.submask = inet_addr(argv[3]);
            hostaddr.ip = inet_addr(argv[2]);
            ipold = inet_addr(argv[1]);
            result = RoutSet(argv[0],EN_IPV_4,ipold,&hostaddr);
            break;
        case 5:
            hostaddr.gatway = inet_addr(argv[4]);
            hostaddr.submask = inet_addr(argv[3]);
            hostaddr.ip = inet_addr(argv[2]);
            ipold = inet_addr(argv[1]);
            result = RoutSet(argv[0],EN_IPV_4,ipold,&hostaddr);
            break;
        case 4:
            hostaddr.submask = inet_addr(argv[3]);
            hostaddr.ip = inet_addr(argv[2]);
            ipold = inet_addr(argv[1]);
            result = RoutSet(argv[0],EN_IPV_4,ipold,&hostaddr);
            break;
        case 3:
            hostaddr.ip = inet_addr(argv[2]);
            ipold = inet_addr(argv[1]);
            result = RoutSet(argv[0],EN_IPV_4,ipold,&hostaddr);
            break;
        case 1:
            dev = NetDevGet((const char *)argv[0]);
            if(NULL != dev)
            {
                __RoutShow(dev);
                result = true;
            }
            break;
        case 0:
            dev = NetDevGet(NULL);
            while(NULL != dev)
            {
                __RoutShow(dev);
                dev = dev->nxt;
            }
            result = true;
            break;
        default:
            break;
    }

    return result;
}

//use this function to delete the net dev
static bool_t  NetDevRemove(char *param)
{
    bool_t result;

    result = NetDevUninstall(param);

    return result;
}
//use this function to set the netdev mac
static bool_t NetDevMacSet(char *param)
{
    u8   mac[CN_MACADDR_LEN];
    int  argc = 2;
    char *argv[2];
    bool_t result;

    result = false;
    string2arg(&argc,argv,param);
    if(argc == 2)
    {
        if(string2Mac(argv[1],mac))
        {
            result = NetDevCtrl(argv[0],EN_NETDEV_SETMAC,(ptu32_t)mac);
        }
    }
    return result;
}

//use this function to find a rout to output the ip package
//-----------------------------------------------------------------------------
//功能:use this function to get the default rout
//参数:ver,which means ipv4 or ipv6
//返回:
//备注:if no rout match the destination then we could use the default rout to send the package
//作者:zhangqf@下午5:48:47/2016年12月28日
//-----------------------------------------------------------------------------
tagRout *RoutGetDefault(enum_ipv_t ver)
{
	tagRout *result = NULL;
	if(ver == EN_IPV_4)
	{
		result = pRoutDefaultV4;
	}
	return result;
}

bool_t *RoutSetDefaultAddr(enum_ipv_t ver,ipaddr_t ip,ipaddr_t mask,ipaddr_t gateway,ipaddr_t dns)
{
	bool_t result = false;
	if((NULL != pRoutDefaultV4)&&(ver = EN_IPV_4))
	{
		pRoutDefaultV4->ipaddr.ipv4.ip = ip;
		pRoutDefaultV4->ipaddr.ipv4.submask = mask;
		pRoutDefaultV4->ipaddr.ipv4.gatway = gateway;
		pRoutDefaultV4->ipaddr.ipv4.dns = dns;
		pRoutDefaultV4->ipaddr.ipv4.broad = (ip&mask)|(~mask);

		result = false;
	}
	return true;
}





//-----------------------------------------------------------------------------
//功能:use this function to select a match rout,(same subnet)
//参数:ver:means ipv4 or ipv6;ipaddr:which means the ipv4 address or the ipv6 address
//返回:
//备注:
//作者:zhangqf@下午5:53:32/2016年12月28日
//-----------------------------------------------------------------------------
tagRout *RoutMatch(enum_ipv_t ver,ipaddr_t ipaddr)
{
	tagRout   *result = NULL;
    tagRout   *tmp = NULL;
    tagNetDev *dev = NULL;
    u32        ip;
    if(Lock_MutexPend(pRoutMutex,CN_TIMEOUT_FOREVER))
    {
        if(ver == EN_IPV_4)
        {
        	ip = (u32)ipaddr;
            dev = __NetDevGet(NULL); //get the queue
            while(NULL != dev)       //loop all the dev to find an proper device
            {
                tmp = dev->routq;
                while(NULL != tmp)
                {
                    if(ver == tmp->ver)
                    {
                        if(ip == tmp->ipaddr.ipv4.ip) //host ip use the loop rout
                        {
                            result = pRoutLoop;
                            dev = NULL;   //stop the search
                            break;
                        }
                        else if((ip & tmp->ipaddr.ipv4.submask)==\
                                (tmp->ipaddr.ipv4.ip&tmp->ipaddr.ipv4.submask))
                        {
                            result = tmp;
                            dev = NULL;   //stop the search
                            break;
                        }
                        else
                        {

                        }
                    }
                    tmp = tmp->nxt;
                }
                if(NULL != dev)
                {
                    dev = dev->nxt;
                }
            }
        }
        Lock_MutexPost(pRoutMutex);
    }
    return   result;
}
//-----------------------------------------------------------------------------
//功能:use this function to judge whether the ip is the host address
//参数:
//返回:true when the ip match the INADDR_ANY OR THE ROUT IP else false
//备注:
//作者:zhangqf@下午6:07:57/2016年12月28日
//-----------------------------------------------------------------------------
bool_t RoutHostIp(enum_ipv_t ver,ipaddr_t ipaddr)
{
	bool_t     result = false;
	tagRout   *tmp = NULL;
	tagNetDev *dev = NULL;
	u32        ip;
	if(Lock_MutexPend(pRoutMutex,CN_TIMEOUT_FOREVER))
	{
	    if(ver == EN_IPV_4)
	    {
	    	ip = (u32)ipaddr;
	    	if(ip == INADDR_ANY)
	    	{
	    		result = true;
	    	}
	    	else  //check if any ip is match this
	    	{
		        dev = __NetDevGet(NULL); //get the queue
		        while(NULL != dev)       //loop all the dev to find an proper device
		        {
		            tmp = dev->routq;
		            while(NULL != tmp)
		            {
		                if(ver == tmp->ver)
		                {
		                    if(ip == tmp->ipaddr.ipv4.ip) //host ip use the loop rout
		                    {
		                        result = true;
		                        dev = NULL;   //stop the search
		                        break;
		                    }
		                    else
		                    {
		                    	//continue
		                    }
		                }
		                tmp = tmp->nxt;
		            }
		            if(NULL != dev)
		            {
		                dev = dev->nxt;
		            }
		        }
	    	}
	    }
	    Lock_MutexPost(pRoutMutex);
	}
	return   result;
}

//-----------------------------------------------------------------------------
//功能:use this function to judge whether the ip is the host address
//参数:
//返回:true when the ip match the INADDR_BROADCAST OR THE ROUT IP else false
//备注:
//作者:zhangqf@下午6:07:57/2016年12月28日
//-----------------------------------------------------------------------------
bool_t RoutHostTarget(enum_ipv_t ver,ipaddr_t ipaddr)
{
	bool_t     result = false;
	tagRout   *tmp = NULL;
	tagNetDev *dev = NULL;
	u32        ip;
	if(Lock_MutexPend(pRoutMutex,CN_TIMEOUT_FOREVER))
	{
	    if(ver == EN_IPV_4)
	    {
	    	ip = (u32)ipaddr;
	    	if(ip == INADDR_BROADCAST)
	    	{
	    		result = true;
	    	}
	    	else  //check if any ip is match this
	    	{
		        dev = __NetDevGet(NULL); //get the queue
		        while(NULL != dev)       //loop all the dev to find an proper device
		        {
		            tmp = dev->routq;
		            while(NULL != tmp)
		            {
		                if(ver == tmp->ver)
		                {
		                    if(ip == tmp->ipaddr.ipv4.ip) //host ip use the loop rout
		                    {
		                        result = true;
		                        dev = NULL;   //stop the search
		                        break;
		                    }
		                    else if(ip == tmp->ipaddr.ipv4.broad)
		                    {
		                        result = true;
		                        dev = NULL;   //stop the search
		                        break;
		                    }
		                    else if(ip == tmp->ipaddr.ipv4.multi)
		                    {
		                        result = true;
		                        dev = NULL;   //stop the search
		                        break;
		                    }
		                    else
		                    {
		                    	//continue
		                    }
		                }
		                tmp = tmp->nxt;
		            }
		            if(NULL != dev)
		            {
		                dev = dev->nxt;
		            }
		        }
	    	}
	    }
	    Lock_MutexPost(pRoutMutex);
	}
	return   result;
}
//-----------------------------------------------------------------------------
//功能:use this function to check if the ip is in the subnet of binded to the specified device
//参数:
//返回:
//备注:
//作者:zhangqf@下午2:45:01/2016年12月29日
//-----------------------------------------------------------------------------
bool_t RoutSubNet(tagNetDev *dev,enum_ipv_t ver,ipaddr_t ipaddr)
{
	bool_t     result = false;
	tagRout   *tmp = NULL;
	u32        ip;
	if(Lock_MutexPend(pRoutMutex,CN_TIMEOUT_FOREVER))
	{
	    if(ver == EN_IPV_4)
	    {
	    	ip = (u32)ipaddr;
            tmp = dev->routq;
            while(NULL != tmp)
            {
                if(ver == tmp->ver)
                {
                    if((ip&tmp->ipaddr.ipv4.submask)== \
                    	(tmp->ipaddr.ipv4.ip&tmp->ipaddr.ipv4.submask))
                    {
                        result = true;
                        dev = NULL;   //stop the search
                        break;
                    }
                }
                tmp = tmp->nxt;
            }
	    }
	    Lock_MutexPost(pRoutMutex);
	}
	return   result;
}

//here we create an net dev monitor task to do the broad storm inform
//check every ten seconds;
static  ptu32_t __NetDevMonitor(void)
{
	tagNetDev  *dev;
	tagRout    *rout;

	while(1)
	{
	    if(Lock_MutexPend(pRoutMutex,CN_TIMEOUT_FOREVER))
	    {
	    	dev = pNetDevQ;
	    	while(NULL != dev)
	    	{
		    	if(dev->rfilter[EN_NETDEV_FRAME_BROAD].action)
		    	{
		    		//THE FILTER HAS START, DO THE BROAD INFORM
		    		rout = dev->routq;
		    		while(NULL != rout)
		    		{
		    			ArpInformAll(rout);
		    			rout = rout->nxt;
		    		}
		    	}

	    		dev = dev->nxt;
	    	}

	    	Lock_MutexPost(pRoutMutex);
	    }

	    Djy_EventDelay(10*1000*mS);
	}
	return 0;
}


struct ShellCmdTab  gRoutDebug[] =
{
    {
        "ifconfig",
        RoutIfConfig,
        "usage:ifconfig [devname] [oldip] [newip] [subnetmask] [gatway] [broad] [dns]",
        "usage:ifconfig [devname] [oldip] [newip] [subnetmask] [gatway] [broad] [dns]",
    },
    {
        "netdevremove",
        NetDevRemove,
        "usage:netdevremove devname",
        "usage:netdevremove devname",
    },
    {
        "netdevmac",
        NetDevMacSet,
        "usage:netdevmac devname  mac0-mac1-mac2-mac3-mac4-mac5",
        "usage:netdevmac devname  mac0-mac1-mac2-mac3-mac4-mac5",
    },
    {
        "netdevfilter",
		NetDevFilterStat,
        "usage:netdevfilter",
        "usage:netdevfilter",
    },
};
#define CN_ROUTDEBUG_NUM  ((sizeof(gRoutDebug))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc gRoutDebugCmdRsc[CN_ROUTDEBUG_NUM];
// =============================================================================
// FUNCTION   :this is the rout module initialize function
// PARAMS IN  :
// PARAMS OUT :
// RETURN     :
// DESCRIPTION:
// =============================================================================
bool_t RoutInit(void)
{
    bool_t result;

    u16 evttID;
    u16 eventID;

    pNetDevQ = NULL;
    pRoutLoop = NULL;
    pRoutDefaultV4 = NULL;

    pRoutMutex = Lock_MutexCreate(NULL);
    if(NULL == pRoutMutex)
    {
        goto EXIT_MUTEX;
    }
    result = Sh_InstallCmd(gRoutDebug,gRoutDebugCmdRsc,CN_ROUTDEBUG_NUM);
    if(false == result)
    {
        goto EXIT_ROUTCMD;
    }
    //create the device monitor task
    evttID= Djy_EvttRegist(EN_CORRELATIVE, CN_PRIO_RRS, 0, 1,
             (ptu32_t (*)(void))__NetDevMonitor,NULL, 0x800, "DevMonitor");
    if(evttID == CN_EVTT_ID_INVALID)
    {
        goto EXIT_EVTTFAILED;
    }
    eventID = Djy_EventPop(evttID, NULL, 0, 0, 0, 0);
    if(eventID == CN_EVENT_ID_INVALID)
    {
        goto EXIT_EVENTFAILED;
    }
    result = true;
    return result;

EXIT_EVENTFAILED:
	Djy_EvttUnregist(evttID);
EXIT_EVTTFAILED:
EXIT_ROUTCMD:
    Lock_MutexDelete(pRoutMutex);
    pRoutMutex = NULL;
EXIT_MUTEX:
    result = false;
    return result;
}


