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
//文件描述:easy nor flash文件系统特有的shell命令
//其他说明:
//修订历史:
//2. ...
//1. 日期:
//   作者: 贺敏
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include <ctype.h>
#include "stdlib.h"
#include "string.h"
#include "lock.h"
#include "systime.h"
#include "file.h"
#include "easynorflash.h"
#include "stdio.h"
#include "shell.h"
#include "endian.h"

extern struct tagSemaphoreLCB *Fs_Semaphore; //保护fs树所用的信号量

bool_t Sh_CreatFile(char *param);
bool_t Sh_ShowFile(char* param);
bool_t Sh_DirEf(char *param);
struct tagShellCmdTab const filesize_cmd_table[] =
{
    {
        "createfile",
        Sh_CreatFile,
        "创建文件",
        "命令格式: createfile 文件名 文件最大字节"
    },
    {
        "file",
        Sh_ShowFile,
        "显示指定文件的文件大小和最大文件尺寸",
        "命令格式: file 文件名"
    },
    {
        "dir_ef",
        Sh_DirEf,
        "显示所有文件的文件大小和最大文件尺寸",
        "命令格式: dir_ef"
    },
};

struct tagGetFileInfo
{
    u32 file_size;
    u32 max_size;
    char name[CN_FILE_NAME_LIMIT + 1];
};


static struct tagShellCmdRsc tg_filesize_cmd_rsc
              [sizeof(filesize_cmd_table)/sizeof(struct tagShellCmdTab)];

//------添加简易文件系统shell命令----------------------------
//功能：添加简易文件系统的shell命令到系统添加
//参数：param: 字符串
//返回：true:  操作成功
//      false: 操作失败
//----------------------------------------------------------
ptu32_t Cmd_InstallEasynorFs(ptu32_t para)
{
    if(Sh_InstallCmd(filesize_cmd_table,tg_filesize_cmd_rsc,
            sizeof(filesize_cmd_table)/sizeof(struct tagShellCmdTab)))
        return true;
    else
        return false;
}

//------ymodem命令创建文件----------------------------------
//功能：创建文件的shell命令
//参数：param: 字符串
//返回：true:  操作成功
//      false: 操作失败
//----------------------------------------------------------
bool_t Sh_CreatFile(char *param)
{
    ptu32_t max_size;
    char *next_param;
    char *filename,*str_maxsize;
    FILE *new_file=NULL;

    if(param == NULL)
    {
        printf("命令语法不正确!\r\n");
        printf("正确的命令:createfile 文件名 文件最大字节\r\n");
        return false;
    }

    filename = Sh_GetWord(param,&next_param);
    str_maxsize  = Sh_GetWord(next_param,&next_param);

    if((filename == NULL) || (str_maxsize == NULL))
    {
        printf("命令语法不正确!\r\n");
        printf("正确的命令:createfile 文件名 文件最大字节\r\n");
        return false;
    }

    max_size = strtol(str_maxsize, (char **)NULL, 0);
    //若文件存在，则打开，若不存在，创建后打开
    new_file = fopen(filename,"w+b");
    if(new_file != NULL)
    {
        //创建后立即修改文件最大尺寸
        if(0 == Djyfs_Ftruncate(new_file,max_size))
        {
            printf("文件创建成功!\r\n");
            return true;
        }
    }
    printf("创建文件失败!\r\n");
    return false;
}

