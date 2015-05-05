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
//文件描述: gui core的显示器部分，不包含硬件相关部分
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-11-21
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

#ifndef __GK_DISPLAY_H__
#define __GK_DISPLAY_H__
#include "stdint.h"
#include "gkernel.h"
#ifdef __cplusplus
extern "C" {
#endif
//绘制操作分四大类:
//1、在bitmap上绘制
//2、在screen上绘制
//3、取bitmap上的像素或bitmap
//4、取screen上的像素或bitmap。
//在bitmap上，绘像素、线时，是带rop2操作码的，硬件:不支持、部分支持、全支持
//bitmap上填充矩形的时候，不带rop2操作码，硬件可能支持，也可能不支持
//以上几种情况，均不能以NULL了之。而是支持的实现之，不支持的返回false，由gui
//kernel实现之。这样的话，也就无须check_raster函数了。

//gui kernel确保，各成员函数的参数中作为绘图目标的bitmap，均来自win buffer或者
//frame buffer中的bitmap，不是用户任意提供的bitmap。
//而作为图像源的bitmap，则可以是cn_sys_pf_xxx中的任意一种，或者与本显示器所使用
//的像素格式相同
struct tagDispDraw
{
    //在矩形位图中画一个像素，若显示器使用cn_custom_pf格式，或者有硬件加速功能，
    //应该实现这个函数，否则直接返回false。即使有硬件加速，但该加速功能不支持
    //r2_code编码的话，也返回false。color的格式是cn_sys_pf_e8r8g8b8
    bool_t (*set_pixel_bm)(struct tagRectBitmap *bitmap,
                         s32 x,s32 y,u32 color,u32 r2_code);
    //在矩形位图中画一条任意细直线，不含端点，若显示器使用cn_custom_pf格式，或者
    //有硬件加速功能，应该实现这个函数，否则直接返回false。即使有硬件加速，但该
    //加速功能不支持r2_code编码的话，也返回false。color格式是cn_sys_pf_e8r8g8b8
    bool_t (*line_bm)(struct tagRectBitmap *bitmap,struct tagRectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code);
    //在矩形位图中画一条任意细直线，含端点，若显示器使用cn_custom_pf格式，或者
    //有硬件加速功能，应该实现这个函数，否则直接返回false。即使有硬件加速，但该
    //加速功能不支持r2_code编码的话，也返回false。color格式是cn_sys_pf_e8r8g8b8
    bool_t (*line_bm_ie)(struct tagRectBitmap *bitmap,struct tagRectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code);
    //Bitmap中矩形填充，如硬件加速不支持在screen上矩形填充，driver可以简化，直接
    //返回false即可。
    //color的格式是CN_SYS_PF_ERGB8888
    //Target: 目标填充区域,渐变色填充时有用
    //Focus: 实际填充区域
    //Color0,Color1:Target左上角和右下角颜色值,如果不是渐变填充,则只需要Color0
    //Mode: 填充模式,CN_FILLRECT_MODE_N族常数
    bool_t (*fill_rect_bm)(    struct tagRectBitmap *DstBitmap,
                               struct tagRectangle *Target,
                               struct tagRectangle *Focus,
                               u32 Color0,u32 Color1,u32 Mode);

    //在两个矩形位图中位块传送，如果显示器使用的像素格式是cn_custom_pf，或者硬件
    //2d加速支持位块传送，须实现这个函数，否则直接返回false。
    //对于有硬件加速，但不只支持部分rop_code编码的情况，可以实现支持的rop_code，
    //不支持的部分，返回false。
    //本函数返回false的话，gui kernel会自行用逐像素方法拷贝。
    bool_t (*blt_bm_to_bm)( struct tagRectBitmap *dst_bitmap,
                            struct tagRectangle *DstRect,
                            struct tagRectBitmap *src_bitmap,
                            struct tagRectangle *SrcRect,
                            u32 RopCode,u32 transparentcolor);
    //从矩形位图中取一像素，并转换成cn_sys_pf_e8r8g8b8，如果显示器格式不是
    //cn_custom_pf，设为NULL。
    u32 (*get_pixel_bm)(struct tagRectBitmap *bitmap,s32 x,s32 y);
    //把src位图内rect矩形区域的内容复制到dest中，调用前，先设置好dest的pf_type，
    //本函数不理会src的pf_type，直接使用screen的格式。
    //本函数用于从窗口剪切矩形，而blt_bm_to_bm用于显示矩形。
    //如果显示器使用的像素格式是cn_custom_pf，或者硬件2d加速支持bm间拷贝图形，须
    //实现这个函数，否则直接返回false。
    //由于dest->pf_type存在很多可能，即使有硬件加速，也存在只支持部分pf_type的情
    //况，对不支持的格式，返回false。
    bool_t (*get_rect_bm)(struct tagRectBitmap *src,struct tagRectangle *rect,
                          struct tagRectBitmap *dest);

    //以下是在screen中绘图的函数
    //在screen中画一个像素，有frame buffer的情况下，正常显示gui不会调用这个函数，
    //如果窗口direct_screen==true，则可能调用本函数，
    //因此，无论是否有frame buffer，driver都必须提供并且必须实现本函数
    //镜像显示器的driver须提供这个函数，也必须实现本函数
    bool_t (*set_pixel_screen)(s32 x,s32 y,u32 color,u32 r2_code);
    //在screen中画一条任意直线，不含端点，如硬件加速不支持在screen上画线，driver
    //可以简化，直接返回false即可。有frame buffer的情况下，正常显示gui不会调用这
    //个函数，如果窗口direct_screen==true，则可能调用本函数，本函数返回false的
    //话，会进一步调用set_pixel_screen函数
    //镜像显示器的driver须提供这个函数，由于不知道本地显示器的情况，镜像显示器
    //driver必须实现本函数，不能简单返回false。
    //color的格式是cn_sys_pf_e8r8g8b8
    bool_t (*line_screen)(struct tagRectangle *limit,
                          s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code);

    //在screen中画一条任意直线，不含端点，如硬件加速不支持在screen上画线，driver
    //可以简化，直接返回false即可。有frame buffer的情况下，正常显示gui不会调用这
    //个函数，如果窗口direct_screen==true，则可能调用本函数，本函数返回false的
    //话，会进一步调用set_pixel_screen函数
    //镜像显示器的driver须提供这个函数，由于不知道本地显示器的情况，镜像显示器
    //driver必须实现本函数，不能简单返回false。
    //color的格式是cn_sys_pf_e8r8g8b8
    bool_t (*line_screen_ie)(struct tagRectangle *limit,
                             s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code);
    //screen中矩形填充，如硬件加速不支持在screen上矩形填充，driver可以简化，直接
    //返回false即可。
    //有frame buffer的情况下，正常显示gui不会调用这个函数，但如果窗口direct_screen
    //==true，则可能调用本函数，本函数返回false的话，会进一步调用set_pixel_screen
    //color的格式是CN_SYS_PF_ERGB8888
    //Target: 目标填充区域,渐变色填充时有用
    //Focus: 实际填充区域
    //Color0,Color1:Target左上角和右下角颜色值,如果不是渐变填充,则只需要Color0
    //Mode: 填充模式,CN_FILLRECT_MODE_N族常数
    bool_t (*fill_rect_screen)(struct tagRectangle *Target,
                               struct tagRectangle *Focus,
                               u32 Color0,u32 Color1,u32 Mode);
    //从内存缓冲区到screen位块传送，只支持块拷贝，不支持rop操作。
    //如硬件加速不支持在screen上绘制位图，driver可以简化，直接
    //返回false即可。有frame buffer的情况下，__gk_output_redraw中会调用这个函数
    //。如果窗口direct_screen==true，也可能调用本函数。本函数返回false的
    //话，gui kernel会进一步调用set_pixel_screen函数
    //即使硬件加速支持，但如果不支持具体的src_bitmap->PixelFormat,也可返回false
    //逐个个像素描镜像显示太慢，镜像显示器的driver必须实现本函数，不能简单返回false。
    bool_t (*bm_to_screen)(struct tagRectangle *dst_rect,
                           struct tagRectBitmap *src_bitmap,s32 xsrc,s32 ysrc);

    //从screen中取一像素，并转换成cn_sys_pf_e8r8g8b8
    u32 (*get_pixel_screen)(s32 x,s32 y);
    //把screen内矩形区域的内容复制到bitmap，调用前，先设置好dest的pf_type
    bool_t (*get_rect_screen)(struct tagRectangle *rect,struct tagRectBitmap *dest);
};

struct tagDisplayRsc
{
    struct tagRscNode  node;  //注:镜像显示器作为子资源结点挂在这里
    float xmm,ymm;          //用毫米表示的显示器尺寸
    s32   width,height;     //按像素表示的显示器尺寸
    u32   pixel_format;     //像素格式，在gk_win.h中定义
    bool_t reset_clip;      //true = 本显示器需要重新生成可视域
    //帧缓冲直接显示。有些cpu使用系统内存做显存，设计driver时，可将显存直接作为
    //帧缓冲用。true=显存与帧缓冲重合
    bool_t framebuf_direct;
    struct tagGkWinRsc  *frame_buffer;  //本显示器的帧缓冲，以窗口管理
    struct tagGkWinRsc  *desktop;       //本显示器的桌面
    struct tagGkWinRsc *z_topmost;      //z轴最前端窗体指针

    //A、如果在系统堆上分配窗口缓冲，DisplayHeap可置为NULL。
    //B、如果要在专用的显存Heap上分配窗口缓存，显卡驱动可使指针DisplayHeap指向该
    //   Heap
    struct tagHeapCB *DisplayHeap;

    struct tagDispDraw draw;            //显示函数集

    //控制显示器
    //这是由driver提供的一个应用程序的入口，该应用程序用于提供一个可视化的方式
    //设定该显示器所有可以由用户设定的参数，比如分辨率和和色彩参数。函数的功能
    //不做统一规定，驱动程序的文档应该提供函数的使用说明。
    //利用本函数，可以提供类似windows中设置显示器属性的功能。
    bool_t (*disp_ctrl)(struct tagDisplayRsc *disp);
};

bool_t GK_InstallDisplay(struct tagDisplayRsc *display,
                         const char *name);
bool_t GK_InstallDisplayMirror(struct tagDisplayRsc *base_display,
                                 struct tagDisplayRsc *mirror_display,char *name);
bool_t GK_SetDefaultDisplay(char *name);
struct tagGkWinRsc *GK_GetRootWin(struct tagDisplayRsc *display);
bool_t GK_SwitchFrameBuffer(struct tagDisplayRsc *display,
                              struct tagRectBitmap *fbuf);
struct tagRectBitmap *GK_CreateFrameBuffer(struct tagDisplayRsc *display);

#ifdef __cplusplus
}
#endif

#endif //__GK_DISPLAY_H__

