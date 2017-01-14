
#include <stdint.h>
#include    "stdlib.h"
#include    "stdio.h"
#include    "djyos.h"
#include    "gdd.h"
#include <gdd_widget.h>


/*============================================================================*/
#define ID_CLOSE    0x1000
#define ID_MOVE_1   0x1100
#define ID_MOVE_2   0x1101

static  HWND hwnd_BTN1=NULL;
static  HWND hwnd_BTN2=NULL;
static  int bMoveMainWin;

static  int x1_inc,y1_inc;
static  int x2_inc,y2_inc;

//开始创建窗口消息处理函数
static ptu32_t HmiCreateMove(struct WindowMsg  *pMsg)
{
    HWND hwnd;
    RECT rc;
    hwnd =pMsg->hwnd;

    x1_inc =2;
    y1_inc =1;
    x2_inc =2;
    y2_inc =1;
    bMoveMainWin=FALSE;
    GetClientRect(hwnd,&rc);
    hwnd_BTN1 =CreateButton("关闭",WS_CHILD|WS_BORDER|WS_VISIBLE,RectW(&rc)-64,RectH(&rc)-28,60,24,hwnd,ID_CLOSE,NULL,NULL);
    hwnd_BTN2 =CreateButton("关闭",WS_CHILD|WS_BORDER|WS_VISIBLE,10,34,100,50,hwnd,ID_CLOSE,NULL,NULL);

    CreateButton("移动主窗口",WS_CHILD|BS_HOLD|WS_VISIBLE|WS_BORDER,4,RectH(&rc)-28,100,54,hwnd,ID_MOVE_1,NULL,NULL);
    GDD_CreateTimer(hwnd,0,200,TMR_START);
    GDD_CreateTimer(hwnd,1,150,TMR_START);
    GDD_CreateTimer(hwnd,2,100,TMR_START);

    return true;
}

