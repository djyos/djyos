//-----------------------------------------------------------------------------
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
//
//
//
#include <string.h>
#include <stdlib.h>
#include <systime.h>
#include <pool.h>
#include <djyos.h>
#include "mount.h"
#include "vfile.h"

//
// 本地变量
//
extern char g_cPathSegment[257]; // ./fs_module.c中定义;
//-----------------------------------------------------------------------------
//功能: 路径检索初始化
//参数: PathContext -- 路径检索的上下文;
//      Path -- 路径;
//返回: 非NULL -- 成功; NULL -- 失败;
//备注:
//-----------------------------------------------------------------------------
void __PathInit(struct PathWalk *PathContext, const char *Path)
{
    struct VCommon *FileCom;
    struct VMount *Mount;

    PathContext->PrivateVHost = NULL;
    PathContext->Private = NULL;
    PathContext->FileOps = NULL;

    if(Path[0] == '/')// 绝对路径
    {
        PathContext->End = g_tSysRoot.Obj;
        PathContext->VOps = g_tSysRoot.VOps;
    }
    else// 其他都是相对路径
    {
        PathContext->End = g_tSysPWD.Obj;
        PathContext->VOps = g_tSysPWD.VOps;
    }

    PathContext->Path = PathContext->UncoveredPath = (char*)Path;

    // 获取文件系统的根名
    if(PathContext->End->Type & OB_MOUNT)
    {
        FileCom = Container(PathContext->End, struct VCommon, Obj);
        Mount = (struct VMount*)(FileCom->Context);
        PathContext->Root = Mount->Name;
    }
    else
    {
        PathContext->Root = g_tSysPWD.Path; // 情景: 当前工作路径设置到了具体文件系统内部的目录
        PathContext->FileOps = g_tSysPWD.FileOps;
    }
}
//-----------------------------------------------------------------------------
//功能: 在虚拟缓冲中检索文件
//参数: PathContext -- 路径检索的上下文;
//      Open -- 1表示在Open逻辑中使用
//返回: 1 -- 找到目标文件; 0 -- 未找到目标文件;
//备注:
//-----------------------------------------------------------------------------
u8 __VPathLookup(struct PathWalk *PathContext, u8 Open)
{
    struct Object *Current, *Parent;
    char *PathCur = PathContext->Path;
    char *PathNext;
    struct VMount *Mount;
    struct VCommon *FileCom;
    u8 Length = 0;

    Current = PathContext->End;
    while(1)
    {
        while(*PathCur == '/')
            PathCur++; // 过滤多余的'/'

        if(*(PathCur) == '\0')
            break; // 遍历路径结束

        Parent = Current;


        // 找到下一个文件或目录名，用PathCur指向文件名的起始，PathNext指向文件名结束（的下一个字符）
        PathNext = PathCur;
        while(1)
        {
            if((*PathNext == '/') || (*PathNext == '\0'))
                break;
            PathNext++;
        }

        if(PathCur[0] == '.' && PathCur[1] == '.')
        {
            // 如果当前节点的父为最顶层，当前节点不变.容错处理，如果其父为空的话，current节点不变.
            if(Current->Parent)
                Current = Current->Parent;
        }
        else if(PathCur[0] != '.')
        {
            // 查找字符串，需要字符串格式，这里为了节省空间，这里原计划直接在原字符串上改，但实现不了,因为const
            if(*PathNext != '\0')
            {
                Length = PathNext - PathCur;
                memcpy(g_cPathSegment, PathCur, Length);
                g_cPathSegment[Length] = '\0';
                Current = OBJ_SearchChild(Current, g_cPathSegment);
            }
            else
                Current = OBJ_SearchChild(Current, PathCur);
        }
        // 先不管有没有找到，先对父节点进行判断。因为路径运行到这个位置，如果父节点是挂载点，
        // 那么就考虑到进入了另一个文件系统了。
        if(Parent->Type & OB_MOUNT)
        {
            PathContext->Path = PathCur;// 文件系统的绝对路径
            FileCom = Container(Parent, struct VCommon, Obj);
            Mount = (struct VMount*)(FileCom->Context);
            PathContext->VOps = Mount->VOps;
            PathContext->FileOps = Mount->Private;
            PathContext->Root = Mount->Name;// 文件系统的根名
        }

        if(NULL == Current)
            break;// 遍历中断，路径上出现不存在(缓存)的节点

        PathCur = PathNext;
    }

    if(NULL != Current)
    {
        PathContext->UncoveredPath = NULL;
        PathContext->End = Current;

        // 情景：打开目标是文件系统的挂载点,此处相当于打开一个文件系统的根目录,
        // 那么需要获取其操作方法
        if((Current->Type & OB_MOUNT) && Open)
        {
            PathContext->Path = NULL;
            FileCom = Container(Current, struct VCommon, Obj);
            Mount = (struct VMount*)(FileCom->Context);
            PathContext->VOps = Mount->VOps;
            PathContext->FileOps = Mount->Private;
            PathContext->Root = Mount->Name;// 文件系统的根名
        }

        return (1);// 在缓存中找到了目标
    }
    else
    {
        PathContext->UncoveredPath = PathCur;
        PathContext->End = Parent;

        return (0);// 在缓存中未找到目录
    }
}

