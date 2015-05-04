#include "stdint.h"
#include  	"stdlib.h"
#include 	"stdio.h"
#include    "djyos.h"
#include    "gdd.h"


/*============================================================================*/
/*
static  WCHAR *ansi_to_unicode(WCHAR *dst,const char *src,int count)
{
    int i;
    char ch;

    i=0;
    while(1)
    {
        ch =*src++;
        dst[i++] =ch;

        if(ch=='\0') break;
        if(i>=count)
        {
            dst[count-1] = 0;
            break;
        }
    }
    return dst;
}

static  char *unicode_to_ansi(char *dst,const WCHAR *src,int count)
{
    int i;
    WCHAR ch;

    i=0;
    while(1)
    {
        ch =*src++;
        dst[i++] =ch;

        if(ch=='\0') break;
        if(i>=count)
        {
            dst[count-1] = 0;
            break;
        }
    }
    return dst;
}
*/

/*============================================================================*/

#define ID_CLOSE    0x1000
#define ID_SEND     0x1001

#define ID_LED1 0x1100
#define ID_LED2 0x1101

#define ID_NET_ON   0x1110
#define ID_NET_OFF  0x1111

static  HWND hwndZigBee;
static  RECT rcZigBeeString;
static  RECT rcGroupBox_LED;
static  RECT rcGroupBox_CTR;
static  char ZigBeeTextBuf[256];
static  char ZigBeeTimeBuf[64];
static  int timer_500ms_count=0;

static  void zigbee_NET_ON(void)
{

}

static  void zigbee_NET_OFF(void)
{

}

static  bool_t zigbee_is_ok(void)
{
    ////zigbee 是否已经连接

    return false;
}

static  void zigbee_LED1_ON(void)
{

}

static  void zigbee_LED1_OFF(void)
{

}

static  void zigbee_LED2_ON(void)
{

}

static  void zigbee_LED2_OFF(void)
{

}

static  void zigbee_send_string(const char *str)
{

}

void  zigbee_ShowString(const char *buf)
{
    memcpy(ZigBeeTextBuf,buf,256);
    InvalidateWindow(hwndZigBee);
}

/*============================================================================*/

