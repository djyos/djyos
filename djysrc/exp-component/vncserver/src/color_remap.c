// =======================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: VNCServer
// 模块版本: V1.00(初始化版本)
// 文件名     ：color_remap.c
// 创建人员: Administrator
// 创建时间: 2013-6-10
// 说明          ：颜色的映射转换表
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <V1.00> <2013-3-5>, <修改人员>: <修改功能概述>
// =======================================================================
#include "rfb.h"

#include "color_remap.h"
#include "debug.h"


void showPixelFormat(rfbPixelFormat *pix)
{
    debug_printf("pixbits=%d t=%d r=%d  rs=%d g=%d gs=%d b=%d  bs=%d\n",pix->bitsPerPixel, pix->trueColour,pix->redMax, pix->redShift,\
        pix->greenMax, pix->greenShift, pix->blueMax,pix->blueShift);
}


//其实想想，这个func应该归screen所有，算了吧，后续改进吧--TODO

void set_client_colormap_function(rfbClientPtr cl)
{
    u8 screenPixbytes = 0 ;

    cl->translateFn = rfbColorTranslate;
    return ;
    if(NULL != cl)
    {
        screenPixbytes = (cl->screen->rfbServerFormat.bitsPerPixel) /8;
        switch (screenPixbytes)
        {
            case 0:
                cl->translateFn = NULL;
                break;
            case 1:
                cl->translateFn = rfbTranslate8toX;
                break;
            case 2:
                cl->translateFn = rfbTranslate16toX;
                break;
            case 4:
                cl->translateFn = rfbTranslate32toX;
                break;
            default:
                cl->translateFn = NULL;
                break;
        }
    }
}

// ========================================================================
// 函数功能:从一段内存中转换一个矩形图形到所知颜色到相关的buf中(源图像是8的转换)
// 输入参数:
// 输出参数：
// 返回值    :true:发送成功  false：发送失败
// 说明         :主要是进行图像格式的转化，在此不做参数检查，内部使用函数
//       本身应该由足够的内存去容纳，同时有目的地址的颜色只能是8 16 32，暂时不支持24
// ==========================================================================
void  rfbTranslate8toX(rfbPixelFormat *src,
                                    rfbPixelFormat *dst,
                                    char *iptr, char *optr,
                                    int bytesBetweenInputLines,
                                    int width, int height)
{
    int i=0,j=0;
    char *srcbuf;
    char *dstbuf;
    char *Temp_srcbuf;
    char *Temp_dstbuf;
    int src_pixelbytes=0;
    int dst_pixelbytes=0;


    u32      v_u32 = 0;
    u16      v_u16 = 0;
    u8       v_u8 = 0;
    u8       must_swap = 0;

    src_pixelbytes=src->bitsPerPixel/8;
    dst_pixelbytes=dst->bitsPerPixel/8;
    srcbuf=iptr;
    dstbuf=optr;
    Temp_srcbuf=srcbuf;
    Temp_dstbuf=dstbuf;
    // showPiexFormat(src);
 //   showPixelFormat(dst);
    if(src->bigEndian == dst->bigEndian)
    {
        must_swap = 0;
    }
    else
    {
        must_swap = 1;
    }

    if(dst_pixelbytes == 1)//8 到8的变换
    {
        if(src->trueColour && dst->trueColour)
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u8=*((u8 *)Temp_srcbuf);
                    v_u8 = (((v_u8 >> src->redShift)&(src->redMax))<< dst->redShift)|\
                           (((v_u8 >> src->greenShift)&(src->greenMax))<< dst->greenShift)|\
                           (((v_u8 >> src->blueShift)&(src->blueMax))<< dst->blueShift);

                     *((u8*)Temp_dstbuf)= v_u8;
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
        else//有非真彩色的话直接拷贝吧
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u8=*((u8 *)Temp_srcbuf);

                     *((u8*)Temp_dstbuf)= v_u8;
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
    }
    if(dst_pixelbytes == 2)//8 到16的变换
    {
        if(src->trueColour && dst->trueColour)
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u8=*((u8 *)Temp_srcbuf);
                    v_u16 = (((v_u8 >> src->redShift)&(src->redMax))<< dst->redShift)|\
                           (((v_u8 >> src->greenShift)&(src->greenMax))<< dst->greenShift)|\
                           (((v_u8 >> src->blueShift)&(src->blueMax))<< dst->blueShift);

                     *((u16*)Temp_dstbuf)= swap16_if_need(v_u16, must_swap);
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
        else if(dst->trueColour)//cl是真彩色
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u8=*((u8 *)Temp_srcbuf);
                    v_u16 = ((v_u8 &(dst->redMax))<< dst->redShift)|\
                            ((v_u8 &(dst->redMax))<< dst->redShift)|\
                            ((v_u8 &(dst->redMax))<< dst->redShift);
                     *((u16*)Temp_dstbuf)= swap16_if_need(v_u16, must_swap);
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
        else
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u8=*((u8 *)Temp_srcbuf);
                    v_u16 = v_u8<<8;
                     *((u16*)Temp_dstbuf)= swap16_if_need(v_u16, must_swap);
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
    }
    if(dst_pixelbytes == 4)//8到32的变换
    {
        if(src->trueColour && dst->trueColour)
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u8=*((u8 *)Temp_srcbuf);
                    v_u32 = (((v_u8 >> src->redShift)&(src->redMax))<< dst->redShift)|\
                           (((v_u8 >> src->greenShift)&(src->greenMax))<< dst->greenShift)|\
                           (((v_u8 >> src->blueShift)&(src->blueMax))<< dst->blueShift);

                     *((u32*)Temp_dstbuf)= swap32_if_need(v_u32, must_swap);
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
        else if(dst->trueColour)//cl是真彩色
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u8=*((u8 *)Temp_srcbuf);
                    v_u32 = ((v_u8 &(dst->redMax))<< dst->redShift)|\
                            ((v_u8 &(dst->redMax))<< dst->redShift)|\
                            ((v_u8 &(dst->redMax))<< dst->redShift);
                     *((u32*)Temp_dstbuf)= swap32_if_need(v_u32, must_swap);
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
        else
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u8=*((u8 *)Temp_srcbuf);
                    v_u32 = v_u8<<8;
                     *((u32*)Temp_dstbuf)= swap32_if_need(v_u32, must_swap);
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
    }
}

