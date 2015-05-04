
#include "vnc_mirror_display.h"
#include "clip_process.h"
#include "vnc_thread.h"
#include "debug.h"
static rfbScreenInfoPtr vncscreen;
//存放的是vncserver的screen，所以应该保证在
//screen消失（退出）前删除mirrordisplay，否则内存出错

int comp_two_fram_buf(unsigned char* buf1, unsigned char* buf2, int count)
{
    int result = 0;
    int i=0;
    for(i=0;i<count;i++)
    {
        if(*(buf1+i)!=*(buf2+i))
        {
            debug_printf("there must be somthing wrong!---byte=%d\n",i);
            return -1;
         }
    }
    debug_printf("mem ok!---i=%d bytes\n",i);
    return 0;
}


// =========================================================================
// 函数功能:将一个bm图形输出到VNC的显示器中
// 输入参数:src_bitmap,需要输出的图像，dst_rect,图像中需要输出的部分
// 输出参数：
// 返回值  :true：输出成功；  false：输出失败
// 说明    :bitmap可以很大，但是mirrordisplay确不是一定能够容纳的下。
// =========================================================================
bool_t vnc_bm_to_screen(struct tagRectangle *dst_rect,\
                        struct tagRectBitmap *src_bitmap,s32 xsrc,s32 ysrc)
{
//为了方便使用clip的相关函数，使用DJYOS的clip结构。在发送数据的时候自己再转换
//先进行矩形框的整理，分为三个步骤
//1,先将dst_rect和screen的clip做减法处理，分为相交ints和different
//2,将ints和different做链接
//3,将screen的clip做合并处理
    struct tagClipRect *newclip;//从rect转化为clip模式
    bool_t ret=true;
    u8    bytesPP;//每个像素所占用的字节数
    u32    i,x =0, y = 0, w =0, h =0;
    u8     *winbuf,*framebuf;
    u32    dst_linebytes;

    if(vncscreen == NULL)
    {
        return false;
    }
    if(0 == vncscreen->mirror_display_enable)
    {
        return true;
    }
    //看看新的矩形是不是合法，我们相信一般的是合法的
    //简单的判断一下，看看
    if((dst_rect->left>dst_rect->right)||(dst_rect->top>dst_rect->bottom))
    {
        ret=false;
        return ret;
    }
    //8位以下的暂时不支持
    bytesPP = vncscreen->rfbServerFormat.bitsPerPixel/8;
    if(bytesPP==0)
    {
        ret=false;
        return ret;

    }
   // debug_printf("---bitmmap color= %d\n", src_bitmap->pf_type >>8);
    //要对写入的地址进行处理

    //如果要写的起始点已经超出Lcd，那么直接不处，默认的(0 0 width heitht)
    if(!((dst_rect->top >=0)&&(dst_rect->top <= vncscreen->height)&&\
            (dst_rect->left >= 0)&&(dst_rect->left <= vncscreen->width)))
    {
        return false;
    }
        //根据dst_rect创建新的clip
    newclip = get_vncclip_space();
    if(NULL==newclip)
    {
        ret=false;
        return ret;
    }
    newclip->next=newclip;
    newclip->previous=newclip;
    newclip->rect.top=dst_rect->top;
    newclip->rect.left=dst_rect->left;
    //要copy的区域已经超出LCD，取合适的即可
    if(dst_rect->right > vncscreen->width)
    {
        newclip->rect.right = vncscreen->width - 1;
    }
    else
    {
        newclip->rect.right = dst_rect->right;
    }

    if(dst_rect->bottom >=(vncscreen->height - cn_vnc_keyboard_height))
    {
        newclip->rect.bottom = vncscreen->height - cn_vnc_keyboard_height -1;
    }
    else
    {
        newclip->rect.bottom= dst_rect->bottom;
    }

    x = newclip->rect.left;
    y = newclip->rect.top;
    w = newclip->rect.right - newclip->rect.left;
    h = newclip->rect.bottom - newclip->rect.top;
    dst_linebytes=w*bytesPP;
    //开始进行screen的队列修改，要进行互斥资源的保护
    XMutexWait(&vncscreen->frame_mutex);

    //方法是很简单，用新建的clip去减原队列，得到different 和ints，则different
    //和新建的clip的和就是所需的clip队列

    anastomose_vncclip(&(vncscreen->clip_head), newclip);


    if(!check_clip_queque(vncscreen->clip_head))
    {
        debug_printf("@the que is not OK before sort!\n");
    }
    vncscreen->clip_head = combine_vnccliplink(vncscreen->clip_head);
    if(!check_clip_queque(vncscreen->clip_head))
    {
        debug_printf("@the que is not OK after sort!\n");
    }

