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
//文件描述:flash文件系统驱动主模块
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
/*
各缩写全称：
MDR  memory medium description record   存储介质(芯片)描述记录
CDR  chip description record            MDR中芯片描述部分
PTT  deedbox                            分区，也指MDR中分区描述部分
DDR  deedbox description record         分区描述记录
DBL  DDR_mirror blocks list             存储DDR信息的块号列表
ARB  abrasion record base               磨损记录基数
ART  abrasion record table              磨损记录表
FDSB file description starting block    文件描述表首块块号，基于本分区
FDT  file description table             文件描述表
MAT  memory allocation table            存储器分配表
PCRB power cut restore block            掉电恢复块
ECCF ecc flag                           ecc使能标志，1=使能，0=不使能。

在创建分区时应该制定该分区是否使用ecc校验，有些专门用来存储图形或者语音的
分区，是允许不使能ecc的，这样可以大幅提高存取速度。

特注:MDR表不做整体ECC校验，否则每次滚动修改都需要改ECC码。备份MDR表要做ECC校验。

A、MDR表：文件系统本身不使用，由模块初始化（module_init_）函数读取，其内容包括
    1、CDR,芯片初始化标志(8字节=djyosfs)，flash芯片总块数（包括坏块），块尺寸，
       MDR_bak块号，分区总数，占用512字节，其中分区总数循环写入，本来设计要
       循环记录可用块数的，但因为可用块数是动态性强，记录也没有多大实用意义，还
       容易造成MDR表磨损，故用动态产生的方法。
    2、1号分区PTT,格式化标志(8字节=ready/unready)，分区名(32字节)，起始块号，
       占用块数，本掉电恢复块的块号，每交换1024次改变一次。2个DDR表链首块块号
       （每使用1024次改一次）。本部分占用1024字节，循环写入以延长寿命。
    3、2号分区PTT,........
    4、一个芯片上可建立的分区数量默认上限是3，但可由芯片管理工具修改。

    flash存储器，不管是nand还是nor型，总是使用头两个可用块存储，第一块存储主MDR，
    第二块存储备份MDR，采用某些机制保证主MDR的写入次数少，擦除次数更少，只有当
    擦除主MDR时，才使用备份MDR。读出时，先读主MDR，若有错误再读备份MDR。这样做
    可以保证：
    1、保证主MDR不会过度磨损。
    2、当且仅当写主MDR时恰逢掉电才需要从备份中
    3、备份MDR表的擦写次数更少。

B、DDR，以存储器说明中DDR表首块号为基准，
    1、DBL,DDR表本身占据的块号列表，每项长度=2或4,主表和备份表个一
    2、FDSB文件描述表首块块号，长度=sizeof(mat_size_t)，每修改1000次改一次。
    3、ARB磨损次数基数，4字节，记录磨损次数的底数，4字节对齐
    4、ART磨损次数表，每块占用2字节，加上基数即得该块实际磨损次数。自动对齐
    5、MAT存储器分配表：每项8字节，保存前一块和后一块的块号，
       首块的前一块保存目录项编号，末块的后一块指向自己，把每个文件串成一个表。
       坏块：前后指针相等但不等于本块块号，而是本块块号加1
       空闲块：前后指针相等，等于本块块号加2
       DDR_main块：前后指针相等，等于本块块号加3
       DDR_bak块：前后指针相等，等于本块块号加4
       掉电恢复块：前后指针相等，等于本块块号加5
特注: 两份DDR表的DBL部分不相同，其他部分完全相同

分区说明(DDR)占用的空间计算如下:
假设需占用x字节，y块，则有:y=(x+block_size-1)/block_size
DBL = 4*y(主表和备份表个一)
ECCF= 4
ARB = 4
ART = 2*block_sum
FDSB = 4
MAT = 4*2*block_sum,为防止一个MAT表项块块边界，MAT表的起始地址应该是4/8字节对齐的。
x = DBL + ARB + ART + FDSB + MAT+对齐损耗
把y的表达式代入上式，解方程得到:
x= (ECCF+ARB+ART+FDSB+MAT)*block_size + (block_size -1)*4/(block_size-4)
x应该是一个介乎(n~n+4)之间的数，再加上对齐损耗即可。

E、目录信息表：每个文件/目录用40字节表示，目录表长度是动态的

    改一个文件的一个块可能涉及到修改：
    1、修改该块本身。
    2、若长度生变或改首块，还要修改目录表。
    3、改MAT表。
    4、如果改目录表或MAT表时发生坏块更替，改A部分。
*/
#include "stdint.h"
#include "stdlib.h"
#include "endian.h"
#include "rsc.h"
#include "lock.h"
#include "time.h"
#include "systime.h"
#include "file.h"
#include "flashfile.h"
#include "mdr.h"
#include "ddr.h"
#include "fdt.h"

#include <string.h>

//flash芯片的资源结点，所有flash芯片都安装在此结点下
static struct tagRscNode tg_flash_chip_root_rsc;
struct tagSemaphoreLCB *g_ptFlashfileSemp; //保护flash_chip树所用的信号量

//----卸载一个芯片------------------------------------------------------------
//功能: 从文件系统中删除该芯片，删除属于该芯片的所有分区，只有在该芯片的所有
//      分区都没有打开的文件的时候才可以删除
//参数: chip，待删除的芯片
//返回: 成功删除返回true，否则返回false
//----------------------------------------------------------------------------
bool_t DFFSD_UninstallChip(struct tagFlashChip *chip)
{
    return true;
}

//----格式化分区------------------------------------------------------------
//功能：格式化分区，该分区应该已经建立，并且经过启动初始化加入到设备链表中。
//      A、未格式化的分区格式化过程:
//      1、计算存储DDR表所需的块数(2份)
//      2、扫描一遍坏块，在MAT表中标记坏块，并从好块中挑出2份DDR，
//      3、填写DDR中的DBL表、MAT表
//      4、ART表排序，并把DDR表写入flash中。
//      5、分配一块做FDT。
//      6、初始化FDT，然后写入flash中
//      7、更新MDR表，并写入flash中
//      B、已经格式化的分区格式化操作
//      再说吧
//参数：fmt_para1，格式化参数，1=使用ecc校验，0表示不用，一般专门存储多媒体文件
//              的分区可以不用
//      fmt_para2，格式化参数，暂不用
//      PTT_device_tag，待格式化的分区，该分区已经安装到设备系统中
//返回：true=成功格式化，含原来就已经格式化好而无需再次格式化的。
//      false=失败
//------------
//更新记录:
// 1.日期: 2015/3/10
//   说明: 删除valid_sum的重复赋值
//   作者: 季兆林
//-----------------------------------------------------------------------------
bool_t DFFSD_FormatPTT(u32 fmt_para1,u32 fmt_para2,
                        struct tagPTTDevice *PTT_device_tag)
{
    struct tagStPTT_Flash *PTT_flash_tag;
    struct tagFlashChip *chip;
    struct tagMAT_Table *MAT_item;
    uint32_t DDR_blocks,valid_blocks;
    uint32_t loop;

    if(PTT_device_tag == NULL)
        return false;
    PTT_flash_tag = (struct tagStPTT_Flash *)PTT_device_tag->PTT_medium_tag;
    chip = PTT_flash_tag->chip;
    //DDR表所占的块数
    DDR_blocks=(chip->block_size+PTT_flash_tag->DDR_size-1)/chip->block_size;

    PTT_flash_tag->DDR_ARB = 0; //磨损次数基数清零
    PTT_flash_tag->nand_ecc = (bool_t)fmt_para1;//是否开启ECC
    MAT_item = PTT_flash_tag->DDR_MAT;//指向内存中建立的MAT表
    valid_blocks = 0;

    for(loop = 0; loop < PTT_flash_tag->block_sum; loop++)//对分区的总规划
    {
        MAT_item[loop].previous = loop;
        MAT_item[loop].next = loop;
        PTT_flash_tag->ART_times[loop] = 0;
        PTT_flash_tag->ART_position[loop] = loop;
        PTT_flash_tag->ART_block_no[loop] = loop;
        if(chip->check_block(loop + PTT_flash_tag->start_block))
        {
            if(valid_blocks < DDR_blocks)
            {//这些块分配给DDR_main表
                if(valid_blocks == 0)
                    PTT_flash_tag->DDR_main=loop;
                //填写DBL表,即DDR_DBL的有效块指向物理实际块号
                PTT_flash_tag->DDR_DBL[valid_blocks] = loop;
                //设置第loop块是DDR_main块
                MAT_item[loop].attr = cn_DDR_main_block;
            }else if(valid_blocks < 2*DDR_blocks)
            {//这些块分配给DDR_bak表
                if(valid_blocks == DDR_blocks)
                    PTT_flash_tag->DDR_bak=loop;
                //填写DBL表
                PTT_flash_tag->DDR_DBL[valid_blocks] = loop;
                //设置第loop块是DDR_bak块
                MAT_item[loop].attr = cn_DDR_bak_block;
            }
            else if(valid_blocks == 2*DDR_blocks)
            {//本块分配给目录表，刚格式化的分区，目录表只有1块
                //填写FDS(目录表首块块号)
                PTT_flash_tag->DDR_FDSB = loop;
                MAT_item[loop].attr = cn_FDT_block;
            }
            else if(valid_blocks == 2*DDR_blocks+1)
            {//分区掉电恢复块
                //设置第loop块是掉电恢复块
                MAT_item[loop].attr = cn_PCRB_block;
                chip->erase_block(loop + PTT_flash_tag->start_block);
                PTT_flash_tag->PCRB_no = loop;
            }
            else
            {//空闲块
                //设置第loop块是空闲块
                MAT_item[loop].attr = cn_free_block;
            }
            valid_blocks++; //找到的可用块数量增量
        }
        else
        {//坏块
            //设置第i块是坏块
            MAT_item[loop].attr = cn_invalid_block;
        }
    }
    //总有效块数，接下来__DFFSD_write_DDR函数如果产生坏块，会修改valid_sum。
    __DFFSD_SortART(PTT_flash_tag); //建立ART表
    for(loop = 0; loop<DDR_blocks; loop++)  //刚格式化，DDR表当然还没有写入
    {
        PTT_flash_tag->writed_DDR_main[loop] = false;
        PTT_flash_tag->writed_DDR_bak[loop] = false;
    }
    if( ! __DFFSD_WriteDDR(PTT_flash_tag))         //写DDR表到flash
    {
        return false;   //DDR表写入flash发生错误
    }
    //以下4行修改flash中的MDR表，MDR表是通过严格限制磨损次数和强力校验来保证
    //正确的，即使在有瑕玷的块，也能保证正确，不会报错
    //将该分区的MDR的DBX设置为"ready"
    __DFFSD_SetMDR_PTT_Formatted(chip,PTT_flash_tag->PTT_no);
    //将DDR_main、DDR_bak和PCRB写入flash的滚动记录区
    __DFFSD_WriteMDR_Item(PTT_flash_tag,CN_MDR_DDR_MAIN,
                        PTT_flash_tag->DDR_main+PTT_flash_tag->start_block);
    __DFFSD_WriteMDR_Item(PTT_flash_tag,CN_MDR_DDR_BAK,
                        PTT_flash_tag->DDR_bak+PTT_flash_tag->start_block);
    __DFFSD_WriteMDR_Item(PTT_flash_tag,CN_MDR_PCRB,
                        PTT_flash_tag->PCRB_no+PTT_flash_tag->start_block);
    if( ! __DFFSD_InitFDT(PTT_flash_tag))    //写目录表
    {
        return false;
    }


    //设置根目录的所在的FDT表项
    if(PTT_device_tag->opened_root!=NULL)
    {
        PTT_device_tag->opened_root->file_medium_tag = 0;
    }

    return true;
}

//----定位读(写)块------------------------------------------------------------
//功能: 确定读写位置的块号，块号是基于分区的相对块号，如果读写位置已经超出文件
//      已分配的块数，则返回cn_limit_uint32，
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      fp,文件指针，调用方需确保是文件而不是目录
//      offset,读写位置偏移量
//返回: 块号
//----------------------------------------------------------------------------
uint32_t __DFFSD_LocateBlock(struct tagStPTT_Flash *PTT_flash_tag,
                              struct tagFileRsc *fp,sint64_t offset)
{
    uint32_t block_no,loop,next_block,end_block;
    struct tagFlashChip *chip;
    struct tagFdtInfo FDT_item;

    //获取文件所属芯片
    chip = PTT_flash_tag->chip;
    //读出文件的目录项
    __DFFSD_ReadFDT_Item(PTT_flash_tag,(uint32_t)fp->file_medium_tag,&FDT_item);
    //从目录项中取文件首块块号
    next_block = pick_little_32bit(FDT_item.fstart_dson,0);
    block_no = (u32)(offset/chip->block_size); //计算读写位置处于文件内第几块
    //计算文件结束位置在文件内的块号，
    if(fp->file_size != 0)
        end_block = (u32)((fp->file_size - 1) / chip->block_size);
    else
        end_block = 0;
    if(block_no > end_block) //读写位置超出文件已分配的块
    {
        return CN_LIMIT_UINT32;
    }
    for(loop = 0; loop < block_no; loop++)  //沿MAT表找到目标位置所在块
    {
        next_block = PTT_flash_tag->DDR_MAT[next_block].next;
    }
    return next_block;
}

//----定位最后一块-------------------------------------------------------------
//功能: 确定文件最后一块的块号
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      fp,文件指针，调用方需确保是文件而不是目录
//返回: 块号
//----------------------------------------------------------------------------
uint32_t __DFFSD_LastBlock(struct tagStPTT_Flash *PTT_flash_tag,
                            struct tagFileRsc *fp)
{
    uint32_t loop,next_block;
    struct tagFdtInfo FDT_item;

    //读出文件的目录项
    __DFFSD_ReadFDT_Item(PTT_flash_tag,(uint32_t)fp->file_medium_tag,&FDT_item);
    //从目录项中取文件首块块号
    loop = pick_little_32bit(FDT_item.fstart_dson,0);
    do
    {
        next_block = loop;
        loop = PTT_flash_tag->DDR_MAT[next_block].next;
    }while(loop != next_block);
    return next_block;
}

//----获取文件访问信息---------------------------------------------------------
//功能: 提取用于访问文件所需的位置信息，包括读位置块号，写位置块号，文件占用块
//      数，以及文件最后一块的块号。
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      fp,文件指针，调用方需确保是文件而不是目录
//      access_tag，用于返回结果的指针
//返回: 在access_tag中返回各种参数
//todo: 考虑利用缓存，避免每次读写都从flash芯片中获取access_tag信息
//----------------------------------------------------------------------------
void __DFFSD_GetAccessInfo(struct tagStPTT_Flash *PTT_flash_tag,
                            struct tagFileRsc *fp,
                            struct tagFileAccess *access_tag)
{
    uint32_t block,next_block,r_block,w_block,blocks=0;
    struct tagFlashChip *chip;
    struct tagFdtInfo FDT_item;
    uint32_t buffed_data; //文件写缓冲区中的数据量和剩余数据量

    buffed_data = Ring_Check(fp->p_write_buf); //检查写缓冲区中的数据量
    //获取文件所属芯片
    chip = PTT_flash_tag->chip;
    r_block = (u32)(fp->read_ptr / chip->block_size);

