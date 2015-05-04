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
#include "rfb.h"
#include "vncserver_event.h"
#include "vnc_mirror_display.h"
#include "vnc_thread.h"//创建多线程
#include "debug.h"//调试函数
#include "vncserver.h"
#include "vnc_keyboard.h"
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

//应该显式的调用malloc和free，而不应该藏着掖着，不然很容易出现内存泄露问题

int vncserver_start()
{
    bool_t  ret;
  //进行必要的初始化
    rfbserver=M_MallocLc(sizeof(rfbScreenInfo),0);
    if(NULL==rfbserver)
    {
        debug_printf("the main failed--rfbDefaultScreenInit\n");
        return -1;
    }
    rfbDefaultScreenInit(rfbserver);
  //安装镜像显示器
    ret= install_vncserver_mirrordisplay(rfbserver);
    if(false==ret)
    {
        free(rfbserver);
        debug_printf("##failed to install the vnc mirror screen!\n");
        return -1;
    }
#if CN_CFG_KEYBOARD == 1
    vnc_keyboard_visual_init(rfbserver);
    install_vncserver_keyboard(rfbserver);
    install_vncserver_touchlcd(rfbserver);
#endif

    debug_printf("screen width=%d  height=%d\n",rfbserver->width,rfbserver->height);
//创建更新数据图像任务
    ret=create_thread(ProcessFrameUpdateThread,rfbserver);
    if(ret!=0)
    {
      debug_printf("failed to create the frameUpdateThread thread!\n");
      goto pop_update_event_failed;
    }
  //创建检测物理端口任?

    ret=create_thread(ProcessMediaInterfaceThread,rfbserver);
    if(ret!=0)
    {
        debug_printf("failed to create the event thread!\n");
        goto pop_net_event_failed;
    }

    //使能镜像显示器
    rfbserver->mirror_display_enable = 1;
    debug_printf("---------------server main ok--------------!\n");
    return  0;
//失败以后，应该等待子线程结束
pop_net_event_failed:
    rfbserver->netevent_end = 1;
pop_update_event_failed:
    rfbserver->mainevevent_tobedead = 1;

    vncserver_exit(rfbserver);
    debug_printf("---------------server main failed--------------!\n");
    return -1;
}

// =============================================================================
// 函数功能：vnc服务器退出函数
// 输入参数：
// 输出参数：
// 返回值  ：
// 说明:对vncserver所占用的资源进行释放工作
// =============================================================================
void vncserver_exit(rfbScreenInfoPtr rfbserver)
{
    rfbserver->mainevevent_tobedead = 1;
//等待俩事件都结束
    while(1)
    {
        delay(1000);
        if((rfbserver->updateevent_end == 1)&&(rfbserver->netevent_end == 1))
        {
            break;
        }
    }
//俩子事件都已经结束（其实并没有子事件这种说法,借鉴linux等形象说法）
//卸载显示器
    uninstall_vncserver_mirrrordisplay(rfbserver);
//卸载虚拟设备
#if CN_CFG_KEYBOARD == 1
    uninstall_vnc_keyboard();
    uninstall_vnc_touchboard();
#endif
//删除互斥量
    XMutexDelete(&(rfbserver->frame_mutex));
    XMutexDelete(&(rfbserver->cl_mutex));
//释放server的空间
    free(rfbserver);
    rfbserver = NULL;
    debug_printf("The vncserver exit normally!\n");
}