//----shell显示文件大小---------------------------------------------------
//功能：简易文件系统在创建文件时，需指定文件最大尺寸，此shell命令的服务函数
//      显示指定的文件的文件大小和最大文件尺寸
//参数：param，字符串参数
//返回：true=成功，false=失败
//-------------------------------------------------------------------------
bool_t Sh_ShowFile(char* param)
{
    struct tagRscNode *fs_rsc_node;
    struct tagPTTDevice *tagPTTDevice;
    struct tagEasynorFlashChip *chip;
    struct tagEasyFlashFileInfo fileinfo;
    bool_t found = false;

    char *disk;
    bool_t result = false;
    u32 block_no,offset,loop;
    u32 *hsize,len;

    char *filename,*next_param;
    if(param == NULL)
    {
        printf("命令语法不正确!\r\n");
        printf("正确的命令:file 文件名\r\n");
        return false;
    }

    filename = Sh_GetWord(param,&next_param);
    
    //获取当前盘符
    len = Djyfs_GetCwdLen( );
    disk = M_MallocLc(len,0);
    if(disk == NULL)
        return false;
    Djyfs_GetCwd(disk,len);
    
    for(loop = 0; loop <len || loop < 256; loop++)
    {
    	if(disk[loop] == ':')
    	{
    		disk[loop] = '\0';//添加磁盘符字符串结束符
    		break;
    	}
    }
    if((loop == len) || (loop == 256))
    {
    	printf("磁盘符错误！\r\n");
    	return false;
    }

    Lock_SempPend(Fs_Semaphore,CN_TIMEOUT_FOREVER);
    fs_rsc_node = Rsc_SearchTree("fs");    //取文件系统设备的资源结点指针
    if(fs_rsc_node == NULL)
    {
        Lock_SempPost(Fs_Semaphore);
        printf("文件系统未建立!\r\n");
        return false;
    }
    
    //在分区设备中查名为PTT_name的结点
    tagPTTDevice = (struct tagPTTDevice *)
            Rsc_SearchSon(fs_rsc_node,disk);
    if(NULL != tagPTTDevice)
    {	
		chip = (struct tagEasynorFlashChip *)tagPTTDevice->PTT_medium_tag;
	
		for (loop = 1; loop <= chip->files_sum; loop++)
		{
			block_no = (loop * CN_EASYNOR_CFG_ITEM_LIMIT)/chip->block_size;
			block_no = chip->start_block + block_no;
			offset = (loop * CN_EASYNOR_CFG_ITEM_LIMIT) % chip->block_size;
	
			chip->read_data(block_no, offset, chip->block_buf,
					CN_EASYNOR_PTT_NAME_LIMIT);
			chip->block_buf[CN_EASYNOR_PTT_NAME_LIMIT] = '\0';
			if (strncmp((const char*)chip->block_buf,(const char*)filename,
					CN_EASYNOR_PTT_NAME_LIMIT) == 0)
			{
				chip->read_data(block_no, offset, chip->block_buf,
						CN_EASYNOR_CFG_ITEM_LIMIT);
				found = true;
				fileinfo.start_item = loop;
				fileinfo.max_size = pick_little_32bit(chip->block_buf,
					CN_EASYNOR_MAXSIZE_OFF >> 2);
				break;
			}
		}
	
		if(!found)
		{
			printf("指定的文件名错误或不存在\r\n");
			return false;
		}
	
		//读出该文件的实时尺寸和最大尺寸
		for (loop = 0; loop < CN_EASYNOR_FILESIZE_NUM; loop++)
		{
			hsize =(u32*) (chip->block_buf + CN_EASYNOR_FILESIZE_END - loop * 8);
			if ((*(u64*) hsize == CN_LIMIT_UINT64) || (*(u64*) hsize == 0))
				continue;
			if (*hsize == ~(*(hsize + 1)))
			{
				fileinfo.filesize = pick_little_32bit(chip->block_buf,
						(CN_EASYNOR_FILESIZE_END - loop * 8) >> 2);
				break;
			}
		}
	
		//打印信息
		printf("\r\n文件名                          文件大小(byte)      文件最大尺寸(byte)\r\n");
		if(loop != CN_EASYNOR_FILESIZE_NUM)
		{
			printf("%-32s0x%08x          0x%08x\r\n",
						  filename,fileinfo.filesize,fileinfo.max_size);
		}
		else
			printf("查找文件大小失败\r\n");
		
		result = true;
    }
    else
    {
    	printf("未找到磁盘!\r\n");
    	result = false;
    }
    
	Lock_SempPost(Fs_Semaphore);
	free(disk);
    return result;
}

//----shell显示文件大小---------------------------------------------------
//功能：从文件系统中读出每一个文件的文件名和文件大小以及文件最大尺寸
//参数：param，字符串参数
//返回：true=成功，false=失败
//-------------------------------------------------------------------------
u32 __Easynor_ReadFileSize(struct tagPTTDevice *tagPTTDevice,
                           uint32_t file_num,
                           struct tagGetFileInfo *file)
{
    u32 block_no,offset,loop;
    u8 *hsize;
    struct tagEasynorFlashChip *chip;

    chip = (struct tagEasynorFlashChip *) tagPTTDevice->PTT_medium_tag;

