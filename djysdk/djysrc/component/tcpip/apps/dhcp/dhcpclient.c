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

#include "../../rout.h"
#include "dhcpLib.h"

typedef struct DhcpTask
{
    struct DhcpTask           *nxt;            //the task lst, for the extend fuction
    u32                        stat;           //the task stat
    u32                        timeout;        //when timeout,do the
    u32                        transID;        //trans action ID
    u8                         mac[CN_MACADDR_LEN]; //the rout mac
    char                       name[CN_TCPIP_NAMELEN]; //dev name
    tagDhcpReplyPara           replypara;      //used to record the reply
}tagDhcpTask;
enum __EN_DHCPCLIENT_STAT
{
    EN_CLIENT_DICOVER,  //which means the task has snd the discover request
    EN_CLIENT_REQUEST,  //which means the task has recieve the offer and snd the request
    EN_CLIENT_STABLE,   //which means the task has recieve the ack of the request from the server
    EN_CLIENT_INFORM,   //which means the task has recieve the ack of the request from the server
};

#define CN_DHCP_TIMEOUT      0x3    //if not reply int the time, then will snd again
static int                   gDhcpClientSock = -1;
static tagDhcpTask          *pgDhcpTaskLst = NULL;
struct MutexLCB             *pDhcpClientSync;
static u32                   gDhcpClientTxID = 0x10000000;
static tagDhcpMsg            gDhcpClientMsg;
//the first time to get the ip from the server is init->discover->request->stable
//then if you want to extend the leas time, then:request,if reive ack, then goto
//the stable, if nack then goto discover and so the whole flow again
//do the client socket int ,after this ,we could read and write the client socket,
//which means we could do the client socket communicate; this func returns the
//socket of the client
static int __initSocket(void)
{
    int result = -1;
    struct sockaddr_in ipportaddr;

    result = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //USE THE DEFAULE UDP PROGRAM
    if(-1 == result)
    {
        closesocket(result);
        printf("%s:err socket\n\r",__FUNCTION__);
        goto ERR_SOCKET;
    }

    ipportaddr.sin_family = AF_INET;
    ipportaddr.sin_port = htons(DHCP_CLIENT_PORT);
    ipportaddr.sin_addr.s_addr = INADDR_ANY;

    if(-1 == bind(result, (struct sockaddr *)&ipportaddr, sizeof(ipportaddr)))
    {
        closesocket(result);
        printf("%s:err bind\n\r",__FUNCTION__);
        goto ERR_SOCKET;
    }

    ipportaddr.sin_family = AF_INET;
    ipportaddr.sin_port = htons(DHCP_SERVER_PORT);
    ipportaddr.sin_addr.s_addr = INADDR_BROAD;
    if(-1 == connect(result,(struct sockaddr *)&ipportaddr,sizeof(ipportaddr)))
    {
        closesocket(result);
        printf("%s:err connect\n\r",__FUNCTION__);
        goto ERR_SOCKET;
    }
    int opt = 1;
    if(0 != setsockopt(result,SOL_SOCKET,SO_NOBLOCK,&opt, sizeof(opt)))
    {
        closesocket(result);
        printf("%s:err setsockopt block\n\r",__FUNCTION__);
        goto ERR_SOCKET;
    }

    opt = 1;
    if(0 != setsockopt(result,SOL_SOCKET,SO_BROADCAST,&opt, sizeof(opt)))
    {
        closesocket(result);
        printf("%s:err setsockopt broad\n\r",__FUNCTION__);
        goto ERR_SOCKET;
    }
    return result;

ERR_SOCKET:
    result = -1;
    return result;
}

static bool_t __cpyReplyMsg(tagDhcpMsg *msg)
{
	u32            txid;
    tagDhcpTask   *tmp;
    bool_t         result = false;

    if(msg->op != DHCP_BOOTREPLY)
    {
    	return result;
    }

    tmp = pgDhcpTaskLst;
    txid = ntohl(msg->xid);
    while(NULL != tmp)
    {
        if(tmp->transID == txid)
        {
        	pasteDhcpReplyMsg(&tmp->replypara,msg);
        	if(tmp->replypara.msgtype == DHCP_OFFER)
        	{
        		printf("%s:offer!\n\r",__FUNCTION__);
        		tmp->stat = EN_CLIENT_REQUEST;
        		tmp->timeout = 0;
        	}
        	else if(tmp->replypara.msgtype == DHCP_ACK)
        	{
        		printf("%s:ack!\n\r",__FUNCTION__);

        		tmp->stat = EN_CLIENT_INFORM;
        		tmp->timeout = tmp->replypara.leasetime;
        		//here we also to update the rout info
        		tagHostAddrV4   hostaddr;
        		hostaddr.ip = tmp->replypara.offerip;
        		hostaddr.gatway = tmp->replypara.router;
        		hostaddr.submask = tmp->replypara.ipmask;
        		hostaddr.dns = tmp->replypara.dns1;
        		hostaddr.broad = (~(hostaddr.submask&hostaddr.ip))|hostaddr.submask;
        		RoutUpdate(tmp->name,EN_IPV_4,&hostaddr);
        		RoutSetDefault(EN_IPV_4,hostaddr.ip);
        	}
        	else if(tmp->replypara.msgtype == DHCP_NAK)
        	{
        		printf("%s:nak!\n\r",__FUNCTION__);

        		tmp->stat = EN_CLIENT_DICOVER;
        		tmp->timeout = 0;
        	}
            break;
        }
        else
        {
            tmp = tmp->nxt;
        }
    }

    return result;
}