//-----------------------------------------------------------------------------
//功能: 在虚拟缓冲中补齐路径缺失的目录。
//参数: Path -- 在虚拟缓冲中缺失的路径;
//      Obj -- 路径起点;
//返回: 非NULL -- 成功; NULL -- 失败,内存不足或路径节点已存在.
//备注: 没有处理“////”的情景，因为前期的函数可以保证不会出现这种情景。
//-----------------------------------------------------------------------------
struct VCommon *__CoverPath(const char *Path, struct Object *Obj)
{
    struct VCommon *FileCom = NULL;
    struct Object *Parent = Obj;
    struct Object *Child;
    char *PathCur = (char *)Path;
    char *PathNext, *Slash;
    u8 Last = 0;
    u16 Length;

    while(1)
    {
        while(*PathCur == '/')
            PathCur++;// 过滤多余的'/'

        PathNext = PathCur;

        while(1)
        {
            if('\0' == *PathNext)
            {
                Last = 1;
                break;
            }

            if('/' == *PathNext)// 判断路径'/'后面是否就直接结束了
            {
                Slash = PathNext;
                Slash++;
                while(1)
                {
                    if('/' == *Slash)
                    {
                        Slash++;
                        continue;
                    }
                    else if('\0' == *Slash)
                        Last = 1;
                    break;// '/'后面跟随着是非'/'和'\0'字符
                }
                break;// '/'后面跟随着是非'/'和'\0'字符
            }

            PathNext++;
        }

        Length = PathNext - PathCur;// 当前文件名长度

        if(PathCur[0] == '.' && PathCur[1] == '.')// 如果当前节点的父为最顶层，当前节点不变
        {
            if(Parent->Parent)
                Parent = Parent->Parent;// 容错处理，如果其父为空的话，Parent节点不变
        }
        else if(PathCur[0] != '.')
        {
            if(*PathNext != '\0')
            {
                Length = PathNext - PathCur;
                memcpy(g_cPathSegment, PathCur, Length);
                g_cPathSegment[Length] = '\0';
                Child = OBJ_SearchChild(Parent, g_cPathSegment);
            }
            else
                Child = OBJ_SearchChild(Parent, PathCur);


            if(NULL == Child)
            {
                Length += (sizeof(struct VCommon) + 1);// 1是'\0'

                FileCom = (struct VCommon*)(malloc(Length));
                if(NULL == FileCom)
                    return (NULL);

                if(*PathNext != '\0')
                    strcpy(FileCom->Name, g_cPathSegment);
                else
                    strcpy(FileCom->Name, PathCur);

                FileCom->Context = NULL;

                Child = OBJ_AddChild(Parent, &(FileCom->Obj), Length, (u16)OB_FILE, FileCom->Name);
            }

            Parent = Child;
        }

        if(Last)
            return ((NULL == Parent) ? NULL : Container(Parent, struct VCommon, Obj)); // todo
        else
            PathCur = PathNext;
    }
}

//-----------------------------------------------------------------------------
//功能: 打开文件
//参数: Path -- 文件路径;
//      Mode -- 文件操作模式;
//返回: 非NULL -- 成功; NULL -- 失败;
//备注:
//-----------------------------------------------------------------------------
struct VFile *__OpenInner(const char *Path, u32 Mode, u8 Lock)
{
    struct PathWalk PContext;
    struct VFile *Fp;
    u8 IsFound;
    s32 Ret = -1;

    if(Lock)
        Lock_MutexPend(g_ptVFMutex, CN_TIMEOUT_FOREVER);// 进互斥

    // 初始化PContext
    __PathInit(&PContext, Path);

    IsFound = __VPathLookup(&PContext, 1);// 缓存中检索

    if(PContext.VOps && PContext.VOps->VOpen)// 系统文件不支持该函数
        Ret = PContext.VOps->VOpen(&PContext, Mode, IsFound);

    if(Lock)
        Lock_MutexPost(g_ptVFMutex);// 出互斥

    if(Ret < 0)
    {// 错误发生了
        Fp = NULL;
    }
    else
    {// 成功打开
        Fp = AllocVFile();
        if(NULL != Fp)
        {
            if(PContext.PrivateVHost) // 防止跑飞
                *(PContext.PrivateVHost) = Fp;
            Fp->Private = PContext.Private; // FileContext
            Fp->VOps = PContext.VOps;
        }
    }

    return (Fp);
}

