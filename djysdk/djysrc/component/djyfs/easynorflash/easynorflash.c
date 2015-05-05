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
//所属模块:easyfile system
//作者：lst
//版本：V1.0.0
//文件描述:文件系统与存储介质无关的部分
//其他说明:
//修订历史:
//2. ...
//1. 日期:
//   作者: 贺敏
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "config-prj.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"
#include "lock.h"
#include "systime.h"
#include "file.h"
#include "easynorflash.h"
#include "verify.h"
#include "endian.h"

static struct tagEasyFlashFileInfo *s_ptEasyNorInfo;    //todo:这个变量有疑问。
u16 Crc_16(u8 *buf, u32 len);

bool_t Easynor_Format_PTT(u32 fmt_para1,u32 fmt_para2,
                        struct tagPTTDevice *PTT_device_tag);
u32 Easynor_WriteFile(struct tagFileRsc *fp,uint8_t *buf,u32 len);
u32 Easynor_ReadFile(struct tagFileRsc *fp,uint8_t *buf,u32 len);
u32 Easynor_FlushFile(struct tagFileRsc *fp);
sint64_t Easynor_QueryFileStocks(struct tagFileRsc *fp);
sint64_t Easynor_QueryFileCubage(struct tagFileRsc *fp);
u32 Easynor_SeekFile(struct tagFileRsc *fp,struct tagSeekPara *position);
bool_t Easynor_CreateItem(char *name,struct tagFileRsc *parent,
                         struct tagFileRsc *result,union file_attrs attr,
                         enum _FILE_OPEN_MODE_ mode);
bool_t Easynor_MoveFile(struct tagFileRsc *src_fp,    struct tagFileRsc *dest_fp);
bool_t Easynor_RemoveFile(struct tagFileRsc *fp);
struct tagFileRsc * Easynor_ItemTraversalSon(struct tagFileRsc *parent,
                                             struct tagFileRsc *current);
u32 Easynor_OpenItem(char *name,struct tagFileRsc *parent,
                       struct tagFileRsc *result,enum _FILE_OPEN_MODE_ mode);
bool_t Easynor_LookforItem(char *name,struct tagFileRsc *parent,
                         struct tagFileRsc *result);
bool_t Easynor_CloseItem(struct tagFileRsc *fp);
bool_t Easynor_RenameItem(struct tagFileRsc *fp,char *newname);
void Easynor_CheckPTT(struct tagPTTDevice *PTT_device_tag,
                     sint64_t *sum_size,sint64_t *valid_size,sint64_t *free_size);
sint64_t Easynor_SetFileSize(struct tagFileRsc *fp,s64 new_size);
u32 Easynor_CheckFDT_Size(struct tagPTTDevice *PTT_device_tag);
u32 Easynor_CheckFolder(struct tagFileRsc *parent);

//flash芯片的资源结点，所有flash芯片都安装在此结点下
static struct tagRscNode tg_flash_chip_root_rsc;
struct tagSemaphoreLCB *g_ptEasyNorSemp; //保护flash_chip树所用的信号量

//----改写一块:覆盖数据--------------------------------------------------------
//功能: 把芯片块缓冲区的内容写入flash中某块，覆盖被写入的位置的原有数据，如果
//      在写入时遇到坏块，申请一个新块替换之，并把坏块登记到MAT表中(并不把MAT写
//      入到flash中)，返回替换后的块号。写入前，先看看是否需要擦除，如果需要擦除
//      则要看该块是否已经在busy[8]区，如是，还要用"释放-重新申请-擦写"的过程进
//      行块替换。待写入的数据是块缓冲区从offset开始的size个字节。
//参数: PTT_flash_tag，被操作的flash文件柜的存储媒体标签
//      block_no，目标块号
//      offset，起始地址在块内的偏移量
//      size，写入缓冲区长度
//返回: 写入成功则返回实际写入的块号，
//      写入失败返回cn_limit_uint32，空间不足或者连续遇到10个坏块会导致失败
//-----------------------------------------------------------------------------
u32 __Easynor_WriteUpdateBlock(struct tagEasynorFlashChip *chip, u32 block_no,
        u32 offset, u8 *buf, u32 size)
{
    bool_t block_ready;
    block_ready = chip->query_block_ready(block_no, offset, buf, size);
    if (!block_ready)
    {
        if (offset != 0)
            //把目标块的数据读到块缓冲区中，先读offset以前的部分
            chip->read_data(block_no, 0, chip->block_buf, offset);
        if (offset + size != chip->block_size)
            //再读有效部分以后的部分
            chip->read_data(block_no, offset + size,
                    chip->block_buf + offset + size,
                    chip->block_size - offset - size);
        memcpy(chip->block_buf + offset, buf, size);
        chip->erase_block(block_no); //擦除块
        //写入目标块，整块写入
        return chip->write_data(block_no, 0, chip->block_buf, chip->block_size);
    }
    else
    {
        return chip->write_data(block_no, offset, buf, size);
    }
}

//----改写一块:添加数据--------------------------------------------------------
//功能: 把芯片块缓冲区的内容写入flash中某块，被写入的位置原来没有有效数据，如果
//      在写入时遇到坏块，申请一个新块替换之，并把坏块登记到MAT表中(并不把MAT写
//      入到flash中)，返回替换后的块号。写入前，先看看是否需要擦除，如果需要擦除
//      则要看该块是否已经在busy[8]区，如是，还要用"释放-重新申请-擦写"的过程进
//      行块替换。待写入的数据是块缓冲区从offset开始的size个字节。
//参数: PTT_flash_tag，被操作的flash文件柜的存储媒体标签
//      block_no，目标块号
//      offset，起始地址在块内的偏移量
//      size，写入缓冲区长度
//返回: 写入成功则返回实际写入的块号，
//      写入失败返回cn_limit_uint32，空间不足或者连续遇到10个坏块会导致失败
//-----------------------------------------------------------------------------
u32 __Easynor_WriteAppendBlock(struct tagEasynorFlashChip *chip, u32 block_no,
                                 u32 offset, u8 *buf, u32 size)
{
    bool_t block_ready;
    block_ready = chip->query_block_ready(block_no, offset, buf, size);
    if (!block_ready)
    {
        if (offset != 0)
            //把目标块的数据读到块缓冲区中，先读offset以前的部分
            chip->read_data(block_no, 0, chip->block_buf, offset);
        memcpy(chip->block_buf + offset, buf, size);
        chip->erase_block(block_no); //擦除块
        //写入目标块，整块写入
        return chip->write_data(block_no, 0, chip->block_buf, chip->block_size);
    }
    else
    {
        return chip->write_data(block_no, offset, buf, size);
    }
}

