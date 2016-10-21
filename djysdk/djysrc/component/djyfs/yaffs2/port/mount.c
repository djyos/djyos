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

#include <driver.h>
#include <djyos.h>
#include "../../mount.h"
#include "../../vfile.h"
#include "./yaffs2_port.h"
#include "./drivers/yaffs_drv_generic.h"

//
// YAFFS2文件系统接口
//
struct FileOperations g_tYAF2FileOps =
{
    .FileOpen       = YAF2Open,
    .FileClose      = YAF2Close,
    .FileDelete     = YAF2Delete,
    .FileSync       = YAF2Sync,
    .FileSeek       = YAF2Seek,
    .FileWrite      = YAF2Write,
    .FileRead       = YAF2Read,
    .FileStat       = YAF2Stat,
    .FileTruncate   = YAF2Truncate,
    .DirRead        = YAF2DirRead,
};
//
// FAT的文件系统类型
//
struct FileSysType YAF2 =
{
    .FileOps   = &g_tYAF2FileOps,
    .VOps      = NULL,
    .Install   = YAF2Install,
    .Uninstall = YAF2Uninstall,
    .Property  = MOUNT_FILE,
    .MountList = NULL,
    .Name      = "YAF2",
};

//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: 0 -- 成功; -1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
#define ECC_ON                  ((u32)0x1) // 启用ECC
#define FORMAT                  ((u32)0x2) // 格式化器件
s32 YAF2Install(struct MountInfo *Info, void *Private)
{
    struct yaffs_dev *YDev;
    struct yaffs_param *Param;
    struct FlashChip *Chip;
    struct DjyDevice *DevInfo;
    int Ret;
    unsigned int PageIndex;

    DevInfo = Container(Info->Dev, struct DjyDevice, Node);

    Chip = (struct FlashChip*)DevInfo->PrivateTag;

    if(*(u32*)Private & FORMAT)
    {
        Ret = EarseWholeChip(Chip);
        if(-1 == Ret)
            return (-1);
    }

    yaffs_start_up();

    YDev = malloc(sizeof(struct yaffs_dev)); // "yaffs_dev"类似于superblock作用
    if(NULL == YDev)
        return (-1); // 错误;

    memset(YDev, 0, sizeof(*YDev));

    Param = &YDev->param;
    Param->name = Info->Dev->Name;

    Param->n_reserved_blocks = 6;
    Param->is_yaffs2 = 1;
    Param->n_caches = 0; // 0 -- 不使用cache逻辑
    Param->use_nand_ecc = 0; // 挂载过程未使用ECC，这个对一个新芯片很重要的逻辑
    Param->disable_summary = 1; // 除去summary功能，todo
    if(F_NAND == Chip->Type)
    {
        Param->total_bytes_per_chunk = Chip->Descr.Nand.BytesPerPage; // todo: 考虑nand页小于512的情况
        Param->chunks_per_block = Chip->Descr.Nand.PagesPerBlk;
        Param->start_block = Chip->Descr.Nand.ReservedBlks;
        Param->end_block = (Chip->Descr.Nand.BlksPerLUN * Chip->Descr.Nand.LUNs) - 1; // 例如总块数2048,则范围"0-2047"

        Ret = yaffs_nand_install_drv(YDev, Chip); // 初始化YAFFS2驱动层接口
    }
    else if(F_NOR == Chip->Type)
    {
        // 文件头结构体最大是512Bytes另外还有Tags是16Bytes,
        if(Chip->Descr.Nor.BytesPerPage > 512+16)
        {
            Param->total_bytes_per_chunk = Chip->Descr.Nor.BytesPerPage;
            Param->chunks_per_block = Chip->Descr.Nor.PagesPerSector * Chip->Descr.Nor.PagesPerSector;
        }
        else
        {
            Param->total_bytes_per_chunk = 1024;
            PageIndex = Param->total_bytes_per_chunk / Chip->Descr.Nor.BytesPerPage; // todo: 此处转变为位运算
            Param->chunks_per_block = Chip->Descr.Nor.PagesPerSector * Chip->Descr.Nor.PagesPerSector / PageIndex;
        }

        Param->start_block = Chip->Descr.Nor.ReservedBlks;
        Param->end_block = Chip->Descr.Nor.Blks - 1;
        Param->inband_tags = 1; // tag存放于内页

        Ret = yaffs_nor_install_drv(YDev, Chip); // 初始化YAFFS2驱动层接口
    }
    else
    {
        Ret = YAFFS_FAIL; // 其他设备暂不支持
    }


    if (YAFFS_FAIL == Ret)
    {
        free(YDev);
        return (-1);
    }

    yaffs_add_device(YDev); // 将"yaffs_dev"注册进yaffs2

    Ret = yaffs_mount_reldev(YDev); // 挂载
    if(-1 == Ret)
    {
        yaffs_remove_device(YDev);
        free(YDev);
        return (-1);
    }

    // 如果驱动并不支持ECC，在驱动层会有一次判断
    if(*(u32*)Private & ECC_ON)
        Param->use_nand_ecc = 1;// 使用ECC，只能放到挂载完成后


    Info->Mount.Name = Chip->Name;
    Info->Private = (void*)YDev;

    return (0); // 挂载yaffs2文件系统成功
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: 0 -- 成功; -1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
s32 YAF2Uninstall(struct MountInfo *Info)
{
    struct yaffs_dev *YDev;
    int Ret;

    YDev = (struct yaffs_dev*)(Info->Private);
    Ret = yaffs_unmount_reldev(YDev);
    if(!Ret)// 成功,继续释放"yaffs_dev"
    {
        yaffs_remove_device(YDev);
        free(YDev);
    }

    return (Ret);
}
//-----------------------------------------------------------------------------
//功能: 实现与OS相关的一些逻辑;
//参数:
//返回: "0" -- 成功；
//      "-1" -- 失败。
//备注: 需用户实现的代码.
//-----------------------------------------------------------------------------
int yaffs_start_up(void)
{
    static int start_up_called = 0;

    if(start_up_called)
        return (0);
    start_up_called = 1;

    yaffsfs_OSInitialisation();

    return (0);
}

