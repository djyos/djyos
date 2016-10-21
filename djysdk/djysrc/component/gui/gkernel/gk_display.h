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
//在bitmap上，绘像素、画线时，是带rop2操作码的，硬件加速:不支持、部分支持、全支持
//bitmap上填充矩形的时候，不带rop2操作码，硬件加速可能支持，也可能不支持
//以上几种情况，均不能赋值为NULL函数。而是支持的实现之，不支持的返回false，由gui
//kernel实现之。这样的话，也就无须check_raster函数了。

//gui kernel确保，各成员函数的参数中作为绘图目标的bitmap，均来自win buffer或者
//frame buffer中的bitmap，不是用户任意提供的bitmap。
//而作为图像源的bitmap，则可以是CN_SYS_PF_xxx中的任意一种，或者是CN_CUSTOM_PF

//----在以下情况下,结构内的显示函数必须实现功能:
//1.显示器使用CN_CUSTOM_PF格式，gkernel将不知道怎么绘制。
//2.显卡有硬件加速功能，且该硬件加速器能够支持所要求的绘制任务,硬件加速功能必
//  须由driver来调用,gkernel是不知道怎么调用特定显卡的硬件加速功能的.
//3.优化性能,即使不是CN_CUSTOM_PF格式,也没有硬件加速,在这里实现本函数,执行效率
//  可能比在gkernel中实现要高得多,因为gkernel需要考虑各种特殊情况,做许多判断,
//  而由driver实现,只需要考虑该显示器的特定情况.
//----下列情况可直接返回false,函数功能将由gkernel实现:
//1.驱动编写的初期,心急想抢先看一看能否显示
//2.虽然有硬件加速，但该加速功能不支持所要求的绘制任务.
//3.对执行效率无所谓
//----对于所要求的绘制操作,硬件加速器只支持部分的情况,可以在函数中做条件判断,
//    实现能支持的功能,不支持的功能则返回false,由gkernel实现
//特殊情况:虚拟显示器,需要远程界面但本地却没有显示器的情况,将会在本地虚拟出一个
//显示器,虚拟显示器driver中国全部绘制图像到screen的函数,都无须实现,其他同前面所述

//如果不是虚拟显示器，则此2函数，SetPixelToScreen,CopyBitmapToScreen，必须实现
//一个。如果应用不使用直接写屏，且有framebuffer，则建议实现CopyBitmapToScreen即可。
struct DispDraw
{
    //在矩形位图中画一个像素
    //什么情况下应该实现本函数的功能,参见本结构前面的注释
    //镜像显示器无须实现本函数,直接返回false即可
    //参数:
    // bitmap: 绘制的目标位图
    // x,y: 目标坐标,相对于bitmap的.
    // color: 格式是cn_sys_pf_e8r8g8b8
    // r2_code: 显示效果,详见gkernel.h中的CN_R2_BLACK族常数定义
    bool_t (*SetPixelToBitmap)(struct RectBitmap *bitmap,
                         s32 x,s32 y,u32 color,u32 r2_code);

    //在矩形位图中画一条线.含起点,不包含终点
    //什么情况下应该实现本函数的功能,参见本结构前面的注释
    //镜像显示器无须实现本函数,直接返回false即可
    //参数:
    // bitmap: 绘制的目标位图
    // limit: 线条中只有落入limit范围内的部分才被绘制,limit使用显示器中的绝对坐标,
    //       并非bitmap中的相对坐标.limit的坐标包含左和上边界,不包含右和下边界.
    // x1,y1: 绘制起点,包含
    // x2,y2: 绘制终点,不包含
    // color: 格式是cn_sys_pf_e8r8g8b8
    // r2_code: 显示效果,详见gkernel.h中的CN_R2_BLACK族常数定义
    bool_t (*LineToBitmap)(struct RectBitmap *bitmap,struct Rectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code);

    //在矩形位图中画一条线.包含起点和终点
    //什么情况下应该实现本函数的功能,参见本结构前面的注释
    //镜像显示器无须实现本函数,直接返回false即可
    //参数:
    // bitmap: 绘制的目标位图
    // limit: 线条中只有落入limit范围内的部分才被绘制,limit使用显示器中的绝对坐标,
    //       并非bitmap中的相对坐标.limit的坐标包含左和上边界,不包含右和下边界.
    // x1,y1: 绘制起点,包含
    // x2,y2: 绘制终点,包含
    // color: 格式是cn_sys_pf_e8r8g8b8
    // r2_code: 显示效果,详见gkernel.h中的CN_R2_BLACK族常数定义
    bool_t (*LineToBitmapIe)(struct RectBitmap *bitmap,struct Rectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code);

