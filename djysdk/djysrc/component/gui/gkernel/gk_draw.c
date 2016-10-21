//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. As a constituent part of djyos,do not transplant it to other software
//    without specific prior written permission.

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
// 这份授权条款，在使用者符合以下三条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。
// 3. 本软件作为都江堰操作系统的组成部分，未获事前取得的书面许可，不允许移植到非
//    都江堰操作系统环境下运行。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------

//所属模块: gui
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 跟屏幕绘制相关的操作
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2010-03-30
//   作者:  罗侍田.
//   新版本号：V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "align.h"
#include "stdlib.h"
#include "string.h"
#include "object.h"
#include "gkernel.h"
#include <gui/gkernel/gk_display.h>
#include "gk_syscall.h"
#include "gk_win.h"
#include "gk_draw.h"
#include "gk_clip.h"
#include "font.h"

#define __gk_swap(a, b){a^=b; b^=a; a^=b;}

#ifndef MAX
#define MAX(x,y)    (x>y?x:y)
#endif

#ifndef MIN
#define MIN(x,y)    (x<y?x:y)
#endif

//----光栅变量检查1------------------------------------------------------------
//功能: 检查指定的光栅操作码是否需要目标位图参加运算。
//参数: rop_code，光栅操作码
//返回: 1=需要，0=不需要
//注: 判断逻辑表达式(((fore_code & CN_ROP_D)>>1) == (fore_code & ~CN_ROP_D))的
//    含义，太长，注释无法写下，请参看代码配套的书。
//-----------------------------------------------------------------------------
u32 __GK_RopIsNeedDestination(struct RopGroup RopCode)
{
    u8 rop2;
    if((RopCode.AlphaMode & CN_ALPHA_MODE_NEED_DST) || (RopCode.HyalineEn == 1))
        return CN_GKWIN_DEST_VISIBLE;
    else
    {
        rop2 = RopCode.Rop2Mode;
        if((rop2 == CN_R2_BLACK) || (rop2 == CN_R2_NOTCOPYEN)
           || (rop2 == CN_R2_COPYPEN) || (rop2 == CN_R2_MERGEPEN) )
            return CN_GKWIN_DEST_HIDE;
        else
            return CN_GKWIN_DEST_VISIBLE;
    }
}

//----本地格式转换到rgb24------------------------------------------------------
//功能: 颜色格式转换，把一个本地格式颜色转换成ERGB8888，如果位图格式如果包含
//      alpha通道，则先计算alpha。
//参数: PixelFormat，本地格式
//      color，源色彩
//      ExColor,双功能参数，如果PixelFormat=CN_SYS_PF_GRAY1~8，或者PixelFormat=
//          CN_SYS_PF_ALPHA1~8，则ExColor表示基色。
//          如果PixelFormat=CN_SYS_PF_PALETTE1,则ExColor表示调色板指针。
//          如果PixelFormat不需要此参数，可给0或NULL.
//返回: 真彩色格式
//-----------------------------------------------------------------------------
u32 GK_ConvertColorToRGB24(u16 PixelFormat,u32 color,ptu32_t ExColor)
{
    u32 a,r,g,b;
    switch(PixelFormat)
    {
        case CN_SYS_PF_RGB565:
        {
            r = (color>>8) & 0xf8;
            g = (color>>3) & 0xfc;
            b = (color<<3) & 0xf8;
        }break;
        case CN_SYS_PF_RGB332:
        {
            r = color & 0xe0;
            g = (color<<3) & 0xe0;
            b = (color<<6) & 0xc0;
        }break;
        case CN_SYS_PF_RGBE4444:
        {
            r = (color>>8) & 0xf0;
            g = (color>>4) & 0xf0;
            b = color & 0xf0;
        }break;
        case CN_SYS_PF_ERGB4444:
        {
            r = (color>>4) & 0xf0;
            g = color & 0xf0;
            b = (color<<4) & 0xf0;
        }break;
        //5551格式很别扭，暂时屏蔽掉，有需要时再根据实际格式增加。
//        case CN_SYS_PF_RGBC5551:
//        {
//            u32 c = (color & 1)<<2;
//            r = ((color>>8) & 0xf8) + c;
//            g = ((color>>3) & 0xf8) + c;
//            b = ((color<<2) & 0xfc);
//        }break;
        case CN_SYS_PF_ERGB6666:
        {
            r = (color>>10) & 0xfc;
            g = (color>>4) & 0xfc;
            b = (color<<2) & 0xfc;
        }break;
        case CN_SYS_PF_E12RGB666:
        {
            r = (color>>10) & 0xfc;
            g = (color>>4) & 0xfc;
            b = (color<<2) & 0xfc;
        }break;
        case CN_SYS_PF_RGB101010:
        {
            r = (color>>22) & 0xff;
            g = (color>>12) & 0xff;
            b = (color>>2) & 0xff;
        }break;
        case CN_SYS_PF_RGB888:
        {
            return color;
        }break;
        case CN_SYS_PF_ERGB8888:
        {
            return color;
        }break;
        case CN_SYS_PF_ARGB1555:
        {
            if(color & 0x8000)
            {
                r = (color>>7) & 0xf8;
                g = (color>>2) & 0xf8;
                b = (color<<3) & 0xf8;
            }
            else
            {
                return 0;
            }
        }break;
        case CN_SYS_PF_RGBA5551:
        {
            if(color & 1)
            {
                r = (color>>8) & 0xf8;
                g = (color>>3) & 0xf8;
                b = (color<<2) & 0xf8;
            }
            else
            {
                return 0;
            }
        }break;
        case CN_SYS_PF_ARGB8888:
        {
            a = (color>>24) & 0xff;
            r = ((color>>16) & 0xff) *a / 255;
            g = ((color>>8) & 0xff) *a / 255;
            b = ((color>>0) & 0xff) *a / 255;
        }break;
        case CN_SYS_PF_GRAY1:
        case CN_SYS_PF_ALPHA1:
        {
            return color * ExColor;
        }break;

        case CN_SYS_PF_GRAY2:
        case CN_SYS_PF_ALPHA2:
        {
            r = (((ExColor>>16) & 0xff) * color) / 3;
            g = (((ExColor>>8) & 0xff) * color) / 3;
            b = ((ExColor & 0xff) * color) / 3;
        }break;
        case CN_SYS_PF_GRAY4:
        case CN_SYS_PF_ALPHA4:
        {
            r = (((ExColor>>16) & 0xff) * color) / 15;
            g = (((ExColor>>8) & 0xff) * color) / 15;
            b = ((ExColor & 0xff) * color) / 15;
        }break;
        case CN_SYS_PF_GRAY8:
        case CN_SYS_PF_ALPHA8:
        {
            r = (((ExColor>>16) & 0xff) * color) / 255;
            g = (((ExColor>>8) & 0xff) * color) / 255;
            b = ((ExColor & 0xff) * color) / 255;
        }break;
        case CN_SYS_PF_PALETTE1:
        {
            return ((u32*)ExColor)[color];
        }break;
        case CN_SYS_PF_PALETTE2:
        {
            return ((u32*)ExColor)[color];
        }break;
        case CN_SYS_PF_PALETTE4:
        {
            return ((u32*)ExColor)[color];
        }break;
        case CN_SYS_PF_PALETTE8:
        {
            return ((u32*)ExColor)[color];
        }break;
        default:
        {
            return color;
        }break;
    }
    return (r<<16) | (g<<8) | b;
}

//----rgb24转换到本地----------------------------------------------------------
//功能: 把一个ERGB8888格式的颜色转换成本地格式，本地格式不允许有alpha通道，也不
//      允许是调色板位图。
//参数: PixelFormat，目标格式
//      color，源色彩
//返回: PixelFormat格式的色彩
//注: 不支持调色板格式，直接返回color
//    灰度图的算法，返回color的HLS色彩模型的亮度。
//-----------------------------------------------------------------------------
u32 GK_ConvertRGB24ToPF(u16 PixelFormat,u32 color)
{
    u32 r,g,b;
    //转换过程不需要argb分解因子的转换，在此switch完成，以提高效率
    switch(PixelFormat)
    {
        case CN_SYS_PF_RGB888:
        case CN_SYS_PF_ERGB8888:
        case CN_SYS_PF_ARGB8888:
        case CN_SYS_PF_ARGB1555:
        case CN_SYS_PF_RGBA5551:
        //以下4个格式，不允许用作窗口格式，实际上也不会出现以他们为目标的变换
        case CN_SYS_PF_PALETTE1:
        case CN_SYS_PF_PALETTE2:
        case CN_SYS_PF_PALETTE4:
        case CN_SYS_PF_PALETTE8:
        {
            return color;
        }break;
        default: break;
    }
    r = (color>>16) &0xff;
    g = (color>>8) & 0xff;
    b = color & 0xff;
    switch(PixelFormat)
    {
        case CN_SYS_PF_RGB565:
        {
            return ((r>>3)<<11) | ((g>>2)<<5) | (b>>3);
        }break;
        case CN_SYS_PF_RGB332:
        {
            return ((r>>5)<<5) | ((g>>5)<<2) | (b>>6);
        }break;
        case CN_SYS_PF_RGBE4444:
        {
            return ((r>>4)<<12) | ((g>>4)<<8) | ((b>>4)<<4);
        }break;
        case CN_SYS_PF_ERGB4444:
        {
            return ((r>>4)<<8) | ((g>>4)<<4) | (b>>4);
        }break;
        //5551格式很别扭，暂时屏蔽掉，有需要时再根据实际格式增加。
//        case CN_SYS_PF_RGBC5551:    //实际上丢掉了最低位
//        {
//            return ((r>>3)<<11) | ((g>>3)<<6) | ((b>>3)<<1);
//        }break;
        case CN_SYS_PF_ERGB6666:
        {
            return ((r>>2)<<12) | ((g>>2)<<6) | (b>>2);
        }break;
        case CN_SYS_PF_E12RGB666:
        {
            return ((r>>2)<<12) | ((g>>2)<<6) | (b>>2);
        }break;
        case CN_SYS_PF_RGB101010:
        {
            return (r<<20) | (g<<10) | b;
        }break;
        case CN_SYS_PF_GRAY1:
        case CN_SYS_PF_ALPHA1:
        {
            //根据rgb值计算hls模型的亮度
            return (((MAX(MAX(r,g),b)+MIN(MIN(r,g),b))*1) + 255 )/(2*255);
        }break;
        case CN_SYS_PF_GRAY2:
        case CN_SYS_PF_ALPHA2:
        {
            //根据rgb值计算hls模型的亮度
            return (((MAX(MAX(r,g),b)+MIN(MIN(r,g),b))*3) + 255 )/(2*255);
        }break;
        case CN_SYS_PF_GRAY4:
        case CN_SYS_PF_ALPHA4:
        {
            //根据rgb值计算hls模型的亮度
            return (((MAX(MAX(r,g),b)+MIN(MIN(r,g),b))*15) + 255 )/(2*255);
        }break;
        case CN_SYS_PF_GRAY8:
        case CN_SYS_PF_ALPHA8:
        {
            //根据rgb值计算hls模型的亮度
            return (((MAX(MAX(r,g),b)+MIN(MIN(r,g),b))*255) + 255 )/(2*255);
        }break;
        default:
        {
            return color;
        }
        break;
    }
    return color;
}
//----各种颜色格式间的相互转换-------------------------------------------------
//功能: 各种颜色格式间的相互转换。目标格式不允许有alpha通道，也不允许是调色板位
//      图。如果源格式有alpha通道的话，将与色彩值相乘后再转换。
//参数: dpf_type，目标格式
//      spf_type，源格式
//      color，源色彩，源pf_type格式。
//      ExColor,源色彩的双功能参数，如果spf_type=CN_SYS_PF_GRAY1~8，或者spf_type=
//          CN_SYS_PF_ALPHA1~8，则ExColor表示基色。
//      如果spf_type=CN_SYS_PF_PALETTE1,则ExColor表示调色板指针。
//返回: dpf_type格式的色彩
//-----------------------------------------------------------------------------
u32 GK_ConvertPF2PF(u16 dpf_type,u16 spf_type,u32 color,ptu32_t ExColor)
{
    u32 result;
    //源格式与目标格式相同，直接返回源色彩
    if(spf_type == dpf_type)
        return color;
    //将源色彩转换成真彩色，有alpha通道的话，计算之。
    result = GK_ConvertColorToRGB24(spf_type,color,ExColor);
    //将真彩色转换成目标格式的颜色
    result = GK_ConvertRGB24ToPF(dpf_type,result);
    return result;
}
//----rop2混合-----------------------------------------------------------------
//功能: 计算rop2色彩混合。
//参数: dest，背景颜色
//      PenColor，画笔颜色
//      rop2_code，二元光栅操作码
//返回: 混合后的色彩
//-----------------------------------------------------------------------------
u32 GK_BlendRop2(u32 dest,u32 PenColor,u32 Rop2Code)
{
    switch(Rop2Code)
    {
        case CN_R2_BLACK:         // 混合后的颜色为黑色(black)
        {
            return CN_COLOR_BLACK;
        }break;
        case CN_R2_NOTMERGEPEN:   //混合后的颜色为~(dest | PenColor)
        {
            return ~(dest | PenColor);
        }break;
        case CN_R2_MASKNOTPEN:    //混合后的颜色为~PenColor & dest
        {
            return ~PenColor & dest;
        }break;
        case CN_R2_NOTCOPYEN:    //混合后的颜色为~PenColor
        {
            return ~PenColor;
        }break;
        case CN_R2_MASKPENNOT:    //混合后的颜色为~dest & PenColor
        {
            return ~dest & PenColor;
        }break;
        case CN_R2_NOT:           //混合后的颜色为~dest
        {
            return ~dest;
        }break;
        case CN_R2_XORPEN :        //混合后的颜色为dest ^ PenColor
        {
            return dest ^ PenColor;
        }break;
        case CN_R2_NOTMASKPEN:    //混合后的颜色为~(dest & PenColor)
        {
            return ~(dest & PenColor);
        }break;
        case CN_R2_MASKPEN:       //混合后的颜色为dest & PenColor
        {
            return dest & PenColor;
        }break;
        case CN_R2_NOTXORPEN:     //混合后的颜色为~(dest ^ PenColor)
        {
            return ~(dest ^ PenColor);
        }break;
        case CN_R2_NOP:           //混合后的颜色为dest（原色）
        {
            return dest;
        }break;
        case CN_R2_MERGENOTPEN:   //混合后的颜色为~PenColor | dest
        {
            return ~PenColor | dest;
        }break;
        case CN_R2_COPYPEN:       //混合后的颜色为pen（画笔色）
        {
            return PenColor;
        }break;
        case CN_R2_MERGEPENNOT:   //混合后的颜色为~dest | PenColor
        {
            return ~dest | PenColor;
        }break;
        case CN_R2_MERGEPEN:      //混合后的颜色为dest | PenColor
        {
            return dest | PenColor;
        }break;
        case CN_R2_WHITE:         //混合后的颜色为白色(white)
        {
            return CN_COLOR_WHITE ;
        }break;
        default:
        {
            return PenColor;
            break;
        }
    }
    return 0;
}
//----判断目标点所在的区域-----------------------------------------------------
//功能: 判断目标点所在的区域，分成矩形域的左、右、上、下四部分。
//参数: x、y，目标点,相对窗口的坐标
//      limit，矩形域,相对窗口的坐标
//返回: 标志量c，c = 0表示目标点在矩形域内，否则在矩形域外
//-----------------------------------------------------------------------------
u32 __GK_PointSect(s32 x,s32 y,struct Rectangle *limit)  //确认
{
    s32 c;
    c = 0;

    if(x < limit->left)                 //判断目标点是否在矩形域左方
        c |= 0x01;
    else
    {
        if(x > limit->right)            //判断目标点是否在矩形域右方
        c |= 0x02;
    }
    if(y < limit->top)                  //判断目标点是否在矩形域上方
        c |= 0x04;
    else
    {
        if(y > limit->bottom)           //判断目标点是否在矩形域下方
        c |= 0x08;
    }
    return c;
}
//----求取直线段与矩形域的交点-------------------------------------------------
//功能: 求取直线段与矩形域的交点。
//参数: limit，矩形域,相对窗口的坐标
//      x1、y1、x2、y2，起、终点,相对窗口的坐标
//返回: 标志量flag，flag = 0表示斜线不在limit内，flag = 1表示斜线有点在limit内
//说明: 此处的直线段是针对斜线段的
//-----------------------------------------------------------------------------
u32 __GK_OlineSectInter(struct Rectangle *limit,s32 *x1,s32 *y1,s32 *x2,s32 *y2)//确认
{
    s32 c1,c2,i=0;
    s32 flag = 1;

    c1 = __GK_PointSect(*x1,*y1,limit);
    c2 = __GK_PointSect(*x2,*y2,limit);

    if((c1 == 0)&&(c2 == 0))        //斜线在limit内
        return flag;
    else if((c1&c2) != 0)           //斜线在limit外，整条线段在limit的一条边界外
    {
        flag = 0;
        return flag;
    }
    else
    {
        if(c1 == 0)                 //点(x1,y1)在limit内，点(x2,y2)肯定不在
        {
            do
            {
                if((c2&0x01) != 0)
                {
                    *y2 = *y1+(*y1-*y1)*(limit->left-*x1)/(*x1-*x1);
                    *x2 = limit->left;
                    c2 = __GK_PointSect(*x2,*y2,limit);
                }
                else if((c2&0x02) != 0)
                {
                    *y2 = *y1+(*y2-*y1)*(limit->right-*x1)/(*x2-*x1);
                    *x2 = limit->right;
                    c2 = __GK_PointSect(*x2,*y2,limit);
                }
                else if((c2&0x04) != 0)
                {
                    *x2 = *x1+(*x2-*x1)*(limit->top-*y1)/(*y2-*y1);
                    *y2 = limit->top;
                    c2 = __GK_PointSect(*x2,*y2,limit);
                }
                else
                {
                    *x2 = *x1+(*x2-*x1)*(limit->bottom-*y1)/(*y2-*y1);
                    *y2 = limit->bottom;
                    c2 = __GK_PointSect(*x2,*y2,limit);
                }
            }while(c2 != 0);
            return flag;
        }
        else                    //点(x1,y1)不在limit内，点(x2,y2)可能在limit内
        {                       //也可能在limit外
            do{
                if((c1&0x01) != 0)
                {
                    *y1 = *y1+(*y2-*y1)*(limit->left-*x1)/(*x2-*x1);
                    *x1 = limit->left;
                    c1 = __GK_PointSect(*x1,*y1,limit);
                    i++;
                }
                else if((c1&0x02) != 0)
                {
                    *y1 = *y1+(*y2-*y1)*(limit->right-*x1)/(*x2-*x1);
                    *x1 = limit->right;
                    c1 = __GK_PointSect(*x1,*y1,limit);
                    i++;
                }
                else if((c1&0x04) != 0)
                {
                    *x1 = *x1+(*x2-*x1)*(limit->top-*y1)/(*y2-*y1);
                    *y1 = limit->top;
                    c1 = __GK_PointSect(*x1,*y1,limit);
                    i++;
                }
                else
                {
                    if((c1&0x08) != 0)
                    {
                        *x1 = *x1+(*x2-*x1)*(limit->bottom-*y1)/(*y2-*y1);
                        *y1 = limit->bottom;
                        c1 = __GK_PointSect(*x1,*y1,limit);
                        i++;
                    }
                }
                //直线和limit有交点时，一个端点最多求两次交点
                if(i>2)
                {
                    flag = 0;
                    return flag;
                }
            }while(c1 != 0);
            if(c2 == 0)
                return flag;
            else
            {
                do
                {
                    if((c2&0x01) != 0)
                    {
                        *y2 = *y1+(*y2-*y1)*(limit->left-*x1)/(*x2-*x1);
                        *x2 = limit->left;
                        c2 = __GK_PointSect(*x2,*y2,limit);
                    }
                    else if((c2&0x02) != 0)
                    {
                        *y2 = *y1+(*y2-*y1)*(limit->right-*x1)/(*x2-*x1);
                        *x2 = limit->right;
                        c2 = __GK_PointSect(*x2,*y2,limit);
                    }
                    else if((c2&0x04) != 0)
                    {
                        *x2 = *x1+(*x2-*x1)*(limit->top-*y1)/(*y2-*y1);
                        *y2 = limit->top;
                        c2 = __GK_PointSect(*x2,*y2,limit);
                    }
                    else
                    {
                        *x2 = *x1+(*x2-*x1)*(limit->bottom-*y1)/(*y2-*y1);
                        *y2 = limit->bottom;
                        c2 = __GK_PointSect(*x2,*y2,limit);
                    }
                }while(c2 != 0);
                return flag;
            }
        }
    }
}

