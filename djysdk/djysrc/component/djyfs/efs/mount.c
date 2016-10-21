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
#include <driver/flash/flash.h>
#include <djyfs/filesystems_configs.h>
#include "efs_port.h"

//
// EFS的文件系统操作接口
//
struct FileOperations EfsFileOps =
{
	.FileOpen       = EfsOpen,
	.FileClose      = EfsClose,
	.FileDelete     = EfsDelete,
	.FileSync       = EfsSync,
	.FileSeek       = EfsSeek,
	.FileWrite      = EfsWrite,
	.FileRead       = EfsRead,
	.FileStat       = EfsStat,
	.DirRead        = EfsDirRead,
};
//
// EFS的文件系统类型
//
struct FileSysType EFS =
{
	.FileOps = &EfsFileOps,
	.VOps = NULL,
	.Install = EfsInstall,
	.Uninstall = EfsUninstall,
	.Format = EfsFormat,
	.Property = MOUNT_FILE,
	.MountList = NULL,
	.Name = "EFS",
};
//-----------------------------------------------------------------------------
//功能:1.对盘符“EFS”的全局扫描；2.初始化EFS文件
//参数: 
//返回: 0 -- 成功; -1 -- 失败;
//备注: 
//-----------------------------------------------------------------------------
#define INSTALL_MAKE_NEW 			(0x1)
s32 EfsInstall(struct MountInfo *Info, void *Private)
{
	char *DiskName;
	tagEFS *NewEfs;
	FSRESULT Ret;
	u32 Length;

	Length = strlen(Info->Dev->Name);
	DiskName = malloc(Length + 2);// 文件系统根名,FAT下就是分区名
	if(NULL == DiskName)
		return (-1);

	strcpy(DiskName, Info->Dev->Name);
	DiskName[Length] = ':';
	DiskName[Length+1] = '\0';

	NewEfs = malloc(sizeof(tagEFS));
	if(NULL == NewEfs)
		return (-1);
	
	memset(NewEfs,0x00,sizeof(tagEFS));
    Ret = Efs_Mount(NewEfs, DiskName, 1);// 挂载
    if ((FSR_NO_FILESYSTEM == Ret) && ((*(u32*)Private) & INSTALL_MAKE_NEW))
    	Ret = Efs_Mkfs(NewEfs,DiskName, 1, 0);//设备上不存在文件系统，则新建

    if(FSR_OK != Ret)// 失败
    {
    	free(NewEfs);
    	return (-1);
    }

    // 成功
    Info->Mount.Name = DiskName;
	Info->Private = (void*)NewEfs;
	return (0);
}

//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: 0 -- 成功;
//备注: 
//-----------------------------------------------------------------------------
s32 EfsUninstall(struct MountInfo *Info)
{
	
	free(Info->Mount.Name);
	free(Info->Private);
	return (0);
}

//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: 0 -- 成功; -1 -- 参数错误; -3 -- 格式化失败;
//备注: 
//-----------------------------------------------------------------------------
s32 EfsFormat(void *Private)
{
	FSRESULT Ret;
	tagEFS *NewEfs = (tagEFS *)Private;

	if(!NewEfs)
		return (-1);

	Ret = Efs_Mkfs(NewEfs, NewEfs->name, 1, 0);
	if(FSR_OK != Ret)// 失败
		return (-3);

	return (0);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: 0 -- 成功;
//备注:
//-----------------------------------------------------------------------------

#ifdef EFS_ON
extern struct NandDescr *s_ptNandInfo;
extern bool_t EFS_IF_Erase(u32 BlkNo);
extern u32 EFS_IF_ReadData(u32 BlkNo, u32 Offset, u8 *Buf, u32 Size, u8 Flags);
extern u32 EFS_IF_WriteData(u32 BlkNo, u32 Offset, u8 *Buf, u32 Size, u8 Flags);
extern bool_t EFS_IF_CheckBlockReady(u32 block,u32 offset, u8 *buf,u32 size);
extern s32 EFS_IF_IsFragmentWritten(u32 BlkNo, u32 Offset);

bool_t ModuleInit_EfsInfo(tagEFS* efs,char *para)
{
	//芯片初始化
	u8 *chip_sector_buf;

    chip_sector_buf = (u8*)malloc(EFS_CFG_LIMIT);
    if( (chip_sector_buf == NULL) || (s_ptNandInfo == NULL))
        return FALSE;


    efs->block_buf_mutex = Lock_MutexCreate("EFS_BLOCK_BUF_SEMP");
    if(NULL == efs->block_buf_mutex)
    {
    	free(chip_sector_buf);
    	return FALSE;
    }

    strcpy(efs->name,para);
    efs->block_size          = s_ptNandInfo->BytesPerPage * s_ptNandInfo->PagesPerBlk;//块大小
    efs->start_block         = 0;//该文件系统起始sector
    efs->block_sum           = s_ptNandInfo->BlksPerLUN;//sector用于存放该文件系统
    efs->block_buf           = chip_sector_buf;
    efs->files_sum           = 0;
    efs->erase_block         = EFS_IF_Erase;
    efs->query_block_ready   = EFS_IF_CheckBlockReady;
    efs->read_data           = EFS_IF_ReadData;
    efs->write_data          = EFS_IF_WriteData;
    return true;

}
#else
bool_t ModuleInit_EfsInfo(tagEFS* efs,char *para)
{
	return FALSE;
}
#endif
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: 0 -- 成功;
//备注:
//-----------------------------------------------------------------------------
bool_t Efs_Initlization(tagEFS* efs,char *para)
{
	return ModuleInit_EfsInfo(efs,para);
}


