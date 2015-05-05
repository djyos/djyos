
#include  	"stdlib.h"
#include    "djyos.h"
#include    "gdd.h"


/*============================================================================*/
#define ID_CLOSE    0x1000
#define ID_BORDER   0x1100
#define ID_BKGND    0x1101
#define ID_COLOR    0x1102

static const struct   {
    const char *text;
    u32 flag;
}DrawText_Cfg_Tbl[]={
        "水平左对齐+垂直顶部对齐"   ,DT_LEFT|DT_TOP,
        "水平左对齐+垂直居中"     ,DT_LEFT|DT_VCENTER,
        "水平左对齐+垂直底部对齐"   ,DT_LEFT|DT_BOTTOM,

        "水平居中+垂直顶部对齐"      ,DT_CENTER|DT_TOP,
        "水平居中+垂直居中"            ,DT_CENTER|DT_VCENTER,
        "水平居中+垂直底部对齐"      ,DT_CENTER|DT_BOTTOM,

        "水平右对齐+垂直顶部对齐"   ,DT_RIGHT|DT_TOP,
        "水平右对齐+垂直居中"      ,DT_RIGHT|DT_VCENTER,
        "水平右对齐+垂直底部对齐"   ,DT_RIGHT|DT_BOTTOM,

	    "多行显示1\r\n水平左对齐+垂直顶部对齐"     ,DT_LEFT|DT_TOP,
	    "多行显示2\r\n水平左对齐+垂直居中"     ,DT_LEFT|DT_VCENTER,
	    "多行显示3\r\n水平左对齐+垂直底部对齐"     ,DT_LEFT|DT_BOTTOM,

	    "多行显示4\r\n水平居中+垂直顶部对齐"     ,DT_CENTER|DT_TOP,
	    "多行显示5\r\n水平居中+垂直居中"     ,DT_CENTER|DT_VCENTER,
	    "多行显示6\r\n水平右对齐+垂直底部对齐"     ,DT_CENTER|DT_BOTTOM,

	    "多行显示7\r\n水平右对齐+垂直顶部对齐"     ,DT_RIGHT|DT_TOP,
	    "多行显示8\r\n水平右对齐+垂直居中"     ,DT_RIGHT|DT_VCENTER,
	    "多行显示9\r\n水平右对齐+垂直底部对齐"     ,DT_RIGHT|DT_BOTTOM,

        NULL,NULL,
};

static const struct   {
    u32 text_color;
    u32 bd_color;
    u32 bk_color;
}DrawText_Color_Tbl[8]={

        RGB(255,255,255),RGB(255,255,255),RGB(200,0,240),
        RGB(1,1,1),RGB(0,0,0),RGB(255,255,255),
        RGB(255,0,0),RGB(255,0,0),RGB(0,0,0),
        RGB(0,255,0),RGB(0,255,0),RGB(0,0,0),
        RGB(0,100,255),RGB(0,100,255),RGB(0,0,0),
        RGB(1,1,1),RGB(0,0,0),RGB(240,200,0),
        RGB(255,255,0),RGB(0,0,0),RGB(0,100,100),
        RGB(255,0,255),RGB(0,0,0),RGB(100,0,100),
};

