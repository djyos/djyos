// =======================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: VNCServer
// 模块版本: V1.00(初始化版本)
// 文件名  ：clip_process.c
// 创建人员: Administrator
// 创建时间: 2013-6-9
// 说明文档：该文件是VNCclip的clip处理算法的实现，大多是仅仅内部使用的函数，
//           很多不会进行内部参数检查的，不希望将时间浪费在此，因为使用环境
//           的限制
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <V1.00> <2013-3-5>, <修改人员>: <修改功能概述>
// =======================================================================
#include "clip_process.h"
#include "debug.h"

struct tagMemCellPool               *pg_vncclip_pool;
// the vnc clip pool,没有办法，为了避免mirrordisplay扯上关系，在此设置全局变量

// =============================================================================
// 函数功能:设置VNC的内存池
// 输入参数:
// 输出参数：
// 返回值  :
// =============================================================================

void vnc_set_vncclip_pool(struct tagMemCellPool  *newpool)
{
    pg_vncclip_pool = newpool;
}

// =============================================================================
// 函数功能:为VNCclip分配存储空间，
// 输入参数:
// 输出参数：
// 返回值  :NULL，失败，否则成功
// 说明    :这是从VNC的内存池分配的空间，所以一般的情况下，请不要擅自删除，使用API函数
//          对应的分配和删除
// =============================================================================
struct tagClipRect *get_vncclip_space()
{
    struct tagClipRect *clip = (struct tagClipRect *)Mb_Malloc(pg_vncclip_pool, 0);
    return clip;
}

// =============================================================================
// 函数功能:释放vncclip的存储空间，
// 输入参数:
// 输出参数：
// 返回值  :NULL，失败，否则成功
// 说明    :这是从VNC的内存池分配的空间，所以一般的情况下，请不要擅自删除，使用API函数
//          对应的分配和删除
// =============================================================================
void free_vncclip_space(struct tagClipRect *clip)
{
     Mb_Free(pg_vncclip_pool,clip);
}

// =============================================================================
// 函数功能:释放vncclip队列的存储空间，
// 输入参数:
// 输出参数：
// 返回值  :NULL，失败，否则成功
// 说明    :这是从VNC的内存池分配的空间，所以一般的情况下，请不要擅自删除，使用API函数
//          对应的分配和删除
// =============================================================================
void free_vnccliplink_space(struct tagClipRect *clip)
{

    struct tagClipRect *temp,*temp1;
    if(clip != NULL)
    {
        temp = clip;
        do
        {
            temp1 = temp->next;
            free_vncclip_space(temp);
            temp = temp1;
        }while(temp != clip);
    }
}

