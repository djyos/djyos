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
#include "tpl.h"
#include "ip.h"
#include "ipV4.h"
#include "tcpipconfig.h"

//some defines for the tcp
//LOCAL MACHINE STAT ENUM
enum _EN_TCPSTATE
{
    EN_TCP_MC_CREAT = 0 ,  //刚被创建的状态，一般而言只有主动创建的才会在此状态
    EN_TCP_MC_SYNRCV,      //服务器端收到了FIN
    EN_TCP_MC_SYNSNT,      //客户端已经发送了FIN信号
    EN_TCP_MC_ESTABLISH,   //稳定链接状态
    EN_TCP_MC_FINWAIT1,    //主动关闭发送了FIN
    EN_TCP_MC_FINWAIT2,    //主动关闭发送了FIN并且收到了ACK
    EN_TCP_MC_CLOSING,     //主动关闭发送了FIN(未收到ACK)，收到对面的FIN
    EN_TCP_MC_TIMEWAIT,    //主动关闭发送FIN并且接收到了FIN，2ML超时
    EN_TCP_MC_CLOSEWAIT,   //被动关闭收到了对面的FIN
    EN_TCP_MC_LASTACK,     //被动关闭发送了FIN
    EN_TCP_MC_2FREE,       //待释放状态
};
//some default defines for the tcp
#define CN_TCP_BUFLENDEFAULT    0x2000      //8KB
#define CN_TCP_BUFTRIGDEFAULT   0           //TRIGGER LEVEL DEFAULT
#define CN_TCP_MSSDEFAULT       1460        //MSS DEFAULT
#define CN_TCP_LISTENDEFAULT    5           //LISTEN BAKLOG DEFAULT
#define CN_TCP_RCMBLENDEFAULT   0x10        //most allowing num frame in the recombine list
#define CN_TCP_ACCEPTMAX        0x10000

//define for the channel stat
//rcv fin makes krcv 0
//snt fin makes ksnd 0
//shutdown_rd makes krcv and arcv 0
//shutdown_wr makes asnd 0
//close makes the ksnd krcv 0 arcv 0 asnd 0
#define CN_TCP_CHANNEL_STATASND    (1<<0)  //APP COULD SND DATA
#define CN_TCP_CHANNEL_STATARCV    (1<<1)  //APP COULD NOT SND DATA
#define CN_TCP_CHANNEL_STATKSND    (1<<2)  //STACK COULD SND DATA
#define CN_TCP_CHANNEL_STATKRCV    (1<<3)  //STAK COULD RCV DATA
#define CN_TCP_CHANNEL_STATCONGEST (1<<4)  //the rcv window is full or channel is bad

//define for the tcp timer
#define CN_TCP_TICK_TIME                   (100*mS) //Units:Micro Seconds
#define CN_TCP_TICK_2ML                    (10)     //unit:tcp tick
#define CN_TCP_TICK_KEEPALIVE              (180*10) //unit:tcp tick
#define CN_TCP_TICK_PERSIST                (10)     //unit:tcp tick
#define CN_TCP_TICK_ACK                    (5)      //Unit:package number
#define CN_TCP_TIMER_SYN                   (1<<0)   //SYNC TIMER
#define CN_TCP_TIMER_2MSL                  (1<<1)   //2ML TIMER control bit
#define CN_TCP_TIMER_KEEPALIVE             (1<<2)   //KEEP TIMER control
#define CN_TCP_TIMER_PERSIST               (1<<3)   //PERSIST TIMER control bit
#define CN_TCP_TIMER_CORK                  (1<<4)   //CORK TIMER control bit
#define CN_TCP_TIMER_LINGER                (1<<5)   //LINGER TIMER START
#define CN_TCP_TIMER_RESEND                (1<<6)   //RESEND TIMER
#define CN_TCP_TIMER_FIN                   (1<<7)   //FIN TIMER
#define CN_TCP_TIMER_NONE                  (0)      //NO TIMER IS START
//define for the rtt
#define CN_TCP_RTOMAX                      150    //unit:tcp tick
#define CN_TCP_RTOMIN                      2      //unit:tcp tick
#define CN_TCP_SA_INIT                     0
#define CN_TCP_SD_INIT                     4      //unit:tcp tick
#define CN_TCP_FASTRETRANS                 3
#define CN_TCP_CWND_LIMIT                  60000
#define CN_TCP_RSNDMAX                     8     //RESND TIMES MOST
//tcp flags
#define CN_TCP_FLAG_FIN  (1<<0)
#define CN_TCP_FLAG_SYN  (1<<1)
#define CN_TCP_FLAG_RST  (1<<2)
#define CN_TCP_FLAG_PSH  (1<<3)
#define CN_TCP_FLAG_ACK  (1<<4)
#define CN_TCP_FLAG_URG  (1<<5)
#define CN_TCP_FLAG_MSK  0x3f
#pragma pack(1)
typedef struct TcpHdr
{
    u16 portsrc;     //source port
    u16 portdst;     //dstination port
    u32 seqno;       //sequence number
    u32 ackno;       //acknowledge number
    u8  hdroff;
    u8  flags;       //some flags like sync and ack
    u16 window;      //the remote window size
    u16 chksum;      //package checksum
    u16 urgencyp;    //urgency pointer
    u8  opt[0];      //optional
}tagTcpHdr;
//this structure is used in the option to set the windowscale
typedef struct WindowScale
{
    u8 resvered;
    u8 kind;
    u8 len;
    u8 shiftbits;
}tagWindowScale;
//this structure is used in the option to set the mss
typedef struct Mss
{
    u8  kind;
    u8  len;
    u16 mss;
}tagMss;
//we use the fixed mss and window scale in the syn handshake
typedef struct SynOption
{
    tagMss mss;
    tagWindowScale winscale;
}tagSynOption;
#pragma pack()
//receive buffer
typedef struct
{
    struct SemaphoreLCB      *bufsync;     //the buffer sync member
    struct SemaphoreLCB       bufsyncmem;  //we use this for the buffer sync memory
    u32                       timeout;     //if block, the block time
    int                       buflen;      //the valid data length in the buffer
    int                       buflenlimit; //the buffer length
    int                       trigerlevel;   //the buffer trigerlevel
    tagNetPkg                *ph;          //the buffer package head
    tagNetPkg                *pt;          //the buffer package tail
    u32                       rcvnxt;      //this is we receive next number
}tagRecvBuf;
//send buffer
typedef struct
{
    struct SemaphoreLCB      *bufsync;     //if the buflenleft is more than the triggerlevel, then will active
    struct SemaphoreLCB       bufsyncmem;  //we use this for the buffer sync mem
    u32                       timeout;     //if block, the block time
    u32                       rtotick;     //used to count how many ticks has gone since the last data send
    u8                       *buf;         //the tcp send buffer, use the malloc to get
    int                       buflenlimit; //the buffer length
    u32                       unackno;     //which means the unack sequence
    int                       unackoff;    //which means off the buf to ack
    int                       unacklen;     //which means the length between the uack and sndnxt;
    u32                       sndnxtno;    //which means the sequence no to send
    int                       sndnxtoff;   //which means off the buf to send
    int                       dataoff;     //which means off the buf,and buf from dataoff to unackoff could be write
    int                       datalen;     //which means the unsend data length
    int                       buflenleft;  //the free buffer len,which means how many data you could use
    int                       trigerlevel; //the buffer trigerlevel,if the bufleftlen is less than this ,then will active the sync
    									   //the default is the size of the buffer
    u8                        dupacktims;  //when receive the same ack,add it,when over,do the resend
}tagSendBuf;  //this buf used for the tcp send
//each client has an CCB for the tcp state control
typedef struct CCB
{
	struct CCB               *nxt;          //used for the list
    tagSocket                *server;       //if this is an accept one
    u16                       machinestat;  //the machine stat of the tcb
    u16                       channelstat;  //the stat of the channel,which mean we could recv or send
    tagRecvBuf                rbuf;         //rcv buffer
    tagSendBuf                sbuf;         //sendbuffer
    tagNetPkg                *pkgrecomblst; //the package recomb queue
    u16                       pkgrecomblen; //the package recomb queue length
    //the send window member
    u16                       mss;         //you could send the most data one time
    u8                        sndwndscale; //the remote window scale,update by the handshake
    u32                       cwnd;        //the conggest avoiding window
    u32                       ssthresh;    //slow thresh,default 65535
    u32                       sndwnd;      //the remote receive window,update by the receive frame
    //round trip time measure
    /* RTT(round trip time)相关变量 */
    /*
    Err = M-A           //A是RTT平均值，M是实际测量值，Err是误差
    A←A + gErr         //用误差更新平均值
    D←D + h( | Err |-D) //D是均值偏差，用误差更新均值偏差
    RTO = A + 4D        //计算得到RTO (重传超时时间)  (初始化时RTO = A + 2D  )
    g=0.125 ; h=0.25
    */
    s16 sa, sd;                /* 用于计算RTO：sa = 8*A；sv = 4*D */
    s16 rto;                   /* RTO */
    //tcp timer
    u8                         timerctrl;
    u16                        mltimer;     //(unit:tcp tick,used for 2msl wait)
    u16                        lingertimer; //(unit:tcp tick,used for linger close)
    u16                        keeptimer;   //(unit:tcp tick,used for keep alive)
    u16                        persistimer; //(unit:tcp tick,used for probe window)
    u16                        corktimer;   //(unit:tcp tick,used for cork timeout)
    u8                         resndtimes;  //when resend,add it;when ack the data then clear it
    u8                         resndtimer;  //when resend time is zero,do the resend
    u8                         acktimes;    //if acktimes overrun the limit, then will do fast ack
}tagCCB;
//each server has an SCB
typedef struct SCB
{
	struct SCB                *nxt;                 //used for the list
	int                        backlog;             //which limit the pending num
    int                        pendnum;             //which means how much still in pending
    u32                        accepttime;          //block time for the accept
    tagSocket                 *clst;                //all the client including the pending stat
    struct SemaphoreLCB       *acceptsemp;          //if block, then wait this num
    struct SemaphoreLCB        acceptsyncmem;
}tagSCB;  //tcp server control block
//we use this structure to statistics the tcp state
typedef struct
{
	u32  ccbnum;
	u32  ccbfreenum;
	u32  scbnum;
	u32  scbfreenum;
}tagTcpStatistics;
//define for the register in the tpl layer
static tagTlayerProto         gTcpProto;
//used for the connection to snd syn (server or client)  tcp option
static tagSynOption           sgSynOptDefault;   //when snd syn,use this const option
static u32                    gTcpTicks = 0;     //used to count the tcp ticks
//we use the hash table to search the socket
typedef struct
{
    u32                          tablen;     //how long the hash tab is
    struct MutexLCB             *tabsync;    //used to peotect the hash tab
    struct MutexLCB              tabsyncmem; //supply for the sync mem
    tagSocket                   *array[0];   //this is the hash tab
}tagTcpHashTab;
static tagTcpHashTab   *pTcpHashTab = NULL;
// =============================================================================
// FUNCTION：this function is used to initialize the tcp hash tab
// PARA  IN：len ,this parameter limits the hashtab length
// PARA OUT：
// RETURN  ：
// INSTRUCT:
// =============================================================================
static bool_t __hashTabInit(u32 len)
{
    bool_t result = false;
    pTcpHashTab = malloc(sizeof(tagTcpHashTab) + len *sizeof(tagSocket *));
    if(NULL == pTcpHashTab)
    {
        goto ERR_ARRAYMEM;
    }
    memset((void *)pTcpHashTab,0,sizeof(tagTcpHashTab) + len *sizeof(tagSocket *));

    pTcpHashTab->tabsync = Lock_MutexCreate_s(&pTcpHashTab->tabsyncmem,NULL);
    if(NULL == pTcpHashTab->tabsync)
    {
        goto ERR_SYNC;
    }

    pTcpHashTab->tablen = len;
    result = true;
    return result;

ERR_SYNC:
    free(pTcpHashTab);
    pTcpHashTab = NULL;
ERR_ARRAYMEM:
    return result;
}
// =============================================================================
// FUNCTION：this function is used to find an socket in the hashtab
// PARA  IN：the specified ip and port determines the item together
// PARA OUT：
// RETURN  ：the socket find else NULL(no one mathes)
// INSTRUCT:send an mail to the mail box, which to be dealt by the ip engine
// =============================================================================
static tagSocket *__hashSocketSearch(u32 iplocal, u16 portlocal,u32 ipremote,u16 portremote)
{
    tagSocket *result = NULL;

    tagSocket *tmp;
    u32 hashKey;

    hashKey = iplocal+portlocal + ipremote +portremote;
    hashKey = hashKey%pTcpHashTab->tablen;
    tmp = pTcpHashTab->array[hashKey];
    while((NULL != tmp))
    {
        if((iplocal == tmp->element.v4.iplocal)&&(portlocal == tmp->element.v4.portlocal)&&\
           (ipremote == tmp->element.v4.ipremote)&&(portremote == tmp->element.v4.portremote))
        {
            result = tmp;
            break;
        }
        else
        {
            tmp = tmp->nxt;
        }
    }

    return result;
}

//this function is only used for the bind function
//when we bind a address,we must make sure than if any socket with the same address
//if any socket find with the same address, then bind failed
static tagSocket *__hashSocketLocalSearch(u32 iplocal, u16 portlocal)
{
    tagSocket *result = NULL;

    int i = 0;
    tagSocket *tmp;

    for(i =0; i < pTcpHashTab->tablen;i++ )
    {
        tmp = pTcpHashTab->array[i];
        while((NULL != tmp))
        {
            if((iplocal == tmp->element.v4.iplocal)&&(portlocal == tmp->element.v4.portlocal))
            {
                result = tmp;
                break;
            }
            else
            {
                tmp = tmp->nxt;
            }
        }
        if(NULL != tmp)
        {
            break;
        }
    }

    return result;
}
// =============================================================================
// FUNCTION:this function is used to create an hash item
// PARA  IN:ip and port are the only hash property we need
// PARA OUT：
// RETURN  :the hash item we create
// INSTRUCT:add the create item to the last
// =============================================================================
static bool_t __hashSocketAdd(tagSocket *sock)
{
    tagSocket  *tmp;
    u32         hashKey;

    hashKey = sock->element.v4.iplocal+sock->element.v4.portlocal+\
              sock->element.v4.ipremote + sock->element.v4.portremote;
    hashKey = hashKey%pTcpHashTab->tablen;
    tmp = pTcpHashTab->array[hashKey];
    if(NULL == tmp)
    {
        pTcpHashTab->array[hashKey] = sock;
        sock->nxt = NULL;
    }
    else
    {
        while(NULL != tmp->nxt)
        {
            tmp = tmp->nxt;
        }
        tmp->nxt = sock;
        sock->nxt = NULL;
    }
    return true;
}

// =============================================================================
// FUNCTION:this function is used to free the item created
// PARA  IN:ip and port are the only hash property we need
// PARA OUT：
// RETURN  :the hash item we create
// INSTRUCT:add the create item to the last
// =============================================================================
static bool_t __hashSocketRemove(tagSocket *sock)
{

    tagSocket  *tmp;
    u32         hashKey;

    hashKey = sock->element.v4.iplocal+sock->element.v4.portlocal+\
              sock->element.v4.ipremote + sock->element.v4.portremote;
    hashKey = hashKey%pTcpHashTab->tablen;
    tmp = pTcpHashTab->array[hashKey];
    if(sock == tmp)
    {
        pTcpHashTab->array[hashKey] = sock->nxt;
        sock->nxt = NULL;
    }
    else
    {
        while((NULL != tmp)&&(sock != tmp->nxt))
        {
            tmp = tmp->nxt;
        }
        if(NULL != tmp)
        {
            tmp->nxt = sock->nxt;
            sock->nxt = NULL;
        }
    }
    return true;
}
//this function used to compute a send sequence number;make sure you could not guess
//the sequence number when handshake
static u32 __computeIsn(void)
{
    #define CN_TCP_ISNPRE_MS    (250)
    #define CN_ISN_PER_CONNECT  (64000)
    static u16 sIsnCnt = 0;
    u32 result;

    result = (u32)((sIsnCnt++)*CN_ISN_PER_CONNECT +
            DjyGetSysTime()*mS*CN_TCP_ISNPRE_MS);
    return result;
}

