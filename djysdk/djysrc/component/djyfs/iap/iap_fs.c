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
//TODO: 支持多个文件，那么就可以支持multiboot
#include <systime.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <driver.h>
#include <driver/flash/flash.h>
#include <djyfs/vfile.h>
#include <djyfs/mount.h>
#include <djyos.h>

//
// 底层接口函数
//
extern s32 LowLevelWrite(void *LowLevel, u8 *Buf, u32 Bytes, u32 Addr);
extern s32 LowLevelRead(void *LowLevel, u8 *Buf, u32 Bytes, u32 Addr);
extern s32 LowLevelErase(void *LowLevel, u32 Bytes, u32 Addr);
extern void *LowLevelInit(void *Dev, u32 *Base);
void LowLevelDeInit(void *LowLevel);
extern s32 LowLevelFormat(void *LowLevel);
extern u32 LowLevelCRC32(u32 Offset, u32 Len);

//
// 本地函数
//
static struct FileContext *IAP_Open(const char *Path, u32 Mode,
									const char *Root);
static s32 IAP_Close(struct FileContext *FileCt);
static s32 IAP_Delete(const char *Path, const char *Root);
static s32 IAP_Write(const void *Buf, u32 Size, u32 Nmemb,
					 struct FileContext *FileCt);
static s32 IAP_Read(void *Buf, u32 Size, u32 Nmemb,
					struct FileContext *FileCt);
static s32 IAP_DirentRead(struct FileContext *FileCt, struct Dirent *Content);
static s32 IAP_Stat(struct FileContext *FileCt, const char *Path,
					struct Stat *Buf, const char *Root);
static s32 IAP_FS_Install(struct MountInfo *Info, void *Private);
static s32 IAP_FS_Uninstall(struct MountInfo *Info);

//
// IAP的文件系统操作接口
//
struct FileOperations IAP_FileOps =
{
    .FileOpen       = IAP_Open,
    .FileClose      = IAP_Close,
    .FileDelete     = IAP_Delete,
    .FileSync       = NULL,
    .FileSeek       = NULL,
    .FileWrite      = IAP_Write,
    .FileRead       = IAP_Read,
    .FileStat       = IAP_Stat,
    .DirRead        = IAP_DirentRead,
};

//
// IAP的文件系统类型
//
struct FileSysType FS_IAP =
{
    .FileOps = &IAP_FileOps,
    .VOps = NULL,
    .Install = IAP_FS_Install,
    .Uninstall = IAP_FS_Uninstall,
    .Format = NULL,
    .Property = MOUNT_FILE,
    .MountList = NULL,
    .Name = "IAP",
};

#define S_APP_UPDATE_DONE				((u32)0x61707000)
#define S_APP_UPDATE_ING           		((u32)0xFFFFFFFF)
#define S_APP_DEBUG				  		((u32)0x00676264)
#define MAX_FILES						(1) // 支持的最大文件数
#define FILE_HEAD_SIZE          		((u32)256)
#define FILE_HEAD_OFFSET				(FILE_HEAD_SIZE)
#define FILE_NAME_MAX_LEN				(240) // 支持的最大文件名长度(包括结束符)

//
// 文件格式（头部）
//
struct HeadFormat {
	u32 Size;
	u32 CRC;
	u32 Reserved;
	u32 Signature;
	char Name[240];
};

//
// 文件系统管理
//
struct IAP_FS_Manager {
    void *Vol; // 文件系统底层抽象
	u32 Base; // 文件系统在volume中的偏置
    struct IAP_File	*Files[MAX_FILES]; // 文件系统内最多支持存放文件数
};

//
// IAP 文件描述
//
struct IAP_File
{
	u32 Base; // 文件在文件系统空间中的偏置
	u32 Cur;
	u32 Size;
	u16 State;
	u16 ID; // 从零计，作用于多文件
	char *Name;
};

//
//
//
struct IAP_Dir
{
    u8 DirentIndex;
};

static struct IAP_FS_Manager *s_ptIAP_Core;
//-----------------------------------------------------------------------------
//功能: 解析文件格式(头部)
//参数: FileHead -- 文件头部格式
//返回: 文件描述符
//备注:
//-----------------------------------------------------------------------------
static struct IAP_File *__FileDecode(struct HeadFormat *FileHead)
{
	u16 i;
	u16 NameLen;
	struct IAP_File *IFile;
	u8 *Temp = (u8*)FileHead;
	
	for(i = 0; i < FILE_HEAD_SIZE; i++)
	{	
		if(0xFF != Temp[i])
			break; 
	}		
	
