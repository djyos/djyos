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
//----------------------------------------------------
//Copyright (C), 2004-2009,  lst.
//版权所有 (C), 2004-2009,   lst.
//所属模块:flash文件系统
//作者：lst
//版本：V1.0.0
//文件描述:flash文件系统中MDR表部分
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "endian.h"
#include "lock.h"
#include "file.h"
#include "djyos.h"
#include "systime.h"
#include "flashfile.h"
#include "ddr.h"
#include "mdr.h"

bool_t __check_word(const char *word);

//----初始化flash芯片---------------------------------------------------------
//功能：把芯片标记为djyosfs芯片，但并不建立分区。
//参数: chip，被操作的芯片
//      reserves，保留块数量
//返回: true=初始化成功，false=失败
//---------------------------------------------------------------------------
void DFFSD_InitMDR(struct tagFlashChip *chip,uint32_t reserves)
{
    uint8_t MDR_CDR_buffer[CN_MDR_CDR_SIZE];
    struct tagMDR_CDR_Mirror *MDR_CDR = (struct tagMDR_CDR_Mirror *)MDR_CDR_buffer;
    uint32_t valid_blocks = 1;  //首块肯定是可用的，故初值=1
    uint32_t i,MDR_bak_block = 0;
    //注意，nandflash只有首块是保证有效的，故MDR表尺寸不能超过首块，这不成问题，
    //目前为止nandflash的block_size没有小于4K的。
    //而norflash则没有这个限制，norflash保证整片都是好的。
    for(i = 1; i < chip->block_sum; i++)
    {    //找一个好块作为MDR备用块
        if(chip->check_block(i))
        {
            valid_blocks++;
        }
    }
    MDR_bak_block = __DFFSD_Probe_MDR_Bak(chip);
    chip->valid_sum = valid_blocks;
    chip->rev_blocks = reserves;    //保留块数量
    //保留块起始块号
    chip->rev_start = MDR_bak_block
                      +(CN_MDR_SIZE + chip->block_size -1) / chip->block_size;
    chip->PTT_start = chip->rev_start + reserves;   //分区起始块号
    if(valid_blocks <= chip->PTT_start)
        return;     //有效块数太少
    chip->MDR.MDR_bak_block = MDR_bak_block;
    chip->MDR.roll_next_main = CN_ROLL_OFFSET;
    chip->MDR.roll_next_bak = CN_ROLL_OFFSET;
    for(i = 0; i < CN_PTT_SUM_MAX; i++)
        chip->MDR.PTT_created[i] = false;
    strcpy((char*)MDR_CDR_buffer,"djyosfs");
    fill_little_32bit(MDR_CDR->chip_CDR_size,0,CN_MDR_CDR_SIZE);
    fill_little_32bit(MDR_CDR->chip_block_sum,0,chip->block_sum);
    fill_little_32bit(MDR_CDR->chip_block_size,0,chip->block_size);
    fill_little_32bit(MDR_CDR->chip_MDR_bak,0,MDR_bak_block);
    fill_little_32bit(MDR_CDR->chip_reserve,0,reserves);
    __DFFSD_Fill_ECC_MDR_CDR(MDR_CDR_buffer);
    __DFFSD_EraseMDR_Main(chip);
    __DFFSD_WriteMDR_Main(chip,MDR_CDR_buffer,
                (uint32_t)MDR_CDR->chip_MDR_CDR_ECC-(uint32_t)MDR_CDR +4);

    __DFFSD_WriteEndFlagMain(chip);

    __DFFSD_EraseMDR_Bak(chip);
    __DFFSD_WriteMDR_Bak(chip,MDR_CDR_buffer,
                (uint32_t)MDR_CDR->chip_MDR_CDR_ECC-(uint32_t)MDR_CDR +4);
    __DFFSD_WriteEndFlagBak(chip);

}

//----读MDR_bak表-------------------------------------------------------------
//功能: 把MDR表读到缓冲区。
//参数: chip，目标芯片，chip->bak_start必须已经初始化
//      buf，保存所读数据的缓冲区
//返回: true=正确读取，false=错误。
//-----------------------------------------------------------------------------
bool_t __DFFSD_ReadMDR_Bak(struct tagFlashChip *chip,uint8_t *buf)
{
    uint32_t complete_len=0,read_len;
    uint32_t block_no = chip->MDR.MDR_bak_block;
    do  //本循环用于读取整个MDR_main到缓冲区，MDR_size可能大于1块，故循环。
    {
        if((CN_MDR_SIZE-complete_len) > chip->block_size)
            read_len = chip->block_size;
        else
            read_len = CN_MDR_SIZE-complete_len;
        //MDR表逐个记录单独校验，若整块校验，则每次增加记录都要改校验码，不可行
        chip->read_data_no_ecc(block_no,0,buf+complete_len,read_len);
        block_no++;
        complete_len += read_len;
    }while(complete_len < CN_MDR_SIZE);
    return true;
}

//----读MDR_main表-------------------------------------------------------------
//功能: 把MDR表读到缓冲区。
//参数: chip，目标芯片
//      buf，保存所读数据的缓冲区
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_ReadMDR_Main(struct tagFlashChip *chip,uint8_t *buf)
{
    uint32_t complete_len=0,read_len;
    uint32_t block_no = 0;      //MDR_main从0开始存储
    do  //本循环用于读取整个MDR_main到缓冲区，MDR_size可能大于1块，故循环。
    {
        if((CN_MDR_SIZE-complete_len) > chip->block_size)
            read_len = chip->block_size;
        else
            read_len = CN_MDR_SIZE-complete_len;
        //MDR表逐个记录单独校验，若整块校验，则每次增加记录都要改校验码，不可行
        chip->read_data_no_ecc(block_no,0,buf+complete_len,read_len);
        block_no++;
        complete_len += read_len;
    }while(complete_len < CN_MDR_SIZE);
    return;
}

