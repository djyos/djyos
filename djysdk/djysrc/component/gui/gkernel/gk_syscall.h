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
//文件描述: gui kernel主体部分
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-11-21
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __GK_SYSCALL_H__
#define __GK_SYSCALL_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "gkernel.h"


struct RectBitmap;
//gui kernel的syscall编码,由上层通过管道传递给gui kernel
//窗口控制命令组
#define CN_GKSC_SET_PEN_COLOR       0x000   //设置窗口画笔颜色
#define CN_GKSC_GET_PEN_COLOR       0x001   //取画笔颜色
#define CN_GKSC_SET_CANVAS_COLOR    0x002   //设置窗口画布颜色
#define CN_GKSC_GET_CANVAS_COLOR    0x003   //取画布颜色
#define CN_GKSC_SET_SIZE            0x004   //设置窗口分辨率
#define CN_GKSC_GET_SIZE            0x005   //取窗口分辨率
#define CN_GKSC_SET_CURSOR          0x006   //设置窗口光标位置
#define CN_GKSC_GET_CURSOR          0x007   //设置窗口光标位置
#define CN_GKSC_CREAT_GKWIN         0x008   //创建窗口命令
#define CN_GKSC_CREAT_DESKTOP       0x009   //创建桌面命令
#define CN_GKSC_SET_DIRECT_SCREEN   0x00a   //设置窗口的直接写屏属性
#define CN_GKSC_UNSET_DIRECT_SCREEN 0x00b   //取消窗口的直接写屏属性
#define CN_GKSC_FLUSH_WBUF          0x00c   //把窗口缓冲区中的图像刷新到screen
#define CN_GKSC_FLUSH_FBUF          0x00d   //把帧缓冲区中的图像刷新到screen
#define CN_GKSC_SET_ROP_CODE        0x00e   //设置窗口的光栅属性
#define CN_GKSC_SET_PAT_BUF         0x00f   //设置窗口的图案位图
#define CN_GKSC_SET_TRANSPARENTCOLOR 0x010  //设置窗口的透明色
#define CN_GKSC_SET_BITMSK          0x011   //设置窗口的掩码位图
#define CN_GKSC_DESTROY_WIN         0x012   //释放窗口的资源
#define CN_GKSC_SET_PRIO            0x013   //改变窗口的优先级
#define CN_GKSC_SET_BOUND_MODE      0x014   //设置窗口边界模式
#define CN_GKSC_MOVE_WIN            0x015   //移动窗口
#define CN_GKSC_CHANGE_WIN_AREA     0x016   //移动窗口
#define CN_GKSC_SYNC_SHOW           0x017   //立即输出至显示器
#define CN_GKSC_DSP_REFRESH         0x018   //重新刷新显示器
#define CN_GKSC_SET_VISIBLE         0x019   //重新刷新显示器

//绘制命令组
#define CN_GKSC_SET_PIXEL           0x100   //画点命令
#define CN_GKSC_LINETO              0x101   //画单像素宽线命令，不包含结束端点
#define CN_GKSC_LINETO_INC_END      0x102   //画单像素宽线命令，包含结束端点
#define CN_GKSC_DRAW_BITMAP_ROP     0x104   //画bitmap,带光栅操作
#define CN_GKSC_FILL_WIN            0x105   //用指定颜色填充窗口
#define CN_GKSC_DRAW_TEXT           0x106
#define CN_GKSC_FILL_RECT           0x107   //用指定颜色填充窗口内矩形，支持渐变
#define CN_GKSC_DRAW_CIRCLE         0x108   //画圆
#define CN_GKSC_BEZIER              0x109   //画贝塞尔曲线

struct GkscParaCreateDesktop
{
    struct DisplayRsc *display;        //所属显示器
    struct GkWinRsc *desktop;          //桌面窗口,由上层分配结构体
    s32 width,height;           //桌面尺寸，若小于显示器尺寸则调整为显示器尺寸
    u32 color;                          //创建桌面时填充的颜色
    u32 buf_mode;    //定义缓冲模式，参见 CN_WINBUF_PARENT 族常量定义
    //以下成员只在buf_mode=true时有效，否则忽略之。
    u16 PixelFormat;            //像素格式
    u16 rsv;            //保留用于4字节对齐
    u32 BaseColor;        //灰度图基色(当pf_type == CN_SYS_PF_GRAY1 ~8)
                            //pf_type取其他值时不需要此参数
};
struct GkscParaCreateGkwin
{
    struct GkWinRsc *parent_gkwin;   //新窗口的父窗口
    struct GkWinRsc *gkwin;          //新窗口,由调用者分配结构体
    struct GkWinRsc **result;        //用于返回结果
    s32 left,top,right,bottom;          //新窗口的位置，相对于父窗口的坐标
    u32 color;                          //创建窗口时填充的颜色
    u32 buf_mode;    //定义缓冲模式，参见 CN_WINBUF_PARENT 族常量定义
    char *name;             //窗口名字(标题)
    //以下成员只在buf_mode=true时有效，否则忽略之。
    u16 PixelFormat;                //像素格式，不允许用编号大于CN_GKWIN_PF_LIMIT的格式
    u16 rsv;                        //保留用于4字节对齐
    u32  HyalineColor;                  //透明色，gkwin中的rop_code允许透明色时有用
    u32 BaseColor  ;                //灰度图基色(当pf_type == CN_SYS_PF_GRAY1 ~8)
                                    //pf_type取其他值时不需要此参数
    struct RopGroup RopCode;        //窗口的光栅属性，在windows的光栅操作码的基础上有扩展
};
struct GkscParaFillWin
{
    struct GkWinRsc *gkwin;         //绘制的目标窗口
    u32 color;                      //填充使用的颜色
};

