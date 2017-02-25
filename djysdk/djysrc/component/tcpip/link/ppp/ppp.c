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
//author:zhangqf
//date  :下午4:43:13/2017年1月4日
//usage :
//-----------------------------------------------------------------------------

//standard includes
#include "ppp.h"

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <os.h>
//add your own specified header here

#include <sys/socket.h>

#include "iodev.h"
#include "osarch.h"
/*
 * FCS lookup table as calculated by genfcstab.
 */
static const u16 fcstab[256] = {
  0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
  0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
  0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
  0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
  0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
  0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
  0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
  0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
  0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
  0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
  0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
  0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
  0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
  0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
  0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
  0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
  0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
  0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
  0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
  0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
  0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
  0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
  0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
  0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
  0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
  0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
  0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
  0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
  0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
  0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
  0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
  0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};
/*
 * The basic PPP frame.
 */
#define PPP_HDRLEN      4       /* octets for standard ppp header */
#define PPP_FCSLEN      2       /* octets for FCS */
/*
 * Significant octet values.
 */
#define PPP_ALLSTATIONS 0xff    /* All-Stations broadcast address */
#define PPP_UI          0x03    /* Unnumbered Information */
#define PPP_FLAG        0x7e    /* Flag Sequence */
#define PPP_ESCAPE      0x7d    /* Asynchronous Control Escape */
#define PPP_TRANS       0x20    /* Asynchronous transparency modifier */
/*
 * Protocol field values.
 */
#define PPP_IP          0x21    /* Internet Protocol */
#define PPP_AT          0x29    /* AppleTalk Protocol */
#define PPP_VJC_COMP    0x2d    /* VJ compressed TCP */
#define PPP_VJC_UNCOMP  0x2f    /* VJ uncompressed TCP */
#define PPP_COMP        0xfd    /* compressed packet */
#define PPP_IPCP        0x8021  /* IP Control Protocol */
#define PPP_ATCP        0x8029  /* AppleTalk Control Protocol */
#define PPP_CCP         0x80fd  /* Compression Control Protocol */
#define PPP_LCP         0xc021  /* Link Control Protocol */
#define PPP_PAP         0xc023  /* Password Authentication Protocol */
#define PPP_LQR         0xc025  /* Link Quality Report protocol */
#define PPP_CHAP        0xc223  /* Cryptographic Handshake Auth. Protocol */
#define PPP_CBCP        0xc029  /* Callback Control Protocol */
/*
 * Values for FCS calculations.
 */
#define PPP_INITFCS     0xffff  /* Initial FCS value */
#define PPP_GOODFCS     0xf0b8  /* Good final FCS value */
#define PPP_FCS(fcs, c) (((fcs) >> 8) ^ fcstab[((fcs) ^ (c)) & 0xff])
/*
 *  CP (LCP, IPCP, etc.) codes.
 */
#define CONFREQ     1 /* Configuration Request */
#define CONFACK     2 /* Configuration Ack */
#define CONFNAK     3 /* Configuration Nak */
#define CONFREJ     4 /* Configuration Reject */
#define TERMREQ     5 /* Termination Request */
#define TERMACK     6 /* Termination Ack */
#define CODEREJ     7 /* Code Reject */
#define PROTOREJ    8 /* protocol reject */
#define ECHOREQ     9 /* echo request */
#define ECHOREP     10/* echo reply */
#define DISREQ      11/* discard request*/
//lcp options supported here
#define LCP_OPT_MRU     1
#define LCP_OPT_ACCM    2
#define LCP_OPT_AUTH    3
#define LCP_OPT_QUAL    4
#define LCP_OPT_MAGIC   5
#define LCP_OPT_PFC     7
#define LCP_OPT_ACFC    8
#define PPP_MTU         1500     /* Default MTU (size of Info field) */

typedef enum
{
	EN_PPPOS_RX_DROPS = 0,   //which means drops all the data receive until receive start flag
	EN_PPPOS_RX_START,       //which means received start flag
	EN_PPPOS_RX_ADDRESS,     //which means received address flag
	EN_PPPOS_RX_CONTROL,     //which means received control flag
	EN_PPPOS_RX_P1,          //which means received  p1  flag
	EN_PPPOS_RX_CPROTO,      //which means use the common proto receiver
}enPpposRxStat;
//usage:this data structure used for the receive data controller
#define CN_PPPOS_RXTXLEN       0X800   //large enough to storage all the frames
typedef struct
{
	u16 rxbuflen;
	u16 datalen;
	u16 framlen;                     //which means the data has the len
	u8  buf[CN_PPPOS_RXTXLEN];
	u16 protocol;
	u16 fcs;                          /*The frame checksum has computed*/
	enPpposRxStat stat;               /* The input process state.      */
	u8     inescape;                  /* has received the a escaped flag*/
}tagPppRcvCtrl;
//usage:this data structure used for the send control
typedef struct
{
	u16      datalen;
	u16      buflen;
	u8       buf[CN_PPPOS_RXTXLEN];
	mutex_t  mutex;   //used for the multi thread
	u16      fcs;
}tagPppTxCtrl;

//usage:this data structure used for the machine state monitor
typedef struct
{
	u64 timedead;      //the timeout time by the system
	u32 timeoutnum;    //continuing timeout times
	u32 timeoutlimit;  //continuing timeout times limit,if overlap,goto dead state
}tagTimeout;
//usage:use this data structure for the lcp negotiation  options
#define CN_OPTION_BUFLEN         64
typedef struct
{
	u32      mru;
	u32      accm;
	u32      macgic;
	u16      authcode;
	u8       authencode;
	u8       optionrej[CN_OPTION_BUFLEN];
	u8       rejlen;
	u8       optionnack[CN_OPTION_BUFLEN];
	u8       naklen;
	u8       optionack[CN_OPTION_BUFLEN];
	u8       acklen;
	u8       done;
}tagLcpPeerOptions; //when we receive the configure request, we use this data structure to decode
typedef struct
{
	u32      enmru:1;
	u32      enaccm:1;
	u32      enmagic:1;
	u32      enauth:1;
	u32      mru;
	u32      accm;
	u32      macgic;
	u16      authcode;
	u8       authencode;
	u8       id;
	u8       done;
	u8       buf[CN_OPTION_BUFLEN];
	u8       buflen;
	u8       datalen;
}tagLcpHostStat;
//usage:this data structure used for the lcp
typedef struct
{
	tagLcpPeerOptions   peer;    //the peer options
	tagLcpHostStat      host;    //the peer options
}tagLcp;
//usage:this data structure used for the auth
#define CN_NAME_LIMIT  32
typedef struct
{
	u8    id;
}tagAuth;
//usage:this data structure used for the ncp
typedef struct
{
	u32 enIp:1;
	u32 enDns:1;
	u32 enDnsBak:1;
	u32 enNbns:1;
	u32 enNbnsBak:1;
	u32 ip;
	u32 dns;
	u32 dnsbak;
	u32 nbns;
	u32 nbnsbak;
	u8  id;
	u8  buf[CN_OPTION_BUFLEN];
	u8  datalen;
	u8  buflen;
}tagNcp;
#define NCP_OPT_VJTCP 2
#define NCP_OPT_IP    3
#define NCP_OPT_DNS   0X81
#define NCP_OPT_NBNS  0X82
#define NCP_OPT_DNS2  0X83
#define NCP_OPT_NBNS2 0X84
//usage:this data structure used for the ppp controller
typedef enum
{
	EN_PPP_ATDEBUG= 0,       //use the at command to do the debug
	EN_PPP_DEAD,             //the link has not established yet
	EN_PPP_ESTABLISH,        //do the lcp negotiation
	EN_PPP_AUTH,             //do the authentication
	EN_PPP_NCP,              //do the net negotiation
	EN_PPP_NETWORK,          //do the normal net work
	EN_PPP_TERMINATE,        //do the normal shutdown

}enPppStat;
const char *gPppStatName[]={"ATDEBUG","DEAD","ESATBLISH","AUTH","NCP","NETWORK","TERMINAL"};
//the following for the debug info
typedef struct
{
	u32    numtotal;      //which means how many we have send or receive
	u32    numok;         //which means how many we have send or receive ok
	u32    numfcserr;     //which means the frame droped for the fcserr;
	u32    numoverlap;    //which means the frame too large
	u32    numlcp;        //which means how many for the lcp
	u32    numpap;        //which means how many for the pap
	u32    numchap;       //which means how many for the chap
	u32    numipcp;       //which means how many for the ipcp
	u32    numip;         //which means how many for the ip
	u32    numunknown;    //which means we don't know the proto
}tagPppDebug;

//usage:this data struct used for the ip control
typedef struct
{
}tagIp;

typedef bool_t (*fnModemReset)(void);
//usage:this data struct used for the ppp control
typedef struct
{
	tagPppRcvCtrl       rxctrl;  //used for the receive
	tagPppTxCtrl        txctrl;  //used for the send
	ptu32_t             sfd;     //the serial device fd
	ptu32_t             nfd;     //the net device fd
	tagLcp              lcp;     //used for the lcp
	tagAuth             auth;    //used for the auth
	tagNcp              ncp;     //used for the ncp
	tagIp               ip;      //used for the ip
	enPppStat           stat;    //used for the machine state
	char                apn[CN_NAME_LIMIT];
	char                user[CN_NAME_LIMIT];
	char                passwd[CN_NAME_LIMIT];
	char                iodevname[CN_NAME_LIMIT];
	char                rcvtaskname[CN_NAME_LIMIT];
	char                dealtaskname[CN_NAME_LIMIT];
	char                cgmi[CN_NAME_LIMIT];
	char                cgmm[CN_NAME_LIMIT];
	char                cgmr[CN_NAME_LIMIT];
	char                cgsn[CN_NAME_LIMIT];
	fnModemReset        modemreset;
	tagTimeout          mstimeout;
	tagPppDebug         rxinfo;
	tagPppDebug         txinfo;
}tagPppCtrl;
static tagPppCtrl gPppCtrl;

#pragma pack(1)
typedef struct
{
	u8   code;
	u8   id;
	u16  len;
}tagCHdr;
#define CN_PPPOS_CHDRSIZE   sizeof(tagCHdr)
//usage:we use this data structure to do the negotiation decode
typedef struct
{
	u8 type;
	u8 len;
	u8 v[0];
}tagPppOptions;
#define CN_PPPOS_OPTSIZE sizeof(tagPppOptions)
typedef struct
{
	u8  a;
	u8  c;
	u16 p;
}tagPppHdr;
#define CN_PPP_HDRSIZE  (sizeof(tagPppHdr))
#pragma pack()

