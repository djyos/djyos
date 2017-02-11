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
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <os.h>
//add your own specified header here

#include <sys/socket.h>

#include "sio.h"
#include "pppos.h"
#include "osarch.h"
/*
 * FCS lookup table as calculated by genfcstab.
 * @todo: smaller, slower implementation for lower memory footprint?
 */
static const u_short fcstab[256] = {
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
 * Constants and structures defined by the internet system,
 * Per RFC 790, September 1981, and numerous additions.
 */

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

/*
 * Packet sizes
 *
 * Note - lcp shouldn't be allowed to negotiate stuff outside these
 *    limits.  See lcp.h in the pppd directory.
 * (XXX - these constants should simply be shared by lcp.c instead
 *    of living in lcp.h)
 */
#define PPP_MTU                         1500     /* Default MTU (size of Info field) */
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
	//the following for the debug info
	struct
	{
		u32    numfcserr;     //which means the frame droped for the fcserr;
		u32    numuncomplete; //which means the stop flag comes too early
		u32    numoverlap;    //which means the frame too large
		u32    numunknown;    //which means we don't know the proto
		u32    numok;         //which means how many we have received
		u32    numlcp;        //which means how many for the lcp
		u32    numpap;        //which means how many for the pap
		u32    numchap;       //which means how many for the chap
		u32    numipcp;       //which means how many for the ipcp
		u32    numip;         //which means how many for the ip
	}debug;
}tagPpposRxCtrl;
//usage:this data structure used for the send control
typedef struct
{
	u16      datalen;
	u16      buflen;
	u8       buf[CN_PPPOS_RXTXLEN];
	mutex_t  mutex;   //used for the multi thread
	u16      fcs;
	struct
	{
		u32 numsend;
		u32 numerr;
	}debug;

}tagPpposTxCtrl;
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
	s64      timeout;
	u8       buf[CN_OPTION_BUFLEN];
	u8       buflen;
	u8       datalen;
}tagLcpHostStat;
#define CN_PPPOS_TIMEOUT   (5*1000*mS)  //1 second,if not ack receive then do the resend
//usage:this data structure used for the lcp
typedef struct
{
	tagLcpPeerOptions   peer;    //the peer options
	tagLcpHostStat      host;    //the peer options
}tagLcp;
//usage:this data structure used for the auth
#define CN_NAMELEN_LIMIT  32
typedef struct
{
	u8  user[CN_NAMELEN_LIMIT];
	u8  userlen;
	u8  passwd[CN_NAMELEN_LIMIT];
	u8  passlen;
	u8  id;
	u64 timeout;
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
	u64 timeout;
}tagNcp;
#define NCP_OPT_VJTCP 2
#define NCP_OPT_IP    3
#define NCP_OPT_DNS   0X81
#define NCP_OPT_NBNS  0X82
#define NCP_OPT_DNS2  0X83
#define NCP_OPT_NBNS2 0X84
//usage:this data structure used for the pppos controller
typedef enum
{
	EN_PPP_DEAD = 0,         //the link has not established yet
	EN_PPP_ESTABLISH,        //do the lcp negotiation
	EN_PPP_AUTH,             //do the authentication
	EN_PPP_NCP,              //do the net negotiation
	EN_PPP_NETWORK,          //do the normal net work
	EN_PPP_TERMINATE,        //do the normal shutdown
}enPppStat;
const char *gPppStatName[]={"DEAD","ESATBLISH","AUTH","NCP","NETWORK","TERM"};
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
}tagPpposDebug;
typedef bool_t (*fnPppDialUp)(ptu32_t fd,u8 type);
typedef struct
{
	tagPpposRxCtrl      rxctrl;  //used for the receive
	tagPpposTxCtrl      txctrl;  //used for the send
	ptu32_t             sfd;     //the serial device fd
	ptu32_t             nfd;     //the net device fd
	tagLcp              lcp;     //used for the lcp
	tagAuth             auth;    //used for the auth
	tagNcp              ncp;     //used for the ncp
	enPppStat           stat;    //used for the machine state
	fnPppDialUp         fnDial;  //used for the dial up
	tagPpposDebug       rxinfo;
	tagPpposDebug       txinfo;
}tagPpposCtrl;
static tagPpposCtrl gPppos;

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
typedef struct __PpposMsg
{
	struct __PpposMsg *nxt;
	tagPpposCtrl      *pppos;
	u16 len;
	u16 proto;
	u8  data[0];
}tagPpposMsg;
#define CN_PPPOS_MSGHDR_SZIE   sizeof(tagPpposMsg)
#define CN_PPPOS_MSGTIMEOUT    (200*mS)
typedef struct
{
	tagPpposMsg   *msgH;
	tagPpposMsg   *msgT;
	u16            msglen;
	mutex_t        mutex;
	semp_t         sync;
}tagPpposMsgQ;
static tagPpposMsgQ gPpposMsgQ;


#define CN_NAME_LIMIT   32
//usage:use this data structure to storage the sim info
typedef struct
{




}tagSimInfo;
//usage:use this data structure to storage the modem info
typedef struct
{
	u8  cgmi[CN_NAME_LIMIT];
	u8  cgmm[CN_NAME_LIMIT];
	u8  cgmr[CN_NAME_LIMIT];
	u8  cgsn[CN_NAME_LIMIT];
	u8  cscs[CN_NAME_LIMIT];
}tagModemInfo;
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

