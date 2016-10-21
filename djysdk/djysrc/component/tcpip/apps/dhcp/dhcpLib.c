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

#include "dhcpLib.h"

void makeDhcpRequestMsg(tagDhcpMsg *msg,tagDhcpRequestPara *para)
{
    char *p;
    u32 data32;
    u16 data16;

    p = (char *)(msg->options + 4);
    memset((void *)msg, 0, sizeof(tagDhcpMsg));
    //fill the dhcp header
    msg->op        =   DHCP_BOOTREQUEST;
    msg->htype     =   ARPHRD_ETHER;
    msg->hlen      =   ETH_ALEN;
    msg->xid       =   htonl(para->transaction);
    msg->secs      =   0;
    msg->flags     =   htons(CN_DHCP_FLAG_BROAD);

    memcpy(msg->chaddr,para->clientmac,ETH_ALEN);
    //the following set the dhcp options
    //set the magic number
    memcpy(msg->options,CN_DHCP_MAGICCOOKIE,4);
    //dhcp msg operate type
    ((tagDhcpOpt *)p)->type = dhcpMessageType;
    ((tagDhcpOpt *)p)->len = 1;
    ((tagDhcpOpt *)p)->data[0] = para->msgtype;
    p += sizeof(tagDhcpOpt) + ((tagDhcpOpt *)p)->len;

    //clientID
    ((tagDhcpOpt *)p)->type = dhcpClientIdentifier;
    ((tagDhcpOpt *)p)->len = 7;
    ((tagDhcpOpt *)p)->data[0] = ARPHRD_ETHER;
    memcpy(&((tagDhcpOpt *)p)->data[1],para->clientmac,ETH_ALEN);
    p += sizeof(tagDhcpOpt) + ((tagDhcpOpt *)p)->len;

    //dhcp host name
    ((tagDhcpOpt *)p)->type = hostName;
    ((tagDhcpOpt *)p)->len = strlen(CN_DHCP_CLIENT_HOST);
    memcpy(&((tagDhcpOpt *)p)->data[0],CN_DHCP_CLIENT_HOST,strlen(CN_DHCP_CLIENT_HOST));
    p += sizeof(tagDhcpOpt) + ((tagDhcpOpt *)p)->len;

    //verdorID
    ((tagDhcpOpt *)p)->type = vendorSpecificInfo;
    ((tagDhcpOpt *)p)->len = strlen(CN_DHCP_CLIENT_VENDOR);
    memcpy(&((tagDhcpOpt *)p)->data[0],CN_DHCP_CLIENT_VENDOR,strlen(CN_DHCP_CLIENT_VENDOR));
    p += sizeof(tagDhcpOpt) + ((tagDhcpOpt *)p)->len;

    //dhcp max msg szie
    ((tagDhcpOpt *)p)->type = dhcpMaxMsgSize;
    ((tagDhcpOpt *)p)->len = 2;
    data16 = htons(CN_MSGMAX_LEN);
    memcpy(&((tagDhcpOpt *)p)->data[0],&data16,2);
    p += sizeof(tagDhcpOpt) + ((tagDhcpOpt *)p)->len;

    //requestIp
    if(para->msgtype == DHCP_REQUEST)
    {
        ((tagDhcpOpt *)p)->type = dhcpRequestedIPaddr;
        ((tagDhcpOpt *)p)->len = 4;
        data32 = para->reqip;
        memcpy(&((tagDhcpOpt *)p)->data[0],&data32,4);
        p += sizeof(tagDhcpOpt) + ((tagDhcpOpt *)p)->len;

        ((tagDhcpOpt *)p)->type = dhcpServerIdentifier;
        ((tagDhcpOpt *)p)->len = 4;
        data32 = para->dhcpserver;
        memcpy(&((tagDhcpOpt *)p)->data[0],&data32,4);
        p += sizeof(tagDhcpOpt) + ((tagDhcpOpt *)p)->len;

    }
    //dhcp parameter request
    ((tagDhcpOpt *)p)->type = dhcpParamRequest;
    ((tagDhcpOpt *)p)->len = 14;
    ((tagDhcpOpt *)p)->data[0] = subnetMask;
    ((tagDhcpOpt *)p)->data[1] = routersOnSubnet;
    ((tagDhcpOpt *)p)->data[2] = dns;
    ((tagDhcpOpt *)p)->data[3] = hostName;
    ((tagDhcpOpt *)p)->data[4] = domainName;
    ((tagDhcpOpt *)p)->data[5] = rootPath;
    ((tagDhcpOpt *)p)->data[6] = defaultIPTTL;
    ((tagDhcpOpt *)p)->data[7] = broadcastAddr;
    ((tagDhcpOpt *)p)->data[8] = performMaskDiscovery;
    ((tagDhcpOpt *)p)->data[9] = performRouterDiscovery;
    ((tagDhcpOpt *)p)->data[10] = nisDomainName;
    ((tagDhcpOpt *)p)->data[11] = nisServers;
    ((tagDhcpOpt *)p)->data[12] = ntpServers;
    ((tagDhcpOpt *)p)->data[13] = staticRoute;
    p += sizeof(tagDhcpOpt) + ((tagDhcpOpt *)p)->len;
//end of the options
    ((tagDhcpOpt *)p)->type = endOption;
}
void showDhcpRequestMsg(tagDhcpRequestPara *para)
{
	printf("DHCPDISREQ:\n\r");
	printf("          :operation type:%d\n\r",para->optype);
	printf("          :message type  :%d\n\r",para->msgtype);
	printf("          :request ipaddr:%s\n\r",inet_ntoa(*(struct in_addr*)&para->reqip));
	printf("          :client mac    :%02X:%02X:%02X:%02X:%02X:%02X\n\r",\
			para->clientmac[0],para->clientmac[1],para->clientmac[2],\
			para->clientmac[3],para->clientmac[4],para->clientmac[5]);
	printf("          :client transID:%d\n\r",para->transaction);
	return;
}