    if(NULL == vncscreen->clip_head)
    {
      debug_printf("----镜像显示器的clip竟然为空，不可能的事情---\n");
    }

//解决了矩形框的问题，现在开始拷贝数据，注意的是，mirror的像素格式和主显示器的是一样的
//暂时不支持8bit以下的像素格式
//公用一个基础显示器，不必数据拷贝

    winbuf=src_bitmap->bm_bits+y*(src_bitmap->linebytes)+x*bytesPP;
    framebuf=vncscreen->frameBuffer+y*vncscreen->paddedWidthInBytes+x*bytesPP;
    //以行为单位进行拷贝，只需要拷贝DST_RECT部分啊
    for(i=0;i<h;i++)
    {
       memcpy(framebuf,winbuf,w*bytesPP);
       framebuf+=vncscreen->paddedWidthInBytes;
       winbuf+=src_bitmap->linebytes;
    }
//激活update事件，主要是设置其优先级,和本事件的优先级是一样的
//用激活事件行不通：
//1，如果update等待eventpop，则会导致softkey的更改不及时
//2，如果不等待eventpop，则pop优先级只会更改独立型事件的优先级，不会更改关联型
//3，综上所述，我们将此事的优先级存起来，让update自己更新吧
//    Djy_EventPop(vncscreen->event_id_update,0,NULL,0,0,0,djy_get_event_prio());
//    vncscreen->event_update_prio = djy_get_event_prio();

    XMutexPost(&vncscreen->frame_mutex);
    ret=true;
    return ret;
}