    w_block = (u32)((fp->write_ptr-buffed_data)/ chip->block_size);

    if(w_block>0)
    {
        if(((fp->write_ptr-buffed_data)% chip->block_size) == 0)
            access_tag->write_block_no = CN_LIMIT_UINT32;
    }
    //读出文件的目录项
    __DFFSD_ReadFDT_Item(PTT_flash_tag,(uint32_t)fp->file_medium_tag,&FDT_item);
    //从目录项中取文件首块块号
    next_block = pick_little_32bit(FDT_item.fstart_dson,0);
    //读文件真实尺寸，为防止符号扩展导致不必要的麻烦，按无符号数移位。
    //true_size只反映flash中的数据，不包括写缓冲区中的数据
    access_tag->true_size = pick_little_32bit(FDT_item.file_size,0)
            +((uint64_t)pick_little_32bit(FDT_item.file_size,1)<<32);
    do
    {
        block = next_block;
        if(blocks == r_block)
            access_tag->read_block_no = block;
        if(blocks == w_block)
            access_tag->write_block_no = block;
        blocks++;
        next_block = PTT_flash_tag->DDR_MAT[next_block].next;
    }while(block != next_block);
    access_tag->last_block_no = block;
    return ;
}

//----写操作后同步文件缓冲区---------------------------------------------------
//功能: 执行文件写操作之后，调用本函数同步缓冲区。
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      fp，目标文件指针
//      write_len，实际写入长度
//返回: true=成功，false=有错误
//-----------------------------------------------------------------------------
bool_t __DFFSD_SyncBufAfterWrite(struct tagStPTT_Flash *PTT_flash_tag,
                               FILE *fp,uint32_t write_len)
{
    switch(fp->open_mode)
    {
        case EN_R_RB:   //只读文件，无需操作
        {
        }break;
        case EN_W_WB:   //只写文件，无需操作
        {
        }break;
        case EN_A_AB:   //只写文件，无需操作
        {
        }break;
        case EN_R_RB_PLUS:  //读写位置相同，若读缓冲区有数据，删掉write_len长度
        case EN_W_WB_PLUS:  //的数据量。
        {
            Ring_DumbRead(fp->p_read_buf,write_len);
        }break;
        case EN_A_AB_PLUS:  //读写位置可以不同，读前调用__sync_buf_before_read
        {                   //函数已经保证写缓冲区不会与读缓冲区冲突，新写入的
                            //数据肯定不会覆盖读缓冲区中原有数据，故无需处理。
        }break;
        default:break;
    }
    return true;
}

//----改写一块:覆盖数据--------------------------------------------------------
//功能: 把芯片块缓冲区的内容写入flash中某块，覆盖被写入的位置的原有数据，如果
//      在写入时遇到坏块，申请一个新块替换之，并把坏块登记到MAT表中(并不把MAT写
//      入到flash中)，返回替换后的块号。写入前，先看看是否需要擦除，如果需要擦除
//      则要看该块是否已经在busy[8]区，如是，还要用"释放-重新申请-擦写"的过程进
//      行块替换。待写入的数据是块缓冲区从offset开始的size个字节。
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      block_no，目标块号
//      offset，起始地址在块内的偏移量
//      size，写入缓冲区长度
//返回: 写入成功则返回实际写入的块号，
//      写入失败返回cn_limit_uint32，空间不足或者连续遇到10个坏块会导致失败
//-----------------------------------------------------------------------------
uint32_t __DFFSD_WriteUpdateBlock(struct tagStPTT_Flash *PTT_flash_tag,
                             uint32_t block_no,uint32_t offset,uint32_t size)
{
    uint32_t fail_counter;  //如果连续分配10块均不能正确写入，则返回
    uint32_t write_result,result_block,PTT_start_block;
    struct tagFlashChip *chip = PTT_flash_tag->chip;
    uint8_t *buf;
    bool_t block_ready,write_success;

    buf = chip->block_buf;
    PTT_start_block = PTT_flash_tag->start_block;
    fail_counter = 0;
    result_block = block_no;
    if(PTT_flash_tag->nand_ecc)
        block_ready = chip->query_block_ready_with_ecc(
                                    result_block+PTT_start_block,
                                    offset,buf+offset,size);
    else
        block_ready = chip->query_block_ready_no_ecc(
                                    result_block+PTT_start_block,
                                    offset,buf+offset,size);
    if(offset != 0)
        //把目标块的数据读到块缓冲区中，先读offset以前的部分
        chip->read_data_with_ecc(block_no+PTT_start_block,
                                 0,buf,offset);
    if(offset+size != chip->block_size)
        //再读有效部分以后的部分
        chip->read_data_with_ecc(block_no+PTT_start_block,
                                 offset+size,buf+offset+size,
                                 chip->block_size-offset-size);

    //把数据写入到掉电恢复块，
    chip->write_PCRB(PTT_flash_tag->PCRB_no+PTT_start_block,
                     block_no + PTT_start_block,buf);
    do  //因写入过程可能产生坏块，故需要用循环，直到正确写入才退出
    {
        if((fail_counter == 0) && (block_ready)) //无需擦除的情况
        {
            write_result = chip->write_data_with_ecc(
                                result_block + PTT_start_block,
                                offset,buf+offset,size);
            if(write_result == size)
                write_success = true;
            else
                write_success = false;
        }else           //需要擦除的情况
        {
            chip->erase_block(result_block+PTT_start_block);//擦除块
            //写入目标块，整块写入
            write_result = chip->write_data_with_ecc(
                                result_block + PTT_start_block,
                                0,buf,chip->block_size);
            if(write_result == chip->block_size)
            {
                //磨损次数增量
                __DFFSD_IncAbrasionTimes(PTT_flash_tag,result_block);
                write_success = true;
            }else
                write_success = false;
        }
        if(!write_success)   //写入失败，坏块
        {
            fail_counter++;     //写入失败次数增量
            //登记坏块到磨损表
            __DFFSD_RegisterInvalid(PTT_flash_tag,result_block);
            //分配新块，必须使用简易分配法，否则容易造成连环套
            result_block = __DFFSD_AllocateBlockSimple(PTT_flash_tag);
            //注意，掉电恢复块的目标块号无需修改，因为此时MAT表以及可能需要修改
            //的FDT还没有改变。
            if(result_block == CN_LIMIT_UINT32) //分配失败(无空闲块)
            {
                return CN_LIMIT_UINT32;         //返回错误
            }
        }else       //成功写入
        {
            break;
        }
    }while(fail_counter <= 10); //以连续遇到10个坏块为异常退出条件
    if(fail_counter > 10)           //因10次坏块而结束do-while循环
        return CN_LIMIT_UINT32;
    else                            //数据已经正确写入
        return result_block;
}

//----改写一块:添加数据--------------------------------------------------------
//功能: 把芯片块缓冲区的内容写入flash中某块，被写入的位置原来没有有效数据，如果
//      在写入时遇到坏块，申请一个新块替换之，并把坏块登记到MAT表中(并不把MAT写
//      入到flash中)，返回替换后的块号。写入前，先看看是否需要擦除，如果需要擦除
//      则要看该块是否已经在busy[8]区，如是，还要用"释放-重新申请-擦写"的过程进
//      行块替换。待写入的数据是块缓冲区从offset开始的size个字节。
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      block_no，目标块号
//      offset，起始地址在块内的偏移量
//      size，写入缓冲区长度
//返回: 写入成功则返回实际写入的块号，
//      写入失败返回cn_limit_uint32，空间不足或者连续遇到10个坏块会导致失败
//-----------------------------------------------------------------------------
uint32_t __DFFSD_WriteAppendBlock(struct tagStPTT_Flash *PTT_flash_tag,
                             uint32_t block_no,uint32_t offset,uint32_t size)
{
    uint32_t fail_counter;  //如果连续分配10块均不能正确写入，则返回
    uint32_t write_result,result_block,PTT_start_block;
    struct tagFlashChip *chip = PTT_flash_tag->chip;
    uint8_t *buf;
    bool_t block_ready,PCRB_writed=false,write_success;

    buf = chip->block_buf;
    PTT_start_block = PTT_flash_tag->start_block;
    fail_counter = 0;
    result_block = block_no;
    if(PTT_flash_tag->nand_ecc)
        block_ready = chip->query_block_ready_with_ecc(
                                    result_block+PTT_start_block,
                                    offset,buf+offset,size);
    else
        block_ready = chip->query_block_ready_no_ecc(
                                    result_block+PTT_start_block,
                                    offset,buf+offset,size);

    do  //因写入过程可能产生坏块，故需要用循环，直到正确写入才退出
    {
        if((fail_counter == 0) && (block_ready))//不出故障和不需要擦除，立马写入
        {
            write_result = chip->write_data_with_ecc(
                                    result_block + PTT_start_block,
                                    offset,buf+offset,size);
            if(write_result == size)
                write_success = true;
            else
                write_success = false;
        }else
        {
            if(!PCRB_writed)
            {
                if(offset != 0)
                    //把目标块的数据读到块缓冲区中，先读offset以前的部分
                    chip->read_data_with_ecc(block_no+PTT_start_block,
                                             0,buf,offset);
                if(offset+size != chip->block_size)
                    //再读有效部分以后的部分
                    chip->read_data_with_ecc(block_no+PTT_start_block,
                                             offset+size,buf+offset+size,
                                             chip->block_size-offset-size);
                //把数据写入到掉电恢复块，
                chip->write_PCRB(PTT_flash_tag->PCRB_no+PTT_start_block,
                                 block_no + PTT_start_block,buf);
                PCRB_writed = true;
            }
            chip->erase_block(result_block+PTT_start_block);
            //写入目标块，整块写入
            write_result = chip->write_data_with_ecc(
                                result_block + PTT_start_block,
                                0,buf,chip->block_size);
            if(write_result == chip->block_size)
            {
                //磨损次数增量
                __DFFSD_IncAbrasionTimes(PTT_flash_tag,result_block);
                write_success = true;
            }else
                write_success = false;
        }
        if(!write_success)   //写入失败，坏块
        {
            fail_counter++;     //写入失败次数增量
            //登记坏块到磨损表
            __DFFSD_RegisterInvalid(PTT_flash_tag,result_block);
            //分配新块，必须使用简易分配法，否则容易造成连环套
            result_block = __DFFSD_AllocateBlockSimple(PTT_flash_tag);
            //注意，掉电恢复块的目标块号无需修改，因为此时MAT表以及可能需要修改
            //的FDT还没有改变。
            if(result_block == CN_LIMIT_UINT32) //分配失败(无空闲块)
            {
                return CN_LIMIT_UINT32;         //返回错误
            }
        }else       //成功写入
        {
            break;
        }
    }while(fail_counter <= 10); //以连续遇到10个坏块为异常退出条件
    if(fail_counter > 10)           //因10次坏块而结束do-while循环
        return CN_LIMIT_UINT32;
    else                            //数据已经正确写入
        return result_block;
}


//----把数据写入新块-----------------------------------------------------------
//功能: 把芯片的块缓冲区的内容写入到一个尚无有效数据的新块中，因写入前块内并无
//      有效数据，故无需动用掉电恢复块，如果在写入时遇到坏块，申请一个新块替换
//      之，并把坏块登记到MAT表中(并不把MAT写入到flash中)，返回替换后的块号。
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      block_no，目标块号
//      offset，起始地址在块内的偏移量
//      size，写入缓冲区长度
//返回: 写入成功则返回实际写入的块号，
//      写入失败返回cn_limit_uint32，空间不足或者连续遇到10个坏块会导致失败
//-----------------------------------------------------------------------------
uint32_t __DFFSD_WriteNewBlock(struct tagStPTT_Flash *PTT_flash_tag,
                             uint32_t block_no,uint32_t offset,uint32_t size)

{
    uint32_t fail_counter;  //如果连续分配10块均不能正确写入，则返回
    uint32_t write_result,result_block,PTT_start_block;
    struct tagFlashChip *chip = PTT_flash_tag->chip;

    fail_counter = 0;
    PTT_start_block = PTT_flash_tag->start_block;
    result_block = block_no;
    do  //因写入过程可能产生坏块，故需要用循环，直到正确写入才退出
    {
        //查看是否需要擦除
        if(!chip->query_block_ready_with_ecc(
                                    result_block+PTT_start_block,
                                    offset,chip->block_buf+offset,size))
        {//需要擦除
            //擦除目标块
            chip->erase_block(result_block + PTT_start_block);
            //把数据写入目标块中
            write_result = chip->write_data_with_ecc(
                                result_block+PTT_start_block,
                                offset,chip->block_buf+offset,size);
        }else   //不需要擦除，新申请的块刚好是一个已经被擦除的块。
        {
            //直接把数据写入到目标块中
            write_result = chip->write_data_with_ecc(
                                result_block + PTT_start_block,
                                offset,chip->block_buf+offset,size);
        }
        if(write_result != size)   //写入失败
        {
            fail_counter++;     //写入失败次数增量
            __DFFSD_RegisterInvalid(PTT_flash_tag,result_block);   //登记坏块
            //分配新块，必须使用简易分配法，否则容易造成连环套
            result_block = __DFFSD_AllocateBlockSimple(PTT_flash_tag);
            if(result_block == CN_LIMIT_UINT32) //分配失败(无空闲块)
            {
                return CN_LIMIT_UINT32;         //返回错误
            }
        }else                   //成功写入
        {
            //当然了，擦写一次，目标块的磨损次数就加1
            //即使无需擦除，这也是新申请的一块的第一次写入，磨损次数自然也要加1
            __DFFSD_IncAbrasionTimes(PTT_flash_tag,result_block);
            break;
        }
    }while(fail_counter <= 10);     //以连续遇到10个坏块为异常退出条件
    if(fail_counter > 10)           //因10次坏块而结束do-while循环
        return CN_LIMIT_UINT32;
    else                            //数据已经正确写入
        return result_block;
}

//----移动文件读指针-----------------------------------------------------------
//功能: 把一个文件的读指针移动到一个新位置
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      fp，目标文件指针
//      new_ptr，新的读指针
//返回: true=成功，false=有错误
//-----------------------------------------------------------------------------
bool_t __DFFSD_MoveFileReadPtr(struct tagStPTT_Flash *PTT_flash_tag,
                            FILE *fp,sint64_t new_ptr)
{
    if(new_ptr > fp->file_size)
        return false;

    switch (fp->open_mode)
    {
        case EN_R_RB:
        {
            fp->read_ptr = new_ptr;             //读位置
        }break;
        case EN_W_WB:
        {
            return false;                       //读位置无效
        }break;
        case EN_A_AB:
        {
            return false;                       //读位置无效
        }break;
        case EN_R_RB_PLUS:
        {
            fp->read_ptr = new_ptr;             //读位置
            fp->write_ptr = new_ptr;            //写位置
        }break;
        case EN_W_WB_PLUS:
        {
            fp->read_ptr = new_ptr;             //读位置
            fp->write_ptr = new_ptr;            //写位置
        }break;
        case EN_A_AB_PLUS:
        {
            fp->read_ptr = new_ptr;             //读位置
        }break;
        default: return false;
    }
    return true;
}

