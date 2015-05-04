// =======================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: VNCServer
// 模块版本: V1.00(初始化版本)
// 文件名  ：vncserverapi.c
// 创建人员: Administrator
// 创建时间: 2013-3-5
//           该文件实现VNCSERVER的主要API函数
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <V1.00> <2013-3-5>, <修改人员>: <修改功能概述>
// =======================================================================
#include  "vncserver_event.h"

#include  "rfb.h"
#include  "debug.h"
#include  "vnc_socket.h"//关于网络的处理部分

#include "vnc_image_update.h"
#include "vnc_thread.h"

//默认的镜像显示器的大小，现在看来是不现实的，因为镜像显示器只能作为物理显示器的子显示器存在
#define  SCREENWIDTH   640
#define  SCREENHEIGHT  240

void rfbDefaultScreenInit(rfbScreenInfoPtr rfbserver)
{
   XMutexInit(&(rfbserver->cl_mutex), "ClientMutex");
   XMutexInit(&(rfbserver->frame_mutex), "ClipMutex");
   //目前只关心TCP的链接问题，其余的不会关心的（UDP,HTTP等）
   rfbserver->rfbPort=5900;
   rfbserver->socketInitDone=FALSE;
   rfbserver->maxFd=0;
   rfbserver->rfbListenSock=-1;

   rfbserver->desktopName = "LibVNCServer";
   rfbserver->rfbAlwaysShared = FALSE;
   rfbserver->rfbNeverShared = FALSE;

    gethostname(rfbserver->rfbThisHost, 255);

   //将客户端队列初始化为空；
   rfbserver->rfbClientHead=NULL;
   rfbserver->cl_number=0;

   //镜像显示器为空
   rfbserver->mirror_display = NULL;
   rfbserver->mirror_display_enable = 0;

   //初始化主事件和子事件的状态
   rfbserver->mainevevent_tobedead = 0;
   rfbserver->updateevent_end = 0;
   rfbserver->netevent_end = 0;
}

// =========================================================================
// 函数功能：负责图形数据更新给客户端
// 输入参数：服务器参数rfbScreen
// 输出参数：
// 返回值  ：
// 说明    ：
// =========================================================================
void   ProcessFrameUpdateThread()
{
//  rfbserver->event_id_update = Djy_MyEventId();
//  rfbserver->event_update_prio = djy_get_event_prio();
    while(1)
    {
//     Djy_WaitEvttPop(rfbserver->event_id_update,NULL,CN_TIMEOUT_FOREVER);
//      Djy_SetEventPrio(rfbserver->event_update_prio);
       delay(50);
       XMutexWait(&rfbserver->cl_mutex);
       //实在没有什么好做的，因为都是引用别人的
       if((rfbserver->updateevent_end == 1)||(rfbserver->mainevevent_tobedead == 1))
       {
           XMutexPost(&rfbserver->cl_mutex);
           break;
       }
       UpdateFrame2Clist(rfbserver);

       XMutexPost(&rfbserver->cl_mutex);
    }
//自然消失
    rfbserver->updateevent_end = 1;
}
// =========================================================================
// 函数功能：处理新客户端的连接和正常通信
// 输入参数：服务器参数rfbserver
// 输出参数：
// 返回值  ：
// 说明    ：
// =========================================================================
void   ProcessMediaInterfaceThread()
{
    rfbClientPtr cl,temp;
    rfbInitSockets(rfbserver);
//  rfbserver->event_id_net = Djy_MyEventId();
    printf("--the server socket Init end!\n");
      while(1)//循环检测
      {
          delay(10);
          XMutexWait(&rfbserver->cl_mutex);

          //主事件或者图形更新事件任意一个失败，都将退出
          if((rfbserver->mainevevent_tobedead == 1)||(rfbserver->updateevent_end == 1))
          {
              //客户端列表和socket的处理
              cl = rfbserver->rfbClientHead;
              while(cl)
              {
                  temp = cl->next;
                  rfbCloseClient(cl);
                  cl = temp;
              }
              //服务器的socket处理,暂时不处理，待DJYOS版本的TCPIP出台

              //释放锁，退出循环
              XMutexPost(&rfbserver->cl_mutex);
              break;
          }

          rfbCheckFds(rfbserver,4000);

          XMutexPost(&rfbserver->cl_mutex);

      }
//事件结束
      rfbserver->netevent_end = 1;
}
