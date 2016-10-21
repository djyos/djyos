/*
 * S1210.C
 *
 *  Created on: 2015年6月2日
 *      Author: zhangqf
 */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <endian.h>

#include "os.h"

#include  <sys/socket.h>

#define CN_CAN_FRAMLEN          13
#define CN_CAN_NUM              4
#define CN_CAN_SOCKETPORTBASE   4001

static int gClientSockTab[CN_CAN_NUM];
static int gServerSockTab[CN_CAN_NUM];
static u32 gSockFrameCount[CN_CAN_NUM];

char *gTaskAcceptName[CN_CAN_NUM] = \
        {"SR1210ACCEPT00","SR1210ACCEPT01","SR1210ACCEPT02","SR1210ACCEPT03"};

char *gTaskRecvName[CN_CAN_NUM] = \
        {"SR1210RECEIV00","SR1210RECEIV01","SR1210RECEIV02","SR1210RECEIV03"};

//do the can_send debug
u8 canSendStat[CN_CAN_NUM];

//do the task delay here
#define CN_TASK_DELAY  (1*mS)
void taskServerDelay(int ms)
{
    Djy_EventDelay(ms*CN_TASK_DELAY);
}

int Eth2Can(int interface,u8 *buf, int len)
{
    Can_Send(interface,buf,len);
    return 0;
}

int Can2Eth(int interface, u8 *buf, int len)
{
    int result = -1;
    if((interface>= 0)&&(interface< CN_CAN_NUM))
    {
        result = send(gClientSockTab[interface],buf,len, 0);
    }

    return result;
}

int  _rcvlenExact(int sock,unsigned char *buf, int len)
{
    int result;
    int offset;
    unsigned char *dst;

    result = len;
    dst = buf;
    while(len > 0)
    {
        offset =recv(sock,(void *)dst,len,0);
        if(offset > 0)
        {
            len = len-offset;
            dst+=offset;
        }
        else if(offset ==-1)
        {
            //continue to rcv
            taskServerDelay(1);
        }
        else
        {
            result = offset;
            break;
        }
    }
    return result;
}

static ptu32_t __taskAccept(void)
{
    int server;
    int client;
    int addrlen;
    struct sockaddr_in addr;
    ptu32_t para;
    int sockopt;
    u16 sport;
    //INITIAL LIZE THE DATA
    Djy_GetEventPara(&para,NULL);

    server = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == server)
    {
        printf("%s:socket:err!\n\r",__FUNCTION__);
        return 0;
    }
    sport = (u16)(para+CN_CAN_SOCKETPORTBASE);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(sport);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(-1==bind(server, (struct sockaddr*)&addr, sizeof(addr)))
    {
        printf("%s:sock:%d:bind:err!\n\r",__FUNCTION__,server);
        closesocket(server);
        return 0;
    }

    if(-1 == listen(server, 2))
    {
        printf("%s:sock:%d:listen:err!\n\r",__FUNCTION__,server);
        closesocket(server);
        return 0;
    }

    //set it to the nonblock here
    if(0 != setsockopt(server,SOL_SOCKET,SO_NOBLOCK,&sockopt,sizeof(sockopt)))
    {
        printf("%s:sock:%d:listen:err!\n\r",__FUNCTION__,server);
        closesocket(server);
        return 0;
    }

    gServerSockTab[para] = server;
    while(1)
    {
        client = accept(server, (struct sockaddr*)&addr, &addrlen);
        if(client != -1)
        {
            //关闭NAGLE
            sockopt = 1;
            if(0 != setsockopt(client, IPPROTO_TCP,TCP_NODELAY,&sockopt,sizeof(sockopt)))
            {
                printf("%s:setsockopt:TCPNODEALY FAILD\n\r",__FUNCTION__,sockopt);
                closesocket(client);
                continue;
            }

            //set it noblock here
            sockopt = 1;
            if(0 != setsockopt(client,SOL_SOCKET,SO_NOBLOCK,&sockopt,sizeof(sockopt)))
            {
                printf("%s:setsockopt:TCP NOBLOCK FAILD\n\r",__FUNCTION__,sockopt);
                closesocket(client);
                continue;
            }
            //close the old one
            if(gClientSockTab[para] != client)
            {
                closesocket(gClientSockTab[para]);
                //OK, UPDATE THE SOCK TABLE
                gClientSockTab[para] = client;
            }
        }

        taskServerDelay(1000);
    }
    closesocket(server);
    printf("%s:TASK END\n\r",__FUNCTION__);

    return 0;
}
static ptu32_t __taskRecv(void)
{
    int client;
    ptu32_t para;
    int recvlen;
    u8  recvbuf[CN_CAN_FRAMLEN];
    //INITIAL LIZE THE DATA
    Djy_GetEventPara(&para,NULL);

    while(1)
    {
        client = gClientSockTab[para];
        if(client > 0)
        {
            recvlen = _rcvlenExact(client,recvbuf,CN_CAN_FRAMLEN);
            if(recvlen > 0)
            {
                gSockFrameCount[para]++;

                //SND IT TO THE CAN PORT
                Eth2Can(para,recvbuf,CN_CAN_FRAMLEN);
            }
            else
            {
                gSockFrameCount[para] = 0;
                closesocket(client);
                if(gClientSockTab[para] == client)
                {
                    gClientSockTab[para] = -1;
                }
            }
        }
        else
        {
            taskServerDelay(1);
        }
    }

    return 0;
}


