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
#include "string.h"
#include "stdint.h"
#include "stdlib.h"

#include <gui/gkernel/gk_display.h>
#include "board-config.h"

#include "cpu_peri.h"
#include "cpu_peri_lcd.h"


static u32 cfg_LcdPf = CN_SYS_PF_ERGB8888;

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
static u8 *pg_video_buf,*pg_frame_buffer;


void __lcd_gpio_init(void)
{
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(4),PORT_PINMUX_ALT7); // D[0]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(5),PORT_PINMUX_ALT7); // D[1]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(6),PORT_PINMUX_ALT7); // D[2]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(7),PORT_PINMUX_ALT7); // D[3]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(8),PORT_PINMUX_ALT7); // D[4]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(9),PORT_PINMUX_ALT7); // D[5]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(10),PORT_PINMUX_ALT7); // D[6]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(11),PORT_PINMUX_ALT7); // D[7]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(12),PORT_PINMUX_ALT7); // D[8]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(13),PORT_PINMUX_ALT7); // D[9]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(14),PORT_PINMUX_ALT7); // D[10]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(15),PORT_PINMUX_ALT7); // D[11]

    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(16),PORT_PINMUX_ALT5); // D[12]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(17),PORT_PINMUX_ALT5); // D[13]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(18),PORT_PINMUX_ALT5); // D[14]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(19),PORT_PINMUX_ALT5); // D[15]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(20),PORT_PINMUX_ALT5); // D[16]

    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(21),PORT_PINMUX_ALT7); // D[17]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(22),PORT_PINMUX_ALT7); // D[18]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(23),PORT_PINMUX_ALT7); // D[19]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(24),PORT_PINMUX_ALT7); // D[20]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(25),PORT_PINMUX_ALT7); // D[21]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(26),PORT_PINMUX_ALT7); // D[22]
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(27),PORT_PINMUX_ALT7); // D[23]

    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(0),PORT_PINMUX_ALT7); // PCLK
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(1),PORT_PINMUX_ALT7); // DE
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(2),PORT_PINMUX_ALT7); // HSYNC
    PORT_MuxConfig(PORT_PORT_F,PORT_PIN(3),PORT_PINMUX_ALT7); // VSYNC
}