//----分配一个存储介质---------------------------------------------------------
//功能: 建立新分区时，调用此函数从存储介质中分配存储器。
//参数: chip，目标芯片
//      size，待分配的块数。
//      result，返回分配结果
//返回: true=分配成功，结果保存在result中，false = 分配额失败
////---------------------------------------------------------------------------
bool_t __DFFSD_Allocate_PTT(struct tagFlashChip *chip,
                            uint32_t size,uint32_t *result)
{
    //uint8_t MDR_buf[CN_MDR_SIZE];
    //分区起始块数组，用于查询空闲空间，+2是为包含可能存在的首尾空闲区
    uint32_t block_start[CN_PTT_SUM_MAX+2];
    //分区结束块数组，用于查询空闲空间，+2是为包含可能存在的首尾空闲区
    uint32_t block_end[CN_PTT_SUM_MAX+2];
    struct tagMDR_PTT_Mirror *MDR_PTT;
    uint32_t loop,loop1=0,temp;
    uint8_t *MDR_buf;

    MDR_buf = (uint8_t*)M_MallocLc(CN_MDR_SIZE, 0);
    if(MDR_buf == NULL)
        return false;
    __DFFSD_ReadMDR_Main(chip,MDR_buf); //读出MDR到缓冲区(原样读出，不解析)
    block_start[0] = chip->PTT_start;
    block_end[0] = chip->PTT_start;
    //MDR_PTT指针指向MDR缓冲区中第一个PTT区，用于检查各PTT区是否空闲
    MDR_PTT = (struct tagMDR_PTT_Mirror *)(MDR_buf+CN_MDR_CDR_SIZE);

    for(loop = 1; loop < CN_PTT_SUM_MAX+1; loop++)
    {//读出各已经建立的分区的起始块号和结束块号，保存到block_start和block_end
    //两个数组里，数组元素比分区数多两个，是用于保存分区首尾空闲区间的
        if(chip->MDR.PTT_created[loop-1])
        {//对已经建立的分区，从它的MDR_PTT缓存中读出起始和结束块号

            block_start[loop] = pick_little_32bit(MDR_PTT->PTT_start_block,0);
            //block_start[loop] = block_end[loop-1]
            //                + pick_little_32bit(MDR_PTT->PTT_blocks_sum,0);
            //结束块号，该块已经不属于本分区了。
            block_end[loop] = block_start[loop]
                            + pick_little_32bit(MDR_PTT->PTT_blocks_sum,0);
        }else
        {//为建立的分区，起始和结束块号均初始化为最近发现的已建立的分区的
        //起始和结束块号，等效于本分区是空的。
            block_start[loop] = block_end[loop-1];
            block_end[loop] = block_end[loop-1];
        }
        MDR_PTT++;  //缓冲区指向下一个分区的MDR_PTT
    }

    block_start[CN_PTT_SUM_MAX+1] = chip->block_sum;//芯片尾部空闲区间的起始块号
    block_end[CN_PTT_SUM_MAX+1] = chip->block_sum;  //芯片尾部空闲区间的结束块号
    for(loop = 0; loop <CN_PTT_SUM_MAX+1; loop++)
    {//用冒泡法对各分区的起始块号排队(不是空闲块尺寸)，芯片反复使用后可能发生
    //交错，各分区所占用的存储器位置并不与其PTT在MDR中的顺序相同，只有按块地址
    //排序后，才能有效确定各可用空间的尺寸。
        for(loop1 = loop+1; loop1 < CN_PTT_SUM_MAX+2; loop1++)
        {
            if(block_start[loop] > block_start[loop1])
            {
                temp = block_start[loop];
                block_start[loop] = block_start[loop1];
                block_start[loop1] = temp;
                temp = block_end[loop];
                block_end[loop] = block_end[loop1];
                block_end[loop1] = temp;
            }
        }
    }
    //找一个满足要求的连续空闲空间
    for(loop = 0; loop < CN_PTT_SUM_MAX+1; loop++)
    {

        if(!chip->MDR.PTT_created[loop])
        {
            if(block_start[loop+1] - block_end[loop] >= size)
            {
                *result = block_start[loop];  //找到满足要求的空间
                    break;
               }
        }
    }
    free(MDR_buf);
    if(loop == CN_PTT_SUM_MAX +1)
        return false;   //没有找到满足要求的连续空闲区域
    else
        return true;
}