// ========================================================================
// 函数功能:从一段内存中转换一个矩形图形到所知颜色到相关的buf中(源图像是16的转换)
// 输入参数:
// 输出参数：
// 返回值    :true:发送成功  false：发送失败
// 说明         :主要是进行图像格式的转化，在此不做参数检查，内部使用函数
//       本身应该由足够的内存去容纳，同时有目的地址的颜色只能是8 16 32，暂时不支持24
// ==========================================================================
void  rfbTranslate16toX(rfbPixelFormat *src,
                                    rfbPixelFormat *dst,
                                    char *iptr, char *optr,
                                    int bytesBetweenInputLines,
                                    int width, int height)
{

    int i=0,j=0;
    char *srcbuf;
    char *dstbuf;
    char *Temp_srcbuf;
    char *Temp_dstbuf;
    int src_pixelbytes=0;
    int dst_pixelbytes=0;


    u32      v_u32 = 0;
    u16      v_u16 = 0;
    u8       v_u8 = 0;
    u8       must_swap = 0;

    src_pixelbytes=src->bitsPerPixel/8;
    dst_pixelbytes=dst->bitsPerPixel/8;
    srcbuf=iptr;
    dstbuf=optr;
    Temp_srcbuf=srcbuf;
    Temp_dstbuf=dstbuf;


    // showPiexFormat(src);
//    showPixelFormat(dst);

    if(src->bigEndian == dst->bigEndian)
    {
        must_swap = 0;
    }
    else
    {
        must_swap = 1;
    }

    if(dst_pixelbytes == 1)//16 到8的变换
    {
        if(src->trueColour && dst->trueColour)
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u16=*((u16 *)Temp_srcbuf);
//                    v_u8 = (((v_u16 >> src->redShift)&(src->redMax))<< dst->redShift)|\
//                             (((v_u16 >> src->greenShift)&(src->greenMax))<< dst->greenShift)|\
//                             (((v_u16 >> src->blueShift)&(src->blueMax))<< dst->blueShift);
                    v_u8 = (((v_u16>>11)&0x38)<<2)|\
                            (((v_u16>>5)&0x38)>>1)|\
                            (((v_u16>>0)&0x18)>>3);
                     *((u8*)Temp_dstbuf)= v_u8;
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
        else if(src->trueColour)//源是真彩色，转化8位灰色
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u16 = *((u16 *)Temp_srcbuf);
                    v_u8 = (((v_u16 >> src->redShift)&(src->redMax/2))<< (src->redShift/2))|\
                           (((v_u16 >> src->greenShift)&(src->greenMax/2))<< (src->greenShift/2))|\
                           (((v_u16 >> src->blueShift)&(src->blueMax/2))<< (src->blueShift/2));
                     *((u8*)Temp_dstbuf) = v_u8;
                     Temp_dstbuf += dst_pixelbytes;
                     Temp_srcbuf += src_pixelbytes;
                  }
                  srcbuf += bytesBetweenInputLines;
                  dstbuf += width*dst_pixelbytes;
            }
        }
        else//非真彩色的话直接拷贝吧
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u8 = (*((u16 *)Temp_srcbuf)) >> 8;

                     *((u8*)Temp_dstbuf) = v_u8;
                     Temp_dstbuf += dst_pixelbytes;
                     Temp_srcbuf += src_pixelbytes;
                  }
                  srcbuf += bytesBetweenInputLines;
                  dstbuf += width*dst_pixelbytes;
            }
        }
    }
    if(dst_pixelbytes == 2)//16 到16的变换
    {
        if(src->trueColour && dst->trueColour)
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u16=*((u16 *)Temp_srcbuf);
                    v_u16 = (((v_u16 >> src->redShift)&(src->redMax))<< dst->redShift)|\
                           (((v_u16 >> src->greenShift)&(src->greenMax))<< dst->greenShift)|\
                           (((v_u16 >> src->blueShift)&(src->blueMax))<< dst->blueShift);

                     *((u16*)Temp_dstbuf)= swap16_if_need(v_u16, must_swap);
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
        else//cl是真彩色
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u16=*((u16 *)Temp_srcbuf);
                     *((u16*)Temp_dstbuf)= swap16_if_need(v_u16, must_swap);
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
    }
    if(dst_pixelbytes == 4)//16到32的变换
    {
        if(src->trueColour && dst->trueColour)
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u16=*((u16 *)Temp_srcbuf);
                    v_u32 = (((v_u16 >> src->redShift)&(src->redMax))<< dst->redShift)|\
                           (((v_u16 >> src->greenShift)&(src->greenMax))<< dst->greenShift)|\
                           (((v_u16 >> src->blueShift)&(src->blueMax))<< dst->blueShift);

                     *((u32*)Temp_dstbuf)= swap32_if_need(v_u32, must_swap);
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
        else if(dst->trueColour)//cl是真彩色
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u16=(*((u16 *)Temp_srcbuf)) >> 8;
                    v_u32 = ((v_u16 &(dst->redMax))<< dst->redShift)|\
                            ((v_u16 &(dst->greenMax))<< dst->greenShift)|\
                            ((v_u16 &(dst->blueMax))<< dst->blueShift);
                     *((u32*)Temp_dstbuf)= swap32_if_need(v_u32, must_swap);
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
        else
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u16=*((u16 *)Temp_srcbuf);
                    v_u32 = v_u16 << 16;
                     *((u32*)Temp_dstbuf)= swap32_if_need(v_u32, must_swap);
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
    }
}


