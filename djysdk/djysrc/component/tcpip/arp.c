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
#include <sys/socket.h>

#include "rout.h"
#include "link.h"
#include "tcpipconfig.h"

#include "arp.h"

//arp的数据报
#pragma pack(1)
typedef struct
{
#define CN_ARP_HW_TYPE_ETHERNET        0x01
    u16 hwtype;
    u16 protype;
    u8  hwaddrlen;
    u8  proaddrlen;
#define CN_ARP_OP_REQUEST              0x01
#define CN_ARP_OP_RESPONSE             0x02
    u16 opcode;
    u8  senhwaddr[CN_MACADDR_LEN];
    u8  senproaddr[CN_IPADDR_LEN_V4];
    u8  tarhwaddr[CN_MACADDR_LEN];
    u8  tarproaddr[CN_IPADDR_LEN_V4];
    u8  rev_blank[18];  // 以太网上要求上层数据包至少有46字节，这18字节用于填充
}tagArpHdr;

typedef struct
{
    u8 macdst[CN_MACADDR_LEN];
    u8 macsrc[CN_MACADDR_LEN];
    u8 proto[2];
    tagArpHdr arp;
}tagArpFrame;
#pragma pack()

#define CN_ARP_HDRLEN          (sizeof(tagArpHdr))
#define CN_ARP_FRAMELEN        (sizeof(tagArpFrame))
#define CN_ARPITEM_SCANCYCLE   (60*1000*mS)   //(30 seconds)
#define CN_ARPITEM_LIFE        (20)           //20 UNIT(20 minute)
#define CN_ARPITEM_LIFE_TRIG   (10)           //if life less than this, we will give the arp request

typedef struct  __ArpItem
{
   struct  __ArpItem  *nxt;                   //as the hash chain
   u32 ip;
   u8  mac[CN_MACADDR_LEN];
   u8  timeout;
   u8  pro;
   u32 reffers;        //means how many times reffered, if none and the timeout is up
                       //this arp item will be deleted,each cycle will reset
}tagArpItem;
static struct MutexLCB      *pArpItemMutex = NULL;//use this to protect the arp item
static struct SemaphoreLCB  *pArpEngineSync = NULL; //use this to triger the arp scan thread
static tagArpItem          **pArpItemTab  = NULL;     //which used to arp hash chain
static tagNetPkg            *pArpPkgQH  = NULL;     //this used to storage the arp package head
static struct MutexLCB      *pArpPkgMutex = NULL; //this used to protect the arp package queue

typedef struct
{
   u32 sndnum;
   u32 snderr;
   u32 rcvnum;
   u32 rcvreq;
   u32 rcvres;
   u32 rcverr;
   u32 rcvcached;
}tagArpStatistics;

static tagArpStatistics ArpStatistics;
//use this function to match an item in the arp tab with specified ip address
static tagArpItem *__ArpMatchItem(u32 ip)
{
    tagArpItem  *result = NULL;
    u32          offset;
    tagArpItem *tmp;

    offset = ip%gArpItemHashLen;
    tmp = pArpItemTab[offset];
    while(NULL != tmp)
    {
        if(tmp->ip == ip)
        {
            result = tmp;
            break;
        }
        tmp = tmp->nxt;
    }

    return result;
}
//use this function to malloc mem and insert an arpitem to the arp item tab
static tagArpItem *__ArpCreateItem(u32 ip)
{
    tagArpItem *result;
    u32         offset;

    result = (tagArpItem *)malloc(sizeof(tagArpItem));
    if(NULL != result)
    {
        memset((void *)result,0,sizeof(tagArpItem));
        result->ip = ip;
        offset = ip%gArpItemHashLen;
        result->nxt = pArpItemTab[offset];
        pArpItemTab[offset] = result;
    }
    return result;
}
//use this function to delete the arp item
static bool_t __ArpDeleteItem(u32 ip)
{
    bool_t      result = false;
    tagArpItem *tmp,*bak;
    u32         offset;

    offset = ip%gArpItemHashLen;
    tmp = pArpItemTab[offset];

    bak = tmp;
    while(NULL != tmp)
    {
        if(tmp->ip == ip)
        {
            //match this, so delete it
            if(pArpItemTab[offset] == tmp)
            {
                pArpItemTab[offset] = tmp->nxt;
            }
            else
            {
                bak->nxt = tmp->nxt;
            }
            free((void *)tmp);
            result = true;
            break;
        }
        else
        {
            bak = tmp;
            tmp = tmp->nxt;
        }
    }

    return result;
}


