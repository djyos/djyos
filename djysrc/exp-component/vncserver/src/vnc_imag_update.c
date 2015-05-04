// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: VNCServer
// 模块版本: V1.00(初始化版本)
// 文件名     ：frameupdate.c
// 创建人员: Administrator
// 创建时间: 2013-3-8
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <V1.00> <2013-3-8>, <修改人员>: <修改功能概述>
// =============================================================================

#include "vnc_image_update.h"
#include "vnc_socket.h"
#include "rfb.h"
#include "debug.h"

#include "vnc_thread.h"
#include "vnc_mirror_display.h"
#include "clip_process.h"
#include "vnc_keyboard.h"
#include "color_remap.h"
// =============================================================================
// 函数功能:判断是否该发送数据
// 输入参数:待判断的cl
// 输出参数：
// 返回值  :true:可以发送  false：没有必要发送
// 说明    :当客户端有过请求（证明准备好接受数据）
// =============================================================================
bool_t  ready_to_send_frame(rfbClientPtr cl)
{
    if((cl->HasRequest == 1)&&(cl->client_dirty == 0 ))
    {
        return true;
    }
    else
    {
        return false;
    }
}
// =============================================================================
// 函数功能:将screen的clbuffer数据刷新给cl，长度为ublen
// 输入参数:待发送的cl
// 输出参数：
// 返回    :true:发送成功  false：发送失败
// 说明    :内部使用，不做参数检查
// =============================================================================
bool_t rfbSendUpdateBuf(rfbClientPtr cl)
{
    if (WriteExact(cl, cl->screen->cl_buffer, cl->screen->ublen) < 0)
    {
        debug_printf("#error:rfbSendUpdateBuf: write\n");
        return false;
    }

    cl->screen->ublen = 0;
    return true;
}

// =============================================================================
// 函数功能:判断一个clip是不是该client的感兴趣的区域
// 输入参数:cl，客户端， clip 待判断的clip
// 输出参数：
// 返回值 :true:感兴趣  false：不感兴趣
// 说明:判断该clip和cl感兴趣区域是不是有交集
// =============================================================================
static  bool_t vnc_check_clip_in_client(rfbClientPtr cl, struct tagClipRect *clip)
{
//此判据比较简单，仅仅靠判断起始点，实际上应该看看整个clip是否有交集--TODO
//	默认的是相交的情况
	u8 h_crossflag = 1;
	u8 v_crossflag = 1;
	if((((u16)clip->rect.left)> (cl->modifiedregion.x + cl->modifiedregion.w))||\
			(((u16)clip->rect.right) < cl->modifiedregion.x)) //判断水平是否交集
	{
		h_crossflag = 0;
	}
	else
	{
		h_crossflag = 1;
	}

	if((((u16)clip->rect.top)> (cl->modifiedregion.y + cl->modifiedregion.h))||\
			(((u16)clip->rect.bottom) < cl->modifiedregion.y)) //判断垂直是否交集
	{
		v_crossflag = 0;
	}
	else
	{
		v_crossflag = 1;
	}
	if((h_crossflag)&(v_crossflag))
	{
		return true;
	}
	else
	{
		return false;
	}

}
// =============================================================================
// 函数功能:根据cl的感兴趣的区域，调整该clip，主要是将超出感兴趣的部分消除
// 输入参数:cl，客户端， clip 待判断的clip
// 输出参数:rec,存储调整好的位置
// 返回值 :
// 说明:如果不需要调整，则仅仅存储原来的矩形即可
// =============================================================================
static void vnc_adjust_clip_in_client(rfbClientPtr cl,u16 x, \
		u16 y, u16 w, u16 h,rfbRectangle *rec)
{
	if(x < (cl->modifiedregion.x))
	{
		rec->x = cl->modifiedregion.x;
	}
	else
	{
		rec->x = x;
	}
	if(y < (cl->modifiedregion.y))
	{
		rec->y = cl->modifiedregion.y;
	}
	else
	{
		rec->y = y;
	}


	if( (x + w) >(cl->modifiedregion.x + cl->modifiedregion.w))
	{
		rec->w = cl->modifiedregion.x + cl->modifiedregion.w - x;
	}
	else
	{
		rec->w = w;
	}
	if( (y + h) >(cl->modifiedregion.y + cl->modifiedregion.h))
	{
		rec->h = cl->modifiedregion.y + cl->modifiedregion.h - x;
	}
	else
	{
		rec->h = h;
	}
}
// =============================================================================
// 函数功能:判断cip队列在该cl的感兴趣的区域有多少个
// 输入参数:cl，客户端
// 输出参数：
// 返回值 ：该server在cl区域范围有多少clip
// 说明:
// =============================================================================
static  u16 vnc_check_clipnumbers_in_client(rfbClientPtr cl)
{
	u16 ret = 0;
	struct tagClipRect * pclip;

	pclip = cl->screen->clip_head;
	if(NULL == pclip)
	{
		return ret;
	}
	do{
		if(vnc_check_clip_in_client(cl,pclip))
		{
			ret++;
		}
		pclip = pclip->next;
	}while(cl->screen->clip_head != pclip );

	return ret;
}


