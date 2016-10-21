
#include <stdint.h>
#include    "stdlib.h"
#include    "stdio.h"
#include    "djyos.h"
#include    "gdd.h"
#include  <widget.h>

/*============================================================================*/
#define ID_CLOSE    0x1000

#define ID_START_1  0x1100
#define ID_START_2  0x1101
#define ID_START_3  0x1102
#define ID_START_4  0x1103

#define ID_CLR_1    0x1200
#define ID_CLR_2    0x1201
#define ID_CLR_3    0x1202
#define ID_CLR_4    0x1203

HWND g_ptMainHwnd;
static struct   {
    u32 text_color;
    u32 bd_color;
    u32 bk_color;
}DrawText_Color_Tbl[6]={

         {RGB(255,0,0),RGB(255,0,0),RGB(0,0,0)},
		 {RGB(0,255,0),RGB(0,255,0),RGB(0,0,0)},
		 {RGB(0,160,255),RGB(0,160,255),RGB(0,0,0)},
		 {RGB(255,255,0),RGB(255,255,0),RGB(0,0,0)},
};

static  int cfg_idx=0;
static  int bBorder=TRUE;
static  int bBKGND=TRUE;
static  int bColor=TRUE;

const int timer_interval[]={
        50,
        200,
        500,
        1000,
};