void __lcd_controller_init(void)
{
    // Disable MPU
    MPU->CESR &= ~MPU_CESR_VLD_MASK;

    // Enable LCDC and DDR Modules
    SIM->SCGC3 |= SIM_SCGC3_LCDC_MASK;

    // set LCD Screen Start Address
    LCDC->LSSAR = FRAME_START_ADDRESS;
    // set LCD Size. The XMAX bitfield is the screen x-size/16.
    LCDC->LSR = LCDC_LSR_XMAX( (CN_LCD_XSIZE / 16) ) | LCDC_LSR_YMAX( CN_LCD_YSIZE );
    // set LCD virtual page width
    LCDC->LVPWR = LCDC_LVPWR_VPW( CN_LCD_XSIZE );
    // set LCD cursor positon & settings (turn off)
    LCDC->LCPR = 0;
    LCDC->LCWHB = 0;

    // set LCD panel configuration. Use endianess to work with TWR-LCD-RGB lines.
    LCDC->LPCR =
    LCDC_LPCR_TFT_MASK      |       //TFT Screen
    LCDC_LPCR_COLOR_MASK    |       //Color
    LCDC_LPCR_BPIX(0x7)     |       //24 bpp
    LCDC_LPCR_FLMPOL_MASK   |       //first line marker active low (VSYNC)
    LCDC_LPCR_LPPOL_MASK    |       //line pulse active low (HSYNC)
    LCDC_LPCR_END_SEL_MASK  |       //Use big-endian mode (0xFFAA5500 means R=AA,G=55,B=00).
//  LCDC_LPCR_SWAP_SEL_MASK |       //Set if needed for LCD data lines match up correctly with the LCD
    LCDC_LPCR_SCLKIDLE_MASK |       //Enalbe LSCLK when vsync is idle
    LCDC_LPCR_SCLKSEL_MASK  |       //Always enable clock
    LCDC_LPCR_PCD(11);             //Divide 120 PLL0 clock (default clock) by (11+1)=12 to get 10MHz clock

    #define REVE
    //If RevE or later TWR-LCD-RGB, need to adjust clock settings
    #ifdef REVE

    LCDC->LPCR |= LCDC_LPCR_CLKPOL_MASK;    //In TFT mode, active on negative edge of LSCLK.

    // set LCD horizontal configuration based on panel data (Figure 3-3 in Seiko datasheet)
    LCDC->LHCR =
    LCDC_LHCR_H_WIDTH(41)   |    //(41+1)=42 SCLK period for HSYNC activated
    LCDC_LHCR_H_WAIT_1(1)  |    //(1+1)=2 SCLK period between end of OE and beginning of HSYNC
    LCDC_LHCR_H_WAIT_2(0);     //(0+3)=3 SCLK periods between end of HSYNC and beginning of OE

    // set LCD vertical configuration based on panel data (Figure 3-3 in Seiko datasheet)
    LCDC->LVCR =
    LCDC_LVCR_V_WIDTH(2)  |   //2 lines period for VSYNC activated
    LCDC_LVCR_V_WAIT_1(1) |    //1 line period between end of OE and beginning of VSYNC
    LCDC_LVCR_V_WAIT_2(1);    //1 line periods between end of VSYNC and beginning of OE
    //Earlier TWR-LCD-RGB versions (B, C, and D)
    #else
    LCDC->LHCR =
    LCDC_LHCR_H_WIDTH(40)   |    //(40+1)=41 SCLK period for HSYNC activated
    LCDC_LHCR_H_WAIT_1(1)  |    //(1+1)=2 SCLK period between end of OE and beginning of HSYNC
    LCDC_LHCR_H_WAIT_2(0);     //(0+3)=3 SCLK periods between end of HSYNC and beginning of OE

    // set LCD vertical configuration based on panel data (Figure 3-3 in Seiko datasheet)
    LCDC->LVCR =
    LCDC_LVCR_V_WIDTH(10)  |   //10 lines period for VSYNC activated
    LCDC_LVCR_V_WAIT_1(2) |    //2 line period between end of OE and beginning of VSYNC
    LCDC_LVCR_V_WAIT_2(2);    //2 line periods between end of VSYNC and beginning of OE
    #endif

    // set the LCD panning offset (not used in 24bpp mode)
    LCDC->LPOR = 0;

    // set LCD interrupt configuration register
    LCDC->LICR = 0;

    //Disable LCDC interrupts
    LCDC->LIER = 0;

    //Disable the graphic window. See the "color" and "fsl" demos for examples of
    //  using the graphic window feature
    LCDC->LGWCR &=~LCDC_LGWCR_GWE_MASK;

    //Set background plane DMA to burst mode
    LCDC->LDCR&=~LCDC_LDCR_BURST_MASK;

    //Set graphic window DMA to burst mode
    LCDC->LGWDCR&=~LCDC_LGWDCR_GWBT_MASK;
}
//----初始化lcd硬件------------------------------------------------------------
//功能: 初始化lcd硬件。
//参数: 无
//返回: 无
//备注: 本函数是移植关键函数
//----------------------------------------------------------------------------
void __lcd_hard_init(void)
{
    pg_video_buf = (u8*)FRAME_START_ADDRESS;
    pg_frame_buffer = (u8*)FRAME_START_ADDRESS;

    __lcd_gpio_init();
    __lcd_controller_init();

}
//----LCD控制信号------------------------------------------------------------
//功能: LCD 视频输出和逻辑使能/禁止，1=允许视频输出和 LCD控制信号，
//      0=禁止视频输出和 LCD控制信号。
//参数: onoff，控制信号
//返回: 无
//-----------------------------------------------------------------------------
void __lcd_envid_of(int onoff)
{
    if(onoff == 1)
        SIM->MCR |=SIM_MCR_LCDSTART_MASK;  //Start LCDC
    else
        SIM->MCR &= ~SIM_MCR_LCDSTART_MASK;  // End LCDC
}
//----控制LCD电源--------------------------------------------------------------
//功能: LCD电源控制引脚使能。
//参数: invpwren，表明PWREN信号极性
//      pwren，LCD_PWREN输出信号使能/禁止
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
bool_t __lcd_disp_ctrl(struct DisplayRsc *disp)
{
    return true;
}