	if(i == FILE_HEAD_SIZE)
	{
		printf("IAP: file head all 0xFF. no files\r\n");
		return (NULL); // 全FF数据，无效
	}
	
	if((S_APP_UPDATE_DONE != FileHead->Signature) &&
	   (S_APP_DEBUG != FileHead->Signature))
	{
		printf("IAP: file is bad.\r\n");
		return (NULL); // 格式错误
	}
	
	IFile = malloc(sizeof(struct IAP_File));
	if(!IFile)
	{
		printf("IAP: memory out.\r\n");
		return (NULL);
	}	
	
	NameLen = strlen(FileHead->Name) + 1; // 1是结束符
	if(FILE_NAME_MAX_LEN < NameLen)
	{
		printf("IAP: too long file name: %d .\r\n", NameLen);
		goto FAILURE;
	}
	
	IFile->Name = malloc(NameLen);
	if(!IFile->Name)
	{
		printf("IAP: memory out.\r\n");
		goto FAILURE;
	}
	
	strcpy(IFile->Name, FileHead->Name);
	IFile->Cur = 0;
	IFile->Size = FileHead->Size;
	IFile->State = (u16)S_APP_UPDATE_DONE;
	return (IFile);
	
FAILURE:
	if(IFile)
		free(IFile);
	return (NULL);	
}

//-----------------------------------------------------------------------------
//功能: 计算文件的CRC值
//参数:
//返回: 文件的CRC32值
//备注:
//-----------------------------------------------------------------------------
static u32 __CalculateFileCRC(struct IAP_File *IFile)
{
    char *Data = (char*)(IFile->Base + FILE_HEAD_OFFSET);
	u32 Len = IFile->Size;

    return ((u32)LowLevelCRC32((u32)Data, Len));
}

//-----------------------------------------------------------------------------
//功能: 建立文件格式（头部）
//参数:
//返回: -1 -- 失败；0 -- 成功。
//备注:
//-----------------------------------------------------------------------------
s32 __FileEncode(struct IAP_File *IFile)
{
	s32 Res;
	struct HeadFormat FileHead;
	
	if((u16)S_APP_UPDATE_DONE == IFile->State)
		return (-1);
		
	FileHead.Size = IFile->Size;
	strcpy(FileHead.Name, IFile->Name);
	FileHead.Signature = S_APP_UPDATE_DONE;
	FileHead.Reserved = -1;
	FileHead.CRC = __CalculateFileCRC(IFile);	
	Res = LowLevelWrite(s_ptIAP_Core->Vol, (u8*)&FileHead, FILE_HEAD_SIZE,
						IFile->Base);
	if(FILE_HEAD_SIZE != Res)
		return (-1);
	
	IFile->State = (u16)S_APP_UPDATE_DONE;	
	return (0);
}

//-----------------------------------------------------------------------------
//功能: 获取文件的基地址
//参数: Core -- 文件系统
//      Index -- 文件标号
//返回: 文件的基地址（volume内部）
//备注:
//-----------------------------------------------------------------------------
static u32 __FileBase(struct IAP_FS_Manager *Core, u32 Index)
{
	return (Core->Base + Index);
}

//-----------------------------------------------------------------------------
//功能: 扫描文件
//参数:
//返回: 
//备注: TODO：多文件逻辑
//-----------------------------------------------------------------------------
s32 __ScanFiles(struct IAP_FS_Manager * Core)
{
	s32 Res;
	struct HeadFormat FileHead;
	struct IAP_File *IFile;
	
	Res = LowLevelRead(Core->Vol, (u8*)&FileHead, FILE_HEAD_SIZE, Core->Base);
	if(Res != FILE_HEAD_SIZE)
		return (-1);
	
	IFile = __FileDecode(&FileHead);
	if(!IFile)
	{
		printf("IAP: format the disk, please wait ... ");
		Res = LowLevelFormat(s_ptIAP_Core->Vol); // 不存在有效文件，为保险起见，格式化整个vol
		printf("done\r\n");
		if(Res)
			return (-1);
		return (0); // 当前系统无文件，后续逻辑不执行
	}
	
	IFile->ID = 0;
	IFile->Base = __FileBase(Core, IFile->ID);
	Core->Files[IFile->ID] = IFile;
	return (0);
}

//-----------------------------------------------------------------------------
//功能: 加载已存在文件
//参数: Index -- 文件标号
//返回:
//备注: 未考虑互斥
//-----------------------------------------------------------------------------
static struct IAP_File *__LoadFile(u32 Index)
{
	struct IAP_File *IFile;
	