//----移动文件写指针-----------------------------------------------------------
//功能: 把一个文件的写指针移动到一个新位置
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      fp，目标文件指针
//      new_ptr，新的写指针
//返回: true=成功，false=有错误
//-----------------------------------------------------------------------------
bool_t __DFFSD_MoveFileWritePtr(struct tagStPTT_Flash *PTT_flash_tag,
                             FILE *fp,sint64_t new_ptr)
{
    switch (fp->open_mode)
    {
        case EN_R_RB:
        {
            return false;                       //只读文件，写位置无效
        }break;
        case EN_W_WB:
        {
            fp->write_ptr = new_ptr;            //写位置
        }break;
        case EN_A_AB:
        {
            fp->write_ptr = new_ptr;            //写位置
        }break;
        case EN_R_RB_PLUS:
        {
            fp->read_ptr = new_ptr;             //读位置
            fp->write_ptr = new_ptr;            //写位置
        }break;
        case EN_W_WB_PLUS:
        {
            fp->read_ptr = new_ptr;             //读位置
            fp->write_ptr = new_ptr;            //写位置
        }break;
        case EN_A_AB_PLUS:
        {
            fp->write_ptr = new_ptr;            //写位置
        }break;
        default: return false;
    }
    if(new_ptr > fp->file_size)       //写指针超出文件长度
    {
        //在FDT表中修改文件长度
       __DFFSD_UpdateFDT_FileSize(PTT_flash_tag,fp,fp->write_ptr);
        fp->file_size = fp->write_ptr;  //用写指针覆盖文件长度
    }
    return true;
}

//----写缓冲区写入flash--------------------------------------------------------
//功能: 把文件写缓冲区的数据写入flash中
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      fp，被操作的文件指针
//返回：实际写入flash的字节数
//------------
//更新记录:
// 1.日期: 2015/3/19
//   说明: 增加对文件时间属性的支持,增加函数__DFFSD_UpdateFileTime。
//   作者: 季兆林
//-----------------------------------------------------------------------------
uint32_t __DFFSD_WriteBufToFlash(struct tagStPTT_Flash *PTT_flash_tag,
                                    struct tagFileRsc *fp)
{
    struct tagFlashChip *chip;
    struct tagFileAccess access_tag;
    sint64_t wr_point;
    uint32_t completed = 0,write_len;
    uint32_t offset_start;
    uint32_t block_no,start_block,end_block,loop;
    uint32_t true_block,last_block;
    uint32_t buffed_data; //文件写缓冲区中的数据量和剩余数据量
    chip = PTT_flash_tag->chip;     //取分区(文件)所属芯片

    if(fp->p_write_buf != NULL)
    {
        buffed_data = Ring_Check(fp->p_write_buf); //检查写缓冲区中的数据量
        if(buffed_data == 0)        //写缓冲区没有数据
            return 0;
    }else
        return 0;       //没有写缓冲区
    wr_point = fp->write_ptr-buffed_data;   //计算开始写入数据的位置
    //计算起始写位置处于文件内第几块
    start_block = (u32)(wr_point /chip->block_size);
    offset_start = wr_point % chip->block_size; //计算起始偏移
    //计算终止读写位置处于文件内第几块
    end_block = (u32)((fp->write_ptr - 1)/chip->block_size);
    __DFFSD_GetAccessInfo(PTT_flash_tag,fp,&access_tag);
    //定位写入点的块号，分区内的相对块号。
    block_no = access_tag.write_block_no;
    last_block = access_tag.last_block_no;
/*★★★这个逻辑感觉有问题,当需要跨块操作时*/
    for(loop = start_block; loop <= end_block; loop++)
    {

        if(loop > start_block|| block_no == CN_LIMIT_UINT32)
        {//要写入里的数据，最多跨界一块.第一块，自动分配好了，后面的，都必须自己
        //分配
            //分配一个新块
            block_no = __DFFSD_AllocateBlock(PTT_flash_tag);
            loop =0xfffffffe;//作为下面追加数据的条件
            if(block_no == CN_LIMIT_UINT32) //分配失败(无空闲块)
                break;
        }
        //计算本次写入的数据长度
        if(buffed_data - completed > chip->block_size - offset_start)
            write_len = chip->block_size - offset_start;
        else
            write_len = buffed_data - completed;
        Ring_Read(fp->p_write_buf,chip->block_buf+offset_start,write_len);
        if(loop == 0xfffffffe)    //追加数据
        {
            if(offset_start == 0)   //待写入的块内尚无有效数据
            {
                //写入目标块
                true_block = __DFFSD_WriteNewBlock(PTT_flash_tag,block_no,
                                                        0,write_len);
                if(true_block != CN_LIMIT_UINT32)       //正确写入
                {
                    if(wr_point != 0)
                        __DFFSD_AddMAT_Item(PTT_flash_tag,last_block,true_block);
                }else       //写入错误
                    break;
            }else                   //虽是追加，但待写入的块已经有部分有效数据
            {
                //写入目标块
                true_block = __DFFSD_WriteAppendBlock(PTT_flash_tag,block_no,
                                                offset_start,write_len);
                if(true_block != CN_LIMIT_UINT32)
                {//正确写入
                    if(true_block != block_no)   //写入时发生了块替换
                        __DFFSD_UpdateMAT_Item(PTT_flash_tag,block_no,true_block);
                }else       //写入错误
                    break;
            }
            last_block = true_block;
            access_tag.true_size += write_len;
            chip->erase_block(PTT_flash_tag->PCRB_no
                              + PTT_flash_tag->start_block);
        }else   //在文件的中间写入，若需擦除就需要使用PCRB块进行保护
        {
            //写入目标块
            true_block = __DFFSD_WriteUpdateBlock(PTT_flash_tag,block_no,
                                            offset_start,write_len);
            if(true_block != CN_LIMIT_UINT32)   //正确写入
            {
                if(true_block != block_no)   //写入时发生了块替换
                    __DFFSD_UpdateMAT_Item(PTT_flash_tag,block_no,true_block);
            }else       //写入错误
                break;
            //确定下一循环写入点的块号，分区内的相对块号。
            block_no = PTT_flash_tag->DDR_MAT[block_no].next;
            chip->erase_block(PTT_flash_tag->PCRB_no
                              + PTT_flash_tag->start_block);
        }
        completed += write_len;
        wr_point += write_len;
        offset_start = 0;   //第二块开始，起始偏移地址肯定是0
    }
    __DFFSD_WriteDDR(PTT_flash_tag);//把DDR表的变化写入flash中(DDR表可能没变化)
    __DFFSD_UpdateFileTime(fp);
    return completed;
}

//----读操作前同步文件缓冲区---------------------------------------------------
//功能: 执行文件写操作之前，调用本函数同步缓冲区。
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      fp，目标文件指针
//      read_len，欲读取长度
//返回: true=成功，false=有错误
//特别说明: struct file_rsc中有home_left，通过它也是可以很快找到该文件所属的dbx
//          的，但home_left是左手句柄，而flash_driver处于文件系统的右手，应该禁
//          止它访问左手句柄，故需要文件系统提供PTT_flash_tag指针
//-----------------------------------------------------------------------------
bool_t __DFFSD_SyncBufBeforeRead(struct tagStPTT_Flash *PTT_flash_tag,
                              FILE *fp,uint32_t read_len)
{
    switch(fp->open_mode)
    {
        case EN_R_RB:       //只读文件，无需操作
        {
        }break;
        case EN_W_WB:       //只写文件，无需操作
        {
        }break;
        case EN_A_AB:       //只写文件，无需操作
        {
        }break;
        case EN_R_RB_PLUS:  //读写位置相同，若写缓冲区有数据，必须写到flash
        case EN_W_WB_PLUS:  //与en_r_rb_plus处理方式相同
        {
            __DFFSD_WriteBufToFlash(PTT_flash_tag,fp);
        }break;
        case EN_A_AB_PLUS:  //追加写，可读。如果读位置与写缓冲区有交错则把写缓
                            //冲区写入flash，否则什么也不干
        {
            if(fp->read_ptr + read_len > fp->write_ptr)
                __DFFSD_WriteBufToFlash(PTT_flash_tag,fp);
        }break;
        default:break;
    }
    return true;
}


//----写入flash文件------------------------------------------------------------
//功能：把数据写入文件，执行下列操作：
//      1、如果写缓冲区有足够的空间，把数据写入写缓冲区即可。
//      2、如果写缓冲区没有足够的空间，连同写缓冲区原有数据和输入缓冲区中的数据
//         一并写入flash
//      3、写入完成后检查读缓冲区是否需要同步并执行之。
//      4、检查FDT表和DDR表是否需要修改并执行之
//参数：buf，输入缓冲区
//      len，本次写入的数据量
//      fp，被操作的文件指针
//返回：本次写入的数据量，不包含写缓冲区中原有的数据
//注:   本函数并没有先调用__DFFSD_write_buf_to_flash函数，再写buf中的内容，那样
//      可能需要多一次调用flash写操作，效率低；而是把写缓冲区的数据与buf中的数据
//      合并一次写入flash.
//------------
//更新记录:
// 1.日期: 2015/3/19
//   说明: 增加对文件时间属性的支持,增加函数__DFFSD_UpdateFileTime。
//         todo:如何处理写入失败?
//   作者: 季兆林
//-----------------------------------------------------------------------------
uint32_t DFFSD_WriteFile(struct tagFileRsc *fp,uint8_t *buf,uint32_t len)
{
    struct tagFlashChip *chip;
    struct tagPTTDevice *PTT_device_tag;
    struct tagStPTT_Flash *PTT_flash_tag;
    struct tagFileAccess access_tag;
    uint32_t completed = 0,write_len,write_sum;
    sint64_t wr_point;
    uint32_t offset_start;
    uint32_t block_no,start_block,end_block,loop,temp;
    uint32_t true_block,last_block;
    uint32_t buffed_data,spare_len; //文件写缓冲区中的数据量和剩余数据量

    if((fp==NULL)||(len==0)||(buf==NULL))
        return 0;
    PTT_device_tag = (struct tagPTTDevice *)(fp->home_PTT);
    //分区指针
    PTT_flash_tag = (struct tagStPTT_Flash *)PTT_device_tag->PTT_medium_tag;
    chip = PTT_flash_tag->chip;     //取分区(文件)所属芯片
    buffed_data = Ring_Check(fp->p_write_buf); //检查缓冲区中的数据量
    if(Ring_Capacity(fp->p_write_buf) - buffed_data >= len)
    {//缓冲区内有足够的容量，直接把数据写到缓冲区即可。
        completed = Ring_Write(fp->p_write_buf,(uint8_t*)buf,len);
        completed += buffed_data;
    }
    else
    {
        //如果缓冲区没有足够的容量，则把本次需要写入的数据连同缓冲区原来的数据
        //一并写入flash，下面开始执行写入操作。
        //计算连缓冲区中的数据在一起，一共要写入flash的数据量
        write_sum = len + buffed_data;
        spare_len = buffed_data; //缓冲区剩余数据量
        wr_point = fp->write_ptr - buffed_data;
        //计算起始写位置处于文件内第几块和偏移量
        start_block = (u32)(wr_point /chip->block_size);
        offset_start = (u32)(wr_point % chip->block_size);
        //计算终止写位置处于文件内第几块
        end_block = (u32)((fp->write_ptr + len -1)/chip->block_size);
        __DFFSD_GetAccessInfo(PTT_flash_tag,fp,&access_tag);
        //定位写入点的块号，分区内的相对块号。
        block_no = access_tag.write_block_no;
        last_block = access_tag.last_block_no;
        for(loop = start_block; loop <= end_block; loop++)
        {
            if(((wr_point >= access_tag.true_size) && (wr_point != 0)))
            {
                //分配一个新块
                block_no = __DFFSD_AllocateBlock(PTT_flash_tag);
                if(block_no == CN_LIMIT_UINT32) //分配失败(无空闲块)
                    break;
            }
            //计算本次写入的数据长度
            if(write_sum - completed > chip->block_size - offset_start)
                write_len = chip->block_size - offset_start;
            else
                write_len = write_sum - completed;
            if(spare_len != 0)  //写缓冲区里还有数据
            {
                temp = Ring_Read(fp->p_write_buf,
                                 chip->block_buf+offset_start,write_len);
                spare_len -= temp;  //写缓冲区减去实际读出的数据量
                //实际读出的数据量比write_len少，说明本次读已经把写缓冲区读空。
                if(temp != write_len)
                {
                    //继续从输入缓冲区中拷贝数据
                    memcpy(chip->block_buf+temp+offset_start,
                                buf,write_len-temp);
                }
            }
            else   //写缓冲区已经空，从输入缓冲区拷贝数据
                memcpy(chip->block_buf+offset_start,
                                       buf+completed-buffed_data,write_len);
            if(wr_point >= access_tag.true_size)    //追加数据
            {
                if(offset_start == 0)   //待写入的块内尚无有效数据
                {
                    //写入目标块
                    true_block = __DFFSD_WriteNewBlock(PTT_flash_tag,block_no,
                                                            0,write_len);
                    if(true_block != CN_LIMIT_UINT32)
                    {//正确写入
                        if(wr_point != 0)
                            __DFFSD_AddMAT_Item(PTT_flash_tag,last_block,true_block);
                    }
                    else //写入错误
                        break;
                }
                else //虽是追加，但待写入的块已经有部分有效数据
                {
                    //写入目标块
                    true_block = __DFFSD_WriteAppendBlock(PTT_flash_tag,block_no,
                                                    offset_start,write_len);
                    if(true_block != CN_LIMIT_UINT32)
                    {//正确写入
                        if(true_block != block_no)   //写入时发生了块替换
                            __DFFSD_UpdateMAT_Item(PTT_flash_tag,block_no,true_block);
                    }
                    else //写入错误
                        break;
                }
                last_block = true_block;
                access_tag.true_size += write_len;
                wr_point += write_len;
                chip->erase_block(PTT_flash_tag->PCRB_no
                                  + PTT_flash_tag->start_block);
            }
            else //在文件的中间写入，若需擦除就需要使用PCRB块进行保护
            {
                //写入目标块
                true_block = __DFFSD_WriteUpdateBlock(PTT_flash_tag,block_no,
                                                offset_start,write_len);
                if(true_block != CN_LIMIT_UINT32)
                {//正确写入
                    if(true_block != block_no)   //写入时发生了块替换
                        __DFFSD_UpdateMAT_Item(PTT_flash_tag,block_no,true_block);
                }else       //写入错误
                    break;
                //确定下一循环写入点的块号，分区内的相对块号。
                block_no = PTT_flash_tag->DDR_MAT[block_no].next;
                wr_point += write_len;
                if(wr_point > access_tag.true_size)
                    access_tag.true_size = wr_point;
                chip->erase_block(PTT_flash_tag->PCRB_no
                                  + PTT_flash_tag->start_block);
            }
            completed += write_len;
            offset_start = 0;   //第二块开始，起始偏移地址肯定是0
        }
        __DFFSD_UpdateFileTime(fp);

    }
    if((completed - buffed_data) > 0) //写入有数据
    {
        __DFFSD_SyncBufAfterWrite(PTT_flash_tag,fp,completed-buffed_data);
        __DFFSD_MoveFileWritePtr(PTT_flash_tag,fp,
                                    fp->write_ptr + completed-buffed_data);
    }
    __DFFSD_WriteDDR(PTT_flash_tag);//如果DDR表有变化，写入flash中
    return completed - buffed_data;
}
//-----------------------------------------------------------------------------
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
uint32_t DFFSD_ReadFile(struct tagFileRsc *fp,uint8_t *buf,uint32_t len)
{
    struct tagFlashChip *chip;
    struct tagPTTDevice *PTT_device_tag;
    struct tagStPTT_Flash *PTT_flash_tag;
    uint32_t completed = 0,read_len,result_len;
    uint32_t offset_start;
    uint32_t block_no,start_block,end_block,loop;
    uint32_t read_result;
    if((fp==NULL)||(len==0)||(buf==NULL))
        return 0;
    PTT_device_tag = (struct tagPTTDevice *)(fp->home_PTT);
    //分区指针
    PTT_flash_tag = (struct tagStPTT_Flash *)PTT_device_tag->PTT_medium_tag;
    __DFFSD_SyncBufBeforeRead(PTT_flash_tag,fp,len);
    chip = PTT_flash_tag->chip;     //取分区(文件)所属芯片
    //计算起始读写位置处于文件内第几块
    start_block = (u32)(fp->read_ptr/chip->block_size);
    //计算终止读写位置处于文件内第几块
    end_block = (u32)((fp->read_ptr + len -1)/chip->block_size);
    offset_start = fp->read_ptr % chip->block_size;
    if(fp->read_ptr + len >fp->file_size)
        result_len = (u32)(fp->file_size - fp->read_ptr);
    else
        result_len = len;
    //定位读出点的块号，分区内的相对块号。
    block_no = __DFFSD_LocateBlock(PTT_flash_tag,fp,fp->read_ptr);
    for(loop = start_block; loop <= end_block; loop++)
    {
        if(result_len - completed > chip->block_size - offset_start)
            read_len = chip->block_size - offset_start;
        else
            read_len = result_len - completed;
        //读出目标块
        read_result =chip->read_data_with_ecc(
                                block_no + PTT_flash_tag->start_block,
                                offset_start,buf+completed,read_len);
        if(read_result == 2)    //读出错误，坏块
        {
            break;
        }
        completed += read_len;
        offset_start = 0;   //第二块开始，起始偏移地址肯定是0
        block_no = PTT_flash_tag->DDR_MAT[block_no].next;
    }
    fp->read_ptr += completed;
    return completed;
}