//use this function to do the arp request
static bool_t __ArpSndRequest(u32 ip)
{
    bool_t             result;
    enum_ip_class      ipclass;
    tagNetPkg         *pkg;
    tagRout           *rout;
    u32                ipsrc;
    u32                ipdst;
    u16                proto;
    u8                *macsrc;
    u8                *macdst;
    tagArpHdr         *hdr;
    tagArpFrame       *frame;

    result = false;
    ipdst = ip;
    TCPIP_DEBUG_INC(ArpStatistics.sndnum);
    //if the src is in the sub net, we will response it, else do nothing
    ipclass = RoutIpClass(EN_IPV_4,ipdst,&rout);
    if(ipclass == EN_IPV4_INSUBNET)  //same subnet, we will do the request
    {
        //we will create an item for this ip
        pkg = PkgMalloc(CN_ARP_FRAMELEN, CN_PKLGLST_END);
        if(NULL != pkg)
        {
            //LEAVE SOME SPACE FOR THE LINK
            frame = (tagArpFrame*)(pkg->buf + pkg->offset);
            memset((void *)frame,0,sizeof(tagArpFrame));

            hdr = &frame->arp;
            ipdst = ip;
            ipsrc = rout->ipaddr.ipv4.ip;
            macdst = (u8 *)(CN_MAC_BROAD);
            macsrc = NetDevGetMac(rout->dev);
            proto = htons(EN_NET_PROTO_ARP);

            memcpy(frame->macdst,macdst,CN_MACADDR_LEN);
            memcpy(frame->macsrc,macsrc,CN_MACADDR_LEN);
            memcpy(frame->proto,&proto,2);
            memcpy(hdr->tarproaddr ,&ipdst,CN_IPADDR_LEN_V4);
            memcpy(hdr->senproaddr ,&ipsrc,CN_IPADDR_LEN_V4);
            memset(hdr->tarhwaddr,0,CN_MACADDR_LEN);
            memcpy(hdr->senhwaddr,macsrc,CN_MACADDR_LEN);
            hdr->hwtype = htons(CN_ARP_HW_TYPE_ETHERNET);
            hdr->protype = htons(EN_NET_PROTO_IP);
            hdr->hwaddrlen = CN_MACADDR_LEN;
            hdr->proaddrlen = CN_IPADDR_LEN_V4;
            hdr->opcode = htons(CN_ARP_OP_REQUEST);
            pkg->datalen = CN_ARP_FRAMELEN;
            result = LinkSendRaw((ptu32_t)rout->dev,pkg,CN_ARP_FRAMELEN,CN_IPDEV_NONE);
            PkgTryFreePart(pkg);
            result = true;
        }
    }

    if(result == false)
    {
        TCPIP_DEBUG_DEC(ArpStatistics.snderr);
    }

    return result;
}

