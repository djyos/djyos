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

enum __DNS_OPCODE
{
	DNS_OPCODE_STANDQUERY = 0,
	DNS_OPCODE_REVERSEQUERY,
	DNS_OPCODE_SERVERSTAT,
	DNS_OPCODE_RESERVE1,
	DNS_OPCODE_INSTRUCTION,
	DNS_OPCODE_DIAGRAMUPDATE,
};

enum __DNS_RCODE
{
	DNS_RCODE_NOERR = 0,
	DNS_RCODE_SERIOUSEFAIL,
	DNS_RCODE_NAMEERR,
	DNS_RCODE_NOIMPLEMENT,
	DNS_RCODE_REFUSE,
	DND_RCODE_RESERVE,
};

/* http://en.wikipedia.org/wiki/List_of_DNS_record_types */
enum dns_query_type {
	DNS_A_RECORD     = 0x01,
	DNS_CNAME_RECORD = 0x05,
	DNS_MX_RECORD    = 0x0f,
};

enum QueryClass //指定信息的协议组。
{
	IN            =0x01,    //指定 Internet 类别。
	CSNET         =0x02,    //指定 CSNET 类别。（已过时）
	CHAOS         =0x03,    //指定 Chaos 类别。
	HESIOD        =0x04,    //指定 MIT Athena Hesiod 类别。
	ANY           =0xFF     //指定任何以前列出的通配符。
};

#pragma pack(1)
typedef struct
{
	u16	            tid;		/* Transaction ID */
	u16	            flags;		/* Flags */
	u16	            nqueries;	/* Questions */
	u16	            nanswers;	/* Answers */
	u16	            nauth;		/* Authority PRs */
	u16	            nother;		/* Other PRs */
	u8          	data[0];	/* Data, variable length */
}tagDnsHeader;
#pragma pack()

#define CN_RESULT_NUM     10
static unsigned char      gDnsCNameOffset;
static unsigned char     *gDnsCNameAddr[CN_RESULT_NUM+1];
static unsigned char      gDnsINameV4Offset;
static unsigned char     *gDnsINameAddrV4[CN_RESULT_NUM+1];

typedef struct
{
	u16 type;
	u16 class;
}tagDnsQType;
typedef struct
{
	u16  type;
	u16  class;
	u32  ttl;
}tagDnsAType;


static void __DnsClearResult()
{
	memset((void *)gDnsCNameAddr,0,sizeof(gDnsCNameAddr));
	memset((void *)gDnsINameAddrV4,0,sizeof(gDnsINameAddrV4));
	gDnsCNameOffset = 0;
	gDnsINameV4Offset = 0;
}

#define CN_DNS_ERR_LENTH  31
//decode the dns answers
enum __DNS_DECODE_STAT
{
	EN_DECODE_STAT_HEADER = 0,
	EN_DECODE_STAT_QUESTION,
	EN_DECODE_STAT_ANSWER,
	EN_DECODE_STAT_SUCCESS,
	EN_DECODE_STAT_ERR,
	EN_DECODE_STAT_DONE,
};



static  void __decodeName(unsigned  char *name)
{
	unsigned char *s;
	unsigned char num;

	s = name;
	while(*s != '\0')
	{
		num = *s+1;
		*s = '.';
		s += num;
	}

	return;
}

static void __encodeName(unsigned char *name)
{
	char *s,*tmp;
    char *pos;
	char num;

	s = (char *)name;
	pos =(char *) name-1;
	while(*s != '\0')
	{
		tmp = strchr(s,'.');
		if(tmp == NULL)
		{
			num = strlen(s);
			*pos = num;
			s += num;
			pos = s;
		}
		else //got the '.'
		{
			num = tmp -s;
			*pos = num;
			s = tmp+1;
			pos = tmp;
		}
	}

	return;
}