//----位图中画像素-------------------------------------------------------------
//功能: 在位图中画一个像素，只有在bitmap的像素格式为cn_custom_pf时，才需要绘制。
//      如果显卡不打算支持自定义格式，本函数直接返回。
//参数: bitmap，目标位图
//      limit，限制矩形，只绘制在该矩形内部的部分
//      x、y，坐标
//      color，绘图用的颜色，cn_sys_pf_e8r8g8b8格式
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
//      limit，限制矩形，只绘制在该矩形内部的部分
//      x1、y1、x2、y2，起点终点坐标
//      color，绘图用的颜色，cn_sys_pf_e8r8g8b8格式
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=失败，无硬件加速或不支持按r2_code画线
//-----------------------------------------------------------------------------
bool_t __lcd_line_bm(struct RectBitmap *bitmap,struct Rectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//----位图中画线(含端点)-------------------------------------------------------
//功能: 在位图中画一条直线，只绘制在limit限定的区域内的部分。
//参数: bitmap，目标位图
//      limit，限制矩形，只绘制在该矩形内部的部分
//      x1、y1、x2、y2，起点终点坐标
//      color，绘图用的颜色，cn_sys_pf_e8r8g8b8格式
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=失败，无硬件加速或不支持按r2_code画线
//-----------------------------------------------------------------------------
bool_t __lcd_line_bm_ie(struct RectBitmap *bitmap,struct Rectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//----位图中填充矩形-----------------------------------------------------------
//功能: 把位图中的矩形用color颜色填充。
//参数: bitmap，目标位图
//      dst_rect，待填充的矩形
//      color，填充颜色，cn_sys_pf_e8r8g8b8格式
//返回: true=成功绘制，false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_fill_rect_bm(struct RectBitmap *dst_bitmap,
                          struct Rectangle *Target,
                          struct Rectangle *Focus,
                          u32 Color0,u32 Color1,u32 Mode)
{
    u32 x,y;
    u32 pixel;
    u32 *dst,*p;
    s32 m,n;
    if(Mode != CN_FILLRECT_MODE_N)
        return false;

    if(dst_bitmap->PixelFormat != CN_SYS_PF_ERGB8888)
        return false;

    pixel = GK_ConvertRGB24ToPF(CN_SYS_PF_ERGB8888,Color0);

    dst = (u32*)((ptu32_t)dst_bitmap->bm_bits
                              + Focus->top * dst_bitmap->linebytes);
    dst += Focus->left;

    for(y = Focus->top; y < Focus->bottom; y++)
    {
        p =dst;
        n=(Focus->right-Focus->left+7)/8;
        m=(Focus->right-Focus->left)%8;
        for(x=0; x<n;x++)
        {
           switch (m)
           {
           	   case 0:*p++ =pixel;
			   case 7:*p++ =pixel;
			   case 6:*p++ =pixel;
			   case 5:*p++ =pixel;
			   case 4:*p++ =pixel;
			   case 3:*p++ =pixel;
			   case 2:*p++ =pixel;
			   case 1:*p++ =pixel;
           }
           m=0;
        }
        dst += dst_bitmap->linebytes >> 2;
    }
    return true;
}
//----bitmap中位块传送---------------------------------------------------------
//功能: 把一个位图中的一个矩形区域传送到另一个区域，两个位图的像素格式不必相同，
//      源和目的位图可以相同，
//参数: dst_bitmap，目标位图
//      dst_rect，目标矩形
//      src_bitmap，源位图
//      xsrc、ysrc，源位图中被传送的区域左上角坐标
//      pat，pattern指针
//      bitmsk，rop4操作需要的掩码
//      rop_code，三元光栅操作码
//      transparentcolor，透明色
//返回: true=成功,false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_blt_bm_to_bm( struct RectBitmap *dst_bitmap,
                            struct Rectangle *DstRect,
                            struct RectBitmap *src_bitmap,
                            struct Rectangle *SrcRect,
                            struct RopGroup RopCode, u32 HyalineColor)
{
    u32 *src_offset,*dst_offset;    //源位图点阵缓冲区可能不对齐!!!
    u32 y;
    struct RopGroup Rop = { 0, 0, 0, CN_R2_COPYPEN, 0, 0, 0  };