//i want send a arp response to (ip,mac) through (rout) with the
//ip and mac inside rout
static bool_t __ArpSndResponse(u32 ip,u8 *mac,tagRout *rout)
{

    bool_t             result;
    u32                ipsrc;
    u32                ipdst;
    u16                proto;
    u8                *macsrc;
    u8                *macdst;
    tagArpHdr         *hdr;
    tagNetPkg         *pkg;
    tagArpFrame       *frame;

    result = false;
    //we will create an item for this ip
    pkg = PkgMalloc(CN_ARP_FRAMELEN, CN_PKLGLST_END);
    if(NULL != pkg)
    {
        //LEAVE SOME SPACE FOR THE LINK
        frame = (tagArpFrame*)(pkg->buf + pkg->offset);
        memset((void *)frame,0,sizeof(tagArpFrame));

        hdr = &frame->arp;
        ipdst = ip;
        ipsrc = rout->ipaddr.ipv4.ip;
        macdst = mac;
        macsrc = NetDevGetMac(rout->dev);
        proto = htons(EN_NET_PROTO_ARP);

        memcpy(frame->macdst,macdst,CN_MACADDR_LEN);
        memcpy(frame->macsrc,macsrc,CN_MACADDR_LEN);
        memcpy(frame->proto,&proto,2);
        memcpy(hdr->tarproaddr ,&ipdst,CN_IPADDR_LEN_V4);
        memcpy(hdr->senproaddr ,&ipsrc,CN_IPADDR_LEN_V4);
        memcpy(hdr->tarhwaddr, macdst, CN_MACADDR_LEN);
        memcpy(hdr->senhwaddr,macsrc,CN_MACADDR_LEN);
        hdr->hwtype = htons(CN_ARP_HW_TYPE_ETHERNET);
        hdr->protype = htons(EN_NET_PROTO_IP);
        hdr->hwaddrlen = CN_MACADDR_LEN;
        hdr->proaddrlen = CN_IPADDR_LEN_V4;
        hdr->opcode = htons(CN_ARP_OP_RESPONSE);
        pkg->datalen = CN_ARP_FRAMELEN;
        result = LinkSendRaw((ptu32_t)rout->dev,pkg,CN_ARP_FRAMELEN,CN_IPDEV_NONE);
        PkgTryFreePart(pkg);
        result = true;
    }
    return result;
}


//use this function to deal with the arp request
bool_t __Arp_Process_Request(tagNetDev *netdev,tagArpHdr *arp)
{
    bool_t             result;
    u32                ipdst;
    u32                ipsrc;
    enum_ip_class      ipclass;
    u8                *macsrc;
    tagRout           *rout;

    result = false;
    memcpy(&ipdst,arp->tarproaddr,CN_IPADDR_LEN_V4);
    memcpy(&ipsrc,arp->senproaddr,CN_IPADDR_LEN_V4);
    macsrc = arp->senhwaddr;

    ipclass = RoutIpClass(EN_IPV_4,ipdst,&rout);
    if(ipclass == EN_IPV4_HOSTTARGET)
    {
        //if the src is in the sub net, we will response it, else do nothing
        ipclass = RoutIpClass(EN_IPV_4,ipsrc,&rout);
        if(ipclass == EN_IPV4_INSUBNET)  //same subnet, so could response now
        {
            //create the item or update it
            ArpItemCreate(ipsrc,macsrc,CN_ARPITEM_PRO_STABLE);
            //we will create an item for this ip
            __ArpSndResponse(ipsrc,macsrc,rout);
        }
    }
    return result;
}

//use this function to deal with the response
bool_t __Arp_Process_Response(tagNetDev *netdev,tagArpHdr *arp)
{
    bool_t             result;
    u32                ipsrc;
    enum_ip_class      ipclass;
    u8                *macsrc;
    tagRout           *rout;

    result = false;
    memcpy(&ipsrc,arp->senproaddr,CN_IPADDR_LEN_V4);
    macsrc = arp->senhwaddr;
    //if the src is in the sub net, update the item
    ipclass = RoutIpClass(EN_IPV_4,ipsrc,&rout);
    if(ipclass == EN_IPV4_INSUBNET)  //same subnet,
    {
        //do the arp item update
        ArpItemUpdate(ipsrc,macsrc);
    }
    return result;
}