	if(Index >= MAX_FILES)
		return (NULL);

	IFile = s_ptIAP_Core->Files[Index];
	if(!IFile)
		return (NULL);
		
	return (IFile);	
}

//-----------------------------------------------------------------------------
//功能: 查找文件
//参数: FileName -- 文件名
//返回:
//备注: 未考虑互斥
//-----------------------------------------------------------------------------
static struct IAP_File *__FindFile(const char *FileName)
{
	s32 Res;
	u32 Index;
	struct IAP_File *IFile;
	
	for(Index = 0; Index < MAX_FILES; Index++)
	{
		IFile = __LoadFile(Index);
		if(!IFile)
			return (NULL);
			
		Res = strcmp(FileName, IFile->Name);	
		if(!Res)
			return (IFile);
	}
	
	return (NULL);
}


//-----------------------------------------------------------------------------
//功能: 重新建文件
//参数: FileName -- 文件名
//返回:
//备注: 如果文件已存在,则将其线删除
//-----------------------------------------------------------------------------
static struct IAP_File *__NewFile(const char *FileName)
{
    s32 Res = 0;
    u32 EraseBytes;
	u32 NameLen = strlen(FileName)+1;
	struct IAP_File *IFile = s_ptIAP_Core->Files[0];

	if(IFile) // 文件已存在则先格式化文件
	{
		EraseBytes = IFile->Size + FILE_HEAD_SIZE;
		Res = LowLevelErase(s_ptIAP_Core->Vol, EraseBytes, IFile->Base);
		if(EraseBytes != Res)
			return (NULL);
	}	
	else
	{
		IFile = malloc(sizeof(struct IAP_File) + NameLen);
		if(!IFile)
		{
			printf("IAP: memery out.\r\n");
			return (NULL);
		}
		IFile->Name = ((char*)IFile) + sizeof(struct IAP_File);
		IFile->ID = 0;
		s_ptIAP_Core->Files[IFile->ID] = IFile;
	}

	strcpy(IFile->Name, FileName);
	IFile->State = (u16)S_APP_UPDATE_ING;
	IFile->Base = __FileBase(s_ptIAP_Core, IFile->ID);
	IFile->Cur = 0;
	IFile->Size = 0;
	return (IFile);
}

//-----------------------------------------------------------------------------
//功能: 打开文件
//参数:
//返回: 非NULL -- 成功; NULL -- 失败;
//备注:
//-----------------------------------------------------------------------------
static struct FileContext *IAP_Open(const char *Path, u32 Mode, const char *Root)
{
    struct FileContext *FileCt;
    struct FileBuf *Buf;

    if((Mode & M_APPEND) || (Mode & M_LINK))
        return (NULL); // 当前不支持的模式

    if((Path) && (Mode & M_DIR)) // 除非根目录,其他目录不能创建或打开
        return (NULL);

    FileCt = AllocContext();
    if(!FileCt)
        return (NULL);

    FileCt->FileOps = (void *)&IAP_FileOps;
    if(Mode & M_READ)
        FileCt->Property |= P_READ;

    if(Mode & M_WRITE)
        FileCt->Property |= P_WRITE;

    if(!(Mode & M_TYPE)) // 参数未指定,默认为打开文件
        Mode |= M_REG;

    if(Mode & M_REG) // 打开文件
    {
        struct IAP_File *IFile;

        if(Mode & M_CREATE)
            IFile = __NewFile(Path); // 如果是新建的话,就直接先将原文件删除.这里不管原文件是什么
        else
            IFile = __FindFile(Path);

        if(IFile)
        {
			if(!AllocFileBuf(&Buf, 1)) // 强制申请Buf,M7的内置Flash存在对齐问题
			{
				FileCt->Property |= P_REG;
				FileCt->Private = (void *)(IFile);
				FileCt->Buf = Buf;
				return (FileCt); // 成功;
			}  
        }
		// 失败;
    }

    //IAP文件系统不支持打开目录，但有一级根目录.如果创建二级目录则失败
    if((Mode & M_DIR) && (!(Mode & M_CREATE)) ) // 打开目录
    {
        struct IAP_Dir *Root = (struct IAP_Dir*)malloc(sizeof(*Root));
        if(Root)
        {
            Root->DirentIndex = 0;
            FileCt->Property |= P_DIR;
            FileCt->Buf = NULL;
            FileCt->Private = (void *)(Root);
            return (FileCt); // 成功
        }
    }

    // 失败
    FreeContext(FileCt);
    return (NULL);
}