//-----------------------------------------------------------------------------
//功能: 打开文件,互斥方式
//参数: Path -- 文件路径;
//      Mode -- 文件操作模式;
//返回: 非NULL -- 成功; NULL -- 失败;
//备注:
//-----------------------------------------------------------------------------
struct VFile *__Open(const char *Path, u32 Mode)
{
    return (__OpenInner(Path, Mode, 1));
}

//-----------------------------------------------------------------------------
//功能: 打开文件,非互斥方式
//参数: Path -- 文件路径;
//      Mode -- 文件操作模式;
//返回: 非NULL -- 成功; NULL -- 失败;
//备注:
//-----------------------------------------------------------------------------
struct VFile *__OpenUnlock(const char *Path, u32 Mode)
{
    return (__OpenInner(Path, Mode, 0));
}

//-----------------------------------------------------------------------------
//功能: 申请文件描述结构体
//参数:
//返回: 非NULL -- 成功; NULL -- 失败;
//备注:
//-----------------------------------------------------------------------------
struct VFile *AllocVFile(void)
{
    struct VFile *Fp = NULL;
    struct VFilePool *Pool = g_ptVFilePool;

    Lock_MutexPend(g_ptVFilePoolMutex, CN_TIMEOUT_FOREVER);
    while(Pool)
    {
        Fp = (struct VFile*)Mb_Malloc(Pool->Management, 0);
        if(Fp)
        {
            Pool->Used++;// 获取成功,计数增一;
            break;
        }

        // 当前内存池已用完，查找下一个，或扩展内存池;
        if(NULL == Pool->Next)
            Pool = __ExtendVFilePool();
        else
            Pool = Pool->Next;
    }
    Lock_MutexPost(g_ptVFilePoolMutex);

    return (Fp);
}
//-----------------------------------------------------------------------------
//功能: 释放文件描述结构体
//参数: File -- 文件描述结构体;
//返回: 非NULL -- 成功; NULL -- 失败;
//备注:
//-----------------------------------------------------------------------------
void FreeVFile(struct VFile *File)
{
    struct VFilePool *Pool = PoolOf(File);

    Lock_MutexPend(g_ptVFilePoolMutex, CN_TIMEOUT_FOREVER);

    Mb_Free(Pool->Management, File);

    if((!(--Pool->Used)) && g_ptVFilePool->Next)// 删除Pool，但至少保留一个Pool不被删除
    {
        if(Pool != g_ptVFilePool)
        {
            struct VFilePool *PrePool = g_ptVFilePool;
            while(PrePool)
            {
                if(PrePool->Next == Pool)
                {
                    PrePool->Next = Pool->Next;
                    break;
                }
                PrePool = PrePool->Next;
            }
        }
        else
            g_ptVFilePool = Pool->Next;

        Mb_DeletePool(Pool->Management);// 注销内存池
        free(Pool->Space);// 释放空间
        free(Pool);
    }

    Lock_MutexPost(g_ptVFilePoolMutex);
}
//-----------------------------------------------------------------------------
//功能: 扩展文件描述结构体空间.
//参数:
//返回: 非NULL -- 成功; NULL -- 失败;
//备注: 调用本函数，需注意互斥处理。
//-----------------------------------------------------------------------------
struct VFilePool *__ExtendVFilePool(void)
{
    struct VFilePool *New, *Next, *Cur;
    u16 ID = 0;

    New = (struct VFilePool*)malloc(sizeof(struct VFilePool));
    if(NULL == New)
        return (NULL);

    New->Space = (struct VFile*)malloc(VFILE_POOL_SIZE);
    if(NULL == New->Space)
    {
        free(New);
        return (NULL);
    }

    memset(New->Space, 0, VFILE_POOL_SIZE);// 内容初始化

    New->Management = (struct MemCellPool*)Mb_CreatePool(New->Space,
                       VFILE_NUM, VFILE_SIZE, 0, 0, "vfile pool");
    New->Used = 0;

    // 将新VFilePool链入链表,链表是按ID号的从小到大排序.
    if((g_ptVFilePool) && (0 == g_ptVFilePool->ID))
    {
        Cur = g_ptVFilePool;
        while(1)
        {
            ID++;
            Next = Cur->Next;
            if(Next == NULL || Next->ID != ID)
                break;
        }
        New->ID = ID;
        New->Next = Next;
        Cur->Next = New;
    }
    else
    {
        New->ID = ID;
        New->Next = g_ptVFilePool;
        g_ptVFilePool = New;
    }

    return (New);
}


//-----------------------------------------------------------------------------
//功能: 获取文件描述结构体所在的空间.
//参数: Fp -- 文件描述结构体;
//返回: 文件描述结构体所在的空间;
//备注:
//-----------------------------------------------------------------------------
struct VFilePool *PoolOf(struct VFile *Fp)
{
    struct VFilePool *Pool = g_ptVFilePool;

