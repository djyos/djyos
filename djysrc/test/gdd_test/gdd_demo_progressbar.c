
#include  	"stdlib.h"
#include 	"stdio.h"
#include    "djyos.h"
#include    "gdd.h"


/*============================================================================*/
#define ID_CLOSE    0x1000
#define ID_SHOWTEXT 0x1100
#define ID_ORG      0x1101


#define	ID_PROGBAR1	0x1111
#define	ID_PROGBAR2	0x1112

static int prog1_val=0;
static int prog1_inc=1;

static int prog2_val=0;
static int prog2_inc=1;

static PROGRESSBAR_DATA pb1;
static PROGRESSBAR_DATA pb2;

static char text_buf[128];

static	int	 text_align_idx=0;
static	const u32 text_align[9]={
		DT_LEFT|DT_TOP,
		DT_LEFT|DT_VCENTER,
		DT_LEFT|DT_BOTTOM,

		DT_CENTER|DT_TOP,
		DT_CENTER|DT_VCENTER,
		DT_CENTER|DT_BOTTOM,

		DT_RIGHT|DT_TOP,
		DT_RIGHT|DT_VCENTER,
		DT_RIGHT|DT_BOTTOM,

};

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

        		text_align_idx =0;

                prog1_val=0;
                prog2_val=0;

                prog1_inc=1;
                prog2_inc=1;

            	pb1.Flag    =PBF_SHOWTEXT|PBF_ORG_LEFT;
            	pb1.Range	=100;
            	pb1.Pos		=prog1_val;
            	pb1.FGColor =RGB(0,160,0);
            	pb1.BGColor =RGB(10,10,10);
            	pb1.TextColor =RGB(240,240,240);
            	pb1.DrawTextFlag =DT_VCENTER|DT_CENTER;


            	pb2.Flag    =PBF_SHOWTEXT|PBF_ORG_BOTTOM;
            	pb2.Range	=100;
            	pb2.Pos		=prog1_val;
            	pb2.FGColor =RGB(200,0,0);
            	pb2.BGColor =RGB(10,10,200);
            	pb2.TextColor =RGB(1,1,1);
            	pb2.DrawTextFlag =DT_VCENTER|DT_CENTER;

                GetClientRect(hwnd,&rc0);
                CreateWindow(BUTTON,"关闭",WS_CHILD|BS_NORMAL|WS_BORDER|WS_VISIBLE,RectW(&rc0)-64,RectH(&rc0)-28,60,24,hwnd,ID_CLOSE,NULL);

                i=RectH(&rc0)-(3*30);
                CreateWindow(CHECKBOX,"显示文字",WS_CHILD|CBS_SELECTED|WS_VISIBLE,4,i+0*32,128,24,hwnd,ID_SHOWTEXT,NULL);
                CreateWindow(CHECKBOX,"增长方向",WS_CHILD|CBS_SELECTED|WS_VISIBLE,4,i+1*32,128,24,hwnd,ID_ORG,NULL);
 //               CreateWindow(CHECKBOX,"改变颜色",WS_CHILD|CBS_SELECTED|WS_VISIBLE,4,i+2*32,128,24,hwnd,ID_COLOR,NULL);

                CreateWindow(PROGRESSBAR,"水平进度条1",WS_CHILD|PBS_HOR|WS_VISIBLE,8,16,128,28,hwnd,ID_PROGBAR1,NULL);
                CreateWindow(PROGRESSBAR,"垂直进度条2",WS_CHILD|PBS_VER|WS_VISIBLE,160,16,28,128,hwnd,ID_PROGBAR2,NULL);

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
									HWND wnd;

									text_align_idx++;
									if(text_align_idx>=9)
									{
										text_align_idx=0;
									}
									pb1.DrawTextFlag =text_align[text_align_idx];
									pb2.DrawTextFlag =text_align[text_align_idx];

									wnd =GetDlgItem(hwnd,ID_PROGBAR1);
									SendMessage(wnd,PBM_SETDATA,(u32)&pb1,NULL);

									wnd =GetDlgItem(hwnd,ID_PROGBAR2);
									SendMessage(wnd,PBM_SETDATA,(u32)&pb2,NULL);
                        		}
                                break;
                                /////
                        case    2:
                                {
                                	HWND wnd;

                                	if(prog1_val<=0)
                                	{
                                		prog1_val =0;
                                		prog1_inc =1;
                                	}
                                	if(prog1_val>=100)
                                	{
                                		prog1_val =100;
                                		prog1_inc =-1;
                                	}
                                	wnd =GetDlgItem(hwnd,ID_PROGBAR1);
                                	pb1.Pos =prog1_val;
                                	SendMessage(wnd,PBM_SETDATA,(u32)&pb1,NULL);

                                   	sprintf(text_buf,"进度:%d...",pb1.Pos);
                                   	SetWindowText(wnd,text_buf,1000);
                                	prog1_val += prog1_inc;

                                	if(prog2_val<=0)
                                	{
                                		prog2_val =0;
                                		prog2_inc =1;
                                	}
                                	if(prog2_val>=100)
                                	{
                                		prog2_val =100;
                                		prog2_inc =-1;
                                	}
                                	wnd =GetDlgItem(hwnd,ID_PROGBAR2);
                                	pb2.Pos =prog2_val;
                                	SendMessage(wnd,PBM_SETDATA,(u32)&pb2,NULL);

                                	sprintf(text_buf,"%d%%",pb2.Pos);
                                	SetWindowText(wnd,text_buf,1000);
                                	prog2_val += prog2_inc;


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

                    switch(id)
                    {
                        case ID_CLOSE:
                            if(event==BTN_UP)
                            {
                                PostMessage(hwnd,MSG_CLOSE,0,0);
                            }
                            break;
                            ////

                        case ID_SHOWTEXT:
                        	if(event==CBN_SELECTED)//复选框选中
                        	{
                        		pb1.Flag |= PBF_SHOWTEXT;
                        		pb2.Flag |= PBF_SHOWTEXT;
                			}

                        	if(event==CBN_UNSELECTED)//复选框未选中
                        	{
                        		pb1.Flag &= ~PBF_SHOWTEXT;
                        	 	pb2.Flag &= ~PBF_SHOWTEXT;
                        	}
                        	break;
                        	////
                        case	ID_ORG:
                        	 if(event==CBN_SELECTED)
                        	 {
                        	    pb1.Flag &= ~PBF_ORG_RIGHT;
                        	    pb2.Flag &= ~PBF_ORG_BOTTOM;
                        	 }

                        	 if(event==CBN_UNSELECTED)
                        	 {
                        	     pb1.Flag |= PBF_ORG_RIGHT;
                        	     pb2.Flag |= PBF_ORG_BOTTOM;
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
void    GDD_Demo_Progressbar(void)
{
    HWND hwnd;
    MSG msg;
    RECT rc;


    //WDD_SleepMS(200);

    GetClientRect(GetDesktopWindow(),&rc);

    InflateRect(&rc,-20,-20);

    //创建主窗口
    hwnd = CreateWindow(win_proc,"进度条控件演示",WS_MAIN_WINDOW,
                        rc.left,rc.top,RectW(&rc),RectH(&rc),NULL,0x0000,NULL);

    ShowWindow(hwnd,TRUE);  //显示窗口

    while(GetMessage(&msg,hwnd))
    {
        DispatchMessage(&msg);
    }


}
