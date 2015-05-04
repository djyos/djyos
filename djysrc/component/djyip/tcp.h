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
// 模块描述: Tcp.h:
// 模块版本: V1.00
// 创建人员: Zqf
// 创建时间: 11:26:50 AM/Sep 10, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================


#ifndef TCP_H_
#define TCP_H_

#include "stdint.h"
#include "stddef.h"

#include "os.h"

#include "socket.h"

#pragma pack(1)

typedef struct _tagTcpHdr
{
	u16 portsrc;     //source port
	u16 portdst;     //dstination port
	u32 seqno;       //sequence number
	u32 ackno;       //acknowledge number
#if(CN_CFG_BYTE_ORDER == CN_CFG_BIG_ENDIAN)
	u8  hdrlen:4;    //header len (word 4bytes)
	u8  reserv1:4;   //低4位
#else
	u8  reserv1:4;   //低4位
	u8  hdrlen:4;    //header len (word 4bytes)
#endif
	u8  flags;       //some flags like sync and ack
#define CN_TCP_FLAG_FIN  (1<<0)
#define CN_TCP_FLAG_SYN  (1<<1)
#define CN_TCP_FLAG_RST  (1<<2)
#define CN_TCP_FLAG_PSH  (1<<3)
#define CN_TCP_FLAG_ACK  (1<<4)
#define CN_TCP_FLAG_URG  (1<<5)
#define CN_TCP_FLAG_MSK  0x3f
	u16 window;      //the remote window size
	u16 chksum;      //package checksum
	u16 urgencyp;    //urgency pointer
	u8  opt[0];      //选项
}tagTcpHdr;
// 用于TCP校验的伪部首
typedef struct _tagTcpPseudoHdr 
{
	u32 ipsrc;
	u8	ipdst;
	u8	zero;
	u8	protocol;
	u16	len;	       // 传输层的长度
}tagTcpPseudoHdr;

typedef struct _tagWindowScale
{
	u8 resvered;
	u8 kind;
	u8 len;
	u8 shiftbits;
}tagWindowScale;

typedef struct _tagMss
{
	u8  kind;
	u8  len;
	u16 mss;
}tagMss;

typedef struct _tagSynOption
{
	tagMss mss;
	tagWindowScale winscale;
}tagSynOption;
#pragma pack()
//通信端的状态
enum _EN_TCPSTATE
{
	EN_TCPSTATE_CLOSED = 0,   //已经被关闭
	EN_TCPSTATE_CREATED ,     //刚被创建的状态，一般而言只有主动创建的才会在此状态
	EN_TCPSTATE_SSYNRCV,      //服务器端收到了FIN
	EN_TCPSTATE_CSYNSNT,      //客户端已经发送了FIN信号
	EN_TCPSTATE_ESTABLISHED,  //稳定链接状态
	EN_TCPSTATE_FIN_WAITE1,   //主动关闭发送了FIN
	EN_TCPSTATE_FIN_WAITE2,   //主动关闭发送了FIN并且收到了ACK
	EN_TCPSTATE_CLOSING,      //主动关闭发送了FIN(未收到ACK)，收到对面的FIN
	EN_TCPSTATE_TIME_WAIT,    //主动关闭发送FIN并且接收到了FIN，2ML超时
	EN_TCPSTATE_CLOSE_WAIT,   //被动关闭收到了对面的FIN
	EN_TCPSTATE_LAST_ACK,     //被动关闭发送了FIN
	EN_TCPSTATE_2FREE,        //待释放状态
	EN_TCPSTATE_FREE,         //释放状态
};

enum _EN_TCPSEND_REASON
{
	EN_TCPSEND_SYN2SEND         = 0,    //需要发送SYN,一般发生在主动链接的时候
	EN_TCPSEND_DATA2SEND           ,    //有数据待发
	EN_TCPSEND_DATAPROBE           ,    //探测对面端口，一般的为当前发送窗口为0，且有数据要发送
	                                    //有且仅发送一包数据，不计入窗口管理中
	EN_TCPSEND_FIN2SEND            ,    //要发送FIN
};

