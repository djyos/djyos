//----------------------------------------------------
//Copyright (C), 2004-2011,  罗侍田.
//版权所有 (C), 2004-2011,   罗侍田.
//所属模块:时钟模块
//作者:  罗侍田
//版本：V1.0.0
//文件描述: windows窗口模拟液晶驱动
//其他说明:
//修订历史:
// 1. 日期: 2012-10-09
//   作者:  罗侍田
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "gkernel.h"
#include "stddev.h"
#include "cpu_peri.h"
#include "cpu.h"
#include "gk_display.h"
#include <string.h>
#include <process.h>
#include <Windows.h>
#include <winGDI.h>
#include <tchar.h>
#include "touch.h"

//坐标系说明,描述了显存偏移地址与屏上像素点的坐标映射关系,注意单色和灰度显示器
//每字节可能包含不止一个像素点,图中没有加以区分.
//不同的系统有不同的对应关系,这种变化影响 部分显示函数的编写方法.
//应用程序以及gui驱动程序无需关心这种映射关系.
//例外的是,如果你用数组与屏幕直接映射的方法显示bmp图形,需要了解这种映射关系.
//----------------------------------------------------->x   右上角
// |0x0000,0x0001,0x0002--------------------------xmax-1
// |xmax,xmax+1,xmax+2---------------------------2*max-1
// |----------------------------------------------------
// |----------------------------------------------------
// |----------------------------------------------------
// |(ymax-1)*xmax,(ymax-1)*xmax+1------------ymax*xmax-1    右下角
//\/


HDC     lcd_hdc ;
BITMAPINFO *lcdbmi;
TCHAR szLcdDriver[]       =_TEXT("都江堰操作系统");    // 标题栏文本

LRESULT CALLBACK    WndProc_lcd(HWND, UINT, WPARAM, LPARAM);

struct tagDisplayRsc tg_lcd_display;

bool_t touched = false;
s32 touch_x=0,touch_y=0,touch_z=0;
RECT tg_WindowRect;
s32 s32g_window_height,s32g_window_width;

extern HWND tg_keyboard_wnd;

u16 *pg_frame_buffer;

u16* getframebuff()
{
   return pg_frame_buffer;

}

bool_t __lcd_line_bm_ie(struct tagRectBitmap *bitmap,struct tagRectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code);
bool_t __lcd_blt_bm_to_bm( struct tagRectBitmap *dst_bitmap,
                            struct tagRectangle *DstRect,
                            struct tagRectBitmap *src_bitmap,
                            struct tagRectangle *SrcRect,
                            u32 RopCode, u32 KeyColor);
//----初始化lcd硬件------------------------------------------------------------
//功能: 如名
//参数: 无
//返回: 无
//备注: 本函数是移植关键函数.
//----------------------------------------------------------------------------
void __lcd_hard_init(void)
{
    RECT ClientRect;
    HWND lcd_wnd;
//    HBITMAP hbmi;

    lcdbmi = malloc(sizeof(BITMAPINFO)+8);
    lcdbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    lcdbmi->bmiHeader.biWidth = LCD_XSIZE;
    lcdbmi->bmiHeader.biHeight = LCD_YSIZE;
    lcdbmi->bmiHeader.biPlanes = 1;
    lcdbmi->bmiHeader.biBitCount = 16;
    lcdbmi->bmiHeader.biCompression = BI_BITFIELDS;
    lcdbmi->bmiHeader.biSizeImage = 0;
    lcdbmi->bmiHeader.biXPelsPerMeter = 1;
    lcdbmi->bmiHeader.biYPelsPerMeter = 1;
    lcdbmi->bmiHeader.biClrUsed = 0;
    lcdbmi->bmiHeader.biClrImportant = 0;
    lcdbmi->bmiColors[0].rgbBlue = 0;
    lcdbmi->bmiColors[0].rgbGreen = 0xf8;
    lcdbmi->bmiColors[0].rgbRed = 0;
    lcdbmi->bmiColors[0].rgbReserved = 0;
    lcdbmi->bmiColors[1].rgbBlue = 0xe0;
    lcdbmi->bmiColors[1].rgbGreen = 0x07;
    lcdbmi->bmiColors[1].rgbRed = 0;
    lcdbmi->bmiColors[1].rgbReserved = 0;
    lcdbmi->bmiColors[2].rgbBlue = 0x1f;
    lcdbmi->bmiColors[2].rgbGreen = 0;
    lcdbmi->bmiColors[2].rgbRed = 0;
    lcdbmi->bmiColors[2].rgbReserved = 0;
    CreateDIBSection(0,lcdbmi,DIB_RGB_COLORS,(void**)&pg_frame_buffer,NULL,0);


    RegisterWinClass(hInstance,WndProc_lcd,szLcdDriver);
    lcd_wnd = CreateWindow(szLcdDriver, szLcdDriver, WS_CAPTION | WS_MINIMIZEBOX,
                       CW_USEDEFAULT,CW_USEDEFAULT, 0, 0,
                       NULL, NULL, hInstance, NULL);

    if (!lcd_wnd)
    {
        return;
    }
    GetWindowRect(lcd_wnd, &tg_WindowRect);
    GetClientRect(lcd_wnd, &ClientRect);
    tg_WindowRect.right += (cn_lcd_window_xsize - ClientRect.right);
    tg_WindowRect.bottom += (cn_lcd_window_ysize - ClientRect.bottom);
    s32g_window_height =  tg_WindowRect.bottom-tg_WindowRect.top;
    s32g_window_width = tg_WindowRect.right-tg_WindowRect.left;
    MoveWindow(lcd_wnd, tg_WindowRect.left, tg_WindowRect.top, s32g_window_width, s32g_window_height, false);

    ShowWindow(lcd_wnd, SW_SHOW);
    UpdateWindow(lcd_wnd);

    lcd_hdc = GetDC (lcd_wnd) ;
}