//----把数据写入新块-----------------------------------------------------------
//功能: 把芯片的块缓冲区的内容写入到一个尚无有效数据的新块中，因写入前块内并无
//      有效数据，故无需动用掉电恢复块，如果在写入时遇到坏块，申请一个新块替换
//      之，并把坏块登记到MAT表中(并不把MAT写入到flash中)，返回替换后的块号。
//参数: PTT_flash_tag，被操作的flash文件柜的存储媒体标签
//      block_no，目标块号
//      offset，起始地址在块内的偏移量
//      size，写入缓冲区长度
//返回: 写入成功则返回实际写入的块号，
//      写入失败返回cn_limit_uint32，空间不足或者连续遇到10个坏块会导致失败
//-----------------------------------------------------------------------------
u32 __Easynor_WriteNewBlock(struct tagEasynorFlashChip *chip, u32 block_no,
                              u32 offset, u8 *buf, u32 size)
{
    bool_t block_ready;
    block_ready = chip->query_block_ready(block_no, offset, buf, size);
    if (!block_ready)
    {
        chip->erase_block(block_no); //擦除块
        //写入目标块，整块写入
        return chip->write_data(block_no, offset, buf, size);
    }
    else
    {
        return chip->write_data(block_no, offset, buf, size);
    }
}
//----检查文件分配表filesize-----------------------------------------------------------
//功能: 检查一个文件分配表的filesize，只检查，不作任何改写
//参数: buf，缓冲区
//      location，filesize数据的位置，如果filesize出错，则是出错的位置
//返回: 0 = 无错，1=有错没有改正
//-----------------------------------------------------------------------------
u32 __Easynor_CheckFileSize(u8 *buf, u32 *location)
{
    u32 filesize_no, offset;
    u32 ret = 0;

    for (filesize_no = 0; filesize_no < CN_EASYNOR_FILESIZE_NUM;
            filesize_no++)
    {
        offset = filesize_no * 8;
        if(pick_little_64bit(buf + offset,0) == CN_LIMIT_UINT64)//为全1，则该条无错
            break;
        else if(pick_little_64bit(buf + offset,0) == 0)//表示被改写过的大小，为全0,直接跳过
            continue;
        else if(pick_little_32bit(buf + offset,0) !=
                ~(pick_little_32bit(buf + offset + 4,0)))//正反码有错
        {
            ret = 1;
            break;
        }
    }
    *location = filesize_no;
    return ret;
}
//----检查扇区缓冲区-----------------------------------------------------------
//功能: 检查一个文件分配表的扇区缓冲区，有错误则改正之,但不写入芯片。若无错或有
//      错但能改正，同时把该缓冲区包含的文件数增加到chip->files_sum。
//参数: buf，缓冲区
//      index，第几个扇区
//      chip，所属芯片
//返回: 0 = 无错，1=有错并改正，2=有错不能改正
//-----------------------------------------------------------------------------
u32 __Easynor_CheckSingleBuf(u8 *buf, u32 index, struct tagEasynorFlashChip *chip)
{
    u32 offset, location, item_no,item_num;
    u32 temp_files_sum = 0;
    u32 ret = 0;

    item_num = (CN_EASYNOR_CFG_LIMIT > chip->block_size)?
            chip->block_size:CN_EASYNOR_CFG_LIMIT;
    item_num = item_num / CN_EASYNOR_CFG_ITEM_LIMIT;
    if(index == chip->start_block)
        item_no = 1;
    else
        item_no = 0;
    //一个块内有多个信息条，从第二 条开始检查,
    for (;item_no < item_num;item_no++)
    {
        offset = item_no * CN_EASYNOR_CFG_ITEM_LIMIT;
        if (buf[offset] != 0xFF) //name为非空，表示该条有用
        {
            temp_files_sum++; //该条用到，则文件数加1
            offset += CN_EASYNOR_FILESIZE_OFF;
            if(__Easynor_CheckFileSize(buf + offset,&location)) //有错，则将filesize改写成0
            {
            	ret = 1;
            	memset(buf+offset+8*location,0,8);
            }
        }
    }
    //把本块内所有文件数放到chip->files_sum
    chip->files_sum += temp_files_sum;
    return ret;
}

//----检查扇区-----------------------------------------------------------
//功能: 检查一对(主+备份)文件分配表的扇区缓冲区，有错误则改正之,该数据已经在
//      flash中，改正后写进去。同时把该缓冲区包含的文件数增加到chip->files_sum。
//参数: buf，缓冲区
//      index，第几个扇区
//      chip，所属芯片
//返回: 无
//-----------------------------------------------------------------------------
void __Easynor_CheckBlock(u8 *buf_main,u32 index_main, u8 *buf_bak, u32 index_bak,
                   struct tagEasynorFlashChip *chip)
{
    u32 item_no,offset,item_num;
    u32 reflash = 0, location;
    u32 temp_files_sum = 0;
    u8 * correct_filesize;

    item_num = (CN_EASYNOR_CFG_LIMIT > chip->block_size)?
            chip->block_size:CN_EASYNOR_CFG_LIMIT;
    item_num = item_num / CN_EASYNOR_CFG_ITEM_LIMIT;
    if(index_main == chip->start_block)
        item_no = 1;
    else
        item_no = 0;
    //从主备文件分配表的第二个表开始检查
    for (;item_no < item_num;item_no++)
    {
        offset = item_no * CN_EASYNOR_CFG_ITEM_LIMIT;
        if ((buf_main[offset] != 0xFF) || (buf_bak[offset] != 0xFF)) //从名称判断是否有用到该文件配置
        {
            temp_files_sum++;
            //检查主备分配表name,startblock,maxsize,crc
            if(memcmp(buf_main+offset,buf_bak+offset,CN_EASYNOR_FILESIZE_OFF))//若不相同
            {
                //主分配表错误,则发生擦除或者写入错误，需要要本条item全部复制
                if(memcmp(buf_main + CN_EASYNOR_VERIFITY_OFF, "easyfile", 8))
                {
                    memcpy(buf_main+offset,buf_bak+offset,CN_EASYNOR_CFG_ITEM_LIMIT);
                }
                else    //备用分配表错误
                {
                    memcpy(buf_bak+offset,buf_main+offset,CN_EASYNOR_CFG_ITEM_LIMIT);
                }
                reflash = 1;
                continue;
            }

            //核对主备文件分配表的filesize
            offset += CN_EASYNOR_FILESIZE_OFF;
            if(memcmp(buf_main+offset,buf_bak+offset,
                    CN_EASYNOR_VERIFITY_OFF - CN_EASYNOR_FILESIZE_OFF))//主备分配表filesize大小不同
            {
                if (__Easynor_CheckFileSize(buf_main + offset, &location)) //主分配表有错
                {
                    correct_filesize = buf_bak+offset+8*location;
                    if(pick_little_64bit(correct_filesize,0) == CN_LIMIT_UINT64)//主分配表filesize错误，但是bak还没写
                    {
                        memset(correct_filesize, 0, 8);//与主分配表保持一致
                        memset(buf_main + offset + 8 * location, 0, 8);
                        chip->write_data(index_main,offset+8 * location,buf_main+offset+8*location,8);
                        chip->write_data(index_bak, offset+8 * location,buf_bak+offset+8*location,8);
                    }
                    else if(location == CN_EASYNOR_FILESIZE_NUM)//错误的是最后一条filesize
                    {
                        reflash = 1;
                        memset(buf_main+offset,0xff,CN_EASYNOR_VERIFITY_OFF-CN_EASYNOR_FILESIZE_OFF);
                        memcpy(buf_main+offset,correct_filesize,8);//保存正确filesize到第一条位置
                        memset(buf_bak+offset,0xff,CN_EASYNOR_VERIFITY_OFF-CN_EASYNOR_FILESIZE_OFF);
                        memcpy(buf_bak+offset,buf_main+offset,8);//保存正确filesize到第一条位置
                    }
                    else////主分配表filesize错误，但是bak已写，即bak文件大小正确
                    {
                        //将写为0的主filesize写到备份中相对应的位置
                        memset(buf_main + offset + 8 * location, 0, 8);//将主分配表中filesize错误条写成0
                        memcpy(buf_main + offset + 8 * location + 8,correct_filesize,8);//从备份表中拷贝正确的filesize到下一条
                        memcpy(correct_filesize + 8,correct_filesize,8);
                        memset(correct_filesize, 0, 8);//与主分配表保持一致

                        //将更新的filesize写到芯片中
                        chip->write_data(index_main,offset+8 * location,buf_main+offset+8*location,16);
                        chip->write_data(index_bak, offset+8 * location,buf_bak+offset+8*location,16);
                    }
                }
                else//主分配表正确
                {
                    correct_filesize = buf_main+offset+8*location;
                    if(pick_little_64bit(correct_filesize,0) == CN_LIMIT_UINT64)//主分配表filesize错误，但是bak还没写
                    {
                        memset(correct_filesize, 0, 8);//与主分配表保持一致
                        memset(buf_bak + offset + 8 * location, 0, 8);
                        chip->write_data(index_main,offset+8 * location,buf_main+offset+8*location,8);
                        chip->write_data(index_bak, offset+8 * location,buf_bak+offset+8*location,8);
                    }
                    else if(location == CN_EASYNOR_FILESIZE_NUM)//错误的是最后一条filesize
                    {
                        reflash = 1;
                        memset(buf_bak+offset,0xff,CN_EASYNOR_VERIFITY_OFF-CN_EASYNOR_FILESIZE_OFF);
                        memcpy(buf_bak+offset,correct_filesize,8);//保存正确filesize到第一条位置
                        memset(buf_main+offset,0xff,CN_EASYNOR_VERIFITY_OFF-CN_EASYNOR_FILESIZE_OFF);
                        memcpy(buf_main+offset,buf_bak+offset,8);//保存正确filesize到第一条位置
                    }
                    else
                    {
                        //将bak该filesize写成0
                        memset(buf_bak + offset + 8 * location, 0, 8);
                        memcpy(correct_filesize + 8,correct_filesize, 8); //下移并拷贝
                        memcpy(buf_bak + offset + 8 * location + 8,correct_filesize, 8); //拷贝主文件分配表的filesize
                        memset(correct_filesize, 0, 8); //与备用分配表相持一致

                        //将更新的filesize写到芯片中
                        chip->write_data(index_main,offset+8 * location,buf_main+offset+8*location,16);
                        chip->write_data(index_bak, offset+8 * location,buf_bak+offset+8*location,16);
                    }
                }
            }
        }
    }

