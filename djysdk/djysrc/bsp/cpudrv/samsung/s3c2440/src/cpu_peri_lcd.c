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
#include "stdlib.h"
#include "stddef.h"
#include "string.h"
#include "gkernel.h"
#include "gk_display.h"
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
#define M5D(n)          ((n) & 0x1fffff)    //获得低21位
struct tagDisplayRsc tg_lcd_display;
//定义显存，共定义了一个显存和一个帧缓冲，16是对齐的开销
static u8 u8g_dsp_buffer[cn_frame_buffer_size + 16];
static u8 u8g_video_buffer[cn_frame_buffer_size + 16];
static u16 *pg_frame_buffer,*pg_video_buf;
//volatile unsigned short LCD_BUFFER[SCR_YSIZE_TFT][SCR_XSIZE_TFT];

u32 GK_GetPixelBm(struct tagRectBitmap *bitmap, s32 x,s32 y);

//----初始化lcd硬件------------------------------------------------------------
//功能: 初始化lcd硬件。
//参数: 无
//返回: 无
//备注: 本函数是移植关键函数
//----------------------------------------------------------------------------
void __lcd_hard_init(void)
{

    struct gpio_reg *gpio =(struct gpio_reg*)GPIO_REG_BASE;
    struct lcd_reg *lcd =(struct lcd_reg*)LCD_REG_BASE;

    //端口 C 的上拉使能寄存器 ,0,使能附加上拉功能到相应端口引脚
    gpio->GPCUP = 0x00000000;
    //GPC4 ,VM;GPC3 VFRAME;GPC2  VLINE;GPC1 VLINE;GPC0 输出。
    //Initialize VD[7:0]，16-31位,VD[23:0] ,（LCD 像素数据输出端口）
    gpio->GPCCON = 0xaaaa02a9;
    gpio->GPDUP = 0x00000000;
    gpio->GPDCON = 0xaaaaaaaa; //Initialize VD[23:8]，0-31位。
#if 1
  //  alignbuf = (u8*)(((u32)u8g_dsp_buffer +15)&0xfffffff0);

    /*
    pg_frame_buffer = (u16*)align_up(16,(ptu32_t)u8g_dsp_buffer);//显存要求16字节对齐
    pg_video_buf = (u16*)align_up(16,(ptu32_t)u8g_video_buffer); //显存要求16字节对齐
     */

    pg_frame_buffer = (u8*)(((u32)u8g_dsp_buffer +15)&0xfffffff0);
    pg_video_buf = (u8*)(((u32)u8g_video_buffer +15)&0xfffffff0);


#endif
    lcd->LCDCON1 = (LCDCON1_CLKVAL<<8)|(LCDCON1_MMODE<<7)|(3<<5)
                            |(12<<1)|0;
    lcd->LCDCON2 = (LCDCON2_VBPD<<24)|(LINEVAL_TFT<<14)|(LCDCON2_VFPD<<6)
                            |(LCDCON2_VSPW);
    lcd->LCDCON3 = (LCDCON3_HBPD<<19)|(HOZVAL_TFT<<8)|(LCDCON3_HFPD);
    lcd->LCDCON4 = (LCDCON4_MVAL<<8)|(LCDCON4_HSPW);
    lcd->LCDCON5 = (1<<11) | (1<<10) | (1<<9) | (1<<8) | (0<<7)
                 | (0<<6) | (1<<3)  |(LCDCON5_BSWP<<1) | (LCDCON5_HWSWP);
    lcd->LCDSADDR1 = (((u32)pg_video_buf>>22)<<21)
                                |M5D((uint32_t)pg_video_buf>>1);
    lcd->LCDSADDR2 = M5D(((u32)pg_video_buf
                            +(cn_lcd_xsize*cn_lcd_ysize*2))>>1);
    lcd->LCDSADDR3 = (((cn_lcd_xsize-cn_lcd_xsize)/1)<<11)
                                |(cn_lcd_xsize/1);
    lcd->LCDINTMSK |= (3);         //LCD中断屏蔽寄存器
    lcd->TCONSEL &= (~7) ;       //TCON控制寄存器
    lcd->TPAL = 0;                 //临时调色板寄存器
}

