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

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "efs_port.h"


struct EFS_Dir{
	const char *Root;
	struct Dirent *CurDirent;
	u32    item;				//wenjian
};

extern struct FileOperations EfsFileOps;
//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: 非NULL -- 成功; NULL -- 失败;
//备注: 
//-----------------------------------------------------------------------------
struct FileContext *EfsOpen(const char *Path, u32 Mode, const char *Root)
{
	FSRESULT Ret;
	u8 LocalMode = 0;
	struct FileContext *FileCt;
	struct FileBuf *Buf;

	if((Mode & M_APPEND) || (Mode & M_LINK))
		return (NULL); // 当前不支持的模式

	if((NULL != Path) && (Mode & M_DIR))// 非EFS根目录不能创建或打开
		return (NULL);
	
	FileCt = AllocContext();
	if(!FileCt)
		return (NULL);
		
	FileCt->FileOps = (void *)&EfsFileOps;
	if(Mode & M_READ)
	{
		LocalMode |= EF_OP_READ;
		FileCt->Property |= P_READ;
	}	
	
	if(Mode & M_WRITE)
	{
		LocalMode |= EF_OP_WRITE;
		FileCt->Property |= P_WRITE;
	}	
	
	if(Mode & M_CREATE)
		LocalMode |= EF_OP_CREATE;

	if(Mode & M_OPEN)
		LocalMode |= EF_OP_OPEN;

	if(!(Mode & M_TYPE))// 参数未指定，则两个都要尝试下
		Mode |= M_REG;
		
	if(Mode & M_REG)// 打开文件
	{
		tagFileRsc *NewFile = malloc(sizeof(tagFileRsc));
		if(NewFile)
		{	
			Ret = Efs_FindFs(NewFile,Root);
			if(FSR_OK == Ret)
			{
				Ret = Efs_Open(NewFile, Path, LocalMode);// 打开文件
				if(FSR_OK == Ret)
				{
					if(!AllocFileBuf(&Buf, 0))
					{
						FileCt->Property |= P_REG;
						FileCt->Private = (void *)(NewFile);
						FileCt->Buf = Buf;
						return (FileCt);// 成功
					}
				}
			}
			// 打开文件失败
			free(NewFile);
		}	
	}