//usage:we use this function to pass the info to the state machine
typedef struct __PppMsg
{
	struct __PppMsg   *nxt;
	tagPppCtrl        *ppp;
	u16 len;
	u16 proto;
	u8  data[0];
}tagPppMsg;
#define CN_PPPOS_MSGHDR_SZIE   sizeof(tagPppMsg)
#define CN_PPPOS_MSGTIMEOUT    (1000*mS)
typedef struct
{
	tagPppMsg     *msgH;
	tagPppMsg     *msgT;
	u16            msglen;
	mutex_t        mutex;
	semp_t         sync;
}tagPppMsgQ;
static tagPppMsgQ gPppMsgQ;

#define CN_SERIAL_TIMEOUT     (2*1000*mS)
typedef struct
{
	u8       *buf;
	u16       buflen;
	u16       datalen;
	mutex_t   mutex;
	semp_t    semp;
}tagRcvHook;
static tagRcvHook gRcvHook;
//usage:this function to register a buffer to get message from the receive engine
static int  __RcvMsgPop(u8 *buf,u16 len,u32 timeout)
{
	int result = -1;
	if(mutex_lock(gRcvHook.mutex))
	{
		gRcvHook.buf = buf;
		gRcvHook.buflen = len;
		mutex_unlock(gRcvHook.mutex);
		if((semp_pendtimeout(gRcvHook.semp,timeout))&&(gRcvHook.datalen > 0))
		{
			result = gRcvHook.datalen;
		}
		if(mutex_lock(gRcvHook.mutex))
		{
			gRcvHook.buf = NULL;
			gRcvHook.buflen = 0;
			gRcvHook.datalen = 0;
			mutex_unlock(gRcvHook.mutex);
		}
	}
	return result;
}
//usage:this function used for the receive engine to put the message to the hook
static void  __RcvMsgPush(u8 *buf,u16 len)
{
	u16 cpylen;
	if(mutex_lock(gRcvHook.mutex))
	{
		if(NULL != gRcvHook.buf)
		{
			cpylen = len>gRcvHook.buflen?gRcvHook.buflen:len;
			memcpy(gRcvHook.buf,buf,cpylen);
			gRcvHook.datalen = cpylen;
			semp_post(gRcvHook.semp);
		}
		mutex_unlock(gRcvHook.mutex);
	}
	return;
}

//usage:we use this function to deal the at result as the args format
static int __fetchPara(char *text,char *seperate,char *argv[],int argc)
{
	int result;
	char *s;
	int len,i;
	s = seperate;
	len = strlen(text);
	while(*s != '\0') //make all the charactor in text matching the seperate to 0
	{
		for(i =0;i<len;i++)
		{
			if(text[i]==*s)
			{
				text[i]='\0';
			}
		}
		s++;
	}
	//ok now check the para start
	result = 0;
	i = 0;
	while((i <(len-1))&&(result < argc))
	{
		if((text[i]=='\0')&&(text[i+1]!='\0'))
		{
			argv[result] = text+i+1;
			result++;
		}
		i++;
	}
	return result;
}

#define CN_AT_LEN  64
//usage:send the at command to the serial device
static int __AtCmd(tagPppCtrl *ppp,char *cmd,u8 *buf,int buflen,char *argv[],int argc)
{
	char   cmdbuf[CN_AT_LEN];
	int  result = -1;

	memset(cmdbuf,0,CN_AT_LEN);
	snprintf(cmdbuf,CN_AT_LEN,"%s\r",cmd);
	result = iodevwrite(ppp->sfd,(u8 *)cmdbuf,(u32)strlen(cmdbuf));
	//if need the result,then we will wait for the timeout
	if((NULL != buf)&&(buflen>0))
	{
		result = __RcvMsgPop(buf,buflen,CN_SERIAL_TIMEOUT);
		if((result > 0)&&(argc > 0)&&(NULL != argv))
		{
			result = __fetchPara((char *)buf,"\n\r",argv,argc);
		}
	}
	return  result;
}