//usage:this function to register a receive hook
static int  __ppposRcvHookMessageGet(u8 *buf,u16 len,u32 timeout)
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
			mutex_unlock(gRcvHook.mutex);
		}
	}
	return result;
}
//usage:this function to put the message to the hook
static void  __ppposRcvHookMessagePut(u8 *buf,u16 len)
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

//-----------------------------------------------------------------------------
//功能:use this function to do the at command here and get info
//参数:
//返回:>0, the command execute success, -1 the command the execute failed
//备注:if returns ok then we will copy the info to the buffer
//作者:zhangqf@上午9:57:30/2017年1月16日
//-----------------------------------------------------------------------------
#define CN_AT_SUCEESS_OK      "OK"
//usage: this function to dispath the first visisual strings
int stringVFetch(char *string,char *buf,int len)
{
	int i = 0;
	int cpylen = 0;
	//find the first  visible char
	while(0 == isalnum(string[i]))
	{
		i++;
	}
	while(isalnum(string[i])&&(len > 0))
	{
		*buf++ = string[i];
		i++;
		len--;
		cpylen++;
	}
	return cpylen;
}
//usage:send the at command to the serial device
static int ppposAtCommand(tagPpposCtrl *pppos,char *cmd,u8 *buf,u8 buflen)
{
	u8   cmdbuf[64];
	int  result = -1;
	u8  *okstr;

	memset(cmdbuf,0,64);
	snprintf(cmdbuf,64,"%s\r",cmd);
	result = sio_write(pppos->sfd,cmdbuf,strlen(cmdbuf));
	//then we will wait for the timeout
	if((NULL != buf)&&(buflen > 0))
	{
		memset(cmdbuf,0,64);
		result = __ppposRcvHookMessageGet(cmdbuf,64,CN_SERIAL_TIMEOUT);
		if(result > 0)
		{
			//check success or failure
			okstr = strstr(cmdbuf,CN_AT_SUCEESS_OK);
			if(NULL != okstr)
			{
				//check the len and cpy the message here
				stringVFetch(cmdbuf,buf,buflen);
			}
			else
			{
				result = -1;
			}
		}
	}
	return  result;
}

//usage:do the at command in the shell mode
static bool_t ppposAtCommandShell(char *param)
{
	ppposAtCommand(&gPppos,param,NULL,0);
	return true;
}

//-----------------------------------------------------------------------------
//功能:we use this function to send data to the peer
//参数:
//返回:
//备注:here we add the address control and the tail it self
//作者:zhangqf@下午4:04:11/2017年1月6日
//-----------------------------------------------------------------------------
static bool_t __ppposSend(tagPpposCtrl *pppos,u32 proto,tagCHdr *chdr,u8 *buf,u16 l,u32 accm)
{
	u8             *dst,*dstedge,*src;
	u8              c;
	u16             fcs,len;
	tagPpposTxCtrl *tx;
	tagPppHdr       hdr;

	tx = &pppos->txctrl;
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
			sio_write(pppos->sfd,tx->buf,len);
		}
		else
		{
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
        //send the buf to the pppos
        tagPpposCtrl *pppos;
        pppos = (tagPpposCtrl*)NetDevPrivate(dev);
        result = __ppposSend(pppos,PPP_IP,NULL,buf,cpylen,pppos->lcp.peer.accm);
        free(buf);
    }
    return result;
}