static  int timer_count[8];
static  int timer_run[8];
//创建主窗口
static ptu32_t HmiCreate(struct WindowMsg *pMsg)
{

    HWND hwnd;
    RECT rc0;
    u32 i,x,y;
    hwnd=pMsg->hwnd;

    cfg_idx =0;
    bBorder=TRUE;
    bBKGND=TRUE;
    bColor=TRUE;

    for(i=0;i<8;i++)
    {
        timer_count[i] =0;
        timer_run[i]    =FALSE;
    }
    GetClientRect(hwnd,&rc0);
    CreateButton("关闭",WS_CHILD|BS_NORMAL|WS_BORDER|WS_VISIBLE,RectW(&rc0)-64,RectH(&rc0)-28,60,24,hwnd,ID_CLOSE,NULL,NULL);
    ClientToScreen(hwnd,(POINT *)&rc0,2);
    y=20+rc0.top;
    x=128+8;
    CreateButton("运行",WS_CHILD|BS_HOLD|WS_VISIBLE,x,y+0*28,45,24,hwnd,ID_START_1,NULL,NULL);
    CreateButton("运行",WS_CHILD|BS_HOLD|WS_VISIBLE,x,y+1*28,45,24,hwnd,ID_START_2,NULL,NULL);
    CreateButton("运行",WS_CHILD|BS_HOLD|WS_VISIBLE,x,y+2*28,45,24,hwnd,ID_START_3,NULL,NULL);
    CreateButton("运行",WS_CHILD|BS_HOLD|WS_VISIBLE,x,y+3*28,45,24,hwnd,ID_START_4,NULL,NULL);

    x=128+8+49;
    CreateButton("清零",WS_CHILD|WS_VISIBLE,x,y+0*28,45,24,hwnd,ID_CLR_1,NULL,NULL);
    CreateButton("清零",WS_CHILD|WS_VISIBLE,x,y+1*28,45,24,hwnd,ID_CLR_2,NULL,NULL);
    CreateButton("清零",WS_CHILD|WS_VISIBLE,x,y+2*28,45,24,hwnd,ID_CLR_3,NULL,NULL);
    CreateButton("清零",WS_CHILD|WS_VISIBLE,x,y+3*28,45,24,hwnd,ID_CLR_4,NULL,NULL);

    GDD_CreateTimer(hwnd,0,timer_interval[0],0);
    GDD_CreateTimer(hwnd,1,timer_interval[1],0);
    GDD_CreateTimer(hwnd,2,timer_interval[2],0);
    GDD_CreateTimer(hwnd,3,timer_interval[3],0);
    GDD_CreateTimer(hwnd,7,500,TMR_START);
    return true;
}
//定时器处理函数
static ptu32_t HmiTimer(struct WindowMsg *pMsg)
{
    u32 i;
    HWND hwnd;
    hwnd =pMsg->hwnd;
    i =pMsg->Param1&0xFF;
    timer_count[i]++;
    InvalidateWindow(hwnd,FALSE);

    return true;
}
//子控件通知消息处理函数
static ptu32_t HmiNotify(struct WindowMsg *pMsg)
{
    HWND hwnd;

    hwnd =pMsg->hwnd;
    u16 event,id;
    u32 i;
    event =HI16(pMsg->Param1);
    id =LO16(pMsg->Param1);

    switch(event)
    {
        case BTN_DOWN:  //按钮按下
            if(id>=ID_START_1 && id<=ID_START_4)
            {
                i =id&0xF;
                timer_run[i] =TRUE;
                GDD_ResetTimer(GDD_FindTimer(hwnd,i),timer_interval[i],TMR_START);
            }

            if(id>=ID_CLR_1 && id<=ID_CLR_4)
            {
                i =id&0xF;
                timer_count[i] =0;
                InvalidateWindow(hwnd,TRUE);
            }
            break;
            ////

        case BTN_UP:    //按钮弹起
            if(id==ID_CLOSE)
            {
                PostMessage(hwnd,MSG_CLOSE,0,0);
            }

            if(id>=ID_START_1 && id<=ID_START_4)
            {
                i =id&0xF;
                timer_run[i] =FALSE;
                GDD_ResetTimer(GDD_FindTimer(hwnd,i),timer_interval[i],0);
            }
            break;
        default: break;
    }
    return true;
}
//背景擦出消息处理函数
static ptu32_t HmiErasebkgnd(struct WindowMsg *pMsg)
{
    HWND hwnd;
    HDC hdc;
    RECT rc;
    hwnd =pMsg->hwnd;
    hdc =(HDC)pMsg->Param1;

    GetClientRect(hwnd,&rc);
    SetFillColor(hdc,RGB(100,100,100));
    FillRect(hdc,&rc);

    return true;
}
//绘图消息处理函数
static ptu32_t HmiPaint(struct WindowMsg *pMsg)
{
    HWND hwnd;
    HDC  hdc;
    RECT rc;
    char wbuf[32];
    u32 i;

    hwnd =pMsg->hwnd;
    hdc =BeginPaint(hwnd);
    SetRect(&rc,4,20,128,24);
    for(i=0;i<4;i++)
    {
        SetTextColor(hdc,DrawText_Color_Tbl[i].text_color);
        SetDrawColor(hdc,DrawText_Color_Tbl[i].bd_color);
        SetFillColor(hdc,DrawText_Color_Tbl[i].bk_color);
        sprintf(wbuf," 定时器%d: %d",i+1,timer_count[i]);

        if(timer_run[i]!=FALSE)
        {
            DrawText(hdc,wbuf,-1,&rc,DT_LEFT|DT_VCENTER|DT_BORDER|DT_BKGND);
        }
        else
        {
            if(timer_count[7]&0x01)
            {
                DrawText(hdc,wbuf,-1,&rc,DT_LEFT|DT_VCENTER|DT_BORDER|DT_BKGND);

            }
            else
            {
                DrawText(hdc,wbuf,-1,&rc,DT_LEFT|DT_VCENTER|DT_BKGND);
            }
        }

        OffsetRect(&rc,0,28);

    }
    EndPaint(hwnd,hdc);
    return true;
}
//消息处理函数表
static struct MsgProcTable s_gHmiMsgTimeTable[] =
{
    {MSG_CREATE,HmiCreate},         //主窗口创建消息
    {MSG_TIMER,HmiTimer},           //定时器消息
    {MSG_NOTIFY,HmiNotify},         //子控件发来的通知消息
    {MSG_ERASEBKGND,HmiErasebkgnd}, //背景擦除
    {MSG_PAINT,HmiPaint},           //绘制消息
};

static struct MsgTableLink  s_gHmiMsgLink;
/*============================================================================*/

void    GDD_Demo_Timer(void)
{

    struct WindowMsg msg;
    RECT rc;

    GetClientRect(GetDesktopWindow(),&rc);

    //创建主窗口
    s_gHmiMsgLink.LinkNext = NULL;
    s_gHmiMsgLink.MsgNum = sizeof(s_gHmiMsgTimeTable) / sizeof(struct MsgProcTable);
    s_gHmiMsgLink.myTable = (struct MsgProcTable *)&s_gHmiMsgTimeTable;
    g_ptMainHwnd = CreateWindow("窗口定时器",WS_MAIN_WINDOW,
                                rc.left,rc.top,RectW(&rc),RectH(&rc),
                                NULL,0x0000, CN_WINBUF_PARENT,NULL,&s_gHmiMsgLink);
    SetFocusWindow(g_ptMainHwnd);
    ShowWindow(g_ptMainHwnd,TRUE);  //显示窗口

    while(GetMessage(&msg,g_ptMainHwnd))
    {
        DispatchMessage(&msg);
    }
    printf("win_exit.\r\n");

}