//----LCD控制信号------------------------------------------------------------
//功能: LCD 视频输出和逻辑使能/禁止，1=允许视频输出和 LCD控制信号，
//      0=禁止视频输出和 LCD控制信号。
//参数: onoff，控制信号
//返回: 无
//-----------------------------------------------------------------------------
void __lcd_envid_of(int onoff)
{
	struct lcd_reg *lcd =(struct lcd_reg*)LCD_REG_BASE;

    if(onoff == 1)
    	lcd->LCDCON1 |= 1;   //允许视频输出和 LCD控制信号
    else                        //禁止视频输出和 LCD控制信号
    	lcd->LCDCON1 = lcd->LCDCON1 & 0x3fffe;
}
//----控制LCD电源--------------------------------------------------------------
//功能: LCD电源控制引脚使能。
//参数: invpwren，表明PWREN信号极性
//      pwren，LCD_PWREN输出信号使能/禁止
//返回: 无
//-----------------------------------------------------------------------------
void __lcd_power_enable(int invpwren,int pwren)
{
	struct gpio_reg *gpio =(struct gpio_reg*)GPIO_REG_BASE;
	struct lcd_reg *lcd =(struct lcd_reg*)LCD_REG_BASE;

	gpio->GPGUP  = (gpio->GPGUP & (~(1<<4))) | (1<<4); //上拉使能
	gpio->GPGCON = (gpio->GPGCON & (~(3<<8))) | (3<<8);
	gpio->GPGDAT = gpio->GPGDAT | (1<<4) ;

    lcd->LCDCON5 = (lcd->LCDCON5 & (~(1<<3))) | (pwren<<3);
    lcd->LCDCON5 = (lcd->LCDCON5 & (~(1<<5))) |(invpwren<<5);
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
//----格式化pattern------------------------------------------------------------
//功能: 把用户提交的作为pattern的bitmap格式化成driver能识别的格式。
//参数: pattern，保存结果的指针
//      bitmap，用户提供的位图
//返回: true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_format_pattern(u8 *pattern,struct tagRectBitmap *bitmap)
{
    s32 x,y;
    u32 color,bytes;

    if(bitmap->PixelFormat == cn_lcd_pf)
        return true;
    bytes = 0;
    for(y = 0;y < bitmap->height;y++)
    {
        for(x = 0;x < bitmap->width;x++)
        {
            color = GK_GetPixelBm(bitmap,x,y);
            color = GK_ConvertPF2PF(cn_lcd_pf,bitmap->PixelFormat,color,NULL);
            pattern[bytes] = (u8)color;
            pattern[bytes+1] = (u8)(color>>8);
            bytes += 2;
        }
    }
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
bool_t __lcd_set_pixel_bm(struct tagRectBitmap *bitmap,
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
bool_t __lcd_line_bm(struct tagRectBitmap *bitmap,struct tagRectangle *limit,
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
bool_t __lcd_line_bm_ie(struct tagRectBitmap *bitmap,struct tagRectangle *limit,
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
bool_t __lcd_fill_rect_bm(struct tagRectBitmap *dst_bitmap,
                          struct tagRectangle *Target,
                          struct tagRectangle *Focus,
                          u32 Color0,u32 Color1,u32 Mode)
{
    s32 x, y;
    u16 pixel;
    u16 *dst,*p;

    if(Mode != CN_FILLRECT_MODE_N)
        return false;

    if(dst_bitmap->PixelFormat != CN_SYS_PF_RGB565)
        return false;

    pixel = GK_ConvertRGB24ToPF(CN_SYS_PF_RGB565,Color0);

    dst = (u16*)((u32)dst_bitmap->bm_bits
                      + Focus->top * dst_bitmap->linebytes);
    dst += Focus->left;

    for(y = Focus->top; y < Focus->bottom; y++)
    {
        p =dst;
        for(x=Focus->left; x<Focus->right;x++)
        {
            *p++ =pixel;
        }
        dst += dst_bitmap->linebytes >> 1;
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
bool_t __lcd_blt_bm_to_bm(struct tagRectBitmap *dst_bitmap,
                        struct tagRectangle *DstRect,
                        struct tagRectBitmap *src_bitmap,
						struct tagRectangle *SrcRect,
						u32 RopCode,u32 transparentcolor)
{
    u16 *src_offset,*dst_offset;    //源位图点阵缓冲区可能不对齐!!!
    u32 y;
    if((dst_bitmap->PixelFormat != CN_SYS_PF_RGB565)
      ||(src_bitmap->PixelFormat != CN_SYS_PF_RGB565)
      ||(RopCode != 0))
    {
        return false;
    }
    else
    {
        if((ptu32_t)src_bitmap->bm_bits & 1)    //源位图缓冲区非对齐模型
        {
        }else
        {
            dst_offset = (u16*)((ptu32_t)dst_bitmap->bm_bits
                                      + DstRect->top * dst_bitmap->linebytes);
            dst_offset += DstRect->left;

            src_offset = (u16*)((ptu32_t)src_bitmap->bm_bits
                                      + SrcRect->top * src_bitmap->linebytes);
            src_offset += SrcRect->left;

            for(y = DstRect->top; y < DstRect->bottom; y++)
            {
                memcpy(dst_offset,src_offset,(DstRect->right-DstRect->left)<<1);
                dst_offset += dst_bitmap->linebytes >> 1;
                src_offset += src_bitmap->linebytes >> 1;
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
u32 __lcd_get_pixel_bm(struct tagRectBitmap *bitmap,s32 x,s32 y)
{
    return 0;
}
//----读取bitmap中矩形块-------------------------------------------------------
//功能: 把一个位图中的矩形读取到另一个位图中。
//参数: src，源位图
//      rect，欲读取的矩形
//      dest，保存矩形的位图，其长宽必须与rect相同
//返回: true=成功读取，false=失败，原因可能是提供了不支持的像素格式
//-----------------------------------------------------------------------------
bool_t __lcd_get_rect_bm(struct tagRectBitmap *src,struct tagRectangle *rect,
                     struct tagRectBitmap *dest)
{
    return false;
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
    byteoffset = y*cn_lcd_xsize + x;
    pen = GK_ConvertRGB24ToPF(CN_SYS_PF_RGB565,color);
    dest = (u32)pg_video_buf[byteoffset];
    dest = GK_BlendRop2(dest,pen,rop2_code);
    pg_video_buf[byteoffset] = (u16)dest;
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
bool_t __lcd_line_screen(struct tagRectangle *limit,
                    s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//----screen中画线(含端点)------------------------------------------------------
//功能: 在screen中画一条直线，只绘制在limit限定的区域内的部分。
//参数: limit，限制矩形，只绘制在该矩形内部的部分
//      x1、y1、x2、y2，起点终点坐标
//      color，绘图用的颜色，cn_sys_pf_e8r8g8b8格式
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=失败，无硬件加速或不支持按r2_code画线
//-----------------------------------------------------------------------------
bool_t __lcd_line_screen_ie(struct tagRectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//----screen中填充矩形-----------------------------------------------------------
//功能: 把screen中的矩形用color颜色填充，color的格式是cn_sys_pf_e8r8g8b8。
//参数: dst_rect，待填充的矩形
//      color，填充颜色
//返回: true=成功绘制，false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_fill_rect_screen(struct tagRectangle *Target,
                              struct tagRectangle *Focus,
                              u32 Color0,u32 Color1,u32 Mode)
{
    int x,y;
    u32 pen;
    u16 *p;
    return false;

    if(Mode != CN_FILLRECT_MODE_N)
        return false;
    pen = __GK_ConvertColorToPF(CN_SYS_PF_RGB565,Color0);

    for(y=Focus->top;y<Focus->bottom;y++)
    {
        p =&pg_video_buf[y*cn_lcd_xsize];

        for(x=Focus->left;x<Focus->right;x++)
        {
            *p++ =pen;
        }
    }
    return true;
}

//----bitmap到screen位块传送---------------------------------------------------
//功能: 把一个位图中的一个矩形区域传送到另一个区域，两个位图的像素格式必须相同，
//      源和目的位图可以相同。
//参数: dst_rect，目标矩形
//      src_bitmap，源位图
//      xsrc、ysrc，源位图中被传送的区域左上角坐标(以源位图左上角坐标为原点)
//返回: true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_bm_to_screen(struct tagRectangle *dst_rect,
                struct tagRectBitmap *src_bitmap,s32 xsrc,s32 ysrc)
{
    u32 size,double_width_src;
    u32 byteoffset_rect,byteoffset_bitmap;
    s32 y;

    double_width_src = src_bitmap->width*2;     //源位图一行所占的字节数
    //每一行要被传送的字节数
    size = (dst_rect->right-dst_rect->left)*(cn_lcd_pf>>8)/8;
    //源位图左上角坐标的字节偏移量
    byteoffset_bitmap = ysrc*double_width_src + xsrc*2;
    //目标矩形左上角坐标的双字节偏移量，因为pg_video_buf为u16的指针
    //因为是传送到screen上，所以每行双字节数取cn_lcd_xsize
    byteoffset_rect = dst_rect->top*cn_lcd_xsize + dst_rect->left;

    //bitmap到screen位块传送
    for(y = dst_rect->top;y < dst_rect->bottom;y++)
    {
        memcpy(&(pg_video_buf[byteoffset_rect]),
                    &src_bitmap->bm_bits[byteoffset_bitmap],size);
        byteoffset_bitmap += double_width_src;
        byteoffset_rect += cn_lcd_xsize;
    }
    return true;
}
//----从screen中取像素---------------------------------------------------------
//功能: 从screen中取一像素，并转换成cn_sys_pf_e8r8g8b8或cn_sys_pf_a8r8g8b8格式。
//参数: x、y，坐标
//返回: 像素颜色值
//-----------------------------------------------------------------------------
u32 __lcd_get_pixel_screen(s32 x,s32 y)
{
    u32 r,g,b,color,offset;

    offset = pg_lcd_reg->LCDSADDR1;
    offset += y*cn_lcd_xsize*2;
    offset += x*2;
    color = (u32)(*(u16*)offset);
    r = (color>>11) &0x1f;
    g = (color>>5) & 0x3f;
    b = color & 0x1f;
    return (r<<19) | (g<<10) | (b<<3);
}

//----读取bitmap中矩形块-------------------------------------------------------
//功能: 把一个位图中的矩形读取到另一个位图中。
//参数: rect，欲读取的矩形
//      dest，保存矩形的位图，其长宽必须与rect相同
//返回: true=成功读取，false=失败，原因可能是提供了不支持的像素格式
//-----------------------------------------------------------------------------
bool_t __lcd_get_rect_screen(struct tagRectangle *rect,struct tagRectBitmap *dest)
{
    return false;
 }
//----初始化lcd设备------------------------------------------------------------
//功能: 初始化lcd设备。
//参数: 无
//返回: 显示器资源指针
//-----------------------------------------------------------------------------
static struct tagGkWinRsc frame_win;
static struct tagRectBitmap frame_bm;

struct tagDisplayRsc* ModuleInstall_LCD(s32 width,s32 height,const char *DisplayName,const char* HeapName)
{

    __lcd_hard_init();
    __lcd_power_enable(0,1);
    __lcd_envid_of(1);

    frame_bm.PixelFormat = cn_lcd_pf;
    frame_bm.width = width;
    frame_bm.height = height;
    frame_bm.linebytes = width*2;
    frame_bm.bm_bits = (u8*)pg_frame_buffer;

    frame_win.wm_bitmap =&frame_bm;

    tg_lcd_display.frame_buffer = &frame_win;

    tg_lcd_display.xmm = 0;
    tg_lcd_display.ymm = 0;
    tg_lcd_display.width = width;
    tg_lcd_display.height = height;
    tg_lcd_display.pixel_format = CN_SYS_PF_RGB565;
    tg_lcd_display.reset_clip = false;
    tg_lcd_display.framebuf_direct = false;
    //无须初始化frame_buffer和desktop，z_topmost三个成员

    tg_lcd_display.draw.set_pixel_bm = __lcd_set_pixel_bm;
    tg_lcd_display.draw.fill_rect_bm = __lcd_fill_rect_bm;
    tg_lcd_display.draw.get_pixel_bm = __lcd_get_pixel_bm;
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
    tg_lcd_display.DisplayHeap =  M_FindHeap(HeapName);
    tg_lcd_display.disp_ctrl = __lcd_disp_ctrl;

    if(GK_InstallDisplay(&tg_lcd_display,(char*)DisplayName))
        return (ptu32_t)&tg_lcd_display;
    else
        return (ptu32_t)NULL;
}