static void __dealTask(void)
{
    tagDhcpTask      *task;

    tagDhcpRequestPara  reqpara;

    task = pgDhcpTaskLst;
    while(NULL != task)
    {
        if(task->timeout == 0)
        {
            if(task->stat == EN_CLIENT_DICOVER)
            {
            	memcpy(reqpara.clientmac,task->mac,CN_MACADDR_LEN);
            	reqpara.optype = DHCP_BOOTREQUEST;
            	reqpara.msgtype = DHCP_DISCOVER;
            	reqpara.transaction = task->transID;
            	reqpara.reqip = INADDR_ANY;

            	makeDhcpRequestMsg(&gDhcpClientMsg,&reqpara);
            	send(gDhcpClientSock,(void *)&gDhcpClientMsg,sizeof(gDhcpClientMsg),0);
                task->timeout = CN_DHCP_TIMEOUT;
                printf("%s:discover..\n\r",__FUNCTION__);
            }
            else if(task->stat == EN_CLIENT_REQUEST)
            {
            	memcpy(reqpara.clientmac,task->mac,CN_MACADDR_LEN);
            	reqpara.optype = DHCP_BOOTREQUEST;
            	reqpara.msgtype = DHCP_REQUEST;
            	reqpara.transaction = task->transID;
            	reqpara.reqip = task->replypara.offerip;
            	reqpara.dhcpserver = task->replypara.dhcpserver;

            	makeDhcpRequestMsg(&gDhcpClientMsg,&reqpara);
            	send(gDhcpClientSock,(void *)&gDhcpClientMsg,sizeof(gDhcpClientMsg),0);
                task->timeout = CN_DHCP_TIMEOUT;
                printf("%s:request..\n\r",__FUNCTION__);
            }
            else
            {
            	//maybe do the inform here,but we do it as discover
            	task->stat = EN_CLIENT_DICOVER;
                task->transID = gDhcpClientTxID++;
                task->timeout = 0;
                printf("%s:inform..\n\r",__FUNCTION__);
            }
            memset((void *)&gDhcpClientMsg,0,sizeof(gDhcpClientMsg));
        }
        else
        {
            task->timeout--;
        }
        task = task->nxt;
    }
    return;
}
ptu32_t __DhcpClientMain(void)
{
    int               recvlen;

    gDhcpClientSock = __initSocket();
    if(gDhcpClientSock != -1)
    {
        while(1)
        {
            Lock_MutexPend(pDhcpClientSync, CN_TIMEOUT_FOREVER);
            do{
                recvlen = recv(gDhcpClientSock, (void *)&gDhcpClientMsg, sizeof(gDhcpClientMsg), 0);
                if(recvlen >0)
                {
                    __cpyReplyMsg(&gDhcpClientMsg);
                }
            }while(recvlen >0);
            //check all the task,if any work need to do
            __dealTask();

            Lock_MutexPost(pDhcpClientSync);
            Djy_EventDelay(1000*mS);        //each seconds we will be runing
        }
    }
    else
    {
        printf("%s:DHCP CLIENT SOCKET ERR\n\r",__FUNCTION__);
    }
    return 0;
}


bool_t DhcpAddClientTask(const char *name)
{
    bool_t result = false;
    tagDhcpTask *task;
    tagNetDev   *dev;
	tagHostAddrV4  ipv4addr;

    if(Lock_MutexPend(pDhcpClientSync, CN_TIMEOUT_FOREVER))
    {
    	//we use the static ip we like
    	memset((void *)&ipv4addr,0,sizeof(ipv4addr));
    	if(((dev = NetDevGet(name))!= NULL)&&\
			(RoutCreate(name,EN_IPV_4,(void *)&ipv4addr,CN_ROUT_DHCP)))
        {
            task = malloc(sizeof(tagDhcpTask));
            if(NULL != task)
            {
                memset((void *)task, 0, sizeof(tagDhcpTask));
                task->stat = EN_CLIENT_DICOVER;
                task->timeout = 0;
                memcpy(task->name,name,CN_TCPIP_NAMELEN);
                memcpy(task->mac,NetDevGetMac(dev),CN_MACADDR_LEN);
                task->transID = gDhcpClientTxID++;
                //add it to the lst
                task->nxt = pgDhcpTaskLst;
                pgDhcpTaskLst = task;
                result = true;
            }
        }
        Lock_MutexPost(pDhcpClientSync);
    }
    return result;
}

//this is main dhcp client module
bool_t  ModuleInstall_DhcpClient(ptu32_t para)
{
    bool_t  result = false;
    u16 evttID;
    u16 eventID;

    pDhcpClientSync = Lock_MutexCreate(NULL);
    if(NULL == pDhcpClientSync)
    {
    	printf("%s:dhcpy client sync failed\n\r",__FUNCTION__);
    	goto SYNC_FAILED;
    }
    evttID= Djy_EvttRegist(EN_CORRELATIVE, CN_PRIO_RRS, 0, 1,
            (ptu32_t (*)(void))__DhcpClientMain,NULL, 0x800, "DhcpClient");
    if(evttID == CN_EVTT_ID_INVALID)
    {
    	goto EVTT_FAILED;
    }
    eventID = Djy_EventPop(evttID, NULL, 0, 0, 0, 0);
    if(eventID == CN_EVENT_ID_INVALID)
    {
    	goto EVENT_FAILED;
    }

    result = true;
    return result;

EVENT_FAILED:
	Djy_EvttUnregist(evttID);
	evttID = CN_EVTT_ID_INVALID;
EVTT_FAILED:
	Lock_MutexDelete(pDhcpClientSync);
	pDhcpClientSync = NULL;
SYNC_FAILED:
	return result;
}

//this is the dhcp entry
bool_t ServiceDhcpInit(ptu32_t para)
{
    bool_t result;
    result = ModuleInstall_DhcpClient(0);
    return result;
}