    chip->files_sum += temp_files_sum;
    if (reflash) //分配表信息错或filesize满，则写入flash
    {
        chip->erase_block(index_main);
        chip->erase_block(index_bak);
        chip->write_data(index_main, 0, buf_main, chip->block_size);
        chip->write_data(index_bak, 0, buf_bak, chip->block_size);
    }
}

//----检查norflash芯片---------------------------------------------------------
//功能: 检查chip格式是否正确，如果不正确，恢复之。
//      不正确的原因: 1、未建立文件系统，同时建立文件系统。
//      2、操作中掉电了，用备用数据恢复之。
//      判掉电的算法:
//          1、每个sector最后的CRC校验存在且与计算的校验值相等，则表明该sector除
//          文件尺寸外的其他数据是正确的 。若不正确，则从备用中copy。若备用的不正
//          确，则备份之。
//          2、filesize使用正反码循环存储，找到最后一个校验正确的即可。如果有校
//          验不正确的存在，把该条信息清零即可。
//          3、"easyfile"存在的情况，只会有一份配置信息可能出错。
//参数: chip，待检查的flash芯片。
//返回: true = 成功，false = 失败
//-----------------------------------------------------------------------------
bool_t __Easynor_CheckNorChip(struct tagEasynorFlashChip *chip)
{
    u32 loop, blocks, end_block, blocksize;
    u8 *bakbuf, *block_buf;
    u8 mainblockerr = 0,bakblockerr = 0;
    u16 crc16_check,temp;
    bakbuf = M_MallocLc(chip->block_size, 0);
    if (bakbuf == NULL)
        return false;
    blocks = (CN_EASYNOR_CFG_LIMIT + chip->block_size-1) / chip->block_size;
    end_block = chip->start_block + blocks;
    blocksize = chip->block_size;
    block_buf = chip->block_buf;

    //检查文件系统是否已经建立
    chip->read_data(chip->start_block, 0, block_buf, CN_EASYNOR_CFG_ITEM_LIMIT);
    chip->read_data(end_block, 0, bakbuf, CN_EASYNOR_CFG_ITEM_LIMIT);

    //主备区都被破坏，则需要重新建立文件系统
    if ((memcmp(bakbuf, "easyfile", 8)||
          memcmp(bakbuf + CN_EASYNOR_VERIFITY_OFF, "easyfile", 8)) &&
          (memcmp(block_buf, "easyfile", 8)||
                    memcmp(block_buf + CN_EASYNOR_VERIFITY_OFF, "easyfile", 8)) )
    {
        printf("文件系统未建立，建立文件系统---\r\n");
        //文件系统未建立，建立之
        memset(bakbuf, 0xff, blocksize);
        memcpy(bakbuf, "easyfile", 8);
        memcpy(bakbuf + CN_EASYNOR_VERIFITY_OFF, "easyfile", 8);
        fill_little_32bit(bakbuf, 2, chip->block_sum);
        fill_little_32bit(bakbuf, 3, chip->block_size);

        crc16_check = Crc_16(bakbuf,16);
//        fill_little_16bit(bakbuf,(blocksize/2)-1,crc16_check);//将CRC16填充在该block最后两字节中
        fill_little_16bit((u8*)&temp,0,crc16_check);    //转化为小端
        for (loop = chip->start_block; loop < end_block; loop++)
        {
            chip->erase_block(loop);
            chip->erase_block(loop + blocks);
        }
        chip->write_data(chip->start_block, 0, bakbuf,blocksize);
        chip->write_data(end_block, 0, bakbuf, blocksize);
        //added,将最后的CRC写到文件系统信息的最后两个字节
        chip->write_data(chip->start_block + blocks - 1,blocksize-2, (u8*)&temp,2);
        chip->write_data(end_block + blocks - 1,blocksize-2, (u8*)&temp,2);
        free(bakbuf);
        return true;
    }

    //检查文件系统信息区最后两字节CRC是否正确
    chip->read_data(chip->start_block + blocks - 1, blocksize-2, block_buf+blocksize-2, 2);
    chip->read_data(end_block + blocks - 1, blocksize-2, bakbuf+blocksize-2, 2);

    crc16_check = pick_little_16bit(block_buf,(blocksize/2)-1);
    if(crc16_check != Crc_16(block_buf,16))
        mainblockerr = 1;

    crc16_check = pick_little_16bit(bakbuf,(blocksize/2)-1);
    if(crc16_check != Crc_16(bakbuf,16)) //bak校验错
        bakblockerr = 1;

    //以下检查数据正确性
    for (loop = chip->start_block; loop < end_block; loop++)
    {
        chip->read_data(loop, 0, block_buf, blocksize);
        chip->read_data(loop + blocks, 0, bakbuf, blocksize);

        if ((block_buf[0] != 0xFF) || (bakbuf[0] != 0xFF)) //表示本block已用
        {
//            crc16_check = pick_little_16bit(block_buf,(blocksize/2)-1);
//            if(crc16_check != Crc_16(block_buf,16)) //主校验错,发生CRC错误主要是擦除出错
            if(mainblockerr == 1)
            {
//                if(crc16_check != Crc_16(bakbuf,16)) //bak校验错
                if(bakblockerr == 1)
                {
                    //两个缓冲区校验均出错，表明文件系统已经损坏，报错
                    free(bakbuf);
                    return false;
                }
                else //bak校验正确
                {
                    temp = __Easynor_CheckSingleBuf(bakbuf, loop, chip);
                    if (temp == 0)
                    {
                        chip->erase_block(loop);
                        chip->write_data(loop, 0, bakbuf, blocksize);
                    }
                    else if (temp == 1)
                    {
                        chip->erase_block(loop);
                        chip->write_data(loop, 0, bakbuf, blocksize);
                        chip->erase_block(loop + blocks);
                        chip->write_data(loop + blocks, 0, bakbuf, blocksize);
                    }
                    else
                    {
                        free(bakbuf);
                        return false;
                    }
                }
            }
            else //主校验正确
            {
//                crc16_check = pick_little_16bit(bakbuf,(blocksize/2)-1);
//                if(crc16_check != Crc_16(bakbuf,16)) //bak校验错
                if(bakblockerr == 1)
                {
                    temp = __Easynor_CheckSingleBuf(block_buf, loop,chip);
                    if (temp == 0)
                    {
                        chip->erase_block(loop + blocks);
                        chip->write_data(loop + blocks, 0, block_buf,blocksize);
                    }
                    else if (temp == 1)
                    {
                        chip->erase_block(loop);
                        chip->write_data(loop, 0, block_buf, blocksize);
                        chip->erase_block(loop + blocks);
                        chip->write_data(loop + blocks, 0, block_buf,blocksize);
                    }
                    else
                    {
                        free(bakbuf);
                        return false;
                    }
                }
                else //bak校验也正确
                {
                    __Easynor_CheckBlock(block_buf,loop, bakbuf, loop+blocks, chip);
                }
            }
        }
        else
        {
            break; //该块未使用
        }
    }
    free(bakbuf);
    return true;
}

//----DFFSD模块初始化----------------------------------------------------------
//功能: 就是在系统资源链表中添加flash芯片根结点
//参数: 无
//返回: 1
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_Easynor(ptu32_t para)
{
    g_ptEasyNorSemp = Lock_SempCreate(1,1,CN_SEMP_BLOCK_FIFO,"flash_chip tree");
    if(g_ptEasyNorSemp == NULL)
        return 0;  //flash_chip树信号量申请失败
    s_ptEasyNorInfo = M_Malloc(gc_u32CfgOpenedfileLimit * sizeof(struct tagEasyFlashFileInfo),0);
    if (s_ptEasyNorInfo == NULL)
        Lock_SempDelete(g_ptEasyNorSemp);
    //在资源链表中建立一个根结点，系统中所有芯片都挂在这个结点下
    Rsc_AddTree(&tg_flash_chip_root_rsc,sizeof(struct  tagRscNode),RSC_RSCNODE,
                        "flash chip");
    return 1;
}