//----刷写缓冲区---------------------------------------------------------------
//功能: 把文件写缓冲区的数据写入flash
//参数：fp，被操作的文件指针
//返回：实际写入flash的字节数
//-----------------------------------------------------------------------------
uint32_t DFFSD_FlushFile(struct tagFileRsc *fp)
{
    struct tagPTTDevice *PTT_device_tag;
    struct tagStPTT_Flash *PTT_flash_tag;
    if(fp==NULL)
        return 0;
    PTT_device_tag = (struct tagPTTDevice *)(fp->home_PTT);
    //分区指针
    PTT_flash_tag = (struct tagStPTT_Flash *)PTT_device_tag->PTT_medium_tag;
    return __DFFSD_WriteBufToFlash(PTT_flash_tag,fp);
}

//----查询文件可读数据量-------------------------------------------------------
//功能：查询一个文件内有多少可读数据，对于固态文件（如flash文件）来说，就是文件
//      长度-当前指针，但流的标准定义并非一定如此，比如通信端子。而且，文件读写
//      指针如何表示与具体文件系统驱动的实现有关，文件系统的核心程序不应该做任何
//      假设。
//参数：fp，被操作的文件指针
//返回：文件可读数据的字节数
//-----------------------------------------------------------------------------
sint64_t DFFSD_QueryFileStocks(struct tagFileRsc *fp)
{
    struct tagPTTDevice *PTT_device_tag;
    struct tagStPTT_Flash *PTT_flash_tag;
    struct tagFdtInfo FDT_item;

    if(fp == NULL)
        return 0;
    PTT_device_tag = (struct tagPTTDevice *)(fp->home_PTT);
    PTT_flash_tag = (struct tagStPTT_Flash*)PTT_device_tag->PTT_medium_tag;
    __DFFSD_ReadFDT_Item(PTT_flash_tag,
                          (uint32_t)fp->file_medium_tag,&FDT_item);
    if(FDT_item.mattr & CN_FS_ATTR_DIRECTORY)
        return 0;       //目录不能读
    return fp->file_size - fp->read_ptr;
}


//----查询文件可写数据量-------------------------------------------------------
//功能：查询一个文件还能写入多少数据，对于固态文件（如flash文件），可能就是文件
//      柜的剩余空间，但流的标准定义并非一定如此，比如通信端子。而且，分区剩余
//      空间如何表示与具体文件系统驱动的实现有关，文件系统的核心程序不应该做任何
//      假设。不能认为PTT_flash_tag->free_sum就是文件仍可写入的数据量。
//      flash文件系统的计算方式是:
//      1、文件已分配的块的容量-文件尺寸。
//      2、加上分区剩余空间PTT_flash_tag->free_sum。
//      3、减去文件缓冲区中已有的数据量
//参数：fp，被操作的文件指针
//返回：文件还能写入的字节数
//-----------------------------------------------------------------------------
sint64_t DFFSD_QueryFileCubage(struct tagFileRsc *fp)
{
    struct tagPTTDevice *PTT_device_tag;
    struct tagStPTT_Flash *PTT_flash_tag;
    struct tagFlashChip *chip;
    struct tagFdtInfo FDT_item;
    if(fp == NULL)
        return 0;
    PTT_device_tag = (struct tagPTTDevice *)(fp->home_PTT);
    PTT_flash_tag = (struct tagStPTT_Flash*)PTT_device_tag->PTT_medium_tag;
    chip = PTT_flash_tag->chip;
    __DFFSD_ReadFDT_Item(PTT_flash_tag,
                          (uint32_t)fp->file_medium_tag,&FDT_item);
    if((FDT_item.mattr & CN_FS_ATTR_READ_ONLY)
                || (FDT_item.mattr & CN_FS_ATTR_DIRECTORY))
        return 0;       //只读文件或目录不能写入
/*
    //取文件尺寸
    size = pick_little_32bit(FDT_item.file_size,0)
            +((uint64_t)pick_little_32bit(FDT_item.file_size,1)<<32);
    if(size == 0)   //根据文件至少占用一块的原则，0尺寸文件也已经分配了一块
        size = chip->block_size;
    else
    {
        //文件必须占用整数块，下面计算已经分配但未使用的空间
        size = chip->block_size - size % chip->block_size;
        if(size == chip->block_size)
            size = 0;
    }
    buffed_size = Ring_Check(fp->p_write_buf);
*/      //本段为什么会被注释掉，当初漏了注释，先别删吧。

    return PTT_flash_tag->free_sum * chip->block_size;  // - buffed_size + size;
}

//----设置文件长度-------------------------------------------------------------
//功能：设置文件长度，短于实际长度将截尾，长于实际长度将追加空间。本函数的主要
//      作用在于在正式写入数据之前，预先分配存储器。
//参数：fp，被操作的文件指针
//      new_size，新的文件长度。
//返回：新的文件长度，若与new_size参数不等，一般是因为分区没有足够的容量导致
//------------
//更新记录:
// 1.日期: 2015/3/19
//   说明: 增加对文件时间属性的支持,增加函数__DFFSD_UpdateFileTime。
//   作者: 季兆林
//-----------------------------------------------------------------------------
sint64_t DFFSD_SetFileSize(struct tagFileRsc *fp,s64 new_size)
{
    struct tagPTTDevice *PTT_device_tag;
    sint64_t result,temp;
    uint32_t origin_blocks,new_blocks,last_block_no,next_block_no;
    uint32_t loop;
    struct tagStPTT_Flash *PTT_flash_tag;
    struct tagFlashChip *chip;
    struct tagFdtInfo FDT_item;
    //读父目录的FDT_item
    if(fp == NULL)
        return 0;
    if(new_size == fp->file_size)
        return new_size;    //新的文件长度与原长度相等，直接返回。
    PTT_device_tag = (struct tagPTTDevice *)(fp->home_PTT);
    PTT_flash_tag = (struct tagStPTT_Flash*)PTT_device_tag->PTT_medium_tag;
    chip = PTT_flash_tag->chip;
    __DFFSD_ReadFDT_Item(PTT_flash_tag,
                          (uint32_t)fp->file_medium_tag,&FDT_item);
    if((FDT_item.mattr & CN_FS_ATTR_READ_ONLY)
                || (FDT_item.mattr & CN_FS_ATTR_DIRECTORY))
        return 0;       //只读文件或目录不能执行设置长度的操作
    //下面两个if用于确定原文件占用的块数和新文件长度所需的块数
    if(fp->file_size == 0)
        origin_blocks = 1;
    else
        origin_blocks = (u32)((fp->file_size + chip->block_size -1)/chip->block_size);
    if(new_size < 0)
        new_size = 0;
    if(new_size == 0)
        new_blocks = 1;
    else
        new_blocks = (u32)((new_size + chip->block_size -1)/chip->block_size);
    last_block_no = __DFFSD_LastBlock(PTT_flash_tag,fp);
    if(origin_blocks < new_blocks)     //文件占用块数需增加
    {
        for(loop = origin_blocks; loop < new_blocks; loop++)
        {
            next_block_no = __DFFSD_AllocateBlock(PTT_flash_tag);
            if(next_block_no != CN_LIMIT_UINT32)
            {
                __DFFSD_AddMAT_Item(PTT_flash_tag,last_block_no,next_block_no);
                last_block_no = next_block_no;
            }else
                break;
        }
        if(loop == new_blocks)
            result = new_size;
        else
            result = (sint64_t)loop * chip->block_size;
    }
    else if(origin_blocks > new_blocks)     //文件占用块数需减少
    {
        result = new_size;
        for(loop = origin_blocks; loop > new_blocks; loop--)
        {
            next_block_no = last_block_no;
            last_block_no = PTT_flash_tag->DDR_MAT[next_block_no].previous;
            __DFFSD_FreeBlock(PTT_flash_tag,next_block_no);
        }
        //最后一块的向后指针指向自己
        PTT_flash_tag->DDR_MAT[last_block_no].next = last_block_no;
    }
    else //占用块数不变
        result = new_size;

    fp->file_size = result;
    //以下处理读写缓冲区，但并不处理读写指针，仅把非法指针(因文件缩短而超出文件
    //长度的指针位置)移到合法位置。
    if(fp->p_read_buf != NULL)      //读缓冲区指针非空，说明有读缓冲区
    {
        if(fp->read_ptr > result)   //读指针在文件尺寸后面
        {
            Ring_Flush(fp->p_read_buf); //清空读缓冲区
            fp->read_ptr = result;      //读指针回到文件末
        }
        else
        {
            temp = fp->read_ptr + Ring_Check(fp->p_read_buf);
            //读指针未超出文件长度但读缓冲区中数据超出文件长度，将超出部分砍掉
            if(temp > result)
            {
                Ring_SkipTail(fp->p_read_buf,(u32)(temp-result));
            }
        }
    }
    if(fp->p_write_buf != NULL)     //写缓冲区指针非空，说明有写缓冲区
    {
        if(fp->write_ptr > result)  //写指针超出文件长度
        {
            temp = result - fp->write_ptr;  //计算超出数量
            Ring_SkipTail(fp->p_write_buf,(u32)temp);   //砍掉超出文件长度的并且可能
                                                    //还在缓冲区中的数据。
            fp->write_ptr = result; //写指针回到文件末
        }
        else
        {
            //写指针没有超出文件长度，无需处理
        }
    }
    __DFFSD_UpdateFDT_FileSize(PTT_flash_tag,fp,result);
    __DFFSD_WriteDDR(PTT_flash_tag); //可能修改DDR,写入flash
    __DFFSD_UpdateFileTime(fp);
    return result;
}

//----设置文件指针-------------------------------------------------------------
//功能：把文件指针设置到指定的位置，并相应地同步读写缓冲区。
//参数：fp，被操作的文件指针
//      position的成员:
//          offset，移动的位置
//          whence，从何处开始计算移动长度，参见SEEK_CUR等的定义
//返回：0=成功，1=失败
//-----------------------------------------------------------------------------
uint32_t DFFSD_SeekFile(struct tagFileRsc *fp,struct tagSeekPara *position)
{
    struct tagPTTDevice *PTT_device_tag;
    struct tagStPTT_Flash *PTT_flash_tag;
    sint64_t new_position,temp;
    uint32_t buffed_size;
    uint8_t  whence;
    if((position == NULL) ||(fp == NULL))
        return 1;
    PTT_device_tag = (struct tagPTTDevice *)(fp->home_PTT);
    whence = position->whence;
    PTT_flash_tag = (struct tagStPTT_Flash*)PTT_device_tag->PTT_medium_tag;
    switch (fp->open_mode)
    {
        case EN_R_RB:           //只读方式，需移动读指针和读缓冲区
        case EN_A_AB_PLUS:      //写追加方式，写指针不能变，读指针的方式与
                                  //只读文件一致
        {
            if(whence == SEEK_SET)
                new_position = position->offset;
            else if(whence == SEEK_CUR)
                new_position = position->offset + fp->read_ptr;
            else if(whence == SEEK_END)
                new_position = fp->file_size + position->offset;
            else
                return 1;
            if(fp->read_ptr == new_position)
                return 0;
            temp = new_position - fp->read_ptr;
            buffed_size = Ring_Check(fp->p_read_buf);
            if((temp > 0) && (temp < buffed_size))
                //往后移且移动量比缓冲区中的数据少
                Ring_DumbRead(fp->p_read_buf,(ptu32_t)temp);
            else
                Ring_Flush(fp->p_read_buf); //往前移或者移动量大于缓冲区中数据量
            if(new_position > fp->file_size)
                fp->read_ptr = fp->file_size;
            else if(new_position < 0)
                fp->read_ptr = 0;
            else
                fp->read_ptr = new_position;
        }break;
        case EN_W_WB:   //只写文件，需移动写指针
        {
            if(whence == SEEK_SET)
                new_position = position->offset;
            else if(whence == SEEK_CUR)
                new_position = position->offset + fp->write_ptr;
            else if(whence == SEEK_END)
                new_position = fp->file_size - position->offset;
            else
                return 1;
            if(fp->write_ptr == new_position)
                return 1;
            __DFFSD_WriteBufToFlash(PTT_flash_tag,fp);
            if(new_position > fp->file_size)
            {
                //延长文件，分配更多存储器，但不填充新增加的存储器
                if(DFFSD_SetFileSize(fp,new_position) != new_position)
                    return 1;
                else
                {
                    fp->write_ptr = new_position;   //设置写指针
                }
            }else if(new_position < 0)
                fp->write_ptr = 0;
            else
                fp->write_ptr = new_position;
        }break;
        case EN_A_AB:   //追加写方式，seek操作无效
        {
        }break;
        case EN_R_RB_PLUS:  //这两者都是可读写方式，读写指针重合，处理方式相同
        case EN_W_WB_PLUS:
        {
            if(whence == SEEK_SET)
                new_position = position->offset;
            else if(whence == SEEK_CUR)
                new_position = position->offset + fp->read_ptr;
            else if(whence == SEEK_END)
                new_position = fp->file_size + position->offset;
            else
                return 1;

            if(fp->read_ptr == new_position)
                return 0;
            __DFFSD_WriteBufToFlash(PTT_flash_tag,fp);//写缓冲区数据必须写入

            if(new_position > fp->file_size)    //新位置超出文件长度
            {
                //延长文件，分配更多存储器，但不初始化新增加的存储器
                if(DFFSD_SetFileSize(fp,new_position) != new_position)
                    return 1;
                else
                {
                    fp->write_ptr = new_position;   //设置写指针
                    fp->read_ptr = new_position;    //设置读指针
                }
            }else if(new_position < 0)  //新位置小于0，设为0
            {
                fp->write_ptr = 0;      //写缓冲区已经写入，处理写指针即可
                fp->read_ptr = 0;    //设置读指针，读指针和读缓冲区可以独立设置
                Ring_Flush(fp->p_read_buf); //清空读缓冲区
            }
            else
            {
                fp->write_ptr = new_position;//写缓冲区已经写入，处理写指针即可
                fp->read_ptr = new_position;    //设置读指针
                if(new_position < fp->read_ptr) //新位置使读指针后移(朝文件首)
                {
                    Ring_Flush(fp->p_read_buf); //缓冲区中数据全部作废
                }else
                {
                    //计算缓冲区结束处处于文件的什么位置
                    temp = fp->read_ptr + Ring_Check(fp->p_read_buf);
                    if(new_position < temp)
                    {//读指针前移，但未超出读缓冲区覆盖的范围。
                        //缓冲区中数据部分作废
                        Ring_DumbRead(fp->p_read_buf,(u32)(temp-new_position));
                    }else
                    {//读指针前移，但超出读缓冲区覆盖的范围。
                        Ring_Flush(fp->p_read_buf);     //缓冲区中数据全部作废
                    }
                }
            }
        }break;
        default: return 1;
    }
    return 0;
}