static  u32 win_proc(MSG *pMsg)
{
    HWND hwnd;
    HDC hdc;
    RECT rc,rc0;
    u32 x,y;

    hwnd =pMsg->hwnd;

    switch(pMsg->Code)
    {
        case    MSG_CREATE:
                timer_500ms_count=0;
                memset(ZigBeeTextBuf,0,sizeof(ZigBeeTextBuf));
                memset(ZigBeeTimeBuf,0,sizeof(ZigBeeTimeBuf));
                zigbee_ShowString("Zigbee Text");
                hwndZigBee =hwnd;
                GetClientRect(hwnd,&rc0);
                SetFocusWindow(hwnd);
                CreateWindow(BUTTON,"关闭",WS_BORDER|WS_DLGFRAME|WS_CHILD|BS_SIMPLE|WS_VISIBLE,RectW(&rc0)-60,RectH(&rc0)-60,56,56,hwnd,ID_CLOSE,NULL);

                SetRect(&rcZigBeeString,4,4,RectW(&rc0)-4*2,60);

                x=4;
                y=RectH(&rc0)-70;
                SetRect(&rcGroupBox_LED,x,y,100,66);
                SetRect(&rcGroupBox_CTR,x+100+4,y,100,66);

                x=rcGroupBox_LED.left+12;
                y=rcGroupBox_LED.top+18;
                CreateWindow(BUTTON,"LED1",WS_BORDER|WS_CHILD|BS_HOLD|WS_VISIBLE,x,y+0*24,72,20,hwnd,ID_LED1,NULL);
                CreateWindow(BUTTON,"LED2",WS_BORDER|WS_CHILD|BS_HOLD|WS_VISIBLE,x,y+1*24,72,20,hwnd,ID_LED2,NULL);

                x=rcGroupBox_CTR.left+12;
                y=rcGroupBox_CTR.top+18;
                CreateWindow(BUTTON,"组网",WS_BORDER|WS_CHILD|WS_VISIBLE,x,y+0*24,72,20,hwnd,ID_NET_ON,NULL);
                CreateWindow(BUTTON,"断开",WS_BORDER|WS_CHILD|WS_VISIBLE,x,y+1*24,72,20,hwnd,ID_NET_OFF,NULL);

                CreateWindow(BUTTON,"发送",WS_BORDER|WS_CHILD|BS_SIMPLE|WS_VISIBLE,RectW(&rc0)-60,rcGroupBox_CTR.top-(24+2),56,24,hwnd,ID_SEND,NULL);

                GDD_CreateTimer(hwnd,0,500,TMR_START);

                break;
                ////
        case MSG_KEY_DOWN:
         		break;
         		//////

        case MSG_KEY_UP:
        {
        		u16 key_val;
                hdc =BeginPaint(hwnd);
        		key_val =LO16(pMsg->Param1);
        		sprintf(ZigBeeTextBuf,"KeyVal:%04XH",key_val);
                DrawText(hdc,ZigBeeTextBuf,-1,&rc,DT_LEFT|DT_VCENTER|DT_BORDER|DT_BKGND);
                EndPaint(hwnd,hdc);
//        		InvalidateWindow(hwnd);
        }
         		break;
         		//////

        case    MSG_TIMER:
                {
                    timer_500ms_count++;

                    sprintf(ZigBeeTimeBuf,"TIME:%06d",timer_500ms_count);

                    /*
                    i=SendMessage(GetDesktopWindow(),MSG_GET_POS,timer_500ms_count,0);
                    printf("i=%08XH.\r\n",i);
                    */
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
                            if(id==ID_LED1)
                            {
                                zigbee_LED1_ON();
                            }

                            if(id==ID_LED2)
                            {
                                zigbee_LED2_ON();
                            }

                            if(id==ID_NET_ON)
                            {
                                zigbee_NET_ON();
                            }

                            if(id==ID_NET_OFF)
                            {
                                zigbee_NET_OFF();
                            }

                            if(id==ID_SEND)
                            {
                                zigbee_send_string(ZigBeeTextBuf);
                            }
                            break;
                            ////

                        case BTN_UP:    //按钮弹起
                            if(id==ID_CLOSE)
                            {
                                PostMessage(hwnd,MSG_CLOSE,0,0);
                            }

                            if(id==ID_LED1)
                            {
                                zigbee_LED1_OFF();
                            }

                            if(id==ID_LED2)
                            {
                                zigbee_LED2_OFF();
                            }


                            break;
                            ////



                    }

                }
                break;
                ////

        case    MSG_PAINT:
                {

                    hdc =BeginPaint(hwnd);
                    GetClientRect(hwnd,&rc0);
                    SetFillColor(hdc,RGB(150,150,150));
                    FillRect(hdc,&rc0);

                    SetTextColor(hdc,RGB(0,0,128));
                    SetDrawColor(hdc,RGB(100,100,100));
                    DrawGroupBox(hdc,&rcZigBeeString,"信息接收区");

                    ////
                    SetRect(&rc,
                            rcGroupBox_LED.left,
                            rcGroupBox_LED.top-(24+2),
                            RectW(&rcGroupBox_LED),
                            24);

                    if(zigbee_is_ok())
                    {
                        SetTextColor(hdc,RGB(0,255,0));
                        SetDrawColor(hdc,RGB(0,200,0));
                        SetFillColor(hdc,RGB(0,128,0));
                        DrawText(hdc,"连接成功",-1,&rc,DT_CENTER|DT_VCENTER|DT_BORDER|DT_BKGND);

                    }
                    else
                    {
                        SetTextColor(hdc,RGB(255,0,0));
                        SetDrawColor(hdc,RGB(200,0,0));
                        SetFillColor(hdc,RGB(100,0,0));
                        if(timer_500ms_count&0x01)
                        {
                            DrawText(hdc,"未连接",-1,&rc,DT_CENTER|DT_VCENTER|DT_BORDER|DT_BKGND);

                        }
                        else
                        {
                            DrawText(hdc," ",-1,&rc,DT_CENTER|DT_VCENTER|DT_BORDER|DT_BKGND);
                        }

                    }

                    ////
                    SetRect(&rc,
                            rcGroupBox_CTR.left,
                            rcGroupBox_CTR.top-(24+2),
                            RectW(&rcGroupBox_CTR),
                            24);
                    SetTextColor(hdc,RGB(0,255,255));
                    SetDrawColor(hdc,RGB(0,200,200));
                    SetFillColor(hdc,RGB(0,80,80));
                    DrawText(hdc,ZigBeeTimeBuf,-1,&rc,DT_CENTER|DT_VCENTER|DT_BORDER|DT_BKGND);

                    ////
                    SetTextColor(hdc,RGB(0,255,0));
                    SetDrawColor(hdc,RGB(0,200,0));
                    SetFillColor(hdc,RGB(0,80,0));

                    CopyRect(&rc,&rcZigBeeString);
                    InflateRectEx(&rc,-4,-20,-4,-4);
                    DrawText(hdc,ZigBeeTextBuf,-1,&rc,DT_LEFT|DT_VCENTER|DT_BORDER|DT_BKGND);

                    SetTextColor(hdc,RGB(0,0,128));
                    SetDrawColor(hdc,RGB(80,80,80));
                    DrawGroupBox(hdc,&rcGroupBox_LED,"LED控制");
                    DrawGroupBox(hdc,&rcGroupBox_CTR,"网络控制");

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

void    GDD_Demo_ZigBee(void)
{
    HWND hwnd;
    MSG msg;
    RECT rc;

    GetClientRect(GetDesktopWindow(),&rc);

    InflateRect(&rc,-20,-20);

    //创建主窗口
    hwnd = CreateWindow(win_proc,"ZigBee测试",WS_MAIN_WINDOW,
                        rc.left,rc.top,RectW(&rc),RectH(&rc),NULL,0x0000,NULL);
    //hwnd = CreateWindow(win_proc,L"ZigBee测试",0,rc.x,rc.y,rc.w,rc.h,NULL,0x0000,NULL);

    ShowWindow(hwnd,TRUE);  //显示窗口

    while(GetMessage(&msg,hwnd))
    {
        DispatchMessage(&msg);
    }
    printf("win_exit.\r\n");

}