//定时器消息处理函数
static ptu32_t HmiTimerMove(struct WindowMsg  *pMsg)
{
    HWND hwnd;
    RECT rc,rc0;
    hwnd =pMsg->hwnd;
        switch(pMsg->Param1)
        {
        case 0:
            {
                static int x_inc=2;
                static int y_inc=1;

                if(bMoveMainWin==FALSE)
                {
                    break;
                }

                GetWindowRect(hwnd,&rc);

                if(rc.left > 200)
                {
                    x_inc =-3;
                }

                if(rc.left <- 100)
                {
                    x_inc =3;
                }

                if(rc.top > 200)
                {
                    y_inc =-2;
                }

                if(rc.top < -150)
                {
                    y_inc =2;
                }

                rc.left+=x_inc;
                rc.top+=y_inc;
                MoveWindow(hwnd,rc.left,rc.top);//相对父窗口客户区的位置
            }
            break;
        case 1:
        {

            GetClientRect(hwnd,&rc0);
            ClientToScreen(hwnd,(POINT*)&rc0,2);

            GetWindowRect(hwnd_BTN1,&rc);

            if((rc.right) >= (rc0.right))
            {
                x1_inc =-4;
            }

            if(rc.left <= rc0.left)
            {
                x1_inc =4;
            }

            if((rc.bottom) >= (rc0.bottom))
            {
                y1_inc =-5;
            }

            if(rc.top <= rc0.top)
            {
                y1_inc =5;
            }

            rc.left +=x1_inc;
            rc.top +=y1_inc;
//            ScreenToWindow(hwnd,(POINT*)&rc,2);
            ScreenToClient(hwnd,(POINT*)&rc,2);
            MoveWindow(hwnd_BTN1,rc.left,rc.top);//相对父窗口客户区的位置
        }break;

        case 2:
        {
            GetClientRect(hwnd,&rc0);
            ClientToScreen(hwnd,(POINT*)&rc0,2);//相对屏幕坐标

            GetWindowRect(hwnd_BTN2,&rc);//相对屏幕坐标
            if((rc.right) >= (rc0.right))
            {
                x2_inc =-7;
            }

            if(rc.left <= rc0.left)
            {
                x2_inc =7;
            }

            if((rc.bottom) >= (rc0.bottom))
            {
                y2_inc =-4;
            }

            if(rc.top <= rc0.top)
            {
                y2_inc =4;
            }

            rc.left +=x2_inc;
            rc.top +=y2_inc;
//            ScreenToWindow(hwnd,(POINT*)&rc,2);
            ScreenToClient(hwnd,(POINT*)&rc,2);//屏幕坐标转换为客户区
            MoveWindow(hwnd_BTN2,rc.left,rc.top);//相对父窗口客户区的位置
        }
        break;
        default:
            break;
        }

        InvalidateWindow(hwnd,TRUE);

    return true;
}
//子控件发来的消息处理函数
static ptu32_t HmiNotifyMove(struct WindowMsg  *pMsg)
{
    HWND hwnd;
    u16 event,id;

    hwnd =pMsg->hwnd;
    event =HI16(pMsg->Param1);
    id =LO16(pMsg->Param1);
    switch(event)
    {
        case MSG_BTN_DOWN:  //按钮按下
            if(id==ID_MOVE_1)
            {
                bMoveMainWin=TRUE;
            }
            break;
        case MSG_BTN_UP:    //按钮弹起
            if(id==ID_CLOSE)
            {
                PostMessage(hwnd,MSG_CLOSE,0,0);
            }

            if(id==ID_MOVE_1)
            {
                bMoveMainWin=FALSE;
            }
            break;
        default:
            break;
    }
    return true;
}
//绘制消息处函数
static ptu32_t HmiPaintMove(struct WindowMsg  *pMsg)
{
    HWND hwnd;
    HDC  hdc;
    RECT rc,rc0;
    char wbuf[32];

    hwnd =pMsg->hwnd;
    hdc =BeginPaint(hwnd);

    GetClientRect(hwnd,&rc0);
    SetTextColor(hdc,RGB(0,240,0));
    SetFillColor(hdc,RGB(40,40,40));
    SetDrawColor(hdc,RGB(0,240,0));

    GetWindowRect(hwnd,&rc);
    sprintf(wbuf,"当前主窗口位置: %d,%d",rc.left,rc.top);
    DrawText(hdc,wbuf,-1,&rc0,DT_LEFT|DT_VCENTER|DT_BORDER|DT_BKGND);
    EndPaint(hwnd,hdc);
    return true;
}

//消息处理函数表
static struct MsgProcTable s_gHmiMsgMoveTable[] =
{
    {MSG_CREATE,HmiCreateMove},         //主窗口创建消息
    {MSG_TIMER,HmiTimerMove},           //定时器消息
    {MSG_NOTIFY,HmiNotifyMove},         //子控件发来的通知消息
    {MSG_PAINT,HmiPaintMove},           //绘制消息
};

static struct MsgTableLink  s_gHmiMsgLink;

/*============================================================================*/

void    GDD_Demo_MoveWindow(void)
{
    HWND hwnd;
    struct WindowMsg  msg;
    RECT rc;

    GetClientRect(GetDesktopWindow(),&rc);

    //创建主窗口
    s_gHmiMsgLink.LinkNext = NULL;
    s_gHmiMsgLink.MsgNum = sizeof(s_gHmiMsgMoveTable) / sizeof(struct MsgProcTable);
    s_gHmiMsgLink.myTable = (struct MsgProcTable *)&s_gHmiMsgMoveTable;
    hwnd = CreateWindow("窗口/控件移动",WS_MAIN_WINDOW,
                              rc.left,rc.top,RectW(&rc),RectH(&rc),
                              NULL,0x0000, CN_WINBUF_PARENT,NULL,&s_gHmiMsgLink);
    SetFocusWindow(hwnd);
   
    SetWindowShow(hwnd);

    while(GetMessage(&msg,hwnd))
    {
        DispatchMessage(&msg);
    }

    printf("win_exit gdd_move.\r\n");

}
