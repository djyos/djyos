/*
 * ie.c
 *
 *  Created on: 2016年3月16日
 *      Author: zqf
 */


#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "os.h"

#include <sys/socket.h>
#include <netdb.h>

#define SOCKET  int

#define CN_REQ_LENGTH  1024
static char szHttpRest[CN_REQ_LENGTH] = {0};

int ieMain(int argc, const char *argv[])
{
    struct in_addr ipaddr;

    struct hostent *host;

    if(0 == inet_aton(argv[1],&ipaddr))
    {
        //use the dns to get the ip
        host = gethostbyname(argv[1]);
        if(NULL != host)
        {
            memcpy((void *)&ipaddr,host->h_addr_list[0],sizeof(ipaddr));
        }
        else
        {
            printf("%s:Unknown host:%s\n\r",__FUNCTION__,argv[1]);
            return false;
        }
    }
    // 度娘服务器信息
    struct sockaddr_in  webServerAddr;
    webServerAddr.sin_family = AF_INET;
    webServerAddr.sin_addr.s_addr=ipaddr.s_addr;
    webServerAddr.sin_port = htons(80);

    // 创建客户端通信socket
    SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);

    // 去连接度娘
    int nRet = connect(sockClient ,(struct sockaddr*)&webServerAddr, sizeof(webServerAddr));
    if(nRet < 0)
    {
        printf("connect error\n");
        return 1;
    }

    //拼凑请求数据包
    sprintf(szHttpRest, "GET %s HTTP/1.1\r\nHost:%s\r\nConnection: Keep-Alive\r\n\r\n", argv[2],argv[1]);

    // 准备向度娘发送http的GET请求
    printf("\n\r---------------------send is :---------------------\n\r");
    printf("%s\n\r", szHttpRest);

    // 利用tcp向度娘发送http的GET请求
    nRet = send(sockClient , szHttpRest, strlen(szHttpRest) + 1, 0);
    if(nRet < 0)
    {
        printf("send error\n");
        return 1;
    }

    u32  bytesRcv =0;
    while(1)
    {
            // 每次接收一个字节
            char szRecvBuf[2] = {0};
            nRet = recv(sockClient ,szRecvBuf, 1 ,0);

            // 接收错误
            if(nRet <= 0)
            {
                break;
            }
            bytesRcv+=nRet;
            printf("%c",szRecvBuf[0]);
    }
    closesocket(sockClient);
    printf("\n\r%s:%d bytes received\n\r",__FUNCTION__,bytesRcv);

    return nRet;
}

bool_t ieShell(char *param)
{
    int argc =2;

    char *argv[3];

    string2arg(&argc,&argv[1],param);

    argv[0] = NULL;
    if(argc == 2)
    {
        ieMain(argc,argv);
    }

    return true;
}

#include <shell.h>
static struct ShellCmdTab  gIeDebug[] =
{
    {
        "ie",
        ieShell,
        "usage:ie + domain + page",
        NULL
    }
};




#define CN_IE_DEBUG_CMDNUM  ((sizeof(gIeDebug))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc gIeDebugCmdRsc[CN_IE_DEBUG_CMDNUM];

bool_t ModuleInstallIe(ptu32_t param)
{
    bool_t result;

    result = Sh_InstallCmd(gIeDebug,gIeDebugCmdRsc,CN_IE_DEBUG_CMDNUM);

    return result;
}