//----新文件分配文件缓冲区-----------------------------------------------------
//功能: 根据新建文件的打开模式，为文件分配缓冲区。
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      fp，由文件系统提供的文件名指针
//      mode，文件打来模式
//返回: true = 相等，false=不相等
//----------------------------------------------------------------------------
bool_t __DFFSD_allocate_file_buf_create(struct tagStPTT_Flash *PTT_flash_tag,
                              struct tagFileRsc *fp,
                              enum _FILE_OPEN_MODE_ mode)
{
    struct tagFlashChip *chip;
    uint32_t read_buf_len;
    uint32_t write_buf_len;
    uint8_t *buf;
    uint8_t *buf1;
    chip = PTT_flash_tag->chip;
    read_buf_len = chip->PTT_read_buf_size;
    write_buf_len = chip->PTT_write_buf_size;
    if(fp->file_attr.bits.folder)     //FDT条目是一个目录
    {
        Ring_Init(&fp->read_rw_buf,NULL,0);     //目录的读缓冲区设为空
        Ring_Init(&fp->write_buf,NULL,0);       //目录的写缓冲区设为空
        fp->p_read_buf = NULL;
        fp->p_write_buf = NULL;
        fp->read_ptr = 0;
        fp->write_ptr = 0;
        fp->file_size = 0;
    }else       //是一个文件，需要根据mode分配读写缓冲区
    {
        switch(mode)
        {
            case EN_R_RB:
            {
                buf =(uint8_t*)M_MallocLcHeap(read_buf_len,chip->ChipHeap,0);//本模式只有读缓冲区
                if(buf == NULL)
                    return false;           //缓冲区分配失败，返回错误
                Ring_Init(&fp->read_rw_buf,buf,read_buf_len);   //初始化读缓冲区
                Ring_Init(&fp->write_buf,NULL,0);           //初始化写缓冲区为空
                fp->p_read_buf = &fp->read_rw_buf;  //读缓冲区指针赋值
                fp->p_write_buf = NULL;             //写缓冲区指针置空
                fp->read_ptr = 0;                   //读位置置0
                fp->write_ptr = 0;                  //写位置置0
            }break;
            case EN_W_WB:
            case EN_A_AB:
            {
                buf =(uint8_t*)M_MallocLcHeap(write_buf_len,chip->ChipHeap,0);//本模式只有写缓冲区
                if(buf == NULL)
                    return false;           //缓冲区分配失败，返回错误
                Ring_Init(&fp->read_rw_buf,NULL,0);         //初始化读缓冲区为空
                Ring_Init(&fp->write_buf,buf,write_buf_len);//初始化写缓冲区
                fp->p_read_buf = NULL;              //读缓冲区指针置空
                fp->p_write_buf = &fp->write_buf;   //写缓冲区指针赋值
                fp->read_ptr = 0;                   //读位置置0
                fp->write_ptr = 0;                  //写位置置0
            }break;
            case EN_R_RB_PLUS:
            case EN_W_WB_PLUS:
            case EN_A_AB_PLUS:
            {
                buf = (uint8_t*)M_MallocLcHeap(read_buf_len,chip->ChipHeap,0);      //分配读缓冲区
                if(buf == NULL)
                    return false;           //缓冲区分配失败，返回错误
                buf1 = (uint8_t*)M_MallocLcHeap(write_buf_len,chip->ChipHeap,0);    //分配写缓冲区
                if(buf1 == NULL)
                {

                    free(buf);
                    return false;           //缓冲区分配失败，返回错误
                }
                Ring_Init(&fp->read_rw_buf,buf,read_buf_len);   //初始化读缓冲区
                Ring_Init(&fp->write_buf,buf1,write_buf_len);   //初始化写缓冲区
                fp->p_read_buf = &fp->read_rw_buf;      //设置读缓冲区指针
                fp->p_write_buf = &fp->write_buf;       //设置写缓冲区指针
                fp->read_ptr = 0;                       //读位置置0
                fp->write_ptr = 0;                      //写位置置0
            }break;
            default:break;
        }
    }
    return true;
}

//----建立项目（文件/目录）----------------------------------------------------
//功能：建立文件(目录)，attr参数将表明建立目录还是文件。如果建立文件，则同时分配
//      1块存储器。删除文件内容使文件长度为0时，也不会把最后一块释放。总之，任何
//      情况下，文件至少占用1块存储器.
//      （文件/目录）建立后，同时打开它，新打开的文件，结果在result中。
//参数：name，目录（文件）名
//      parent，父目录指针。
//      attr，新文件（目录）的属性
//返回：true=成功，false=失败，一般是因为分区没有足够的容量导致
//特注：本函数不检查重名，由调用者保证。作为存储介质，专守存储数据的职责，是否
//      允许重名是上一层的责任。
//------------
//更新记录:
// 1.日期: 2015/3/19
//   说明: 增加对文件时间属性支持
//   作者: 季兆林
//-----------------------------------------------------------------------------
bool_t DFFSD_CreateItem(char *name,struct tagFileRsc *parent,
                         struct tagFileRsc *result,union file_attrs attr,
                         enum _FILE_OPEN_MODE_ mode)
{
    struct tagPTTDevice *PTT_device_tag;
    struct tagFlashChip *chip;
    uint32_t FDT_item_no_parent,FDT_item_no_eldest,FDT_item_no_least;
    uint32_t FDT_item_no_me,block_me,temp;
    struct tagFdtInfo FDT_item_me,FDT_item_parent,FDT_item_eldest,FDT_item_least;
    struct tagStPTT_Flash *PTT_flash_tag;
    if(parent == NULL)
        return false;
    PTT_device_tag =(struct tagPTTDevice *)(parent->home_PTT);
    PTT_flash_tag = (struct tagStPTT_Flash*)PTT_device_tag->PTT_medium_tag;
    chip = PTT_flash_tag->chip;
    FDT_item_no_parent = (uint32_t)parent->file_medium_tag;
    //读父目录的FDT条目
    __DFFSD_ReadFDT_Item(PTT_flash_tag,FDT_item_no_parent,&FDT_item_parent);
    if(!(FDT_item_parent.mattr & CN_FS_ATTR_DIRECTORY))
        return false; //parent不是一个目录，而是文件
    FDT_item_no_me = __DFFSD_AllocateFDTItem(PTT_flash_tag);//分配一个FDT条目
    //debug 要不要对分配的FDT进行检测，有可能已经被使用了。如果被使用了，说明
    //FDT表乱了，不能挽救了。所以不能让分配的出现问题。这就必须在创建的时候，小心
    //再小心，千万不能建立成功了，而没有从空闲链表弄好。
    if(FDT_item_no_me == CN_LIMIT_UINT32)   //分配不成功
        return false;
    if(FDT_item_no_parent == 0)
    {
        //父目录是根目录，分配时FDT条目时会修改根目录条目，故需重读
        __DFFSD_ReadFDT_Item(PTT_flash_tag,0,&FDT_item_parent);
    }
    if( ! attr.bits.folder)//创建的item是文件
    {
        block_me = __DFFSD_AllocateBlock(PTT_flash_tag);
        if(block_me == CN_LIMIT_UINT32)//存储块申请失败
        {
            __DFFSD_FreeFDTItem(PTT_flash_tag,FDT_item_no_me);
            return false;
        }

        fill_little_32bit(FDT_item_me.fstart_dson, 0, block_me);
        PTT_flash_tag->DDR_MAT[block_me].attr = cn_file_block_start;
        PTT_flash_tag->DDR_MAT[block_me].previous = FDT_item_no_me;
        PTT_flash_tag->DDR_MAT[block_me].next = block_me;
        //计算last_block_no在MAT表中位置处在DDR表中是第几块
        temp = __DFFSD_MAT_Start(PTT_flash_tag);
        temp = (temp + block_me*4*2) / chip->block_size;
        //DDR_bak中标记该块为未写入
        PTT_flash_tag->writed_DDR_bak[temp] = false;
        //DDR_main中标记该块为未写入
        PTT_flash_tag->writed_DDR_main[temp] = false;

    }
    else
    {
        block_me = CN_LIMIT_UINT32;
        fill_little_32bit(FDT_item_me.fstart_dson,0,CN_LIMIT_UINT32);
    }
    __DFFSD_AsmFDT_Item(&FDT_item_me,attr,name);
    fill_little_32bit(FDT_item_me.parent,0,FDT_item_no_parent);
    FDT_item_no_eldest = pick_little_32bit(FDT_item_parent.fstart_dson,0);
    if(FDT_item_no_eldest == CN_LIMIT_UINT32)   //父目录是空目录
    {
        //构建新建目录项
        fill_little_32bit(FDT_item_me.previous,0,FDT_item_no_me);
        fill_little_32bit(FDT_item_me.next,0,FDT_item_no_me);
        //新建目录项写入flash
        __DFFSD_WriteFDT_Item(PTT_flash_tag,FDT_item_no_me,&FDT_item_me);
        //父目录项的子项指向新建目录项
        fill_little_32bit(FDT_item_parent.fstart_dson,0,FDT_item_no_me);
    }
    else //父目录项有子项(包括目录和文件)
    {
        __DFFSD_ReadFDT_Item(PTT_flash_tag,FDT_item_no_eldest,&FDT_item_eldest);
        FDT_item_no_least = pick_little_32bit(FDT_item_eldest.previous,0);
        //构建新建目录项
        fill_little_32bit(FDT_item_me.previous,0,FDT_item_no_least);
        fill_little_32bit(FDT_item_me.next,0,FDT_item_no_eldest);
        //新建目录项写入flash
        __DFFSD_WriteFDT_Item(PTT_flash_tag,FDT_item_no_me,&FDT_item_me);
        if(FDT_item_no_least == FDT_item_no_eldest) //父目录项只有一个子项
        {
            //该子项的前后指针均指向新建目录项
            fill_little_32bit(FDT_item_eldest.previous,0,FDT_item_no_me);
            fill_little_32bit(FDT_item_eldest.next,0,FDT_item_no_me);
            //该子项写回flash
            __DFFSD_WriteFDT_Item(PTT_flash_tag,
                                   FDT_item_no_eldest,&FDT_item_eldest);
        }else       //父目录项有多个子项
        {
            //第一个子项的前指针指向新建目录项
            fill_little_32bit(FDT_item_eldest.previous,0,FDT_item_no_me);
            //第一个子项写回flash
            __DFFSD_WriteFDT_Item(PTT_flash_tag,
                                   FDT_item_no_eldest,&FDT_item_eldest);
            //读出最后一个子项
            __DFFSD_ReadFDT_Item(PTT_flash_tag,
                                  FDT_item_no_least,&FDT_item_least);
            //新建子项的后指针指向新建目录项
            fill_little_32bit(FDT_item_least.next,0,FDT_item_no_me);
            //最后一个子项写回flash
            __DFFSD_WriteFDT_Item(PTT_flash_tag,
                                   FDT_item_no_least,&FDT_item_least);
        }
    }
    //更新上层目录的时间属性
    __DFFSD_UpdateItemTime(&FDT_item_parent);
    __DFFSD_WriteFDT_Item(PTT_flash_tag, FDT_item_no_parent, &FDT_item_parent);
    __DFFSD_WriteDDR(PTT_flash_tag);
    //更新item对应的文件对象时间
    result->CreateTime = FDT_item_me.CreateTime;
    result->ModTime = FDT_item_me.ModTime;
    parent->CreateTime = FDT_item_parent.CreateTime;
    parent->ModTime = FDT_item_parent.ModTime;
    result->file_attr = attr;

    //name是模块内部提供的字符串指针，已经经过字符串长度合法性检查
    if(strlen(name) <= CN_FLASH_FILE_NAME_LIMIT)//读取分区名字
        strcpy(result->name,name);
    else
    {
        memcpy(result->name,name,CN_FLASH_FILE_NAME_LIMIT);
        result->name[CN_FLASH_FILE_NAME_LIMIT] = '\0';
    }
    result->file_size = 0;
    result->file_medium_tag = (ptu32_t)FDT_item_no_me;
    result->file_semp = NULL;
    if(__DFFSD_allocate_file_buf_create(PTT_flash_tag,result,mode))//分配文件缓冲区
    {
        return true;
    }else
        return false;
}

