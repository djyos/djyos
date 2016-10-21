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
#include <sys/time.h>
#include <unistd.h>
#include "../../tcpipconfig.h"

#include "ftp.h"

#define CN_FTP_SPORT	     21    //THIS IS THE SERVER BIND PORT
#define CN_FTP_ROOT		     "/"   //USE THIS AS THE ROOT ABSOLUTE PATH
#define CN_FTP_LISTENMAX	 1    //COULD LISTEN 1 CLIENT AT THE SAME TIME
#define CN_FTP_WELCOMMSG     "220 DJYOS FTP SERVER READY.\r\n"
#define CN_FTP_NAMELEN       16   //USE TO LIMITE THE NAME AND PASS
#define CN_PATH_LENGTH       128  //USE TO LIMITE THE DIR PATH LEN
#define CN_CLIENT_BUFLEN     512  //USE TO CACHE THE DATA MESSGAGE

typedef struct __FtpClient
{
	struct __FtpClient* next;
	//defines for the client info
	bool_t anonymous;
	char   user[CN_FTP_NAMELEN];
	char   passwd[CN_FTP_NAMELEN];
	//communication channel
	int    cmdsock;              //this is for the control info channel
	bool_t isactive;             //true active mode else passive mode
	u16    dataport;             //data socket port
	u32    dataip;               //data socket ip
	int    pasvsock;             //data socket
	//info for the communication info
	char   curdir[CN_PATH_LENGTH];//store the current directory
	off_t  fileoffset;            //point to the file offset
	u8     buf[CN_CLIENT_BUFLEN]; //buf for the client send and receive
	int    buflen;                //how many data in the buf
}tagFtpClient;
static tagFtpClient* pFtpClient = NULL;

