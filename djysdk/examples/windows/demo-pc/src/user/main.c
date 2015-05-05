#include "stdint.h"
#include "stdio.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stddev.h"
#include "assert.h"
#include "systime.h"
#include "gkernel.h"
#include "time.h"
#include "djyos.h"
#include "timer.h"

#include "d:\djysdk\djysrc\test\testbmp\gray1_64x48.c"
#include "d:\djysdk\djysrc\test\testbmp\gray2_64x48.c"
#include "d:\djysdk\djysrc\test\testbmp\gray4_64x48.c"
#include "d:\djysdk\djysrc\test\testbmp\gray8_64x48.c"
#include "d:\djysdk\djysrc\test\testbmp\rgb565_240x180.c"
#include "d:\djysdk\djysrc\test\testbmp\index_4.c"
#include "d:\djysdk\djysrc\test\testbmp\tulips.c"
#include "d:\djysdk\djysrc\test\testbmp\Jellyfish.c"

void run_module_init(void);

#if (1)
// ÔÚÏÔÊ¾Æ÷Ö÷ÌåÉÏÏÔÊ¾ÄÚÈÝ
void dis_wnd_body_text(struct tagGkWinRsc *parent, s32 area)
{
    struct tagInputDeviceMsg input_msg;
    s32 touch_x=0, touch_y=0;

    switch (area)
    {
    case 1:
//        input_msg = (struct tagInputDeviceMsg *)Djy_GetEventPara(NULL);
        Stddev_ReadDefaultMsg(&input_msg,0);
        if(input_msg.input_type == EN_STDIN_SINGLE_TOUCH)
        {
            touch_x = input_msg.input_data.tagSingleTouchMsg.x;
            touch_y = input_msg.input_data.tagSingleTouchMsg.y;
            GK_ApiDrawCircle(parent, touch_x, touch_y, 10,
                    CN_COLOR_WHITE, CN_R2_COPYPEN, CN_TIMEOUT_FOREVER);
        }
        break;
    case 2:
        break;
    case 3:
        break;
    default:
        break;
    }
}

// ÔÚÏÔÊ¾Æ÷µ×¶ËÏÔÊ¾Ê±¼ä
void dis_wnd_bottom_text(struct tagGkWinRsc *parent)
{
    static char text_time[]="2012Äê07ÔÂ06ÈÕ  23:59:59";
    static s64 last_nowtime = 0;
    struct tagRectangle rect;
    char text_buf[32];
    struct tm dtm;
    s64 nowtime;

    rect.left = 0;
    rect.top = 5;
    rect.right = 24*8+15;
    rect.bottom = 16+5;

    // ¸üÐÂÊ±¼ä
    nowtime = TM_Time(NULL);
    if ((nowtime - last_nowtime) > 0)
    {
        Tm_LocalTime_r(&nowtime,&dtm);
        text_time[0] = '\0';
        // Äê
        itoa(dtm.tm_year, text_buf, 10);
        strcat(text_buf, (const char*)"Äê");
        strcat(text_time, text_buf);
        // ÔÂ
        if (dtm.tm_mon<10)
        {
            strcat(text_time, "0");
        }
        itoa(dtm.tm_mon, text_buf, 10);
        strcat(text_buf, "ÔÂ");
        strcat(text_time, text_buf);
        // ÈÕ
        if (dtm.tm_mday<10)
        {
            strcat(text_time, "0");
        }
        itoa(dtm.tm_mday, text_buf, 10);
        strcat(text_buf, "ÈÕ ");
        strcat(text_time, text_buf);
        // Ê±
        if (dtm.tm_hour<10)
        {
            strcat(text_time, "0");
        }
        itoa(dtm.tm_hour, text_buf, 10);
        strcat(text_buf, ":");
        strcat(text_time, text_buf);
        // ·Ö
        if (dtm.tm_min<10)
        {
            strcat(text_time, "0");
        }
        itoa(dtm.tm_min, text_buf, 10);
        strcat(text_buf, ":");
        strcat(text_time, text_buf);
        // Ãë
        if (dtm.tm_sec<10)
        {
            strcat(text_time, "0");
        }
        itoa(dtm.tm_sec, text_buf, 10);
        strcat(text_time, text_buf);

        GK_ApiFillRect(parent, &rect, CN_COLOR_GREEN,0,CN_FILLRECT_MODE_N,0);
        GK_ApiDrawText(parent,NULL,NULL, 5, 5, text_time, strnlen(text_time, 100),
                            CN_COLOR_WHITE, CN_R2_COPYPEN,CN_TIMEOUT_FOREVER);
    }
}
#endif


