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
#include <string.h>
#include <stdlib.h>
#include "gkernel.h"
#include "cpu_peri.h"
#include "os.h"
#include "board-config.h"
#include <gui/gkernel/gk_display.h>
#include <LCM240128C/sr5333/lcm240128c_config.h>

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

struct DisplayRsc tg_lcd_display;
u16 *pg_frame_buffer;
u8 *CmdPort = (u8*)0x60000000;
u8 *DataPort = (u8*)0x60000001;
u8 ContrastLevel;
#define CN_TONTRAST_LEVEL_DEFAULT   0x68


static const Pin LCD_BLK[] = {
        {PIO_PA10, PIOA, ID_PIOA, PIO_OUTPUT_0,    PIO_DEFAULT}
};

static const Pin LCD_Pins[] = {
    PIN_EBI_DBUS,
    PIN_EBI_DNRD,
    PIN_EBI_DNWE,
    PIN_EBI_ABUS0,
    PIN_EBI_ENCS0,
    PIN_LCD_BACKLED,
};
static void Lcd_PortEBI_Init(void)
{
    PIO_Configure(LCD_Pins,PIO_LISTSIZE(LCD_Pins));
    PMC_EnablePeripheral(ID_SMC ) ;

    SMC->SMC_CS_NUMBER[0].SMC_SETUP = (0
                    | (SMC_SETUP_NWE_SETUP(40))
                    | (SMC_SETUP_NCS_WR_SETUP(20))
                    | (SMC_SETUP_NRD_SETUP(40))
                    | (SMC_SETUP_NCS_RD_SETUP(20))
                      );

    SMC->SMC_CS_NUMBER[0].SMC_PULSE = (0
                    | (SMC_PULSE_NWE_PULSE(50))
                    | (SMC_PULSE_NCS_WR_PULSE(80))

                    | (SMC_PULSE_NRD_PULSE(50))
                    | (SMC_PULSE_NCS_RD_PULSE(80))
                      );

    SMC->SMC_CS_NUMBER[0].SMC_CYCLE = (0
                    | (SMC_CYCLE_NWE_CYCLE(150))
                    | (SMC_CYCLE_NRD_CYCLE(150))
                      );

    SMC->SMC_CS_NUMBER[0].SMC_MODE = (0
                    | (SMC_MODE_DBW_8_BIT)
                    | (SMC_MODE_EXNW_MODE_DISABLED)
                    | (SMC_MODE_BAT_BYTE_WRITE)
                    | (SMC_MODE_READ_MODE)
                    | (SMC_MODE_WRITE_MODE)
                      );
}

//----初始化lcd硬件------------------------------------------------------------
//功能: 如名
//参数: 无
//返回: 无
//备注: 本函数是移植关键函数.
//----------------------------------------------------------------------------
void InitLCM240128C(void)
{
    ContrastLevel = CN_TONTRAST_LEVEL_DEFAULT;
    *CmdPort = 0xE2;                            // 设置温度补偿系数-0.05%/C
    Djy_EventDelay(10*mS);
    *CmdPort = 0x25;                            // 设置温度补偿系数-0.05%/C
    *CmdPort = 0x2b;                            // 内部DC-DC
    *CmdPort = 0xc4;                            // LCD映像MY=1，MX=0，LC0=0
    *CmdPort = 0xa3;                            // 设置行扫描频率 FR=263Hz
    *CmdPort = 0xd1;                            // 彩色数据格式R-G-B
    *CmdPort = 0xd5;                            // 设置数据位为12位RRRR-GGGG-BBBB
    *CmdPort = 0xc8; *CmdPort = 0x18;           // 设置M信号为帧翻转
    *CmdPort = 0xeb;                            // 设置偏压比1/12
    *CmdPort = 0xa7;                            // 负性显示
    *CmdPort = 0xa4;                            // 正常显示
    *CmdPort = 0x81; *CmdPort = ContrastLevel;  // 设置对比度  对应单色图案
    *CmdPort = 0xdf;                            // 设置扫描模式 启动FRC
    *CmdPort = 0xad;                            // 开显示  启动灰度显示
    *CmdPort = 0xf4; *CmdPort = 0x00;           // 设置操作窗口左边界
    *CmdPort = 0xf5; *CmdPort = 0x00;           // 设置操作窗口上边界
    *CmdPort = 0xf6; *CmdPort = 0x4f;           // 设置操作窗口右边界
    *CmdPort = 0xf7; *CmdPort = 0x7f;           // 设置操作窗口下边界
    *CmdPort = 0xf8;                            // 设置窗口操作使能
    *CmdPort = 0x00; *CmdPort = 0x10;           // 设置起始列地址
    *CmdPort = 0x60; *CmdPort = 0x70;           // 设置起始行地址
    Djy_EventDelay(50*mS);
}