    // easy nor 不支持打开目录，因为只有一级根目录,如果创建目录则失败
	if((Mode & M_DIR) && (!(Mode & M_CREATE)) )// 打开目录
	{
		struct EFS_Dir *RootDir = malloc(sizeof(*RootDir));
		if(RootDir)
		{
			RootDir->Root = Root;
			RootDir->CurDirent = NULL;
			FileCt->Property |= P_DIR;
			FileCt->Buf = NULL;
			FileCt->Private = (void *)(RootDir);
			return (FileCt);// 成功
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
s32 EfsClose(struct FileContext *FileCt)
{
	FSRESULT Ret;
	
	switch(FileCt->Property & P_TYPE)
	{
		case P_REG:
		{
			tagFileRsc *File = (tagFileRsc *)(FileCt->Private);
			Ret = Efs_Close(File);
			if(FSR_OK == Ret)
			{
				FreeFileBuf(FileCt->Buf);
				break;
			}
			return (-2);// 失败
		}

		case P_DIR:
		{
			struct EFS_Dir *RootDir = (struct EFS_Dir *)(FileCt->Private);
			if(RootDir->CurDirent)
				free(RootDir->CurDirent);
			free(RootDir);
			break;
		}
		default:
			return (-2);// 失败
	}
	
	FreeContext(FileCt);
	return (0);
	
}
//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: 0 -- 成功; -3 -- 失败;
//备注: 
//-----------------------------------------------------------------------------
s32 EfsSync(struct FileContext *FileCt)
{		
    return (0);
}
//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: 0 -- 成功; -1 -- 操作失败; -2 -- 参数错误;
//备注: 
//-----------------------------------------------------------------------------
s32 EfsSeek(struct FileContext *FileCt, s64 Offset, int Whence)
{
	FSRESULT Ret;
    u32 NewPos;
    tagFileRsc *File = (tagFileRsc*)FileCt->Private;
	
	switch(Whence)
	{
		case V_SEEK_SET: NewPos = Offset;break;
		case V_SEEK_CUR: NewPos = Offset + File->ptr;break;
		case V_SEEK_END: NewPos = Offset + File->file_size; break;
		default: return (-2);// 参数错误
	}

	Ret = Efs_Seek(File,NewPos);
	if( FSR_OK != Ret)
		return (-1);
    return (0);
}

//-----------------------------------------------------------------------------
//功能: nor 只支持删除文件列表的最后一个谁的
//参数: 
//返回: 
//备注: 
//-----------------------------------------------------------------------------
s32 EfsDelete(const char *Path, const char *Root)
{
	FSRESULT Ret;

	Ret = Efs_Remove(Path,Root);
	if(FSR_OK != Ret)
		return (-3);
	return (0);	
}

//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: !0 -- 成功; 0 -- 失败;
//备注: 
//-----------------------------------------------------------------------------
s32 EfsWrite(const void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt)
{
	u32 Result;
	tagFileRsc *File = (tagFileRsc*)FileCt->Private;
	u32 Length = Size * Nmemb;
	
	Efs_Write(File, Buf, Length, &Result);
    
	return((s32)Result);
}

//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: !0 -- 成功; 0 -- 失败;
//备注: 
//-----------------------------------------------------------------------------
s32 EfsRead(void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt)
{
	u32 Result;
	tagFileRsc *File = (tagFileRsc*)FileCt->Private;
	u32 Length = Size * Nmemb;
	
	Efs_Read(File, Buf, Length, &Result);
	
	return((s32)Result);
}

//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: 0 -- 成功; -1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
s32 EfsStat(struct FileContext *FileCt, const char *Path,
					struct Stat *Buf, const char *Root)
{
	tagFileRsc *File;
	u32 FileSize = 0;
	FSRESULT Ret;

	if(FileCt)
	{
		File = (tagFileRsc *)FileCt->Private;
		Buf->Property = FileCt->Property;
		Buf->Size = File->file_size;
		return (0);
	}

	if(Path && Root)
	{
		Ret = Efs_Stat(Path, Root, &FileSize);
		if(FSR_OK != Ret)
			return (-1);

		Buf->Size = FileSize;
		Buf->Property = P_READ | P_WRITE | P_REG;
	}
	//根目录不考虑，已经存于内存之中了
	return (0);
}
//-----------------------------------------------------------------------------
//功能: 读目录内容
//参数:
//返回: 1 -- 结束,全部读完;
//      -1 -- 参数错误; -2 -- 函数不支持; -3 -- 其他错误;
//      0 -- 成功读取返回;
//备注: 每读一次,返回一个目录项
//-----------------------------------------------------------------------------
s32 EfsDirRead(struct FileContext *FileCt, struct Dirent *Content)
{
	FSRESULT ret;
	struct EFS_Dir *RootDir = (struct EFS_Dir *)(FileCt->Private);

	if( (NULL == FileCt) || (NULL == Content) )
		return (-1);
	
	if(NULL == RootDir->CurDirent)
	{
		RootDir->CurDirent = (struct Dirent *)malloc(sizeof(struct Dirent));
		if(NULL == RootDir->CurDirent)
			return (-3);

		memset(RootDir->CurDirent, 0x0, sizeof(struct Dirent));
		RootDir->item = 1;	//表示从第一个文件开始扫描
	}

	ret = Efs_DirRead(RootDir->CurDirent->Name, RootDir->Root, &(RootDir->item));
	if(FSR_OK == ret)
	{
		RootDir->CurDirent->Property |= P_REG;
		*Content = *(RootDir->CurDirent);
		return (0);
	}

	if(FSR_NO_FILE == ret)
		return (1);

	if(FSR_INVALID_PARAMETER == ret)
		return (-1);
	return (0);
}
