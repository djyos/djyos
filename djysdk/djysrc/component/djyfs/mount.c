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

#include <string.h>
#include <stdlib.h>
#include <int_hard.h>
#include <int.h>
#include <systime.h>
#include <driver.h>
#include <djyos.h>
#include "mount.h"
#include "iofile.h"
#include "file.h"

//
//
//
extern char *g_pWokingPath; // shell使用
//
// 本地变量
//
static struct FileSysTypeInfo *s_ptFSTypeList;// 文件系统类型链表
static char s_cPWDRoot[257];
//
// 函数声明
//
s32 Get(struct Object *Obj);
void Put(struct Object *Obj);
static struct FileSysType *FindFSType(const char *Type);
static char *__PWDFixRootPath(struct Object *Obj);
static s32 __PathLen(struct Object *Obj, u8 Opts);
s32 __WorkingPath(char *Path);

//-----------------------------------------------------------------------------
//功能: 安装一个文件系统.
//参数: MountPath -- 挂载点路径;
//      DevPath -- 文件系统所在设备路径;
//      Type -- 准备挂载的文件系统类型;
//      Private -- 传递给安装函数;
//返回:  0 -- 成功.
//      -1 -- 输入参数错误; -2 -- 文件系统类型不存在;
//      -3 -- 设备无法打开; -4 -- 挂载目录不存在;
//      -5 -- 文件系统无法安装进设备;
//      -6 -- 内存不足;
//备注:
//-----------------------------------------------------------------------------
s32 Mount(const char *MountPath, const char *DevPath, const char *Type, void *Private)
{
    struct FileSysType *FSType;
    struct MountInfo *NewMountInfo;
    struct VCommon *FileCom;
    struct FileContext *FileCt;
    struct DjyDevice *Dev;
    struct VFile *DevFp = NULL, *DirFp = NULL;
    s32 Ret = 0;


    if(!MountPath || !DevPath || !Type)
        return (-1);

    FSType = FindFSType(Type);
    if(!FSType)
        return (-2);// 目标文件系统类型尚未注册

    DevFp = __Open(DevPath, M_READ | M_WRITE | M_OPEN);// 打开设备
    if (!DevFp)
        return (-3);

    FileCt = (struct FileContext *)(DevFp->Private);
    Dev = (struct DjyDevice *)(FileCt->Private);

    if(Get(&Dev->Node))// 设备被用于文件系统了，从此不能该设备删除，除非文件系统卸载
    {
        // 这个错误正常情况是不会出现的
        Ret = -3;
        goto FAIL;
    }

    DirFp = __Open(MountPath, M_READ | M_DIR | M_OPEN);// 打开挂载点目录
    if(!DirFp)
    {
        Ret = -4;
        goto FAIL;
    }

    // 挂载点类型校验
    FileCt = (struct FileContext*)(DirFp->Private);
    FileCom = FileCt->CHost;
    if(!(FileCom->Obj.Type & OB_DIR ))// DIR已经是O_MOUNT也可以了
    {
        Ret = -4;
        goto FAIL;
    }

    NewMountInfo = malloc(sizeof(struct MountInfo));
    if(!NewMountInfo)
    {
        Ret = -6;
        goto FAIL;
    }

    NewMountInfo->Type = FSType;
    NewMountInfo->Dev = &(Dev->Node);
    Ret = FSType->Install(NewMountInfo, Private);// 挂载文件系统
    if(Ret)
    {
        Ret = -5;
        goto FAIL;
    }

    // 初始化安装点
    NewMountInfo->Mount.OldObj = FileCom->Obj.Child;
    NewMountInfo->Mount.Context = FileCom->Context;
    if(NULL == NewMountInfo->Mount.Name)// 用户可以在自己的install函数中指定，例如FAT
        NewMountInfo->Mount.Name = Dev->Node.Name;// 安装点名(目前研究的几个文件系统，都是已设备名作为文件系统根的区分)

    if(FSType->Property == MOUNT_FILE)// 当前只支持文件类型的安装
    {
        NewMountInfo->Mount.Private = (void *)FSType->FileOps;// 文件系统的具体操作方式
        NewMountInfo->Mount.Property = MOUNT_FILE;
        NewMountInfo->Mount.VOps = &g_tFOps;

        if(FileCom->Obj.Type & OB_MOUNT)
            NewMountInfo->Mount.Property |= MOUNT_ON_MOUNT;
        else
            NewMountInfo->Mount.Property |= MOUNT_ON_DIR;

        Lock_MutexPend(g_ptVFMutex, CN_TIMEOUT_FOREVER);
        FileCom->Context = &(NewMountInfo->Mount);// File的上下文重新设置为新的Mount
        FileCom->Obj.Type |= OB_MOUNT;
        FileCom->Obj.Child = NULL;
        Lock_MutexPost(g_ptVFMutex);

        Lock_MutexPend(g_ptFSTypeMutex, CN_TIMEOUT_FOREVER);
        if(FSType->MountList)// 新的挂载信息链入
        {
            NewMountInfo->Next = FSType->MountList;
            FSType->MountList = NewMountInfo;
        }
        else
        {
            FSType->MountList = NewMountInfo;// 链表的第一个
            NewMountInfo->Next = NULL;
        }
        Lock_MutexPost(g_ptFSTypeMutex);
    }

FAIL:
    if(DirFp)
        fclose(DirFp);
    if(DevFp)
        fclose(DevFp);


    return (Ret);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: -1 -- 安装点不存在; -2 -- 文件系统尚有文件被使用;
//      -3 -- 严重错误,不在安装信息不在链表;
//      -4 -- 设备信息错误。
//备注: 当前只支持安装点路径卸载
//-----------------------------------------------------------------------------
s32 Unmount(const char *Path)
{
    struct VFile *Fp;
    struct VCommon *FileCom;
    struct FileContext *FileCt;
    struct VMount *Mount;
    struct FileSysType *FSType;
    struct MountInfo *Temp;
    struct MountInfo *MountInfo;

    s32 Ret = 0;

    Fp = __Open(Path, M_READ | M_DIR | M_OPEN);
    if(!Fp)
        return -1;

    FileCt = (struct FileContext*)Fp->Private;
    FileCom = FileCt->CHost;

    if(!(FileCom->Obj.Type & OB_MOUNT))// 类型校验
    {
        Ret = -1;
        goto FAIL;
    }


    if(FileCom->Obj.Child == NULL)
    {
        Mount = (struct VMount*)FileCom->Context;

        Lock_MutexPend(g_ptVFMutex, CN_TIMEOUT_FOREVER);
        FileCom->Obj.Child = Mount->OldObj;// 恢复原文件系统下的内容
        FileCom->Context = Mount->Context;
        if(Mount->Property == MOUNT_ON_DIR)
            FileCom->Obj.Type &= (~OB_MOUNT);// 没有后续MOUNT
        Lock_MutexPost(g_ptVFMutex);
    }
    else
    {
        Ret = -2;
        goto FAIL;// 文件系统下有文件仍在使用中
    }

    MountInfo = Container(Mount, struct MountInfo, Mount);

    FSType = MountInfo->Type;

    // 将当前安装点从它所属的文件系统类型的链表上删除
    Lock_MutexPend(g_ptFSTypeMutex, CN_TIMEOUT_FOREVER);

    Temp = FSType->MountList;

    if(MountInfo == Temp)
        FSType->MountList = MountInfo->Next;// 第一个
    else
    {
        while((Temp->Next) && (Temp->Next != MountInfo))
            Temp = Temp->Next;
        if(Temp->Next)
            Temp->Next = MountInfo->Next;
        else// 遍历到最后一个都为发现本安装点信息，肯定是系统有问题了,但是先不释放本结构
        {
            Ret = -3;
            Lock_MutexPost(g_ptFSTypeMutex);
            goto FAIL;
        }
    }

    Lock_MutexPost(g_ptFSTypeMutex);

    if(FSType->Uninstall(MountInfo->Private))
        return (-4);

    Put(MountInfo->Dev);

    free(MountInfo);

FAIL:
    if(Fp)
        fclose(Fp);

    return (Ret);
}

//-----------------------------------------------------------------------------
//功能: 注册文件系统类型
//参数:
//返回: 0  -- 成功;
//      -1 -- 输入参数错误; -2 -- 目标文件系统类型已存在;-3 -- 内存不足;
//备注:
//-----------------------------------------------------------------------------
s32 RegisterFSType(struct FileSysType *NewType)
{
    struct FileSysTypeInfo *New;

    if(NULL == NewType)
        return (-1);

    if(FindFSType(NewType->Name))
        return (-2);

    New = malloc(sizeof(struct FileSysTypeInfo));
    if(NULL == New)
        return (-3);
    New->Type = NewType;

    Lock_MutexPend(g_ptFSTypeMutex, CN_TIMEOUT_FOREVER);
    if(s_ptFSTypeList)
    {
        New->Next = s_ptFSTypeList->Next;
        s_ptFSTypeList = New;
    }
    else
    {
        New->Next = NULL;
        s_ptFSTypeList = New;
    }
    Lock_MutexPost(g_ptFSTypeMutex);

    return (0);
}
//-----------------------------------------------------------------------------
//功能: 注销文件系统类型
//参数:
//返回: 0  -- 成功;
//      -1 -- 输入参数错误; -2 -- 文件系统类型已存在;-3 -- 目标文件系统不存在;
//备注:
//-----------------------------------------------------------------------------
s32 UnRegisterFSType(const char *Type)
{
    struct FileSysTypeInfo *Cur, *Pre;
    char *Name;

    if(NULL == Type)
        return (-1);

    Lock_MutexPend(g_ptFSTypeMutex, CN_TIMEOUT_FOREVER);
    if(!s_ptFSTypeList)
    {
        Lock_MutexPost(g_ptFSTypeMutex);
        return (-2);
    }

    Cur = Pre = s_ptFSTypeList;
    while(1)
    {
        Name = Cur->Type->Name;
        if(!strcmp(Name, Type))
        {
            if(Cur == s_ptFSTypeList)
                s_ptFSTypeList = NULL;
            else
                Pre->Next = Cur->Next;
        }
        else
        {
            if(!Cur->Next)
                return (-3);
            Pre = Cur;
            Cur = Cur->Next;
        }
    }
    Lock_MutexPost(g_ptFSTypeMutex);

    free(Cur);

    return (0);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: 非NULL -- 成功;
//      NULL -- 输入参数错误 或 文件系统类型不存在;
//备注:
//-----------------------------------------------------------------------------
static struct FileSysType *FindFSType(const char *Type)
{
    struct FileSysTypeInfo *Cur;
    struct FileSysType *Ret = NULL;
    char *Name;

    if(NULL == Type)
        return (NULL);

    Lock_MutexPend(g_ptFSTypeMutex, CN_TIMEOUT_FOREVER);
    if(s_ptFSTypeList)
    {
        Cur = s_ptFSTypeList;
        while(1)
        {
            Name = Cur->Type->Name;
            if(0 == strcmp(Name, Type))
            {
                Ret = Cur->Type;
                break;// 找到
            }
            else if(NULL == Cur->Next)
            {
                break;// 最后一个节点了
            }
            Cur = Cur->Next;
        }
    }
    Lock_MutexPost(g_ptFSTypeMutex);

    return (Ret);

}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
s32 Get(struct Object *Obj)
{
    s32 Ret = 0;
    atom_low_t   ALock;

    ALock = Int_LowAtomStart();
    if(NULL == Obj)
        Ret = -1;
    else if(Obj->Inuse == -1)
        Ret = -2;
    else
        Obj->Inuse++;
    Int_LowAtomEnd(ALock);
    return (Ret);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
void Put(struct Object *Obj)
{
    if(Obj)
        Obj->Inuse--;
}
//-----------------------------------------------------------------------------
//功能: 设置环境变量,系统根
//参数:
//返回: 0 -- 成功; -1 -- 目录无法打开; -2 -- 目录无法使用;
//备注:
//-----------------------------------------------------------------------------
s32 SetRoot(const char *Path)
{
    struct FileContext *FileCt;
    struct VCommon *FileCom;
    s32 Ret;
    struct VFile *Dir = __Open(Path, M_READ | M_DIR | M_OPEN);// 打开目录，可以起到防止文件被意外删除的效果

    if(NULL == Dir)
        return -1;

    FileCt = (struct FileContext *)(Dir->Private);
    FileCom = FileCt->CHost;
    Ret = Get(&(FileCom->Obj));
    if(!Ret)
    {
        Lock_MutexPend(g_ptFSTypeMutex, CN_TIMEOUT_FOREVER); // 进互斥
        Put(g_tSysRoot.Obj);
        g_tSysRoot.Obj = &(FileCom->Obj);
        g_tSysRoot.VOps = Dir->VOps;
        Lock_MutexPost(g_ptFSTypeMutex); // 出互斥
    }

    fclose(Dir);// 关闭文件
    return (Ret);
}
//-----------------------------------------------------------------------------
//功能: 设置环境变量,系统当前工作路径
//参数:
//返回: 0 -- 成功; -1 -- 目录无法打开; -2 -- 目录无法使用;
//备注:
//-----------------------------------------------------------------------------
s32 SetPWD(const char *Path)
{
    struct FileContext *FileCt;
    struct VCommon *FileCom;
    s32 Ret;
    struct VFile *Dir = __Open(Path, M_READ | M_DIR | M_OPEN);// 打开目录，可以起到防止文件被意外删除的效果

    if(NULL == Dir)
        return (-1);

    FileCt = Dir->Private;
    FileCom = FileCt->CHost;
    Ret = Get(&(FileCom->Obj));
    if(!Ret)
    {
        Lock_MutexPend(g_ptFSTypeMutex, CN_TIMEOUT_FOREVER); // 进互斥
        Put(g_tSysPWD.Obj);
        g_tSysPWD.Obj = &(FileCom->Obj);
        g_tSysPWD.VOps = Dir->VOps;
        g_tSysPWD.Path = __PWDFixRootPath(g_tSysPWD.Obj);
        g_tSysPWD.FileOps = FileCt->FileOps;
        Lock_MutexPost(g_ptFSTypeMutex); // 出互斥
    }

    fclose(Dir);

    __WorkingPath(g_pWokingPath); // 用于shell

    return (Ret);
}

//-----------------------------------------------------------------------------
//功能: 获取当前工作路径字符串长度(含结束符)
//参数:
//返回: 非零-- 成功; 零 -- 失败，未设置当前工作路径;
//备注:
//-----------------------------------------------------------------------------
s32 CurWorkPathLen(void)
{
#if 0
    struct Object *Obj;
    u32 PathLen = 0;
    u32 PathSegment = 0;

    Obj = g_tSysPWD.Obj;
    if(NULL == Obj)
        return (0); // 当前工作路径未设置

    for(;;)
    {
        PathLen += strlen(OBJ_Name(Obj));
        Obj = OBJ_Parent(Obj);
        if(NULL == Obj)
            break;
        PathSegment++;
    }

    if(0 == PathSegment)
        PathSegment = 1; // 此处的情景是"/"为当前路径,置1表示字符串结束符

    return (PathLen + PathSegment);
#else
    return (__PathLen(g_tSysPWD.Obj, 0));
#endif
}


//-----------------------------------------------------------------------------
//功能: 工作路径字符串长度(含结束符)
//参数: Obj -- 追溯的起始节点
//      Opts -- 0 -- 回溯到根节点结束
//              1 -- 回溯到第一个挂载点结束
//返回:
//备注:
//-----------------------------------------------------------------------------
static s32 __PathLen(struct Object *Obj, u8 Opts)
{
    struct VCommon *FileCom;
    struct VMount *Mount;
    u32 PathLen = 1;
    u32 PathSegment = 0;

    if(NULL == Obj)
        return (0); // 当前工作路径未设置

    for(;;)
    {
        PathLen += strlen(OBJ_Name(Obj));
        Obj = OBJ_Parent(Obj);
        switch(Opts)
        {
            case 0: if(NULL == Obj)
                    {
                        if(PathSegment)
                            PathSegment--; // 去除根目录重复统计
                        goto DONE;
                    }
                    else
                        break;
            case 1: if(OB_MOUNT & Obj->Type)
                    {
                        FileCom = Container(Obj, struct VCommon, Obj);
                        Mount = (struct VMount*)(FileCom->Context);
                        PathLen += strlen(Mount->Name);
                        if((0 == PathSegment) &&
                           (('/' != Mount->Name[0]) && ('\0' != Mount->Name[1]))) // 排除根目录逻辑
                            PathSegment++; // 情景,挂载点第一级目录项
                        goto DONE;
                    }
                    else
                        break;
            default: goto DONE;
        }
        PathSegment++;
    }

DONE:
    return (PathLen + PathSegment);
}

//-----------------------------------------------------------------------------
//功能: 获取当前工作路径
//参数: Buf -- 当前工作路径
//      BufSize -- 路径长度，包括字符串结束符
//返回: 0 -- 成功; -1 -- 未设置当前工作路径; -2 -- 参数错误; -3 -- 内存不足;
//      -4 -- 其他;
//备注:
//-----------------------------------------------------------------------------
s32 CurWorkPath(char *Buf, u32 BufSize)
{
    struct Object *Obj;
    u32 ObjNameLen, Offset;
    char *PathTemp, *ObjName;
    s32 Ret = 0;

    Lock_MutexPend(g_ptFSTypeMutex, CN_TIMEOUT_FOREVER); // 进互斥(防止操作过程当前工作路径被更改)
    Offset = CurWorkPathLen();
    if(0 == Offset)
    {
        Ret = -1; // 当前路径未设置
        goto FAIL;
    }

    if((NULL == Buf) || (BufSize < Offset))
    {
        Ret = -2; // 参数错误
        goto FAIL;
    }

    PathTemp = (char*)malloc(Offset);
    if(NULL == PathTemp)
    {
        Ret = -3; // 内存不足
        goto FAIL;
    }

    Offset = Offset - 1;
    PathTemp[Offset] = '\0'; // 路径的结束符
    Obj = g_tSysPWD.Obj; // 路径最后一个节点

    for(;;)
    {
        ObjName = OBJ_Name(Obj);
        ObjNameLen = strlen(ObjName);
        Offset = Offset - ObjNameLen;
        memcpy((PathTemp + Offset), ObjName, ObjNameLen);
        if(Offset) // 去除根的情况
        {
            Offset--;
            PathTemp[Offset] = '/'; // 路径之间的分隔或者是根
        }

        if(0 == Offset)
        {
            strcpy(Buf, PathTemp);
            break; // 结束
        }
        Obj = OBJ_Parent(Obj);
        if(NULL == Obj)
        {
            Ret = -4;
            break;
        }
    }

FAIL:
    if(PathTemp)
        free(PathTemp);
    Lock_MutexPost(g_ptFSTypeMutex); // 出互斥
    return (Ret);
}
//-----------------------------------------------------------------------------
//功能: 用于shell打印当前工作目录
//参数:
//返回: 0 -- 成功; -1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
s32 __WorkingPath(char *Path)
{
    u16 Len;

    Len = CurWorkPathLen();
    if(257 < Len)
        return (-1); // 路径过长

    if(CurWorkPath(Path, Len))
        return (-1);

    return (0);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
#if 0
struct Object *GetMountByName(struct FileSysType *FSType, const char *MountName)
{
    struct MountInfo *MountInfo = FSType->MountList;

    while(MountInfo)
    {
        if(strcmp(MountInfo->Mount.Name, MountName))
            MountInfo = MountInfo->Next;
        else
            return (MountInfo->Dir);
    }
    return (NULL);
}
#endif
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static char *__PWDFixRootPath(struct Object *Obj)
{
    struct VCommon *FileCom;
    struct VMount* Mount;
    char *ObjName;
    u16 ObjNameLen;
    u32 Len;
    char *PathTemp;

    if(OB_MOUNT & Obj->Type)
        return (NULL); // todo:当前节点是挂载点时,不执行,系统中其他逻辑可覆盖

    Len = __PathLen(Obj, 1);
    PathTemp = s_cPWDRoot;
    Len = Len - 1;
    PathTemp[Len] = '\0'; // 路径的结束符

    for(;;)
    {
        if(OB_MOUNT & OBJ_Type(Obj))
        {
            FileCom = Container(Obj, struct VCommon, Obj);
            Mount = (struct VMount*)(FileCom->Context);
            ObjName = Mount->Name;
        }
        else
        {
            ObjName = OBJ_Name(Obj);
        }
        ObjNameLen = strlen(ObjName);
        Len = Len - ObjNameLen;
        memcpy((PathTemp + Len), ObjName, ObjNameLen);
        if(Len)
        {
            Len--;
            PathTemp[Len] = '/'; // 路径之间的分隔或者是根
        }

        if(0 == Len)
            break; // 结束

        Obj = OBJ_Parent(Obj);
        if(NULL == Obj)
        {
            return (NULL); // 不应该出现这种情况
        }
    }

    return(PathTemp);
}
//-----------------------------------------------------------------------------
//功能: 格式化文件系统
//参数:
//返回: -1 -- 参数错误; -2 -- 文件系统内有文件正在被使用; -3 -- 格式化失败;
//      0 -- 成功;
//备注:
//-----------------------------------------------------------------------------
s32 Format(const char *MountPath)
{

    struct VFile *Fp;
    struct FileContext *FileCt;
    struct VCommon *FileCom;
    struct VMount *Mount;
    struct MountInfo *MountInfos;
    struct FileSysType *Type;
    s32 Ret = -2;

    if(!MountPath)
        return (-1);

    Fp = __Open(MountPath, M_READ | M_DIR | M_OPEN);
    if(!Fp)
        return (-1);

    FileCt = (struct FileContext*)(Fp->Private);
    FileCom = FileCt->CHost;
    if(!(FileCom->Obj.Type & OB_MOUNT))
        return (-1);

    if(NULL == OBJ_Child(&FileCom->Obj))
    {

        Mount = (struct VMount*)FileCom->Context;
        MountInfos = Container(Mount, struct MountInfo, Mount);
        Type = MountInfos->Type;
        if(Type && Type->Format)
            Ret = Type->Format(MountInfos->Private);
        else
            Ret = -3; // 函数未注册;
    }

    fclose(Fp);
    return (Ret);
}