//-----------------------------------------------------------------------------
//功能: 关闭文件
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static s32 IAP_Close(struct FileContext *FileCt)
{
    if((FileCt->Property & P_REG) && (FileCt->Property & P_WRITE))
    {
        s32 Res;
        struct IAP_File *IFile = (struct IAP_File*)FileCt->Private;

        if((u16)S_APP_UPDATE_ING == IFile->State)
        {
            Res = __FileEncode(IFile); // 文件关闭时，建立文件头
            if(Res)
                return (-1);
        }
        IFile->Cur = 0;
    }

    if(FileCt->Property & P_DIR)
        free(FileCt->Private);
		
	if(FileCt->Buf)
		free(FileCt->Buf);

    FreeContext(FileCt);
    return (0);
}

//-----------------------------------------------------------------------------
//功能: 写文件
//参数:
//返回: 实际写入的单元数
//备注:
//-----------------------------------------------------------------------------
static s32 IAP_Write(const void *Buf, u32 Size, u32 Nmemb,
					 struct FileContext *FileCt)
{
    s32 Res;
	u32 Offset;
	u32 Bytes = Size * Nmemb;
    struct IAP_File *IFile = (struct IAP_File*)FileCt->Private;

	Offset = IFile->Cur + FILE_HEAD_SIZE + IFile->Base;	
	Res = LowLevelWrite(s_ptIAP_Core->Vol, (u8*)Buf, Bytes, Offset);
	IFile->Cur += Res;
	IFile->Size = IFile->Cur;
	return ((s32)(Res/Size));
}

//-----------------------------------------------------------------------------
//功能: 读文件
//参数:
//返回: 实际读出的单元数
//备注:
//-----------------------------------------------------------------------------
static s32 IAP_Read(void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt)
{
    s32 Res;
    u32 BytesToRd;
    struct IAP_File *IFile = (struct IAP_File*)FileCt->Private;
	u32 Offset = IFile->Cur + FILE_HEAD_SIZE + IFile->Base;

    BytesToRd = Size * Nmemb;
    if((IFile->Cur + BytesToRd) > IFile->Size) // 越界
        BytesToRd = IFile->Size - IFile->Cur;

    Res = LowLevelRead(s_ptIAP_Core->Vol, Buf, BytesToRd, Offset);
	IFile->Cur += Res;
    return ((s32)(Res/Size));
}

//-----------------------------------------------------------------------------
//功能: 删除文件
//参数:
//返回:
//备注: 未考虑互斥
//-----------------------------------------------------------------------------
static s32 __DelFile(const char *FileName)
{
    s32 Res;
	struct IAP_File *IFile;
	u32 EraseBytes;

	IFile = __FindFile(FileName);
	if(!IFile)
		return (-1); // 文件未找到

	EraseBytes = IFile->Size + FILE_HEAD_SIZE;
	Res = LowLevelErase(s_ptIAP_Core->Vol, EraseBytes, IFile->Base);
	if(EraseBytes != Res)
		return (-1); // 擦除失败
		
	s_ptIAP_Core->Files[IFile->ID] = NULL;
	free(IFile);

	return (0);
}
//-----------------------------------------------------------------------------
//功能: 文件查询
//参数:
//返回: 0 -- 成功; -1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
static s32 IAP_Delete(const char *Path, const char *Root)
{
	return (__DelFile(Path));
}
//-----------------------------------------------------------------------------
//功能: 文件查询
//参数:
//返回: 0 -- 成功; -1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
static s32 IAP_Stat(struct FileContext *FileCt, const char *Path,
					struct Stat *Buf, const char *Root)
{
    if(FileCt)
    {
        struct IAP_File *IFile = (struct IAP_File*)FileCt->Private;
        Buf->Property = FileCt->Property;
        Buf->Size = IFile->Size;
        return (0);
    }

