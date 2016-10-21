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
#include <stddef.h>
#include <djyos.h>
#include <systime.h>
#include "file.h"

//-----------------------------------------------------------------------------
//功能: 模式转换
//参数: mode -- 模式
//返回: 非0 -- 成功; 0 -- 失败;
//备注:
//-----------------------------------------------------------------------------
static u32 __ToMode(const char *mode)
{
	u32 Mode;
	u8 StrEnd = 0; // 字符串结束标志

	Mode = M_OPEN; // 打开与创建可共存，todo: 支持所有类型

	switch(mode[0])
	{
		case 'r': Mode |= M_READ;break;
		case 'w': Mode |= M_WRITE | M_CREATE | M_TRUNCATE;break;
		case 'a': Mode |= M_WRITE | M_CREATE | M_APPEND;break;
		default : return (0); // 参数格式错误
	}

	switch(mode[1])
	{
		case '\0': Mode |= M_TEXT; StrEnd = 1;break;
		case 'b': Mode |= M_BIN;break;
		case '+': Mode |= M_WRITE | M_READ;break;
		default : return (0); // 参数格式错误
	}

	if(!StrEnd)
	{
		switch(mode[2])
		{
			case '\0':
				if(!(Mode & M_BIN))
					Mode |= M_TEXT; // todo
				break;
			case 'b':
				if('b' == mode[1])
					return (0); // 出现两次'b', 则格式错误
				Mode |= M_BIN;
				break;

			case '+':
				if('+' == mode[1])
					return (0); // 出现两次'+', 则格式错误
				Mode |= M_WRITE | M_READ;
				break;
			default : return (0); // 参数格式错误
		}
	}

	return (Mode);

}
//-----------------------------------------------------------------------------
//功能: 打开文件
//参数: filename -- 文件所在路径;
//      mode -- 文件操作模式;
//返回: 非NULL -- 成功; NULL -- 失败;
//备注:
//-----------------------------------------------------------------------------
FILE *fopen(const char *filename, const char *mode)
{
	u32 Mode;
	
	if((!filename) || (!mode)  || ('\0' == *filename))
		return (NULL);

	Mode = __ToMode(mode);
	if(Mode)
		return(__Open(filename, Mode));
	else
		return (NULL);
}
//-----------------------------------------------------------------------------
//功能: 关闭文件
//参数: stream -- 文件流
//返回: 0 -- 成功;
//      -1 -- 参数错误; -2 -- 文件类型错误; 其他;
//备注: todo:容错处理,情景(重复关闭一个文件).
//-----------------------------------------------------------------------------
int fclose(FILE *stream)
{
	s32 Ret;
	
	if(NULL == stream)
		return (-1);

	Ret = stream->VOps->VClose(stream->Private);
	if(!Ret)
		FreeVFile(stream);// 成功，释放句柄.

	return (Ret);
}
//-----------------------------------------------------------------------------
//功能: 读文件
//参数: ptr -- 流缓冲;
//      size -- 需要读的单位数;
//      nmemb -- 单位数;
//      stream -- 文件流;
//返回: 0 -- 失败; >0 -- 成功读出的字节数;
//备注:
//-----------------------------------------------------------------------------
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	s32 Ret;

	if((NULL == ptr) || (NULL == stream))
		return (0);
	
	if(0 == (size * nmemb))
		return (0);

	Ret = stream->VOps->VRead(ptr, size, nmemb, stream->Private);
	if(Ret < 0)
		return (0);

	return ((size_t)Ret);
}
//-----------------------------------------------------------------------------
//功能: 写文件
//参数: ptr -- 流缓冲;
//      size -- 需要写的单位数;
//      nmemb -- 单位数;
//      stream -- 文件流;
//返回:  0 -- 失败; >0 -- 实际写入的字节数
//备注:
//-----------------------------------------------------------------------------
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	s32 Ret;

	if((NULL == ptr) || (NULL == stream))
		return (0);
	
	if(0 == (size * nmemb))
		return (0);
		
	Ret = stream->VOps->VWrite(ptr, size, nmemb, stream->Private);
	if(Ret < 0)
		return (0);

	return((size_t)Ret);
}
//-----------------------------------------------------------------------------
//功能: 移动读写位置
//参数: stream -- 文件流;
//      offset -- 需移动相对偏置;
//      whence -- offset的起点;
//返回: 0 -- 成功; 
//      -1 -- 文件类型错误; -2 -- 参数错误; -3 -- 文件写入错误;
//      -4 -- 文件系统不支持seek;
//备注: 不支持设备文件
//-----------------------------------------------------------------------------
int fseek(FILE *stream, long int offset, int whence)
{
	struct FileContext *FileCt;
	struct VCommon *FileCom;
	struct FileBuf *Buf;
	struct FileOperations *FileOps;
	u32 Length;
	s32 Front, Back;

	if(NULL == stream)
		return (-2);
		
	FileCt = (struct FileContext*)stream->Private;
	FileCom = FileCt->CHost;
	if(FileCom->Obj.Type != OB_REG)
		return (-1);// 必须是文件
	
	FileOps = (struct FileOperations*)(FileCt->FileOps);
	if((NULL == FileOps) || (NULL == FileOps->FileSeek))
		return (-4);
	
	Buf = FileCt->Buf;	
	if(Buf && (BUF_DIRTY == Buf->Status))
	{
		switch(whence)
		{
			case V_SEEK_SET:
			case V_SEEK_END:	
			{
				Length = Buf->Current - Buf->Start;
				if((NULL == FileOps->FileWrite) || 
				   (Length != FileOps->FileWrite(Buf->Start, Length, 1, FileCt)))
					return (-3);
				Buf->Current = Buf->Start;
				Buf->Status = BUF_CLEAN;
				break;
			}	
			case V_SEEK_CUR:
			{
				Front = Buf->Start - Buf->Current;// Buffer已用空间
				Back = Buf->End - Buf->Current;// Buffer未用空间
				if((offset >= Front) && (offset <= Back))// 新seek的位置在Buffer内
				{
					Buf->Current += offset;
					return (0);
				}
				else// 新seek的位置超出Buffer范围,将Buffer数据先写入设备
				{
					Length = Buf->Current - Buf->Start;
					if((NULL == FileOps->FileWrite) || 
					   (Length != FileOps->FileWrite(Buf->Start, Length, 1, FileCt)))
						return (-3);
					Buf->Current = Buf->Start;
					Buf->Status = BUF_CLEAN;
				}
				break;
			}	
			default:
				return (-2);
		}
	}	
		
	return (FileOps->FileSeek(FileCt, offset, whence));	
}
//-----------------------------------------------------------------------------
//功能: 删除文件或目录
//参数: filename -- 文件所在路径;
//返回: 0 -- 成功; -1 -- 参数错误; -2 -- 文件无法删除; -3 -- 其他;
//备注: 不支持设备文件
//-----------------------------------------------------------------------------
int remove(const char *filename)
{
	struct VCommon *FileCom;
	struct PathWalk PathContext;
	struct FileOperations *FileOps;
	u8 IsFound;
	
	if((NULL == filename) || ('\0' == *filename))
		return (-1);

	__PathInit(&PathContext, filename);
	
	IsFound = __VPathLookup(&PathContext, 0);
	
	
	if(IsFound)
	{
		FileCom = Container(PathContext.End, struct VCommon, Obj);
		if(FileCom->Obj.Child || FileCom->Obj.Inuse || FileCom->Context)
			return (-2);// 文件存在子节点或正在被使用，不能被删除
	}

	FileOps = __FileOps(&PathContext);

	if(NULL == FileOps->FileDelete)
		return (-2);// 该文件系统不支持本函数
	
	return (FileOps->FileDelete(PathContext.Path, PathContext.Root));
}
//-----------------------------------------------------------------------------
//功能: 将文件缓冲区数据写回文件所在设备
//参数: stream -- 文件流;
//返回: 0 -- 成功; -1 -- 输入参数错误; -2 -- 函数不支持; -3 -- 其他错误;
//备注: 不支持设备文件
//-----------------------------------------------------------------------------
int fflush(FILE *stream)
{
	struct FileContext *FileCt;
	struct VCommon *FileCom;
	struct FileBuf *Buf;
	struct FileOperations *FileOps;
	u32 Length;
	
	if(NULL == stream)
		return (-1);
	
	FileCt = (struct FileContext*)stream->Private;
	FileCom = FileCt->CHost;
	
	if(FileCom->Obj.Type != OB_REG)
		return (-1);// 不是文件类型，应该返回EOF
		
	FileOps = (struct FileOperations*)(FileCt->FileOps);
	if((NULL == FileOps) || (NULL == FileOps->FileSync))
		return (-2);
	
	Buf = FileCt->Buf;
	if(Buf && (BUF_DIRTY == Buf->Status))// 先将虚拟层的缓冲刷入设备
	{
		Length = Buf->Current - Buf->Start;
		if((NULL == FileOps->FileWrite) || 
		   (Length != FileOps->FileWrite(Buf->Start, Length, 1, FileCt)))
			return (-3);// 写入失败
		Buf->Current = Buf->Start;
		Buf->Status = BUF_CLEAN;
	}
		
	return (FileOps->FileSync(FileCt));	
}
//-----------------------------------------------------------------------------
//功能: 打开文件
//参数:
//返回: 非NULL -- 成功; NULL -- 失败;
//备注:
//-----------------------------------------------------------------------------
FILE *freopen(const char *filename, const char *mode, FILE *stream)
{
	struct FileContext *FileCt;
	u32 NewProperty = 0;
	u32 Mode;
	u8 IsFound;
	struct PathWalk PathCt;
	s32 Ret = -1;

	if(((NULL == filename) && (NULL == stream)) || (NULL == mode))
		return (NULL);

	if(filename)
	{
		Mode = __ToMode(mode);
		if(0 == Mode)
			return (NULL);

		__PathInit(&PathCt, filename);

		IsFound = __VPathLookup(&PathCt, 1); // 缓存中检索

		if(PathCt.VOps->VOpen) // 系统文件不支持该函数
			Ret = PathCt.VOps->VOpen(&PathCt, Mode, IsFound);

		if(Ret < 0)
			return (NULL);

		if((stream->VOps)&&(stream->VOps->VClose)) // 关闭stream上下文
			stream->VOps->VClose(stream->Private);

		if(PathCt.PrivateVHost)
			*(PathCt.PrivateVHost) = stream;
		stream->Private = PathCt.Private; // 实际是FileContext
		stream->VOps = PathCt.VOps;

		return (stream);
	}

	// 未指定新文件，则直接变更文件属性,todo:更改底层属性
	FileCt = (struct FileContext *)stream->Private;

	switch(mode[0])
	{
		case 'r': NewProperty |= P_READ;break;
		case 'w': NewProperty |= P_WRITE;break;
		case 'a': NewProperty |= P_WRITE;break;
		default : return (NULL);// 参数格式错误
	}

	switch(mode[1])
	{
		case '\0':
		case 'b' : break;
		case '+' : NewProperty |= P_WRITE | P_READ;break;
		default  : return (NULL);// 参数格式错误
	}

	if('\0' != mode[1])
	{
		switch(mode[2])
		{
			case '\0':
			case 'b' :
				if('b' == mode[1])
					return (NULL);// 出现两次'b', 则格式错误
				break;

			case '+' :
				if('+' == mode[1])
					return (NULL);// 出现两次'+', 则格式错误
				NewProperty |= P_WRITE | P_READ;
				break;

			default  : return (NULL); // 参数格式错误
		}
	}

	FileCt->Property &= ~(P_WRITE | P_READ);
	FileCt->Property |= NewProperty;
	return (stream);
}
