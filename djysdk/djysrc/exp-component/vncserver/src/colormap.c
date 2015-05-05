// =======================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: VNCServer
// 模块版本: V1.00(初始化版本)
// 文件名     ：colormap.c
// 创建人员: Administrator
// 创建时间: 2013-3-9
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <V1.00> <2013-3-9>, <修改人员>: <修改功能概述>
// =======================================================================


#include "debug.h"
#include "rfbproto.h"
#include "colormap.h"
#include "color_remap.h"
#include "vnc_socket.h"

//RGB332的像素格式数据
static const rfbPixelFormat BGR233Format = {
    8, 8, 0, 1, 7, 7, 3, 5, 2, 0, 0, 0
};

int rfbEconomicTranslate = FALSE;
/*
 * Macro to compare pixel formats.
 */
//宏定义两种像素格式数据是否一样
#define PF_EQ(x,y)                                                      \
        ((x.bitsPerPixel == y.bitsPerPixel) &&                          \
         (x.depth == y.depth) &&                                        \
         ((x.bigEndian == y.bigEndian) || (x.bitsPerPixel == 8)) &&     \
         (x.trueColour == y.trueColour) &&                              \
         (!x.trueColour || ((x.redMax == y.redMax) &&                   \
                            (x.greenMax == y.greenMax) &&               \
                            (x.blueMax == y.blueMax) &&                 \
                            (x.redShift == y.redShift) &&               \
                            (x.greenShift == y.greenShift) &&           \
                            (x.blueShift == y.blueShift))))
/*
typedef void (*rfbInitTableFnType)(char **table, rfbPixelFormat *in,
			rfbPixelFormat *out);

*/


/*
 * rfbTranslateNone is used when no translation is required.
 */

/*
 * rfbSetClientColourMapBGR233 sets the client's colour map so that it's
 * just like an 8-bit BGR233 true colour clien*/
// =========================================================================
// 函数功能:设置客户端的8位颜色为真彩色（RGB332格式）
// 输入参数:待交流的客户cl
// 输出参数：
// 返回值  :
// 说明    :设置客户端的8位颜色消息映射，并将映射的内容发送过去
// =========================================================================
static int rfbSetClientColourMapBGR233(rfbClientPtr cl)
{
    char buf[sz_rfbSetColourMapEntriesMsg + 256 * 3 * 2];
    rfbSetColourMapEntriesMsg *scme = (rfbSetColourMapEntriesMsg *)buf;
    u16 *rgb = (u16 *)(&buf[sz_rfbSetColourMapEntriesMsg]);
    int i, len;
    int r, g, b;
//	unsigned char bellbuf[8];

    if (cl->format.bitsPerPixel != 8) {
        debug_printf("%s: client not 8 bits per pixel\n",
                "rfbSetClientColourMapBGR233");
        rfbCloseClient(cl);
        return FALSE;
    }

    scme->type = rfbSetColourMapEntries;

    scme->firstColour = Swap16IfLE(0);
    scme->nColours = Swap16IfLE(256);

    len = sz_rfbSetColourMapEntriesMsg;

    i = 0;

    for (b = 0; b < 4; b++) {
        for (g = 0; g < 8; g++) {
            for (r = 0; r < 8; r++)
            {
                rgb[i++] = Swap16IfLE(r * 65535 / 7);
                rgb[i++] = Swap16IfLE(g * 65535 / 7);
                rgb[i++] = Swap16IfLE(b * 65535 / 3);
                //8位映射成RGB(332)模式
            }
        }
    }

    len += 256 * 3 * 2;
    if (WriteExact(cl, buf, sz_rfbSetColourMapEntriesMsg) < 0) {
        debug_printf("rfbSetClientColourMapBGR233: write");
        rfbCloseClient(cl);
        return FALSE;
    }
	if (WriteExact(cl, buf+sz_rfbSetColourMapEntriesMsg, 256*3*2) < 0) {
        debug_printf("rfbSetClientColourMapBGR233: write");
        rfbCloseClient(cl);
        return FALSE;
    }

    return TRUE;
}
// =========================================================================
// 函数功能:映射客户端的color
// 输入参数:待交流的客户cl
// 输出参数：
// 返回值    :
// 说明         :当客户端的发来setpixelformat消息时，如果设置的cl的颜色不是真彩色时，
//          则服务器必须根据这种情况将这种客户端的颜色发送过去。以便客户端进行映射
// =========================================================================
int rfbSetTranslateFunction(rfbClientPtr cl)
{
    debug_printf("u32 format for client %s:\n",cl->host);
   // PrintPixelFormat(&cl->format);

    /*
     * Check that bits per pixel values are valid
     */
//客户端和服务器都不支持24位模式。请见谅
    if ((cl->screen->rfbServerFormat.bitsPerPixel != 8) &&
        (cl->screen->rfbServerFormat.bitsPerPixel != 16) &&
        (cl->screen->rfbServerFormat.bitsPerPixel != 32))
    {
        debug_printf("%s: server bits per pixel not 8, 16 or 32\n",
                "rfbSetTranslateFunction");
        rfbCloseClient(cl);
        return FALSE;
    }

    if ((cl->format.bitsPerPixel != 8) &&
        (cl->format.bitsPerPixel != 16) &&
        (cl->format.bitsPerPixel != 32))
    {
        debug_printf("%s: client bits per pixel not 8, 16 or 32\n",
                "rfbSetTranslateFunction");
        rfbCloseClient(cl);
        return FALSE;
    }

    if (!cl->format.trueColour && (cl->format.bitsPerPixel != 8)) {
        debug_printf("rfbSetTranslateFunction: client has colour map "
                "but %d-bit - can only cope with 8-bit colour maps\n",
                cl->format.bitsPerPixel);
        rfbCloseClient(cl);
        return FALSE;
    }

    /*
     * bpp is valid, now work out how to translate
     */
    if (!cl->format.trueColour) {
        /*
         * truecolour -> color map
         *
         * Set client's colour map to BGR233, then effectively it's
         * truecolour as well
         */
		debug_printf("--Begin to send the color entry to %s\n",cl->host);
        if (!rfbSetClientColourMapBGR233(cl))//将color映射表发送给client
            return FALSE;

        //cl->format = BGR233Format;
    }


//设置cl的映射函数
    if (PF_EQ(cl->format,cl->screen->rfbServerFormat))
    {

        /* client & server the same */
        debug_printf("no translation needed\n");
        cl->translateFn = rfbTranslateNone;
        return TRUE;
    }
    else
    {
    	set_client_colormap_function(cl);
    }
   //当服务器和和客户端的像素格式不一样时，必须要进行转换，这样才不会因为服务器和客户端的
    //像素格式不一样而发生紊乱

	debug_printf("--set translate function end!\n");
    return TRUE;
}




