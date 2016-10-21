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
#include <stdarg.h>
#include <systime.h>
#include <lock.h>
#include <stdlib.h>
#include <string.h>
#include "tricks.h"
#include "iofile.h"
#include "mount.h"
#include "file.h"


static void __Tostat(struct Stat *Src, struct stat *Dest);
//-----------------------------------------------------------------------------
//功能: 打开文件
//参数: pathname -- 文件路径;
//      flags -- 操作标志;
//      mode -- 操作模式;
//返回: -1 -- 失败; 其他 -- 文件句柄号;
//备注: 
//-----------------------------------------------------------------------------
int open(const char *pathname, int fmt, ...)
{
	va_list Params;
	u32 mode = 0; // 暂时不考虑
	struct VFile *Fp;
	u32 Mode = 0;
	s32 oflag = fmt;

	if((NULL == pathname)  || ('\0' == *pathname))
		return (-1);

	va_start(Params, fmt);
	mode = va_arg(Params, int);
	va_end(Params);

	switch (oflag & O_ACCMODE) // 打开方式
	{
		case O_RDONLY: Mode |= M_READ; break; // 缺省值
		case O_WRONLY: Mode |= M_WRITE; break;
		case O_RDWR: Mode |= M_WRITE | M_READ; break;
		default: return (-1); // 参数格式错误
	}
	oflag &= ~O_ACCMODE;
	
	if(oflag & O_TRUNC)// 截断
	{
		Mode |= M_TRUNCATE; 
		oflag &= ~O_TRUNC;
	}
	
	if(oflag & O_DIRECTORY)// 目录
	{
		Mode |= M_DIR;
		oflag &= ~O_DIRECTORY;
	}
	else
	{
		Mode |= M_REG; // 默认打开文件
	}
	
	if(oflag & O_APPEND)// 追加
	{
		Mode |= M_APPEND; 
		oflag &= ~O_APPEND;
	}

	if((oflag & O_CREAT) && (oflag & O_EXCL))
	{
		Mode |= M_CREATE; // 仅创建
		oflag &= ~O_CREAT;
		oflag &= ~O_EXCL;
	}
	else if(oflag & O_CREAT)
	{
		Mode |= (M_CREATE | M_OPEN); // 创建或打开
		oflag &= ~O_CREAT;
	}
	else
	{
		Mode |= M_OPEN; // 仅打开不创建
		if(oflag & O_EXCL)
			 oflag &= ~O_EXCL; // 不可知逻辑
	}
	
	if(oflag)
		return (-1);// 其他标志, 目前不支持
		
	Fp = __Open(pathname, Mode);// 打开文件
	if(!Fp)
		return (-1);// 文件打开失败
	
	return (VFileToHandle(Fp));
}
//-----------------------------------------------------------------------------
//功能: 关闭文件.
//参数: fd -- 文件句柄号;
//返回: 0 -- 成功;
//      -1 -- 参数错误; -2 -- 文件类型错误; 其他;  
//备注: 
//-----------------------------------------------------------------------------
int close(int fd)
{
	s32 Ret;
	struct VFile *Fp;
	
	Fp = HandleToVFile(fd);
	if(NULL == Fp)
		return (-1);

	Ret = Fp->VOps->VClose(Fp->Private);
	if(!Ret)
		FreeVFile(Fp);// 成功，释放句柄

	return (Ret);
}
//-----------------------------------------------------------------------------
//功能: 读文件.
//参数: fd -- 文件句柄号;
//      buf -- 读缓冲;
//      count -- 需要读的数据大小（Byte计）;
//返回: >= 0 -- 成功; -1 -- 失败;    
//备注: 
//-----------------------------------------------------------------------------
int read(int fd, void *buf, size_t count)
{
	struct VFile *Fp;
	s32 Ret;
	
	if(NULL == buf)
		return (-1);// 参数错误
		
	if(0 == count)
		return (0);
		
	Fp = HandleToVFile(fd);
	if(NULL == Fp)
		return (-1);

	Ret = Fp->VOps->VRead(buf, count, 1, Fp->Private);
	if(0 == Ret)
		Ret = -1;
	
	return (Ret);		
}
//-----------------------------------------------------------------------------
//功能: 写文件.
//参数: fd -- 文件句柄号;
//      buf -- 写缓冲;
//      count -- 需要写的数据大小（Byte计）;
//返回:  >= 0 -- 成功; -1 -- 失败; 
//备注: 
//-----------------------------------------------------------------------------
int write(int fd, const void *buf, size_t count)
{
	struct VFile *Fp;
	s32 Ret;
	
	if(NULL == buf)
		return (-1);// 参数错误
		
	if(0 == count)
		return (0);
		
	Fp = HandleToVFile(fd);
	if(NULL == Fp)
		return (-1);
	
	Ret = Fp->VOps->VWrite(buf, count, 1, Fp->Private);
	if(0 == Ret)
		Ret = -1;
	
	return (Ret);
}
//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回:  
//备注: todo: cntl()
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//功能: 查询文件状态
//参数: file_name -- 文件路径；
//      buf -- 文件状态;
//返回: 0 -- 成功; -1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
int fstat(int fd, struct stat *buf)
{
	s32 Ret;
	struct FileContext *FileCt;
	struct FileOperations *FileOps;
	struct Stat FileStat;
	struct VFile *Fp = NULL;

	if(NULL == buf)
		return(-1);

	Lock_MutexPend(g_ptVFMutex, CN_TIMEOUT_FOREVER);// 进互斥,因为存在一定的检索动作
	Fp = HandleToVFile(fd);
	if(NULL == Fp)
	{
		Ret = -1; // 未找到句柄号
		goto FAIL;
	}

	FileCt = (struct FileContext*)Fp->Private;
	if(FileCt->Property & P_DEV)
	{
		Ret = -1; // todo: 设备文件暂不支持
		goto FAIL;
	}

	FileOps = (struct FileOperations*)(FileCt->FileOps);
	if((NULL == FileOps) || (NULL == FileOps->FileStat))
	{
		Ret = -1; // 文件系统不支持该函数
		goto FAIL;
	}

	Ret = FileOps->FileStat(FileCt, NULL, &FileStat, NULL);

FAIL:
	Lock_MutexPost(g_ptVFMutex);// 出互斥
	if(0 == Ret)
		__Tostat(&FileStat, buf);

	return (Ret);
}