void  rfbTranslate16toX_old(rfbPixelFormat *src,
                                    rfbPixelFormat *dst,
                                    char *iptr, char *optr,
                                    int bytesBetweenInputLines,
                                    int width, int height)
{

    int i=0,j=0;
    char *srcbuf;
    char *dstbuf;
    char *Temp_srcbuf;
    char *Temp_dstbuf;
    int src_pixelbytes=0;
    int dst_pixelbytes=0;


    u32      v_u32 = 0;
    u16      v_u16 = 0;
    u8       v_u8 = 0;
    u8       must_swap = 0;

    src_pixelbytes=src->bitsPerPixel/8;
    dst_pixelbytes=dst->bitsPerPixel/8;
    srcbuf=iptr;
    dstbuf=optr;
    Temp_srcbuf=srcbuf;
    Temp_dstbuf=dstbuf;


    // showPiexFormat(src);
//    showPixelFormat(dst);

    if(src->bigEndian == dst->bigEndian)
    {
        must_swap = 0;
    }
    else
    {
        must_swap = 1;
    }

    if(dst_pixelbytes == 1)//16 到8的变换
    {
        if(src->trueColour && dst->trueColour)
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u16=*((u16 *)Temp_srcbuf);
                    v_u8 = (((v_u16 >> src->redShift)&(src->redMax))<< dst->redShift)|\
                           (((v_u16 >> src->greenShift)&(src->greenMax))<< dst->greenShift)|\
                           (((v_u16 >> src->blueShift)&(src->blueMax))<< dst->blueShift);

                     *((u8*)Temp_dstbuf)= v_u8;
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
        else//有非真彩色的话直接拷贝吧
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u8 = (*((u16 *)Temp_srcbuf)) >> 8;

                     *((u8*)Temp_dstbuf) = v_u8;
                     Temp_dstbuf += dst_pixelbytes;
                     Temp_srcbuf += src_pixelbytes;
                  }
                  srcbuf += bytesBetweenInputLines;
                  dstbuf += width*dst_pixelbytes;
            }
        }
    }
    if(dst_pixelbytes == 2)//16 到16的变换
    {
        if(src->trueColour && dst->trueColour)
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u16=*((u16 *)Temp_srcbuf);
                    v_u16 = (((v_u16 >> src->redShift)&(src->redMax))<< dst->redShift)|\
                           (((v_u16 >> src->greenShift)&(src->greenMax))<< dst->greenShift)|\
                           (((v_u16 >> src->blueShift)&(src->blueMax))<< dst->blueShift);

                     *((u16*)Temp_dstbuf)= swap16_if_need(v_u16, must_swap);
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
        else//cl是真彩色
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u16=*((u16 *)Temp_srcbuf);
                     *((u16*)Temp_dstbuf)= swap16_if_need(v_u16, must_swap);
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
    }
    if(dst_pixelbytes == 4)//8到32的变换
    {
        if(src->trueColour && dst->trueColour)
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u8=*((u8 *)Temp_srcbuf);
                    v_u32 = (((v_u8 >> src->redShift)&(src->redMax))<< dst->redShift)|\
                           (((v_u8 >> src->greenShift)&(src->greenMax))<< dst->greenShift)|\
                           (((v_u8 >> src->blueShift)&(src->blueMax))<< dst->blueShift);

                     *((u32*)Temp_dstbuf)= swap32_if_need(v_u32, must_swap);
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
        else if(dst->trueColour)//cl是真彩色
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u16=(*((u16 *)Temp_srcbuf)) >> 8;
                    v_u32 = ((v_u16 &(dst->redMax))<< dst->redShift)|\
                            ((v_u16 &(dst->redMax))<< dst->redShift)|\
                            ((v_u16 &(dst->redMax))<< dst->redShift);
                     *((u32*)Temp_dstbuf)= swap32_if_need(v_u32, must_swap);
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
        else
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u16=*((u16 *)Temp_srcbuf);
                    v_u32 = v_u16 << 16;
                     *((u32*)Temp_dstbuf)= swap32_if_need(v_u32, must_swap);
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
    }
}

