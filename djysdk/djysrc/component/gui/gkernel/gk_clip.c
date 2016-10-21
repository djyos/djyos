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
//文件描述: gui kernel剪切域管理部分
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2010-04-07
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "string.h"
#include "pool.h"
#include "gkernel.h"
#include <gui/gkernel/gk_display.h>
#include "gk_win.h"
#include "gk_clip.h"
#include "gk_draw.h"
struct MemCellPool *g_ptClipRectPool;   //剪切域内存池

//----释放clip队列-------------------------------------------------------------
//功能: 释放一个clip队列的所有节点，该队列是一个双向循环队列。
//参数: clip，指向待释放队列的一个节点的指针
//返回: 无
//-----------------------------------------------------------------------------
struct ClipRect *__GK_FreeClipQueue(struct ClipRect *clip)
{
    struct ClipRect *temp,*temp1;
    if(clip != NULL)
    {
        temp = clip;
        do
        {
            temp1 = temp->next;
            Mb_Free(g_ptClipRectPool,temp);
            temp = temp1;
        }while(temp != clip);
    }
    return NULL;
}

//----合并区域简化版-----------------------------------------------------------
//功能: 把给定的剪切域队列中可合并的区域合并，简化版的意思是，队列中的区域已经按
//      左->右，从上->下的顺序排列，合并后，多余的clip将被释放。
//参数: clipq: 指向待合并的队列首
//返回: 无
//-----------------------------------------------------------------------------
struct ClipRect * __GK_CombineClip_s(struct ClipRect *clipq)
{
    struct ClipRect *clip,*clip1,*tempclip;
    if(clipq == NULL)
        return NULL;
    //合并剪切域
    clip = clipq;
    clip1 = clipq->next;
    //执行水平合并,由于clip是按照先横后纵的方式划分的，因此水平合并只需要查
    //看相邻矩形是否可以合并即可
    while(clip1 != clipq)
    {
        if((clip1->rect.top == clip->rect.top)
            &&(clip1->rect.bottom == clip->rect.bottom)
            &&(clip1->rect.left == clip->rect.right)) //可合并
        {
            clip->rect.right = clip1->rect.right;
            clip->next = clip1->next;
            clip1->next->previous = clip;
            Mb_Free(g_ptClipRectPool,clip1);
            clip1 = clip->next;
        }
        else if((clip1->rect.top == clip->rect.top) //存在剪切域相同的情况
            &&(clip1->rect.bottom == clip->rect.bottom)
            &&(clip1->rect.left == clip->rect.left)
            &&(clip1->rect.right== clip->rect.right))
        {
            clip->next = clip1->next;
            clip1->next->previous = clip;
            Mb_Free(g_ptClipRectPool,clip1);
            clip1 = clip->next;
        }else
        {
            clip = clip1;
            clip1 = clip1->next;
        }
    }
    //执行垂直合并，可合并的矩形可能不相邻，故需两重循环才能完成。
    clip = clipq;
    do{
        clip1 = clip->next;
        while(clip1 != clip)
        {
            if((clip1->rect.left == clip->rect.left)
                &&(clip1->rect.right == clip->rect.right)
                &&(clip->rect.bottom == clip1->rect.top)) //可合并
            {
                clip->rect.bottom = clip1->rect.bottom;
                clip1->previous->next = clip1->next;
                clip1->next->previous = clip1->previous;
                tempclip = clip1;
                clip1 = clip1->next;
                Mb_Free(g_ptClipRectPool,tempclip);
            }else
                clip1 = clip1->next;
        }
        clip = clip->next;
    }while(clip != clipq);
    return clipq;
}