typedef ptu32_t (*taskFun)(void);
static bool_t __createTask(taskFun func,void *stack,u32 stacksize,char *name,int interface)
{
    bool_t result = false;
    u16  evttID;
    u16  eventID;
    evttID = Djy_EvttRegist(EN_CORRELATIVE, CN_PRIO_RRS, 0, 1,\
           (ptu32_t (*)(void))func,stack, stacksize, name);

    if (evttID != CN_EVTT_ID_INVALID)
    {
        eventID = Djy_EventPop(evttID, NULL, 0, interface, 0, 0);
        if(eventID == CN_EVENT_ID_INVALID)
        {
            Djy_EvttUnregist(evttID);
            printf("%s:Task:%s Interface:%d  Failed\n\r",__FUNCTION__,name,interface);
        }
        else
        {
            printf("%s:Task:%s Interface:%d  success\n\r",__FUNCTION__,name,interface);
            result = true;
        }
    }
    return result;
}


bool_t __ethcanserverstat(char *param)
{
    int i = 0;
    //print all the client
    for(i =0;i < CN_CAN_NUM;i++)
    {
        if(-1 != gServerSockTab[i])
        {
            sockinfo(gServerSockTab[i],NULL);
        }
        else
        {
            printf("%s:interface:%d invalid fd:%d\r\n",__FUNCTION__,i,gClientSockTab[i]);
        }
    }

    return true;
}
bool_t __ethcanclientstat(char *param)
{
    int i = 0;

    //print all the client
    for(i =0;i < CN_CAN_NUM;i++)
    {
        if(-1 != gClientSockTab[i])
        {
            sockinfo(gClientSockTab[i],NULL);
        }
        else
        {
            printf("%s:interface:%d invalid fd:%d\r\n",__FUNCTION__,i,gClientSockTab[i]);
        }
    }


    return true;
}

bool_t __ethcanFrameRcv(char *param)
{
    int i = 0;
    for(i=0;i< CN_CAN_NUM; i++)
    {
        printf("%s:Rcv %d Frams\n\r",__FUNCTION__,gSockFrameCount[i]);
    }
    return true;
}
bool_t __ethcanRcvReset(char *param)
{
    int i = 0;

    for(i=0;i< CN_CAN_NUM; i++)
    {
        gSockFrameCount[i] = 0;
    }
    return true;
}

#include "shell.h"
struct ShellCmdTab  gEthCanDebug[] =
{
    {
        "canserver",
        __ethcanserverstat,
        "usage:show the eth can server socket stat",
        NULL
    },
    {
        "canclient",
        __ethcanclientstat,
        "usage:show the eth can client socket stat",
        NULL
    },
    {
        "canrcv",
        __ethcanFrameRcv,
        "usage:show how many frams recve",
        NULL
    },
    {
        "canrcvrst",
        __ethcanRcvReset,
        "usage:reset the counter",
        NULL
    },
};
#define CN_ETHCAN_DEBUGNUM  ((sizeof(gEthCanDebug))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc gEthCanDebugCmdRsc[CN_ETHCAN_DEBUGNUM];
static bool_t __installEthCanDebug(void)
{
    bool_t result;

    result = Sh_InstallCmd(gEthCanDebug,gEthCanDebugCmdRsc,CN_ETHCAN_DEBUGNUM);

    return result;
}

#define CN_SERVER_STACKSIZE  0x1000
bool_t EthCan_Main(void)
{
    int i =0;
    for(i =0; i< CN_CAN_NUM;i++)
    {
        gServerSockTab[i] = -1;
        gClientSockTab[i] = -1;
        __createTask(__taskAccept,NULL,CN_SERVER_STACKSIZE,gTaskAcceptName[i],i);
        __createTask(__taskRecv,NULL,CN_SERVER_STACKSIZE,gTaskRecvName[i],i);
    }
    return __installEthCanDebug();
}