static struct MutexLCB *pCBSync = NULL;      //use this to protect the
static struct MutexLCB  gCBSyncMem;          //this is for the sync memory
static tagCCB          *pCCBFreeList = NULL; //this is used for the free ccb queue
static tagSCB          *pSCBFreeList = NULL; //this is used for the free scb queue
//this function is used to initialize the ccb and scb module
//malloc the mem from the configuration and create the sync
//after the initialize, we malloc CB from the free queue
static bool_t  __initCB(int ccbnum, int scbnum)
{
    bool_t result = false;
    int i = 0;

    pCBSync = Lock_MutexCreate_s(&gCBSyncMem,NULL);

    //do the ccb initialize
    pCCBFreeList = malloc(ccbnum *sizeof(tagCCB));
    if((ccbnum > 0)&&(NULL == pCCBFreeList))
    {
    	goto CCB_MEM;
    }
    //do CCB initialize
    if(ccbnum > 1)
    {
        //do the initialize
        for(i=0;i <(ccbnum -1);i++)
        {
            pCCBFreeList[i].nxt = &pCCBFreeList[i +1];
        }
    }
    //do the scb initialize
    pSCBFreeList = malloc(scbnum *sizeof(tagSCB));
    if((scbnum > 0)&&(NULL == pSCBFreeList))
    {
    	goto SCB_MEM;
    }
    //do CCB initialize
    if(scbnum > 1)
    {
        //do the initialize
        for(i=0;i <(scbnum -1);i++)
        {
            pSCBFreeList[i].nxt = &pSCBFreeList[i +1];
        }
    }
    result = true;
    return result;

SCB_MEM:
	free((void *)pCCBFreeList);
	pCCBFreeList = NULL;
CCB_MEM:
 	Lock_MutexDelete_s(pCBSync);
 	pCBSync = NULL;
 	result = false;
    return result;
}
//malloc a ccb
static tagCCB  *mallocccb(void)
{
    tagCCB           *result = NULL;
    if(Lock_MutexPend(pCBSync,CN_TIMEOUT_FOREVER))
    {
        if(NULL != pCCBFreeList)
        {
            result = pCCBFreeList;
            pCCBFreeList = result->nxt;
        }

    	Lock_MutexPost(pCBSync);
    }
    return result;
}
//free a ccb
static bool_t  freeccb(tagCCB  *ccb)
{
    if(Lock_MutexPend(pCBSync,CN_TIMEOUT_FOREVER))
    {
        ccb->nxt = pCCBFreeList;
        pCCBFreeList = ccb;

        Lock_MutexPost(pCBSync);
    }
    return true;
}
//malloc a scb
static tagSCB  *mallocscb(void)
{
    tagSCB           *result = NULL;

    if(Lock_MutexPend(pCBSync,CN_TIMEOUT_FOREVER))
    {
        if(NULL != pSCBFreeList)
        {
            result = pSCBFreeList;
            pSCBFreeList = result->nxt;
        }
    	Lock_MutexPost(pCBSync);
    }
    return result;
}
//free a scb
static bool_t  freescb(tagSCB  *scb)
{
    if(Lock_MutexPend(pCBSync,CN_TIMEOUT_FOREVER))
    {
        scb->nxt = pSCBFreeList;
        pSCBFreeList = scb;

        Lock_MutexPost(pCBSync);
    }
    return true;
}

// =============================================================================
// FUNCTION:this function to malloc an ccb and init it
// PARA  IN:
// PARA OUT：
// RETURN  :
// INSTRUCT:
// =============================================================================
//malloc a ccb and init the member in it
static tagCCB  *__CreateCCB(void)
{
	tagCCB *result;

	result =  mallocccb();
    if(NULL == result)
    {
        goto EXIT_CCBMEM;
    }
    memset((void *)result,0, sizeof(tagCCB));
    result->sbuf.bufsync = Lock_SempCreate_s(&result->sbuf.bufsyncmem,1,1,CN_BLOCK_FIFO,NULL);
    result->rbuf.bufsync = Lock_SempCreate_s(&result->rbuf.bufsyncmem,1,0,CN_BLOCK_FIFO,NULL);
    if((NULL == result->sbuf.bufsync)||(NULL == result->rbuf.bufsync))
    {
        goto EXIT_CCBSEMP;
    }

    //malloc the send buffer  and init it
    result->sbuf.buf = malloc(gTcpSndBufSizeDefault);
    if(NULL == result->sbuf.buf)
    {
    	goto EXIT_CCBSBUF;
    }
    result->sbuf.buflenleft =  gTcpSndBufSizeDefault;
    result->sbuf.buflenlimit = gTcpSndBufSizeDefault;
    result->sbuf.trigerlevel = 0;
    result->sbuf.timeout     = CN_TIMEOUT_FOREVER;
    result->sbuf.dataoff     = 0;
    result->sbuf.datalen     = 0;
    result->sbuf.unackoff    = 0;
    result->sbuf.unackno     = __computeIsn();
    result->sbuf.unacklen     = 0;
    result->sbuf.sndnxtoff   = 0;
    result->sbuf.sndnxtno    = result->sbuf.unackno;
    result->sbuf.dupacktims  = 0;
    result->sbuf.rtotick     = gTcpTicks;

    //init the receive buffer
    result->rbuf.buflen       = 0;
    result->rbuf.buflenlimit  = gTcpRcvBufSizeDefault;
    result->rbuf.timeout      = CN_TIMEOUT_FOREVER;
    result->rbuf.trigerlevel    = 0;
    result->rbuf.rcvnxt       = 0;
    result->rbuf.ph = NULL;
    result->rbuf.pt = NULL;

    result->pkgrecomblst = NULL;
    result->pkgrecomblen = 0;
    //set the local mss to default
    //the window initialize
    result->ssthresh    = 65535;
    result->sndwnd      = 0;
    result->sndwndscale = 0;
    result->mss         = gTcpMssDefault;
    //this the send round trip initialize
    result->sa = CN_TCP_SA_INIT;
    result->sd = CN_TCP_SD_INIT;
    result->rto = result->sa + result->sd*2; //rto init:rto = A+2D
    result->cwnd = result->mss*10;

    //do the timer init
    result->timerctrl   = CN_TCP_TIMER_NONE;
    result->acktimes    = 0;
    result->keeptimer   = 0;
    result->persistimer = 0;
    result->mltimer     = 0;
    //init the stat
    result->channelstat =0;
    result->machinestat = EN_TCP_MC_CREAT;

    return result;

EXIT_CCBSBUF:
EXIT_CCBSEMP:
	Lock_SempDelete_s(result->rbuf.bufsync);
	Lock_SempDelete_s(result->sbuf.bufsync);
    freeccb(result);
    result = NULL;
EXIT_CCBMEM:
    return result;
}
//reset the ccb to the specified stat, such as the CREAT or 2FREE
static void  __ResetCCB(tagCCB *ccb,u16 machinestat)
{
    //init the ccb member
    //set the snd and receive buf limit to default
    Lock_SempPost(ccb->sbuf.bufsync);
    //reset the receive buf
    ccb->rbuf.buflen       = 0;
    ccb->rbuf.buflenlimit  = CN_TCP_BUFLENDEFAULT;
    ccb->rbuf.timeout      = CN_TIMEOUT_FOREVER;
    ccb->rbuf.trigerlevel    = 0;
    PkgTryFreeQ(ccb->rbuf.ph);
    ccb->rbuf.ph = NULL;
    ccb->rbuf.pt = NULL;
    Lock_SempPost(ccb->rbuf.bufsync);

    //reset the recomb queue
    PkgTryFreeQ(ccb->pkgrecomblst);
    ccb->pkgrecomblst = NULL;
    ccb->pkgrecomblen = 0;
    //init the stat
    ccb->channelstat =0;
    ccb->machinestat = machinestat;
    //do the timer init
    ccb->timerctrl   = CN_TCP_TIMER_NONE;
    ccb->acktimes    = 0;
    ccb->keeptimer   = 0;
    ccb->persistimer = 0;
    ccb->mltimer     = 0;
}
//use this function to free the ccb
static bool_t __DeleCCB(tagCCB *ccb)
{
    //free all the pkg to snd
	free(ccb->sbuf.buf);
	//free all the pkg to rcv
    PkgTryFreeQ(ccb->rbuf.ph);
    //free all the pkg to recomb
    PkgTryFreeQ(ccb->pkgrecomblst);
    //del the semp for the buf
    Lock_SempDelete_s(ccb->rbuf.bufsync);
    Lock_SempDelete_s(ccb->sbuf.bufsync);

    freeccb(ccb);
    return true;
}

//use this function to create a scb and init it
static tagSCB* __CreateScb(void)
{
	tagSCB *result;

	result = mallocscb();
	if(NULL == result)
	{
		goto SCB_MEM;
	}
    result->acceptsemp = Lock_SempCreate_s(&result->acceptsyncmem,CN_TCP_ACCEPTMAX,0,CN_BLOCK_FIFO,NULL);
    if(NULL == result->acceptsemp)
    {
    	goto SCB_SYNC;
    }
    result->backlog     =  CN_TCP_LISTENDEFAULT;
    result->clst        =  NULL;
    result->pendnum     =  0;
    result->accepttime  =  CN_TIMEOUT_FOREVER;
    return result;
SCB_SYNC:
	freescb(result);
	result = NULL;
SCB_MEM:
    return  result;
}
//use this function to delete an scb and free it
static bool_t __DeleteScb(tagSCB* scb)
{
    Lock_SempDelete_s(scb->acceptsemp);
    freescb(scb);
    return true;
}
static bool_t __ReseSCB(tagSCB* scb)
{
    Lock_SempPost(scb->acceptsemp);
    scb->backlog =  CN_TCP_LISTENDEFAULT;
    scb->clst    =  NULL;
    scb->pendnum =  0;
    scb->accepttime  =  CN_TIMEOUT_FOREVER;
    return true;
}

// =============================================================================
// FUNCTION:this function used to make a tcp terminal
// PARA  IN:the parameters has the same meaning as socket
// PARA OUT：
// RETURN  :
// INSTRUCT:
// =============================================================================
static u32 gPortEngineTcp = 1024;//usually, the dynamic port is more than 1024
static tagSocket * __socket(int family, int type, int protocal)
{
    tagSocket    *sock,*tmp;
    u16           findtime = 0;
    if(AF_INET == family)
    {
        sock = SocketMalloc();
        if(NULL != sock)
        {
            if(Lock_MutexPend(pTcpHashTab->tabsync, CN_TIMEOUT_FOREVER))
            {
                //ok, find any port could use
                do
                {
                    tmp = __hashSocketLocalSearch(INADDR_ANY,gPortEngineTcp);
                    if(NULL != tmp)
                    {
                        gPortEngineTcp ++;
                    }
                    findtime++;
                }while((tmp != NULL)&&(findtime < 65535));//we find until get the port,if over the failed
                //find over now, check if we go the port
                if(NULL == tmp)
                {
                    sock->af_inet = AF_INET;
                    sock->element.v4.iplocal = INADDR_ANY;
                    sock->element.v4.portlocal = gPortEngineTcp ++;
                    //malloc an cb for the socket
                    sock->cb = __CreateCCB();
                    if(NULL == sock->cb)
                    {
                        SocketFree(sock);
                        sock = NULL;
                    }
                    else
                    {
                        //initialize the sock add the sock to the hash tab
                        sock->sockstat |=CN_SOCKET_OPEN|CN_SOCKET_CLIENT|CN_SOCKET_PROBLOCK|CN_SOCKET_PRONAGLE;
                        sock->proto = &gTcpProto;
                        __hashSocketAdd(sock);
                    }
                }
                else
                {
                	//no port for you,you must do some kidding
                }
                Lock_MutexPost(pTcpHashTab->tabsync);
            }//end if for the lock pend
        }//end if NULL != sock
    }//end if AF_INET == family
    return sock;
}
// =============================================================================
// FUNCTION:this function used to bind a address for the socket
// PARA  IN:the parameters has the same meaning as bind
// PARA OUT：
// RETURN  :0 success while -1 failed
// INSTRUCT:
// =============================================================================
static int __bind(tagSocket *sock,struct sockaddr *addr, int addrlen)
{
    int  result;
    u32 ip;
    u16 port;
    struct sockaddr_in *sockaddrin;
    tagSocket  *tmp;

    result = -1;
    if((NULL == addr)||(addrlen != sizeof(struct sockaddr))||\
       (addr->sa_family != AF_INET))
    {
        return result;
    }
    if(Lock_MutexPend(pTcpHashTab->tabsync, CN_TIMEOUT_FOREVER))
    {
        if(Lock_MutexPend(sock->sync,CN_TIMEOUT_FOREVER))
        {
            if((0 == (sock->sockstat&CN_SOCKET_BIND))&&\
               (0 == (CN_SOCKET_CLOSE&sock->sockstat)))  //NOT BIND YET
            {
                sockaddrin = (struct sockaddr_in *)addr;
                ip = sockaddrin->sin_addr.s_addr;
                port = sockaddrin->sin_port;
                if(RoutHostIp(EN_IPV_4,ip))    //first it must be an local ip
                {
                    tmp = __hashSocketLocalSearch(ip,port);
                }
                if(NULL != tmp)
                {
                    if(tmp == sock) //same socket, do noting
                    {
                        sock->sockstat |= CN_SOCKET_BIND;
                        result = 0;
                    }
                    else
                    {
                    	//some one has the same ip and port ,so bind failed
                    	result = -1;
                    }
                }
                else //no one matches, so reinsert the socket
                {
                    __hashSocketRemove(sock);
                    sock->element.v4.iplocal = sockaddrin->sin_addr.s_addr;
                    sock->element.v4.portlocal = sockaddrin->sin_port;
                    sock->sockstat |= CN_SOCKET_BIND;
                    __hashSocketAdd(sock);
                    result = 0;
                }
            }
            Lock_MutexPost(sock->sync);
        }
        Lock_MutexPost(pTcpHashTab->tabsync);
    }
    return  result;
}
// =============================================================================
// FUNCTION:this function used to make the socket to listen
// PARA  IN:the parameters has the same meaning as listen
// PARA OUT：
// RETURN  :0 success while -1 failed
// INSTRUCT:this function to make the socket to become an server terminal
// =============================================================================
static int __listen(tagSocket *sock, int backlog)
{
    int  result;
    tagSCB  *scb;

    result = -1;
    if(Lock_MutexPend(sock->sync, CN_TIMEOUT_FOREVER))
    {
        if((CN_SOCKET_BIND &sock->sockstat)&&\
           (0 == (CN_SOCKET_CONNECT&sock->sockstat))&&\
           (0 == (CN_SOCKET_CLOSE&sock->sockstat))&&\
           (0==(CN_SOCKET_LISTEN&sock->sockstat)))
        {
            scb = __CreateScb();
            if(NULL != scb)
            {
                //may be we should cpy some options from the CCB,which set before listen
                __DeleCCB(sock->cb);
                scb->accepttime = ((tagCCB *)(sock->cb))->rbuf.timeout;
                sock->cb = scb;
                scb->backlog = backlog;
                sock->sockstat&=(~CN_SOCKET_CLIENT);
                sock->sockstat |=CN_SOCKET_LISTEN;

                result = 0;
            }
        }
        Lock_MutexPost(sock->sync);
    }
    return  result;
}

//find an new client which is stable in the scb client queue
static tagSocket *__acceptclient(tagSCB *scb)
{
    tagSocket    *result;
    tagSocket    *client;
    tagSocket    *pre;
    tagCCB       *ccb;

    result = NULL;
    client = scb->clst;
    pre = client;
    while(NULL != client)  //find the established and unopend socket
    {
        ccb = (tagCCB*)client->cb;
        if(EN_TCP_MC_ESTABLISH== ccb->machinestat)
        {
            ccb->channelstat|=CN_TCP_CHANNEL_STATARCV|CN_TCP_CHANNEL_STATASND;
            scb->pendnum--;
            //remove it from the scb client
            if(scb->clst == client)
            {
            	scb->clst = client->nxt;
            }
            else
            {
            	pre->nxt = client->nxt;
            }
        	client->nxt = NULL;
            result = client;
            break;
        }
        else
        {
            client = client->nxt;
            pre = client;
        }
    }
    return result;
}