//----读取位图中的像素---------------------------------------------------------
//功能: 在一个bitmap读取像素，按bitmap的像素格式读出，未做任何转换。
//参数: bitmap，目标位图
//      x、y，目标像素的坐标
//返回: 像素颜色值,直接读取，不做格式转换，即使灰度图或调色板格式，也没有转换成
//      真实色彩
//-----------------------------------------------------------------------------
u32 GK_GetPixelBm(struct RectBitmap *bitmap, s32 x,s32 y)
{
    u8 color_bits,bit_offset;
    u32 color=0,byte_offset;
    if(bitmap == NULL)
        return 0;
    color_bits = (bitmap->PixelFormat & CN_PF_BITWIDE_MSK) >> 8;
    //根据位图颜色格式读取像素
    switch(color_bits)
    {
        case 1:
        {
            //像素坐标对应的字节偏移量
            byte_offset = y*bitmap->linebytes + x/8;
            //像素坐标在相应字节中的位偏移
            bit_offset = 7-x%8;     //最高位显示在左边，最低位在右边
            //取像素颜色
            color = (u32)(bitmap->bm_bits[byte_offset]>>bit_offset)&0x1;
        }break;
        case 2:
        {
            byte_offset = y*bitmap->linebytes + x/4;
            bit_offset = (3-x%4)<<1;     //最高位显示在左边，最低位在右边
            color = (u32)(bitmap->bm_bits[byte_offset]>>bit_offset)&0x3;
        }break;
        case 4:
        {
            byte_offset = y*bitmap->linebytes + x/2;
            bit_offset = (1-x%2)<<2;     //最高位显示在左边，最低位在右边
            color = (u32)(bitmap->bm_bits[byte_offset]>>bit_offset)&0xf;
        }break;
        case 8:
        {
            byte_offset = y*bitmap->linebytes + x;
            color = (u32)bitmap->bm_bits[byte_offset];
        }break;
        case 12:
        {
            byte_offset = y*bitmap->linebytes + x*3/2;
            bit_offset = (x*12)%8;
            //注:因对齐问题，不可把目标地址强制转换成16位指针。
            if(bit_offset == 0)
            {
                color = (u32)bitmap->bm_bits[byte_offset]+
                        (u32)((bitmap->bm_bits[byte_offset+1]&0xf)<<8);
            }
            else
            {
                color = (u32)((bitmap->bm_bits[byte_offset]&0xf0)>>4)+
                        (u32)(bitmap->bm_bits[byte_offset+1]<<4);
            }
        }break;
        case 16:
        {
            byte_offset = y*bitmap->linebytes + x*2;
            color = (u32)(bitmap->bm_bits[byte_offset]
                    |(bitmap->bm_bits[byte_offset+1]<<8));
        }break;
        case 24:
        {
            byte_offset = y*bitmap->linebytes + x*3;
            color = (u32)bitmap->bm_bits[byte_offset];
            color += (u32)bitmap->bm_bits[byte_offset+1]<<8;
            color += (u32)bitmap->bm_bits[byte_offset+2]<<16;
        }break;
        case 32:
        {
            byte_offset = y*bitmap->linebytes + x*4;
            color = (u32)(*(u32*)(&bitmap->bm_bits[byte_offset]));
        }break;
        default:
        break;
    }
    return color;
}
//----求取目标颜色的argb分量---------------------------------------------------
//功能: 求取目标颜色的argb分量，按像素格式所需要的位数，并没有转成真彩。
//参数: PixelFormat，目标颜色颜色格式
//      color，目标颜色
//      a、r、g、b，目标颜色的a、r、g、b分量
//      ExColor,双功能参数，如果PixelFormat=CN_SYS_PF_GRAY1~8，或者PixelFormat=
//          CN_SYS_PF_ALPHA1~8，则ExColor表示基色。
//          如果PixelFormat=CN_SYS_PF_PALETTE1,则ExColor表示调色板指针。
//          如果PixelFormat不需要此参数，可给0或NULL.
//返回: 无，
//注: 如果是灰度或调色板格式，则转化为RGB24后取各分量，a(alpha)分量为0
//-----------------------------------------------------------------------------
void __GK_GetRgbBm(u16 PixelFormat,u32 color,u32 *a,u32 *r,
                                            u32 *g,u32 *b,ptu32_t ExColor)
{
    u32 rgb32,lr,lg,lb;
    //根据位图颜色格式分离目标颜色中的rgb分量
    switch(PixelFormat)
    {
        case CN_SYS_PF_RGB565:
        {
            *r = (color>>11) & 0x1f;
            *g = (color>>5) & 0x3f;
            *b = color & 0x1f;
        }break;
        case CN_SYS_PF_RGB332:
        {
            *r = (color>>5) & 0x07;
            *g = (color<<2) & 0x07;
            *b = color & 0x03;
        }break;
        case CN_SYS_PF_RGBE4444:
        {
            *r = (color>>12) & 0x0f;
            *g = (color>>8) & 0x0f;
            *b = (color>>4) & 0x0f;
        }break;
        case CN_SYS_PF_ERGB4444:
        {
            *r = (color>>8) & 0x0f;
            *g = (color>>4) & 0x0f;
            *b = color & 0x0f;
        }break;
        //5551格式很别扭，暂时屏蔽掉，有需要时再根据实际格式增加。
//        case CN_SYS_PF_RGBC5551:
//        {
//            u32 c;
//            c = (color & 1);
//            *r = (((color>>10) & 0x3e)+c);
//            *g = (((color>>5) & 0x3e)+c);
//            *b = color & 0x3f;
//        }break;
        case CN_SYS_PF_ERGB6666:
        {
            *r = (color>>12) & 0x3f;
            *g = (color>>6) & 0x3f;
            *b = color & 0x3f;
        }break;
        case CN_SYS_PF_E12RGB666:
        {
            *r = (color>>12) & 0x3f;
            *g = (color>>6) & 0x3f;
            *b = color & 0x3f;
        }break;
        case CN_SYS_PF_RGB101010:
        {
            *r = (color>>20) & 0x3ff;
            *g = (color>>10) & 0x3ff;
            *b = color & 0x3ff;
        }break;
        case CN_SYS_PF_RGB888:
        {
            *r = (color>>16) & 0xff;
            *g = (color>>8) & 0xff;
            *b = color & 0xff;
        }break;
        case CN_SYS_PF_ERGB8888:
        {
            *r = (color>>16) & 0xff;
            *g = (color>>8) & 0xff;
            *b = color & 0xff;
        }break;

        case CN_SYS_PF_ARGB1555:
        {
            *a = (color>>16) & 0x01;
            *r = (color>>10) & 0x1f;
            *g = (color>>5) & 0x1f;
            *b = color & 0x1f;
        }break;
        case CN_SYS_PF_RGBA5551:
        {
            *a = (color) & 0x01;
            *r = (color>>11) & 0x1f;
            *g = (color>>6) & 0x1f;
            *b = (color>>1) & 0x1f;
        }break;
        case CN_SYS_PF_ARGB8888:
        {
            *a = (color>>24) & 0xff;
            *r = (color>>16) & 0xff;
            *g = (color>>8) & 0xff;
            *b = color & 0xff;
        }break;
        case CN_SYS_PF_GRAY1:
        case CN_SYS_PF_ALPHA1:
        {
            rgb32 = color * ExColor;
            *r = (rgb32>>16) & 0xff;
            *g = (rgb32>>8) & 0xff;
            *b = rgb32 & 0xff;
        }break;
        case CN_SYS_PF_GRAY2:
        case CN_SYS_PF_ALPHA2:
        {
            lr = (ExColor>>16) & 0xff;
            lg = (ExColor>>8) & 0xff;
            lb = ExColor & 0xff;
            *r = (lr * color) / 3;
            *g = (lg * color) / 3;
            *b = (lb * color) / 3;
        }break;
        case CN_SYS_PF_GRAY4:
        case CN_SYS_PF_ALPHA4:
        {
            lr = (ExColor>>16) & 0xff;
            lg = (ExColor>>8) & 0xff;
            lb = ExColor & 0xff;
            *r = (lr * color) / 15;
            *g = (lg * color) / 15;
            *b = (lb * color) / 15;
        }break;
        case CN_SYS_PF_GRAY8:
        case CN_SYS_PF_ALPHA8:
        {
            lr = (ExColor>>16) & 0xff;
            lg = (ExColor>>8) & 0xff;
            lb = ExColor & 0xff;
            *r = (lr * color) / 255;
            *g = (lg * color) / 255;
            *b = (lb * color) / 255;
        }break;
        case CN_SYS_PF_PALETTE1:
        case CN_SYS_PF_PALETTE2:
        case CN_SYS_PF_PALETTE4:
        case CN_SYS_PF_PALETTE8:
        {
            lr = ((((u32*)ExColor)[color])>>16) & 0xff;
            lg = ((((u32*)ExColor)[color])>>8) & 0xff;
            lb = (((u32*)ExColor)[color]) & 0xff;
            *r = (lr * color) / 255;
            *g = (lg * color) / 255;
            *b = (lb * color) / 255;
        }break;
        default:
        {
            *a = 0;
            *r = 0;
            *g = 0;
            *b = 0;
        }
        break;
    }
}
//----计算alpha混合-------------------------------------------------------
//功能: 对两个像素求alpha混合值,分离color,RGB分别计算,转换成dst的像素格式返回.
//      dst的格式如果调色板类型，混合后的返回值是RGB24格式的，因为混合后的颜色可
//          能不在颜色表中。
//      dst的格式如果是灰度图，混合后的颜色再计算亮度输出。
//参数: SrcColor,DstColor，待混合的色彩
//      PixelFormat, 色彩的格式
//      ExColor,双功能参数，如果PixelFormat=CN_SYS_PF_GRAY1~8，或者PixelFormat=
//          CN_SYS_PF_ALPHA1~8，则ExColor表示基色。
//          如果PixelFormat=CN_SYS_PF_PALETTE1,则ExColor表示调色板指针。
//          如果PixelFormat不需要此参数，可给0或NULL.
//      Asrc，Adst，src 和 dst alpha,0~255
//      AlphaMode,alpha混合模式，参见 CN_ALPHA_MODE_MSK 及相关定义
//返回: 组合后的颜色
//-----------------------------------------------------------------------------
u32 __GK_AlphaBlendBitmapToWin(u32 SrcColor,u32 DstColor, u16 PixelFormat,
                  ptu32_t dst_ExColor,u32 Asrc,u32 Adst,u32 AlphaMode)
{
    u32 a,r,g,b;
    u32 a_src=0,r_src,g_src,b_src;
    u32 a_dst=0,r_dst,g_dst,b_dst;
    u32 result;
    //求取目标位图颜色的rgb分量
    __GK_GetRgbBm(PixelFormat,SrcColor,&a_dst,&r_dst,&g_dst,&b_dst,dst_ExColor);
    //求取源位图颜色的rgb分量
    __GK_GetRgbBm(PixelFormat,DstColor,&a_src,&r_src,&g_src,&b_src,dst_ExColor);
    //根据目标位图颜色格式求取组合后的颜色
    switch(AlphaMode)
    {
        case CN_ALPHA_MODE_AsAdN:
        {
            a = a_src*Asrc/255+a_dst*(255-Asrc)*Adst/255;
            r = r_src*Asrc/255+r_dst*(255-Asrc)*Adst/255;
            g = g_src*Asrc/255+g_dst*(255-Asrc)*Adst/255;
            b = b_src*Asrc/255+b_dst*(255-Asrc)*Adst/255;
        }break;
        case CN_ALPHA_MODE_AsAd:
        {
            a = a_src*Asrc/255+a_dst*Adst/255;
            r = r_src*Asrc/255+r_dst*Adst/255;
            g = g_src*Asrc/255+g_dst*Adst/255;
            b = b_src*Asrc/255+b_dst*Adst/255;
        }break;
        case CN_ALPHA_MODE_As:
        {
            a = a_src*Asrc/255;
            r = r_src*Asrc/255;
            g = g_src*Asrc/255;
            b = b_src*Asrc/255;
        }break;
        case CN_ALPHA_MODE_AsN:
        default:
        {
            a = a_src*Asrc/255+a_dst*(255-Asrc)/255;
            r = r_src*Asrc/255+r_dst*(255-Asrc)/255;
            g = g_src*Asrc/255+g_dst*(255-Asrc)/255;
            b = b_src*Asrc/255+b_dst*(255-Asrc)/255;
        }break;
    }
    switch(PixelFormat)
    {
        case CN_SYS_PF_RGB565:
        {
            result = (r<<11) | (g<<5) | b;
        }break;
        case CN_SYS_PF_RGB332:
        {
            result = (r<<5) | (g<<2) | b;
        }break;
        case CN_SYS_PF_RGBE4444:
        {
            result = (r<<16) | (g<<8) | (b<<4);
        }break;
        case CN_SYS_PF_ERGB4444:
        {
            result = (r<<8) | (g<<4) | b;
        }break;
        //5551格式很别扭，暂时屏蔽掉，有需要时再根据实际格式增加。
//        case CN_SYS_PF_RGBC5551:
//        {
//            u32 c;
//            c = r & 1;
//            r = r>>1;
//            g = g>>1;
//            b = b>>1;
//            color = (r<<11) | (g<<6) | (b<<1) | c;
//            return color;
//        }break;
        case CN_SYS_PF_ERGB6666:
        {
            result = (r<<12) | (g<<6) | b;
        }break;
        case CN_SYS_PF_E12RGB666:
        {
            result = (r<<12) | (g<<6) | b;
        }break;
        case CN_SYS_PF_RGB101010:
        {
            result = (r<<20) | (g<<10) | b;
        }break;
        case CN_SYS_PF_RGB888:
        case CN_SYS_PF_ERGB8888:
        {
            result = (r<<16) | (g<<8) | b;
        }break;
        case CN_SYS_PF_ARGB1555:
        {
            result = (a<<15) | (r<<10) | (g<<5) | b;
        }break;
        case CN_SYS_PF_RGBA5551:
        {
            result = (a &1) | (r<<11) | (g<<6) | (b<<1);
        }break;
        case CN_SYS_PF_ARGB8888:
        {
            result = (a<<24) | (r<<16) | (g<<8) | b;
       }break;
        case CN_SYS_PF_GRAY1:
        {
            //根据rgb值计算hls模型的亮度
            result = (((MAX(MAX(r,g),b)+MIN(MIN(r,g),b))*1) + 255 )/(2*255);
        }break;
        case CN_SYS_PF_GRAY2:
        {
            //根据rgb值计算hls模型的亮度
            result = (((MAX(MAX(r,g),b)+MIN(MIN(r,g),b))*3) + 255 )/(2*255);
        }break;
        case CN_SYS_PF_GRAY4:
        {
            //根据rgb值计算hls模型的亮度
            result = (((MAX(MAX(r,g),b)+MIN(MIN(r,g),b))*15) + 255 )/(2*255);
        }break;
        case CN_SYS_PF_GRAY8:
        {
            //根据rgb值计算hls模型的亮度
            return (((MAX(MAX(r,g),b)+MIN(MIN(r,g),b))*255) + 255 )/(2*255);
        }break;
        case CN_SYS_PF_PALETTE1:
        case CN_SYS_PF_PALETTE2:
        case CN_SYS_PF_PALETTE4:
        case CN_SYS_PF_PALETTE8:
        {
            result = (r<<16) | (g<<8) | b;
        }break;
        default:
        {
            result = 0;
        }
        break;
    }

    return result;
}

//----在内存画点(执行二元光栅操作)---------------------------------------------
//功能: 在一个bitmap上画点，执行二元光栅操作，要求color和bitmap的格式相同。
//参数: bitmap，目标位图
//      x、y，画图坐标
//      color，颜色，bitmap的兼容格式
//      rop2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
void __GK_SetPixelRop2Bm(struct RectBitmap *bitmap,
                         s32 x,s32 y,u32 color,u32 Rop2Code)
{
    u8 bit_offset;
    u8 color_bits;
    u32 byte_offset;
    u32 pf,alpha;
    color_bits = (bitmap->PixelFormat & CN_PF_BITWIDE_MSK) >>8;
    //根据位图颜色格式在内存上画点
    switch(color_bits)
    {
        case 1:
        {
            byte_offset = y*bitmap->linebytes + x/8;//点对应坐标的字节偏移量
            //点对应坐标在字节中的位偏移量高位在左边，低位在右
            bit_offset = 7-x%8;
            //目标位图上对应坐标的颜色
            pf = (u32)bitmap->bm_bits[byte_offset];
            //进行二元光栅操作后的颜色
            pf = GK_BlendRop2(pf,color<<bit_offset,Rop2Code);
            //保留除目标位的，其它位颜色
            bitmap->bm_bits[byte_offset] &= ~(1<<bit_offset);
            //将混合后的颜色添加到相应位
            bitmap->bm_bits[byte_offset] |= (u8)pf;
        }break;
        case 2:
        {
            byte_offset = y*bitmap->linebytes + x/4;//点对应坐标的字节偏移量
            //点对应坐标在字节中的位偏移量高位在左边，低位在右
            bit_offset = (3-x%4)<<1;
            //目标位图上对应坐标的颜色
            pf = (u32)bitmap->bm_bits[byte_offset];
            //进行二元光栅操作后的颜色
            pf = GK_BlendRop2(pf,color<<bit_offset,Rop2Code);
            //保留除目标位的，其它位颜色
            bitmap->bm_bits[byte_offset] &= ~(3<<bit_offset);
            //将混合后的颜色添加到相应位
            bitmap->bm_bits[byte_offset] |= (u8)pf;
        }break;
        case 4:
        {
            byte_offset = y*bitmap->linebytes + x/2;//点对应坐标的字节偏移量
            //点对应坐标在字节中的位偏移量高位在左边，低位在右
            bit_offset = (1-x%2)<<2;
            //目标位图上对应坐标的颜色
            pf = (u32)bitmap->bm_bits[byte_offset];
            //进行二元光栅操作后的颜色
            pf = GK_BlendRop2(pf,color<<bit_offset,Rop2Code);
            //保留除目标位的，其它位颜色
            bitmap->bm_bits[byte_offset] &= ~(0xf<<bit_offset);
            //将混合后的颜色添加到相应位
            bitmap->bm_bits[byte_offset] |= (u8)pf;
        }break;
        case 8:
        {
            byte_offset = y*bitmap->linebytes + x;//点对应坐标的字节偏移量
            //进行二元光栅操作后的颜色
            pf = GK_BlendRop2((u32)bitmap->bm_bits[byte_offset],
                    color,Rop2Code);
            bitmap->bm_bits[byte_offset] = (u8)pf;//将混合后的颜色添加到相应位
        }break;
        case 12:
        {
            byte_offset = y*bitmap->linebytes + x*3/2;//点对应坐标的字节偏移量
            bit_offset = (x*12)%8;              //点对应坐标在字节中的位偏移量
            //注:因对齐问题，不可把目标地址强制转换成16位指针。
            pf = (u32)bitmap->bm_bits[byte_offset]
                  + (u32)(bitmap->bm_bits[byte_offset+1]<<8);
            pf = pf>>bit_offset;
            pf = GK_BlendRop2(pf,color,Rop2Code);//进行二元光栅操作后的颜色
            if(bit_offset == 0)
            {
                bitmap->bm_bits[byte_offset] = (u8)(pf&0xff);
                bitmap->bm_bits[byte_offset+1] &= 0xf0;
                bitmap->bm_bits[byte_offset+1] |= (u8)(pf>>8);
            }else
            {
                bitmap->bm_bits[byte_offset] &= 0xf;
                bitmap->bm_bits[byte_offset] |= (u8)((pf&0xf)<<4);
                bitmap->bm_bits[byte_offset+1] = (u8)(pf>>4);
            }
        }break;
        case 16:
        {
            byte_offset = y*bitmap->linebytes + x*2;//点对应坐标的字节偏移量
            //目标位图上对应坐标的颜色
            pf = (u32)(*(u16*)(&bitmap->bm_bits[byte_offset]));
//            alpha = pf;
            //进行二元光栅操作后的颜色
            pf = GK_BlendRop2(pf,color,Rop2Code);
//            if(bitmap->PixelFormat == CN_SYS_PF_ARGB1555)
//            {
//                pf &= 0x7fff;
//                pf |= alpha & 0x8000;
//            }
            //注:因对齐问题，不可把目标地址强制转换成16位指针。
            bitmap->bm_bits[byte_offset] = (u8)pf;
            bitmap->bm_bits[byte_offset+1] = (u8)(pf>>8);
        }break;
        case 24:
        {
            byte_offset = y*bitmap->linebytes + x*3;//点对应坐标的字节偏移量
            pf = (u32)bitmap->bm_bits[byte_offset];
            pf += (u32)bitmap->bm_bits[byte_offset+1]<<8;
            pf += (u32)bitmap->bm_bits[byte_offset+2]<<16;
            //进行二元光栅操作后的颜色
            pf = GK_BlendRop2(pf,color,Rop2Code);
            bitmap->bm_bits[byte_offset] = (u8)pf;
            bitmap->bm_bits[byte_offset+1] = (u8)(pf >> 8);
            bitmap->bm_bits[byte_offset+2] = (u8)(pf >> 16);
        }break;
        case 32:
        {
            byte_offset = y*bitmap->linebytes + x*4;//点对应坐标的字节偏移量
            pf = (u32)(*(u32*)(&bitmap->bm_bits[byte_offset]));
            alpha = pf;
            //进行二元光栅操作后的颜色
            pf = GK_BlendRop2(pf,color,Rop2Code);
            if(bitmap->PixelFormat == CN_SYS_PF_ARGB8888)
            {
                pf &= 0x00ffffff;
                pf |= alpha & 0xff000000;
            }
            //注:因对齐问题，不可把目标地址强制转换成32位指针。
            bitmap->bm_bits[byte_offset] = (u8)pf;
            bitmap->bm_bits[byte_offset+1] = (u8)(pf >> 8);
            bitmap->bm_bits[byte_offset+2] = (u8)(pf >> 16);
            bitmap->bm_bits[byte_offset+3] = (u8)(pf >> 24);
        }break;
    }
}

//----RGB888格式Alpha混合-------------------------------------------------------
//功能: 将背景色和前景色进行Alpha混合
//参数: bk_c, 背景色
//     fr_c, 前景色
//     alpha, Alpha值
//返回:新的颜色值(RGB888格式)
//-----------------------------------------------------------------------------
static  u32 __alpha_blend_color(u32 bk_c,u32 fr_c,u8 alpha)
{
    u8 bk_r,bk_g,bk_b;
    u8 fr_r,fr_g,fr_b;

    bk_r =bk_c>>16;
    bk_g =bk_c>>8;
    bk_b =bk_c;
    fr_r =fr_c>>16;
    fr_g =fr_c>>8;
    fr_b =fr_c;
    fr_r =(((fr_r-bk_r)*alpha)>>8) + bk_r;
    fr_g =(((fr_g-bk_g)*alpha)>>8) + bk_g;
    fr_b =(((fr_b-bk_b)*alpha)>>8) + bk_b;

    return  (fr_r<<16)|(fr_g<<8)|(fr_b);
}

//----位图间copy像素(RopCode)----------------------------------------------
//功能: 把src_bitmap位图上指定的一个像素，绘制到dst_bitmap上，执行扩展rop操作。
//      扩展rop操作的定义，见 CN_ROP_ALPHA_SRC_MSK 及相关定义。注意，目标位图不
//      允许含alpha通道，也不允许是调色板位图。
//参数: dst_bitmap，目标位图
//      src_bitmap，源位图
//      x_dst、y_dst，目标位图中的目标坐标
//      x_src、y_src，源位图中的目标坐标
//      RopCode，光栅操作码，见 CN_ROP_ALPHA_SRC_MSK 及相关定义
//      HyalineColor，透明色,ERGB8888格式
//返回: 无
//-----------------------------------------------------------------------------
void __GK_CopyPixelRopBm(struct RectBitmap *dst_bitmap,
                                struct RectBitmap *src_bitmap,
                                s32 x_dst,s32 y_dst,s32 x_src,s32 y_src,
                                struct RopGroup RopCode,u32 HyalineColor)
{
    u8 src_alpha,dst_alpha;
    u8 Rop2;
    u32 color;
    u32 dst_color,src_color;

    //读取源位图中的颜色，未转换成ERGB8888
    src_color = GK_GetPixelBm(src_bitmap,x_src,y_src);
    //转换时，已经计算源位图的alpha通道了(如果有)
    src_color = GK_ConvertColorToRGB24(src_bitmap->PixelFormat,
                                       src_color,src_bitmap->ExColor);
    if(RopCode.AlphaEn == 1)       //alpha混合
    {
        if((RopCode.HyalineEn == 0) || (src_color != HyalineColor))
        {
            src_alpha = RopCode.SrcAlpha;
            dst_alpha = RopCode.DstAlpha;
            //将源位图颜色进行颜色格式转换，得到与目标位图颜色格式一致的颜色
            src_color = GK_ConvertRGB24ToPF(dst_bitmap->PixelFormat,src_color);
            //读取目标位图中的颜色，未转换成ERGB8888
            dst_color = GK_GetPixelBm(dst_bitmap,x_dst,y_dst);
            color = __GK_AlphaBlendBitmapToWin(src_color,dst_color,
                            dst_bitmap->PixelFormat,
                            dst_bitmap->ExColor,src_alpha,dst_alpha,
                            RopCode.AlphaMode);
            //绘制像素
            __GK_SetPixelRop2Bm(dst_bitmap,x_dst,y_dst,color,CN_R2_COPYPEN);
        }
    }
    else if(RopCode.Rop2Mode != CN_R2_COPYPEN) //如果同时使能alpha混合和rop2，rop2将被忽略
    {
        if((RopCode.HyalineEn ==0) || (src_color != HyalineColor))
        {
            Rop2 = RopCode.Rop2Mode;
            //将源位图颜色进行颜色格式转换，得到与目标位图颜色格式一致的颜色
            src_color = GK_ConvertRGB24ToPF(dst_bitmap->PixelFormat,src_color);
            //绘制像素
            __GK_SetPixelRop2Bm(dst_bitmap,x_dst,y_dst,src_color,Rop2);
        }
    }
    else if(RopCode.HyalineEn == 1)     //透明色使能
    {
        if(src_color != HyalineColor)
        {
            //将源位图颜色进行颜色格式转换，得到与目标位图颜色格式一致的颜色
            src_color = GK_ConvertRGB24ToPF(dst_bitmap->PixelFormat,src_color);
            //绘制像素
            __GK_SetPixelRop2Bm(dst_bitmap,x_dst,y_dst,src_color,CN_R2_COPYPEN);
        }
    }
    else
    {
        switch(src_bitmap->PixelFormat)
        {
           case CN_SYS_PF_ALPHA1:
               if(GK_GetPixelBm(src_bitmap,x_src,y_src))
               {
                    //将源位图颜色进行颜色格式转换，得到与目标位图颜色格式一致的颜色
                    src_color = GK_ConvertRGB24ToPF(dst_bitmap->PixelFormat,src_bitmap->ExColor);
                    //绘制像素
                    __GK_SetPixelRop2Bm(dst_bitmap,x_dst,y_dst,src_color,CN_R2_COPYPEN);
               }
               break;

           case CN_SYS_PF_ALPHA2:
               src_alpha = GK_GetPixelBm(src_bitmap,x_src,y_src);
               src_alpha =(src_alpha<<6)&0xC0;

               dst_color = GK_GetPixelBm(dst_bitmap,x_dst,y_dst);
               dst_color = GK_ConvertColorToRGB24(dst_bitmap->PixelFormat,dst_color,dst_bitmap->ExColor);

               dst_color = __alpha_blend_color(dst_color,src_bitmap->ExColor,src_alpha);
               dst_color = GK_ConvertRGB24ToPF(dst_bitmap->PixelFormat,dst_color);
               __GK_SetPixelRop2Bm(dst_bitmap,x_dst,y_dst,dst_color,CN_R2_COPYPEN);
               break;

           case CN_SYS_PF_ALPHA4:
               src_alpha = GK_GetPixelBm(src_bitmap,x_src,y_src);
               src_alpha = (src_alpha<<4)&0xF0;

               dst_color = GK_GetPixelBm(dst_bitmap,x_dst,y_dst);
               dst_color = GK_ConvertColorToRGB24(dst_bitmap->PixelFormat,dst_color,dst_bitmap->ExColor);

               dst_color = __alpha_blend_color(dst_color,src_bitmap->ExColor,src_alpha);
               dst_color = GK_ConvertRGB24ToPF(dst_bitmap->PixelFormat,dst_color);
               __GK_SetPixelRop2Bm(dst_bitmap,x_dst,y_dst,dst_color,CN_R2_COPYPEN);
               break;

           case CN_SYS_PF_ALPHA8:
               src_alpha = GK_GetPixelBm(src_bitmap,x_src,y_src);
               src_alpha = src_alpha&0xFF;

               dst_color = GK_GetPixelBm(dst_bitmap,x_dst,y_dst);
               dst_color = GK_ConvertColorToRGB24(dst_bitmap->PixelFormat,dst_color,dst_bitmap->ExColor);

               dst_color = __alpha_blend_color(dst_color,src_bitmap->ExColor,src_alpha);
               dst_color = GK_ConvertRGB24ToPF(dst_bitmap->PixelFormat,dst_color);
               __GK_SetPixelRop2Bm(dst_bitmap,x_dst,y_dst,dst_color,CN_R2_COPYPEN);
               break;

           default:
               //将源位图颜色进行颜色格式转换，得到与目标位图颜色格式一致的颜色
               src_color = GK_ConvertRGB24ToPF(dst_bitmap->PixelFormat,src_color);
               __GK_SetPixelRop2Bm(dst_bitmap,x_dst,y_dst,src_color,CN_R2_COPYPEN);
               break;
        }

    }

}