// =============================================================================
// 函数功能:，将vncclip加入到clip队列中
// 输入参数:
// 输出参数：
// 返回值  :
// 说明    :
// =============================================================================
void connect_clip2cliplink(struct tagClipRect **mlink,struct tagClipRect *sub)
{
    struct tagClipRect *temp;

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
// =============================================================================
// 函数功能:将clip从clip队列中移除
// 输入参数:
// 输出参数：
// 返回值  :
// 说明    :将*mlink总队列中移除，同时返回移除后的队列队首（当前clip的下一个），
//          如果空则返回NULL
// =============================================================================
struct tagClipRect*  remove_clip_from_cliplink(struct tagClipRect **mlink)
{
   struct  tagClipRect  *clip = NULL;
   if(NULL == *mlink)
   {
    clip = NULL;
   }
   else if((*mlink)->next == (*mlink))//仅仅一个clip
   {
    clip = NULL;
   }
   else if((*mlink)->next == (*mlink)->previous)//两个clip
   {
    clip=(*mlink)->next;
    clip->next = clip;
    clip->previous = clip;
   }
   else
   {
        clip=(*mlink)->next;
        clip->previous = (*mlink)->previous;
        (*mlink)->previous->next = clip;
   }
   return clip;
}
// =============================================================================
// 函数功能:打印clip队列的情况（调试函数，调试用）
// 输入参数:
// 输出参数：
// 返回值  :
// 说明    :
// =============================================================================-
void show_clip_queque(struct tagClipRect *clip)
{

        int o,p,q;
        struct tagClipRect *var1,*var2;
        if(clip==NULL)
         {
            return;
         }
        var1=clip;
         var2=var1;
         do{
             o=(int)var1;
             p=(int)var1->previous;
             q=(int)var1->next;

            printf(" connected:cur=%08x pre=%08x  next=%08x  top=%d left=%d\n",o,p,q,var1->rect.top,var1->rect.left);
            var1=var1->next;
         }while(var1!=var2);
         debug_printf("\n");
}
// =============================================================================
// 函数功能:检查clip队列的正确性（调试函数，调试用）
// 输入参数:
// 输出参数：
// 返回值  :
// 说明    :通过正向数数和反向数数来判断队列的正确性，尽管很弱智，但是用在此处足够
//          不排除八字形的问题
// =============================================================================-
bool_t check_clip_queque(struct tagClipRect *cliphead)
{

    int backward=0;
    int forward=0;
    struct tagClipRect *pclip;
    pclip=cliphead;

    if(NULL==pclip)
    {
       return true;

    }
    else
    {
        do{
          backward++;
          pclip=pclip->next;
        }while(pclip!=cliphead);

         do{
          forward++;
          pclip=pclip->previous;
        }while(pclip!=cliphead);

        if(backward!=forward)
         {
            printf("some fatal error has happend in the clip queque!for=%d back=%d\n",forward,backward);
            return false;
        }

    }
        return true;
}


// =========================================================================
// 函数功能:输出clip的个数(用于调试用，正常程序是不使用的,影响速度)
// 输入参数:clip队列头
// 输出参数：
// 返回值  :clip的个数，0xFFFF是出错了，严重的bug
// 说明    :必须保证clip是双向循环列表
// =========================================================================
u16 count_clip_number(struct tagClipRect *cliphead)
{
    int ret=0;
    struct tagClipRect *pclip;
    pclip=cliphead;

    if(NULL==pclip)
    {
        return ret;
    }
    else
    {
        do{
          ret++;
          pclip=pclip->next;
        }while(pclip!=cliphead);
    }
    return ret;
}

//融入法来将新建的clip加入到原来的clip队列中
enum twoclipstate
{
    INCLUDING=0, //A在B中
    INCLUDED,  //B在A中
    CROSSING, //相交
    ISLAND,   //不想交
    FAILED,   //something too bad
    SUCCESS,  //do something to the clip success
    ANYOPER,  //该clip可以做任何操作
};
// =============================================================================
// 函数功能:判断该clip描述的到底是线还是点
// 输入参数:待判定的clip
// 输出参数：
// 返回值  :
// 说明    :1 是矩形  0 线或者点
// =============================================================================
static IsRealRec(struct tagRectangle rec)//看是不是线点还是真正的矩形
{
    if((rec.top == rec.bottom)||(rec.left == rec.right))
    {
        return 0;
    }
    else
    {
        return 1;
    }

}
// =============================================================================
// 函数功能:判断clip_compared相对于CLIPA的关系。
// 输入参数:
// 输出参数：
// 返回值  :INCLUDED A包含B，INCLUDING B包含A ，CROSSING 普通相交 ，ISLAND,不相交
// 说明    :
// =============================================================================
static int  judge_position_B2A(struct tagClipRect *A, struct tagClipRect *B)//判断Bclip相对于Aclip的状态
{
    if(NULL==A)//A为空
    {
        return INCLUDING;
    }
    else if(NULL == B)// B为空
    {
        return INCLUDED;
    }
    else//A B均不空，一般就是这种情况
    {
        if((A->rect.top >= B->rect.bottom)||(A->rect.left>=B->rect.right)||\
        (A->rect.right <= B->rect.left)||(A->rect.bottom <= B->rect.top))//不相交的情况
        {
            return ISLAND;
        }
        else if((A->rect.top <= B->rect.top)&&(A->rect.left <= B->rect.left)\
        &&(A->rect.right >= B->rect.right)&&(A->rect.bottom >= B->rect.bottom))//A包含B
        {
            return  INCLUDED;
        }
        else if((A->rect.top >= B->rect.top)&&(A->rect.left >= B->rect.left)\
        &&(A->rect.right <= B->rect.right)&&(A->rect.bottom <= B->rect.bottom))//A包含B
        {
            return  INCLUDING;
        }
        else//普通的相交过程 ,different可以分为四部分
        {
            return  CROSSING;
        }//普通相交

    } //正常处理

}//函数结束


// =============================================================================
// 函数功能:用clipA来击碎CLIPB
// 输入参数:
// 输出参数:different ：B相对于A不同的地方，ints B相对于A相同的地方
// 返回值  :成功、失败
// 说明    :目前而言ints对于算法没有用，所以没有做，所以千万别用ints，免得意外惊喜！
// =============================================================================

static int  break_vncclip_BbyA(struct tagClipRect *A, struct tagClipRect *B,\
    struct tagClipRect **different, struct tagClipRect **ints)
{
    int  ret = 0;
    int  intsTop = 0, intsLeft = 0, intsRight=0, intsBottom=0;
    struct tagClipRect  *temp = NULL, *curque = NULL;
    struct tagRectangle    rec;

    intsTop = (A->rect.top > B->rect.top? A->rect.top : B->rect.top);
    intsLeft = (A->rect.left > B->rect.left? A->rect.left : B->rect.left);
    intsRight = (A->rect.right < B->rect.right? A->rect.right : B->rect.right);
    intsBottom = (A->rect.bottom < B->rect.bottom? A->rect.bottom : B->rect.bottom);
            //上面矩形
    rec.top = B->rect.top;
    rec.bottom = intsTop;
    rec.left = B->rect.left;
    rec.right = B->rect.right;
    if(IsRealRec(rec))
    {
        temp =  get_vncclip_space();
        if(temp)
        {
            if(curque)
            {
                temp->rect = rec;
                temp->previous = curque;
                temp->next = curque->next;
                curque->next->previous = temp;
                curque->next = temp;
                curque = temp;
            }
            else
            {
                temp->rect = rec;
                temp->previous = temp;
                temp->next = temp;
                curque = temp;
            }
        }
    }
            //下面矩形
    rec.top = intsBottom;
    rec.bottom = B->rect.bottom;
    rec.left = B->rect.left;
    rec.right = B->rect.right;
    if(IsRealRec(rec))
    {
        temp =  get_vncclip_space();
        if(temp)
        {
            if(curque)
            {
                temp->rect = rec;
                temp->previous = curque;
                temp->next = curque->next;
                curque->next->previous = temp;
                curque->next = temp;
                curque = temp;
            }
            else
            {
                temp->rect = rec;
                temp->previous = temp;
                temp->next = temp;
                curque = temp;
            }
        }
    }
            //左边矩形
    rec.top = intsTop;
    rec.bottom = intsBottom;
    rec.left = B->rect.left;
    rec.right = intsLeft;
    if(IsRealRec(rec))
    {
        temp =  get_vncclip_space();
        if(temp)
        {
            if(curque)
            {
                temp->rect = rec;
                temp->previous = curque;
                temp->next = curque->next;
                curque->next->previous = temp;
                curque->next = temp;
                curque = temp;
            }
            else
            {
                temp->rect = rec;
                temp->previous = temp;
                temp->next = temp;
                curque = temp;
            }
        }
    }
            //右边矩形
    rec.top = intsTop;
    rec.bottom = intsBottom;
    rec.left = intsRight;
    rec.right = B->rect.right;
    if(IsRealRec(rec))
    {
        temp =  get_vncclip_space();
        if(temp)
        {
            if(curque)
            {
                temp->rect = rec;
                temp->previous = curque;
                temp->next = curque->next;
                curque->next->previous = temp;
                curque->next = temp;
                curque = temp;
            }
            else
            {
                temp->rect = rec;
                temp->previous = temp;
                temp->next = temp;
                curque = temp;
            }
        }
    }
    if(curque)
    {
        *different = curque;
        return SUCCESS ;
    }
    else
    {
        return FAILED;
    }

}

// =============================================================================
// 函数功能:判断A和B谁更适合用来击碎clip2break
// 输入参数:
// 输出参数:
// 返回值  :1 A更适合， 0 B更适合
// 说明    :规则是看A和B究竟谁和clipbreaked的公共面积大，面积大的自然最合适（产生碎片小，容易吸收）
//          需要改进的是，这个计算存在多次冗余，可以用rect存储，不用每次计算要计算的
// =============================================================================
//--TODO
static int judge_proper_clipbreaker(struct tagClipRect *A,\
             struct tagClipRect *B, struct tagClipRect  *clip2bebroken)
{
    struct tagRectangle  ints1;//A相交的面积
    struct tagRectangle  ints2;//B相交的面积

    ints1.top =(A->rect.top > clip2bebroken->rect.top? A->rect.top:clip2bebroken->rect.top);
    ints1.left = (A->rect.left > clip2bebroken->rect.left? A->rect.left:clip2bebroken->rect.left);
    ints1.right = (A->rect.right < clip2bebroken->rect.right?A->rect.right:clip2bebroken->rect.right);
    ints1.bottom = (A->rect.bottom < clip2bebroken->rect.bottom?A->rect.bottom:clip2bebroken->rect.bottom);

    ints2.top =(B->rect.top > clip2bebroken->rect.top? B->rect.top:clip2bebroken->rect.top);
    ints2.left = (B->rect.left > clip2bebroken->rect.left? B->rect.left:clip2bebroken->rect.left);
    ints2.right = (B->rect.right < clip2bebroken->rect.right?B->rect.right:clip2bebroken->rect.right);
    ints2.bottom = (B->rect.bottom < clip2bebroken->rect.bottom?B->rect.bottom:clip2bebroken->rect.bottom);

    if(((ints1.bottom-ints1.top)*(ints1.right-ints1.left)) > ((ints2.bottom-ints2.top)*(ints2.right-ints2.left)))
    {
        return 1;
    }
    else
    {
        return 0;
    }

}



// ============================================================================
// 函数功能:将newclip归并到head队列中
// 输入参数:head，clip队列。newclip，待归并的clip块
// 输出参数:
// 返回值  :
// 说明    :小的newclip会直接被吸收，boss级别的会init原clip然后free掉，cross的会击碎，
//          然后被原队列吸收掉
// ============================================================================
//将新增的clip加入到clip队列中，事实上并不是每个clip都需要加入，或者加入完整的。
//
//当state为included的时候，直接将当前clip从比较队列删除
//当state为including的时候，该clip将不可以被击碎，在内循环结束时候从比较队列删除
//当state为ISLAND的时候，表示clip可以继续下次比较，如果在队尾还是ISLANGD状态，
//则从比较队列移除，加入剩余队列，程序会在最后将这部分队列加入到总队列
//当state为CROSSING的时候，可以继续向下比较，当最后仍然为CROSING，则用最合适的clip
//将其击碎
//内部使用，不再进行参数检查，如果外部使用，请保证clip队列的正确性
//newclip要么被转移，要么被查分删除，请不要再free或者使用
void anastomose_vncclip(struct tagClipRect **head, struct tagClipRect *newclip)
{
    int clipstate = ANYOPER,clipstatebak = ANYOPER;
    struct tagClipRect *startCompare = NULL;
    struct tagClipRect *properBreakclip = NULL;//最合适用来击碎当前clip的块
    struct tagClipRect *islandClip = NULL;//用来收留那些到死都是光棍的clip
    struct tagClipRect *clipCompared = NULL;
    struct tagClipRect *temp;//

    struct tagClipRect *different = NULL, *ints=NULL;
    //目前为止ints没有使用，后续要使用的话请做好内存的泄露防备工作

    clipCompared = newclip;

    if(NULL == *head )
    {
        *head = newclip;
        return;
    }
    else if(NULL==newclip)
    {
        return;
    }
    else
    {
      //正常处理
    }

    while(clipCompared != NULL)//一直比较，直到将newcip的
    {
        startCompare = *head;
        //Compared = clipCompared;
        do
        {

            clipstate=judge_position_B2A(startCompare,clipCompared);
            if(clipstate == INCLUDING) //待比较的clip包含原来的clip块
            {
                clipstatebak = clipstate;
                startCompare->rect = clipCompared->rect;
                startCompare = startCompare->next;
            }
            else if(clipstate == INCLUDED) //被包含
            {
                temp = clipCompared;
                clipCompared = remove_clip_from_cliplink(&clipCompared);
                free_vncclip_space(temp);
                clipstatebak = ANYOPER;
                break;
            }
            else if(clipstate == CROSSING) //相交
            {
                if(clipstatebak != INCLUDING)
                {
                    if(NULL == properBreakclip)
                    {
                        properBreakclip = startCompare;

                    }
                    else
                    {
                        if(judge_proper_clipbreaker(startCompare, properBreakclip,clipCompared))
                        {
                            properBreakclip = startCompare;
                        }
                        else
                        {
                            // 不做任何改变
                        }
                    }
                    clipstatebak = clipstate;
                }
                else
                {
                    //不做任何改变
                }
                startCompare = startCompare->next;
            }
            else//孤立
            {
                if( (clipstatebak== ISLAND)||(clipstatebak == ANYOPER))
                {
                    clipstatebak = ISLAND;
                }
                startCompare = startCompare->next;
            }
        }while(*head != startCompare); //小循环结束
        if(clipstatebak == INCLUDING)//此clip已经毫无用处，可以删除
        {
           temp = clipCompared;
           clipCompared = remove_clip_from_cliplink(&clipCompared);
           free_vncclip_space(temp);
         //  properBreakclip = NULL;
          // clipstatebak = ANYOPER;
        }
        else if(clipstatebak == CROSSING) //需要击碎
        {
            //击碎clip
             break_vncclip_BbyA(properBreakclip, clipCompared, &different, &ints);
             connect_clip2cliplink(&clipCompared, different);
            //删除clip
             temp= clipCompared;
             clipCompared = remove_clip_from_cliplink(&clipCompared);
             free_vncclip_space(temp);
        }
        else if(clipstatebak == ISLAND)//将此clip移动到island队列中
        {
            temp = clipCompared;
            clipCompared = remove_clip_from_cliplink(&clipCompared);
            temp->previous = temp;
            temp->next = temp;
            connect_clip2cliplink(&islandClip, temp);
        }
        else//肯定是INCLUDED引起的，基本上
        {
           //clipcompared 被原队列吞掉了，最好的事情
        }
        clipstatebak = ANYOPER;
        properBreakclip = NULL;
        startCompare = *head;
    }//主循环结束

    //将island的队列加入到 原来队列中
    connect_clip2cliplink(head, islandClip);
}
//----合并区域简化版-----------------------------------------------------------
//功能: 把给定的剪切域队列中可合并的区域合并，简化版的意思是，队列中的区域已经按
//      左->右，从上->下的顺序排列，合并后，多余的clip将被释放。
//参数: clipq: 指向待合并的队列首
//返回: 无
//-----------------------------------------------------------------------------
struct tagClipRect *combine_vncclip_s(struct tagClipRect *clipq)
{
    struct tagClipRect *clip,*clip1,*tempclip;
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
            free_vncclip_space(clip1);
            clip1 = clip->next;
        }
        else if((clip1->rect.top == clip->rect.top) //存在剪切域相同的情况
            &&(clip1->rect.bottom == clip->rect.bottom)
            &&(clip1->rect.left == clip->rect.left)
            &&(clip1->rect.right== clip->rect.right))
        {
            clip->next = clip1->next;
            clip1->next->previous = clip;
            free_vncclip_space(clip1);
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
                free_vncclip_space(tempclip);
            }else
                clip1 = clip1->next;
        }
        clip = clip->next;
    }while(clip != clipq);
    return clipq;
}