//usage:this function to dial up by the machine
#define CN_CMD_SLEEP    (1000*mS)
#define CN_CMD_TRIES    10  //if over, then return failed
static bool_t __DialUp(tagPppCtrl *ppp)
{
	bool_t result = false;
	char *argv[6];
	int  argc;
	int  retries;
	u8   buf[CN_AT_LEN];
	//first we should check if the sim card inserted:at+cpin?
	retries =0;
	memset(argv,0,sizeof(argv));
	memset(buf,0,sizeof(buf));
	printf("SIM CHECK:");
	while(retries <CN_CMD_TRIES )
	{
		argc = __AtCmd(ppp,"at+cpin?",buf,CN_AT_LEN,argv,6);
		if((argc >0)&&strstr(argv[0],"READY"))
		{
			break;
		}
		Djy_EventDelay(CN_CMD_SLEEP);
		printf(".");
		retries++;
	}
	if(retries < CN_CMD_TRIES)
	{
		printf("DONE\n\r");
	}
	else
	{
		printf("TIMEOUT\n\r");
		return result;
	}
	//we'd better to get the modem info
	//cgmi:the manufacture
	retries =0;
	memset(argv,0,sizeof(argv));
	memset(buf,0,sizeof(buf));
	printf("CGMI:");
	while(retries <CN_CMD_TRIES )
	{
		argc = __AtCmd(ppp,"at+cgmi",buf,CN_AT_LEN,argv,6);
		if((argc >0)&&strstr(argv[argc-1],"OK"))
		{
			break;
		}
		Djy_EventDelay(CN_CMD_SLEEP);
		printf(".");
		retries++;
	}
	if(retries < CN_CMD_TRIES)
	{
		printf("%s DONE\n\r",argv[0]);
		strncpy(ppp->cgmi,argv[0],CN_NAME_LIMIT);
	}
	else
	{
		printf("TIMEOUT\n\r");
		return result;
	}
	//cgmm:the modem name
	retries =0;
	memset(argv,0,sizeof(argv));
	memset(buf,0,sizeof(buf));
	printf("CGMM:");
	while(retries <CN_CMD_TRIES )
	{
		argc = __AtCmd(ppp,"at+cgmm",buf,CN_AT_LEN,argv,6);
		if((argc >0)&&strstr(argv[argc-1],"OK"))
		{
			break;
		}
		Djy_EventDelay(CN_CMD_SLEEP);
		printf(".");
		retries++;
	}
	if(retries < CN_CMD_TRIES)
	{
		printf("%s DONE\n\r",argv[0]);
		strncpy(ppp->cgmm,argv[0],CN_NAME_LIMIT);
	}
	else
	{
		printf("TIMEOUT\n\r");
		return result;
	}
	//cgmr:the modem version
	retries =0;
	memset(argv,0,sizeof(argv));
	memset(buf,0,sizeof(buf));
	printf("CGMR:");
	while(retries <CN_CMD_TRIES )
	{
		argc = __AtCmd(ppp,"at+cgmr",buf,CN_AT_LEN,argv,6);
		if((argc >0)&&strstr(argv[argc-1],"OK"))
		{
			break;
		}
		Djy_EventDelay(CN_CMD_SLEEP);
		printf(".");
		retries++;
	}
	if(retries < CN_CMD_TRIES)
	{
		printf("%s DONE\n\r",argv[0]);
		strncpy(ppp->cgmr,argv[0],CN_NAME_LIMIT);
	}
	else
	{
		printf("TIMEOUT\n\r");
		return result;
	}
	//cgsn:the modem sn number
	retries =0;
	memset(argv,0,sizeof(argv));
	memset(buf,0,sizeof(buf));
	printf("CGSN:");
	while(retries <CN_CMD_TRIES )
	{
		argc = __AtCmd(ppp,"at+cgsn",buf,CN_AT_LEN,argv,6);
		if((argc >0)&&strstr(argv[argc-1],"OK"))
		{
			break;
		}
		Djy_EventDelay(CN_CMD_SLEEP);
		printf(".");
		retries++;
	}
	if(retries < CN_CMD_TRIES)
	{
		printf("%s DONE\n\r",argv[0]);
		strncpy(ppp->cgsn,argv[0],CN_NAME_LIMIT);
	}
	else
	{
		printf("TIMEOUT\n\r");
		return result;
	}
	//check if we has register the sim card to the carrieroperator
	retries =0;
	memset(argv,0,sizeof(argv));
	memset(buf,0,sizeof(buf));
	printf("SIM REGISTER:");
	while(retries <CN_CMD_TRIES )
	{
		argc = __AtCmd(ppp,"at+cgreg?",buf,CN_AT_LEN,argv,6);
		if((argc >0)&&(strstr(argv[0],",1")||strstr(argv[0],",5")))
		{
			break;
		}
		Djy_EventDelay(CN_CMD_SLEEP);
		printf(".");
		retries++;
	}
	if(retries < CN_CMD_TRIES)
	{
		printf("DONE\n\r");
	}
	else
	{
		printf("TIMEOUT\n\r");
		return result;
	}

	//OK,now set the apn to the carrieroperator
	retries =0;
	memset(argv,0,sizeof(argv));
	memset(buf,0,sizeof(buf));
	printf("APN SET:");
	char cgdcont[64];
	memset(cgdcont,0,64);
	snprintf(cgdcont,63,"%s%s%s%s","at+cgdcont=1,\"ip\",","\"",ppp->apn,"\"");
	while(retries <CN_CMD_TRIES )
	{
		argc = __AtCmd(ppp,cgdcont,buf,CN_AT_LEN,argv,6);
		if((argc >0)&&(strstr(argv[0],"OK")||strstr(argv[0],"ok")))
		{
			break;
		}
		Djy_EventDelay(CN_CMD_SLEEP);
		printf(".");
		retries++;
	}
	if(retries < CN_CMD_TRIES)
	{
		printf("DONE\n\r");
	}
	else
	{
		printf("TIMEOUT\n\r");
		return result;
	}

	//now we begin to atd(call the data service)
	retries =0;
	memset(argv,0,sizeof(argv));
	memset(buf,0,sizeof(buf));
	printf("ATD:");
	while(retries <CN_CMD_TRIES )
	{
		argc = __AtCmd(ppp,"atd*99#",buf,CN_AT_LEN,argv,6);
		if((argc >0)&&(strstr(argv[0],"CONNECT")||strstr(argv[0],"connect")))
		{
			break;
		}
		Djy_EventDelay(CN_CMD_SLEEP);
		printf(".");
		retries++;
	}
	if(retries < CN_CMD_TRIES)
	{
		printf("DONE\n\r");
	}
	else
	{
		printf("TIMEOUT\n\r");
		return result;
	}
	result = true;
	return result;
}
//usage:this var used for the ppp frame debug switch
static bool_t  gPppFrameDebug = false;
//usage:use this function to show the ppp frame
static bool_t __PppFrameInfo(tagPppCtrl *ppp,bool_t read,u16 proto,u8 *buf,u16 len)
{
	time_t printtime;
	tagPppDebug  *debug;

	if(read)
	{
		if(gPppFrameDebug)
		{
			printtime = time(NULL);
			printf("[RCV:%s]:%04x:",ctime(&printtime),proto);
			//should add the head for the beuty show
			printf("%02x ",0x7e);
			tagPppHdr       hdr;
			hdr.a = PPP_ALLSTATIONS;
			hdr.c = PPP_UI;
			hdr.p = htons(proto);
			for(u16 i=0;i <CN_PPP_HDRSIZE;i++)
			{
				printf("%02x ",((u8 *)&hdr)[i]);
			}

			for(u16 i=0;i <len;i++)
			{
				printf("%02x ",buf[i]);
			}

			printf("\n\r");
		}
		//do the statistics
		debug = &ppp->rxinfo;
		TCPIP_DEBUG_INC(debug->numtotal);
		if(proto == PPP_LCP)
		{
			TCPIP_DEBUG_INC(debug->numlcp);
		}
		else if(proto == PPP_PAP)
		{
			TCPIP_DEBUG_INC(debug->numpap);
		}
		else if(proto == PPP_CHAP)
		{
			TCPIP_DEBUG_INC(debug->numchap);
		}
		else if(proto == PPP_IPCP)
		{
			TCPIP_DEBUG_INC(debug->numipcp);
		}
		else if(proto == PPP_IP)
		{
			TCPIP_DEBUG_INC(debug->numip);
		}
		else
		{
			TCPIP_DEBUG_INC(debug->numunknown);
		}

	}
	else
	{
		if(gPppFrameDebug)
		{
			printtime = time(NULL);
			printf("[SND:%s]:%04x:",ctime(&printtime),proto);
			for(u16 i=0;i <len;i++)
			{
				printf("%02x ",buf[i]);
			}
			printf("\n\r");
		}

		//do the statistics
		debug = &ppp->txinfo;
		TCPIP_DEBUG_INC(debug->numtotal);
		if(proto == PPP_LCP)
		{
			TCPIP_DEBUG_INC(debug->numlcp);
		}
		else if(proto == PPP_PAP)
		{
			TCPIP_DEBUG_INC(debug->numpap);
		}
		else if(proto == PPP_CHAP)
		{
			TCPIP_DEBUG_INC(debug->numchap);
		}
		else if(proto == PPP_IPCP)
		{
			TCPIP_DEBUG_INC(debug->numipcp);
		}
		else if(proto == PPP_IP)
		{
			TCPIP_DEBUG_INC(debug->numip);
		}
		else
		{
			TCPIP_DEBUG_INC(debug->numunknown);
		}
	}


	return true;
}
//-----------------------------------------------------------------------------
//功能:we use this function to send data to the peer
//参数:
//返回:
//备注:here we add the address control and the tail it self
//作者:zhangqf@下午4:04:11/2017年1月6日
//-----------------------------------------------------------------------------
static bool_t __PppMsgSnd(tagPppCtrl *ppp,u16 proto,tagCHdr *chdr,u8 *buf,u16 l,u32 accm)
{
	u8             *dst,*dstedge,*src;
	u8              c;
	u16             fcs,len;
	tagPppTxCtrl   *tx;
	tagPppHdr       hdr;

	tx = &ppp->txctrl;
	if(mutex_lock(tx->mutex))
	{
		//do some initialize;
		tx->datalen = 0;
		fcs = PPP_INITFCS;
		dst = tx->buf;
		dstedge = tx->buf + tx->buflen;
		//add the start flag
		c = PPP_FLAG;
		*dst++= c;
		//add the proto header
		hdr.a = PPP_ALLSTATIONS;
		hdr.c = PPP_UI;
		hdr.p = htons(proto);

		//now cpy use the accm and compute the fcs
		len = CN_PPP_HDRSIZE;
		src = (u8 *)&hdr;
		while((dst < dstedge)&&(len > 0))
		{
			c = *src++;
			len--;
			if((c == PPP_FLAG)||(c== PPP_ESCAPE)||\
			   ((c <0x20)&&((1<<c)&accm)))
			{
				*dst++ = PPP_ESCAPE;
				fcs = PPP_FCS(fcs,c);
				c ^= PPP_TRANS;
				*dst++= c;
			}
			else
			{
				*dst++=c;
				fcs = PPP_FCS(fcs,c);
			}
		}
		//add the code header
		if(NULL != chdr)
		{
			len = CN_PPPOS_CHDRSIZE;
			src = (u8 *)chdr;
			while((dst < dstedge)&&(len > 0))
			{
				c = *src++;
				len--;
				if((c == PPP_FLAG)||(c== PPP_ESCAPE)||\
				   ((c <0x20)&&((1<<c)&accm)))
				{
					*dst++ = PPP_ESCAPE;
					fcs = PPP_FCS(fcs,c);
					c ^= PPP_TRANS;
					*dst++= c;
				}
				else
				{
					*dst++=c;
					fcs = PPP_FCS(fcs,c);
				}
			}
		}
		//add the data
		len = l;
		src = (u8 *)buf;
		while((dst < dstedge)&&(len > 0))
		{
			c = *src++;
			len--;
			if((c == PPP_FLAG)||(c== PPP_ESCAPE)||\
			   ((c <0x20)&&((1<<c)&accm)))
			{
				*dst++ = PPP_ESCAPE;
				fcs = PPP_FCS(fcs,c);
				c ^= PPP_TRANS;
				*dst++= c;
			}
			else
			{
				*dst++=c;
				fcs = PPP_FCS(fcs,c);
			}
		}
		fcs = ~fcs;
		//add the fcs,at least 5 bytes left for the fcs and the stop flag
		if((dst+5)< dstedge)
		{
			len = 2;
			src = (u8 *)&fcs;
			while((dst < dstedge)&&(len > 0))
			{
				c = *src++;
				len--;
				if((c == PPP_FLAG)||(c== PPP_ESCAPE)||\
				   ((c <0x20)&&((1<<c)&accm)))
				{
					*dst++ = PPP_ESCAPE;
					c ^= PPP_TRANS;
					*dst++= c;
				}
				else
				{
					*dst++=c;
				}
			}
			//add the stop flag
			*dst++= PPP_FLAG;
			//compute the len, and send it to the device;
			len =(u16)(dst -tx->buf);
			iodevwrite(ppp->sfd,tx->buf,len);
			__PppFrameInfo(ppp,false,proto,tx->buf,len);
		}
		else
		{
			//buf maybe overlap
			TCPIP_DEBUG_INC(ppp->txinfo.numoverlap);
		}
		mutex_unlock(tx->mutex);
	}

	return true;
}
//here we create a ppp net device to the stack
#include <sys/socket.h>
//data flow in the loop
#define CN_PPP_MTU      4*1024        //4KB
#define CN_PPP_DEVICE   "pppdev"
//-----------------------------------------------------------------------------
//功能:the linkoutloop call this function to pass the package to the stack
//参数:
//返回:
//备注:
//作者:zhangqf@上午9:24:20/2016年12月29日
//-----------------------------------------------------------------------------
static bool_t __PppDevOut(ptu32_t dev,tagNetPkg *pkg,u32 framlen,u32 netdevtask)
{
    bool_t  result;
    tagNetPkg *tmp;
    u8 *buf;
    u8 *src;
    u8 *dst;
    u32 cpylen;

    result = false;

    buf = malloc(framlen);
    if(NULL != buf)
    {
    	cpylen = 0;
    	dst = buf;
        tmp = pkg;
        while(NULL != tmp)
        {
            src = (u8 *)(tmp->buf + tmp->offset);
            memcpy(dst, src, tmp->datalen);
            dst += tmp->datalen;
            cpylen += tmp->datalen;
            if(tmp->pkgflag & CN_PKLGLST_END)
            {
                tmp = NULL;
            }
            else
            {
                tmp = tmp->partnext;
            }
        }
        //send the buf to the ppp
        tagPppCtrl *ppp;
        ppp = (tagPppCtrl*)NetDevPrivate(dev);
        result = __PppMsgSnd(ppp,PPP_IP,NULL,buf,cpylen,ppp->lcp.peer.accm);
        free(buf);
    }
    return result;
}

static bool_t __IpFrameDeal(tagPppMsg   *msg )
{
	//we should package the msg and put it to the stack
	u8 *src,*dst;
	u16 len;
	tagNetPkg *pkg;
	tagPppCtrl *ppp;

	src = msg->data;
	len = msg->len;
	ppp = msg->ppp;

	pkg = PkgMalloc(len,CN_PKLGLST_END);
	if(NULL != pkg)
	{
		dst = pkg->buf + pkg->offset;
		memcpy(dst,src,len);
		pkg->datalen = len;

		LinkPost(ppp->nfd,pkg);

		PkgTryFreePart(pkg);
	}
	return true;
}
//-----------------------------------------------------------------------------
//功能:Install the ppp device
//参数:
//返回:
//备注:
//作者:zhangqf@上午9:26:04/2016年12月29日
//-----------------------------------------------------------------------------
static ptu32_t __PppNetDevAdd(ptu32_t para)
{
	ptu32_t         dev;
    tagNetDevPara   devpara;
    tagHostAddrV4   devaddr;
    //then we will register a loop device to the stack
    memset((void *)&devpara,0,sizeof(devpara));
    devpara.ifsend = __PppDevOut;
    devpara.iftype = EN_LINK_RAW;
    devpara.name = CN_PPP_DEVICE;
    devpara.private = para;
    devpara.mtu = CN_PPP_MTU;
    devpara.devfunc = CN_IPDEV_NONE;
    memcpy(devpara.mac,CN_MAC_BROAD,CN_MACADDR_LEN);
    dev = NetDevInstall(&devpara);
    if(0 == dev)
    {
    	goto EXIT_PPPDEV;
    }
    //here means we are successful
    //then we will create a loop rout to the stack
    devaddr.ip      = inet_addr("192.168.253.100");
    devaddr.gatway  = inet_addr("192.168.253.1");
    devaddr.submask = inet_addr("255.255.255.0");
    devaddr.dns     = inet_addr("192.168.253.1");
    devaddr.broad   = inet_addr("255.255.255.255");
    //loop could receive the broad mail,but never send the broad mail
    if(false == RoutCreate(CN_PPP_DEVICE,EN_IPV_4,(void *)&devaddr,CN_ROUT_NONE))
    {
    	printf("%s:NetDevInstall Err\n\r",__FUNCTION__);
    	goto EXIT_PPPROUT;
    }
    //here means we are successful
    RoutSetDefault(EN_IPV_4,devaddr.ip);
    return dev;

EXIT_PPPROUT:
	NetDevUninstall(CN_PPP_DEVICE);
EXIT_PPPDEV:
    return 0;
}


