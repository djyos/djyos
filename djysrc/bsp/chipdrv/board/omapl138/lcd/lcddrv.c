//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
// Copyright (c) 2014 著作权由深圳鹏瑞软件有限公司所有。著作权人保留一切权利。
//
// 这份授权条款，在使用者符合下列条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------
#include "stdint.h"
#include "gkernel.h"
#include "lcddrv.h"
#include "cpu_peri.h"

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
//y

bool_t __lcd_set_pixel_bm(struct tagStRectBitmap *bitmap,
                         s32 x,s32 y,u32 color,u32 r2_code);
bool_t __lcd_line_bm(struct tagStRectBitmap *bitmap,struct tagStRect *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code);
bool_t __lcd_line_bm_ie(struct tagStRectBitmap *bitmap,struct tagStRect *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code);
bool_t __lcd_fill_rect_bm(struct tagStRectBitmap *dst_bitmap,
                         struct tagStRect *dst_rect,u32 color);
bool_t __lcd_blt_bm_to_bm( struct tagStRectBitmap *dst_bitmap,
                            struct tagStRect *dst_rect,
                            struct tagStRectBitmap *src_bitmap,s32 xsrc,s32 ysrc,
                            u8 *pat,struct tagStRectBitmap *mask,u32 rop_code,
                            u32 transparentcolor);
bool_t __lcd_get_rect_bm(struct tagStRectBitmap *src,struct tagStRect *rect,
                        struct tagStRectBitmap *dest);
bool_t __lcd_set_pixel_screen(s32 x,s32 y,u32 color,u32 r2_code);
bool_t __lcd_line_screen(struct tagStRect *limit,
                    s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code);
bool_t __lcd_line_screen_ie(struct tagStRect *limit,
                       s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code);
bool_t __lcd_fill_rect_screen(struct tagStRect *dst_rect,u32 color);
bool_t __lcd_bm_to_screen(struct tagStRect *dst_rect,
            struct tagStRectBitmap *src_bitmap,s32 xsrc,s32 ysrc);
u32 __lcd_get_pixel_screen(s32 x,s32 y);
bool_t __lcd_get_rect_screen(struct tagStRect *rect,struct tagStRectBitmap *dest);
bool_t __lcd_disp_ctrl(struct tagDisplayRsc *disp);


struct tagDisplayRsc tg_lcd_display;

//size = [CN_PALETTE_SIZE + cn_frame_buffer_size]
u8 *u8g_dsp_buffer = (u8*)(CN_FRAME_BASE_ADDRESS +CN_PALETTE_SIZE);
u8 u8g_frame_buffer[cn_frame_buffer_size];
static u16 s_u16Contrast = 75;
#define M_GuiSwap(a, b){a^=b; b^=a; a^=b;}

