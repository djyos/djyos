
#include  	"stdlib.h"
#include 	"stdio.h"
#include    "djyos.h"
#include    "gdd.h"


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


static struct   {
    u32 text_color;
    u32 bd_color;
    u32 bk_color;
}DrawText_Color_Tbl[6]={

        RGB(255,0,0),RGB(255,0,0),RGB(0,0,0),
        RGB(0,255,0),RGB(0,255,0),RGB(0,0,0),
        RGB(0,160,255),RGB(0,160,255),RGB(0,0,0),
        RGB(255,255,0),RGB(255,255,0),RGB(0,0,0),
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

static  u32 win_proc(MSG *pMsg)
{
    HWND hwnd;
    HDC hdc;
    RECT rc,rc0;
    u32 i,x,y;

    hwnd =pMsg->hwnd;

    switch(pMsg->Code)
    {
        case    MSG_CREATE:
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
                CreateWindow(BUTTON,"关闭",WS_CHILD|BS_NORMAL|WS_BORDER|WS_VISIBLE,RectW(&rc0)-64,RectH(&rc0)-28,60,24,hwnd,ID_CLOSE,NULL);

                y=20;
                x=128+8;
                CreateWindow(BUTTON,"运行",WS_CHILD|BS_HOLD|WS_VISIBLE,x,y+0*28,60,24,hwnd,ID_START_1,NULL);
                CreateWindow(BUTTON,"运行",WS_CHILD|BS_HOLD|WS_VISIBLE,x,y+1*28,60,24,hwnd,ID_START_2,NULL);
                CreateWindow(BUTTON,"运行",WS_CHILD|BS_HOLD|WS_VISIBLE,x,y+2*28,60,24,hwnd,ID_START_3,NULL);
                CreateWindow(BUTTON,"运行",WS_CHILD|BS_HOLD|WS_VISIBLE,x,y+3*28,60,24,hwnd,ID_START_4,NULL);

                y=20;
                x=128+8+64;
                CreateWindow(BUTTON,"清零",WS_CHILD|WS_VISIBLE,x,y+0*28,60,24,hwnd,ID_CLR_1,NULL);
                CreateWindow(BUTTON,"清零",WS_CHILD|WS_VISIBLE,x,y+1*28,60,24,hwnd,ID_CLR_2,NULL);
                CreateWindow(BUTTON,"清零",WS_CHILD|WS_VISIBLE,x,y+2*28,60,24,hwnd,ID_CLR_3,NULL);
                CreateWindow(BUTTON,"清零",WS_CHILD|WS_VISIBLE,x,y+3*28,60,24,hwnd,ID_CLR_4,NULL);

                GDD_CreateTimer(hwnd,0,timer_interval[0],0);
                GDD_CreateTimer(hwnd,1,timer_interval[1],0);
                GDD_CreateTimer(hwnd,2,timer_interval[2],0);
                GDD_CreateTimer(hwnd,3,timer_interval[3],0);
                GDD_CreateTimer(hwnd,7,500,TMR_START);

                break;
                ////
        case    MSG_TIMER:
                {
                    i =pMsg->Param1&0xFF;
                    timer_count[i]++;
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
                                InvalidateWindow(hwnd);
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
                            ////



                    }

                }
                break;
                ////

        case    MSG_PAINT:
                {
                    char wbuf[32];

                    hdc =BeginPaint(hwnd);
                    GetClientRect(hwnd,&rc0);
                    SetFillColor(hdc,RGB(100,100,100));
                    FillRect(hdc,&rc0);

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

                }
                break;
                ////

        default:
                return  DefWindowProc(pMsg);


    }
    return  0;
}

/*============================================================================*/

void    GDD_Demo_Timer(void)
{
    HWND hwnd;
    MSG msg;
    RECT rc;


    //WDD_SleepMS(200);

    GetClientRect(GetDesktopWindow(),&rc);

    //InflateRect(&rc,-20,-20);

    //创建主窗口
    hwnd = CreateWindow(win_proc,"窗口定时器",WS_MAIN_WINDOW,
                        rc.left,rc.top,RectW(&rc),RectH(&rc),NULL,0x0000,NULL);

    ShowWindow(hwnd,TRUE);  //显示窗口

    while(GetMessage(&msg,hwnd))
    {
        DispatchMessage(&msg);
    }
    printf("win_exit.\r\n");

}