    // 获取Fp所在VFile池空间
    while(Pool)
    {
        if((Pool->Space <= Fp) && (Fp < (Pool->Space + VFILE_NUM)))
            break;// 找到，地址落在了Pool内
        Pool = Pool->Next;
    }

    return(Pool);
}

//-----------------------------------------------------------------------------
//功能: 申请文件缓冲区
//参数: BufRet -- 缓冲区返回;
//      Force -- 1-强制申请, 0 - 非强制;
//返回: 0 -- 成功; -1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
const u32 gc_CONFIG_FS __attribute__ ((weak));
#define BUF_SIZE        	(512)
s32 AllocFileBuf(struct FileBuf **BufRet, u8 Force)
{
    struct FileBuf *Buf = NULL;

    if((gc_CONFIG_FS & FS_BUFFERED) || (Force))
    {
        Buf = (struct FileBuf*)malloc(sizeof(struct FileBuf));
        if(!Buf)
            return (-1);

        Buf->Start = Buf->Current = (u8*)malloc(BUF_SIZE);
        if(!Buf->Start)
        {
            free(Buf);
            return (-1);
        }

        Buf->End = Buf->Start + BUF_SIZE;
        Buf->Status = BUF_CLEAN;
    }

    *BufRet =  Buf;
    return (0);
}
//-----------------------------------------------------------------------------
//功能: 释放文件缓冲区.
//参数: Buf -- 文件缓冲区;
//返回:
//备注:
//-----------------------------------------------------------------------------
void FreeFileBuf(struct FileBuf *Buf)
{
    if(Buf)
    {
        free(Buf->Start);
        free(Buf);
    }
}

//-----------------------------------------------------------------------------
//功能: 路径转换
//参数: RelPath -- 相对路径;
//      Root -- RelPath路径的起点;
//返回:
//备注: 上级系统保证了互斥，这里不考虑互斥
//-----------------------------------------------------------------------------
char *PathCopy(const char *Root, const char *RelPath, char (*Rebuild)(char *Path))
{
    u8 Length ;

    if(Root) // 没有的话，则是个相对路径
    {
        Length = strlen(Root);
        memcpy(g_cPathSegment, Root, Length);
        if(RelPath && ('/' != RelPath[0]))
        	g_cPathSegment[Length++] = '/';
    }

    if(RelPath)
        strcpy((g_cPathSegment+Length), RelPath); // 拼接完整的路径
    else
        g_cPathSegment[Length] = '\0'; // 只有根目录

    if(Rebuild)
        Rebuild(g_cPathSegment);

    return(g_cPathSegment);
}

//-----------------------------------------------------------------------------
//功能: 申请文件上下文
//参数:
//返回: !NULL -- 成功; NULL -- 失败;
//备注:
//-----------------------------------------------------------------------------
struct FileContext *AllocContext(void)
{
    struct FileContext *FileCt;

    FileCt = malloc(sizeof(struct FileContext));
    if(!FileCt)
        return (NULL);

    memset(FileCt, 0, sizeof(struct FileContext));

    return (FileCt);
}