bool_t pasteDhcpReplyMsg(tagDhcpReplyPara *para,tagDhcpMsg *msg)
{
    unsigned char *p;
    tagDhcpOpt *item;
    int lenleft;
    u32 data32;
    u8  data8;
    bool_t result = true;
    //for the static message paste
    para->optype  =  msg->op;
    para->transaction = ntohl(msg->xid);
    para->offerip = msg->yiaddr;
    para->dhcpservernxt = msg->siaddr;
    para->relayagent = msg->giaddr;
    memcpy(para->clientmac,msg->chaddr,CN_MACADDR_LEN);
    //for the option message paste
    p = msg->options;
    lenleft = 312;
    if(0 != memcmp(p,CN_DHCP_MAGICCOOKIE,4))
    {
    	result = false;
    	printf("%s:dhcp magic cookie error\n\r",__FUNCTION__);
    	return result;
    }

    p += 4;
    lenleft -= 4;
    while(lenleft > 0)
    {
        item = (tagDhcpOpt *)p;
        switch(item->type)
        {
            case  dhcpMessageType:
                  data8 = *(u8 *)(item->data);
                  para->msgtype = data8;
                  p += sizeof(tagDhcpOpt) + item->len;
                  lenleft = lenleft -sizeof(tagDhcpOpt)-item->len;
                  break;
            case routersOnSubnet:
                  memcpy(&data32, item->data,4);
                  para->router = data32;
                  p += sizeof(tagDhcpOpt) + item->len;
                  lenleft = lenleft -sizeof(tagDhcpOpt)-item->len;
                  break;
            case subnetMask:
                  memcpy(&data32, item->data,4);
                  para->ipmask = data32;
                  p += sizeof(tagDhcpOpt) + item->len;
                  lenleft = lenleft -sizeof(tagDhcpOpt)-item->len;
                  break;
            case dhcpServerIdentifier:
                  memcpy(&data32, item->data,4);
                  para->dhcpserver= data32;
                  p += sizeof(tagDhcpOpt) + item->len;
                  lenleft = lenleft -sizeof(tagDhcpOpt)-item->len;
                  break;
            case dhcpRenewTime:
                  memcpy(&data32, item->data,4);
                  para->renewtime= ntohl(data32);
                  p += sizeof(tagDhcpOpt) + item->len;
                  lenleft = lenleft -sizeof(tagDhcpOpt)-item->len;
                  break;
            case dhcpRebindTime:
                  memcpy(&data32, item->data,4);
                  para->bindtime= ntohl(data32);
                  p += sizeof(tagDhcpOpt) + item->len;
                  lenleft = lenleft -sizeof(tagDhcpOpt)-item->len;
                  break;
            case dhcpIPaddrLeaseTime:
                  memcpy(&data32, item->data,4);
                  para->leasetime= ntohl(data32);
                  p += sizeof(tagDhcpOpt) + item->len;
                  lenleft = lenleft -sizeof(tagDhcpOpt)-item->len;
                  break;
            case dns:
                  memcpy(&data32, item->data,4);
                  para->dns1= data32;
                  if(item->len > 4)
                  {
                      memcpy(&data32, item->data+4,4);
                      para->dns2= data32;
                  }
                  p += sizeof(tagDhcpOpt) + item->len;
                  lenleft = lenleft -sizeof(tagDhcpOpt)-item->len;
                  break;
            case domainName:
            	  if(item->len >= CN_DHCP_STRLEN)
            	  {
            		  memcpy(para->domainname, item->data,CN_DHCP_STRLEN-1);
            	  }
            	  else
            	  {
            		  memcpy(para->domainname, item->data,item->len);
            	  }
                  p += sizeof(tagDhcpOpt) + item->len;
                  lenleft = lenleft -sizeof(tagDhcpOpt)-item->len;
                  break;

            case endOption:
                  lenleft = 0;
                  break;
            default:
                  p += sizeof(tagDhcpOpt) + item->len;
                  lenleft = lenleft -sizeof(tagDhcpOpt)-item->len;
                  break;
        }
    }

    return result;
}
void showDhcpReplyMsg(tagDhcpReplyPara *para)
{
	printf("DHCPOFFSERACK:\n\r");
	printf("          :operation type:%d\n\r",para->optype);
	printf("          :message   type:%d\n\r",para->msgtype);
	printf("          :transaction id:%d\n\r",para->transaction);
	printf("          :offer ipaddr  :%s\n\r",inet_ntoa(*(struct in_addr*)&para->offerip));
	printf("          :dhcpservernxt :%s\n\r",inet_ntoa(*(struct in_addr*)&para->dhcpservernxt));
	printf("          :relay agent   :%s\n\r",inet_ntoa(*(struct in_addr*)&para->relayagent));
	printf("          :client mac    :%02X:%02X:%02X:%02X:%02X:%02X\n\r",\
			para->clientmac[0],para->clientmac[1],para->clientmac[2],\
			para->clientmac[3],para->clientmac[4],para->clientmac[5]);
	printf("          :renew time    :%d(seconds)\n\r",para->renewtime);
	printf("          :bind time     :%d(seconds)\n\r",para->bindtime);
	printf("          :lease time    :%d(seconds)\n\r",para->leasetime);
	printf("          :dhcp server   :%s\n\r",inet_ntoa(*(struct in_addr*)&para->dhcpserver));
	printf("          :ip mask       :%s\n\r",inet_ntoa(*(struct in_addr*)&para->ipmask));
	printf("          :router        :%s\n\r",inet_ntoa(*(struct in_addr*)&para->router));
	printf("          :domain name   :%s\n\r",para->domainname);
	printf("          :domain name s1:%s\n\r",inet_ntoa(*(struct in_addr*)&para->dns1));
	printf("          :domain name s2:%s\n\r",inet_ntoa(*(struct in_addr*)&para->dns2));
	return;
}