extern u8 pat_white[128];
extern u8 pat_black[128];
struct tagGkWinRsc wnd_body, wnd_gray8, wnd_body_selector;
struct tagGkWinRsc wnd_gray4, wnd_bottom,wnd_banner;
struct tagGkWinRsc wnd_gray2, wnd_gray1;
struct tagGkWinRsc *desktop;
void djy_main(void)
{
//    djyfs_file *fp;
    static char *banner_text = "---- DJYOS ----";
    u8 flip_inc1 = -2,flip_inc2 = 2,flip_inc4 = -2,flip_inc8 = 2;
    struct tagInputDeviceMsg *input_msg;
    struct tagRectBitmap bitmap;
    struct tagRectangle Grad;
    u32 r=0;
    s32 touch_x=0, touch_y=0;
    u8 alpha = 0x80;


    if( sqlite3_initialize())
      printf("initial sqlite3 error");
    test_sqlite1(0);

    desktop = GK_ApiGetDesktop("sim_display");


    GK_ApiCreateGkwin(desktop, &wnd_banner, 0, 0, 240, 25,
            CN_COLOR_BLUE, CN_WINBUF_BUF, "banner", CN_SYS_PF_DISPLAY,0,0,0);
    GK_ApiDrawText(&wnd_banner,NULL,NULL, 60, 5,
                banner_text, strnlen(banner_text, 100),CN_COLOR_RED,CN_R2_COPYPEN,0);

    // body
    GK_ApiCreateGkwin(desktop, &wnd_body, 0, 25, 240, 290,
            CN_COLOR_RED, CN_WINBUF_BUF, "body",CN_SYS_PF_DISPLAY,0,0, 0);
#if 0
    GK_ApiCreateGkwin(&wnd_body, &wnd_body_selector, 0, 0, 240, 20,
            CN_COLOR_RED+CN_COLOR_GREEN, CN_WINBUF_BUF, "body selector",
            CN_SYS_PF_DISPLAY,0,0,0);
    GK_ApiLineto(&wnd_body_selector, 60, 0, 60, 20,
            CN_COLOR_BLACK, CN_R2_COPYPEN, 0);
    GK_ApiLineto(&wnd_body_selector, 120, 0, 120, 20,
            CN_COLOR_BLACK, CN_R2_COPYPEN, 0);
    GK_ApiLineto(&wnd_body_selector, 180, 0, 180, 20,
            CN_COLOR_BLACK, CN_R2_COPYPEN, 0);
#endif
    bitmap.bm_bits = (u8*)gImage_rgb565_240x180;
    bitmap.width = 240;
    bitmap.height = 180;
    bitmap.linebytes = 480;
    bitmap.PixelFormat = CN_SYS_PF_RGB565;
    bitmap.ExColor = CN_COLOR_WHITE;
    GK_ApiDrawBitMap(&wnd_body, &bitmap, 0, 20, 0,0,CN_TIMEOUT_FOREVER);

    bitmap.bm_bits = (u8*)acindex_4;
    bitmap.width = 128;
    bitmap.height = 128;
    bitmap.linebytes = 64;
    bitmap.PixelFormat = CN_SYS_PF_PALETTE4;
    bitmap.ExColor = (ptu32_t)Colorsindex_4;
    GK_ApiDrawBitMap(&wnd_body, &bitmap, 0, 140,0,0, CN_TIMEOUT_FOREVER);

    bitmap.bm_bits = (u8*)acTulips_128_96;
    bitmap.width = 128;
    bitmap.height = 96;
    bitmap.linebytes = 128;
    bitmap.PixelFormat = CN_SYS_PF_PALETTE8;
    bitmap.ExColor = (ptu32_t)ColorsTulips;
    GK_ApiDrawBitMap(&wnd_body, &bitmap, 40, 20, 0,0,CN_TIMEOUT_FOREVER);

    bitmap.bm_bits = (u8*)acJellyfish_128_96;
    bitmap.width = 128;
    bitmap.height = 96;
    bitmap.linebytes = 128;
    bitmap.PixelFormat = CN_SYS_PF_PALETTE8;
    bitmap.ExColor = (ptu32_t)ColorsJellyfish;
    GK_ApiDrawBitMap(&wnd_body, &bitmap, 80, 116,0,0, CN_TIMEOUT_FOREVER);
    GK_ApiCreateGkwin(&wnd_body, &wnd_gray8, 0, 20, 64, 68,
            CN_COLOR_BLUE, CN_WINBUF_BUF, "gray8",CN_SYS_PF_DISPLAY,0,0, 0);
    GK_ApiCreateGkwin(&wnd_body, &wnd_gray4, 0,68,64,116,
            CN_COLOR_WHITE, CN_WINBUF_BUF, "gray4",CN_SYS_PF_DISPLAY,0,0, 0);
    GK_ApiCreateGkwin(&wnd_body, &wnd_gray2, 0,116,64,164,
            CN_COLOR_RED, CN_WINBUF_BUF, "gray2",CN_SYS_PF_DISPLAY,0,0, 0);

    GK_ApiCreateGkwin(&wnd_body, &wnd_gray1, 0,164,64,212,
            CN_COLOR_RED, CN_WINBUF_BUF, "gray1",CN_SYS_PF_DISPLAY,0,0, 0);

    bitmap.bm_bits = (u8*)gImage_gray8_64x48;
    bitmap.width = 64;
    bitmap.height = 48;
    bitmap.linebytes = 64;
    bitmap.PixelFormat = CN_SYS_PF_GRAY8;
    bitmap.ExColor = CN_COLOR_WHITE;
    GK_ApiDrawBitMap(&wnd_gray8, &bitmap, 0, 0, 0,0,CN_TIMEOUT_FOREVER);

    bitmap.bm_bits = (u8*)gImage_gray4_64x48;
    bitmap.width = 64;
    bitmap.height = 48;
    bitmap.linebytes = 32;
    bitmap.PixelFormat = CN_SYS_PF_GRAY4;
    bitmap.ExColor = CN_COLOR_WHITE;
    GK_ApiDrawBitMap(&wnd_gray4, &bitmap, 0, 0, 0,0,CN_TIMEOUT_FOREVER);

    bitmap.bm_bits = (u8*)gImage_gray2_64x48;
    bitmap.width = 64;
    bitmap.height = 48;
    bitmap.linebytes = 16;
    bitmap.PixelFormat = CN_SYS_PF_GRAY2;
    bitmap.ExColor = CN_COLOR_WHITE;
    GK_ApiDrawBitMap(&wnd_gray2, &bitmap, 0, 0,0,0, CN_TIMEOUT_FOREVER);

    bitmap.bm_bits = (u8*)gImage_gray1_64x48;
    bitmap.width = 64;
    bitmap.height = 48;
    bitmap.linebytes = 8;
    bitmap.PixelFormat = CN_SYS_PF_GRAY1;
    bitmap.ExColor = CN_COLOR_WHITE;
    GK_ApiDrawBitMap(&wnd_gray1, &bitmap, 0, 0, 0,0,CN_TIMEOUT_FOREVER);

    // bottom
    GK_ApiCreateGkwin(desktop, &wnd_bottom, 0, 295, 240, 320,
            CN_COLOR_BLUE, CN_WINBUF_BUF, "bottom",CN_SYS_PF_DISPLAY,0,0, 0);
    GK_ApiFillWin(desktop, CN_COLOR_BLACK, CN_TIMEOUT_FOREVER); // ±³¾°Í¿ºÚ

    GK_ApiSyncShow(CN_TIMEOUT_FOREVER);

    GDD_Demo();     //´Ëº¯Êý²»»á·µ»Ø

    Grad.left = 0;
    Grad.top = 0;
    Grad.right = 64;
    Grad.bottom = 48;
    while(1)
    {
        flip_inc1 -=2;
        flip_inc2 +=2;
        flip_inc4 -=2;
        flip_inc8 +=2;
        GK_ApiMoveWin(&wnd_gray1, flip_inc1, 164, 0);
        GK_ApiMoveWin(&wnd_gray2, flip_inc2, 116,0);
        GK_ApiMoveWin(&wnd_gray4, flip_inc4, 68,0);
        GK_ApiMoveWin(&wnd_gray8, flip_inc8, 20,CN_TIMEOUT_FOREVER);
        Grad.left = 0;
        Grad.top = 0;
        Grad.right = 64;
        Grad.bottom = 48;
        GK_ApiFillRect(&wnd_gray1,&Grad,CN_COLOR_BLUE,CN_COLOR_RED,CN_FILLRECT_MODE_H,0);
        GK_ApiFillRect(&wnd_gray2,&Grad,CN_COLOR_GREEN,CN_COLOR_BLUE,CN_FILLRECT_MODE_V,0);
        GK_ApiFillRect(&wnd_gray4,&Grad,CN_COLOR_RED+CN_COLOR_BLUE,CN_COLOR_RED+CN_COLOR_GREEN,CN_FILLRECT_MODE_SN,0);
        GK_ApiFillRect(&wnd_gray8,&Grad,CN_COLOR_WHITE,CN_COLOR_RED,CN_FILLRECT_MODE_SP,0);
        Grad.left = 70;
        Grad.top = 60;
        Grad.right = 200;
        Grad.bottom = 100;
        GK_ApiFillRect(&wnd_body,&Grad,CN_COLOR_WHITE,CN_COLOR_RED,CN_FILLRECT_MODE_SP,0);
//        GK_ApiSyncShow(CN_TIMEOUT_FOREVER);
        dis_wnd_bottom_text(&wnd_bottom);
        GK_ApiDrawText(&wnd_body,NULL,NULL, 0, 0,"flash", 5,CN_COLOR_WHITE,CN_R2_XORPEN,0);
        GK_ApiDrawText(&wnd_body,NULL,NULL, 80, 120,"flash", 5,CN_COLOR_WHITE,CN_R2_XORPEN,0);
        GK_ApiSyncShow(CN_TIMEOUT_FOREVER);

        Djy_WaitEvttPop(Djy_MyEvttId(),NULL,200*mS);
    }
}