//----LCD背光点亮------------------------------------------------------------
//功能: 略
//参数: OnOff，true=点亮，false=熄灭
//返回: 无
//-----------------------------------------------------------------------------
void __lcd_BackLight(bool_t OnOff)
{
    if(OnOff)
        PIO_Clear(LCD_BLK);
    else
        PIO_Set(LCD_BLK);
}

//----LCD显示使能------------------------------------------------------------
//功能: 略
//参数: OnOff，true=使能显示，false=关闭显示
//返回: 无
//-----------------------------------------------------------------------------
void __lcd_envid_of(bool_t OnOff)
{
    if(OnOff)
        *CmdPort = 0xaf;                // 开显示  启动灰度显示
    else
        *CmdPort = 0xae;                // 关显示  启动灰度显示
}

//----控制LCD电源--------------------------------------------------------------
//功能: LCD 电源控制引脚使能
//参数: OnOff，true=打开驱动电源，false=关闭电源
//返回: 无
//-----------------------------------------------------------------------------
void __lcd_power_enable(bool_t OnOff)
{
    if(OnOff)
        *CmdPort = 0x28;                // 开显示  启动灰度显示
    else
        *CmdPort = 0x2b;                // 关显示  启动灰度显示
}
//----控制显示器---------------------------------------------------------------
//功能: 这是由driver提供商编写的，用于设置显示器各种参数的函数，属于应用程序的
//      范畴，功能由设备厂商定义。
//参数: disp，显示器指针
//返回: true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_disp_ctrl(struct DisplayRsc *disp)
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
bool_t __lcd_set_pixel_bm(struct RectBitmap *bitmap,
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
bool_t __lcd_line_bm(struct RectBitmap *bitmap,struct Rectangle *limit,
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
bool_t __lcd_line_bm_ie(struct RectBitmap *bitmap,struct Rectangle *limit,
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
bool_t __lcd_fill_rect_bm(struct RectBitmap *dst_bitmap,
                          struct Rectangle *Target,
                          struct Rectangle *Focus,
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
bool_t __lcd_blt_bm_to_bm( struct RectBitmap *dst_bitmap,
                            struct Rectangle *DstRect,
                            struct RectBitmap *src_bitmap,
                            struct Rectangle *SrcRect,
                            struct RopGroup RopCode, u32 HyalineColor)
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
bool_t __lcd_get_pixel_bm(struct RectBitmap *bitmap,s32 x,s32 y)
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
bool_t __lcd_line_screen(struct Rectangle *limit,
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
bool_t __lcd_line_screen_ie(struct Rectangle *limit,
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
bool_t __lcd_fill_rect_screen(struct Rectangle *Target,
                              struct Rectangle *Focus,
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
bool_t __lcd_bm_to_screen(struct Rectangle *dst_rect,
                struct RectBitmap *src_bitmap,s32 xsrc,s32 ysrc)
{
    s32 xstart,xend;
    s32 realxsrc;
    s32 loopx,loopy;
    u32 offset;
    u8 c;
    //根据LCD模块的要求，x的start和end都要调整为6的倍数
    xstart = (dst_rect->left/6)*6;
    xend = ((dst_rect->right-1)/6)*6;
    realxsrc = xsrc - (dst_rect->left-xstart);

    *CmdPort = 0xf4; *CmdPort = xstart/3;           // 设置操作窗口左边界
    *CmdPort = 0xf5; *CmdPort = (u8)dst_rect->top;  // 设置操作窗口上边界
    *CmdPort = 0xf6; *CmdPort = xend/3+1;             // 设置操作窗口右边界
    *CmdPort = 0xf7; *CmdPort = (u8)(dst_rect->bottom-1);  // 设置操作窗口下边界
    *CmdPort = 0xf8;                                // 设置窗口操作使能
    *CmdPort = xstart/3 & 0x0f;
        *CmdPort = 0x10|(xstart/3 >> 4);            // 设置起始列地址

    *CmdPort = 0x60 + ((u8)dst_rect->top & 0x0f);
        *CmdPort = 0x70 + ((u8)(dst_rect->top)>> 4);// 设置起始行地址

    if(realxsrc%2 == 0)
    {
        offset = ysrc * src_bitmap->linebytes + realxsrc/2;
        for(loopy = 0; loopy < dst_rect->bottom - dst_rect->top; loopy++)
        {
            for(loopx = 0; loopx < (xend-xstart+6)/2; loopx++ )
            {
                c = src_bitmap->bm_bits[offset+loopx];
                *DataPort = c;
            }
            offset += src_bitmap->linebytes;
        }
    }
    else
    {

    }
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
bool_t __lcd_get_rect_screen(struct Rectangle *rect,struct RectBitmap *dest)
{
    return false;
 }

//----初始化lcd设备------------------------------------------------------------
//功能: 如名
//参数: 无
//返回: 显示器资源指针
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_LCM240128C(ptu32_t para)
{
    static struct GkWinRsc frame_win;
    static struct RectBitmap FrameBitmap;

    PIO_Configure(LCD_BLK, PIO_LISTSIZE(LCD_BLK));
    __lcd_BackLight(1);
    Lcd_PortEBI_Init();
    InitLCM240128C( );

    pg_frame_buffer = malloc(CN_LCD_XSIZE*CN_LCD_YSIZE/2);
    memset(pg_frame_buffer,0,CN_LCD_XSIZE*CN_LCD_YSIZE/2);
    FrameBitmap.bm_bits = (u8*)pg_frame_buffer;
    FrameBitmap.width = CN_LCD_XSIZE;
    FrameBitmap.height = CN_LCD_YSIZE;
    FrameBitmap.PixelFormat = CN_SYS_PF_GRAY4;
    FrameBitmap.linebytes = CN_LCD_XSIZE/2;
    FrameBitmap.ExColor = 0xffffffff;
    frame_win.wm_bitmap = &FrameBitmap;
    tg_lcd_display.frame_buffer = &frame_win;

    tg_lcd_display.xmm = 0;
    tg_lcd_display.ymm = 0;
    tg_lcd_display.width = CN_LCD_XSIZE;
    tg_lcd_display.height = CN_LCD_YSIZE;
    tg_lcd_display.pixel_format = CN_SYS_PF_GRAY4;
    tg_lcd_display.reset_clip = false;
    tg_lcd_display.framebuf_direct = false;
    //无须初始化frame_buffer和desktop，z_topmost三个成员

    tg_lcd_display.draw.SetPixelToBitmap = __lcd_set_pixel_bm;
    tg_lcd_display.draw.FillRectToBitmap = __lcd_fill_rect_bm;
    tg_lcd_display.draw.LineToBitmap = __lcd_line_bm;
    tg_lcd_display.draw.LineToBitmapIe = __lcd_line_bm_ie;
    tg_lcd_display.draw.BltBitmapToBitmap = __lcd_blt_bm_to_bm;
//    tg_lcd_display.draw.check_raster = __lcd_check_raster;
    tg_lcd_display.draw.SetPixelToScreen = __lcd_set_pixel_screen;
    tg_lcd_display.draw.LineToScreen = __lcd_line_screen;
    tg_lcd_display.draw.LineToScreenIe = __lcd_line_screen_ie;
    tg_lcd_display.draw.FillRectToScreen = __lcd_fill_rect_screen;
    tg_lcd_display.draw.CopyBitmapToScreen = __lcd_bm_to_screen;
    tg_lcd_display.draw.GetPixelFromScreen = __lcd_get_pixel_screen;
    tg_lcd_display.draw.GetRectFromScreen = __lcd_get_rect_screen;

    tg_lcd_display.disp_ctrl = __lcd_disp_ctrl;

    if(GK_InstallDisplay(&tg_lcd_display,(char*)para))
        return (ptu32_t)&tg_lcd_display;
    else
        return 0;
}