//----写入一个MDR_PTT块--------------------------------------------------------
//功能: 把一个MDR_PTT块写入flash中，需同时写入MDR_main和MDR_bak中
//参数: chip，目标芯片
//      PTT_no，分区号
//      MDR_PTT_buf，已经填充好的缓冲区
//返回: true=成功，false=失败
//注：加入写入是否正确的校验功能----db
//-----------------------------------------------------------------------------
bool_t __DFFSD_write_MDR_PTT(struct tagFlashChip *chip,
                           uint32_t PTT_no,uint8_t *MDR_PTT_buf)
{
    struct tagMDR_TempRecord MDR_record;
    uint32_t block_offset,block_no;
    uint8_t *MDR_buf;

    MDR_buf = (uint8_t*)M_MallocLc(CN_MDR_SIZE, 0);
    if(MDR_buf == NULL)
        return false;
    //新MDR_PTT在MDR表中的偏移量(字节)
    block_offset = CN_MDR_CDR_SIZE + PTT_no * CN_MDR_PTT_SIZE;
    block_no = block_offset / chip->block_size;//新MDR_PTT在MDR表中的块号
    block_offset = block_offset % chip->block_size; //块内偏移量
    //检查MDR_main是否需要擦除
    if(chip->query_block_ready_no_ecc(
                            block_no,block_offset,MDR_PTT_buf,CN_MDR_PTT_SIZE))
    {//MDR_main中该PTT块无需擦除
        __DFFSD_WriteMDR_PTT_Main(chip,MDR_PTT_buf,PTT_no);  //直接写入MDR_main
        //下面把新MDR_PTT写入到MDR_bak中
        block_no += chip->MDR.MDR_bak_block;    //取MDR_bak块号
        //检查MDR_bak是否需要擦除
        if(chip->query_block_ready_no_ecc(
                            block_no,block_offset,MDR_PTT_buf,CN_MDR_PTT_SIZE))
        {//MDR_bak中该PTT块无需擦除
            __DFFSD_WriteMDR_PTT_Bak(chip,MDR_PTT_buf,PTT_no);
        }else
        {//MDR_bak中该PTT块需要擦除
            __DFFSD_ReadMDR_Main(chip,MDR_buf); //读完整MDR表
            __DFFSD_UasmPTT(chip,&MDR_record,MDR_buf);
            //从缓冲区中取出滚动区的有效数据，保存到MDR_record中，因新分区
            //尚未完成初始化，滚动区中没有该分区的数据。
            __DFFSD_UasmRoll(chip,MDR_buf,
                              &MDR_record,chip->MDR.roll_next_main);
            memcpy(MDR_buf+CN_MDR_CDR_SIZE + PTT_no * CN_MDR_PTT_SIZE,
                        MDR_PTT_buf,CN_MDR_PTT_SIZE);
            __DFFSD_EraseMDR_Bak(chip);
            __DFFSD_WriteMDR_Bak(chip,MDR_buf,CN_MDR_AND_PTT_SIZE);
            __DFFSD_WriteRollBak(chip,&MDR_record);
            __DFFSD_WriteEndFlagBak(chip);
        }
    }
    else
    {//MDR_main中该PTT块需要擦除
        __DFFSD_ReadMDR_Main(chip,MDR_buf);
        __DFFSD_UasmPTT(chip,&MDR_record,MDR_buf);
        __DFFSD_UasmRoll(chip,MDR_buf,&MDR_record,chip->MDR.roll_next_main);
        memcpy(MDR_buf+CN_MDR_CDR_SIZE + PTT_no * CN_MDR_PTT_SIZE,
                            MDR_PTT_buf,CN_MDR_PTT_SIZE);
        __DFFSD_EraseMDR_Bak(chip);
        __DFFSD_WriteMDR_Bak(chip,MDR_buf,CN_MDR_AND_PTT_SIZE);
        __DFFSD_WriteRollBak(chip,&MDR_record);
        __DFFSD_WriteEndFlagBak(chip);

        __DFFSD_EraseMDR_Main(chip);
        __DFFSD_WriteMDR_Main(chip,MDR_buf,CN_MDR_AND_PTT_SIZE);
        __DFFSD_WriteRollMain(chip,&MDR_record);
        __DFFSD_WriteEndFlagMain(chip);
    }
    free(MDR_buf);
    return true;
}
//----建立一个分区----------------------------------------------------------
//功能: 在指定芯片上增加一个分区,新分区只是登记在芯片上，并未加载至操作系统，
//      若要加载，必须重新启动系统。
//参数: chip，被操作的芯片
//      size，新增加的分区尺寸(用块数表示)
//      name，新增加的分区名字
//      nand_ecc，本分区是否需要ecc校验
//返回: true = 成功，false = 失败，失败原因一般是存储空间不够。
//------------
//更新记录:
// 1.日期: 2015/3/10
//   说明: 增加分区名合法性检查。
//   作者: 季兆林
// 2.日期: 2015/3/17
//   说明: 1.修正对MDR_PTT->PTT_name长度判定。
//         2.修正PTT_device_tag互斥量释放的bug。
//         3.调整__DFFSD_write_MDR_PTT写入次序。
//         4.删除重复赋值变量。
//         5.增加对MDR_PTT_buf[]的初始化
//   作者: 季兆林
//-----------------------------------------------------------------------------
bool_t DFFSD_Create_PTT(struct tagFlashChip *chip, uint32_t size,char *name)
{
    struct tagStPTT_Flash *PTT_flash_tag = NULL;
    struct tagPTTDevice *PTT_device_tag = NULL;
    //djy_handle_t fs_handle_right = NULL;
    struct tagMDR_PTT_Mirror *MDR_PTT;
    struct tagRscNode *fs_rsc_node;
    uint32_t PTT_start,PTT_no,name_len,loop,table_size,DDR_blocks;
    uint8_t MDR_PTT_buf[CN_MDR_PTT_SIZE]={0};

    if((chip == NULL) || (size == 0) || (name == NULL))
        return false;
    name_len = strnlen(name,CN_FLASH_PTT_NAME_LIMIT+1);  //检测字符串长度，
    if(name_len == 0)               //分区名长度为0，非法
        return false;
    if(!__check_word(name)) //分区名合法性检查
        return false;
    for(loop = 0; loop < CN_PTT_SUM_MAX; loop++)//检查是否还可以建立新的分区
    {
        if(!chip->MDR.PTT_created[loop])
        {
            PTT_no = loop;      //第loop个分区未建立，其MDR_PTT区块是空的
            break;              //为新分区找到MDR_PTT块后退出循环
        }
    }
    if(loop == CN_PTT_SUM_MAX)
        return false;   //MDR中已经没有空闲的PTT块

    //以下初始化新分区的MDR_PTT缓存
    MDR_PTT = (struct tagMDR_PTT_Mirror*)MDR_PTT_buf;

    if(name_len <= CN_FLASH_PTT_NAME_LIMIT)     //copy分区名
    {
        strcpy(MDR_PTT->PTT_name,name);
    }
    else
    {
        memcpy(MDR_PTT->PTT_name,name,CN_FLASH_PTT_NAME_LIMIT);
        MDR_PTT->PTT_name[CN_FLASH_PTT_NAME_LIMIT] = '\0';
    }
    Lock_SempPend(Fs_Semaphore,CN_TIMEOUT_FOREVER);
    fs_rsc_node = Rsc_SearchTree("fs"); //取文件系统设备的资源节点指针
    if(fs_rsc_node == NULL)
    {
        Lock_SempPost(Fs_Semaphore);
        return false;
    }
    //在分区设备中查名为MDR_PTT->PTT_name的节点
    if(Rsc_SearchSon(fs_rsc_node,MDR_PTT->PTT_name))
    {
        Lock_SempPost(Fs_Semaphore);
        Djy_SaveLastError(EN_FS_PTT_NAME_REPEAT);
        printf("分区名[%s]重名.\n\r", MDR_PTT->PTT_name);
        return false;
    }
    Lock_SempPost(Fs_Semaphore);
    if( ! __DFFSD_Allocate_PTT(chip, size, &PTT_start)) //分配存储空间
        return false;
    strcpy(MDR_PTT->PTT_format_flag, "unready"); //格式化标志
    fill_little_32bit(MDR_PTT->PTT_start_block, 0, PTT_start); //起始块号
    fill_little_32bit(MDR_PTT->PTT_blocks_sum, 0, size); //分区尺寸(块数)
    __DFFSD_Fill_ECC_MDR_PTT((uint8_t *)MDR_PTT); //填充ECC码

    //分配分区设备专用数据结构内存，该结构将嵌入到反设备结构中
    PTT_device_tag = (struct tagPTTDevice*)
                M_Malloc(sizeof(struct tagPTTDevice),0);
    if(PTT_device_tag == NULL)
        goto goto_exit_install_chip;
    //分配flash文件系统专用数据结构内存，该结构将嵌入到分区设备中
    PTT_flash_tag = (struct tagStPTT_Flash*)
                M_Malloc(sizeof(struct tagStPTT_Flash),0);
    if(PTT_flash_tag == NULL)
        goto goto_exit_install_chip;
    PTT_flash_tag->ART_position = NULL;
    PTT_flash_tag->ART_block_no = NULL;
    PTT_flash_tag->ART_times = NULL;
    PTT_flash_tag->writed_DDR_main = NULL;
    PTT_flash_tag->writed_DDR_bak = NULL;
    PTT_flash_tag->chip = chip;         //flash分区存储器所属芯片
    PTT_flash_tag->PTT_no = PTT_no;     //本分区在芯片中的序号
    PTT_flash_tag->PCRB_no = CN_LIMIT_UINT32;   //未格式化
    PTT_flash_tag->DDR_main = CN_LIMIT_UINT32;  //未格式化
    PTT_flash_tag->DDR_bak = CN_LIMIT_UINT32;   //未格式化
    PTT_flash_tag->start_block = PTT_start;
    PTT_flash_tag->block_sum = size;

    table_size = PTT_flash_tag->block_sum * (4+4+2);
    //为磨损位置表分配内存
    PTT_flash_tag->ART_position = (uint32_t*)M_Malloc(table_size,0);
    if(PTT_flash_tag->ART_position == NULL)
        goto goto_exit_install_chip;
    //为磨损块号表分配内存
    PTT_flash_tag->ART_block_no = PTT_flash_tag->ART_position
                                  + PTT_flash_tag->block_sum;
    //为磨损次数表分配内存
    PTT_flash_tag->ART_times = (uint16_t*)(PTT_flash_tag->ART_block_no
                                  + PTT_flash_tag->block_sum);

    //计算DDR表尺寸
    PTT_flash_tag->DDR_size = __DFFSD_CalculateDDR_Size(PTT_flash_tag);
    //1份DDR表占用的块数
    DDR_blocks = (PTT_flash_tag->DDR_size
                        +chip->block_size-1)/chip->block_size;
    //申请两份DBL表所需内存
    PTT_flash_tag->DDR_DBL =(uint32_t*)M_Malloc(DDR_blocks*8,0);
    if(PTT_flash_tag->DDR_DBL == NULL)
        goto goto_exit_install_chip;
    //申请MAT表所需内存
    PTT_flash_tag->DDR_MAT = (struct tagMAT_Table *)M_Malloc(
                        PTT_flash_tag->block_sum*sizeof(struct tagMAT_Table),0);
    if(PTT_flash_tag->DDR_MAT == NULL)
        goto goto_exit_install_chip;
    //两份写入标志
    PTT_flash_tag->writed_DDR_main = (bool_t*)M_Malloc(
                                        DDR_blocks*sizeof(bool_t)*2,0);
    if(PTT_flash_tag->writed_DDR_main == NULL)
        goto goto_exit_install_chip;
    PTT_flash_tag->writed_DDR_bak=PTT_flash_tag->writed_DDR_main+DDR_blocks;
//    for(loop = 0; loop < DDR_blocks; loop++)
//    {
//        PTT_flash_tag->writed_DDR_main[loop] = false;
//        PTT_flash_tag->writed_DDR_bak[loop] = false;
//    }

//    __DFFSD_WriteDDR(PTT_flash_tag);
    //填充函数指针和分区名
    __DFFSD_Fill_FDT_Device(PTT_device_tag,name);
    //分区设备的存储媒体指向flash分区存储器
    PTT_device_tag->PTT_medium_tag = (ptu32_t)PTT_flash_tag;
    PTT_device_tag->formatted =false;
    PTT_device_tag->chip_mutex_tag = chip->chip_mutex;

    //考虑到内存数据容易处理,最后才将分区信息写入到FLASH;(逻辑未完善，需增加释放节点逻辑)
    if( ! __DFFSD_write_MDR_PTT(chip, PTT_no, MDR_PTT_buf))
        goto goto_exit_install_chip;

    Lock_MutexPend(PTT_device_tag->chip_mutex_tag,CN_TIMEOUT_FOREVER);
    if(EN_FS_NO_ERROR !=  Djyfs_FsDriverCtrl(enum_fs_add_PTT,
                                             (ptu32_t)PTT_device_tag,0))
    {
        goto goto_exit_install_chip;
    }
    Lock_MutexPost(PTT_device_tag->chip_mutex_tag);

    //这是正常出口
    chip->MDR.PTT_created[PTT_no] = true; //分区创建成功
    return true;

goto_exit_install_chip:
    if(PTT_flash_tag !=NULL)
    {
        if(PTT_flash_tag->writed_DDR_main != NULL)
            free(PTT_flash_tag->writed_DDR_main);
        if(PTT_flash_tag->DDR_MAT != NULL)
            free(PTT_flash_tag->DDR_MAT);
        if(PTT_flash_tag->DDR_DBL != NULL)
            free(PTT_flash_tag->DDR_DBL);
        if(PTT_flash_tag->ART_position != NULL)
            free(PTT_flash_tag->ART_position);
        free(PTT_flash_tag);
    }
    if(PTT_device_tag !=NULL)
        free(PTT_device_tag);
    return false; //分区加入到设备树失败
}

