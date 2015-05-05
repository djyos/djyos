
#include  	"stdlib.h"
#include 	"stdio.h"
#include    "djyos.h"
#include    "gdd.h"


/*============================================================================*/
#define ID_CLOSE    0x1000
#define ID_LEFT 	0x1100
#define ID_RIGHT    0x1101


#define	ID_LISTBOX1	0x1111
#define	ID_LISTBOX2	0x1112

static char text_buf[128];
static HWND hwndLB1=NULL;
static HWND hwndLB2=NULL;

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
                CreateWindow(BUTTON,"关闭",WS_CHILD|BS_NORMAL|WS_BORDER|WS_VISIBLE,RectW(&rc0)-64,RectH(&rc0)-28,60,24,hwnd,ID_CLOSE,NULL);

                SetRect(&rc,4,8,120,100);
                hwndLB1=CreateWindow(LISTBOX,"列表框1",WS_CHILD|WS_BORDER|WS_VISIBLE,rc.left,rc.top,RectW(&rc),RectH(&rc),hwnd,ID_LISTBOX1,NULL);
                OffsetRect(&rc,RectW(&rc)+8,0);
                hwndLB2=CreateWindow(LISTBOX,"列表框2",WS_CHILD|WS_BORDER|WS_VISIBLE,rc.left,rc.top,RectW(&rc),RectH(&rc),hwnd,ID_LISTBOX2,NULL);

                GetWindowRect(hwndLB1,&rc);
                OffsetRect(&rc,0,RectH(&rc)+4);
                ScreenToClient(hwnd,(POINT*)&rc,2);
                CreateWindow(BUTTON,"-->",WS_CHILD|BS_NORMAL|WS_BORDER|WS_VISIBLE,rc.right-50,rc.top,50,20,hwnd,ID_RIGHT,NULL);

                GetWindowRect(hwndLB2,&rc);
                OffsetRect(&rc,0,RectH(&rc)+4);
                ScreenToClient(hwnd,(POINT*)&rc,2);
                CreateWindow(BUTTON,"<--",WS_CHILD|BS_NORMAL|WS_BORDER|WS_VISIBLE,rc.left,rc.top,50,20,hwnd,ID_LEFT,NULL);


                SendMessage(hwndLB1,LBM_ADDSTRING,0,(u32)"ListItem-0");
                SendMessage(hwndLB1,LBM_ADDSTRING,1,(u32)"ListItem-1");
                SendMessage(hwndLB1,LBM_ADDSTRING,2,(u32)"ListItem-2");
                SendMessage(hwndLB1,LBM_ADDSTRING,3,(u32)"ListItem-3");
                SendMessage(hwndLB1,LBM_ADDSTRING,4,(u32)"ListItem-4");
                SendMessage(hwndLB1,LBM_ADDSTRING,5,(u32)"ListItem-5");
                SendMessage(hwndLB1,LBM_ADDSTRING,6,(u32)"ListItem-6");
                SendMessage(hwndLB1,LBM_ADDSTRING,7,(u32)"ListItem-7");
                SendMessage(hwndLB1,LBM_ADDSTRING,8,(u32)"ListItem-8");
                SendMessage(hwndLB1,LBM_ADDSTRING,9,(u32)"ListItem-9");
                SendMessage(hwndLB1,LBM_SETTOPINDEX,0,0);
                SendMessage(hwndLB1,LBM_SETCURSEL,3,0);

                SendMessage(hwndLB2,LBM_ADDSTRING,0,(u32)"ListItem-10");
                SendMessage(hwndLB2,LBM_ADDSTRING,1,(u32)"ListItem-11");
                SendMessage(hwndLB2,LBM_ADDSTRING,2,(u32)"ListItem-12");
                SendMessage(hwndLB2,LBM_ADDSTRING,3,(u32)"ListItem-13");
                SendMessage(hwndLB2,LBM_ADDSTRING,4,(u32)"ListItem-14");
                SendMessage(hwndLB2,LBM_ADDSTRING,5,(u32)"ListItem-15");
                SendMessage(hwndLB2,LBM_ADDSTRING,6,(u32)"ListItem-16");
                SendMessage(hwndLB2,LBM_ADDSTRING,7,(u32)"ListItem-17");
                SendMessage(hwndLB2,LBM_ADDSTRING,8,(u32)"ListItem-18");
                SendMessage(hwndLB2,LBM_ADDSTRING,9,(u32)"ListItem-19");
                SendMessage(hwndLB2,LBM_SETTOPINDEX,0,0);
                SendMessage(hwndLB2,LBM_SETCURSEL,3,0);

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

                    if(event==BTN_UP && id==ID_CLOSE)
                    {
                    	PostMessage(hwnd,MSG_CLOSE,0,0);
                    }////


                    if(event==BTN_UP && id==ID_RIGHT)
                    {
                    	char *buf;
                    	int i;
                    	i =SendMessage(hwndLB1,LBM_GETCURSEL,0,0);

                    	if(i>=0)
                    	{
                    	     buf =(char*)malloc(SendMessage(hwndLB1,LBM_GETTEXTLEN,i,0));
                    	     if(buf!=NULL)
                    	     {
                    	         SendMessage(hwndLB1,LBM_GETTEXT,i,(u32)buf);
                    	         SendMessage(hwndLB1,LBM_DELSTRING,i,0);

                    	         SendMessage(hwndLB2,LBM_ADDSTRING,-1,(u32)buf);
                    	         i=SendMessage(hwndLB2,LBM_GETCOUNT,0,0)-1;
                    	         SendMessage(hwndLB2,LBM_SETTOPINDEX,i-3,0);
                    	         SendMessage(hwndLB2,LBM_SETCURSEL,-1,0);

                    	         free(buf);
                    	      }
                    	}
                    }////

                    if(event==BTN_UP && id==ID_LEFT)
                    {
                    	char *buf;
                        int i;
                        i =SendMessage(hwndLB2,LBM_GETCURSEL,0,0);
                        if(i>=0)
                        {
                    		buf =(char*)malloc(SendMessage(hwndLB2,LBM_GETTEXTLEN,i,0));
                    		if(buf!=NULL)
                    		{
                    			SendMessage(hwndLB2,LBM_GETTEXT,i,(u32)buf);
                    			SendMessage(hwndLB2,LBM_DELSTRING,i,0);

                    			SendMessage(hwndLB1,LBM_ADDSTRING,-1,(u32)buf);
                    			i=SendMessage(hwndLB1,LBM_GETCOUNT,0,0)-1;
                    			SendMessage(hwndLB1,LBM_SETTOPINDEX,i-3,0);
                    			SendMessage(hwndLB1,LBM_SETCURSEL,-1,0);

                    			free(buf);
                    		}
                         }
                    }////

                    if(event==LBN_SELCHANGE && id==ID_LISTBOX1)
                    {
                    	printf("listbox1 sel change.\r\n");
                    }////

                    if(event==LBN_SELCHANGE && id==ID_LISTBOX2)
                    {
                    	printf("listbox2 sel change.\r\n");
                    }////
                }
                break;
                ////

        case    MSG_PAINT:
                {
                    hdc =BeginPaint(hwnd);

                    GetClientRect(hwnd,&rc0);
                    SetFillColor(hdc,RGB(200,200,200));
                    FillRect(hdc,&rc0);

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
void    GDD_Demo_Listbox(void)
{
    HWND hwnd;
    MSG msg;
    RECT rc;


    //WDD_SleepMS(200);

    GetClientRect(GetDesktopWindow(),&rc);

    InflateRect(&rc,-20,-20);

    //创建主窗口
    hwnd = CreateWindow(win_proc,"列表框控件演示",WS_MAIN_WINDOW,
                        rc.left,rc.top,RectW(&rc),RectH(&rc),NULL,0x0000,NULL);

    ShowWindow(hwnd,TRUE);  //显示窗口

    while(GetMessage(&msg,hwnd))
    {
        DispatchMessage(&msg);
    }


}