//this function used to display the ftp connection stat
static void __showclient(tagFtpClient *client)
{
	struct sockaddr_in addr;
	socklen_t len;

	printf("FTP DEBUG******************************\n\r");
	printf("Client:user       :%s\r\n",client->user);
	printf("Client:passwd     :%s\r\n",client->passwd);
	printf("Client:curdir     :%s\r\n",client->curdir);
	printf("Client:cmdsock    :%d\r\n",client->cmdsock);
	if(client->cmdsock != -1)
	{
		getsockname(client->cmdsock,(struct sockaddr *)&addr,&len);
		printf("Client:cmdsockipl :%s  port:%d\r\n",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
		getpeername(client->cmdsock,(struct sockaddr *)&addr,&len);
		printf("Client:cmdsockipr :%s  port:%d\r\n",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
	}
	if(client->isactive)
	{
	    printf("Client:port mode:server:%s port:0x%02x\r\n",inet_ntoa(*(struct in_addr *)&client->dataip),client->dataport);
	}
	else
	{
	    if(client->pasvsock != -1)
	    {
		    printf("Client:pasvsock   :%d\r\n",client->pasvsock);
	        getsockname(client->pasvsock,(struct sockaddr *)&addr,&len);
	        printf("Client:datasockipl:%s  port:%d\r\n",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
	        getpeername(client->pasvsock,(struct sockaddr *)&addr,&len);
	        printf("Client:datasockipr:%s  port:%d\r\n",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
	    }
	    printf("Client:buf:0x%08x  datalen:%d\r\n",(u32)client->buf,client->buflen);
	    printf("FTP DEBUG******************************\n\r");
	}
	return;
}
static bool_t ftpstat(char *para)
{
    static tagFtpClient* client = NULL;

    client = pFtpClient;
    while(NULL != client)
    {
        __showclient(client);
        client = client->next;
    }
    return true;
}

//we use this function to get the file size
static int __filesize(char * filename)
{
	struct stat s;
	int len = 0;
	if (stat(filename, &s) == 0)
	{
		len = s.st_size;
	}
	return len;
}

//we use the curdir and the file name to get the absolute path
static int __mkpath(tagFtpClient* client, char* path, char* new_path, size_t size)
{
    int curdirlen;
    if(path[0] == '/')
    {
        //its an absolute path
        snprintf((char *)new_path,size, "%s",path);
    }
    else
    {
        //its an relative path
        curdirlen = strlen(client->curdir);
        if(client->curdir[curdirlen-1] == '/')  //end with '/' then cat it to the tail
        {
            snprintf((char *)new_path,size, "%s%s", client->curdir, path);
        }
        else
        {
            snprintf((char *)new_path,size, "%s/%s", client->curdir, path);
        }
    }

	return 0;
}

//we use this function to send the message to the ftp client
//usually this is the response message
static int __sendresponse(tagFtpClient *client,int responsecode,char *msg)
{
	int result = 0;
	memset(client->buf,0,CN_CLIENT_BUFLEN);
	client->buflen = snprintf((char *)client->buf,CN_CLIENT_BUFLEN,"%d  %s\r\n",responsecode,msg);

	if(client->buflen > 0)
	{
		result = sendexact(client->cmdsock,client->buf,client->buflen);
		memset(client->buf,0,CN_CLIENT_BUFLEN);
		client->buflen = 0;
	}
	else
	{
		result = client->buflen;
	}
	return result;
}

//use this function to match the command, the return value is 0 match others not match
int __cmdmatch(char* src, char* match)
{
	while (*match)
	{
		/* check source */
		if (*src == 0) return -1;

		if (*match != *src) return -1;
		match ++; src ++;
	}
	return 0;
}

//we use this function to check if the user and password is legal login
static bool_t __checkuser(tagFtpClient* client)
{
	return true;
}

//this function used to connect to the port that the client supplied
//this is used in the active mode,when get the server, we set the keeptimer alive
static int __connectport(tagFtpClient *client)
{
	int sock;
	int opt;
	struct sockaddr_in serveraddr;

	sock =  socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == sock)
	{
		printf("%s:socket err\n\r",__FUNCTION__);
		goto ERR_SOCKET;
	}

	serveraddr.sin_addr.s_addr=client->dataip;
	serveraddr.sin_port=client->dataport;
	serveraddr.sin_family=AF_INET;
	if(connect(sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr))==-1)
	{
		printf("%s:connect %s :%d err\n\r",__FUNCTION__,\
				inet_ntoa(*(struct in_addr*)&serveraddr.sin_addr.s_addr),\
				ntohs(serveraddr.sin_port));
		goto ERR_CONNECT;
	}

	opt =1;
	if(0 !=setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,&opt,sizeof(opt))) //no nagle
	{
	    printf("%s:set sock no delay failed\r\n",__FUNCTION__);
	    goto ERR_SETSOCKOPT;
	}
	opt = 1;
	if(0 != setsockopt(sock,SOL_SOCKET,SO_KEEPALIVE,&opt,sizeof(opt))) //enable the keepalive
	{
		printf("%s:set sock keepalive failed\r\n",__FUNCTION__);
	    goto ERR_SETSOCKOPT;
	}
	return sock;

ERR_SETSOCKOPT:
ERR_CONNECT:
	closesocket(sock);
	sock = -1;
ERR_SOCKET:
	return sock;
}

//this function used to deal with the user name
static int __usermsgdeal(tagFtpClient* client,char *user)
{
	int result = 0;
	int code;
	char *msg;

	strncpy(client->user,user,CN_FTP_NAMELEN-1);
	if(strcmp(user, "anonymous") == 0)
	{
		client->anonymous = true;
		code = CN_LOGIN;
		msg = "LOGIN OK";
	}
	else
	{
		client->anonymous = false;
		code = CN_NEEDPASS;
		msg = "Need Passwd";
	}
	__sendresponse(client,code,msg);
	return result;
}
//this function used to deal with the user pass word
static int __passmsgdeal(tagFtpClient* client,char *passwd)
{
	int result = 0;

	strncpy(client->passwd,passwd,CN_FTP_NAMELEN-1);
	if(__checkuser(client))
	{
		__sendresponse(client,CN_LOGIN,"LOGIN OK");
		result = 0;
	}
	else
	{
		// incorrect password
		__sendresponse(client,CN_LOGOUT,"LOGIN FAILED:user or passwd not matched");
		result= -1;
	}
	return result;
}
//this function used to deal with list message
static int __listmsgdeal(tagFtpClient* client,char *details)
{
	DIR* dirp;
	struct dirent* entry;
	struct stat s;
	int sock;
	if(client->isactive)
	{
		__sendresponse(client,CN_OPENCONNECTION,"open ASCII mode data connection for directory list");
		sock = __connectport(client);
		if(sock == -1)
		{
			__sendresponse(client,CN_OPENDATACONNECTIONFAILED,"open the data connection failed");
		}
	}
	else
	{
		sock = client->pasvsock;
		client->pasvsock = -1;
		__sendresponse(client,CN_OPENCONNECTION,"open ASCII mode data connection for directory list");
	}
	if(sock != -1)
	{
		dirp = opendir(client->curdir);
		if (dirp == NULL)
		{
			__sendresponse(client,CN_LOCALERR,"open dir err");
		}
		else
		{
			while (1)
			{
				entry = readdir(dirp);
				if (entry == NULL)
					break;
				if(details)
				{
					__mkpath(client,entry->d_name,(char *)client->buf,CN_CLIENT_BUFLEN);
					if (stat((const char *)client->buf, &s) == 0)
					{
						if (s.st_mode & S_IFDIR)
							client->buflen = snprintf((char *)client->buf,CN_CLIENT_BUFLEN,\
									"drw-r--r-- 1 admin admin %d Jan 1 2000 %s\r\n", 0, entry->d_name);
						else
							client->buflen = snprintf((char *)client->buf,CN_CLIENT_BUFLEN,\
									"-rw-r--r-- 1 admin admin %d Jan 1 2000 %s\r\n",  s.st_size, entry->d_name);
					}
				}
				else
				{
					client->buflen = snprintf((char *)client->buf,CN_CLIENT_BUFLEN,\
							"%s\r\n",entry->d_name);
				}
				if(0>=sendexact(sock,client->buf,client->buflen))
				{
					break;
				}
			}
			closedir(dirp);
		}
		__sendresponse(client,CN_CLOSEDATACONNECTION,"CLOSE DATA CONNECTION");
		closesocket(sock);
	}
	return 0;
}
//this function for the path build
static int __pwdmsgdeal(tagFtpClient *client,char *para)
{
	client->buflen = snprintf((char *)client->buf,CN_CLIENT_BUFLEN,\
			"%d \"%s\" is current directory.\r\n",CN_PATHBUILD,client->curdir);
	sendexact(client->cmdsock,client->buf,client->buflen);
	memset(client->buf,0,CN_CLIENT_BUFLEN);
	client->buflen = 0;
	return 0;
}
//this function for the type message
static int __typemsgdeal(tagFtpClient *client,char *para)
{
	if(strcmp(para, "I")==0)
	{
		__sendresponse(client,CN_CMDSUCCESS,"TYPE BINARY SELECTED");
	}
	else
	{
		__sendresponse(client,CN_CMDSUCCESS,"TYPE A SELECTED");
	}
	return 0;
}
//this function is for the port message
static int __portmsgdeal(tagFtpClient *client,char *para)
{
    int i;
    int portcom[6];
    char tmpip[100];
    i=0;
    portcom[i++]=atoi(strtok(para, ".,;()"));
    for(;i<6;i++)
        portcom[i]=atoi(strtok(0, ".,;()"));
    sprintf(tmpip, "%d.%d.%d.%d", portcom[0], portcom[1], portcom[2], portcom[3]);
    client->dataip = inet_addr(tmpip);
    client->dataport = htons(portcom[4] * 256 + portcom[5]);
    client->isactive = true;
    __sendresponse(client,CN_CMDSUCCESS,"port command success");

    return 0;
}

//this function for the type message
static u32   gPsvPort = 1024;
static int __psvmsgdeal(tagFtpClient *client,char *para)
{
	int  addr_len = sizeof(struct sockaddr_in);
	struct sockaddr_in local, pasvremote;
	u16 port;
	int dig1, dig2;
	int sockserver;
	int sockclient;
	int len;
	int  opt;
	int  optlen;
	unsigned char ipByte[4];

	port = gPsvPort++;
	len = sizeof(local);
	getsockname(client->cmdsock,(struct sockaddr *)&local,&len);
	local.sin_port = htons(port);

	dig1 = (int)(port>>8);
	dig2 = (int)(port&0xff);

	optlen = sizeof(opt);
	if((sockserver=socket(AF_INET, SOCK_STREAM, 0))==-1)
	{
		printf("%s:socket err\n\r",__FUNCTION__);
		return 0;
	}

	opt = 1;
	if(setsockopt(sockserver, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))==-1)
	{
		printf("%s:setsockopt server err\n\r",__FUNCTION__);
		//close the server
		closesocket(sockserver);
		return 0;
	}

	opt = 3*1000*mS;
	if(setsockopt(sockserver, SOL_SOCKET, SO_RCVTIMEO, &opt, sizeof(opt))==-1)
	{
		printf("%s:setsockopt server err\n\r",__FUNCTION__);
		//close the server
		closesocket(sockserver);
		return 0;
	}

	if(bind(sockserver, (struct sockaddr *)&local, addr_len)==-1)
	{
		printf("%s:bind err\n\r",__FUNCTION__);
		//close the server
		closesocket(sockserver);
		return 0;
	}


	if(listen(sockserver, 1)==-1)
	{
		printf("%s:listen err\n\r",__FUNCTION__);
		//close the server
		closesocket(sockserver);
		return 0;
	}

	memcpy((void *)ipByte,(void *)&local.sin_addr.s_addr,4);
	client->buflen = snprintf((char *)client->buf,CN_CLIENT_BUFLEN,"227 Entering passive mode (%d,%d,%d,%d,%d,%d)\r\n",\
			ipByte[0], ipByte[1], ipByte[2], ipByte[3], dig1, dig2);
	send(client->cmdsock,client->buf,client->buflen,0);
	memset(client->buf,0,CN_CLIENT_BUFLEN);
	client->buflen = 0;

	if((sockclient = accept(sockserver, (struct sockaddr*)&pasvremote, &addr_len))==-1)
	{
		printf("%s:accept err\n\r",__FUNCTION__);
		__sendresponse(client,425,"CAN NOT OPEN DATA CONNECTION");
		if(sockserver !=  -1)
		{
			closesocket(sockserver);
		}
	}
	else
	{
		opt =1;
		optlen = sizeof(opt);
		setsockopt(sockclient,IPPROTO_TCP,TCP_NODELAY,&opt,optlen);

		opt = 1;
		setsockopt(sockclient,SOL_SOCKET,SO_KEEPALIVE,&opt,sizeof(opt));
		printf("Got Data(PASV) connection from %s\r\n", inet_ntoa(pasvremote.sin_addr));

		//close the server
		closesocket(sockserver);

		client->isactive = false;
		if(client->pasvsock != -1)
		{
			closesocket(client->pasvsock);
		}
		client->pasvsock = sockclient;

	}
	return 0;
}


