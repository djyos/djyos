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


static struct FileContext *IAPOpen(const char *Path, u32 Mode, const char *Root);
static s32 IAPClose(struct FileContext *FileCt);
static s32 IAPDelete(const char *Path, const char *Root);
static s32 IAPWrite(const void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt);
static s32 IAPRead(void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt);
static s32 IAPDirentRead(struct FileContext *FileCt, struct Dirent *Content);
static s32 IAPStat(struct FileContext *FileCt, const char *Path, struct Stat *Buf, const char *Root);
static s32 IAP_FS_Install(struct MountInfo *Info, void *Private);
static s32 IAP_FS_Uninstall(struct MountInfo *Info);

//
// IAP的文件系统操作接口
//
struct FileOperations IAPFileOps =
{
    .FileOpen       = IAPOpen,
    .FileClose      = IAPClose,
    .FileDelete     = IAPDelete,
    .FileSync       = NULL,
    .FileSeek       = NULL,
    .FileWrite      = IAPWrite,
    .FileRead       = IAPRead,
    .FileStat       = IAPStat,
    .DirRead        = IAPDirentRead,
};
//
// IAP的文件系统类型
//
struct FileSysType IAP_FS =
{
    .FileOps = &IAPFileOps,
    .VOps = NULL,
    .Install = IAP_FS_Install,
    .Uninstall = IAP_FS_Uninstall,
    .Format = NULL,
    .Property = MOUNT_FILE,
    .MountList = NULL,
    .Name = "IAP",
};
//
struct IAP{
    u32 Size;
    u32 CRC;
    u32 Reserved;
    u32 Signature;
#define S_APP_DONE			(u32)0x61707000
#define S_APP_PRO           (u32)-1
#define S_DEBUG				  (u32)0x00676264
    u8 FileName[240];
    u8 *HeadSpace;
    u8 *DataSpace;
#define BINOFFSET              (u32)256
};
//
struct IAP_FS{
    struct FlashChip *Super;
    struct IAP Head;
};
//
struct IAPFile
{
    struct IAP_FS *Info;
    u32 End;
    u32 Cur;
};
//
struct IAP_Dir
{
    u8 DirentIndex;
};

static u32 BinSpace;
static u32 BinInfoSpace;
static struct IAP_FS *sp_tFilesInfo;
//-----------------------------------------------------------------------------
//功能: 文件系统安装
//参数:
//返回: 0 -- 成功; -1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
static s32 IAP_FS_Install(struct MountInfo *Info, void *Private)
{
    struct DjyDevice *Dev;
    struct EmbdFlashDescr *Embd;
    u16 SizeofFileInfo = sizeof(*sp_tFilesInfo);

    if((sp_tFilesInfo) || (!Info))
        return (-1);

    sp_tFilesInfo = malloc(SizeofFileInfo);
    if(!sp_tFilesInfo)
        return (-1);
    memset(sp_tFilesInfo, 0x0, SizeofFileInfo);

    Dev = Container(Info->Dev, struct DjyDevice, Node);
    sp_tFilesInfo->Super = (struct FlashChip*)Dev->PrivateTag;
    Embd = &sp_tFilesInfo->Super->Descr;
//    BinInfoSpace = Embd->BytesPerPage *
//                     (Embd->PagesPerSmallSect * Embd->SmallSectorsPerPlane +
//                      Embd->PagesPerLargeSect * Embd->LargeSectorsPerPlane);
//    if(Embd->SmallSectorsPerPlane)
//    	BinInfoSpace += (Embd->ReservedSects-1) * Embd->BytesPerPage * Embd->PagesPerNormalSect;
//    else
//    	BinInfoSpace += (Embd->ReservedSects) * Embd->BytesPerPage * Embd->PagesPerNormalSect;
    BinInfoSpace = Embd->ReservedPages  * Embd->BytesPerPage;
    BinSpace = BinInfoSpace + BINOFFSET;

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
    free(sp_tFilesInfo);
    sp_tFilesInfo = NULL;
    return (0);
}

//-----------------------------------------------------------------------------
//功能: 初始化(重置)文件信息
//参数:
//返回:
//备注: 未考虑互斥
//-----------------------------------------------------------------------------
static struct IAP_FS *__NewOrDelFile(const char *Path)
{
    s32 Ret = 0;
    struct EmbdFlashDescr *Embd = &(sp_tFilesInfo->Super->Descr);
    u32 SectorToErase,Sectors;

//    if(Embd->SmallSectorsPerPlane)
//    {
//    	Sectors = (Embd->NormalSectorsPerPlane+1) * Embd->Planes;//todo
//    }
//    else
//    {
//    	Sectors= (Embd->NormalSectorsPerPlane) * Embd->Planes;
//    }

