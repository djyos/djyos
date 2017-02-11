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
#include "fat_port.h"

extern struct FileOperations g_tFATFileOps; // ./mount.c文件中定义
extern char *PathCopy(const char *Root, const char *RelPath, char(*Rebuild)(char *Path));

//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: 非NULL -- 成功; NULL -- 失败;
//备注: 
//-----------------------------------------------------------------------------
struct FileContext *FATOpen(const char *Path, u32 Mode, const char *Root)
{
	char *LocalPath;
	struct FileContext *FileCt;
	struct FileBuf *Buf;
	FRESULT Res = FR_OK;
	u8 LocalMode = 0;
	
	// 先排除不支持功能
	if((Mode & M_APPEND) || (Mode & M_LINK))
		return (NULL);// 当前不支持的模式,(Mode & M_TRUNCATE)还没有处理
	
	LocalPath = PathCopy(Root, Path, NULL);// todo:考虑放入虚拟层
	
	FileCt = AllocContext();
	if(!FileCt)
		return (NULL);

	FileCt->FileOps = (void*)&g_tFATFileOps;

	// todo: 读写属性都没有怎么处理？
	if(Mode & M_READ)
	{
		LocalMode |= FA_READ;
		FileCt->Property |= P_READ;
	}	
	
	if(Mode & M_WRITE)
	{
		LocalMode |= FA_WRITE;
		FileCt->Property |= P_WRITE;
	}	
	
	if(Mode & M_CREATE)
	{
		if((Mode & M_OPEN))
			LocalMode |= FA_OPEN_ALWAYS;// 如果文件不存在,则创建一个再打开.如此便能保证fopen打开一次性打开一个已存在文件
		else
			LocalMode |= FA_CREATE_NEW;	
	}	
	
	if(!(Mode & M_TYPE))// 参数未指定，则两个都要尝试下
		Mode |= M_REG | M_DIR;
		
	if(Mode & M_REG)// 打开文件
	{
		FIL *NewFile = malloc(sizeof(FIL));
		if(NewFile)
		{	
			Res = f_open(NewFile, LocalPath, LocalMode);// 打开文件
			if((FR_OK == Res) || 
			   ((FR_EXIST == Res) && (Mode & M_OPEN)))//打开已存在文件
			{
				if(0 == AllocFileBuf(&Buf, 0)) // 为文件创建缓冲区
				{
					FileCt->Property |= P_REG;
					FileCt->Private = (void *)(NewFile);
					FileCt->Buf = Buf;
					return (FileCt);// 成功
				}
				else
				{
					f_close(NewFile); // 失败
					free(NewFile);
					FreeContext(FileCt);
					if((Mode & M_CREATE) && (!(Mode & M_OPEN)))
						f_unlink(LocalPath);
					return (NULL);
				}
			}
			// 打开文件失败
			free(NewFile);
		}	
	}
	
	if(Mode & M_DIR)// 打开目录
	{
		_DIR *NewDir;
		
		if(LocalMode & FA_CREATE_NEW)// 需创建，先试图创建目录
		{
			Res = f_mkdir(LocalPath);
		}
		if(FR_OK == Res)
		{
			NewDir = malloc(sizeof(_DIR));
			if(NewDir)
			{
				Res = f_opendir(NewDir, LocalPath);
				if(FR_OK == Res)
				{
					FileCt->Property |= P_DIR;
					FileCt->Private = (void *)(NewDir);
					FileCt->Buf = NULL;
					return (FileCt);// 成功
				}
				// 打开目录失败
				free(NewDir);
			}
		}
	}
	