//use this function to get a package from the arp package queue
static tagNetPkg* __ArpPkgPop(void)
{
    tagNetPkg *result = NULL;
    if(Lock_MutexPend(pArpPkgMutex,CN_TIMEOUT_FOREVER))
    {
        result = pArpPkgQH;
        if(NULL != result)
        {
            pArpPkgQH = result->partnext;
            result->partnext = NULL;
            TCPIP_DEBUG_DEC(ArpStatistics.rcvcached);
        }
        Lock_MutexPost(pArpPkgMutex);
    }
    return result;
}
//this function used to process the arp package
static void __ArpPkgDeal(void)
{
    tagArpHdr    *hdr;
    u16           opcode;
    tagNetDev    *dev;
    tagNetPkg    *pkg;

    while((pkg = __ArpPkgPop()) != NULL)
    {
        dev = (tagNetDev *)pkg->private;

        if((NULL != dev)&& (NULL!= pkg))
        {
            hdr = (tagArpHdr *)(pkg->buf + pkg->offset);
            pkg->offset += sizeof(tagArpHdr);
            pkg->datalen -= sizeof(tagArpHdr);
            opcode = ntohs(hdr->opcode);
            switch (opcode)
            {
                case CN_ARP_OP_REQUEST:
                    __Arp_Process_Request(dev,hdr);
                    TCPIP_DEBUG_INC(ArpStatistics.rcvreq);
                    break;
                case CN_ARP_OP_RESPONSE:
                    __Arp_Process_Response(dev,hdr);
                    TCPIP_DEBUG_INC(ArpStatistics.rcvres);
                    break;
                default:
                    TCPIP_DEBUG_INC(ArpStatistics.rcverr);
                    break;
            }
        }
        PkgTryFreePart(pkg);
    }
}
// =============================================================================
// FUNCTION:this function is the arp post function. the link will call this function
//          to deal with arp package
// PARA  IN:dev, which dev receive the package
//          pkg, the arp frame
// PARA OUT:
// RETURN  :true post success while false failed
// INSTRUCT:
// =============================================================================
bool_t  ArpPushPkg(tagNetDev *dev,tagNetPkg *pkg)
{
    bool_t result = false;

    TCPIP_DEBUG_INC(ArpStatistics.rcvnum);

    if(Lock_MutexPend(pArpPkgMutex,CN_TIMEOUT_FOREVER))
    {
        pkg->private = (ptu32_t)dev;
        pkg->partnext = pArpPkgQH;
        pArpPkgQH = pkg;
        PkgCachedPart(pkg);
        TCPIP_DEBUG_INC(ArpStatistics.rcvcached);
        Lock_MutexPost(pArpPkgMutex);

        Lock_SempPost(pArpEngineSync);
        result = true;
    }

    return result;
}

//this function used to scan the arp item tab with a fixed cycle
//if the life of the item is less than the trigger level, then it will send a request
//if the life is zero, then it will be delete from the queue
//we scan the item by the hash value, so we will not block the sync too much
static ptu32_t  __ArpProcessor(void)
{
    u32         offset;
    tagArpItem *tmp;
    tagArpItem *bak;

    while(1)
    {
        if(Lock_SempPend(pArpEngineSync,CN_ARPITEM_SCANCYCLE))
        {
            //there is some arp package that we should process
            __ArpPkgDeal(); //deal all the arp package
        }
        else
        {
            //timeout we should process all the timeout arp item
            for(offset =0; offset < gArpItemHashLen;offset ++)
            {
                if(Lock_MutexPend(pArpItemMutex,CN_TIMEOUT_FOREVER))
                {
                    tmp = pArpItemTab[offset];
                    while(NULL != tmp)
                    {
                        bak = tmp->nxt;
                        if(tmp->timeout == 0)
                        {
                            __ArpDeleteItem(tmp->ip);
                        }
                        else if(tmp->timeout <= CN_ARPITEM_LIFE_TRIG)
                        {
                            //we should send an arp request
                            __ArpSndRequest(tmp->ip);
                            tmp->timeout--;
                        }
                        else
                        {
                            //we need do nothing here
                            if(tmp->pro & CN_ARPITEM_PRO_DYNAMIC)
                            {
                                tmp->timeout--;
                            }
                        }
                        tmp = bak;
                    }
                    Lock_MutexPost(pArpItemMutex);
                }
            }
        }
    }

    return 0;
}

// =============================================================================
// FUNCTION:use this function to update the arp item
// PARA  IN:ip,the is an ipv4 address
//         :mac, the corresponding mac address with ip
// PARA OUT:
// RETURN  :true success while false failed
// INSTRUCT:
// =============================================================================
bool_t ArpItemUpdate(u32 ip,u8 *mac)
{
    bool_t      result = false;
    tagArpItem *tmp;

    if(Lock_MutexPend(pArpItemMutex,CN_TIMEOUT_FOREVER))
    {
        tmp = __ArpMatchItem(ip);
        if((NULL != tmp)&&(tmp->pro &CN_ARPITEM_PRO_DYNAMIC))
        {
            memcpy(tmp->mac,mac,CN_MACADDR_LEN);
            tmp->pro |= CN_ARPITEM_PRO_STABLE;
            tmp->timeout = CN_ARPITEM_LIFE;
        }
        Lock_MutexPost(pArpItemMutex);
    }
    return result;
}

