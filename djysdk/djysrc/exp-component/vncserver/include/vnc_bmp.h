// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名称：vnc_bmp.h
// 模块描述: 关于bmp图像文件的处理
// 模块版本: V1.00
// 创建时间: 2013-7-1
// 原始作者: Administrator
// 功能说明：  主要是为了对键盘界面的处理
// =============================================================================
#ifndef __VNC_BMP_H__
#define __VNC_BMP_H__

#include "stdint.h"

#define   cn_bmp_fileheader_size 14
#define   cn_bmp_mapinfoheader_size 40
//bm文件头信息
typedef struct _bitmap_fileheader
{
    u16 bfType;
    u32 bfSize;
    u16 bfReserved1;
    u16 bfReserved2;
    u32 bfOffBits;
}bitmap_fileheader;
//bm文件图像信息
typedef struct _bitmap_infoheader
{
    u32 biSize;
    s32 biWidth;
    s32 biHeight;
    u16 biPlanes;
    u16 biBitCount;
    u32 biCompression;
    u32 biSizeImag;
    s32 biXPelsPerMeter;
    s32 biYPelsPerMeter;
    u32 biCLrUsed;
    u32 biClrImportant;
}bitmap_infoheader;

bool_t vnc_get_image_from_bmp(const char *filename, \
        struct tagRectBitmap *bitmap);

#endif /* __VNC_BMP_H__ */