struct GkscParaGradientFillWin
{
    struct GkWinRsc *gkwin;         //绘制的目标窗口
    struct Rectangle rect;          //待填充的矩形
    u32 Color0;                     //颜色0
    u32 Color1;                     //颜色1
    u32 Mode;

};

//字体中的显点用color，隐点不显示。
//可设置显示用的rop2编码
struct GkscParaDrawText
{
    struct GkWinRsc* gkwin;         //目标窗口
    struct FontRsc *pFont;          //使用的字体,NULL表示用系统当前字体
    struct Charset *pCharset;       //使用的字符集,NULL表示用系统当前字符集
    s32 x, y;                       //显示位置，相对于gkwin
    s32 count;                      //要显示的字符数，-1则一直到串结束符。
    u32 color;                      //画点使用的颜色
    u32 Rop2Code;                   //rop2编码
};
struct GkscParaSetPixel
{
    struct GkWinRsc *gkwin;         //绘制的目标窗口
    s32 x,y;                        //点对应的坐标
    u32 color;                      //画点使用的颜色
    u32 Rop2Code;                   //二元光栅操作码
};
struct GkscParaDrawBitmapRop
{
    struct GkWinRsc *gkwin;         //绘制的目标窗口
    struct RectBitmap bitmap;       //待绘制的位图
    u32 HyalineColor;                   //透明色,与bitmap相同格式
    s32 x,y;                        //绘制位置的左上角坐标
    struct RopGroup RopCode;        //光栅操作码
};
struct GkscParaLineto
{
    struct GkWinRsc *gkwin;         //绘制的目标窗口
    s32 x1,y1,x2,y2;                //目标直线的起点，终点坐标
    u32 color;                      //绘制直线使用的颜色
    u32 Rop2Code;                   //二元光栅操作码
};
struct GkscParaDrawCircle
{
    struct GkWinRsc *gkwin;         //绘制的目标窗口
    s32 x0,y0;                      //圆心坐标
    s32 r;                          //圆的半径
    u32 color;                      //画圆使用的颜色
    u32 Rop2Code;                   //二元光栅操作码
};
struct GkscParaBezier
{
    struct GkWinRsc *gkwin;         //绘制的目标窗口
    float x1,y1,x2,y2,x3,y3,x4,y4;  //绘制Bezier曲线的四个控制点
    u32 color;                      //画Bezier曲线使用的颜色
    u32 Rop2Code;                   //二元光栅操作码
};
struct GkscParaMoveWin
{
    struct GkWinRsc *gkwin;      //目标窗口
    s32 left,top;                   //目标窗口新位置的左上角坐标
};
struct GkscParaChangeWinArea
{
    struct GkWinRsc *gkwin;      //目标窗口
    s32 left,top,right,bottom;      //目标窗口新的坐标
};
struct GkscParaSetBoundMode
{
    struct GkWinRsc *gkwin;      //目标窗口
    u32 mode;                 //窗口边界模式，true为受限，false为不受限
};
struct GkscParaSetPrio
{
    struct GkWinRsc *gkwin;      //目标窗口
    u32 prio;                   //新的优先级
};
struct GkscParaSetVisible
{
    struct GkWinRsc *gkwin;      //目标窗口
    u32 Visible;                 //新的优先级
};
struct GkscParaSetRopCode
{
    struct GkWinRsc *gkwin;      //绘制的目标窗口
    struct RopGroup RopCode;     //新的光栅操作码
};
struct GkscParaSetPatBuf
{
    struct GkWinRsc *gkwin;      //绘制的目标窗口
    struct RectBitmap pattern; //要设置的图案位图
};
struct GkscParaSetHyalineColor
{
    struct GkWinRsc *gkwin;      //绘制的目标窗口
    u32 HyalineColor;                   //要设置的透明色
};
struct GkscParaSetBitmsk
{
    struct GkWinRsc *gkwin;      //绘制的目标窗口
    struct RectBitmap Rop4Msk;  //要设置的掩码位图
};
#ifdef __cplusplus
}
#endif

#endif //__GK_SYSCALL_H__