//-----------------------------------------------------------------------------
//功能: 查询文件状态
//参数: file_name -- 文件路径；
//      buf -- 文件状态;
//返回: 0 -- 成功; -1 -- 失败;
//备注: todo: 是否需要互斥
//-----------------------------------------------------------------------------
int stat(const char *filename, struct stat *buf)
{
	struct PathWalk PathContext;
	u8 IsFound;
	struct FileOperations *FileOps;
	s32 Ret;
	struct Stat FileStat;

	if((NULL == buf) || (NULL == filename) || ('\0' == *filename))
		return(-1);

	Lock_MutexPend(g_ptVFMutex, CN_TIMEOUT_FOREVER);// 进互斥

	__PathInit(&PathContext, filename);

	IsFound = __VPathLookup(&PathContext, 0);

	if(!(PathContext.End->Type & OB_FILE))
	{
		Ret = -1; // 目前只支持文件
		goto FAIL;
	}

	FileOps = (struct FileOperations*)__FileOps(&PathContext);

	if((NULL == FileOps) || (NULL == FileOps->FileStat))
	{
		Ret = -1;// 该文件系统类型不支持
		goto FAIL;
	}

	Ret = FileOps->FileStat(NULL, PathContext.Path, &FileStat, PathContext.Root);

FAIL:
	Lock_MutexPost(g_ptVFMutex);// 出互斥

	if(0 == Ret)
		__Tostat(&FileStat, buf);

	return (Ret);
}
//-----------------------------------------------------------------------------
//功能:
//参数: file_name -- 文件路径；
//      buf -- 文件状态;
//返回: 0 -- 成功; -1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
int ftruncate(int fd, off_t length)
{
	struct VFile *Fp;
	struct FileContext *FileCt;
//	struct VCommon *FileCom;
	struct FileOperations *FileOps;

	Fp = HandleToVFile(fd);
	if(NULL == Fp)
		return (-1); // (EBADF)文件句柄无效或该文件已被关闭

	FileCt = (struct FileContext*)Fp->Private;
	if(0 == (FileCt->Property & P_REG) || (0 == (FileCt->Property & P_WRITE)))
		return (-1); // 只支持文件

	FileOps = (struct FileOperations*)(FileCt->FileOps);
	if((NULL == FileOps) || (NULL == FileOps->FileTruncate))
		return (-1); // 函数未注册

	return(FileOps->FileTruncate(FileCt, length));
}

//-----------------------------------------------------------------------------
//功能: 文件描述结构体转文件句柄号.
//参数: Fp -- 文件描述结构体;
//返回: 文件句柄号.
//备注: 
//-----------------------------------------------------------------------------
int VFileToHandle(struct VFile *Fp)
{
	int Fd;
	struct VFilePool *Pool;
	
	Pool = PoolOf(Fp);

	Fd = ((VFILE_NUM - 1) - (Fp - Pool->Space)) & 0xFFFF;// note: 这里是数组的减法，不是地址减法
	Fd |= ((Pool->ID << 16) & 0xFFFF0000);
	
	return (Fd);
}
//-----------------------------------------------------------------------------
//功能: 文件句柄号转文件描述结构体
//参数: fd -- 文件句柄号;
//返回: 文件描述结构体
//备注: 
//-----------------------------------------------------------------------------
struct VFile *HandleToVFile(int Fd)
{
	u16 ID, Offset;
	struct VFilePool *Pool;
	