void __lcd_hard_init(void)
{
    u32 x,y,reg;
    u16 *pdata;
    // enable power and setup lcdc.
    Cpucfg_EnableLPSC(cn_PSC1_LCDC);

        /* Palette */
    pdata = (u16*)(u8g_dsp_buffer - CN_PALETTE_SIZE);
#if CN_LCD_TYPE == CN_MODE_GREY_MONO
    *pdata++ = 0;
    *pdata++ = 15;
#elif CN_LCD_TYPE == CN_MODE_GREY_04
    *pdata++ = 0;
    *pdata++ = 5;
    *pdata++ = 10;
    *pdata++ = 15;
#elif CN_LCD_TYPE == CN_MODE_GREY_16
    *pdata++ = 0;
    *pdata++ = 1;
    *pdata++ = 2;
    *pdata++ = 3;
    *pdata++ = 4;
    *pdata++ = 5;
    *pdata++ = 6;
    *pdata++ = 7;
    *pdata++ = 8;
    *pdata++ = 9;
    *pdata++ = 10;
    *pdata++ = 11;
    *pdata++ = 12;
    *pdata++ = 13;
    *pdata++ = 14;
    *pdata++ = 15;
#endif

#if ((CN_LCD_TYPE == CN_MODE_GREY_MONO) || (CN_LCD_TYPE == CN_MODE_GREY_04) || (CN_LCD_TYPE == CN_MODE_GREY_16))

    //以下分配LCD控制器所需的引脚
    GPIO_CfgPinFunc(7,8,cn_p7_8_lcd_d0);
    GPIO_CfgPinFunc(7,9,cn_p7_9_lcd_d1);
    GPIO_CfgPinFunc(7,10,cn_p7_10_lcd_d2);
    GPIO_CfgPinFunc(7,11,cn_p7_11_lcd_d3);
    GPIO_CfgPinFunc(8,10,cn_p8_10_lcd_mclk);
    GPIO_CfgPinFunc(8,11,cn_p8_11_lcd_pclk);
    GPIO_CfgPinFunc(6,0,cn_p6_0_nlcd_ac_enb_cs);
    GPIO_CfgPinFunc(8,8,cn_p8_8_lcd_vsync);
    GPIO_CfgPinFunc(8,9,cn_p8_9_lcd_hsync);

    g_ptSysconfig0Reg->MSTPRI[2] &= 0x0fffffff;      //设置LCD数据传送次高优先级
    g_ptSysconfig0Reg->MSTPRI[2] |= 0x10000000;      //设置LCD数据传送次高优先级
    // Turn raster controller off
    g_ptLcdcReg->RASTER_CTRL       &= 0xfffffffe;

    // Clear status bits
    g_ptLcdcReg->LCD_STAT           = 0x000003ff;

    // PCLK = 1.27MHz, 帧频66，raster mode
    reg = CN_LCD_CLKVAL /(CN_LCD_SIZE_X * CN_LCD_SIZE_Y * 66) * 4;
    g_ptLcdcReg->LCD_CTRL &= ~CN_LCDC_LCD_CTRL_CLKDIV_MASK;
    g_ptLcdcReg->LCD_CTRL |= reg << CN_LCDC_LCD_CTRL_CLKDIV_SHIFT;
    g_ptLcdcReg->LCD_CTRL |= CN_LCDC_LCD_CTRL_MODESEL_MASK;

    g_ptLcdcReg->RASTER_CTRL =
                       (CN_LCDC_RASTER_CTRL_NIB_MODE_ENABLE << CN_LCDC_RASTER_CTRL_NIB_MODE_SHIFT)
                       +(CN_LCDC_RASTER_CTRL_MONO_COLOR_MONOCHROME << CN_LCDC_RASTER_CTRL_MONO_COLOR_SHIFT);


    // HBP = 0; HFP = 0; HSW = 0x3; PPL = 0x13
    g_ptLcdcReg->RASTER_TIMING_0 =
                        (4 << CN_LCDC_RASTER_TIMING_0_HBP_SHIFT)
                       +(4 << CN_LCDC_RASTER_TIMING_0_HFP_SHIFT)
                       +(4 << CN_LCDC_RASTER_TIMING_0_HSW_SHIFT)
                       +((CN_LCD_SIZE_X/16 -1)<< CN_LCDC_RASTER_TIMING_0_PPL_SHIFT);
    // VBP = 0x3; VFP = 0x2; VSW = 0xA: LPP = 0xef;
    g_ptLcdcReg->RASTER_TIMING_1    =
                         (10 << CN_LCDC_RASTER_TIMING_1_VBP_SHIFT)
                        +(9  << CN_LCDC_RASTER_TIMING_1_VFP_SHIFT)
                        +(5  << CN_LCDC_RASTER_TIMING_1_VSW_SHIFT)
                        +((CN_LCD_SIZE_Y -1)<< CN_LCDC_RASTER_TIMING_1_LPP_SHIFT);
    g_ptLcdcReg->RASTER_TIMING_2 =
                         (CN_LCDC_RASTER_TIMING_2_IPC_RISING << CN_LCDC_RASTER_TIMING_2_IPC_SHIFT);

    g_ptLcdcReg->LCDDMA_CTRL        = 0x00000540;
    // Frame buffer start
    g_ptLcdcReg->LCDDMA_FB0_BASE    = (u32)(u8g_dsp_buffer - CN_PALETTE_SIZE);
    // Frame buffer end,不知为何要减1
    g_ptLcdcReg->LCDDMA_FB0_CEILING = (u32)(u8g_dsp_buffer + cn_frame_buffer_size - 1);
    g_ptLcdcReg->RASTER_CTRL       |= 0x00000001;         // Enable controller
#endif

    return;
}

void Lcd_ClearScreen(void)
{
    memset(u8g_dsp_buffer,0,cn_frame_buffer_size);
}
void lcd_back_light_on(void)
{
}

void lcd_back_light_off(void)
{
}
void Lcd_Display_On(void)
{
    Cpucfg_EnableLPSC(cn_PSC1_LCDC);
}

void Lcd_Display_Off(void)
{
    Cpucfg_DisableLPSC(cn_PSC1_LCDC);
}