//
//  函数: WndProc_lcd(HWND, unsigned, WORD, LONG)
//
//  目的: 处理主窗口的消息。
//
LRESULT CALLBACK WndProc_lcd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT         ps ;
    switch (message)
    {
        case WM_PAINT:
            BeginPaint (hWnd, &ps) ;
            StretchDIBits (lcd_hdc,    0, 0,cn_lcd_window_xsize, cn_lcd_window_ysize,
                       0, LCD_YSIZE, LCD_XSIZE,-LCD_YSIZE,pg_frame_buffer,lcdbmi,DIB_RGB_COLORS,SRCCOPY) ;
            EndPaint (hWnd, &ps) ;
            break;
        case WM_MOVE:
            StretchDIBits (lcd_hdc,    0, 0,cn_lcd_window_xsize, cn_lcd_window_ysize,
               0, LCD_YSIZE, LCD_XSIZE,-LCD_YSIZE,pg_frame_buffer,lcdbmi,DIB_RGB_COLORS,SRCCOPY) ;
            GetWindowRect(hWnd, &tg_WindowRect);
            MoveWindow(tg_keyboard_wnd, tg_WindowRect.left+s32g_window_width + 5,
                       tg_WindowRect.top, s32g_window_width, s32g_window_height,
                       true);
            break ;
        case WM_ACTIVATE:
            StretchDIBits (lcd_hdc,    0, 0,cn_lcd_window_xsize, cn_lcd_window_ysize,
               0, LCD_YSIZE, LCD_XSIZE,-LCD_YSIZE,pg_frame_buffer,lcdbmi,DIB_RGB_COLORS,SRCCOPY) ;

            break ;
        case WM_LBUTTONDOWN:
            touch_x = LOWORD(lParam);
            touch_y = HIWORD(lParam);
            touch_z = 1;
            touched = true;
            break;
        case WM_LBUTTONUP:
            touch_x = LOWORD(lParam);
            touch_y = HIWORD(lParam);
            touch_z = 0;
            touched = false;
            break;
        case WM_MOUSEMOVE:
            touch_x = LOWORD(lParam);
            touch_y = HIWORD(lParam);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


//----LCD控制信号------------------------------------------------------------
//功能: LCD 视频输出和逻辑使能/禁止,1=允许视频输出和 LCD控制信号,
//      0=禁止视频输出和 LCD控制信号
//参数: onoff，控制信号
//返回: 无
//-----------------------------------------------------------------------------
void __lcd_envid_of(int onoff)
{

}
//----控制LCD电源--------------------------------------------------------------
//功能: LCD 电源控制引脚使能
//参数: invpwren，
//      pwren,
//返回: 无
//-----------------------------------------------------------------------------
void __lcd_power_enable(int invpwren,int pwren)
{

}
//----控制显示器---------------------------------------------------------------
//功能: 这是由driver提供商编写的，用于设置显示器各种参数的函数，属于应用程序的
//      范畴，功能由设备厂商定义。
//参数: disp，显示器指针
//返回: true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_disp_ctrl(struct tagDisplayRsc *disp)
{
    return true;
}


//----位图中画像素-------------------------------------------------------------
//功能: 在位图中画一个像素，只有在bitmap的像素格式为cn_custom_pf时，才需要绘制。
//      如果显卡不打算支持自定义格式，本函数直接返回。
//参数: bitmap，目标位图
//      limit，限制矩形，只绘制在该矩形内部的部分。
//      x，y，坐标
//      color，绘图用的颜色
//      r2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
bool_t __lcd_set_pixel_bm(struct tagRectBitmap *bitmap,
                     s32 x,s32 y,u32 color,u32 r2_code)
{
    return false;
}
//----位图中画线---------------------------------------------------------------
//功能: 在位图中画一条直线，不绘制(x2,y2)点，只绘制在limit限定的区域内的部分。
//参数: bitmap，目标位图
//      limit，限制矩形，只绘制在该矩形内部的部分。
//      x1,y1,x2,y2，起点终点坐标
//      color，绘图用的颜色
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=失败，无硬件加速或不支持按r2_code画线。
//-----------------------------------------------------------------------------
bool_t __lcd_line_bm(struct tagRectBitmap *bitmap,struct tagRectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//----位图中画线(含端点)-------------------------------------------------------
//功能: 在位图中画一条直线，只绘制在limit限定的区域内的部分
//参数: bitmap，目标位图
//      limit，限制矩形，只绘制在该矩形内部的部分。
//      x1,y1,x2,y2，起点终点坐标
//      color，绘图用的颜色
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=失败，无硬件加速或不支持按r2_code画线。
//-----------------------------------------------------------------------------
bool_t __lcd_line_bm_ie(struct tagRectBitmap *bitmap,struct tagRectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//----位图中填充矩形-----------------------------------------------------------
//功能: 把位图中的矩形用color颜色填充
//参数: bitmap，目标位图
//      dst_rect,待填充的矩形
//      color，填充颜色
//返回:  true=成功绘制，false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_fill_rect_bm(struct tagRectBitmap *dst_bitmap,
                          struct tagRectangle *Target,
                          struct tagRectangle *Focus,
                          u32 Color0,u32 Color1,u32 Mode)
{
    return false;
}
//----bitmap中位块传送---------------------------------------------------------
//功能: 把一个位图中的一个矩形区域传送到另一个区域，两个位图的像素格式不必相同。
//      源和目的位图可以相同
//参数: dst_bitmap，目标位图
//      dst_rect，目标矩形
//      src_bitmap，源位图
//      xsrc,ysrc，源位图中被传送的区域左上角坐标
//      pat，pattern指针
//      mask，rop4操作需要的掩码
//      rop_code，三元光栅操作码
//      transparentcolor，关键颜色
//返回: true=成功,false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_blt_bm_to_bm( struct tagRectBitmap *dst_bitmap,
                            struct tagRectangle *DstRect,
                            struct tagRectBitmap *src_bitmap,
                            struct tagRectangle *SrcRect,
                            u32 RopCode, u32 KeyColor)
{
    return false;
}
//----读取位图中的像素---------------------------------------------------------
//功能: 取位图中的一个像素，只有在bitmap的像素格式为cn_custom_pf时，才需要读取。
//      如果显卡不打算支持自定义格式，本函数直接返回false。
//参数: bitmap，目标位图
//      x，y，坐标
//返回:  true=成功读取，false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_get_pixel_bm(struct tagRectBitmap *bitmap,s32 x,s32 y)
{
    return false;
}
//----读取bitmap中矩形块-------------------------------------------------------
//功能: 把一个位图中的矩形读取到另一个位图中
//参数: src，源位图
//      rect，欲读取的矩形。
//      dest，保存矩形的位图，其长宽必须与rect相同。
//返回: true=成功读取，false=失败，原因可能是提供了不支持的像素格式。
//-----------------------------------------------------------------------------
bool_t __lcd_get_rect_bm(struct tagRectBitmap *src,struct tagRectangle *rect,
                     struct tagRectBitmap *dest)
{
    return false;
}
//----screen中画像素-----------------------------------------------------------
//功能: 在screen中画一个像素，有frame buffer的driver，直接返回即可
//参数: x,y,像素坐标
//      color，颜色
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=没有绘制，或无需绘制
//-----------------------------------------------------------------------------
bool_t __lcd_set_pixel_screen(s32 x,s32 y,u32 color,u32 r2_code)
{
    return false;
}
//----screen中画线---------------------------------------------------------------
//功能: 在screen中画一条任意直线，不含端点，只绘制在limit限定的区域内的部分。如
//      不支持画线的硬件加速，直接返回false。
//参数: limit，限制矩形，只绘制在该矩形内部的部分。
//      x1,y1,x2,y2，起点终点坐标
//      color，绘图用的颜色
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=失败，无硬件加速或不支持按r2_code画线。
//-----------------------------------------------------------------------------
bool_t __lcd_line_screen(struct tagRectangle *limit,
                    s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//----screen中画线(含端点)------------------------------------------------------
//功能: 在screen中画一条直线，只绘制在limit限定的区域内的部分
//参数: limit，限制矩形，只绘制在该矩形内部的部分。
//      x1,y1,x2,y2，起点终点坐标
//      color，绘图用的颜色
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=失败，无硬件加速或不支持按r2_code画线。
//-----------------------------------------------------------------------------
bool_t __lcd_line_screen_ie(struct tagRectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//----screen中填充矩形-----------------------------------------------------------
//功能: 把screen中的矩形用color颜色填充，color的格式是cn_sys_pf_e8r8g8b8
//参数: dst_rect,待填充的矩形
//      color，填充颜色
//返回:  true=成功绘制，false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_fill_rect_screen(struct tagRectangle *Target,
                              struct tagRectangle *Focus,
                              u32 Color0,u32 Color1,u32 Mode)
{
    return false;
}

//----bitmap到screen位块传送---------------------------------------------------
//功能: 把一个位图中的一个矩形区域传送到另一个区域，两个位图的像素格式必须相同。
//      源和目的位图可以相同
//参数: dst_rect，目标矩形
//      src_bitmap，源位图
//      xsrc,ysrc，源位图中被传送的区域左上角坐标
//返回: true=成功,false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_bm_to_screen(struct tagRectangle *dst_rect,
                struct tagRectBitmap *src_bitmap,s32 xsrc,s32 ysrc)
{
#if 1
    StretchDIBits (lcd_hdc,   dst_rect->left*X_SCALE, dst_rect->top*Y_SCALE,
                            (dst_rect->right - dst_rect->left)*X_SCALE,
                            (dst_rect->bottom - dst_rect->top)*Y_SCALE,
                            xsrc,ysrc + dst_rect->bottom - dst_rect->top,
                            dst_rect->right - dst_rect->left,
                            -(dst_rect->bottom - dst_rect->top),
                            pg_frame_buffer,lcdbmi,DIB_RGB_COLORS,SRCCOPY) ;
#else
    StretchDIBits (lcd_hdc,    0, 0,320, 240,
               0, 240, 320,-240,pg_frame_buffer,lcdbmi,DIB_RGB_COLORS,SRCCOPY) ;
#endif
    return true;
}
//----从screen中取像素---------------------------------------------------------
//功能: 从screen中取一像素，并转换成cn_sys_pf_e8r8g8b8或cn_sys_pf_a8r8g8b8格式
//参数: x,y，坐标
//返回: 像素颜色值
//-----------------------------------------------------------------------------
u32 __lcd_get_pixel_screen(s32 x,s32 y)
{
    //x86_windows下，就是利用windows提取函数提取。
    return 0;
}

//----读取bitmap中矩形块-------------------------------------------------------
//功能: 把一个位图中的矩形读取到另一个位图中
//参数: rect，欲读取的矩形。
//      dest，保存矩形的位图，其长宽必须与rect相同。
//返回: true=成功读取，false=失败，原因可能是提供了不支持的像素格式。
//-----------------------------------------------------------------------------
bool_t __lcd_get_rect_screen(struct tagRectangle *rect,struct tagRectBitmap *dest)
{
    return false;
 }
//----初始化lcd设备------------------------------------------------------------
//功能: 如名
//参数: 无
//返回: 显示器资源指针
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_Lcd(ptu32_t para)
{
    static struct tagGkWinRsc frame_win;
    static struct tagRectBitmap FrameBitmap;
    __lcd_hard_init();
//  __lcd_power_enable(0,1);
//  __lcd_envid_of(1);

    FrameBitmap.bm_bits = (u8*)pg_frame_buffer;
    FrameBitmap.width = LCD_XSIZE;
    FrameBitmap.height = LCD_YSIZE;
    FrameBitmap.PixelFormat = cn_lcd_pf;
    FrameBitmap.linebytes = cn_lcd_line_size;
    FrameBitmap.ExColor = 0;
    frame_win.wm_bitmap = &FrameBitmap;
    tg_lcd_display.frame_buffer = &frame_win;


    tg_lcd_display.xmm = 0;
    tg_lcd_display.ymm = 0;
    tg_lcd_display.width = LCD_XSIZE;
    tg_lcd_display.height = LCD_YSIZE;
    tg_lcd_display.pixel_format = CN_SYS_PF_RGB565;
    tg_lcd_display.reset_clip = false;
    tg_lcd_display.framebuf_direct = false;
    //无须初始化frame_buffer和desktop，z_topmost三个成员

    tg_lcd_display.draw.set_pixel_bm = __lcd_set_pixel_bm;
    tg_lcd_display.draw.fill_rect_bm = __lcd_fill_rect_bm;
    tg_lcd_display.draw.get_pixel_bm = NULL;
    tg_lcd_display.draw.line_bm = __lcd_line_bm;
    tg_lcd_display.draw.line_bm_ie = __lcd_line_bm_ie;
    tg_lcd_display.draw.blt_bm_to_bm = __lcd_blt_bm_to_bm;
    tg_lcd_display.draw.get_rect_bm = __lcd_get_rect_bm;
//    tg_lcd_display.draw.check_raster = __lcd_check_raster;
    tg_lcd_display.draw.set_pixel_screen = __lcd_set_pixel_screen;
    tg_lcd_display.draw.line_screen = __lcd_line_screen;
    tg_lcd_display.draw.line_screen_ie = __lcd_line_screen_ie;
    tg_lcd_display.draw.fill_rect_screen = __lcd_fill_rect_screen;
    tg_lcd_display.draw.bm_to_screen = __lcd_bm_to_screen;
    tg_lcd_display.draw.get_pixel_screen = __lcd_get_pixel_screen;
    tg_lcd_display.draw.get_rect_screen = __lcd_get_rect_screen;

//    tg_lcd_display.bmmalloc = lcd_bmmalloc;     //hjj,2416
    tg_lcd_display.disp_ctrl = __lcd_disp_ctrl;

    if(GK_InstallDisplay(&tg_lcd_display,(char*)para))
        return (ptu32_t)&tg_lcd_display;
    else
        return 0;
}

//以下是触摸屏部分，因为windows只有一个鼠标，只能模拟一个定位设备，触摸屏和鼠标
//是不能同时使能的

//----读取触摸点坐标-----------------------------------------------------------
//功能: 读取stmpe811采集到的触摸点坐标，如果有多点，则平均之
//参数: touch_data，采集到的坐标
//返回: 1=触摸笔按下，0=触摸笔提起，
//-----------------------------------------------------------------------------
ufast_t read_touch_windows(struct tagSingleTouchMsg *touch_data)
{
    if(touched == true)
    {
        touch_data->x = touch_x;
        touch_data->y = touch_y;
        touch_data->z = touch_z;
        return true;
    }
    else
    {
        touch_data->z = touch_z;
        return false;
    }
}

//----初始化触摸屏模块---------------------------------------------------------
//功能:
//参数: display_name,本触摸屏对应的显示器名(资源名)
//返回: 无
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_TouchWindows(ptu32_t para)
{
    static struct tagSingleTouchPrivate windows_touch;
    windows_touch.read_touch = read_touch_windows;
    Touch_InstallDevice((char*)para,&windows_touch);
    return 1;
}

