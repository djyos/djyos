// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名称：vnc_bmp.c
// 模块描述:
// 模块版本: V1.00
// 创建时间: 2013-7-1
// 原始作者: Administrator
// 功能说明：
// =============================================================================
#include "rfb.h"
#include "vnc_bmp.h"
#include "debug.h"

#include <stdio.h>//djyfs的读写不行，为什么？--TODO

#define widthbytes(bits) (((bits)+31)/32*4)
//求得当前图像的行bytes，主要是4字节对齐
// ===========================================================================
// 函数功能：测试函数
// 输出参数：
// 返回值    ：
// 说明         ：
// ===========================================================================
void debug_test_show_fileheader(bitmap_fileheader *fileheader)
{
    debug_printf("file_type = %04x\n",fileheader->bfType);
    debug_printf("file_size = %08x\n",fileheader->bfSize);
    debug_printf("file_ofset = %08x\n",fileheader->bfOffBits);
}

// ===========================================================================
// 函数功能：实现从文件中读取bmp图片信息，并写入到内存当中，
// 输入参数：filename文件名称,bitmap,存储bmp图像，其mapbits由此函数动态分配
// 输出参数：
// 返回值    ：
// 说明         ：该段内存仅仅记录图像的大小和RGB值以及基本色彩信息
//          实际上，该函数仅仅是将BMP颜色的位图从文件中解析成RGB各式的数据存放进
//          内存而已
//          现在针对24位的色彩进行转换处理
// ===========================================================================
bool_t vnc_get_image_from_bmp(const char *filename, \
        struct tagRectBitmap *bitmap)
{
    bool_t result = true;
    u16 read_size = 0;
    u16 width = 0;
    u16 height = 0;
    u16 i = 0;

    u8  *buf;//缓冲
    u32  num;

    bitmap_fileheader  file_header;
    bitmap_infoheader  image_info;
    u32 linebytes_bmp;//4字节对齐后
    u32 linebytes_image;//实际图像数据
    u32 data_offset;

//
//获取头文件信息
//    fp = fopen(filename,"r");
    fp = fopen(filename,"r+");
    if(NULL == fp)
    {
        result = false;
        debug_printf("open the file =%s failed\n",filename);
        goto open_filefailed;
    }
    else
    {
        debug_printf("open the file =%s success\n",filename);
    }

//    fseek(fp,0,SEEK_SET);
//    num = fread(&(file_header.bfType),2,1,fp);
//    num = fread(&(file_header.bfSize),12,1,fp);
//    debug_printf("read block = %d \n",num);
//    debug_test_show_fileheader(&file_header);
//    fseek(fp,cn_bmp_fileheader_size,SEEK_SET);
//    num = fread(&image_info,40,1,fp);
//    debug_printf("read block = %d \n",num);
//    data_offset = 0;
    fseek(fp,0,SEEK_SET);
    num = fread(&(file_header.bfType),2,1,fp);
    num = fread(&(file_header.bfSize),12,1,fp);
    debug_printf("read block = %d \n",num);
    debug_test_show_fileheader(&file_header);
    fseek(fp,cn_bmp_fileheader_size,SEEK_SET);
    num = fread(&image_info,40,1,fp);
    debug_printf("read block = %d \n",num);

    data_offset = file_header.bfOffBits;
    if(image_info.biWidth > 0)
    {
        width = image_info.biWidth;
    }
    else
    {
        width  = (u16)(-image_info.biWidth);
    }
    if(image_info.biHeight > 0)
    {
        height = image_info.biHeight;

    }
    else
    {
        height = (u16)(-image_info.biHeight);
    }

    linebytes_bmp = widthbytes(width*image_info.biBitCount);
    linebytes_image = (width*image_info.biBitCount +7)/8;

    buf = M_MallocLc(linebytes_image * height, 0);
    debug_printf("malloc %d bytes  buf = %08x\n",linebytes_image * height,buf);
    bitmap->bm_bits = buf;
    if(NULL == buf)
    {
        result = false;
        debug_printf("#failed:no mem for keymap\n");
        goto malloc_failed;
    }

    bitmap->width = width;
    bitmap->height = height;
    bitmap->linebytes = linebytes_image;
    if(image_info.biBitCount == 24)//24位，目前就用这种888，不用调色板
    {
        bitmap->PixelFormat = CN_SYS_PF_RGB888;
        //一行一行的读取数据，大部分条件下图像都是倒数的
        if(image_info.biHeight > 0)//倒序，先bottom后top
        {
            data_offset = data_offset + (height-1)*linebytes_bmp;//先指向底
            for(i = 0; i < height; i++)//行为单位读取所有行数据
            {
//              fseek(fp,data_offset,SEEK_SET);
//              fread(buf,linebytes_image,1,fp);
                fseek(fp,data_offset,SEEK_SET);
                fread(buf,linebytes_image,1,fp);
                buf += linebytes_image;
                data_offset = data_offset - linebytes_bmp;
            }
        }
        else
        {
            for(i = 0; i < height; i++)//行为单位读取所有行数据
            {
//              fseek(fp,data_offset,SEEK_SET);
//              fread(buf,linebytes_image,1,fp);
                fseek(fp,data_offset,SEEK_SET);
                fread(buf,linebytes_image,1,fp);
                buf += linebytes_image;
                data_offset = data_offset + linebytes_bmp;
            }
        }
    }

malloc_failed:
//    fclose(fp);
    fclose(fp);
open_filefailed:
    return result;
}