// =============================================================================
// FUNCTION:use this function to create a arp item
// PARA  IN:ip,the is an ipv4 address
//         :mac, the corresponding mac address with ip
//         :pro, the item property, such as dynamic or static
// PARA OUT:
// RETURN  :true success while false failed
// INSTRUCT:
// =============================================================================
bool_t ArpItemCreate(u32 ip,u8 *mac,u32 pro)
{
    bool_t      result = false;
    tagArpItem *tmp;

    if(Lock_MutexPend(pArpItemMutex,CN_TIMEOUT_FOREVER))
    {
        tmp = __ArpMatchItem(ip);
        if(NULL != tmp)
        {
            memcpy(tmp->mac,mac,CN_MACADDR_LEN);
            tmp->pro = pro|CN_ARPITEM_PRO_STABLE;
            tmp->timeout = CN_ARPITEM_LIFE;
            result = true;
        }
        else
        {
            tmp = __ArpCreateItem(ip);
            if(NULL != tmp)
            {
                memcpy(tmp->mac,mac,CN_MACADDR_LEN);
                tmp->pro = pro|CN_ARPITEM_PRO_STABLE;
                tmp->timeout = CN_ARPITEM_LIFE;
                result = true;
            }
        }
        Lock_MutexPost(pArpItemMutex);
    }
    return result;
}
// =============================================================================
// FUNCTION:use this function to delete the arp item
// PARA  IN:ip,the is an ipv4 address
// PARA OUT:
// RETURN  :true success while false failed
// INSTRUCT:
// =============================================================================
bool_t ArpItemDelete(u32 ip)
{
    bool_t      result = false;

    if(Lock_MutexPend(pArpItemMutex,CN_TIMEOUT_FOREVER))
    {
        result = __ArpDeleteItem(ip);

        Lock_MutexPost(pArpItemMutex);
    }
    return result;
}

// =============================================================================
// FUNCTION:use this function to find the mac with the specified address
// PARA  IN:ip,the is an ipv4 address
//          mac, use to storage the mac address
// PARA OUT:
// RETURN  :true find while false not or finding now
// INSTRUCT:
// =============================================================================
bool_t ArpFindMac(u32 ip,u8 *mac)
{
    bool_t      result = false;
    tagArpItem *tmp;
    if(ip == INADDR_BROAD)
    {
        memcpy(mac,CN_MAC_BROAD,CN_MACADDR_LEN);
        result = true;
    }
    else
    {
        if(Lock_MutexPend(pArpItemMutex,CN_TIMEOUT_FOREVER))
        {
            tmp = __ArpMatchItem(ip);
            if(NULL == tmp)
            {
                //which means no item matches, so we should build a item and send the
                //arp request
                tmp =__ArpCreateItem(ip);
                if(NULL != tmp)
                {
                    tmp->pro = CN_ARPITEM_PRO_DYNAMIC;
                    tmp->timeout = CN_ARPITEM_LIFE_TRIG;
                    __ArpSndRequest(ip);
                }
            }
            else
            {
                if(tmp->pro &CN_ARPITEM_PRO_STABLE)
                {
                    memcpy(mac,tmp->mac,CN_MACADDR_LEN);
                    tmp->reffers++;
                    result = true;
                }
                else
                {
                    __ArpSndRequest(ip);  //do an arp request
                }
            }
            Lock_MutexPost(pArpItemMutex);
        }
    }
    return result;
}
// =============================================================================
// FUNCTION:use this function to clean all the arp item
// PARA  IN:
// PARA OUT:
// RETURN  :
// INSTRUCT:
// =============================================================================
bool_t ArpTabClean()
{
    bool_t      result = false;
    tagArpItem *tmp;
    u32 offset;

    if(Lock_MutexPend(pArpItemMutex,CN_TIMEOUT_FOREVER))
    {
        for(offset =0; offset < gArpItemHashLen;offset ++)
        {
            tmp = pArpItemTab[offset];
            while(NULL != tmp)
            {
                pArpItemTab[offset] = tmp->nxt;
                free((void *)tmp);
                tmp = pArpItemTab[offset];
            }
        }

        result = true;
        Lock_MutexPost(pArpItemMutex);
    }

    return result;
}