    //计算该文件分配表在所在的block和offset
    block_no = (file_num * CN_EASYNOR_CFG_ITEM_LIMIT)/chip->block_size;
    block_no = block_no + chip->start_block;
    offset = (file_num * CN_EASYNOR_CFG_ITEM_LIMIT)%chip->block_size;
    //配置文件信息
    chip->read_data(block_no, offset,chip->block_buf,CN_EASYNOR_CFG_ITEM_LIMIT);

    //文件大小
    for (loop = 0; loop < CN_EASYNOR_FILESIZE_NUM; loop++)
    {
        hsize = (u8*)(chip->block_buf + CN_EASYNOR_FILESIZE_END - loop * 8);
        if ((pick_little_64bit(hsize,0) == CN_LIMIT_UINT64) ||
            (pick_little_64bit(hsize,0) == 0))
            continue;
        if (pick_little_32bit(hsize,0) == ~(pick_little_32bit(hsize + 4,0)))
        {
            file->file_size = pick_little_32bit(chip->block_buf,
                    (CN_EASYNOR_FILESIZE_END - loop * 8) >> 2);
            break;
        }
    }
    if(loop == CN_EASYNOR_FILESIZE_NUM)//没有得到正确的文件大小
        return false;

    file->max_size = pick_little_32bit(chip->block_buf,
                  CN_EASYNOR_MAXSIZE_OFF >> 2);
    loop = 0;
    while((chip->block_buf[loop]) && (loop++ < CN_EASYNOR_FILENAME_LIMIT));
    if(loop >= CN_EASYNOR_FILENAME_LIMIT)//文件名错误
        return false;
    strncpy((char *)file->name, (char *)chip->block_buf, CN_EASYNOR_FILENAME_LIMIT);
    return true;
}

//----shell显示文件大小---------------------------------------------------
//功能：简易文件系统在创建文件时，需指定文件最大尺寸，此shell命令的服务函数
//      显示所有文件的文件大小和最大文件尺寸
//参数：param，字符串参数
//返回：true=成功，false=失败
//-------------------------------------------------------------------------
bool_t Sh_DirEf(char *param)
{
    struct tagPTTDevice *tagPTTDevice;
    struct tagEasynorFlashChip *chip;
    struct tagRscNode *fs_rsc_node;
    u32 loop,len;
    char *disk;
    bool_t result = false;
    struct tagGetFileInfo fileinfo;

    //获取当前盘符
    len = Djyfs_GetCwdLen( );
    disk = M_MallocLc(len,0);
    if(disk == NULL)
        return false;
    Djyfs_GetCwd(disk,len);
    
    for(loop = 0; loop <len || loop < 256; loop++)
    {
    	if(disk[loop] == ':')
    	{
    		disk[loop] = '\0';//添加磁盘符字符串结束符
    		break;
    	}
    }
    if((loop == len) || (loop == 256))
    {
    	printf("磁盘符错误！\r\n");
    	return false;
    }
    
    Lock_SempPend(Fs_Semaphore,CN_TIMEOUT_FOREVER);
    fs_rsc_node = Rsc_SearchTree("fs");    //取文件系统设备的资源结点指针
    if(fs_rsc_node == NULL)
    {
        Lock_SempPost(Fs_Semaphore);
        printf("文件系统未建立!\r\n");
        return false;
    }

    //在分区设备中查名为MDR_PTT->PTT_name的结点
    tagPTTDevice = (struct tagPTTDevice *)Rsc_SearchSon(fs_rsc_node,disk);
    if(NULL != tagPTTDevice)
    {
		chip = (struct tagEasynorFlashChip *)tagPTTDevice->PTT_medium_tag;
	
		printf("\r\n文件名                          文件大小(byte)      文件最大尺寸(byte)\r\n");
		for(loop = 1; loop <= chip->files_sum; loop++)
		{
			if(__Easynor_ReadFileSize(tagPTTDevice, loop, &fileinfo))
			{
				printf("%-32s0x%08x          0x%08x\r\n",
						  fileinfo.name,fileinfo.file_size,fileinfo.max_size);
			}
			else
				printf("查找文件信息出错!\r\n");
		}
		printf("总文件%d个文件\r\n",chip->files_sum);
		result = true;
    }
    else
    {
    	printf("未找到磁盘\r\n");
    	result = false;
    }

	Lock_SempPost(Fs_Semaphore);
    free(disk);
    return result;
}

