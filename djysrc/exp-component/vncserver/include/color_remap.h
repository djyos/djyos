// =======================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: VNCServer
// 模块版本: V1.00(初始化版本)
// 文件名     ：color_remap.h
// 创建人员: Administrator
// 创建时间: 2013-6-10
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <V1.00> <2013-3-5>, <修改人员>: <修改功能概述>
// =======================================================================

#ifndef COLOR_REMAP_H_
#define COLOR_REMAP_H_

#include "rfbproto.h"

void set_client_colormap_function(rfbClientPtr cl);

void  rfbTranslate8toX(rfbPixelFormat *src,
                                    rfbPixelFormat *dst,
                                    char *iptr, char *optr,
                                    int bytesBetweenInputLines,
                                    int width, int height);

void  rfbTranslate16toX(rfbPixelFormat *src,
                                    rfbPixelFormat *dst,
                                    char *iptr, char *optr,
                                    int bytesBetweenInputLines,
                                    int width, int height);


void  rfbTranslate32toX(rfbPixelFormat *src,
                                    rfbPixelFormat *dst,
                                    char *iptr, char *optr,
                                    int bytesBetweenInputLines,
                                    int width, int height);

void  rfbColorTranslate(rfbPixelFormat *src,
                                    rfbPixelFormat *dst,
                                    char *iptr, char *optr,
                                    int bytesBetweenInputLines,
                                    int width, int height);
void rfbTranslateNone(rfbPixelFormat *in, rfbPixelFormat *out,
                 char *iptr, char *optr, int bytesBetweenInputLines,
                 int width, int height);

void showPixelFormat(rfbPixelFormat *pix);
#endif /* COLOR_REMAP_H_ */