static bool_t __ppposIpDeal(tagPpposMsg   *msg )
{
	//we should package the msg and put it to the stack
	u8 *src,*dst;
	u16 len;
	tagNetPkg *pkg;
	tagPpposCtrl *pppos;

	src = msg->data;
	len = msg->len;
	pppos = msg->pppos;

	pkg = PkgMalloc(len,CN_PKLGLST_END);
	if(NULL != pkg)
	{
		dst = pkg->buf + pkg->offset;
		memcpy(dst,src,len);
		pkg->datalen = len;

		LinkPost(pppos->nfd,pkg);

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
static ptu32_t PppDevAdd(ptu32_t para)
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
    if(NULL == dev)
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


extern bool_t __authSendRequest(tagPpposCtrl *pppos);
//-----------------------------------------------------------------------------
//功能:use this function for the dial up
//参数:
//返回:
//备注:
//作者:zhangqf@下午3:30:54/2017年1月9日
//-----------------------------------------------------------------------------
/* Use AT+CGDCONT=1,"IP","xxx" to define a PDP context where CID=1, PDP type= IP, APN=xxx (xxx is the APN for the network provider China-Mobile and it should be replaced with the APN by your network provider) */
//[2015-09-14 15:09:23:131_S:] AT+CGDCONT=1,"IP","CMNET"
//[2015-09-14 15:09:23:156_R:] 30 0D
//
///* Use ATD*99# to dial up with defined PDP context */
//[2015-09-14 15:09:23:160_S:] ATD*99#
//
//[2015-09-14 15:09:23:174_R:] DSR:1 CTS:1 RI:0 (DCD:1)
//[2015-09-14 15:09:23:176_R:] 31 20 37
//[2015-09-14 15:09:23:189_R:] 32 30 30 30 30 30 0D
static u32  dialEnd = 0;
static bool_t  pppDialUp(ptu32_t fd,u8 type)
{
	while(0 == dialEnd)
	{
		Djy_EventDelay(1000*mS);
	}
	dialEnd = 0;
	return true;
}

static bool_t dialup(char *param)
{
	dialEnd = 1;
	return true;
}
static bool_t dialend(char *param)
{
	u8 data[]={0x7E,0xFF,0x03,0xC0,0x21,0x05,0x05,0x00,0x04,0x5C,0xA4,0x7E};

	sio_write(gPppos.sfd,data,sizeof(data));
	gPppos.stat = EN_PPP_DEAD;
	dialEnd = 0;

	return true;
}

static bool_t sndflag(char *param)
{

	u8 data;
	data = 0x7e;
	sio_write(gPppos.sfd,&data,sizeof(data));
	return true;
}

//-----------------------------------------------------------------------------
//功能:we use this function to pass the receive data to the queue
//参数:
//返回:
//备注:
//作者:zhangqf@上午10:10:36/2017年1月5日
//-----------------------------------------------------------------------------
static bool_t PpposPutMsg(tagPpposCtrl *pppos)
{
	bool_t         result = false;
	u16            msglen;
	tagPpposMsg   *msg;

	msglen = pppos->rxctrl.datalen + CN_PPPOS_MSGHDR_SZIE;
	msg = malloc(msglen);
	if(NULL != msg)
	{
		memset((void *)msg,0,sizeof(tagPpposMsg));
		msg->pppos =pppos;
		msg->len = pppos->rxctrl.datalen;
		msg->proto = pppos->rxctrl.protocol;
		memcpy(msg->data,pppos->rxctrl.buf,pppos->rxctrl.datalen);
		if(mutex_lock(gPpposMsgQ.mutex))
		{
			if(NULL == gPpposMsgQ.msgT)
			{
				gPpposMsgQ.msgH = msg;
			}
			else
			{
				gPpposMsgQ.msgT->nxt = msg;
			}
			gPpposMsgQ.msgT = msg;
			mutex_unlock(gPpposMsgQ.mutex);
			semp_post(gPpposMsgQ.sync);
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
static tagPpposMsg *PpposGetMsg(u32 timeout)
{
	tagPpposMsg   *msg = NULL;
	semp_pendtimeout(gPpposMsgQ.sync,timeout);
	if(mutex_lock(gPpposMsgQ.mutex))
	{
		if(NULL != gPpposMsgQ.msgH)
		{
			if(gPpposMsgQ.msgH == gPpposMsgQ.msgT)
			{
				gPpposMsgQ.msgT = NULL;
			}
			msg = gPpposMsgQ.msgH;
			gPpposMsgQ.msgH = msg->nxt;
			msg->nxt = NULL;
		}
		mutex_unlock(gPpposMsgQ.mutex);
	}
	return msg;
}
//deal the configure request
static bool_t __lcpDealCRQ(tagPpposCtrl *pppos,u8 id,u8 *buf, u16 len)
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
	peer = &pppos->lcp.peer;
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
				printf("mru:0x%04x\n\r",v16);
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
				printf("accm:0x%08x\n\r",v32);
				peer->accm = v32;
				memcpy(&peer->optionack[peer->acklen],opt,opt->len);
				peer->acklen += opt->len;
				break;
			case LCP_OPT_AUTH:
				memcpy(&v16,opt->v,sizeof(v16));
				v16 = ntohs(v16);
				printf("auth:0x%04x\n\r",v16);
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
				printf("magic:0x%04x\n\r",v32);
				peer->macgic = v32;
				memcpy(&peer->optionack[peer->acklen],opt,opt->len);
				peer->acklen += opt->len;
				break;
			case LCP_OPT_PFC: //not supported yet
				printf("pfc:\n\r");
				memcpy(&peer->optionrej[peer->rejlen],opt,opt->len);
				peer->rejlen += opt->len;
				break;
			case LCP_OPT_ACFC://not supported yet
				printf("acfc:\n\r");
				memcpy(&peer->optionrej[peer->rejlen],opt,opt->len);
				peer->rejlen += opt->len;
				break;
			default:
				printf("unknown:type:%02x len:%d\n\r",opt->type,opt->len);
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
		__ppposSend(pppos,PPP_LCP,&cdr,peer->optionrej,peer->rejlen,accm);
	}
	else if(peer->naklen > 0)//if no rej, then do the nack
	{
		cdr.code = CONFNAK;
		cdr.id = id;
		cdr.len = peer->naklen + CN_PPPOS_CHDRSIZE;
		cdr.len = htons(cdr.len);
		__ppposSend(pppos,PPP_LCP,&cdr,peer->optionnack,peer->naklen,accm);
	}
	else //if no rej and nack, then we do the ack
	{
		cdr.code = CONFACK;
		cdr.id = id;
		cdr.len = peer->acklen + CN_PPPOS_CHDRSIZE;
		cdr.len = htons(cdr.len);
		__ppposSend(pppos,PPP_LCP,&cdr,peer->optionack,peer->acklen,accm);
		peer->done = 1;
		if(pppos->lcp.host.done)
		{
			//if the peer has agree the negotiation,then we do the auth
			pppos->stat = EN_PPP_AUTH;
			__authSendRequest(pppos);
			printf("LCP:SUCCESS!,GOTO AUTH\n\r");
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
static bool_t __lcpDealCACK(tagPpposCtrl *pppos,u8 id,u8 *buf, u16 len)
{
	//i think all i want to do is to change the state,and do the next thing
	//maybe should send the pap or chap to the peer
	pppos->lcp.host.done = 1;
	pppos->lcp.host.id++;
	if(pppos->lcp.peer.done)//if the peer has agree the negotiation,then we do the auth
	{
		pppos->stat = EN_PPP_AUTH;
		__authSendRequest(pppos);
		printf("LCP:SUCCESS!,GOTO AUTH\n\r");
	}
	return true;
}

//we use this function to send the lcp request
static bool_t __lcpSendRequest(tagPpposCtrl *pppos)
{

	u8                 *dst;
	tagPppOptions       opt;
	tagLcpHostStat     *host;
	u16                 v16;
	u32                 v32;
	tagCHdr             cdr;
	u32                 accm;

	host = &pppos->lcp.host;
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
	__ppposSend(pppos,PPP_LCP,&cdr,host->buf,host->datalen,accm);
	host->timeout = DjyGetSysTime()+CN_PPPOS_TIMEOUT;

	return true;
}


static bool_t __lcpSendEchoReq(tagPpposCtrl *pppos)
{
	u32     magic;
	tagCHdr cdr;
	u32     accm;

	accm = 0xffffffff;
	magic = pppos->lcp.peer.macgic;
	magic = htonl(magic);

	cdr.code = ECHOREQ;
	cdr.id = pppos->lcp.host.id;
	cdr.len = htons((sizeof(magic)+sizeof(cdr)));
	__ppposSend(pppos,PPP_LCP,&cdr,(u8 *)&magic,sizeof(magic),accm);

	pppos->lcp.host.timeout = DjyGetSysTime()+100*1000*mS;

	return true;
}
//-----------------------------------------------------------------------------
//功能:we use this function to deal the nack options
//参数:
//返回:
//备注:if we receive the nack,it means we should modify the options we send
//作者:zhangqf@上午11:37:16/2017年1月7日
//-----------------------------------------------------------------------------
static bool_t __lcpDealCNACK(tagPpposCtrl *pppos,u8 id,u8 *buf, u16 len)
{
	u8                 *src;
	u8                 *stop;
	tagPppOptions      *opt;
	tagLcpHostStat  *host;
	u16                 v16;
	u32                 v32;

	src = buf;
	stop = buf + len;
	host = &pppos->lcp.host;
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
	__lcpSendRequest(pppos);
	return true;
}
//-----------------------------------------------------------------------------
//功能:we use this function to deal the reject options
//参数:
//返回:
//备注:if we receive the nack,it means we should modify the options we send
//作者:zhangqf@上午11:37:16/2017年1月7日
//-----------------------------------------------------------------------------
static bool_t __lcpDealCREJ(tagPpposCtrl *pppos,u8 id,u8 *buf, u16 len)
{
	u8                 *src;
	u8                 *stop;
	tagPppOptions      *opt;
	tagLcpHostStat  *host;

	src = buf;
	stop = buf + len;
	host = &pppos->lcp.host;
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
	__lcpSendRequest(pppos);
	return true;
}
//-----------------------------------------------------------------------------
//功能:we use this function to deal the echo request
//参数:
//返回:
//备注:just send the magic number we have received
//作者:zhangqf@上午11:37:16/2017年1月7日
//-----------------------------------------------------------------------------
static bool_t __lcpDealEchoReq(tagPpposCtrl *pppos,u8 id,u8 *buf, u16 len)
{
	tagCHdr  cdr;

	cdr.code = ECHOREP;
	cdr.id = id;
	cdr.len = 6;
	__ppposSend(pppos,PPP_LCP,&cdr,buf,len,pppos->lcp.peer.accm);
	return true;
}
//-----------------------------------------------------------------------------
//功能:we use this function to deal the echo reply
//参数:
//返回:
//备注:just change the echo request timeout
//作者:zhangqf@上午11:37:16/2017年1月7日
//-----------------------------------------------------------------------------
static bool_t __lcpDealEchoRep(tagPpposCtrl *pppos,u8 id,u8 *buf, u16 len)
{
	pppos->lcp.host.id++;
	return true;
}
//usage:we use this function to deal a lcp msg
static bool_t __ppposLcpDeal(tagPpposMsg   *msg )
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
	printf("LCP:code:%02x id:%02x len:%04x\n\r",hdr.code,hdr.id,hdr.len);
	//now decode the lcp options here
	switch(hdr.code)
	{
		case CONFREQ:
			__lcpDealCRQ(msg->pppos,hdr.id,src,len);
			break;
		case CONFACK:
			__lcpDealCACK(msg->pppos,hdr.id,src,len);
			break;
		case CONFNAK:
			__lcpDealCNACK(msg->pppos,hdr.id,src,len);
			break;
		case CONFREJ:
			__lcpDealCREJ(msg->pppos,hdr.id,src,len);
			break;
		case TERMREQ:
			//we should send the term ack and turn to the down state
			hdr.code = TERMACK;
			__ppposSend(msg->pppos,PPP_LCP,&hdr,src,len,0xFFFFFFFF);
			//goto dead
			msg->pppos->stat = EN_PPP_DEAD;
			break;
		case TERMACK:

			break;
		case CODEREJ:
			break;
		case PROTOREJ:
			break;
		case ECHOREQ:
			__lcpDealEchoReq(msg->pppos,hdr.id,src,len);
			break;
		case ECHOREP:
			__lcpDealEchoRep(msg->pppos,hdr.id,src,len);
			break;
		case DISREQ:
			break;
		default://unknow the code, we should send the reject code here
			hdr.code = CODEREJ;
			hdr.len = htons(msg->len);
			__ppposSend(msg->pppos,PPP_LCP,&hdr,msg->data,msg->len,0xffffff);
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
bool_t __authSendRequest(tagPpposCtrl *pppos)
{
	u8       len;
	tagCHdr  cdr;
	tagAuth  *auth;
	u8       *buf;
	u8       *dst;

	if(pppos->lcp.peer.authcode == PPP_PAP) //USE USERNAME AND PASSWD
	{
		auth = &pppos->auth;
		len = auth->userlen + auth->passlen + 2;
		buf = malloc(len);
		if(NULL != buf)
		{
			dst = buf;
			*dst++ = auth->userlen;
			memcpy(dst,auth->user,auth->userlen);
			dst += auth->userlen;

			*dst++ = auth->passlen;
			memcpy(dst,auth->passwd,auth->passlen);
			dst += auth->passlen;

			cdr.code = EN_PAP_REQ;
			cdr.id = auth->id;
			cdr.len = htons(len + CN_PPPOS_CHDRSIZE);
			__ppposSend(pppos,PPP_PAP,&cdr,buf,len,pppos->lcp.peer.accm);
			auth->timeout = DjyGetSysTime()+CN_PPPOS_TIMEOUT;
			//free the buf
			free(buf);
		}
	}

	return true;
}

//usage:we use this function to deal a auth msg
static bool_t __ppposAuthDeal(tagPpposMsg   *msg)
{
	tagCHdr      hdr;
	u8          *src;
	u16          i,len,proto;
	tagPpposCtrl *pppos;

	pppos = msg->pppos;
	src = msg->data;
	proto = msg->proto;
	len = msg->len;
	//now do the decode now
	memcpy(&hdr,src,CN_PPPOS_CHDRSIZE);
	hdr.len = htons(hdr.len);
	src += CN_PPPOS_CHDRSIZE;
	len -= CN_PPPOS_CHDRSIZE;
	printf("AUTH:code:%02x id:%02x len:%04x\n\r",hdr.code,hdr.id,hdr.len);
	if((proto == PPP_PAP)&&(pppos->lcp.host.authcode == PPP_PAP))
	{
		//we support the pap mode
		if(hdr.code == EN_PAP_ACK)
		{
			//we has passed ,goto the ncp
			pppos->stat = EN_PPP_NCP;
			printf("AUTH:SUCCESS!,GOTO NCP\n\r");
		}
		else if(hdr.code == EN_PAP_NACK)
		{
			pppos->stat = EN_PPP_DEAD; //FAILED
			//print the fail reason
			for(i = 0;i<len;i++)
			{
				printf("%c",src[i]);
			}
			printf("\n\r");
			printf("AUTH:FAILED!,GOTO DEAD\n\r");
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
static bool_t __ncpSendRequest(tagPpposCtrl *pppos)
{

	u8                 *dst;
	tagPppOptions       opt;
	u32                 v32;
	tagCHdr             cdr;
	tagNcp             *ncp;

	ncp = &pppos->ncp;
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
	__ppposSend(pppos,PPP_IPCP,&cdr,ncp->buf,ncp->datalen,pppos->lcp.peer.accm);
	ncp->timeout = DjyGetSysTime()+CN_PPPOS_TIMEOUT;
	return true;
}
static bool_t __ncpOptionModify(tagNcp *ncp,u8 *src,u16 len,bool_t rej)
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
				printf("IPV4:");
				if(rej)
				{
					ncp->enIp = 0;
					printf("REJECT\n\r");
				}
				else
				{
					memcpy(&v32,opt->v,sizeof(v32));
					ncp->ip = v32;
					ncp->enIp = 1;
					printf("%08x(ADVICE)\n\r",v32);
				}
				break;
			case NCP_OPT_DNS:
				printf("DNS:");
				if(rej)
				{
					ncp->enDns = 0;
					printf("REJECT\n\r");
				}
				else
				{
					memcpy(&v32,opt->v,sizeof(v32));
					ncp->dns = v32;
					ncp->enDns =1;
					printf("%08x(ADVICE)\n\r",v32);
				}
				break;
			case NCP_OPT_DNS2:
				printf("DNSBAK:");
				if(rej)
				{
					ncp->enDnsBak = 0;
					printf("REJECT\n\r");
				}
				else
				{
					memcpy(&v32,opt->v,sizeof(v32));
					ncp->dnsbak = v32;
					ncp->enDnsBak = 1;
					printf("%08x(ADVICE)\n\r",v32);
				}
				break;
			case NCP_OPT_NBNS:
				printf("NBNS:");
				if(rej)
				{
					ncp->enNbns = 0;
					printf("REJECT\n\r");
				}
				else
				{
					memcpy(&v32,opt->v,sizeof(v32));
					ncp->nbns = v32;
					ncp->enNbns = 1;
					printf("%08x(ADVICE)\n\r",v32);
				}
				break;
			case NCP_OPT_NBNS2:
				printf("NBNSBAK:");
				if(rej)
				{
					ncp->enNbnsBak = 0;
					printf("REJECT\n\r");
				}
				else
				{
					memcpy(&v32,opt->v,sizeof(v32));
					ncp->nbnsbak = v32;
					ncp->enNbnsBak=1;
					printf("%08x(ADVICE)\n\r",v32);
				}
				break;
			default:
				break;
		}
	}
	ncp->id++;
	return true;
}

static bool_t __ppposNcpDeal(tagPpposMsg   *msg)
{
	tagCHdr      hdr;
	u8          *src;
	u16          len;
	tagPpposCtrl *pppos;
	tagNcp       *ncp;

	pppos = msg->pppos;
	src = msg->data;
	//now do the decode now
	memcpy(&hdr,src,CN_PPPOS_CHDRSIZE);
	hdr.len = htons(hdr.len);
	src += CN_PPPOS_CHDRSIZE;
	len = msg->len -  CN_PPPOS_CHDRSIZE;
	printf("ipcp:code:%02x id:%02x len:%04x\n\r",hdr.code,hdr.id,hdr.len);
	//here we should check if it is a ack nack rej or something else
    ncp = &pppos->ncp;
	switch(hdr.code)
    {
		case CONFACK:
			//turn to another state
			pppos->stat = EN_PPP_NETWORK;
			//maybe we should add the rout to the net stack --TODO
			printf("NCP:SUCCESS!,GOTO NETWORK\n\r");
			//here we should create a net device and build a rout here
			//modify the rout
			RoutSetDefaultAddr(EN_IPV_4,ncp->ip,0xFFFFFFFF,ncp->ip,ncp->dns);
			break;
    	case CONFREQ:
    		if(len != 0)
    		{
    			//reject all
    			hdr.len = htons(hdr.len);
    			hdr.code = CONFREJ;
    			__ppposSend(pppos,PPP_IPCP,&hdr,src,len,pppos->lcp.peer.accm);
    		}
    		else
    		{
    			//ack it
    			hdr.code = CONFACK;
    			hdr.len = htons(hdr.len);
    			__ppposSend(pppos,PPP_IPCP,&hdr,NULL,0,pppos->lcp.peer.accm);
    		}
    		break;
    	case CONFNAK:
    		//modify the request and resend the request
    		__ncpOptionModify(ncp,src,len,false);
    		__ncpSendRequest(pppos);
    		break;
    	case CONFREJ:
    		//modify the request and resend the request
    		__ncpOptionModify(ncp,src,len,true);
    		__ncpSendRequest(pppos);
    		break;
    	default:
    		//send the code reject
			hdr.code = CODEREJ;
			hdr.len = htons(hdr.len);
			__ppposSend(pppos,PPP_IPCP,&hdr,src,len,pppos->lcp.peer.accm);
    		break;
    }
	return true;
}
//-----------------------------------------------------------------------------
//功能:this the pppos main task thread
//参数:
//返回:
//备注:
//作者:zhangqf@上午10:50:16/2017年1月5日
//-----------------------------------------------------------------------------
ptu32_t ppposDealEngine(void)
{
	tagPpposMsg   *msg = NULL;
	tagPpposCtrl  *pppos;
	tagCHdr        hdr;
	u64            timenow;

	Djy_GetEventPara((ptu32_t *)&pppos,NULL);
	//first we must make the link up,which use the at subsets to make the link up
	while(1)
	{
		msg = PpposGetMsg(CN_PPPOS_MSGTIMEOUT);
		if(NULL != msg)
		{
			printf("\n\r"); //--todo for the debug
			//we receive a message from the peer, we should deal it carefully
			switch(msg->proto)
			{
				case PPP_LCP://this is a lcp frame,so we should deal it with the LCP
					__ppposLcpDeal(msg);
					break;
				case PPP_PAP:
					__ppposAuthDeal(msg);
					break;
				case PPP_CHAP:
					__ppposAuthDeal(msg);
					break;
				case PPP_IPCP:
					__ppposNcpDeal(msg);
					break;
				case PPP_IP:
					__ppposIpDeal(msg);
					break;
				default:
					//maybe a unknown proto, we should send a protorej message to the peer
					printf("Unknown:proto:%04x\n\r",msg->proto);
					hdr.code = PROTOREJ;
					hdr.id = pppos->lcp.host.id;
					hdr.len = htons(msg->len);
					__ppposSend(msg->pppos,PPP_LCP,&hdr,msg->data,msg->len,0xffffff);
					break;
			}
			free(msg);
		}
		else
		{
			//maybe timeout, we should check if any pppos to do the timeout resend
			timenow = (u64)DjyGetSysTime();
			switch(pppos->stat)
			{
				case EN_PPP_DEAD:
					//use the at sets to do the link up
					if(NULL != pppos->fnDial)
					{
						if(pppos->fnDial(pppos->sfd,0))
						{
							pppos->stat = EN_PPP_ESTABLISH;
							printf("%s:DIAL SUCCESS,GOTO LCP\n\r",__FUNCTION__);
						}
						else
						{
							//do the exit--dial failed
							printf("%s:dial failed\n\r",__FUNCTION__);
						}
					}
					break;
				case EN_PPP_ESTABLISH:
					//we should check if the config request timeout
					if(timenow > pppos->lcp.host.timeout)
					{
						__lcpSendRequest(pppos);
					}
					break;
				case EN_PPP_AUTH:
					//we should check if the auth timeout
					if(timenow > pppos->auth.timeout)
					{
						__authSendRequest(pppos);
					}
					break;
				case EN_PPP_NCP:
					//we should check if the ncp timeout
					if(timenow > pppos->ncp.timeout)
					{
						__ncpSendRequest(pppos);
					}
					break;
				case EN_PPP_NETWORK:
					//here we should check if any echo request timeout
					if(timenow > pppos->lcp.host.timeout)
					{
						__lcpSendEchoReq(pppos);
					}
					break;
				case EN_PPP_TERMINATE:
					//do the reset and shut down
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
static void ppposRcvReset(tagPpposRxCtrl *rxctrl)
{
	rxctrl->fcs = PPP_INITFCS;
	rxctrl->stat = EN_PPPOS_RX_DROPS;
	rxctrl->framlen = 0;
	rxctrl->datalen = 0;
	rxctrl->inescape = 0;
	rxctrl->protocol = 0;
}

static u16 ppposProtoInput(tagPpposCtrl *pppos,u8 *s,u16 l )
{
	u8              curchar;
	tagPpposRxCtrl *rxctrl;
	u8             *str;
	u16             lendone;
	tagCHdr         hdr;
	str = s;
	rxctrl = &pppos->rxctrl;
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
			ppposRcvReset(rxctrl);//reset the receive state
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
					PpposPutMsg(pppos);
				}
				else
				{
					//drop the frame please
				}
				ppposRcvReset(rxctrl);//reset the receive state
				break;
			}
			else if(rxctrl->datalen >= rxctrl->rxbuflen)
			{
				//maybe overlap,then drop this
				ppposRcvReset(rxctrl);//reset the receive state
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
static void ppposInput(tagPpposCtrl *pppos,u8 *s,u16 l)
{
	u8              curchar;
	tagPpposRxCtrl *rxctrl;
	u16             lendone;

	rxctrl = &pppos->rxctrl;
	while (l > 0)
	{
		if(rxctrl->stat == EN_PPPOS_RX_CPROTO)
		{
			//let the proto do the decode
			lendone = ppposProtoInput(pppos,s,l);  //let the proto do the left
			if(lendone != l)
			{
				s += lendone;
				l -= lendone;
				//do another
				ppposRcvReset(rxctrl);
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
							ppposRcvReset(rxctrl);
						}
						break;
					default:
						ppposRcvReset(rxctrl);
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
ptu32_t ppposReceiveEngine(void)
{
#define CN_READ_BUF 128

	u8  siobuf[CN_READ_BUF];
	int len;
	tagPpposCtrl *pppos;
	Djy_GetEventPara((ptu32_t *)&pppos,NULL);
	if(NULL != pppos)
	{
		//do the receive state reset
		ppposRcvReset(&pppos->rxctrl);
		while(1)
		{
			len = sio_read(pppos->sfd,siobuf,CN_READ_BUF,200*mS);
			if(len > 0)
			{
				__ppposRcvHookMessagePut(siobuf,len);
				ppposInput(pppos,siobuf,len);
			}
			else
			{
				//do the receive reset
				ppposRcvReset(&pppos->rxctrl);//reset the receive state
			}
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------
//功能:here we install the shell command in the system for the debug
//参数:
//返回:
//备注:
//作者:zhangqf@下午2:40:30/2017年1月9日
//-----------------------------------------------------------------------------
//usage:use this function to do some  at command
static bool_t  ppposdebugmode(char *param)
{
	int level;
	level = strtol(param,NULL,0);
	DebugModeSet(level);
	return true;
}

//-----------------------------------------------------------------------------
//功能:we use this function to do the debug set or debug info
//参数:
//返回:
//备注:
//作者:zhangqf@下午4:29:34/2017年1月16日
//-----------------------------------------------------------------------------
//usage:show the pppos state
static void ppposState(tagPpposCtrl *pppos)
{
	tagPpposDebug *debug;
	printf("PPPOS:STATE:%s\n\r",gPppStatName[pppos->stat]);
	printf("%4s%-9s%-9s%-9s%-9s%-9s%-9s%-9s%-9s%-9s%-9s\n\r",\
			"Type","Total","OkNum","FcsErrNum","OverNum","LcpNum","PapNum",\
			"ChapNum","IpcpNum","IpNum","Unknown");
	debug = &pppos->rxinfo;
	printf("%4s%08x %08x %08x %08x %08x %08x %08x %08x %08x %08x\n\r",\
			"Rcv:",debug->numtotal,debug->numok,debug->numfcserr,debug->numoverlap,\
			debug->numlcp,debug->numpap,debug->numchap,debug->numipcp,debug->numip,debug->numunknown);
	debug = &pppos->txinfo;
	printf("%4s%08x %08x %08x %08x %08x %08x %08x %08x %08x %08x\n\r",\
			"Snd:",debug->numtotal,debug->numok,debug->numfcserr,debug->numoverlap,\
			debug->numlcp,debug->numpap,debug->numchap,debug->numipcp,debug->numip,debug->numunknown);
}
//usage:use this function do the shell interface
static bool_t ppposStatShell(char *param)
{
	int argc = 4;
	const char *argv[4];
	string2arg(&argc,argv,param);
	if(argc > 0)
	{

	}
	else
	{
		//show the pppos mode here
		ppposState(&gPppos);
	}
	return true;
}
struct ShellCmdTab  gPppDebug[] =
{
    {
        "iocmd",
		ppposAtCommandShell,
        "usage:iocmd+ATCMD",
        "usage:iocmd+ATCMD",
    },
    {
        "debugmode",
		ppposdebugmode,
        "usage:debugmode",
        "usage:debugmode",
    },
    {
        "pppos",
		ppposStatShell,
        "usage:pppos",
        "usage:pppos",
    },
    {
        "dialup",
		dialup,
        "usage:dialup",
        "usage:dialup",
    },
    {
        "dialend",
		dialend,
        "usage:dialend",
        "usage:dialend",
    },
    {
        "sndflag",
		sndflag,
        "usage:sndflag",
        "usage:sndflag",
    },

};
#define CN_ARPDEBUG_NUM  ((sizeof(gPppDebug))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc gPppDebugCmdRsc[CN_ARPDEBUG_NUM];
//-----------------------------------------------------------------------------
//功能:this is the pppos main function here
//参数:
//返回:
//备注:
//作者:zhangqf@下午4:06:57/2017年1月5日
//-----------------------------------------------------------------------------
bool_t ModuleInstall_Pppos(const char *name)
{
	//initialize the message queue
	memset(&gPpposMsgQ,0,sizeof(gPpposMsgQ));
	gPpposMsgQ.mutex = mutex_init(NULL);
	if(NULL == gPpposMsgQ.mutex)
	{
		goto EXIT_MSGBOX_MUTEX;
	}
	gPpposMsgQ.sync = semp_init();
	if(NULL == gPpposMsgQ.sync)
	{
		goto EXIT_MSGBOX_SYNC;
	}
	//now initialize the pppos ctrol block
	memset(&gPppos,0,sizeof(gPppos));
	gPppos.rxctrl.rxbuflen = CN_PPPOS_RXTXLEN;   //receive buffer
	gPppos.txctrl.buflen = CN_PPPOS_RXTXLEN;     //send buffer
	gPppos.txctrl.mutex = mutex_init(NULL);
	if(NULL == gPppos.txctrl.mutex)
	{
		goto EXIT_TXBUF_MUTEX;
	}

	gPppos.stat = EN_PPP_DEAD;
	//initialize the dial up
	gPppos.fnDial = pppDialUp;
    //initialize the lcp options
	gPppos.lcp.host.mru = PPP_MTU;
	gPppos.lcp.host.enmru = 0;
	gPppos.lcp.host.accm = 0;
	gPppos.lcp.host.enaccm = 1;
	gPppos.lcp.host.authcode = PPP_PAP;
	gPppos.lcp.host.authencode = 0;
	gPppos.lcp.host.enauth = 0;
	gPppos.lcp.host.macgic = (u32)DjyGetSysTime();
	gPppos.lcp.host.enmagic = 0;
	gPppos.lcp.host.buflen = CN_OPTION_BUFLEN;
	gPppos.lcp.host.id = 1;
	//initialize the auth
	gPppos.auth.id = 0x10;
	//initialize the ncp
	gPppos.ncp.enIp = 0;
	gPppos.ncp.enDns = 1;
	gPppos.ncp.enDnsBak = 1;
	gPppos.ncp.enNbns = 1;
	gPppos.ncp.enNbnsBak = 1;

	gPppos.ncp.id = 0x20;

	//now open the serail device
	gPppos.sfd = sio_open(name);
	if(0  == gPppos.sfd)
	{
		goto EXIT_SIO_OPEN;
	}
	//create the two main task
	taskcreate("ppposdealer",0x800,19,ppposDealEngine,(ptu32_t)&gPppos);
	taskcreate("ppposreceive",0x800,20,ppposReceiveEngine,(ptu32_t)&gPppos);
	gPppos.nfd=PppDevAdd((ptu32_t)&gPppos);
	Sh_InstallCmd(gPppDebug,gPppDebugCmdRsc,CN_ARPDEBUG_NUM);

	//debug.for init the rcv hook
	gRcvHook.mutex = mutex_init(NULL);
	gRcvHook.semp = semp_init();
	gRcvHook.buf = NULL;
	gRcvHook.buflen = 0;
	gRcvHook.datalen = 0;

	return true;

EXIT_SIO_OPEN:
	mutex_del(gPppos.txctrl.mutex);
	gPppos.txctrl.mutex = NULL;
EXIT_TXBUF_MUTEX:
	semp_del(gPpposMsgQ.sync);
	gPpposMsgQ.sync = NULL;
EXIT_MSGBOX_SYNC:
	mutex_del(gPpposMsgQ.mutex);
	gPpposMsgQ.mutex = NULL;
EXIT_MSGBOX_MUTEX:
	return false;
}

