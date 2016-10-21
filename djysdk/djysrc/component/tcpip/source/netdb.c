/*
 * netdb.c
 *
 *  Created on: 2015Äê9ÔÂ6ÈÕ
 *      Author: zqf
 */

#include <sys/socket.h>


static struct servent  gServEnt[]={
        {"sntp",NULL,htons(123),"udp"},
        {"ftp",NULL,htons(21),"tcp"},
        {"telnet",NULL,htons(23),"tcp"},
        {"smtp",NULL,htons(25),"tcp"},
        {"dns",NULL,htons(53),"udp"},
        {"dns",NULL,htons(53),"tcp"},
        {"dhcps",NULL,htons(67),"udp"},
        {"dhcpc",NULL,htons(68),"udp"},
        {"tftp",NULL,htons(69),"udp"},
};

#define CN_SERVENT_NUM  (sizeof(gServEnt)/sizeof(struct servent))
struct servent *getservbyname(const char *name, const char *proto)
{
    int i = 0;
    struct servent *result = NULL;

    for(i= 0; i <CN_SERVENT_NUM;i++)
    {
        if(0==strcmp(name,gServEnt[i].s_name))
        {
            if(!((NULL != proto)&&(0==strcmp(proto,gServEnt[i].s_proto))))
            {
                result = &gServEnt[i];
                break;
            }
        }
    }
    return result;
}

u32 inet_addr(const char *addr)
{
    int para;
    int tmp[4];
    u32 result = INADDR_NONE;
    u8  ip[4];


    para = sscanf(addr,"%d.%d.%d.%d",&tmp[0],&tmp[1],&tmp[2],&tmp[3]);
    if(para == 4)
    {
        ip[0] = (u8)tmp[0];
        ip[1] = (u8)tmp[1];
        ip[2] = (u8)tmp[2];
        ip[3] = (u8)tmp[3];
        memcpy((void *)&result,(void *)ip,sizeof(result));
    }

    return result;
}



int inet_aton(const char *string,struct in_addr *addr)
{
    int para;
    int tmp[4];
    u8  ip[4];
    int result = 0;

    para = sscanf(string,"%d.%d.%d.%d",&tmp[0],&tmp[1],&tmp[2],&tmp[3]);
    if(para == 4)
    {
        ip[0] = (u8)tmp[0];
        ip[1] = (u8)tmp[1];
        ip[2] = (u8)tmp[2];
        ip[3] = (u8)tmp[3];
        memcpy((void *)addr,(void *)ip,sizeof(ip));
        result = 1;
    }

    return result;
}


#define CN_INET_ADDRLEN     16
static  char gInetAddr[CN_INET_ADDRLEN];

char *inet_ntoa(struct in_addr addr)
{
    u8 ip[4];
    int tmp[4];
    memcpy((void *)ip,(void *)&addr,sizeof(ip));
    memset((void *)gInetAddr,0,CN_INET_ADDRLEN);

    tmp[0] = (int)ip[0];
    tmp[1] = (int)ip[1];
    tmp[2] = (int)ip[2];
    tmp[3] = (int)ip[3];
    sprintf(gInetAddr,"%d.%d.%d.%d",tmp[0],tmp[1],tmp[2],tmp[3]);

    return (char *)gInetAddr;
}
const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt)
{
    char *str;
    char          *result = NULL;
    struct in_addr addr;

    if((NULL == src)||(NULL == dst))
    {
        return result;
    }

    if(af==AF_INET)
    {

        memcpy((void *)&addr,src,sizeof(addr));
        str = inet_ntoa(addr);

		if((NULL != str)&&(cnt > (strlen(str)+1)))
		{
			memcpy((void*)dst,(void*)str,strlen(str)+1);
			result = dst;
		}
	}

    return result;
}
int inet_pton(int af, const char *src, void *dst)
{

    int result = -1;
//    unsigned char *str;
    struct in_addr addr;

    if((NULL == src)||(NULL == dst))
    {
        result = 0;
        return result;
    }

    if(af==AF_INET)
    {
        if(inet_aton(src,&addr))
        {
            memcpy((void *)dst,&addr,sizeof(addr));
            result = 1;
        }
    }

    return result;
}

extern struct hostent*  DnsNameResolve(const char *name);
struct hostent *gethostbyname(const char *name)
{
    struct hostent *result;

    result = DnsNameResolve(name);

    return result;
}

static char gHostName[] = "DJYOS_TERNIMAL";
int  gethostname(char *name, int len)
{
	strcpy(name,gHostName);
	return 0;
}