//----查找MDR_bak的块号--------------------------------------------------------
//功能: 在读MDR_main失败，无法从中获取MDR_bak的块号时，根据芯片的块尺寸计算
//      MDR_bak可能的存放位置。当用本函数确定块号时，应该从该块号开始连续查找
//      5块，直到找到合法的MDR_bak块为止，如果5块都未能找到合法的MDR_bak，则认为
//      本flash没有按照djyosfs初始化过。
//参数: chip，目标芯片
//返回: 块号，0表示没有找到
//-----------------------------------------------------------------------------
uint32_t __DFFSD_Probe_MDR_Bak(struct tagFlashChip *chip)
{
    return (CN_MDR_SIZE+chip->block_size-1) / chip->block_size;
}
//----校验MDR_CDR--------------------------------------------------------------
//功能: 校验MDR_CDR，并改正的数据，如果不能改正，返回错误
//参数: buf，保存有未校验的MDR_CDR数据的缓冲区
//返回: true=无错或者被修正，false=有错并且不能改正。
//-----------------------------------------------------------------------------
bool_t __DFFSD_Verify_MDR_CDR(uint8_t *buf)
{
    return true;
}
//----校验MDR_PTT--------------------------------------------------------------
//功能: 校验MDR_PTT，并改正的数据，如果不能改正，返回错误
//参数: buf，保存有未校验的MDR_PTT数据的缓冲区
//返回: true=无错或者被修正，false=有错并且不能改正。
//-----------------------------------------------------------------------------
bool_t __DFFSD_verify_MDR_PTT(uint8_t *buf)
{
    return true;
}

//----校验MDR结束标志----------------------------------------------------------
//功能: 1.校验MDR表的最后4个字节是否AA5555AA,最多只允许一位不符。
//      2.校验CDR和PTT表的ECC
//参数: buf，字节缓冲区
//返回: true=校验正确，false=错误
//-----------------------------------------------------------------------------
//特别注意，对齐问题/大小端问题/最小寻址单元大于8位问题，这些问题使得不能把
//AA5555AA组成32位整数来比较
bool_t __DFFSD_Verify_EndFlag(uint8_t *buf)
{
    uint8_t temp[4] = {0xAA,0x55,0x55,0xAA};
    uint8_t xor;
    ufast_t loop4,loop8,error_bit = 0;
    for(loop4 = 0; loop4 < 4; loop4++)
    {
        if(buf[loop4] != temp[loop4])
        {
            xor = buf[loop4] ^ temp[loop4];
            for(loop8 = 0; loop8 < 8; loop8++)
            {
                if(xor & (1<<loop8))
                    error_bit ++;
                if(error_bit > 1)
                    return false;
            }
        }else
            continue;
    }
    if(! __DFFSD_Verify_MDR_CDR(buf))
        return false;
    if(! __DFFSD_verify_MDR_PTT(buf))
        return false;
    return true;
}

//----MDR滚动记录ECC校验-------------------------------------------------------
//功能: MDR中滚动存储的记录进行ECC校验，并改正数据。
//参数: buf，记录指针
//返回: 0=无错，1=有错但被修正，2=无法修正
//-----------------------------------------------------------------------------
uint32_t __DFFSD_Verify_MDR_Record(uint8_t *buf)
{
    return 0;
}

//----扫描滚动存储区-----------------------------------------------------------
//功能: 扫描滚动存储缓冲区，读取已经格式化的分区的DDR_main块号、DDR_bak块号、
//      PCRB块号。
//参数: chip，被操作的芯片
//      MDR_buf，缓冲区
//      MDR_record，返回扫描到的数据的指针
//      end_offset，最后一条有效记录位置
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_UasmRoll(struct tagFlashChip *chip,uint8_t *MDR_buf,
                       struct tagMDR_TempRecord *MDR_record,uint32_t end_offset)
{
    uint8_t *record;
    uint32_t loop,loop3,item_sum=0;
    bool_t got_DDR_main[CN_PTT_SUM_MAX];//各分区DDR_main块号是否找到标志
    bool_t got_DDR_bak[CN_PTT_SUM_MAX]; //各分区DDR_bak块号是否找到标志
    bool_t got_PCRB[CN_PTT_SUM_MAX];    //各分区PCRB块号是否找到标志

    for(loop3 = 0; loop3 < CN_PTT_SUM_MAX; loop3++)
    {
        got_DDR_main[loop3] = false;
        got_DDR_bak[loop3] = false;
        got_PCRB[loop3] = false;
        if(MDR_record->formatted[loop3])
        {
            item_sum +=3;   //每个已经格式化的分区都有3条滚动记录
        }
    }
    //循环扫描所有滚动存储区的有效条目。
    for(loop = end_offset-8;loop >= CN_ROLL_OFFSET; loop-=8)
    {
        record = MDR_buf + loop;
        for(loop3 = 0; loop3 < CN_PTT_SUM_MAX; loop3++)
        {
            if(! MDR_record->formatted)     //分区未格式化
                continue;
            __DFFSD_Verify_MDR_Record(record); //校验该记录
            if(!got_DDR_main[loop3])        //DDR_main滚动记录还未找到
            {
                //检查本项目是不是第loop个分区的DDR_main块号
                if((record[0] == CN_MDR_DDR_MAIN) && (record[1] == loop3))
                {
                    item_sum--;                 //找到，剩余项目数减量
                    got_DDR_main[loop3] = true;  //标记该DDR_main块号项已找到
                    //将结果写入返回的结构指针中
                    MDR_record->DDR_main_block_no[loop3]
                            = pick_little_32bit(&record[2],0);
                    break;
                }
            }
            if(!got_DDR_bak[loop3])         //DDR_bak滚动记录还未找到
            {
                //检查本项目是不是第loop个分区的DDR_bak块号
                if((record[0] == CN_MDR_DDR_BAK) && (record[1] == loop3))
                {
                    item_sum--;                 //找到，剩余项目数减量
                    got_DDR_bak[loop3] = true;  //标记该DDR_bak块号项已找到
                    //将结果写入返回的结构指针中
                    MDR_record->DDR_bak_block_no[loop3]
                                    = pick_little_32bit(&record[2],0);
                    break;
                }
            }
            if(!got_PCRB[loop3])                //PCRB滚动记录还未找到
            {
                //检查本项目是不是第loop个分区的PCRB块号
                if((record[0] == CN_MDR_PCRB) && (record[1] == loop3))
                {
                    item_sum--;                 //找到，剩余项目数减量
                    got_PCRB[loop3] = true;  //标记该PCRB块号项已找到
                    //将结果写入返回的结构指针中
                    MDR_record->PCRB_block_no[loop3] =
                                pick_little_32bit(&record[2],0);
                    break;
                }
            }
        }
        if(item_sum == 0)   //剩余项目为0，表明需要找的项目已经全部找到
            break;
    }
}

//----检查MDR_bak的完整性------------------------------------------------------
//功能: 检查MDR_bak是否完整，是否因上次掉电时正在写入而导致不完整，方法是检查
//      尾部的AA5555AA，以及最后一条滚动记录的校验码
//参数: chip，目标芯片
//返回: true=完整，false=不完整
//-----------------------------------------------------------------------------
bool_t __DFFSD_CheckMDR_Bak(struct tagFlashChip *chip)
{
    uint8_t *MDR_buf;

    MDR_buf = (uint8_t*)M_MallocLc(CN_MDR_SIZE, 0);
    if(MDR_buf == NULL)
        return false;
    __DFFSD_ReadMDR_Bak(chip,MDR_buf);
    if(!__DFFSD_Verify_EndFlag(MDR_buf))
    {
        free(MDR_buf);
        return false;
    }
    free(MDR_buf);
    return true ;
}

