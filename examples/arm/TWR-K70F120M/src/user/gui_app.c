/*
 * gui_app.c

 *
 *  Created on: 2014-10-11
 *      Author: Administrator
 */
#if 1
#include "stdint.h"
#include "cpu_peri.h"
#include "gkernel.h"
#include <string.h>
#include "time.h"
#include "string.h"

// 在显示器底端显示时间
void dis_wnd_bottom_text(struct tagGkWinRsc *parent)
{
	static char text_time[]="2012年07月06日  23:59:59";
	static s64 last_nowtime = 0;
	struct tagRectangle rect;
	char text_buf[32];
	struct tm dtm;
	s64 nowtime;

	rect.left = 0;
	rect.top = 5;
	rect.right = 24*8+15;
	rect.bottom = 16+5;

	// 更新时间
	nowtime = TM_Time(NULL);
	if ((nowtime - last_nowtime) > 0)
	{
		Tm_LocalTime_r(&nowtime,&dtm);
		text_time[0] = '\0';
		// 年
		itoa(dtm.tm_year, text_buf, 10);
		strcat(text_buf, (const char*)"年");
		strcat(text_time, text_buf);
		// 月
		if (dtm.tm_mon<10)
		{
			strcat(text_time, "0");
		}
		itoa(dtm.tm_mon, text_buf, 10);
		strcat(text_buf, "月");
		strcat(text_time, text_buf);
		// 日
		if (dtm.tm_mday<10)
		{
			strcat(text_time, "0");
		}
		itoa(dtm.tm_mday, text_buf, 10);
		strcat(text_buf, "日 ");
		strcat(text_time, text_buf);
		// 时
		if (dtm.tm_hour<10)
		{
			strcat(text_time, "0");
		}
		itoa(dtm.tm_hour, text_buf, 10);
		strcat(text_buf, ":");
		strcat(text_time, text_buf);
		// 分
		if (dtm.tm_min<10)
		{
			strcat(text_time, "0");
		}
		itoa(dtm.tm_min, text_buf, 10);
		strcat(text_buf, ":");
		strcat(text_time, text_buf);
		// 秒
		if (dtm.tm_sec<10)
		{
			strcat(text_time, "0");
		}
		itoa(dtm.tm_sec, text_buf, 10);
		strcat(text_time, text_buf);

		GK_ApiFillRect(parent, &rect, CN_COLOR_RED,0,CN_FILLRECT_MODE_N,0);
		GK_ApiDrawText(parent,NULL,NULL,  5, 5, text_time, strnlen(text_time, 100),
                            CN_COLOR_WHITE,0, CN_TIMEOUT_FOREVER);
	}
}

void gui_app(void)
{
    static struct tagGkWinRsc wnd_body, wnd_bodymsk, wnd_body_selector;
    static struct tagGkWinRsc wnd_banner, wnd_bottom;
    static struct tagGkWinRsc wnd_body_flip, wnd_body_main;
    static char *banner_text = "---- DJYOS ----";
	s32 flip_x = 0, flip_y = 120, flip_inc = 2;
	s32 main_x = 320, main_y = 32, main_inc = -2;
    struct tagGkWinRsc *desktop;
    struct tagInputDeviceMsg *input_msg;
    struct tagRectBitmap bitmap;
    s32 touch_x=0, touch_y=0;
    u8 alpha = 0x30;
    u32 rop = 0x8000;

    desktop = GK_ApiGetDesktop("LCD_TWRK70");

    // banner
    GK_ApiCreateGkwin(desktop, &wnd_banner, 0, 0, 480, 50,CN_COLOR_BLUE,
    		CN_WINBUF_BUF, "banner",  CN_SYS_PF_DISPLAY,0,0,0);
//    GK_ApiDrawText(&wnd_banner, 180, 15,banner_text,
//    		strnlen(banner_text, 100),0x010101,CN_R2_COPYPEN,0);
    GK_ApiDrawText(&wnd_banner,NULL,NULL,  180, 15,banner_text,
    		16,0x010101,CN_R2_COPYPEN,0);

    // body
    GK_ApiCreateGkwin(desktop, &wnd_body, 0, 50, 480, 215,CN_COLOR_RED,
    		CN_WINBUF_BUF, "body",  CN_SYS_PF_DISPLAY,0,0,0);
    GK_ApiCreateGkwin(&wnd_body, &wnd_body_selector, 0, 0, 480, 50,
    		CN_COLOR_RED+CN_COLOR_GREEN, CN_WINBUF_BUF, "body selector",
    		 CN_SYS_PF_DISPLAY,0,0,0);
    GK_ApiLineto(&wnd_body_selector, 120, 0, 120, 50,
    		CN_COLOR_BLACK, CN_R2_COPYPEN, 0);
    GK_ApiLineto(&wnd_body_selector, 240, 0, 240, 50,
    		CN_COLOR_BLACK, CN_R2_COPYPEN, 0);
    GK_ApiLineto(&wnd_body_selector, 360, 0, 360, 50,
    		CN_COLOR_BLACK, CN_R2_COPYPEN, 0);

    GK_ApiDrawText(&wnd_body, NULL,NULL, 120, 80,"Welcome To DJYOS !",
    		strlen("Welcome To DJYOS !"),0x010101,CN_R2_COPYPEN,0);
    GK_ApiDrawText(&wnd_body,NULL,NULL, 200, 140,"Hello World!",
    		strlen("Hello World!"),0x010101,CN_R2_COPYPEN,0);

    GK_ApiSyncShow(CN_TIMEOUT_FOREVER);
    // bottom
    GK_ApiCreateGkwin(desktop, &wnd_bottom, 0, 245, 480, 272,CN_COLOR_BLUE,
    		CN_WINBUF_BUF, "bottom",CN_SYS_PF_DISPLAY,0,0, 0);

    for(;;)
    {
//    	dis_wnd_bottom_text(&wnd_bottom);
    	GK_ApiSyncShow(CN_TIMEOUT_FOREVER);
        Djy_WaitEvttPop(Djy_MyEvttId(),NULL,20*mS);
    }
}

#endif