//paset the client request message
bool_t pasteDhcpRequestMsg(tagDhcpRequestPara *para,tagDhcpMsg *msg)
{
    unsigned char *p;
    tagDhcpOpt *item;
    int lenleft;
    u8  data8;
    bool_t result = true;
    //for the static message paste
    para->optype  =  msg->op;
    para->transaction = ntohl(msg->xid);
    memcpy(para->clientmac,msg->chaddr,CN_MACADDR_LEN);
    //for the option message paste
    p = msg->options;
    lenleft = 312;
    if((0 != memcmp(p,CN_DHCP_MAGICCOOKIE,4))||(para->optype != DHCP_BOOTREQUEST))
    {
    	result = false;
    	printf("%s:dhcp magic cookie error or opcode error\n\r",__FUNCTION__);
    	return result;
    }

    p += 4;
    lenleft -= 4;
    while(lenleft > 0)
    {
        item = (tagDhcpOpt *)p;
        switch(item->type)
        {
            case  dhcpMessageType:
                  data8 = *(u8 *)(item->data);
                  para->msgtype = data8;
                  p += sizeof(tagDhcpOpt) + item->len;
                  lenleft = lenleft -sizeof(tagDhcpOpt)-item->len;
                  break;
            case endOption:
                  lenleft = 0;
                  break;
            default:
                  p += sizeof(tagDhcpOpt) + item->len;
                  lenleft = lenleft -sizeof(tagDhcpOpt)-item->len;
                  break;
        }
    }

    return result;
}