//tell all the others this rout has existed
bool_t ArpInformAll(tagRout *rout)
{
    bool_t      result = false;
    u32         offset;
    tagArpItem *tmp;
    u32         ip;
    u8         *mac;

    if(NULL != rout)
    {
        //timeout we should process all the timeout arp item
        for(offset =0; offset < gArpItemHashLen;offset ++)
        {
            if(Lock_MutexPend(pArpItemMutex,CN_TIMEOUT_FOREVER))
            {
                tmp = pArpItemTab[offset];
                while(NULL != tmp)
                {
                    //we need do nothing here
                    if(tmp->pro & CN_ARPITEM_PRO_STABLE)
                    {
                        ip = tmp->ip;
                        mac = tmp->mac;
                        __ArpSndResponse(ip,mac,rout);
                    }
                    tmp = tmp->nxt;
                }
                Lock_MutexPost(pArpItemMutex);
            }
        }
        result = true;
    }
    return result;
}
bool_t ArpInform(char *param)
{
    tagRout *rout;
    tagNetDev *dev;

    if(NULL == param)
    {
        return false;
    }
    dev = NetDevGet((const char *)param);
    if(NULL == dev)
    {
        return false;
    }
    rout = dev->routq;
    while(NULL != rout)
    {
        ArpInformAll(rout);

        rout = rout->nxt;
    }
    return true;
}

//debug the arp module
static bool_t ArpShow(char *param)
{
    bool_t      result = false;
    u32         offset = 0;
    u32         num = 0;
    tagArpItem *tmp;
    struct in_addr  addr;
    if(Lock_MutexPend(pArpItemMutex,CN_TIMEOUT_FOREVER))
    {
        printf("ArpItem:\r\n");
        printf("%-6s%-18s%-18s%-8s%-6s%-10s%-10s\n\r",\
                "Item","IpAddress","MacAddress","Type","Life","Refers","Property");
        for(offset =0;offset < gArpItemHashLen;offset++)
        {
            tmp = pArpItemTab[offset];
            while(tmp != NULL)
            {
                memcpy((void *)&addr,(void *)&tmp->ip,sizeof(addr));
                printf("%-4d  %-18s%-18s%-8s%-4d  %-8d  %-8d\n\r",\
                        num,inet_ntoa(addr),Mac2string(tmp->mac),\
                        tmp->pro&CN_ARPITEM_PRO_DYNAMIC?"dynamic":"static",tmp->timeout,tmp->reffers,tmp->pro);
                tmp = tmp->nxt;
                num++;
            }
        }
        Lock_MutexPost(pArpItemMutex);

        printf("ArpTotal:%d :hashlen:%d\n\r",num,gArpItemHashLen);
        printf("ARP STATISTICS:\n\r");
        printf("ARP STATISTICS:SND         :%d\n\r",ArpStatistics.sndnum);
        printf("ARP STATISTICS:SND  ERR    :%d\n\r",ArpStatistics.snderr);
        printf("ARP STATISTICS:RCV         :%d\n\r",ArpStatistics.rcvnum);
        printf("ARP STATISTICS:RCV  REQ    :%d\n\r",ArpStatistics.rcvreq);
        printf("ARP STATISTICS:RCV  RES    :%d\n\r",ArpStatistics.rcvres);
        printf("ARP STATISTICS:RCV  ERR    :%d\n\r",ArpStatistics.rcverr);
        printf("ARP STATISTICS:RCV  CACHED :%d\n\r",ArpStatistics.rcvcached);

        result = true;
    }
    return result;
}
//add arp
static bool_t ArpAdd(char *param)
{
    u8   mac[CN_MACADDR_LEN];
    int  argc = 2;
    char *argv[2];
    bool_t result;
    struct in_addr addr;

    result = false;
    string2arg(&argc,argv,param);
    if(argc == 2)
    {
        if((inet_aton(argv[0],&addr))&&\
           string2Mac(argv[1],mac))
        {
            result = ArpItemCreate(addr.s_addr,mac,CN_ARPITEM_PRO_NONE);
        }
    }
    return result;
}
//delete a specified arp or all
static bool_t ArpDel(char *param)
{
    int  argc = 2;
    char *argv[2];
    bool_t result;
    struct in_addr addr;

    result = false;
    string2arg(&argc,argv,param);
    if(argc == 1)
    {
        if(inet_aton(argv[0],&addr))
        {
            result = ArpItemDelete(addr.s_addr);
        }
    }
    else
    {
        //clean all the arp
        result = ArpTabClean();
    }
    return result;
}