//this function used for the retr message
static int __retrmsgdeal(tagFtpClient *client,char *para)
{
	char filename[256];
	int file_size;
	int sock;
	int fd;

	__mkpath(client, para, filename, 256);
	file_size = __filesize(filename);
	if (file_size == -1)
	{
		client->fileoffset=0;
		__sendresponse(client,CN_FILEINVALID,"file unavailable");
		return 0;
	}

	fd = open(filename, O_RDONLY, 0);
	if (fd < 0)
	{
		client->fileoffset=0;
		__sendresponse(client,CN_FILEINVALID,"file unavailable");
		return 0;
	}

	if((client->fileoffset>0) && (client->fileoffset < file_size))
	{
		lseek(fd, client->fileoffset, SEEK_SET);
	}

	if(client->isactive)
	{
		__sendresponse(client,CN_OPENCONNECTION,"open ASCII mode data connection for directory list");
		if(client->pasvsock != -1) //there must be something wrong
		{
			printf("%s:isactive but the dtsock is exist\n\r",__FUNCTION__);
		}
		sock = __connectport(client);
		if(sock == -1)
		{
			__sendresponse(client,CN_OPENDATACONNECTIONFAILED,"open the data connection failed");
		}
	}
	else
	{
		sock = client->pasvsock;
		client->pasvsock = -1;
		__sendresponse(client,CN_OPENCONNECTION,"open BINARY mode data connection for  filetransfer");
	}

	if(sock > 0)
	{
		while((client->buflen = read(fd, client->buf, CN_CLIENT_BUFLEN))>0)
		{
			if(false == sendexact(sock, client->buf, client->buflen))
			{
				break; //maybe any send err happend
			}
		}
		__sendresponse(client,CN_CLOSEDATACONNECTION,"close data connection");
		close(fd);
		closesocket(sock);
	}


	return 0;
}