    SectorToErase = (Embd->ReservedPages)/Embd->PagesPerNormalSect;

    Sectors =  (Embd->LargeSectorsPerPlane * Embd->PagesPerLargeSect +
    		Embd->SmallSectorsPerPlane * Embd->PagesPerSmallSect)
    				/ Embd->PagesPerNormalSect;
    Sectors= (Embd->NormalSectorsPerPlane + Sectors) * Embd->Planes;

    if(strlen(Path) > 239)
        return (NULL); // 文件名不能大于设定空间

    for(; SectorToErase < Sectors; SectorToErase++)
    {
        Ret = sp_tFilesInfo->Super->Ops.ErsBlk(SectorToErase);
        if(Ret)
            break;
    }

    if(!Ret)
    {
        sp_tFilesInfo->Head.Size = 0;
        sp_tFilesInfo->Head.CRC = 0;
        sp_tFilesInfo->Head.Reserved = -1;
        sp_tFilesInfo->Head.Signature = S_APP_PRO;
        if(Path)
        	strcpy(sp_tFilesInfo->Head.FileName, Path);
        sp_tFilesInfo->Head.HeadSpace = (u8*)BinInfoSpace;
        sp_tFilesInfo->Head.DataSpace = (u8*)BinSpace;
        return (sp_tFilesInfo);
    }