struct ShellCmdTab  gArpDebug[] =
{
    {
        "arp",
        ArpShow,
        "usage:arp",
        "usage:arp",
    },
    {
        "arpadd",
        ArpAdd,
        "usage:arpadd ip  mac",
        "usage:arpadd ip  mac",
    },
    {
        "arpdel",
        ArpDel,
        "usage:arpdel [ip]",
        "usage:arpadd [ip]",
    },
    {
        "arpinform",
        ArpInform,
        "usage:arpinform name",
        "usage:arpinform name",
    },
};
#define CN_ARPDEBUG_NUM  ((sizeof(gArpDebug))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc gArpDebugCmdRsc[CN_ARPDEBUG_NUM];
// =============================================================================
// FUNCTION:this is the arp module init function
// PARA  IN:
// PARA OUT:
// RETURN  :true find while false not
// INSTRUCT:
// =============================================================================
bool_t ArpInit(void)
{
    bool_t result;
    u16   evttID;
    u16   eventID;

    pArpItemMutex = Lock_MutexCreate(NULL);
    if(NULL == pArpItemMutex)
    {
        goto EXIT_ITEMMUTEX;
    }
    pArpEngineSync = Lock_SempCreate(1,0,CN_BLOCK_FIFO,NULL);
    if(NULL == pArpEngineSync)
    {
        goto EXIT_ENGINESYNC;
    }
    pArpItemTab = (tagArpItem **)malloc(gArpItemHashLen*sizeof(void *));
    if(NULL == pArpItemTab)
    {
        goto EXIT_ITEMTAB;
    }
    memset((void *)pArpItemTab,0,gArpItemHashLen*sizeof(void *));
    pArpPkgQH = NULL;

    pArpPkgMutex = Lock_MutexCreate(NULL);
    if(NULL == pArpItemMutex)
    {
        goto EXIT_PKGMUTEX;
    }
    evttID= Djy_EvttRegist(EN_CORRELATIVE, gArpTaskPrior, 0, 1,
            (ptu32_t (*)(void))__ArpProcessor,NULL, gArpTaskStack, "ArpProcessor");
    if(evttID == CN_EVTT_ID_INVALID)
    {
        goto EXIT_EVTTFAILED;
    }
    eventID = Djy_EventPop(evttID, NULL, 0, 0, 0, 0);
    if(eventID == CN_EVENT_ID_INVALID)
    {
        goto EXIT_EVENTFAILED;
    }
    result = Sh_InstallCmd(gArpDebug,gArpDebugCmdRsc,CN_ARPDEBUG_NUM);
    return result;

EXIT_EVENTFAILED:
    Djy_EvttUnregist(evttID);
EXIT_EVTTFAILED:
    Lock_MutexDelete(pArpPkgMutex);
    pArpPkgMutex = NULL;
EXIT_PKGMUTEX:
    free((void *)pArpItemTab);
    pArpItemTab = NULL;
EXIT_ITEMTAB:
    Lock_SempDelete(pArpEngineSync);
    pArpEngineSync = NULL;
EXIT_ENGINESYNC:
    Lock_MutexDelete(pArpItemMutex);
    pArpItemMutex = NULL;
EXIT_ITEMMUTEX:
    result = false;
    return result;
}











