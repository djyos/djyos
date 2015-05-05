// =======================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: VNCServer//定义server和client的结构
// 模块版本: V1.00(初始化版本)
// 文件名     ：rfb.h
// 创建人员: Administrator
// 创建时间: 2013-3-5
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <V1.00> <2013-3-5>, <修改人员>: <修改功能概述>
// =======================================================================

#ifndef _RFB_H_
#define _RFB_H_

//#include "misc.h"//基本数据结构的重定义
#include "rfbproto.h"  //关于RFB协议的一些定义

/*
 *  Copyright (C) 2000, 2001 Const Kaplinsky.  All Rights Reserved.
 *  Copyright (C) 2000 Tridia Corporation.  All Rights Reserved.
 *  Copyright (C) 1999 AT&T Laboratories Cambridge.  All Rights Reserved.
 *
 *  This is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 *  USA.
 */

/*
 * rfbproto.h - header file for the RFB protocol version 3.3
 *
 * Uses types CARD<n> for an n-bit unsigned integer, INT<n> for an n-bit signed
 * integer (for n = 8, 16 and 32).
 *
 * All multiple byte integers are in big endian (network) order (most
 * significant byte first).  Unless noted otherwise there is no special
 * alignment of protocol structures.
 *
 *
 * Once the initial handshaking is done, all messages start with a type byte,
 * (usually) followed by message-specific data.  The order of definitions in
 * this file is as follows:
 *
 *  (1) Structures used in several types of message.
 *  (2) Structures used in the initial handshaking.
 *  (3) Message types.
 *  (4) Encoding types.
 *  (5) For each message type, the form of the data following the type byte.
 *      Sometimes this is defined by a single structure but the more complex
 *      messages have to be explained by comments.
 */

 //DJY中一些定义
#include "stdint.h"
#include "lock.h"
#include "vnc_define.h"

typedef struct _rect
{
  u16 x;
  u16 y;
  u16 w;
  u16 h;
}rect;

typedef void (*KbdAddEventProcPtr)(int down, KeySym keySym,\
        struct rfbClientRec* cl);
typedef void (*KbdReleaseAllKeysProcPtr)(struct rfbClientRec* cl);
typedef void (*PtrAddEventProcPtr)(int buttonMask, int x, int y, \
        struct rfbClientRec* cl);
typedef void (*SetXCutTextProcPtr)(char* str,int len, struct rfbClientRec* cl);
typedef void (*rfbTranslateFnType)(rfbPixelFormat *in,
                                   rfbPixelFormat *out,
                                   char *iptr, char *optr,
                                   int bytesBetweenInputLines,
                                   int width, int height);

//服务器screen的定义，基本上每个服务器只有一个。
#define CL_BUFF_SIZE   1024
#define cn_vnc_keyboard_height  0x64  //高度32像素
#define cn_vnc_keyboard_lines   0x01  //一排
#define cn_vnc_keyboard_visual_enable 0x01