//----检测滚动区结束位置-------------------------------------------------------
//功能: 检测滚动存储区第一个空闲记录的偏移量，用折半查找方法
//参数: chip，目标芯片
//      MDR_buf，MDR表缓冲区
//返回: 偏移量
//-----------------------------------------------------------------------------
uint32_t __DFFSD_FindRollNext(struct tagFlashChip *chip,uint8_t *MDR_buf)
{
    uint32_t busy_endian,free_endian,middle,result;
    uint32_t block_offset;
    busy_endian = CN_ROLL_OFFSET/8;
    free_endian = CN_MDR_SIZE /8 -1;   //最后一项用于完整性标记
    while(1)
    {
        middle = (busy_endian+free_endian)/2; //计算中间项
        block_offset = 8*middle;
        //测试第block_no块偏移block_offset开始的8字节是否擦除后未使用过。
        //虽然至今所知的flash都可以用是否等于0xff来判断是否擦除，但datasheet
        //上并没有这样说，难保哪天不会出来一个不是这样的，故把判断交给芯片
        //driver才可确保可移植性。
        if(chip->query_ready_with_data(NULL,MDR_buf+block_offset,8))
        {//第middle单元是空闲的
            if(middle == busy_endian)
            {
                result = middle*8;
                break;
            }else
                free_endian = middle;
        }else
        {//第middle单元是已经使用了的
            if(free_endian == middle+1)
            {
                result = free_endian*8;
                break;
            }else
                busy_endian = middle;
        }
    }//查找滚动位置结束
    return result;
}

//----解析PTT表----------------------------------------------------------------
//功能: 扫描MDR缓冲区的3个PTT表，找出已经建立的分区并读出分区参数
//参数: chip，目标芯片，也用于保存扫描结果
//      MDR_record，用于保存结果的指针
//      MDR_buf，MDR表缓冲区
//返回: 已经建立的分区数量
//-----------------------------------------------------------------------------
ufast_t __DFFSD_UasmPTT(struct tagFlashChip *chip,
                         struct tagMDR_TempRecord *MDR_record,uint8_t *MDR_buf)
{
    struct tagMDR_PTT_Mirror *MDR_PTT;
    uint32_t loop,name_len;
    ufast_t PTT_sum=0;    //已经建立的分区数量
    bool_t formatted,unformatted;
    //取MDR_PTT记录的起始地址
    MDR_PTT = (struct tagMDR_PTT_Mirror *)(MDR_buf + CN_MDR_CDR_SIZE);
    for(loop = 0; loop < CN_PTT_SUM_MAX; loop++)
    {//本循环： 1、判断分区创建否
     //         2、读分区名，
     //         3、读各分区的起始块号和块数，不管初始化否
        formatted = (strcmp(MDR_PTT->PTT_format_flag,"ready")==0);
        unformatted = (strcmp(MDR_PTT->PTT_format_flag,"unready")==0);
        if(formatted || unformatted)    //不管格式化否，都表示分区已经建立。
        {
            PTT_sum++;                              //分区总数增量
            chip->MDR.PTT_created[loop] = true;     //分区创建标志设为true
            MDR_record->formatted[loop] = formatted;    //设格式化标志
            name_len = strnlen(MDR_PTT->PTT_name,CN_FLASH_PTT_NAME_LIMIT+1);
            if(name_len == 0)
                strcpy(MDR_record->name[loop],"no name deedbox");
            else if(name_len <= CN_FLASH_PTT_NAME_LIMIT)
                strcpy(MDR_record->name[loop],MDR_PTT->PTT_name);
            else
            {
                memcpy(MDR_record->name[loop],MDR_PTT->PTT_name,
                                        CN_FLASH_PTT_NAME_LIMIT);
                MDR_record->name[loop][CN_FLASH_PTT_NAME_LIMIT] = '\0';
            }
            //分区起始块号
            MDR_record->start_blocks[loop] =
                            pick_little_32bit(MDR_PTT->PTT_start_block,0);
            //分区尺寸(以块数表示)
            MDR_record->blocks_sum[loop] =
                            pick_little_32bit(MDR_PTT->PTT_blocks_sum,0);

        }else   //MDR_PTT既不是"ready"也不是"unready"开头，表明分区未创建
        {
            chip->MDR.PTT_created[loop] = false;    //分区创建标志设为false
            MDR_record->formatted[loop] = false;    //设格式化标志
        }
        //取下一个分区MDR_PTT的偏移地址
        MDR_PTT = (struct tagMDR_PTT_Mirror *)((uint32_t)MDR_PTT +CN_MDR_PTT_SIZE);
    }
    return PTT_sum;
}