// =============================================================================
// FUNCTION:this function used to accept client connection from the server
// PARA  IN:the parameters has the same meaning as accept
// PARA OUT:
// RETURN  :
// INSTRUCT:if any ternimal hung on the server is established, then will return
// =============================================================================
static tagSocket *__accept(tagSocket *sock, struct sockaddr *addr, int *addrlen)
{
    tagSocket           *result;
    tagSCB             *scb;
    struct sockaddr_in  *addrin;
    u32                  waittime;

    result = NULL;

    //first we will find if any client is ready
    if((0 ==(CN_SOCKET_CLIENT&sock->sockstat))&&\
       (sock->sockstat &CN_SOCKET_LISTEN)&&\
       Lock_MutexPend(sock->sync, CN_TIMEOUT_FOREVER))
    {
        scb = (tagSCB *)sock->cb;
        result = __acceptclient(scb);
        waittime = scb->accepttime;
        if((NULL == result)&&(0 != waittime))
        {
        	//if none find and permit the wait
            Lock_MutexPost(sock->sync);
            if(Lock_SempPend(scb->acceptsemp,waittime))
            {
                if(Lock_MutexPend(sock->sync, CN_TIMEOUT_FOREVER))
                {
                    result = __acceptclient(scb);
                }
            }
        }
        if(NULL== result)  //no one to accept
        {
            sock->iostat&=(~CN_SOCKET_IOACCEPT);
            Multiplex_Set(sock->ioselect,sock->iostat);
        }
        else
        {
        	result->sockstat |= CN_SOCKET_OPEN;
        }
        Lock_MutexPost(sock->sync);
    }
    if((NULL != result)&&(Lock_MutexPend(pTcpHashTab->tabsync,CN_TIMEOUT_FOREVER)))
    {
        __hashSocketAdd(result);
        Lock_MutexPost(pTcpHashTab->tabsync);
    }
    if((NULL != result)&&(NULL != addr) &&(NULL != addrlen))
    {
        //fill the address if possible
        addrin = (struct sockaddr_in *)addr;
        addrin->sin_family = AF_INET;
        addrin->sin_port = result->element.v4.portremote;
        addrin->sin_addr.s_addr = result->element.v4.ipremote;
        *addrlen = sizeof(struct sockaddr);
    }
    return result;
}

//use this function to send a packge or pakage list to the ip layer
static bool_t __sendmsg(tagSocket *sock, tagNetPkg *pkg,u16 translen)
{
    bool_t       result;
    tagTcpHdr    *hdr;
    tagCCB      *ccb;
    u32          iplocal, ipremote;

    iplocal = sock->element.v4.iplocal;
    ipremote = sock->element.v4.ipremote;
    hdr = (tagTcpHdr *)(pkg->buf + pkg->offset);
    hdr->chksum = 0;
    //if any ack timer set, we'd better to clear the ack timer
    ccb = (tagCCB *)sock->cb;
    result = IpSend(EN_IPV_4,(ptu32_t)iplocal, (ptu32_t)ipremote,pkg,translen,IPPROTO_TCP,\
                     CN_IPDEV_TCPOCHKSUM,&hdr->chksum);
    if(result)
    {
        ccb->acktimes = 0;
        ccb->keeptimer = CN_TCP_TICK_KEEPALIVE;
    }
    return result;
}

//use this function to make a tcp header
static tagNetPkg  *__buildhdr(tagSocket *sock, u8 flags,\
		void *option, u8 optionlen,u32 pkgflag,u32 sndno)
{
    tagNetPkg  *result;
    tagTcpHdr  *hdr;
    tagCCB     *ccb;
    u32         datalen;

    datalen = sizeof(tagTcpHdr)+((optionlen+3)/4)*4;
    result = PkgMalloc(datalen,pkgflag);
    if(NULL != result)
    {
        result->datalen = datalen;
        ccb = (tagCCB*)sock->cb;
        hdr =(tagTcpHdr *)(result->offset + result->buf);
        hdr->portdst = sock->element.v4.portremote;
        hdr->portsrc = sock->element.v4.portlocal;
        hdr->ackno = htonl(ccb->rbuf.rcvnxt);
        hdr->seqno = htonl(sndno);
        hdr->hdroff = ((datalen/4)&0x0f)<<4;
        hdr->flags = flags;
        hdr->chksum = 0;
        if(ccb->rbuf.buflenlimit> ccb->rbuf.buflen)
        {
            hdr->window = htons(ccb->rbuf.buflenlimit- ccb->rbuf.buflen);
        }
        else
        {
            hdr->window = 0;
        }
        hdr->urgencyp = 0;
        memcpy((void *)hdr->opt,option,optionlen);
    }
    return result;
}
//build a tcp header and as your wish to send it
static bool_t __sendflag(tagSocket *sock, u8 flags, void *option, u8 optionlen,u32 sndno)
{
    bool_t              result;
    tagNetPkg          *pkg;

    result=false;
    //OK, construct the package for the connect
    pkg = __buildhdr(sock, flags, option, optionlen,CN_PKLGLST_END,sndno);
    if(NULL != pkg)
    {
        result = __sendmsg(sock,pkg,pkg->datalen);
        PkgTryFreePart(pkg);
    }
    return  result;
}
// =============================================================================
// FUNCTION:the client use this function to connect the server, will do the handshake
// PARA  IN:sock, the client socket
//          addr, the server addr,
//          addrlen, the server addr len
// PARA OUT:
// RETURN  :0 success while -1 failed
// INSTRUCT:
// =============================================================================
//make sure this is the client, so we should do the handshaker with the server
//much more, we must adjust its handle in the hash tab
static int __connect(tagSocket *sock, struct sockaddr *serveraddr, int addrlen)
{
    int  result;
    struct sockaddr_in *addrin;
    tagCCB             *ccb;
    result = -1;

    //fist we make sure to adjust the its place in the hash tab
    if(Lock_MutexPend(pTcpHashTab->tabsync,CN_TIMEOUT_FOREVER))
    {
    	if((NULL != serveraddr)&&(addrlen == sizeof(struct sockaddr_in))&&\
    	    (CN_SOCKET_CLIENT&sock->sockstat))
        {
    	    addrin = (struct sockaddr_in *)serveraddr;
            __hashSocketRemove(sock);
            sock->element.v4.ipremote =addrin->sin_addr.s_addr;
            sock->element.v4.portremote = addrin->sin_port;
            sock->sockstat |= CN_SOCKET_BIND;
            __hashSocketAdd(sock);
            result = 0;
        }
        Lock_MutexPost(pTcpHashTab->tabsync);
    }
    if(0 == result)
    {
    	result = -1;
    	//now do the handshake with the server
        if(Lock_MutexPend(sock->sync, CN_TIMEOUT_FOREVER))
        {
            ccb = (tagCCB *)sock->cb;
            if((0 ==(CN_SOCKET_CONNECT &sock->sockstat))&&\
               (EN_TCP_MC_CREAT ==ccb->machinestat))  //it does not connect yet
            {
                __sendflag(sock,CN_TCP_FLAG_SYN,(void *)&sgSynOptDefault,\
                                        sizeof(sgSynOptDefault),ccb->sbuf.sndnxtno);
                //change the machine stat and open the timer flag
                ccb->resndtimer = ccb->rto;
                ccb->sbuf.sndnxtno++;
                ccb->machinestat = EN_TCP_MC_SYNSNT;
                ccb->timerctrl  = CN_TCP_TIMER_SYN;
                Lock_MutexPost(sock->sync); //release the mutex
                //wait the sync signal happens
                Lock_SempPend(ccb->rbuf.bufsync,ccb->rbuf.timeout);
            	//check the connection
                Lock_MutexPend(sock->sync, CN_TIMEOUT_FOREVER);
                if(ccb->machinestat == EN_TCP_MC_ESTABLISH)
                {
                    //the app could send and rceive data from now on
                    ccb->channelstat|=CN_TCP_CHANNEL_STATASND|CN_TCP_CHANNEL_STATARCV;
                    result = 0; //connection success
                }
                else
                {
                    __ResetCCB(ccb, EN_TCP_MC_2FREE); //THE NEXT STEP NEED BE CLOSE
                }
            }
            Lock_MutexPost(sock->sync);
        }
    }
    return  result;
}
//we use this function to copy the data to the buffer,the len has been specified by the
//buflenleft,we write from the data off, may be round over
static void __cpy2sndbuf(tagSocket *sock, const void *msg, int len)
{
    tagCCB           *ccb;
    u8               *src;
    u8               *dst;
    int               cpylen;

    ccb = (tagCCB *)sock->cb;

    cpylen = ccb->sbuf.buflenlimit-ccb->sbuf.dataoff;
    if(cpylen >= len)
    {
    	cpylen = len;
    	len = 0;
    }
    else
    {
    	len -= cpylen;
    }
    dst = (u8 *)(ccb->sbuf.buf + ccb->sbuf.dataoff);
    src = (u8 *)msg;
    memcpy(dst,src,cpylen);
    src += cpylen;
    ccb->sbuf.dataoff += cpylen;
    if(ccb->sbuf.dataoff == ccb->sbuf.buflenlimit) //actually,we could be same at most
    {
    	ccb->sbuf.dataoff = 0;
    }
    //update the member
    ccb->sbuf.buflenleft -= cpylen;
    ccb->sbuf.datalen    += cpylen;

    if(len > 0)
    {
    	cpylen = len;
        dst = (u8 *)(ccb->sbuf.buf + ccb->sbuf.dataoff);
        memcpy(dst,src,cpylen);
        ccb->sbuf.dataoff += cpylen;
        if(ccb->sbuf.dataoff >= ccb->sbuf.buflenlimit)
        {
        	ccb->sbuf.dataoff = 0;
        }
        //update the member
        ccb->sbuf.buflenleft -= cpylen;
        ccb->sbuf.datalen    += cpylen;
    }
}
//check how many data could send in the current state
//consider how many data in the buffer and the channel could send most
//the result is the little one during them
static int __chkchannelsendlen(tagSocket *sock)
{
    int      result;
    int      datalen;
    tagCCB  *ccb;

    ccb =(tagCCB *)sock->cb;
    if(0 ==(ccb->channelstat & CN_TCP_CHANNEL_STATKSND))
    {
    	return 0;
    }

    datalen = ccb->sbuf.datalen;
	if(CN_SOCKET_PRONAGLE&sock->sockstat) //nagle is open
    {
    	//this means nothing to ack now or the new data is more than the mss,
    	if(ccb->sbuf.unackno == ccb->sbuf.sndnxtno)
    	{
    		//this means nothing to acknowledge, so you could send as much data as possible
    		result = datalen;
    	}
    	else if(datalen > ccb->mss)
    	{
    		result = datalen;
    	}
    	else
    	{
    		result = 0;
    	}
    }
    else //nagle is close
    {
    	//we will send the mixnum of (sndwnd cwnd datalen)
    	result = ccb->cwnd;
    	if(result > ccb->sndwnd)
    	{
    		result = ccb->sndwnd;
    	}
    	if(result > datalen)
    	{
    		result = datalen;
    	}
    }
    return result;
}