    //Bitmap中矩形填充,支持渐变色
    //什么情况下应该实现本函数的功能,参见本结构前面的注释
    //镜像显示器无须实现本函数,直接返回false即可
    //参数:
    // DstBitmap: 绘制的目标位图
    // Target: 目标填充区域,渐变色填充时用它的坐标做计算颜色的起点和终点
    // Focus: 实际填充区域,gkernel确保其在Target矩形内部
    // Color0,Color1: Target左上角和右下角颜色值,如果不是渐变填充,Color1将被忽略,
    //              像素格式是CN_SYS_PF_ERGB8888
    //Mode: 填充模式,CN_FILLRECT_MODE_N族常数
    bool_t (*FillRectToBitmap)(    struct RectBitmap *DstBitmap,
                               struct Rectangle *Target,
                               struct Rectangle *Focus,
                               u32 Color0,u32 Color1,u32 Mode);

    //在两个矩形位图中位块传送,如果矩形尺寸不相等,则要实现拉伸或压缩
    //什么情况下应该实现本函数的功能,参见本结构前面的注释
    //镜像显示器无须实现本函数,直接返回false即可
    //参数:
    // dst_bitmap: 目标位图
    // DstRect: 目标矩形
    // src_bitmap: 源位图
    // SrcRect: 源矩形
    // RopCode: 显示效果,参见 gkernel.h文件中 CN_ROP_ALPHA_SRC_MSK 族常量的说明
    // HyalineColor: 透明色,如果参数RopCode中设定了 CN_ROP_KEYCOLOR_EN,则
    //                  本参数指定透明色,CN_SYS_PF_ERGB8888格式
    bool_t (*BltBitmapToBitmap)( struct RectBitmap *dst_bitmap,
                            struct Rectangle *DstRect,
                            struct RectBitmap *src_bitmap,
                            struct Rectangle *SrcRect,
                            struct RopGroup RopCode,u32 HyalineColor);

    //以下是在screen中绘图的函数
    //在screen中画一个像素，有frame buffer的情况下，正常显示gui不会调用这个函数，
    //如果窗口direct_screen==true，则可能调用本函数，无论是否有frame buffer，
    //driver都必须提供并且必须实现本函数
    //镜像显示器必须实现本函数的功能,不能直接返回false
    //参数:
    // x,y: 像素坐标.
    // color: 颜色值,CN_SYS_PF_ERGB8888格式
    // r2_code: 显示效果,详见gkernel.h中的CN_R2_BLACK族常数定义
    bool_t (*SetPixelToScreen)(s32 x,s32 y,u32 color,u32 r2_code);

    //在screen中画一组像素，有frame buffer的情况下，正常显示gui不会调用这个函数，
    //如果窗口direct_screen==true，则可能调用本函数，无论是否有frame buffer，
    //driver都必须提供并且必须实现本函数
    //镜像显示器必须实现本函数的功能,不能直接返回false
    //参数:
    // PixelGroup: 像素坐标数组指针.
    // color: 颜色值,CN_SYS_PF_ERGB8888格式
    // n: 坐标点的数量
    // r2_code: 显示效果,详见gkernel.h中的CN_R2_BLACK族常数定义
    bool_t (*SetPixelGroupToScreen)(struct PointCdn *PixelGroup,u32 color,u32 n, u32 r2_code);

    //在screen中画一条任意直线，含起点不含终点.
    //什么情况下应该实现本函数的功能,参见本结构前面的注释
    //镜像显示器必须实现本函数的功能,不能直接返回false
    //参数:
    // limit: 画线范围,所画的线中,只有落在其范围内的才有效,含左上边界,不含右下边界.
    // x1,y1: 画线起点,包含
    // x2,y2: 画线终点,不包含
    // color: 颜色值,CN_SYS_PF_ERGB8888格式
    // r2_code: 显示效果,详见gkernel.h中的CN_R2_BLACK族常数定义
    bool_t (*LineToScreen)(struct Rectangle *limit,
                          s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code);

    //在screen中画一条任意直线，包含起点和终点.
    //什么情况下应该实现本函数的功能,参见本结构前面的注释
    //镜像显示器必须实现本函数的功能,不能直接返回false
    //参数:
    // limit: 画线范围,所画的线中,只有落在其范围内的才有效,含左上边界,不含右下边界.
    // x1,y1: 画线起点,包含
    // x2,y2: 画线终点,包含
    // color: 颜色值,CN_SYS_PF_ERGB8888格式
    // r2_code: 显示效果,详见gkernel.h中的CN_R2_BLACK族常数定义
    bool_t (*LineToScreenIe)(struct Rectangle *limit,
                             s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code);