    return (NULL);
}
//-----------------------------------------------------------------------------
//功能: 获取文件信息
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static struct IAP_FS *__LoadFile(const char *Path)
{

    struct IAP *Head = (struct IAP*)BinInfoSpace;

    if(S_APP_DONE == Head->Signature)
    {
        sp_tFilesInfo->Head.Size = Head->Size;
        sp_tFilesInfo->Head.CRC = Head->CRC;
        sp_tFilesInfo->Head.Reserved = Head->Reserved;
        sp_tFilesInfo->Head.Signature = Head->Signature;
        strcpy(sp_tFilesInfo->Head.FileName, Head->FileName);
        sp_tFilesInfo->Head.HeadSpace = (u8*)BinInfoSpace;
        sp_tFilesInfo->Head.DataSpace = (u8*)BinSpace;
    }

    if(Path && strcmp(sp_tFilesInfo->Head.FileName, Path))
        return (NULL);
    else
        return (sp_tFilesInfo);
}
//-----------------------------------------------------------------------------
//功能: 打开文件
//参数:
//返回: 非NULL -- 成功; NULL -- 失败;
//备注:
//-----------------------------------------------------------------------------
static struct FileContext *IAPOpen(const char *Path, u32 Mode, const char *Root)
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

    FileCt->FileOps = (void *)&IAPFileOps;
    if(Mode & M_READ)
        FileCt->Property |= P_READ;

    if(Mode & M_WRITE)
        FileCt->Property |= P_WRITE;

    if(!(Mode & M_TYPE)) // 参数未指定,默认为打开文件
        Mode |= M_REG;

    if(Mode & M_REG) // 打开文件
    {
        struct IAPFile *IFile;
        struct IAP_FS *Info;

        if(Mode & M_CREATE)
            Info = __NewOrDelFile(Path); // 如果是新建的话,就直接先原文件删除
        else
            Info = __LoadFile(Path);

        if(Info)
        {
            IFile = (struct IAPFile*)malloc(sizeof(*IFile));
            if(IFile)
            {
                IFile->Info = Info;
                IFile->Cur = BINOFFSET;
                IFile->End = Info->Head.Size + IFile->Cur;
				if(!AllocFileBuf(&Buf, 1)) // 强制申请Buf,M7的内置Flash存在对齐问题
                {
                    FileCt->Property |= P_REG;
                    FileCt->Private = (void *)(IFile);
                    FileCt->Buf = Buf;
                    return (FileCt); // 成功;
                }
            }
            free(IFile); // 失败;
        }
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
//功能: 计算文件的CRC值
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static u32 __CalculateFileCRC(struct IAPFile *IFile)
{
    extern u32 crc32( char *buf, u32 len);
    extern u16 crc16( char *buf, u32 len);

    struct IAP_FS *Info = IFile->Info;

    return ((u32)crc32((char *)Info->Head.DataSpace, Info->Head.Size));

}
//-----------------------------------------------------------------------------
//功能: 关闭文件
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static s32 IAPClose(struct FileContext *FileCt)
{
    if((FileCt->Property & P_REG) && (FileCt->Property & P_WRITE))
    {
        s32 Ret;
        u32 PageNo, Offset;
        struct IAPFile *IFile = (struct IAPFile*)FileCt->Private;
        struct FlashChip *Super = IFile->Info->Super;
        struct IAP *FileHead = &(IFile->Info->Head);

        if(FileHead->Signature == S_APP_PRO)
        {
            FileHead->Size = IFile->End - BINOFFSET;
            FileHead->Signature = S_APP_DONE;
            Lock_MutexPend((struct MutexLCB*)Super->Lock, CN_TIMEOUT_FOREVER);
            FileHead->CRC = __CalculateFileCRC(IFile);
            PageNo = ((u32)FileHead->HeadSpace) / Super->Descr.Embd.BytesPerPage;
            Offset = ((u32)FileHead->HeadSpace) % Super->Descr.Embd.BytesPerPage;
            Ret = Super->Ops.RdPage(PageNo, Super->Buf, NO_ECC);
            if(Ret)
            {
                memcpy((Super->Buf+Offset), FileHead, BINOFFSET);
                Ret = Super->Ops.WrPage(PageNo, Super->Buf, NO_ECC);
            }
            Lock_MutexPost((struct MutexLCB*)Super->Lock);
            if(!Ret)
                return (-1);
        }
    }

    if(FileCt->Property & P_DIR)
        free(FileCt->Private);
	if(FileCt->Buf)
		free(FileCt->Buf);

    FreeContext(FileCt);
    return (0);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static s32 IAPSeek(struct FileContext *FileCt, s64 Offset, int Whence)
{
    ;// FTP需要
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static s32 IAPDelete(const char *Path, const char *Root)
{
	__NewOrDelFile(NULL);
	return (0);
}
//-----------------------------------------------------------------------------
//功能: 页号修正,剔除保留的页数量
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static u32 __PageFix(struct FlashChip *Super, u32 PageNo)
{
//    u32 ResevedPages;
    struct EmbdFlashDescr *Embd = &sp_tFilesInfo->Super->Descr;

//    ResevedPages = (Embd->PagesPerSmallSect * Embd->SmallSectorsPerPlane +
//                   Embd->PagesPerLargeSect * Embd->LargeSectorsPerPlane);
//
//	if(Embd->SmallSectorsPerPlane)
//    {
//		ResevedPages += (Embd->ReservedSects - 1) * Embd->PagesPerNormalSect;
//    }
//    else
//    {
//		ResevedPages = Embd->ReservedSects * Embd->PagesPerNormalSect + ResevedPages;
//    }


    return (PageNo + Embd->ReservedPages);

}
//-----------------------------------------------------------------------------
//功能: 写文件
//参数:
//返回: 实际写入的单元数
//备注:
//-----------------------------------------------------------------------------
static s32 IAPWrite(const void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt)
{
    u32 i, PageNo, Offset;
    s32 Ret;
    u32 LenToWrOrig, LenToWr, OnceToWr;
    struct IAPFile *IFile = (struct IAPFile*)FileCt->Private;
    struct FlashChip *Super = IFile->Info->Super;
    u8 *DataBuf = (u8*)Buf;

    LenToWrOrig = LenToWr = Size * Nmemb;
    Lock_MutexPend((struct MutexLCB*)Super->Lock, CN_TIMEOUT_FOREVER);
    PageNo = (IFile->Cur) / Super->Descr.Embd.BytesPerPage;
    PageNo = __PageFix(Super, PageNo);
    Offset = (IFile->Cur) % Super->Descr.Embd.BytesPerPage;
    while(LenToWr > 0)
    {
        OnceToWr = MIN((Super->Descr.Embd.BytesPerPage - Offset), LenToWr);
        if(OnceToWr != Super->Descr.Embd.BytesPerPage)
        {
        	Ret = Super->Ops.RdPage(PageNo, Super->Buf, NO_ECC);
            if(!Ret)
                goto FAILURE; //

            // 做简单的检查
            for(i = Offset; i < (Offset + OnceToWr); i++)
            {
                if((0xFF != Super->Buf[i]) && (Super->Buf[i] != DataBuf[i]))
                {
                    printf("data cannot be writen, offset %d, destination 0x%x, source 0x%x\r\n",
                            i, Super->Buf[i], DataBuf[i]);
                    goto FAILURE; //
                }
            }
        }

        memcpy((Super->Buf + Offset), DataBuf, OnceToWr);
        Ret = Super->Ops.WrPage(PageNo, Super->Buf, NO_ECC);
        if(Ret != Super->Descr.Embd.BytesPerPage)
            goto FAILURE;
        LenToWr -= OnceToWr;
        Offset = 0;
        PageNo++;
        DataBuf += OnceToWr;
        IFile->Cur += OnceToWr;
    }

FAILURE:	
	if(IFile->Cur > IFile->End)
		IFile->End = IFile->Cur;
	Lock_MutexPost((struct MutexLCB*)Super->Lock);	
	
	return ((s32)((LenToWrOrig - LenToWr)/Size));
}
//-----------------------------------------------------------------------------
//功能: 读文件
//参数:
//返回: 实际读出的单元数
//备注:
//-----------------------------------------------------------------------------
static s32 IAPRead(void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt)
{
    s32 Ret;
    u32 PageNo, Offset;
    u32 LenToRdOrig, LenToRd, OnceToRd;
    struct IAPFile *IFile = (struct IAPFile*)FileCt->Private;
    u8 *DataBuf = (u8*)Buf;
    struct FlashChip *Super = IFile->Info->Super;

    LenToRd = Size * Nmemb;
    if((IFile->Cur + LenToRd) > IFile->End)
        LenToRd = IFile->End - IFile->Cur;

    LenToRdOrig = LenToRd;
    Lock_MutexPend((struct MutexLCB*)Super->Lock, CN_TIMEOUT_FOREVER);
    PageNo = (IFile->Cur) / Super->Descr.Embd.BytesPerPage;
    PageNo = __PageFix(Super, PageNo);
    Offset = (IFile->Cur) % Super->Descr.Embd.BytesPerPage;
    while(LenToRd > 0)
    {
        OnceToRd = MIN((Super->Descr.Embd.BytesPerPage - Offset), LenToRd);
        Ret = Super->Ops.RdPage(PageNo, Super->Buf, NO_ECC);
        if(!Ret)
            goto FAILURE; //
        memcpy(DataBuf, (Super->Buf + Offset), OnceToRd);
        LenToRd -= OnceToRd;
        Offset = 0;
        PageNo++;
        DataBuf += OnceToRd;
        IFile->Cur += OnceToRd;
    }

FAILURE:
    Lock_MutexPost((struct MutexLCB*)Super->Lock);
    return((s32)((LenToRdOrig - LenToRd)/Size));
}
//-----------------------------------------------------------------------------
//功能: 文件查询
//参数:
//返回: 0 -- 成功; -1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
static s32 IAPStat(struct FileContext *FileCt, const char *Path, struct Stat *Buf, const char *Root)
{
    if(FileCt)
    {
        struct IAPFile *IFile = (struct IAPFile*)FileCt->Private;
        Buf->Property = FileCt->Property;
        Buf->Size = IFile->End;
        return (0);
    }

    if(Path)
    {
        struct IAP_FS *FileInfo = __LoadFile(Path);

        if(!FileInfo)
            return (-1);

        Buf->Size = FileInfo->Head.Size;
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
static s32 IAPDirentRead(struct FileContext *FileCt, struct Dirent *Content)
{
    struct IAP_FS *Info = __LoadFile(NULL);
    struct IAP_Dir *Root = (struct IAP_Dir*)FileCt->Private;

    if((!Content) || (!FileCt))
        return (-1);

    if((!Root->DirentIndex) &&
        (S_APP_DONE == Info->Head.Signature))
    {
        Root = (struct IAP_Dir*)FileCt->Private;
        strcpy(Content->Name, Info->Head.FileName);
        Content->Property = P_REG | P_READ;
        Root->DirentIndex += 1;
        return (0);
    }
    return (1);
}
//-----------------------------------------------------------------------------
//功能: 安装BOOTFS
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
s32 ModuleInstall_BootFS(void)
{
    s32 Ret;

    RegisterFSType(&IAP_FS);
    Ret = open("/iboot", O_DIRECTORY | O_CREAT | O_RDWR, 0);/* 创建/efs目录 */
    if(-1 == Ret)
    {
        printf("create diretionay \"/iboot\" error\r\n");
        return (-1);
    }

    Ret = ModuleInstall_EmbededFlash("embedded flash", FLASH_BUFFERED, 0);
    if(Ret)
    {
        printf("install \"embedded flash\" error\r\n");
        return (-1);
    }

    Ret = Mount("/iboot", "/dev/embedded flash", "IAP", NULL);
    if(Ret < 0)
    {
        printf("mount \"IAP_FS\" error\r\n");
        return (-1);
    }

    return (0);
}