static  s8 cfg_idx=0;
static  s8 color_idx=0;
static  s8 bBorder=TRUE;
static  s8 bBKGND=TRUE;
static  s8 bColor=TRUE;

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
                cfg_idx =0;
                bBorder=TRUE;
                bBKGND=TRUE;
                bColor=TRUE;

                GetClientRect(hwnd,&rc0);
                CreateWindow(BUTTON,"关闭",WS_CHILD|BS_NORMAL|WS_BORDER|WS_VISIBLE,RectW(&rc0)-64,RectH(&rc0)-28,60,24,hwnd,ID_CLOSE,NULL);

                i=RectH(&rc0)-(3*30);
                CreateWindow(CHECKBOX,"绘制边框",WS_CHILD|CBS_SELECTED|WS_VISIBLE,4,i+0*32,128,24,hwnd,ID_BORDER,NULL);
                CreateWindow(CHECKBOX,"绘制背景",WS_CHILD|CBS_SELECTED|WS_VISIBLE,4,i+1*32,128,24,hwnd,ID_BKGND,NULL);
                CreateWindow(CHECKBOX,"改变颜色",WS_CHILD|CBS_SELECTED|WS_VISIBLE,4,i+2*32,128,24,hwnd,ID_COLOR,NULL);

                GDD_CreateTimer(hwnd,1,3000,TMR_START);
                GDD_CreateTimer(hwnd,2,1000,TMR_START);

                break;
                ////
        case    MSG_TIMER:
                {
                    switch(pMsg->Param1)
                    {
                        case    1:
                                cfg_idx++;
                                if(DrawText_Cfg_Tbl[cfg_idx].text==NULL)
                                {
                                    cfg_idx=0;
                                }
                                InvalidateWindow(hwnd);
                                break;
                                /////
                        case    2:
                                if(bColor!=FALSE)
                                {
                                    color_idx++;
                                    if(color_idx>=8)
                                    {
                                        color_idx=0;
                                    }
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

                    if(id==ID_CLOSE)
                    {
                    	if(event==BTN_UP)
                    	{
                    		PostMessage(hwnd,MSG_CLOSE,0,0);
                    	}
                    	InvalidateWindow(hwnd);
                    }

                    if(id==ID_BORDER)
                    {
                    	if(event==CBN_SELECTED)
                    	{
                    		bBorder =TRUE;
                    	}
                    	if(event==CBN_UNSELECTED)
                    	{
                    		bBorder =FALSE;
                    	}
                    	InvalidateWindow(hwnd);
                    }

                    if(id==ID_BKGND)
                    {
                    	if(event==CBN_SELECTED)
                    	{
                    		bBKGND =TRUE;
                    	}
                    	if(event==CBN_UNSELECTED)
                    	{
                    		bBKGND =FALSE;
                    	}
                    	InvalidateWindow(hwnd);
                    }

                    if(id==ID_COLOR)
                    {
                    	if(event==CBN_SELECTED)
                    	{
                    		bColor =TRUE;
                    	}
                    	if(event==CBN_UNSELECTED)
                    	{
                    		bColor =FALSE;
                    	}
                    	InvalidateWindow(hwnd);
                    }

                }
                break;
                ////

        case    MSG_PAINT:
                {

                    hdc =BeginPaint(hwnd);
                    GetClientRect(hwnd,&rc0);
                    SetFillColor(hdc,RGB(200,200,200));
                    FillRect(hdc,&rc0);


                    SetRect(&rc,4,10,RectW(&rc0)-4*2,60);
                    SetTextColor(hdc,DrawText_Color_Tbl[color_idx].text_color);
                    SetDrawColor(hdc,DrawText_Color_Tbl[color_idx].bd_color);
                    SetFillColor(hdc,DrawText_Color_Tbl[color_idx].bk_color);

                    i=DrawText_Cfg_Tbl[cfg_idx].flag;
                    if(bBorder)
                    {
                        i |= DT_BORDER;
                    }
                    if(bBKGND)
                    {
                        i |= DT_BKGND;
                    }

                    DrawText(hdc,DrawText_Cfg_Tbl[cfg_idx].text,-1,&rc,i);
                   // DrawText(hdc,"DrawText\r\n**绘制字符串**",-1,&rc,DT_BORDER|DT_BKGND|DT_VCENTER|DT_CENTER);

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
void    GDD_Demo_DrawText(void)
{
    HWND hwnd;
    MSG msg;
    RECT rc;


    //WDD_SleepMS(200);

    GetClientRect(GetDesktopWindow(),&rc);

    InflateRect(&rc,-20,-20);

    //创建主窗口
    hwnd = CreateWindow(win_proc,"DrawText(字符绘制)",WS_MAIN_WINDOW,
                        rc.left,rc.top,RectW(&rc),RectH(&rc),NULL,0x0000,NULL);

    ShowWindow(hwnd,TRUE);  //显示窗口

    while(GetMessage(&msg,hwnd))
    {
        DispatchMessage(&msg);
    }

    printf("win_exit.\r\n");

}