static void __DnsPackageData(const char *resolvename,unsigned char *buf, int *len)
{
	tagDnsHeader *header;
	int name_len, datalen;
	unsigned char  *p, *pkt;
	u16 qtype = htons(DNS_A_RECORD);
	u16 qclass = htons(IN);
	pkt = p = (unsigned char *)buf;

	/* Prepare DNS packet header */
	header           = (tagDnsHeader *) pkt;
	header->tid      = htons(1);
	header->flags    = htons(0x100);	/* standard query */
	header->nqueries = htons(1);		/* Just one query */
	header->nanswers = htons(0);
	header->nauth    = htons(0);
	header->nother   = htons(0);

	/* Encode DNS name */
	p = (unsigned char *) &header->data;	/* For encoding host name into packet */

	p++;
	name_len = strlen(resolvename) +1;
	memcpy((void *)p, (void *)resolvename,name_len);
	__encodeName(p);
	p += name_len;

    memcpy((void *)p,(void *)&qtype,sizeof(qtype));
    p+=sizeof(qtype);

    memcpy((void *)p,(void *)&qclass,sizeof(qclass));
    p+=sizeof(qclass);

    datalen = (int)(p-pkt);
    *len = datalen;

	return;
}


static void  __DnsUnpackageData(unsigned char *data, unsigned int datalen)
{
	u32 stat;
	int len;
	unsigned char *p;
	unsigned short rsclen;
	tagDnsHeader  *header;
	tagDnsQType   qtype;
	tagDnsAType   atype;

	stat= EN_DECODE_STAT_HEADER;
	p = data;

	while((stat != EN_DECODE_STAT_DONE)&&(p < (data +datalen)))
	{
		switch(stat)
		{
			case EN_DECODE_STAT_HEADER:
				header = (tagDnsHeader  *)p;
				p += sizeof(tagDnsHeader);
				header->tid = ntohs(header->tid);
				header->flags = ntohs(header->flags);
				header->nqueries = ntohs(header->nqueries);
				header->nanswers = ntohs(header->nanswers);
				header->nauth = ntohs(header->nauth);
				header->nother = ntohs(header->nother);

				if((header->nanswers <1)||(header->nqueries >1))
				{
					stat = EN_DECODE_STAT_DONE;
				}
				else
				{
					stat = EN_DECODE_STAT_QUESTION;
				}
				break;
			case EN_DECODE_STAT_QUESTION:
				len = strlen((const char *)p) +1;
				gDnsCNameAddr[gDnsCNameOffset] = p+1;
				gDnsCNameOffset++;
				__decodeName(p);
				p+=len;

				memcpy((void *)&qtype,p,sizeof(qtype));
				qtype.type = ntohs(qtype.type);
				qtype.class = ntohs(qtype.class);
				p += sizeof(qtype);

				stat = EN_DECODE_STAT_ANSWER;
				break;
			case EN_DECODE_STAT_ANSWER:
				if(*p == 0xc0)
				{
					//this is the offset name
					printf("%s:answer domain name:offset:%d\n\r",__FUNCTION__,p[1]);
					p += 2;
				}
				else
				{
					len = strlen((const char *)p) +1;
					gDnsCNameAddr[gDnsCNameOffset] = p+1;
					gDnsCNameOffset++;
					__decodeName(p);
					p+=len;
				}

				memcpy((void *)&atype,p,sizeof(atype));
				atype.type = ntohs(atype.type);
				atype.class = ntohs(atype.class);
				atype.ttl = ntohl(atype.ttl);
				p += sizeof(atype);

				memcpy((void *)&rsclen,p,sizeof(rsclen));
				p += sizeof(rsclen);
				rsclen = ntohs(rsclen);

				if((atype.type == DNS_A_RECORD)&&(atype.class == IN))
				{
					//this is the ipaddress
					gDnsINameAddrV4[gDnsINameV4Offset] = p;
					gDnsINameV4Offset++;
				}
				else if((atype.type == DNS_CNAME_RECORD)&&(atype.class == IN))
				{
					if(*p != 0xc0)
					{
						gDnsCNameAddr[gDnsCNameOffset] = p+1;
						gDnsCNameOffset++;
						__decodeName(p);
					}
				}
				else
				{

				}
				p += rsclen;
				break;
			default:
				break;
		}
	}
	gDnsCNameAddr[gDnsCNameOffset] = NULL;
	gDnsINameAddrV4[gDnsINameV4Offset] = NULL;

	return ;
}
#define CN_DNS_SERVER_PORT  53
#define CN_DNS_MSGBUF_LEN   0x100
static struct hostent       gHostEnt;
static unsigned char        gDnsBuf[CN_DNS_MSGBUF_LEN];
struct hostent  *DnsNameResolve(const char *name)
{
	struct hostent *result = NULL;
	struct sockaddr_in addr;
	int sock;
	u32 dnsip;
	int datalen;
	int msglen;
	int addrlen;
	int opt =1;