typedef struct _tagTcpAlgorithm
{
    //TCP协议控制
    u8 oobinline   :1;     //1,带外数据不在队里，0 带外数据在队列里面
    u8 trigerlevel :1;     //1,收发有触发水平，0收发无触发水平
    u8 nagle       :1;     //1,开启nagle算法
    u8 linger      :1;     //延时关闭是否打开
}tagTcpAlgorithm;

//tcp communication channel controller
typedef struct
{
    //tcp channel control
	//fin,0 means no fin to snd,1 means fin to snd
//    u8 fin :1;
//    //when accept. the sestablished and open =0 will bingo
//    u8 open :1;
//    //when closed, this bit will be set
//    u8 close:1;
//    //the following control the app read/write and kernel read/write
//    //they all means more new data,not include the retransmit
//    u8 asnd:1;    //1 means the app could write while 0 not
//    u8 arcv:1;    //1 means the app could read while 0 not
//    u8 ksnd:1;    //1 means the stack kernel could snd more new data while 0 not
//    u8 krcv:1;    //1 means the stack kernel could rcv more new data while 0 not
    u8 fin;
    //when accept. the sestablished and open =0 will bingo
    u8 open;
    //when closed, this bit will be set
    u8 close;
    //the following control the app read/write and kernel read/write
    //they all means more new data,not include the retransmit
    u8 asnd;    //1 means the app could write while 0 not
    u8 arcv;    //1 means the app could read while 0 not
    u8 ksnd;    //1 means the stack kernel could snd more new data while 0 not
    u8 krcv;    //1 means the stack kernel could rcv more new data while 0 not
}tagTcpChannelstate;
//rcv fin makes krcv 0
//snt fin makes ksnd 0
//shutdown_rd makes krcv and arcv 0
//shutdown_wr makes asnd 0
//close makes the krcv 0 arcv 0 asnd 0

typedef struct _tagTcpIpOption
{
	u8 ttl;
	u8 tos;
}tagTcpIpOption;

#define CN_TCP_LISTEN_DEFAULT   5
//TCP的分段数据（UNACK 乱序重组数据等）
#define CN_TCP_RSNDTIMES_LIMIT  8