    //screen中矩形填充.
    //什么情况下应该实现本函数的功能,参见本结构前面的注释
    //镜像显示器可以不实现本函数的功能,直接返回false,gkernel将调用 SetPixelToScreen
    //实现,但调用此函数效率极低,强烈建议镜像显示器实现本函数
    //参数:
    // Target: 目标填充区域,渐变色填充时用它的坐标做计算颜色的起点和终点
    // Focus: 实际填充区域,gkernel确保其在Target矩形内部
    // Color0,Color1: Target左上角和右下角颜色值,如果不是渐变填充,Color1将被忽略,
    //              像素格式是CN_SYS_PF_ERGB8888
    // Mode: 填充模式,CN_FILLRECT_MODE_N族常数
    bool_t (*FillRectToScreen)(struct Rectangle *Target,
                               struct Rectangle *Focus,
                               u32 Color0,u32 Color1,u32 Mode);

    //从内存缓冲区到screen位块传送，只支持块拷贝，不支持rop特效,也不支持压缩和拉伸。
    //什么情况下应该实现本函数的功能,参见本结构前面的注释
    //镜像显示器必须实现本函数的功能,不能直接返回false
    //参数:
    // dst_rect: screen上的目标区域,超出此范围的不理会.含左上边界,不包含右下边界
    // src_bitmap: 被copy的源位图
    // xsrc,ysrc: 源位图中被传送的起始坐标.
    bool_t (*CopyBitmapToScreen)(struct Rectangle *dst_rect,
                           struct RectBitmap *src_bitmap,s32 xsrc,s32 ysrc);

    //从screen中取一像素，并转换成CN_SYS_PF_ERGB8888
    // 显示器driver需要实现本函数.
    //镜像显示器无须实现本函数,直接返回false即可
    //参数:
    // x,y: 被读取的像素坐标
    u32 (*GetPixelFromScreen)(s32 x,s32 y);

    //把screen内矩形区域的内容复制到bitmap，调用前，先设置好dest的pf_type
    //什么情况下应该实现本函数的功能,参见本结构前面的注释,强烈建议,仅在该显示器的
    // struct DisplayRsc结构成员framebuf_direct==true的情况下,才考虑返回false,
    // 让gkernel去实现.
    //镜像显示器无须实现本函数,直接返回false即可
    //参数:
    //rect: screen中待复制的区域
    //dest: 接收位图的位图结构指针,其长宽尺寸应该 ≥ rect.
    bool_t (*GetRectFromScreen)(struct Rectangle *rect,struct RectBitmap *dest);
};

struct DisplayRsc
{
    struct Object  node;  //注:镜像显示器作为子资源结点挂在这里
    float xmm,ymm;          //用毫米表示的显示器尺寸
    s32   width,height;     //按像素表示的显示器尺寸
    u32   pixel_format;     //像素格式，在gk_win.h中定义
    bool_t reset_clip;      //true = 本显示器需要重新生成可视域
    //帧缓冲直接显示。有些cpu使用系统内存做显存，设计driver时，可将显存直接作为
    //帧缓冲用。true=显存与帧缓冲重合
    bool_t framebuf_direct;
    struct GkWinRsc  *frame_buffer;  //本显示器的帧缓冲，以窗口管理
    struct GkWinRsc  *desktop;       //本显示器的桌面
    struct GkWinRsc *z_topmost;      //z轴最前端窗体指针

    //A、如果在系统堆上分配窗口缓冲，DisplayHeap可置为NULL。
    //B、如果要在专用的显存Heap上分配窗口缓存，显卡驱动可使指针DisplayHeap指向该
    //   Heap
    struct HeapCB *DisplayHeap;

    struct DispDraw draw;            //显示函数集

    //控制显示器
    //这是由driver提供的一个应用程序的入口，该应用程序用于提供一个可视化的方式
    //设定该显示器所有可以由用户设定的参数，比如分辨率和和色彩参数。函数的功能
    //不做统一规定，驱动程序的文档应该提供函数的使用说明。
    //利用本函数，可以提供类似windows中设置显示器属性的功能。
    bool_t (*disp_ctrl)(struct DisplayRsc *disp);
};

bool_t GK_InstallDisplay(struct DisplayRsc *display,const char *name);
bool_t GK_InstallDisplayMirror(struct DisplayRsc *base_display,
                                 struct DisplayRsc *mirror_display,char *name);
bool_t GK_SetDefaultDisplay(const char *name);
struct GkWinRsc *GK_GetRootWin(struct DisplayRsc *display);
bool_t GK_SwitchFrameBuffer(struct DisplayRsc *display,
                              struct RectBitmap *fbuf);
struct RectBitmap *GK_CreateFrameBuffer(struct DisplayRsc *display);

#ifdef __cplusplus
}
#endif

#endif //__GK_DISPLAY_H__