//----初始化读MDR表------------------------------------------------------------
//功能: 芯片初始化时读整个MDR表，提取有效数据。如果发现MDR_main和MDR_bak中任意
//      一个有错误，刷新之
//参数: chip，被操作的芯片，同时用于返回结果
//      MDR_record，保存有效数据的指针
//返回: 扫描MDR的结果保存在chip和MDR_record两个结构指针中
//      true=芯片已经按djyosfs初始化，false=芯片未按djyosfs初始化
//-----------------------------------------------------------------------------
//特注: 调试本函数时多次引起rvd崩溃，访问非法地址而被windows关闭，而ARM本身运行
//      正常，重新打开rvd后，甚至可以从上次退出的地方接着运行。
bool_t __DFFSD_ReadMDR(struct tagFlashChip *chip,struct tagMDR_TempRecord *MDR_record)
{
    uint8_t *MDR_buf;
    uint32_t roll_size;
    uint8_t roll_buf[CN_PTT_SUM_MAX*3*8];
    bool_t MDR_main_valid,MDR_bak_valid;
    struct tagMDR_CDR_Mirror *MDR_CDR;
    ufast_t PTT_sum = 0;    //已经建立的分区数量


    MDR_buf = (uint8_t*)M_MallocLc(CN_MDR_SIZE, 0);
    if(MDR_buf == NULL)
        return false;
    __DFFSD_ReadMDR_Main(chip,MDR_buf);
    if(!__DFFSD_Verify_EndFlag(&MDR_buf[CN_MDR_SIZE-4]))  //检查MDR_main是否完整
    {//MDR_main有错误，读取MDR_bak
        MDR_main_valid = false;
        chip->MDR.MDR_bak_block = __DFFSD_Probe_MDR_Bak(chip);   //探测MDR_bak的存储位置
    }else
    {
        MDR_main_valid = true;
        MDR_CDR = (struct tagMDR_CDR_Mirror *)MDR_buf;
        if(strcmp(MDR_CDR->chip_init_flag,"djyosfs") != 0)
        {
            free(MDR_buf);
            return false;      //存储器尚未初始化
        }
        //下面把CDR表的内容读取到chip资源中
        chip->MDR.MDR_bak_block = pick_little_32bit(MDR_CDR->chip_MDR_bak,0);
        //读取保留块的数量和起始块号，保留块数量并不考虑其中可能有坏块
        chip->rev_start = chip->MDR.MDR_bak_block
                          + (CN_MDR_SIZE+chip->block_size -1)/chip->block_size;
        chip->rev_blocks = pick_little_32bit(MDR_CDR->chip_reserve,0);
        //计算用于分区的起始块号，
        chip->PTT_start = chip->rev_start + chip->rev_blocks;
        PTT_sum = __DFFSD_UasmPTT(chip,MDR_record,MDR_buf);
        if(PTT_sum != 0)
        {
            //找到滚动存储区的下一个存储位置
            chip->MDR.roll_next_main = __DFFSD_FindRollNext(chip,MDR_buf);
            //扫描并提取滚动记录中的有效记录(即每类记录的最后一条记录)
            __DFFSD_UasmRoll(chip,MDR_buf,MDR_record,
                              chip->MDR.roll_next_main);
        }else
            chip->MDR.roll_next_main = CN_ROLL_OFFSET;
    }

    __DFFSD_ReadMDR_Bak(chip,MDR_buf);     //读MDR_bak到缓冲区中
    if(!__DFFSD_Verify_EndFlag(&MDR_buf[CN_MDR_SIZE-4]))  //检查MDR_bak是否完整
    {
        MDR_bak_valid = false;  //MDR_bak有错误
    }else
    {
        MDR_bak_valid = true;
        if(MDR_main_valid)      //如果MDR_main有效，则只读取下一个滚动存储位置
        {
            if(PTT_sum != 0)
                chip->MDR.roll_next_bak = __DFFSD_FindRollNext(chip,MDR_buf);
            else
                chip->MDR.roll_next_bak = CN_ROLL_OFFSET;
        }else   //如果MDR_main无效，则所有分区信息从MDR_bak读取
        {
            MDR_CDR = (struct tagMDR_CDR_Mirror *)MDR_buf;
            if(strcmp(MDR_CDR->chip_init_flag,"djyosfs")!=0)
            {
                free(MDR_buf);
                return false;      //存储器尚未初始化
            }
            //下面把CDR表的内容读取到chip资源中
            //读取保留块的数量和起始块号，保留块数量并不考虑其中可能有坏块
            chip->rev_start = chip->MDR.MDR_bak_block
                          +(CN_MDR_SIZE+chip->block_size -1)/chip->block_size;
            chip->rev_blocks = pick_little_32bit(MDR_CDR->chip_reserve,0);
            //计算用于分区的起始块号，
            chip->PTT_start = chip->rev_start + chip->rev_blocks;
            PTT_sum = __DFFSD_UasmPTT(chip,MDR_record,MDR_buf);
            if(PTT_sum != 0)
            {
                chip->MDR.roll_next_bak = __DFFSD_FindRollNext(chip,MDR_buf);
                //扫描并提取滚动记录中的有效记录(即每类记录的最后一条记录)
                __DFFSD_UasmRoll(chip,MDR_buf,MDR_record,
                                  chip->MDR.roll_next_bak);
            }
        }
    }
    if((!MDR_main_valid) && (!MDR_bak_valid))
    {
        free(MDR_buf);    //如果MDR_main和MDR_bak都错误，说明未初始化存储器
        return false;
    }
    else
    {
        roll_size = __DFFSD_AsmRollBuf(chip,MDR_record,roll_buf);
        if(!MDR_main_valid) //MDR_main有误，用MDR_bak恢复之
        {
            __DFFSD_EraseMDR_Main(chip);
            memcpy(MDR_buf+CN_ROLL_OFFSET,roll_buf,roll_size);
            __DFFSD_WriteMDR_Main(chip,MDR_buf,CN_ROLL_OFFSET + roll_size);
            __DFFSD_WriteEndFlagMain(chip);
        }else if(!MDR_bak_valid) //MDR_bak有误，用MDR_main恢复之
        {
            __DFFSD_EraseMDR_Bak(chip);
            memcpy(MDR_buf+CN_ROLL_OFFSET,roll_buf,roll_size);
            __DFFSD_WriteMDR_Bak(chip,MDR_buf,CN_ROLL_OFFSET + roll_size);
            __DFFSD_WriteEndFlagBak(chip);
        }
    }
    free(MDR_buf);
    return true;
}

//----填充滚动条目校验码-------------------------------------------------------
//功能: 给一个MDR的滚动存储记录增加校验码。
//参数: buf，记录指针
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_FillECC_MDR_Record(uint8_t *buf)
{
}

//----填充MDR_CDR校验码--------------------------------------------------------
//功能: 给一个MDR_CDR增加校验码。
//参数: buf，CDR记录指针
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_Fill_ECC_MDR_CDR(uint8_t *buf)
{
}

//----填充MDR_PTT校验码--------------------------------------------------------
//功能: 给一个MDR_PTT增加校验码。
//参数: buf，PTT记录指针
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_Fill_ECC_MDR_PTT(uint8_t *buf)
{
}

//----写MDR_main的结束标记-----------------------------------------------------
//功能: 写入MDR_main的结束标志，表示MDR_main是完整的
//参数: chip，被操作的芯片
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_WriteEndFlagMain(struct tagFlashChip *chip)
{
    uint32_t block_no,block_offset;
    uint8_t buf[4];
    block_no = (CN_MDR_SIZE -4)/chip->block_size;
    block_offset = (CN_MDR_SIZE -4)%chip->block_size;
    buf[0] = 0xaa;
    buf[1] = 0x55;
    buf[2] = 0x55;
    buf[3] = 0xaa;
    chip->write_data_no_ecc(block_no,block_offset,buf,4);
}

//----写MDR_bak的结束标记-----------------------------------------------------
//功能: 写入MDR_bak的结束标志，表示MDR_bak是完整的
//参数: chip，被操作的芯片
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_WriteEndFlagBak(struct tagFlashChip *chip)
{
    uint32_t block_no,block_offset;
    uint8_t buf[4];
    block_no = (CN_MDR_SIZE -4)/chip->block_size + chip->MDR.MDR_bak_block;
    block_offset = (CN_MDR_SIZE -4)%chip->block_size;
    buf[0] = 0xaa;
    buf[1] = 0x55;
    buf[2] = 0x55;
    buf[3] = 0xaa;
    chip->write_data_no_ecc(block_no,block_offset,buf,4);
}

//----写MDR_main--------------------------------------------------------------
//功能: 从DDR_main开头部分开始，写入指定长度的数据。
//参数: chip，被操作的芯片
//      MDR_buf，缓冲区指针
//      len,写入长度
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_WriteMDR_Main(struct tagFlashChip *chip,uint8_t *MDR_buf,uint32_t len)
{
    uint32_t complete_len=0,write_len;
    uint32_t block_no;
    block_no = 0;   //MDR_main默认从0块开始

    do  //把缓冲区中的数据写入到flash中，数据可能跨块边界，故循环
    {
        if((len - complete_len) > chip->block_size)
            write_len = chip->block_size;
        else
            write_len = len-complete_len;
        //写入数据到flash中，不校验
        chip->write_data_no_ecc(block_no,0,MDR_buf+complete_len,write_len);
        block_no++;
        complete_len += write_len;
    }while(complete_len < len);
}

//----写MDR_bak--------------------------------------------------------------
//功能: 从DDR_bak开头部分开始，写入指定长度的数据。
//参数: chip，被操作的芯片
//      MDR_buf，缓冲区指针
//      len,写入长度
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_WriteMDR_Bak(struct tagFlashChip *chip,uint8_t *MDR_buf,uint32_t len)
{
    uint32_t complete_len=0,write_len;
    uint32_t block_no;
    block_no = chip->MDR.MDR_bak_block;   //MDR_main默认从0块开始
    do  //把缓冲区中的数据写入到flash中，数据可能跨块边界，故循环
    {
        if((len - complete_len) > chip->block_size)
            write_len = chip->block_size;
        else
            write_len = len-complete_len;
        //写入数据到flash中，不校验
        chip->write_data_no_ecc(block_no,0,MDR_buf+complete_len,write_len);
        block_no++;
        complete_len += write_len;
    }while(complete_len < len);
}