//----填充PTT_device_tag-------------------------------------------------------
//功能: 填充PTT_device_tag
//参数：PTT_device_tag，被操作的文件柜指针，通用结构，非flash专用。
//      name，文件柜名
//返回: 0=成功，非0=失败
//-----------------------------------------------------------------------------
u32 Easynor_InstallChip(struct tagEasynorFlashChip *chip)
{
//    tagDevHandle fs_handle_right = NULL;
    static struct tagPTTDevice PTT_device_tag;

    if(chip == NULL)
        return -1;
//    if ((fs_handle_right = Driver_DevOpenRight("fs", 0)) == NULL)
//        return -1;

    PTT_device_tag.chip_mutex_tag = Lock_MutexCreate(chip->PTT_name);
    if(PTT_device_tag.chip_mutex_tag == NULL)
        return false;

    PTT_device_tag.format = Easynor_Format_PTT;
    PTT_device_tag.write = Easynor_WriteFile;
    PTT_device_tag.read = Easynor_ReadFile;
    PTT_device_tag.flush = Easynor_FlushFile;
    PTT_device_tag.query_file_stocks = Easynor_QueryFileStocks;
    PTT_device_tag.query_file_cubage = Easynor_QueryFileCubage;
    PTT_device_tag.check_PTT = Easynor_CheckPTT;
    PTT_device_tag.set_file_size = Easynor_SetFileSize;
    PTT_device_tag.seek_file = Easynor_SeekFile;
    PTT_device_tag.create_item = Easynor_CreateItem;
//    PTT_device_tag->move_file= Easynor_MoveFile;
    PTT_device_tag.remove_item = Easynor_RemoveFile;
    PTT_device_tag.open_item = Easynor_OpenItem;
    PTT_device_tag.close_item = Easynor_CloseItem;
    PTT_device_tag.lookfor_item = Easynor_LookforItem;
    PTT_device_tag.rename_item = Easynor_RenameItem;
    PTT_device_tag.item_traversal_son = Easynor_ItemTraversalSon;
    PTT_device_tag.check_fdt_size = Easynor_CheckFDT_Size;
    PTT_device_tag.read_fdt = NULL;
    PTT_device_tag.check_folder = Easynor_CheckFolder;
    PTT_device_tag.PTT_medium_tag = (ptu32_t) chip;
    PTT_device_tag.opened_sum = 0;
    strncpy(PTT_device_tag.name, chip->PTT_name, CN_EASYNOR_PTT_NAME_LIMIT);

    if(__Easynor_CheckNorChip(chip) == false)
    {
        printf("磁盘装入失败！\r\n");
        return -1;
    }
    PTT_device_tag.formatted = true;

    Lock_SempPend(g_ptEasyNorSemp,CN_TIMEOUT_FOREVER);
    //把新flash芯片加入到flash芯片根资源结点下，排在队列尾
    Rsc_AddSon(&tg_flash_chip_root_rsc,&chip->chip_node,
                        sizeof(struct tagEasynorFlashChip),RSC_EFS_NOR_CHIP,chip->PTT_name);
    Lock_SempPost(g_ptEasyNorSemp);

    Djyfs_FsDriverCtrl(enum_fs_add_PTT,(ptu32_t)&PTT_device_tag,0);
    return 0;
}

//----格式化文件柜------------------------------------------------------------
//功能：格式化文件柜，该文件柜应该已经建立，并且经过启动初始化加入到设备链表中。
//      A、未格式化的文件柜格式化过程:
//      1、计算存储DDR表所需的块数(2份)
//      2、扫描一遍坏块，在MAT表中标记坏块，并从好块中挑出2份DDR，
//      3、填写DDR中的DBL表、MAT表
//      4、ART表排序，并把DDR表写入flash中。
//      5、分配一块做FDT。
//      6、初始化FDT，然后写入flash中
//      7、更新MDR表，并写入flash中
//      B、已经格式化的文件柜格式化操作
//      再说吧
//参数：fmt_para1，格式化参数，1=使用ecc校验，0表示不用，一般专门存储多媒体文件
//              的文件柜可以不用
//      fmt_para2，格式化参数，暂不用
//      PTT_device_tag，待格式化的文件柜，该文件柜已经安装到设备系统中
//返回：true=成功格式化，含原来就已经格式化好而无需再次格式化的。
//      false=失败
//-----------------------------------------------------------------------------
bool_t Easynor_Format_PTT(u32 fmt_para1, u32 fmt_para2,
        struct tagPTTDevice *PTT_device_tag)
{
    struct tagEasynorFlashChip *chip;
    u32 loop, blocks, end_block;
    u16 crc16_check,little_crc16;

    if (PTT_device_tag == NULL)
        return false;
    if (PTT_device_tag->opened_sum != 0)
        return false;
    //文件所在flash芯片指针
    chip = (struct tagEasynorFlashChip *) PTT_device_tag->PTT_medium_tag;
    blocks = (CN_EASYNOR_CFG_LIMIT + chip->block_size - 1) / chip->block_size;
    end_block = chip->start_block + blocks;

    memset(chip->block_buf, 0xff, chip->block_size);
    memcpy(chip->block_buf, "easyfile", 8);
    memcpy(chip->block_buf + CN_EASYNOR_VERIFITY_OFF, "easyfile", 8);
    fill_little_32bit(chip->block_buf, 2, chip->block_sum);
    fill_little_32bit(chip->block_buf, 3, chip->block_size);

    //将CRC16填充在该block最后两字节中
    crc16_check = Crc_16(chip->block_buf,16);
//    fill_little_16bit(chip->block_buf,(chip->block_size/2)-1,crc16_check);
    fill_little_16bit((u8*)&little_crc16,0,crc16_check);    //转化为小端
    chip->files_sum = 0;
    PTT_device_tag->formatted = true;
    for (loop = chip->start_block; loop < end_block; loop++)
    {
        chip->erase_block(loop);
        chip->erase_block(loop + blocks);
    }
    chip->write_data(chip->start_block, 0, chip->block_buf,chip->block_size);
    chip->write_data(end_block, 0, chip->block_buf, chip->block_size);
    chip->write_data(chip->start_block + blocks - 1,chip->block_size-2, (u8*)&little_crc16,2);
    chip->write_data(end_block + blocks - 1,chip->block_size-2, (u8*)&little_crc16,2);
    return true;
}

//----修改文件尺寸-------------------------------------------------------------
//功能: 修改文件分配表中文件尺寸部分。1、在主分配表中找一个空闲位置写入。2、如果
//      找不到，则读出整块，把整个filesize区填充成ff，然后把文件尺寸更新到第一个
//      空位上，擦除块并写入之。3、在bak上做相同的操作。
//参数: fp，待操作的文件指针
//      newsize，新的文件尺寸
//返回: 无
//-----------------------------------------------------------------------------
void __Easynor_ChangeFileSize(struct tagFileRsc *fp, u32 newsize)
{
    struct tagEasyFlashFileInfo *fileinfo;
    struct tagPTTDevice *PTT_device_tag;
    struct tagEasynorFlashChip *chip;
    u8 *block_buf;
    u32 offset, blocksize, cfg_blocks, block_no, item_no,temp;
    u32 filesize_no,file_max_size;

    PTT_device_tag = (struct tagPTTDevice *) (fp->home_PTT);
    fileinfo = (struct tagEasyFlashFileInfo *) fp->file_medium_tag;
    chip = (struct tagEasynorFlashChip *) PTT_device_tag->PTT_medium_tag;

    block_buf = chip->block_buf;
    blocksize = chip->block_size;
    cfg_blocks = (CN_EASYNOR_CFG_LIMIT+ blocksize-1) / blocksize;

    //计算该文件分配表在所在的block
    block_no = (fileinfo->start_item * CN_EASYNOR_CFG_ITEM_LIMIT) / chip->block_size;
    block_no = block_no + chip->start_block;
    //该文件分配表的在其block中的偏移
    offset = (fileinfo->start_item * CN_EASYNOR_CFG_ITEM_LIMIT) % chip->block_size;
    //计算该文件在所属的block中是第几个文件item
    item_no = offset / CN_EASYNOR_CFG_ITEM_LIMIT;

    //读出该文件分配表
    chip->read_data(block_no, offset,block_buf,CN_EASYNOR_CFG_ITEM_LIMIT);

    //文件尺寸不能大于文件最大尺寸
    file_max_size = pick_little_32bit(block_buf,CN_EASYNOR_MAXSIZE_OFF >> 2);

    if(newsize > file_max_size)
        return;

    offset = CN_EASYNOR_FILESIZE_OFF;
    for (filesize_no = 0; filesize_no < CN_EASYNOR_FILESIZE_NUM;filesize_no++)
    {
        if (pick_little_64bit(block_buf + offset,0) == CN_LIMIT_UINT64) //在主分配表找到空闲位置
        {
            fill_little_32bit(block_buf, offset >> 2, newsize);
            fill_little_32bit(block_buf, (offset >> 2) + 1, ~newsize);
            temp = offset + item_no * CN_EASYNOR_CFG_ITEM_LIMIT;
            chip->write_data(block_no, temp, block_buf + offset, 8);
            chip->write_data(block_no + cfg_blocks, temp, block_buf + offset, 8);
            break;
        }
        offset += 8;
    }

    if (filesize_no == CN_EASYNOR_FILESIZE_NUM) //主分配表没有空余位置
    {
        //先读出整块数据到buf,并擦除
        chip->read_data(block_no, 0, block_buf,blocksize);

        chip->erase_block(block_no);
        chip->erase_block(block_no + cfg_blocks);

        offset = item_no * CN_EASYNOR_CFG_ITEM_LIMIT + CN_EASYNOR_FILESIZE_OFF;
        memset(block_buf + offset,0xFF,CN_EASYNOR_VERIFITY_OFF - CN_EASYNOR_FILESIZE_OFF);
        fill_little_32bit(block_buf, offset >> 2, newsize);
        fill_little_32bit(block_buf, (offset >> 2) + 1, ~newsize);

        chip->write_data(block_no, 0, block_buf, blocksize);
        chip->write_data(block_no + cfg_blocks, 0, block_buf, blocksize);
    }
}