//use this function to send the new data or the data with the fin flag
static void __senddata(tagSocket *sock,int length)
{
    u16                 datalen;
    tagNetPkg          *pkghdr;
    tagNetPkg          *pkgdata;
    tagCCB             *ccb;
    u8                  flags;
    u8                 *data;
    bool_t              sendresult;
    u32                 translen;

    ccb = (tagCCB *)sock->cb;
    while(length > 0) //we send all the data specified by length or until send failed
    {
    	//compute the data datalen flags first
    	flags = CN_TCP_FLAG_ACK;
    	if((length <= ccb->mss)) //we could send it at one time
    	{
    		flags |= CN_TCP_FLAG_PSH;
    		if(0 == (ccb->channelstat & CN_TCP_CHANNEL_STATASND))
    		{
    			flags |= CN_TCP_FLAG_FIN;   //we would not send any more
    		}
    		datalen = length;
    	}
    	else
    	{
    		datalen = ccb->mss;
    	}
    	//make sure the data not round trip
		if((ccb->sbuf.sndnxtoff + datalen )>ccb->sbuf.buflenlimit)
		{
			//this means the data round up ,we just send the part
			datalen = ccb->sbuf.buflenlimit - ccb->sbuf.sndnxtoff;
			flags &=(~CN_TCP_FLAG_FIN);
		}
		else
		{
			//no round trip,just do it
		}
		data = ccb->sbuf.buf + ccb->sbuf.sndnxtoff;
    	pkghdr = __buildhdr(sock,flags,NULL,0,0,ccb->sbuf.sndnxtno);
    	if(NULL != pkghdr)
    	{
    		pkgdata = PkgMalloc(sizeof(tagNetPkg),CN_PKLGLST_END);
    		if(NULL != pkgdata)
    		{
    			//we got the data pkgnow, make sure how many data we will send
    			pkgdata->buf = data;
    			pkgdata->offset = 0;
    			pkgdata->datalen = datalen;
    			pkgdata->partnext = NULL;
    			pkghdr->partnext = pkgdata;
    			translen = datalen + pkghdr->datalen;
    			sendresult = __sendmsg(sock,pkghdr,translen);
    			//free the package
    			PkgTryFreePart(pkghdr);
    			PkgTryFreePart(pkgdata);
    			//update the members
    			if(sendresult)
    			{
        			ccb->sbuf.sndnxtno += datalen;
        			ccb->sbuf.sndnxtoff += datalen;
        			if(ccb->sbuf.sndnxtoff == ccb->sbuf.buflenlimit)
        			{
        				ccb->sbuf.sndnxtoff = 0; //round
        			}
        			ccb->sbuf.unacklen += datalen;
        			ccb->sbuf.datalen -= datalen;
                    if(ccb->sndwnd > datalen)
                    {
                        ccb->sndwnd -= datalen;
                    }
                    else
                    {
                        ccb->sndwnd = 0;
                    }
        			length -= datalen;
                    if(flags & CN_TCP_FLAG_FIN) //IF FIN SEND ,THEN STAT CHANGE,CLOSE THE KERNEL SEND
                    {
                    	ccb->sbuf.sndnxtno++;
                        if(EN_TCP_MC_CLOSEWAIT == ccb->machinestat)
                        {
                            ccb->machinestat = EN_TCP_MC_LASTACK;
                        }
                        else
                        {
                            ccb->machinestat = EN_TCP_MC_FINWAIT1;
                        }
                        ccb->channelstat &= (~CN_TCP_CHANNEL_STATKSND);
                        ccb->timerctrl |= CN_TCP_TIMER_FIN;  //open the fin timer
                        //no send anymore and break
                        break;
                    }
    			}
    			else
    			{
    		        ccb->timerctrl |= CN_TCP_TIMER_PERSIST;
    		        ccb->persistimer = CN_TCP_TICK_PERSIST;
    				break;  //send failed,open the persist timer
    			}
    		}
    		else
    		{
    			PkgTryFreePart(pkghdr);  //no pkg memory for the tcpdata
    			break;
    		}
    	}
    	else
    	{
    		break;  //no pkg memory for the tcp header
    	}
    	ccb->sbuf.rtotick = gTcpTicks;
    }
    if(ccb->sbuf.unacklen > 0)
    {
		ccb->resndtimer = ccb->rto;
		ccb->timerctrl |= CN_TCP_TIMER_RESEND;   //OPEN THE RESEND TIMER
    }
    if(((ccb->sbuf.datalen > 0)&&(ccb->sndwnd < ccb->mss)))
    {
        ccb->timerctrl |= CN_TCP_TIMER_PERSIST;
        ccb->persistimer = CN_TCP_TICK_PERSIST;
    }
    else
    {
        ccb->persistimer &= (~CN_TCP_TICK_PERSIST);
        ccb->persistimer = 0;
    }
    return;
}
//use this function to resend the data,we just resend the unacknowledge data
static void __resenddata(tagSocket *sock)
{
    u16                 datalen;
    tagNetPkg          *pkghdr;
    tagNetPkg          *pkgdata;
    tagCCB             *ccb;
    u8                  flags;
    u8                 *data;
    u32                 translen;

    ccb = (tagCCB *)sock->cb;
    //first we want to make sure if any data or how many data to resend
    //we could only send a mss at most
    datalen = ccb->sbuf.unacklen;
    if((datalen + ccb->sbuf.unackoff)>ccb->sbuf.buflenlimit)
    {
    	//the unack is round up, so just send part
    	datalen = ccb->sbuf.buflenlimit - ccb->sbuf.unackoff;
    }
    if(datalen > ccb->mss)
    {
    	//sorry to tell you that we could send a mss at most
    	datalen = ccb->mss;
    }
    if(datalen > 0)
    {
    	//compute the data datalen flags first
    	flags = CN_TCP_FLAG_ACK|CN_TCP_FLAG_PSH;
		data = ccb->sbuf.buf + ccb->sbuf.unackoff;
    	pkghdr = __buildhdr(sock,flags,NULL,0,0,ccb->sbuf.unackno);
    	if(NULL != pkghdr)
    	{
    		pkgdata = PkgMalloc(sizeof(tagNetPkg),CN_PKLGLST_END);
    		if(NULL != pkgdata)
    		{
    			//we got the data pkgnow, make sure how many data we will send
    			pkgdata->buf = data;
    			pkgdata->offset = 0;
    			pkgdata->datalen = datalen;
    			pkgdata->partnext = NULL;
    			pkghdr->partnext = pkgdata;
    			translen = datalen + pkghdr->datalen;
    			__sendmsg(sock,pkghdr,translen);
    			//free the package
    			PkgTryFreePart(pkghdr);
    			PkgTryFreePart(pkgdata);
    		}
    		else
    		{
    			PkgTryFreePart(pkghdr);  //no pkg memory for the tcpdata
    		}
    	}
    }
    return;
}
// =============================================================================
// FUNCTION:this function used to send data to the socket
// PARA  IN:sock, the socket to send
//          msg, the message to send
//          len, the message length
//          flags,not used yet
// PARA OUT:
// RETURN  :
// INSTRUCT:-1 failed while others return the send length really
// =============================================================================
//1,first we must got the sync,
//2,then got the mutex to copy data
static int __send(tagSocket *sock, const void *msg, int len, int flags)
{
    int        result;
    int        sndlen;
    tagCCB    *ccb;

    result = -1;
    if(0 == (CN_SOCKET_CLIENT &sock->sockstat))
    {
    	return result;
    }
    ccb = (tagCCB *)sock->cb;
    if(Lock_SempPend(ccb->sbuf.bufsync,ccb->sbuf.timeout))
    {
    	//got the mutex
    	if(Lock_MutexPend(sock->sync, CN_TIMEOUT_FOREVER))
    	{
    		//check if we could copy it to the buffer
    		if(CN_TCP_CHANNEL_STATASND&ccb->channelstat)
    		{
            	sndlen = ccb->sbuf.buflenleft>len?len:ccb->sbuf.buflenleft;
                if(sndlen > 0)
                {
                    //enough,we could cpy the buf to the sbuf;
                   __cpy2sndbuf(sock,msg,sndlen);
                   result = sndlen;
                   //check if we could send some data out
                   sndlen = __chkchannelsendlen(sock);
                   if(sndlen > 0)
                   {
                       __senddata(sock,sndlen);
                   }
                   //if the fin or send is not normal, then will not clean the trigle
                   //which means that the channel is shutdown and you could got -1 result
                   if((ccb->sbuf.buflenleft > ccb->sbuf.trigerlevel)||
                      (0 ==(ccb->channelstat&CN_TCP_CHANNEL_STATASND)))
                   {
                       sock->iostat|= CN_SOCKET_IOWRITE;
                       Multiplex_Set(sock->ioselect, sock->iostat);
                	   Lock_SempPost(ccb->sbuf.bufsync);
                   }
                   else
                   {
                       sock->iostat&= (~CN_SOCKET_IOWRITE);
                       Multiplex_Set(sock->ioselect, sock->iostat);
                   }
                }
                else
                {
                	if(len == 0)
                	{
                		result = len;   //0 bytes copied
                	}
                	else
                	{
                		result = -1;    //unkonw reason
                	}
                }
    		}
    		else
    		{
    			result = 0;  // the channel is shutdown now
                sock->iostat|= CN_SOCKET_IOWRITE;
                Multiplex_Set(sock->ioselect, sock->iostat);
    			Lock_SempPost(ccb->sbuf.bufsync);
    		}
            Lock_MutexPost(sock->sync);
    	}
    }
    else
    {
    	//has not got the sync yet. maybe time out
    }
    return result;
}

//use this function to copy data from the receive buf
static int __cpyfromrcvbuf(tagCCB *ccb, void *buf, int len)
{
    int  result;
    int cpylen;
    u8   *cpybuf;
    u8   *srcbuf;
    tagNetPkg  *pkg;

    srcbuf = (u8 *)buf;
    result = 0;

    pkg = ccb->rbuf.ph;
    while((len >0) && (NULL != pkg))
    {
        cpylen = len > pkg->datalen?pkg->datalen:len;
        cpybuf = (u8 *)(pkg->buf + pkg->offset);
        memcpy(srcbuf, cpybuf,cpylen);
        pkg->offset += cpylen;
        pkg->datalen -= cpylen;
        ccb->rbuf.buflen -= cpylen;
        result +=cpylen;
        len-= cpylen;
        srcbuf+=cpylen;
        if(0==pkg->datalen) //release the no data pkg
        {
            ccb->rbuf.ph = pkg->partnext;
            if(NULL == ccb->rbuf.ph)
            {
                ccb->rbuf.pt = NULL;
            }
            PkgTryFreePart(pkg);
            pkg = ccb->rbuf.ph;
        }
    }

    return result;
}
// =============================================================================
// FUNCTION:this function used to receive data from the socket
// PARA  IN:the parameters has the same meaning as recv
// PARA OUT:
// RETURN  :
// INSTRUCT:0 means the socket is closed or fin receive -1 means no data,others means
//          the data length copy from the socket
// =============================================================================
static int __recv(tagSocket *sock, void *buf,int len, unsigned int flags)
{
    int        result;
    tagCCB    *ccb;

    result = -1;
    if(0 == (CN_SOCKET_CLIENT &sock->sockstat))
    {
    	return result;
    }
    ccb = (tagCCB *)sock->cb;
    if(Lock_SempPend(ccb->rbuf.bufsync,ccb->rbuf.timeout))
    {
    	//got the mutex
    	if(Lock_MutexPend(sock->sync, CN_TIMEOUT_FOREVER))
    	{
    		if(CN_TCP_CHANNEL_STATARCV & ccb->channelstat)
    		{
    			//maybe there are some data
                result = __cpyfromrcvbuf(ccb,buf,len);   //the buf already has data
                if(result == 0)
                {
                	//no data yet
                	if(0 == (ccb->channelstat&CN_TCP_CHANNEL_STATKRCV))
                	{
                		//the remote is shutdown,so close the areceive
                		ccb->channelstat &= (~CN_TCP_CHANNEL_STATARCV);
                	}
                	else
                	{
                		result = -1;  //not shutdown yet
                	}
                }

                if((ccb->rbuf.buflen > ccb->rbuf.trigerlevel)||
                   (0 == (ccb->channelstat&CN_TCP_CHANNEL_STATKRCV)))
                {
                    sock->iostat |= CN_SOCKET_IOREAD;
                    Multiplex_Set(sock->ioselect, sock->iostat);
                    Lock_SempPost(ccb->rbuf.bufsync);
                }
                else
                {
                    sock->iostat &= (~CN_SOCKET_IOREAD);
                    Multiplex_Set(sock->ioselect, sock->iostat);
                    ccb->channelstat &=(~CN_TCP_CHANNEL_STATCONGEST);
                    //changed the rcv window
                    //we may prevent the data in because of the limited buf, then we
                    //if we has some space, we'd better to notice the remote terminal
                    //if the data is zero, we could snd the window
                    __sendflag(sock,CN_TCP_FLAG_ACK,NULL,0,ccb->sbuf.sndnxtno);
                }
    		}
    		else
    		{
    			result = 0;  // the channel receive is shutdown now
                sock->iostat|= CN_SOCKET_IOREAD;
                Multiplex_Set(sock->ioselect, sock->iostat);
    			Lock_SempPost(ccb->rbuf.bufsync);
    		}

    		Lock_MutexPost(sock->sync);
    	}
    }
    else
    {
    	//not got the sync yet
    }
    return  result;
}

// =============================================================================
// FUNCTION:this function used to shutdown the socket receive channel
// PARA  IN:
// PARA OUT:
// RETURN  :
// INSTRUCT:0 success while -1 failed
// =============================================================================
static int __shutdownRD(tagSocket *sock)
{
    int result;
    tagCCB *ccb;

    result = -1;
    ccb = (tagCCB *)sock->cb;

    //clear the rcv buf and recombination queue
    //and set some specified flags at the same time
    ccb->channelstat&=(~(CN_TCP_CHANNEL_STATARCV|CN_TCP_CHANNEL_STATKRCV));
    //release the rcv and recombination queue
    PkgTryFreeQ(ccb->rbuf.ph);
    ccb->rbuf.buflen = 0;
    ccb->rbuf.buflenlimit  = CN_TCP_BUFLENDEFAULT;
    ccb->rbuf.ph = NULL;
    ccb->rbuf.pt = NULL;
    ccb->rbuf.trigerlevel = 0;
    Lock_SempPost(ccb->rbuf.bufsync);
    //free all the recomblst
    PkgTryFreeQ(ccb->pkgrecomblst);
    ccb->pkgrecomblst = NULL;
    ccb->pkgrecomblen = 0;
    result = 0;

    return result;
}
// =============================================================================
// FUNCTION:this function used to shutdown the socket send channel
// PARA  IN:
// PARA OUT:
// RETURN  :
// INSTRUCT:0 success while -1 failed
// =============================================================================
static int __shutdownWR(tagSocket *sock)
{
    int result;
    tagCCB     *ccb;

    result = -1;
    if(sock->sockstat & CN_SOCKET_CLIENT)
    {
        ccb = (tagCCB *)sock->cb;
        ccb->channelstat&=(~(CN_TCP_CHANNEL_STATASND));  //APP NO PERMIT TO TRANSMISSION
        //if send buf still has some data to snd, then set the flag is OK
        if(ccb->sbuf.datalen == 0)
        {
            ccb->channelstat&=(~(CN_TCP_CHANNEL_STATKSND));
            __sendflag(sock,CN_TCP_FLAG_FIN|CN_TCP_FLAG_ACK,NULL,0,ccb->sbuf.sndnxtno);
            ccb->sbuf.sndnxtno++;
            ccb->resndtimer = ccb->rto;
            ccb->timerctrl |= CN_TCP_TIMER_FIN;
            if(EN_TCP_MC_CLOSEWAIT == ccb->machinestat)
            {
                ccb->machinestat = EN_TCP_MC_LASTACK;
            }
            else
            {
            	ccb->machinestat  = EN_TCP_MC_FINWAIT1;
            }
        	result = 0;
        }
        else
        {
        	//send the fin with the data, this will do in __senddata
        }
    }
    return result;
}
// =============================================================================
// FUNCTION:this function used to shutdown the socket  channel
// PARA  IN:sock, the socket to shutdown
//          how,SHUT_RD means receive channel,SHUT_WR means send channel,SHUT_RDWR means all channle
// PARA OUT:
// RETURN  :
// INSTRUCT:0 success while -1 failed
// =============================================================================
static int __shutdown(tagSocket *sock, u32 how)
{
    int    result;

    if((CN_SOCKET_CLIENT&sock->sockstat)&&\
        Lock_MutexPend(sock->sync,CN_TIMEOUT_FOREVER))
    {
        switch(how)
        {
            case SHUT_RD:
                result = __shutdownRD(sock);
                break;
            case SHUT_WR:
                result = __shutdownWR(sock);
                break;
            case SHUT_RDWR:
                result = __shutdownRD(sock);
                if(0 ==result)
                {
                    result = __shutdownWR(sock);
                }
                break;
            default:
                result = -1;
                break;
        }
        Lock_MutexPost(sock->sync);
    }
    return  result;
}
// =============================================================================
// FUNCTION:this function used to close the socket  channel
// PARA  IN:sock, the socket to close
// PARA OUT:
// RETURN  :
// INSTRUCT:0 success while -1 failed;
//          1,we will not delete it here but mark some flags,we will delete it in
//            the tick processor(scan the sock with CLOSE and ccb stat is 2free)
//          2,if the linger is closed, then send rst;if linger is open and there
//             is still some data in the send buffer,then send the rst, otherwise
//            do the close handshake
// =============================================================================
static int __closesocket(tagSocket *sock)
{
    int       result;
    tagCCB   *ccb;
    tagSCB   *scb;
    tagSocket *client;
    tagSocket *server;
    result = -1;

    if(Lock_MutexPend(pTcpHashTab->tabsync,CN_TIMEOUT_FOREVER))
    {
        if(Lock_MutexPend(sock->sync,CN_TIMEOUT_FOREVER))
        {
            if(CN_SOCKET_CLIENT&sock->sockstat)  //sock client
            {
                client = sock;
                client->sockstat |= CN_SOCKET_CLOSE;
                ccb = (tagCCB *)client->cb;
                if(ccb->machinestat == EN_TCP_MC_CREAT)
                {
                    __ResetCCB(ccb, EN_TCP_MC_2FREE);
                    result = true;
                }
                else
                {
                    __shutdownRD(client);
                    result = __shutdownWR(client);
                }
                Lock_MutexPost(client->sync);
            }
            else   //this is the server,so just close it and close all the client hung on it
            {
                server = sock;
                server->sockstat |= CN_SOCKET_CLOSE;
                //shutdown each socket hunge on the server(not accept yet)
                scb = (tagSCB *)server->cb;
                while(NULL != scb->clst)
                {
                    client = scb->clst;
                    scb->clst = client->nxt;

                    ccb = (tagCCB *)client->cb;
                    __ResetCCB(ccb,EN_TCP_MC_2FREE);
                    __DeleCCB(ccb);
                    SocketFree(client);
                }
                __ReseSCB(scb);
                __DeleteScb(scb);
                __hashSocketRemove(server);
                SocketFree(server);
            }
            result = 0;
        }

        Lock_MutexPost(pTcpHashTab->tabsync);
    }

    return result;
}