// =============================================================================
// 函数功能:将一个矩形框以raw(源码，非加密)的形式发送给cl
// 输入参数:待发送的cl,x,y,w,h,这个screen的图像窗口位置信息，
// 输出参数：
// 返回值  :true:发送成功  false：发送失败
// 说明    :screen和cl的像素格式等的转换在此进行，第一次发送的时候clbuffer中已经有了
//          图像更新的信息
// =============================================================================
bool_t  send_rect_enraw(rfbClientPtr cl, int x, int y, int w, int h)
{
    rfbFramebufferUpdateRectHeader rect;
    int nlines;
    int bytesPerLine = w * (cl->format.bitsPerPixel / 8);
    char *fbptr = (cl->screen->frameBuffer + (cl->screen->paddedWidthInBytes * y)
          	         + (x * (cl->screen->rfbServerFormat.bitsPerPixel / 8)));
    if (cl->screen->ublen + sz_rfbFramebufferUpdateRectHeader > CL_BUFF_SIZE)
    {
        if (!rfbSendUpdateBuf(cl))
            return false;
    }

 //根据cl对clip做出调整，防止超出cl的感兴趣区域
    vnc_adjust_clip_in_client(cl, x, y, w, h,&(rect.r));
    rect.r.x = Swap16IfLE(rect.r.x);
    rect.r.y = Swap16IfLE(rect.r.y);
    rect.r.w = Swap16IfLE(rect.r.w);
    rect.r.h = Swap16IfLE(rect.r.h);
    rect.encoding = Swap32IfLE(rfbEncodingRaw);

//    拷贝矩形头信息
    memcpy(&(cl->screen->cl_buffer[cl->screen->ublen]), \
    		(char *)&rect,sz_rfbFramebufferUpdateRectHeader);
    cl->screen->ublen += sz_rfbFramebufferUpdateRectHeader;
    nlines = (CL_BUFF_SIZE - cl->screen->ublen) / bytesPerLine;

    while (true)
    {
        if (nlines > h)
            nlines = h;

        if(NULL != cl->translateFn)
        {
        	(cl->translateFn)(&(cl->screen->rfbServerFormat),\
             &cl->format, fbptr, &(cl->screen->cl_buffer[cl->screen->ublen]),\
             cl->screen->paddedWidthInBytes, w, nlines);
        }

        cl->screen->ublen += nlines * bytesPerLine;
        h -= nlines;
//每次都发送CL_BUFF_SIZE个数据，更好的利用网络
        if (h == 0)
            return true;

        if (!rfbSendUpdateBuf(cl))
        {
             return false;
        }
        fbptr += (cl->screen->paddedWidthInBytes * nlines);

        nlines = (CL_BUFF_SIZE - cl->screen->ublen) / bytesPerLine;
        if (nlines == 0) 
        {//一行的数据都发送不了，这个真心是受不了的
//          debug_printf("send buffer  for %d  bytes per line\n", bytesPerLine);
            return false;
        }
    }//end while
}//end send_rect_enraw
// =============================================================================
// 函数功能:将整幅图像都更新给cl客户端
// 输入参数:客户端cl
// 输出参数 ：
// 返回值   :true,更新成功，false更新失败
// 说明     :内部使用函数，不在做参数之类的安全监察
// =============================================================================
static bool_t  SendFrambuf2cl(rfbClientPtr cl)
{
	int clip_numbers=0;
	rfbScreenInfoPtr screen;
	rfbFramebufferUpdateMsg *Msg;
	bool_t ret=true;

	//cl要可用才行啊
	if(cl->client_dirty)
	{
		return ret;
	}

	clip_numbers=0x01;
	screen=cl->screen;
	screen->ublen=0;
	Msg=(rfbFramebufferUpdateMsg *)&(screen->cl_buffer[screen->ublen]);
	Msg->type=rfbFramebufferUpdate;
	Msg->nRects=Swap16IfLE(clip_numbers);//发送的是整个buffer
	Msg->pad=0xFF;


	screen->ublen+=sz_rfbFramebufferUpdateMsg;
//	ret=send_rect_enraw(cl,0,0,screen->width,screen->height);
	ret = send_rect_enraw(cl, cl->modifiedregion.x, cl->modifiedregion.y,\
			cl->modifiedregion.w, cl->modifiedregion.h);
	if(ret==false)
	{
	  return ret;
	 // rfbCloseClient(cl);//在更新程序中具体的做这样的事情
	}
	ret=rfbSendUpdateBuf(cl);

      cl->InitDone=TRUE;
    //  debug_printf("send the whole buf data end!\n");
    return ret;
}