//the node used to hold the snd but not acked data,waiting the remote to ack
typedef struct _tagTcpSndUnackNode
{
	struct _tagTcpSndUnackNode  *nxt; //used for the list
	u32 secno;           //the node number for the debug
	u32 masks;           //set the maks for the debug
	u32 seqno;           //the pkg seqno
	u16 datalen;         //the len that the sec hold
	u8  flags;           //the tcp flags used to snd
	u8  rsndtimes;       //which means how many times the section has been rsnd
	s64 sndtime;         //which means when the pkg is snt
	s64 timeout;         //the time when the pkg timeout,used for retransmiton
	tagNetPkg  *lsthead; //head pkg that not acked yet
//	tagNetPkg  *lsttail; //tail pkg that not acked yet
}tagTcpSndUnackNode;
//the node used to hold the not sequence data, waiting the remote resnd to queue
typedef struct _tagTcpReCombNode
{
	struct _tagTcpReCombNode  *nxt;            //used for the list
	u32                       seqno;           //the pkg seqno
	u32                       datalen;         //the total len of pkg
	tagNetPkg                 *lsthead;        //pkg head
	tagNetPkg                 *lsttail;        //pkg tail
}tagTcpReCombNode;
//每个远端SOCKET有且只有一个TCB
typedef struct _tagTcpTCB
{
	tagSocket                   *owner;            //该TCB的拥有者
	tagSocketNetAddr            raddr;             //远端通信地址
    u8                          state;             //TCP通信的状态（远端socket的状态）
    //算法控制块，acknodelay等
    tagTcpAlgorithm  	        algorithm;
    tagTcpChannelstate     	    channelstate;      //tcb的打开关闭状态
//tcp data buf,snd buf (include unacked buf) and rcv buf (include recombination buf)
#define CN_TCP_BUF_LEN_lIMIT           0x2000      //8KB
	tagSocketBuf     	        sbuf;              //snd buffer
    tagSocketBuf     	        rbuf;              //rcv buffer
    tagTcpSndUnackNode          *unacksechead;     //unacked queue head
    tagTcpSndUnackNode          *unacksectail;     //unacked queue tail
    tagTcpReCombNode            *recomblst;        //queue for recombination
//tcp basic window
#define CN_TCP_MSS_DEFAULT     1400
	u16	                        lmss;         //本方mss，默认536
	u16	                        rmss;         //对方mss，连接时被告知
	u16	                        mss;          //本方MSS与对方MSS中的较小值，创建连接的时候赋值
	u32	                        snduna;       // 待确认的第一个字节序号，即最后收到的确认号
	u32	                        sndnxt;       // 下一个要发送的序号，序号在调用打包发送函数成功后更新
	u32	                        rcvnxt;       // 期待接收的字节序号
#define  CN_TCP_RCVWIN_DEFAULT  0xF000  //64KB
	u16	                        rcvwnd;       // 本方接收缓冲区大小
	u16	                        sndwnd;       // 对方的接收窗口大小，更新自收到报文的窗口成员
    u8                          sndwndscale;  // 窗口因子
	bool_t                      rcvwndcongest;// 接收窗口阻塞
	//tcp congestion window member
    /* RTT(round trip time)相关变量 */
    /*
    RTT计算方法:
    Err = M-A    		//A是RTT平均值，M是实际测量值，Err是误差
    A←A + gErr    		//用误差更新平均值
    D←D + h( | Err |-D)    //D是均值偏差，用误差更新均值偏差
    RTO = A + 4D    	//计算得到RTO (重传超时时间)  (初始化时RTO = A + 2D  )
    g=0.125 ; h=0.25
    */
    s32 sa, sd; 	/* 用于计算RTO：sa = 8*A；sv = 4*D */
    s32 rto;    	/* RTO */
#define CN_TCP_RTOMAX            (120*1000*mS)
#define CN_TCP_RTOMIN            (200*mS)
#define CN_TCP_SA_INIT           0
#define CN_TCP_SD_INIT           (3*100*mS)       //Units：Micro Seconds
    /* 拥塞控制相关变量 */
    u16 cwnd;        /* 拥塞避免窗口, 默认为dMSS */
    u16 ssthresh;    /* 慢启动门限，默认为65535 */
    /* 快速重传和快速恢复 */
    u8 dupacks;    	/* 重复ACK计数 */
#define CN_TCP_FASTRETRANS   3
#define CN_TCP_CWND_LIMIT    60000
//tcp tick timer
#define CN_TCP_TIMER_ACK                   (1<<0)   //ack timer control bit
#define CN_TCP_TICK_ACK                    (2)
#define CN_TCP_TIMER_2ML                   (1<<1)   //2ml timer control bit
#define CN_TCP_TIMER_LINGER                (1<<2)   //linger timer control bit
#define CN_TCP_TIMER_KEEPALIVE             (1<<3)   //keepalive timer control bit
#define CN_TCP_TICK_KEEPALIVE              (10000)
#define CN_TCP_TIMER_PERSIST               (1<<4)   //persis timer control bit
#define CN_TCP_TICK_PERSIST                (10)
#define CN_TCP_TIMER_RETRANSMIT            (1<<5)   //retransmit timer control bit
#define CN_TCP_TIMER_CORK                  (1<<6)   //cork timer control bit
    u8                          timerflag;
    u16                         acktimer;          //(unit:tcp tick,use for ack echo)
    u16                         mltimer;           //(unit:tcp tick,used for 2ml wait)
    u16                         lingertimer;       //(unit:tcp tick,used for linger close)
    u16                         keeptimer;         //(unit:tcp tick,used for keep alive)
    u16                         persistimer;       //(unit:tcp tick,used for probe window)
    u16                         corktimer;         //(unit:tcp tick,used for cork timeout)
    //传输的时候的IP选项
    tagTcpIpOption              ipoption;          //use for ip optionnal
}tagTcpTcb;
#define  CN_TCP_TICK_TIME     (100*mS)        //Units:Micro Seconds


//tcp server node, used to hange the communication client
typedef struct _tagTcpTnode
{
	u32                        connum;              //已经通信的远端个数（包括正在链接的）
	u32                        connum2accept;       //可以被accept的个数
	u32                        connumlimit;         //远端个数限制
	tagSocket                  *clientlst;          //used to hange the client
}tagTcpTnode;
/////////////////////FUNCTION EXPORT DECLARATION//////////////////////////////

#endif /* TCP_H_ */