//----写MDR_main的PTT部分------------------------------------------------
//功能: 把PTT写入到MDR_main中，调用此函数前，确保保存PTT记录的部分已经被擦除.
//参数: chip，被操作的芯片
//      MDR_buf，缓冲区指针
//      PTT_no，分区编号
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_WriteMDR_PTT_Main(struct tagFlashChip *chip,uint8_t *MDR_PTT_buf,
                                                      uint32_t PTT_no)
{
    uint32_t complete_len=0,write_len,write_size;
    uint32_t block_no,block_offset;
    write_size = CN_MDR_PTT_SIZE;
    block_no = (CN_MDR_CDR_SIZE + PTT_no*CN_MDR_PTT_SIZE) / chip->block_size ;
    block_offset = (CN_MDR_CDR_SIZE + PTT_no*CN_MDR_PTT_SIZE)% chip->block_size;
    do  //把缓冲区中的数据写入到flash中，数据可能跨块边界，故循环
    {
        if((write_size-complete_len) > chip->block_size-block_offset)
            write_len = chip->block_size-block_offset;
        else
            write_len = write_size-complete_len;
        //写入数据到flash中，不校验
        chip->write_data_no_ecc(block_no,block_offset,
                                MDR_PTT_buf+complete_len,write_len);
        block_no++;
        block_offset = 0;       //下一块肯定从块首开始写
        complete_len += write_len;
    }while(complete_len < write_size);
}

//----写MDR_bak的PTT部分------------------------------------------------
//功能: 把PTT写入到MDR_bak中，调用此函数前，确保保存PTT记录的部分已经被擦除.
//参数: chip，被操作的芯片
//      MDR_buf，缓冲区指针
//      PTT_no，分区编号
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_WriteMDR_PTT_Bak(struct tagFlashChip *chip,uint8_t *MDR_PTT_buf,
                                                 uint32_t PTT_no)
{
    uint32_t complete_len=0,write_len,write_size;
    uint32_t block_no,block_offset;
    write_size = CN_MDR_PTT_SIZE;
    block_no = (CN_MDR_CDR_SIZE + PTT_no*CN_MDR_PTT_SIZE) / chip->block_size
                                    + chip->MDR.MDR_bak_block;
    block_offset = (CN_MDR_CDR_SIZE + PTT_no*CN_MDR_PTT_SIZE) % chip->block_size ;
    do  //把缓冲区中的数据写入到flash中，数据可能跨块边界，故循环
    {
        if((write_size-complete_len) > chip->block_size-block_offset)
            write_len = chip->block_size-block_offset;
        else
            write_len = write_size-complete_len;
        //写入数据到flash中，不校验
        chip->write_data_no_ecc(block_no,block_offset,
                                MDR_PTT_buf+complete_len,write_len);
        block_no++;
        block_offset = 0;       //下一块肯定从块首开始写
        complete_len += write_len;
    }while(complete_len < write_size);
}

//----写一条MDR滚动记录--------------------------------------------------------
//功能: MDR表中写入一个滚动记录，只有MDR_main中才会有此操作，MDR_bak中是所有滚动
//      记录同时写入，不会有单项写入的操作。
//参数: chip，被操作的芯片
//      PTT_no，分区编号
//      type，写入类型，可能值cn_MDR_DDR_main、cn_MDR_DDR_bak、cn_MDR_PCRB
//      value，被写入的数值
//返回: 无
//注意: 不能对未格式化的分区调用本函数，这是一个内部函数，不检查目标分区
//      是否已经格式化
//-----------------------------------------------------------------------------
void __DFFSD_WriteMDR_RollRecord(struct tagFlashChip *chip,uint32_t PTT_no,
                                   uint32_t type,uint32_t value)
{
    uint8_t item_buf[8];
    uint32_t block_no,block_offset;
    uint8_t *MDR_buf;
    struct tagMDR_TempRecord MDR_record;

    MDR_buf = (uint8_t*)M_MallocLc(CN_MDR_SIZE, 0);
    if(MDR_buf == NULL)
         return ;
    item_buf[0] = type;             //填充记录类型
    item_buf[1] = PTT_no;           //填充分区号
    //填充该分区的DDR_main块号
    fill_little_32bit(&item_buf[2],0,value);
    __DFFSD_FillECC_MDR_Record(item_buf);//填充校验码
    if((chip->MDR.roll_next_main + 8) > CN_MDR_SIZE-8)
    {//特别注明，如cn_MDR_size<784,此处将成死循环
        __DFFSD_ReadMDR_Main(chip,MDR_buf);
        __DFFSD_UasmPTT(chip,&MDR_record,MDR_buf);
        __DFFSD_UasmRoll(chip,MDR_buf,&MDR_record,
                          chip->MDR.roll_next_main);
        if(type == CN_MDR_DDR_MAIN)
            MDR_record.DDR_main_block_no[PTT_no] = value;
        else if(type == CN_MDR_DDR_BAK)
            MDR_record.DDR_bak_block_no[PTT_no] = value;
        else if(type == CN_MDR_PCRB)
            MDR_record.PCRB_block_no[PTT_no] = value;
        __DFFSD_WriteRollBak(chip,&MDR_record);
        __DFFSD_EraseMDR_Main(chip);
        __DFFSD_WriteMDR_Main(chip,MDR_buf,CN_MDR_AND_PTT_SIZE);
        __DFFSD_WriteRollMain(chip,&MDR_record);
        __DFFSD_WriteEndFlagMain(chip);
    }else
    {
        block_no = chip->MDR.roll_next_main / chip->block_size ;
        block_offset = chip->MDR.roll_next_main % chip->block_size ;
        chip->write_data_no_ecc(block_no,block_offset,item_buf,8);
        chip->MDR.roll_next_main += 8;
    }
    free(MDR_buf);
}

//----构造滚动存储区-----------------------------------------------------------
//功能: 从滚动存储记录生成能够直接保存在flash中的数据，已格式化的分区才会有
//      滚动记录。
//参数: chip，被操作的芯片
//      MDR_record，滚动存储记录
//      roll_buf，滚动存储区缓冲区
//返回: roll_buf中有效数据长度
//-----------------------------------------------------------------------------
uint32_t __DFFSD_AsmRollBuf(struct tagFlashChip *chip,
                          struct tagMDR_TempRecord *MDR_record,uint8_t *roll_buf)
{
    uint32_t loop,PTT_sum=0;
    uint8_t *buf = roll_buf;
    for(loop = 0; loop < CN_PTT_SUM_MAX; loop++)
    {
        if(MDR_record->formatted[loop])   //分区已经格式化
        {
            buf[0] = (uint8_t)CN_MDR_DDR_MAIN; //填充记录类型
            buf[1] = (uint8_t)loop;            //填充分区号
            //填充该分区的DDR_main块号
            fill_little_32bit(buf+2,0,MDR_record->DDR_main_block_no[loop]);
            __DFFSD_FillECC_MDR_Record(buf);//填充校验码
            buf += 8;
            buf[0] = CN_MDR_DDR_BAK; //填充记录类型
            buf[1] = loop;            //填充分区号

            //填充该分区的DDR_bak块号
            fill_little_32bit(buf+2,0,MDR_record->DDR_bak_block_no[loop]);
            __DFFSD_FillECC_MDR_Record(buf);//填充校验码
            buf += 8;
            buf[0] = CN_MDR_PCRB; //填充记录类型
            buf[1] = loop;            //填充分区号

            //填充该分区的PCRB块号
            fill_little_32bit(buf+2,0,MDR_record->PCRB_block_no[loop]);
            __DFFSD_FillECC_MDR_Record(buf);//填充校验码
            buf += 8;
            PTT_sum +=8*3;
        }
    }
    return PTT_sum;
}
//----写MDR_main的全部滚动记录-------------------------------------------------
//功能: 写入MDR_main表中的全部滚动存储项
//参数: chip，被操作的芯片
//      MDR_record，保存全部滚动存储项的指针
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_WriteRollMain(struct tagFlashChip *chip,
                             struct tagMDR_TempRecord *MDR_record)
{
    struct tagStMDR_Flag *MDR_flag = &chip->MDR;
    uint8_t roll_buf[CN_PTT_SUM_MAX*3*8];
    uint32_t complete_len=0,write_len,write_size;
    uint32_t block_no,block_offset;
    bool_t write_end_flag=false;
    uint8_t *MDR_buf;