// =============================================================================
// 函数功能:将贞缓冲中的更新clip更新给cl客户端
// 输入参数:客户端cl
// 输出参数：
// 返回值  :true,更新成功，false更新失败
// 说明    :内部使用函数，不在做参数之类的安全监察
// =============================================================================
static bool_t  SendFramClip2cl(rfbClientPtr cl)
{
	int clip_numbers=0;
	int  x,y,w,h,i;
	rfbScreenInfoPtr screen;
	rfbFramebufferUpdateMsg *Msg;
	struct tagClipRect *clip;
	bool_t ret=true;

//cl要可用才行啊
	if(cl->client_dirty)
	{
		return ret;
	}

	screen=cl->screen;
	screen->ublen=0;
    clip_numbers=vnc_check_clipnumbers_in_client(cl);
	if(0 == clip_numbers)
	{
		return ret;
	}
	Msg=(rfbFramebufferUpdateMsg *)&(screen->cl_buffer[screen->ublen]);
	Msg->type=rfbFramebufferUpdate;
	Msg->nRects=Swap16IfLE(clip_numbers);
	Msg->pad=0xFF;
	screen->ublen+=sz_rfbFramebufferUpdateMsg;
	clip=screen->clip_head;

  //  debug_printf("clip_numbers= %d\n",clip_numbers);
	for(i=0;i<clip_numbers;i++)//遍历所有的clip,依次发送出去
	{
		//如果该clip不是感兴趣区域，那么不用发送的
		if(vnc_check_clip_in_client(cl,clip))
		{
			x = clip->rect.left;
			y = clip->rect.top;
			w = clip->rect.right-clip->rect.left;
		    h = clip->rect.bottom-clip->rect.top;
		    ret = send_rect_enraw(cl,x,y,w,h);
		    if(ret==false)
		    {
		    	return ret;
		    }
		}
	    clip = clip->next;
	}
	ret=rfbSendUpdateBuf(cl);
	return ret ;
}