//this function deal with SOL_SOCK
static int __setsockopt_sol(tagSocket *sock,int optname,const void *optval, int optlen)
{
    bool_t result;
    tagCCB *ccb;
    tagSCB *scb;
    struct linger      *mylinger;
	u8 *buf;
	int buflen;

    result = -1;
    switch(optname)
    {
        case SO_BROADCAST:
            break;
        case SO_DEBUG:
            break;
        case SO_DONTROUTE:
            break;
        case SO_ERROR:
            break;
        case SO_KEEPALIVE:
            if(CN_SOCKET_CLIENT&sock->sockstat)
            {
                ccb = (tagCCB *)sock->cb;
                if(*(int *)optval)
                {
                    ccb->timerctrl |= CN_TCP_TIMER_KEEPALIVE;
                    ccb->keeptimer = CN_TCP_TICK_KEEPALIVE;
                }
                else
                {
                    ccb->timerctrl &= (~CN_TCP_TIMER_KEEPALIVE);
                }
                result = 0;
            }
            break;
        case SO_LINGER:
            if((CN_SOCKET_CLIENT&sock->sockstat)&&\
                    (optlen == sizeof(struct linger)))
            {
                ccb = (tagCCB *)sock->cb;
                mylinger  = (struct linger *)optval;
                if(mylinger->l_onff)
                {
                    ccb->timerctrl |= CN_TCP_TIMER_LINGER;
                    ccb->lingertimer = mylinger->l_linger*1000/CN_TCP_TICK_TIME;
                }
                else
                {
                    ccb->timerctrl &= (~CN_TCP_TIMER_LINGER);
                }
                result = 0;
            }
            break;
        case SO_OOBINLINE:
            break;
        case SO_RCVBUF:
            if(CN_SOCKET_CLIENT&sock->sockstat)
            {
                ccb = (tagCCB *)sock->cb;
            	int buflen;
            	buflen = *(int *)optval;
                if(buflen >0)
                {
                    ccb->rbuf.buflenlimit = buflen;
                    ccb->rbuf.trigerlevel = 0;
                    result = 0;
                }
            }
            break;
        case SO_SNDBUF:
            if(CN_SOCKET_CLIENT&sock->sockstat)
            {
                ccb = (tagCCB *)sock->cb;
            	buflen = *(int *)optval;
                if((buflen >0)&&((buf = malloc(buflen))!= NULL))
                {
                    ccb->sbuf.buflenleft =  buflen;
                    ccb->sbuf.buflenlimit = buflen;
                    ccb->sbuf.trigerlevel = 0;
                    ccb->sbuf.timeout     = CN_TIMEOUT_FOREVER;
                    ccb->sbuf.dataoff     = 0;
                    ccb->sbuf.datalen     = 0;
                    ccb->sbuf.unackoff    = 0;
                    ccb->sbuf.unacklen    = 0;
                    ccb->sbuf.sndnxtoff   = 0;
                    ccb->sbuf.dupacktims  = 0;
                    if(NULL != ccb->sbuf.buf)
                    {
                    	free((void *)ccb->sbuf.buf);
                    }
                    ccb->sbuf.buf = buf;
                    result = 0;
                }
            }
            break;
        case SO_RCVLOWAT:
            if(CN_SOCKET_CLIENT&sock->sockstat)
            {
                ccb = (tagCCB *)sock->cb;
                if(*(int *)optval>=0)
                {
                    ccb->rbuf.trigerlevel = *(int *)optval;
                    result = 0;
                }
            }
            break;
        case SO_SNDLOWAT:
            if(CN_SOCKET_CLIENT&sock->sockstat)
            {
                ccb = (tagCCB *)sock->cb;
                if(*(int *)optval>=0)
                {
                    ccb->sbuf.trigerlevel = *(int *)optval;
                    result = 0;
                }
            }
            break;
        case SO_RCVTIMEO:
            if(CN_SOCKET_CLIENT&sock->sockstat)
            {
                ccb = (tagCCB *)sock->cb;
                if(*(int *)optval>=0)
                {
                    ccb->rbuf.timeout = *(int *)optval;
                    result = 0;
                }
            }
            else
            {
            	scb = (tagSCB *)sock->cb;
                if(*(int *)optval>=0)
                {
                    scb->accepttime = *(int *)optval;
                    result = 0;
                }
            }
            break;
        case SO_SNDTIMEO:
            if(CN_SOCKET_CLIENT&sock->sockstat)
            {
                ccb = (tagCCB *)sock->cb;
                if(*(int *)optval>=0)
                {
                    ccb->sbuf.timeout = *(int *)optval;
                    result = 0;
                }
            }
            break;
        case SO_REUSEADDR:
            result = 0;
            break;
        case SO_TYPE:
            result = 0;
            break;
        case SO_BSDCOMPAT:
            result = 0;
            break;
        case SO_NOBLOCK:
            if(*(int *)optval)
            {
                sock->sockstat &= (~CN_SOCKET_PROBLOCK);
                if(CN_SOCKET_CLIENT&sock->sockstat)
                {
                    ccb = (tagCCB *)sock->cb;
                    ccb->rbuf.timeout = 0;
                    ccb->sbuf.timeout = 0;
                }
                else
                {
                    scb = (tagSCB *)sock->cb;
                    scb->accepttime = 0;
                }
            }
            else
            {
                sock->sockstat |=  CN_SOCKET_PROBLOCK;
                if(CN_SOCKET_CLIENT&sock->sockstat)
                {
                    ccb = (tagCCB *)sock->cb;
                    ccb->rbuf.timeout = CN_TIMEOUT_FOREVER;
                    ccb->sbuf.timeout = CN_TIMEOUT_FOREVER;
                }
                else
                {
                    scb = (tagSCB *)sock->cb;
                    scb->accepttime = CN_TIMEOUT_FOREVER;
                }
            }
            result = 0;
            break;
        default:
            result = -1;
            break;
    }
    return result;
}
//this function deal with the IPPROTO_IP
static int __setsockopt_ip(tagSocket *sock,int optname,const void *optval, int optlen)
{
    bool_t result;

    result = -1;
    switch(optname)
    {
        case IP_HDRINCL:
            break;
        case IP_OPTIONS:
            break;
        case IP_TOS:
            break;
        case IP_TTL:
            break;
        default:
            break;
    }

    return result;
}
//this function deal with ipproto_tcp
static int __setsockopt_tcp(tagSocket *sock,int optname,const void *optval, int optlen)
{
    bool_t result;
    tagCCB *ccb;

    result = -1;
    switch(optname)
    {
        case TCP_MAXSEG:
            if(CN_SOCKET_CLIENT&sock->sockstat)
            {
                ccb = (tagCCB *)sock->cb;
                ccb->mss = *(u8 *)optval;
                result = 0;
            }
            break;
        case TCP_NODELAY:
            if(CN_SOCKET_CLIENT&sock->sockstat)
            {
                if(*(int *)optval)
                {
                    sock->sockstat &=(~CN_SOCKET_PRONAGLE);
                }
                else
                {
                    sock->sockstat |= CN_SOCKET_PRONAGLE;
                }
                result = 0;
            }
            break;
        default:
            break;
    }

    return result;
}
// =============================================================================
// FUNCTION:this function used to the socket option
// PARA  IN:sock, the socket to set option
//          level,such as IPPROTO_TCP
//          optname, level details, such as SO_KEEPALIVE and so on
//          optvalue, the value of the optname;
//          optlen,the length of optvalue
// PARA OUT:
// RETURN  :
// INSTRUCT:0 success while -1 failed
// =============================================================================
static int __setsockopt(tagSocket *sock, int level, int optname,\
               const void *optval, int optlen)
{
    int  result;

    if(Lock_MutexPend(sock->sync,CN_TIMEOUT_FOREVER))
    {
        switch(level)
        {
            case SOL_SOCKET:
                result = __setsockopt_sol(sock,optname,optval,optlen);
                break;
            case IPPROTO_IP:
                result = __setsockopt_ip(sock,optname,optval,optlen);
                break;
            case IPPROTO_TCP:
                result = __setsockopt_tcp(sock,optname,optval,optlen);
                break;
            default:
                result = -1;
                break;
        }
        Lock_MutexPost(sock->sync);
    }
    return  result;
}
// =============================================================================
// FUNCTION:this function used to get the socket option, not implement yet
// PARA  IN:sock, the socket to set option
//          level,such as IPPROTO_TCP
//          optname, level details, such as SO_KEEPALIVE and so on
//          optvalue, the value of the optname;
//          optlen,the length of optvalue
// PARA OUT:
// RETURN  :
// INSTRUCT:0 success while -1 failed
// =============================================================================
static int __getsockopt(tagSocket *sock, int level, int optname, void *optval,\
               int *optlen)
{
    int  result = -1;


    return  result;
}
////////////////////////MACHINE STAT DEAL///////////////////////////////////////
//do the tcp checksum if the net dev not do it
static bool_t  __rcvchecksum(u32 iplocal,u32 ipremote, tagNetPkg *pkglst)
{
    bool_t  result;
    u16     chksum;
    u16     translen;
    tagNetPkg *pkg;
    translen = 0;
    pkg = pkglst;
    while(pkg != NULL)
    {
        translen += pkg->datalen;
        pkg = pkg->partnext;
    }
    IpPseudoPkgLstChkSumV4(iplocal, ipremote, IPPROTO_TCP,\
                         pkglst,translen,&chksum);
    if(0 == chksum)
        result =true;
    else
        result = false;
    return result;
}
//if could not find or create a socket for the remote, then just reset it
static bool_t __resetremoteraw(u32 iplocal, u16 portlocal,u32 ipremote, u16 portremote,u32 ackno)
{
    bool_t result ;
    tagNetPkg *pkg;
    tagTcpHdr   *hdr;
    u16 translen;

    result = false;
    pkg = PkgMalloc(sizeof(tagTcpHdr),CN_PKLGLST_END);
    if(NULL != pkg)
    {
        hdr = (tagTcpHdr *)(pkg->offset + pkg->buf);
        hdr->chksum = 0;
        hdr->flags = CN_TCP_FLAG_RST|CN_TCP_FLAG_ACK;
        hdr->hdroff = 0x05<<4;
        hdr->portdst = portremote;
        hdr->portsrc = portlocal;
        hdr->ackno  = htonl(ackno);
        hdr->seqno = 0;
        hdr->urgencyp = 0;
        pkg->datalen = sizeof(tagTcpHdr);
        translen = pkg->datalen;

        result = IpSend(EN_IPV_4,(ptu32_t)iplocal, (ptu32_t)ipremote, pkg,translen,IPPROTO_TCP,\
                 CN_IPDEV_TCPOCHKSUM,&hdr->chksum);
        PkgTryFreePart(pkg);
    }
    return result;
}

enum __EN_TCP_OPT_CODE
{
    EN_TCP_OPT_END = 0,
    EN_TCP_OPT_NONE,
    EN_TCP_OPT_MSS ,
    EN_TCP_OPT_SCALE,
    EN_TCP_OPT_SACKP,
    EN_TCP_OPT_SACKR,
    EN_TCP_OPT_TAMP = 8,
};

//do the remote optional
static void dealtcpoption(tagCCB *ccb, tagTcpHdr *hdr)
{
    u8  totallen;
    u8  optlen;
    u8  kind;
    u8  data8;
    u16 data16;
    u8  *src;
    u8  *optsrc;

    totallen = ((hdr->hdroff>>4) -0x05)*4;
    src = (u8 *)hdr->opt;
    while(totallen > 0)
    {
        optsrc = src;
        kind = *optsrc++;
        switch(kind)
        {
            case EN_TCP_OPT_END:      //OPTION END
                totallen = 0;
                break;
            case EN_TCP_OPT_NONE:     //OPTION NONE
                totallen -= 1;
                src += 1;
                break;
            case EN_TCP_OPT_MSS:      //SET THE REMOTE MSS
                optlen = *optsrc++;
                data16 = *(u16 *)(optsrc);
                data16 = htons(data16);
                ccb->mss = ccb->mss > data16?data16:ccb->mss;
                totallen -= optlen;
                src += optlen;
                break;
            case EN_TCP_OPT_SCALE:      //SET THE REMOTE WND SCALE
                optlen = *optsrc++;
                data8 = *(u8 *)(optsrc);
                ccb->sndwndscale = data8;
                ccb->sndwnd = ccb->sndwnd<<ccb->sndwndscale;
                totallen -= optlen;
                src += optlen;
                break;
            case EN_TCP_OPT_TAMP:
                optlen = *optsrc++;
                totallen -= optlen;
                src += optlen;
                break;
            default:
                totallen = 0;
                break;
        }
    }
    return ;
}

//we use this function to deal with the data accepted, which means that if the krcv
//bit of the ccb is set then could use this function to deal with the data received
//check the krcv bit and the pkg.data len > 0
//which means how many data received
static u32 __rcvdata(tagSocket *client, u32 seqno,tagNetPkg *pkg)
{
    tagCCB   *ccb;
    u32        pkgstart;
    u32        pkgstop;
    u32        rcvlen;
    u32        pkgdataoff;
    tagNetPkg *pkgcomb;

    rcvlen = 0;
    ccb = (tagCCB *)client->cb;
    pkgstart = seqno;
    pkgstop = seqno + pkg->datalen;
    if(ccb->rbuf.rcvnxt == pkgstart)
    {
    	//this is just the pkg what we want to receive next
        PkgCachedPart(pkg); //cached the package
        //and add it to the receive queue
        if(NULL == ccb->rbuf.pt)
        {
            ccb->rbuf.ph = pkg;
        }
        else
        {
            ccb->rbuf.pt->partnext = pkg;
        }
        ccb->rbuf.pt = pkg;
        ccb->rbuf.buflen += pkg->datalen;
        ccb->rbuf.rcvnxt+= pkg->datalen;
        rcvlen += pkg->datalen;
    }
    else if((pkgstop > pkgstart)&&((ccb->rbuf.rcvnxt > pkgstart)&&(ccb->rbuf.rcvnxt < pkgstop)))
    {
    	//this is only part of the package data valid
    	pkgdataoff = ccb->rbuf.rcvnxt - pkgstart;
    	pkg->offset += pkgdataoff;
    	pkg->datalen -= pkgdataoff;
    	//then cached it and add it to the queue
        PkgCachedPart(pkg); //cached the package
        //and add it to the receive queue
        if(NULL == ccb->rbuf.pt)
        {
            ccb->rbuf.ph = pkg;
        }
        else
        {
            ccb->rbuf.pt->partnext = pkg;
        }
        ccb->rbuf.pt = pkg;
        ccb->rbuf.buflen += pkg->datalen;
        ccb->rbuf.rcvnxt+= pkg->datalen;
        rcvlen += pkg->datalen;
    }
    else if((pkgstop < pkgstart)&&((ccb->rbuf.rcvnxt > pkgstart)||(ccb->rbuf.rcvnxt < pkgstop)))
    {
    	//this is only part of the package data valid and the seqo turn over
    	pkgdataoff = ccb->rbuf.rcvnxt - pkgstart;
    	pkg->offset += pkgdataoff;
    	pkg->datalen -= pkgdataoff;
    	//then cached it and add it to the queue
        PkgCachedPart(pkg); //cached the package
        //and add it to the receive queue
        if(NULL == ccb->rbuf.pt)
        {
            ccb->rbuf.ph = pkg;
        }
        else
        {
            ccb->rbuf.pt->partnext = pkg;
        }
        ccb->rbuf.pt = pkg;
        ccb->rbuf.buflen += pkg->datalen;
        ccb->rbuf.rcvnxt+= pkg->datalen;
        rcvlen += pkg->datalen;
    }
    else
    {
    	//this data is out of sequence, we'd better to cached it in the recombination queue
        //could cached in the recombination queue
    	if(gEnableTcpReoder)
    	{
            PkgCachedPart(pkg);
            pkg->private = seqno;
            //find an proplace to insert the pkg
            if(NULL == ccb->pkgrecomblst)//this queue is empty
            {
                pkg->partnext = NULL;
                ccb->pkgrecomblst = pkg;
            }
            else
            {
                pkg->partnext = ccb->pkgrecomblst;
                ccb->pkgrecomblst = pkg;
            }
            ccb->pkgrecomblen++;
    	}
    }
    //if we have receive some data to the receive buffer, then we check all the data
    //in the combination queue, if mathes then move it to the receive buffer, else drops now
    if(rcvlen > 0)
    {
    	if(gEnableTcpReoder)
    	{
        	while(NULL != ccb->pkgrecomblst)
        	{
        		pkgcomb = ccb->pkgrecomblst;
        		ccb->pkgrecomblst = pkgcomb->partnext;
        		pkgcomb->partnext = NULL;
        		pkgstart = (u32)pkgcomb->private;
        		pkgstop = pkgstart + pkgcomb->datalen;
        		if(pkgstart == ccb->rbuf.rcvnxt)
        		{
        	        //and add it to the receive queue
        	        if(NULL == ccb->rbuf.pt)
        	        {
        	            ccb->rbuf.ph = pkgcomb;
        	        }
        	        else
        	        {
        	            ccb->rbuf.pt->partnext = pkgcomb;
        	        }
        	        ccb->rbuf.pt = pkgcomb;
        	        ccb->rbuf.buflen += pkgcomb->datalen;
        	        ccb->rbuf.rcvnxt+= pkgcomb->datalen;
        	        rcvlen += pkgcomb->datalen;
        		}
        		else
        		{
        			//drops it now
        			PkgTryFreePart(pkgcomb);
        		}
            	ccb->pkgrecomblen = 0;
        	}
    	}
        if(ccb->rbuf.buflen > ccb->rbuf.trigerlevel)
        {
            client->iostat |= CN_SOCKET_IOREAD;
            Multiplex_Set(client->ioselect, client->iostat);
            Lock_SempPost(ccb->rbuf.bufsync);
        }
        if(ccb->rbuf.buflen >= (2*ccb->rbuf.buflenlimit))
        {
            ccb->channelstat |= CN_TCP_CHANNEL_STATCONGEST;
        }
    }
    return rcvlen;
}
#define __TCPCHKINTARGETNOE(target,start,stop) (stop>=start?((target>start)&&(target<stop)):\
                                            ((target>start)||(target<stop)))