//this function used for the stor message
static int __stormsgdeal(tagFtpClient *client,char *file)
{
	char filename[256];
	int sock;
	int fd;

	s64  timestart;
	s64  timestop;
	s64  time1;
	s64  time2;
	s64  time3;
	u32  timetotal;
	u32  timenet;
	u32  timefs;
	u32  filelen = 0;

	__mkpath(client, file, filename, 256);
	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0);
	if (fd < 0)
	{
		client->fileoffset=0;
		__sendresponse(client,CN_FILEINVALID,"file unavailable");
		return 0;
	}

	if(client->isactive)
	{
		__sendresponse(client,CN_OPENCONNECTION,"open ASCII mode data connection for directory list");
		if(client->pasvsock != -1) //there must be something wrong
		{
			printf("%s:isactive but the dtsock is exist\n\r",__FUNCTION__);
		}
		sock = __connectport(client);
		if(sock == -1)
		{
			__sendresponse(client,CN_OPENDATACONNECTIONFAILED,"open the data connection failed");
		}
	}
	else
	{
		sock = client->pasvsock;
		client->pasvsock = -1;
		__sendresponse(client,CN_OPENCONNECTION,"open BINARY mode data connection for  filetransfer");
	}

	if(sock > 0)
	{
		filelen = 0;
		timestart = DjyGetSysTime();
		while(1)
		{
			time1 = DjyGetSysTime();
			client->buflen=recv(sock, client->buf, CN_CLIENT_BUFLEN, 0);
			time2 = DjyGetSysTime();
			if(client->buflen > 0)
			{

				write(fd, client->buf, client->buflen);
				filelen += client->buflen;
			}
			else if(client->buflen < 0)
			{
				//may be timeout, just another time
			}
			else
			{
				//this is closed
				break;
			}
			time3 = DjyGetSysTime();

			timenet += (u32)(time2-time1);
			timefs +=(u32)(time3-time2);
		}
		timestop = DjyGetSysTime();
		timetotal = (u32)(timestop - timestart);
		printf("file:%s:size:%d nettime:%d fstime:%d totaltime:%d\n\r",\
				file,filelen,timenet,timefs,timetotal);
		__sendresponse(client,CN_CLOSEDATACONNECTION,"close data connection");
		close(fd);
		closesocket(sock);
	}
	return 0;
}