	sock = socket(AF_INET, SOCK_DGRAM,0);
	if(sock == -1)
	{
		goto EXIT_DNSMAIN;
	}

	if(false == RoutDns(EN_IPV_4,(ipaddr_t)&dnsip))
	{
		goto EXIT_DNSMAIN;
	}

	opt = 5*1000*mS;  //time out time
	if(0 != setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,&opt,sizeof(opt)))
	{
		goto EXIT_DNSMAIN;
	}

	memset((void *)&addr,0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(CN_DNS_SERVER_PORT);
	addr.sin_addr.s_addr = dnsip;

	__DnsPackageData(name,gDnsBuf,&datalen);
	__DnsClearResult();

	msglen = sendto(sock,gDnsBuf,datalen,0,(struct sockaddr*)&addr,sizeof(addr));

	if(msglen != datalen)
	{
		goto EXIT_DNSMAIN;
	}
	msglen = recvfrom(sock,gDnsBuf,CN_DNS_MSGBUF_LEN,0,(struct sockaddr *)&addr,&addrlen);

	if((msglen > 0)&&(addr.sin_addr.s_addr == dnsip))
	{
		//this is the msg we need
		__DnsUnpackageData(gDnsBuf,msglen);
		gHostEnt.h_name = (char *)name;
		gHostEnt.h_addrtype = AF_INET;
		gHostEnt.h_length = 4;
		gHostEnt.h_aliases = (char **)gDnsCNameAddr;
		gHostEnt.h_addr_list = (char **)gDnsINameAddrV4;

		result = &gHostEnt;
	}
	else
	{
		goto EXIT_DNSMAIN;
	}

EXIT_DNSMAIN:
	closesocket(sock);
	return result;
}


bool_t DnsNameResolveShell(char *param)
{
	bool_t result = true;
	char *name;
	char **alias;
	char **addr;
	struct hostent *host;
	char   ipstr[20];

	name = param;
	printk("%s:Resolve Name:%s\n\r",__FUNCTION__,name);
	host = DnsNameResolve(name);

	if(NULL != host)
	{
		printk("%s:GetName:%s addrtype:%d lenth:%d\n\r",\
				__FUNCTION__,host->h_name,host->h_addrtype,host->h_length);

		alias = host->h_aliases;
		while(NULL != *alias)
		{
			printk("%s:alias:%s\n\r",__FUNCTION__,*alias);
			alias++;
		}
		addr = host->h_addr_list;
		while(NULL != *addr)
		{
			if(inet_ntop(AF_INET,*addr,ipstr,20)>0)
			{
				printk("%s:ip:%s\n\r",__FUNCTION__,ipstr);
			}
			addr++;
		}
	}
	else
	{
		printk("%s:Resolve Name:%s--FAILED\n\r",__FUNCTION__,name);
	}

	return result;
}



struct ShellCmdTab  gServiceDns[] =
{
    {
        "gethostbyname",
        DnsNameResolveShell,
        "usage:gethostbyname hostname",
        "usage:gethostbyname hostname",
    }
};

#define CN_DNSDEBUG_NUM  ((sizeof(gServiceDns))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc gServiceDnsCmdRsc[CN_DNSDEBUG_NUM];

//THIS IS DNS MODULE FUNCTION
bool_t ServiceDnsInit(ptu32_t para)
{
    bool_t result;

    result = Sh_InstallCmd(gServiceDns,gServiceDnsCmdRsc,CN_DNSDEBUG_NUM);

    return result;
}

