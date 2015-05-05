
#include  	"stdlib.h"
#include 	"stdio.h"
#include    "djyos.h"
#include    "gdd.h"


/*============================================================================*/
#define ID_CLOSE    0x1000

#define ID_MOVE_1   0x1100
#define ID_MOVE_2   0x1101

static  HWND hwnd_BTN1=NULL;
static  HWND hwnd_BTN2=NULL;
static  int bMoveMainWin;

static  int x1_inc,y1_inc;
static  int x2_inc,y2_inc;

static  u32 win_proc(MSG *pMsg)
{
    HWND hwnd;
    HDC hdc;
    RECT rc,rc0;
    u32 i;

    hwnd =pMsg->hwnd;

    switch(pMsg->Code)
    {
        case    MSG_CREATE:

                x1_inc =2;
                y1_inc =1;

                x2_inc =2;
                y2_inc =1;

                bMoveMainWin=FALSE;

                GetClientRect(hwnd,&rc0);
                hwnd_BTN1 =CreateWindow(BUTTON,"关闭",WS_CHILD|BS_NORMAL|WS_BORDER|WS_VISIBLE,RectW(&rc0)-64,RectH(&rc0)-28,60,24,hwnd,ID_CLOSE,NULL);
                hwnd_BTN2 =CreateWindow(BUTTON,"关闭",WS_CHILD|BS_NORMAL|WS_BORDER|WS_VISIBLE,4,4,60,24,hwnd,ID_CLOSE,NULL);


                CreateWindow(BUTTON,"移动主窗口",WS_CHILD|BS_HOLD|WS_BORDER|WS_VISIBLE,4,RectH(&rc0)-28,100,24,hwnd,ID_MOVE_1,NULL);


                GDD_CreateTimer(hwnd,0,200,TMR_START);
                GDD_CreateTimer(hwnd,1,150,TMR_START);
                GDD_CreateTimer(hwnd,2,100,TMR_START);

                break;
                ////

        case    MSG_TIMER:
                {

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

                            if(rc.left > 320)
                            {
                                x_inc =-3;
                            }

                            if(rc.left <- 200)
                            {
                                x_inc =3;
                            }

                            if(rc.top > 200)
                            {
                                y_inc =-2;
                            }

                            if(rc.top < -100)
                            {
                                y_inc =2;
                            }

                            rc.left+=x_inc;
                            rc.top+=y_inc;

                            ScreenToClient(GetParent(hwnd),(POINT*)&rc,2);
                            MoveWindow(hwnd,rc.left,rc.top);
                        }
                        break;
                        /////

                    case 1:
                    {

                        GetClientRect(hwnd,&rc0);
                        ClientToScreen(hwnd,(POINT*)&rc0,2);

                        GetWindowRect(hwnd_BTN1,&rc);
                        /*
                        GetWindowRect(hwnd_BTN2,&rc0);
                        if(IsIntersectRect(&rc,&rc0))
                        {
                            x1_inc *=-1;
                            y1_inc *=-1;

                            x2_inc *=-1;
                            y2_inc *=-1;
                        }
                         */

                        if((rc.right) >= (rc0.right))
                        {
                            x1_inc =-3;
                        }

                        if(rc.left <= rc0.left)
                        {
                            x1_inc =3;
                        }

                        if((rc.bottom) >= (rc0.bottom))
                        {
                            y1_inc =-2;
                        }

                        if(rc.top <= rc0.top)
                        {
                            y1_inc =2;
                        }

                        rc.left +=x1_inc;
                        rc.top +=y1_inc;

                        ScreenToClient(hwnd,(POINT*)&rc,2);
                        MoveWindow(hwnd_BTN1,rc.left,rc.top);
                    }

                    case 2:
                    {

                        GetClientRect(hwnd,&rc0);
                        ClientToScreen(hwnd,(POINT*)&rc0,2);

                        GetWindowRect(hwnd_BTN2,&rc);
                        /*
                        GetWindowRect(hwnd_BTN1,&rc0);
                        if(IsIntersectRect(&rc,&rc0))
                        {
                            x1_inc *=-1;
                            y1_inc *=-1;

                            x2_inc *=-1;
                            y2_inc *=-1;
                        }
                        */

                        if((rc.right) >= (rc0.right))
                        {
                            x2_inc =-2;
                        }

                        if(rc.left <= rc0.left)
                        {
                            x2_inc =2;
                        }

                        if((rc.bottom) >= (rc0.bottom))
                        {
                            y2_inc =-1;
                        }

                        if(rc.top <= rc0.top)
                        {
                            y2_inc =1;
                        }

                        rc.left +=x2_inc;
                        rc.top +=y2_inc;

                        ScreenToClient(hwnd,(POINT*)&rc,2);
                        MoveWindow(hwnd_BTN2,rc.left,rc.top);
                    }

                    break;
                    ////

                    default:

                        break;

                    }

                    InvalidateWindow(hwnd);
                }
                break;

        case    MSG_NOTIFY:
                {
                    u16 event,id;

                    event =HI16(pMsg->Param1);
                    id =LO16(pMsg->Param1);

                    switch(event)
                    {

                        case BTN_DOWN:  //按钮按下
                            if(id==ID_MOVE_1)
                            {
                                bMoveMainWin=TRUE;
                            }
                            break;
                            ////

                        case BTN_UP:    //按钮弹起
                            if(id==ID_CLOSE)
                            {
                                PostMessage(hwnd,MSG_CLOSE,0,0);
                            }

                            if(id==ID_MOVE_1)
                            {
                                bMoveMainWin=FALSE;
                            }
                            break;
                            ////

                        default:
                            break;

                    }


                }
                break;
                ////

        case    MSG_PAINT:
                {
                    char wbuf[32];

                    hdc =BeginPaint(hwnd);

                    GetClientRect(hwnd,&rc0);


                    SetTextColor(hdc,RGB(0,240,0));
                    SetFillColor(hdc,RGB(40,40,40));
                    SetDrawColor(hdc,RGB(0,240,0));


                    GetWindowRect(hwnd,&rc);
                    sprintf(wbuf,"当前主窗口位置: %d,%d",rc.left,rc.top);
                    DrawText(hdc,wbuf,-1,&rc0,DT_LEFT|DT_VCENTER|DT_BORDER|DT_BKGND);

                    EndPaint(hwnd,hdc);

                }
                break;
                ////

        default:
                return  DefWindowProc(pMsg);


    }
    return  0;
}

/*============================================================================*/

void    GDD_Demo_MoveWindow(void)
{
    HWND hwnd;
    MSG msg;
    RECT rc;

    //WDD_SleepMS(200);

    GetClientRect(GetDesktopWindow(),&rc);

    //InflateRect(&rc,-40,-40);


    //创建主窗口
    hwnd = CreateWindow(win_proc,"窗口/控件移动",WS_MAIN_WINDOW,
                        rc.left,rc.top,RectW(&rc),RectH(&rc),NULL,0x0000,NULL);

    ShowWindow(hwnd,TRUE);  //显示窗口

    while(GetMessage(&msg,hwnd))
    {
        DispatchMessage(&msg);
    }
    printf("win_exit.\r\n");

}