    if(Path)
    {
        struct IAP_File *IFile = __FindFile(Path);

        if(!IFile)
            return (-1);

        Buf->Size = IFile->Size;
        Buf->Property = P_READ | P_REG;
        return (0);
    }

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
static s32 IAP_DirentRead(struct FileContext *FileCt, struct Dirent *Content)
{
    struct IAP_Dir *Root;
    struct IAP_File *IFile;

    if((!Content) || (!FileCt))
        return (-1);

	Root = (struct IAP_Dir*)FileCt->Private;
	IFile = __LoadFile(Root->DirentIndex);
    if(IFile)
    {
        strcpy(Content->Name, IFile->Name);
        Content->Property = P_REG | P_READ;
        Root->DirentIndex += 1;
        return (0);
    }
    
	return (1); // 结束
}

//-----------------------------------------------------------------------------
//功能: 文件系统安装
//参数:
//返回: 0 -- 成功; -1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
static s32 IAP_FS_Install(struct MountInfo *Info, void *Private)
{
	s32 Res;
	u32 CoreBase;
	struct DjyDevice *Dev;

    if((s_ptIAP_Core) || (!Info))
	{
        printf("\r\nIAP: has been installed.\r\n");
		return (-1);
	}
	
    s_ptIAP_Core = malloc(sizeof(*s_ptIAP_Core));
    if(!s_ptIAP_Core)
	{
        printf("\r\nIAP: nmemory out.\r\n");
		return (-1);
	}	
	
    memset(s_ptIAP_Core, 0x0, sizeof(*s_ptIAP_Core));
    Dev = Container(Info->Dev, struct DjyDevice, Node);
    // 底层设备驱动接口
	s_ptIAP_Core->Vol = LowLevelInit((void*)Dev->PrivateTag, &CoreBase);
	if(NULL == s_ptIAP_Core->Vol)
	{
		free(s_ptIAP_Core);
		s_ptIAP_Core = NULL;
		return (-1);
	}
	
	s_ptIAP_Core->Base = CoreBase;
	Res = __ScanFiles(s_ptIAP_Core); // 扫描已存在文件
	if(Res)
	{
		free(s_ptIAP_Core);
		s_ptIAP_Core = NULL;
		return (-1);
    }
	
	Info->Mount.Name = NULL;
    Info->Private = NULL;
    return (0);
}

//-----------------------------------------------------------------------------
//功能: 文件系统卸载
//参数:
//返回: 0 -- 成功;
//备注:
//-----------------------------------------------------------------------------
static s32 IAP_FS_Uninstall(struct MountInfo *Info)
{
    LowLevelDeInit(s_ptIAP_Core->Vol);
	free(s_ptIAP_Core);
    s_ptIAP_Core = NULL;
    return (0);
}

//-----------------------------------------------------------------------------
//功能: 安装IAP
//参数: Dir -- 安装目录
//返回: -1 -- 失败； 0 -- 成功。
//备注:
//-----------------------------------------------------------------------------
s32 ModuleInstall_IAP_FS(const char *Dir)
{
    s32 Ret;
	char *Path = "/iboot"; // 默认安装目录

	if(Dir)
		Path = (char*)Dir;
		
    RegisterFSType(&FS_IAP);
    Ret = open(Path, O_DIRECTORY | O_CREAT | O_RDWR, 0); // 创建/iboot目录 
    if(-1 == Ret)
    {
        printf("\r\nIAP: create diretionay \"/iboot\" error\r\n");
        return (-1);
    }

    Ret = ModuleInstall_EmbededFlash("embedded flash", FLASH_BUFFERED, 0);
    if(Ret)
    {
        printf("\r\nIAP: install \"embedded flash\" error\r\n");
        return (-1);
    }

    Ret = Mount(Path, "/dev/embedded flash", "IAP", NULL);
    if(Ret < 0)
    {
        printf("\r\nIAP: mount \"IAP\" file system error\r\n");
        return (-1);
    }

    return (0);
}

//-----------------------------------------------------------------------------
//功能: 安装BOOTFS
//参数: Dir -- 安装目录
//返回: -1 -- 失败； 0 -- 成功。
//备注:
//-----------------------------------------------------------------------------
#include "stdio.h"
void LocalTest(void)
{
	u8 *TestData;
	u32 i, TestLen = 0x2000;
	s32 Res, Handle;
	char *TestFile = "/iboot/test.txt";

	TestData = malloc(0x2000);
	if(!TestData)
		while(1);

	for(i = 0; i < TestLen; i++)
		TestData[i] = (u8)i;

	Res = ModuleInstall_IAP_FS(NULL);
	if(Res)
		while(1);

	Handle = open(TestFile, 2 | (00000100));
	if(-1 == Handle)
		while(1);

	Res = write(Handle, TestData, TestLen);
	if(TestLen != Res)
		while(1);

	Res = close(Handle);
	if(Res)
		while(1);

	Handle = open(TestFile, 2);
		if(-1 == Handle)
			while(1);

	Res = read(Handle, TestData, TestLen);
	if(TestLen != Res)
		while(1);

	for(i = 0; i < TestLen; i++)
	{
		if(TestData[i] != (u8)i)
			while(1);
	}

	Res = close(Handle);
	if(Res)
		while(1);

//	Res = remove(TestFile);
//	if(Res)
		while(1);
}