typedef struct
{
    struct tagMutexLCB  cl_mutex;//对client列表的保护
    struct tagMutexLCB  frame_mutex;//对VNC的镜像buffer的保护
//    u16  event_id_update;
//    u16  event_id_net;
//    u8   event_update_prio;
// 像素信息
    u16 width;
    u16 height;
    u32 paddedWidthInBytes;
//    u16 depth;
//    u16 bitsPerPixel;
    rfbPixelFormat rfbServerFormat;
//    u16 oscolortype_server;
// 网络部分
    char* desktopName;
    char rfbThisHost[255];
    int rfbPort;
    int rfbListenSock;
    int socketInitDone;
    int rfbMaxClientWait;
//    char* rfbAuthPasswdFile;          //目前使用的是静态密码123
    int rfbDeferUpdateTime;         //对于端口和  图像的更新间隔
    int rfbAlwaysShared;           //关于是否共享的问题
    int rfbNeverShared;
    struct rfbClientRec* rfbClientHead;//排列的客户端表

//用有的镜像显示器
    struct tagDisplayRsc *mirror_display;
    u8    mirror_display_enable;//当还没有使能的时候，是不会向镜像显示器写图像的
//用于客户端额记录
    int cl_number;//记录cl的数目
    fd_set allFds;
    int maxFd;
// 关于数据的存储
    char* frameBuffer;
    char cl_buffer[CL_BUFF_SIZE];//用于给客户端发送数据使用的buffer
    u16  ublen;//用来指示cl_buffer中的数据长度
    struct tagClipRect *clip_head; //标记改变的区域
//  用于server退出时使用，同时也监视着子事件的状态
    u8  mainevevent_tobedead;
    u8  updateevent_end;
    u8  netevent_end;
//vnc输入设备的处理
#if CN_CFG_KEYBOARD == 1
    u32   key_state;//按键状态
    u32   key_update_list;//按键刷新标志
    struct tagKeyBoardPrivate vnc_keyboard;//vnc的客户端键盘驱动
    struct tagSingleTouchPrivate vnc_lcdtouch;//vnc的触摸屏驱动
#endif
    KbdAddEventProcPtr kbdAddEvent;
    KbdReleaseAllKeysProcPtr kbdReleaseAllKeys;
    PtrAddEventProcPtr ptrAddEvent;
    SetXCutTextProcPtr setXCutText;

} rfbScreenInfo, *rfbScreenInfoPtr;


//客户端定义，每个客户端都拥有一个这样的结构，用来表达服务器和客户端建立的一些数据特征

#define CHALLENGESIZE 16  //挑战验证长度
#define MAX_ENCODINGS 10  //加密的种类
#define TIGHT_DEFAULT_COMPRESSION  6 //tight方式压缩级别


typedef struct rfbClientRec
{
    rfbScreenInfoPtr screen;//所属的VNCserver

    int sock; //socket套接字
    char *host;//客户端的名字
    int  protocolMajorVersion;//客户端主版本号
    int  protocolMinorVersion;//客户端次版本号

    int readyForSetColourMapEntries;//准备好发送颜色映射表

    /* Possible client states: */
    enum {
        RFB_PROTOCOL_VERSION,   /* establishing protocol version */
        RFB_SECURITY_TYPE,      //安全类型
        RFB_AUTHENTICATION,     /* authenticating */
        RFB_INITIALISATION,     /* sending initialisation messages */
        RFB_NORMAL              /* normal protocol messages */
    } state;
    u8 HasRequest;//即准备好，可以进行图形数据的传输。
    rect modifiedregion;//客户端感兴趣的区域
    u8  InitDone;  //用来标记是否要发送整个感兴趣区域
    //用来标记该client是否是可用的，由update标记为dirty，由端口监视删除
    u8  client_dirty;
//存储认证挑战
    u8 authChallenge[CHALLENGESIZE];
// 像素信息
    rfbTranslateFnType translateFn;//颜色转换函数
    rfbPixelFormat format;//像素格式
    u16   oscolortype_client;

//关于数据压缩
    /* statistics */

    int useCopyRect;
    int preferredEncoding;
    int correMaxWidth, correMaxHeight;
    int rfbBytesSent[MAX_ENCODINGS];
    int rfbRectanglesSent[MAX_ENCODINGS];
    int rfbLastRectMarkersSent;
    int rfbLastRectBytesSent;
    int rfbCursorBytesSent;
    int rfbCursorUpdatesSent;

    int rfbFramebufferUpdateMessagesSent;
    int rfbRawBytesEquivalent;
    int rfbKeyEventsRcvd;
    int rfbPointerEventsRcvd;
    int compStreamInited;
    u32 zlibCompressLevel;
    int zsActive[4];
    int zsLevel[4];
    int tightCompressLevel;
    int tightQualityLevel;

    int enableLastRectEncoding;   /* client supports LastRect encoding */
    int enableCursorShapeUpdates; /* client supports cursor shape updates */
    int useRichCursorEncoding;    /* rfbEncodingRichCursor is preferred */
    int cursorWasChanged;         /* cursor shape update should be sent */

    //做成双向不循环列表
    struct rfbClientRec *prev;
    struct rfbClientRec *next;

} rfbClientRec, *rfbClientPtr;
rfbScreenInfoPtr rfbserver;
#endif /* RFB_H_ */