	ID = (Fd >> 16) & 0xFFFF;// 所在的VFilePool的ID
	
	Offset = ((VFILE_NUM - 1) - (Fd & 0xFFFF));// 在Pool中的偏置
	
	Pool = g_ptVFilePool;
	
	while(Pool)// 查找VFile所在的pool
	{
		if(Pool->ID != ID)
			Pool = Pool->Next;
		else
			break;
	}
	
	if(Pool)
		return (Pool->Space + Offset);
	else
		return (NULL);
}
//-----------------------------------------------------------------------------
//功能: 删除文件或目录
//参数:
//返回: 0 -- 成功; -1 -- 失败;
//备注: todo
//-----------------------------------------------------------------------------
int unlink(const char *pathname)
{
    if(remove(pathname))
    	return (-1);
    return (0);
}
//-----------------------------------------------------------------------------
//功能: 打开目录
//参数:
//返回: 非NULL -- 成功; NULL -- 失败;
//备注: todo，临时逻辑
//-----------------------------------------------------------------------------
DIR *opendir(const char *name)
{
	DIR *DirD;
    s32 Handle;

    if((NULL == name)  || ('\0' == *name))
    	return (NULL);

    Handle = open(name, O_DIRECTORY | O_RDWR, 0);
    if(-1 == Handle)
    {
    	return (NULL);
    }

    DirD = malloc(sizeof(*DirD));
    if(NULL == DirD)
    {
    	close(Handle);
    }
    memset(DirD, 0, sizeof(*DirD));

    DirD->__fd = (void*)Handle;
    return (DirD);
}
//-----------------------------------------------------------------------------
//功能: 读目录项
//参数:
//返回:
//备注: todo,临时逻辑
//-----------------------------------------------------------------------------
struct dirent *readdir(DIR *dir)
{
	s32 Handle;
	struct VFile *Fp;
	struct Dirent DirentInfo;
	struct dirent *DirentRet;
	u8 i;

	if(NULL == dir)
		return (NULL);

	Handle = (s32)dir->__fd;

	Fp = HandleToVFile(Handle);
	if(NULL == Fp)
		return (NULL);

	if(ReadDir(Fp, &DirentInfo))
		return (NULL);

	DirentRet = (struct dirent *)dir->__data;
	if(NULL == DirentRet)
	{
		DirentRet = malloc(sizeof(*DirentRet));
		if(NULL == DirentRet)
			return (NULL);
		memset(DirentRet, 0, sizeof(*DirentRet));
		dir->__data = (void*)DirentRet;
	}

	for(i = 0; i < NAME_MAX; i++)
	{
		DirentRet->d_name[i] = DirentInfo.Name[i];
		if('\0' == DirentInfo.Name[i])
			break;
	}

	return (DirentRet);
}
//-----------------------------------------------------------------------------
//功能: 创建目录
//参数:
//返回: 0 -- 成功; -1 -- 失败
//备注: todo,临时逻辑
//-----------------------------------------------------------------------------
int mkdir (const char *filename, mode_t mode)
{
    s32 Ret;

	Ret = open(filename, O_DIRECTORY | O_RDWR | O_CREAT, mode);
	if(-1 != Ret)
	{
		close(Ret);
		return (0);
	}

	return (-1);

}
//-----------------------------------------------------------------------------
//功能: 关闭目录
//参数:
//返回: 0 -- 成功; -1 -- 失败
//备注: todo,临时逻辑
//-----------------------------------------------------------------------------
int closedir(DIR *dir)
{
    s32 Ret;

	if(NULL == dir)
    	return (0);

    if(dir->__data)
    	free((void*)dir->__data);

    Ret = close((s32)dir->__fd);
    if(Ret < 0)
    	return (-1);

    free(dir);
    return (0);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static void __Tostat(struct Stat *Src, struct stat *Dest)
{
	memset(Dest, 0, sizeof(*Dest));

	Dest->st_size = (off_t)Src->Size;
	if(Src->Property & P_DIR)
		Dest->st_mode = S_IFDIR;
	else if(Src->Property & P_REG)
		Dest->st_mode = S_IFREG;
	else if(Src->Property & P_LINK)
		Dest->st_mode = S_IFLNK;
	// 其他参数暂时不支持
}