#define __TCPCHKINTARGET(target,start,stop) (stop>=start?((target>=start)&&(target<=stop)):\
                                            ((target>=start)||(target<=stop)))
// =============================================================================
// FUNCTION:we do the acknowledge to the data
// PARA  IN:
// PARA OUT:
// RETURN  :
// INSTRUCT:should do with the send window and conguest window too
// =============================================================================
static bool_t __ackdata(tagSocket *client, tagTcpHdr *hdr)
{
    u32                ackno;
    u32                acklen;
    tagCCB            *ccb;
    s32                err;
    s32                uerr;
    u32                usedtime;

    acklen = 0;
    ccb    = (tagCCB *)client->cb;
    ackno  = ntohl(hdr->ackno);
    //first we will deal the send buffer to release some space
    if((ackno  == ccb->sbuf.unackno)&&(ccb->sbuf.unacklen > 0))
    {
    	ccb->sbuf.dupacktims++;
    	if(ccb->sbuf.dupacktims > CN_TCP_FASTRETRANS)
    	{
            //do the fast retransmition
            ccb->rto = ccb->rto<<1;
            if(ccb->rto < CN_TCP_RTOMIN)
            {
                ccb->rto = CN_TCP_RTOMIN;
            }
            else if(ccb->rto > CN_TCP_RTOMAX)
            {
                ccb->rto = CN_TCP_RTOMAX;
            }

            ccb->ssthresh = ccb->cwnd>>1;
            if(ccb->ssthresh < (2*ccb->mss))
            {
                ccb->ssthresh = 2*ccb->mss;
            }
            ccb->cwnd = ccb->ssthresh + 3*ccb->mss;
            __resenddata(client);
    	}
    }
	//1,sndnxtno > unackno, then ackno must be between them
	//2,sndnxtno <= unackno,then ackno must be bigger than unack no or less than sndnxtno
    else if(((ccb->sbuf.sndnxtno > ccb->sbuf.unackno)&&\
	   ((ackno >ccb->sbuf.unackno)&&(ackno <= ccb->sbuf.sndnxtno)))||\
			((ccb->sbuf.sndnxtno < ccb->sbuf.unackno)&&\
			 ((ackno >ccb->sbuf.unackno)||(ackno <= ccb->sbuf.sndnxtno))))
	{
		acklen = ackno-ccb->sbuf.unackno;
		if(acklen > ccb->sbuf.unacklen)
		{
			acklen = ccb->sbuf.unacklen;
		}
		ccb->sbuf.unackoff   +=  acklen;
		if(ccb->sbuf.unackoff >= ccb->sbuf.buflenlimit)
		{
			ccb->sbuf.unackoff -= ccb->sbuf.buflenlimit;
		}
    	ccb->sbuf.unackno = ackno;
		ccb->sbuf.unacklen   -=  acklen;
		ccb->sbuf.buflenleft += acklen;
    	ccb->sbuf.dupacktims = 0;
    	//do the cwnd
        if(ccb->cwnd > ccb->ssthresh)
        {
            ccb->cwnd += ccb->cwnd/32;  //it is congestion avoid
        }
        else
        {
            ccb->cwnd += ccb->mss;
        }
        if(ccb->resndtimes == 0)
        {
        	//no resend happens,so measure the rto time
            usedtime = gTcpTicks - ccb->sbuf.rtotick;
            err = usedtime - ccb->sa;
            uerr = err>0?err:(-err);
            //scale the sa sd and rto
            ccb->sa = ccb->sa+(err>>3);
            ccb->sd = ccb->sd + ((uerr-ccb->sd)>>2);
            ccb->rto = ccb->sa + (ccb->sd<<2);
            if(ccb->rto < CN_TCP_RTOMIN)
            {
                ccb->rto = CN_TCP_RTOMIN;
            }
            else if(ccb->rto > CN_TCP_RTOMAX)
            {
                ccb->rto = CN_TCP_RTOMAX;
            }
        }
        else
        {
        	ccb->resndtimes = 0;//the remote has acknowledge some data,so just clear it
        }

        if((ccb->sbuf.buflenleft > ccb->sbuf.trigerlevel)||
           (0 ==(ccb->channelstat&CN_TCP_CHANNEL_STATASND)))
        {
        	client->iostat|= CN_SOCKET_IOWRITE;
            Multiplex_Set(client->ioselect, client->iostat);
     	    Lock_SempPost(ccb->sbuf.bufsync);
        }
        else
        {
            client->iostat&= (~CN_SOCKET_IOWRITE);
            Multiplex_Set(client->ioselect, client->iostat);
        }
        if(ccb->sbuf.sndnxtno == ccb->sbuf.unackno)  //noting to resend, so close the resend timer
        {
        	ccb->timerctrl &= (~CN_TCP_TIMER_RESEND);
        }
        ccb->resndtimer = ccb->rto;
	}
    else //ack the unseen data, we could do something or nothing
    {
    	//maybe nothing to ack or ack the unseen data
    }
    //update the window
    ccb->sndwnd = (ntohs(hdr->window))<<ccb->sndwndscale;
    if(ccb->sndwnd > ccb->sbuf.unacklen)
    {
        ccb->sndwnd -= ccb->sbuf.unacklen;
        ccb->timerctrl &= (~CN_TCP_TIMER_PERSIST);
    }
    else
    {
        ccb->sndwnd = 0;
        ccb->timerctrl |= CN_TCP_TIMER_PERSIST;
        ccb->persistimer = CN_TCP_TICK_PERSIST;
    }
    return true;
}

// =============================================================================
// FUNCTION:in this stat, we just care about the acknowledge to the syn we have send
// PARA  IN:
// PARA OUT:
// RETURN  :
// INSTRUCT:WE JUST CARE ABOUT THE ACK TO THE SYNACK WE SEND
// =============================================================================
static bool_t __rcvsyn_ms(tagSocket *client, tagTcpHdr *hdr, tagNetPkg *pkg)
{
    tagCCB   *ccb;
    tagSCB   *scb;
    tagSocket *server;

    //ack the data
    if(hdr->flags|CN_TCP_FLAG_ACK)
    {
        __ackdata(client,hdr);
        ccb = (tagCCB *)client->cb;
        if(ccb->sbuf.unackno == ccb->sbuf.sndnxtno) //ok, has ack the syn
        {
            ccb->machinestat = EN_TCP_MC_ESTABLISH;
            ccb->channelstat|= CN_TCP_CHANNEL_STATKRCV| CN_TCP_CHANNEL_STATKSND;
            ccb->sbuf.unackno = ccb->sbuf.sndnxtno;
            //close the syn resend timer
            ccb->timerctrl &= (~CN_TCP_TIMER_SYN);
            ccb->resndtimer = ccb->rto;
            //notice the server to accept
            server = ccb->server;
            scb = (tagSCB *)server->cb;
            server->iostat|= CN_SOCKET_IOACCEPT;
            Multiplex_Set(server->ioselect, server->iostat);
            Lock_SempPost(scb->acceptsemp);
        }
    }
    return true;
}
// =============================================================================
// FUNCTION:in this stat, we just care about the acknowledge to the syn ack we sent
// PARA  IN:
// PARA OUT:
// RETURN  :
// INSTRUCT:WE JUST CARE ABOUT THE SYNACK TO THE SYN WE SEND
// =============================================================================
static bool_t __sndsyn_ms(tagSocket *client, tagTcpHdr *hdr,tagNetPkg *pkg)
{
    tagCCB   *ccb;

    ccb = (tagCCB *)client->cb;
    //only care about the ack to the syn
    if(hdr->flags|CN_TCP_FLAG_ACK)
    {
        __ackdata(client,hdr);
        if((ccb->sbuf.unackno == ccb->sbuf.sndnxtno)&&\
           (hdr->flags == (CN_TCP_FLAG_ACK|CN_TCP_FLAG_SYN))) //ok, has ack the syn
        {
            ccb->machinestat = EN_TCP_MC_ESTABLISH;
            ccb->channelstat|= CN_TCP_CHANNEL_STATKRCV| CN_TCP_CHANNEL_STATKSND;
            ccb->sbuf.unackno = ccb->sbuf.sndnxtno;
            //close the syn resend timer
            ccb->timerctrl &= (~CN_TCP_TIMER_SYN);
            ccb->resndtimer = ccb->rto;
            dealtcpoption(ccb,hdr);
            ccb->rbuf.rcvnxt = ntohl(hdr->seqno) + 1;
            __sendflag(client,CN_TCP_FLAG_ACK,NULL,0,ccb->sbuf.sndnxtno);
            //notice the applications  the connect success
            Lock_SempPost(ccb->rbuf.bufsync);
        }
    }
    return true;
}
// =============================================================================
// FUNCTION：__stable_ms
// PARA  IN：
// PARA OUT：
// RETURN  ：
// INSTRUCT:ANY DATA WILL BE DEALT
// =============================================================================
static bool_t __stable_ms(tagSocket *client, tagTcpHdr *hdr,tagNetPkg *pkg)
{
    u32       seqno;
    tagCCB  *ccb;
    u32       rcvlen = 0;

    seqno =ntohl(hdr->seqno);
    ccb = (tagCCB *)client->cb;
    //deal the acknowledge to release the send buffer
    if(hdr->flags|CN_TCP_FLAG_ACK)
    {
        __ackdata(client, hdr);
    }
    //receive the data to the receive buffer
    if((NULL != pkg)&&(pkg->datalen > 0))
    {
    	if((0==(CN_TCP_CHANNEL_STATCONGEST&ccb->channelstat))&&\
    		       (CN_TCP_CHANNEL_STATKRCV&ccb->channelstat))
    	{
            //we will deal the pkg for two reson:
            //1,the channel permit to receive more
            //2,the pkg is sequance and is the last pkg
    		rcvlen = __rcvdata(client,seqno,pkg);
    	}
    	ccb->acktimes ++;
    }
    //if any fin received
    if((CN_TCP_FLAG_FIN & hdr->flags)&&((ccb->rbuf.rcvnxt == seqno)||(rcvlen > 0)))
    {
    	ccb->acktimes += CN_TCP_TICK_ACK;
        ccb->rbuf.rcvnxt++;
        ccb->machinestat= EN_TCP_MC_CLOSEWAIT;
        //could never to receive more data
        ccb->channelstat &= (~CN_TCP_CHANNEL_STATKRCV);
        client->iostat |= CN_SOCKET_IOREAD;
        Multiplex_Set(client->ioselect, client->iostat);
        Lock_SempPost(ccb->rbuf.bufsync);
        //--TODO,maybe signal an sigpipe signal
    }
    else if(CN_TCP_FLAG_SYN & hdr->flags) //maybe the acknowledge to the syn we sent has lost
    {
        __sendflag(client,CN_TCP_FLAG_ACK,NULL,0,ccb->sbuf.sndnxtno);
    }
    else
    {

    }
    //if any more data should be sent
    if(ccb->channelstat&CN_TCP_CHANNEL_STATKSND)
    {
        __senddata(client,__chkchannelsendlen(client));
    }
    if(ccb->acktimes >= CN_TCP_TICK_ACK)
    {
        __sendflag(client,CN_TCP_FLAG_ACK,NULL,0,ccb->sbuf.sndnxtno);
    }
    return true;
}
// =============================================================================
// FUNCTION：__finwait1_ms
// PARA  IN：
// PARA OUT：
// RETURN  ：
// INSTRUCT:ANY DATA WILL BE DEALT
// =============================================================================
static bool_t __finwait1_ms(tagSocket *client, tagTcpHdr *hdr,tagNetPkg *pkg)
{
    u32      seqno;
    tagCCB  *ccb;
    u32       rcvlen = 0;

    seqno =ntohl(hdr->seqno);
    ccb = (tagCCB *)client->cb;
    //deal the acn to release the package in the send buffer
    if(hdr->flags|CN_TCP_FLAG_ACK)
    {
        __ackdata(client, hdr);
    }
    //receive the data to the receive buffer
    if((NULL != pkg)&&(pkg->datalen > 0))
    {
    	if((0==(CN_TCP_CHANNEL_STATCONGEST&ccb->channelstat))&&\
    		       (CN_TCP_CHANNEL_STATKRCV&ccb->channelstat))
    	{
            //we will deal the pkg for two reson:
            //1,the channel permit to receive more
            //2,the pkg is sequance and is the last pkg
    		rcvlen = __rcvdata(client,seqno,pkg);
    	}
    	ccb->acktimes ++;
    }

    if(ccb->sbuf.unackno == ccb->sbuf.sndnxtno)             //FIN ACK --TIME_WAIT
    {
    	ccb->timerctrl &= (~CN_TCP_TIMER_FIN);  //CLOSE THE FIN TIMER
        ccb->machinestat= EN_TCP_MC_FINWAIT2;
    }
    //if any fin received
    if((CN_TCP_FLAG_FIN & hdr->flags)&&((ccb->rbuf.rcvnxt== seqno)||(rcvlen > 0)))
    {
    	ccb->acktimes += CN_TCP_TICK_ACK;
        ccb->rbuf.rcvnxt++;
        if(ccb->machinestat == EN_TCP_MC_FINWAIT1)
        {
            ccb->machinestat = EN_TCP_MC_CLOSING;
        }
        else
        {
            ccb->machinestat = EN_TCP_MC_TIMEWAIT;
            ccb->timerctrl = CN_TCP_TIMER_2MSL;
            ccb->mltimer = CN_TCP_TICK_2ML;
        }
        //could never to receive more data
        ccb->channelstat &= (~CN_TCP_CHANNEL_STATKRCV);
        client->iostat |= CN_SOCKET_IOREAD;
        Multiplex_Set(client->ioselect, client->iostat);
        Lock_SempPost(ccb->rbuf.bufsync);
        //--TODO,maybe signal an sigpipe signal
    }
    if(ccb->acktimes>= CN_TCP_TICK_ACK)
    {
        __sendflag(client,CN_TCP_FLAG_ACK,NULL,0,ccb->sbuf.sndnxtno);
    }
    return true;
}

