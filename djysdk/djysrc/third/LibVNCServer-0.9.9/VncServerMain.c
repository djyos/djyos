// =======================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: VNCServer
// 模块版本: V1.00(初始化版本)
// 文件名     ：VNCSERVER.c
// 创建人员: Administrator
// 创建时间: 2013-3-8
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <V1.00> <2013-3-8>, <修改人员>: <修改功能概述>
// =============================================================================
#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#include <rfb/rfb.h>
#include "VncOsPort.h"
#include "VncNetPort.h"

rfbScreenInfo* pgRfbServerScreen = NULL;
void rfbDefaultScreenInit(rfbScreenInfo* ptscreen)
{
   //目前只关心TCP的链接问题，其余的不会关心的（UDP,HTTP等）
   ptscreen->listenport=5900;
   ptscreen->listenSock=-1;
   ptscreen->desktopName = "LibVNCServer";
   //将客户端队列初始化为空；
   ptscreen->clientHead=NULL;
   INIT_MUTEX(ptscreen->clientsync);
   ptscreen->protocolMajorVersion = rfbProtocolMajorVersion;
   ptscreen->protocolMinorVersion = rfbProtocolMinorVersion;

   /* cursor */

   ptscreen->cursorX=ptscreen->cursorY=ptscreen->underCursorBufferLen=0;
   ptscreen->underCursorBuffer=NULL;
   ptscreen->dontConvertRichCursorToXCursor = FALSE;
   ptscreen->cursor = NULL;
   INIT_MUTEX(ptscreen->cursorMutex);

   IF_PTHREADS(ptscreen->backgroundLoop = FALSE);

   /* proc's and hook's */

   ptscreen->setTranslateFunction = rfbSetTranslateFunction;
   ptscreen->kbdAddEvent = NULL;
   ptscreen->kbdReleaseAllKeys = NULL;
   ptscreen->ptrAddEvent = NULL;
   ptscreen->setXCutText = NULL;
   ptscreen->getCursorPtr = NULL;
   ptscreen->newClientHook = NULL;
   ptscreen->displayHook = NULL;
   ptscreen->displayFinishedHook = NULL;
   ptscreen->getKeyboardLedStateHook = NULL;
   ptscreen->xvpHook = NULL;

}
// =========================================================================
// 函数功能：处理新客户端的连接和正常通信
// 输入参数：服务器参数pgRfbServerScreen
// 输出参数：
// 返回值  ：
// 说明    ：
// =========================================================================
ptu32_t ClientListenTask()
{
    rfbInitServerSocket(pgRfbServerScreen);
    while(1)//循环检测
    {
        rfbCheckFds(pgRfbServerScreen,4000);
    }
    return 0;
}
// =============================================================================
// 函数功能：VNC服务器
// 输入参数：
// 输出参数：
// 返回值  ：
// 说明    ：采用多线程的形式实现服务器的功能，一共有三个线程
//          1,accept线程用于专门处理新客户端的连接
//          2,messsadge 线程用于专门处理客户端和服务器的消息交流
//          3,update线程用于处理将图像更新给客户端
// =============================================================================
int VncServer_Main(const char *lcdname)
{
    bool_t  ret;
    pgRfbServerScreen=M_Malloc(sizeof(rfbScreenInfo),0);
    if(NULL==pgRfbServerScreen)
    {
    	printk("%s :No Mem For pgRfbServerScreen\n\r",__FUNCTION__);
        goto EXIT_NOMEMFORSCREEN;
    }
    else
    {
    	memset((void *)pgRfbServerScreen, 0, sizeof(rfbScreenInfo));
    }
    rfbDefaultScreenInit(pgRfbServerScreen);

    //安装VNC显示器
    extern bool_t VncDispInstall(rfbScreenInfo* rfbserver,const char *lcdname);
    if(false == VncDispInstall(pgRfbServerScreen,lcdname))
    {
    	printk("%s :VncDisplay　Init Failed\n\r",__FUNCTION__);
    	goto EXIT_DISPLAYFAILED;
    }
    //创建检测物理端口任?
    ret=TaskCreate(ClientListenTask,pgRfbServerScreen,"VncServerListen");
    if(false == ret)
    {
    	printk("%s :CreateListenTask Failed\n\r",__FUNCTION__);
        goto CLIENTLISTENTASK_FAILED;
    }
	printk("%s :INIT SUCCESS\n\r",__FUNCTION__);
    return  0;
//失败以后，应该等待子线程结束
CLIENTLISTENTASK_FAILED:
EXIT_DISPLAYFAILED:
	free(pgRfbServerScreen);
	pgRfbServerScreen = NULL;
EXIT_NOMEMFORSCREEN:
    printk("%s:INIT FAILED!\n\r",__FUNCTION__);
    return -1;
}
