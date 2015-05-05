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
//文件描述: gui kernel应用程序接口部分
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2011-10-07
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __GKERNEL_H__
#define __GKERNEL_H__
#include "stdint.h"
#include "rsc.h"
#include "errno.h"
#include "font.h"
#include "charset.h"

#ifdef __cplusplus
extern "C" {
#endif

struct tagDisplayRsc;
struct tagFontRsc;
//位图像素色彩格式定义，
//客户格式，显卡driver内部可使用上述定义以外的私有格式。
//应用程序可以通过gk_api_get_pixel_format查询显卡实际使用的格式。
//如果显卡使用私有格式，则上层查询显卡格式时，返回0x8000(CN_CUSTOM_PF).
//应用程序绘制点、线、填充等功能时，必须使用cn_sys_pf_e8r8g8b8格式。
//应用程序绘制位图时，如果使用显卡实际使用的像素格式，将获得最优化的显示速度。
//ARGBxxxx格式中的A称之为ALPHA通道，AlphaBlend时，使用该ALPHA值，否则使用
//  RopCode中携带的Alpha值。
//CN_SYS_PF_GRAY1~8是基于基色的灰度图，基色是struct tagRectBitmap结构中的
//  ExColor成员,ExColor是RGB888格式的。真实颜色是:灰度*基色
//CN_SYS_PF_GRAYx和CN_SYS_PF_ALPHAx的区别在于，后者需要背景色参与。
//CN_SYS_PF_PALETTE1~8是基于调色板的，此时struct tagRectBitmap结构中的ExColor是
//  调色板指针，是一个RGB888格式的色彩表。
#define CN_CUSTOM_PF            0x8000              //最高位为1，显卡自定义格式
                                                    //bit8~14声明像素位宽

#define CN_SYS_PF_DISPLAY       (0x0000)            //与所属display相同，只允许
                                                    //在API中使用
#define CN_PF_BITWIDE_MSK       0x7f00              //像素格式位宽掩码
#define CN_PF_INDEX_MSK         0x00ff              //像素格式编号掩码
#define CN_SYS_PF_RGB565        ((16<<8) | 0x00)    //16色，r在高位
#define CN_SYS_PF_RGB332        ((8<<8)  | 0x01)    //256色，r在高位
#define CN_SYS_PF_RGBE4444      ((16<<8) | 0x02)    //12位色,16位存储，占高12bit，r在高位
#define CN_SYS_PF_ERGB4444      ((16<<8) | 0x03)    //12位色,16位存储，占占低12bit，r在高位
//5551格式很别扭，暂时屏蔽掉，有需要时再根据实际格式增加。
//#define CN_SYS_PF_RGBC5551      ((16<<8) | 0x04)    //16位色,r在高位，c为rgb共用低位
#define CN_SYS_PF_ERGB6666      ((24<<8) | 0x05)    //18位色,r在高位，24位存储，高6bit空
#define CN_SYS_PF_E12RGB666     ((32<<8) | 0x06)    //18位色,r在高位，32位存储，高14bit空
#define CN_SYS_PF_RGB101010     ((32<<8) | 0x07)    //30位色,高两位空
#define CN_SYS_PF_RGB888        ((24<<8) | 0x08)    //24位色，r在高字节，
#define CN_SYS_PF_ERGB8888      ((32<<8) | 0x09)    //24位色，r在高字节，最高字节空

#define CN_DISPLAY_PF_LIMIT     0X80                //窗口和位图专用格式开始，下
                                                    //面的格式，不可用于显示器
#define CN_SYS_PF_GRAY1         ((1<<8)  | 0x83)    //1位灰度
#define CN_SYS_PF_GRAY2         ((2<<8)  | 0x84)    //2位灰度
#define CN_SYS_PF_GRAY4         ((4<<8)  | 0x85)    //4位灰度
#define CN_SYS_PF_GRAY8         ((8<<8)  | 0x86)    //8位灰度

#define CN_GKWIN_PF_LIMIT       0xc0                //位图专用格式开始，以下的
                                                    //格式，只可用于位图
//调色板格式定义必须按以下顺序连续排列
#define CN_SYS_PF_PALETTE1      ((1<<8)  | 0xC0)    //1位色，调色板
#define CN_SYS_PF_PALETTE2      ((2<<8)  | 0xC1)    //2位色，调色板
#define CN_SYS_PF_PALETTE4      ((4<<8)  | 0xC2)    //4位色，调色板
#define CN_SYS_PF_PALETTE8      ((8<<8)  | 0xC3)    //8位色，调色板
#define CN_SYS_PF_ALPHA1        ((1<<8)  | 0xc4)    //1位alpha
#define CN_SYS_PF_ALPHA2        ((2<<8)  | 0xc5)    //2位alpha
#define CN_SYS_PF_ALPHA4        ((4<<8)  | 0xc6)    //4位alpha
#define CN_SYS_PF_ALPHA8        ((8<<8)  | 0xc7)    //8位alpha
#define CN_SYS_PF_ARGB1555      ((16<<8) | 0xc8)    //15位色+1位alpha，a在高位
#define CN_SYS_PF_RGBA5551      ((16<<8) | 0xc9)    //15位色+1位alpha，a在高位
#define CN_SYS_PF_ARGB8888      ((32<<8) | 0xca)    //24位色+8位alpha，a在高字节

//二元光栅操作，名称与windows定义的一致，只在前面加了前缀CN
#define CN_R2_BLACK         0   //  0   dest = black（黑色）
#define CN_R2_NOTMERGEPEN   1   // dpon dest = ~(dest or pen)
#define CN_R2_MASKNOTPEN    2   // dpna dest = ~pen and dest
#define CN_R2_NOTCOPYEN     3   // pn   dest = ~pen
#define CN_R2_MASKPENNOT    4   // pdna dest = ~dest and pen
#define CN_R2_NOT           5   // dn   dest = ~dest
#define CN_R2_XORPEN        6   // dpx  dest = dest xor pen
#define CN_R2_NOTMASKPEN    7   // dpan dest = ~(dest and pen)
#define CN_R2_MASKPEN       8   // dpa  dest = dest and pen
#define CN_R2_NOTXORPEN     9   // dpxn dest = ~(dest xor pen)
#define CN_R2_NOP           10  // d    dest = dest（原色）
#define CN_R2_MERGENOTPEN   11  // dpno dest = ~pen or dest
#define CN_R2_COPYPEN       12  // p    dest = pen（画笔色）
#define CN_R2_MERGEPENNOT   13  // pdno dest = ~dest or pen
#define CN_R2_MERGEPEN      14  // dpo  dest = dest or pen
#define CN_R2_WHITE         15  //  1   dest = white（白色）
#define CN_R2_LAST          15

//渐变填充模式定义
#define CN_FILLRECT_MODE_N  0     //直接填充，只有Color0有效
#define CN_FILLRECT_MODE_H  1     //水平填充，Color0表示左边颜色，Color1右边
#define CN_FILLRECT_MODE_V  2     //垂直填充，Color0表示上边颜色，Color1下边
#define CN_FILLRECT_MODE_SP 3     //倾斜填充，Color0表示左上角颜色，Color1右下角
#define CN_FILLRECT_MODE_SN 4     //倾斜填充，Color0表示右上角颜色，Color1左下角

//常用颜色定义
#define CN_COLOR_RED                (0xff<<16)
#define CN_COLOR_GREEN              (0xff<<8)
#define CN_COLOR_BLUE               (0xff)
#define CN_COLOR_BLACK              (0)
#define CN_COLOR_WHITE              (0xffffff)

#define CN_WINBUF_PARENT    0       //继承父窗口，若为桌面，则继承帧缓冲
#define CN_WINBUF_NONE      1       //没有缓冲区
#define CN_WINBUF_BUF       2       //有缓冲区

//显示器设备控制常数
#define CN_GET_DISP_SIZE            3   //取显示器分辨率
#define CN_GET_DISP_FONT            4   //取显示器的默认字体指针
#define CN_GET_DISP_HEIGHT     5   //取字高,以特定字符(汉字)为参数
#define CN_GET_DISP_WIDTH      6   //取字宽,以特定字符(汉字)为参数
#define CN_GET_DISP_LINE_HEIGHT     7   //取最大字体高,即行高,
#define CN_GET_DISP_LINE_WIDTH      8   //取最大字体宽,在竖排文字显示时用得着,

//出错信息定义
enum _GK_ERROR_CODE_
{
    EN_GK_NO_ERROR = CN_GK_NO_ERROR,      //没有错误
    EN_GK_GKWIN_LIMIT,                    //窗口数量太多
    EN_GK_NO_MEMORY,                      //显存不足
    EN_GK_BLT_NO_SUPPORT,                  //不支持的blt代码
    EN_GK_CHARSET_INSTALL_ERROR,          //安装字符集出错
    EN_GK_FONT_INSTALL_ERROR,             //安装字体出错
};

struct tagPointCdn
{
    s32 x,y;       //像素坐标,坐标可以是负的(窗口外)
};

struct tagRectangle
{
    s32 left,top,right,bottom;      //坐标可以是负的,不包含right和bottom
};

struct tagRectBitmap        //矩形位图
{
    u32 PixelFormat;        //像素色彩格式
    s32 width;              //位图的宽度(以像素为单位)
    s32 height;             //位图的高度(以像素为单位)
    u32 linebytes;          //一行用多少字节
    ptu32_t ExColor;        //混合颜色(当pf_type == CN_SYS_PF_GRAY1~8，或者
                            //  pf_type == CN_SYS_PF_ALPHA1~8)
                            //调色板指针(PixelFormat == CN_SYS_PF_PALETTE1~8)
                            //pf_type取其他值时本成员无效
    u8  *bm_bits;           //指向存储像素阵列的数组
};

struct tagStAnyBitmap      //任意形状位图
{
    u16 PixelFormat;            //像素色彩格式
    u8  *bm_bits;           //指向存储像素阵列的数组
    //........定义边界曲线，再说吧
};
//剪切域队列
struct tagClipRect
{
    struct tagClipRect *next,*previous;       //把可视域连成双向循环链表
    struct tagRectangle rect;                    //可是区域的位置和尺寸。
};

//绘制函数中，支持rop2、alpha、KeyColor透明
//窗口属性中，只支持alpha和KeyColor透明，且alpha公式不允许需要dst alpha参与运算。
//byte0: Src Alpha(As),byte1: Dst Alpha(Ad)，
//byte2: alpha混合模式定义
//byte3: bit31=1表示支持alpha，bit30=1表示支持KeyColor透明,
//bit29=1表示支持rop2运算，bit24~27表示rop运算码
//特别注意，这里所说的alpha，针对常量alpha的，bitmap中的alpha通道不受影响
#define CN_ROP_ALPHA_SRC_MSK        0x000000ff  //Src Alpha(As)
#define CN_ROP_ALPHA_SRC_OFF        0
#define CN_ROP_ALPHA_DST_MSK        0x0000ff00  //Dst Alpha(Ad)
#define CN_ROP_ALPHA_DST_OFF        8
#define CN_ROP_ALPHA_MODE_MSK       0x00ff0000  //alpha混合运算公式
#define CN_ROP_ALPHA_MODE_OFF       16
#define CN_ROP_ROP2_MSK             0x0f000000  //ROP2运算公式,参看CN_R2_BLACK族常数
#define CN_ROP_ROP2_OFF             24

//注:rop2和alpha不能同时支持，若是能了alpha，rop2将自动忽略
#define CN_ROP_ALPHA_EN             0x80000000  //1=使能alpha运算
#define CN_ROP_KEYCOLOR_EN          0x40000000  //1=使能KeyColor透明
#define CN_ROP_ROP2_EN              0x20000000  //1=使能rop2运算
//以下定义alpha运算公式
//定义规则:bit7=1表示运算时需dst像素参与；bit6=1表示运算时需Ad参与
#define CN_ALPHA_MODE_AsN           0x00    //dst = S*As+D*(1-As)
#define CN_ALPHA_MODE_AsAdN         0x40    //dst = S*As+D*(1-As)*Ad
#define CN_ALPHA_MODE_AsAd          0x41    //dst = S*As+D*Ad
#define CN_ALPHA_MODE_As            0x80    //dst = S*As
#define CN_ALPHA_MODE_NEED_DST      0x80    //bit7=1,需要dst像素参与运算
#define CN_ALPHA_MODE_NEED_AD       0x40    //bit6=1,需要dst alpha参与运算

#define CN_BOUND_LIMIT      true
#define CN_BOUND_UNLIMIT    false
#define CN_GKWIN_NAME_LIMIT     127

//微言:原来定义了光标和输入焦点,但考虑到光标应该是应用程序的事,删除了.
struct tagGkWinRsc                  //窗口资源定义
{
    struct tagRscNode node;       //资源结点
    void *user_private;         //用户设置的标识，通常指向一个数据结构,
                                //如果=NULL，则指向gkwin自身
    struct tagGkWinRsc *z_back;   //窗口Z序链表，从最前端往回看
    struct tagGkWinRsc *z_top;
    char win_name[CN_GKWIN_NAME_LIMIT+1];         //窗口名字(标题)
//可视域队列，每个窗口的可视域通过此队列连接
//虚拟窗口无可视域
//最小化的窗口无可视域
//被完全遮挡的窗口无可视域
//隐藏的窗口无可视域
    struct tagClipRect *visible_clip; //新可视域链表,双向循环链表
    struct tagClipRect *visible_bak;  //原可视域链表,双向循环链表
    struct tagClipRect *redraw_clip;  //需要刷新显示的可视域链表
    struct tagClipRect *changed_clip; //被修改的区域链表
    struct tagClipRect *copy_clip;    //用于visible_clip的临时备份
    struct tagDisplayRsc *disp;       //本窗口所属显示器
    sfast_t z_prio;             //前端优先级，-128~127，表示在兄弟窗口中的相对位
                                //置，0表示与父win同级，大于0表示被父win覆盖
    bool_t direct_screen;       //直接写屏属性，本属性为true，则所有对本win的绘
                                //制操作均直接画在screen上，无论是否有frame
                                //buffer和win buffer。
    bool_t dest_blend;          //true窗口显示时需要和背景色混合，false=不需要
    u32 KeyColor;               //透明显示透明色，rop_code允许透明色时用,RGB888
    u32 RopCode;               //光栅操作代码，除windows定义的标准光栅操作码外，
                                //还包括alpha混合、KeyColor透明
                                //如果要求dst参与运算，则不管窗口显示在何处，都
                                //取父窗口的颜色和alpha。并且要求父窗口必须有
                                //窗口缓冲。
    struct tagRectBitmap changed_msk;//用来标记窗口中的被改写区域，
                                      //每bit代表8*8像素
    struct tagRectBitmap *wm_bitmap; //窗口实体(暂为矩形，考虑升级为任意形状)
//    struct tagRectBitmap *Rop4Msk;//位掩码，每位一个像素，只有支持rop4才需要
//    u8 *pat_buf;                //pattern位图缓冲区，支持rop才有
//                                //像素格式与显示器像素格式相同。
    ufast_t change_flag;        //可取值:cn_gkwin_change_none等
    bool_t bound_limit;         //true=显示范围限制在父窗口边界内，任何显示器的
                                //直接放在桌面的窗口，必定受限
                                //false=不受父窗口边界限制,但如果父窗口是受限的，
                                //则限制于祖父窗口
    s32 absx0,absy0;            //窗口绝对位置，相对于所属screen的桌面原点，
    s32 left,top,right,bottom;  //窗口矩形，相对于父窗口，像素坐标

    s32 limit_left,limit_top; //被祖先窗口切割后的x边界，相对于父窗口
    s32 limit_right,limit_bottom; //被祖先窗口切割后的y边界，相对于父窗口

};

bool_t GK_ApiCreateDesktop(struct tagDisplayRsc *display,
                                        struct tagGkWinRsc *desktop,
                                        s32 width,s32 height,u32 color,
                                        u32 buf_mode,u16 PixelFormat,
                                        u32 BaseColor);
struct tagGkWinRsc *GK_ApiGetDesktop(char *display_name);
bool_t GK_ApiCreateGkwin(struct tagGkWinRsc *parent,
                                        struct tagGkWinRsc *newwin,
                                        s32 left,s32 top,s32 right,s32 bottom,
                                        u32 color,u32 buf_mode,
                                        char *name,u16 PixelFormat,u32 KeyColor,
                                        u32 BaseColor,u32 RopCode);
void GK_ApiFillWin(struct tagGkWinRsc *gkwin,u32 color,u32 sync_time);
void GK_ApiFillRect(struct tagGkWinRsc *gkwin,struct tagRectangle *rect,
                            u32 Color0,u32 Color1,u32 Mode,u32 sync_time);
void GK_ApiSyncShow(u32 sync_time);
void GK_ApiDrawText(struct tagGkWinRsc *gkwin,
                    struct tagFontRsc *pFont,
                    struct tagCharset *pCharset,
                    s32 x,s32 y,
                    const char *text,u32 count,u32 color,
                    u32 Rop2Code,u32 SyncTime);
void GK_ApiSetPixel(struct tagGkWinRsc *gkwin,s32 x,s32 y,
                        u32 color,u32 rop2_code,u32 sync_time);
void GK_ApiLineto(struct tagGkWinRsc *gkwin, s32 x1,s32 y1,
                    s32 x2,s32 y2,u32 color,u32 rop2_code,u32 sync_time);
void GK_ApiLinetoIe(struct tagGkWinRsc *gkwin, s32 x1,s32 y1,
                    s32 x2,s32 y2,u32 color,u32 rop2_code,u32 sync_time);
void GK_ApiDrawBitMap(struct tagGkWinRsc *gkwin,
                                struct tagRectBitmap *bitmap,
                                s32 x,s32 y,
                                u32 KeyColor,u32 RopCode,u32 SyncTime);
void GK_ApiDrawCircle(struct tagGkWinRsc *gkwin,s32 x0,s32 y0,
                    u32 r,u32 color,u32 rop2_code,u32 sync_time);
void GK_ApiDrawBezier(struct tagGkWinRsc *gkwin,float x1,float y1,
                    float x2,float y2,float x3,float y3,float x4,float y4,
                    u32 color,u32 rop2_code,u32 sync_time);
void GK_ApiMoveWin(struct tagGkWinRsc *gkwin,s32 left,s32 top,u32 sync_time);
void GK_ApiChangeWinArea(struct tagGkWinRsc *gkwin, s32 left,s32 top,
                                       s32 right,s32 bottom, u32 SyncTime);
void GK_ApiSetBoundMode(struct tagGkWinRsc *gkwin,bool_t mode);
void GK_ApiSetPrio(struct tagGkWinRsc *gkwin,sfast_t prio,u32 sync_time);
bool_t GK_ApiSetRopCode(struct tagGkWinRsc *gkwin,
                            u32 RopCode,u32 sync_time);
bool_t GK_ApiSetPatBuf(struct tagGkWinRsc *gkwin,
                            struct tagRectBitmap *pattern,
                            u32 sync_time);
bool_t GK_ApiSetTransparentColor(struct tagGkWinRsc *gkwin,
                                    u32 transparentcolor);
bool_t GK_ApiSetRop4Msk(struct tagGkWinRsc *gkwin,
                                struct tagRectBitmap *Rop4Msk,u32 sync_time);
void GK_ApiSetDirectScreen(struct tagGkWinRsc *gkwin,
                                                u32 sync_time);
void GK_ApiCancelDirectScreen(struct tagGkWinRsc *gkwin,
                                                u32 sync_time);
void GK_ApiDestroyWin(struct tagGkWinRsc *gkwin);
u16 GK_ApiGetPixelFormat(struct tagDisplayRsc *display);

u32 GK_ConvertPF2PF(u16 spf_type,u16 dpf_type,u32 color,ptu32_t ExColor);
u32 GK_BlendRop2(u32 dest,u32 pen,u32 rop2_code);
u32 GK_ConvertColorToRGB24(u16 PixelFormat,u32 color,ptu32_t ExColor);
u32 GK_ConvertRGB24ToPF(u16 PixelFormat,u32 color);

u32 GK_GetPixelBm(struct tagRectBitmap *bitmap,s32 x,s32 y);

#ifdef __cplusplus
}
#endif

#endif //__GKERNEL_H__