//----写入flash文件------------------------------------------------------------
//功能：把缓冲区中的数据写入文件，执行下列操作：
//      1、如果写缓冲区有足够的空间，把数据写入写缓冲区即可。
//      2、如果写缓冲区没有足够的空间，连同写缓冲区原有数据和输入缓冲区中的数据
//         一并写入flash
//      3、写入完成后检查读缓冲区是否需要同步并执行之。
//      4、检查FDT表和DDR表是否需要修改并执行之
//参数：buf，输入缓冲区
//      len，本次写入的数据量
//      fp，被操作的文件指针
//返回：本次写入的数据量，不包含写缓冲区中原有的数据
//注:   本函数并没有先调用__easynor_write_buf_to_flash函数，再写buf中的内容，那样
//      可能需要多一次调用flash写操作，效率低；而是把写缓冲区的数据与buf中的数据
//      合并一次写入flash.
//-----------------------------------------------------------------------------
u32 Easynor_WriteFile(struct tagFileRsc *fp, uint8_t *buf, u32 len)
{
    struct tagEasyFlashFileInfo *fileinfo;
    struct tagPTTDevice *PTT_device_tag;
    struct tagEasynorFlashChip *chip;
    u32 start_block, offset_start, end_block, loop;
    u32 write_sum, wr_point, write_len, completed = 0;

    if ((fp == NULL) || (len == 0) || (buf == NULL))
        return 0;

    PTT_device_tag = (struct tagPTTDevice *) (fp->home_PTT);
    fileinfo = (struct tagEasyFlashFileInfo *) fp->file_medium_tag;
    //文件所在flash芯片指针
    chip = (struct tagEasynorFlashChip *) PTT_device_tag->PTT_medium_tag;

    //如果超出了最大尺寸，则超出部分会被舍弃
    if (len <= (fileinfo->max_size - fp->file_size))
        write_sum = len;
    else
        write_sum = fileinfo->max_size - fp->file_size;
    wr_point = fp->write_ptr;
    //计算起始写位置处于文件内第几块和偏移量
    start_block = (u32) (wr_point / chip->block_size)
                + fileinfo->start_block;
    offset_start = (u32) (wr_point % chip->block_size);
    //计算终止写位置处于文件内第几块
    end_block = (u32) ((fp->write_ptr + len - 1) / chip->block_size)
                + start_block;
    for (loop = start_block; loop <= end_block; loop++)
    {
        //计算本次写入的数据长度,如果达到block尾部，则需截尾
        if (write_sum - completed > chip->block_size - offset_start)
            write_len = chip->block_size - offset_start;
        else
            write_len = write_sum - completed;
        if (wr_point >= fp->file_size) //追加数据
        {
            if (offset_start == 0) //待写入的块内尚无有效数据
            {
                //写入目标块
                __Easynor_WriteNewBlock(chip, loop, offset_start,
                            &buf[completed], write_len);
            }
            else //虽是追加，但待写入的块已经有部分有效数据
            {
                //写入目标块
                __Easynor_WriteAppendBlock(chip, loop, offset_start,
                        &buf[completed], write_len);
            }
        }
        else //在文件的中间写入，若需擦除就需要使用PCRB块进行保护
        {
            //写入目标块
            __Easynor_WriteUpdateBlock(chip, loop, offset_start,
                        &buf[completed], write_len);
        }
        completed += write_len;
        offset_start = 0; //第二块开始，起始偏移地址肯定是0
    }

    fp->write_ptr += completed;
    fp->read_ptr += completed;
    if (fp->file_size < fp->write_ptr)
    {
        __Easynor_ChangeFileSize(fp, fp->write_ptr);
        fp->file_size = fp->write_ptr;
    }

    return completed;
}
//--------读flash文件------------------------------------------------------------
//功能；把文件中的数据读入到缓冲区中，执行以下操作
//      1、看读缓冲区中有没有数据，有则读入到接收缓冲区中
//      2、若读缓冲区中的数据量不足或没有数据，则从flash中补充读取，并按照不越块
//         边界为限最大限度地填写读缓冲区
//      3、读出完成后看是否需要同步写缓冲区并执行之
//参数：buf，输入缓冲区
//      len，读出的数据量
//      fp，被操作的文件指针
//返回：实际读取的字节数
//-----------------------------------------------------------------------------
u32 Easynor_ReadFile(struct tagFileRsc *fp, uint8_t *buf, u32 len)
{
    struct tagEasyFlashFileInfo *fileinfo;
    struct tagPTTDevice *PTT_device_tag;
    struct tagEasynorFlashChip *chip;
    u32 completed = 0, read_len, result_len;
    u32 offset_start;
    u32 start_block, end_block, loop;

    if ((fp == NULL) || (len == 0) || (buf == NULL))
        return 0;
    PTT_device_tag = (struct tagPTTDevice *) (fp->home_PTT);
    fileinfo = (struct tagEasyFlashFileInfo *) fp->file_medium_tag;
    if (fp->read_ptr + len > fp->file_size)
        result_len = (u32) (fp->file_size - fp->read_ptr);
    else
        result_len = len;
    //文件所在flash芯片指针
    chip = (struct tagEasynorFlashChip *) PTT_device_tag->PTT_medium_tag;
    //计算起始读写位置处于文件内第几块
    start_block = (u32) (fp->read_ptr / chip->block_size)
            + fileinfo->start_block;
    offset_start = fp->read_ptr % chip->block_size;
    //计算终止读写位置处于文件内第几块
    end_block = (u32) ((fp->read_ptr + result_len - 1) / chip->block_size)
            + fileinfo->start_block;
    for (loop = start_block; loop <= end_block; loop++)
    {
        if (result_len - completed > chip->block_size - offset_start)
            read_len = chip->block_size - offset_start;
        else
            read_len = result_len - completed;
        //读出目标块
        chip->read_data(loop, offset_start, buf + completed, read_len);
        completed += read_len;
        offset_start = 0; //第二块开始，起始偏移地址肯定是0
    }
    fp->read_ptr += completed;
    return completed;
}

//----刷写缓冲区---------------------------------------------------------------
//功能: 把文件写缓冲区的数据写入flash,easynor每次文件操作均直接写入，无需flush。
//参数：fp，被操作的文件指针
//返回：实际写入flash的字节数
//-----------------------------------------------------------------------------
u32 Easynor_FlushFile(struct tagFileRsc *fp)
{
    return 0;
}

//----查询文件可读数据量-------------------------------------------------------
//功能：查询一个文件内有多少可读数据，对于固态文件（如flash文件）来说，就是文件
//      长度-当前指针，但流的标准定义并非一定如此，比如通信端子。而且，文件读写
//      指针如何表示与具体文件系统驱动的实现有关，文件系统的核心程序不应该做任何
//      假设。
//参数：fp，被操作的文件指针
//返回：文件可读数据的字节数
//-----------------------------------------------------------------------------
sint64_t Easynor_QueryFileStocks(struct tagFileRsc *fp)
{
    if (fp == NULL)
        return 0;
    return fp->file_size - fp->read_ptr;
}