	// 失败
	FreeContext(FileCt);
	return (NULL);

}
//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: 0 -- 成功; -2 -- 失败;
//备注: 
//-----------------------------------------------------------------------------
s32 FATClose(struct FileContext *FileCt)
{
	FRESULT Res;
	
	switch(FileCt->Property & P_TYPE)
	{
		case P_REG:
		{
			FIL *File = (FIL *)(FileCt->Private);
			Res = f_close(File);
			if(FR_OK == Res)
			{
				FreeFileBuf(FileCt->Buf);
				break;
			}
			return (-2);// 失败
		}
		case P_DIR:
		{
			_DIR *Dir = (_DIR*)(FileCt->Private);
			Res = f_closedir(Dir);
			if(FR_OK == Res)
				break;
			return (-2);// 失败
		}
		default:
			return (-2);// 失败
	}
	
	free(FileCt->Private);// 释放"FIL"或"_DIR"
	FreeContext(FileCt);
	return (0);
	
}
//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: 0 -- 成功; -3 -- 失败;
//备注: 
//-----------------------------------------------------------------------------
s32 FATSync(struct FileContext *FileCt)
{
	FRESULT Res;
    FIL *File = (FIL*)(FileCt->Private);
	
	Res = f_sync(File);
    if (FR_OK != Res)
        return (-3);
		
    return (0);
}
//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: 0 -- 成功; -1 -- 操作失败; -2 -- 参数错误;
//备注: 
//-----------------------------------------------------------------------------
s32 FATSeek(struct FileContext *FileCt, s64 Offset, int Whence)
{
	FRESULT Res;
    DWORD NewPos;
    FIL *File = (FIL*)FileCt->Private;
	
	switch(Whence)
	{
		case V_SEEK_SET: NewPos = Offset;break;
		case V_SEEK_CUR: NewPos = Offset + File->fptr;break;
		case V_SEEK_END: NewPos = Offset + File->fsize; break;
		default: return (-2);// 参数错误
	}
	
	Res = f_lseek(File, NewPos);
    if (FR_OK != Res)
        return (-1);
    return (0);
}
//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: 
//备注: 
//-----------------------------------------------------------------------------
s32 FATDelete(const char *Path, const char *Root)
{
	FRESULT Res;
	char *LocalPath = PathCopy(Root, Path, NULL);
	
	Res = f_unlink(LocalPath);
	if(FR_OK != Res)
		return (-3);
	return (0);	
}
//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: !0 -- 成功; 0 -- 失败;
//备注: 
//-----------------------------------------------------------------------------
s32 FATWrite(const void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt)
{

	u32 Result;
    FIL *File = (FIL*)FileCt->Private;	
	u32 Length = Size * Nmemb;
	
	f_write(File, Buf, Length, &Result);
    
	return((s32)Result);
}
//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: !0 -- 成功; 0 -- 失败;
//备注: 
//-----------------------------------------------------------------------------
s32 FATRead(void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt)
{

	u32 Result;
    FIL *File = (FIL*)FileCt->Private;	
	u32 Length = Size * Nmemb;
	
	f_read(File, Buf, Length, &Result);
	
	return((s32)Result);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: 0 -- 成功; -1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
s32 FATStat(struct FileContext *FileCt, const char *Path, struct Stat *Buf, const char *Root)
{
	FIL *File;
	char *LocalPath;
	FILINFO FileInfo = {0};// 要初始化，否则源程序会跑飞
	FRESULT Res;

	if(FileCt)
	{
		if(FileCt->Property & P_REG)
		{
			File = (FIL*)FileCt->Private;
			Buf->Size = File->fsize;
		}
		else
		{
			Buf->Size = 0;
		}

		Buf->Property = FileCt->Property;
		return (0);
	}

	if(Path && Root)
	{
		LocalPath = PathCopy(Root, Path, NULL);
		Res = f_stat(LocalPath, &FileInfo);
		if(FR_OK != Res)
			return (-1);

		Buf->Size = FileInfo.fsize;

		if(FileInfo.fattrib & AM_RDO) //只读
			Buf->Property = P_READ;
		else
			Buf->Property = P_READ | P_WRITE;

		if(FileInfo.fattrib & AM_DIR)// 目录
			Buf->Property |= P_DIR;

		if(FileInfo.fattrib & AM_ARC)// 文档
			Buf->Property |= P_REG;

		return (0);
	}

	return (-1);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: 0 -- 成功; -1 -- 失败;
//备注: todo: off_t类型待议
//-----------------------------------------------------------------------------
s32 FATTruncate(struct FileContext *FileCt, off_t NewSize)
{
	FIL *File;
	FRESULT Res;

	File = (FIL*)(FileCt->Private);

	File->fptr = (DWORD)NewSize; // todo

	Res = f_truncate(File);
	if(FR_OK == Res)
		return (0);
	return (-1);
}

//-----------------------------------------------------------------------------
//功能: 读目录内容
//参数:
//返回: 1 -- 结束,全部读完;
//      -1 -- 参数错误; -2 -- 函数不支持; -3 -- 其他错误;
//      0 -- 成功读取返回;
//备注: 每读一次,返回一个目录项
//-----------------------------------------------------------------------------
s32 FATDirRead(struct FileContext *FileCt, struct Dirent *Content)
{
	FILINFO ItemInfo;
	char LongNameBuf[256];
	FRESULT Res;
	_DIR *Dir = (_DIR *)FileCt->Private;
	
	ItemInfo.lfname = LongNameBuf;
	Res = f_readdir(Dir, &ItemInfo);
	if(FR_OK !=Res)
		return (-3);
		
	if(0 == ItemInfo.fname[0])
		return (1);
		
	strcpy(Content->Name, ItemInfo.fname);
	switch(ItemInfo.fattrib & 0x30)
	{
		case AM_ARC: Content->Property = P_REG; break;
		case AM_DIR: Content->Property = P_DIR; break;
		default: return (-3); // 尚未支持的文件格式
	}
	return (0);
}