//========================================================
void Lcd_ContrastSet(u16 u16Contrast)
{
}
// 函数功能：控制液晶对比度增加
// 输入参数：无
// 输出参数：无
// 返回值  ：无
//========================================================
void Lcd_ContrastUp(void)
{
    s_u16Contrast++;
}

//========================================================
// 函数功能：控制液晶对比度下降
// 输入参数：无
// 输出参数：无
// 返回值  ：无
//========================================================
void Lcd_ContrastDown(void)
{
    s_u16Contrast--;
}

bool_t __lcd_set_pixel_bm(struct tagStRectBitmap *bitmap,
                         s32 x,s32 y,u32 color,u32 r2_code)
{
    return false;
}
bool_t __lcd_line_bm(struct tagStRectBitmap *bitmap,struct tagStRect *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
bool_t __lcd_line_bm_ie(struct tagStRectBitmap *bitmap,struct tagStRect *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
bool_t __lcd_fill_rect_bm(struct tagStRectBitmap *dst_bitmap,
                         struct tagStRect *dst_rect,u32 color)
{
    return false;
}
bool_t __lcd_blt_bm_to_bm( struct tagStRectBitmap *dst_bitmap,
                            struct tagStRect *dst_rect,
                            struct tagStRectBitmap *src_bitmap,s32 xsrc,s32 ysrc,
                            u8 *pat,struct tagStRectBitmap *mask,u32 rop_code,
                            u32 transparentcolor)
{
    return false;
}
bool_t __lcd_get_rect_bm(struct tagStRectBitmap *src,struct tagStRect *rect,
                        struct tagStRectBitmap *dest)
{
    return false;
}

bool_t __lcd_set_pixel_screen(s32 x,s32 y,u32 color,u32 r2_code)
{
    u32 byteoffset,bitoffset;
    u8 dest,pixel;
    u8g_dsp_buffer = (u8*)(CN_FRAME_BASE_ADDRESS +CN_PALETTE_SIZE);     //db
    byteoffset = y*cn_lcd_line_size + x/8;
    bitoffset =(7- x%8);              //每像素1bit，字节高bit显示在左边
#if 0
    switch(x%8)                         //液晶模块数据线反了。
    {
        case 0:bitoffset = 4;break;
        case 1:bitoffset = 5;break;
        case 2:bitoffset = 6;break;
        case 3:bitoffset = 7;break;
        case 4:bitoffset = 0;break;
        case 5:bitoffset = 1;break;
        case 6:bitoffset = 2;break;
        case 7:bitoffset = 3;break;
    }
#endif
    pixel = (u8)__GK_ConvertColorToPF(CN_SYS_PF_GRAY1,color);
    if(CN_R2_COPYPEN == r2_code)
    {
        u8g_dsp_buffer[byteoffset] = (pixel<<bitoffset)
                      + (u8g_dsp_buffer[byteoffset] & (~(0x01 << bitoffset)));
    }else
    {
        dest = u8g_dsp_buffer[byteoffset] << bitoffset;
        pixel = GK_BlendRop2(dest, pixel<<bitoffset, r2_code);
        u8g_dsp_buffer[byteoffset] = ((pixel & 0x1)<<bitoffset)
                      + (u8g_dsp_buffer[byteoffset] & (~(0x01 << bitoffset)));
    }
    return true;
}
//在screen中画一条任意直线，不含端点，如硬件加速不支持在screen上画线，或者有
//frame_buffer，driver可以简化，直接返回false即可
//镜像显示器的driver须提供这个函数
bool_t __lcd_line_screen(struct tagStRect *limit,
                    s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}

//在screen中画一条任意直线，含端点，如硬件加速不支持在screen上画线，或者有
//frame_buffer，driver可以简化，直接返回false即可
//镜像显示器的driver须提供这个函数
bool_t __lcd_line_screen_ie(struct tagStRect *limit,
                       s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//screen中矩形填充，如硬件加速不支持在screen上矩形填充，或者有frame_buffer，
//driver可以简化，直接返回false即可
//镜像显示器的driver须提供这个??
bool_t __lcd_fill_rect_screen(struct tagStRect *dst_rect,u32 color)
{
    return false;
}
//从内存缓冲区到screen位块传送，只支持块拷贝，不支持rop操作。
//镜像显示器的driver须提供这个函数
bool_t __lcd_bm_to_screen(struct tagStRect *dst_rect,
            struct tagStRectBitmap *src_bitmap,s32 xsrc,s32 ysrc)
{
    return false;
}

//从screen中取一像素，并转换成cn_sys_pf_e8r8g8b8
u32 __lcd_get_pixel_screen(s32 x,s32 y)
{
    u32 byteoffset,bitoffset;
    byteoffset = y*cn_lcd_line_size + x/8;
    //    bitoffset =(7- x%8);              //每像素1bit，字节高bit显示在左边
    switch(x%8)                         //液晶模块数据线反了。
    {
        case 0:bitoffset = 4;break;
        case 1:bitoffset = 5;break;
        case 2:bitoffset = 6;break;
        case 3:bitoffset = 7;break;
        case 4:bitoffset = 0;break;
        case 5:bitoffset = 1;break;
        case 6:bitoffset = 2;break;
        case 7:bitoffset = 3;break;
    }
    return __GK_ConvertColorTo24(CN_SYS_PF_GRAY1,
                (u8g_dsp_buffer[byteoffset]>>bitoffset)&0x1);
}
//把screen内矩形区域的内容复制到bitmap，调用前，先设置好dest的pf_type
bool_t __lcd_get_rect_screen(struct tagStRect *rect,struct tagStRectBitmap *dest)
{
    return false;
}

//----控制显示器---------------------------------------------------------------
//功能: 这是由driver提供商编写的，用于设置显示器各种参数的函数，属于应用程序的
//      范畴，功能由设备厂商定义。在ut2416中，本函数为空。
//参数: disp，显示器指针
//返回: true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_disp_ctrl(struct tagDisplayRsc *disp)
{
    return true;
}

//----初始化lcd设备------------------------------------------------------------
//功能: 如名
//参数: 无
//返回: 显示器资源指针
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_Lcd(ptu32_t para)
{
    static struct tagGkWinRsc frame_win;

    __lcd_hard_init( );
    frame_win.wm_bitmap.bm_bits = u8g_dsp_buffer;
//    frame_win.wm_bitmap.bm_bits = u8g_frame_buffer;
    frame_win.wm_bitmap.width = LCD_XSIZE;
    frame_win.wm_bitmap.height = LCD_YSIZE;
    frame_win.wm_bitmap.pf_type = cn_lcd_pf;
    frame_win.wm_bitmap.linebytes = cn_lcd_line_size;
    tg_lcd_display.frame_buffer = &frame_win;

    tg_lcd_display.xmm = 0;
    tg_lcd_display.ymm = 0;
    tg_lcd_display.width = LCD_XSIZE;
    tg_lcd_display.height = LCD_YSIZE;
    tg_lcd_display.patx = 8;
    tg_lcd_display.paty = 8;
    tg_lcd_display.pixel_format = CN_SYS_PF_GRAY1;
    tg_lcd_display.framebuf_direct = true;
    //无须初始化frame_buffer和desktop，z_topmost三个成员

    tg_lcd_display.draw.set_pixel_bm = __lcd_set_pixel_bm;
    tg_lcd_display.draw.line_bm = __lcd_line_bm;
    tg_lcd_display.draw.line_bm_ie = __lcd_line_bm_ie;
    tg_lcd_display.draw.fill_rect_bm = __lcd_fill_rect_bm;
    tg_lcd_display.draw.blt_bm_to_bm = __lcd_blt_bm_to_bm;
    tg_lcd_display.draw.get_pixel_bm = NULL;
    tg_lcd_display.draw.get_rect_bm = __lcd_get_rect_bm;
    tg_lcd_display.draw.set_pixel_screen = __lcd_set_pixel_screen;
    tg_lcd_display.draw.line_screen = __lcd_line_screen;
    tg_lcd_display.draw.line_screen_ie = __lcd_line_screen_ie;
    tg_lcd_display.draw.fill_rect_screen = __lcd_fill_rect_screen;
    tg_lcd_display.draw.bm_to_screen = __lcd_bm_to_screen;
    tg_lcd_display.draw.get_pixel_screen = __lcd_get_pixel_screen;
    tg_lcd_display.draw.get_rect_screen = __lcd_get_rect_screen;

//    tg_lcd_display.bmmalloc = lcd_bmmalloc;
    tg_lcd_display.vmalloc = NULL;
    tg_lcd_display.vrmalloc = NULL;
    tg_lcd_display.vfree = NULL;
    tg_lcd_display.disp_ctrl = __lcd_disp_ctrl;

    GK_InstallDisplay(&tg_lcd_display,(char*)para);
    return (ptu32_t)&tg_lcd_display;
}


