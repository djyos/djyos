

#include    "stdlib.h"
#include    "stdio.h"
#include    "djyos.h"
#include    "gdd.h"

/*============================================================================*/
#define ID_START    0x1000
#define ID_SHOWTEXT 0x1100
#define ID_ORG      0x1101


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

                GetClientRect(hwnd,&rc0);
                CreateWindow(BUTTON,"开始",WS_CHILD|BS_NORMAL|WS_BORDER|WS_VISIBLE,(RectW(&rc0)-64)>>1,RectH(&rc0)-28,60,24,hwnd,ID_START,NULL);

                GDD_CreateTimer(hwnd,1,3000,TMR_START);
                GDD_CreateTimer(hwnd,2,100,TMR_START);

                break;
                ////
        case    MSG_TIMER:
                {
                    switch(pMsg->Param1)
                    {
                        case    1:
                                {

                                }
                                break;
                                /////
                        case    2:
                                {

                                    InvalidateWindow(hwnd);
                                }
                                break;
                                /////
                    }
                }
                break;

        case    MSG_NOTIFY:
                {
                    u16 event,id;

                    event =HI16(pMsg->Param1);
                    id =LO16(pMsg->Param1);
                    if(id==ID_START && event==BTN_UP)
                    {
                        SendMessage(hwnd,MSG_CLOSE,0,0);
                    }
                }
                break;
                ////

        case    MSG_PAINT:
                {
                    RECT rc;

                    hdc =BeginPaint(hwnd);

                    GetClientRect(hwnd,&rc0);
                    rc.left =0;
                    rc.top =20;
                    rc.right =rc0.right;
                    rc.bottom =rc0.bottom-20;

                    SetDrawColor(hdc,RGB(255,255,255));
                    DrawRect(hdc,&rc0);

                    InflateRect(&rc0,-1,-1);
                    SetFillColor(hdc,RGB(200,0,200));
                    FillRect(hdc,&rc0);


                    InflateRectEx(&rc,0,0,0,24);
                    SetTextColor(hdc,RGB(255,255,255));
                    DrawText(hdc,
                            "DJYGUI演示程序\r\n"
                            "2015-0409-1131\r\n"
                            "按屏幕'开始'按钮继续...",
                            -1,&rc,DT_VCENTER|DT_CENTER);

                    EndPaint(hwnd,hdc);

                }
                break;
                ////

        default:
                return  DefWindowProc(pMsg);


    }
    return  0;
}

/*========================================================================================*/
void    GDD_Demo_Start(void)
{
    HWND hwnd;
    MSG msg;
    RECT rc;


    //WDD_SleepMS(200);

    GetClientRect(GetDesktopWindow(),&rc);

    InflateRect(&rc,-((RectW(&rc)-200)>>1),-((RectH(&rc)-160)>>1));

    //创建主窗口
    hwnd = CreateWindow(win_proc,"GDD 演示",WS_BORDER,
                        rc.left,rc.top,RectW(&rc),RectH(&rc),NULL,0x0000,NULL);

    ShowWindow(hwnd,TRUE);  //显示窗口

    while(GetMessage(&msg,hwnd))
    {
        DispatchMessage(&msg);
    }


}

/*========================================================================================*/

void    GDD_Demo_Progressbar(void);
void    GDD_Demo_Listbox(void);
void    GDD_Demo_DrawText(void);

void    GDD_Demo(void)
{
    printf("GDD_Demo.\r\n");
    while(GetDesktopWindow()==NULL)
    {
        Djy_EventDelay(100*mS);
    }

    GDD_Demo_Start();
    while(1)
    {
        //  lua_test();
        GDD_Demo_Progressbar();
        GDD_Demo_Listbox();
        GDD_Demo_DrawText();
        //  WDD_Demo_MultiLanguage();
        GDD_Demo_Timer();
        GDD_Demo_MoveWindow();
        GDD_Demo_Keyboard();
    }
}