// =============================================================================
// FUNCTION：__finwait2_ms
// PARA  IN：
// PARA OUT：
// RETURN  ：
// INSTRUCT:we must consider that if any more data comes but we are not to accept
//          more data,such as closed ,what should we do then?--TODO
// =============================================================================
static bool_t __finwait2_ms(tagSocket *client, tagTcpHdr *hdr,tagNetPkg *pkg)
{
    u32      seqno;
    tagCCB  *ccb;
    u32       rcvlen = 0;

    seqno =ntohl(hdr->seqno);
    ccb = (tagCCB *)client->cb;
    //deal the acn to release the package in the send buffer
    if(hdr->flags|CN_TCP_FLAG_ACK)
    {
        __ackdata(client, hdr);
    }
    //receive the data to the receive buffer
    if((NULL != pkg)&&(pkg->datalen > 0))
    {
    	if((0==(CN_TCP_CHANNEL_STATCONGEST&ccb->channelstat))&&\
    		       (CN_TCP_CHANNEL_STATKRCV&ccb->channelstat))
    	{
            //we will deal the pkg for two reson:
            //1,the channel permit to receive more
            //2,the pkg is sequance and is the last pkg
    		rcvlen = __rcvdata(client,seqno,pkg);
    	}
    	ccb->acktimes ++;
    }
    //if any fin received
    if((CN_TCP_FLAG_FIN & hdr->flags)&&((ccb->rbuf.rcvnxt== seqno)||(rcvlen > 0)))
    {
    	ccb->acktimes += CN_TCP_TICK_ACK;
        ccb->rbuf.rcvnxt++;
        ccb->machinestat = EN_TCP_MC_TIMEWAIT;
        ccb->timerctrl = CN_TCP_TIMER_2MSL;
        ccb->mltimer = CN_TCP_TICK_2ML;
        //could never to receive more data
        ccb->channelstat &= (~CN_TCP_CHANNEL_STATKRCV);
        client->iostat |= CN_SOCKET_IOREAD;
        Multiplex_Set(client->ioselect, client->iostat);
        Lock_SempPost(ccb->rbuf.bufsync);
        //--TODO,maybe signal an sigpipe signal
    }
    if(ccb->acktimes>= CN_TCP_TICK_ACK)
    {
        __sendflag(client,CN_TCP_FLAG_ACK,NULL,0,ccb->sbuf.sndnxtno);
    }
    return true;
}
// =============================================================================
// FUNCTION：__closing_ms
// PARA  IN：
// PARA OUT：
// RETURN  ：
// INSTRUCT:ANY DATA WILL BE SEE AS RETRANSMITION
// =============================================================================
static bool_t __closing_ms(tagSocket *client, tagTcpHdr *hdr,tagNetPkg *pkg)
{
    tagCCB  *ccb;

    ccb = (tagCCB *)client->cb;
    //deal the ack first
    if(hdr->flags|CN_TCP_FLAG_ACK)
    {
        __ackdata(client, hdr);
    }
    if(ccb->sbuf.unackno == ccb->sbuf.sndnxtno)
    {
        ccb->machinestat = EN_TCP_MC_TIMEWAIT;
    	ccb->timerctrl &= (~CN_TCP_TIMER_FIN);  //CLOSE THE FIN TIMER
        ccb->timerctrl |= CN_TCP_TIMER_2MSL;    //open the 2ml timer
        ccb->mltimer = CN_TCP_TICK_2ML;
     }
     if((NULL != pkg)&&(pkg->datalen > 0))
     {
    	 ccb->acktimes ++;
     }
     if(ccb->acktimes>= CN_TCP_TICK_ACK)
     {
         __sendflag(client,CN_TCP_FLAG_ACK,NULL,0,ccb->sbuf.sndnxtno);
     }
     return true;
}
// =============================================================================
// FUNCTION：__timewait_ms
// PARA  IN：
// PARA OUT：
// RETURN  ：
// INSTRUCT:ANY DATA WILL BE SEE AS RETRANSMITION
// =============================================================================
static bool_t __timewait_ms(tagSocket *client, tagTcpHdr *hdr,tagNetPkg *pkg)
{
    tagCCB  *ccb;
    ccb = (tagCCB *)client->cb;

    if((NULL != pkg)&&(pkg->datalen > 0))
    {
    	ccb->acktimes ++;
    }
    if(ccb->acktimes>= CN_TCP_TICK_ACK)
    {
        __sendflag(client,CN_TCP_FLAG_ACK,NULL,0,ccb->sbuf.sndnxtno);
    }
    return true;
}
// =============================================================================
// FUNCTION：__closewait_ms
// PARA  IN：
// PARA OUT：
// RETURN  ：
// INSTRUCT:ANY DATA WILL BE SEE AS RETRANSMITION
// =============================================================================
static bool_t __closewait_ms(tagSocket *client, tagTcpHdr *hdr,tagNetPkg *pkg)
{
    tagCCB  *ccb;
    ccb = (tagCCB *)client->cb;
    //deal the ack first
    if(hdr->flags|CN_TCP_FLAG_ACK)
    {
        __ackdata(client, hdr);
    }
    //if any fin received
	if((NULL != pkg)&&(pkg->datalen > 0))
	{
		ccb->acktimes ++;
	}
    //see if anymore data to send
	if(ccb->channelstat&CN_TCP_CHANNEL_STATKSND)
	{
		//SEND ANY DATA
		__senddata(client,__chkchannelsendlen(client));
	}
    if(ccb->acktimes>= CN_TCP_TICK_ACK)
    {
        __sendflag(client,CN_TCP_FLAG_ACK,NULL,0,ccb->sbuf.sndnxtno);
    }
    return true;
}

// =============================================================================
// FUNCTION：__lastack_ms
// PARA  IN：
// PARA OUT：
// RETURN  ：
// INSTRUCT:ANY DATA WILL BE SEE AS RETRANSMITION
// =============================================================================
static bool_t __lastack_ms(tagSocket *client, tagTcpHdr *hdr,tagNetPkg *pkg)
{
    tagCCB         *ccb;

    ccb = (tagCCB *)client->cb;
    //deal the ack first
    if(hdr->flags|CN_TCP_FLAG_ACK)
    {
        __ackdata(client, hdr);
    }
	if(ccb->sbuf.unackno == ccb->sbuf.sndnxtno)
	{
		ccb->machinestat = EN_TCP_MC_2FREE;
    	ccb->timerctrl &= (~CN_TCP_TIMER_FIN);  //CLOSE THE FIN TIMER
	}
	if((NULL != pkg)&&(pkg->datalen > 0))
	{
		ccb->acktimes ++;
	}
	//so it will be free next
    if(ccb->acktimes>= CN_TCP_TICK_ACK)
    {
        __sendflag(client,CN_TCP_FLAG_ACK,NULL,0,ccb->sbuf.sndnxtno);
    }
    return true;
}

//this function to deal with the remote package received in the socket
static bool_t __dealrecvpkg(tagSocket *client, tagTcpHdr *hdr,tagNetPkg *pkg)
{
    tagCCB  *ccb;
    ccb = (tagCCB *)client->cb;
    if((hdr->flags & CN_TCP_FLAG_RST)&&(ccb->rbuf.rcvnxt == htonl(hdr->seqno)))
    {
        __ResetCCB(ccb, EN_TCP_MC_2FREE);           //general deal the reset socket
        client->iostat |= CN_SOCKET_IOREAD|CN_SOCKET_IOWRITE;
        Multiplex_Set(client->ioselect, client->iostat);
    }
    else
    {
        //rcv the pkg means we still in the communication, so reset the keepalive
        switch(ccb->machinestat)
        {
            case EN_TCP_MC_SYNSNT:
                __sndsyn_ms(client, hdr, pkg);
                break;
            case EN_TCP_MC_SYNRCV:
                __rcvsyn_ms(client, hdr,pkg);
                break;
            case EN_TCP_MC_ESTABLISH:
                __stable_ms(client, hdr,pkg);
                break;
            case EN_TCP_MC_FINWAIT1:
                __finwait1_ms(client, hdr, pkg);
                break;
            case EN_TCP_MC_FINWAIT2:
                __finwait2_ms(client, hdr,pkg);
                break;
            case EN_TCP_MC_CLOSING:
                __closing_ms(client,hdr,pkg);
                break;
            case EN_TCP_MC_TIMEWAIT:
                __timewait_ms(client, hdr, pkg);
                break;
            case EN_TCP_MC_CLOSEWAIT:
                __closewait_ms(client,hdr,pkg);
                break;
            case EN_TCP_MC_LASTACK:
                __lastack_ms(client,hdr,pkg);
                break;
            default:
                break;
        }
    }
    return true;
}

static tagSocket* __newclient(tagSocket *server, tagTcpHdr *hdr,\
                          u32 ipdst,u16 portdst,u32 ipsrc, u16 portsrc)
{
    tagSCB *scb;
    tagCCB *ccb;
    tagSocket *result= NULL;

    scb = (tagSCB *)server->cb;
    if((0 ==(CN_SOCKET_CLOSE&server->sockstat))&&\
       (CN_SOCKET_LISTEN&server->sockstat)&&\
       (scb->pendnum <scb->backlog)&&\
       (CN_TCP_FLAG_SYN&hdr->flags)&&\
       (0 == (CN_TCP_FLAG_ACK&hdr->flags)))  //could accept more in the pending
    {
        result = SocketMalloc();
        if(NULL != result)
        {
            ccb = __CreateCCB();
            if(NULL == ccb)
            {
                SocketFree(result);
                result = NULL;
            }
            else
            {
                result->cb = (void *)ccb;
                result->sockstat |=CN_SOCKET_CLIENT|CN_SOCKET_PROBLOCK|CN_SOCKET_PRONAGLE;
                result->element.v4.iplocal = ipdst;
                result->element.v4.portlocal = portdst;
                result->element.v4.ipremote = ipsrc;
                result->element.v4.portremote = portsrc;
                result->af_inet = AF_INET;
                result->proto = &gTcpProto;
                ccb->rbuf.rcvnxt = htonl(hdr->seqno)+1;
                ccb->sndwnd = htons(hdr->window);
                dealtcpoption(ccb,hdr);

                //ok, now send the syn ack and open the syn timer
                __sendflag(result,CN_TCP_FLAG_ACK|CN_TCP_FLAG_SYN,\
                          (void *)&sgSynOptDefault,sizeof(sgSynOptDefault),ccb->sbuf.sndnxtno);
                result->nxt = scb->clst;
                scb->clst = result;
                scb->pendnum++;
                ccb->server = server;
                ccb->timerctrl = CN_TCP_TIMER_SYN;
                ccb->resndtimer = ccb->rto;
                ccb->machinestat = EN_TCP_MC_SYNRCV;
                ccb->sbuf.sndnxtno++;
            }
        }
    }

    return result;

}
static tagSocket* __tcpmatchclient(tagSocket *server, u32 ip, u16 port)
{
    tagSocket *result = NULL;
    tagSocket *client;
    tagSCB   *scb;

    scb = (tagSCB *)server->cb;
    client = scb->clst;
    while(NULL != client)
    {
        if((ip == client->element.v4.ipremote)&&(port == client->element.v4.portremote))
        {
            result = client;
            break;
        }
        else
        {
            client = client->nxt;
        }
    }
    return result;
}

static bool_t __tcprcvdealv4(u32 ipsrc, u32 ipdst,  tagNetPkg *pkg, u32 devfunc)
{

    u16         portdst;
    u16         portsrc;
    tagTcpHdr  *hdr;
    tagSocket  *server;
    tagSocket  *client;
    tagSocket  *sock;
    u32         cpyhdr[15];

    if(0 == (devfunc &CN_IPDEV_TCPICHKSUM))
    {
        if(false == __rcvchecksum(ipdst, ipsrc, pkg))
        {
            return true;  //chksum failed
        }
    }
    hdr = (tagTcpHdr *)(pkg->buf + pkg->offset);
    pkg->offset += (hdr->hdroff>>4)*4;
    if(pkg->datalen> (hdr->hdroff>>4)*4)
    {
        pkg->datalen -= (hdr->hdroff>>4)*4;
    }
    else
    {
        pkg = NULL;
    }
    //cpy the hdr to the thread stack avoid the align error
    memcpy((void *)cpyhdr,(void *)hdr,(hdr->hdroff>>4)*4);
    hdr = (tagTcpHdr *)cpyhdr;

    //ok,now we deal the pkg now, the pkg check success
    portdst = hdr->portdst;
    portsrc = hdr->portsrc;

    //if any client match this pkg
    Lock_MutexPend(pTcpHashTab->tabsync, CN_TIMEOUT_FOREVER);
    if((sock = __hashSocketSearch(ipdst,portdst,ipsrc,portsrc)) != NULL)
    {
        //get the communicate client
        client = sock;
        Lock_MutexPend(client->sync,CN_TIMEOUT_FOREVER);
        __dealrecvpkg(client,hdr,pkg);
        Lock_MutexPost(client->sync);
    }
    else if((sock = __hashSocketSearch(INADDR_ANY,portdst,ipsrc,portsrc)) != NULL)
    {
        //the inaddr_any client matches
        client = sock;
        Lock_MutexPend(client->sync,CN_TIMEOUT_FOREVER);
        __dealrecvpkg(client,hdr,pkg);
        Lock_MutexPost(client->sync);
    }
    else if((sock = __hashSocketSearch(ipdst,portdst,0,0))!= NULL)
    {
        //the specified server matches
        server = sock;
        Lock_MutexPend(server->sync,CN_TIMEOUT_FOREVER);
        client = __tcpmatchclient(server,ipsrc,portsrc);
        if(NULL == client)
        {
            client = __newclient(server,hdr,ipdst, portdst, ipsrc, portsrc);
            if(NULL == client) //could not add any more client
            {
                __resetremoteraw(ipdst, portdst,ipsrc, portsrc,ntohl(hdr->seqno)+1);
            }
        }
        else
        {
            __dealrecvpkg(client,hdr,pkg);
        }
        Lock_MutexPost(server->sync);
    }
    else if((sock = __hashSocketSearch(INADDR_ANY,portdst,0,0))!= NULL)
    {
        //the inaddr_any server matches
        server = sock;
        Lock_MutexPend(server->sync,CN_TIMEOUT_FOREVER);
        client = __tcpmatchclient(server,ipsrc,portsrc);
        if(NULL == client)
        {
            client = __newclient(server,hdr,ipdst, portdst, ipsrc, portsrc);
            if(NULL == client) //could not add any more client
            {
                __resetremoteraw(ipdst, portdst,ipsrc, portsrc,ntohl(hdr->seqno)+1);
            }
        }
        else
        {
            __dealrecvpkg(client,hdr,pkg);
        }
        Lock_MutexPost(server->sync);
    }
    else   //no port matches, so just reset it
    {
        if(0==(hdr->flags & CN_TCP_FLAG_RST))
        {
            __resetremoteraw(ipdst, portdst,ipsrc, portsrc,ntohl(hdr->seqno)+1);
        }
    }
    Lock_MutexPost(pTcpHashTab->tabsync);

    return true;

}

// =============================================================================
// FUNCTION:__recvProcess,use this function to receive the package
// PARA  IN:ipsrc,the source ip
//          ipdst,the destination ip
//          package,the tcp package
//          checksum,if has done the checksum
// PARA OUT:
// RETURN  :
// INSTRUCT:true success while false failed
// =============================================================================
bool_t __recvProcess(enum_ipv_t  ver,ptu32_t ipsrc, ptu32_t ipdst,  tagNetPkg *pkg, tagNetDev *dev)
{
    bool_t result = false;
    u32 devfunc;

    if(ver == EN_IPV_4)
    {
        devfunc = NetDevGetFunc(dev);
        result = __tcprcvdealv4((u32)ipsrc,(u32)ipdst,pkg,devfunc);
    }

    return result;
}