    if((dst_bitmap->PixelFormat != CN_SYS_PF_ERGB8888)
             ||(src_bitmap->PixelFormat != CN_SYS_PF_ERGB8888)
             ||(memcmp(&RopCode, &Rop ,sizeof(struct RopGroup))!=0))
    {
        return false;
    }
    else
    {
        if((ptu32_t)src_bitmap->bm_bits & 1)    //源位图缓冲区非对齐模型
        {
        }else
        {
            dst_offset = (u32*)((ptu32_t)dst_bitmap->bm_bits
                                      + DstRect->top * dst_bitmap->linebytes);
            dst_offset += DstRect->left;

            src_offset = (u32*)((ptu32_t)src_bitmap->bm_bits
                                      + SrcRect->top * src_bitmap->linebytes);
            src_offset += SrcRect->left;

            for(y = DstRect->top; y < DstRect->bottom; y++)
            {
                memcpy(dst_offset,src_offset,(DstRect->right-DstRect->left)<<2);
                dst_offset += dst_bitmap->linebytes >> 2;
                src_offset += src_bitmap->linebytes >> 2;
            }
        }
    }
    return true;
}
//----读取位图中的像素---------------------------------------------------------
//功能: 取位图中的一个像素，只有在bitmap的像素格式为cn_custom_pf时，才需要读取。
//      如果显卡不打算支持自定义格式，本函数直接返回false。
//参数: bitmap，目标位图
//      x、y，坐标
//返回: 读取的像素值，cn_sys_pf_e8r8g8b8格式
//-----------------------------------------------------------------------------
u32 __lcd_get_pixel_bm(struct RectBitmap *bitmap,s32 x,s32 y)
{
    return 0;
}
//----screen中画像素-----------------------------------------------------------
//功能: 在screen中画一个像素.
//参数: x、y，像素坐标
//      color，颜色
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=没有绘制，或无需绘制
//-----------------------------------------------------------------------------
bool_t __lcd_set_pixel_screen(s32 x,s32 y,u32 color,u32 rop2_code)
{
    u32 dest,pen;
    u32 byteoffset;
    byteoffset = y*CN_LCD_XSIZE + x;
    pen = GK_ConvertRGB24ToPF(CN_SYS_PF_ERGB8888,color);
    dest = (u32)pg_video_buf[byteoffset];
    dest = GK_BlendRop2(dest,pen,rop2_code);
    pg_video_buf[byteoffset] = (u32)dest;
    return true;
}
//----screen中画线---------------------------------------------------------------
//功能: 在screen中画一条任意直线，不含端点，只绘制在limit限定的区域内的部分。如
//      不支持画线的硬件加速，直接返回false。
//参数: limit，限制矩形，只绘制在该矩形内部的部分
//      x1、y1、x2、y2，起点终点坐标
//      color，绘图用的颜色
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=失败，无硬件加速或不支持按r2_code画线
//-----------------------------------------------------------------------------
bool_t __lcd_line_screen(struct Rectangle *limit,
                    s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//----screen中画线(含端点)------------------------------------------------------
//功能: 在screen中画一条直线，只绘制在limit限定的区域内的部分。
//参数: limit，限制矩形，只绘制在该矩形内部的部分
//      x1、y1、x2、y2，起点终点坐标
//      color，绘图用的颜色，CN_SYS_PF_ERGB8888格式
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=失败，无硬件加速或不支持按r2_code画线
//-----------------------------------------------------------------------------
bool_t __lcd_line_screen_ie(struct Rectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//----screen中填充矩形-----------------------------------------------------------
//功能: 把screen中的矩形用color颜色填充，color的格式是CN_SYS_PF_ERGB8888。
//参数: dst_rect，待填充的矩形
//      color，填充颜色
//返回: true=成功绘制，false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_fill_rect_screen(struct Rectangle *Target,
                              struct Rectangle *Focus,
                              u32 Color0,u32 Color1,u32 Mode)
{
//  int x,y;
//    u32 pen;
//    u32 *p;
//
//    if(Mode != CN_FILLRECT_MODE_N)
//        return false;
//    pen = GK_ConvertRGB24ToPF(CN_SYS_PF_ERGB8888,Color0);
//
//    for(y=Focus->top;y<Focus->bottom;y++)
//    {
//      p =&pg_video_buf[y*cfg_LcdxSize];
//
//      for(x=Focus->left;x<Focus->right;x++)
//      {
//          *p++ =pen;
//      }
//    }
//    return true;
    return false;
}

//----bitmap到screen位块传送---------------------------------------------------
//功能: 把一个位图中的一个矩形区域传送到另一个区域，两个位图的像素格式必须相同，
//      源和目的位图可以相同。
//参数: dst_rect，目标矩形
//      src_bitmap，源位图
//      xsrc、ysrc，源位图中被传送的区域左上角坐标(以源位图左上角坐标为原点)
//返回: true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_bm_to_screen(struct Rectangle *dst_rect,
                struct RectBitmap *src_bitmap,s32 xsrc,s32 ysrc)
{
    u32 size,double_width_src;
    u32 byteoffset_rect,byteoffset_bitmap;
    s32 y;

    double_width_src = src_bitmap->width*2;     //源位图一行所占的字节数
    //每一行要被传送的字节数
    size = (dst_rect->right-dst_rect->left)*(cfg_LcdPf>>8)/8;
    //源位图左上角坐标的字节偏移量
    byteoffset_bitmap = ysrc*double_width_src + xsrc*2;
    //目标矩形左上角坐标的双字节偏移量，因为pg_video_buf为u16的指针
    //因为是传送到screen上，所以每行双字节数取LCD_XSIZE
    byteoffset_rect = dst_rect->top*CN_LCD_XSIZE + dst_rect->left;