    MDR_buf = (uint8_t*)M_MallocLc(CN_MDR_SIZE, 0);
    if(MDR_buf == NULL)
        return ;
    write_size = __DFFSD_AsmRollBuf(chip,MDR_record,roll_buf);
    if(write_size != 0)
    {
        //如果写入长度+当前偏移大于滚动区容量
        if((MDR_flag->roll_next_main + write_size) > CN_MDR_SIZE-8)
        {//特别注明，如cn_MDR_size<784,此处将成死循环
            __DFFSD_WriteRollBak(chip,MDR_record);
            __DFFSD_ReadMDR_Main(chip,MDR_buf);
            __DFFSD_EraseMDR_Main(chip);
            __DFFSD_WriteMDR_Main(chip,MDR_buf,CN_MDR_AND_PTT_SIZE);
            MDR_flag->roll_next_main = CN_ROLL_OFFSET;
            write_end_flag = true;
        }
        block_no = MDR_flag->roll_next_main / chip->block_size ;
        block_offset = MDR_flag->roll_next_main % chip->block_size;
        MDR_flag->roll_next_main += write_size;
        do  //把缓冲区中的数据写入到flash中，数据可能跨块边界，故循环
        {
            if((write_size-complete_len) > chip->block_size-block_offset)
                write_len = chip->block_size-block_offset;
            else
                write_len = write_size-complete_len;
            //写入数据到flash中，不校验
            chip->write_data_no_ecc(block_no,block_offset,
                                    roll_buf+complete_len,write_len);
            block_no++;
            block_offset = 0;       //下一块肯定从块首开始写
            complete_len += write_len;
        }while(complete_len < write_size);
        if(write_end_flag)
            __DFFSD_WriteEndFlagMain(chip);
    }
    free(MDR_buf);
}

//----写MDR_bak的全部滚动记录-------------------------------------------------
//功能: 写入MDR_bak表中的全部滚动存储项
//参数: chip，被操作的芯片
//      MDR_record，保存全部滚动存储项的指针
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_WriteRollBak(struct tagFlashChip *chip,
                            struct tagMDR_TempRecord *MDR_record)
{
    struct tagStMDR_Flag *MDR_flag = &chip->MDR;
    uint8_t roll_buf[CN_PTT_SUM_MAX*3*8];
    uint32_t complete_len=0,write_len,write_size;
    uint32_t block_no,block_offset;
    bool_t write_end_flag=false;
    uint8_t *MDR_buf;

    MDR_buf = (uint8_t*)M_MallocLc(CN_MDR_SIZE, 0);
    if(MDR_buf == NULL)
        return ;
    write_size = __DFFSD_AsmRollBuf(chip,MDR_record,roll_buf);
    if(write_size != 0)
    {
        if((MDR_flag->roll_next_bak + write_size) > CN_MDR_SIZE-8)
        {//特别注明，如cn_MDR_size<784,此处将成死循环
            __DFFSD_ReadMDR_Bak(chip,MDR_buf);
            __DFFSD_EraseMDR_Bak(chip);
            __DFFSD_WriteMDR_Bak(chip,MDR_buf,CN_MDR_AND_PTT_SIZE);
            MDR_flag->roll_next_bak = CN_ROLL_OFFSET;
            write_end_flag = true;
        }
        block_no = MDR_flag->roll_next_main / chip->block_size ;
        block_offset = MDR_flag->roll_next_main % chip->block_size ;
        MDR_flag->roll_next_bak += write_size;
        do  //把缓冲区中的数据写入到flash中，数据可能跨块边界，故循环
        {
            if((write_size-complete_len) > chip->block_size-block_offset)
                write_len = chip->block_size-block_offset;
            else
                write_len = write_size-complete_len;
            //写入数据到flash中，不校验
            chip->write_data_no_ecc(block_no,block_offset,
                                    roll_buf+complete_len,write_len);
            block_no++;
            block_offset = 0;       //下一块肯定从块首开始写
            complete_len += write_len;
        }while(complete_len < write_size);
        if(write_end_flag)
            __DFFSD_WriteEndFlagBak(chip);
    }
    free(MDR_buf);
}

//----写入一个MDR记录----------------------------------------------------------
//功能: 保存一个MDR记录，本函数供flash driver使用，flash driver并不知道使用滚动
//      方式记录，本函数只是__DFFSD_write_MDR_roll_record函数的包装而已。
//参数: PTT_flash_tag，被操作的分区
//      type，写入类型，可能值cn_MDR_DDR_main、cn_MDR_DDR_bak、cn_MDR_PCRB
//      value，被写入的数值
//返回: 无
//注意: 不能对未格式化的分区调用本函数，这是一个内部函数，不检查目标分区
//      是否已经格式化
//-----------------------------------------------------------------------------
void __DFFSD_WriteMDR_Item(struct tagStPTT_Flash *PTT_flash_tag,
                            uint32_t type,uint32_t value)
{
    uint32_t PTT_no;
    struct tagFlashChip *chip;
    if(PTT_flash_tag == NULL)
        return;
    if((type==CN_MDR_DDR_MAIN)||(type==CN_MDR_DDR_BAK)||(type==CN_MDR_PCRB))
    {
        PTT_no = PTT_flash_tag->PTT_no;
        chip = PTT_flash_tag->chip;
        __DFFSD_WriteMDR_RollRecord(chip,PTT_no,type,value);
    }
}

//----标记分区为已格式化-----------------------------------------------------
//功能: 在MDR_PTT表中标记一个分区为已经格式化，在MDR_main和MDR_bak中都要修改。
//参数：chip，被操作的芯片
//      PTT_no，分区号
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_SetMDR_PTT_Formatted(struct tagFlashChip *chip,uint32_t PTT_no)
{
    uint8_t *MDR_buf;
    struct tagMDR_PTT_Mirror *MDR_PTT;

    MDR_buf = (uint8_t*)M_MallocLc(CN_MDR_SIZE, 0);
    if(MDR_buf == NULL)
        return ;
    //计算被写的分区的MDR_PTT记录的地址
    MDR_PTT = (struct tagMDR_PTT_Mirror *)(MDR_buf +
                        CN_MDR_CDR_SIZE + PTT_no * CN_MDR_PTT_SIZE);
    __DFFSD_ReadMDR_Bak(chip,MDR_buf);
    strcpy(MDR_PTT->PTT_format_flag,"ready");
    __DFFSD_Fill_ECC_MDR_PTT((uint8_t *)MDR_PTT);
    __DFFSD_EraseMDR_Bak(chip);
    //最后4字节是结束标志AA5555AA，故减4
    __DFFSD_WriteMDR_Bak(chip,MDR_buf,CN_MDR_SIZE-4);
    __DFFSD_WriteEndFlagBak(chip);

    __DFFSD_ReadMDR_Main(chip,MDR_buf);
    strcpy(MDR_PTT->PTT_format_flag,"ready");
    __DFFSD_Fill_ECC_MDR_PTT((uint8_t *)MDR_PTT);
    __DFFSD_EraseMDR_Main(chip);
    //最后4字节是结束标志AA5555AA，故减4
    __DFFSD_WriteMDR_Main(chip,MDR_buf,CN_MDR_SIZE-4);
    __DFFSD_WriteEndFlagMain(chip);
    free(MDR_buf);
    return;
}
//----擦除MDR_main-------------------------------------------------------------
//功能: 擦除MDR_main，MDR_main的起始块默认为0
//参数：chip，被操作的芯片
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_EraseMDR_Main(struct tagFlashChip *chip)
{
    uint32_t blocks,loop;
    blocks = (CN_MDR_SIZE + chip->block_size -1) / chip->block_size;
    for(loop = 0; loop < blocks; loop++)
        chip->erase_block(loop);
}

//----擦除MDR_bak-------------------------------------------------------------
//功能: 擦除MDR_bak，MDR_bak的起始块默认为0
//参数：chip，被操作的芯片
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_EraseMDR_Bak(struct tagFlashChip *chip)
{
    uint32_t blocks,loop;
    blocks = (CN_MDR_SIZE + chip->block_size -1) / chip->block_size;
    for(loop = 0; loop < blocks; loop++)
        chip->erase_block(loop + chip->MDR.MDR_bak_block);
}

