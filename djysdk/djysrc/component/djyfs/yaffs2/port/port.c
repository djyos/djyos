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
#include <stddef.h>
#include <djyfs/vfile.h>
#include "../yaffs2-583dbd9/yaffs2-583dbd9/direct/yaffsfs.h"

extern struct FileOperations g_tYAF2FileOps; // ./mount.c文件中定义
//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: !NULL -- 成功; NULL -- 失败;
//备注: 
//-----------------------------------------------------------------------------
struct FileContext *YAF2Open(const char *Path, u32 Mode, const char *Root)
{
	int YFlag = 0;
    int YMode = S_IREAD | S_IWRITE;
	char *YPath = PathCopy(Root, Path, NULL);
	struct FileContext *FileCt;
	struct FileBuf *Buf;
	
	FileCt = AllocContext();
	if(!FileCt)
		return (NULL);
		
	FileCt->FileOps = (void *)&g_tYAF2FileOps;

	if((Mode & M_READ) && (Mode & M_WRITE))
	{
		YFlag |= O_RDWR;
		FileCt->Property = P_READ | P_WRITE;
	}
	else if(Mode & M_READ)
	{
		YFlag |= O_RDONLY;
		FileCt->Property = P_READ;
	}
	else if(Mode & M_WRITE)
	{
		YFlag |= O_WRONLY;
		FileCt->Property = P_WRITE;
	}
	else
		goto FAIL;// 参数错误
	
	if(Mode & M_CREATE)
	{
		if(Mode & M_OPEN)
			YFlag |= O_CREAT;// 创建或打开已存在
		else
			YFlag |= O_CREAT | O_EXCL;// 仅创建
	}
		
	if(Mode & M_TRUNCATE)
		YFlag |= O_TRUNC;
		
	if(Mode & M_APPEND)
		YFlag |= O_APPEND;	

	if(!(Mode & M_TYPE))// 参数未指定，则两个都要尝试下
		Mode |= M_REG | M_DIR;
		
	if(Mode & M_REG)// 文件类型
	{
		int Handle;
		Handle = yaffs_open(YPath, YFlag, YMode);
		if(Handle < 0)
			goto FAIL;//todo: 已存在文件是何结果待检验

		if(AllocFileBuf(&Buf, 0))// 为文件创建缓冲区
		{
			yaffs_close(Handle);
			if((Mode & M_CREATE) && (!(Mode & M_OPEN))) // 失败则删除已创建的文件
				yaffs_unlink(YPath);
			goto FAIL;
		}
		
		FileCt->Property |= P_REG;
		FileCt->Private = (void *)Handle;
		FileCt->Buf = Buf;
		return (FileCt);// 成功
	}

	if(Mode & M_DIR)// 目录类型
	{
		yaffs_DIR *Dir;
		if(YFlag & O_CREAT)
		{
			int Ret;
			Ret = yaffs_mkdir(YPath, YMode);
			if(Ret < 0)// 文件已存在,则表示创建错误;或没有空间
				goto FAIL;
		}

		Dir = yaffs_opendir(YPath);
		if(NULL == Dir)
			goto FAIL;

		FileCt->Property |= P_DIR;
		FileCt->Private = (void *)Dir;
		FileCt->Buf = NULL;
		return (FileCt);// 成功
	}

FAIL: //失败
	FreeContext(FileCt);
	return (NULL);
		

}
//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: 
//备注: 
//-----------------------------------------------------------------------------
s32 YAF2Close(struct FileContext *FileCt)
{
	int Ret;
	
	switch(FileCt->Property & P_TYPE)
	{
		case P_REG:
		{
			int File = (int)FileCt->Private;
			Ret = yaffs_close(File);
			if(!Ret)
			{
				FreeFileBuf(FileCt->Buf);
				break;// 成功
			}
			return (-2);// 失败
		}
		case P_DIR:
		{
			yaffs_DIR *Dir = (yaffs_DIR*)FileCt->Private;
			Ret = yaffs_closedir(Dir);
			if(!Ret)
				break;// 成功
			return (-2);// 失败
		}
		default:
			return (-2);// 失败
	}
	
	free(FileCt);
	return (0);
}
//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: 
//备注: 
//-----------------------------------------------------------------------------
s32 YAF2Seek(struct FileContext *FileCt, s64 Offset, s32 Whence)
{
	Y_LOFF_T Ret;
	int Handle = (int)(FileCt->Private);
	
	Ret = yaffs_lseek(Handle, Offset, Whence);
    if (-1 == Ret)
		return (-1);
		
	return (0);
	
}
//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: 0 -- 成功; -3 -- 失败;
//备注: 
//-----------------------------------------------------------------------------
s32 YAF2Sync(struct FileContext *FileCt)
{
	int Ret;
	int Handle = (int)FileCt->Private;

	Ret = yaffs_flush(Handle);
	if(Ret)
		return (-3);
	return (0);
}
//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: 0 -- 成功; -3 -- 错误;
//备注: 
//-----------------------------------------------------------------------------
s32 YAF2Delete(const char *Path, const char *Root)
{
	int Ret;
	char *YPath = PathCopy(Root, Path, NULL);
	u32 Length;
	Length = strlen(Path);
	
	// 检查路径是否说明,删除的是目录还是文件;如果'/'结尾，表示删除的是目录
	if(Path[Length] != '/')
	{
		Ret = yaffs_unlink(YPath);
		if(!Ret)
			return (0);// 成功
	}
	
	Ret = yaffs_rmdir(YPath);
	if(!Ret)
		return (0);// 成功
		 
	return (-3);
}