// ========================================================================
// 函数功能:从一段内存中转换一个矩形图形到所知颜色到相关的buf中(源图像是16的转换)
// 输入参数:
// 输出参数：
// 返回值    :true:发送成功  false：发送失败
// 说明         :主要是进行图像格式的转化，在此不做参数检查，内部使用函数
//       本身应该由足够的内存去容纳，同时有目的地址的颜色只能是8 16 32，暂时不支持24
// ==========================================================================
void  rfbTranslate32toX(rfbPixelFormat *src,
                                    rfbPixelFormat *dst,
                                    char *iptr, char *optr,
                                    int bytesBetweenInputLines,
                                    int width, int height)
{

    int i=0,j=0;
    char *srcbuf;
    char *dstbuf;
    char *Temp_srcbuf;
    char *Temp_dstbuf;
    int src_pixelbytes=0;
    int dst_pixelbytes=0;


    u32      v_u32 = 0;
    u16      v_u16 = 0;
    u8       v_u8 = 0;
    u8       must_swap = 0;

    src_pixelbytes=src->bitsPerPixel/8;
    dst_pixelbytes=dst->bitsPerPixel/8;
    srcbuf=iptr;
    dstbuf=optr;
    Temp_srcbuf=srcbuf;
    Temp_dstbuf=dstbuf;


    // showPiexFormat(src);
 //   showPiexFormat(dst);

    if(src->bigEndian == dst->bigEndian)
    {
        must_swap = 0;
    }
    else
    {
        must_swap = 1;
    }

    if(dst_pixelbytes == 1)//32 到8的变换
    {
        if(src->trueColour && dst->trueColour)
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u32=*((u32 *)Temp_srcbuf);
                    v_u8 = (((v_u32 >> src->redShift)&(src->redMax))<< dst->redShift)|\
                           (((v_u32 >> src->greenShift)&(src->greenMax))<< dst->greenShift)|\
                           (((v_u32 >> src->blueShift)&(src->blueMax))<< dst->blueShift);

                     *((u8*)Temp_dstbuf)= v_u8;
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
        else//有非真彩色的话直接拷贝吧
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u8 = (*((u32 *)Temp_srcbuf)) >> 24;

                     *((u8*)Temp_dstbuf) = v_u8;
                     Temp_dstbuf += dst_pixelbytes;
                     Temp_srcbuf += src_pixelbytes;
                  }
                  srcbuf += bytesBetweenInputLines;
                  dstbuf += width*dst_pixelbytes;
            }
        }
    }
    if(dst_pixelbytes == 2)//16 到16的变换
    {
        if(src->trueColour && dst->trueColour)
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u32=*((u32 *)Temp_srcbuf);
                    v_u16 = (((v_u32 >> src->redShift)&(src->redMax))<< dst->redShift)|\
                           (((v_u32 >> src->greenShift)&(src->greenMax))<< dst->greenShift)|\
                           (((v_u32 >> src->blueShift)&(src->blueMax))<< dst->blueShift);

                     *((u16*)Temp_dstbuf)= swap16_if_need(v_u16, must_swap);
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
        else//cl是真彩色
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u16 = (*((u32 *)Temp_srcbuf)) >> 16;
                     *((u16*)Temp_dstbuf)= swap16_if_need(v_u16, must_swap);
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
    }
    if(dst_pixelbytes == 4)//8到32的变换
    {
        if(src->trueColour && dst->trueColour)
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u32=*((u32 *)Temp_srcbuf);
                    v_u32 = (((v_u8 >> src->redShift)&(src->redMax))<< dst->redShift)|\
                           (((v_u8 >> src->greenShift)&(src->greenMax))<< dst->greenShift)|\
                           (((v_u8 >> src->blueShift)&(src->blueMax))<< dst->blueShift);

                     *((u32*)Temp_dstbuf)= swap32_if_need(v_u32, must_swap);
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
        else
        {
            for(i=0;i<height;i++)
            {
                Temp_srcbuf=srcbuf;
                Temp_dstbuf=dstbuf;
                for(j=0;j<width;j++)
                {
                    v_u32=*((u32 *)Temp_srcbuf);
                     *((u32*)Temp_dstbuf)= swap32_if_need(v_u32, must_swap);
                     Temp_dstbuf+=dst_pixelbytes;
                     Temp_srcbuf+=src_pixelbytes;
                  }
                  srcbuf+=bytesBetweenInputLines;
                  dstbuf+=width*dst_pixelbytes;
            }
        }
    }
}
// =============================================================================
// 函数功能：用于颜色的转换
// 输入参数：len,表示的是颜色的个数，以src为准，不保证dst空间问题，请慎重
// 返回值    ：
// 说明         ：当源和目的相同时，用此种方案，直接拷贝
// =============================================================================
void rfbTranslateNone(rfbPixelFormat *in, rfbPixelFormat *out,
                 char *iptr, char *optr, int bytesBetweenInputLines,
                 int width, int height)
{
    int bytesPerOutputLine = width * (out->bitsPerPixel / 8);


    while (height > 0) {
        memcpy(optr, iptr, bytesPerOutputLine);
        iptr += bytesBetweenInputLines;
        optr += bytesPerOutputLine;
        height--;
    }
}
// =============================================================================
// 函数功能：用于颜色的转换
// 输入参数：len,表示的是颜色的个数，以src为准，不保证dst空间问题，请慎重
// 返回值    ：
// 说明         ：取的是djyos系统的颜色标准，转换的长度以len为准;为了简洁，转换的方式很
//          简单，先转换成RGB24色，再转成用户需要的颜色
// =============================================================================
void vnc_colormap(u16 dst_pftype, u8 *dst, u16 src_pftype, u8 *src , u16 len, \
                    u8 swap)
{
    u32 a =0,c = 0,r,g,b;
    u16 i = 0;
    u32 color;

    for (i = 0; i < len; i++)
    {
        switch(src_pftype)
        {
            case CN_SYS_PF_RGB565:
            {
                color = (u32)(*((u16 *)src));
                src =(u8 *)(src + 2);
                r = (color>>8) & 0xf8;
                g = (color>>3) & 0xfc;
                b = (color<<3) & 0xf8;
            }break;
            case CN_SYS_PF_ARGB1555:
            {
                color = (u32)(*((u16 *)src));
                src =(u8 *)(src + 2);
                a = color>>24;
                r = (color>>8) & 0xf8;
                g = (color>>2) & 0xf8;
                b = (color<<3) & 0xf8;
            }break;
////        case CN_SYS_PF_GRAY1://暂时不处理
//          {
////                color = *((u16 *)src);
////                src =(u8 *)(src + 2);
////                r = color<<7;
////                g = r;
////                b = r;
//          }break;
//          case CN_SYS_PF_GRAY2:
//          {
//              r = color<<6;
//              g = r;
//              b = r;
//          }break;
//          case CN_SYS_PF_GRAY4:
//          {
//              r = color<<4;
//              g = r;
//              b = r;
//          }break;
            case CN_SYS_PF_GRAY8:
            {
                color = (u32)(*((u8 *)src));
                src =(u8 *)(src + 1);
                r = color & 0xff;
                g = r;
                b = r;
            }break;
            case CN_SYS_PF_RGB332:
            {
                color = (u32)(*((u8 *)src));
                src =(u8 *)(src + 1);
                r = color & 0xe0;
                g = (color<<3) & 0xe0;
                b = (color<<6) & 0xc0;
            }break;
            case CN_SYS_PF_RGBE4444:
            {
                color = (u32)(*((u16 *)src));
                src =(u8 *)(src + 2);
                r = (color>>8) & 0xf0;
                g = (color>>4) & 0xf0;
                b = color & 0xf0;
            }break;
            case CN_SYS_PF_ERGB4444:
            {
                color = (u32)(*((u16 *)src));
                src =(u8 *)(src + 2);
                r = (color>>4) & 0xf0;
                g = color & 0xf0;
                b = (color<<4) & 0xf0;
            }break;
            //5551格式很别扭，暂时屏蔽掉，有需要时再根据实际格式增加。
//          case CN_SYS_PF_RGBC5551:/
//          {
//              u32 c = (color & 1)<<2;
//              color = (u32)(*((u16 *)src));
//              src =(u8 *)(src + 2);
//              r = ((color>>8) & 0xf8) + c;
//              g = ((color>>3) & 0xf8) + c;
//              b = ((color<<2) & 0xfc);
//          }break;
            case CN_SYS_PF_ERGB6666:
            {
                u32 c = (color & 1)<<2;
                color = (u32)(*((u32 *)src));
                src =(u8 *)(src + 3);
                r = (color>>10) & 0xfc;
                g = (color>>4) & 0xfc;
                b = (color<<2) & 0xfc;
            }break;
            case CN_SYS_PF_RGB888:
            {
                color = (u32)(*((u32 *)src));
                src =(u8 *)(src + 3);
                r = (color>>16) & 0xFF;
                g = (color>>8) & 0xFF;
                b = (color<<0) & 0xFF;
            }break;
            case CN_SYS_PF_E12RGB666:
            {
                color = (u32)(*((u32 *)src));
                src =(u8 *)(src + 4);
                r = (color>>10) & 0xfc;
                g = (color>>4) & 0xfc;
                b = (color<<2) & 0xfc;
            }break;
            case CN_SYS_PF_RGB101010:
            {
                color = (u32)(*((u32 *)src));
                src =(u8 *)(src + 4);
                r = (color>>22) & 0xff;
                g = (color>>12) & 0xff;
                b = (color>>2) & 0xff;
            }break;
            default:
            {
                a = 0;
                r = 0;
                g = 0;
                b = 0;
            }break;
        }

        switch(dst_pftype)
           {
               case CN_SYS_PF_RGB565:
               {
                   *((u16 *)dst) = ((r>>3)<<11) | ((g>>2)<<5) | (b>>3);
                   dst = (u8 *)(dst + 2);
               }break;
               case CN_SYS_PF_ARGB1555:
               {
                   *((u16 *)dst) = ((a>>7)<<15) | ((r>>3)<<10) | ((g>>3)<<5) | (b>>3);
                   dst = (u8 *)(dst + 2);
               }break;
               //不足8bit的颜色暂时不处理
//             case CN_SYS_PF_GRAY1:
//             {
//                 //根据rgb值计算hls模型的亮度
//                 return (((MAX(MAX(r,g),b)+MIN(MIN(r,g),b))*1) + 255 )/(2*255);
//             }break;
//             case CN_SYS_PF_GRAY2:
//             {
//                 //根据rgb值计算hls模型的亮度
//                 return (((MAX(MAX(r,g),b)+MIN(MIN(r,g),b))*3) + 255 )/(2*255);
//             }break;
//             case CN_SYS_PF_GRAY4:
//             {
//                 //根据rgb值计算hls模型的亮度
//                 return (((MAX(MAX(r,g),b)+MIN(MIN(r,g),b))*15) + 255 )/(2*255);
//             }break;
               case CN_SYS_PF_GRAY8:
               {
                   //根据rgb值计算hls模型的亮度
                   *((u8 *)dst) = (((max(max(r,g),b)+min(min(r,g),b))*255) + 255 )/(2*255);
                   dst = (u8 *)(dst + 1);
               }break;
               case CN_SYS_PF_RGB332:
               {
                   *((u8 *)dst) = ((r>>5)<<5) | ((g>>5)<<2) | (b>>6);
                   dst = (u8 *)(dst + 1);
               }break;
               case CN_SYS_PF_RGBE4444:
               {
                   *((u16 *)dst) =  ((r>>4)<<12) | ((g>>4)<<8) | ((b>>4)<<4);
                   dst = (u8 *)(dst + 2);

               }break;
               case CN_SYS_PF_ERGB4444:
               {
                   *((u16 *)dst) =((r>>4)<<8) | ((g>>4)<<4) | (b>>4);
                   dst = (u8 *)(dst + 2);
               }break;

               case CN_SYS_PF_RGBA5551:    //实际上丢掉了最低位
               {
                   *((u16 *)dst) = ((r>>3)<<11) | ((g>>3)<<6) | ((b>>3)<<1);
                   dst = (u8 *)(dst + 2);
               }break;
               case CN_SYS_PF_ERGB6666:
               {
                   *((u32 *)dst) = ((r>>2)<<12) | ((g>>2)<<6) | (b>>2);
                   dst = (u8 *)(dst + 3);
               }break;
               case CN_SYS_PF_RGB888:
               {
                   *((u8 *)(dst)) = b;
                   *((u8 *)(dst+1)) = g;
                   *((u8 *)(dst+2)) = r;
                   dst = (u8 *)(dst + 3);
               }break;
               case CN_SYS_PF_E12RGB666:
               {
                   *((u32 *)dst) = ((r>>2)<<12) | ((g>>2)<<6) | (b>>2);
                   dst = (u8 *)(dst + 4);
               }break;
               case CN_SYS_PF_RGB101010:
               {
                   *((u32 *)dst) = (r<<20) | (g<<10) | b;
                   dst = (u8 *)(dst + 4);
               }break;
               default:
               {
                   *((u32 *)dst) = color;
                   dst = (u8 *)(dst + 4);
               }
               break;
           }
    }

}