//use this function to send the file size
static int __sizemsgdeal(tagFtpClient* client,char *para)
{
	char filename[256];
	int file_size;

	__mkpath(client, para, filename, 256);
	file_size = __filesize((char *)filename);
	if( file_size == -1)
	{
		__sendresponse(client,CN_FILENOTVALID,"file invalid");
	}
	else
	{
		client->buflen = snprintf((char *)client->buf,CN_CLIENT_BUFLEN,\
				"%d %d\r\n",CN_FILESTAT,file_size);
		send(client->cmdsock,client->buf,client->buflen,0);
		memset(client->buf,0,CN_CLIENT_BUFLEN);
		client->buflen = 0;
	}

	return 0;
}

//use this function cwd
static int __cwdmsgdeal(tagFtpClient* client,char *para)
{
	char filename[256];

    __mkpath(client, para, filename, 256);
    strncpy(client->curdir,filename,CN_PATH_LENGTH);
	client->buflen = snprintf((char *)client->buf,CN_CLIENT_BUFLEN,\
			"%d Changed to directory \"%s\"\r\n",250,client->curdir);
	send(client->cmdsock,client->buf,client->buflen,0);
	memset(client->buf,0,CN_CLIENT_BUFLEN);
	client->buflen = 0;

	return 0;
}

