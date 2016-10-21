#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <gkernel.h>
#include <gdd.h>
#include <widget.h>

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
HWND g_ptMainHwnd;
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

static void  zigbee_ShowString(const char *buf)
{
    memcpy(ZigBeeTextBuf,buf,256);
    InvalidateWindow(hwndZigBee,TRUE);
}

/*============================================================================*/
static ptu32_t HmiCreate(struct WindowMsg *pMsg)
{
    HWND hwnd;
    u32 x,y;
    RECT rc0;
    hwnd =pMsg->hwnd;


	timer_500ms_count=0;
	memset(ZigBeeTextBuf,0,sizeof(ZigBeeTextBuf));
	memset(ZigBeeTimeBuf,0,sizeof(ZigBeeTimeBuf));
	hwndZigBee =hwnd;
	zigbee_ShowString("Zigbee Text");
	GetClientRect(hwnd,&rc0);
	ClientToScreen(hwnd,(POINT*)&rc0,2);
	CreateButton("关闭",WS_BORDER|WS_DLGFRAME|WS_CHILD|BS_SIMPLE|WS_VISIBLE,rc0.left+4,rc0.top+100,56,56,hwnd,ID_CLOSE,NULL,NULL);

	SetRect(&rcZigBeeString,4,4,RectW(&rc0)-4*2,60);

	x=4;
	y=RectH(&rc0)-70;
	SetRect(&rcGroupBox_LED,x,y,100,66);
	SetRect(&rcGroupBox_CTR,x+100+4,y,100,66);

	x=rcGroupBox_LED.left+12;
	y=rcGroupBox_LED.top+18;
	CreateButton("LED1",WS_BORDER|WS_CHILD|BS_HOLD|WS_VISIBLE,x,y+1*24+8,72,20,hwnd,ID_LED1,NULL,NULL);
	CreateButton("LED2",WS_BORDER|WS_CHILD|BS_HOLD|WS_VISIBLE,x,y+2*24+8,72,20,hwnd,ID_LED2,NULL,NULL);

	x=rcGroupBox_CTR.left+12;
	y=rcGroupBox_CTR.top+18;
	CreateButton("组网",WS_BORDER|WS_CHILD|WS_VISIBLE,x,y+1*24+8,72,20,hwnd,ID_NET_ON,NULL,NULL);
	CreateButton("断开",WS_BORDER|WS_CHILD|WS_VISIBLE,x,y+2*24+8,72,20,hwnd,ID_NET_OFF,NULL,NULL);

	CreateButton("发送",WS_BORDER|WS_CHILD|BS_SIMPLE|WS_VISIBLE,RectW(&rc0)-60,rcGroupBox_CTR.top-(24+2),56,24,hwnd,ID_SEND,NULL,NULL);

	GDD_CreateTimer(hwnd,0,500,TMR_START);

	return true;
}

//定时器处理函数
static ptu32_t HmiTimer(struct WindowMsg *pMsg)
{
    HWND hwnd;
    hwnd =pMsg->hwnd;
	timer_500ms_count++;
	sprintf(ZigBeeTimeBuf,"TIME:%06d",timer_500ms_count);
	InvalidateWindow(hwnd,FALSE);
    return true;
}
//控控件通知消息处理函数
static ptu32_t HmiNotify(struct WindowMsg *pMsg)
{
    HWND hwnd;
    u16 event,id;
    hwnd =pMsg->hwnd;

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
	}
    return true;
}

/*
//背景擦出消息处理函数
static ptu32_t HmiErasebkgnd(struct WindowMsg *pMsg)
{
    HWND hwnd;
    HDC  hdc;
    RECT rc;
    hwnd =pMsg->hwnd;
	hdc =pMsg->Param1;
	GetClientRect(hwnd,&rc);
	SetFillColor(hdc,RGB(150,150,150));
	FillRect(hdc,&rc);
    return true;
}
*/

static ptu32_t HmiPaint(struct WindowMsg *pMsg)
{
    HWND hwnd;
    HDC  hdc;
    RECT rc;

    hwnd =pMsg->hwnd;
    hdc = BeginPaint(hwnd);
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
    return true;
}


//消息处理函数表
static struct MsgProcTable s_gHmiMsgZigbeeTable[] =
{
    {MSG_CREATE,HmiCreate},         //主窗口创建消息
    {MSG_TIMER,HmiTimer},           //定时器消息
    {MSG_NOTIFY,HmiNotify},         //子控件发来的通知消息
    {MSG_PAINT,HmiPaint},           //绘制消息
};

static struct MsgTableLink  s_gHmiMsgLink;
/*============================================================================*/
void    GDD_Demo_ZigBee(void)
{
    struct WindowMsg msg;
    RECT rc;

    GetClientRect(GetDesktopWindow(),&rc);

    InflateRect(&rc,-7,-7);

    //创建主窗口
	s_gHmiMsgLink.LinkNext = NULL;
	s_gHmiMsgLink.MsgNum = sizeof(s_gHmiMsgZigbeeTable) / sizeof(struct MsgProcTable);
	s_gHmiMsgLink.myTable =(struct MsgProcTable *) &s_gHmiMsgZigbeeTable;
	g_ptMainHwnd = CreateWindow("ZigBee测试",WS_MAIN_WINDOW,
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