//-----------------------------------------------------------------------------
//功能: 释放文件上下文.
//参数: FileCt -- 文件上下文;
//返回:
//备注:
//-----------------------------------------------------------------------------
void FreeContext(struct FileContext * FileCt)
{
    if(FileCt)
        free(FileCt);
}
//-----------------------------------------------------------------------------
//功能: 将Member插入队列成员MemInList之后
//参数: ListMem  --
//      New -- 新加添的成员
//返回:
//备注:
//-----------------------------------------------------------------------------
void __ListInsert(struct FileContextList *MemInList, struct FileContextList *New)
{
    New->Pre = MemInList;
    New->Next = MemInList->Next;
    if(MemInList->Next)
        MemInList->Next->Pre = New;
    MemInList->Next = New;
}
//-----------------------------------------------------------------------------
//功能: 打开文件
//参数: PathContext  -- 路径上下文;
//      Mode -- 文件操作模式;
//      IsFound -- 文件是否已缓存;1:已缓存;0:未缓存;
//返回: 1 -- 新打开一个文件(未打开过); 2 -- 打开一个文件(已打开过);
//      -1 -- 文件类型错误(输入参数错误); -2 -- 文件无法打开; -3 -- 待定错误;
//备注: 不做函数是否注册检查，
//-----------------------------------------------------------------------------
s32 FOpen(struct PathWalk *PathContext, u32 Mode, u8 IsFound)
{
    struct FileContext *FileCt;
    struct VCommon *FileCom;
    struct FileOperations *FileOps;
    struct VMount *Mount;
    char *PathTemp;
    u32 Length;
    s32 Ret;
    struct Object *End = PathContext->End;

    if(!(End->Type & OB_FILE))// 类型校验
        return (-1);

    FileOps = (struct FileOperations *)__FileOps(PathContext);

   if(IsFound)// 文件在缓存中发现了，可能被打开过或者是临时文件
   {
        struct FileContext **ExFileCt;// 已存在的上下文件链
        u16 TypeCheck = End->Type & (~OB_FILE);

        FileCom = Container(End, struct VCommon, Obj);

        if((Mode & M_TYPE) &&
            TypeCheck &&
            !(((Mode & M_REG) && (TypeCheck & OB_REG)) ||
              ((Mode & M_DIR) && (TypeCheck & OB_DIR)) ||
              ((Mode & M_LINK) && (TypeCheck & OB_LINK))))
            return (-1); // 目标类型与Mode不一致

        FileCt = FileOps->FileOpen(PathContext->Path, Mode, PathContext->Root);
        if(NULL == FileCt)
            return (-2); // 文件无法打开

        if(End->Type & OB_MOUNT)// 当前节点是安装点
        {
            Mount = (struct VMount*)(FileCom->Context);

            while(Mount->Property & MOUNT_ON_MOUNT)// 存在安装点覆盖,此时寻找的是文件的上下文
                Mount = (struct VMount*)Mount->Context;

            ExFileCt = (struct FileContext**)&(Mount->Context);
        }
        else
            ExFileCt = (struct FileContext**)&(FileCom->Context);// 普通文件

        FileCt->CHost = FileCom;
        if(*ExFileCt)
            __ListInsert(&((*ExFileCt)->List), &(FileCt->List));
        else
            *ExFileCt = FileCt; // 适用如下情景：系统根等,因为这种节点建立时不建立上下文

        Ret = 2;
   }
   else
   {
       PathTemp = PathContext->UncoveredPath;
       Length = strlen(PathTemp);
       if(PathTemp[Length-1] == '/')
       {
           if((Mode & M_TYPE) && (M_DIR != (Mode & M_TYPE)))
               return (-1);// 模式与路径名不符,例如是M_REG

           Mode |= M_DIR;// 模式上未设置模式，这里将其设置为称为目录
       }

       FileCt = FileOps->FileOpen(PathContext->Path, Mode, PathContext->Root);// 打开物理设备中的目标文件
       if(NULL == FileCt)
           return (-2);// 目标文件无法打开

       FileCom = __CoverPath(PathContext->UncoveredPath, End);// 在内存中建立路径
       if(FileCom)
       {
           switch(FileCt->Property & P_TYPE) //todo:
           {
               case P_REG: FileCom->Obj.Type |= OB_REG; break;
               case P_DIR: FileCom->Obj.Type |= OB_DIR; break;
               case P_LINK: FileCom->Obj.Type |= OB_LINK; break;
               default :
                   FileOps->FileClose(FileCt);
                   return (-3);// 其他类型的应该是错误的。
           }
           FileCom->Context = (void*)FileCt;
           FileCt->CHost = FileCom;
           Ret = 1;
       }
       else
       {
           FileOps->FileClose(FileCt);// 发生错误,关闭已打开的目标文件
           Ret = -3; // 内存不足(建立路径失败)
       }
   }

   if(Ret > 0)
   {
       PathContext->Private = FileCt;
       PathContext->PrivateVHost = &(FileCt->FHost);
   }

    return (Ret);
}
//-----------------------------------------------------------------------------
//功能: 将成员从队列中删除，并返回队列的头部（动作完成之后的队列）
//参数: Mem -- 需被删除的队列成员
//返回:
//备注:
//-----------------------------------------------------------------------------
struct FileContextList *__ListDel(struct FileContextList *Mem)
{
    struct FileContextList *Head; // List头
    struct FileContextList *PreMem = Mem->Pre;
    struct FileContextList *NextMem = Mem->Next;

    // 成员Mem剔除出队列
    if(PreMem)
        PreMem->Next = NextMem;

    if(NextMem)
        NextMem->Pre = PreMem;

    // 获取队列头
    if(PreMem)
    {
        Head = PreMem;
        while(Head->Pre)
            Head = Head->Pre; // 获取队列头
    }
    else
    {
        Head = NextMem; // 本次删除的是队列头，所以后续的成员就称为了队列头包括NULL
    }