void makeDhcpReplyMsg(tagDhcpMsg *msg,tagDhcpReplyPara *para)
{

    char *p;
    u32  data32;

    memset((void *)msg,0,sizeof(tagDhcpMsg));
    //OK, NOW FILL THE STATIC MESSAGE
    //fill the dhcp header
    msg->op        =   DHCP_BOOTREPLY;
    msg->htype     =   ARPHRD_ETHER;
    msg->hlen      =   ETH_ALEN;
    msg->xid       =   htonl(para->transaction);
    msg->secs      =   0;
    msg->flags     =   htons(CN_DHCP_FLAG_BROAD);
    msg->ciaddr    =   para->offerip;
    msg->yiaddr    =   para->offerip;
    msg->siaddr    =   para->dhcpservernxt;
    msg->giaddr    =   para->relayagent;
    memcpy(msg->chaddr,para->clientmac,ETH_ALEN);
    //NOW FILL THE OPTIONNAL MESSAGES
    p = (char *)(msg->options);
    //set the magic number
    memcpy(p,CN_DHCP_MAGICCOOKIE,4);
    p += 4;
    //dhcp msg operate type
    ((tagDhcpOpt *)p)->type = dhcpMessageType;
    ((tagDhcpOpt *)p)->len = 1;
    ((tagDhcpOpt *)p)->data[0] = para->msgtype;
    p += sizeof(tagDhcpOpt) + ((tagDhcpOpt *)p)->len;

    //renew time
    ((tagDhcpOpt *)p)->type = dhcpRenewTime;
    ((tagDhcpOpt *)p)->len = 4;
    data32 = htonl(para->renewtime);
    memcpy(&((tagDhcpOpt *)p)->data[0],&data32,4);
    p += sizeof(tagDhcpOpt) + ((tagDhcpOpt *)p)->len;

    //rebind time
    ((tagDhcpOpt *)p)->type = dhcpRebindTime;
    ((tagDhcpOpt *)p)->len = 4;
    data32 = htonl(para->bindtime);
    memcpy(&((tagDhcpOpt *)p)->data[0],&data32,4);
    p += sizeof(tagDhcpOpt) + ((tagDhcpOpt *)p)->len;

    //lease time
    ((tagDhcpOpt *)p)->type = dhcpIPaddrLeaseTime;
    ((tagDhcpOpt *)p)->len = 4;
    data32 = htonl(para->leasetime);
    memcpy(&((tagDhcpOpt *)p)->data[0],&data32,4);
    p += sizeof(tagDhcpOpt) + ((tagDhcpOpt *)p)->len;

    //SERVER ID
    ((tagDhcpOpt *)p)->type = dhcpServerIdentifier;
    ((tagDhcpOpt *)p)->len = 4;
    memcpy(&((tagDhcpOpt *)p)->data[0],&para->dhcpserver,4);
    p += sizeof(tagDhcpOpt) + ((tagDhcpOpt *)p)->len;

    //SUBNET MASK
    ((tagDhcpOpt *)p)->type = subnetMask;
    ((tagDhcpOpt *)p)->len = 4;
    memcpy(&((tagDhcpOpt *)p)->data[0],&para->ipmask,4);
    p += sizeof(tagDhcpOpt) + ((tagDhcpOpt *)p)->len;

    //vendor info
    ((tagDhcpOpt *)p)->type = vendorSpecificInfo;
    ((tagDhcpOpt *)p)->len = strlen(CN_DHCP_SERVER_VENDOR);
    memcpy(&((tagDhcpOpt *)p)->data[0],CN_DHCP_SERVER_VENDOR,strlen(CN_DHCP_SERVER_VENDOR));
    p += sizeof(tagDhcpOpt) + ((tagDhcpOpt *)p)->len;

    //domain name
    ((tagDhcpOpt *)p)->type = domainName;
    ((tagDhcpOpt *)p)->len = strlen(para->domainname);
    memcpy(&((tagDhcpOpt *)p)->data[0],para->domainname,strlen(para->domainname));
    p += sizeof(tagDhcpOpt) + ((tagDhcpOpt *)p)->len;

    //router
    ((tagDhcpOpt *)p)->type = routersOnSubnet;
    ((tagDhcpOpt *)p)->len = 4;
    memcpy(&((tagDhcpOpt *)p)->data[0],&para->router,4);
    p += sizeof(tagDhcpOpt) + ((tagDhcpOpt *)p)->len;

    //dns
    ((tagDhcpOpt *)p)->type = dns;
    ((tagDhcpOpt *)p)->len = 8;
    memcpy(&((tagDhcpOpt *)p)->data[0],&para->dns1,4);
    memcpy(&((tagDhcpOpt *)p)->data[4],&para->dns2,4);
    p += sizeof(tagDhcpOpt) + ((tagDhcpOpt *)p)->len;
    //end of the options
    ((tagDhcpOpt *)p)->type = endOption;
}



