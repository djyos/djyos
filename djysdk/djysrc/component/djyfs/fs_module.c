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
#include <lock.h>
#include "mount.h"
#include "vfile.h"


//
//
//
extern struct VCommon g_tRootCom; // rsc.c文件中定义
extern struct VCommon g_tSysCom; // rsc.c文件中定义
extern void __DevModule2FS(void *FileOps); // dev_module.c中定义
extern void FsShellInstall(void); // fs_shell.c中定义
//
// 系统环境变量：系统根、系统当前
//
struct SysEnv g_tSysRoot, g_tSysPWD;
//
// 文件系统的虚拟层接口
//
struct VOperations g_tFOps =
{
    .VOpen      = FOpen,
    .VClose     = FClose,
    .VWrite     = FWrite,
    .VRead      = FRead,
    .VCntrl     = NULL, // 对于真正文件，这个暂时不实现，因为有专门的fxxxx类函数
};
//
// RAM文件系统逻辑
//
static struct FileContext *RamOpen(const char *Path, u32 Mode, const char *Root);
static s32 RamClose(struct FileContext *FileCt);
static s32 RamStat(struct FileContext *FileCt, const char *Path, struct Stat *Buf, const char *Root);
static s32 RamDirRead(struct FileContext *FileCt, struct Dirent *Content);

static struct FileOperations s_tRamFileOps =
{
    .FileOpen = RamOpen,
    .FileClose = RamClose,
	.FileStat = RamStat,
	.DirRead = RamDirRead,
    // todo: 当前只实现目录的创建，其他不实现
};

//
// 虚拟文件系统的根文件系统挂载点
//
static struct VMount s_tRootMount =
{
	.VOps 		= &g_tFOps,
	.OldObj 	= NULL,
	.Property 	= MOUNT_ON_DIR | MOUNT_FILE,
	.Context 	= NULL,
	.Private 	= (void*)&s_tRamFileOps,
};
//
// 虚拟文件系统的"sys"文件系统挂载点
//
static struct VMount s_tSysMount =
{
    .VOps       = NULL,
    .OldObj     = NULL,
    .Property   = MOUNT_ON_DIR,
    .Context    = NULL,
    .Private    = NULL,
};

struct VFilePool *g_ptVFilePool;// todo:考虑到/dev独立的话，这个空间需再考虑存放位置
struct MutexLCB *g_ptVFMutex;// 虚拟文件系统互斥锁
struct MutexLCB *g_ptFSTypeMutex;// 文件系统类型管理互斥锁
struct MutexLCB *g_ptVFilePoolMutex;// 文件句柄互斥锁
char g_cPathSegment[257]; // 路径的某段，用于替代动态申请;
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: 0 -- 成功; -1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
s32  ModuleInstall_FileSystem(void)
{
    // 文件系统相关互斥锁
    g_ptVFMutex = Lock_MutexCreate("virtual file system");
    if(!g_ptVFMutex)
        goto FAIL;

    g_ptFSTypeMutex = Lock_MutexCreate("file system type");
    if(!g_ptFSTypeMutex)
        goto FAIL;

    g_ptVFilePoolMutex = Lock_MutexCreate("virtual file pool");
    if(!g_ptVFilePoolMutex)
        goto FAIL;

    g_ptVFilePool = __ExtendVFilePool();// 预分配一个VFile Pool,注意这个是在启动过程中
    if(NULL == g_ptVFilePool)
        goto FAIL;

    // 将系统根变为虚拟文件系统逻辑
    g_tRootCom.Obj.Type = OB_DIR | OB_MOUNT;
    g_tRootCom.Obj.Inuse = 2;// 因为下面会将本节点作为默认的根和当前目录
    g_tRootCom.Context = (void*)(&s_tRootMount);
    s_tRootMount.Name = g_tRootCom.Obj.Name;

    // "/sys"节点
    g_tSysCom.Obj.Type = OB_DIR | OB_MOUNT;
    g_tSysCom.Context = (void*)&s_tSysMount;
    s_tSysMount.Name = g_tSysCom.Obj.Name;

    // "/dev"节点
    __DevModule2FS((void*)(&s_tRamFileOps));

    // 设置系统根和当前目录
    g_tSysRoot.Obj = g_tSysPWD.Obj = &(g_tRootCom.Obj);
    g_tSysRoot.VOps = g_tSysPWD.VOps = &g_tFOps;

    // shell命令安装
    FsShellInstall();

    return (0);

FAIL:
    if(g_ptVFMutex)
        Lock_MutexDelete(g_ptVFMutex);
    if(g_ptFSTypeMutex)
        Lock_MutexDelete(g_ptFSTypeMutex);
    if(g_ptVFilePoolMutex)
        Lock_MutexDelete(g_ptVFilePoolMutex);
    return (-1);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: NULL -- 打开失败; 非NULL -- 打开成功。
//备注: 只支持目录
//-----------------------------------------------------------------------------
static struct FileContext *RamOpen(const char *Path, u32 Mode, const char *Root)
{
    struct FileContext *FileCt;
    char *PathNext;
    struct Object *Current, *Parent;
    char Temp;
    char *PathCur = PathCopy(Root, Path, NULL);

    if((Mode & M_APPEND) || (Mode & M_LINK) || (Mode & M_REG))
        return (NULL);// 当前不支持的模式

    if((Mode & M_CREATE) && (0 == (Mode & M_DIR)))// 目前只支持创建目录
    	return (NULL);
#if 0
    if(NULL == Root)
    	Current = g_tSysPWD.Obj;
    else
    	Current = g_tSysRoot.Obj;
#else
    Current = g_tSysRoot.Obj;
#endif

    // todo: 此处遍历逻辑待优化,因为重复了
    while(1)
	{
		while(*PathCur == '/')
			PathCur++;// 过滤多余的'/'

		if((*PathCur == '\0') || (PathCur == NULL))
			break;// 遍历路径结束或路径不存在

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

                Temp = *PathNext;
                *PathNext = '\0';
                Current = OBJ_SearchChild(Current, PathCur);
                *PathNext = Temp;
			}
			else
                Current = OBJ_SearchChild(Current, PathCur);
		}

		if(NULL == Current)
		{
			if(*PathNext != '\0')
				return (NULL); // 存在多个不存在节点
			break; // 遍历中断，路径上出现不存在(缓存)的节点
		}

		PathCur = PathNext;
	}

    if((NULL == Current) && (0 == (Mode & M_CREATE)))
    	return (NULL); // 不创建且节点不存在

    FileCt = AllocContext();
    if(!FileCt)
        return (NULL);

    FileCt->Property = P_DIR;
    FileCt->FileOps = (void*)&s_tRamFileOps;
    return (FileCt);
}