// =============================================================================
// FUNCTION:deal the tcp timer
// PARA  IN:
// PARA OUT:
// RETURN  :
// INSTRUCT:true success while false failed
// =============================================================================
static bool_t __dealclienttimer(tagSocket *client)
{
	u8        flag;
    tagCCB   *ccb;

    ccb = (tagCCB *)client->cb;
    if(ccb->timerctrl&CN_TCP_TIMER_LINGER)   //deal the linger timer
    {
        if( 0 == ccb->lingertimer)
        {
            __ResetCCB(ccb,EN_TCP_MC_2FREE);
        }
        else
        {
            ccb->lingertimer--;
        }
    }

    if(ccb->timerctrl &CN_TCP_TIMER_2MSL)    //deal the 2ml timer
    {
        if(ccb->mltimer == 0)
        {
            __ResetCCB(ccb,EN_TCP_MC_2FREE);
        }
        else
        {
            ccb->mltimer--;
        }
    }
    //chk all the transmissions if any package need retransmission
    if(ccb->timerctrl &(CN_TCP_TIMER_SYN|CN_TCP_TIMER_RESEND|CN_TCP_TIMER_RESEND))
    {
        if(ccb->resndtimer == 0)
        {

        	if(ccb->resndtimes > CN_TCP_RSNDMAX)
        	{
                __ResetCCB(ccb,EN_TCP_MC_2FREE);  //resend timeout
        	}
        	else
        	{
        	    if(ccb->timerctrl & CN_TCP_TIMER_SYN)  //syn
        	    {
        	    	//we will help you to send the syn
        	    	flag = CN_TCP_FLAG_SYN;
        	    	if(ccb->machinestat == EN_TCP_MC_SYNRCV)
        	    	{
        	    		flag |= CN_TCP_FLAG_ACK;
        	    	}
        			__sendflag(client,flag,&sgSynOptDefault,sizeof(sgSynOptDefault),ccb->sbuf.unackno);
        	    }
        	    else if(ccb->timerctrl & CN_TCP_TIMER_RESEND) //resend
        	    {
            		//do the resend
            		__resenddata(client);

        	    }
        	    else  //fin
        	    {
            		__sendflag(client,CN_TCP_FLAG_FIN|CN_TCP_FLAG_ACK,NULL,0,ccb->sbuf.sndnxtno-1);
        	    }
        		//do the rto update
                ccb->rto = ccb->rto<<1;
                if(ccb->rto < CN_TCP_RTOMIN)
                {
                    ccb->rto = CN_TCP_RTOMIN;
                }
                else if(ccb->rto > CN_TCP_RTOMAX)
                {
                    ccb->rto = CN_TCP_RTOMAX;
                }
                //because we has dealt an timeout retransmition, so we must deal the
                //congestion window
                ccb->ssthresh = ccb->cwnd>>1;
                if(ccb->ssthresh < (2*ccb->mss))
                {
                    ccb->ssthresh = 2*ccb->mss;
                }
                ccb->cwnd = ccb->mss;
                ccb->resndtimer = ccb->rto;
        		ccb->resndtimes++;
        	}
        }
        else
        {
    		ccb->resndtimer--;
        }
    }
    if(ccb->timerctrl &CN_TCP_TIMER_CORK)     //do the socket cork
    {
        //this is the to-do deal
    }
    if(ccb->timerctrl &CN_TCP_TIMER_KEEPALIVE)   //do the socket keepalive timer
    {
        if(ccb->keeptimer == 0)
        {
            __sendflag(client,CN_TCP_FLAG_ACK,NULL,0,ccb->sbuf.sndnxtno);
            ccb->keeptimer = CN_TCP_TICK_KEEPALIVE;
        }
        else
        {
            ccb->keeptimer--;
        }
    }
    if(ccb->timerctrl &CN_TCP_TIMER_PERSIST)   //do the socket persist
    {
        if(0 == ccb->persistimer)
        {
        	if(ccb->sbuf.datalen > 0)
        	{
                __senddata(client,1);
        	}
        	else
        	{
        		__sendflag(client,CN_TCP_FLAG_ACK,NULL,0,ccb->sbuf.sndnxtno);
        	}
            ccb->persistimer = CN_TCP_TICK_PERSIST;
        }
        else
        {
            ccb->persistimer--;
        }
    }
    if(ccb->acktimes > 0)
    {
        __sendflag(client,CN_TCP_FLAG_ACK,NULL,0,ccb->sbuf.sndnxtno);
    }
    if(NULL != ccb->pkgrecomblst)
    {
    	PkgTryFreeQ(ccb->pkgrecomblst);
    	ccb->pkgrecomblst = NULL;
    	ccb->pkgrecomblen = 0;
    }
    return true;
}
// =============================================================================
// FUNCTION:the tcp tick main function
// PARA  IN:
// PARA OUT:
// RETURN  :
// INSTRUCT:never return
// =============================================================================
static ptu32_t __tcptick(void)
{
    tagSocket    *sock;
    tagSocket    *server;
    tagSocket    *client;
    tagSocket    *clientnxt;
    tagSocket    *clientpre;
    tagSCB       *scb;
    tagCCB       *ccb;
    int i;
    while(1)
    {
        for(i = 0; i <pTcpHashTab->tablen; i ++)
        {
        	//each hash number we will lock and unlock ,so left some time for others
            Lock_MutexPend(pTcpHashTab->tabsync, CN_TIMEOUT_FOREVER);
            sock = pTcpHashTab->array[i];
            while(NULL != sock)
            {
                if(CN_SOCKET_CLIENT&sock->sockstat)//this is the client
                {
                    client = sock;
                    Lock_MutexPend(client->sync, CN_TIMEOUT_FOREVER);
                    ccb = (tagCCB *)client->cb;
                    if((CN_SOCKET_CLOSE&client->sockstat)&&\
                        (EN_TCP_MC_2FREE == ccb->machinestat))
                    {
                        __ResetCCB(ccb,EN_TCP_MC_2FREE);
                        __DeleCCB(ccb);
                        sock = client->nxt;
                        __hashSocketRemove(client);
                        SocketFree(client);
                    }
                    else
                    {
                        __dealclienttimer(client);
                        sock = client->nxt;
                        Lock_MutexPost(client->sync);
                    }
                }//end for the client
                else//this is an server, we should deal the client hang on it
                {
                    server = sock;
                    Lock_MutexPend(server->sync, CN_TIMEOUT_FOREVER);
                    scb = (tagSCB *)server->cb;
                    client = scb->clst;
                    clientpre = client;
                    while(NULL != client)  //deal all the client
                    {
                        __dealclienttimer(client);
                        ccb = (tagCCB *)client->cb;
                        if(EN_TCP_MC_2FREE == ccb->machinestat)
                        {
                        	//remove it from the queue
                            if(client == scb->clst)
                            {
                            	scb->clst = client->nxt;
                            	clientpre = client->nxt;
                            }
                            else
                            {
                            	clientpre->nxt = client->nxt;
                            }
                        	clientnxt = client->nxt;
                        	client->nxt = NULL;
                            __ResetCCB(ccb,EN_TCP_MC_2FREE);
                            __DeleCCB(ccb);
                            SocketFree(client);  //free the client
                        }
                        else
                        {
                        	clientpre = client;
                            clientnxt = client->nxt;
                        }
                        client = clientnxt;
                    }
                    //deal the server it self
                    sock = server->nxt;
                    Lock_MutexPost(server->sync);
                }
            }
            Lock_MutexPost(pTcpHashTab->tabsync);
        }
        gTcpTicks++;
        Djy_EventDelay(CN_TCP_TICK_TIME);  //the timer tick
    }
    return 0;
}

static char *gCCBLinkStat[]=
{
    "EN_TCP_MC_CREAT",
    "EN_TCP_MC_SYNRCV",
    "EN_TCP_MC_SYNSNT",
    "EN_TCP_MC_ESTABLISH",
    "EN_TCP_MC_FINWAIT1",
    "EN_TCP_MC_FINWAIT2",
    "EN_TCP_MC_CLOSING",
    "EN_TCP_MC_TIMEWAIT",
    "EN_TCP_MC_CLOSEWAIT",
    "EN_TCP_MC_LASTACK",
    "EN_TCP_MC_2FREE"
};

static void __tcpdebugsockinfo(tagSocket *sock,char *prefix)
{
    printf("%s:iplocal :%s    portlocal :%d\r\n",\
            prefix,inet_ntoa(*(struct in_addr*)&sock->element.v4.iplocal),ntohs(sock->element.v4.portlocal));
    printf("%s:ipremote:%s    portremote:%d\r\n",\
            prefix,inet_ntoa(*(struct in_addr*)&sock->element.v4.ipremote),ntohs(sock->element.v4.portremote));
    printf("%s:iostat  :0x%08x sockstat:0x%08x    private:0x%08x\n\r",\
            prefix,sock->iostat,sock->sockstat,sock->private);
    printf("%s:syncstat:%d\n\r",prefix,sock->sync->enable);
    printf("%s:errno   :%d\n\r",prefix,sock->errorno);
}

static void __tcpdebugccb(tagCCB *ccb,char *prefix)
{
	//machine state
    printf("%s:machinestat:%s\n\r",prefix,gCCBLinkStat[ccb->machinestat]);
    //channel stat
    printf("%s:channelstat:apprcv:%s\n\r",prefix,\
    		ccb->channelstat&CN_TCP_CHANNEL_STATARCV?"enable":"disable");
    printf("%s:channelstat:appsnd:%s\n\r",prefix,\
    		ccb->channelstat&CN_TCP_CHANNEL_STATASND?"enable":"disable");
    printf("%s:channelstat:knlrcv:%s\n\r",prefix,\
    		ccb->channelstat&CN_TCP_CHANNEL_STATKRCV?"enable":"disable");
    printf("%s:channelstat:knlsnd:%s\n\r",prefix,\
    		ccb->channelstat&CN_TCP_CHANNEL_STATKSND?"enable":"disable");
    //send buffer
    printf("%s:sndbuffer:buflen:%04d bufleft:%04d  unacklen:%04d datalen:%04d\n\r",prefix,\
    		ccb->sbuf.buflenlimit,ccb->sbuf.buflenleft,ccb->sbuf.unacklen,ccb->sbuf.datalen);
    printf("%s:sndbuffer:unackoff:%04d sndnxtoff:%04d dataoff:%04d  trigger:%04d\n\r",prefix,\
    		ccb->sbuf.unackoff,ccb->sbuf.sndnxtoff,ccb->sbuf.dataoff,ccb->sbuf.trigerlevel);
    printf("%s:sndbuffer:unackno:0x%08x sndnxtno:0x%08x \n\r",prefix,\
    		ccb->sbuf.unackno,ccb->sbuf.sndnxtno);
    printf("%s:sndbuffer:timeout:0x%08x rtotick:0x%08x dupacks:%04d sync:%s\n\r",prefix,\
    		ccb->sbuf.timeout,ccb->sbuf.rtotick,ccb->sbuf.dupacktims,ccb->sbuf.bufsync->lamp_counter?"enable":"disable");
    //receive buffer
    printf("%s:rcvbuf:buflen:%04d datalen:%04d trigerlevel:%04d\n\r",\
            prefix,ccb->rbuf.buflenlimit,ccb->rbuf.buflen,ccb->rbuf.trigerlevel);
    printf("%s:rcvbuf:rcvnxtno:0x%08x timeout:0x%08x sync:%s\n\r",prefix,\
            ccb->rbuf.rcvnxt,ccb->rbuf.timeout,ccb->rbuf.bufsync->lamp_counter?"enable":"disable");
    //recomb queue
    printf("%s:recomb:len:%d\n\r",prefix,ccb->pkgrecomblen);
    //windows
    printf("%s:mss:%04d sndwnd:%d sndwndscale:%d\n\r",prefix,ccb->mss,ccb->sndwnd,ccb->sndwndscale);
    printf("%s:cwnd :%d ssh:%d \r\n",prefix,ccb->cwnd,ccb->ssthresh);
    //resend
    printf("%s:resndtimer:%04d resndtimes:%04d acktimes:%04d\r\n",prefix,\
    		ccb->resndtimer,ccb->resndtimes,ccb->acktimes);
    //timer
    printf("%s:syntimer:%s  \r\n",prefix,ccb->timerctrl&CN_TCP_TIMER_SYN?"enable":"disable");
    printf("%s:2mltimer:%s  \r\n",prefix,ccb->timerctrl&CN_TCP_TIMER_2MSL?"enable":"disable");
    printf("%s:kpatimer:%s  \r\n",prefix,ccb->timerctrl&CN_TCP_TIMER_KEEPALIVE?"enable":"disable");
    printf("%s:pestimer:%s  \r\n",prefix,ccb->timerctrl&CN_TCP_TIMER_PERSIST?"enable":"disable");
    printf("%s:cortimer:%s  \r\n",prefix,ccb->timerctrl&CN_TCP_TIMER_CORK?"enable":"disable");
    printf("%s:ligtimer:%s  \r\n",prefix,ccb->timerctrl&CN_TCP_TIMER_LINGER?"enable":"disable");
    printf("%s:sndtimer:%s  \r\n",prefix,ccb->timerctrl&CN_TCP_TIMER_RESEND?"enable":"disable");
    printf("%s:fintimer:%s  \r\n",prefix,ccb->timerctrl&CN_TCP_TIMER_FIN?"enable":"disable");
	return;
}

static void __tcpdebugscb(tagSCB *scb,char *prefix)
{
    printf("%s:backlog  :%d pendnum:%d\n\r",prefix,scb->backlog,scb->pendnum);
    printf("%s:accepttm :0x%08x acceptsync:%d\n\r",prefix,scb->accepttime,scb->acceptsemp->lamp_counter);
}

#define CN_TCP_DEBUG_PREFIX  "         "
static void __debug(tagSocket *sock,char *filter)
{
    tagCCB   *ccb;
    tagSCB   *scb;

    if(sock->sockstat&CN_SOCKET_CLIENT)
    {
        ccb = (tagCCB *)sock->cb;
        printf("TCPCLIENT:fd:%d address:0x%08x\n\r",sock->sockfd,(u32)sock);
        __tcpdebugsockinfo(sock,CN_TCP_DEBUG_PREFIX);
        __tcpdebugccb(ccb,CN_TCP_DEBUG_PREFIX);
    }
    else
    {
        scb = (tagSCB *)sock->cb;
        printf("TCPSERVER:fd:%d address:0x%08x\n\r",sock->sockfd,(u32)sock);
        __tcpdebugsockinfo(sock,CN_TCP_DEBUG_PREFIX);
        __tcpdebugscb(scb,CN_TCP_DEBUG_PREFIX);
    }
}
// =============================================================================
// FUNCTION:this is tcp main initialize function
// PARA  IN:
// PARA OUT:
// RETURN  :
// INSTRUCT:true success while false failed
// =============================================================================
bool_t TcpInit(ptu32_t para)
{
    bool_t result = false;
    u16  evttID;
    u16  eventID;

    if(false == __hashTabInit(gTcpClientNum+gTcpServerNum))
    {
        return result;
    }
    if(false == __initCB(gTcpClientNum,gTcpServerNum))
    {
        return result;
    }

    sgSynOptDefault.mss.kind = EN_TCP_OPT_MSS;             //do the syn option
    sgSynOptDefault.mss.len = 4;
    sgSynOptDefault.mss.mss = htons(gTcpMssDefault);
    sgSynOptDefault.winscale.resvered = EN_TCP_OPT_NONE;
    sgSynOptDefault.winscale.kind = EN_TCP_OPT_SCALE;
    sgSynOptDefault.winscale.len = 3;
    sgSynOptDefault.winscale.shiftbits = 0;

    eventID = CN_EVENT_ID_INVALID;//create the tcp ticker thread
    evttID= Djy_EvttRegist(EN_CORRELATIVE, gTcpTickTaskPrior, 0, 1,
            (ptu32_t (*)(void))__tcptick,NULL, gTcpTickTaskStack, "TcpTick");

    if (evttID != CN_EVTT_ID_INVALID)
    {
        eventID = Djy_EventPop(evttID, NULL, 0, 0, 0, 0);
        if(eventID != CN_EVENT_ID_INVALID)
        {
             Djy_EvttUnregist(evttID);
        }
    }
    if(CN_EVENT_ID_INVALID == eventID)
    {
        printk("%s:ERR:TCP TICKTASK CREATE FAILED\n\r,__FUNCTION__");
        goto EXIT_TICKSTASKFAILED;
    }

    memset((void *)&gTcpProto,0,sizeof(gTcpProto));
    gTcpProto.socket = __socket;                //register the tcp to the  tpl
    gTcpProto.bind = __bind;
    gTcpProto.listen = __listen;
    gTcpProto.accept = __accept;
    gTcpProto.connect = __connect;
    gTcpProto.getsockopt = __getsockopt;
    gTcpProto.recv = __recv;
    gTcpProto.recvfrom = NULL;
    gTcpProto.send = __send;
    gTcpProto.sendto = NULL;
    gTcpProto.setsockopt = __setsockopt;
    gTcpProto.shutdown = __shutdown;
    gTcpProto.close = __closesocket;
    gTcpProto.debuginfo = __debug;

    if((false ==TPL_RegisterProto(AF_INET,SOCK_STREAM,IPPROTO_TCP, &gTcpProto))||\
       (false ==TPL_RegisterProto(AF_INET,SOCK_STREAM,0, &gTcpProto))||\
       (false == IpRegisterTplHandler(IPPROTO_TCP,__recvProcess)))
    {
        printk("%s:ERR:TCP PROTO REGISTER FAILED\n\r,__FUNCTION__");
        goto EXIT_REGISTERTCPFAILED;
    }
    result = true;
    return result;

EXIT_REGISTERTCPFAILED:
EXIT_TICKSTASKFAILED:
    return false;
}