//use this function cdup
static int __cdupmsgdeal(tagFtpClient* client,char *para)
{
	int i = 0;
	if(0 !=strcmp(client->curdir,CN_FTP_ROOT)) //BACK TO THE PARENT DIR
	{
		i = strlen(client->curdir);
		for(;i>0;i--)
		{
			if(client->curdir[i] == '/')
			{
				client->curdir[i] ='\0';
				break;
			}
			else
			{
				client->curdir[i] ='\0';
			}
		}
	}
	client->buflen = snprintf((char *)client->buf,CN_CLIENT_BUFLEN,\
			"%d Changed to directory \"%s\"\r\n",CN_PATHBUILD,client->curdir);
	send(client->cmdsock,client->buf,client->buflen,0);
	memset(client->buf,0,CN_CLIENT_BUFLEN);
	client->buflen = 0;
	return 0;
}
//use this function rest
static int __restmsgdeal(tagFtpClient* client,char *para)
{
    if(atoi(para)>=0)
    {
        client->fileoffset=atoi(para);
        __sendresponse(client,350,"Send RETR or STOR to start transfert.");
    }
    else
    {
        __sendresponse(client,CN_CMDFAILED,"rest cmd execute failed");
    }
    return 0;
}

//use this function mkd
static int __mkdmsgdeal(tagFtpClient* client,char *para)
{
    char filename[256];

    __mkpath(client, para, filename, 256);
    printf("MKD:%s\n\r",filename);
    if(mkdir((char *)filename, 0) == -1)
    {
        __sendresponse(client,550,"Directory existed");

    }
    else
    {
        __sendresponse(client,257,"Directory created successfully");
    }

    return 0;
}
//use this function delete
static int __delemsgdeal(tagFtpClient* client,char *para)
{
    char filename[256];

    __mkpath(client, para, filename, 256);
    if(unlink((char *)filename) == -1)
    {
        __sendresponse(client,550,"Directory not exist");
    }
    else
    {
        __sendresponse(client,250,"File deleted successfully");
    }

    return 0;
}

//use this function rmd
static int __rmdmsgdeal(tagFtpClient* client,char *para)
{
    char filename[256];

    __mkpath(client, para, filename, 256);
    if(unlink((char *)filename) == -1)
    {
        __sendresponse(client,550,"Directory not exist");
    }
    else
    {
        __sendresponse(client,257,"File deleted successfully");
    }

    return 0;
}