// =============================================================================
// 函数功能:安装镜像显示器
// 输入参数:screen，rfbserver的显示屏
// 输出参数：
// 返回值  :true:成功  false：失败
// 说明    :在安装镜像显示器的时候，我们会获取主显示器的一些参数，借此可以初始化我们的
//          screen的参数，比方说尺寸以及像素格式，以及screen的buf的申请，同时我们也
//        要有自己的clippool，避免和底层的clip有说不清的关系
// =============================================================================
#define  cn_limit_vncclip          100//max menuitem num
static struct tagClipRect            s_vncclip_rsc[cn_limit_vncclip];
struct tagMemCellPool  *vncclip_pool;
bool_t install_vncserver_mirrordisplay(rfbScreenInfoPtr rfbserver)
{
    bool_t  ret=false;
    struct tagDisplayRsc *base_display;
    struct tagGkWinRsc *desktop;
    struct tagDisplayRsc *mirror_display;//我们的镜像显示器

    if(NULL==rfbserver)
    {
        return ret;
    }

    //通过桌面找到显示器，这种方法有点out ---TODO
    desktop = GK_ApiGetDesktop("sim_display");

    if(NULL==desktop)//找不到桌面。
    {
        return ret;
    }
    base_display=desktop->disp;
    if(NULL==base_display)//找不到主显示器
    {
        return ret;
    }
    if((base_display->pixel_format>>8)<8)//暂时不支持8位像素格式
    {
        return ret;
    }
    vncclip_pool = Mb_CreatePool((void*)s_vncclip_rsc,
                                     cn_limit_vncclip,
                                     sizeof(struct tagClipRect),
                                     20,100,
                                     "vncclip_pool");
    if(NULL == vncclip_pool)
    {
        debug_printf("#fatalerror--create the vncclip_pool failed!\n");
        return ret;
    }

    mirror_display=M_MallocLc(sizeof(struct tagDisplayRsc),0);
    if(NULL==mirror_display)
    {
        debug_printf("#fatalerror--malloc mirror display mem failed!\n");
        Mb_DeletePool(vncclip_pool);
        return ret;
    }
    memset(mirror_display,0,sizeof(struct tagDisplayRsc));
    //初始化镜像显示器的一些必备属性，跟主显示器保持一致--TODO，

    mirror_display->pixel_format = base_display->pixel_format;
    mirror_display->width = base_display->width;
    if(cn_vnc_keyboard_visual_enable)
    {
        mirror_display->height = base_display->height + cn_vnc_keyboard_height;
    }
    else
    {
        mirror_display->height = base_display->height;
    }
    mirror_display->frame_buffer = NULL;
    mirror_display->draw.bm_to_screen=vnc_bm_to_screen;

    ret= GK_InstallDisplayMirror(base_display,mirror_display,"vnc_server_disp");

    if(ret)//安装成功
    {   //区域设置
        rfbserver->width=mirror_display->width;
        rfbserver->height=mirror_display->height;
        rfbserver->rfbServerFormat.bitsPerPixel=(mirror_display->pixel_format>>8);
        rfbserver->frameBuffer=M_MallocLc(rfbserver->width*\
                    (rfbserver->height)*(rfbserver->rfbServerFormat.bitsPerPixel/8),0);

        if(NULL==rfbserver->frameBuffer)
        {
            Rsc_DelNode((struct tagRscNode *)mirror_display);
            free(mirror_display);
            Mb_DeletePool(vncclip_pool);
            debug_printf("#fatalerror--malloc rfbserver frame buf failed!\n");

            return ret;
        }
//镜像显示器和其基础显示器公用一个buf。
        //初始化一些rfbserver的参数
        debug_printf("fram buf = %08x   %d bytes\n",rfbserver->frameBuffer,\
           rfbserver->width*(rfbserver->height)*(rfbserver->rfbServerFormat.bitsPerPixel/8));
        memset(rfbserver->frameBuffer,0,rfbserver->width*(rfbserver->height)*(rfbserver->rfbServerFormat.bitsPerPixel/8));
        rfbserver->clip_head=NULL;

        //像素格式
     //   rfbserver->rfbServerFormat.bitsPerPixel=rfbserver->bitsPerPixel;
        //感觉rfbScreen对于色彩的设置有点冗余，可以考虑只用format就好了，后续修正--TODO
        rfbserver->rfbServerFormat.bigEndian=false;
        rfbserver->rfbServerFormat.trueColour=true;
        rfbserver->paddedWidthInBytes=(rfbserver->rfbServerFormat.bitsPerPixel/8)*rfbserver->width;
        switch(rfbserver->rfbServerFormat.bitsPerPixel)//目前就列举这几种吧，后续的再行补充
        {
            case 8://332格式
                debug_printf("the server vmirror_display used 332 format!\n");
                rfbserver->rfbServerFormat.redShift=5;
                rfbserver->rfbServerFormat.greenShift=2;
                rfbserver->rfbServerFormat.blueShift=0;
                rfbserver->rfbServerFormat.redMax=((1<<3)-1);
                rfbserver->rfbServerFormat.greenMax=((1<<3)-1);
                rfbserver->rfbServerFormat.blueMax=((1<<2)-1);
                rfbserver->rfbServerFormat.depth=8;
                break;
            case 16://565格式
                debug_printf("the server vmirror_display used 565 format!\n");
                rfbserver->rfbServerFormat.redShift=11;
                rfbserver->rfbServerFormat.greenShift=5;
                rfbserver->rfbServerFormat.blueShift=0;
                rfbserver->rfbServerFormat.redMax=((1<<5)-1);
                rfbserver->rfbServerFormat.greenMax=((1<<6)-1);
                rfbserver->rfbServerFormat.blueMax=((1<<5)-1);
                rfbserver->rfbServerFormat.depth=16;
                break;
            case 32://真彩色888
                debug_printf("the server vmirror_display used 888 format!\n");
                rfbserver->rfbServerFormat.redShift=16;
                rfbserver->rfbServerFormat.greenShift=8;
                rfbserver->rfbServerFormat.blueShift=0;
                rfbserver->rfbServerFormat.redMax=((1<<8)-1);
                rfbserver->rfbServerFormat.greenMax=((1<<8)-1);
                rfbserver->rfbServerFormat.blueMax=((1<<8)-1);
                rfbserver->rfbServerFormat.depth=24;
                break;
            default://不应该处理，后果很严重，后虚修补bug--TODO
                break;
        }

        vncscreen=rfbserver;
        vnc_set_vncclip_pool(vncclip_pool);
        rfbserver->mirror_display = mirror_display;
        return ret;
    }
    else
    {
        Mb_DeletePool(vncclip_pool);
        free(mirror_display);
        debug_printf("#fatalerror--install the mirror display failed!\n");

        return ret;
    }
}

// =============================================================================
// 函数功能:卸载显示器
// 输入参数:rfbserver，rfbserver的显示屏
// 输出参数：
// 返回值:true:成功  false：失败
// 说明:在安装镜像显示器的时候，我们会获取主显示器的一些参数，借此可以初始化我们的
//          screen的参数，比方说尺寸以及像素格式，以及screen的buf的申请，同时我们也
//        要有自己的clippool，避免和底层的clip有说不清的关系
// =============================================================================
void  uninstall_vncserver_mirrrordisplay(rfbScreenInfoPtr rfbserver)
{
    XMutexWait(&(rfbserver->frame_mutex));

    free_vnccliplink_space(rfbserver->clip_head);
    Mb_DeletePool(vncclip_pool);
    free(rfbserver->frameBuffer);
    Rsc_DelNode((struct tagRscNode *)(rfbserver->mirror_display));
    free(rfbserver->mirror_display);

    rfbserver->clip_head = NULL;
    rfbserver->frameBuffer = NULL;
    rfbserver->mirror_display = NULL;
    vnc_set_vncclip_pool(NULL);
    vncscreen = NULL;

    XMutexPost(&(rfbserver->frame_mutex));
}


