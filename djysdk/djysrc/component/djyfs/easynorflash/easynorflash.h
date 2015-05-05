//----------------------------------------------------
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
//所属模块:文件系统
//作者：lst
//版本：V1.0.0
//文件描述:easynor文件系统主模块
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者: 贺敏
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __EASY_FLASHFILE_H__
#define __EASY_FLASHFILE_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
#include "rsc.h"

#define CN_EASYNOR_PTT_NAME_LIMIT       31          //分区名最大长度,最后一个字节'\0'
#define CN_EASYNOR_FILENAME_LIMIT       31          //分区名最大长度,最后一个字节'\0'
#define CN_EASYNOR_CFG_LIMIT            16384       //配置信息长度
#define CN_EASYNOR_CFG_ITEM_LIMIT       256         //每个配置项的长度

//文件分配表各成员偏移量
#define CN_EASYNOR_NAME_OFF             0           //文件名，size=32
#define CN_EASYNOR_STARTBLOCK_OFF       32          //文件起始块，size=4
#define CN_EASYNOR_MAXSIZE_OFF          36          //文件最大长度
#define CN_EASYNOR_FILESIZE_OFF         40          //文件尺寸，正反码，size=8，共26个
#define CN_EASYNOR_FILESIZE_END         240         //最后一个文件尺寸
#define CN_EASYNOR_VERIFITY_OFF         248         //校验符，="easyfile"

#define CN_EASYNOR_FILESIZE_NUM         26
struct tagEasynorFlashChip
{
    struct tagRscNode   chip_node;
    u32 block_sum;         //总块数
    u32 block_size;        //块尺寸
    u32 start_block;       //用于存储文件的首块块号
    u32 files_sum;            //文件数
    u8 *block_buf;         //驱动提供的缓冲区，size= block_size
    char PTT_name[CN_EASYNOR_PTT_NAME_LIMIT+1];        //所创建的分区名字

    //以下是典型的flash操作函数指针，编写一个具体芯片的驱动时，设为具体芯片的
    //操作函数,这些函数并不是每一种芯片都需要，不需要的用NULL。

    //检查把参数规定的内容写入flash前，是否需要擦除。如果buf=NULL则检查由offset
    //和size设定的区域是否被擦除
    //true = 已准备好，不需要擦除(或空块),false = 需要擦除；
    bool_t   (*query_block_ready)(u32 block,u32 offset,
                                  u8 *buf,u32 size);
    //擦除1块，返回: true = 成功擦除，false = 坏块
    bool_t (*erase_block)(u32 block);
    //读一块之内的任意长度数据，返回校验过的正确数据
    //不带校验的读函数，返回 实际读取的数据量，参数错误则返回cn_limit_uint32
    u32 (*read_data)(u32 block,u32 offset,u8 *buf,u32 size);
    //不带校验的读函数，返回：cn_limit_uint32=参数错误，其他值：实际写入数量
    u32 (*write_data)(u32 block,u32 offset,u8 *buf,u32 size);
};

struct tagEasyFlashFileInfo
{
    u32 start_block;        //存储本文件的首块号。
    u32 max_size;           //本文件最大长度，filesize不能超过这个长度。
    u32 start_item;            //该文件分配表在分配表中的位置
    u32 filesize;
};



ptu32_t ModuleInstall_Easynor(ptu32_t para);
u32 Easynor_InstallChip(struct tagEasynorFlashChip *chip);
bool_t Easynor_CreateFile(char *filename, char *PTTname,u32 max_size);


#ifdef __cplusplus
}
#endif

#endif // __EASY_FLASHFILE_H__