    return (Head);
}
//-----------------------------------------------------------------------------
//功能: 关闭文件
//参数: File -- 文件上下文;
//返回: 0 -- 成功;
//      -1 -- 参数错误; -2 -- 文件类型错误; 其他;
//备注:
//-----------------------------------------------------------------------------
s32 FClose(void *File)
{
    struct FileContext *FileCt;
    struct VCommon *FileCom;
    struct FileOperations *FileOps;
    s32 Ret;
    u32 BufferedDataLen;
    struct FileContextList *Head;
    struct FileContext *FileCtHead = NULL;

    if(NULL == File)
        return (-1);

    FileCt = (struct FileContext*)File;
    FileCom = FileCt->CHost;

    if(!(FileCom->Obj.Type & OB_FILE))// 类型校验
        return (-2);

    FileOps = (struct FileOperations*)(FileCt->FileOps);
    if((NULL == FileOps) || (NULL == FileOps->FileClose))
        return (-2); // 函数未注册

    // 文件关闭时,将缓冲数据写入
    if((FileCom->Obj.Type & OB_REG) && FileCt->Buf)
    {
        if(BUF_DIRTY == FileCt->Buf->Status)
        {
            BufferedDataLen = FileCt->Buf->Current - FileCt->Buf->Start;
            if(BufferedDataLen !=
                    FileOps->FileWrite(FileCt->Buf->Start, 1, BufferedDataLen, FileCt))
            {
                return (-2); // 文件写入失败
            }

            FileCt->Buf->Status = BUF_CLEAN;
            FileCt->Buf->Current = FileCt->Buf->Start;
        }
    }
    Lock_MutexPend(g_ptVFMutex,CN_TIMEOUT_FOREVER);// 进互斥

#if 0 // 暂时不开启这个功能
    if(FileCom->Obj.Type & OB_DIR)
        __FlushDir(FileCt->FHost);
#endif

    Head = __ListDel(&(FileCt->List)); // 先将成员从其所属队列中删除
    if(Head)
        FileCtHead = Container(Head, struct FileContext, List); // 用于VCommon

    Ret = FileOps->FileClose(FileCt); // 关闭物理设备中的文件
    if(Ret)
    {
        // 文件未关闭成功，则将上下文重接入链表,且队列头可能由于上面的删除发生了变化
        if(Head)
        {
            __ListInsert(Head, &(FileCt->List));
            if(FileCom->Obj.Type & OB_MOUNT)
            {
                struct VMount *Mount = (struct VMount*)FileCom->Context;

                while(Mount->Property & MOUNT_ON_MOUNT)
                    Mount = (struct VMount*)Mount->Context;

                Mount->Context = FileCtHead;// 当前目录是挂载点，只设置上下文为NULL
            }
            else
            {
                FileCom->Context = FileCtHead; // 先设置上下文为NULL,再考虑是否删除
            }
        }
    }
    else // 成功关闭
    {
        // 删除文件在内存节点缓存;
        // 文件节点缓存不能被删除的条件包括：存在子节点，存在上下文, 被引用(SysRoot、SysPWD、挂载点)
        if(FileCom->Obj.Type & OB_MOUNT)
        {
            struct VMount *Mount = (struct VMount*)FileCom->Context;

            while(Mount->Property & MOUNT_ON_MOUNT)
                Mount = (struct VMount*)Mount->Context;

            Mount->Context = FileCtHead;// 当前目录是挂载点，只设置上下文为NULL
        }
        else
        {
            FileCom->Context = FileCtHead; // 先设置上下文为NULL,再考虑是否删除
            if(NULL == FileCtHead) // 文件的上下文已经全部被关闭
            {
                while(!FileCom->Obj.Inuse) // 文件未被引用
                {
                    struct Object *Parent;
                    if(OBJ_Del(&FileCom->Obj)) // 注销当前节点
                    {
                        Parent = FileCom->Obj.Parent;
                        free(FileCom); // 删除文件的节点缓存
                        if(Parent)
                        {
                            FileCom = Container(Parent, struct VCommon, Obj);
                            if(NULL == FileCom->Context)
                                continue; // 父文件不存在上下，则继续，尝试删除
                        }
                    }

                    break; // 存在子节点，退出
                }
            }
        }
    }

    Lock_MutexPost(g_ptVFMutex);// 出互斥
    return (Ret);
}
//-----------------------------------------------------------------------------
//功能: 写文件
//参数: Buf -- 数据缓冲;
//      size -- 需要写的单位数;
//      nmemb -- 单位数;
//      Private -- 文件上下文;
//返回: 0 -- 失败; 非0 -- 成功;
//备注:
//-----------------------------------------------------------------------------
s32 FWrite(const void *Buf, u32 Size, u32 Nmemb, void *Private)
{
    struct FileContext *FileCt;
    struct VCommon *FileCom;
    struct FileOperations *FileOps;
    u32 Length, LengthLef, BufLeft, BufUsed, BufSize;
    u8 *DataPtr = (u8*)Buf;

    if(NULL == Private)
        return (0);// 参数错误

    FileCt = (struct FileContext*)Private;
    FileCom = (struct VCommon*)(FileCt->CHost);

    if((FileCom->Obj.Type != OB_REG) || !(FileCt->Property & P_WRITE))
        return (0);// 文件类型和属性错误

    FileOps = (struct FileOperations *)(FileCt->FileOps);
    if((NULL == FileOps) || (NULL == FileOps->FileWrite))
        return (0);// 函数未注册

    LengthLef = Length = Size *Nmemb;
    while(FileCt->Buf) // 带缓冲逻辑
    {
        BufLeft = FileCt->Buf->End - FileCt->Buf->Current;
        BufUsed = FileCt->Buf->Current - FileCt->Buf->Start;
        if(BufLeft >= LengthLef) // 缓存空间足够
        {
            memcpy(FileCt->Buf->Current, DataPtr, LengthLef);
            FileCt->Buf->Current += LengthLef;
            FileCt->Buf->Status = BUF_DIRTY;
            return (Length);
        }
        else if(BUF_DIRTY == FileCt->Buf->Status)// 缓存空间不足,先将缓冲中数据刷入
        {
            if(BufLeft) // 考虑写入数据对齐逻辑
            {
            	memcpy(FileCt->Buf->Current, DataPtr, BufLeft);
            	LengthLef -= BufLeft;
            	DataPtr += BufLeft;
            }

            BufSize = FileCt->Buf->End - FileCt->Buf->Start;
        	if((BufSize != FileOps->FileWrite(FileCt->Buf->Start, 1, BufSize, Private)))
                return (0);// 写入错误

            FileCt->Buf->Current = FileCt->Buf->Start;
            FileCt->Buf->Status = BUF_CLEAN;
            continue;// 已有数据刷入后,再次判断
        }

        break;
    }

    // 无缓冲逻辑
    return (FileOps->FileWrite(DataPtr, 1, Length, Private));

}
//-----------------------------------------------------------------------------
//功能: 读文件
//参数: Buf -- 数据缓冲;
//      size -- 需要读的单位数;
//      nmemb -- 单位数;
//      Private -- 文件上下文;
//返回: 0 -- 失败; 非0 -- 成功;
//备注: 因为读写操作都是递增方向的，当前缓冲逻辑只支持写，对读起不到提升效果。
//-----------------------------------------------------------------------------
s32 FRead(void *Buf, u32 Size, u32 Nmemb, void *Private)
{
    struct FileContext *FileCt;
    struct VCommon *FileCom;
    struct FileOperations *FileOps;
    u32 Length;

    if(NULL == Private)
        return (0);// 参数错误

    FileCt = (struct FileContext*)Private;
    FileCom = (struct VCommon*)(FileCt->CHost);

    if((FileCom->Obj.Type != OB_REG) || !(FileCt->Property & P_READ))
        return (0);// 文件类型和属性错误

    FileOps = (struct FileOperations *)(FileCt->FileOps);
    if((NULL == FileOps) || (NULL == FileOps->FileRead))
        return (0);// 函数未注册

    if(FileCt->Buf)
    {
        if(BUF_DIRTY == FileCt->Buf->Status)
        {
            // 不对FileWrite是否注册做检查, 因为只有写才会造成BUF_DIRTY, 则在写时做了检查
            Length = FileCt->Buf->Current - FileCt->Buf->Start;
            if(Length != FileOps->FileWrite(FileCt->Buf->Start, 1, Length, FileCt))
                return (0);// 写入错误
            FileCt->Buf->Current = FileCt->Buf->Start;
            FileCt->Buf->Status = BUF_CLEAN;
        }

    }

    Length = Size * Nmemb;
    return (FileOps->FileRead(Buf, 1, Length, Private));
}
//-----------------------------------------------------------------------------
//功能: 缓冲目录内容(目录下包含的文件等)到文件系统
//参数:
//返回: 0 -- 成功; -1 -- 失败
//备注: 使用是需要上锁
//-----------------------------------------------------------------------------
s32 __CacheDir(struct VFile *Fp)
{
    struct FileContext *FileCt;
    struct VCommon *FileCom;
    struct Object *Child, *DirObj;
    struct FileOperations *FileOps;
    s32 Ret;
    struct Dirent Content;

    FileCt = (struct FileContext *)Fp->Private;
    FileCom = FileCt->CHost;
    DirObj = &(FileCom->Obj);

    Child = OBJ_Child(DirObj);
    if((NULL != Child) && (0 == (Child->Type & OB_FILE)))
        return (0); // 不存在属于文件系统类型的子项

    FileOps = (struct FileOperations *)FileCt->FileOps;

    if(NULL == FileOps->DirRead)
        return (-1); // 不支持该功能

    do
    {
        Ret = FileOps->DirRead(FileCt, &Content);
        if(Ret)
            break; // 出错
        FileCom = __CoverPath(Content.Name, DirObj);
        if(FileCom)
        {
            switch(Content.Property & P_TYPE)
            {
                case P_REG: FileCom->Obj.Type |= OB_REG; break;
                case P_DIR: FileCom->Obj.Type |= OB_DIR; break;
                case P_LINK: FileCom->Obj.Type |= OB_LINK; break;
            }
        }
    }while(1);

    return (0);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
struct Object *__FileObj(struct VFile *Fp)
{
    struct FileContext *FileCt;
    struct VCommon *FileCom;

    FileCt = (struct FileContext *)Fp->Private;
    FileCom = FileCt->CHost;
    return (&(FileCom->Obj));
}
//-----------------------------------------------------------------------------
//功能: 刷新目录内容(目录下包含的文件等)到文件系统
//参数:
//返回:
//备注: 使用是需要上锁,对应__CacheDir,免得由于__CacheDir产生的无法回收的节点
//-----------------------------------------------------------------------------
void __FlushDir(struct VFile *Fp)
{
    struct VCommon *FileCom;
    struct Object *Obj, *ObjNext, *Tail;
    u8 AllChecked = 0;

    Obj = __FileObj(Fp);

    Obj = OBJ_Child(Obj); // 取第一个子项
    if((Obj && (0 == (Obj->Type & OB_FILE))) || (NULL == Obj))
        return ; // 不存在属于文件系统类型的子项 或 没有子项

    // 遍历所有子项,删除临时缓存的子项
    Tail = OBJ_Previous(Obj); // 获取子项尾节点

    while(1)
    {
        ObjNext = OBJ_Next(Obj);

        FileCom = Container(Obj, struct VCommon, Obj);
        if((NULL == FileCom->Context) && (0 == Obj->Inuse))
        {
            if(OBJ_Del(Obj))
                free(FileCom);
        }

        if(AllChecked)
            break;

        Obj = ObjNext;

        if(Tail == Obj)
            AllChecked = 1; // 到达尾部，所有子项都获取了
    }
}
//-----------------------------------------------------------------------------
//功能: 从当前文件节点的父继承操作方法
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
void *__InheritFileOps(struct Object *FileObj)
{
    struct Object *Parent;
    struct VMount *Mount;
    struct VCommon *FileCom;

    Parent = FileObj;
    while(Parent)
    {
        if(Parent->Type & OB_MOUNT)
        {
             FileCom = Container(Parent, struct VCommon, Obj);
             Mount = (struct VMount*)(FileCom->Context);
             return (Mount->Private);
        }
        Parent = OBJ_Parent(Parent);
    }

    return (NULL);
}
//-----------------------------------------------------------------------------
//功能: 获取文件的操作方法
//参数: PathCt -- 文件路径遍历时的上下文
//返回:
//备注:
//----------------------------------------------------------------------------
void *__FileOps(struct PathWalk *PathCt)
{
    if(NULL != PathCt->FileOps)
        return (PathCt->FileOps);
    else
        return (__InheritFileOps(PathCt->End)); // 情景：当前工作路径是在某个文件系统内部
}
//-----------------------------------------------------------------------------
//功能: 读目录项
//参数: Dir -- 目录描述符;
//
//返回: 1 -- 结束,全部读完;
//      -1 -- 参数错误; -2 -- 函数不支持; -3 -- 其他错误;
//      0 -- 成功读取返回;
//备注:
//-----------------------------------------------------------------------------
 s32 ReadDir(struct VFile *Dir, struct Dirent *DirentInfo)
{
    struct FileContext *FileCt;
    struct VCommon *FileCom;
    struct Object *Child, *DirObj;
    struct FileOperations *FileOps;

    if((NULL == DirentInfo) || (NULL == Dir))
        return (-1);

    FileCt = (struct FileContext *)Dir->Private;
    FileCom = FileCt->CHost;
    DirObj = &(FileCom->Obj);

    Child = OBJ_Child(DirObj);
    if((NULL != Child) && (0 == (Child->Type & OB_FILE)))
        return (-3); // 不存在属于文件系统类型的子项

    FileOps = (struct FileOperations *)FileCt->FileOps;

    if((NULL == FileOps) || (NULL == FileOps->DirRead))
        return (-2); // 不支持该功能

    return (FileOps->DirRead(FileCt, DirentInfo));
}
 //-----------------------------------------------------------------------------
 //功能: 判断是否普通文件(目录、文件等)
 //参数:
 //返回:
 //备注:
 //-----------------------------------------------------------------------------
 bool_t IS_FILE(struct VFile *File)
 {
     struct FileContext *FileCt;
     struct VCommon *FileCom;
     if(NULL == File)
         return (FALSE);

     FileCt = (struct FileContext*)File->Private;
     FileCom = FileCt->CHost;
     if(FileCom && (FileCom->Obj.Type & OB_FILE))
         return (TRUE);

     return (FALSE);

 }
 //-----------------------------------------------------------------------------
 //功能: 判断是否是设备文件
 //参数:
 //返回:
 //备注:
 //-----------------------------------------------------------------------------
 bool_t IS_DEV(struct VFile *File)
  {
     if(IS_FILE(File))
         return (FALSE);
     return (TRUE);

  }