//----查询文件可写数据量-------------------------------------------------------
//功能：查询一个文件还能写入多少数据，对于固态文件（如flash文件），可能就是文件
//      柜的剩余空间，但流的标准定义并非一定如此，比如通信端子。而且，文件柜剩余
//      空间如何表示与具体文件系统驱动的实现有关，文件系统的核心程序不应该做任何
//      假设。不能认为PTT_flash_tag->free_sum就是文件仍可写入的数据量。
//      flash文件系统的计算方式是:
//      1、文件已分配的块的容量-文件尺寸。
//      2、加上文件柜剩余空间PTT_flash_tag->free_sum。
//      3、减去文件缓冲区中已有的数据量
//参数：fp，被操作的文件指针
//返回：文件还能写入的字节数
//-----------------------------------------------------------------------------
sint64_t Easynor_QueryFileCubage(struct tagFileRsc *fp)
{
    struct tagEasyFlashFileInfo *fileinfo;
//    struct tagPTTDevice *PTT_device_tag;
    if (fp == NULL)
        return 0;
//    PTT_device_tag = (struct tagPTTDevice *) (fp->home_PTT);
    fileinfo = (struct tagEasyFlashFileInfo *) fp->file_medium_tag;
    return fileinfo->max_size - fp->write_ptr;
}

//----设置文件指针-------------------------------------------------------------
//功能：把文件指针设置到指定的位置，并相应地同步读写缓冲区。如果新位置超出文件长
//      度，则导致延长文件，但只有max_size=0的文件能被延长，只能延长一次
//参数：fp，被操作的文件指针
//      offset，移动的位置
//      whence，从何处开始计算移动长度，参见SEEK_CUR等的定义
//返回：0=成功，1=失败
//-----------------------------------------------------------------------------
u32 Easynor_SeekFile(struct tagFileRsc *fp, struct tagSeekPara *position)
{
    sint64_t new_position;
    uint8_t whence;
    if ((position == NULL) || (fp == NULL))
        return 1;
    whence = position->whence;
    if (whence == SEEK_SET)
        new_position = position->offset;
    else if (whence == SEEK_CUR)
        new_position = position->offset + fp->read_ptr;
    else if (whence == SEEK_END)
        new_position = fp->file_size + position->offset;
    else
        return 1;
    //简易文件系统不允许做文件延长的工作
    if (new_position > fp->file_size)
//        Easynor_SetFileSize(fp, new_position);
        return 1;
    fp->read_ptr = new_position;
    fp->write_ptr = new_position;
    return 0;
}

//----建立文件-----------------------------------------------------------------
//功能：因创建文件时需要制定max_size参数，标准的djyfs_fopen调用无法实现，故提供
//      单独的api:easynor_create_file用于创建文件，不允许创建目录。
//参数：name，目录（文件）名
//      parent，父目录指针。
//      attr，新文件（目录）的属性
//返回：true=成功，false=失败，
//-----------------------------------------------------------------------------
bool_t Easynor_CreateItem(char *name, struct tagFileRsc *parent,
                           struct tagFileRsc *result, union file_attrs attr,
                           enum _FILE_OPEN_MODE_ mode)
{
    struct tagEasyFlashFileInfo *fileinfo;
    struct tagEasynorFlashChip *chip;
    struct tagPTTDevice *PTT_device_tag;

    u32 block_no,start_block;
    u32 size,offset;
    u8 cfg_blocks;

    if ((parent == NULL) || (result == NULL))
        return 0;
    PTT_device_tag = (struct tagPTTDevice *) (parent->home_PTT);
    chip = (struct tagEasynorFlashChip *)PTT_device_tag->PTT_medium_tag;

    if (chip->files_sum >= CN_EASYNOR_CFG_LIMIT / CN_EASYNOR_CFG_ITEM_LIMIT - 1)
        return false; //已达可创建文件的上限

    offset = (chip->files_sum * CN_EASYNOR_CFG_ITEM_LIMIT) % chip->block_size;
    block_no = (chip->files_sum * CN_EASYNOR_CFG_ITEM_LIMIT) / chip->block_size;
    block_no = chip->start_block + block_no;
    //计算文件系统文件分配表主分配所使用的blocks
    cfg_blocks = (CN_EASYNOR_CFG_LIMIT + chip->block_size-1) / chip->block_size;

    fileinfo = &s_ptEasyNorInfo[PTT_device_tag->opened_sum];
    //当文件系统刚刚建立，还不曾有创建文件时，作如下处理
    if(chip->files_sum < 1)
    {
        //第一个文件存放的地址是在该分区表后面的第一个block，偏移量为0
        start_block = chip->start_block + 2*cfg_blocks;
        if(start_block >= (chip->start_block + chip->block_sum))
            return false;
        offset = CN_EASYNOR_CFG_ITEM_LIMIT;
    }
    else
    {
        //以下计算现有的最后一个文件在文件分配表中的位置(块号和偏移)。注意，文件分配
        //表第0个被系统占用
        //读出文件配置，取该文件的最大长度和初始块号，用于计算新文件的初始块号。
        chip->read_data(block_no, offset, chip->block_buf,CN_EASYNOR_CFG_ITEM_LIMIT);
        start_block = pick_little_32bit(chip->block_buf,
                                        CN_EASYNOR_STARTBLOCK_OFF >> 2);
        size = pick_little_32bit(chip->block_buf, CN_EASYNOR_MAXSIZE_OFF >> 2);

        //上一个文件尺寸最大尺寸没有配置
        if(size == 0xFFFFFFFF)
            return false;
        //新文件的初始块号,easyfile中每个文件占用一个block
        start_block += (size + chip->block_size - 1) / chip->block_size;

        //以下计算新文件在文件分配表中的位置(块号和偏移)
        offset += CN_EASYNOR_CFG_ITEM_LIMIT;
        if (offset >= chip->block_size)
        {
            offset = 0;
            block_no++;
        }
    }
    //以下初始化文件分配表并写入flash
    memset((char *)chip->block_buf, 0xff, CN_EASYNOR_CFG_ITEM_LIMIT);
    strncpy((char *)chip->block_buf, name, CN_EASYNOR_PTT_NAME_LIMIT);
    fill_little_32bit(chip->block_buf, CN_EASYNOR_STARTBLOCK_OFF >> 2,
            start_block);
    fill_little_32bit(chip->block_buf, CN_EASYNOR_MAXSIZE_OFF >> 2, 0xFFFFFFFF);
    fill_little_32bit(chip->block_buf, CN_EASYNOR_FILESIZE_OFF >> 2, 0);
    fill_little_32bit(chip->block_buf, (CN_EASYNOR_FILESIZE_OFF >> 2) + 1, ~0);
    memcpy(chip->block_buf + CN_EASYNOR_VERIFITY_OFF, "easyfile", 8);
    chip->write_data(block_no, offset, chip->block_buf,CN_EASYNOR_CFG_ITEM_LIMIT);
    chip->write_data(block_no+cfg_blocks,offset,chip->block_buf,CN_EASYNOR_CFG_ITEM_LIMIT);

    chip->files_sum++;
    //更新文件信息，创建即打开
    result->file_medium_tag = (ptu32_t)fileinfo;
    result->file_size = 0;
    strncpy(result->name, name, CN_EASYNOR_FILENAME_LIMIT);
    result->read_ptr = 0;
    result->write_ptr = 0;
    result->file_attr = attr;

    //更新文件信息，创建即打开
    fileinfo->filesize = 0;
    fileinfo->max_size = 0;
    fileinfo->start_block = start_block;
    fileinfo->start_item = chip->files_sum;

    return true;
}

//----移动项目（文件/目录）----------------------------------------------------
//功能：固定只有两个文件，不支持移动
//参数：name，目录（文件）名
//      parent，父目录指针。
//      attr，新文件（目录）的属性
//返回：true=成功，false=失败，一般是因为文件柜没有足够的容量导致
//-----------------------------------------------------------------------------
bool_t Easynor_MoveFile(struct tagFileRsc *src_fp, struct tagFileRsc *dest_fp)
{
    return false;
}