//-----------------------------------------------------------------------------
//功能:
//参数: Size -- 保证不为零
//返回: >0 -- 成功; 0 -- 失败;
//备注: 
//-----------------------------------------------------------------------------
s32 YAF2Write(const void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt)
{
	int Ret;
    int File = (int)FileCt->Private;
    u32 AllSize = Size * Nmemb;

    Ret = yaffs_write(File, Buf, AllSize);
	if(-1 == Ret)
		return (0);// 失败

    return (Ret/Size);
}
//-----------------------------------------------------------------------------
//功能:
//参数: Size -- 保证不为零
//返回: >0 -- 成功; 0 -- 失败;
//备注: 
//-----------------------------------------------------------------------------
s32 YAF2Read(void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt)
{
	int Ret;
    int File = (int)FileCt->Private;
    u32 AllSize = Size * Nmemb;

    Ret = yaffs_read(File, Buf, AllSize);
	if(-1 == Ret)
		return (0);// 失败

    return (Ret/Size);
}

//-----------------------------------------------------------------------------
//功能: 查询文件状态
//参数:
//返回: 0 -- 成功; -1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
s32 YAF2Stat(struct FileContext *FileCt, const char *Path, struct Stat *Buf, const char *Root)
{
	int Ret;
	int File;
	struct yaffs_stat FileInfo;
	char *YPath;

	if(FileCt)// 文件已经在缓冲中
	{
		Buf->Property = FileCt->Property;
		if(FileCt->Property & P_REG)
		{
			File = (int)FileCt->Private;
			Ret = yaffs_fstat(File, &FileInfo);
			if(-1 == Ret)
				return (-1);
			Buf->Size = (long long)FileInfo.st_size;
		}
		else
		{
			Buf->Property = FileCt->Property;
			Buf->Size = 0;
		}
		return (0);

	}

	// 文件不在缓冲中
	YPath = PathCopy(Root, Path, NULL);
	Ret = yaffs_stat(YPath, &FileInfo);
	if(-1 == Ret)
		return (-1);

	Buf->Size = (long long)FileInfo.st_size;

	Buf->Property = P_READ | P_WRITE;
	if(FileInfo.st_mode & S_IFDIR)
	{
		Buf->Property |= P_DIR;
		Buf->Size = 0; // yaffs2的目录大小不为零
	}
	else if(FileInfo.st_mode & S_IFREG)
		Buf->Property |= P_REG;
	else if(FileInfo.st_mode & S_IFLNK)
		Buf->Property |= P_LINK;

	return (0);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: 0 -- 成功; -1 -- 失败;
//备注: todo: off_t类型待议
//-----------------------------------------------------------------------------
s32 YAF2Truncate(struct FileContext *FileCt, off_t NewSize)
{
	int File;

	File = (int)FileCt->Private;
	return(yaffs_ftruncate(File, (Y_LOFF_T)NewSize));// todo
}
//-----------------------------------------------------------------------------
//功能: 读目录内容
//参数:
//返回: 1 -- 结束,全部读完;
//      -1 -- 参数错误; -2 -- 函数不支持; -3 -- 其他错误;
//      0 -- 成功读取返回;
//备注: 每读一次,返回一个目录项
//-----------------------------------------------------------------------------
// 由YAFFS2控制的结构体
struct yaffsfs_DirSearchContext {
    struct yaffs_dirent de;
    YCHAR name[NAME_MAX + 1];
    struct yaffs_obj *dirObj;
    struct yaffs_obj *nextReturn;
    struct list_head others;
    s32 offset:20;
    u8 inUse:1;
};
s32 YAF2DirRead(struct FileContext *FileCt, struct Dirent *Content)
{
	 u32 FileType;
	 struct yaffs_dirent *De;
	 struct yaffsfs_DirSearchContext *Dsc;
	 yaffs_DIR *Dir = (yaffs_DIR*)FileCt->Private;

	 Dsc = (struct yaffsfs_DirSearchContext *)Dir;
	 if(Dsc->inUse && Dsc->nextReturn)
	 {
		 FileType = yaffs_get_obj_type((struct yaffs_obj *)Dsc->nextReturn);
		 switch(FileType)
		 {
		 	 case DT_REG: Content->Property = P_REG; break;
		 	 case DT_DIR: Content->Property = P_DIR; break;
		 	 case DT_LNK: Content->Property = P_LINK; break;
		 	 default : return (-3); // 其他类型不支持
		 }
	 }

	 De = yaffs_readdir(Dir);
	 if(NULL == De)
		 return (1);

	 strcpy(Content->Name, De->d_name);

	 return (0);
}