// =============================================================================
// 函数功能:判断clipA是不是在clipB的右下方
// 输入参数:
// 输出参数：
// 返回值  :1 是的，0 不是
// 说明    :水平的权重更高
// =============================================================================
static int AIsBR2B_vnc(struct tagClipRect *A, struct tagClipRect *B)
{
    if((A->rect.top > B->rect.top) || \
    ((A->rect.top == B->rect.top) && (A->rect.left > B->rect.left)))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//----合并区域通用版-----------------------------------------------------------
//功能: 把给定的剪切域队列中可合并的区域合并，与简化版不同的是，队列中的clip是
//      随机排列的。方法是:
//      1、把所有clip按先y后x坐标排序，使队列变成左→右，再上→下排序的。
//      2、调用__gk_combine_clip_s函数进行排序。
//参数: clipq: 指向待合并的队列首
//返回: 返回的是排好顺序的队首
//-----------------------------------------------------------------------------

struct tagClipRect * combine_vnccliplink(struct tagClipRect *clipq)
{

    struct tagClipRect  *startclip, *endclip,  *max;
    struct tagClipRect  *A, *B, *C, *D;
    int    firstTime=0;

    if(clipq == NULL)       //队列是空的
        return NULL;
    else if(clipq->next == clipq)   //队列只有一个clip
        return clipq;
    //下面用冒泡排序法把clipq中的剪切域按左→右，再上→下排序
    //剪切域很多时，冒泡法会比较慢，改进之。--db
    //startclip为每次冒泡开始的地方
    //endclip为每次冒泡结束的地方
    //排序的方法是，第一次将最大的max沉到最后面，然后每次都从max->next开始，
    //找一个次大的，直到所start和end相同
    startclip = clipq;
    endclip = startclip->previous;

    do
    {
        A = startclip;
        firstTime++;
        do
        {
            B = A->next;
            if(AIsBR2B_vnc(A, B))//A比B更右下，所以需要交换AB的位置
            {
                if(A == B->next)//队列中仅仅有两个clip
                {
                    //之所以这样，我们希望A指向的总是最max的
                }
                else//将A和B的位置调换，注意A和B是相邻的
                {
                    C = A->previous;
                    D = B->next;
                    C->next = B;
                    D->previous = A;

                    A->previous = B;
                    A->next = D;
                    B->next = A;
                    B->previous = C;
                }
            }
            else
            {
                A = B;
            }
        }while(B != endclip);
        if(firstTime ==1)//主循环跑一次就可以知道最大的是哪个了
        {
            max=A;
        }
        endclip = A;
        startclip = max->next;
        endclip = endclip->previous;
    }while(startclip!= endclip);

    return  combine_vncclip_s(startclip);
}