    //bitmap到screen位块传送
    for(y = dst_rect->top;y < dst_rect->bottom;y++)
    {
        memcpy(&(pg_video_buf[byteoffset_rect]),
                    &src_bitmap->bm_bits[byteoffset_bitmap],size);
        byteoffset_bitmap += double_width_src;
        byteoffset_rect += CN_LCD_XSIZE;
    }
    return true;
}
//----从screen中取像素---------------------------------------------------------
//功能: 从screen中取一像素，并转换成CN_SYS_PF_ERGB8888或cn_sys_pf_a8r8g8b8格式。
//参数: x、y，坐标
//返回: 像素颜色值
//-----------------------------------------------------------------------------
u32 __lcd_get_pixel_screen(s32 x,s32 y)
{
    u32 color,offset;

    offset = LCDC->LSSAR;
    offset += y*CN_LCD_XSIZE*2;
    offset += x*2;
    color = (u32)(*(u32*)offset);
    //R5G6B5
//    r = (color>>11) &0x1f;
//    g = (color>>5) & 0x3f;
//    b = color & 0x1f;
//    return (r<<19) | (g<<10) | (b<<3);
    //R8G8B8
    return color;
}

//----读取bitmap中矩形块-------------------------------------------------------
//功能: 把一个位图中的矩形读取到另一个位图中。
//参数: rect，欲读取的矩形
//      dest，保存矩形的位图，其长宽必须与rect相同
//返回: true=成功读取，false=失败，原因可能是提供了不支持的像素格式
//-----------------------------------------------------------------------------
bool_t __lcd_get_rect_screen(struct Rectangle *rect,struct RectBitmap *dest)
{
    return false;
}

//----初始化lcd设备------------------------------------------------------------
//功能: 初始化lcd设备。
//参数: 无
//返回: 显示器资源指针
//-----------------------------------------------------------------------------
struct DisplayRsc* ModuleInstall_LCD(s32 width,s32 height,const char *DisplayName,const char* HeapName)
{

    static struct GkWinRsc frame_win;
    static struct RectBitmap FrameBitmap;

    __lcd_hard_init();
    __lcd_power_enable(0,1);
    __lcd_envid_of(1);

    FrameBitmap.bm_bits = (u8*)pg_frame_buffer;
    FrameBitmap.width = width;
    FrameBitmap.height = height;
    FrameBitmap.PixelFormat = cfg_LcdPf;
    FrameBitmap.linebytes = width*4;
    FrameBitmap.ExColor = 0;
    frame_win.wm_bitmap = &FrameBitmap;
    tg_lcd_display.frame_buffer = &frame_win;

    tg_lcd_display.xmm = 0;
    tg_lcd_display.ymm = 0;
    tg_lcd_display.width = width;
    tg_lcd_display.height = height;
    tg_lcd_display.pixel_format = CN_SYS_PF_ERGB8888;
    tg_lcd_display.reset_clip = false;
    tg_lcd_display.framebuf_direct = true;
    //无须初始化frame_buffer和desktop，z_topmost三个成员

    tg_lcd_display.draw.SetPixelToBitmap = __lcd_set_pixel_bm;
    tg_lcd_display.draw.FillRectToBitmap = __lcd_fill_rect_bm;
    tg_lcd_display.draw.LineToBitmap = __lcd_line_bm;
    tg_lcd_display.draw.LineToBitmapIe = __lcd_line_bm_ie;
    tg_lcd_display.draw.BltBitmapToBitmap = __lcd_blt_bm_to_bm;
    tg_lcd_display.draw.SetPixelToScreen = __lcd_set_pixel_screen;
    tg_lcd_display.draw.LineToScreen = __lcd_line_screen;
    tg_lcd_display.draw.LineToScreenIe = __lcd_line_screen_ie;
    tg_lcd_display.draw.FillRectToScreen = __lcd_fill_rect_screen;
    tg_lcd_display.draw.CopyBitmapToScreen = __lcd_bm_to_screen;
    tg_lcd_display.draw.GetPixelFromScreen = __lcd_get_pixel_screen;
    tg_lcd_display.draw.GetRectFromScreen = __lcd_get_rect_screen;

    tg_lcd_display.DisplayHeap = (struct HeapCB *)M_FindHeap(HeapName);
    tg_lcd_display.disp_ctrl = __lcd_disp_ctrl;

    if(GK_InstallDisplay(&tg_lcd_display,(char*)DisplayName))
        return (ptu32_t)&tg_lcd_display;
    else
        return (ptu32_t)NULL;
}