// =============================================================================
// 函数功能:将新的key的视觉发送给客户端
// 输入参数:客户端cl
// 输出参数：
// 返回值  :true 发送成功 false 失败(一定是因为写socket失败)
// 说明    :内部使用函数，不在做参数之类的安全监察
// =============================================================================
static bool_t  sendkeylist2cl(rfbClientPtr cl)
{
	u16 key_num=0;
	u16 key_num_bak = 0;
	u8  key_number = 0, i =0;
	u16  x,y,w,h;
	rfbScreenInfoPtr screen;
	rfbFramebufferUpdateMsg *Msg;
//	struct tagClipRect *clip;
	bool_t ret=true;

//cl要可用才行啊
	if(cl->client_dirty)
	{
		return ret;
	}

	screen=cl->screen;
	screen->ublen=0;
	key_num=vnc_get_keychanged_numbers(screen);
	if(0 == key_num)
	{
		return ret;
	}
	Msg=(rfbFramebufferUpdateMsg *)&(screen->cl_buffer[screen->ublen]);
	Msg->type=rfbFramebufferUpdate;
	Msg->nRects=Swap16IfLE(key_num);
	Msg->pad=0xFF;
	screen->ublen+=sz_rfbFramebufferUpdateMsg;
	for(i=0; i<cn_vnc_key_num; i++)//遍历所有的key
	{
		if(vnc_check_key_changed(screen,i))//该键发生了变化
		{
			vnc_get_keyposition_by_number(screen, i, &x, &y);
			w = cn_vnc_key_width;
		    h = cn_vnc_key_height;
		    ret = send_rect_enraw(cl,x,y,w,h);
		    if(ret == false)
		    {
		    	return ret;
		    }
		    key_num_bak++;
		}
		if(key_num_bak == key_num)
		{
			break;//发送够了，要退出去
		}
	}
	ret=rfbSendUpdateBuf(cl);
	return ret ;
}

// =============================================================================
// 函数功能:为cl队列更新数据
// 输入参数:vncserver的screen
// 输出参数：
// 返回值  :true,更新成功，false，有cl没有更新成功，会将这样的cl删除的
// 说明    :更新数据，包括更新Image，还有软键盘、信号灯等
// =============================================================================
bool_t UpdateFrame2Clist(rfbScreenInfoPtr screen)
{
	   rfbClientPtr cl = NULL, cl_next = NULL;
       int clip_numbers=0; 
       bool_t ret=true;
       bool_t bNoClip=false;

//和镜像显示器竞争使用frambuf资源
       XMutexWait(&(screen->frame_mutex));
       cl=screen->rfbClientHead;
       
       if(cl)
       {
    	   vnc_changed_keyboard_visual(screen);//在此刷新keyvisual到buf
       }
       while(cl)//便利所有的cl，进行更新
       {
    	   if(ready_to_send_frame(cl))//可以发送数据，好吧，那就发送吧
    	   {
    		   screen->ublen = 0;
    		   cl_next=cl->next;
 //cl有被删除的危险，在删除之前还是先备份cl->next吧
//如果是第一次，那么还要用初始化，不仅仅发送我们的镜像显示器,
//还有可能是我们的软件盘、信号灯之类的，
    		   if(!cl->InitDone)
    		   {
     			   ret=SendFrambuf2cl(cl);
     			   if(false==ret)
     			   {
     				   cl->client_dirty = 1;
     			   }                
               }
    		   else//发送更新的数据
    		   {
                 //发送信号灯更新数据
                 //发送按键更新数据
#if  CN_CFG_KEYBOARD ==1
    			   ret = sendkeylist2cl(cl);
    			   if(false==ret)
    			   {
      				  //由端口监视程序负责处理cl
      				   cl->client_dirty = 1;
    			   }
#endif
                 //循环发送clip数据
    			   ret=SendFramClip2cl(cl);
    			   if(false==ret)
    			   {
      				  //由端口监视程序负责处理cl
      				   cl->client_dirty = 1;
    			   }

    		   }//发送数据形式（整幅图形or递增形式）
          }//可以发送数据
          cl=cl_next;

      }//while结束

       //清除 key队列
       vnc_reset_keyboard_statelist(screen);
      //清空clip队列
      if(NULL!=screen->clip_head)
      {
    	  free_vnccliplink_space(screen->clip_head);
    	  screen->clip_head = NULL;
      }
      //释放frame资源
      XMutexPost(&(screen->frame_mutex));
      return ret;
}