//----在内存画点---------------------------------------------------------------
//功能: 把一个bitmap上的一个点copy到另一个bitmap。如果源位图有alpha通道，则需要
//      计算alpha
//参数: dst_bitmap，目标位图
//      src_bitmap，源位图
//      x_dst、y_dst，目标位图中的目标坐标
//      x_src、y_src，源位图中的目标坐标
//返回: 无
//-----------------------------------------------------------------------------
void __GK_CopyPixelBm(struct RectBitmap *dst_bitmap,
                      struct RectBitmap *src_bitmap,
                      s32 x_dst,s32 y_dst,s32 x_src,s32 y_src)
{
    u32 pf_src,cvt_src;
    //目标位图颜色格式
//    color_bits = (dst_bitmap->PixelFormat & CN_PF_BITWIDE_MSK)>>8;
    //取源位图颜色，返回源位图颜色格式的像素值
    pf_src = GK_GetPixelBm(src_bitmap,x_src,y_src);
    //将取出的源位图颜色进行颜色格式转换，得到与目标位图颜色格式一致的颜色
    cvt_src = GK_ConvertPF2PF(dst_bitmap->PixelFormat,
                            src_bitmap->PixelFormat,pf_src,src_bitmap->ExColor);
    __GK_SetPixelRop2Bm(dst_bitmap,x_dst,y_dst,cvt_src,CN_R2_COPYPEN);
}