static int __msgdeal(tagFtpClient* client, char *buf)
{
	int  result;
	char *parameter_ptr, *ptr;
	/* remove \r\n */
	ptr = buf;
	while (*ptr)
	{
		if (*ptr == '\r' || *ptr == '\n') *ptr = 0;
		ptr ++;
	}

	/* get request parameter */
	parameter_ptr = strchr(buf, ' '); if (parameter_ptr != NULL) parameter_ptr ++;
	result = 0;
	// debug:
	printf("%s requested:%s :para:%s\r\n",client->user, buf,parameter_ptr);
	if(__cmdmatch(buf, "USER")==0)
	{
		// login correct
		result = __usermsgdeal(client,parameter_ptr);
	}
	else if(__cmdmatch(buf, "PASS")==0)
	{
		result = __passmsgdeal(client,parameter_ptr);
	}
	else if(__cmdmatch(buf, "LIST")==0  )
	{
		result = __listmsgdeal(client,"detail");
	}
	else if(__cmdmatch(buf, "NLST")==0 )
	{
		result = __listmsgdeal(client,"simple");
	}
	else if(__cmdmatch(buf, "PWD")==0 || __cmdmatch(buf, "XPWD")==0)
	{
		result = __pwdmsgdeal(client,parameter_ptr);
	}
	else if(__cmdmatch(buf, "TYPE")==0)
	{
		result = __typemsgdeal(client,parameter_ptr);
	}
	else if(__cmdmatch(buf, "PASV")==0)
	{
		result = __psvmsgdeal(client,parameter_ptr);
	}
	else if(__cmdmatch(buf, "PORT")==0)
	{
	    result = __portmsgdeal(client,parameter_ptr);
	}
	else if (__cmdmatch(buf, "RETR")==0)
	{
		result = __retrmsgdeal(client,parameter_ptr);
	}
	else if (__cmdmatch(buf, "STOR")==0)
	{
		result = __stormsgdeal(client,parameter_ptr);
    }
	else if(__cmdmatch(buf, "SIZE")==0)
	{
		result = __sizemsgdeal(client,parameter_ptr);
	}
	else if(__cmdmatch(buf, "SYST")==0)
	{
		__sendresponse(client,215,"DJYOS");
		result = 0;
	}
	else if(__cmdmatch(buf, "CWD")==0)
	{
		result = __cwdmsgdeal(client,parameter_ptr);
	}
	else if(__cmdmatch(buf, "CDUP")==0)
	{
		result = __cdupmsgdeal(client,parameter_ptr);
	}
	else if(__cmdmatch(buf, "REST")==0)
	{
	    result = __restmsgdeal(client,parameter_ptr);
	}
	else if(__cmdmatch(buf, "MKD")==0)
	{
	    result = __mkdmsgdeal(client,parameter_ptr);
	}
	else if(__cmdmatch(buf, "DELE")==0)
	{
	    result = __delemsgdeal(client,parameter_ptr);
	}
	else if(__cmdmatch(buf, "RMD")==0)
	{
	    result = __rmdmsgdeal(client,parameter_ptr);
	}

	else if(__cmdmatch(buf, "QUIT")==0)
	{
		__sendresponse(client,CN_LOGOUT,"QUIT CMD,bye");
		result = -1;
	}
	else
	{
		__sendresponse(client,CN_CMDINVALID,"this cmd is not supported yet");
		result = 0;
	}
	return result;
}

//use this function to deal the connection
static bool_t __ClientEngine(int sockclient)
{
	int sockopt;
	int rcvlen;
	tagFtpClient  *client = NULL;

	client = NULL;

	sockopt = 1;
	if(0 != setsockopt(sockclient,SOL_SOCKET,SO_KEEPALIVE,&sockopt,sizeof(sockopt)))
	{
		printf("Error on setsockopt keepalive\r\nContinuing...\r\n");
		closesocket(sockclient);
		goto EXIT_QUIT;
	}
	//now we malloc an client for the connection and initialize it
	client = malloc(sizeof(tagFtpClient));
	if(NULL != client)
	{
		//do the initialize
		memset((void *)client,0,sizeof(tagFtpClient));
		client->cmdsock = sockclient;
		strcpy(client->curdir,CN_FTP_ROOT);
		pFtpClient = client;
		//ok, now use  the client processor to deal it
		//first we send the welcome message
		__sendresponse(client,CN_SERVERREADY,"welcome to the ftperver for djysos");
		//then do the while loop to do the interactive
		while(1)
		{
			//receive messages from the command sock
			rcvlen=recv(client->cmdsock, client->buf, CN_CLIENT_BUFLEN, 0);
			if(rcvlen > 0)
			{
				if(__msgdeal(client,(char *)client->buf)==-1)
				{
					break;
				}
			}
			else if(rcvlen < 0)
			{
				//may be time out,continue
			}
			else
			{
				break;
			}
		}
		//now its time to exit
		if(-1 != client->pasvsock)
		{
			closesocket(client->pasvsock);
		}
		closesocket(sockclient);
		free((void *)client);
		pFtpClient = NULL;
	}
	else
	{
		printf("Error on malloc mem for the connection\r\nContinuing...\r\n");
		closesocket(sockclient);
	}

EXIT_QUIT:
	return true;
}