//----合并区域通用版-----------------------------------------------------------
//功能: 把给定的剪切域队列中可合并的区域合并，与简化版不同的是，队列中的clip是
//      随机排列的。方法是:
//      1、把所有clip按先y后x坐标排序，使队列变成左→右，再上→下排序的。
//      2、调用__gk_combine_clip_s函数进行排序。
//参数: clipq: 指向待合并的队列首
//返回: 返回的是排好顺序的队首
//-----------------------------------------------------------------------------
//is ClipA bottomer and righter than B? 1 yes 0 no
//水平位置有更高的权重比
static u8 __ClipAIsBR2ClipB(struct ClipRect *ClipA, struct ClipRect *ClipB)
{
    if((ClipA->rect.top > ClipB->rect.top) || \
    ((ClipA->rect.top == ClipB->rect.top) && (ClipA->rect.left > ClipB->rect.left)))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
struct ClipRect * __GK_CombineClip(struct ClipRect *clipq)
{

    struct ClipRect  *startclip, *endclip,  *maxclip;
    struct ClipRect  *clippop, *clippopnext, *clippop_pbak, *clippopnext_nbak;
    int    firstTime=0;

    if(clipq == NULL)       //队列是空的
        return NULL;
    else if(clipq->next == clipq)   //队列只有一个clip
        return clipq;
    //下面用冒泡排序法把clipq中的剪切域按左→右，再上→下排序
    //剪切域很多时，冒泡法会比较慢，改进之。--db
    //startclip为每次冒泡开始的地方
    //endclip为每次冒泡结束的地方
    //排序的方法是，第一次将最大的maxclip（最下面最右面）沉到最后面，然后每次都
    //从maxclip->next开始（因为是双向循环），找一个次大的，直到startclip和
    //endclip相同
    startclip = clipq;
    endclip = startclip->previous;

    do
    {
        clippop = startclip;
        firstTime++;
        do
        {
            clippopnext = clippop->next;
            //clipop比他的下一个更右下，所以需要交换AB的位置
            if(__ClipAIsBR2ClipB(clippop, clippopnext))
            {
                if(clippop == clippopnext->next)//队列中仅仅有两个clip
                {
                    //之所以这样，我们希望A指向的总是最maxclip的
                }
                else//将A和B的位置调换，注意A和B是相邻的
                {
                    clippop_pbak = clippop->previous;
                    clippopnext_nbak = clippopnext->next;
                    clippop_pbak->next = clippopnext;
                    clippopnext_nbak->previous = clippop;

                    clippop->previous = clippopnext;
                    clippop->next = clippopnext_nbak;
                    clippopnext->next = clippop;
                    clippopnext->previous = clippop_pbak;
                }
            }
            else
            {
                clippop = clippopnext;    //一次比较遍历
            }
        }while(clippopnext != endclip);
        if(firstTime ==1)//主循环跑一次就可以知道最大的是哪个了
        {
            maxclip = clippop;
        }
        endclip = clippop;
        startclip = maxclip->next;  //todo:此句貌似可以往前挪2行
        endclip = endclip->previous;
        //printf("used times =%d\n",firstTime);
    }while(startclip!= endclip);

    return  __GK_CombineClip_s(startclip);
}



//----求矩形交集---------------------------------------------------------------
//功能: 给定两个矩形，求他们相交后的矩形。
//参数: rect1，矩形1指针
//      rect2，矩形2指针
//      result，返回结果指针
//返回: true = 两矩形相交，false = 不相交
//-----------------------------------------------------------------------------
/*
bool_t __GK_GetRectInts(struct Rectangle *rect1,struct Rectangle *rect2,
                     struct Rectangle *result)
{
    s16 max_left,max_top,min_right,min_bottom;
    //求两矩形左上角的最大坐标值
    max_left = rect1->left > rect2->left? rect1->left:rect2->left;
    max_top = rect1->top > rect2->top? rect1->top:rect2->top;
    //求两矩形右下角的最小坐标值
    min_right = rect1->right > rect2->right?rect2->right:rect1->right;
    min_bottom = rect1->bottom > rect2->bottom?rect2->bottom:rect1->bottom;
    if((min_right <= max_left) || (min_bottom <= max_top))
        return false;
    else
    {
        result->left = max_left;
        result->top = max_top;
        result->right = min_right;
        result->bottom = min_bottom;
        return true;
    }
}
*/

//----合并区域通用版-----------------------------------------------------------
//功能: 把给定的剪切域队列中可合并的区域合并，与简化版不同的是，队列中的clip是
//      随机排列的。方法是:
//      1、把所有clip按先y后x坐标排序，使队列变成左→右，再上→下排序的。
//      2、调用__gk_combine_clip_s函数进行排序。
//参数: clipq: 指向待合并的队列首
//返回: 无
//-----------------------------------------------------------------------------
struct ClipRect * __gk_combine_clip_old(struct ClipRect *clipq)
{
    struct ClipRect *min,*now,*start,*temp,*temp_previous,*temp_next;
    struct ClipRect *work;
    s32 top,left,i=0;
    if(clipq == NULL)       //队列是空的
        return NULL;
    else if(clipq->next == clipq)   //队列只有一个clip
        return clipq;
    //下面用冒泡排序法把clipq中的剪切域按左→右，再上→下排序
    //剪切域很多时，冒泡法会比较慢，改进之。--db
    min = clipq;
    start = clipq;
    now = clipq;
    temp = clipq;
    do{
        top = min->rect.top;
        left = min->rect.left;
        work = min->next;
        do{
            if((work->rect.top < top)
                || ((work->rect.top == top) && (work->rect.left <= left)))
            {
                min = work;
                top = work->rect.top;
                left = work->rect.left;
            }
            work = work->next;
        }while(work != start);
        if(min != start)  //不是待排序的第一个，需交换min与temp的位置
        {
            if(temp->next != min)   //要交换的min与temp不相邻
            {
                i++;
                if(i == 1)
                    start = min;
                now = temp;
                now->previous->next = min;
                now->next->previous = min;
                min->previous->next = now;
                min->next->previous = now;
                temp_next = min->next;
                temp_previous = min->previous;
                min->previous = now->previous;
                min->next = now->next;
                now->next = temp_next;
                now->previous = temp_previous;
            }
            else                    //要交换的min与temp相邻
            {
                i++;
                if(i == 1)
                    start = min;
                now->previous->next = min;
                min->next->previous = now;
                min->previous = now->previous;
                now->next = min->next;
                now->previous = min;
                min->next = now;
            }
            temp = min;
        }
    }while(min != start);
    return __GK_CombineClip_s(start);
  // check_clip_queque(start);
   //return start;
}
//----求矩形交集---------------------------------------------------------------
//功能: 给定两个矩形，求他们相交后的矩形。
//参数: rect1，矩形1指针
//      rect2，矩形2指针
//      result，返回结果指针
//返回: true = 两矩形相交，false = 不相交
//-----------------------------------------------------------------------------
bool_t __GK_GetRectInts(struct Rectangle *rect1,struct Rectangle *rect2,
                        struct Rectangle *result)
{
    s16 max_left,max_top,min_right,min_bottom;
    //求两矩形左上角的最大坐标值
    max_left = rect1->left > rect2->left? rect1->left:rect2->left;
    max_top = rect1->top > rect2->top? rect1->top:rect2->top;
    //求两矩形右下角的最小坐标值
    min_right = rect1->right > rect2->right?rect2->right:rect1->right;
    min_bottom = rect1->bottom > rect2->bottom?rect2->bottom:rect1->bottom;
    if((min_right <= max_left) || (min_bottom <= max_top))
        return false;
    else
    {
        result->left = max_left;
        result->top = max_top;
        result->right = min_right;
        result->bottom = min_bottom;
        return true;
    }
}

//----扫描可视域---------------------------------------------------------------
//功能: 1、把visible_clip备份到visible_bak中。
//      2、所有窗口生成新的visible_clip
//参数: display，被扫描的显示器
//返回: false=失败，一般是因为剪切域池容量不够
//-----------------------------------------------------------------------------
bool_t __GK_ScanNewVisibleClip(struct DisplayRsc *display)
{
    struct GkWinRsc *tempwin;
    struct Rectangle *rect;
    struct ClipRect *clip,*clip1,*clip_head = NULL;
    s32 num,rect_left,rect_top,rect_right,rect_bottom,loop,temp;
    u8 *sort_array_x,*sort_array_y;

    num = (display->width+1)*sizeof(u8) + (display->height+1)*sizeof(u8);
    sort_array_x = M_MallocLc(num,0);
    if(sort_array_x == NULL)
        return false;
    sort_array_y = sort_array_x + display->width+1;
    memset(sort_array_x,0,num);
    tempwin = display->z_topmost;
//    rect_left = 0;
//    rect_top = 0;
    while(1)
    {
        //要求窗口可视，且被祖先窗口限制后，仍然有可视范围
        if((tempwin->WinProperty.Visible == CN_GKWIN_VISIBLE)
           &&(tempwin->limit_right != 0) && (tempwin->limit_bottom != 0) )
        {
            //取窗口可视边框，该边框是窗口受祖先窗口限制后的矩形
            temp = tempwin->limit_left + tempwin->absx0;
            sort_array_x[temp] = 1;

            temp = tempwin->limit_right + tempwin->absx0;
            sort_array_x[temp] = 1;

            temp = tempwin->limit_top + tempwin->absy0;
            sort_array_y[temp] = 1;

            temp = tempwin->limit_bottom + tempwin->absy0;
             sort_array_y[temp] = 1;
        }
        //执行__GK_GetRedrawClipAll函数注释中的step1
        //保存窗口原来的可视域
        tempwin->visible_bak = tempwin->visible_clip;
        tempwin->visible_clip = NULL;
        if(tempwin != display->desktop)
            tempwin = tempwin->z_back;
        else
            break;
    }
    //处理桌面下的窗口，这些窗口肯定没有可视域
    while(1)
    {
        tempwin = tempwin->z_back;
        if(tempwin == display->z_topmost)
            break;
        tempwin->visible_bak = tempwin->visible_clip;
        tempwin->visible_clip = NULL;
    }
    //按从左到右，从上到下的顺序(顺序不能改变)把所有垂直线、水平线围成的小clip
    //串成双向链表，由clip_head做链表头
    rect_top = 0;
    temp = 0;
    for(rect_bottom =1;rect_bottom <= display->height; rect_bottom++)
    {
        if(sort_array_y[rect_bottom] == 0)
            continue;
        rect_left = 0;
        for(rect_right =1;rect_right <= display->width; rect_right++)
        {
            if(sort_array_x[rect_right] == 0)
                continue;
            clip = (struct ClipRect*)Mb_Malloc(g_ptClipRectPool,0);
            if(clip != NULL)
            {
                clip->rect.left = rect_left;
                clip->rect.right = rect_right;
                clip->rect.top = rect_top;
                clip->rect.bottom = rect_bottom;
                rect_left = rect_right;
                __GK_ClipConnect(&clip_head,clip);      //把小clip加入到链接起来
                temp++;
            }
            else
            {
                __GK_FreeClipQueue(clip_head);
                free(sort_array_x);
                return false;           //内存池不足，无法生成可视域队列
            }
        }
        rect_top = rect_bottom;
    }
    free(sort_array_x);

    //下面判断小clip的归属，并把他们加入到所属win的new_clip队列中
    tempwin = display->z_topmost;
    while(1)
    {
        clip = clip_head;
        if(tempwin->WinProperty.Visible == CN_GKWIN_VISIBLE)
        {
            for(loop = temp; loop >0; loop--)
            {
                rect = &(clip->rect);
                if((rect->left>=tempwin->limit_left + tempwin->absx0)
                   &&(rect->top>=tempwin->limit_top + tempwin->absy0)
                   &&(rect->right<=tempwin->limit_right + tempwin->absx0)
                   &&(rect->bottom<=tempwin->limit_bottom + tempwin->absy0))
                {   //矩形在tempwin的可显示范围内,若不在则无需处理
                    //允许alpha或透明，区域将加入窗口可视域，但不从临时链表中删除。
                    if(tempwin->WinProperty.DestBlend == CN_GKWIN_DEST_VISIBLE)
                    {
                        clip1 = (struct ClipRect*)Mb_Malloc(g_ptClipRectPool,0);
                        if(clip1 != NULL)
                        {
                            *clip1 = *clip;
                        }
                        else
                        {
                            __GK_FreeClipQueue(clip_head);
                            return false;           //内存池不足，无法生成可视域队列
                        }
                    }else   //不允许透明和alpha，区域加入窗口可视域，从临时链表删除
                    {
                        if(clip == clip_head)
                            clip_head = clip_head->next;
                        clip1 = clip;
                        clip->previous->next = clip->next;
                        clip->next->previous = clip->previous;
                        temp--;
                    }
                    clip = clip->next;
                    //把小clip加入到visible_clip队列中
                    __GK_ClipConnect(&tempwin->visible_clip,clip1);
                }else       //矩形不在tpwin的可显示范围内，无需处理。
                {
                    clip = clip->next;
                }
            }
            __GK_CombineClip_s(tempwin->visible_clip);//合并clip,按先x后y的顺序合并
        }
        if(tempwin->visible_clip == 0xa0a0a0a)
            tempwin = NULL;
        if(tempwin != display->desktop)
            tempwin = tempwin->z_back;
        else
            break;
    }
    return true;
}

//----生成新可视域-------------------------------------------------------------
//功能: 全部窗口生成新的可视域队列，把原来的可视域放到visible_bak中，并释放原来
//      的visible_bak队列:然后生成新的可视域，最后把新增可视域加入到redraw_clip
//      队列中。完成的是 __GK_GetRedrawClipAll 函数注释中的step1~3。
//参数: display，被扫描的显示器
//返回: false=失败，一般是因为剪切域池容量不够
//-----------------------------------------------------------------------------
bool_t __GK_GetVisibleClip(struct DisplayRsc *display)
{
    struct GkWinRsc *tempwin;
    if(__GK_ScanNewVisibleClip(display) == false)
        return false;
    tempwin = display->z_topmost;
    while(1)
    {
        //备份visible_clip
        tempwin->copy_clip = __GK_CopyClipLink(tempwin->visible_clip);
//        if(tempwin->WinProperty.ChangeFlag == CN_GKWIN_CHANGE_ALL)
//        {
//            //全部可视域需重绘，
//            __GK_ClipLinkConnect(&tempwin->redraw_clip,tempwin->visible_clip);
//            tempwin->visible_clip = NULL;   //可视域已经全部加入重绘域
//            tempwin->WinProperty.ChangeFlag = CN_GKWIN_CHANGE_NONE;
//        }
//        else
        {
            __GK_GetNewClip(tempwin);         //新增可视域加入redraw_clip
        }
        if(tempwin != display->desktop)
            tempwin = tempwin->z_back;
        else
            break;
    }
    return true;
}

//----备份clip_link---------------------------------------------------------
//功能: 备份剪切域队列。
//参数: clip_link，被备份的剪切域队列
//返回: 备份的队列，出错则返回NULL
//-----------------------------------------------------------------------------
struct ClipRect * __GK_CopyClipLink(struct ClipRect *clip_link)
{
    struct ClipRect *clip1,*clip2,*clip_rsc,*result = NULL;

    clip_rsc = clip_link;
    if(clip_rsc == NULL)
        return NULL;

    //创建第一个节点
    clip1 = (struct ClipRect*)Mb_Malloc(g_ptClipRectPool,0);
    if(clip1 == NULL)
        return NULL;
    clip1->rect = clip_rsc->rect;
    result = clip1;
    clip1->next = NULL;
    clip_rsc = clip_rsc->next;
    while(clip_rsc != clip_link)
    {
        clip2 = (struct ClipRect*)Mb_Malloc(g_ptClipRectPool,0);
        if(clip2 == NULL)
            goto error_exit;
        clip2->rect = clip_rsc->rect;
        clip_rsc = clip_rsc->next;
        clip1->next = clip2;
        clip2->previous = clip1;
        clip2->next = NULL;
        clip1 = clip2;
    }
    //以下两句使copy_clip链表形成闭环
    result->previous = clip1;
    clip1->next = result;
    return result;
error_exit:
    clip1 = result;
    do
    {
        clip2 = clip1->next;
        Mb_Free(g_ptClipRectPool,clip1);
        clip1 = clip2;
    }while(clip2 != NULL);
    return NULL;
}



//----剪切域队列中增加一个节点-------------------------------------------------
//功能: 把clip加入到队列clip_link中，新结点放在clip_link结点前面。
//参数: clip_link，目标队列中的插入点
//      clip，待插入的节点
//返回: 无
//-----------------------------------------------------------------------------
void __GK_ClipConnect(struct ClipRect **clip_link,struct ClipRect *clip)
{
    if(*clip_link == NULL)
    {
        *clip_link = clip;
        clip->next = clip;
        clip->previous = clip;
    }else
    {
        clip->next = *clip_link;
        clip ->previous = (*clip_link)->previous;
        (*clip_link)->previous->next = clip;
        (*clip_link)->previous = clip;
    }
}

//----剪切域队列中增加一个节点-------------------------------------------------
//功能: 把clip加入到队列clip_link中，新结点放在clip_link结点前面。
//参数: clip_link，目标队列中的插入点
//      clip，待插入的节点
//返回: 无
//-----------------------------------------------------------------------------

void __GK_ClipLinkConnect(struct ClipRect **mlink,struct ClipRect *sub)
{
    struct ClipRect *temp;

    if(*mlink == NULL)
        *mlink = sub;
    else if(sub != NULL)
    {
        temp = sub->previous;
        sub->previous = (*mlink)->previous;
        (*mlink)->previous->next = sub;
        (*mlink)->previous = temp;
        temp->next = (*mlink);
    }
}


//----获取被修改区域-----------------------------------------------------------
//功能: 获取一个窗口的被修改部分的剪切域，形成双向循环链表，链表中的clip以先
//      左->右，再上->下的顺序排列，并清零changed_msk数组。
//参数: gkwin，目标窗口
//返回: 链表指针，NULL表示窗口未被修改
//-----------------------------------------------------------------------------
struct ClipRect *__GK_GetChangedClip(struct GkWinRsc *gkwin)
{
    struct ClipRect *clip=NULL,*clip1,*tempclip,*clip_head = NULL;
    u8 *msk;
    u32 offset_bit,offset,msk_line_words;
    s16 loopx,loopy;
    bool_t start;
    s32 width,height;

    if(gkwin->WinProperty.ChangeFlag == CN_GKWIN_CHANGE_ALL)   //整个窗口均被修改
    {
        clip = (struct ClipRect *)Mb_Malloc(g_ptClipRectPool,0);
        if(clip != NULL)
        {
            clip->rect.left = gkwin->limit_left + gkwin->absx0;
            clip->rect.top = gkwin->limit_top + gkwin->absy0;
            clip->rect.right = gkwin->limit_right + gkwin->absx0;
            clip->rect.bottom = gkwin->limit_bottom + gkwin->absy0;
            clip->next = clip;
            clip->previous = clip;
            gkwin->WinProperty.ChangeFlag = CN_GKWIN_CHANGE_NONE;
            return clip;            //把整个窗口的可显示区域当作一个剪切域返回
        }
        else
            return NULL;
    }else if(gkwin->WinProperty.ChangeFlag == CN_GKWIN_CHANGE_NONE)    //没有修改
    {
        return NULL;
    }else       //部分修改，先按x方向取得剪切域，再按y方向合并
    {
        offset = 0;                 //首行字偏移量
        msk = gkwin->changed_msk.bm_bits;
        width = gkwin->wm_bitmap->width;
        height = gkwin->wm_bitmap->height;
        msk_line_words = gkwin->changed_msk.linebytes;
        for(loopy = 0; loopy < height; loopy +=8)
        {
            start = false;      //开始一段连续的被修改区域
            offset_bit = 0;     //起始块的位偏移
            for(loopx = 0; loopx < width; loopx+=8)
            {
                if(msk[offset+offset_bit/8]&(u8)(1<<(8-(offset_bit%8)-1)))
                {   //该块的changed flag为true。
                    if(start == false)      //changed block开始被修改
                    {
                        start = true;
                        clip =(struct ClipRect*)Mb_Malloc(g_ptClipRectPool,0);
                        if(clip != NULL)
                        {
                            clip->rect.left = gkwin->absx0 + offset_bit*8;
                            clip->rect.top = loopy + gkwin->absy0;
                            clip->rect.right = clip->rect.left + 8;
                            clip->rect.bottom = clip->rect.top + 8;
                        }
                        else
                        {
                            __GK_FreeClipQueue(clip_head);
                            return NULL;
                        }
                    }else                   //changed block持续
                    {
                        clip->rect.right += 8;
                    }
                }else     //如果本行的最后一块也是被改写的块，则程序不会走到这里
                {
                    if(start == true)     //changed block结束
                    {
                        __GK_ClipConnect(&clip_head,clip);
                        start = false;
                    }
                }
                offset_bit++;
            }
            //本行最后一块是被改写块，需要在这里把该区域加入被修改区域
            if(start == true)
                __GK_ClipConnect(&clip_head,clip);
            offset += msk_line_words;
        }

        //至此，x方向条状clip已经生成，现在沿y方向合并
        clip = clip_head;
        do{
            clip1 = clip->next;
            while(clip1 != clip)
            {
                if((clip1->rect.left == clip->rect.left)
                    &&(clip1->rect.right == clip->rect.right)
                    &&((clip->rect.bottom)== clip1->rect.top))//可合并
                {
                    clip->rect.bottom += 8;
                    clip1->previous->next = clip1->next;
                    clip1->next->previous = clip1->previous;
                    tempclip = clip1;
                    clip1 = clip1->next;
                    Mb_Free(g_ptClipRectPool,tempclip);
                }else
                    clip1 = clip1->next;
            }
            clip = clip->next;
        }while(clip != clip_head);
        gkwin->WinProperty.ChangeFlag = CN_GKWIN_CHANGE_NONE;
    }
    memset(msk,0,msk_line_words * gkwin->changed_msk.height);
    return clip_head;
}

//----提取剪切域队列重叠部分---------------------------------------------------
//功能: 提取src队列中的所有矩形与sub队列中重叠的部分，用ins参数返回；
//      src中非重叠的部分也形成新的剪切域队列，从different返回。
//参数: src，被减的队列指针，执行后，本队列将被释放或转移
//      sub，减数队列指针
//      ins，src与sub相交的部分
//      different，src与sub不相交的部分
//返回: 无
//-----------------------------------------------------------------------------
void __GK_ClipLinkSub(struct ClipRect *src,struct ClipRect *sub,
                       struct ClipRect **ins,struct ClipRect **different)
{
    struct ClipRect *difftemp = NULL;  //不重叠的部分暂存
    struct ClipRect *worksrc;          //src中矩形被减产生的工作子队列
    struct ClipRect *LoopSrc;
    struct ClipRect *EndLoop;
    struct ClipRect *workloop;         //工作子队列的循环遍历指针
    struct ClipRect *subloop;          //减数队列的循环遍历指针
    struct ClipRect *clip_ints=NULL;   //用于收集重叠的区域
    struct ClipRect *temp1;            //临时变量
    struct Rectangle ints_rect,src_rect;
    bool_t EndSub;
    ufast_t uftemp;
    //减数队列为空
    if(sub == NULL)
    {   //被减数队列为空
        if(src == NULL)
        {//src与sub相交、不相交的部分都为空
            *ins = NULL;
            *different = NULL;
            return;
        }else
        {//src与sub相交部分为空、不相交的部分为src
            *ins = NULL;
            *different = src;
            return;
        }
    }else //减数队列不为空
    {   //被减数队列为空
        if(src == NULL)
        {//src与sub相交、不相交的部分都为空
            *ins = NULL;
            *different = NULL;//

          //  *different = sub;
            return;
        }
    }
    //被减数队列、减数队列都不为空
    LoopSrc = src;
    do{
        worksrc = LoopSrc;
        LoopSrc = LoopSrc->next;
        worksrc->next = worksrc;
        worksrc->previous = worksrc;
        subloop = sub;
        do{
            workloop = worksrc;
            EndLoop = worksrc->previous;
            EndSub = false;
            //此处用循环是必要的，减sub中第一个矩形时worksrc只有一个矩形，可
            //以不用循环来处理，但减第二个矩形时，worksrc可能分裂成一个队列。
            do
            {
                if(__GK_GetRectInts(&workloop->rect,&subloop->rect,&ints_rect))
                {//矩形相交
                    temp1 = (struct ClipRect *)Mb_Malloc(g_ptClipRectPool,0);
                    temp1->rect = ints_rect;
                    //把相交区域加入clip_ints
                    __GK_ClipConnect(&clip_ints,temp1);
                    src_rect = workloop->rect;
                    uftemp = 0;
                    //以下提取workloop中与subloop中不相交的部分，直接替代src队列
                    //中workloop的位置
                    if(subloop->rect.top > src_rect.top)        //上部矩形
                    {   //首个矩形，覆盖原结点
                        workloop->rect.bottom = subloop->rect.top;
                        workloop = workloop->next;
                        uftemp++;
                    }
                    if(subloop->rect.left > src_rect.left)      //左部矩形
                    {   //首个矩形，覆盖原结点
                        if(uftemp == 0)
                        {
                            workloop->rect.right = subloop->rect.left;
                            if(src_rect.bottom < subloop->rect.bottom)
                                workloop->rect.bottom = src_rect.bottom;
                            else
                                workloop->rect.bottom = subloop->rect.bottom;
                            workloop = workloop->next;
                        }else
                        {
                            temp1 =
                             (struct ClipRect *)Mb_Malloc(g_ptClipRectPool,0);
                            temp1->rect.left = src_rect.left;
                            temp1->rect.top = subloop->rect.top;
                            temp1->rect.right = subloop->rect.left;
                            if(src_rect.bottom < subloop->rect.bottom)
                                temp1->rect.bottom = src_rect.bottom;
                            else
                                temp1->rect.bottom = subloop->rect.bottom;
                            __GK_ClipConnect(&workloop,temp1);
                        }
                        uftemp++;
                    }
                    if(subloop->rect.right < src_rect.right)    //右部矩形
                    {   //首个矩形，覆盖原结点
                        if(uftemp == 0)
                        {
                            workloop->rect.left = subloop->rect.right;
                            if(src_rect.bottom < subloop->rect.bottom)
                                workloop->rect.bottom = src_rect.bottom;
                            else
                                workloop->rect.bottom = subloop->rect.bottom;
                            workloop = workloop->next;
                        }else
                        {
                            temp1 =
                             (struct ClipRect *)Mb_Malloc(g_ptClipRectPool,0);
                            temp1->rect.left = subloop->rect.right;
                            if(src_rect.top < subloop->rect.top)
                                temp1->rect.top = subloop->rect.top;
                            else
                                temp1->rect.top = src_rect.top;
                            temp1->rect.right = src_rect.right;
                            if(src_rect.bottom < subloop->rect.bottom)
                                temp1->rect.bottom = src_rect.bottom;
                            else
                                temp1->rect.bottom = subloop->rect.bottom;
                            __GK_ClipConnect(&workloop,temp1);
                        }
                        uftemp++;
                    }
                    if(subloop->rect.bottom < src_rect.bottom)      //下部矩形
                    {   //首个矩形，覆盖原结点
                        if(uftemp == 0)
                        {
                            workloop->rect.top = subloop->rect.bottom;
                            workloop = workloop->next;
                        }else
                        {
                            temp1 =
                             (struct ClipRect *)Mb_Malloc(g_ptClipRectPool,0);
                            temp1->rect.left = src_rect.left;
                            temp1->rect.top = subloop->rect.bottom;
                            temp1->rect.right = src_rect.right;
                            temp1->rect.bottom = src_rect.bottom;
                            __GK_ClipConnect(&workloop,temp1);
                        }
                        uftemp++;
                   }
                   if(uftemp == 0)                    //源矩形被完全覆盖,
                   {
                        if(worksrc->next == worksrc)    //源队列只有一个矩形
                        {
                            Mb_Free(g_ptClipRectPool,worksrc);
                            worksrc = NULL;
//                            workloop = NULL;
                            EndSub = true;
                        }else
                        {
                            temp1 = workloop;
                            workloop = workloop->next;
                            temp1->previous->next = workloop;
                            workloop->previous = temp1->previous;
                            if(worksrc == temp1)
                                worksrc = workloop;
                            if(EndLoop == temp1)
                                EndSub = true;
                            Mb_Free(g_ptClipRectPool,temp1);
                        }
                    }
                   else
                   {
                        workloop = workloop->next;
                   }
                }
                else           //矩形不相交
                {
                    workloop = workloop->next;
                }
                if(workloop->previous == EndLoop)
                    EndSub = true;
            }while(EndSub == false);
            if(worksrc == NULL)
                break;
            subloop = subloop->next;
        }while(subloop != sub);
        __GK_ClipLinkConnect(&difftemp,worksrc);
    }while(src != LoopSrc);
    *different = difftemp;
    *ins = clip_ints;
}
//----取得新增可视域-----------------------------------------------------------
//功能: 出现在visible_clip中且不在visible_bak的区域，加入redraw_clip。
//      同时出现在visible_clip和visible_bak中的部分，留在visible_clip中
//      如果dest_blend==true，则仅在visible_bak出现的部分留在visible_bak
//      否则，释放visiblvisible_bak
//参数: gkwin，目标窗口
//返回: 无
//-----------------------------------------------------------------------------
void __GK_GetNewClip(struct GkWinRsc *gkwin)
{
    struct ClipRect *redraw;       //visible_clip-visible_bak多出的部分
    struct ClipRect *res;          //visible_clip与visible_bak相交的clip

    if(gkwin->visible_bak != NULL)
    {
        __GK_ClipLinkSub(gkwin->visible_clip,gkwin->visible_bak,&res,&redraw);
        __GK_ClipLinkConnect(&gkwin->redraw_clip,redraw);
        gkwin->visible_clip = res;
        if(gkwin->WinProperty.DestBlend == CN_GKWIN_DEST_VISIBLE)
        {
            __GK_ClipLinkSub(gkwin->visible_bak,gkwin->visible_clip,
                              &res,&redraw);
            gkwin->visible_bak = redraw;
            __GK_FreeClipQueue(res);
        }
        else
        {
            gkwin->visible_bak = __GK_FreeClipQueue(gkwin->visible_bak);
        }
    }
    else
    {
        __GK_ClipLinkConnect(&gkwin->redraw_clip,gkwin->visible_clip);
        gkwin->visible_clip = NULL;
    }
}

//----提取剪切域交集-----------------------------------------------------------
//功能: 剪取srcclip队列中与desclip相交部分，不相交的部分仍然留在srcclip中。
//参数: srcclip，原剪切域链表
//      desclip，目标剪切域链表
//返回: 提取的可视域，NULL = 没有相交部分
//-----------------------------------------------------------------------------
struct ClipRect *__GK_GetClipLinkInts(struct ClipRect **srcclip,
                                             struct ClipRect *desclip)
{
    struct ClipRect *redraw,*res;
    //提取剪切域队列重叠部分
    __GK_ClipLinkSub(*srcclip,desclip,&redraw,&res);
    *srcclip = res;
    return redraw;
}

//----生成重绘区域-------------------------------------------------------------
//功能: 扫描整个窗口链表，重新生成全部窗口的可视域,以及待重绘的clip，方法和过程:
//      1、释放所有的visible_bak，把visible_clip转到visible_bak中。
//      2、重新生成所有win的visible_clip，并复制到 copy_clip 中临时保存。
//      3、visible_clip减去中visible_bak所得加入redraw_clip队列。剩余部分保留在
//          visible_clip中。如果窗口的dest_blend == true，则visible_bak中保存
//          由可见变为不可见的部分，否则visible_bak将被释放。
//      4、从changed msk中提取changed_clip。
//      5、取visible_clip与changed_clip的交集，加入redraw_clip队列。
//      6、从z_top起，取visible_bak,扫描其后窗口的可视域，与其重叠的部分，加入
//         redraw域。符合此条件的visible_bak，必定是因为窗口移开所致。
//      7、从z_top起，对dest_blend == true的win，扫描z轴中在其后面的
//         win的visible_clip，与其需重绘部分重叠的，加入redraw_clip队列。
//         此后dest_blend != true的win的visible_clip可释放。
//      8、合并前几步产生的redraw_clip队列。
//      9、从z_top起，对任一dest_blend == true的win的visible_clip，扫描z轴中在其
//         后面的win的redraw_clip，重叠的部分加入redraw_clip队列，并合并之。
//参数: display，被扫描的显示器
//返回: false=失败，一般是因为剪切域池容量不够
//-----------------------------------------------------------------------------
bool_t __GK_GetRedrawClipAll(struct DisplayRsc *display)
{
    struct GkWinRsc *tempwin;
    struct GkWinRsc *special_win;
    struct ClipRect *changed_clip,*tempclip1;
    if(display->reset_clip == true)         //看是否需要重新生成可视域
    {
        if(__GK_GetVisibleClip(display) == false)     //完成step1、step2、step3
            return false;
        display->reset_clip = false;
    }
    tempwin = display->desktop;
    while(1)
    {   //扫描desktop到z_topmost之间的全部窗口，完成step4~5，在desktop后面的
        //窗口不显示，或者在step1~3中已经全部转移到 redraw_clip 中，无需处理。
        if(tempwin->visible_clip != NULL)
        {
            //备份visible_clip
            if(tempwin->copy_clip == NULL)  //copy_clip可能已经在step1~3中设置了
                tempwin->copy_clip = __GK_CopyClipLink(tempwin->visible_clip);
            //整个窗口全部被修改，整体绘制、窗口移动、拉伸、压缩会导致全体修改
            //reset_clip == true成立的话，cn_gkwin_change_all的情况在
            //__gk_get_visible_clip函数中已经完成，在该函数中完成，可以减轻不少
            //运算量，而且对这里没有影响。
            if(tempwin->WinProperty.ChangeFlag == CN_GKWIN_CHANGE_ALL)
            {
                //全部可视域需重绘
                __GK_ClipLinkConnect(&tempwin->redraw_clip,
                                       tempwin->visible_clip);
                tempwin->visible_clip = NULL;   //可视域已经全部加入重绘域
                tempwin->WinProperty.ChangeFlag = CN_GKWIN_CHANGE_NONE;
            }else
            {   //有部分修改
                if(tempwin->WinProperty.ChangeFlag == CN_GKWIN_CHANGE_PART)
                {
                    //取得修改域
                    changed_clip = __GK_GetChangedClip(tempwin);
                    //提取可视域中修改的区域
                    tempclip1 = __GK_GetClipLinkInts(
                                    &tempwin->visible_clip,changed_clip);
                    __GK_FreeClipQueue(changed_clip);
                    //把修改区域加入到redraw_clip中
                    __GK_ClipLinkConnect(&tempwin->redraw_clip,tempclip1);
                    tempwin->WinProperty.ChangeFlag = CN_GKWIN_CHANGE_NONE;
                }
            }
//              tempwin->visible_bak = NULL;
        }
        else
            tempwin->WinProperty.ChangeFlag = CN_GKWIN_CHANGE_NONE;
        //清除changed_msk
        __GK_ShadingClear(tempwin);
        if(tempwin != display->z_topmost)
            tempwin = tempwin->z_top;
        else
            break;
    }

    //执行step6，改变位置的、dest_blend=true的窗口，改变前遮挡的其他窗口的
    //可视域，加入redraw域。
    special_win = display->z_topmost;
    while(1)
    {
        //窗口移动，伸缩会导致visible_bak != NULL
        if((special_win->WinProperty.DestBlend == CN_GKWIN_DEST_VISIBLE)
                    && (special_win->visible_bak != NULL))
        {
            tempwin = special_win->z_back;
            while(1)
            {
//                tempclip2 = __GK_CopyClipLink(special_win->visible_bak);
                //取得背景窗口需要重绘的区域
                tempclip1 =
                    __GK_GetClipLinkInts(&tempwin->visible_clip,
                                          special_win->visible_bak);

//                __GK_FreeClipQueue(tempclip2);
                //把修改区域加入到背景窗口redraw_clip中
                __GK_ClipLinkConnect(&tempwin->redraw_clip,tempclip1);
                if(tempwin != display->desktop)
                    tempwin = tempwin->z_back;
                else
                    break;
            }
            special_win->visible_bak = __GK_FreeClipQueue(special_win->visible_bak);
        }
        if(special_win != display->desktop)
            special_win = special_win->z_back;
        else
            break;
    }

    //执行step7，窗口需要背景参与运算，此窗口内容有改变
    special_win = display->z_topmost;
    while(1)
    {   //hjj,step7和step9存在先后执行的情况，调用__gk_get_cliplink_ints_res
        //会对参数有影响(可能导致参数值改变)，使用时应多做考虑
        if(special_win->WinProperty.DestBlend == CN_GKWIN_DEST_VISIBLE)
        {//special_win需要背景参与运算才执行下面的程序
            if(special_win->redraw_clip != NULL)
            {
                tempwin = special_win->z_back;
                while(1)
                {
                    //取得背景窗口需要重绘的区域
                    tempclip1 =
                        __GK_GetClipLinkInts(&tempwin->visible_clip,
                                                special_win->redraw_clip);

                    //把修改区域加入到背景窗口redraw_clip中
                    __GK_ClipLinkConnect(&tempwin->redraw_clip,tempclip1);
                    if(tempwin != display->desktop)
                        tempwin = tempwin->z_back;
                    else
                        break;
                }
            }
        }
        else
        {//释放掉不需要背景参与运算的窗口的可视域
            special_win->visible_clip =
                            __GK_FreeClipQueue(special_win->visible_clip);
        }
        if(special_win != display->desktop)
            special_win = special_win->z_back;
        else
            break;
    }

    //执行step8，合并前几步产生的redraw_clip队列
    tempwin = display->z_topmost;
    while(1)
    {
        //合并redraw_clip
        tempwin->redraw_clip = __GK_CombineClip(tempwin->redraw_clip);
        if(tempwin != display->desktop)
            tempwin = tempwin->z_back;
        else
            break;
    }

    //执行step9，本步骤无须判断desktop，故循环体有所不同
    //需要背景参与运算的窗口，其背景改变
    special_win = display->z_topmost;
    do{
        if((special_win->WinProperty.DestBlend == CN_GKWIN_DEST_VISIBLE)
                        &&(special_win->visible_clip != NULL))
        {//special_win需要背景窗口参与运算，才执行下面的程序
            tempwin = special_win;
            do{
                tempwin = tempwin->z_back;
                //取得背景窗口的修改域
                tempclip1 =
                    __GK_GetClipLinkInts(&special_win->visible_clip,
                                              tempwin->redraw_clip);
                //把修改区域加入到窗口的redraw_clip中
                __GK_ClipLinkConnect(&special_win->redraw_clip,tempclip1);
            }while(tempwin != display->desktop);
            //释放掉不需要使用的区域
            special_win->visible_clip =
                            __GK_FreeClipQueue(special_win->visible_clip);
            //合并redraw_clip
            special_win->redraw_clip =
                            __GK_CombineClip(special_win->redraw_clip);
        }
        special_win = special_win->z_back;
    }while(special_win != display->desktop);

    //恢复visible_clip
    tempwin = display->z_topmost;
    while(1)
    {
        tempwin->visible_clip = tempwin->copy_clip;
        tempwin->copy_clip = NULL;
        if(tempwin != display->desktop)
            tempwin = tempwin->z_back;
        else
            break;
    }
    return true;
}
//----生成可视域---------------------------------------------------------------
//功能: 生成z1(含)至z2(含)之间的窗口的可视域。
//参数: z1，z轴中靠后的窗口，包含在内
//      z2，z轴中靠前的窗口，包含在内
//返回: 无
//-----------------------------------------------------------------------------
void __GK_GetClipSection( struct GkWinRsc *z1,struct GkWinRsc *z2)
{
}