//----画点---------------------------------------------------------------------
//功能: 在窗口内画一个像素点。
//参数: 参数是结构para的成员
//      gkwin，目标窗口指针
//      x、y，画图坐标
//      color，画点使用的颜色
//      rop2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
void GK_SetPixel(struct GkscParaSetPixel *para)
{
    struct ClipRect *clip;
    struct GkWinRsc *fb_gkwin,*pixelwin;
    struct RectBitmap *bitmap;
    u32 pf_color;
    s32 offsetx,offsety;
    if((para->Rop2Code == CN_R2_NOP) || (para->Rop2Code > CN_R2_LAST))
        return;                                     //不执行操作
    pixelwin = para->gkwin;
    //说明有win buffer，且直接写屏属性为false
    if((pixelwin->wm_bitmap != NULL)
        && (pixelwin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
    {   //处理方法:在win buffer中绘图，标志changed_msk
        bitmap = pixelwin->wm_bitmap;
        //若显示器使用cn_custom_pf格式，应该由硬件实现绘制像素
        if(bitmap->PixelFormat & CN_CUSTOM_PF)
        {
            pixelwin->disp->draw.SetPixelToBitmap(bitmap,para->x,para->y,
                                                 para->color,para->Rop2Code);
        }else//由软件实现绘制像素
        {
            //规定调用者给定的颜色需是真彩色
            pf_color = GK_ConvertRGB24ToPF(bitmap->PixelFormat,para->color);
            __GK_SetPixelRop2Bm(bitmap,para->x,para->y,
                                pf_color,para->Rop2Code);      //绘制像素
        }
        __GK_ShadingPixel(pixelwin,para->x,para->y);//标志像素的changed_msk
    }else
    {
        clip = pixelwin->visible_clip;
        if(clip == NULL)        //可视域为空，直接返回
            return ;
        fb_gkwin = pixelwin->disp->frame_buffer;
        offsetx = pixelwin->absx0 +para->x;
        offsety = pixelwin->absy0 +para->y;
        //有frame buffer，且直接写屏属性为false
        if((fb_gkwin != NULL) && (pixelwin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
        {   //处理方法:在frame buffer中绘图，但只绘gkwin中的可视区域
            bitmap = fb_gkwin->wm_bitmap;
            do{
                //像素对应的坐标在可视域内，则绘制像素
                if(!((offsetx<clip->rect.left) || (offsetx>=clip->rect.right)
                    || (offsety<clip->rect.top) ||(offsety>=clip->rect.bottom)))
                {
                    //若显示器使用cn_custom_pf格式，应该由硬件driver实现绘制像素
                    if(bitmap->PixelFormat & CN_CUSTOM_PF)
                    {
                        fb_gkwin->disp->draw.SetPixelToBitmap(bitmap,offsetx,
                                           offsety,para->color,para->Rop2Code);
                    }else//由软件实现绘制像素
                    {
                        //规定调用者给定的颜色颜色格式需是真彩色
                        pf_color = GK_ConvertRGB24ToPF(
                                                bitmap->PixelFormat,para->color);
                        __GK_SetPixelRop2Bm(bitmap,offsetx,offsety,
                                                pf_color,para->Rop2Code);
                    }
                    //标志像素的changed_msk
                    __GK_ShadingPixel(fb_gkwin,offsetx,offsety);
                    break;      //像素已经绘制，退出循环
                }else//像素对应的坐标不在可视域内，指向下一个可视域
                    clip = clip->next;
            }while(clip != pixelwin->visible_clip);
        }
        else
        //无win buffer，也无frame buffer，直接画在screen上
        //直接写屏属性为true，不管有无缓冲区，都直接画在screen上
        {
            do{
                //像素对应的坐标在可视域内，则绘制像素
                if(!((offsetx<clip->rect.left) || (offsetx>=clip->rect.right)
                    || (offsety<clip->rect.top) ||(offsety>=clip->rect.bottom)))
                {
                    __GK_SetPixelScreen(pixelwin->disp,offsetx,offsety,
                                          para->color,para->Rop2Code);
                    break;      //像素已经绘制，退出循环
                }else//像素对应的坐标不在可视域内，指向下一个可视域
                    clip = clip->next;
            }while(clip != pixelwin->visible_clip);
        }
    }
}

//----设置changed_msk:像素-----------------------------------------------------
//功能: 绘制像素时，设置窗口的changed_msk，把改写过的区域着色，由调用者确保坐标
//      合法。
//参数: gkwin，目标窗口指针
//      x、y，目标像素对应的坐标
//返回: 无
//-----------------------------------------------------------------------------
void __GK_ShadingPixel(struct GkWinRsc *gkwin,s32 x,s32 y)
{
    s32 msk_x,msk_y;
    if(gkwin->WinProperty.ChangeFlag == CN_GKWIN_CHANGE_ALL)   //已经全部着色
    {
        return;
    }
    gkwin->WinProperty.ChangeFlag = CN_GKWIN_CHANGE_PART;
    msk_x = x/8;       //计算x在msk位图中第几列
    msk_y = y/8;       //计算y在msk位图中第几行
    //将像素所在的changed_msk中的bit置为1
    __GK_SetPixelRop2Bm(&(gkwin->changed_msk),msk_x,msk_y,
                                CN_COLOR_WHITE,CN_R2_COPYPEN);
    return;
}

//----设置changed_msk----------------------------------------------------------
//功能: 把一个窗口的rect限定的区域设置为被改写
//参数: bitmap，目标bitmap
//      rect，改写区域
//返回: 无
//说明: 内部函数，不检查坐标合法性，由调用者保证坐标的合法性
//-----------------------------------------------------------------------------
void __GK_ShadingRect(struct GkWinRsc *gkwin,struct Rectangle *rect)
{
    struct RectBitmap *bitmap;
    s32 x,y;
    s32 msk_x1,msk_x2,msk_y1,msk_y2;

    if(gkwin->WinProperty.ChangeFlag == CN_GKWIN_CHANGE_ALL)   //已经全部着色
    {
        return;
    }
    bitmap = gkwin->wm_bitmap;
    if((rect->left <= 0) && (rect->top <= 0)
            && (rect->right >= bitmap->width)&&(rect->bottom >= bitmap->height))
    {
        gkwin->WinProperty.ChangeFlag = CN_GKWIN_CHANGE_ALL;   //被msk的区域覆盖整个窗口
        return;
    }

    gkwin->WinProperty.ChangeFlag = CN_GKWIN_CHANGE_PART;
    msk_x1 = rect->left/8;          //计算x1在msk位图中第几列
    msk_x2 = (rect->right-1)/8;     //计算x2在msk位图中第几列，不包含终点，-1
    msk_y1 = rect->top/8;           //计算y1在msk位图中第几行
    msk_y2 = (rect->bottom-1)/8;    //计算y2在msk位图中第几行，不包含终点，-1

    for(y = msk_y1;y <= msk_y2;y++)
    {
        for(x = msk_x1;x <= msk_x2;x++)
            __GK_SetPixelRop2Bm(&(gkwin->changed_msk),x,y,
                                CN_COLOR_WHITE,CN_R2_COPYPEN);
    }
}

//----设置changed_msk----------------------------------------------------------
//功能: 在窗口或frame buffer绘制直线时，设置窗口的changed_msk，
//      把改写过的区域着色。
//参数: gkwin，目标窗口
//      limit，允许绘图的限制区域，一个矩形的区域，超出此区域的直线不画
//      x1、y1、x2、y2， 起点、终点坐标
//返回: 无
//-----------------------------------------------------------------------------
void __GK_ShadingLine(struct GkWinRsc *gkwin,struct Rectangle *limit,
                     s32 x1,s32 y1,s32 x2,s32 y2)
{
    if(gkwin->WinProperty.ChangeFlag == CN_GKWIN_CHANGE_ALL)   //已经全部着色
    {
        return;
    }
    gkwin->WinProperty.ChangeFlag = CN_GKWIN_CHANGE_PART;

    if(y1 == y2)        //水平直线着色
    {
        s32 x;
        s32 msk_x1,msk_x2,msk_y1;
        if(x1>x2)
            __gk_swap(x1,x2);
        if((y1 <= limit->top) || (y1 >= limit->bottom)
            ||(x2 <= limit->left) || (x1 >= limit->right))
            return;         //所绘直线在limit之外
        if(x1 < limit->top)
            x1 = limit->top;
        if(x2 > limit->bottom)
            x2 = limit->bottom;
        msk_x1 = x1/8;     //计算x1在msk位图中第几列
        msk_x2 = (x2-1)/8; //计算(x2-1)在msk位图中第几列，-1是因为终点不包含在内
        msk_y1 = y1/8;     //计算y1在msk位图中第几行
        for(x = msk_x1;x <= msk_x2;x++)
        {
            //将changed_msk相应的bit置1
            __GK_SetPixelRop2Bm(&(gkwin->changed_msk),x,msk_y1,
                                    CN_COLOR_WHITE,CN_R2_COPYPEN);
        }

    }
    else if(x1 == x2)   //垂直线着色
    {
        s32 y;
        s32 msk_x1,msk_y1,msk_y2;
        if(y1>y2)
            __gk_swap(y1,y2);
        if((x1<limit->left) || (x1 >= limit->right)
            ||(y2 <= limit->top) || (y1 >= limit->bottom))
            return;         //所绘直线在limit之外
        if(y1 < limit->top)
            y1 = limit->top;
        if(y2 > limit->bottom)
            y2 = limit->bottom;
        msk_x1 = x1/8;     //计算x1在msk位图中第几列
        msk_y1 = y1/8;     //计算y1在msk位图中第几行
        msk_y2 = (y2-1)/8; //计算(y2-1)在msk位图中第几行，-1是因为终点不包含在内
        for(y = msk_y1;y <= msk_y2;y++)
        {
            //将changed_msk相应的bit置1
            __GK_SetPixelRop2Bm(&(gkwin->changed_msk),msk_x1,y,
                                    CN_COLOR_WHITE,CN_R2_COPYPEN);
        }
    }
    else
    {
        s32 x,y;
        s32 msk_x,msk_y;
        s32 dx,dy;
        dx = abs(x2-x1);
        dy = abs(y2-y1);

        //x方向变化量大
        if(dx > dy)
        {
            if(((x1>x2)&&(y1>y2)) || ((x1<x2)&&(y1<y2)))
            {//x、y增减方向相同，即同增或者同减
                //选取x1、x2中较小的数作为x1
                if(x1 > x2)
                {
                    __gk_swap(x1,x2);
                    __gk_swap(y1,y2);
                }
                //向下对齐，取8*8像素，x每变化8像素后y与直线的交点，
                //将斜线上所有的点所在的8*8像素区域对应的
                //在changed_msk中的bit置为1，具体说明请看djygui的书
                for(x = align_down(8,x1); x <= x2; x+=8)
                {
                    //对计算的交点进行四舍五入处理，x、y增减方向相同
                    //所以以0.5为基准量
                    y = ((((x-x1)<<1)*(y2-y1)+(x2-x1))/((x2-x1)<<1))+y1;
                    msk_x = x/8;    //计算x在msk位图中第几列
                    msk_y = y/8;    //计算y在msk位图中第几行
                    //将changed_msk相应的bit置1
                    __GK_SetPixelRop2Bm(&(gkwin->changed_msk),msk_x,
                                         msk_y,CN_COLOR_WHITE,CN_R2_COPYPEN);
                    //标志(msk_x，msk_y)下面的一个点
                    msk_y += 1;
                    //将changed_msk相应的bit置1
                    __GK_SetPixelRop2Bm(&(gkwin->changed_msk),msk_x,
                                         msk_y,CN_COLOR_WHITE,CN_R2_COPYPEN);
                }
            }
            else
            {//x、y增减方向相反，即x增、y减，x减、y增
                //选取x1、x2中较小的数作为x1
                if(x1 > x2)
                {
                    __gk_swap(x1,x2);
                    __gk_swap(y1,y2);
                }
                //向下对齐，取8*8像素，x每变化8像素后y与直线的交点，
                //将斜线上所有的点所在的8*8像素区域对应的
                //在changed_msk中的bit置为1，具体说明请看djygui的书
                for(x = align_down(8,x1); x <= x2; x+=8)
                {
                    //对所得交点进行四舍五入处理，y为减方向，所以以-0.5为基准量
                    y = ((((x-x1)<<1)*(y2-y1)-(x2-x1))/((x2-x1)<<1))+y1;
                    msk_x = x/8;    //计算x在msk位图中第几列
                    msk_y = y/8;    //计算y在msk位图中第几行
                    //将changed_msk相应的bit置1
                    __GK_SetPixelRop2Bm(&(gkwin->changed_msk),msk_x,
                                         msk_y,CN_COLOR_WHITE,CN_R2_COPYPEN);
                    //标志(msk_x，msk_y)上面的一个点
                    msk_y -= 1;
                    //将changed_msk相应的bit置1
                    __GK_SetPixelRop2Bm(&(gkwin->changed_msk),msk_x,
                                         msk_y,CN_COLOR_WHITE,CN_R2_COPYPEN);
                }
            }
        }else
        {//y方向变化量大，或者x，y变化量相等
            if(((x1>x2)&&(y1>y2)) || ((x1<x2)&&(y1<y2)))
            {//x、y增减方向相同，即同增或者同减
                //选取y1、y2中较小的数作为y1
                if(y1 > y2)
                {
                    __gk_swap(x1,x2);
                    __gk_swap(y1,y2);
                }
                //向下对齐，取8*8像素，y每变化8像素后x与直线的交点，
                //将斜线上所有的点所在的8*8像素区域对应的
                //在changed_msk中的bit置为1，具体说明请看djygui的书
                for(y = align_down(8,y1); y <= y2; y+=8)
                {
                    x = ((((y-y1)<<1)*(x2-x1)+(y2-y1))/((y2-y1)<<1))+x1;
                    msk_x = x/8;    //计算x在msk位图中第几列
                    msk_y = y/8;    //计算y在msk位图中第几行
                    //将changed_msk相应的bit置1
                    __GK_SetPixelRop2Bm(&(gkwin->changed_msk),msk_x,
                                         msk_y,CN_COLOR_WHITE,CN_R2_COPYPEN);
                    msk_x += 1;
                    //将changed_msk相应的bit置1
                    __GK_SetPixelRop2Bm(&(gkwin->changed_msk),msk_x,
                                         msk_y,CN_COLOR_WHITE,CN_R2_COPYPEN);
                }
            }
            else
            {//x、y增减方向相反，即x增、y减，x减、y增
                //选取y1、y2中较小的数作为y1
                if(y1 > y2)
                {
                    __gk_swap(x1,x2);
                    __gk_swap(y1,y2);
                }
                //向下对齐，取8*8像素，y每变化8像素后x与直线的交点，
                //将斜线上所有的点所在的8*8像素区域对应的
                //在changed_msk中的bit置为1，具体说明请看djygui的书
                for(y = align_down(8,y1); y <= y2; y+=8)
                {
                    x = ((((y-y1)<<1)*(x2-x1)-(y2-y1))/((y2-y1)<<1))+x1;
                    msk_x = x/8;    //计算x在msk位图中第几列
                    msk_y = y/8;    //计算y在msk位图中第几行
                    //将changed_msk相应的bit置1
                    __GK_SetPixelRop2Bm(&(gkwin->changed_msk),msk_x,
                                         msk_y,CN_COLOR_WHITE,CN_R2_COPYPEN);
                    msk_x -= 1;
                    //将changed_msk相应的bit置1
                    __GK_SetPixelRop2Bm(&(gkwin->changed_msk),msk_x,
                                             msk_y,CN_COLOR_WHITE,CN_R2_COPYPEN);
                }
            }
        }
    }
}

//----清除changed_msk----------------------------------------------------------
//功能: 清除一个窗口的改写标志。
//参数: gkwin，目标窗口指针
//返回: 无
//-----------------------------------------------------------------------------
void __GK_ShadingClear(struct GkWinRsc *gkwin)
{
    u32 msk_size;
    if(gkwin->changed_msk.bm_bits)
    {
        msk_size =
            (gkwin->wm_bitmap->width +7)/8 /8*((gkwin->wm_bitmap->height +7)/8);
        memset(gkwin->changed_msk.bm_bits,0,msk_size);
    }
}

//----绘制斜线并标记掩码-------------------------------------------------------
//功能: 绘制斜线并标志位。
//参数: gkwin，目标窗口
//      x1、y1、x2、y2，起、终点
//      color，给定的颜色
//      rop2_code，二元光栅操作码
//返回: 无
//说明: 采用的是Bresenham算法
//-----------------------------------------------------------------------------
void __GK_DrawOline(struct GkWinRsc *gkwin,s32 x1,s32 y1,
                                        s32 x2,s32 y2,u32 color,u32 Rop2Code)
{
    u32 pf_color;
    s32 x,y,dx,dy,s1,s2,p,interchange,i;

    struct RectBitmap *bitmap;
    bitmap = gkwin->wm_bitmap;

    x = x1;
    y = y1;
    dx = abs(x2-x1);
    dy = abs(y2-y1);
    //标志绘制斜线方向
    if(x2 > x1)
        s1 = 1;
    else
        s1 = -1;
    if(y2 > y1)
        s2 = 1;
    else
        s2 = -1;
    //标志最大变化的变量，interchange为1，y变化最大，为0，x变化最大
    if(dy > dx)
    {
        __gk_swap(dx, dy);
        interchange = 1;
    }
    else
        interchange = 0;
    p = (dy<<1)-dx;            //设置初始误差判别值
    //颜色格式为显卡自定义格式
    if(bitmap->PixelFormat & CN_CUSTOM_PF)
    {
        for(i=1;i<=dx;i++)//hjj
        {
            gkwin->disp->draw.SetPixelToBitmap(bitmap,x,y,color,Rop2Code);
            //取离数学点较近的像素
            if(p >= 0)
            {
                if(interchange == 0)
                    y = y+s2;
                else
                    x = x+s1;
                p = p-(dx<<1);
            }
            if(interchange == 0)
                x = x+s1;
            else
                y = y+s2;
            p = p+(dy<<1);
        }
    }
    else
    {//颜色格式为其它格式
        pf_color = GK_ConvertRGB24ToPF(bitmap->PixelFormat,color);
        for(i=1;i<=dx;i++)
        {
            __GK_SetPixelRop2Bm(bitmap,x,y,pf_color,Rop2Code);
            //取离数学点较近的像素
            if(p >= 0)
            {
                if(interchange == 0)
                    y = y+s2;
                else
                    x = x+s1;
                p = p-(dx<<1);
            }
            if(interchange == 0)
                x = x+s1;
            else
                y = y+s2;
            p = p+(dy<<1);
        }
    }
}

//----画垂直线(不含端点)-------------------------------------------------------
//功能: 在窗口内从当前光标处开始画一条垂直线。
//参数: gkwin，目标窗口指针
//      limit，绘制的限制区，只绘制直线在limit矩形内部的部分
//      x1、y1，起始点的坐标
//      y2，结束点的纵坐标
//      color，颜色，cn_sys_pf_e8r8g8b8格式
//      rop2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
void __GK_VlinetoBm(struct GkWinRsc *gkwin,struct Rectangle *limit,
                     s32 x1,s32 y1,s32 y2,u32 color,u32 Rop2Code)
{
    s32 y;
    s32 msk_x1,msk_y1,msk_y2;
    u32 pf_color;
    struct RectBitmap *bitmap;
    if(y1>y2)
        __gk_swap(y1,y2);
    if((x1<limit->left) || (x1 >= limit->right)
        ||(y2 <= limit->top) || (y1 >= limit->bottom))
        return;         //所绘直线在limit之外
    bitmap = gkwin->wm_bitmap;
    //垂直线超出limit的部分不绘制
    if(y1 < limit->top)
        y1 = limit->top;
    if(y2 > limit->bottom)
        y2 = limit->bottom;
    if(bitmap->PixelFormat & CN_CUSTOM_PF)
    {
        for(y = y1; y < y2;y++)
        {
            gkwin->disp->draw.SetPixelToBitmap(bitmap,x1,y,color,Rop2Code);
        }
    }else
    {
        //用户给定颜色颜色格式为rgb24
        pf_color = GK_ConvertRGB24ToPF(bitmap->PixelFormat,color);
        for(y = y1;y < y2;y++)
        {
            __GK_SetPixelRop2Bm(bitmap,x1,y,pf_color,Rop2Code);
        }
    }

    //下面处理changed_msk标志位图，把窗口中因划线而修改的区域着色
    if(gkwin->WinProperty.ChangeFlag != CN_GKWIN_CHANGE_ALL)
    {
        gkwin->WinProperty.ChangeFlag = CN_GKWIN_CHANGE_PART;  //标志窗口部分修改
        msk_x1 = x1/8;     //计算x1在msk位图中第几列
        msk_y1 = y1/8;     //计算y1在msk位图中第几行
        msk_y2 = (y2-1)/8; //计算(y2-1)在msk位图中第几行，-1是因为终点不包含在内
        for(y = msk_y1;y <= msk_y2;y++)
        {   //将changed_msk相应的bit置1
            __GK_SetPixelRop2Bm(&(gkwin->changed_msk),msk_x1,y,
                                    CN_COLOR_WHITE,CN_R2_COPYPEN);
        }
    }
}

//----画水平直线(不含端点)-------------------------------------------------------
//功能: 在窗口内从当前光标处开始画一条水平直线。
//参数: gkwin，目标窗口指针
//      limit，绘制的限制区，只绘制直线在limit矩形内部的部分
//      x1、y1，起始点的坐标
//      x2，结束点的横坐标
//      color，颜色，cn_sys_pf_e8r8g8b8格式
//      rop2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
void __GK_HlinetoBm(struct GkWinRsc *gkwin,struct Rectangle *limit,
                     s32 x1,s32 y1,s32 x2,u32 color,u32 Rop2Code)
{
    s32 x;
    s32 msk_x1,msk_x2,msk_y1;
    u32 pf_color;
    struct RectBitmap *bitmap;
    if(x1>x2)
        __gk_swap(x1,x2);
    if((y1<limit->top) || (y1 >= limit->bottom)
        ||(x2 <= limit->left) || (x1 >= limit->right))
        return;         //所绘直线在limit之外
    bitmap = gkwin->wm_bitmap;
    //水平线在limit外部分不绘制
    if(x1 < limit->left)
        x1 = limit->left;
    if(x2 > limit->right)
        x2 = limit->right;
    if(bitmap->PixelFormat & CN_CUSTOM_PF)
    {
        for(x = x1; x < x2;x++)
        {
            gkwin->disp->draw.SetPixelToBitmap(bitmap,x,y1,color,Rop2Code);
        }
    }else
    {
        //用户给定颜色颜色格式为rgb24
        pf_color = GK_ConvertRGB24ToPF(bitmap->PixelFormat,color);
        for(x = x1;x < x2;x++)
        {//对1、2、4位色screen，这个循环很耗cpu，考虑改进--db
            __GK_SetPixelRop2Bm(bitmap,x,y1,pf_color,Rop2Code);
        }
    }

    //下面处理changed_msk标志位图，把窗口中因划线而修改的区域着色
    if(gkwin->WinProperty.ChangeFlag != CN_GKWIN_CHANGE_ALL)
    {
        gkwin->WinProperty.ChangeFlag = CN_GKWIN_CHANGE_PART;  //标志窗口部分修改
        msk_x1 = x1/8;     //计算x1在msk位图中第几列
        msk_x2 = (x2-1)/8; //计算(x2-1)在msk位图中第几列，-1是因为终点不包含在内
        msk_y1 = y1/8;     //计算y1在msk位图中第几行
        for(x = msk_x1;x <= msk_x2;x++)
        {   //将changed_msk相应的bit置1
            __GK_SetPixelRop2Bm(&(gkwin->changed_msk),x,msk_y1,
                                    CN_COLOR_WHITE,CN_R2_COPYPEN);
        }
    }
}
//----画斜线(不含端点)---------------------------------------------------------
//功能: 在窗口内从当前光标处开始画一条斜线。
//参数: gkwin，目标窗口指针
//      limit，绘制的限制区，只绘制直线在limit矩形内部的部分
//      x1、y1，起始点的坐标
//      x2、y2，结束点的坐标
//      color，颜色，cn_sys_pf_e8r8g8b8格式
//      rop2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
void __GK_OlinetoBm(struct GkWinRsc *gkwin,struct Rectangle *limit,//确认
                       s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 Rop2Code)
{
    s32 x,y;
    s32 msk_x,msk_y;
    s32 dx,dy;

    if(__GK_OlineSectInter(limit,&x1,&y1,&x2,&y2))//取得斜线与limit的交点
    {
        __GK_DrawOline(gkwin,x1,y1,x2,y2,color,Rop2Code);//绘制斜线

        //下面处理changed_msk标志位图，把窗口中因划线而修改的区域着色
        if(gkwin->WinProperty.ChangeFlag != CN_GKWIN_CHANGE_ALL)
        {
            gkwin->WinProperty.ChangeFlag = CN_GKWIN_CHANGE_PART;//标志窗口部分修改
            dx = abs(x2-x1);
            dy = abs(y2-y1);
            //x方向变化量大
            if(dx > dy)
            {
                if(((x1>x2)&&(y1>y2)) || ((x1<x2)&&(y1<y2)))
                {//x、y增减方向相同，即同增或者同减
                    //选取x1、x2中较小的数作为x1
                    if(x1 > x2)
                    {
                        __gk_swap(x1,x2);
                        __gk_swap(y1,y2);
                    }
                    //向下对齐，取8*8像素，x每变化8像素后y与直线的交点，
                    //将斜线上所有的点所在的8*8像素区域对应的
                    //在changed_msk中的bit置为1，具体说明请看djygui的书
                    for(x = align_down(8,x1); x <= x2; x+=8)
                    {
                        //对计算的交点进行四舍五入处理，x、y增减方向相同
                        //所以以0.5为基准量
                        y = ((((x-x1)<<1)*(y2-y1)+(x2-x1))/((x2-x1)<<1))+y1;
                        msk_x = x/8;    //计算x在msk位图中第几列
                        msk_y = y/8;    //计算y在msk位图中第几行
                        //将changed_msk相应的bit置1
                        __GK_SetPixelRop2Bm(&(gkwin->changed_msk),msk_x,
                                             msk_y,CN_COLOR_WHITE,CN_R2_COPYPEN);
                        //标志(msk_x，msk_y)下面的一个点
                        msk_y += 1;
                        //将changed_msk相应的bit置1
                        __GK_SetPixelRop2Bm(&(gkwin->changed_msk),msk_x,
                                             msk_y,CN_COLOR_WHITE,CN_R2_COPYPEN);
                    }
                }
                else
                {//x、y增减方向相反，即x增、y减，x减、y增
                    //选取x1、x2中较小的数作为x1
                    if(x1 > x2)
                    {
                        __gk_swap(x1,x2);
                        __gk_swap(y1,y2);
                    }
                    //向下对齐，取8*8像素，x每变化8像素后y与直线的交点，
                    //将斜线上所有的点所在的8*8像素区域对应的
                    //在changed_msk中的bit置为1，具体说明请看djygui的书
                    for(x = align_down(8,x1); x <= x2; x+=8)
                    {
                        //对所得交点进行四舍五入处理，y为减方向，所以以-0.5为基准量
                        y = ((((x-x1)<<1)*(y2-y1)-(x2-x1))/((x2-x1)<<1))+y1;
                        msk_x = x/8;    //计算x在msk位图中第几列
                        msk_y = y/8;    //计算y在msk位图中第几行
                        //将changed_msk相应的bit置1
                        __GK_SetPixelRop2Bm(&(gkwin->changed_msk),msk_x,
                                             msk_y,CN_COLOR_WHITE,CN_R2_COPYPEN);
                        //标志(msk_x，msk_y)上面的一个点
                        msk_y -= 1;
                        //将changed_msk相应的bit置1
                        __GK_SetPixelRop2Bm(&(gkwin->changed_msk),msk_x,
                                             msk_y,CN_COLOR_WHITE,CN_R2_COPYPEN);
                    }
                }
            }else
            {//y方向变化量大，或者x，y变化量相等
                if(((x1>x2)&&(y1>y2)) || ((x1<x2)&&(y1<y2)))
                {//x、y增减方向相同，即同增或者同减
                    //选取y1、y2中较小的数作为y1
                    if(y1 > y2)
                    {
                        __gk_swap(x1,x2);
                        __gk_swap(y1,y2);
                    }
                    //向下对齐，取8*8像素，y每变化8像素后x与直线的交点，
                    //将斜线上所有的点所在的8*8像素区域对应的
                    //在changed_msk中的bit置为1，具体说明请看djygui的书
                    for(y = align_down(8,y1); y <= y2; y+=8)
                    {
                        x = ((((y-y1)<<1)*(x2-x1)+(y2-y1))/((y2-y1)<<1))+x1;
                        msk_x = x/8;    //计算x在msk位图中第几列
                        msk_y = y/8;    //计算y在msk位图中第几行
                        //将changed_msk相应的bit置1
                        __GK_SetPixelRop2Bm(&(gkwin->changed_msk),msk_x,
                                             msk_y,CN_COLOR_WHITE,CN_R2_COPYPEN);
                        msk_x += 1;
                        //将changed_msk相应的bit置1
                        __GK_SetPixelRop2Bm(&(gkwin->changed_msk),msk_x,
                                             msk_y,CN_COLOR_WHITE,CN_R2_COPYPEN);
                    }
                }
                else
                {//x、y增减方向相反，即x增、y减，x减、y增
                    //选取y1、y2中较小的数作为y1
                    if(y1 > y2)
                    {
                        __gk_swap(x1,x2);
                        __gk_swap(y1,y2);
                    }
                    //向下对齐，取8*8像素，y每变化8像素后x与直线的交点，
                    //将斜线上所有的点所在的8*8像素区域对应的
                    //在changed_msk中的bit置为1，具体说明请看djygui的书
                    for(y = align_down(8,y1); y <= y2; y+=8)
                    {
                        x = ((((y-y1)<<1)*(x2-x1)-(y2-y1))/((y2-y1)<<1))+x1;
                        msk_x = x/8;    //计算x在msk位图中第几列
                        msk_y = y/8;    //计算y在msk位图中第几行
                        //将changed_msk相应的bit置1
                        __GK_SetPixelRop2Bm(&(gkwin->changed_msk),msk_x,
                                             msk_y,CN_COLOR_WHITE,CN_R2_COPYPEN);
                        msk_x -= 1;
                        //将changed_msk相应的bit置1
                        __GK_SetPixelRop2Bm(&(gkwin->changed_msk),msk_x,
                                                 msk_y,CN_COLOR_WHITE,CN_R2_COPYPEN);
                    }
                }
            }
        }
    }
}
//----画直线(不含端点)---------------------------------------------------------
//功能: 在窗口上画直线，端点不画，须处理changed_msk区。
//参数: gkwin，目标窗口指针
//      limit，允许绘图的限制区域，一个矩形的区域,超出此区域的直线不画
//      x1、y1、x2、y2，起点和终点坐标
//      color，绘图所用的颜色
//      rop2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
void __GK_LinetoBm(struct GkWinRsc *gkwin,struct Rectangle *limit,
                    s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 Rop2Code)
{
    if(y1 == y2)        //绘水平线
        __GK_HlinetoBm(gkwin,limit,x1,y1,x2,color,Rop2Code);
    else if(x1 == x2)   //绘垂直线
        __GK_VlinetoBm(gkwin,limit,x1,y1,y2,color,Rop2Code);
    else                //绘斜线
        __GK_OlinetoBm(gkwin,limit,x1,y1,x2,y2,color,Rop2Code);
}

//----在screen上画像素---------------------------------------------------------
//功能: 在screen上直接画点。
//参数: gkwin，绘制的目标窗口
//      x、y， 坐标
//      color，绘图所用的颜色，cn_sys_pf_e8r8g8b8格式
//      r2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
void __GK_SetPixelScreen(struct DisplayRsc *display,s32 x,s32 y,
                           u32 color,u32 Rop2Code)
{
    struct DisplayRsc *mirror,*current;

    display->draw.SetPixelToScreen(x,y,color,Rop2Code);//在screen上画点
#if 0
    mirror = (struct DisplayRsc *)display->node.Child;
    if(mirror == NULL)      //没有镜像显示器，直接返回
        return;
#else
    mirror = (struct DisplayRsc *)OBJ_Child(&display->node);
#endif


    current = mirror;
    while(current != NULL)
    {
        current->draw.SetPixelToScreen(x,y,color,Rop2Code);
        current = (struct DisplayRsc *)OBJ_TraveScion(&mirror->node,
                &current->node);
    }
}

//----画直线(不含端点)---------------------------------------------------------
//功能: 在screen上直接画直线，端点不画。
//参数: display，绘制的目标显示器
//      limit，允许绘图的限制区域，一个矩形的区域，超出此区域的直线不画
//      x1、y1、x2、y2，起点和终点坐标
//      color，绘图所用的颜色，cn_sys_pf_e8r8g8b8格式
//      r2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
void __GK_LinetoScreen(struct DisplayRsc *display,struct Rectangle *limit,
                   s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 Rop2Code)
{
    struct DisplayRsc *mirror,*current;
    s32 x,y,ax,ay,s1,s2,p,interchange,i;

    if(display->draw.LineToScreen(limit, x1, y1, x2, y2, color,Rop2Code))
    {
    }else   //硬件加速不支持直线绘制，软件实现
    {
        if(y1 == y2)    //绘水平线
        {
            if(x1 > x2)
                __gk_swap(x1,x2);
            if((y1 < limit->top) || (y1 >= limit->bottom)
                ||(x2 <= limit->left) || (x1 >= limit->right))
                return;         //所绘直线在limit之外
            if(x1 < limit->left)
                x1 = limit->left;
            if(x2 > limit->right)
                x2 = limit->right;
            for(x = x1; x < x2;x++)
            {
                display->draw.SetPixelToScreen(x,y1,color,Rop2Code);
            }
        }else if(x1 == x2)   //绘垂直线
        {
            if(y1 > y2)
                __gk_swap(y1,y2);
            if((x1 < limit->left) || (x1 >= limit->right)
                ||(y2 <= limit->top) || (y1 >= limit->bottom))
                return;         //所绘直线在limit之外
            if(y1 < limit->top)
                y1 = limit->top;
            if(y2 > limit->bottom)
                y2 = limit->bottom;
            for(y = y1; y < y2;y++)
            {
                display->draw.SetPixelToScreen(x1,y,color,Rop2Code);
            }
        }else            //绘斜线
        {
            if( __GK_OlineSectInter(limit,&x1,&y1,&x2,&y2))
            {
                x = x1;
                y = y1;
                ax = abs(x2-x1);
                ay = abs(y2-y1);
                if(x2 > x1)   //绘制斜线方向
                   s1 = 1;
                else
                   s1 = -1;
                if(y2 > y1)
                   s2 = 1;
                else
                   s2 = -1;
                if(ay > ax)
                {
                    __gk_swap(ax,ay);
                    interchange = 1;
                }
                else
                    interchange = 0;
                p = (ay<<1)-ax;   //设置初始误差判别值
                for(i = 1;i <= ax;i++)
                {
                    display->draw.SetPixelToScreen(x,y,color,Rop2Code);

                    if(p >= 0)     //取离数学点较近的像素
                    {
                        if(interchange == 0)
                           y = y+s2;
                        else
                           x = x+s1;
                        p = p-(ax<<1);
                    }
                    if(interchange == 0)
                        x = x+s1;
                    else
                        y = y+s2;
                    p = p+(ay<<1);
                }
            }
        }
    }
#if 0
    mirror = (struct DisplayRsc *)display->node.child;
#else
    mirror = (struct DisplayRsc *)OBJ_Child(&display->node);
#endif
    if(mirror == NULL)      //没有镜像显示器，直接返回
        return;
    current = mirror;
    while(current != NULL)
    {
        current->draw.LineToScreen(limit, x1, y1, x2, y2, color,Rop2Code);
        current = (struct DisplayRsc *)OBJ_TraveScion(&mirror->node,
                &current->node);
    }
}
//----传送bitmap到screen-------------------------------------------------------
//功能: 把bitmap的部分从内存传送到screen。
//参数: gkwin，绘制的目标窗口
//      rect，绘图区域，一个矩形的区域，超出此区域的部分不画
//      bitmap，绘制的位图
//      x、y，bitmap要传送部分左上角坐标
//返回: 无
//-----------------------------------------------------------------------------
void __GK_BltBmToScreen(struct DisplayRsc *display,struct Rectangle *rect,
                        struct RectBitmap *bitmap,s32 x,s32 y)
{
    //todo :增加alpha1,2,4,8格式.
    // 参考 __GK_CopyPixelRopBm;

    u32 color;
    s32 x_bmap,y_bmap,x_rect,y_rect;

    struct DisplayRsc *mirror,*current;

    if(display->draw.CopyBitmapToScreen(rect,bitmap,x,y))
    {
    }
    else//bitmap颜色格式与显示屏不一样或者硬件加速不支持由软件实现
    {
        //传送bitmap到screen
        y_bmap = y;
        for(y_rect = rect->top;y_rect < rect->bottom;y_rect++)
        {
            x_bmap = x;
            for(x_rect = rect->left;x_rect < rect->right;x_rect++)
            {

                color = GK_GetPixelBm(bitmap,x_bmap,y_bmap);
                switch(bitmap->PixelFormat)
                {
                case CN_SYS_PF_GRAY1:
                case CN_SYS_PF_ALPHA1:
                        if(color!=0)
                        {
                            display->draw.SetPixelToScreen(x_rect,y_rect,
                                    bitmap->ExColor,CN_R2_COPYPEN);
                        }
                        break;
                        ////

                    default:
                         color = GK_ConvertColorToRGB24(bitmap->PixelFormat,color,
                                 bitmap->ExColor);
                         display->draw.SetPixelToScreen(x_rect,y_rect,
                                                         color,CN_R2_COPYPEN);

                }


                x_bmap++;
            }
            y_bmap++;
        }
    }
#if 0
    mirror = (struct DisplayRsc *)display->node.child;
#else
    mirror = (struct DisplayRsc *)OBJ_Child(&display->node);
#endif
    if(mirror == NULL)      //没有镜像显示器，直接返回
        return;
    current = mirror;
    while(current != NULL)
    {
        current->draw.CopyBitmapToScreen(rect,bitmap,x,y);
        current = (struct DisplayRsc *)OBJ_TraveScion(&mirror->node,
                                                        &current->node);
    }
}

//----填充矩形-----------------------------------------------------------------
//功能: 在dst_bitmap中dst_rect所占据的位置上，用color颜色填充。color的格式是
//      cn_sys_pf_e8r8g8b8，要转换成dst_bitmap->pf_type匹配的颜色填充。
//参数: dst_bitmap，被填充的位图
//      dst_rect，填充的位置
//      color，填充的颜色，格式是cn_sys_pf_e8r8g8b8
//返回: 无
//-----------------------------------------------------------------------------
void __GK_FillRect(struct RectBitmap *dst_bitmap,
                     struct Rectangle *dst_rect,u32 color)
{
    s32 x,y,x1,y1,x2,y2;
    u8 bit_offset,i;
    u8 color_bits;
    s32 byte_offset,temp_bytes;
    u32 pf,pf_color;
    u8 bit_offset1,bit_offset2,LeftColor,LeftMsk,RightColor,RightMsk,FillColor;
    s32 byte_offset1,byte_offset2;

    x1 = dst_rect->left;
    y1 = dst_rect->top;
    y2 = dst_rect->bottom;
    x2 = dst_rect->right;
    temp_bytes = dst_bitmap->linebytes;
    //color的格式是cn_sys_pf_e8r8g8b8，转换成dst_bitmap->pf_type匹配的格式
    pf_color = GK_ConvertRGB24ToPF(dst_bitmap->PixelFormat,color);
    //被填充的位图的每像素位宽
    color_bits = (dst_bitmap->PixelFormat & CN_PF_BITWIDE_MSK)>>8;
    LeftColor = 0;
    RightColor = 0;
    switch(color_bits)
    {
        case 1:
        {
            pf = pf_color & 0x01;               //取给定的填充颜色
            byte_offset1 = y1*temp_bytes+x1/8;  //dst_rect首行左边界所在字节
            // -1是因为矩形右边界是不包含在绘制坐标内的。
            byte_offset2 = y1*temp_bytes+(x2-1)/8;  //dst_rect首行右边界所在字节
            bit_offset1 = x1%8;   //dst_rect左边界在所在字节的位，x=0，bit=7
            bit_offset2 = x2%8;     //7减去dst_rect右边界在所在字节的位，7-bit
            if(pf == 1)
                FillColor = 0xff;
            else
                FillColor = 0;
            //dst_rect的首字节与尾字节不在一个字节内
            if(byte_offset1 != byte_offset2)
            {
                //dst_rect每行的首字节各位的颜色
                //首字节取颜色的位数与bit相同，右边是高位
                LeftColor = FillColor &(0xff >> bit_offset1);//左端像素在字节低位
                RightColor = FillColor << (8-bit_offset2);  //右端像素在字节高位
                LeftMsk = ~(0xff>>bit_offset1);     //左端须保留的位
                RightMsk = (0xff>>bit_offset2);     //右端须保留的位
                for(y = y1;y < y2;y++)  //填充颜色
                {
                    //填充dst_rect每行中间字节的颜色
                    for(x = byte_offset1+1; x < byte_offset2; x++)
                    {
                        dst_bitmap->bm_bits[x] = FillColor;
                    }
                    dst_bitmap->bm_bits[byte_offset1] &= LeftMsk;
                    dst_bitmap->bm_bits[byte_offset1] |= LeftColor;  //字节头赋值
                    dst_bitmap->bm_bits[byte_offset2] &= RightMsk;
                    dst_bitmap->bm_bits[byte_offset2] |= RightColor; //字节尾赋值
                    //字节数换行
                    byte_offset1 += temp_bytes;
                    byte_offset2 += temp_bytes;
                }
            }
            else//dst_rect首字节与尾字节在一个字节内
            {   //bit1在左，bit2在右，右边为高位，此处的bit_offset2=7-bit2
                for(i = 7-bit_offset2;i <= bit_offset1;i++)
                {
                    RightColor |= 1<<i;
                    LeftColor |= pf<<i;
                }
                LeftColor = ((u8)0xff << bit_offset1)>>bit_offset1;
                LeftColor = (LeftColor >> (8-bit_offset2))<<(8-bit_offset2);
                RightColor = LeftColor & FillColor;
                for(y = y1;y < y2;y++)
                {   //保留目标字节内不需要填充的位
                    dst_bitmap->bm_bits[byte_offset1] &= ~LeftColor;
                    //填充相应的位
                    dst_bitmap->bm_bits[byte_offset1] |= RightColor;
                    byte_offset1 += temp_bytes; //字节数换行
                }
            }
        }break;
        case 2:
        {
            pf = pf_color & 0x03;               //取给定的填充颜色
            byte_offset1 = y1*temp_bytes+x1/4;  //dst_rect首行左边界所在字节
            // -1是因为矩形右边界是不包含在绘制坐标内的。
            byte_offset2 = y1*temp_bytes+(x2-1)/4;  //dst_rect首行右边界所在字节
            bit_offset1 = (x1%4)<<1;//dst_rect左边界在所在字节的位，x=0，bit=7
            bit_offset2 = (x2%4)<<1;//7减去dst_rect右边界在所在字节的位，7-bit
            FillColor = (u8)((pf<<6)|(pf<<4)|(pf<<2)|pf);//每行中间字节的颜色
            //dst_rect首行的首字节与尾字节不在一个字节内
            if(byte_offset1 != byte_offset2)
            {   //dst_rect每行的首字节各位的颜色
                LeftColor = FillColor &(0xff >> bit_offset1);//左端像素在字节低位
                RightColor = FillColor << (8-bit_offset2);  //右端像素在字节高位
                LeftMsk = ~(0xff>>bit_offset1);     //左端须保留的位
                RightMsk = (0xff>>bit_offset2);     //右端须保留的位
                for(y = y1;y < y2;y++)  //填充颜色
                {
                    //填充dst_rect每行中间字节的颜色
                    for(x = byte_offset1+1; x < byte_offset2; x++)
                    {
                        dst_bitmap->bm_bits[x] = FillColor;
                    }
                    dst_bitmap->bm_bits[byte_offset1] &= LeftMsk;
                    dst_bitmap->bm_bits[byte_offset1] |= LeftColor;  //字节头赋值
                    dst_bitmap->bm_bits[byte_offset2] &= RightMsk;
                    dst_bitmap->bm_bits[byte_offset2] |= RightColor;    //字节尾赋值
                    //字节数换行
                    byte_offset1 += temp_bytes;
                    byte_offset2 += temp_bytes;
                }
            }
            else//dst_rect首行的首字节与尾字节在一个字节内
            {
                //左边像素在字节高位
                for(i = 6-bit_offset2;i <= bit_offset1;i = i+2)
                {
                    RightColor |= 0x3<<i;
                    LeftColor |= pf<<i;
                }
                LeftColor = ((u8)0xff << bit_offset1)>>bit_offset1;
                LeftColor = (LeftColor >> (8-bit_offset2))<<(8-bit_offset2);
                RightColor = LeftColor & FillColor;
                for(y = y1;y < y2;y++)
                {   //保留目标字节内不需要填充的位
                    dst_bitmap->bm_bits[byte_offset1] &= ~LeftColor;
                    //填充相应的位
                    dst_bitmap->bm_bits[byte_offset1] |= RightColor;
                    byte_offset1 += temp_bytes; //字节数换行
                }
            }
        }break;
        case 4:
        {
            pf = pf_color & 0xf;                //取给定的填充颜色
            byte_offset1 = y1*temp_bytes+x1/2;  //dst_rect首行左边界所在字节
            // -1是因为矩形右边界是不包含在绘制坐标内的。
            byte_offset2 = y1*temp_bytes+(x2-1)/2;  //dst_rect首行右边界所在字节
            bit_offset1 = (x1%2)<<2;//dst_rect左边界在所在字节的位，x=0，bit=7
            bit_offset2 = (x2%2)<<2;//7减去dst_rect右边界在所在字节的位，7-bit
            FillColor = (u8)((pf<<4)|pf);     //每行中间字节的颜色
            //dst_rect首行的首字节与尾字节不在一个字节内
            if(byte_offset1 != byte_offset2)
            {
                LeftColor = FillColor &(0xff >> bit_offset1);//左端像素在字节低位
                RightColor = FillColor << (8-bit_offset2);  //右端像素在字节高位
                LeftMsk = ~(0xff>>bit_offset1);     //左端须保留的位
                RightMsk = (0xff>>bit_offset2);     //右端须保留的位
                for(y = y1;y < y2;y++)          //填充颜色
                {
                    //填充dst_rect每行中间字节的颜色
                    for(x = byte_offset1+1; x < byte_offset2; x++)
                    {
                        dst_bitmap->bm_bits[x] = FillColor;
                    }
                    dst_bitmap->bm_bits[byte_offset1] &= LeftMsk;
                    dst_bitmap->bm_bits[byte_offset1] |= LeftColor;  //字节头赋值
                    dst_bitmap->bm_bits[byte_offset2] &= RightMsk;
                    dst_bitmap->bm_bits[byte_offset2] |= RightColor;    //字节尾赋值
                    //字节数换行
                    byte_offset1 += temp_bytes;
                    byte_offset2 += temp_bytes;
                }
            }
            else//dst_rect首行的首字节与尾字节在一个字节内
            {   //高位在左边
                if(x1 & 1)
                {
                    LeftMsk = 0xf0;
                    LeftColor = FillColor >> 4;
                }
                else
                    LeftColor = FillColor << 4;
                if((x2-1) & 1)
                    LeftColor |= FillColor >> 4;
                else
                {
                    LeftMsk = 0x0f;
                    LeftColor &= FillColor << 4;
                }

                for(y = y1;y < y2;y++)
                {   //保留目标字节内不需要填充的位
                    dst_bitmap->bm_bits[byte_offset1] &= LeftMsk;
                    //填充相应的位
                    dst_bitmap->bm_bits[byte_offset1] |= LeftColor;
                    byte_offset1 += temp_bytes; //字节数换行
                }
            }
        }break;
        case 8:
        {
            pf = pf_color;                      //取给定的填充颜色
            byte_offset1 = y1*temp_bytes+x1;    //dst_rect首行左边界所在字节
            byte_offset2 = y1*temp_bytes+x2;    //dst_rect首行右边界所在字节
            for(y = y1;y < y2;y++)      //填充颜色，每次填充一个像素
            {
                //填充dst_rect每行中间字节的颜色
                for(x = byte_offset1;x <= byte_offset2;x++)
                {
                    dst_bitmap->bm_bits[x] = (u8)pf;
                }
                //字节数换行
                byte_offset1 += temp_bytes;
                byte_offset2 += temp_bytes;
             }
        }break;
        case 12:
        {
            pf = pf_color;           //取给定的填充颜色
            for(y = y1;y < y2;y++)  //填充颜色
            {
                temp_bytes = y*temp_bytes;              //y行的字节偏移量
                for(x = x1;x < x2;x++)
                {
                    byte_offset = temp_bytes + x*3/2;   //目标像素所在字节
                    bit_offset = (x*12)%8;              //目标像素的位偏移量
                    if(bit_offset == 0)
                    {
                        dst_bitmap->bm_bits[byte_offset] = (u8)(pf&0xff);
                        dst_bitmap->bm_bits[byte_offset+1] &= 0xf0;
                        dst_bitmap->bm_bits[byte_offset+1] |= (u8)(pf>>8);
                    }else
                    {
                        dst_bitmap->bm_bits[byte_offset] &= 0x0f;
                        dst_bitmap->bm_bits[byte_offset] |= (u8)pf<<4;
                        dst_bitmap->bm_bits[byte_offset+1] = (u8)(pf>>4);
                    }
                }
             }
        }break;
        case 16:
        {
            pf = pf_color;                       //取给定的填充颜色
            if((u32)dst_bitmap->bm_bits & 1 )      //非对齐地址
            {
                byte_offset1 = y1*temp_bytes+x1*2;  //dst_rect首行左边界所在字节
                byte_offset2 = y1*temp_bytes+x2*2;  //dst_rect首行右边界所在字节
                for(y = y1;y < y2;y++)              //填充颜色，每次填充两个像素
                {
                    for(x = byte_offset1;x <= byte_offset2;x = x+2)
                    {
    //                    alpha = pf;
    //                    if(dst_bitmap->PixelFormat == CN_SYS_PF_ARGB1555)
    //                    {
    //                        pf &= 0x7fff;
    //                        pf |= alpha & 0x8000;
    //                    }
                        //注:因对齐问题，不可把目标地址强制转换成16位指针。
                        dst_bitmap->bm_bits[x] = (u8)pf;
                        dst_bitmap->bm_bits[x+1] = (u8)(pf>>8);
                    }
                    //字节数换行
                    byte_offset1 += temp_bytes;
                    byte_offset2 += temp_bytes;
                }
            }
            else                        //对齐的地址
            {
                u16 *vm;
                vm = (u16*)((u32)dst_bitmap->bm_bits+y1*dst_bitmap->linebytes);
                for(y = y1;y < y2;y++)              //填充颜色，每次填充两个像素
                {
                    for(x = x1;x < x2;x++)
                    {
    //                    alpha = pf;
    //                    if(dst_bitmap->PixelFormat == CN_SYS_PF_ARGB1555)
    //                    {
    //                        pf &= 0x7fff;
    //                        pf |= alpha & 0x8000;
    //                    }
                        vm[x] = pf;
                    }
                    vm += dst_bitmap->linebytes>>1;
                }

            }
        }break;
        case 24:
        {
            pf = pf_color;                      //取给定的填充颜色
            byte_offset1 = y1*temp_bytes+x1*3;  //dst_rect首行左边界所在字节
            byte_offset2 = y1*temp_bytes+x2*3;  //dst_rect首行右边界所在字节
            for(y = y1;y < y2;y++)              //填充颜色，每次填充一个像素
            {
                for(x = byte_offset1;x <= byte_offset2;x = x+3)
                {
                    dst_bitmap->bm_bits[x] = (u8)pf;
                    dst_bitmap->bm_bits[x+1] = (u8)(pf >> 8);
                    dst_bitmap->bm_bits[x+2] = (u8)(pf >> 16);
                }
                //字节数换行
                byte_offset1 += temp_bytes;
                byte_offset2 += temp_bytes;
             }
        }break;
        case 32:
        {
            pf = pf_color;                      //取给定的填充颜色
            byte_offset1 = y1*temp_bytes+x1*4;  //dst_rect首行左边界所在字节
            byte_offset2 = y1*temp_bytes+x2*4;  //dst_rect首行右边界所在字节
            for(y = y1;y < y2;y++)              //填充颜色，每次填充一个像素
            {
                for(x = byte_offset1;x <= byte_offset2;x = x+4)
                {
                    pf = pf_color;
//                    alpha = pf;
//                    if(dst_bitmap->PixelFormat == CN_SYS_PF_ARGB8888)
//                    {
//                        pf &= 0x00ffffff;
//                        pf |= alpha & 0xff000000;
//                    }
                    //注:因对齐问题，不可把目标地址强制转换成32位指针。
                    dst_bitmap->bm_bits[x] = (u8)pf;
                    dst_bitmap->bm_bits[x+1] = (u8)(pf >> 8);
                    dst_bitmap->bm_bits[x+2] = (u8)(pf >> 16);
                    dst_bitmap->bm_bits[x+3] = (u8)(pf >> 24);
                }
                //字节数换行
                byte_offset1 += temp_bytes;
                byte_offset2 += temp_bytes;
             }
        }break;
    }
}

//----填充位图-----------------------------------------------------------------
//功能: 本函数是__GK_FillRect的特殊情况，即需要填充的矩形是整个bitmap。
//参数: dst_bitmap,被填充的位图
//      color，填充的颜色，格式是cn_sys_pf_e8r8g8b8
//返回: 无
//-----------------------------------------------------------------------------
void __GK_FillBm(struct RectBitmap *dst_bitmap,u32 color)
{
    u32 pf_color,color_bits;
    u32 byte_offset,total_bytes;

    //color的格式是cn_sys_pf_e8r8g8b8，转换成dst_bitmap->pf_type匹配的格式
    pf_color = GK_ConvertRGB24ToPF(dst_bitmap->PixelFormat,color);
    //被填充的位图的每像素位宽
    color_bits = (dst_bitmap->PixelFormat & CN_PF_BITWIDE_MSK)>>8;
    switch(color_bits)
    {
        case 1:
        {//组合颜色，直接将相应的位置数
            if(1 == pf_color)
                memset( dst_bitmap->bm_bits,0xff,
                        dst_bitmap->linebytes*dst_bitmap->height);
            else
                memset( dst_bitmap->bm_bits,0,
                        dst_bitmap->linebytes*dst_bitmap->height);
        }break;
        case 2:
        {//组合颜色，直接将相应的位置数
            memset( dst_bitmap->bm_bits,
                    pf_color + (pf_color<<2) + (pf_color<<4) + (pf_color<<6),
                    dst_bitmap->linebytes*dst_bitmap->height);
        }break;
        case 4:
        {//组合颜色，直接将相应的位置数
            memset( dst_bitmap->bm_bits,(u8)(pf_color|(pf_color<<4)),
                    dst_bitmap->linebytes*dst_bitmap->height);
        }break;
        case 8:
        {//直接将相应的位置数
            memset( dst_bitmap->bm_bits,(u8)pf_color,
                    dst_bitmap->linebytes*dst_bitmap->height);
        }break;
        case 12:
        {   //总共需要填充的字节数
            total_bytes = dst_bitmap->linebytes*dst_bitmap->height;
            //12位的颜色格式，一次可填充3个字节，即两个像素
            for(byte_offset = 0;byte_offset < total_bytes;)
            {
                dst_bitmap->bm_bits[byte_offset] = (u8)(pf_color&0xff);
                dst_bitmap->bm_bits[byte_offset+1] = (u8)((pf_color>>8)&0xf);
                dst_bitmap->bm_bits[byte_offset+1] |= (u8)((pf_color&0xf)<<4);
                dst_bitmap->bm_bits[byte_offset+2] = (u8)(pf_color>>4);
                byte_offset += 3;
            }
        }break;
        case 16:
        {   //总共需要填充的字节数
            total_bytes = dst_bitmap->linebytes*dst_bitmap->height;
            if((u32)dst_bitmap->bm_bits & 1 )
            {
                for(byte_offset = 0;byte_offset < total_bytes;byte_offset += 2)
                {
    //                alpha = pf_color;
    //                if(dst_bitmap->PixelFormat == CN_SYS_PF_ARGB1555)
    //                {
    //                    pf_color &= 0x7fff;
    //                    pf_color |= alpha & 0x8000;
    //                }
                    //注:因对齐问题，不可把目标地址强制转换成16位指针。
                    //一次写一个像素，两个字节
                    dst_bitmap->bm_bits[byte_offset] = (u8)pf_color;
                    dst_bitmap->bm_bits[byte_offset+1] = (u8)(pf_color>>8);
                }
            }
            else
            {
                u16 *vm;
                vm = (u16*)(u32)dst_bitmap->bm_bits;
                for(byte_offset = 0;byte_offset < total_bytes>>1;byte_offset++)
                {
                    vm[byte_offset] = pf_color;
                }
            }
        }break;
        case 24:
        {   //总共需要填充的字节数
            total_bytes = dst_bitmap->linebytes*dst_bitmap->height;
            for(byte_offset = 0;byte_offset < total_bytes;)
            {
                dst_bitmap->bm_bits[byte_offset] = (u8)pf_color;
                dst_bitmap->bm_bits[byte_offset+1] = (u8)(pf_color >> 8);
                dst_bitmap->bm_bits[byte_offset+2] = (u8)(pf_color >> 16);
                byte_offset += 3;   //一次写一个像素，三个字节
             }
        }break;
        case 32:
        {   //总共需要填充的字节数
            total_bytes = dst_bitmap->linebytes*dst_bitmap->height;
            for(byte_offset = 0;byte_offset < total_bytes;)
            {
//                alpha = pf_color;
//                if(dst_bitmap->PixelFormat == CN_SYS_PF_ARGB8888)
//                {
//                    pf_color &= 0x00ffffff;
//                    pf_color |= alpha & 0xff000000;
//                }
                //注:因对齐问题，不可把目标地址强制转换成32位指针。
                dst_bitmap->bm_bits[byte_offset] = (u8)pf_color;
                dst_bitmap->bm_bits[byte_offset+1] = (u8)(pf_color >> 8);
                dst_bitmap->bm_bits[byte_offset+2] = (u8)(pf_color >> 16);
                dst_bitmap->bm_bits[byte_offset+3] = (u8)(pf_color >> 24);
                byte_offset += 4;   //一次写一个像素，四个字节
             }
        }break;
    }
}
//----绘制8个像素点------------------------------------------------------------
//功能: 绘制8个像素点，在画圆的情况下调用，利用了圆的八分特性，
//      同时标志changed_msk。
//参数: gkwin，目标窗口
//      bitmap，目标位图
//      x0、y0，圆心坐标
//      x、y，要绘制的点的坐标，相对于圆心
//      color，给定的颜色
//      rop2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
void __gk_set_all_pixels_circle(struct GkWinRsc *gkwin,
                                struct RectBitmap *bitmap,s32 x0,s32 y0,
                                s32 x,s32 y,u32 color,u32 Rop2Code)
{
    //绘制8个像素点
    __GK_SetPixelRop2Bm(bitmap,x0+x,y0+y,color,Rop2Code);
    __GK_SetPixelRop2Bm(bitmap,x0+x,y0-y,color,Rop2Code);
    __GK_SetPixelRop2Bm(bitmap,x0-x,y0+y,color,Rop2Code);
    __GK_SetPixelRop2Bm(bitmap,x0-x,y0-y,color,Rop2Code);
    __GK_SetPixelRop2Bm(bitmap,x0+y,y0+x,color,Rop2Code);
    __GK_SetPixelRop2Bm(bitmap,x0+y,y0-x,color,Rop2Code);
    __GK_SetPixelRop2Bm(bitmap,x0-y,y0+x,color,Rop2Code);
    __GK_SetPixelRop2Bm(bitmap,x0-y,y0-x,color,Rop2Code);
    //对8个像素点分别进行changed_msk标志
    __GK_SetPixelRop2Bm(&(gkwin->changed_msk),(x0+x)/8,(y0+y)/8,
                                CN_COLOR_WHITE,CN_R2_COPYPEN);
    __GK_SetPixelRop2Bm(&(gkwin->changed_msk),(x0+x)/8,(y0-y)/8,
                                CN_COLOR_WHITE,CN_R2_COPYPEN);
    __GK_SetPixelRop2Bm(&(gkwin->changed_msk),(x0-x)/8,(y0+y)/8,
                                CN_COLOR_WHITE,CN_R2_COPYPEN);
    __GK_SetPixelRop2Bm(&(gkwin->changed_msk),(x0-x)/8,(y0-y)/8,
                                CN_COLOR_WHITE,CN_R2_COPYPEN);
    __GK_SetPixelRop2Bm(&(gkwin->changed_msk),(x0+y)/8,(y0+x)/8,
                                CN_COLOR_WHITE,CN_R2_COPYPEN);
    __GK_SetPixelRop2Bm(&(gkwin->changed_msk),(x0+y)/8,(y0-x)/8,
                                CN_COLOR_WHITE,CN_R2_COPYPEN);
    __GK_SetPixelRop2Bm(&(gkwin->changed_msk),(x0-y)/8,(y0+x)/8,
                                CN_COLOR_WHITE,CN_R2_COPYPEN);
    __GK_SetPixelRop2Bm(&(gkwin->changed_msk),(x0-y)/8,(y0-x)/8,
                                CN_COLOR_WHITE,CN_R2_COPYPEN);
}
//----绘制目标点并着色---------------------------------------------------------
//功能: 绘制目标点，同时标志changed_msk。
//参数: gkwin，目标窗口
//      limit，绘制的限制区，只绘制直线在limit矩形内部的部分
//      bitmap，目标位图
//      x、y，要绘制的点的坐标
//      color，给定的颜色
//      rop2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
void __gk_pixel_sect_inter(struct GkWinRsc *gkwin,struct Rectangle *limit,
                                struct RectBitmap *bitmap,s32 x,s32 y,
                                u32 color,u32 Rop2Code)
{
    if((x >= limit->left)&&(x < limit->right)
        &&(y >= limit->top)&&(y < limit->bottom))
    {//待绘制的目标点在限制区内，画点并标志changed_msk
        __GK_SetPixelRop2Bm(bitmap,x,y,color,Rop2Code);
        __GK_SetPixelRop2Bm(&(gkwin->changed_msk),x/8,y/8,
                                CN_COLOR_WHITE,CN_R2_COPYPEN);
    }
    else    //待绘制的目标点在限制区外，直接返回
        return;
}
//----绘制多个目标点并标志-----------------------------------------------------
//功能: 绘制多个目标点，同时标志changed_msk。
//参数: gkwin，目标窗口
//      limit，绘制的限制区，只绘制直线在limit矩形内部的部分
//      bitmap，目标位图
//      x0、y0，圆心坐标
//      x、y，要绘制的点的坐标
//      color，给定的颜色
//      rop2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
void __gk_set_part_pixels_circle(struct GkWinRsc *gkwin,struct Rectangle *limit,
                                struct RectBitmap *bitmap,s32 x0,s32 y0,
                                s32 x,s32 y,u32 color,u32 Rop2Code)
{
    __gk_pixel_sect_inter(gkwin,limit,bitmap,x0+x,y0+y,color,Rop2Code);
    __gk_pixel_sect_inter(gkwin,limit,bitmap,x0+x,y0-y,color,Rop2Code);
    __gk_pixel_sect_inter(gkwin,limit,bitmap,x0-x,y0+y,color,Rop2Code);
    __gk_pixel_sect_inter(gkwin,limit,bitmap,x0-x,y0-y,color,Rop2Code);
    __gk_pixel_sect_inter(gkwin,limit,bitmap,x0+y,y0+x,color,Rop2Code);
    __gk_pixel_sect_inter(gkwin,limit,bitmap,x0+y,y0-x,color,Rop2Code);
    __gk_pixel_sect_inter(gkwin,limit,bitmap,x0-y,y0+x,color,Rop2Code);
    __gk_pixel_sect_inter(gkwin,limit,bitmap,x0-y,y0-x,color,Rop2Code);
}
//----画圆---------------------------------------------------------------------
//功能: 在窗口内画一个圆。
//参数: gkwin，目标窗口
//      limit，绘制的限制区，只绘制直线在limit矩形内部的部分
//      x0、y0，圆心坐标
//      r，圆的半径
//      color，给定的颜色
//      rop2_code，二元光栅操作码
//返回: 无
//说明: 使用中点画圆算法
//-----------------------------------------------------------------------------
void __GK_DrawCircleBm(struct GkWinRsc *gkwin,struct Rectangle *limit,
                        s32 x0,s32 y0,s32 r,u32 color,u32 Rop2Code)
{
    s32 x,y,e;
    s32 left,right,top,bottom;
    u32 pf_color,flag;
    struct RectBitmap *bitmap;

    //选取(x,y)为(0,r)作为初始坐标(不是实际的圆，平移到原点的圆)，圆上的特殊点
    x = 0;
    y = r;
    e = 1-r;
    left = limit->left-x0;      //窗口左边界到圆心x方向的距离
    top = limit->top-y0;        //窗口左边界到圆心y方向的距离
    right = limit->right-x0;    //窗口右边界到圆心x方向的距离
    bottom = limit->bottom-y0;  //窗口右边界到圆心y方向的距离
    bitmap = gkwin->wm_bitmap;
    //要求给定的颜色的颜色格式为24位
    //绘制像素前需将24位调整为与显示屏一致的颜色格式
    pf_color = GK_ConvertRGB24ToPF(gkwin->disp->pixel_format,color);

    //整个圆都在limit内
    if((x0-limit->left >= r)&&(limit->right-x0 > r)
        &&(y0-limit->top >= r)&&(limit->bottom-y0 > r))
        flag = 1;
    //圆上的点不都在limit内，分整个圆在窗口外和部分在窗口外两种情况
    else if((left*left+top*top < r*r)
        &&(right*right+top*top < r*r)
        &&(left*left+bottom*bottom < r*r)
        &&(right*right+bottom*bottom < r*r))
        return;
    //圆上的点部分在limit内
    else
        flag = 0;
    if(flag)
    {//整个圆都在limit内，计算出四分之一个圆的坐标
     //根据对称性，可得圆上其它所有点的坐标
        while(x <= y)
        {
            //不做判断，绘制8个像素，同时标志像素的changed_msk
            __gk_set_all_pixels_circle(gkwin,bitmap,x0,y0,x,y,
                                                pf_color,Rop2Code);
            if(e < 0)
                e += x*2+3;
            else
            {
                e += 2*(x-y)+5;
                y--;
            }
            x++;
        }
    }
    else
    {//圆上部分点在limit内，需要对圆上各个点是否在limit内做出判断
        while(x <= y)
        {
            //判断要绘制点的位置后，决定是否绘制，绘制的像素同时标志changed_msk
            __gk_set_part_pixels_circle(gkwin,limit,bitmap,x0,y0,x,y,
                                                pf_color,Rop2Code);
            if(e < 0)
                e += x*2+3;
            else
            {
                e += 2*(x-y)+5;
                y--;
            }
            x++;
        }
    }
    if(gkwin->WinProperty.ChangeFlag != CN_GKWIN_CHANGE_ALL)
        //画一个圆只改变了窗口的部分内容
        gkwin->WinProperty.ChangeFlag = CN_GKWIN_CHANGE_PART;
}
//----目标点的绘制-------------------------------------------------------------
//功能: 据目标点所在的位置，得其是否需要绘制。
//参数: gkwin，目标窗口
//      limit，绘制的限制区，只绘制直线在limit矩形内部的部分
//      x、y，目标点的坐标
//      color，画圆使用的颜色
//      rop2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
void __Gk_PixelSectInterScreen(struct DisplayRsc *display,
                                    struct Rectangle *limit,
                                    s32 x,s32 y,u32 color,u32 Rop2Code)
{
    if((x >= limit->left)&&(x < limit->right)
        &&(y >= limit->top)&&(y < limit->bottom))
    //目标点在limit内，需要绘制
        display->draw.SetPixelToScreen(x,y,color,Rop2Code);
    else
        return;
}

//----在screen上画圆-----------------------------------------------------------
//功能: 在screen上画一个圆。
//参数: gkwin，目标窗口
//      limit，绘制的限制区，只绘制直线在limit矩形内部的部分
//      x0、y0，圆心坐标
//      r，圆的半径
//      color，画圆使用的颜色
//      rop2_code，二元光栅操作码
//返回: 无
//使用中点画圆算法
//-----------------------------------------------------------------------------
void __GK_DrawCircleScreen(struct DisplayRsc *display,struct Rectangle *limit,
                        s32 x0,s32 y0,s32 r,u32 color,u32 Rop2Code)//确认
{
    s32 x,y,e;
    s32 left,right,top,bottom;
    u32 flag;
    struct DisplayRsc *mirror,*current;
    struct DispDraw *mydraw;

    mydraw = &display->draw;
    //选取(x,y)为(0,r)作为初始坐标(不是实际的圆，平移到原点的圆)，圆上的特殊点
    x = 0;
    y = r;
    e = 1-r;
    left = x0 - limit->left;
    top = y0 - limit->top;
    right = limit->right-x0;
    bottom = limit->bottom-y0;
//    pf_color = GK_ConvertRGB24ToPF(display->pixel_format,color);

    //整个圆都在limit内
    if((x0-limit->left >= r)&&(limit->right-x0 > r)
        &&(y0-limit->top >= r)&&(limit->bottom-y0 > r))
        flag = 1;

    //圆上的点都不在limit内
    else if((left*left+top*top < r*r)
        &&(right*right+top*top < r*r)
        &&(left*left+bottom*bottom < r*r)
        &&(right*right+bottom*bottom < r*r))
        return;
    //圆上的点部分在limit内
    else
        flag = 0;

    if(flag)
    {//整个圆都在limit内，利用圆的八分特性，只需要计算八分之一个圆的坐标
        while(x <= y)
        {
            mydraw->SetPixelToScreen(x0+x,y0+y,color,Rop2Code);
            mydraw->SetPixelToScreen(x0+x,y0-y,color,Rop2Code);
            mydraw->SetPixelToScreen(x0-x,y0+y,color,Rop2Code);
            mydraw->SetPixelToScreen(x0-x,y0-y,color,Rop2Code);
            mydraw->SetPixelToScreen(x0+y,y0+x,color,Rop2Code);
            mydraw->SetPixelToScreen(x0+y,y0-x,color,Rop2Code);
            mydraw->SetPixelToScreen(x0-y,y0+x,color,Rop2Code);
            mydraw->SetPixelToScreen(x0-y,y0-x,color,Rop2Code);
            if(e < 0)
                e += x*2+3;
            else
            {
                e += 2*(x-y)+5;
                y--;
            }
            x++;
        }
    }
    else
    {//圆上的点部分在limit内，需要考虑圆上各个点是否在limit内
        while(x <= y)
        {
            __Gk_PixelSectInterScreen(display,limit,
                                x0+x,y0+y,color,Rop2Code);
            __Gk_PixelSectInterScreen(display,limit,
                                x0+x,y0-y,color,Rop2Code);
            __Gk_PixelSectInterScreen(display,limit,
                                x0-x,y0+y,color,Rop2Code);
            __Gk_PixelSectInterScreen(display,limit,
                                x0-x,y0-y,color,Rop2Code);
            __Gk_PixelSectInterScreen(display,limit,
                                x0+y,y0+x,color,Rop2Code);
            __Gk_PixelSectInterScreen(display,limit,
                                x0+y,y0-x,color,Rop2Code);
            __Gk_PixelSectInterScreen(display,limit,
                                x0-y,y0+x,color,Rop2Code);
            __Gk_PixelSectInterScreen(display,limit,
                                x0-y,y0-x,color,Rop2Code);
            if(e < 0)
                e += x*2+3;
            else
            {
                e += 2*(x-y)+5;
                y--;
            }
            x++;
        }
    }
    //镜像显示
#if 0
    mirror = (struct DisplayRsc *)display->node.child;
#else
    mirror = (struct DisplayRsc *)OBJ_Child(&display->node);
#endif
    if(mirror == NULL)      //没有镜像显示器，直接返回
        return;
    current = mirror;
    while(current != NULL)
    {//存在镜像显示器
        x = 0;
        y = r;
        if(flag)
        {//整个圆都在limit内，利用圆的八分特性，只需要计算八分之一个圆的坐标
            while(x <= y)
            {
                current->draw.SetPixelToScreen(x0+x,y0+y,color,Rop2Code);
                current->draw.SetPixelToScreen(x0+x,y0-y,color,Rop2Code);
                current->draw.SetPixelToScreen(x0-x,y0+y,color,Rop2Code);
                current->draw.SetPixelToScreen(x0-x,y0-y,color,Rop2Code);
                current->draw.SetPixelToScreen(x0+y,y0+x,color,Rop2Code);
                current->draw.SetPixelToScreen(x0+y,y0-x,color,Rop2Code);
                current->draw.SetPixelToScreen(x0-y,y0+x,color,Rop2Code);
                current->draw.SetPixelToScreen(x0-y,y0-x,color,Rop2Code);
                if(e < 0)
                    e += x*2+3;
                else
                {
                    e += 2*(x-y)+5;
                    y--;
                }
                x++;
            }
        }
        else
        {//圆上的点部分在limit内，需要考虑圆上各个点是否在limit内
            while(x <= y)
            {
                //只绘制在limit内的像素
                if((x0+x >= limit->left)&&(x0+x < limit->right)
                        &&(y0+y >= limit->top)&&(y0+y < limit->bottom))
                    current->draw.SetPixelToScreen(x0+x,y0+y,color,Rop2Code);
                if((x0+x >= limit->left)&&(x0+x < limit->right)
                        &&(y0-y >= limit->top)&&(y0+y < limit->bottom))
                    current->draw.SetPixelToScreen(x0+x,y0-y,color,Rop2Code);
                if((x0-x >= limit->left)&&(x0-x < limit->right)
                        &&(y0+y >= limit->top)&&(y0+y < limit->bottom))
                    current->draw.SetPixelToScreen(x0-x,y0+y,color,Rop2Code);
                if((x0-x >= limit->left)&&(x0-x < limit->right)
                        &&(y0-y >= limit->top)&&(y0-y < limit->bottom))
                    current->draw.SetPixelToScreen(x0-x,y0-y,color,Rop2Code);
                if((x0+y >= limit->left)&&(x0+y < limit->right)
                        &&(y0+x >= limit->top)&&(y0+x < limit->bottom))
                    current->draw.SetPixelToScreen(x0+y,y0+x,color,Rop2Code);
                if((x0+y >= limit->left)&&(x0+y < limit->right)
                        &&(y0-x >= limit->top)&&(y0-x < limit->bottom))
                    current->draw.SetPixelToScreen(x0+y,y0-x,color,Rop2Code);
                if((x0-y >= limit->left)&&(x0-y < limit->right)
                        &&(y0+x >= limit->top)&&(y0+x < limit->bottom))
                    current->draw.SetPixelToScreen(x0-y,y0+x,color,Rop2Code);
                if((x0-y >= limit->left)&&(x0-y < limit->right)
                        &&(y0-x >= limit->top)&&(y0-x < limit->bottom))
                    current->draw.SetPixelToScreen(x0-y,y0-x,color,Rop2Code);
                if(e < 0)
                    e += x*2+3;
                else
                {
                    e += 2*(x-y)+5;
                    y--;
                }
                x++;
            }
        }
        current = (struct DisplayRsc *)OBJ_TraveScion(&mirror->node,
                        &current->node);
    }
}
//----画圆---------------------------------------------------------------------
//功能: 在窗口内画一个圆。
//参数: gkwin，目标窗口
//      x0、y0，圆心坐标，相对于窗口
//      r，圆的半径
//      color，画圆使用的颜色
//      rop2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
void GK_DrawCircle(struct GkscParaDrawCircle *para)//确认
{
    struct Rectangle limit;
    struct ClipRect *clip;
    struct GkWinRsc *fb_gkwin,*cirwin;
//    struct DispDraw *my_draw_fun;
    s32 offsetx,offsety;
    if((para->Rop2Code == CN_R2_NOP) || (para->Rop2Code > CN_R2_LAST))
        return;                                         //不执行操作
    if(para->r <= 0)                                         //无须绘制
        return;
    cirwin = para->gkwin;
//    my_draw_fun = &cirwin->disp->draw;
    //说明有win buffer，且直接写屏属性为false
//todo 修改by zhb 20160602
    if(cirwin->wm_bitmap!=NULL)
    {
        if((cirwin->wm_bitmap->bm_bits!=NULL)&&(cirwin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
      {   //处理方法:在win buffer中绘图，标志changed_msk
        limit.left = 0;
        limit.top = 0;
        limit.right = cirwin->wm_bitmap->width;
        limit.bottom = cirwin->wm_bitmap->height;
        //以硬件加速不支持圆的绘制考虑，用软件实现
        __GK_DrawCircleBm(cirwin,&limit,para->x0,para->y0,para->r,
                                    para->color,para->Rop2Code);
        }
    }
    else
    {
        clip = cirwin->visible_clip;
        if(clip == NULL)
            return ;
        fb_gkwin = cirwin->disp->frame_buffer;
        offsetx = cirwin->absx0;
        offsety = cirwin->absy0;
        //有frame buffer，且直接写屏属性为false
        if((fb_gkwin != NULL) && (cirwin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
        {   //处理方法:在frame buffer中绘图，但只绘gkwin中的可视区域
            do{
                //以硬件加速不支持圆的绘制考虑，用软件实现
                __GK_DrawCircleBm(fb_gkwin,&clip->rect,
                                    para->x0+offsetx,para->y0+offsety,
                                    para->r,para->color,para->Rop2Code);
                clip = clip->next;
            }while(clip != cirwin->visible_clip);
        }else
        //无win buffer，也无frame buffer，直接画在screen上
        //直接写屏属性为true，不管有无缓冲区，都直接画在screen上
        {
            do{
                __GK_DrawCircleScreen(cirwin->disp,&clip->rect,
                            para->x0+offsetx,para->y0+offsety,para->r,
                            para->color,para->Rop2Code);
                clip = clip->next;
            }while(clip != cirwin->visible_clip);
        }
    }
}

//----画贝塞尔曲线-------------------------------------------------------------
//功能: 在窗口上画贝塞尔曲线。
//参数: gkwin，目标窗口
//      limit，允许绘图的限制区域，一个矩形的区域，超出此区域的直线不画
//      x1、y1、x2、y2、x3、y3、x4、y4，端点坐标
//      color，绘图所用的颜色
//      rop2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
void __GK_BezierBm(struct GkWinRsc *gkwin,struct Rectangle *limit,
                    float x1,float y1,float x2,float y2,float x3,float y3,
                    float x4,float y4,u32 color,u32 Rop2Code)
{
    float a,b,c;
    float x12,y12,x23,y23,x34,y34;
    float x1223,y1223,x2334,y2334;
    float x,y,ab;

    //a、b、c是直线(x1,y1)、(x4,y4)以一般式ax+by+c=0时对应的各个参数
    a = y4-y1;
    b = x1-x4;
    c = y1*(x4-x1)-x1*(y4-y1);
    ab = a*a+b*b;

    //点(x2,y2)、(x3,y3)到直线(x1,y1)、(x4,y4)的距离小于1(个像素)，
    //则以直线代替曲线
    if(((a*x2+b*y2+c)*(a*x2+b*y2+c) < ab) &&((a*x3+b*y3+c)*(a*x3+b*y3+c) < ab))
    {
        __GK_LinetoBm(gkwin,limit,(s32)x1,(s32)y1,(s32)x4,(s32)y4,
                                color,Rop2Code);
        return;
    }
    x12 = x1+x2;
    y12 = y1+y2;
    x23 = x2+x3;
    y23 = y2+y3;
    x34 = x3+x4;
    y34 = y3+y4;

    x1223 = x12+x23;
    y1223 = y12+y23;
    x2334 = x23+x34;
    y2334 = y23+y34;

    x = x1223+x2334;
    y = y1223+y2334;

    //不满足中间两个控制点到以端点为起、终点的直线的距离小于1(个像素)时，
    //在曲线的中点处分割曲线
    __GK_BezierBm(gkwin,limit,x1,y1,x12/2,y12/2,
                    x1223/4,y1223/4,x/8,y/8,color,Rop2Code);
    __GK_BezierBm(gkwin,limit,x/8,y/8,x2334/4,y2334/4,
                    x34/2,y34/2,x4,y4,color,Rop2Code);
}

//----在screen上画贝塞尔曲线---------------------------------------------------
//功能: 在screen上画贝塞尔曲线。
//参数: gkwin，目标窗口
//      limit，允许绘图的限制区域，一个矩形的区域，超出此区域的直线不画
//      x1、y1、x2、y2、x3、y3、x4、y4，端点坐标
//      color，绘图所用的颜色
//      rop2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
void __GK_BezierScreen(struct DisplayRsc *display,struct Rectangle *limit,
                    float x1,float y1,float x2,float y2,float x3,float y3,
                    float x4,float y4,u32 color,u32 Rop2Code)
{
    float a,b,c;
    float x12,y12,x23,y23,x34,y34;
    float x1223,y1223,x2334,y2334;
    float x,y,ab;

    //a、b、c是直线(x1,y1)、(x4,y4)以一般式ax+by+c=0时对应的各个参数
    a = y4-y1;
    b = x1-x4;
    c = y1*(x4-x1)-x1*(y4-y1);
    ab = a*a+b*b;

    //点(x2,y2)、(x3,y3)到直线(x1,y1)、(x4,y4)的距离小于1(个像素)，
    //则以直线代替曲线
    if(((a*x2+b*y2+c)*(a*x2+b*y2+c) < ab) &&((a*x3+b*y3+c)*(a*x3+b*y3+c) < ab))
    {
        __GK_LinetoScreen(display,limit,(s32)x1,(s32)y1,(s32)x4,(s32)y4,
                           color,Rop2Code);
        return;
    }
    x12 = x1+x2;
    y12 = y1+y2;
    x23 = x2+x3;
    y23 = y2+y3;
    x34 = x3+x4;
    y34 = y3+y4;

    x1223 = x12+x23;
    y1223 = y12+y23;
    x2334 = x23+x34;
    y2334 = y23+y34;

    x = x1223+x2334;
    y = y1223+y2334;

    //不满足中间两个控制点到以端点为起、终点的直线的距离小于1(个像素)时，
    //在曲线的中点处分割曲线
    __GK_BezierScreen(display,limit,x1,y1,x12/2,y12/2,
                    x1223/4,y1223/4,x/8,y/8,color,Rop2Code);
    __GK_BezierScreen(display,limit,x/8,y/8,x2334/4,y2334/4,
                    x34/2,y34/2,x4,y4,color,Rop2Code);
}
//----画贝塞尔曲线-------------------------------------------------------------
//功能: 在窗口内画一条贝塞尔曲线。
//参数: 参数是结构para的成员
//      gkwin，目标窗口指针
//      x1、y1、x2、y2、x3、y3、x4、y4，绘制Bezier曲线的四个控制点
//      color，画线使用的颜色
//      r2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
void GK_Bezier(struct GkscParaBezier *para)
{
    s32 offsetx,offsety;
    struct Rectangle limit;
    struct ClipRect *clip;
    struct GkWinRsc *fb_gkwin,*bzrwin;
//    struct DispDraw *my_draw_fun;
    if((para->Rop2Code == CN_R2_NOP) || (para->Rop2Code > CN_R2_LAST))
        return;                                         //不执行操作
    bzrwin = para->gkwin;
//    my_draw_fun = &bzrwin->disp->draw;
    //说明有win buffer，且直接写屏属性为false
    if((bzrwin->wm_bitmap != NULL)
        && (bzrwin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
    {   //处理方法:在win buffer中绘图，标志changed_msk
        limit.left = 0;
        limit.top = 0;
        limit.right = bzrwin->wm_bitmap->width;
        limit.bottom = bzrwin->wm_bitmap->height;
        //以硬件加速不支持贝塞尔曲线的绘制考虑，用软件实现
        __GK_BezierBm(bzrwin,&limit,para->x1,para->y1,para->x2,para->y2,
                            para->x3,para->y3,para->x4,para->y4,
                            para->color,para->Rop2Code);
    }else
    {
        clip = bzrwin->visible_clip;
        if(clip == NULL)
            return ;
        fb_gkwin = bzrwin->disp->frame_buffer;
        offsetx = bzrwin->absx0;
        offsety = bzrwin->absy0;
        //有frame buffer，且直接写屏属性为false
        if((fb_gkwin != NULL) && (bzrwin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
        {   //处理方法:在frame buffer中绘图，但只绘gkwin中的可视区域
            do{
                //以硬件加速不支持贝塞尔曲线的绘制考虑，用软件实现
                __GK_BezierBm(fb_gkwin,&clip->rect,para->x1+offsetx,
                            para->y1+offsety,para->x2+offsetx,para->y2+offsety,
                            para->x3+offsetx,para->y3+offsety,para->x4+offsetx,
                            para->y4+offsety,para->color,para->Rop2Code);
                clip = clip->next;
            }while(clip != bzrwin->visible_clip);
        }else
        //无win buffer，也无frame buffer，直接画在screen上
        //直接写屏属性为true，不管有无缓冲区，直接画在screen上
        {
            do{
                __GK_BezierScreen(bzrwin->disp,&clip->rect,para->x1+offsetx,
                            para->y1+offsety,para->x2+offsetx,para->y2+offsety,
                            para->x3+offsetx,para->y3+offsety,para->x4+offsetx,
                            para->y4+offsety,para->color,para->Rop2Code);
                clip = clip->next;
            }while(clip != bzrwin->visible_clip);
        }
    }
}
//----画直线-------------------------------------------------------------------
//功能: 在窗口内画一条直线。
//参数: 参数是结构para的成员
//      gkwin，目标窗口指针
//      x1、y1，画线起点坐标
//      x2、y2，画线终点坐标(不包含)
//      color，画线使用的颜色
//      r2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
void GK_Lineto(struct GkscParaLineto *para)
{
    struct Rectangle limit;
    struct ClipRect *clip;
    struct GkWinRsc *fb_gkwin,*lintowin;
    struct DispDraw *my_draw_fun;
    s32 offsetx,offsety;
    if((para->Rop2Code == CN_R2_NOP) || (para->Rop2Code > CN_R2_LAST))
        return;                                         //不执行操作
    if((para->x1==para->x2) && (para->y1==para->y2))    //无须绘制
        return;
    lintowin = para->gkwin;
    my_draw_fun = &lintowin->disp->draw;
    //说明有win buffer，且直接写屏属性为false
    if((lintowin->wm_bitmap != NULL)
        && (lintowin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
    {   //处理方法:在win buffer中绘图，标志changed_msk
        limit.left = 0;
        limit.top = 0;
        limit.right = lintowin->wm_bitmap->width;
        limit.bottom = lintowin->wm_bitmap->height;
        if(!my_draw_fun->LineToBitmap(lintowin->wm_bitmap,&limit,para->x1,
                    para->y1,para->x2,para->y2,para->color,para->Rop2Code))
        {    //硬件加速不支持直线绘制，改用软件实现，软件画线算法中
            //同时处理了changed_msk
            __GK_LinetoBm(lintowin,&limit,para->x1,para->y1,para->x2,
                                    para->y2,para->color,para->Rop2Code);
        }
        else    //硬件加速绘图时，未处理changed_msk，处理之
        {
            __GK_ShadingLine(lintowin,&limit,
                                para->x1,para->y1,para->x2,para->y2);
        }
    }
    else
    {
        clip = lintowin->visible_clip;
        if(clip == NULL)
            return ;
        fb_gkwin = lintowin->disp->frame_buffer;
        offsetx = lintowin->absx0;
        offsety = lintowin->absy0;
        //有frame buffer,且直接写屏属性为false
        if((fb_gkwin != NULL) && (lintowin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
        {   //处理方法:在frame buffer中绘图，但只绘gkwin中的可视区域
            do{
                if(!my_draw_fun->LineToBitmap(
                            fb_gkwin->wm_bitmap,&clip->rect,
                            para->x1+offsetx,para->y1+offsety,para->x2+offsetx,
                            para->y2+offsety,para->color,para->Rop2Code))
                {    //硬件加速不支持直线绘制，改用软件实现，软件画线算法中
                    //同时处理了changed_msk
                    __GK_LinetoBm(fb_gkwin,&clip->rect,
                                    para->x1+offsetx,para->y1+offsety,
                                    para->x2+offsetx,para->y2+offsety,
                                    para->color,para->Rop2Code);
                }
                else    //硬件加速绘图时，未处理changed_msk，处理之
                {
                    __GK_ShadingLine(fb_gkwin,&clip->rect,para->x1+offsetx,
                                      para->y1+offsety,para->x2+offsetx,
                                      para->y2+offsety);
                }
                clip = clip->next;
            }while(clip != lintowin->visible_clip);
        }
        else
        //无win buffer，也无frame buffer，直接画在screen上
        //直接写屏属性为true，不管有无缓冲区，都直接画在screen上
        {
            do{//在screen上直接画直线，不画终点
//              if(!my_draw_fun->LineToScreen(&clip->rect,para->x1+offsetx,
//                           para->y1+offsety,para->x2+offsetx,para->y2+offsety,
//                           para->color,para->Rop2Code))
                {
                    __GK_LinetoScreen(lintowin->disp,&clip->rect,
                            para->x1+offsetx,para->y1+offsety,para->x2+offsetx,
                            para->y2+offsety,para->color,para->Rop2Code);
                }
                clip = clip->next;
            }while(clip != lintowin->visible_clip);
        }
    }
}
//----画直线-------------------------------------------------------------------
//功能: 在窗口内画一条直线,结束点需要绘制。
//参数: gkwin，目标窗口指针
//      gkwin，目标窗口指针
//      x1、y1，画线起点坐标
//      x2、y2，画线终点坐标(包含)
//      color，画线使用的颜色
//      r2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
void GK_LinetoIe(struct GkscParaLineto *para)
{
    s32 dx,dy;
    struct Rectangle limit;
    struct ClipRect *clip;
    struct GkWinRsc *fb_gkwin,*lintoiewin;
    struct DispDraw *my_draw_fun;
    struct DisplayRsc *display;
    s32 offsetx,offsety;
    if((para->Rop2Code == CN_R2_NOP) || (para->Rop2Code > CN_R2_LAST))
        return;                                         //不执行操作
    if((para->x1==para->x2) && (para->y1==para->y2))    //无须绘制
        return;
    lintoiewin = para->gkwin;
    display = lintoiewin->disp;
    my_draw_fun = &display->draw;
    if(para->y1 == para->y2)    //绘制水平线
    {
        //说明有win buffer，且直接写屏属性为false
        if((lintoiewin->wm_bitmap != NULL)
            && (lintoiewin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
        {   //处理方法:在win buffer中绘图，标志changed_msk
            limit.left = 0;
            limit.top = 0;
            limit.right = lintoiewin->wm_bitmap->width;
            limit.bottom = lintoiewin->wm_bitmap->height;
            if(!my_draw_fun->LineToBitmapIe(lintoiewin->wm_bitmap,&limit,para->x1,
                        para->y1,para->x2,para->y2,para->color,para->Rop2Code))
            {    //硬件加速不支持直线绘制，改用软件实现，软件画线算法中
                //同时处理了changed_msk
                __GK_HlinetoBm(lintoiewin,&limit,para->x1,para->y1,
                                para->x2+1,para->color,para->Rop2Code);
            }
            else
            {   //硬件加速支持直线绘制，未处理changed_msk，处理之
                __GK_ShadingLine(lintoiewin,&limit,
                                para->x1,para->y1,para->x2+1,para->y2);
            }

        }
        else
        {
            clip = lintoiewin->visible_clip;
            if(clip == NULL)
                return ;
            fb_gkwin = display->frame_buffer;
            offsetx = lintoiewin->absx0;
            offsety = lintoiewin->absy0;
            //有frame buffer，且直接写屏属性为false
            if((fb_gkwin != NULL) && (lintoiewin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
            {   //处理方法:在frame buffer中绘图，但只绘gkwin中的可视区域
                do
                {
                    if(!my_draw_fun->LineToBitmapIe(
                                fb_gkwin->wm_bitmap,&clip->rect,
                                para->x1+offsetx,para->y1+offsety,
                                para->x2+offsetx,para->y2+offsety,
                                para->color,para->Rop2Code))
                    {    //硬件加速不支持直线绘制，改用软件实现，软件画线算法中
                        //同时处理了changed_msk
                        __GK_HlinetoBm(fb_gkwin,&clip->rect,
                                    para->x1+offsetx,para->y1+offsety,
                                    para->x2+offsetx+1,
                                    para->color,para->Rop2Code);
                    }
                    else
                    {   //硬件加速绘图时，未处理changed_msk，处理之
                        __GK_ShadingLine(fb_gkwin,&clip->rect,
                                        para->x1+offsetx,para->y1+offsety,
                                        para->x2+offsetx+1,para->y2+offsety);
                    }
                    clip = clip->next;
                }while(clip != lintoiewin->visible_clip);
            }
            else
            //无win buffer，也无frame buffer，直接画在screen上
            //直接写屏属性为true，不管有无缓冲区，都直接画在screen上
            {
                do{//在screen上直接画直线，画终点
//                  if(!my_draw_fun->LineToScreenIe(&clip->rect,
//                                para->x1+offsetx,para->y1+offsety,
//                                para->x2+offsetx,para->y2+offsety,
//                               para->color,para->Rop2Code))
                    {
                        __GK_LinetoScreen(display,&clip->rect,
                                        para->x1+offsetx,para->y1+offsety,
                                        para->x2+offsetx+1,para->y2+offsety,
                                        para->color,para->Rop2Code);
                    }
                    clip = clip->next;
                }while(clip != lintoiewin->visible_clip);
            }
        }
    }
    else if(para->x1 == para->x2)    //绘制垂直线
    {
        //说明有win buffer，且直接写屏属性为false
        if((lintoiewin->wm_bitmap != NULL)
            && (lintoiewin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
        {   //处理方法:在win buffer中绘图，标志changed_msk
            limit.left = 0;
            limit.top = 0;
            limit.right = lintoiewin->wm_bitmap->width;
            limit.bottom = lintoiewin->wm_bitmap->height;
            if(!my_draw_fun->LineToBitmapIe(lintoiewin->wm_bitmap,&limit,para->x1,
                        para->y1,para->x2,para->y2,para->color,para->Rop2Code))
            {    //硬件加速不支持直线绘制，改用软件实现，软件画线算法中
                //同时处理了changed_msk
                __GK_VlinetoBm(lintoiewin,&limit,para->x1,
                            para->y1,para->y2+1,para->color,para->Rop2Code);
            }
            else
            {   //硬件加速支持直线绘制，未处理changed_msk，处理之
                __GK_ShadingLine(lintoiewin,&limit,
                                para->x1,para->y1,para->x2,para->y2+1);
            }
        }
        else
        {
            clip = lintoiewin->visible_clip;
            if(clip == NULL)
                return ;
            fb_gkwin = display->frame_buffer;
            offsetx = lintoiewin->absx0;
            offsety = lintoiewin->absy0;
            //有frame buffer，且直接写屏属性为false
            if((fb_gkwin != NULL) && (lintoiewin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
            {   //处理方法:在frame buffer中绘图，但只绘gkwin中的可视区域
                do
                {
                    if(!my_draw_fun->LineToBitmapIe(
                                fb_gkwin->wm_bitmap,&clip->rect,
                                para->x1+offsetx,para->y1+offsety,
                                para->x2+offsetx,para->y2+offsety,
                                para->color,para->Rop2Code))
                    {    //硬件加速不支持直线绘制，改用软件实现，软件画线算法中
                        //同时处理了changed_msk
                        __GK_VlinetoBm(fb_gkwin,&clip->rect,
                                    para->x1+offsetx,para->y1+offsety,
                                    para->y2+offsety+1,
                                    para->color,para->Rop2Code);
                    }
                    else
                    {   //硬件加速绘图时，未处理changed_msk，处理之
                        __GK_ShadingLine(fb_gkwin,&clip->rect,
                                        para->x1+offsetx,para->y1+offsety,
                                        para->x2+offsetx,para->y2+offsety+1);
                    }
                    clip = clip->next;
                }while(clip != lintoiewin->visible_clip);
            }
            else
            //无win buffer，也无frame buffer，直接画在screen上
            //直接写屏属性为true，不管有无缓冲区，都直接画在screen上
            {
                do{//在screen上直接画直线，画终点
//                  if(!my_draw_fun->LineToScreenIe(&clip->rect,
//                                para->x1+offsetx,para->y1+offsety,
//                                para->x2+offsetx,para->y2+offsety,
//                               para->color,para->Rop2Code))
                    {
                        __GK_LinetoScreen(display,&clip->rect,
                                        para->x1+offsetx,para->y1+offsety,
                                        para->x2+offsetx,para->y2+offsety+1,
                                        para->color,para->Rop2Code);
                    }
                    clip = clip->next;
                }while(clip != lintoiewin->visible_clip);
            }
        }
    }
    else    //绘制斜线
    {
        dx = abs(para->x2-para->x1);
        dy = abs(para->y2-para->y1);
        //说明有win buffer，且直接写屏属性为false
        if((lintoiewin->wm_bitmap != NULL)
            && (lintoiewin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
        {   //处理方法:在win buffer中绘图，标志changed_msk
            limit.left = 0;
            limit.top = 0;
            limit.right = lintoiewin->wm_bitmap->width;
            limit.bottom = lintoiewin->wm_bitmap->height;
            if(!my_draw_fun->LineToBitmapIe(lintoiewin->wm_bitmap,&limit,para->x1,
                        para->y1,para->x2,para->y2,para->color,para->Rop2Code))
            {    //硬件加速不支持直线绘制，改用软件实现，软件画线算法中
                //同时处理了changed_msk
                if(dy > dx)
                {
                    __GK_OlinetoBm(lintoiewin,&limit,para->x1,para->y1,
                                    para->x2,para->y2+1,
                                    para->color,para->Rop2Code);
                }
                else
                {
                    __GK_OlinetoBm(lintoiewin,&limit,para->x1,para->y1,
                                    para->x2+1,para->y2,
                                    para->color,para->Rop2Code);
                }
            }
            else
            {   //硬件加速支持直线绘制，未处理changed_msk，处理之
                __GK_ShadingLine(lintoiewin,&limit,
                                para->x1,para->y1,para->x2,para->y2);
            }

        }
        else
        {
            clip = lintoiewin->visible_clip;
            if(clip == NULL)
                return ;
            fb_gkwin = display->frame_buffer;
            offsetx = lintoiewin->absx0;
            offsety = lintoiewin->absy0;
            //有frame buffer，且直接写屏属性为false
            if((fb_gkwin != NULL) && (lintoiewin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
            {   //处理方法:在frame buffer中绘图，但只绘gkwin中的可视区域
                do
                {
                    if(!my_draw_fun->LineToBitmapIe(
                                fb_gkwin->wm_bitmap,&clip->rect,
                                para->x1+offsetx,para->y1+offsety,
                                para->x2+offsetx,para->y2+offsety,
                                para->color,para->Rop2Code))
                    {    //硬件加速不支持直线绘制，改用软件实现，软件画线算法中
                        //同时处理了changed_msk
                            __GK_OlinetoBm(fb_gkwin,&clip->rect,
                                            para->x1+offsetx,para->y1+offsety,
                                            para->x2+offsetx,para->y2+offsety,
                                            para->color,para->Rop2Code);
                    }
                    else
                    {   //硬件加速绘图时，未处理changed_msk，处理之
                            __GK_ShadingLine(fb_gkwin,&clip->rect,
                                            para->x1+offsetx,para->y1+offsety,
                                            para->x2+offsetx,para->y2+offsety);
                    }
                    clip = clip->next;
                }while(clip != lintoiewin->visible_clip);
            }
            else
            //无win buffer，也无frame buffer，直接画在screen上
            //直接写屏属性为true，不管有无缓冲区，都直接画在screen上
            {
                do{//在screen上直接画直线，画终点
//                  if(!my_draw_fun->LineToScreenIe(&clip->rect,para->x1+offsetx,
//                               para->y1+offsety,para->x2+offsetx,para->y2+offsety,
//                               para->color,para->Rop2Code))
                    {
                            __GK_LinetoScreen(display,&clip->rect,
                                            para->x1+offsetx,para->y1+offsety,
                                            para->x2+offsetx,para->y2+offsety,
                                            para->color,para->Rop2Code);
                    }
                    clip = clip->next;
                }while(clip != lintoiewin->visible_clip);
            }
        }
    }
}

//----显示文本-----------------------------------------------------------------
//功能: 用默认字体和默认字符集显示一个文本串。
//参数: gkwin，显示的目标窗口
//      x、y，显示位置，相对于gkwin
//      text，待显示的字符串
//      count，要显示的字符数，-1表示要显示整串
//返回: 无
//-----------------------------------------------------------------------------
void GK_DrawText(struct GkscParaDrawText *para,const char *text)
{
    struct GkscParaDrawBitmapRop bitmap_para;
    struct FontRsc* cur_font;
    struct Charset* cur_enc;
    s32 len, char_num = 0,size,size_bak;
    u32 wc;
    u8 buf[128],*dbuf;  //定义一个足够存32点阵汉字的缓冲区，如果需要显示的字符
                        //超过此点阵，就需要动态分配

    if(para->Rop2Code == CN_R2_NOP)
        return;

    cur_font = para->pFont;
    if(cur_font == NULL)
        cur_font = Font_GetCurFont();
    if(cur_font == NULL)
        return;

    cur_enc = para->pCharset;
    if(cur_enc == NULL)
        cur_enc = Charset_NlsGetCurCharset();
    if(cur_enc == NULL)
        return;

//    bitmap_para.command =CN_GKSC_DRAW_BITMAP;
    bitmap_para.gkwin = para->gkwin;
    bitmap_para.HyalineColor = 0;
    bitmap_para.x = para->x;
    bitmap_para.y = para->y;
    bitmap_para.RopCode = (struct RopGroup){ 0, 0, 0, CN_R2_COPYPEN, 0, 0, 0  };
    bitmap_para.RopCode.HyalineEn = 1;
    bitmap_para.RopCode.Rop2Mode = para->Rop2Code;
    bitmap_para.bitmap.ExColor = para->color;
    dbuf = NULL;
    size_bak = 0;
    for(; ;)
    {
        if((para->count != -1)  && (char_num >= para->count))
            break;
        len= cur_enc->MbToUcs4(&wc, text, -1);
        if(len == -1)
        { // 无效字符
            text++;
        }
        else
        { // 有效字符
            if(len == 0)
                break;
            text += len;
            char_num++;
            if(wc==L'\r')
            {
                bitmap_para.x =  para->x;
                continue;
            }

            if(wc==L'\n')
            {
                bitmap_para.y += cur_font->MaxHeight;
                continue;
            }

            // 取字形
            bitmap_para.bitmap.bm_bits = NULL;      //NULL=查询bitmap.
            cur_font->GetBitmap(wc,0,0,&(bitmap_para.bitmap));
            size = bitmap_para.bitmap.linebytes * bitmap_para.bitmap.height;
            if(size < 128)
                bitmap_para.bitmap.bm_bits = buf;
            else
            {
                if(size > size_bak)
                {
                    free(dbuf);
                    dbuf = M_MallocLc(size,0);
                    if(dbuf == NULL)
                        return;
                    size_bak = size;
                }
                bitmap_para.bitmap.bm_bits = dbuf;
            }
            cur_font->GetBitmap(wc,0,0,&(bitmap_para.bitmap));
            GK_DrawBitMap(&bitmap_para);

            bitmap_para.x += bitmap_para.bitmap.width;
        }
    }
    free(dbuf);
}
//----绘制光栅位图-------------------------------------------------------------
//功能: 在窗口内画光栅位图，位图超出窗口的部分将不予绘制。逐像素一一对应copy，
//      不做拉伸和压缩
//参数: 参数是结构para的成员
//      gkwin，目标窗口指针
//      bitmap，待绘制的位图
//      pattern，要设置的图案位图，像素格式与 bitmap 一致
//      bitmsk，rop4操作才需要的掩码位图，一个单色位图
//      transparentcolor，要设置的透明色
//      x、y，绘制的目标位置
//      rop_code，光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
void GK_DrawBitMap(struct GkscParaDrawBitmapRop *para)
{
//    u32 HyalineColor;
    s32 x_src,y_src,x_dst,y_dst;
    struct PointCdn InsOffset;
    struct Rectangle DstRect,SrcRect,InsRect;
    struct ClipRect *clip;
    struct GkWinRsc *fb_gkwin,*DstGkwin;
    struct RectBitmap *DstBitmap;
    struct RectBitmap *SrcBitmap;
    struct DisplayRsc *display;
    DstGkwin = para->gkwin;
    display = DstGkwin->disp;

//    HyalineColor = para->HyalineColor;
    DstBitmap = DstGkwin->wm_bitmap;
    SrcBitmap = &para->bitmap;
    //待绘制的位图要绘制的位置相对于目标窗口的坐标
    SrcRect.left = 0;
    SrcRect.top = 0;
    SrcRect.right = SrcBitmap->width;
    SrcRect.bottom = SrcBitmap->height;
    DstRect.left = para->x;
    DstRect.top = para->y;
    DstRect.right = para->x+SrcRect.right;
    DstRect.bottom = para->y+SrcRect.bottom;
    //调整待绘制位图要绘制的位置，限制在窗口内
    if(        (DstRect.right <= 0)
            || (DstRect.left >= DstGkwin->limit_right)
            || (DstRect.bottom <= 0)
            || (DstRect.top >= DstGkwin->limit_bottom))
        return;                             //绘图区域在窗口外面
    if(DstRect.left < DstGkwin->limit_left)         //调整左边界在窗口内部
    {
        SrcRect.left = DstGkwin->limit_left - DstRect.left;
        DstRect.left = DstGkwin->limit_left;
    }
    if(DstRect.top < DstGkwin->limit_top)           //调整上边界在窗口内部
    {
        SrcRect.top = DstGkwin->limit_top - DstRect.top;
        DstRect.top = DstGkwin->limit_top;
    }
    if(DstRect.right > DstGkwin->limit_right)       //调整右边界在窗口内部
    {
        SrcRect.right = DstRect.right - DstGkwin->limit_right;
        DstRect.right = DstGkwin->limit_right;
    }
    if(DstRect.bottom > DstGkwin->limit_bottom )      //调整下边界在窗口内部
    {
        SrcRect.bottom = DstRect.bottom - DstGkwin->limit_bottom;
        DstRect.bottom = DstGkwin->limit_bottom;
    }

    //说明有win buffer，且直接写屏属性为false
    if((DstBitmap != NULL) && (DstGkwin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
    {   //处理方法:在win buffer中绘图，标志changed_msk
        if(!display->draw.BltBitmapToBitmap(DstBitmap,&DstRect,
                                       SrcBitmap,&SrcRect,
                                       para->RopCode,para->HyalineColor))
        {
            //驱动部支持 BltBitmapToBitmap 绘制，改由代码实现
            y_dst= DstRect.top;
            for(y_src= 0;
                y_src < DstRect.bottom-DstRect.top;
                y_src++)
            {
                x_dst = DstRect.left;
                for(x_src = 0;
                    x_src < DstRect.right-DstRect.left;
                    x_src++)
                {
                    __GK_CopyPixelRopBm(DstBitmap,SrcBitmap,
                                            x_dst,y_dst,SrcRect.left+x_src,
                                            SrcRect.top+y_src,
                                            para->RopCode,para->HyalineColor);
                    x_dst++;
                }
                y_dst++;
            }
        }
        __GK_ShadingRect(DstGkwin,&DstRect);    //设置绘制部分的changed_msk
    }
    else        //无win buffer，或直接写屏属性为true
    {
        clip = DstGkwin->visible_clip;
        if(clip == NULL)                            //窗口可视域为空，直接返回
            return ;
        //把clip对应的SrcRect位置绘制到framebuffer上，由于clip肯定在framebuffer
        //以内，故只需要计算坐标，无须判断是否超越framebuffer边界。
        fb_gkwin = display->frame_buffer;
        DstBitmap = fb_gkwin->wm_bitmap;
        //有frame buffer，且直接写屏属性为false，把可视域绘制到framebuffer上
        if((fb_gkwin != NULL) && (DstGkwin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
        {   //处理方法:在frame_buffer中绘图，但只绘gkwin中的可视区域
            //为与可视域坐标保持一致，将待位图要绘制的位置的
            //坐标调整成以screen原点为原点的坐标，注意调整后的坐标，可能超出
            //framebuffer的尺寸界限，但没有关系的，取剪切域交集的时候会处理掉。
            DstRect.left += DstGkwin->absx0;
            DstRect.right += DstGkwin->absx0;
            DstRect.top += DstGkwin->absy0;
            DstRect.bottom += DstGkwin->absy0;
            //SrcRect用于保存待绘制的bitmap相对framebuffer的偏移，用于计算clip
            //在framebuffer中对应的位置
            InsOffset.x = DstGkwin->absx0 + para->x;
            InsOffset.y = DstGkwin->absy0 + para->y;
            do{
                if(__GK_GetRectInts(&clip->rect,&DstRect,&InsRect))
                {
                    SrcRect.left = InsRect.left-InsOffset.x;
                    SrcRect.top = InsRect.top-InsOffset.y;
                    SrcRect.right = InsRect.left-InsOffset.x;
                    SrcRect.bottom = InsRect.top-InsOffset.y;
                    if(!fb_gkwin->disp->draw.BltBitmapToBitmap(
                                        DstBitmap,&InsRect,
                                        SrcBitmap,&SrcRect,
                                        para->RopCode,para->HyalineColor))
                    {
                        SrcBitmap = &para->bitmap;

                        y_dst= InsRect.top;
                        //给定的光栅操作码为扩展的光栅操作码
                        if(        (para->RopCode.AlphaEn == 0)
                                && (para->RopCode.HyalineEn == 0)
                                && (para->RopCode.Rop2Mode == CN_R2_COPYPEN ) )
                        {
                            for(y_src= InsRect.top-DstRect.top;
                                y_src < InsRect.bottom-DstRect.top;
                                y_src++)
                            {
                                x_dst = InsRect.left;
                                for(x_src = InsRect.left-DstRect.left;
                                    x_src < InsRect.right-DstRect.left;
                                    x_src++)
                                {
                                    __GK_CopyPixelBm(DstBitmap,SrcBitmap,
                                                     x_dst,y_dst,x_src, y_src);
                                    x_dst++;
                                }
                                y_dst++;
                            }
                        }
                        else
                        {
                            for(y_src= InsRect.top-DstRect.top;
                                y_src < InsRect.bottom-DstRect.top;
                                y_src++)
                            {
                                x_dst = InsRect.left;
                                for(x_src = InsRect.left-DstRect.left;
                                    x_src < InsRect.right-DstRect.left;
                                    x_src++)
                                {
                                    __GK_CopyPixelRopBm(DstBitmap,SrcBitmap,
                                                        x_dst,y_dst,x_src,
                                                        y_src,para->RopCode,
                                                        para->HyalineColor);
                                    x_dst++;
                                }
                                y_dst++;
                            }
                        }
                    }
                    //设置绘制部分的changed_msk
                    __GK_ShadingRect(fb_gkwin,&InsRect);
                }
                clip = clip->next;
            }while(clip != DstGkwin->visible_clip);
        }
        else
        //带rop操作的位图不支持直接写屏，无win buffer，也无frame buffer，
        //或者直接写屏属性为true的情况，将退化为GK_DrawBitMap的功能
        {
            //为与可视域坐标保持一致，将待位图要绘制的位置的
            //坐标调整成以screen原点为原点的坐标
            DstRect.left += DstGkwin->absx0;
            DstRect.right += DstGkwin->absx0;
            DstRect.top += DstGkwin->absy0;
            DstRect.bottom += DstGkwin->absy0;
            do{
                //要绘制的位图目标位置在窗口可视域内才执行绘制程序
                if(__GK_GetRectInts(&clip->rect,&DstRect,&InsRect))
                {
                    //传送bitmap到screen
                    __GK_BltBmToScreen(display,&InsRect,
                                        &para->bitmap,
                                        InsRect.left-DstRect.left,
                                        InsRect.top-DstRect.top);
                }
                clip = clip->next;
            }while(clip != DstGkwin->visible_clip);
        }
    }
}

//----渐变色填充位图---------------------------------------------------------
//功能: 用渐变色填充显示器中的一个矩形区域
//参数: Draw,目标显示器的绘制函数集指针。
//      Target: 渐变过程覆盖的区域。
//      Focus，待填充的剪切域，须包含在Target内部
//      Color0,颜色1，含义见CN_FILLRECT_MODE_LR族常量定义
//      Color1,颜色2，含义见CN_FILLRECT_MODE_LR族常量定义
//      Mode,渐变模式
//返回: 无
//注: 斜线渐变使用的是近似模式，严格模式应该是算出所在像素与顶点的距离，但计算
//    距离就涉及到开方运算。人眼对颜色变化是不敏感的，近似计算也没啥副作用。
//-----------------------------------------------------------------------------
void __GK_GradientFillRect(     struct RectBitmap *bitmap,
                                struct Rectangle *Target,
                                struct Rectangle *Focus,
                                u32 Color0,u32 Color1,u32 Mode)
{
    u32 a0,r0,g0,b0,a1,r1,g1,b1,Color;
    u32 ax,rx,gx,bx,ay,ry,gy,by;
    s32 L0,L1,x,y,CrossX,CrossY;
    a0 = Color0>>24;
    r0 = (Color0>>16)&0xff;
    g0 = (Color0>>8)&0xff;
    b0 = Color0&0xff;
    a1 = Color1>>24;
    r1 = (Color1>>16)&0xff;
    g1 = (Color1>>8)&0xff;
    b1 = Color1&0xff;
    CrossY = Target->bottom - Target->top;
    CrossX = Target->right - Target->left;
    switch(Mode)
    {
        case CN_FILLRECT_MODE_LR:
        {
            for(x = Focus->left; x<Focus->right; x++)
            {
                L0 = x-Target->left;
                L1 = Target->right - x;
                ax = ( ( (L0*a1 + L1*a0)<<8) / CrossX>>8) & 0xff;
                rx = ( ( (L0*r1 + L1*r0)<<8) / CrossX>>8) & 0xff;
                gx = ( ( (L0*g1 + L1*g0)<<8) / CrossX>>8) & 0xff;
                bx = ( ( (L0*b1 + L1*b0)<<8) / CrossX>>8) & 0xff;
                Color = ( ax << 24) +( rx << 16) +( gx << 8) +( bx << 0);
                Color = GK_ConvertRGB24ToPF(bitmap->PixelFormat,Color);
                for(y = Focus->top; y<Focus->bottom; y++)
                {
                    __GK_SetPixelRop2Bm(bitmap,x,y,Color,CN_R2_COPYPEN);
                }
            }
        } break;
        case CN_FILLRECT_MODE_UD:
        {
            for(y = Focus->top; y<Focus->bottom; y++)
            {
                L0 = y-Target->top;
                L1 = Target->bottom- y;
                ay = ( ( (L0*a1 + L1*a0)<<8) / CrossY>>8) & 0xff;
                ry = ( ( (L0*r1 + L1*r0)<<8) / CrossY>>8) & 0xff;
                gy = ( ( (L0*g1 + L1*g0)<<8) / CrossY>>8) & 0xff;
                by = ( ( (L0*b1 + L1*b0)<<8) / CrossY>>8) & 0xff;
                Color = ( ay << 24) +( ry << 16) +( gy << 8) +( by << 0);
                Color = GK_ConvertRGB24ToPF(bitmap->PixelFormat,Color);
                for(x = Focus->left; x<Focus->right; x++)
                {
                    __GK_SetPixelRop2Bm(bitmap,x,y,Color,CN_R2_COPYPEN);
                }
            }

        } break;
        case CN_FILLRECT_MODE_LU2RD:
        {
            for(x = Focus->left; x<Focus->right; x++)
            {
                L0 = x-Target->left;
                L1 = Target->right - x;
                ax = ( ( (L0*a1 + L1*a0)<<8) / CrossX>>8) & 0xff;
                rx = ( ( (L0*r1 + L1*r0)<<8) / CrossX>>8) & 0xff;
                gx = ( ( (L0*g1 + L1*g0)<<8) / CrossX>>8) & 0xff;
                bx = ( ( (L0*b1 + L1*b0)<<8) / CrossX>>8) & 0xff;
                for(y = Focus->top; y<Focus->bottom; y++)
                {
                    L0 = y-Target->top;
                    L1 = Target->bottom- y;
                    ay = ( ( (L0*a1 + L1*a0)<<8) / CrossY>>8) & 0xff;
                    ry = ( ( (L0*r1 + L1*r0)<<8) / CrossY>>8) & 0xff;
                    gy = ( ( (L0*g1 + L1*g0)<<8) / CrossY>>8) & 0xff;
                    by = ( ( (L0*b1 + L1*b0)<<8) / CrossY>>8) & 0xff;
                    Color = ( ((ax+ay)>>1) << 24)
                            +( ((rx+ry)>>1) << 16)
                            +( ((gx+gy)>>1) << 8)
                            +( ((bx+by)>>1) << 0);
                    Color = GK_ConvertRGB24ToPF(bitmap->PixelFormat,Color);
                    __GK_SetPixelRop2Bm(bitmap,x,y,Color,CN_R2_COPYPEN);
                }
            }
        } break;
        case CN_FILLRECT_MODE_RU2LD:
        {
            for(x = Focus->left; x<Focus->right; x++)
            {
                L1 = x-Target->left;
                L0 = Target->right - x;
                ax = ( ( (L0*a1 + L1*a0)<<8) / CrossX>>8) & 0xff;
                rx = ( ( (L0*r1 + L1*r0)<<8) / CrossX>>8) & 0xff;
                gx = ( ( (L0*g1 + L1*g0)<<8) / CrossX>>8) & 0xff;
                bx = ( ( (L0*b1 + L1*b0)<<8) / CrossX>>8) & 0xff;
                for(y = Focus->top; y<Focus->bottom; y++)
                {
                    L0 = y-Target->top;
                    L1 = Target->bottom- y;
                    ay = ( ( (L0*a1 + L1*a0)<<8) / CrossY>>8) & 0xff;
                    ry = ( ( (L0*r1 + L1*r0)<<8) / CrossY>>8) & 0xff;
                    gy = ( ( (L0*g1 + L1*g0)<<8) / CrossY>>8) & 0xff;
                    by = ( ( (L0*b1 + L1*b0)<<8) / CrossY>>8) & 0xff;
                    Color = ( ((ax+ay)>>1) << 24)
                            +( ((rx+ry)>>1) << 16)
                            +( ((gx+gy)>>1) << 8)
                            +( ((bx+by)>>1) << 0);
                    Color = GK_ConvertRGB24ToPF(bitmap->PixelFormat,Color);
                    __GK_SetPixelRop2Bm(bitmap,x,y,Color,CN_R2_COPYPEN);
                }
            }

        } break;
    }
}


//----渐变色填充显示器---------------------------------------------------------
//功能: 用渐变色填充显示器中的一个矩形区域
//参数: Draw,目标显示器的绘制函数集指针。
//      Target: 渐变过程覆盖的区域。
//      Focus，待填充的剪切域，须包含在Target内部
//      Color0,颜色1，含义见CN_FILLRECT_MODE_LR族常量定义
//      Color1,颜色2，含义见CN_FILLRECT_MODE_LR族常量定义
//      Mode,渐变模式
void __GK_GradientFillScreenRect(struct DispDraw *Draw,
                                 struct Rectangle *Target,
                                 struct Rectangle *Focus,
                                 u32 Color0,u32 Color1,u32 Mode)
{
    u32 a0,r0,g0,b0,a1,r1,g1,b1,Color;
    u32 ax,rx,gx,bx,ay,ry,gy,by;
    s32 L0,L1,x,y,CrossX,CrossY;
    a0 = Color0>>24;
    r0 = (Color0>>16)&0xff;
    g0 = (Color0>>8)&0xff;
    b0 = Color0&0xff;
    a1 = Color1>>24;
    r1 = (Color1>>16)&0xff;
    g1 = (Color1>>8)&0xff;
    b1 = Color1&0xff;
    CrossY = Target->bottom - Target->top;
    CrossX = Target->right - Target->left;
    switch(Mode)
    {
        case CN_FILLRECT_MODE_N:
        {
            for(x = Focus->left; x<Focus->right; x++)
            {
                for(y = Focus->top; y<Focus->bottom; y++)
                {
                    Draw->SetPixelToScreen(x,y,Color0,CN_R2_COPYPEN);
                }
            }
        } break;
        case CN_FILLRECT_MODE_LR:
        {
            for(x = Focus->left; x<Focus->right; x++)
            {
                L0 = x-Target->left;
                L1 = Target->right - x;
                ax = ( ( (L0*a1 + L1*a0)<<8) / CrossX>>8) & 0xff;
                rx = ( ( (L0*r1 + L1*r0)<<8) / CrossX>>8) & 0xff;
                gx = ( ( (L0*g1 + L1*g0)<<8) / CrossX>>8) & 0xff;
                bx = ( ( (L0*b1 + L1*b0)<<8) / CrossX>>8) & 0xff;
                Color = ( ax << 24) +( rx << 16) +( gx << 8) +( bx << 0);
                for(y = Focus->top; y<Focus->bottom; y++)
                {
                    Draw->SetPixelToScreen(x,y,Color,CN_R2_COPYPEN);
                }
            }
        } break;
        case CN_FILLRECT_MODE_UD:
        {
            for(y = Focus->top; y<Focus->bottom; y++)
            {
                L0 = y-Target->top;
                L1 = Target->bottom- y;
                ay = ( ( (L0*a1 + L1*a0)<<8) / CrossY>>8) & 0xff;
                ry = ( ( (L0*r1 + L1*r0)<<8) / CrossY>>8) & 0xff;
                gy = ( ( (L0*g1 + L1*g0)<<8) / CrossY>>8) & 0xff;
                by = ( ( (L0*b1 + L1*b0)<<8) / CrossY>>8) & 0xff;
                Color = ( ay << 24) +( ry << 16) +( gy << 8) +( by << 0);
                for(x = Focus->left; x<Focus->right; x++)
                {
                    Draw->SetPixelToScreen(x,y,Color,CN_R2_COPYPEN);
                }
            }

        } break;
        case CN_FILLRECT_MODE_LU2RD:
        {
            for(x = Focus->left; x<Focus->right; x++)
            {
                L0 = x-Target->left;
                L1 = Target->right - x;
                ax = ( ( (L0*a1 + L1*a0)<<8) / CrossX>>8) & 0xff;
                rx = ( ( (L0*r1 + L1*r0)<<8) / CrossX>>8) & 0xff;
                gx = ( ( (L0*g1 + L1*g0)<<8) / CrossX>>8) & 0xff;
                bx = ( ( (L0*b1 + L1*b0)<<8) / CrossX>>8) & 0xff;
                for(y = Focus->top; y<Focus->bottom; y++)
                {
                    L0 = y-Target->top;
                    L1 = Target->bottom- y;
                    ay = ( ( (L0*a1 + L1*a0)<<8) / CrossY>>8) & 0xff;
                    ry = ( ( (L0*r1 + L1*r0)<<8) / CrossY>>8) & 0xff;
                    gy = ( ( (L0*g1 + L1*g0)<<8) / CrossY>>8) & 0xff;
                    by = ( ( (L0*b1 + L1*b0)<<8) / CrossY>>8) & 0xff;
                    Color = ( ((ax+ay)>>1) << 24)
                            +( ((rx+ry)>>1) << 16)
                            +( ((gx+gy)>>1) << 8)
                            +( ((bx+by)>>1) << 0);
                    Draw->SetPixelToScreen(x,y,Color,CN_R2_COPYPEN);
                }
            }
        } break;
        case CN_FILLRECT_MODE_RU2LD:
        {
            for(x = Focus->left; x<Focus->right; x++)
            {
                L1 = x-Target->left;
                L0 = Target->right - x;
                ax = ( ( (L0*a1 + L1*a0)<<8) / CrossX>>8) & 0xff;
                rx = ( ( (L0*r1 + L1*r0)<<8) / CrossX>>8) & 0xff;
                gx = ( ( (L0*g1 + L1*g0)<<8) / CrossX>>8) & 0xff;
                bx = ( ( (L0*b1 + L1*b0)<<8) / CrossX>>8) & 0xff;
                for(y = Focus->top; y<Focus->bottom; y++)
                {
                    L0 = y-Target->top;
                    L1 = Target->bottom- y;
                    ay = ( ( (L0*a1 + L1*a0)<<8) / CrossY>>8) & 0xff;
                    ry = ( ( (L0*r1 + L1*r0)<<8) / CrossY>>8) & 0xff;
                    gy = ( ( (L0*g1 + L1*g0)<<8) / CrossY>>8) & 0xff;
                    by = ( ( (L0*b1 + L1*b0)<<8) / CrossY>>8) & 0xff;
                    Color = ( ((ax+ay)>>1) << 24)
                            +( ((rx+ry)>>1) << 16)
                            +( ((gx+gy)>>1) << 8)
                            +( ((bx+by)>>1) << 0);
                    Draw->SetPixelToScreen(x,y,Color,CN_R2_COPYPEN);
                }
            }

        } break;
    }
}


//----填充窗口的一部分---------------------------------------------------------
//功能: 用指定的颜色填充窗口的一部分。
//参数: 参数是结构para的成员
//      gkwin，目标窗口
//      rect，待填充的矩形
//      color，填充使用的颜色
//返回: 无
//特注: 本函数可能会修改Rect的成员，绝对不允许外部调用。
//-----------------------------------------------------------------------------
void __GK_FillPartWin(struct GkWinRsc *Gkwin,struct Rectangle *Rect,u32 Color)
{
    struct RectBitmap *bitmap;
    struct Rectangle ins_rect;
    struct DispDraw *my_draw_fun;
    struct ClipRect *clip;
    struct GkWinRsc *fb_gkwin;

    bitmap = Gkwin->wm_bitmap;
    //待填充矩形要绘制的位置的坐标，相对于目标窗口

    my_draw_fun = &Gkwin->disp->draw;
    //说明有win buffer，且直接写屏属性为false
    if((Gkwin->wm_bitmap != NULL)
        && (Gkwin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
    {
        //处理方法:在win buffer中绘图，着色changed_msk
        if(!my_draw_fun->FillRectToBitmap(bitmap,Rect,Rect,Color,
                                        0,CN_FILLRECT_MODE_N))
        {
            //硬件加速不支持填充位图，则用软件实现
            __GK_FillRect(bitmap,Rect,Color);
        }
        __GK_ShadingRect(Gkwin,Rect);//着色填充区域的changed_msk
    }else
    {
        clip = Gkwin->visible_clip;
        if(clip == NULL)                //窗口可视域为空，直接返回
            return ;
        fb_gkwin = Gkwin->disp->frame_buffer;
        //接下来的绘制，是在帧缓冲或者screen上绘制，使用绝对坐标，变换之
        Rect->left += Gkwin->absx0;
        Rect->right += Gkwin->absx0;
        Rect->top += Gkwin->absy0;
        Rect->bottom += Gkwin->absy0;
        //有frame buffer,且直接写屏属性为false
        if((fb_gkwin != NULL) && (Gkwin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
        {   //处理方法:在frame buffer中绘图，但只绘gkwin中的可视区域
            do{
                //只填充窗口可视域与要绘制的区域的交集
                if(__GK_GetRectInts(&clip->rect,Rect,&ins_rect))
                {
                    //帧缓冲的坐标即绝对坐标，故无须坐标变换
                    if(!my_draw_fun->FillRectToBitmap(fb_gkwin->wm_bitmap,
                                                  Rect,&ins_rect,Color,
                                                  0,CN_FILLRECT_MODE_N))
                    {
                        //硬件加速不支持填充位图，则用软件实现
                        __GK_FillRect(fb_gkwin->wm_bitmap,
                                            &ins_rect,Color);
                    }
                    //标志填充区域的changed_msk
                    __GK_ShadingRect(fb_gkwin,&ins_rect);
                }
                clip = clip->next;
            }while(clip != Gkwin->visible_clip);
        }
        else
        //无win buffer，也无frame buffer，直接画在screen上
        //直接写屏属性为true，不管有无缓冲区，都直接画在screen上
        {
            do{
                struct DisplayRsc *mirror,*current;
                //只填充窗口可视域与要绘制的区域的交集
                if(__GK_GetRectInts(&clip->rect,Rect,&ins_rect))
                {
                    //硬件加速不支持填充位图，则用软件实现
                    if(!my_draw_fun->FillRectToScreen(Rect,&ins_rect,
                                             Color,0,CN_FILLRECT_MODE_N))
                    {
                        __GK_GradientFillScreenRect(my_draw_fun,Rect,
                                    &ins_rect,Color,0,CN_FILLRECT_MODE_N);
                    }
                    mirror = (struct DisplayRsc *)&Gkwin->disp->node;
                    if(mirror == NULL)      //没有镜像显示器，直接返回
                        return;
                    current = mirror;
                    while(current != NULL)
                    {
                        //硬件加速不支持填充位图，则用软件实现
                        if(!current->draw.FillRectToScreen(Rect,&ins_rect,
                                              Color,0,CN_FILLRECT_MODE_N))
                        {
                            __GK_GradientFillScreenRect(&current->draw,Rect,
                                    &ins_rect,Color,0,CN_FILLRECT_MODE_N);
                        }
                        current = (struct DisplayRsc *)OBJ_TraveScion(
                                                        &mirror->node,
                                                        &current->node);
                    }
                }
                clip = clip->next;
            }while(clip != Gkwin->visible_clip);
        }
    }
}

//----渐变色填充窗口的一部分---------------------------------------------------
//功能: 用指定的颜色填充窗口的一部分。
//参数: 参数是结构para的成员
//      gkwin，目标窗口
//      rect，待填充的矩形(使用相对坐标)
//      color，填充使用的颜色
//返回: 无
//-----------------------------------------------------------------------------
void GK_GradientFillRect(struct GkscParaGradientFillWin *para)
{
    struct RectBitmap *bitmap;
    struct Rectangle target,ins_rect,rc;
    struct DispDraw *my_draw_fun;
    struct ClipRect *clip;
    struct GkWinRsc *fb_gkwin,*fpwwin;
    u32 Color0,Color1,Mode;

    fpwwin = para->gkwin;
    bitmap = fpwwin->wm_bitmap;

    if(bitmap!=NULL)
    {
        rc.left =0;
        rc.top  =0;
        rc.right =bitmap->width;
        rc.bottom =bitmap->height;
    }
    else
    {
        rc.left =0;
        rc.top  =0;
        rc.right =fpwwin->right;
        rc.bottom =fpwwin->bottom;
    }

    if(!__GK_GetRectInts(&para->rect,&rc,&target))
    {
        return;
    }

    Mode = para->Mode;
    if(Mode== CN_FILLRECT_MODE_N)
    {
        __GK_FillPartWin(fpwwin,&target,para->Color0);
        return;
    }

    Color0 = para->Color0;
    Color1 = para->Color1;
    my_draw_fun = &fpwwin->disp->draw;
    //说明有win buffer，且直接写屏属性为false
    if((fpwwin->wm_bitmap != NULL)
        && (fpwwin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
    {
        //处理方法:在win buffer中绘图，着色changed_msk
        if(!my_draw_fun->FillRectToBitmap(bitmap,&target,&target,Color0,Color1,Mode))
        {
            //硬件加速不支持填充位图，则用软件实现
            __GK_GradientFillRect(bitmap,&target,&target,Color0,Color1,Mode);
        }
        __GK_ShadingRect(fpwwin,&para->rect);//着色填充区域的changed_msk
    }else
    {
        clip = fpwwin->visible_clip;
        if(clip == NULL)                //窗口可视域为空，直接返回
            return ;
        fb_gkwin = fpwwin->disp->frame_buffer;
        //接下来的绘制，是在帧缓冲或者screen上绘制，使用绝对坐标，变换之
        target.left += fpwwin->absx0;
        target.right += fpwwin->absx0;
        target.top += fpwwin->absy0;
        target.bottom += fpwwin->absy0;
        //有frame buffer,且直接写屏属性为false
        if((fb_gkwin != NULL) && (fpwwin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
        {   //处理方法:在frame buffer中绘图，但只绘gkwin中的可视区域
            do{
                //只填充窗口可视域与要绘制的区域的交集
                if(__GK_GetRectInts(&clip->rect,&target,&ins_rect))
                {
                    //帧缓冲的坐标即绝对坐标，故无须坐标变换
                    if(!my_draw_fun->FillRectToBitmap(fb_gkwin->wm_bitmap,
                                         &target,&ins_rect,Color0,Color1,Mode))
                    {
                        //硬件加速不支持填充位图，则用软件实现
                        __GK_GradientFillRect(fb_gkwin->wm_bitmap,&target,
                                                &ins_rect,Color0,Color1,Mode);
                    }
                    //标志填充区域的changed_msk
                    __GK_ShadingRect(fb_gkwin,&ins_rect);
                }
                clip = clip->next;
            }while(clip != fpwwin->visible_clip);
        }
        else
        //无win buffer，也无frame buffer，直接画在screen上
        //直接写屏属性为true，不管有无缓冲区，都直接画在screen上
        {
            do{
                struct DisplayRsc *mirror,*current;
                //只填充窗口可视域与要绘制的区域的交集
                if(__GK_GetRectInts(&clip->rect,&target,&ins_rect))
                {
                    //硬件加速不支持填充位图，则用软件实现
                    if(!my_draw_fun->FillRectToScreen(&target,&ins_rect,
                                                      Color0,Color1,Mode))
                    {
                        __GK_GradientFillScreenRect(my_draw_fun,&target,
                                                &ins_rect,Color0,Color1,Mode);
                    }
                    mirror = (struct DisplayRsc *)&fpwwin->disp->node;
                    if(mirror == NULL)      //没有镜像显示器，直接返回
                        return;
                    current = mirror;
                    while(current != NULL)
                    {
                        if(!current->draw.FillRectToScreen(&target,&ins_rect,
                                                       Color0,Color1,Mode) );
                        {
                            __GK_GradientFillScreenRect(&current->draw,&target,
                                                &ins_rect,Color0,Color1,Mode);
                        }
                        current =(struct DisplayRsc *)OBJ_TraveScion(
                                                &mirror->node,&current->node);
                    }
                }
                clip = clip->next;
            }while(clip != fpwwin->visible_clip);
        }
    }
}

//----填充窗口-----------------------------------------------------------------
//功能: 用指定的颜色填充窗口。
//参数: 参数是结构para的成员
//      gkwin，目标窗口指针
//      color，填充使用的颜色
//返回: 无
//-----------------------------------------------------------------------------
void GK_FillWin(struct GkscParaFillWin *para)
{
    struct Rectangle rect;
    struct ClipRect *clip;
    struct GkWinRsc *fb_gkwin,*fpwin;
    struct RectBitmap *bitmap;

    struct DispDraw *my_draw_fun;

    fpwin = para->gkwin;
    bitmap = fpwin->wm_bitmap;
    my_draw_fun = &fpwin->disp->draw;
    rect.left = 0;
    rect.top = 0;
    rect.right = bitmap->width;
    rect.bottom = bitmap->height;
    //说明有win buffer，且直接写屏属性为false
    if((fpwin->wm_bitmap != NULL)
        && (fpwin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
    {   //处理方法:在win buffer中绘图，标志changed_msk
        bitmap = fpwin->wm_bitmap;
        if(!my_draw_fun->FillRectToBitmap(bitmap,&rect,&rect,para->color,
                                        0,CN_FILLRECT_MODE_N))
        {//硬件加速不支持填充位图，则用软件实现
            __GK_FillBm(bitmap,para->color);
        }
        fpwin->WinProperty.ChangeFlag = CN_GKWIN_CHANGE_ALL;
    }else
    {
        clip = fpwin->visible_clip;
        if(clip == NULL)
            return ;
        fb_gkwin = fpwin->disp->frame_buffer;
        //有frame buffer，且直接写屏属性为false
        if((fb_gkwin != NULL) && (fpwin->WinProperty.DirectDraw == CN_GKWIN_UNDIRECT_DRAW))
        {   //处理方法:在frame buffer中绘图，但只绘gkwin中的可视区域
            bitmap = fb_gkwin->wm_bitmap;
            do{
                //帧缓冲的坐标和clip中的坐标都是绝对坐标，无须变换
                if(!my_draw_fun->FillRectToBitmap(bitmap,&rect,&clip->rect,
                                              para->color,0,CN_FILLRECT_MODE_N))
                {//硬件加速不支持填充位图，则用软件实现
                    __GK_FillRect(bitmap,&clip->rect,para->color);
                }
                //标志填充区域的changed_msk
                __GK_ShadingRect(fb_gkwin,&clip->rect);
                clip = clip->next;
            }while(clip != fpwin->visible_clip);
        }else
        //无win buffer，也无frame buffer，直接画在screen上
        //直接写屏属性为true，不管有无缓冲区，都直接画在screen上
        {
            do{
                struct DisplayRsc *mirror,*current;
                if(!my_draw_fun->FillRectToScreen(&rect,&clip->rect,
                                            para->color,0,CN_FILLRECT_MODE_N))
                {
                    //硬件加速不支持填充screen上的矩形域，则用软件实现
                    __GK_GradientFillScreenRect(my_draw_fun,&rect,&clip->rect,
                                            para->color,0,CN_FILLRECT_MODE_N);
                }
                mirror = (struct DisplayRsc *)&fpwin->disp->node;
                if(mirror == NULL)      //没有镜像显示器，直接返回
                    continue;
                current = mirror;
                while(current != NULL)
                {
                    if(!current->draw.FillRectToScreen(&rect,&clip->rect,
                                            para->color,0,CN_FILLRECT_MODE_N))
                    {
                        //硬件加速不支持填充screen上的矩形域，则用软件实现
                        __GK_GradientFillScreenRect(&current->draw,&rect,
                                                    &clip->rect,
                                                    para->color,
                                                    0,CN_FILLRECT_MODE_N);
                    }
                    current = (struct DisplayRsc *)OBJ_TraveScion(
                                                        &mirror->node,
                                                        &current->node);
                }
                clip = clip->next;
            }while(clip != fpwin->visible_clip);
        }
    }
}