// ========================================================================
// 函数功能:从一段内存中转换一个矩形图形到所知颜色到相关的buf中(源图像是8的转换)
// 输入参数:
// 输出参数：
// 返回值    :true:发送成功  false：发送失败
// 说明         :主要是进行图像格式的转化，在此不做参数检查，内部使用函数
//       本身应该由足够的内存去容纳，同时有目的地址的颜色只能是8 16 32，暂时不支持24
// ==========================================================================
void  rfb_image_colortranslate(u16 dst_pftype, u8 *dst, \
                                u16 src_pftype, u8 *src,\
                                u32 bytesBetweenInputLines,\
                                u32 bytesBetweenOutputLines,\
                                u16 width, u16 height, u8 swap)
{
    u16 i=0;

    for (i = 0; i < height; i++ )
    {
        vnc_colormap(dst_pftype,dst, src_pftype ,src,width,swap);
        src = src + bytesBetweenInputLines;
        dst = dst + bytesBetweenOutputLines;
    }
}

void  rfb_generate_colormaptab(u16 dst_pftype, u8 *dst, u16 src_pftype, u8 swap)
{
    int i=0;

}


void  rfbColorTranslate(rfbPixelFormat *src,
                                    rfbPixelFormat *dst,
                                    char *iptr, char *optr,
                                    int bytesBetweenInputLines,
                                    int width, int height)
{

    u16 i=0,j=0, m = 0;
    char *srcbuf;
    char *dstbuf;
    char *Temp_srcbuf;
    char *Temp_dstbuf;
    int src_pixelbytes=0;
    int dst_pixelbytes=0;


    u32      v_u32 = 0;
    u16      v_u16 = 0;
    u8       v_u8 = 0;
    u8       must_swap = 0;
    u8       r,g,b;
    u32      color = 0;

    src_pixelbytes=src->bitsPerPixel/8;
    dst_pixelbytes=dst->bitsPerPixel/8;
    srcbuf=iptr;
    dstbuf=optr;
    Temp_srcbuf=srcbuf;
    Temp_dstbuf=dstbuf;

    if(src->bigEndian == dst->bigEndian)
    {
        must_swap = 0;
    }
    else
    {
        must_swap = 1;
    }

    for(i=0;i<height;i++)
    {
        Temp_srcbuf=srcbuf;
        Temp_dstbuf=dstbuf;
        for(j=0;j<width;j++)
        {
            color = 0;
            for(m = 0; m < src_pixelbytes; m++)//获取像素值，至少是字节为单位
            {
                color = ((*(Temp_srcbuf + m))<<(i<<0x03))|color;
            }
            if(src->trueColour)
            {
                r = ((color>>src->redShift)&(src->redMax))*256/(src->redMax);
                g = ((color>>src->greenShift)&(src->greenMax))*256/(src->greenMax);
                b = ((color>>src->blueShift)&(src->blueMax))*256/(src->blueMax);
            }
            else//这种情况，目前应该只是256灰度色
            {
                if(src_pixelbytes != 1)
                {
                    return;
                }
                else//我们知道这种情况是颜色的映射，映射成332模式，应为映射表给的就是232
                {
                    r = color;
                    g = color;
                    b = color;
                }
            }
            color = 0;
            if(dst->trueColour)//真彩色，很好处理
            {
                r = (r*dst->redMax)/256;
                g = (g*dst->greenMax)/256;
                b = (b*dst->blueMax)/256;
                color = (r<<(dst->redShift))|(g<< dst->greenShift)|(b << dst->blueShift);
            }
            else//这种情况，目前应该只处理成8位色
            {
                if(dst_pixelbytes != 1)
                {
                    return;
                }
                else//我们知道这种情况是颜色的映射，映射成332模式，应为映射表给的就是232
                {
                    r = (r*8)/256;
                    g = (g*8)/256;
                    b = (b*8)/256;
                    color = (r<<5)|(g<<2)|b;
                }
            }

            for(m = 0; m < dst_pixelbytes; m++)//拷贝像素值，至少是字节为单位
            {
                *(Temp_dstbuf+m) = (color>>(m<<0x03))&0xFF;
            }

             Temp_dstbuf+=dst_pixelbytes;
             Temp_srcbuf+=src_pixelbytes;
          }
          srcbuf+=bytesBetweenInputLines;
          dstbuf+=width*dst_pixelbytes;
    }
}