//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: 0 -- 成功;
//备注: 删除上下文件
//-----------------------------------------------------------------------------
static s32 RamClose(struct FileContext *FileCt)
{
	struct VCommon *FileObj = FileCt->CHost;

	if((FileObj) && (0 == FileObj->Obj.Inuse))
		FileObj->Obj.Inuse = 1; // 关闭内存中的文件或者目录，并不删除

	FreeContext(FileCt);
    return (0);
}

//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: 0 -- 成功; -1 -- 失败;
//备注: 当前内存文件系统只支持目录
//-----------------------------------------------------------------------------
static s32 RamStat(struct FileContext *FileCt, const char *Path, struct Stat *Buf, const char *Root)
{
//	if(!FileCt)
//		return (-1); // 容错逻辑
	memset(Buf, 0, sizeof(*Buf));
	Buf->Property = P_DIR;
	Buf->Size = 0;
	return (0);
}
//-----------------------------------------------------------------------------
//功能: 读目录项
//参数:
//返回: 1 -- 结束,全部读完;
//      -1 -- 参数错误; -2 -- 函数不支持; -3 -- 其他错误;
//      0 -- 成功读取返回;
//备注: 在函数内部不能发生文件节点删除
//-----------------------------------------------------------------------------
static s32 RamDirRead(struct FileContext *FileCt, struct Dirent *Content)
{
	u16 Type;
	struct Object *Child, *Current = (struct Object*)FileCt->Private;
	struct VCommon *FileCom = FileCt->CHost;

	Child = OBJ_Child(&(FileCom->Obj));
	if(NULL == Child)
		return (1);

	if(Current && (OBJ_Next(Current) == Child))
	{
		Put(Current);
		FileCt->Private = NULL;
		return (1);
	}

	if(Current)
	{
		Put(Current); //
		Current = FileCt->Private = (void*)OBJ_Next(Current);
	}
	else
	{
		Current = FileCt->Private = (void*)Child;
	}

	Get(Current); // 防止节点在使用过程中被删除
	strcpy(Content->Name, OBJ_Name(Current));
	Type = OBJ_Type(Current) & (~OB_FILE);
	if(Type & OB_REG)
		Content->Property = P_REG;
	if(Type & OB_DIR)
		Content->Property |= P_DIR;
	if(Type & OB_LINK)
		Content->Property |= P_LINK;

	return (0);
}