//----删除项目（文件/目录）---------------------------------------------------
//功能：删除简易文件系统中文件列表的最后一个文件，删除后该文件分配表状态与未建立
//      时一致，即flash中该地址段为全0xFF
//参数：fp，被删除的目录（文件）指针
//返回：true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t Easynor_RemoveFile(struct tagFileRsc *fp)
{
    struct tagEasyFlashFileInfo *fileinfo;
    struct tagEasynorFlashChip *chip;
    struct tagPTTDevice *PTT_device_tag;

    u32 block_no,offset;
    u8 cfg_blocks;

    if (fp == NULL)
        return 0;
    PTT_device_tag = (struct tagPTTDevice *) (fp->home_PTT);
    chip = (struct tagEasynorFlashChip *)PTT_device_tag->PTT_medium_tag;
    fileinfo = (struct tagEasyFlashFileInfo *) fp->file_medium_tag;

    //判断是否为最后一个文件,easynor file sys只允许删除最后一个文件
    if(fileinfo->start_item != chip->files_sum)
    {
        printf("文件并非文件列表的最后一个文件，请重新输入\r\n");
        return false;
    }

    //若为最后一个文件，则可删除
    chip->files_sum --;

    cfg_blocks = (CN_EASYNOR_CFG_LIMIT+ chip->block_size-1) / chip->block_size;
    //计算该文件分配表在所在的block
    block_no = (fileinfo->start_item * CN_EASYNOR_CFG_ITEM_LIMIT) / chip->block_size;
    block_no = block_no + chip->start_block;

    //该文件分配表的在其block中的偏移
    offset = (fileinfo->start_item * CN_EASYNOR_CFG_ITEM_LIMIT) % chip->block_size;

    //先将该block中的数据读出来，并将该文件分配表区域写0xFF,
    chip->read_data(block_no, 0, chip->block_buf, chip->block_size);
    memset(chip->block_buf + offset,0xFF,CN_EASYNOR_CFG_ITEM_LIMIT);

    //擦除主备块并写入
    chip->erase_block(block_no);
    chip->write_data(block_no, 0, chip->block_buf, chip->block_size);
    chip->erase_block(block_no + cfg_blocks);
    chip->write_data(block_no + cfg_blocks, 0, chip->block_buf, chip->block_size);

    return true;
}

//----读取文件(目录)参数-------------------------------------------------------
//功能：从文件分配表项的内存镜像中的中读取文件(目录)信息，并初始化文件结构指针fp。
//参数：PTT_flash_tag，被操作的分区指针，通用结构，非flash专用。
//      file_num，在该文件系统中该文件号
//      current，保存文件(目录)属性的文件指针
//返回：true=成功，false=失败
//------------
//更新记录:
// 1.日期: 2015/3/19
//   说明: 修改文件时间属性格式
//   作者: 季兆林
//-----------------------------------------------------------------------------
bool_t __Easynor_ReadItem(struct tagPTTDevice *PTT_device_tag,
                           uint32_t file_num,
                           struct tagFileRsc *current)
{
    u32 block_no,offset,loop;
    u8 *hsize;
    struct tagEasynorFlashChip *chip;

    chip = (struct tagEasynorFlashChip *) PTT_device_tag->PTT_medium_tag;

    //计算该文件分配表在所在的block
    block_no = (file_num * CN_EASYNOR_CFG_ITEM_LIMIT)/chip->block_size;
    block_no = block_no + chip->start_block;

    //该文件分配表的在其block中的偏移
    offset = (file_num * CN_EASYNOR_CFG_ITEM_LIMIT) % chip->block_size;
    //配置文件信息
    chip->read_data(block_no, offset,chip->block_buf,CN_EASYNOR_CFG_ITEM_LIMIT);

    //以下设置文件资源描述，参照struct file_rsc定义中要求驱动模块维护的部分。
    current->file_attr.bits.folder = 0;
    current->file_attr.bits.archive = 1;
    current->file_semp = NULL;
    current->p_read_buf = NULL;
    current->p_write_buf = NULL;
    //文件的该部分信息easyfile并未实现，因此赋值为合法值
#if 0/* 文件时间格式改变,暂时屏蔽 */
    current->year_high_modify = 0x07;
    current->year_low_modify  = 0xdd;
    current->month_modify     = 1;
    current->date_modify      = 1;
    current->hour_modify      = 9;
    current->minute_modify    = 0;
#endif

    //文件大小
    for (loop = 0; loop < CN_EASYNOR_FILESIZE_NUM; loop++)
    {
        hsize = (u8*)(chip->block_buf + CN_EASYNOR_FILESIZE_END - loop * 8);
        if ((pick_little_64bit(hsize,0) == CN_LIMIT_UINT64) ||
            (pick_little_64bit(hsize,0) == 0))
            continue;
        if (pick_little_32bit(hsize,0) == ~(pick_little_32bit(hsize + 4,0)))
        {
            current->file_size = pick_little_32bit(chip->block_buf,
                    (CN_EASYNOR_FILESIZE_END - loop * 8) >> 2);
            break;
        }
    }
    if(loop == CN_EASYNOR_FILESIZE_NUM)//没有得到正确的文件大小
        return false;

    current->read_ptr = 0;
    current->write_ptr = 0;

    loop = 0;
    while((chip->block_buf[loop]) && (loop++ < CN_EASYNOR_FILENAME_LIMIT));
    if(loop >= CN_EASYNOR_FILENAME_LIMIT)//文件名错误
        return false;
    strncpy((char *)current->name, (char *)chip->block_buf, CN_EASYNOR_FILENAME_LIMIT);
    return true;
}

//----遍历一个目录的子项-------------------------------------------------------
//功能: 不支持
//参数: parent,需要搜索的树枝的祖先结点
//      current_file,当前搜索位置,空则表示第一次查找
//返回: 搜索到的子项指针(可能是文件或目录)
//备注: 这个函数，可以作为windows下 FindFirstFile等三个函数的配合遍历整
//      个目录下的文件
//      第一次遍历时(以current==NULL)为标志，系统将为struct file_rsc结构分配内存，
//      最后一次，将释放内存。如果用户正确地调用easynor_item_traversal_son遍历完
//      整个目录，内存管理将完全正常，否则，为struct file_rsc结构分配的内存将需
//      要用户手动释放
//---------------------------------------------------------------------
struct tagFileRsc * Easynor_ItemTraversalSon(struct tagFileRsc *parent,
                                             struct tagFileRsc *current)
{
    struct tagPTTDevice *PTT_device_tag;
    struct tagEasynorFlashChip *chip;

    if ((parent == NULL))
        return NULL;
    if(parent->file_medium_tag == CN_LIMIT_UINT32)
        return NULL;
    PTT_device_tag = (struct tagPTTDevice *)(parent->home_PTT);
    //文件所在flash芯片指针
    chip = (struct tagEasynorFlashChip *) PTT_device_tag->PTT_medium_tag;

    if(chip->files_sum == 0)    //还没有建立文件
        return NULL;

    if(current == NULL)
    {
        current = M_MallocLc(sizeof(struct tagFileRsc),CN_TIMEOUT_FOREVER);
        if(current == NULL)
            return NULL;
        memset(current,0,sizeof(struct tagFileRsc));
        if(__Easynor_ReadItem(PTT_device_tag,1,current))//读第一个文件的配置信息
            current->file_medium_tag = 1;//第一个文件
        else
        {
            free(current);
            return NULL;
        }
    }
    else if(current->file_medium_tag < chip->files_sum)//medium_tag暂时存放文件号
    {
        current->file_medium_tag ++;
        if(false == __Easynor_ReadItem(PTT_device_tag,
                    current->file_medium_tag,current))
        {
            free(current);
            return NULL;
        }

    }
    else if(current->file_medium_tag >= chip->files_sum)//超过了最大值
    {
        free(current);
        return NULL;
    }

    return current;
}

//----打开（文件/目录）--------------------------------------------------------
//功能：在文件柜中查找并打开文件(目录)，如果是文件，还需要为它分配读写缓冲区。只
//      查父目录的子项，不查孙项及以下
//参数：name，文件（目录）名
//      parent，文件/目录的父目录，
//      result，返回找到的文件的信息
//      mode，文件（目录）打开模式
//返回：cn_fs_open_success,成功打开文件(目录)
//      CN_FS_ITEM_EXIST,文件(目录)存在但不能打开(一般是模式不兼容)
//      CN_FS_ITEM_INEXIST,文件(目录)不存在
//      cn_limit_uint32，其他错误
//----------------------------------------------------------------------------
u32 Easynor_OpenItem(char *name, struct tagFileRsc *parent,
                      struct tagFileRsc *result, enum _FILE_OPEN_MODE_ mode)
{
    u32 loop, block_no, block_offset;
    u8 *hsize;
    struct tagPTTDevice *PTT_device_tag;
    struct tagEasynorFlashChip *chip;
    struct tagEasyFlashFileInfo *fileinfo;
    bool_t found = false;