extern bool_t __AuthSndRequest(tagPppCtrl *ppp);
//-----------------------------------------------------------------------------
//功能:we use this function to pass the receive data to the queue
//参数:
//返回:
//备注:
//作者:zhangqf@上午10:10:36/2017年1月5日
//-----------------------------------------------------------------------------
static bool_t __NetFramePut(tagPppCtrl *ppp)
{
	bool_t         result = false;
	u16            msglen;
	tagPppMsg   *msg;

	msglen = ppp->rxctrl.datalen + CN_PPPOS_MSGHDR_SZIE;
	msg = malloc(msglen);
	if(NULL != msg)
	{
		memset((void *)msg,0,sizeof(tagPppMsg));
		msg->ppp =ppp;
		msg->len = ppp->rxctrl.datalen;
		msg->proto = ppp->rxctrl.protocol;
		memcpy(msg->data,ppp->rxctrl.buf,ppp->rxctrl.datalen);
		if(mutex_lock(gPppMsgQ.mutex))
		{
			if(NULL == gPppMsgQ.msgT)
			{
				gPppMsgQ.msgH = msg;
			}
			else
			{
				gPppMsgQ.msgT->nxt = msg;
			}
			gPppMsgQ.msgT = msg;
			mutex_unlock(gPppMsgQ.mutex);
			semp_post(gPppMsgQ.sync);
		}
		else
		{
			free(msg);
		}
	}
	return result;
}
//-----------------------------------------------------------------------------
//功能:we use this function to get a message from the queue
//参数:
//返回:
//备注:
//作者:zhangqf@上午10:30:49/2017年1月5日
//-----------------------------------------------------------------------------
static tagPppMsg *__NetFrameGet(u32 timeout)
{
	tagPppMsg   *msg = NULL;
	semp_pendtimeout(gPppMsgQ.sync,timeout);
	if(mutex_lock(gPppMsgQ.mutex))
	{
		if(NULL != gPppMsgQ.msgH)
		{
			if(gPppMsgQ.msgH == gPppMsgQ.msgT)
			{
				gPppMsgQ.msgT = NULL;
			}
			msg = gPppMsgQ.msgH;
			gPppMsgQ.msgH = msg->nxt;
			msg->nxt = NULL;
		}
		mutex_unlock(gPppMsgQ.mutex);
	}
	return msg;
}
//usage:use this function to reset the ppp machine state
static bool_t  __PppMsReset(tagPppCtrl *ppp,enPppStat state)
{
	ppp->stat = state;
    //initialize the lcp options
	memset((void *)&ppp->lcp,0,sizeof(tagLcp));
	ppp->lcp.host.mru = PPP_MTU;
	ppp->lcp.host.enmru = 0;
	ppp->lcp.host.accm = 0;
	ppp->lcp.host.enaccm = 1;
	ppp->lcp.host.authcode = PPP_PAP;
	ppp->lcp.host.authencode = 0;
	ppp->lcp.host.enauth = 0;
	ppp->lcp.host.macgic = (u32)DjyGetSysTime();
	ppp->lcp.host.enmagic = 0;
	ppp->lcp.host.buflen = CN_OPTION_BUFLEN;
	ppp->lcp.host.id = 1;
	//initialize the auth
	memset((void *)&ppp->auth,0,sizeof(tagAuth));
	ppp->auth.id = 0x10;
	//initialize the ncp
	memset((void *)&ppp->ncp,0,sizeof(tagNcp));
	ppp->ncp.enIp = 1;
	ppp->ncp.enDns = 1;
	ppp->ncp.enDnsBak = 1;
	ppp->ncp.enNbns = 1;
	ppp->ncp.enNbnsBak = 1;
	ppp->ncp.id = 0x20;

	return true;
}