//----移动文件-----------------------------------------------------------------
//功能：把一个文件从一个目录移动到另一个目录
//参数：name，目录（文件）名
//      parent，父目录指针。
//      attr，新文件（目录）的属性
//返回：true=成功，false=失败，
//更新记录:
// 1.日期: 2015/3/19
//   说明: 增加对文件时间属性的支持(未完成)
//   作者: 季兆林
//-----------------------------------------------------------------------------
bool_t DFFSD_MoveFile(struct tagFileRsc *src_fp, struct tagFileRsc *dest_fp)
{
    struct tagPTTDevice *PTT_device_tag;
    uint32_t src_FDT_item_no,dest_FDT_item_no,dest_FDT_item_no_parent;
    uint32_t FDT_item_no_previous,FDT_item_no_next;
    struct tagFdtInfo src_FDT_item,dest_FDT_item;
    struct tagFdtInfo FDT_item_previous,FDT_item_next;
    struct tagStPTT_Flash *PTT_flash_tag;

    if(dest_fp == NULL || src_fp == NULL)
        return false;
    PTT_device_tag =(struct tagPTTDevice *)(src_fp->home_PTT);
    PTT_flash_tag = (struct tagStPTT_Flash*)PTT_device_tag->PTT_medium_tag;

    src_FDT_item_no = (uint32_t)src_fp->file_medium_tag;
    dest_FDT_item_no = (uint32_t)dest_fp->file_medium_tag;
    //读父目录的FDT条目
    __DFFSD_ReadFDT_Item(PTT_flash_tag,src_FDT_item_no,&src_FDT_item);
    __DFFSD_ReadFDT_Item(PTT_flash_tag,dest_FDT_item_no,&dest_FDT_item);
    if((src_FDT_item.mattr & CN_FS_ATTR_DIRECTORY)||
                                (dest_FDT_item.mattr & CN_FS_ATTR_DIRECTORY))
        return false; //是目录

    dest_FDT_item_no_parent = pick_little_32bit(dest_FDT_item.parent,0);
    fill_little_32bit(src_FDT_item.parent,0,dest_FDT_item_no_parent);
    __DFFSD_WriteFDT_Item(PTT_flash_tag,src_FDT_item_no,&src_FDT_item);



    FDT_item_no_previous = pick_little_32bit(dest_FDT_item.previous,0);
     FDT_item_no_next = pick_little_32bit(dest_FDT_item.next,0);
    //被移动的只有一项
    if((FDT_item_no_previous == src_FDT_item_no)&&(FDT_item_no_next == src_FDT_item_no))
    {
        __DFFSD_FreeFDTItem(PTT_flash_tag,dest_FDT_item_no);
                //构建新建目录项
        fill_little_32bit(src_FDT_item.previous,0,src_FDT_item_no);
        fill_little_32bit(src_FDT_item.next,0,src_FDT_item_no);
        //新建目录项写入flash
        __DFFSD_WriteFDT_Item(PTT_flash_tag,src_FDT_item_no,&src_FDT_item);
        //只有一项，没什么好处理
    }
    else
    {
        if((FDT_item_no_previous != src_FDT_item_no)&&(FDT_item_no_previous==FDT_item_no_next))
        {//被移动的，有一项兄弟目录（文件）
            //把另外一项的前指针和后指针，都指向自己，FDT_item_no_previous和FDT_item_no_next
            //是同一项，随便读一项
             __DFFSD_ReadFDT_Item(PTT_flash_tag,
                                  FDT_item_no_previous,&FDT_item_previous);
            fill_little_32bit(FDT_item_previous.previous,0,FDT_item_no_previous);
            fill_little_32bit(FDT_item_previous.next,0,FDT_item_no_previous);
           __DFFSD_WriteFDT_Item(PTT_flash_tag,FDT_item_no_previous,&FDT_item_previous);
        }
        else
        {
            __DFFSD_ReadFDT_Item(PTT_flash_tag,
                                  FDT_item_no_previous,&FDT_item_previous);
            __DFFSD_ReadFDT_Item(PTT_flash_tag,
                                  FDT_item_no_next,&FDT_item_next);
            //被移动项的前项目的后指针，指向被移动项的后项目
            fill_little_32bit(FDT_item_previous.next,0,FDT_item_no_next);
            //被移动项的后项目的前指针，指向被移动项的前项目
            fill_little_32bit(FDT_item_next.previous,0,FDT_item_no_previous);
            __DFFSD_WriteFDT_Item(PTT_flash_tag,FDT_item_no_previous,&FDT_item_previous);
            __DFFSD_WriteFDT_Item(PTT_flash_tag,FDT_item_no_next,&FDT_item_next);
        }
    }


    FDT_item_no_previous = pick_little_32bit(src_FDT_item.previous,0);
     FDT_item_no_next = pick_little_32bit(src_FDT_item.next,0);
    //被移动的只有一项
    if((FDT_item_no_previous == src_FDT_item_no)&&(FDT_item_no_next == src_FDT_item_no))
    {
        //只有一项，没什么好处理
    }
    else
    {
        if((FDT_item_no_previous != src_FDT_item_no)&&(FDT_item_no_previous==FDT_item_no_next))
        {//被移动的，有一项兄弟目录（文件）
            //把另外一项的前指针和后指针，都指向自己，FDT_item_no_previous和FDT_item_no_next
            //是同一项，随便读一项
             __DFFSD_ReadFDT_Item(PTT_flash_tag,
                                  FDT_item_no_previous,&FDT_item_previous);
            fill_little_32bit(FDT_item_previous.previous,0,FDT_item_no_previous);
            fill_little_32bit(FDT_item_previous.next,0,FDT_item_no_previous);
           __DFFSD_WriteFDT_Item(PTT_flash_tag,FDT_item_no_previous,&FDT_item_previous);
        }
        else
        {
            __DFFSD_ReadFDT_Item(PTT_flash_tag,
                                  FDT_item_no_previous,&FDT_item_previous);
            __DFFSD_ReadFDT_Item(PTT_flash_tag,
                                  FDT_item_no_next,&FDT_item_next);
            //被移动项的前项目的后指针，指向被移动项的后项目
            fill_little_32bit(FDT_item_previous.next,0,FDT_item_no_next);
            //被移动项的后项目的前指针，指向被移动项的前项目
            fill_little_32bit(FDT_item_next.previous,0,FDT_item_no_previous);
            __DFFSD_WriteFDT_Item(PTT_flash_tag,FDT_item_no_previous,&FDT_item_previous);
            __DFFSD_WriteFDT_Item(PTT_flash_tag,FDT_item_no_next,&FDT_item_next);
        }
    }

    __DFFSD_WriteDDR(PTT_flash_tag);//ddr也许改变，若无改变，不会真的写
    return true;
}

//----检查文件夹的子文件（目录）数---------------------------------------------
//功能：检查文件夹的子文件（目录）数，不包含子目录的子条目
//参数：PTT_device_tag，被操作的分区指针，通用结构，非flash专用。
//      fp，目标文件夹的文件指针
//返回：子文件（目录）数
//-----------------------------------------------------------------------------
uint32_t DFFSD_CheckFolder(struct tagFileRsc *parent)
{
    struct tagPTTDevice *PTT_device_tag;
    struct tagFdtInfo FDT_item;
    struct tagStPTT_Flash *PTT_flash_tag;
    uint32_t FDT_item_no,FDT_item_no_next;
    uint32_t result;
    if(parent == NULL)
        return 0;
    if( ! parent->file_attr.bits.folder)      //这是文件而不是目录
        return 0;
    result = 0;
    PTT_device_tag = (struct tagPTTDevice *)(parent->home_PTT);
    PTT_flash_tag = (struct tagStPTT_Flash *)PTT_device_tag->PTT_medium_tag;
    FDT_item_no = (uint32_t)parent->file_medium_tag;
    __DFFSD_ReadFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);
    //第一个子条目的FDT条目号
    FDT_item_no = pick_little_32bit(FDT_item.fstart_dson,0);
    if(FDT_item_no == CN_LIMIT_UINT32)
        return 0;
    FDT_item_no_next = FDT_item_no;
    do
    {
        result++;
        __DFFSD_ReadFDT_Item(PTT_flash_tag,FDT_item_no_next,&FDT_item);
        FDT_item_no_next = pick_little_32bit(FDT_item.next,0);
    }while(FDT_item_no != FDT_item_no_next);
    return result;
}

//----删除项目（文件/目录）---------------------------------------------------
//功能：删除一个文件(目录)，只能删除空目录或已经关闭的文件。
//参数：fp，被删除的目录（文件）指针
//返回：true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t DFFSD_RemoveFile(struct tagFileRsc *fp)
{
    struct tagFdtInfo FDT_item;
    struct tagPTTDevice *PTT_device_tag;
    struct tagStPTT_Flash *PTT_flash_tag;
    struct tagFlashChip *chip;
    uint32_t FDT_item_no,loop,block_no,block_no_next,blocks;
    if(fp == NULL)
        return false;
    if(( ! fp->file_attr.bits.folder) && (fp->open_counter != 0))
        return false;   //fp是一个已经打开的文件
    if(fp->file_attr.bits.folder)
    {
        if(DFFSD_CheckFolder(fp) != 0)
            return false;   //是一个非空目录
    }
    PTT_device_tag = (struct tagPTTDevice *)(fp->home_PTT);
    PTT_flash_tag = (struct tagStPTT_Flash *)PTT_device_tag->PTT_medium_tag;
    chip = PTT_flash_tag->chip;
    FDT_item_no = (uint32_t)fp->file_medium_tag;
    if( ! fp->file_attr.bits.folder) //是个文件，需要释放文件占用的存储器
    {
        if(fp->file_size == 0)  //0长度文件也占用1块
            blocks = 1;
        else
            blocks = (u32)((fp->file_size + (s64)chip->block_size -1) / chip->block_size);
        __DFFSD_ReadFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);//取目录项
        block_no = pick_little_32bit(FDT_item.fstart_dson,0);//取文件首块块号
        for(loop = 0; loop < blocks; loop++)
        {
            //保存下一块块号
            block_no_next = PTT_flash_tag->DDR_MAT[block_no].next;
            __DFFSD_FreeBlock(PTT_flash_tag,block_no); //释放一块
            block_no = block_no_next;
        }
    }
    __DFFSD_FreeFDTItem(PTT_flash_tag,FDT_item_no); //释放FDT项

    __DFFSD_WriteDDR(PTT_flash_tag);
    return true;
}

//----遍历一个目录的子项-------------------------------------------------------
//功能: 从current_file开始,获取下parent的下一个文件的指针,直到parent
//      的子项搜索完毕.
//参数: parent,需要搜索的树枝的祖先结点
//      current_file,当前搜索位置,空则表示第一次查找
//返回: 搜索到的子项指针(可能是文件或目录)
//备注: 这个函数，可以作为windows下 FindFirstFile等三个函数的配合遍历整
//      个目录下的文件
//      第一次遍历时(以current==NULL)为标志，系统将为struct file_rsc结构分配内存，
//      最后一次，将释放内存。如果用户正确地调用DFFSD_item_traversal_son遍历完
//      整个目录，内存管理将完全正常，否则，为struct file_rsc结构分配的内存将需
//      要用户手动释放
//---------------------------------------------------------------------
struct tagFileRsc * DFFSD_ItemTraversalSon(struct tagFileRsc *parent,
                                           struct tagFileRsc *current)
{
    uint32_t FDT_item_no,first_FDT_item_no;
    struct tagFdtInfo FDT_item;
    struct tagPTTDevice *PTT_device_tag;
    struct tagStPTT_Flash *PTT_flash_tag;

    if(parent == NULL)
        return NULL;
    if(!parent->file_attr.bits.folder)       //父目录是个文件。
        return NULL;
    if(parent->file_medium_tag == CN_LIMIT_UINT32)
        return NULL;

    PTT_device_tag =(struct tagPTTDevice *)(parent->home_PTT);
    PTT_flash_tag = (struct tagStPTT_Flash*)PTT_device_tag->PTT_medium_tag;
       __DFFSD_ReadFDT_Item(PTT_flash_tag,parent->file_medium_tag,&FDT_item);
    first_FDT_item_no = pick_little_32bit(FDT_item.fstart_dson,0);
    if(first_FDT_item_no == CN_LIMIT_UINT32)    //父目录是个空目录
        return NULL;

    if(current == NULL)    //如果curent_file是空的，返回第一个子项
    {
        current = M_MallocLc(sizeof(struct tagFileRsc),CN_TIMEOUT_FOREVER);
        if(current == NULL)
            return NULL;
        __DFFSD_ReadFDT_Item(PTT_flash_tag,first_FDT_item_no,&FDT_item);
        __DFFSD_UasmFDT_Item(PTT_flash_tag,current,&FDT_item);
        current->file_medium_tag = (ptu32_t)first_FDT_item_no;
    }
    else if(current->file_medium_tag != CN_LIMIT_UINT32)
    {
        __DFFSD_ReadFDT_Item(PTT_flash_tag,current->file_medium_tag,&FDT_item);
        //parent是cn_limit_uint32,说明current_file对应的文件已经被删除
        if(pick_little_32bit(FDT_item.parent,0) == CN_LIMIT_UINT32)
        {
            //当前文件被删除，取第一个子项
            __DFFSD_ReadFDT_Item(PTT_flash_tag,first_FDT_item_no,&FDT_item);
            __DFFSD_UasmFDT_Item(PTT_flash_tag,current,&FDT_item);
            current->file_medium_tag = (ptu32_t)first_FDT_item_no;
        }
        else
        {
              FDT_item_no = pick_little_32bit(FDT_item.next,0);
            if(FDT_item_no == first_FDT_item_no)    //current_file已经是最后一个
            {
                free(current);
                return NULL;
            }
            __DFFSD_ReadFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);
            __DFFSD_UasmFDT_Item(PTT_flash_tag,current,&FDT_item);
            current->file_medium_tag = (ptu32_t)FDT_item_no;
        }
    }
    else
    {
        return NULL;    //当前位置非法
    }
    return current;
}


//----比较名字-----------------------------------------------------------------
//功能: 比较两个文件(目录)名字符串是否相等，因文件系统支持255字符，而flash
//      driver只支持cn_file_name_len个字符，因此当fs_name的长度超过
//      cn_file_name_len时将截尾比较。若截尾后长度仍不相等，直接返回false，若长
//      度相等，则逐字符比较，全等返回true，否则返回false。
//参数: fs_name，由文件系统提供的文件名指针
//      FDT_name，FDT表中读取的文件名指针
//返回: true = 相等，false=不相等
//----------------------------------------------------------------------------
bool_t __DFFSD_CmpFileName(char *fs_name,char *FDT_name)
{
    uint32_t fs_len,FDT_len;
    //fs_name是已经经过字符串长度合法性检查的指针
    fs_len = strlen(fs_name);
    //FDT_name是已经经过字符串长度合法性检查的指针
    FDT_len = strlen(FDT_name);
    if(fs_len > CN_FLASH_FILE_NAME_LIMIT)
        fs_len = CN_FLASH_FILE_NAME_LIMIT;
    if(fs_len != FDT_len)
        return false;
    fs_len = fs_len > FDT_len? FDT_len:fs_len;
    for(FDT_len = 0; FDT_len < fs_len; FDT_len++)
        if(fs_name[FDT_len] != FDT_name[FDT_len])
            return false;
    return true;
}