    if ((parent == NULL) || (result == NULL))
        return CN_LIMIT_UINT32;
    PTT_device_tag = (struct tagPTTDevice *)(parent->home_PTT);
    //文件所在flash芯片指针
    chip = (struct tagEasynorFlashChip *) PTT_device_tag->PTT_medium_tag;
    fileinfo = &s_ptEasyNorInfo[PTT_device_tag->opened_sum];
    for (loop = 1; loop <= chip->files_sum; loop++)
    {
        block_no = loop * CN_EASYNOR_CFG_ITEM_LIMIT;
        block_offset = block_no % chip->block_size;
        block_no = chip->start_block + block_no / chip->block_size;
        chip->read_data(block_no, block_offset, chip->block_buf,
                CN_EASYNOR_PTT_NAME_LIMIT);
        chip->block_buf[CN_EASYNOR_PTT_NAME_LIMIT] = '\0';
        if (strncmp((const char*)chip->block_buf,(const char*)name,
                CN_EASYNOR_PTT_NAME_LIMIT) == 0)
        {
            chip->read_data(block_no, block_offset, chip->block_buf,
                    CN_EASYNOR_CFG_ITEM_LIMIT);
            found = true;
            fileinfo->start_item = loop;
            break;
        }
    }
    if (!found)
    {
        return CN_FS_ITEM_INEXIST;
    }
    else
    {
        //以下设置文件资源描述，参照struct file_rsc定义中要求驱动模块维护的部分。
        result->file_attr.bits.archive = 1;
        result->file_semp = NULL;
        result->file_medium_tag = (ptu32_t) fileinfo;
        result->p_read_buf = NULL;
        result->p_write_buf = NULL;
        for (loop = 0; loop < CN_EASYNOR_FILESIZE_NUM; loop++)
        {
            hsize = (chip->block_buf + CN_EASYNOR_FILESIZE_END - loop * 8);
            if ((pick_little_64bit(hsize,0) == CN_LIMIT_UINT64) ||
                (pick_little_64bit(hsize,0) == 0))
                continue;
            if (pick_little_32bit(hsize,0) == ~(pick_little_32bit(hsize + 4,0)))
            {
                result->file_size = pick_little_32bit(chip->block_buf,
                        (CN_EASYNOR_FILESIZE_END - loop * 8) >> 2);
                break;
            }
        }

        result->read_ptr = 0;
        result->write_ptr = 0;
        fileinfo->start_block = pick_little_32bit(chip->block_buf,
                CN_EASYNOR_STARTBLOCK_OFF >> 2);
        fileinfo->max_size = pick_little_32bit(chip->block_buf,
                CN_EASYNOR_MAXSIZE_OFF >> 2);
        strncpy(result->name, name, CN_EASYNOR_FILENAME_LIMIT);

        return CN_FS_OPEN_SUCCESS;
    }
}

//----查找（文件/目录）--------------------------------------------------------
//功能：在文件柜中查找并打开文件(目录)，如果是文件，还需要为它分配读写缓冲区。
//参数：name，文件（目录）名
//      parent，文件/目录的父目录，
//      result，返回找到的文件的信息
//返回：true=文件/目录存在，并在result中返回文件信息(不分配文件缓冲区)。
//      false = 文件/目录不存在，不修改result。
//注：easynorflash 文件系统不使用缓冲区以及各种文件模式，打开文件和查找文件的操作
//      是相同的。
//----------------------------------------------------------------------------
bool_t Easynor_LookforItem(char *name, struct tagFileRsc *parent,
                            struct tagFileRsc *result)
{
    if(Easynor_OpenItem(name,parent,result, 0)== CN_FS_OPEN_SUCCESS)
        return true;
    else
        return false;

}

//----关闭文件(目录)-----------------------------------------------------------
//功能: 关闭一个打开的文件(目录)，如果是文件，则释放读写缓冲区，并使文件柜中
//      opened_sum减量，如果是目录，则只是opened_sum减量。本函数只是处理关闭过程
//      与存储介质相关的操作，系统资源链表由file.c模块处理。
//参数：fp，被操作的文件指针
//返回：true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t Easynor_CloseItem(struct tagFileRsc *fp)
{
    return true;
}

//----文件（目录）改名---------------------------------------------------------
//功能：修改一个文件（目录）的名字，新名字保存在fp中。fp中的名字可以达到255字
//      符，但只有前cn_flash_file_name_limit字符有效。
//参数：fp，被操作的文件指针
//返回：true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t Easynor_RenameItem(struct tagFileRsc *fp, char *newname)
{
    return false;
}

//----查询文件柜参数-----------------------------------------------------------
//功能: 返回文件柜总空间，有效空间，空闲空间的字节数
//参数:
//返回: 无
//-----------------------------------------------------------------------------
void Easynor_CheckPTT(struct tagPTTDevice *PTT_device_tag,
                       sint64_t *sum_size, sint64_t *valid_size, sint64_t *free_size)
{
    if (sum_size != NULL)
        *sum_size = 0;
    if (valid_size != NULL)
        *valid_size = 0;
    if (free_size != NULL)
        *free_size = 0;
}

//----设置文件长度-------------------------------------------------------------
//功能：设置文件长度，easyfile模块只实现了filersc中要求的部分功能，只能为最后一
//      个建立的文件分配空间，且只能分配一次。
//参数：fp，被操作的文件指针
//      new_size，新的文件长度。
//返回：新的文件长度,文件名不正确则返回0
//-----------------------------------------------------------------------------
sint64_t Easynor_SetFileSize(struct tagFileRsc *fp, s64 new_size)
{
     struct tagEasynorFlashChip *chip;
     struct tagPTTDevice *PTT_device_tag;
     struct tagEasyFlashFileInfo *fileinfo;
     u32 block_no,offset,cfg_blocks;
     u8 buf[4];

    if (fp == NULL)
        return CN_LIMIT_UINT32;

    PTT_device_tag = (struct tagPTTDevice *)(fp->home_PTT);
    //文件所在flash芯片指针
    chip = (struct tagEasynorFlashChip *) PTT_device_tag->PTT_medium_tag;
    fileinfo = (struct tagEasyFlashFileInfo *)fp->file_medium_tag;

    //item不可能为0
    if(fileinfo->start_item == 0)
        return -1;

    //不是最后一个文件，则只修改其文件大小
    if(fileinfo->start_item != chip->files_sum)
    {
        __Easynor_ChangeFileSize(fp,new_size);
        return 0;
    }

    //计算该文件分配表在所在的block
    block_no = (fileinfo->start_item * CN_EASYNOR_CFG_ITEM_LIMIT) / chip->block_size;
    block_no = block_no + chip->start_block;
    //计算文件系统文件分配表主分配所使用的blocks
    cfg_blocks = (CN_EASYNOR_CFG_LIMIT + chip->block_size-1) / chip->block_size;

    //该文件分配表的在其block中的偏移
    offset = (fileinfo->start_item * CN_EASYNOR_CFG_ITEM_LIMIT)% chip->block_size;

    fill_little_32bit(buf, 0, (u32)new_size);

    //刚创建的文件，最大尺寸被初始化为0xFFFFFFFF，因此，若不需要擦除，则说明
    //此次函数调用是为了修改文件的最大尺寸
    if(chip->query_block_ready(block_no,offset+CN_EASYNOR_MAXSIZE_OFF,buf,sizeof(buf)))
    {
        fileinfo->max_size = new_size;
        chip->write_data(block_no,offset+CN_EASYNOR_MAXSIZE_OFF,buf,sizeof(buf));
        chip->write_data(block_no+cfg_blocks,offset+CN_EASYNOR_MAXSIZE_OFF,buf,sizeof(buf));
    }
    else//若需要擦除，则说明此次函数调用是修改最后一个文件的尺寸
    {
        __Easynor_ChangeFileSize(fp,new_size);
    }
    return 0;
}

//----查询目录表尺寸-----------------------------------------------------------
//功能：查询一个文件柜的目录表尺寸，按字节计算。当应用程序需要读取整个目录表时，
//      必须先用本函数计算目录表尺寸，然后才能分配内存。
//参数：PTT_device_tag，被操作的文件柜指针，通用结构，非flash专用。
//返回：目录表尺寸
//-----------------------------------------------------------------------------
u32 Easynor_CheckFDT_Size(struct tagPTTDevice *PTT_device_tag)
{
    return 0;
}
//----检查文件夹的子文件（目录）数---------------------------------------------
//功能：检查文件夹的子文件（目录）数，不包含子目录的子条目
//参数：PTT_device_tag，被操作的文件柜指针，通用结构，非flash专用。
//      fp，目标文件夹的文件指针
//返回：子文件（目录）数
//-----------------------------------------------------------------------------
u32 Easynor_CheckFolder(struct tagFileRsc *parent)
{
    return 0;
}

