
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <gkernel.h>
#include <gdd.h>
#include <widget.h>

static HWND hwnd;
static RECT rc0;
#define ID_CLOSE    0x1000
static ptu32_t HmiCreate(struct WindowMsg *pMsg)
{

    hwnd=pMsg->hwnd;

    GetClientRect(hwnd,&rc0);
   //创建一个按钮。
    CreateButton("关闭",     //按钮名
    WS_CHILD|BS_NORMAL|WS_BORDER|WS_VISIBLE,    //按钮风格
    RectW(&rc0)-64,RectH(&rc0)-28,60,24,    //按钮位置和大小
    hwnd,ID_CLOSE,NULL,NULL);    //按钮所属的父窗口，ID,附加数据
    return true;
}

static ptu32_t HmiNotify(struct WindowMsg *pMsg)
{
    HWND hwnd;
    u16 event,id;
    hwnd =pMsg->hwnd;

    event =HI16(pMsg->Param1);
    id =LO16(pMsg->Param1);

    if(event==BTN_UP && id==ID_CLOSE)
    {
        PostMessage(hwnd,MSG_CLOSE,0,0);
    }
    return true;
}
static ptu32_t HmiPaint(struct WindowMsg *pMsg)
{
    RECT rc0;
    HWND hwnd;
    hwnd=pMsg->hwnd;
    HDC hdc;
    hdc =BeginPaint(hwnd);  //开始绘图并获得绘图上下文(hdc)。
    GetClientRect(hwnd,&rc0);           //获得窗口客户区矩形
    SetFillColor(hdc,RGB(255,255,255)); //设置填充色。
    FillRect(hdc,&rc0);                 //填充矩形。
    SetTextColor(hdc,RGB(255,0,0));     //设置文字颜色。
    TextOut(hdc,10,20,"Hello World!",-1);  //在客户区10,20的位置输出文字。
    EndPaint(hwnd,hdc); //结束绘制并释放绘图上下文。
    return true;

}
//窗口关闭消息处理函数
static ptu32_t HmiClose(struct WindowMsg *pMsg)
{
    HWND hwnd;
    hwnd =pMsg->hwnd;

    printf("win2_close.\r\n");
    PostQuitMessage(hwnd,0);
    return true;
}
//消息处理函数表
static struct MsgProcTable s_gHmiWordTable[] =
{
    {MSG_CREATE,HmiCreate},         //定时器消息
    {MSG_NOTIFY,HmiNotify},         //子控件发来的通知消息
    {MSG_PAINT,HmiPaint},           //绘制消息
    {MSG_CLOSE,HmiClose}      //窗口关闭消息
};

static struct MsgTableLink  s_gHmiMsgLink;

void Hello_Word(void)
{
    HWND hwnd;
    struct WindowMsg msg;
    RECT rc;

    GetClientRect(GetDesktopWindow(),&rc);
    InflateRect(&rc,-7,-7);
    //创建主窗口
    s_gHmiMsgLink.LinkNext = NULL;
    s_gHmiMsgLink.MsgNum = sizeof(s_gHmiWordTable) / sizeof(struct MsgProcTable);
    s_gHmiMsgLink.myTable = (struct MsgProcTable *)&s_gHmiWordTable;
    hwnd = CreateWindow("Hello World",      //窗口标题名。
                            WS_MAIN_WINDOW,     //窗口风格：主窗口。
							rc.left,rc.top,RectW(&rc),RectH(&rc),      //窗口的位置及大小。
                            NULL,0x0000, CN_WINBUF_PARENT,NULL,&s_gHmiMsgLink); //窗口的父窗口，ID,附加数据。);
    SetFocusWindow(hwnd);
    ShowWindow(hwnd,TRUE);  //显示窗口

    while(GetMessage(&msg,hwnd))
    {
        DispatchMessage(&msg);
    }
    printf("win2_exit hello world.\r\n");
}