//this is the ftp server main,always accept here
static ptu32_t ServerMain()
{
	int sockclient;
	int sockserver;
	struct sockaddr_in local;
	struct sockaddr_in remote;
	int addr_len = sizeof(struct sockaddr);

	local.sin_port=htons(CN_FTP_SPORT);
	local.sin_family=AF_INET;
	local.sin_addr.s_addr=INADDR_ANY;
	sockserver=socket(AF_INET, SOCK_STREAM, 0);
	if(sockserver < 0)
	{
		printf("%s:socket err\r\n",__FUNCTION__);
		return 0;
	}
	if(0 != bind(sockserver, (struct sockaddr *)&local, addr_len))
	{
		printf("%s:bind err\r\n",__FUNCTION__);
		goto SERVER_EXIT;
	}
	if(0 != listen(sockserver, CN_FTP_LISTENMAX))
	{
		printf("%s:listen err\r\n",__FUNCTION__);
		goto SERVER_EXIT;
	}
	while(1) //always do the loop
	{
		sockclient = accept(sockserver, (struct sockaddr*)&remote, &addr_len);
		if(sockclient == -1)
		{
			printf("Error on accept()\r\nContinuing...\r\n");
		}
		else
		{
			__ClientEngine(sockclient);
		}
	}
SERVER_EXIT:
	closesocket(sockserver);
	return 0;
}

static bool_t ftpservice(char *para)
{

    u16 eventID;
    u16 evttID;

    evttID = Djy_EvttRegist(EN_CORRELATIVE, gFtpAcceptPrior, 0, 1,
            ServerMain,NULL, gFtpAcceptStack, "ftpserverAccept");
    if(evttID == CN_EVTT_ID_INVALID)
    {
        printf("%s:Register ftpserver evtt failed\r\n",__FUNCTION__);
        goto EXIT_TFTPSERVEREVTT;
    }
    eventID = Djy_EventPop(evttID, NULL, 0, 0, 0, 0);
    if(eventID == CN_EVENT_ID_INVALID)
    {
        printf("%s:pop ftpserver event failed\r\n",__FUNCTION__);
        goto EXIT_TFTPSERVEREVENT;
    }
    return true;

EXIT_TFTPSERVEREVENT:
    Djy_EvttUnregist(evttID);
EXIT_TFTPSERVEREVTT:
    return false;
}


struct ShellCmdTab  gServiceFtp[] =
{
    {
        "ftpstat",
        ftpstat,
        "usage:ftpstat",
        "usage:ftpstat",
    }
};
#define CN_FTPDEBUG_NUM  ((sizeof(gServiceFtp))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc gServiceFtpCmdRsc[CN_FTPDEBUG_NUM];

//THIS IS PING MODULE FUNCTION
bool_t ServiceFtpInit(ptu32_t para)
{
    bool_t result;
    result = ftpservice(NULL);
    Sh_InstallCmd(gServiceFtp,gServiceFtpCmdRsc,CN_FTPDEBUG_NUM);
    return result;
}