//----分配文件缓冲区-----------------------------------------------------------
//功能: 根据文件的打开模式，为文件分配缓冲区。
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      fp，由文件系统提供的文件名指针
//      mode，文件打来模式
//返回: true = 相等，false=不相等
//----------------------------------------------------------------------------
bool_t __DFFSD_allocate_file_buf(struct tagStPTT_Flash *PTT_flash_tag,
                              struct tagFileRsc *fp,
                              enum _FILE_OPEN_MODE_ mode)
{
    struct tagFlashChip *chip;
    uint32_t read_buf_len;
    uint32_t write_buf_len;
    uint8_t *buf;
    uint8_t *buf1;
    chip = PTT_flash_tag->chip;
    read_buf_len = chip->PTT_read_buf_size;
    write_buf_len = chip->PTT_write_buf_size;
    if(fp->file_attr.bits.folder)     //FDT条目是一个目录
    {
        Ring_Init(&fp->read_rw_buf,NULL,0);     //目录的读缓冲区设为空
        Ring_Init(&fp->write_buf,NULL,0);       //目录的写缓冲区设为空
        fp->p_read_buf = NULL;
        fp->p_write_buf = NULL;
        fp->read_ptr = 0;
        fp->write_ptr = 0;
        fp->file_size = 0;
    }else       //是一个文件，需要根据mode分配读写缓冲区
    {
        switch(mode)
        {
            case EN_R_RB:
            {
                buf =(uint8_t*)M_MallocLcHeap(read_buf_len,chip->ChipHeap,0);//本模式只有读缓冲区
                if(buf == NULL)
                    return false;           //缓冲区分配失败，返回错误
                Ring_Init(&fp->read_rw_buf,buf,read_buf_len);   //初始化读缓冲区
                Ring_Init(&fp->write_buf,NULL,0);           //初始化写缓冲区为空
                fp->p_read_buf = &fp->read_rw_buf;  //读缓冲区指针赋值
                fp->p_write_buf = NULL;             //写缓冲区指针置空
                fp->read_ptr = 0;                   //读位置置0
                fp->write_ptr = 0;                  //写位置置0
            }break;
            case EN_W_WB:
            {
                buf =(uint8_t*)M_MallocLcHeap(write_buf_len,chip->ChipHeap,0);//本模式只有写缓冲区
                if(buf == NULL)
                    return false;           //缓冲区分配失败，返回错误
                Ring_Init(&fp->read_rw_buf,NULL,0);         //初始化读缓冲区为空
                Ring_Init(&fp->write_buf,buf,write_buf_len);//初始化写缓冲区
                fp->p_read_buf = NULL;              //读缓冲区指针置空
                fp->p_write_buf = &fp->write_buf;   //写缓冲区指针赋值
                fp->read_ptr = 0;                   //读位置置0
                fp->write_ptr = 0;                  //写位置置0
                DFFSD_SetFileSize(fp,0);//本模式需要把文件清空
            }break;
            case EN_A_AB:
            {
                buf =(uint8_t*)M_MallocLcHeap(write_buf_len,chip->ChipHeap,0);//本模式只有写缓冲区
                if(buf == NULL)
                    return false;           //缓冲区分配失败，返回错误
                Ring_Init(&fp->read_rw_buf,NULL,0);         //初始化读缓冲区为空
                Ring_Init(&fp->write_buf,buf,write_buf_len);//初始化写缓冲区
                fp->p_read_buf = NULL;              //读缓冲区指针置空
                fp->p_write_buf = &fp->write_buf;   //写缓冲区指针赋值
                fp->read_ptr = 0;                   //读位置置0
                fp->write_ptr = fp->file_size;      //写位置指向文件尾
            }break;
            case EN_R_RB_PLUS:
            {
                buf = (uint8_t*)M_MallocLcHeap(read_buf_len,chip->ChipHeap,0);      //分配读缓冲区
                if(buf == NULL)
                    return false;           //缓冲区分配失败，返回错误
                buf1 = (uint8_t*)M_MallocLcHeap(write_buf_len,chip->ChipHeap,0);    //分配写缓冲区
                if(buf1 == NULL)
                {

                    free(buf);
                return false;           //缓冲区分配失败，返回错误
                }
                Ring_Init(&fp->read_rw_buf,buf,read_buf_len);   //初始化读缓冲区
                Ring_Init(&fp->write_buf,buf1,write_buf_len);   //初始化写缓冲区
                fp->p_read_buf = &fp->read_rw_buf;      //设置读缓冲区指针
                fp->p_write_buf = &fp->write_buf;       //设置写缓冲区指针
                fp->read_ptr = 0;                       //读位置置0
                fp->write_ptr = 0;                      //写位置置0
            }break;
            case EN_W_WB_PLUS:
            {
                buf = (uint8_t*)M_MallocLcHeap(read_buf_len,chip->ChipHeap,0);      //分配读缓冲区
                if(buf == NULL)
                    return false;           //缓冲区分配失败，返回错误
                buf1 = (uint8_t*)M_MallocLcHeap(write_buf_len,chip->ChipHeap,0);    //分配写缓冲区
                if(buf1 == NULL)
                {

                    free(buf);
                    return false;           //缓冲区分配失败，返回错误
                }
                Ring_Init(&fp->read_rw_buf,buf,read_buf_len);   //初始化读缓冲区
                Ring_Init(&fp->write_buf,buf1,write_buf_len);   //初始化写缓冲区
                fp->p_read_buf = &fp->read_rw_buf;      //设置读缓冲区指针
                fp->p_write_buf = &fp->write_buf;       //设置写缓冲区指针
                fp->read_ptr = 0;                       //读位置置0
                fp->write_ptr = 0;                      //写位置置0
                DFFSD_SetFileSize(fp,0);//本模式需要把文件清空
            }break;
            case EN_A_AB_PLUS:
            {
                buf = (uint8_t*)M_MallocLcHeap(read_buf_len,chip->ChipHeap,0);      //分配读缓冲区
                if(buf == NULL)
                    return false;           //缓冲区分配失败，返回错误
                buf1 = (uint8_t*)M_MallocLcHeap(write_buf_len,chip->ChipHeap,0);    //分配写缓冲区
                if(buf1 == NULL)
                {

                    free(buf);
                    return false;           //缓冲区分配失败，返回错误
                }
                Ring_Init(&fp->read_rw_buf,buf,read_buf_len);   //初始化读缓冲区
                Ring_Init(&fp->write_buf,buf1,write_buf_len);   //初始化写缓冲区
                fp->p_read_buf = &fp->read_rw_buf;      //设置读缓冲区指针
                fp->p_write_buf = &fp->write_buf;       //设置写缓冲区指针
                fp->read_ptr = 0;                       //读位置置0
                fp->write_ptr = fp->file_size;          //写位置指向文件尾
            }break;
            default:break;
        }
    }
    return true;
}

//----打开（文件/目录）--------------------------------------------------------
//功能：在分区中查找并打开文件(目录)，如果是文件，还需要为它分配读写缓冲区。只
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
uint32_t DFFSD_OpenItem(char *name,struct tagFileRsc *parent,
                       struct tagFileRsc *result,enum _FILE_OPEN_MODE_ mode)
{
    uint32_t FDT_item_no,FDT_item_eldest;
    struct tagPTTDevice *PTT_device_tag;
    struct tagStPTT_Flash *PTT_flash_tag;
    struct tagFdtInfo FDT_item;
    bool_t found = false;

    if((parent == NULL) || (result == NULL))
        return CN_LIMIT_UINT32;
    PTT_device_tag = (struct tagPTTDevice *)(parent->home_PTT);
    PTT_flash_tag = (struct tagStPTT_Flash *)PTT_device_tag->PTT_medium_tag;

    FDT_item_no = (uint32_t)parent->file_medium_tag;
    //读取父目录的FDT表项
    __DFFSD_ReadFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);
    if(!(FDT_item.mattr & CN_FS_ATTR_DIRECTORY))
        return CN_LIMIT_UINT32;       //父目录实际上是一个文件
    //读取父目录的第一个子目录/文件的FDT表项号
    FDT_item_eldest = pick_little_32bit(FDT_item.fstart_dson,0);
    if(FDT_item_eldest == CN_LIMIT_UINT32)
        return CN_FS_ITEM_INEXIST;  //parent是一个空目录
    FDT_item_no = FDT_item_eldest;
    do  //循环读目录表，直到读完
    {
        //读取的FDT表项
        __DFFSD_ReadFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);
        FDT_item.name[CN_FLASH_FILE_NAME_LIMIT] = '\0'; //字符串结束符，容错用
        if(__DFFSD_CmpFileName(name,FDT_item.name))
        {
            found = true;       //找到匹配的文件(目录)
            break;
        }else
        {
            if(pick_little_32bit(FDT_item.next,0) ==FDT_item_no)//避免死循环
                break;
            FDT_item_no = pick_little_32bit(FDT_item.next,0);
            if(FDT_item_no == CN_LIMIT_UINT32)//防止死循环
                break;
        }
    }while(FDT_item_no != FDT_item_eldest);
    if(found)
    {
        result->file_medium_tag = (ptu32_t)FDT_item_no;
        result->file_semp = NULL;
        //从flash中FDT条目的内存镜像FDT_item中提取文件信息
        __DFFSD_UasmFDT_Item(PTT_flash_tag,result,&FDT_item);
        if(__DFFSD_allocate_file_buf(PTT_flash_tag,result,mode))//分配文件缓冲区
        {
            return CN_FS_OPEN_SUCCESS;
        }else
            return CN_FS_ITEM_EXIST;
    }else
        return CN_FS_ITEM_INEXIST;
}

//----查找（文件/目录）--------------------------------------------------------
//功能：在分区中查找文件(目录)
//参数：name，文件（目录）名
//      parent，文件/目录的父目录，
//      result，返回找到的文件的信息
//返回：true=文件/目录存在，并在result中返回文件信息(不分配文件缓冲区)。
//      false = 文件/目录不存在，不修改result。
//----------------------------------------------------------------------------
bool_t DFFSD_LookForItem(char *name,struct tagFileRsc *parent,
                         struct tagFileRsc *result)
{
    uint32_t FDT_item_no,FDT_item_eldest;
    struct tagPTTDevice *PTT_device_tag;
    struct tagStPTT_Flash *PTT_flash_tag;
    struct tagFdtInfo FDT_item;
    bool_t found = false;
    if(parent == NULL)
        return false;
    PTT_device_tag = (struct tagPTTDevice *)(parent->home_PTT);
    PTT_flash_tag = (struct tagStPTT_Flash *)PTT_device_tag->PTT_medium_tag;
    FDT_item_no = (uint32_t)parent->file_medium_tag;
    //读取父目录的FDT表项
    __DFFSD_ReadFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);
    if(!(FDT_item.mattr & CN_FS_ATTR_DIRECTORY))
        return false;       //父目录实际上是一个文件
    //读取父目录的第一个子目录/文件的FDT表项号
    FDT_item_eldest = pick_little_32bit(FDT_item.fstart_dson,0);
    if(FDT_item_eldest == CN_LIMIT_UINT32)
        return false;  //parent是一个空目录
    FDT_item_no = FDT_item_eldest;
    do  //循环读目录表，直到读完
    {
        //读取的FDT表项
        __DFFSD_ReadFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);
        FDT_item.name[CN_FLASH_FILE_NAME_LIMIT] = '\0'; //字符串结束符，容错用
        if(__DFFSD_CmpFileName(name,FDT_item.name))
        {
            if(result != NULL)
            {
                result->file_medium_tag = (ptu32_t)FDT_item_no;
                result->file_semp = NULL;

                //从flash中FDT条目的内存镜像FDT_item中提取文件信息
                __DFFSD_UasmFDT_Item(PTT_flash_tag,result,&FDT_item);
            }
            found = true;
            break;
        }else
        {
            FDT_item_no = pick_little_32bit(FDT_item.next,0);
        }
    }while(FDT_item_no != FDT_item_eldest);
    return found;
}

//----关闭文件(目录)-----------------------------------------------------------
//功能: 关闭一个打开的文件(目录)，如果是文件，则释放读写缓冲区，并使分区中
//      opened_sum减量，如果是目录，则只是opened_sum减量。本函数只是处理关闭过程
//      与存储介质相关的操作，系统资源链表由file.c模块处理。
//参数：fp，被操作的文件指针
//返回：true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t DFFSD_CloseItem(struct tagFileRsc *fp)
{
    if(fp == NULL)
        return false;
    if(fp->file_attr.bits.folder)
    {
        //是个目录，好像没什么需要处理的
    }else
    {
        if(Ring_Capacity(&fp->write_buf) != 0)    //查看文件写缓冲区是否空
        {
            DFFSD_FlushFile(fp);//写缓冲区数据必须写入
            free(Ring_GetBuf(&fp->write_buf)); //文件写缓冲区非空，释放之
        }
        if(Ring_Capacity(&fp->read_rw_buf) != 0)     //查看文件读缓冲区是否空
        {
            free(Ring_GetBuf(&fp->read_rw_buf));  //文件读缓冲区非空，释放之
        }
    }
    return true;
}

//----文件（目录）改名---------------------------------------------------------
//功能：修改一个文件（目录）的名字，新名字保存在fp中。fp中的名字可以达到255字
//      符，但只有前cn_flash_file_name_limit字符有效。
//参数：fp，被操作的文件指针
//返回：true=成功，false=失败
//更新记录:
// 1.日期: 2015/3/19
//   说明: 增加对文件时间属性的支持,增加函数__DFFSD_UpdateItemTime
//   作者: 季兆林
//-----------------------------------------------------------------------------
bool_t DFFSD_RenameItem(struct tagFileRsc *fp,char *newname)
{
    struct tagPTTDevice *PTT_device_tag;
    struct tagFdtInfo FDT_item;
    struct tagStPTT_Flash *PTT_flash_tag;
    uint32_t name_len;
    uint32_t FDT_item_no;
    if(fp == NULL)
        return false;

    PTT_device_tag = (struct tagPTTDevice *)(fp->home_PTT);
    PTT_flash_tag = (struct tagStPTT_Flash *)PTT_device_tag->PTT_medium_tag;
    FDT_item_no = (uint32_t)fp->file_medium_tag;
    __DFFSD_ReadFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);
    //fp->name是文件系统模块提供的，其名字的长度本来应该是合法的，但是fp作为
    //一个资源结点，是暴露在阳光下的，有可能被误修改，故不能使用strlen。
    name_len = strnlen(fp->name,CN_FLASH_FILE_NAME_LIMIT+1);
    if(name_len <= CN_FLASH_FILE_NAME_LIMIT)  //分区名字不超过cn_flash_file_name_limit
        strcpy(FDT_item.name,newname);
    else
    {

        memcpy(FDT_item.name,newname,CN_FLASH_FILE_NAME_LIMIT);
        FDT_item.name[CN_FLASH_FILE_NAME_LIMIT] = '\0';
    }
    __DFFSD_UpdateItemTime(&FDT_item);
    __DFFSD_WriteFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);
    return true;
}

//----检测并改正FDT表-----------------------------------------------------------
//功能：如果在创建或删除文件的过程中断电，将可能导致FDT表中正在操作的链表断裂，
//      本函数检测并修复之
//参数：PTT_device_tag，被操作的分区指针，通用结构，非flash专用。
//返回：true=无需更改或者成功更改，false=有错误但修复失败
//-----------------------------------------------------------------------------
bool_t __DFFSD_RepairFDT(struct tagStPTT_Flash *PTT_flash_tag)
{
    return true;
}

//----查询目录表尺寸-----------------------------------------------------------
//功能：查询一个分区的目录表尺寸，按字节计算。当应用程序需要读取整个目录表时，
//      必须先用本函数计算目录表尺寸，然后才能分配内存。
//参数：PTT_device_tag，被操作的分区指针，通用结构，非flash专用。
//返回：目录表尺寸
//-----------------------------------------------------------------------------
uint32_t DFFSD_CheckFDT_Size(struct tagPTTDevice *PTT_device_tag)
{
    struct tagStPTT_Flash *PTT_flash_tag;
    struct tagFlashChip *chip;
    uint32_t result,block_no,block_no_next;
    if(PTT_device_tag == NULL)
        return 0;
    PTT_flash_tag = (struct tagStPTT_Flash*)PTT_device_tag->PTT_medium_tag;
    chip = PTT_flash_tag->chip;
    block_no_next = PTT_flash_tag->DDR_FDSB;
    result = 0;
    do
    {
        block_no = block_no_next;

           if(PTT_flash_tag->DDR_MAT[block_no].attr == cn_FDT_block)
            result ++;
        block_no_next = PTT_flash_tag->DDR_MAT[block_no].next;
    }while(block_no != block_no_next);
    return result * chip->block_size;
}

