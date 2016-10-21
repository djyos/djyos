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
#include "efs.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <lock.h>
#include <systime.h>
#include "../djyfs/file.h"
#include <verify.h>
#include <endian.h>
#include <driver/flash/flash.h>

// =============================================================================
//#define FILE_INIT_MAX_SIZE    (0xFFFFFFFF)	//由用户配置
#define FILE_FIXED_SIZE		  (0x200000)	//固定大小为2M

#define MUTEX_WAIT_TIME       CN_TIMEOUT_FOREVER//(2*1000*1000) //互斥访问等待时间

#define CN_EFS_MAX   1                      //简易文件系统的个数
tagEFS *s_pEfsList[CN_EFS_MAX];             //简易文件系统列表

#define CN_MAX_OPENED_FILE_NUM  (EFS_CFG_LIMIT/EFS_ITEM_LIMIT - 1) //支持的文件打开的个数
//static struct EfsFileInfo s_FileInfo[CN_MAX_OPENED_FILE_NUM];//打开文件的信息
volatile static u8 tgOpenedSum = 0;     //打开的文件数
static char NameBuf[256];
static void NameECC(char *Name, u8 *Ecc);
static s32 ChkOrRecNameByECC(char *Name, u8 *Ecc);

//----检查文件分配表filesize-----------------------------------------------------------
//功能: 检查一个文件分配表的filesize，只检查，不作任何改写
//参数: buf，缓冲区
//      location，filesize数据的位置，如果filesize出错，则是出错的位置
//返回: 0 = 无错，1=有错没有改正
//-----------------------------------------------------------------------------
static u32 __Efs_CheckFileSize(u8 *buf, u32 *location)
{
    u32 filesize_no, offset;
    u32 ret = 0;

    for (filesize_no = 0; filesize_no < FILE_FILESIZE_NUM;filesize_no++)
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
//      efs，所属芯片
//返回: 0 = 无错，1=有错并改正，2=有错不能改正
//-----------------------------------------------------------------------------
static u32 __Efs_CheckSingleBuf(u8 *buf, u32 index, struct EasyFS *efs)
{
    u32 offset, location, item_no,item_num;
    u32 temp_files_sum = 0;
    u32 ret = 0;

    item_num = (EFS_CFG_LIMIT > efs->block_size)?
            efs->block_size:EFS_CFG_LIMIT;
    item_num = item_num / EFS_ITEM_LIMIT;
    if(index == efs->start_block)
        item_no = 1;
    else
        item_no = 0;
    //一个块内有多个信息条，从第二 条开始检查,
    for (;item_no < item_num;item_no++)
    {
        offset = item_no * EFS_ITEM_LIMIT;
        if( (buf[offset] != 0xFF) && (buf[offset] != 0x00) )//name为非空，表示该条有用
        {
            temp_files_sum++; //该条用到，则文件数加1
            offset += FILE_FILESIZE_OFF;
            if(__Efs_CheckFileSize(buf + offset,&location)) //有错，则将filesize改写成0
            {
                ret = 1;
                memset(buf+offset+8*location,0,8);
            }
        }
    }
    //把本块内所有文件数放到chip->files_sum
    efs->files_sum += temp_files_sum;
    return ret;
}

//----检查扇区-----------------------------------------------------------
//功能: 检查一对(主+备份)文件分配表的扇区缓冲区，有错误则改正之,该数据已经在
//      flash中，改正后写进去。同时把该缓冲区包含的文件数增加到chip->files_sum。
//参数: buf，缓冲区
//      index，第几个扇区
//      efs，所属芯片
//返回: 无
//-----------------------------------------------------------------------------
static bool_t __Efs_CheckBlock(u8 *buf_main,u32 index_main, u8 *buf_bak,
                    u32 index_bak,struct EasyFS *efs)
{
    u32 item_no,offset,item_num;
    u32 reflash = 0, location;
    u32 temp_files_sum = 0;
    u8 * correct_filesize;

    item_num = (EFS_CFG_LIMIT > efs->block_size)?
            efs->block_size:EFS_CFG_LIMIT;
    item_num = item_num / EFS_ITEM_LIMIT;
    if(index_main == efs->start_block)
        item_no = 1;
    else
        item_no = 0;
    //从主备文件分配表的第二个表开始检查
    for (;item_no < item_num;item_no++)
    {
        offset = item_no * EFS_ITEM_LIMIT;
        if ((buf_main[offset] != 0xFF) || (buf_bak[offset] != 0xFF)) //从名称判断是否有用到该文件配置
        {
            if( (buf_main[offset] != 0x00) && (buf_bak[offset] != 0x00) ) //若为0，则表示该文件被删除
            {
                temp_files_sum++;
            }

            //检查主备分配表name,startblock,maxsize,crc
            if(memcmp(buf_main+offset,buf_bak+offset,FILE_FILESIZE_OFF))//若不相同
            {
                //主分配表错误,则发生擦除或者写入错误，需要要本条item全部复制
                if(memcmp(buf_main + FILE_VERIFITY_OFF, "easyfile", 8))
                {
                    memcpy(buf_main+offset,buf_bak+offset,EFS_ITEM_LIMIT);
                }
                else    //备用分配表错误
                {
                    memcpy(buf_bak+offset,buf_main+offset,EFS_ITEM_LIMIT);
                }
                reflash = 1;
                continue;
            }

            //核对主备文件分配表的filesize......todo:此处如果是nandflash,是否直接改成有错便擦除？
            offset += FILE_FILESIZE_OFF;
            if(memcmp(buf_main+offset,buf_bak+offset,
                    FILE_VERIFITY_OFF - FILE_FILESIZE_OFF))//主备分配表filesize大小不同
            {
                reflash = 1;        //nandflash 有错尽量去纠正，同时擦除该块
                if (__Efs_CheckFileSize(buf_main + offset, &location)) //主分配表有错
                {
                    correct_filesize = buf_bak+offset+8*location;
                    if(pick_little_64bit(correct_filesize,0) == CN_LIMIT_UINT64)//主分配表filesize错误，但是bak还没写
                    {
                        memset(correct_filesize, 0, 8);//与主分配表保持一致
                        memset(buf_main + offset + 8 * location, 0, 8);
//                        efs->write_data(index_main,offset+8 * location,buf_main+offset+8*location,8);
//                        efs->write_data(index_bak, offset+8 * location,buf_bak+offset+8*location,8);
                    }
                    else if(location == FILE_FILESIZE_NUM)//错误的是最后一条filesize
                    {
                        reflash = 1;
                        memset(buf_main+offset,0xff,FILE_VERIFITY_OFF-FILE_FILESIZE_OFF);
                        memcpy(buf_main+offset,correct_filesize,8);//保存正确filesize到第一条位置
                        memset(buf_bak+offset,0xff,FILE_VERIFITY_OFF-FILE_FILESIZE_OFF);
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
//                        efs->write_data(index_main,offset+8 * location,buf_main+offset+8*location,16);
//                        efs->write_data(index_bak, offset+8 * location,buf_bak+offset+8*location,16);
                    }
                }
                else//主分配表正确
                {
                    correct_filesize = buf_main+offset+8*location;
                    if(pick_little_64bit(correct_filesize,0) == CN_LIMIT_UINT64)//主分配表filesize错误，但是bak还没写
                    {
                        memset(correct_filesize, 0, 8);//与主分配表保持一致
                        memset(buf_bak + offset + 8 * location, 0, 8);
//                        efs->write_data(index_main,offset+8 * location,buf_main+offset+8*location,8);
//                        efs->write_data(index_bak, offset+8 * location,buf_bak+offset+8*location,8);
                    }
                    else if(location == FILE_FILESIZE_NUM)//错误的是最后一条filesize
                    {
                        reflash = 1;
                        memset(buf_bak+offset,0xff,FILE_VERIFITY_OFF-FILE_FILESIZE_OFF);
                        memcpy(buf_bak+offset,correct_filesize,8);//保存正确filesize到第一条位置
                        memset(buf_main+offset,0xff,FILE_VERIFITY_OFF-FILE_FILESIZE_OFF);
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
//                        efs->write_data(index_main,offset+8 * location,buf_main+offset+8*location,16);
//                        efs->write_data(index_bak, offset+8 * location,buf_bak+offset+8*location,16);
                    }
                }
            }
        }
    }

    efs->files_sum += temp_files_sum;
    return reflash;
}
//-----------------------------------------------------------------------------
//功能: 检查和修复所有文件的文件名
//参数:
//返回:
//-----------------------------------------------------------------------------
static bool_t __Efs_CheckAllNameECC(u8 *FileInfoBuf)
{
    u32 offset,loop,fixedFiles;
    u8 *buf;

	fixedFiles = EFS_CFG_LIMIT/EFS_ITEM_LIMIT;
	for (loop = 1; loop < fixedFiles; loop++)
    {
		offset = loop * EFS_ITEM_LIMIT;
		buf = FileInfoBuf + offset;
		if( (buf[0] != 0xFF) && (buf[0] != 0x00) )
		{
			buf[FILENAME_LIMIT] = '\0';
			ChkOrRecNameByECC((char *)buf,buf+FILENAME_LIMIT+1);
		}
    }
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
static void __Efs_ChangeFileSize(struct FileRsc *fp, u32 newsize)
{
    struct EfsFileInfo *fileinfo;
    struct EasyFS *efs;
    u8 *block_buf;
    u32 offset, blocksize, cfg_blocks, block_no, item_no,temp;
    u32 filesize_no,file_max_size;
    u16 crc16_check;

    fileinfo = (struct EfsFileInfo *) fp->private;
    efs = (struct EasyFS *) fp->efs;

    block_buf = efs->block_buf;
    blocksize = EFS_CFG_LIMIT;
    cfg_blocks = (EFS_CFG_LIMIT+ efs->block_size-1) / efs->block_size;

    //计算该文件分配表在所在的block
    block_no = (fileinfo->item * EFS_ITEM_LIMIT) / efs->block_size;
    block_no = block_no + efs->start_block;
    //该文件分配表的在其block中的偏移
    offset = (fileinfo->item * EFS_ITEM_LIMIT) % efs->block_size;
    //计算该文件在所属的block中是第几个文件item
    item_no = offset / EFS_ITEM_LIMIT;

    //读出该文件分配表
    efs->read_data(block_no, offset,block_buf,EFS_ITEM_LIMIT,EF_WR_NOECC);

    ChkOrRecNameByECC((char *)(efs->block_buf),efs->block_buf+FILENAME_LIMIT+1);
    //文件尺寸不能大于文件最大尺寸
    file_max_size = pick_little_32bit(block_buf,FILE_MAXSIZE_OFF >> 2);

    if(newsize > file_max_size)
        return;

    offset = FILE_FILESIZE_OFF;
    for (filesize_no = 0; filesize_no < FILE_FILESIZE_NUM;filesize_no++)
    {
        if (pick_little_64bit(block_buf + offset,0) == CN_LIMIT_UINT64) //在主分配表找到空闲位置
        {
            fill_little_32bit(block_buf, offset >> 2, newsize);
            fill_little_32bit(block_buf, (offset >> 2) + 1, ~newsize);
            temp = offset + item_no * EFS_ITEM_LIMIT;
            efs->write_data(block_no, temp, block_buf + offset, 8,EF_WR_NOECC);
            efs->write_data(block_no + cfg_blocks, temp, block_buf + offset, 8,EF_WR_NOECC);
            break;
        }
        offset += 8;
    }

    if (filesize_no == FILE_FILESIZE_NUM) //主分配表没有空余位置
    {
        //先读出整块数据到buf,并擦除
        efs->read_data(block_no, 0, block_buf,blocksize,EF_WR_NOECC);
        __Efs_CheckAllNameECC(efs->block_buf);

        efs->erase_block(block_no);
        efs->erase_block(block_no + cfg_blocks);

        offset = item_no * EFS_ITEM_LIMIT + FILE_FILESIZE_OFF;
        memset(block_buf + offset,0xFF,FILE_VERIFITY_OFF - FILE_FILESIZE_OFF);
        fill_little_32bit(block_buf, offset >> 2, newsize);
        fill_little_32bit(block_buf, (offset >> 2) + 1, ~newsize);

        crc16_check = crc16(efs->block_buf,16);
        fill_little_16bit((u8*)&temp,0,crc16_check);    //转化为小端

        efs->write_data(block_no, 0, block_buf, blocksize,EF_WR_NOECC);
        efs->write_data(block_no + cfg_blocks, 0, block_buf, blocksize,EF_WR_NOECC);

        efs->write_data(efs->start_block + cfg_blocks - 1,
                efs->block_size-2, (u8*)&temp,2,EF_WR_NOECC);
        efs->write_data(block_no + 2*cfg_blocks - 1,
                efs->block_size-2, (u8*)&temp,2,EF_WR_NOECC);

    }
}

// =============================================================================
// 功能：根据文件系统的名称，从文件系统列表中查找文件系统的结构体指针
// 参数：name,文件系统名
// 返回：NULL，未找到，否则返回指针
// =============================================================================
static tagEFS* __Efs_Find(const char *name)
{
    u8 i;
    tagEFS* efs = NULL;

    for(i = 0; i <CN_EFS_MAX;i++)
    {
        if(!strcmp(s_pEfsList[i]->name,name))
        {
            efs = s_pEfsList[i];
            break;
        }
    }
    return efs;
}

// =============================================================================
// 功能：新建并打开文件，初始化文件相关的信息，文件大小为0，最大尺寸为默认
// 参数：filename,文件名
// 返回：true,创建成功，false,创建失败
// =============================================================================
static bool_t __Efs_NewFile(tagFileRsc* fp,const char *filename,
                        tagEfsFileInfo *fileinfo)
{
    struct EasyFS *efs;

    u32 block_no,start_block;
    u32 offset;
    u8 cfg_blocks,item,item_num;
    u16 crc16_check,temp;

    if( (fp == NULL) || (strlen(filename) > FILENAME_LIMIT) )//文件名太长
    {
    	printf("Create New File Failed For Too Long Name!\r\n");
        return 0;
    }

    efs = fp->efs;
    if (efs->files_sum >= EFS_CFG_LIMIT / EFS_ITEM_LIMIT)
        return false; //已达可创建文件的上限

    offset = 0;
    block_no = efs->start_block;
    //计算文件系统文件分配表主分配所使用的blocks
    cfg_blocks = (EFS_CFG_LIMIT + efs->block_size-1) / efs->block_size;

    efs->read_data(block_no, offset, efs->block_buf,EFS_CFG_LIMIT,EF_WR_NOECC);

    __Efs_CheckAllNameECC(efs->block_buf);
    //查找为全0或全FF的位置
    item_num = EFS_CFG_LIMIT/EFS_ITEM_LIMIT;
    for(item = 1; item < item_num; item++)  //todo
    {
        offset = item * EFS_ITEM_LIMIT;
        if(efs->block_buf[offset] == 0x00)  //若为0，则表示该文件被删除,可用于新建文件
        {
            memset(efs->block_buf+offset,0xFF,EFS_ITEM_LIMIT);
            crc16_check = crc16(efs->block_buf,16);
            fill_little_16bit((u8*)&temp,0,crc16_check);    //转化为小端
            //added,将最后的CRC写到文件系统信息的最后两个字节
            efs->erase_block(block_no);
            efs->write_data(block_no,0,efs->block_buf,EFS_CFG_LIMIT,EF_WR_NOECC);
            efs->erase_block(block_no+cfg_blocks);
            efs->write_data(block_no+cfg_blocks,0,efs->block_buf,EFS_CFG_LIMIT,EF_WR_NOECC);

            efs->write_data(efs->start_block + cfg_blocks - 1,
                    efs->block_size-2, (u8*)&temp,2,EF_WR_NOECC);
            efs->write_data(block_no + 2*cfg_blocks - 1,
                    efs->block_size-2, (u8*)&temp,2,EF_WR_NOECC);
            break;
        }
        if(efs->block_buf[offset] == 0xFF) //从名称判断是否有用到该文件配置
        {
            break;
        }
    }

    if(item == EFS_CFG_LIMIT/EFS_ITEM_LIMIT)        //没有找到空闲位置或文件数量达到上限
        return false;

    //计算新建的文件内容存储位置
    start_block = efs->start_block + 2*cfg_blocks +
            (item - 1)*FILE_FIXED_SIZE/efs->block_size;

    //同时需要擦除文件所在的块数据
    block_no = FILE_FIXED_SIZE / efs->block_size;
    for(temp = 0; temp < block_no; temp++)
    {
        efs->erase_block(temp + start_block);
    }

    block_no = efs->start_block;
    //以下初始化文件分配表并写入flash
    memset((char *)efs->block_buf, 0xff, EFS_ITEM_LIMIT);
    strncpy((char *)efs->block_buf, filename, FILENAME_LIMIT+1);
    efs->block_buf[FILENAME_LIMIT] = '\0';
    NameECC(filename,efs->block_buf + FILENAME_LIMIT+1);
    fill_little_32bit(efs->block_buf, FILE_STARTBLOCK_OFF >> 2,start_block);
    fill_little_32bit(efs->block_buf, FILE_MAXSIZE_OFF >> 2, FILE_FIXED_SIZE);
    fill_little_32bit(efs->block_buf, FILE_FILESIZE_OFF >> 2, 0);
    fill_little_32bit(efs->block_buf, (FILE_FILESIZE_OFF >> 2) + 1, ~0);
    memcpy(efs->block_buf + FILE_VERIFITY_OFF, "easyfile", 8);
    efs->write_data(block_no, offset, efs->block_buf,EFS_ITEM_LIMIT,EF_WR_NOECC);
    efs->write_data(block_no+cfg_blocks,offset,efs->block_buf,EFS_ITEM_LIMIT,EF_WR_NOECC);

    strncpy(fp->name,filename,FILENAME_LIMIT+1);
    //更新文件信息，创建即打开
    fp->private = (ptu32_t)fileinfo;
    fp->file_size = 0;
    fp->ptr = 0;

    //更新文件信息，创建即打开
    fileinfo->max_size = FILE_FIXED_SIZE;
    fileinfo->start_block = start_block;
    fileinfo->item = item;
    fileinfo->filesize = 0;

    return true;
}

// =============================================================================
// 功能：检索文件系统，并进行主备区文件系统的校验和校正
// 参数：efs,文件系统指针，未初始化
//      fs,文件系统名称
//      opt
// 返回：true,创建成功，false,创建失败
// =============================================================================
FSRESULT Efs_Mount (tagEFS* efs, const char* fs,u8 opt)
{
    u32 loop, blocks, end_block, fileInfoSize;
    u8 *bakbuf, *block_buf;
    u8 mainblockerr = 0,bakblockerr = 0;
    u16 crc16_check,temp;
    FSRESULT result = FSR_OK;

    if(!efs->block_sum)                 //说明该chip未初始化
    {
        if(!Efs_Initlization(efs,fs))
           return FSR_NOT_READY;
    }

    bakbuf = M_MallocLc(EFS_CFG_LIMIT, 0);//只需文件信息大小
    if (bakbuf == NULL)
        return FSR_NOT_ENOUGH_CORE;

    blocks = (EFS_CFG_LIMIT + efs->block_size-1) / efs->block_size;
    end_block = efs->start_block + blocks;
    fileInfoSize = EFS_CFG_LIMIT;
    block_buf = efs->block_buf;

   if(false == Lock_MutexPend(efs->block_buf_mutex,MUTEX_WAIT_TIME))
   {
       result = FSR_DENIED;
       goto exit;
   }

    //检查文件系统是否已经建立
    efs->read_data(efs->start_block, 0, block_buf, EFS_ITEM_LIMIT,EF_WR_NOECC);
    efs->read_data(end_block, 0, bakbuf, EFS_ITEM_LIMIT,EF_WR_NOECC);

    //主备区都被破坏，则需要重新建立文件系统
    if ((memcmp(bakbuf, "easyfile", 8) ||
            memcmp(bakbuf + FILE_VERIFITY_OFF, "easyfile", 8)) &&
        (memcmp(block_buf, "easyfile", 8)||
            memcmp(block_buf + FILE_VERIFITY_OFF, "easyfile", 8)) )
    {
        printf("文件系统未建立!\r\n");
        result = FSR_NO_FILESYSTEM;
        Lock_MutexPost(efs->block_buf_mutex);
        goto exit;
    }

    //检查文件系统信息区最后两字节CRC是否正确
    efs->read_data(efs->start_block + blocks - 1, efs->block_size-2,
            block_buf+fileInfoSize-2, 2,EF_WR_NOECC);
    efs->read_data(end_block + blocks - 1, efs->block_size-2,
            bakbuf+fileInfoSize-2, 2,EF_WR_NOECC);

    crc16_check = pick_little_16bit(block_buf,(fileInfoSize/2)-1);
    if(crc16_check != crc16(block_buf,16))
        mainblockerr = 1;

    crc16_check = pick_little_16bit(bakbuf,(fileInfoSize/2)-1);
    if(crc16_check != crc16(bakbuf,16)) //bak校验错
        bakblockerr = 1;

    //以下检查数据正确性
    loop = efs->start_block;
    efs->read_data(loop, 0, block_buf, fileInfoSize,EF_WR_NOECC);
    efs->read_data(loop + blocks, 0, bakbuf, fileInfoSize,EF_WR_NOECC);

    switch((mainblockerr << 1) + (bakblockerr << 0))
    {
    case 0x03:      //两个缓冲区校验均出错，表明文件系统已经损坏，报错
        result = FSR_NO_FILESYSTEM;
        break;
    case 0x02:      //mainbuf error, bak校验正确
        temp = __Efs_CheckSingleBuf(bakbuf, loop, efs);
        if (temp == 0)
        {
            efs->erase_block(loop);
            efs->write_data(loop, 0, bakbuf, fileInfoSize,EF_WR_NOECC);

            crc16_check = crc16(bakbuf,16);
            fill_little_16bit((u8*)&temp,0,crc16_check);    //转化为小端
            efs->write_data(loop + blocks - 1,
                    efs->block_size-2, (u8*)&temp,2,EF_WR_NOECC);
        }
        else if (temp == 1)
        {
            efs->erase_block(loop);
            efs->write_data(loop, 0, bakbuf, fileInfoSize,EF_WR_NOECC);
            efs->erase_block(loop + blocks);
            efs->write_data(loop + blocks, 0,
                    bakbuf, fileInfoSize,EF_WR_NOECC);

            crc16_check = crc16(bakbuf,16);
            fill_little_16bit((u8*)&temp,0,crc16_check);    //转化为小端
            efs->write_data(loop + blocks - 1,
                    efs->block_size-2, (u8*)&temp,2,EF_WR_NOECC);

            efs->write_data(loop + 2*blocks - 1,
                    efs->block_size-2, (u8*)&temp,2,EF_WR_NOECC);
        }
        else
        {
            result = FSR_NO_FILESYSTEM;
        }
        break;
    case 0x01:          //主校验正确, bakbuf error
        temp = __Efs_CheckSingleBuf(block_buf, loop,efs);
        if (temp == 0)
        {
            efs->erase_block(loop + blocks);
            efs->write_data(loop + blocks, 0,
                    block_buf,fileInfoSize,EF_WR_NOECC);

            crc16_check = crc16(block_buf,16);
            fill_little_16bit((u8*)&temp,0,crc16_check);    //转化为小端
            efs->write_data(loop + 2*blocks - 1,
                    efs->block_size-2, (u8*)&temp,2,EF_WR_NOECC);
        }
        else if (temp == 1)
        {
            efs->erase_block(loop);
            efs->write_data(loop, 0, block_buf, fileInfoSize,EF_WR_NOECC);
            efs->erase_block(loop + blocks);
            efs->write_data(loop + blocks, 0,
                    block_buf,fileInfoSize,EF_WR_NOECC);

            crc16_check = crc16(block_buf,16);
            fill_little_16bit((u8*)&temp,0,crc16_check);    //转化为小端
            efs->write_data(loop + blocks - 1,
                    efs->block_size-2, (u8*)&temp,2,EF_WR_NOECC);

            efs->write_data(loop + 2*blocks - 1,
                    efs->block_size-2, (u8*)&temp,2,EF_WR_NOECC);
        }
        else
        {
            result = FSR_NO_FILESYSTEM;
        }
        break;
    case 0x00:          //main & bak all right
        if(true == __Efs_CheckBlock(block_buf,loop, bakbuf, loop+blocks, efs) )
        {
            //分配表信息错或filesize满，则写入flash
            efs->erase_block(loop);
            efs->erase_block(loop+blocks);
            efs->write_data(loop, 0, block_buf, fileInfoSize,EF_WR_NOECC);
            efs->write_data(loop+blocks, 0,bakbuf, fileInfoSize,EF_WR_NOECC);

            crc16_check = crc16(block_buf,16);
            fill_little_16bit((u8*)&temp,0,crc16_check);    //转化为小端
            efs->write_data(loop + blocks - 1,
                    efs->block_size-2, (u8*)&temp,2,EF_WR_NOECC);

            efs->write_data(loop + 2*blocks - 1,
                    efs->block_size-2, (u8*)&temp,2,EF_WR_NOECC);
        }
        break;
    default:
        result = FSR_NO_FILESYSTEM;
        break;
    }

    if(result == FSR_OK)
    {
        s_pEfsList[0] = efs;
        strncpy(s_pEfsList[0]->name,fs,EFS_NAME_LIMIT+1);
    }

    Lock_MutexPost(efs->block_buf_mutex);

exit:
    free(bakbuf);
    return result;
}

// =============================================================================
// 功能：创建文件系统，在mount不成功时，会调用，调用后原有文件信息将不存在
// 参数：efs,文件系统指针
//      path,名称
//      au,
// 返回：FSRESULT，正确返回是FSR_OK
// =============================================================================
FSRESULT Efs_Mkfs(tagEFS* efs,const char* path,u8 sfd,u32 au)
{
    u32 loop, blocks, end_block, fileInfoSize;
    u8 *block_buf;
    u16 crc16_check,temp;

    if(!efs->block_sum)                 //说明该chip未初始化
    {
       if(!Efs_Initlization(efs,path))
           return FSR_NOT_READY;
    }

    blocks = (EFS_CFG_LIMIT + efs->block_size-1) / efs->block_size;
    end_block = efs->start_block + blocks;
    fileInfoSize = EFS_CFG_LIMIT;
    block_buf = efs->block_buf;

    if(false == Lock_MutexPend(efs->block_buf_mutex,MUTEX_WAIT_TIME))
    {
       return FSR_DENIED;
    }

    memset(block_buf, 0xff, fileInfoSize);
    memcpy(block_buf, "easyfile", 8);
    memcpy(block_buf + FILE_VERIFITY_OFF, "easyfile", 8);
    fill_little_32bit(block_buf, 2, efs->block_sum);
    fill_little_32bit(block_buf, 3, efs->block_size);

    crc16_check = crc16(block_buf,16);
    fill_little_16bit((u8*)&temp,0,crc16_check);    //转化为小端
    for (loop = efs->start_block; loop < end_block; loop++)
    {
        efs->erase_block(loop);
        efs->erase_block(loop + blocks);
    }
    efs->write_data(efs->start_block, 0, block_buf,fileInfoSize,EF_WR_NOECC);
    efs->write_data(end_block, 0, block_buf, fileInfoSize,EF_WR_NOECC);
    //added,将最后的CRC写到文件系统信息的最后两个字节
    efs->write_data(efs->start_block + blocks - 1,
            efs->block_size-2, (u8*)&temp,2,EF_WR_NOECC);
    efs->write_data(end_block + blocks - 1,
            efs->block_size-2, (u8*)&temp,2,EF_WR_NOECC);

    //每次新建都将fs存放到全局变量tgNorDisk，用于后面操作
    s_pEfsList[0] = efs;
    strncpy(s_pEfsList[0]->name,path,EFS_NAME_LIMIT+1);
    efs->files_sum = 0;

    Lock_MutexPost(efs->block_buf_mutex);
    return FSR_OK;
}

// =============================================================================
// 功能：根据文件指针查找它所属的根目录
// 参数：fp,文件指针
//      fsname,文件名称
// 返回：FSRESULT，正确返回是FSR_OK
// =============================================================================
FSRESULT Efs_FindFs(tagFileRsc* fp,char *fsname)
{
    fp->efs = __Efs_Find(fsname);
    if(NULL != fp->efs)
    {
        return FSR_OK;
    }
    return FSR_INVALID_PARAMETER;
}

// =============================================================================
// 功能：获取单个文件的尺寸
// 参数：fname,分区名；fsname,文件名;filesize,文件尺寸指针
// 返回：FSRESULT，正确返回是FSR_OK
// =============================================================================
FSRESULT Efs_Stat(const char *fname,const char *fsname,u32 *filesize)
{
    tagEFS *efs;
    u32 loop,block_no=0,block_offset = 0;
    u8 found = false,fixedFiles;
    u8 *hsize;
    FSRESULT ret = FSR_NO_FILE;

    efs = __Efs_Find(fsname);
    if(NULL == efs)
        return FSR_INVALID_PARAMETER;

    if(false == Lock_MutexPend(efs->block_buf_mutex,MUTEX_WAIT_TIME))
    {
       return FSR_DENIED;
    }

    //判断是否存在该文件
    fixedFiles = EFS_CFG_LIMIT/EFS_ITEM_LIMIT;
    for (loop = 1; loop < fixedFiles; loop++)
    {
        block_no = loop * EFS_ITEM_LIMIT;
        block_offset = block_no % efs->block_size;
        block_no = efs->start_block + block_no / efs->block_size;
        efs->read_data(block_no, block_offset,efs->block_buf,
                                EFS_ITEM_LIMIT,EF_WR_NOECC);
        efs->block_buf[FILENAME_LIMIT] = '\0';
        ChkOrRecNameByECC((char *)(efs->block_buf),efs->block_buf+FILENAME_LIMIT+1);
        if (strncmp((const char*)efs->block_buf,fname,FILENAME_LIMIT) == 0)
        {
            found = true;
            break;
        }
    }

    if(found)       //未找到指定的文件
    {
        for (loop = 0; loop < FILE_FILESIZE_NUM; loop++)
        {
            hsize = (efs->block_buf + FILE_FILESIZE_END - loop * 8);
            if ((pick_little_64bit(hsize,0) == CN_LIMIT_UINT64) ||
                (pick_little_64bit(hsize,0) == 0))
                continue;
            if (pick_little_32bit(hsize,0) == ~(pick_little_32bit(hsize + 4,0)))
            {
                *filesize = pick_little_32bit(efs->block_buf,
                        (FILE_FILESIZE_END - loop * 8) >> 2);
                break;
            }
        }
        ret = FSR_OK;
    }

    Lock_MutexPost(efs->block_buf_mutex);
    return ret;
}

// =============================================================================
// 功能：读目录，即读第一个文件或者fname的下一个文件
// 参数：fname,分区名；fsname,文件名;FirstRdDirflg,是否为第一次读目录
// 返回：FSRESULT，正确返回是FSR_OK
// =============================================================================
FSRESULT Efs_DirRead(const char *fname,const char *fsname,u32 *item)
{
    FSRESULT result = FSR_NO_FILE;
    tagEFS *efs;
    u32 loop = 1,block_no=0,block_offset = 0;
    u32 fixedFiles;

    efs = __Efs_Find(fsname);
    if(NULL == efs)
        return FSR_INVALID_PARAMETER;

    if(false == Lock_MutexPend(efs->block_buf_mutex,MUTEX_WAIT_TIME))
    {
       return FSR_DENIED;
    }

    //判断是否存在该文件
    loop = *item;
    fixedFiles = EFS_CFG_LIMIT/EFS_ITEM_LIMIT;
    for(; loop < fixedFiles; loop++ )       //查找fname下一个文件或第一个文件
    {
        block_no = loop * EFS_ITEM_LIMIT;
        block_offset = block_no % efs->block_size;
        block_no = efs->start_block + block_no / efs->block_size;
        efs->read_data(block_no, block_offset,
				efs->block_buf,FILENAME_LIMIT + 1 + ECC_LIMIT,EF_WR_NOECC);
		efs->block_buf[FILENAME_LIMIT] = '\0';
		ChkOrRecNameByECC((char *)(efs->block_buf),efs->block_buf+FILENAME_LIMIT+1);
        //查找下一个有效的文件
        if( (efs->block_buf[0] != 0x00) && (efs->block_buf[0] != 0xFF) )
        {
			memcpy(fname,efs->block_buf,FILENAME_LIMIT+1);
            result = FSR_OK;
            break;
        }
    }

    if(loop == fixedFiles)
        *item = 1;
    else
        *item = loop + 1;
    Lock_MutexPost(efs->block_buf_mutex);
    return result;
}

// =============================================================================
// 功能：打开文件，若文件不存在，则可根据mode方式，决定是否新建文件
// 参数：fp,文件指针，未初始化的文件指针
//      name,文件名称
//      mode,
// 返回：FSRESULT，正确返回是FSR_OK
// =============================================================================
FSRESULT Efs_Open(tagFileRsc* fp,const char* name,u8 mode)
{
    tagEFS* efs;
    struct EfsFileInfo *fileinfo;
    u32 loop, block_no, block_offset;
    u8 *hsize,fixedFiles,*buf;
    bool_t found = false;
    FSRESULT Ret;

    if ((fp == NULL) || (name == NULL))
        return FSR_INVALID_PARAMETER;

    if(tgOpenedSum >= CN_MAX_OPENED_FILE_NUM)
        return FSR_TOO_MANY_OPEN_FILES;

    efs = fp->efs;
    fileinfo = malloc(sizeof(tagEfsFileInfo));//&s_FileInfo[tgOpenedSum];
    buf = malloc(CN_FILE_BUF_LIMIT);

    if( (NULL == fileinfo) || (NULL == buf) || efs == NULL)
    {
        Ret = FSR_NOT_ENOUGH_CORE;
        goto exit;
    }

    memset(fileinfo,0xFF,sizeof(struct EfsFileInfo));
    memset(buf,0xFF,CN_FILE_BUF_LIMIT);

    if(false == Lock_MutexPend(efs->block_buf_mutex,MUTEX_WAIT_TIME))
    {
       Ret = FSR_DENIED;
       goto exit;
    }

    //nand ，文件扫描固定文件数
    fixedFiles = EFS_CFG_LIMIT/EFS_ITEM_LIMIT;
    for (loop = 1; loop < fixedFiles; loop++)
    {
        block_no = loop * EFS_ITEM_LIMIT;
        block_offset = block_no % efs->block_size;
        block_no = efs->start_block + block_no / efs->block_size;
        efs->read_data(block_no, block_offset,
        		efs->block_buf,FILENAME_LIMIT+1+ECC_LIMIT,EF_WR_NOECC);
        efs->block_buf[FILENAME_LIMIT] = '\0';
        ChkOrRecNameByECC((char *)(efs->block_buf),efs->block_buf+FILENAME_LIMIT+1);
        if (strncmp((const char*)efs->block_buf,name,FILENAME_LIMIT) == 0)
        {
            efs->read_data(block_no, block_offset,
                    efs->block_buf,EFS_ITEM_LIMIT,EF_WR_NOECC);
            found = true;
            fileinfo->item = loop;
            break;
        }
    }

    if (!found)             //未找到，判断是否是新建文件
    {
        if(mode & EF_OP_CREATE)
        {
            if(__Efs_NewFile(fp,name,fileinfo))
            {
                efs->files_sum++;
                fp->file_size = 0;
                Ret = FSR_OK;
            }
            else
            {
                Ret = FSR_EXIST;
            }
        }
        else
        {
            Ret = FSR_NO_FILE;
        }
    }
    else if(mode & EF_OP_OPEN)      //此处 文件存在，且打开
    {
        //以下设置文件资源描述，参照struct file_rsc定义中要求驱动模块维护的部分。
        for (loop = 0; loop < FILE_FILESIZE_NUM; loop++)
        {
            hsize = (efs->block_buf + FILE_FILESIZE_END - loop * 8);
            if ((pick_little_64bit(hsize,0) == CN_LIMIT_UINT64) ||
                (pick_little_64bit(hsize,0) == 0))
                continue;
            if (pick_little_32bit(hsize,0) == ~(pick_little_32bit(hsize + 4,0)))
            {
                fp->file_size = pick_little_32bit(efs->block_buf,
                        (FILE_FILESIZE_END - loop * 8) >> 2);
                break;
            }
        }

        fileinfo->start_block = pick_little_32bit(efs->block_buf,
                FILE_STARTBLOCK_OFF >> 2);
        fileinfo->max_size = pick_little_32bit(efs->block_buf,FILE_MAXSIZE_OFF >> 2);
        fileinfo->filesize = fp->file_size;
        strncpy(fp->name, name, FILENAME_LIMIT+1);
        Ret = FSR_OK;
    }
    else    //找到了该名称的文件，同时标记为新建，返回错误
    {
        Ret = FSR_INVALID_NAME;
    }

    if(Ret == FSR_OK)
    {
        fp->wr_buf = buf;
        fp->buf_off = 0;
        fp->ptr = 0;
        fp->private = (ptu32_t)fileinfo;
        tgOpenedSum ++;
    }
    else
    {
exit:
        free(buf);
        free(fileinfo);
    }

    Lock_MutexPost(efs->block_buf_mutex);
    return Ret;
}

// =============================================================================
// 功能：关闭文件,主要工作是释放文件指针的空间和打开文件空间
// 参数：fp,文件指针，未初始化的文件指针
// 返回：FSRESULT，正确返回是FSR_OK
// =============================================================================
FSRESULT Efs_Close (tagFileRsc *fp)
{
    tagEfsFileInfo *fileinfo;
    struct EasyFS *efs;
    u32 block,offset;
    FSRESULT ret = FSR_OK;

    if(fp == NULL)
        return FSR_INVALID_PARAMETER;

    efs = fp->efs;
    fileinfo = (tagEfsFileInfo *)fp->private;

    if(false == Lock_MutexPend(efs->block_buf_mutex,MUTEX_WAIT_TIME))
    {
       return FSR_DENIED;
    }
    //若wr_buf有数据，则先将其写入flash，不带ECC
    if(fp->buf_off)
    {
        block = (fileinfo->filesize / efs->block_size) + fileinfo->start_block;
        offset = fileinfo->filesize % efs->block_size;

        if(!efs->query_block_ready(block,offset,fp->wr_buf,fp->buf_off))
        {
            ret = FSR_DENIED;
            goto exit;
        }

        __Efs_ChangeFileSize(fp, fp->file_size);
        //此处写入大小没有256，写入EF_WR_ECC只是为了底层更好做一点
        if(fp->buf_off !=
            efs->write_data(block,offset,fp->wr_buf,fp->buf_off,EF_WR_ECC))
        {
            __Efs_ChangeFileSize(fp, fileinfo->filesize);   //写入失败，则更正filesize
            ret = FSR_DISK_ERR;
            goto exit;
        }
    }

    free(fileinfo);
    free(fp->wr_buf);
    free(fp);//todo ---- 不应该在此处释放，在efs/port.c里面释放，因为在那里malloc
    tgOpenedSum --;
exit:
    Lock_MutexPost(efs->block_buf_mutex);
    return (ret);
}

// =============================================================================
// 功能：将缓存的数据写入flash
// 参数：fp,文件指针
// 返回：FSRESULT，正确返回是FSR_OK
// =============================================================================
FSRESULT Efs_Flush (tagFileRsc *fp)
{
    tagEfsFileInfo *fileinfo;
    struct EasyFS *efs;
    u32 block,offset;
    FSRESULT ret = FSR_OK;

    if(fp == NULL)
        return FSR_INVALID_PARAMETER;

    efs = fp->efs;
    fileinfo = (tagEfsFileInfo *)fp->private;
    if(false == Lock_MutexPend(efs->block_buf_mutex,MUTEX_WAIT_TIME))
    {
       return FSR_DENIED;
    }

    //若wr_buf有数据，则先将其写入flash，不带ECC
    if(fp->buf_off)
    {
        block = (fileinfo->filesize / efs->block_size) + fileinfo->start_block;
        offset = fileinfo->filesize % efs->block_size;

        if(!efs->query_block_ready(block,offset,fp->wr_buf,fp->buf_off))
        {
            ret = FSR_DENIED;
            goto exit;
        }

        __Efs_ChangeFileSize(fp, fp->file_size);
        //此处写入大小没有256，写入EF_WR_ECC只是为了底层更好做一点
        if(fp->buf_off !=
            efs->write_data(block,offset,fp->wr_buf,fp->buf_off,EF_WR_ECC))
        {
            __Efs_ChangeFileSize(fp, fileinfo->filesize);   //写入失败，则更正filesize
            ret = FSR_DISK_ERR;
        }
        else
        {
            fileinfo->filesize = fp->ptr;
            fp->buf_off = 0;
        }
    }
exit:
    Lock_MutexPost(efs->block_buf_mutex);
    return ret;
}

// =============================================================================
// 功能：删除文件
// 参数：fname,文件名
//      fsname,文件系统名称
// 返回：FSRESULT，正确返回是FSR_OK
// =============================================================================
FSRESULT Efs_Remove(const char *fname,const char *fsname)
{
    tagEFS *efs;
    u32 block_no,block_offset;
    u8 cfg_blocks,loop,fixedFiles;
    FSRESULT ret = FSR_NO_FILE;

    efs = __Efs_Find(fsname);
    if(NULL == efs)
    {
    	printf("cannot fine root: \"%s\"", fsname);
    	return FSR_NO_FILESYSTEM;
    }

    if(false == Lock_MutexPend(efs->block_buf_mutex,MUTEX_WAIT_TIME))
    {
       return FSR_DENIED;
    }

    cfg_blocks = (EFS_CFG_LIMIT+ efs->block_size-1) / efs->block_size;

    fixedFiles = EFS_CFG_LIMIT/EFS_ITEM_LIMIT;
    for (loop = 1; loop < fixedFiles; loop++)
    {
        block_no = loop * EFS_ITEM_LIMIT;
        block_offset = block_no % efs->block_size;
        block_no = efs->start_block + block_no / efs->block_size;
        efs->read_data(block_no, block_offset,
        		efs->block_buf,FILENAME_LIMIT+1+ECC_LIMIT,EF_WR_NOECC);
        efs->block_buf[FILENAME_LIMIT] = '\0';
        ChkOrRecNameByECC((char *)(efs->block_buf),efs->block_buf+FILENAME_LIMIT+1);
        if (strncmp((const char*)efs->block_buf,fname,FILENAME_LIMIT) == 0)
        {
            break;//找到该文件
        }
    }
#if 1
if(loop == fixedFiles)
    printf("cannot find :%s\r\n", fname);
#endif

	if(loop < fixedFiles)
	{
		//删除的文件信息，写全0
		efs->read_data(block_no, block_offset, efs->block_buf,
				EFS_ITEM_LIMIT,EF_WR_NOECC);
		memset(efs->block_buf,0x0,EFS_ITEM_LIMIT);
		efs->write_data(block_no, block_offset, efs->block_buf,
				EFS_ITEM_LIMIT,EF_WR_NOECC);
		efs->write_data(block_no+cfg_blocks, block_offset,
				efs->block_buf,EFS_ITEM_LIMIT,EF_WR_NOECC);
		efs->files_sum --;
#if 1
{
        u16 CountTemp; 
        efs->read_data(block_no, block_offset, efs->block_buf,
				EFS_ITEM_LIMIT, EF_WR_NOECC);
        for(CountTemp = 0; CountTemp < EFS_ITEM_LIMIT; CountTemp++)
        {
            if(efs->block_buf[CountTemp] != 0x0)
            {
                printf("\r\nremove error in \"%s\"\r\n", fname);
                printf("remove error file in blk 0x%x offset 0x%x\r\n", block_no, block_offset);
                break;
            } 
        }
        efs->read_data(block_no+cfg_blocks, block_offset, efs->block_buf,
				EFS_ITEM_LIMIT, EF_WR_NOECC);
        for(CountTemp = 0; CountTemp < EFS_ITEM_LIMIT; CountTemp++)
        {
            if(efs->block_buf[CountTemp] != 0x0)
            {
            	printf("\r\nremove error in \"%s\"\r\n", fname);
            	printf("remove error file in blk 0x%x offset 0x%x\r\n", block_no+cfg_blocks, block_offset);
                break;
            }    
        }
}           
#endif

        ret = FSR_OK;
    }

    Lock_MutexPost(efs->block_buf_mutex);
    return ret;
}

// =============================================================================
// 功能：seek函数，两个功能，在EFS中，对于刚新建的文件，该函数实现配置最大尺寸，另外的情况
//      该函数实现移动读写指针（不能实现动态扩大的功能）
// 参数：fp,文件指针
//      newpos,文件读写指针的位置
// 返回：FSRESULT，正确返回是FSR_OK
// =============================================================================
FSRESULT Efs_Seek(tagFileRsc* fp,u32 newpos)
{
    tagEfsFileInfo *fileinfo;
    tagEFS *efs;
    u32 block,offset,alignsize,fsizeoff;
    FSRESULT ret = FSR_OK;

    efs = fp->efs;
    fileinfo = (tagEfsFileInfo *)fp->private;

    if(newpos > fileinfo->max_size)
        return FSR_DENIED;

    if(false == Lock_MutexPend(efs->block_buf_mutex,MUTEX_WAIT_TIME))
    {
       return FSR_DENIED;
    }

    fsizeoff = fileinfo->filesize % CN_FILE_BUF_LEN;
    alignsize = fileinfo->filesize - fsizeoff;
    //向后seek > CN_FILE_BUF_LEN,则将wr_buf写下去
    if( (newpos - alignsize >= CN_FILE_BUF_LEN) && (newpos > alignsize) )
    {
        block = (fileinfo->filesize / efs->block_size) + fileinfo->start_block;
        offset = fileinfo->filesize % efs->block_size;
        if(!efs->query_block_ready(block,offset,fp->wr_buf,CN_FILE_BUF_LEN))
        {
            ret =  FSR_DENIED;
            goto exit;
        }

        if(fsizeoff)        //判断是否需要读出来再write
        {
            offset = alignsize % efs->block_size;
            memset(fp->wr_buf,0xFF,CN_FILE_BUF_LEN);
            efs->read_data(block,offset,fp->wr_buf,fsizeoff,EF_WR_NOECC);
        }

        __Efs_ChangeFileSize(fp, newpos - newpos % CN_FILE_BUF_LEN);//写入flash

        if(CN_FILE_BUF_LEN != efs->write_data(block,alignsize,fp->wr_buf,
                CN_FILE_BUF_LEN,EF_WR_ECC))
        {
            __Efs_ChangeFileSize(fp, fileinfo->filesize);
            ret = FSR_DISK_ERR;
        }
        else
        {
            fileinfo->filesize = newpos - newpos % CN_FILE_BUF_LEN;
            fp->file_size = newpos;
            fp->buf_off = 0;
            memset(fp->wr_buf,0xFF,CN_FILE_BUF_LEN);
        }
    }

    if(ret == FSR_OK)
    {
        fp->ptr = newpos;
    }
exit:
    Lock_MutexPost(efs->block_buf_mutex);
    return ret;
}

// =============================================================================
// 功能：写文件，如果超过文件的最大尺寸限制，则返回错误
// 参数：fp,文件指针
//      buf,待写入的数据缓冲区
//      len,写入数据的字节数
//      bw,实际写入的字节数，指针
// 返回：FSRESULT，正确返回是FSR_OK
// =============================================================================
FSRESULT Efs_Write (tagFileRsc* fp,const void *buf,u32 len,u32* bw)
{
    FSRESULT ret = FSR_OK;
    struct EfsFileInfo *fileinfo;
    struct EasyFS *efs;
    u32 block, offset_block;
    u32 write_sum, wr_point, write_len, completed = 0;
    u32 alignsize;

    *bw= 0;
    if ((fp == NULL) || (len == 0) || (buf == NULL))
        return FSR_INVALID_PARAMETER;

    if(fp->ptr < fp->file_size)
        return FSR_MKFS_ABORTED;

    //文件所在flash芯片指针
    efs = fp->efs;
    fileinfo = (struct EfsFileInfo *) fp->private;
    //如果超出了最大尺寸，则超出部分会被舍弃
    if (len <= (fileinfo->max_size - fp->ptr))
        write_sum = len;
    else
        write_sum = fileinfo->max_size - fp->ptr;
    wr_point = fp->ptr;

    //计算起始写位置处于文件内第几块和偏移量
    block = (fileinfo->filesize / efs->block_size) + fileinfo->start_block;
    offset_block = fileinfo->filesize % efs->block_size;

    if(false == Lock_MutexPend(efs->block_buf_mutex,MUTEX_WAIT_TIME))
    {
       return FSR_DENIED;
    }

    //判断是否改写
    if(!efs->query_block_ready(block,offset_block,(u8*)buf,fp->buf_off + len))
    {
        ret = FSR_DENIED;
        goto exit;
    }

    memset(fp->wr_buf + fp->buf_off,0xFF,CN_FILE_BUF_LIMIT - fp->buf_off);
    if( fileinfo->filesize % CN_FILE_BUF_LEN )//需先读出来,只有close,flush后才有
    {
        fp->buf_off = fileinfo->filesize % CN_FILE_BUF_LEN;
        efs->read_data(block,offset_block,fp->wr_buf,fp->buf_off,EF_WR_NOECC);
    }

    alignsize = fileinfo->filesize - fileinfo->filesize % CN_FILE_BUF_LEN;
    if(wr_point + write_sum >= alignsize + CN_FILE_BUF_LEN )//先写file_size
    {
        write_len = wr_point + write_sum;
        write_len = write_len - (write_len % CN_FILE_BUF_LEN);
        __Efs_ChangeFileSize(fp, write_len);    //将filesize写入flash
    }

    while(1)
    {
        fp->buf_off = wr_point%CN_FILE_BUF_LEN;
        write_len = CN_FILE_BUF_LEN - fp->buf_off;

        if(write_sum < write_len)
        {
            write_len = write_sum;
            memcpy(fp->wr_buf + fp->buf_off,buf+completed,write_len);
            fp->buf_off += write_len;
            break;
        }
        else//write the buf to flash
        {
            memcpy(fp->wr_buf + fp->buf_off,buf+completed,write_len);
            block = (wr_point / efs->block_size) + fileinfo->start_block;
            offset_block = (wr_point % efs->block_size) - fp->buf_off;
            if(CN_FILE_BUF_LEN != efs->write_data(block,offset_block,
                    fp->wr_buf,CN_FILE_BUF_LEN,EF_WR_ECC))
            {
                write_len = 0;
                ret = FSR_DISK_ERR;
                break;
            }
            fp->buf_off = 0;
        }
        wr_point += write_len;
        write_sum -= write_len;
        completed += write_len;
    }

    fp->ptr += completed + write_len;
    fp->file_size = fp->ptr;
    fileinfo->filesize = fp->ptr - fp->ptr % CN_FILE_BUF_LEN;
    *bw = completed + write_len;

    if(ret != FSR_OK)
    {
        __Efs_ChangeFileSize(fp, fileinfo->filesize);
    }
exit:
    Lock_MutexPost(efs->block_buf_mutex);
    return ret;
}

// =============================================================================
// 功能：读 文件
// 参数：fp,文件指针
//      buf,读文件数据存储的缓冲区
//      len,读文件字节数
//      br,实际读取的数据长度
// 返回：FSRESULT，正确返回是FSR_OK
// =============================================================================
FSRESULT Efs_Read (tagFileRsc* fp,void* buf,u32 len,u32* br)
{
    struct EfsFileInfo *fileinfo;
    struct EasyFS *efs;
    u32 completed = 0;
    FSRESULT ret = FSR_OK;

    u32 rd_point,buf_off,copy_len,block,blk_off,readlen;
    u8 rDataBuf[CN_FILE_BUF_LIMIT];

    *br = 0;
    if ((fp == NULL) || (len == 0) || (buf == NULL))
    {
        printf("\r\nEfs_Read - FSR_INVALID_PARAMETER\r\n");
        return FSR_INVALID_PARAMETER;
    }

    //文件所在flash芯片指针
    efs = (struct EasyFS *)fp->efs;
    fileinfo = (struct EfsFileInfo *) fp->private;
    rd_point = fp->ptr;
    if(rd_point + len > fp->file_size)
        len = fp->file_size - rd_point;

    if(false == Lock_MutexPend(efs->block_buf_mutex,MUTEX_WAIT_TIME))
    {
        printf("\r\nEfs_Read - FSR_DENIED- locked\r\n");
        return FSR_DENIED;
    }
    while(len > 0)
    {
        block = rd_point / efs->block_size + fileinfo->start_block;
        blk_off = (u32) (rd_point % efs->block_size);

        buf_off = blk_off % CN_FILE_BUF_LEN;
        if(buf_off)
        {
            blk_off -= buf_off;
        }
        if(rd_point >= fp->file_size - fp->file_size % CN_FILE_BUF_LEN)
        {
            //只有close/flush时，ptr_buf不为0，才会导致file_size不对齐
//          eccflg = EF_WR_NOECC;
            readlen = fp->file_size - rd_point;
        }
        else
        {
//          eccflg = EF_WR_ECC;
            readlen = CN_FILE_BUF_LEN;
        }

        if(rd_point >= fileinfo->filesize)      //从fp->wr_buf读
        {
            memcpy(rDataBuf,fp->wr_buf, CN_FILE_BUF_LEN);
        }
        else
        {
            if(readlen != efs->read_data(block, blk_off,
                    rDataBuf, readlen,EF_WR_ECC))
            {
                printf("\r\nEfs_Read - FSR_DISK_ERR\r\n");
                ret = FSR_DISK_ERR ;
                break;  //读错误或ECC错误
            }
        }

        copy_len = CN_FILE_BUF_LEN - buf_off;
        copy_len = (copy_len > len) ? len : copy_len;
        memcpy(buf + completed,&rDataBuf[buf_off],copy_len);

        completed += copy_len;
        rd_point  += copy_len;
        len       -= copy_len;
    }

    fp->ptr += completed;
    *br = completed;
    Lock_MutexPost(efs->block_buf_mutex);
    return ret;
}
// =============================================================================
// 功能：计算文件名ECC
// 参数：
// 返回：
// =============================================================================
static void NameECC(char *Name, u8 *Ecc)
{
	memset(NameBuf, 0x00, 256);
	strcpy(NameBuf, Name);
	hamming_compute_256x((u8*)NameBuf, 256, Ecc);
}
// =============================================================================
// 功能：通过ECC修复文件名
// 参数：
// 返回：-1 -- ECC校验失败; 0 -- ECC校验成功;
// =============================================================================
static s32 ChkOrRecNameByECC(char *Name, u8 *Ecc)
{
	u32 Ret;

	if( ((Ecc[0] == 0) && (Ecc[1] == 0) && (Ecc[2] == 0)) ||
			((Ecc[0] == 0xFF) && (Ecc[1] == 0xFF) && (Ecc[2] == 0xFF))	)
		return 0;

	memset(NameBuf, 0x00, 256);
	strcpy(NameBuf, Name);
	Ret = hamming_verify_256x((u8*)NameBuf, 256, Ecc);
	if (Ret && (Ret != HAMMING_ERROR_SINGLE_BIT))
	{
		printf("\r\nfile name \"%s\" ecc error [0x%x]\r\n", Name, Ret);
		return (-1);
	}
	return (0);
}