//deal the configure request
static bool_t __LcpDealCRQ(tagPppCtrl *ppp,u8 id,u8 *buf, u16 len)
{
	u8                 *src;
	u8                 *stop;
	tagPppOptions      *opt;
	tagLcpPeerOptions  *peer;
	u16                 v16;
	u32                 v32;
	tagPppOptions       mopt;
	tagCHdr             cdr;

	src = buf;
	stop = buf + len;
	peer = &ppp->lcp.peer;
	peer->naklen = 0;
	peer->rejlen = 0;
	peer->acklen = 0;
	//now decode the lcp options here
	while(src < stop)
	{
		opt = (tagPppOptions *)src;
		src += opt->len;
		switch (opt->type)
		{
			case LCP_OPT_MRU:
				memcpy(&v16,opt->v,sizeof(v16));
				v16 = ntohl(v16);
//				printf("mru:0x%04x\n\r",v16);
				if(v16 > PPP_MTU) //should advise it small
				{
					v16 = htons(PPP_MTU);
					mopt.type = opt->type;
					mopt.len = 4;
					memcpy(&peer->optionnack[peer->naklen],&mopt,CN_PPPOS_OPTSIZE);
					peer->naklen += CN_PPPOS_OPTSIZE;
					memcpy(&peer->optionnack[peer->naklen],&v16,sizeof(v16));
					peer->naklen += sizeof(v16);
				}
				else
				{
					peer->mru = v16;
					memcpy(&peer->optionack[peer->acklen],opt,opt->len);
					peer->acklen += opt->len;
				}
				break;
			case LCP_OPT_ACCM:
				memcpy(&v32,opt->v,sizeof(v32));
				v32 = ntohl(v32);
//				printf("accm:0x%08x\n\r",v32);
				peer->accm = v32;
				memcpy(&peer->optionack[peer->acklen],opt,opt->len);
				peer->acklen += opt->len;
				break;
			case LCP_OPT_AUTH:
				memcpy(&v16,opt->v,sizeof(v16));
				v16 = ntohs(v16);
//				printf("auth:0x%04x\n\r",v16);
				if(v16 != PPP_PAP)  //only md5 supported
				{
					v16 = htons(PPP_PAP);
					mopt.type = opt->type;
					mopt.len = 4;
					memcpy(&peer->optionnack[peer->naklen],&mopt,CN_PPPOS_OPTSIZE);
					peer->naklen += CN_PPPOS_OPTSIZE;
					memcpy(&peer->optionnack[peer->naklen],&v16,sizeof(v16));
					peer->naklen += sizeof(v16);
				}
				else
				{
					peer->authcode = v16;
					memcpy(&peer->optionack[peer->acklen],opt,opt->len);
					peer->acklen += opt->len;
				}
				break;
			case LCP_OPT_MAGIC:
				memcpy(&v32,opt->v,sizeof(v32));
				v32 = ntohl(v32);
//				printf("magic:0x%04x\n\r",v32);
				peer->macgic = v32;
				memcpy(&peer->optionack[peer->acklen],opt,opt->len);
				peer->acklen += opt->len;
				break;
			case LCP_OPT_PFC: //not supported yet
//				printf("pfc:\n\r");
				memcpy(&peer->optionrej[peer->rejlen],opt,opt->len);
				peer->rejlen += opt->len;
				break;
			case LCP_OPT_ACFC://not supported yet
//				printf("acfc:\n\r");
				memcpy(&peer->optionrej[peer->rejlen],opt,opt->len);
				peer->rejlen += opt->len;
				break;
			default:
//				printf("unknown:type:%02x len:%d\n\r",opt->type,opt->len);
				memcpy(&peer->optionrej[peer->rejlen],opt,opt->len);
				peer->rejlen += opt->len;
				break;
		}
	}
	u32 accm = 0xffff;
	//here we should do the machine state changed, and do the time out configure
	if(peer->rejlen > 0) //do the rej
	{
		cdr.code = CONFREJ;
		cdr.id = id;
		cdr.len = peer->rejlen + CN_PPPOS_CHDRSIZE;
		cdr.len = htons(cdr.len);
		__PppMsgSnd(ppp,PPP_LCP,&cdr,peer->optionrej,peer->rejlen,accm);
	}
	else if(peer->naklen > 0)//if no rej, then do the nack
	{
		cdr.code = CONFNAK;
		cdr.id = id;
		cdr.len = peer->naklen + CN_PPPOS_CHDRSIZE;
		cdr.len = htons(cdr.len);
		__PppMsgSnd(ppp,PPP_LCP,&cdr,peer->optionnack,peer->naklen,accm);
	}
	else //if no rej and nack, then we do the ack
	{
		cdr.code = CONFACK;
		cdr.id = id;
		cdr.len = peer->acklen + CN_PPPOS_CHDRSIZE;
		cdr.len = htons(cdr.len);
		__PppMsgSnd(ppp,PPP_LCP,&cdr,peer->optionack,peer->acklen,accm);
		peer->done = 1;
		if(ppp->lcp.host.done)
		{
			//if the peer has agree the negotiation,then we do the auth
			printf("LCP SUCCESS,GOTO AUTH\n\r");
			ppp->mstimeout.timeoutnum = 0;
			ppp->stat = EN_PPP_AUTH;
			__AuthSndRequest(ppp);
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
//功能:when we receive this frame type,it means it has agree all the options we supplied
//参数:
//返回:
//备注:we should changge the stage and go on
//作者:zhangqf@上午10:12:07/2017年1月7日
//-----------------------------------------------------------------------------
static bool_t __LcpDealCACK(tagPppCtrl *ppp,u8 id,u8 *buf, u16 len)
{
	//i think all i want to do is to change the state,and do the next thing
	//maybe should send the pap or chap to the peer
	ppp->lcp.host.done = 1;
	ppp->lcp.host.id++;
	if(ppp->lcp.peer.done)//if the peer has agree the negotiation,then we do the auth
	{
		printf("LCP SUCCESS,GOTO AUTH\n\r");
		ppp->mstimeout.timeoutnum = 0;
		ppp->stat = EN_PPP_AUTH;
		__AuthSndRequest(ppp);
	}
	return true;
}

//we use this function to send the lcp request
static bool_t __LcpSndRequest(tagPppCtrl *ppp)
{

	u8                 *dst;
	tagPppOptions       opt;
	tagLcpHostStat     *host;
	u16                 v16;
	u32                 v32;
	tagCHdr             cdr;
	u32                 accm;

	host = &ppp->lcp.host;
	accm = 0xFFFFFFFF;

	//make the new request
	dst = host->buf;
	if(host->enaccm) //check the accm
	{
		opt.type = LCP_OPT_ACCM;
		opt.len = 6;
		memcpy(dst,&opt,sizeof(opt));
		dst += sizeof(opt);
		v32 = host->accm;
		v32 = htonl(v32);
		memcpy(dst,&v32,sizeof(v32));
		dst += sizeof(v32);
	}
	if(host->enauth) //check the auth
	{
		if(host->authcode == PPP_PAP)
		{
			opt.type = LCP_OPT_AUTH;
			opt.len = 4;
			memcpy(dst,&opt,sizeof(opt));
			dst += sizeof(opt);
			v16 = host->authcode;
			v16 = htonl(v16);
			memcpy(dst,&v16,sizeof(v16));
			dst += sizeof(v16);
		}
		else
		{
			opt.type = LCP_OPT_AUTH;
			opt.len = 5;
			memcpy(dst,&opt,sizeof(opt));
			dst += sizeof(opt);
			v16 = host->authcode;
			v16 = htonl(v16);
			memcpy(dst,&v16,sizeof(v16));
			dst += sizeof(v16);
			*dst++=host->authencode;
		}
	}
	if(host->enmagic) //CHECK THE MAGIC
	{
		opt.type = LCP_OPT_MAGIC;
		opt.len = 6;
		memcpy(dst,&opt,sizeof(opt));
		dst += sizeof(opt);
		v32 = host->macgic;
		v32 = htonl(v32);
		memcpy(dst,&v32,sizeof(v32));
		dst += sizeof(v32);
	}
	if(host->enmagic) //CHECK THE MRU
	{
		opt.type = LCP_OPT_MRU;
		opt.len = 4;
		memcpy(dst,&opt,sizeof(opt));
		dst += sizeof(opt);
		v16 = host->mru;
		v16 = htonl(v16);
		memcpy(dst,&v16,sizeof(v16));
		dst += sizeof(v16);
	}
	host->datalen = dst - host->buf;
	cdr.code = CONFREQ;
	cdr.id = host->id;
	cdr.len = htons((host->datalen+sizeof(cdr)));
	__PppMsgSnd(ppp,PPP_LCP,&cdr,host->buf,host->datalen,accm);

	return true;
}

//use this function to send a echo request
static bool_t __LcpSendEchoReq(tagPppCtrl *ppp)
{
	u32     magic;
	tagCHdr cdr;
	u32     accm;

	accm = 0xffffffff;
	magic = ppp->lcp.peer.macgic;
	magic = htonl(magic);

	cdr.code = ECHOREQ;
	cdr.id = ppp->lcp.host.id;
	cdr.len = htons((sizeof(magic)+sizeof(cdr)));
	__PppMsgSnd(ppp,PPP_LCP,&cdr,(u8 *)&magic,sizeof(magic),accm);
	return true;
}

//use this function to send a terminate request
static bool_t __LcpSendTerminateReq(tagPppCtrl *ppp)
{
	u32     magic;
	tagCHdr cdr;
	u32     accm;

	accm = 0xffffffff;
	magic = ppp->lcp.peer.macgic;
	magic = htonl(magic);

	cdr.code = TERMREQ;
	cdr.id = ppp->lcp.host.id;
	cdr.len = htons((sizeof(magic)+sizeof(cdr)));
	__PppMsgSnd(ppp,PPP_LCP,&cdr,(u8 *)&magic,sizeof(magic),accm);
	return true;
}

//usage:use this function to deal the lcp nack frame
static bool_t __LcpDealCNACK(tagPppCtrl *ppp,u8 id,u8 *buf, u16 len)
{
	u8                 *src;
	u8                 *stop;
	tagPppOptions      *opt;
	tagLcpHostStat     *host;
	u16                 v16;
	u32                 v32;

	src = buf;
	stop = buf + len;
	host = &ppp->lcp.host;
	//now decode the lcp options here
	while(src < stop)
	{
		opt = (tagPppOptions *)src;
		src += opt->len;
		switch (opt->type)
		{
			case LCP_OPT_MRU:
				memcpy(&v16,opt->v,sizeof(v16));
				v16 = ntohl(v16);
				host->mru = v16;
				break;
			case LCP_OPT_ACCM:
				memcpy(&v32,opt->v,sizeof(v32));
				v32 = ntohl(v32);
				host->accm = v32;
				break;
			case LCP_OPT_AUTH:
				memcpy(&v16,opt->v,sizeof(v16));
				v16 = ntohs(v16);
				host->authcode = v16;
				if(v16 != PPP_PAP)  //only md5 supported
				{
					host->authencode = opt->v[2];
				}
				break;
			case LCP_OPT_MAGIC:
				memcpy(&v32,opt->v,sizeof(v32));
				v32 = ntohl(v32);
				host->macgic = v32;
				break;
			default:
				break;
		}
	}
	host->id++;
	//we should resend the request here
	__LcpSndRequest(ppp);
	return true;
}
//-----------------------------------------------------------------------------
//功能:we use this function to deal the reject options
//参数:
//返回:
//备注:if we receive the nack,it means we should modify the options we send
//作者:zhangqf@上午11:37:16/2017年1月7日
//-----------------------------------------------------------------------------
static bool_t __LcpDealCREJ(tagPppCtrl *ppp,u8 id,u8 *buf, u16 len)
{
	u8                 *src;
	u8                 *stop;
	tagPppOptions      *opt;
	tagLcpHostStat  *host;

	src = buf;
	stop = buf + len;
	host = &ppp->lcp.host;
	//now decode the lcp options here
	while(src < stop)
	{
		opt = (tagPppOptions *)src;
		src += opt->len;
		switch (opt->type)
		{
			case LCP_OPT_MRU:
				host->enmru = 0;
				break;
			case LCP_OPT_ACCM:
				host->enaccm = 0;
				break;
			case LCP_OPT_AUTH:
				host->enauth = 0;
				break;
			case LCP_OPT_MAGIC:
				host->enmagic = 0;
				break;
			default:
				break;
		}
	}
	host->id++;
	//we should resend the request here
	__LcpSndRequest(ppp);
	return true;
}
//-----------------------------------------------------------------------------
//功能:we use this function to deal the echo request
//参数:
//返回:
//备注:just send the magic number we have received
//作者:zhangqf@上午11:37:16/2017年1月7日
//-----------------------------------------------------------------------------
static bool_t __LcpDealEchoReq(tagPppCtrl *ppp,u8 id,u8 *buf, u16 len)
{
	tagCHdr  cdr;

	cdr.code = ECHOREP;
	cdr.id = id;
	cdr.len = 6;
	__PppMsgSnd(ppp,PPP_LCP,&cdr,buf,len,ppp->lcp.peer.accm);
	return true;
}
//usage:we use this function to deal a lcp msg
static bool_t __LcpFrameDeal(tagPppMsg   *msg )
{
	tagCHdr      hdr;
	u8          *src;
	u16          len;
	src = msg->data;
	//now do the decode now
	memcpy(&hdr,src,CN_PPPOS_CHDRSIZE);
	hdr.len = htons(hdr.len);
	src += CN_PPPOS_CHDRSIZE;
	len = msg->len - CN_PPPOS_CHDRSIZE;
//	printf("LCP:code:%02x id:%02x len:%04x\n\r",hdr.code,hdr.id,hdr.len);
	//now decode the lcp options here
	switch(hdr.code)
	{
		case CONFREQ:
			__LcpDealCRQ(msg->ppp,hdr.id,src,len);
			break;
		case CONFACK:
			__LcpDealCACK(msg->ppp,hdr.id,src,len);
			break;
		case CONFNAK:
			__LcpDealCNACK(msg->ppp,hdr.id,src,len);
			break;
		case CONFREJ:
			__LcpDealCREJ(msg->ppp,hdr.id,src,len);
			break;
		case TERMREQ:
			//we should send the term ack and turn to the down state
			hdr.code = TERMACK;
			__PppMsgSnd(msg->ppp,PPP_LCP,&hdr,src,len,0xFFFFFFFF);
			//goto dead
			msg->ppp->stat = EN_PPP_DEAD;
			printf("LCP:TERMINATE!,GOTO DEAD\n\r");
			break;
		case TERMACK:
			//goto dead
			if(msg->ppp->stat == EN_PPP_TERMINATE)
			{
				msg->ppp->stat = EN_PPP_DEAD;
				printf("LCP:TERMINATE ACK!,GOTO DEAD\n\r");
			}
			break;
		case CODEREJ:
			break;
		case PROTOREJ:
			break;
		case ECHOREQ:
			__LcpDealEchoReq(msg->ppp,hdr.id,src,len);
			break;
		case ECHOREP:
			msg->ppp->lcp.host.id++;
			msg->ppp->mstimeout.timeoutnum = 0;
			break;
		case DISREQ:
			break;
		default://unknow the code, we should send the reject code here
			hdr.code = CODEREJ;
			hdr.len = htons(msg->len);
			__PppMsgSnd(msg->ppp,PPP_LCP,&hdr,msg->data,msg->len,0xffffff);
			break;
	}
	return true;
}


typedef enum
{
	EN_PAP_REQ = 1,
	EN_PAP_ACK,
	EN_PAP_NACK,
}enPapCode;
//usage:use this function to send our auth
bool_t __AuthSndRequest(tagPppCtrl *ppp)
{
	u8       len;
	tagCHdr  cdr;
	tagAuth  *auth;
	u8       *buf;
	u8       *dst;

	if(ppp->lcp.peer.authcode == PPP_PAP) //USE USERNAME AND PASSWD
	{
		auth = &ppp->auth;
		len = strlen(ppp->user) + strlen(ppp->passwd) + 2;
		buf = malloc(len);
		if(NULL != buf)
		{
			dst = buf;
			*dst++ = strlen(ppp->user);
			memcpy(dst,ppp->user,strlen(ppp->user));
			dst += strlen(ppp->user);

			*dst++ = strlen(ppp->passwd);
			memcpy(dst,ppp->passwd,strlen(ppp->passwd));
			dst += strlen(ppp->passwd);

			cdr.code = EN_PAP_REQ;
			cdr.id = auth->id;
			cdr.len = htons(len + CN_PPPOS_CHDRSIZE);
			__PppMsgSnd(ppp,PPP_PAP,&cdr,buf,len,ppp->lcp.peer.accm);
			//free the buf
			free(buf);
		}
	}

	return true;
}

//usage:we use this function to deal a auth msg
static bool_t __AuthFrameDeal(tagPppMsg   *msg)
{
	tagCHdr      hdr;
	u8          *src;
	u16          len,proto;
	tagPppCtrl *ppp;

	ppp = msg->ppp;
	src = msg->data;
	proto = msg->proto;
	len = msg->len;
	//now do the decode now
	memcpy(&hdr,src,CN_PPPOS_CHDRSIZE);
	hdr.len = htons(hdr.len);
	src += CN_PPPOS_CHDRSIZE;
	len -= CN_PPPOS_CHDRSIZE;
//	printf("AUTH:code:%02x id:%02x len:%04x\n\r",hdr.code,hdr.id,hdr.len);
	if((proto == PPP_PAP)&&(ppp->lcp.host.authcode == PPP_PAP))
	{
		//we support the pap mode
		if(hdr.code == EN_PAP_ACK)
		{
			//we has passed ,goto the ncp
			ppp->mstimeout.timeoutnum = 0;
			ppp->stat = EN_PPP_NCP;
			printf("AUTH SUCCESS,GOTO NCP\n\r");
		}
		else if(hdr.code == EN_PAP_NACK)
		{
			ppp->stat = EN_PPP_DEAD; //FAILED
			ppp->mstimeout.timeoutnum = 0;
			ppp->stat = EN_PPP_TERMINATE;
			printf("AUTH FAILED,GOTO NCP\n\r");
		}
		else
		{
			//other frame should be ignored
		}
	}
	else
	{
		//the chap not supported yet--TODO
	}
	return true;
}

//usage:we use this function to deal a ncp msg
static bool_t __NcpSndRequest(tagPppCtrl *ppp)
{

	u8                 *dst;
	tagPppOptions       opt;
	u32                 v32;
	tagCHdr             cdr;
	tagNcp             *ncp;

	ncp = &ppp->ncp;
	dst = ncp->buf;
	//make the new request
	if(ncp->enIp) //check the ip
	{
		opt.type = NCP_OPT_IP;
		opt.len = 6;
		memcpy(dst,&opt,sizeof(opt));
		dst += sizeof(opt);
		v32 = ncp->ip;
		memcpy(dst,&v32,sizeof(v32));
		dst += sizeof(v32);
	}
	if(ncp->enDns) //check the dns
	{
		opt.type = NCP_OPT_DNS;
		opt.len = 6;
		memcpy(dst,&opt,sizeof(opt));
		dst += sizeof(opt);
		v32 = ncp->dns;
		memcpy(dst,&v32,sizeof(v32));
		dst += sizeof(v32);
	}
	if(ncp->enDnsBak) //check the dns
	{
		opt.type = NCP_OPT_DNS2;
		opt.len = 6;
		memcpy(dst,&opt,sizeof(opt));
		dst += sizeof(opt);
		v32 = ncp->dnsbak;
		memcpy(dst,&v32,sizeof(v32));
		dst += sizeof(v32);
	}
	if(ncp->enNbns) //check the dns
	{
		opt.type = NCP_OPT_NBNS;
		opt.len = 6;
		memcpy(dst,&opt,sizeof(opt));
		dst += sizeof(opt);
		v32 = ncp->nbns;
		memcpy(dst,&v32,sizeof(v32));
		dst += sizeof(v32);
	}
	if(ncp->enNbnsBak) //check the dns
	{
		opt.type = NCP_OPT_NBNS2;
		opt.len = 6;
		memcpy(dst,&opt,sizeof(opt));
		dst += sizeof(opt);
		v32 = ncp->nbnsbak;
		memcpy(dst,&v32,sizeof(v32));
		dst += sizeof(v32);
	}
	ncp->datalen = dst- ncp->buf;
	cdr.code = CONFREQ;
	cdr.id = ncp->id;
	cdr.len = htons(ncp->datalen + CN_PPP_HDRSIZE);
	__PppMsgSnd(ppp,PPP_IPCP,&cdr,ncp->buf,ncp->datalen,ppp->lcp.peer.accm);
	return true;
}
static bool_t __NcpOptionModify(tagNcp *ncp,u8 *src,u16 len,bool_t rej)
{
	u8                 *stop;
	tagPppOptions      *opt;
	u32                 v32;
	stop = src + len;
	//now decode the ncp options here
	while(src < stop)
	{
		opt = (tagPppOptions *)src;
		src += opt->len;
		switch (opt->type)
		{
			case NCP_OPT_IP:
//				printf("IPV4:");
				if(rej)
				{
					ncp->enIp = 0;
//					printf("REJECT\n\r");
				}
				else
				{
					memcpy(&v32,opt->v,sizeof(v32));
					ncp->ip = v32;
					ncp->enIp = 1;
//					printf("%08x(ADVICE)\n\r",v32);
				}
				break;
			case NCP_OPT_DNS:
//				printf("DNS:");
				if(rej)
				{
					ncp->enDns = 0;
//					printf("REJECT\n\r");
				}
				else
				{
					memcpy(&v32,opt->v,sizeof(v32));
					ncp->dns = v32;
					ncp->enDns =1;
//					printf("%08x(ADVICE)\n\r",v32);
				}
				break;
			case NCP_OPT_DNS2:
//				printf("DNSBAK:");
				if(rej)
				{
					ncp->enDnsBak = 0;
//					printf("REJECT\n\r");
				}
				else
				{
					memcpy(&v32,opt->v,sizeof(v32));
					ncp->dnsbak = v32;
					ncp->enDnsBak = 1;
//					printf("%08x(ADVICE)\n\r",v32);
				}
				break;
			case NCP_OPT_NBNS:
//				printf("NBNS:");
				if(rej)
				{
					ncp->enNbns = 0;
//					printf("REJECT\n\r");
				}
				else
				{
					memcpy(&v32,opt->v,sizeof(v32));
					ncp->nbns = v32;
					ncp->enNbns = 1;
//					printf("%08x(ADVICE)\n\r",v32);
				}
				break;
			case NCP_OPT_NBNS2:
//				printf("NBNSBAK:");
				if(rej)
				{
					ncp->enNbnsBak = 0;
//					printf("REJECT\n\r");
				}
				else
				{
					memcpy(&v32,opt->v,sizeof(v32));
					ncp->nbnsbak = v32;
					ncp->enNbnsBak=1;
//					printf("%08x(ADVICE)\n\r",v32);
				}
				break;
			default:
				break;
		}
	}
	ncp->id++;
	return true;
}

static bool_t __NcpFrameDeal(tagPppMsg   *msg)
{
	tagCHdr      hdr;
	u8          *src;
	u16          len;
	tagPppCtrl *ppp;
	tagNcp       *ncp;

	ppp = msg->ppp;
	src = msg->data;
	//now do the decode now
	memcpy(&hdr,src,CN_PPPOS_CHDRSIZE);
	hdr.len = htons(hdr.len);
	src += CN_PPPOS_CHDRSIZE;
	len = msg->len -  CN_PPPOS_CHDRSIZE;
//	printf("ipcp:code:%02x id:%02x len:%04x\n\r",hdr.code,hdr.id,hdr.len);
	//here we should check if it is a ack nack rej or something else
    ncp = &ppp->ncp;
	switch(hdr.code)
    {
		case CONFACK:
			//turn to another state
			ppp->mstimeout.timeoutnum = 0;
			ppp->stat = EN_PPP_NETWORK;
			//here we should create a net device and build a rout here
			//modify the rout
			RoutSetDefaultAddr(EN_IPV_4,ncp->ip,0xFFFFFFFF,ncp->ip,ncp->dns);
			printf("NCP SUCCESS!,GOTO NETWORK\n\r");
			break;
    	case CONFREQ:
    		if(len != 0)
    		{
    			//reject all
    			hdr.len = htons(hdr.len);
    			hdr.code = CONFREJ;
    			__PppMsgSnd(ppp,PPP_IPCP,&hdr,src,len,ppp->lcp.peer.accm);
    		}
    		else
    		{
    			//ack it
    			hdr.code = CONFACK;
    			hdr.len = htons(hdr.len);
    			__PppMsgSnd(ppp,PPP_IPCP,&hdr,NULL,0,ppp->lcp.peer.accm);
    		}
    		break;
    	case CONFNAK:
    		//modify the request and resend the request
    		__NcpOptionModify(ncp,src,len,false);
    		__NcpSndRequest(ppp);
    		break;
    	case CONFREJ:
    		//modify the request and resend the request
    		__NcpOptionModify(ncp,src,len,true);
    		__NcpSndRequest(ppp);
    		break;
    	default:
    		//send the code reject
			hdr.code = CODEREJ;
			hdr.len = htons(hdr.len);
			__PppMsgSnd(ppp,PPP_IPCP,&hdr,src,len,ppp->lcp.peer.accm);
    		break;
    }
	return true;
}
//-----------------------------------------------------------------------------
//功能:this the ppp main task thread
//参数:
//返回:
//备注:
//作者:zhangqf@上午10:50:16/2017年1月5日
//-----------------------------------------------------------------------------
#define CN_LCP_TIME        (5*1000*mS)
#define CN_AUTH_TIME       (5*1000*mS)
#define CN_NCP_TIME        (5*1000*mS)
#define CN_IP_TIME         (10*1000*mS)
#define CN_TIMEOUT_LIMIT    5
ptu32_t __PppDealEngine(void)
{
	tagPppMsg     *msg = NULL;
	tagPppCtrl    *ppp;
	tagCHdr        hdr;
	u64            timenow;

	Djy_GetEventPara((ptu32_t *)&ppp,NULL);
	//first we must make the link up,which use the at subsets to make the link up
	while(1)
	{
		msg = __NetFrameGet(CN_PPPOS_MSGTIMEOUT);
		if(NULL != msg)
		{
			__PppFrameInfo(ppp,true,msg->proto,msg->data,msg->len);
			//we receive a message from the peer, we should deal it carefully
			if(msg->ppp->stat > EN_PPP_DEAD)
			{
				switch(msg->proto)
				{
					case PPP_LCP://this is a lcp frame,so we should deal it with the LCP
						__LcpFrameDeal(msg);
						break;
					case PPP_PAP:
						__AuthFrameDeal(msg);
						break;
					case PPP_CHAP:
						__AuthFrameDeal(msg);
						break;
					case PPP_IPCP:
						__NcpFrameDeal(msg);
						break;
					case PPP_IP:
						__IpFrameDeal(msg);
						break;
					default:
						//maybe a unknown proto, we should send a protorej message to the peer
						hdr.code = PROTOREJ;
						hdr.id = ppp->lcp.host.id;
						hdr.len = htons(msg->len);
						__PppMsgSnd(msg->ppp,PPP_LCP,&hdr,msg->data,msg->len,0xffffff);
						break;
				}
			}
			free(msg);
		}
		else
		{
			//maybe timeout, we should check if any ppp to do the timeout resend
			timenow = (u64)DjyGetSysTime();
			switch(ppp->stat)
			{
				case EN_PPP_ATDEBUG:
					//for this mode, we should do the at debug
					break;
				case EN_PPP_DEAD:
					//use the at command to do the link up
					if(__DialUp(ppp))
					{
						__PppMsReset(ppp,EN_PPP_ESTABLISH);
						printf("DIAL SUCCESS!GOTO LCP\n\r");
					}
					else
					{
						//we should do the hard reset
						if(NULL != ppp->modemreset)
						{
							ppp->modemreset();
						}
					}
					break;
				case EN_PPP_ESTABLISH:
					//we should check if the config request timeout
					if(timenow > ppp->mstimeout.timedead)
					{
						if(ppp->mstimeout.timeoutnum < CN_TIMEOUT_LIMIT)
						{
							__LcpSndRequest(ppp);
							ppp->mstimeout.timedead += CN_LCP_TIME;
							ppp->mstimeout.timeoutnum++;
						}
						else
						{
							ppp->mstimeout.timeoutnum = 0;
							ppp->stat = EN_PPP_TERMINATE;
							printf("LCP TIMEOUT\n\r");
						}
					}
					break;
				case EN_PPP_AUTH:
					//we should check if the auth timeout
					if(timenow > ppp->mstimeout.timedead)
					{
						if(ppp->mstimeout.timeoutnum < CN_TIMEOUT_LIMIT)
						{
							__AuthSndRequest(ppp);
							ppp->mstimeout.timedead += CN_AUTH_TIME;
							ppp->mstimeout.timeoutnum++;
						}
						else
						{
							ppp->mstimeout.timeoutnum = 0;
							ppp->stat = EN_PPP_TERMINATE;
							printf("AUTH TIMEOUT\n\r");
						}
					}
					break;
				case EN_PPP_NCP:
					//we should check if the ncp timeout
					if(timenow > ppp->mstimeout.timedead)
					{
						if(ppp->mstimeout.timeoutnum < CN_TIMEOUT_LIMIT)
						{
							__NcpSndRequest(ppp);
							ppp->mstimeout.timedead += CN_NCP_TIME;
							ppp->mstimeout.timeoutnum++;
						}
						else
						{
							ppp->mstimeout.timeoutnum = 0;
							ppp->stat = EN_PPP_TERMINATE;
							printf("NCP TIMEOUT\n\r");
						}
					}
					break;
				case EN_PPP_NETWORK:
					//here we should check if any echo request timeout
					if(timenow > ppp->mstimeout.timedead)
					{
						if(ppp->mstimeout.timeoutnum < CN_TIMEOUT_LIMIT)
						{
							__LcpSendEchoReq(ppp);
							ppp->mstimeout.timedead += CN_IP_TIME;
							ppp->mstimeout.timeoutnum++;
						}
						else
						{
							ppp->mstimeout.timeoutnum = 0;
							ppp->stat = EN_PPP_TERMINATE;
							printf("IP TIMEOUT\n\r");
						}
					}
					break;
				case EN_PPP_TERMINATE:
					//do the reset and shut down
					if(timenow > ppp->mstimeout.timedead)
					{
						if(ppp->mstimeout.timeoutnum < CN_TIMEOUT_LIMIT)
						{
							__LcpSendTerminateReq(ppp);
							ppp->mstimeout.timedead += CN_NCP_TIME;
							ppp->mstimeout.timeoutnum++;
						}
						else
						{
							ppp->mstimeout.timeoutnum = 0;
							printf("TERMINATE TIMEOUT\n\r");
						}
					}
					break;
				default:
					//do the reset and shut down
					break;
			}
		}
	}
	return 0;
}
////////////////////////THE FOLLOWING USED TO DECODE THE LINK FRAME////////////
//-----------------------------------------------------------------------------
//功能:use this function to do the common proto inputs
//参数:
//返回:return how many data we has dealt
//备注:
//作者:zhangqf@下午8:05:58/2017年1月4日
//-----------------------------------------------------------------------------
//use this function to do the receive state reset
static void __RcvEngineReset(tagPppRcvCtrl *rxctrl)
{
	rxctrl->fcs = PPP_INITFCS;
	rxctrl->stat = EN_PPPOS_RX_DROPS;
	rxctrl->framlen = 0;
	rxctrl->datalen = 0;
	rxctrl->inescape = 0;
	rxctrl->protocol = 0;
}

static u16 __ProtoRcv(tagPppCtrl *ppp,u8 *s,u16 l )
{
	u8              curchar;
	tagPppRcvCtrl *rxctrl;
	u8             *str;
	u16             lendone;
	tagCHdr         hdr;
	str = s;
	rxctrl = &ppp->rxctrl;
	while (l-- > 0)
	{
		curchar = *str++;
		if(curchar == PPP_ESCAPE) //it is a transform char
		{
			rxctrl->inescape = 1;  //record the transform state
		}
		else if(curchar == PPP_FLAG)
		{
			//which means the frame is not complete, so drop it
			str--; //make the ppp_flag still could be used
			__RcvEngineReset(rxctrl);//reset the receive state
			break;
		}
		else
		{
			if (rxctrl->inescape) //clear the transform state and transform the char
			{
				rxctrl->inescape = 0;
				curchar ^= PPP_TRANS;
				//maybe we should check whether it is in the accm
				//if it is not in the accm, we should go to the drop stat
			}
			rxctrl->fcs = PPP_FCS(rxctrl->fcs, curchar);
			//put it to the receive buffer,if buffer is full then drop the frame
			rxctrl->buf[rxctrl->datalen] = curchar;
			rxctrl->datalen++;
			if(rxctrl->datalen == CN_PPPOS_CHDRSIZE)
			{
				//we has get the total header, then we could decode the framelen
				memcpy(&hdr,rxctrl->buf,CN_PPPOS_CHDRSIZE);
				rxctrl->framlen  = ntohs(hdr.len) + PPP_FCSLEN;//we should also receive the fcs
			}
			else if(rxctrl->datalen == rxctrl->framlen)
			{
				//then check the fcs if it is good enough
				if(rxctrl->fcs == PPP_GOODFCS)
				{
					//dispatch the frame check sum
					rxctrl->datalen -= PPP_FCSLEN;
					//pass the message to the msg  queue of the stat machine
					__NetFramePut(ppp);
				}
				else
				{
					//drop the frame please
					TCPIP_DEBUG_INC(ppp->rxinfo.numfcserr);
				}
				__RcvEngineReset(rxctrl);//reset the receive state
				break;
			}
			else if(rxctrl->datalen >= rxctrl->rxbuflen)
			{
				//maybe overlap,then drop this
				__RcvEngineReset(rxctrl);//reset the receive state
				break;
			}
			else
			{
				//continue to decode the frame
			}
		}
	} /* while (l-- > 0), all bytes processed */
	lendone = (u16)(str -s);
	return lendone;
}
static void __PppInput(tagPppCtrl *ppp,u8 *s,u16 l)
{
	u8              curchar;
	tagPppRcvCtrl *rxctrl;
	u16             lendone;

	rxctrl = &ppp->rxctrl;
	while (l > 0)
	{
		if(rxctrl->stat == EN_PPPOS_RX_CPROTO)
		{
			//let the proto do the decode
			lendone = __ProtoRcv(ppp,s,l);  //let the proto do the left
			if(lendone != l)
			{
				s += lendone;
				l -= lendone;
				//do another
				__RcvEngineReset(rxctrl);
			}
		}
		else
		{
			curchar = *s++;
			l--;
			if(curchar == PPP_ESCAPE) //it is a transform char
			{
				rxctrl->inescape = 1;  //record the transform state
			}
			else
			{
				if ((rxctrl->inescape)&&(curchar != PPP_FLAG)) //clear the transform state and transform the char
				{
					rxctrl->inescape = 0;
					curchar ^= PPP_TRANS;
				}
				//call the machine state to do the decode
				switch(rxctrl->stat)
				{
					case EN_PPPOS_RX_DROPS:
						if(curchar== PPP_FLAG)
						{
							rxctrl->stat = EN_PPPOS_RX_START; //find a start flag
						}
						break;
					case EN_PPPOS_RX_START:
						if(curchar == PPP_FLAG)
						{
							//external flag,just stay in the same state
							break;
						}
						else if(curchar == PPP_ALLSTATIONS)
						{
							rxctrl->stat = EN_PPPOS_RX_ADDRESS;
							rxctrl->fcs = PPP_FCS(rxctrl->fcs, curchar);
							break;
						}
					case EN_PPPOS_RX_ADDRESS:
						if(curchar == PPP_UI)
						{
							rxctrl->stat = EN_PPPOS_RX_CONTROL;
							rxctrl->fcs = PPP_FCS(rxctrl->fcs, curchar);
							break;
						}
					case EN_PPPOS_RX_CONTROL:
						if(0 ==(curchar&0x01))
						{
							rxctrl->stat = EN_PPPOS_RX_P1;
							rxctrl->protocol = curchar<<8;
							rxctrl->fcs = PPP_FCS(rxctrl->fcs, curchar);
							break;
						}
					case EN_PPPOS_RX_P1:
						rxctrl->protocol |= curchar;
						rxctrl->fcs = PPP_FCS(rxctrl->fcs, curchar);
						//find which proto it should goto
						if((rxctrl->protocol == PPP_LCP)||\
								(rxctrl->protocol == PPP_PAP)||\
								(rxctrl->protocol == PPP_CHAP)||\
								(rxctrl->protocol == PPP_IP)||\
								(rxctrl->protocol == PPP_IPCP))
						{
							rxctrl->stat = EN_PPPOS_RX_CPROTO;
						}
						else
						{
							//can't find the proto, then do the proreject--TODO
							//and goto the drop stat
							__RcvEngineReset(rxctrl);
						}
						break;
					default:
						__RcvEngineReset(rxctrl);
						break;
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
//功能:this is the main receive thread function
//参数:
//返回:
//备注:
//作者:zhangqf@下午4:21:33/2017年1月5日
//-----------------------------------------------------------------------------
static ptu32_t __PppRcvEngine(void)
{
#define CN_READ_BUF 128

	u8  siobuf[CN_READ_BUF];
	int len;
	tagPppCtrl *ppp;
	Djy_GetEventPara((ptu32_t *)&ppp,NULL);
	if(NULL != ppp)
	{
		//do the receive state reset
		__RcvEngineReset(&ppp->rxctrl);
		while(1)
		{
			//we need the uart to echo more efficient
			len = iodevread(ppp->sfd,siobuf,CN_READ_BUF);
			if(len > 0)
			{
				__RcvMsgPush(siobuf,len);
				__PppInput(ppp,siobuf,len);
			}
			else
			{
				//do the receive reset
				__RcvEngineReset(&ppp->rxctrl);//reset the receive state
			}
		}
	}

	return 0;
}
//usage:show the ppp state
static void __PppStat(tagPppCtrl *ppp)
{
	tagPppDebug *debug;

	printf("MODEMINFO:\n\r");
	printf("         :apn :%s\n\r",ppp->apn);
	printf("         :user:%s\n\r",ppp->user);
	printf("         :pass:%s\n\r",ppp->passwd);
	printf("         :dev :%s\n\r",ppp->iodevname);
	printf("         :cgmi:%s\n\r",ppp->cgmi);
	printf("         :cgmm:%s\n\r",ppp->cgmm);
	printf("         :cgmr:%s\n\r",ppp->cgmr);
	printf("         :cgsn:%s\n\r",ppp->cgsn);

	printf("PPP:STATE:%s\n\r",gPppStatName[ppp->stat]);
	printf("%4s%-9s%-9s%-9s%-9s%-9s%-9s%-9s%-9s%-9s%-9s\n\r",\
			"Type","Total","OkNum","FcsErrNum","OverNum","LcpNum","PapNum",\
			"ChapNum","IpcpNum","IpNum","Unknown");
	debug = &ppp->rxinfo;
	printf("%4s%08x %08x %08x %08x %08x %08x %08x %08x %08x %08x\n\r",\
			"Rcv:",debug->numtotal,debug->numok,debug->numfcserr,debug->numoverlap,\
			debug->numlcp,debug->numpap,debug->numchap,debug->numipcp,debug->numip,debug->numunknown);
	debug = &ppp->txinfo;
	printf("%4s%08x %08x %08x %08x %08x %08x %08x %08x %08x %08x\n\r",\
			"Snd:",debug->numtotal,debug->numok,debug->numfcserr,debug->numoverlap,\
			debug->numlcp,debug->numpap,debug->numchap,debug->numipcp,debug->numip,debug->numunknown);
}
//usage:use this function do the shell interface
static bool_t __PppStatShell(char *param)
{
	int argc = 4;
	const char *argv[4];
	string2arg(&argc,argv,param);
	if(argc > 0)
	{

	}
	else
	{
		//show the ppp mode here
		__PppStat(&gPppCtrl);
	}
	return true;
}
//usage:do the at command in the shell mode
static bool_t __AtCmdShell(char *param)
{
	u8  buf[CN_AT_LEN];

	if(__AtCmd(&gPppCtrl,param,buf,CN_AT_LEN,NULL,0)&&\
			(strstr(buf,"OK")||strstr(buf,"ok")))
	{
		printf("AT:%s:OK\n\r",param);
	}
	else
	{
		printf("AT:%s:FAILED\n\r",param);
	}
	return true;
}
//use the at command to change to data mode
static bool_t __DialUpShell(char *param)
{
	bool_t result;
	result = __DialUp(&gPppCtrl);
	return result;
}

//make the data mode to the at command mode
static bool_t __DialEndShell(char *param)
{
	gPppCtrl.stat = EN_PPP_TERMINATE;
	gPppCtrl.mstimeout.timedead = 1;
	return true;
}
//do the at debug
static bool_t __AtDebugShell(char *param)
{
	gPppCtrl.stat = EN_PPP_ATDEBUG;
	__LcpSendTerminateReq(&gPppCtrl);
	return true;
}
//do the data mode
static bool_t __DialDebugShell(char *param)
{
	gPppCtrl.stat = EN_PPP_DEAD;
	return true;
}

//usage:use this function to set the debug mode
static bool_t  __PppFrameDebugMode(char *param)
{
	int level;
	level = strtol(param,NULL,0);
	if(level)
	{
		gPppFrameDebug = true;
	}
	else
	{
		gPppFrameDebug = false;
	}
	return true;
}
//usage:this function used for the modem reset
static bool_t __ModemReset(char *param)
{
	if(NULL!= gPppCtrl.modemreset)
	{
		gPppCtrl.modemreset();
	}
	return true;
}

extern bool_t IoDevDebug(char *param);
//usage:this is the ppp cmd for the os shell
struct ShellCmdTab  gPppDebug[] =
{
    {
        "iocmd",
		__AtCmdShell,
        "usage:iocmd+ATCMD",
        "usage:iocmd+ATCMD",
    },
    {
        "pppdebug",
		__PppFrameDebugMode,
        "usage:pppdebug",
        "usage:pppdebug",
    },
    {
        "pppstate",
		__PppStatShell,
        "usage:pppstat",
        "usage:pppstat",
    },
    {
        "dial",
		__DialUpShell,
        "usage:dialup",
        "usage:dialup",
    },
    {
        "dialend",
		__DialEndShell,
        "usage:dialend",
        "usage:dialend",
    },
    {
        "atmode",
		__AtDebugShell,
        "usage:atmode",
        "usage:atmode",
    },
    {
        "dialmode",
		__DialDebugShell,
        "usage:dialmode",
        "usage:dialmode",
    },
    {
        "iodebug",
		IoDevDebug,
        "usage:iodebug+type+mode",
        "usage:iodebug+type+mode",
    },
    {
        "modemreset",
		__ModemReset,
        "usage:modemreset",
        "usage:modemreset",
    }
};
#define CN_PPPDEBUG_NUM  ((sizeof(gPppDebug))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc gPppDebugCmdRsc[CN_PPPDEBUG_NUM];
//-----------------------------------------------------------------------------
//功能:this is the ppp main function here
//参数:
//返回:
//备注:
//作者:zhangqf@下午4:06:57/2017年1月5日
//-----------------------------------------------------------------------------
bool_t ModuleInstall_Ppp(const char *devname,const char *user,const char *passwd,const char *apn,bool_t (*fnModemReset)(void))
{
	tagPppCtrl  *ppp;

	//initialize the message queue
	memset(&gPppMsgQ,0,sizeof(gPppMsgQ));
	gPppMsgQ.mutex = mutex_init(NULL);
	if(NULL == gPppMsgQ.mutex)
	{
		goto EXIT_MSGBOX_MUTEX;
	}
	gPppMsgQ.sync = semp_init();
	if(NULL == gPppMsgQ.sync)
	{
		goto EXIT_MSGBOX_SYNC;
	}
	//make the ppp control block and initialize it
	ppp = &gPppCtrl;  //now make a static one
	memset((void *)ppp,0,sizeof(tagPppCtrl));
	ppp->rxctrl.rxbuflen = CN_PPPOS_RXTXLEN;   //receive buffer
	ppp->txctrl.buflen = CN_PPPOS_RXTXLEN;     //send buffer
	ppp->txctrl.mutex = mutex_init(NULL);
	if(NULL == ppp->txctrl.mutex)
	{
		goto EXIT_TXBUF_MUTEX;
	}
	__PppMsReset(ppp,EN_PPP_DEAD);
	strncpy(ppp->apn,apn,CN_NAME_LIMIT);
	strncpy(ppp->user,user,CN_NAME_LIMIT);
	strncpy(ppp->passwd,passwd,CN_NAME_LIMIT);
	strncpy(ppp->iodevname,devname,CN_NAME_LIMIT);
	snprintf(ppp->rcvtaskname,CN_NAME_LIMIT,"%s%s","ppprcv_",devname);
	snprintf(ppp->dealtaskname,CN_NAME_LIMIT,"%s%s","pppdeal_",devname);

	ppp->modemreset = fnModemReset;
	//now open the serial device
	ppp->sfd = iodevopen(ppp->iodevname);
	if(0  == ppp->sfd)
	{
		goto EXIT_SIO_OPEN;
	}
	//create the two main task
	taskcreate(ppp->dealtaskname,0x800,19,__PppDealEngine,(ptu32_t)&gPppCtrl);
	taskcreate(ppp->rcvtaskname,0x800,20,__PppRcvEngine,(ptu32_t)&gPppCtrl);
	ppp->nfd=__PppNetDevAdd((ptu32_t)ppp);
	//debug.for init the rcv hook
	gRcvHook.mutex = mutex_init(NULL);
	gRcvHook.semp = semp_init();
	gRcvHook.buf = NULL;
	gRcvHook.buflen = 0;
	gRcvHook.datalen = 0;
	//install the debug shell for the system
	Sh_InstallCmd(gPppDebug,gPppDebugCmdRsc,CN_PPPDEBUG_NUM);
	printf("%s:success!\n\r",__FUNCTION__);
	return true;

EXIT_SIO_OPEN:
	mutex_del(ppp->txctrl.mutex);
	ppp->txctrl.mutex = NULL;
EXIT_TXBUF_MUTEX:
	semp_del(gPppMsgQ.sync);
	gPppMsgQ.sync = NULL;
EXIT_MSGBOX_SYNC:
	mutex_del(gPppMsgQ.mutex);
	gPppMsgQ.mutex = NULL;
EXIT_MSGBOX_MUTEX:
	printf("%s:err!\n\r",__FUNCTION__);
	return false;
}