//----读目录表-----------------------------------------------------------------
//功能：读出一个分区的整个目录表，按原始数据读出，不做解释
//参数：PTT_device_tag，被操作的分区指针，通用结构，非flash专用。
//      buf，保存读取的数据的缓冲区指针，用户调用前必须分配号空间
//返回：无
//-----------------------------------------------------------------------------
void DFFSD_ReadFDT(struct tagPTTDevice *PTT_device_tag,uint8_t *buf)
{
    struct tagStPTT_Flash *PTT_flash_tag;
    struct tagFlashChip *chip;
    uint32_t block_no,block_no_next;
    uint32_t completed = 0;
    if((PTT_device_tag == NULL) || (buf == NULL))
        return;
    PTT_flash_tag = (struct tagStPTT_Flash*)PTT_device_tag->PTT_medium_tag;
    chip = PTT_flash_tag->chip;
    block_no_next = PTT_flash_tag->DDR_FDSB;
    do
    {
        block_no = block_no_next;
        chip->read_data_with_ecc(block_no + PTT_flash_tag->start_block,0,
                                 buf+completed,chip->block_size);
        block_no_next = PTT_flash_tag->DDR_MAT[block_no].next;
        completed += chip->block_size;
    }while(block_no != block_no_next);
    return ;
}

//----查询分区参数-----------------------------------------------------------
//功能: 返回分区总空间，有效空间，空闲空间的字节数
//参数:
//返回: 无
//-----------------------------------------------------------------------------
void DFFSD_CheckPTT(struct tagPTTDevice *PTT_device_tag,
                     sint64_t *sum_size,sint64_t *valid_size,sint64_t *free_size)
{
    struct tagStPTT_Flash *PTT_flash_tag;
    struct tagFlashChip *chip;
    if(PTT_device_tag == NULL)
    {
        if(sum_size != NULL)
            *sum_size = 0;
        if(valid_size != NULL)
            *valid_size = 0;
        if(free_size != NULL)
            *free_size = 0;
    }else
    {
        PTT_flash_tag =(struct tagStPTT_Flash*)PTT_device_tag->PTT_medium_tag;
        chip = PTT_flash_tag->chip;
        if(sum_size != NULL)
            *sum_size = PTT_flash_tag->block_sum * chip->block_size;
        if(valid_size != NULL)
            *valid_size = PTT_flash_tag->valid_sum * chip->block_size;
        if(free_size != NULL)
            *free_size = PTT_flash_tag->free_sum * chip->block_size;
    }
}
//----填充PTT_device_tag-------------------------------------------------------
//功能: 填充PTT_device_tag
//参数：PTT_device_tag，被操作的分区指针，通用结构，非flash专用。
//      name，分区名
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_Fill_FDT_Device(struct tagPTTDevice *PTT_device_tag,
                                 char *name)
{
    PTT_device_tag->format = DFFSD_FormatPTT;
    PTT_device_tag->write = DFFSD_WriteFile;
    PTT_device_tag->read = DFFSD_ReadFile;
    PTT_device_tag->flush = DFFSD_FlushFile;
    PTT_device_tag->query_file_stocks = DFFSD_QueryFileStocks;
    PTT_device_tag->query_file_cubage = DFFSD_QueryFileCubage;
    PTT_device_tag->check_PTT = DFFSD_CheckPTT;
    PTT_device_tag->set_file_size = DFFSD_SetFileSize;
    PTT_device_tag->seek_file = DFFSD_SeekFile;
    PTT_device_tag->create_item = DFFSD_CreateItem;
//    PTT_device_tag->move_file= DFFSD_MoveFile;
    PTT_device_tag->remove_item = DFFSD_RemoveFile;
    PTT_device_tag->open_item = DFFSD_OpenItem;
    PTT_device_tag->close_item = DFFSD_CloseItem;
    PTT_device_tag->lookfor_item = DFFSD_LookForItem;
    PTT_device_tag->rename_item = DFFSD_RenameItem;
    PTT_device_tag->item_traversal_son = DFFSD_ItemTraversalSon;
    PTT_device_tag->check_fdt_size = DFFSD_CheckFDT_Size;
    PTT_device_tag->read_fdt = DFFSD_ReadFDT;
    PTT_device_tag->check_folder = DFFSD_CheckFolder;
    PTT_device_tag->opened_sum = 0;
    //name肯定是个长度合法的字符串
    if(strlen(name) <= CN_FLASH_PTT_NAME_LIMIT)
        strcpy(PTT_device_tag->name,name);  //读分区名字
    else
    {
        memcpy(PTT_device_tag->name,name,CN_FLASH_PTT_NAME_LIMIT);
        PTT_device_tag->name[CN_FLASH_PTT_NAME_LIMIT] = '\0';
    }
}

//----DFFSD模块初始化----------------------------------------------------------
//功能: 就是在系统资源链表中添加flash芯片根结点
//参数: 无
//返回: 1
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_DFFSD(ptu32_t para)
{
    g_ptFlashfileSemp = Lock_SempCreate(1,0,CN_SEMP_BLOCK_FIFO,"flash_chip tree");
    if(g_ptFlashfileSemp == NULL)
        return 0;  //flash_chip树信号量申请失败
    //在资源链表中建立一个根结点，系统中所有芯片都挂在这个结点下
    Rsc_AddTree(&tg_flash_chip_root_rsc,sizeof(struct  tagRscNode),
                        RSC_RSCNODE,"flash chip");
    return 1;
}

//----扫描芯片-----------------------------------------------------------------
//功能: 启动时扫描芯片，识别芯片中的分区，并把分区加入到系统设备链表
//      中成为“fs”设备的子设备。
//参数: chip，被扫描的芯片
//      start_block_no，扫描起始块号，前面的块是保留块
//返回: 无
//------------
//更新记录:
// 1.日期: 2015/3/19
//   说明: FLASH中存在的分区大于内存支持如何处理？（未完成）
//   作者: 季兆林     
//-----------------------------------------------------------------------------
void __DFFSD_ScanChip(struct tagFlashChip *chip,uint32_t start_block_no)
{
    uint32_t loop,table_size,DDR_blocks,restored_block;
    //djy_handle_t fs_handle_right = NULL;
    struct tagStPTT_Flash *PTT_flash_tag = NULL;
    struct tagPTTDevice *PTT_device_tag = NULL;
    struct tagMDR_TempRecord MDR_record;
    struct tagFdtInfo FDT_item;

    chip->valid_sum = chip->block_sum - start_block_no;
    if(! __DFFSD_ReadMDR(chip,&MDR_record))
    {//系统第一次启动，肯定没有建立分区。故加载的时候，把DFFSD_init_MDR放在这步初始化
        DFFSD_InitMDR(chip,start_block_no); //未初始化的芯片按默认配置初始化
        return ;
    }
    //打开文件系统设备的右手接口
    /*if((fs_handle_right = Driver_DevOpenRight("fs",0)) == NULL)
        return ;*/
    for(loop=0; loop < CN_PTT_SUM_MAX; loop++)   //逐个遍历分区
    {
        PTT_device_tag = NULL;
        PTT_flash_tag = NULL;
        if(!chip->MDR.PTT_created[loop])
            continue;
        //分配分区设备专用数据结构内存，该结构将嵌入到泛设备结构中
        PTT_device_tag = (struct tagPTTDevice*)
                    M_Malloc(sizeof(struct tagPTTDevice),0);
        if(PTT_device_tag == NULL)
            goto goto_exit_install_chip;
        //分配flash文件系统专用数据结构内存，该结构将嵌入到分区设备中
        PTT_flash_tag = (struct tagStPTT_Flash*)
                    M_Malloc(sizeof(struct tagStPTT_Flash),0);
        if(PTT_flash_tag == NULL)
            goto goto_exit_install_chip;
        memset(PTT_flash_tag,0,sizeof(struct tagStPTT_Flash));
        memset(PTT_device_tag,0,sizeof(struct tagPTTDevice));
        PTT_flash_tag->ART_position = NULL;
        PTT_flash_tag->ART_block_no = NULL;
        PTT_flash_tag->ART_times = NULL;
        PTT_flash_tag->writed_DDR_main = NULL;
        PTT_flash_tag->writed_DDR_bak = NULL;
        PTT_flash_tag->DDR_DBL = NULL;
        PTT_flash_tag->DDR_MAT = NULL;
        PTT_flash_tag->chip = chip;     //flash分区存储器所属芯片
        PTT_flash_tag->PTT_no = loop;   //本分区在芯片中的序号
        PTT_flash_tag->start_block = MDR_record.start_blocks[loop];
        PTT_flash_tag->PCRB_no = MDR_record.PCRB_block_no[loop]
                                    -PTT_flash_tag->start_block;
        PTT_flash_tag->DDR_main = MDR_record.DDR_main_block_no[loop]
                                    -PTT_flash_tag->start_block;
        PTT_flash_tag->DDR_bak = MDR_record.DDR_bak_block_no[loop]
                                    -PTT_flash_tag->start_block;
        PTT_flash_tag->block_sum = MDR_record.blocks_sum[loop];
        chip->valid_sum -= PTT_flash_tag->block_sum;

        //填充函数指针和分区名
        __DFFSD_Fill_FDT_Device(PTT_device_tag,MDR_record.name[loop]);
        //分区设备的存储媒体指向flash分区存储器
        PTT_device_tag->PTT_medium_tag = (ptu32_t)PTT_flash_tag;

        PTT_device_tag->formatted = MDR_record.formatted[loop];
        //4+4+2=位置表4字节+块号表4字节+次数表2字节
        table_size = PTT_flash_tag->block_sum * (4+4+2);
        //为磨损位置表分配内存
        PTT_flash_tag->ART_position = (uint32_t*)M_Malloc(table_size,0);
        if(PTT_flash_tag->ART_position == NULL)
            goto goto_exit_install_chip;
        //为磨损块号表分配内存
        PTT_flash_tag->ART_block_no = PTT_flash_tag->ART_position
                                      + PTT_flash_tag->block_sum;
        //为磨损次数表分配内存
        PTT_flash_tag->ART_times = (uint16_t *)(PTT_flash_tag->ART_block_no
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

        if(PTT_device_tag->formatted) //检查分区格式化否
        {
            //从意外掉电中恢复数据，接下来要读取DDR,DDR表有被掉电恢复块保护的可能。
            //不理会不可恢复的情况，因为即使有文件被破坏，其他文件仍然可用，如果
            //DDR表被破坏，将在__read_DDR函数中被检出。
            chip->restore_PCRB(PTT_flash_tag->PCRB_no+PTT_flash_tag->start_block,
                                &restored_block);
            if(__DFFSD_ScanDDR(PTT_flash_tag))
            {
                __DFFSD_SortART(PTT_flash_tag);
                __DFFSD_ReadFDT_Item(PTT_flash_tag,0,&FDT_item);
                __DFFSD_UasmRootFolder(PTT_flash_tag,&FDT_item);
            }else    //DDR表出错，分区需要重新格式化才能使用
                PTT_device_tag->formatted = false;
            if(restored_block < chip->block_sum)
            {
                if(PTT_flash_tag->DDR_MAT[restored_block].attr == cn_FDT_block)
                    __DFFSD_RepairFDT(PTT_flash_tag);
            }
        }
        PTT_device_tag->chip_mutex_tag = chip->chip_mutex;
        if(Djyfs_FsDriverCtrl(enum_fs_add_PTT,
                            (ptu32_t)PTT_device_tag,0)
                            == EN_FS_NO_ERROR)
        {
            continue;   //不释放分区设备相关内存，直接进入下一循环
        }else
        {
            goto goto_exit_install_chip;
        }

    }
    //这是正常出口
    return ;
goto_exit_install_chip:
    printk("系统初始化建立分区. 失败. 系统内存不足\r\n");
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
    if(PTT_device_tag !=NULL)   free(PTT_device_tag);
    return ;
}

//----安装一个芯片------------------------------------------------------------
//功能: 把芯片中已经初始化的分区登记到文件系统中，该分区可能未格式化
//参数: chip，待安装的芯片
//      rev_blocks，该芯片保留块数量
//返回: 建立的分区数量，若返回0则表示芯片尚未初始化
//----------------------------------------------------------------------------
bool_t DFFSD_InstallChip(struct tagFlashChip *chip,char *name,uint32_t rev_blocks)
{
#if CN_MDR_SIZE < CN_MDR_CDR_SIZE + CN_MDR_PTT_SIZE +CN_PTT_SUM_MAX*3*8*2+8
    return false;  //如条件成立，MDR滚动部分容量小于2组条目，没有意义。
#endif

    if(chip == NULL)
        return false;
    chip->chip_mutex = Lock_MutexCreate(name);
    if(chip->chip_mutex == NULL)
        return false;
      //用户没有指定显存专用的堆，则使用系统堆。
    if(chip->ChipHeap == NULL)
        chip->ChipHeap = M_FindHeap("sys");
    chip->block_buf = M_MallocHeap(chip->block_size,chip->ChipHeap,0);  //分配块缓冲区内存
    if(chip->block_buf == NULL)
    {
        Lock_MutexDelete(chip->chip_mutex);
        return false;
    }

    Lock_SempPend(g_ptFlashfileSemp,CN_TIMEOUT_FOREVER);
    //把新flash芯片加入到flash芯片根资源结点下，排在队列尾
    Rsc_AddSon(&tg_flash_chip_root_rsc,&chip->chip_node,
                        sizeof(struct tagFlashChip),RSC_FFS_FLASH_CHIP,name);
    Lock_SempPost(g_ptFlashfileSemp);
    __DFFSD_ScanChip(chip,rev_blocks);
    return true;
}

//-----------------------------------------------------------------------------
//功能: 更新文件时间属性
//参数:
//返回:
//-----------------------------------------------------------------------------
void __DFFSD_UpdateFileTime(struct tagFileRsc *FileHandle)
{
    uint32_t FDT_Index;
    struct tagFdtInfo FDT;
    struct tagPTTDevice *PTT_Index;
    struct tagStPTT_Flash *PTT_Info;

    FDT_Index = FileHandle->file_medium_tag;
    PTT_Index =(struct tagPTTDevice *)(FileHandle->home_PTT);
    PTT_Info = (struct tagStPTT_Flash *)PTT_Index->PTT_medium_tag;

    __DFFSD_ReadFDT_Item(PTT_Info, FDT_Index, &FDT);
    __DFFSD_UpdateItemTime(&FDT);
     __DFFSD_WriteFDT_Item(PTT_Info, FDT_Index, &FDT);
    FileHandle->CreateTime = FDT.CreateTime;
    FileHandle->ModTime = FDT.ModTime;
}
