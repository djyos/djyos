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
//-----------------------------------------------------------------------------//----------------------------------------------------
//Copyright (C), 2004-2009,  lst.
//版权所有 (C), 2004-2009,   lst.
//所属模块:flash文件系统
//作者：lst
//版本：V1.0.0
//文件描述:flash文件系统中DDR表的部分
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "align.h"
#include "endian.h"
#include "file.h"
#include "flashfile.h"
#include "ddr.h"
#include "fdt.h"
#include <string.h>

//----重构DDR_bak--------------------------------------------------------------
//功能：正确读取DDR_main到内存后后，如果校验发现flash中的DDR_bak与内存中的DDR表
//      不一致，将调用本函数。除DBL部分外，两份DDR表的其余部分是相同的，故只需
//      根据DDR表中的MAT表重构出一份DDR_DBL_bak即可，然后把整个DDR_bak表标记为
//      需要写入。
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//返回：无
//-----------------------------------------------------------------------------
void __DFFSD_RebulidDDR_Bak(struct tagStPTT_Flash *PTT_flash_tag)
{
    uint32_t i,loop,DBL_offset,DDR_blocks;
    struct tagFlashChip *chip = PTT_flash_tag->chip;
    bool_t *writed = PTT_flash_tag->writed_DDR_bak; //指向是否完成写入的指针
    uint32_t *DBL_buf;
    DDR_blocks = (PTT_flash_tag->DDR_size + chip->block_size -1)
                                / chip->block_size;    //计算DDR表占用的块数
    DBL_buf = PTT_flash_tag->DDR_DBL+DDR_blocks;
    for(loop = 0; loop < DDR_blocks; loop++)    //设置整个DDR_bak需写入
        writed[loop] = false;
    //以下几行扫描整个分区，初始化DBL_bak表
    DBL_offset = 0;
    for(loop = 0; loop < PTT_flash_tag->block_sum; loop++)
    {
        if(PTT_flash_tag->DDR_MAT[loop].attr == cn_DDR_bak_block)
        {
            DBL_buf[DBL_offset] = loop;     //第loop块是DDR_bak块
            DBL_offset ++;                  //DBL偏移增量
        }
    }
    //异常断电的特殊情况下，可能造成MAT中的DDR_bak块数不够的情况，详见文档
    //《DDR表异常断电算法》
    //为所缺少的DBL块分配新块
    //此时初始化还未完成，不能使用正常的块分配算法，直接找free块，
    if(DBL_offset < DDR_blocks)
    {
        for(loop = 0; loop < PTT_flash_tag->block_sum; loop++)
        {
            if(PTT_flash_tag->DDR_MAT[loop].attr == cn_free_block)
            {
                //第loop块是空闲块,改为DDR_bak块
                DBL_buf[DBL_offset] = loop;
                //把MAT表中第loop块的属性改为DDR_bak块
                PTT_flash_tag->DDR_MAT[loop].attr = cn_DDR_bak_block;
                //计算MAT表中第loop项的地址在DDR表中的偏移量
                i = PTT_flash_tag->DDR_size-(PTT_flash_tag->block_sum-loop)*4*2;
                //计算MAT表中第loop项的地址在DDR表中的偏移块号
                i = i / chip->block_size;
                //第loop块的用途被修改，MAT第koop项位置DDR_main也被修改，重写之
                PTT_flash_tag->writed_DDR_main[i] = false;
                DBL_offset ++;      //DBL偏移增量
                if(DBL_offset == DDR_blocks)
                    break;
            }
        }
    }
}

//----重构DDR_main-------------------------------------------------------------
//功能：正确读取DDR_main到内存后后，肯定DDR_main是有问题的，将调用本函数重构
//      DDR_main表。除DBL部分外，两份DDR表的其余部分是相同的，故只需根据DDR表中
//      的MAT表重构出一份DDR_DBL_main即可，然后把整个DDR_main表标记为需要写入。
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//返回：无
//-----------------------------------------------------------------------------
void __DFFSD_RebulidDDR_Main(struct tagStPTT_Flash *PTT_flash_tag)
{
    uint32_t i,loop,DBL_offset,DDR_blocks;
    struct tagFlashChip *chip = PTT_flash_tag->chip;

    bool_t *writed = PTT_flash_tag->writed_DDR_main; //指向是否完成写入的指针
    uint32_t *DBL_buf;
    DDR_blocks = (PTT_flash_tag->DDR_size + chip->block_size -1)
                                / chip->block_size;    //计算DDR表占用的块数
    DBL_buf = PTT_flash_tag->DDR_DBL;
    for(loop = 0; loop < DDR_blocks; loop++)    //置整个DDR_main需写入
        writed[loop] = false;
    //以下几行扫描整个分区，初始化DBL_main表
    DBL_offset = 0;
    for(loop = 0; loop < PTT_flash_tag->block_sum; loop++)
    {
        if(PTT_flash_tag->DDR_MAT[loop].attr == cn_DDR_main_block)
        {
            DBL_buf[DBL_offset] = loop;     //第loop块是DDR_main块
            DBL_offset ++;                  //DBL偏移增量
        }
    }
    //异常断电的特殊情况下，可能造成MAT中的DDR_main块数不够的情况，详见文档
    //《DDR表异常断电算法》
    //为所缺少的DBL块分配新块
    //此时初始化还未完成，不能使用正常的块分配算法，直接找free块，
    if(DBL_offset < DDR_blocks)
    {
        for(loop = 0; loop < PTT_flash_tag->block_sum; loop++)
        {
            if(PTT_flash_tag->DDR_MAT[loop].attr == cn_free_block)
            {
                //第loop块是空闲块,改为DDR_bak块
                DBL_buf[DBL_offset] = loop;
                //把MAT表中第loop块的属性改为DDR_main块
                PTT_flash_tag->DDR_MAT[loop].attr = cn_DDR_main_block;
                //计算MAT表中第loop项的地址在DDR表中的偏移量
                i = PTT_flash_tag->DDR_size -(PTT_flash_tag->block_sum-loop)*4*2;
                //计算MAT表中第loop项的地址在DDR表中的偏移块号
                i = i / chip->block_size;
                //第loop块的用途被修改，MAT第koop项位置DDR_main也被修改，重写之
                PTT_flash_tag->writed_DDR_main[i] = false;
                DBL_offset ++;      //DBL偏移增量
                if(DBL_offset == DDR_blocks)
                    break;
            }
        }
    }
}

//----计算MAT表起始偏移--------------------------------------------------------
//功能: 计算DDR表中MAT表的起始偏移地址，如果DDR_size不超过1块，则ART表后直接存
//      MAT表，否则MAT表要做8字节对齐以免单个MAT表项跨块。
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//返回: MAT表起始地址偏移
//-----------------------------------------------------------------------------
uint32_t __DFFSD_MAT_Start(struct tagStPTT_Flash *PTT_flash_tag)
{
    uint32_t MAT_start;
    struct tagFlashChip *chip = PTT_flash_tag->chip;

    MAT_start=(PTT_flash_tag->DDR_size+chip->block_size -1)/chip->block_size*4
                +4 +4 + PTT_flash_tag->block_sum*2;
    //如果不对齐，一个MAT表可能一半在第一块，一半在后一块。就糟糕了
    if(PTT_flash_tag->DDR_size > chip->block_size)
    {
        MAT_start = align_up_sys(MAT_start);
    }
    return MAT_start;
}

//----读DDR表------------------------------------------------------------------
//功能：本函数被__DFFSD_scan_DDR函数调用，扫描一个DDR表，读取DDR表的内容，然后
//      初始化PTT_flash_tag与DDR表有关的成员。
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//      which，读取哪一个DDR表，0=DDR_main，1=DDR_bak
//返回：true=完全无错，flash中DDR表被读入到PTT_flash_tag中。
//      false=有错误，flash中DDR表有任何错误，__DFFSD_scan_DDR函数会调用
//      __rebuild_DDR_bak或__rebuild_DDR_main函数重构DDR表
//-----------------------------------------------------------------------------
bool_t __DFFSD_ReadDDR(struct tagStPTT_Flash *PTT_flash_tag,
                            ufast_t which)
{
    uint32_t block_no;      //DDR表的块号
    uint32_t DDR_blocks;    //DDR表占用的块数
    uint32_t loop,read_size;
    uint32_t offset = 0,completed = 0;
    uint32_t ECCF_start,FDSB_start,ARB_start,ART_start,ART_end,MAT_start;
    uint32_t temp;
    struct tagMAT_Table *MAT_item;
    struct tagFlashChip *chip = PTT_flash_tag->chip;  //获取分区对应的芯片
    uint32_t *DBL_buf;

    DDR_blocks = (PTT_flash_tag->DDR_size+chip->block_size-1)/chip->block_size;
    if(which == 0)
    {
        DBL_buf = PTT_flash_tag->DDR_DBL;  //取DBL_main缓冲区指针
        DBL_buf[0] = PTT_flash_tag->DDR_main;
    }else
    {
        DBL_buf = PTT_flash_tag->DDR_DBL+DDR_blocks;   //取DBL_bak缓冲区指针
        DBL_buf[0] = PTT_flash_tag->DDR_bak;
    }
    //根据DDR表，各个成员所占用的空间，进行计算
    ECCF_start = DDR_blocks*4;
    FDSB_start = ECCF_start + 4;
    ARB_start = FDSB_start + 4;
    ART_start = ARB_start +4;
    ART_end = ART_start + PTT_flash_tag->block_sum * 2;
    MAT_start = __DFFSD_MAT_Start(PTT_flash_tag);
    MAT_item = PTT_flash_tag->DDR_MAT;
    for(loop = 0; loop < DDR_blocks; loop++)  //从首块到末块循环
    {
        block_no = DBL_buf[loop] + PTT_flash_tag->start_block;
        if(loop == DDR_blocks-1)
            read_size = PTT_flash_tag->DDR_size - completed;
        else
            read_size = chip->block_size;
        if(chip->read_data_with_ecc(block_no,0,chip->block_buf,read_size)
                                    != read_size)
        {
            return false;   //读函数返回错误
        }else
        {
            while(offset - completed < read_size)
            {
                if(offset < ECCF_start)         //DBL
                {
                    DBL_buf[offset/4] = pick_little_32bit(
                                        chip->block_buf+(offset-completed),0);
                    offset +=4;
                }
                else if(offset < FDSB_start)         //DBL
                {
                    PTT_flash_tag->nand_ecc = pick_little_32bit(
                                        chip->block_buf+(offset-completed),0);
                    offset +=4;
                }else if(offset < ARB_start)  //FDSB
                {
                    PTT_flash_tag->DDR_FDSB = pick_little_32bit(
                                        chip->block_buf+(offset-completed),0);
                    offset +=4;
                }else if(offset < ART_start) //ARB
                {
                    PTT_flash_tag->DDR_ARB = pick_little_32bit(
                                        chip->block_buf+(offset-completed),0);
                    offset +=4;
                }else if(offset < ART_end)   //ART
                {
                    temp = (offset-ART_start)/2;
                    PTT_flash_tag->ART_times[temp] = pick_little_16bit(
                                        chip->block_buf+(offset-completed),0);

                    PTT_flash_tag->ART_position[temp] = temp;
                    PTT_flash_tag->ART_block_no[temp] = temp;
                    offset +=2;
                }else if(offset < MAT_start)    //跳过对齐填充
                {
                    offset = MAT_start;
                }else if(offset <= PTT_flash_tag->DDR_size)
                {
                    temp = pick_little_32bit(
                                        chip->block_buf+(offset-completed),0);
                    MAT_item->attr = temp >>30;
                    //向前指针
                    MAT_item->previous = temp & 0x3fffffff;
                    temp = pick_little_32bit(
                                        chip->block_buf+(offset-completed),1);
                    MAT_item->attr += (temp >>30)<<2;
                    //向后指针
                    MAT_item->next = temp & 0x3fffffff;
                    offset +=8;
                    MAT_item++;
                }
            }
            completed += chip->block_size;
        }
    }
    for(loop = 0; loop < PTT_flash_tag->block_sum; loop++)  //磨损表不排序
    {
        PTT_flash_tag->ART_block_no[loop] = loop;
        PTT_flash_tag->ART_position[loop] = loop;
    }
    return true;        //哈哈，如果有任何错误，函数走不到这里
}

//----校验DDR_bak-------------------------------------------------------------
//功能：根据已经正确读到内存的DDR_main，校验flash中的DDR_bak是否正确。校验中只要
//      发现一个字节不一致，就返回错误，接下来会重构DDR_bak。调用
//      本函数前，PTT_flash_tag中与DDR表相关的成员应该初始化好(除DBL_bak外)。
//      本函数没有对应的__DFFSD_verify_DDR_main函数，因为启动时如果需要读
//      DDR_bak，则DDR_main无需校验，肯定是有错误的。
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//返回：true=完全无错，flash中DDR表被读入到PTT_flash_tag中。
//      false=有错误，flash中DDR表有任何错误，__DFFSD_scan_DDR函数会调用
//      __DFFSD_RebulidDDR_Bak
//-----------------------------------------------------------------------------
bool_t __DFFSD_VerifyDDR_Bak(struct tagStPTT_Flash *PTT_flash_tag)
{
    uint32_t block_no;      //DDR表的块号
    uint32_t DDR_blocks;    //DDR表占用的块数
    uint32_t loop,read_size;
    uint32_t offset = 0,completed = 0;
    uint32_t FDSB_start,ARB_start,ART_start,ART_end,MAT_start;
    uint32_t temp;
    struct tagMAT_Table *MAT_item;
    struct tagFlashChip *chip = PTT_flash_tag->chip;  //获取分区对应的芯片
    uint32_t *DBL_buf;
    uint8_t attr;

    DDR_blocks = (PTT_flash_tag->DDR_size+chip->block_size-1)/chip->block_size;
    DBL_buf = PTT_flash_tag->DDR_DBL+DDR_blocks;   //取DBL_bak缓冲区指针
    DBL_buf[0] = PTT_flash_tag->DDR_bak;
    FDSB_start = DDR_blocks*4;
    ARB_start = FDSB_start + 4;
    ART_start = ARB_start +4;
    ART_end = ART_start + PTT_flash_tag->block_sum *2;
    MAT_start = __DFFSD_MAT_Start(PTT_flash_tag);
    MAT_item = PTT_flash_tag->DDR_MAT;
    for(loop = 0; loop < DDR_blocks; loop++)  //从首块到末块循环
    {
        block_no = DBL_buf[loop] + PTT_flash_tag->start_block;
        if(loop == DDR_blocks-1)
            read_size = PTT_flash_tag->DDR_size - completed;
        else
            read_size = chip->block_size;
        if(chip->read_data_with_ecc(block_no,0,chip->block_buf,read_size)
                                    != read_size)
        {
            return false;   //读函数返回错误
        }else
        {
            while(offset - completed < read_size)
            {
                if(offset < FDSB_start)         //DBL
                {
                    DBL_buf[offset/4] = pick_little_32bit(
                                        chip->block_buf+(offset-completed),0);
                    offset +=4;
                }else if(offset < ARB_start)  //FDSB
                {
                    temp = pick_little_32bit(
                                        chip->block_buf+(offset-completed),0);
                    if(PTT_flash_tag->DDR_FDSB != temp)
                        return false;
                    offset +=4;
                }else if(offset < ART_start) //ARB
                {
                    temp = pick_little_32bit(
                                        chip->block_buf+(offset-completed),0);
                    if(PTT_flash_tag->DDR_ARB != temp)
                        return false;
                    offset +=4;
                }else if(offset < ART_end)   //ART
                {
                    temp = pick_little_16bit(
                                        chip->block_buf+(offset-completed),0);
                    if(PTT_flash_tag->ART_times[(offset-ART_start)/2]
                                != (uint16_t)temp)
                        return false;
                    offset +=2;
                }else if(offset < MAT_start)    //跳过对齐填充
                {
                    offset = MAT_start;
                }else if(offset < PTT_flash_tag->DDR_size)
                {

                    temp = pick_little_32bit(
                                        chip->block_buf+(offset-completed),0);
                    attr = temp >>30;
                    //向前指针
                    if((temp & 0x3fffffff) != MAT_item->previous)
                        return false;
                    temp = pick_little_32bit(
                                        chip->block_buf+(offset-completed),1);
                    attr += (temp >>30)<<2;
                    //向后指针
                    if((temp & 0x3fffffff) != MAT_item->next)
                        return false;
                    if(attr != MAT_item->attr)
                        return false;
                    offset +=8;
                    MAT_item++;
                }
            }
            completed += chip->block_size;
        }
    }
    return true;        //哈哈，如果有任何错误，函数走不到这里
}

//----读取一个分区的DDR表----------------------------------------------------
//功能: 读取一个分区的DDR表，并检测其有效性。先读DDR_main，如果有错误，就读
//      DDR_bak，若仍然错误，返回错误。读取一份正确的DDR表后，用这份正确的DDR表
//      校验另一份DDR表，如果发现错误，在PTT_flash_tag的writed数组中标记。
//      校验的算法太复杂，难于在此解释清楚，有专用文档说明。读取的数据保存在
//      PTT_flash_tag的DDR_mirror指针中，它是flash中DDR表的近似镜像。
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//返回: true=正确读取，false=错误
//备注: 内存有缓存，平时无须从flash中读取DDR,仅在分区尚未完成初始化时读DDR表用
//-----------------------------------------------------------------------------
bool_t __DFFSD_ScanDDR(struct tagStPTT_Flash *PTT_flash_tag)
{
    if(__DFFSD_ReadDDR(PTT_flash_tag,0) == true)  //先读DDR_main
    {
        //DDR_main正确读取，需校验DDR_bak是否一致，校验过程中可能产生修改
        //DDR_main的请求
        if(__DFFSD_VerifyDDR_Bak(PTT_flash_tag) == false)
            __DFFSD_RebulidDDR_Bak(PTT_flash_tag);   //重构DDR_bak
    }else if(__DFFSD_ReadDDR(PTT_flash_tag,1) == true)
    {
        //执行到此DDR_main肯定有错，无需校验，直接重构就可以了
        __DFFSD_RebulidDDR_Main(PTT_flash_tag);
    }else
        return false;
    return true;
}

//----跳跃换区-----------------------------------------------------------------
//功能: 某块从一个磨损分区转移到另一个磨损分区
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//      from，原区号
//      to，目标区号
//      position，被移动的块的原始位置
//返回: 无
//特注: 调用方确保第from分区不是空的，也就是说，position是有效参数
//-----------------------------------------------------------------------------
void __DFFSD_ART_Flip(struct tagStPTT_Flash *PTT_flash_tag,
                         ufast_t from,ufast_t to,uint32_t position)
{
    uint32_t loop,block_position,block_no_temp;
    uint32_t *pos_tab,*block_no_tab;
    uint16_t *times_tab;
    uint32_t block_no,*balance_region;
    uint16_t times_bak;

    block_position = position;
    balance_region = PTT_flash_tag->balance_region;
    pos_tab = PTT_flash_tag->ART_position;          //磨损位置表
    block_no_tab = PTT_flash_tag->ART_block_no;     //磨损块号表，与次数表同列
    times_tab = PTT_flash_tag->ART_times;           //磨损次数表
    block_no = block_no_tab[position];
    times_bak = times_tab[block_position];          //被移动块的磨损次数

    if(to > from)       //向上跳跃
    {
        for(loop = from; loop < to; loop++)
        {
            if(balance_region[loop] == balance_region[loop+1])//第loop分区是空的
            {
                continue;
            }
            //取第loop分区的最后一个单元的块号(即在pos表中的偏移量)
            block_no_temp = block_no_tab[balance_region[loop+1]-1];
            //第loop分区最后一个单元的磨损次数copy到上次循环移空的单元
            times_tab[block_position] = times_tab[balance_region[loop+1]-1];
            //第loop分区最后一个单元的块号copy到上次循环移空的单元
            block_no_tab[block_position] = block_no_temp;
            //第loop分区第一个单元的位置表指向上次移空的单元号
            pos_tab[block_no_temp] = block_position;
            block_position = balance_region[loop+1]-1;    //本次清空的单元索引号
        }
        for(loop = to; loop > from; loop--)
            balance_region[loop]--;        //分区指针上移一格
    }else               //向下跳跃
    {
        for(loop = from; loop > to; loop--)
        {
            if(loop != from)//此条件很重要，因为如果loop=18的话，loop+1下标越限
            {
                if(balance_region[loop] == balance_region[loop+1])//第loop分区空
                {
                    continue;
                }
            }
            //取第loop分区第一个单元的块号(即在pos表中的偏移量)
            block_no_temp = block_no_tab[balance_region[loop]];
            //第loop分区第一个单元的磨损次数copy到上次循环移空的单元
            times_tab[block_position] = times_tab[balance_region[loop]];
            //第loop分区第一个单元的块号copy到上次循环移空的单元
            block_no_tab[block_position] = block_no_temp;
            //第loop分区第一个单元的位置表指向目标单元号
            pos_tab[block_no_temp] = block_position;
            block_position = balance_region[loop];    //本次清空的单元索引号
        }
        for(loop = from; loop > to; loop--)
            balance_region[loop]++;        //分区指针下移一格
    }
    //被移动块的擦除次数写到目标区最后(上移)或首(下移)单元
    times_tab[block_position] = times_bak;
    //被移动块的块号写到目标区ART_block_no表的最后(上移)或首(下移)单元
    block_no_tab[block_position] = block_no;
    //pos表的被移动块指针指向目标区表最后(上移)或首(下移)单元
    pos_tab[block_no] = block_position;
}

//----磨损平衡块交换----------------------------------------------------------
//功能：当磨损平衡表中出现严重失衡时，调用本函数，用busy[0]中的一块与free[8]中的
//      一块交换，把busy[0]拷贝到free[8]中成为busy[8]，这样可以使“死”在busy中
//      的一块重新激活进入free[0]中待用，让free[8]中的一块到busy[8]中“睡”一会
//      儿。严重失衡的标准是:free[0~7]均空，而free[8]和busy[0]不空。
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//返回：true=成功交换一块到free[0]，false=失败
//-----------------------------------------------------------------------------
bool_t __DFFSD_ExchangeBusy0Free8(struct tagStPTT_Flash *PTT_flash_tag)
{
    uint32_t *busy_region,*free_region;
    uint32_t *pos_tab,*block_no_tab;
    uint16_t *times_tab,times_bak;
    uint32_t block_no_bak,block_no;
    uint32_t loop,invalid_num;
    struct tagFlashChip *chip = PTT_flash_tag->chip;  //取分区所属芯片
    pos_tab = PTT_flash_tag->ART_position;          //取磨损位置表
    block_no_tab = PTT_flash_tag->ART_block_no;     //取磨损块号表，与次数表同列
    times_tab = PTT_flash_tag->ART_times;           //取磨损次数表
    busy_region = &PTT_flash_tag->balance_region[9];    //第一个忙分区
    free_region = PTT_flash_tag->balance_region;        //第一个空闲分区

    //以下两句保存busy0.end条目的磨损次数和块号,
    times_bak = times_tab[busy_region[1]-1];
    block_no_bak = block_no_tab[busy_region[1]-1];
    block_no = PTT_flash_tag->start_block + block_no_bak;
    //读出busy0.end的内容
    chip->read_data_with_ecc(block_no,0,chip->block_buf,chip->block_size);
    //把busy0.end的内容写入到free8.0中，若写入不成功，则写入到free8.1，依次类推
    //直到成功。
    for(invalid_num=0;invalid_num < PTT_flash_tag->valid_sum;invalid_num++)
    {
        block_no = block_no_tab[free_region[8]+invalid_num];
        block_no += PTT_flash_tag->start_block;
        if(chip->write_data_with_ecc(block_no,0,chip->block_buf,
                                     chip->block_size) == chip->block_size)
        {
            break;  //正确写入，说明找到了一个好块，跳出循环
        }
    }
    //新产生坏块数量比可用块数量少，说明busy0.end已经正确写入到第
    //free_region[8]+invalid_num块中
    if(invalid_num < PTT_flash_tag->valid_sum)
    {
        for(loop = 1; loop < 8; loop++)
        {       //本循环将busy1~7整体下移一个条目，空出busy7.end
            busy_region[loop]--;        //busy_region[loop]指针下移一格
            if(busy_region[loop] == busy_region[loop-1])
                continue;       //第loop个busy分区是空的
            times_tab[busy_region[loop]] = times_tab[busy_region[loop+1]-1];
            block_no = block_no_tab[busy_region[loop+1]-1];
            block_no_tab[busy_region[loop]] = block_no;
            pos_tab[block_no] = busy_region[loop];
        }
        busy_region[8]--;  //busy_region8指针指向原busy7.end的位置
        //下面把free8.invalid_num块copy到busy_region8.0的位置
        times_tab[busy_region[8]] = times_tab[free_region[8] + invalid_num]+1;
        block_no = block_no_tab[free_region[8] + invalid_num];
        block_no_tab[busy_region[8]] = block_no;
        pos_tab[block_no] = busy_region[8];
        //下面把保存的原busy0.end的信息copy到free8.invalid_num块
        times_tab[free_region[8] + invalid_num] = times_bak;
        block_no_tab[free_region[8]+invalid_num] = block_no_bak;
        pos_tab[block_no_bak] = free_region[8]+invalid_num;
        //下面根据块交换结果修改MAT表,block_no_bak保存的是原busy0.end的块号，
        //block_no保存的是该块被交换到的目标块号。
        __DFFSD_UpdateMAT_Item(PTT_flash_tag,block_no_bak,block_no);
    }
    for(loop = 0; loop < invalid_num; loop++)//本循环把所有坏块转移到invalid_region
    {
        block_no = block_no_tab[free_region[8]+loop];
        __DFFSD_RegisterInvalid(PTT_flash_tag,block_no);
    }
    return true;
}

//----简单分配一块------------------------------------------------------------
//功能: 与__allocate_block函数类似，但是不做任何块交换，而是直接从free[0]区取出
//      一块。如果free[0]空，则实际取到的可能是free[1]或更高区的块
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//返回: 块号，是分区的相对块号，若没有空闲块，返回-1
//----------------------------------------------------------------------------
uint32_t __DFFSD_AllocateBlockSimple(struct tagStPTT_Flash *PTT_flash_tag)
{
    uint32_t block_no;
    uint32_t *block_no_tab;
    uint16_t *times_tab,times_bak;
    uint32_t target_region;
    if(PTT_flash_tag->free_sum== 0)
        return CN_LIMIT_UINT32;
    else
        PTT_flash_tag->free_sum--;
    block_no_tab = PTT_flash_tag->ART_block_no;   //取磨损块号表，与次数表同列
    times_tab = PTT_flash_tag->ART_times;         //取磨损次数表
    times_bak =times_tab[0];        //读取free表0区第一块磨损次数
    //该块变成忙块后插入相应忙区，由于free[0]开始可能会有若干个空闲区，故不
    //能简单地认为free[0]区取出的块不改变磨损次数会出现在busy[0]区
    target_region = times_bak/128 + 9;      //取忙区的目标区号
    //读取free表0区第一块块号
    block_no = block_no_tab[0];
    //从第0区的位置0跳到第target_region区
    __DFFSD_ART_Flip(PTT_flash_tag,0,target_region,0);
    return block_no;
}

//----分配一块----------------------------------------------------------------
//功能: 从free表中取出一块，并把该块放到busy表中。目标块的磨损次数不增量。如果:
//      1.空闲块全部集中在free_region[8]区，且busy_region[0]非空，就要进行坏块
//        磨损平衡替换。从busy_region[0]中取一块A，再从free_region[8]中取一块B，
//        A块的内容copy到B块，把B块放到busy_region[8]中。注意这个过程要涉及到
//        修改MAT表。
//      2.如果同时出现busy_region[0]和free_region[0]均空的现象，磨损表整体轮转
//        一圈，使busy_region[8]和free_region[8]均空。
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//返回: 块号，是分区的相对块号，若没有空闲块，返回-1
//----------------------------------------------------------------------------
uint32_t __DFFSD_AllocateBlock(struct tagStPTT_Flash *PTT_flash_tag)
{
    uint32_t block_no;
    uint32_t *block_no_tab;
    uint16_t *times_tab,times_bak;
    uint32_t *free_region;
    uint32_t target_region;
    if(PTT_flash_tag->free_sum== 0)
        return CN_LIMIT_UINT32;
    else
        PTT_flash_tag->free_sum--;
    block_no_tab = PTT_flash_tag->ART_block_no;   //取磨损块号表，与次数表同列
    times_tab = PTT_flash_tag->ART_times;   //取磨损次数表
    free_region = PTT_flash_tag->balance_region;        //第一个空闲分区
    if(free_region[0] == free_region[8])    //free[0~7]已经空
        __DFFSD_ExchangeBusy0Free8(PTT_flash_tag);  //调用写磨损平衡块交换函数
    times_bak =times_tab[0];//读取free表0区第一块磨损次数
//    times_bak++;    //磨损次数加1，注释掉，改为实际发生擦写时再增加。
    //该块变成忙块后插入end_region区，由于free[0]开始可能会有若干个空闲区，故不
    //能简单地认为free[0]区取出的块不改变磨损次数会出现在busy[0]区
    target_region = times_bak/128 + 9;
    //读取free表0区第一块块号
    block_no = block_no_tab[0];
    //从第0区的位置0跳到第target_region区
    __DFFSD_ART_Flip(PTT_flash_tag,0,target_region,0);
    return block_no;
}

//----写DDR表到flash-----------------------------------------------------------
//功能: 根据PTT_flash_tag->writed_main表中的内容，把DDR表中需要写入的内容写
//      入到flash中DDR_main表中。
//      1.把DDR表写入到flash中，存储位置由DBL_main提供
//      2.如果产生坏块，则申请新块，并在DBL_main、MAT_main、MAT_bak中标记。
//      3.如果产生坏块，MAT_bak和MAT_main的内容都有修改，但只有which参数指定的
//        那个MAT表修改后的内容会在本次写入flash，而另一份在__DFFSD_write_DDR函
//        数中再次调用本函数完成写入
//      4.为简化算法,即使发生坏块替换,也不修改磨损次数,只在mat表中把标示坏块，把
//        新块标示为DDR块.这样,发生坏块替换时,只需要修改MAT表和DBL表就可以了.由于
//        模式次数无需精确平衡，这样做是允许的。
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//      which，读取哪一个DDR表，0=DDR_main，1=DDR_bak
//返回: true=成功写入
//      false=失败，当写入遇到坏块，又没有空闲块可供分配时，会失败
//特注: 用循环写入，
//------------
//更新记录:
// 1.日期: 2015/3/23
//   说明: 1.增加写失败时,MAT_item指针重新设定。
//         2.考虑MAT表对齐
//   作者: 季兆林
//----------------------------------------------------------------------------
bool_t __DFFSD_WriteDDR_Exe(struct tagStPTT_Flash *PTT_flash_tag,
                             ufast_t which)
{
    struct tagFlashChip *chip;    //芯片指针
    uint32_t loop,DDR_blocks,DDR_no,write_result;
    uint32_t write_len,completed = 0;
    uint32_t block_no,block_new,temp;
    bool_t *writed;         //指向是否完成写入的指针
    bool_t finished;
    struct tagMAT_Table *MAT_item;
    uint32_t *DBL_buf;
    uint8_t *buf;
    uint32_t ECCF_start,FDSB_start,ARB_start,ART_start,ART_end,MAT_start,offset;
    uint8_t block_attr;

    chip = PTT_flash_tag->chip; //取得分区所属芯片
    buf = chip->block_buf;
    // 1份DDR表占用的块数，
    DDR_blocks = (PTT_flash_tag->DDR_size+chip->block_size-1)/chip->block_size;
    if(which == 0)
    {
        writed = PTT_flash_tag->writed_DDR_main;
        block_attr = cn_DDR_main_block;
        DBL_buf = PTT_flash_tag->DDR_DBL;
    }else
    {
        writed = PTT_flash_tag->writed_DDR_bak;
        block_attr = cn_DDR_bak_block;
        DBL_buf = PTT_flash_tag->DDR_DBL + DDR_blocks;
    }
    MAT_item = PTT_flash_tag->DDR_MAT;
    //地址0，是DBL的指针地址
    //FDSB,ARB,ART是uint32_t，故都是4字节
    ECCF_start = DDR_blocks*4;
    FDSB_start = ECCF_start + 4;
    ARB_start = FDSB_start + 4;
    ART_start = ARB_start +4;
    ART_end = ART_start + PTT_flash_tag->block_sum *2;
    //这是MAT表存储的起始地址
    MAT_start = __DFFSD_MAT_Start(PTT_flash_tag);
    do
    {
        finished = true;    //假定本循环可以完成全部写入操作
        for(loop = 0; loop < DDR_blocks; loop++)
        {
            if(writed[loop] == true)
            {
                continue;                       //本块已经写入，跳过。
            }
            completed = loop * chip->block_size;
            offset = completed;
            if(loop == DDR_blocks-1)
                write_len = PTT_flash_tag->DDR_size - completed;
            else
                write_len = chip->block_size;

            while(offset - completed < write_len)
            {
                if(offset < ECCF_start)         //写入DBL
                {
                    fill_little_32bit(buf+(offset-completed),0,
                                        DBL_buf[offset/4]);
                    offset +=4;
                }else if(offset < FDSB_start)  //写入FDSB
                {
                    fill_little_32bit(buf+(offset-completed),0,
                                        PTT_flash_tag->nand_ecc);
                    offset +=4;
                }else if(offset < ARB_start)  //写入FDSB
                {
                    fill_little_32bit(buf+(offset-completed),0,
                                        PTT_flash_tag->DDR_FDSB);
                    offset +=4;
                }else if(offset < ART_start) //写入ARB
                {
                    fill_little_32bit(buf+(offset-completed),0,
                                        PTT_flash_tag->DDR_ARB);
                    offset +=4;
                }else if(offset < ART_end)   //写入ART
                {
                    fill_little_16bit(buf+(offset-completed),0,
                                PTT_flash_tag->ART_times[(offset-ART_start)/2]);
                    offset +=2;
                }else if(offset < MAT_start)    //跳过对齐填充
                {
                    offset = MAT_start;
                }
                //写入MAT链表,MAT部分有对齐处理
                //目前只是初始化MAT表的属性成员。
                else if(offset <= (MAT_start + 2*4*PTT_flash_tag->block_sum))
                {//MAT表的数据结构的成员，和一般数据结构不一样。里面的attr是存储
                //在previous指针的高两位和next指针的高两位合并起来的空间。故MAT
                //的数据结构大小只有8字节，而不是9字节。便于对齐。
                    //提取向前指针前30位
                    temp = MAT_item->previous & 0x3fffffff;
                    //提取attr的第1、2位
                    temp += (uint32_t)MAT_item->attr<<30;
                    //向前指针和attr的第1、2位写入
                    fill_little_32bit(buf+(offset-completed),0,temp);
                    //提取向后指针前30位
                    temp = MAT_item->next & 0x3fffffff;
                    //提取attr的第3.4位
                    temp += (uint32_t)(MAT_item->attr>>2)<<30;
                    //向后指针和attr的第3.4位写入
                    fill_little_32bit(buf+(offset-completed),1,temp);
                    MAT_item++;//准备写入下一个MAT表里的数据
                    offset +=8;
                }
            }

            block_no = DBL_buf[loop];
            //擦除目标块
            chip->erase_block(block_no + PTT_flash_tag->start_block);
            write_result = chip->write_data_with_ecc(
                                     block_no + PTT_flash_tag->start_block,
                                     0,chip->block_buf,write_len);    //块写入
            if( write_result == write_len)    //检测是否正确写入
            {
                __DFFSD_IncAbrasionTimes(PTT_flash_tag,block_no);
                writed[loop] = true;    //正确写入则标记本块已经写入
            }
            else
            {   //写入错误，产生了坏块，做下列处理:
                MAT_item = PTT_flash_tag->DDR_MAT;//下面MAT_item当数组使用了。
                //block_no是新产生坏块的块号，block_new是替换坏块的好块块号
                block_new = __DFFSD_AllocateBlock(PTT_flash_tag);
                if(block_new == CN_LIMIT_UINT32)
                    return false;       //分区满，这种情况不多见
                writed[loop] = false;   //标记第loop块未写入
                finished = false;       //本循环没有完成写入
                //把坏块登记到擦除平衡表中，但没有写入到flash
                __DFFSD_RegisterInvalid(PTT_flash_tag,block_no);

                //坏块在DBL中的位置，被新块块号覆盖
                DBL_buf[loop] = block_new;
                //计算DBL中存放block_new的位置在DDR表中的块号
                DDR_no = 4 * loop/chip->block_size;
                writed[DDR_no] = false;

                //更新MAT表中block_new块的属性
                MAT_item[block_new].attr = block_attr;
                //计算block_new在MAT表中位置处在DDR表中是第几块
                DDR_no = (MAT_start+block_new*8) / chip->block_size;
                //DDR_main中标记该块为未写入
                PTT_flash_tag->writed_DDR_main[DDR_no] = false;
                //DDR_bak中标记该块为未写入
                PTT_flash_tag->writed_DDR_bak[DDR_no] = false;

                //该MAT表中block_no块的属性为坏块
                MAT_item[block_no].attr = cn_invalid_block;
                //计算block_no在MAT表中位置处在DDR表中是第几块
                DDR_no=(MAT_start+block_no*8) / chip->block_size;
                //DDR_main中标记该块为未写入
                PTT_flash_tag->writed_DDR_main[DDR_no] = false;
                //DDR_bak中标记该块为未写入
                PTT_flash_tag->writed_DDR_bak[DDR_no] = false;
            }
        }
    }while(finished == false);
    return true;
}

//----写一个分区的DDR表-----------------------------------------------------
//功能：将buf缓冲区中的DDR表写入指定分区的DDR区
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//返回：true=成功执行，false=写入错误
//---------------------------------------------------------------------------
bool_t __DFFSD_WriteDDR(struct tagStPTT_Flash *PTT_flash_tag)
{
    struct tagFlashChip *chip;        //芯片指针
    bool_t finished;
    uint32_t i,DDR_blocks;
    chip = PTT_flash_tag->chip; //取得分区所属芯片

    DDR_blocks = (PTT_flash_tag->DDR_size + chip->block_size -1)
                        / chip->block_size;     //计算DDR表所占的块数
    do
    {
        finished = true;
        __DFFSD_WriteDDR_Exe(PTT_flash_tag,0);    //写入DDR_main
        __DFFSD_WriteDDR_Exe(PTT_flash_tag,1);    //写入DDR_bak
        for(i = 0; i < DDR_blocks; i++)
        {
            if(PTT_flash_tag->writed_DDR_main[i] == false)
            {
                finished = false;
                break;
            }
            if(PTT_flash_tag->writed_DDR_bak[i] == false)
            {
                finished = false;
                break;
            }
        }
    }while(finished == false);  //直到DDR_main和DDR_bak全部完成
    return true;
}

//----MAT链表接龙--------------------------------------------------------------
//功能: 把一个MAT链表延长一块，新增加的块在链表的尾部，本函数没有把MAT表写入flash。
//      当写入文件导致文件延长、需要增加存储块时，调用本函数延长该文件对应的MAT
//      表的表项。
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//      last_block_no，原链表中的最后一块
//      new_block_no，替换块号，本块原来是空闲块
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_AddMAT_Item(struct tagStPTT_Flash *PTT_flash_tag,
                           uint32_t last_block_no,uint32_t new_block_no)
{
    struct tagFlashChip *chip = PTT_flash_tag->chip;
    uint32_t i,MAT_start;
    struct tagMAT_Table *MAT_item = PTT_flash_tag->DDR_MAT;

    MAT_start = __DFFSD_MAT_Start(PTT_flash_tag);
    //原最后一块的向后指针指向新块
    MAT_item[last_block_no].next = new_block_no;
    //新块的向前指针指向原最后一块
    MAT_item[new_block_no].previous = last_block_no;
    //新块的向后指针指向自己
    MAT_item[new_block_no].next = new_block_no;
    if(MAT_item[last_block_no].attr == cn_FDT_block)
    {
        //新块的属性改为FDT块
        MAT_item[new_block_no].attr = cn_FDT_block;
    }else
    {
        //新块的属性改为文件后续块
        MAT_item[new_block_no].attr = cn_file_block_continue;
    }
    //计算last_block_no在MAT表中位置处在DDR表中是第几块
    i=(MAT_start + last_block_no*4*2) / chip->block_size;
    //DDR_bak中标记该块为未写入
    PTT_flash_tag->writed_DDR_bak[i] = false;
    //DDR_main中标记该块为未写入
    PTT_flash_tag->writed_DDR_main[i] = false;

    //计算new_block_no在MAT表中位置处在DDR表中是第几块
    i=(MAT_start +new_block_no*4*2) / chip->block_size;
    //DDR_bak中标记该块为未写入
    PTT_flash_tag->writed_DDR_bak[i] = false;
    //DDR_main中标记该块为未写入
    PTT_flash_tag->writed_DDR_main[i] = false;
}

//----修改MAT表项--------------------------------------------------------------
//功能: 把一个MAT链表中的一块替换成另一块，如果该MAT表项刚好是一个文件的首块，
//      还要修改FDT表。本函数没有把MAT表写入flash。
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//      old_block_no，被替换的块号，替换后不改变其原来属性
//      new_block_no，替换块号，本块原来是空闲块
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_UpdateMAT_Item(struct tagStPTT_Flash *PTT_flash_tag,
                       uint32_t old_block_no,uint32_t new_block_no)
{
    struct tagMAT_Table *MAT_item = PTT_flash_tag->DDR_MAT;
    struct tagFlashChip *chip = PTT_flash_tag->chip;  //取分区所属芯片
    uint32_t pre_block_no,next_block_no,temp,DDR_blocks,u32_temp;
    uint32_t MAT_start,FDSB_start;

    if(old_block_no == new_block_no)
        return;
    MAT_start = __DFFSD_MAT_Start(PTT_flash_tag);

    //计算new_block_no在MAT表中位置处在DDR表中是第几块
    temp = (MAT_start + new_block_no*4*2) / chip->block_size;
    //DDR_bak中标记该块为未写入
    PTT_flash_tag->writed_DDR_bak[temp] = false;
    //DDR_main中标记该块为未写入
    PTT_flash_tag->writed_DDR_main[temp] = false;

    switch(MAT_item[old_block_no].attr)
    {
        case cn_file_block_start:  //原old_block_no块是文件首块
        {
            //new_block_no设置为文件首块
            MAT_item[new_block_no].attr = cn_file_block_start;
            //new_block_no的向前指针指向old_block_no的向前指针(即fdt条目号)
            MAT_item[new_block_no].previous=MAT_item[old_block_no].previous;
            //修改fdt中相应的条目，这个函数也可能会调用__DFFSD_update_MAT_item
            //函数，但不用担心会造成死循环，因为彼时__DFFSD_update_MAT_item的
            //执行路径是switch--case cn_FDT_block
            __DFFSD_UpdateFDT_FstartDson(PTT_flash_tag,
                              MAT_item[new_block_no].previous,new_block_no);

            if(MAT_item[old_block_no].next == old_block_no)
            {//既是首块又是末块，这是一个只有一块的文件
                //new_block_no的向后指针指向自己
                MAT_item[new_block_no].next = new_block_no;
            }else
            {//是首块但不是末块
                next_block_no = MAT_item[old_block_no].next;
                //new_block_no的向后指针指向old_block_no的向后指针
                MAT_item[new_block_no].next = next_block_no;
                //old_block_no的后一块的向前指针指向new_block_no
                MAT_item[next_block_no].previous = new_block_no;

                //计算old_block_no的后一块在MAT表中位置处在DDR表中是第几块
                temp = (MAT_start + next_block_no*4*2) / chip->block_size;
                //DDR_bak中标记该块为未写入
                PTT_flash_tag->writed_DDR_bak[temp] = false;
                //DDR_main中标记该块为未写入
                PTT_flash_tag->writed_DDR_main[temp] = false;
            }

        }break;
        case cn_file_block_continue: //原old_block_no块文件后续块
        {
            //new_block_no设置为文件首块
            MAT_item[new_block_no].attr = cn_file_block_continue;
            pre_block_no = MAT_item[old_block_no].previous;
            //new_block_no的向前指针指向old_block_no的向前指针
            MAT_item[new_block_no].previous = pre_block_no;
            MAT_item[pre_block_no].next = new_block_no;

            //计算old_block_no的前一块在MAT表中位置处在DDR表中是第几块
            temp = (MAT_start + pre_block_no*4*2) / chip->block_size;
            //DDR_bak中标记该块为未写入
            PTT_flash_tag->writed_DDR_bak[temp] = false;
            //DDR_main中标记该块为未写入
            PTT_flash_tag->writed_DDR_main[temp] = false;
            if(MAT_item[old_block_no].next == old_block_no)
            {//是末块
                //old_block_no的前一块的向后指针指向new_block_no
                MAT_item[pre_block_no].next = new_block_no;
                //new_block_no的向后指针指向自己
                MAT_item[new_block_no].next = new_block_no;

            }else
            {//不是末块。
                next_block_no = MAT_item[old_block_no].next;
                //new_block_no的向后指针指向old_block_no的向后指针
                MAT_item[new_block_no].next = next_block_no;
                //old_block_no的后一块的向前指针指向new_block_no
                MAT_item[next_block_no].previous = new_block_no;

                //计算old_block_no的后一块在MAT表中位置处在DDR表中是第几块
                temp = (MAT_start+next_block_no*4*2) / chip->block_size;
                //DDR_bak中标记该块为未写入
                PTT_flash_tag->writed_DDR_bak[temp] = false;
                //DDR_main中标记该块为未写入
                PTT_flash_tag->writed_DDR_main[temp] = false;
            }
        }break;
        case cn_FDT_block : //原old_block_no块是FDT块
        {
            FDSB_start = (PTT_flash_tag->DDR_size + chip->block_size -1)
                                / chip->block_size * 4;
            //new_block_no设置为FDT块
            MAT_item[new_block_no].attr = cn_FDT_block;
            if(old_block_no == PTT_flash_tag->DDR_FDSB)        //首块标志
            {
                PTT_flash_tag->DDR_FDSB = new_block_no;
                //计算DDR_FDSB_off在MAT表中位置处在DDR表中是第几块
                temp = FDSB_start / chip->block_size;
                //DDR_bak中标记该块为未写入
                PTT_flash_tag->writed_DDR_bak[temp] = false;
                //DDR_main中标记该块为未写入
                PTT_flash_tag->writed_DDR_main[temp] = false;
                //new_block_no的向前指针指向自己
                MAT_item[new_block_no].previous = new_block_no;
                if(MAT_item[old_block_no].next == old_block_no)
                {//既是首块又是末块，fdt表只占用1块
                    MAT_item[new_block_no].next = new_block_no;
                }else       //不是末块，是中间块
                {
                    next_block_no = MAT_item[old_block_no].next;  //后一块的块号
                    //new_block_no的向后指针指向next_block_no
                    MAT_item[new_block_no].next = next_block_no;
                    //后一块的向前指针指向new_block_no
                    MAT_item[next_block_no].previous = new_block_no;
                    //计算old_block_no的后一块在MAT表中位置处在DDR表中是第几块
                    temp = (MAT_start + next_block_no*4*2) / chip->block_size;
                    //DDR_bak中标记该块为未写入
                    PTT_flash_tag->writed_DDR_bak[temp] = false;
                    //DDR_main中标记该块为未写入
                    PTT_flash_tag->writed_DDR_main[temp] = false;
                }
                //new_block_no的向后指针指向自己
            }else
            {//不是首块，是末块
                pre_block_no = MAT_item[old_block_no].previous;
                //new_block_no的向前指针指向old_block_no的向前指针
                MAT_item[new_block_no].previous = pre_block_no;
                //old_block_no的前一块的向后指针指向new_block_no
                MAT_item[pre_block_no].next = new_block_no;
                //计算old_block_no的前一块在MAT表中位置处在DDR表中是第几块
                temp = (MAT_start+pre_block_no*4*2) / chip->block_size;
                //DDR_bak中标记该块为未写入
                PTT_flash_tag->writed_DDR_bak[temp] = false;
                //DDR_main中标记该块为未写入
                PTT_flash_tag->writed_DDR_main[temp] = false;
                if(MAT_item[old_block_no].next == old_block_no)     //是末块
                {
                    //new_block_no的向后指针指向自己
                    MAT_item[new_block_no].next = new_block_no;
                }else       //是中间块
                {
                    next_block_no = MAT_item[old_block_no].next;
                    //new_block_no的向后指针指向后一块
                    MAT_item[new_block_no].next = next_block_no;
                    //old_block_no的后一块的向前指针指向new_block_no
                    MAT_item[next_block_no].previous = new_block_no;
                    //计算old_block_no的后一块在MAT表中位置处在DDR表中是第几块
                    temp=(MAT_start+next_block_no*4*2) / chip->block_size;
                    //DDR_bak中标记该块为未写入
                    PTT_flash_tag->writed_DDR_bak[temp] = false;
                    //DDR_main中标记该块为未写入
                    PTT_flash_tag->writed_DDR_main[temp] = false;
                }

            }
        }break;
        case cn_invalid_block : //原old_block_no块是坏块，这是不可能的情况
        {
        }break;
        case cn_free_block :    //原old_block_no块是空闲块，也是不可能的
        {
        }break;
        case cn_DDR_main_block ://原old_block_no块是DDR_main块
        {
            //new_block_no设置为DDR_main块
            MAT_item[new_block_no].attr = cn_DDR_main_block;
            DDR_blocks = (PTT_flash_tag->DDR_size + chip->block_size -1)
                                / chip->block_size;     //计算DDR表所占的块数
            //保存DDR_main的块杯更换，需要相应的DBL项
            for(temp = 0; temp < DDR_blocks; temp++)
            {
                u32_temp = PTT_flash_tag->DDR_DBL[temp];
                if(u32_temp == old_block_no)
                {
                    PTT_flash_tag->DDR_DBL[temp] = new_block_no;
                    break;
                }
            }
            //计算DBL表被修改的位置处在DDR表中是第几块
            temp= temp*4 / chip->block_size;
            //DDR_main中标记该块为未写入
            PTT_flash_tag->writed_DDR_main[temp] = false;
        }break;
        case cn_DDR_bak_block : //原old_block_no块是DDR_bak块
        {
            //new_block_no设置为DDR_main块
            MAT_item[new_block_no].attr = cn_DDR_bak_block;
            DDR_blocks = (PTT_flash_tag->DDR_size + chip->block_size -1)
                                / chip->block_size;     //计算DDR表所占的块数
            for(temp = 0; temp < DDR_blocks; temp++)
            {
                u32_temp = PTT_flash_tag->DDR_DBL[DDR_blocks + temp];
                if(u32_temp == old_block_no)
                    PTT_flash_tag->DDR_DBL[DDR_blocks + temp] = new_block_no;
            }
            //计算DDR表被修改的位置处在DDR表中是第几块
            temp= (DDR_blocks+ temp)*4 / chip->block_size;
            //DDR_main中标记该块为未写入
            PTT_flash_tag->writed_DDR_bak[temp] = false;
        }break;
        case cn_PCRB_block :     //原old_block_no块是掉电恢复块，再说吧
        {
        }break;
        default : break;
    }
    //如果原old_block_no是某文件的首块，还要修改fdt(目录)表中该文件对应的条目
    //如果原old_block_no是fdt表的首块，还要修改DDR表中的FDSB
}

//----磨损平衡表轮转-----------------------------------------------------------
//功能：busy_region[0]和free_region[0]均空的时候，磨损平衡表便轮转一圈，使
//      busy_region[8]和free_region[8]变成空的。
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//返回：无
//-----------------------------------------------------------------------------
void __DFFSD_WhirlART_Table(struct tagStPTT_Flash *PTT_flash_tag)
{
    struct tagFlashChip *chip = PTT_flash_tag->chip;
    uint32_t *busy_region,*free_region,*balance_region;
    uint32_t loop,ARB_block;
    uint16_t whirl_regions;
    uint16_t *times_tab;
    times_tab = PTT_flash_tag->ART_times;               //取磨损次数表
    busy_region = &PTT_flash_tag->balance_region[9];    //第一个忙分区
    free_region = PTT_flash_tag->balance_region;        //第一个空闲分区
    balance_region = PTT_flash_tag->balance_region;
    for(whirl_regions = 0; whirl_regions < 8; whirl_regions++)
    {//检查需轮转的区的数量，即从busy和free的0区开始，检查free和busy同时为空的
     //区的数量，直到第一个不同时为空的区为止。
        if((busy_region[whirl_regions] != busy_region[whirl_regions+1])
              || (free_region[whirl_regions] != free_region[whirl_regions+1]))
            break;
    }
    if(whirl_regions == 0)
        return;
    PTT_flash_tag->DDR_ARB += whirl_regions * 128;//磨损次数基数增加轮转区数
    //计算DDR_ARB在MAT表中位置处在DDR表中是第几块
    ARB_block = (PTT_flash_tag->DDR_size + chip->block_size -1)
                    / chip->block_size * 4 + 4;
    ARB_block = ARB_block / chip->block_size;
    //DDR_bak中标记该块为未写入
    PTT_flash_tag->writed_DDR_bak[ARB_block] = false;
    //DDR_main中标记该块为未写入
    PTT_flash_tag->writed_DDR_main[ARB_block] = false;
    for(loop = 0; loop < balance_region[18]; loop++)
    { //所有块的磨损次数减少轮转区数*128
        times_tab[loop] -= whirl_regions*128;
    }
    for(loop = 1; loop <= (u32)(9 - whirl_regions); loop++)
    {//每个忙、空区往下转移whirl个区，使忙空区最高区开始计算，whirl个区变成空的
        busy_region[loop] = busy_region[loop+whirl_regions];
        free_region[loop] = free_region[loop+whirl_regions];
    }
    for(; loop < 9; loop++)
    {//把原来忙、空区第8区的信息转移到第loop区，这样忙、空第8区为空
        busy_region[loop] = balance_region[18];
        free_region[loop] = busy_region[0];
    }
}

//----磨损次数加1--------------------------------------------------------------
//功能: 当某块的磨损次数增加时，调用本函数，把这种变化登记到内存中磨损平衡表中，
//      但并不写入到flash。
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//      block_no，被增加次数的块
//返回: 无
//----------------------------------------------------------------------------
void __DFFSD_IncAbrasionTimes(struct tagStPTT_Flash *PTT_flash_tag,
                                uint32_t block_no)
{
    uint32_t temp,block_position;
    uint32_t *pos_tab;
    uint16_t *times_tab;
    uint32_t *busy_region,*free_region;
    uint16_t block_times;

    pos_tab = PTT_flash_tag->ART_position;      //取磨损位置表
    times_tab = PTT_flash_tag->ART_times;       //取磨损次数表
    //取目标块在磨损块号表(次数表)中的位置
    block_position = pos_tab[block_no];
    times_tab[block_position]++;   //目标块的磨损次数增量
    block_times = times_tab[block_position];  //取目标块的磨损次数
    if((block_times % 128 != 0)||(block_times >= 1024))
    {
        return;                         //增加次数后不跨区,直接返回。
    }
    //下面处理需要跨区的情况,block_no块要放到磨损次数高一区
    busy_region = &PTT_flash_tag->balance_region[9];    //第一个忙分区
    temp = block_times/128 - 1;                    //取block_no块所在的磨损分区
    //看block_no是忙块还是闲块，只有忙块才会增加次数，这个判断只是容错而已
    if(block_position >= busy_region[0])
        temp += 9;     //block_no是忙块，跳过9个闲区
    __DFFSD_ART_Flip(PTT_flash_tag,temp,temp+1,block_position);

    busy_region = &PTT_flash_tag->balance_region[9];    //第一个忙分区
    free_region = PTT_flash_tag->balance_region;        //第一个空闲分区
    if((busy_region[0] == busy_region[1]) &&(free_region[0] == free_region[1]))
        //当出现free[0]和busy[0]同时空的情况，磨损表就轮转一圈
        __DFFSD_WhirlART_Table(PTT_flash_tag);
}

//----登记一个坏块-------------------------------------------------------------
//功能：把一个坏块登记到内存中的磨损平衡表中，本函数只修改内存中的擦除平衡表，
//      并不写入flash，用户须确保调用本函数后，在适当的时间写入flash。
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//      block_no，坏块块号
//返回：无
//-----------------------------------------------------------------------------
void __DFFSD_RegisterInvalid(struct tagStPTT_Flash *PTT_flash_tag,
                            uint32_t block_no)
{
    struct tagFlashChip *chip = PTT_flash_tag->chip;    //取得分区所属芯片
    uint32_t loop,block_position,block_no_temp;
    uint32_t *pos_tab,*block_no_tab;
    uint16_t *times_tab;
    uint32_t *balance_region,*busy_region;
    uint16_t block_times;

    pos_tab = PTT_flash_tag->ART_position;        //取磨损位置表
    block_no_tab = PTT_flash_tag->ART_block_no;   //取磨损块号表，与次数表同列
    times_tab = PTT_flash_tag->ART_times;         //取磨损次数表
    balance_region = PTT_flash_tag->balance_region; //磨损分区表首址
    busy_region = &PTT_flash_tag->balance_region[9];    //第一个忙分区
    //取坏块在磨损块号表(次数表)中的位置
    block_position = pos_tab[block_no];
    block_times = times_tab[block_position];   //暂存坏块的磨损次数
    loop = block_times/128 +1;        //取坏块的磨损分区的下一分区号
    //看坏块原是忙块还是闲块，由于总是先分配再使用，坏块总是在使用中产生，使用
    //中的块肯定是忙块，故一般不会有闲块变成坏块的情况，容一下错吧
    if(block_position >= busy_region[0])
        loop += 9;     //坏块是忙块，跳过9个区
    for(; loop < 19; loop++)      //遍历所有磨损分区表，把坏块从原分区转移到坏块分区
    {
        balance_region[loop] -= 1;    //分区指针上移一格
        if(balance_region[loop] == balance_region[loop-1])
        {//相连的两分区指针重叠，说明第i-1分区是空的。
            balance_region[loop]--;
            continue;   //无需搬动块，进行下一轮比较。
        }
        //取第i分区的第一个单元的块号(即在pos表中的偏移量)
        block_no_temp = block_no_tab[balance_region[loop]];
        //第i分区第一个单元的磨损次数copy到上次循环移空的单元
        times_tab[block_position] = times_tab[balance_region[loop]];
        //第i分区第一个单元的块号copy到上次循环移空的单元
        block_no_tab[block_position] = block_no_temp;
        //第i分区第一个单元的位置表指向目标单元号
        pos_tab[block_no_temp] = block_position;
        block_position = balance_region[loop];    //本次清空的单元索引号
    }
    //坏块的擦除次数copy到坏块分区的首单元
    times_tab[block_position] = block_times;
    //坏块块号放到坏块分区首单元的ART_block_no表中
    block_no_tab[block_position] = block_no;
    //pos表的坏块指针指向坏块分区首单元
    pos_tab[block_no] = block_position;

    //设置block_no块为坏块
    PTT_flash_tag->DDR_MAT[block_no].attr = cn_invalid_block;
    //计算block_new在MAT表中位置处在DDR表中是第几块
    loop =(__DFFSD_MAT_Start(PTT_flash_tag)+block_no*4*2) / chip->block_size;
    PTT_flash_tag->writed_DDR_bak[loop] = false;     //DDR_bak中标记该块为未写入
    //DDR_main中标记该块为未写入
    PTT_flash_tag->writed_DDR_main[loop] = false;
}

//----释放一块到free表--------------------------------------------------------
//功能: 把一块从busy表转移到free表。
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//      block_no，释放的块号
//返回：无
//----------------------------------------------------------------------------
void __DFFSD_FreeBlock(struct tagStPTT_Flash *PTT_flash_tag,
                        uint32_t block_no)
{
    uint32_t block_position,i;
    uint32_t *pos_tab;
    uint32_t target_region;
    struct tagFlashChip *chip = PTT_flash_tag->chip;

    pos_tab = PTT_flash_tag->ART_position;
    block_position = pos_tab[block_no];
    //与第一个忙分区首块比较
    if(block_position < PTT_flash_tag->balance_region[9])
        return;     //被释放块本来就是闲块，无需释放
    else
        PTT_flash_tag->free_sum++;
    //被释放块的区号。
    target_region = PTT_flash_tag->ART_times[block_position]/128;
    //把目标块移到空闲区
    __DFFSD_ART_Flip(PTT_flash_tag,target_region+9,target_region,block_position);
    //只把块属性改为空闲，不修改链表，最大限度保留数据恢复的可能。
    PTT_flash_tag->DDR_MAT[block_no].attr = cn_free_block;

    //计算MAT表中block_no的地址在DDR表中的偏移量
    i = PTT_flash_tag->DDR_size-(PTT_flash_tag->block_sum-block_no)*4*2;
    //计算MAT表中第block_no的地址在DDR表中的偏移块号
    i = i / chip->block_size;
    PTT_flash_tag->writed_DDR_main[i] = false;
    //只要DDR表里面有改变，最好就立马写入
    __DFFSD_WriteDDR(PTT_flash_tag);
}

//----交换条目-----------------------------------------------------------------
//功能: 交换两个ART条目。
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//      pos1,PTT_flash_tag->ART_block_no表的条目号
//      pos2,PTT_flash_tag->ART_block_no表的另一个条目号
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_ExchangeART_Item(struct tagStPTT_Flash *PTT_flash_tag,
                                uint32_t pos1,uint32_t pos2)
{
    uint32_t block_no1,block_no2;
    uint16_t ART_times;
    block_no1 = PTT_flash_tag->ART_block_no[pos1];
    block_no2 = PTT_flash_tag->ART_block_no[pos2];
    PTT_flash_tag->ART_block_no[pos1] = block_no2;
    PTT_flash_tag->ART_block_no[pos2] = block_no1;
    PTT_flash_tag->ART_position[block_no1] = pos2;
    PTT_flash_tag->ART_position[block_no2] = pos1;
    ART_times = PTT_flash_tag->ART_times[pos1];
    PTT_flash_tag->ART_times[pos1] = PTT_flash_tag->ART_times[pos2];
    PTT_flash_tag->ART_times[pos2] = ART_times;
}

//----磨损块号表排序-----------------------------------------------------------
//功能: 把从flash芯片中读出的未排序的ART表在内存中排序，把排序结果写到
//      PTT_flash_tag的三个表:ART_position,ART_block_no和ART_times中。
//      在执行前，前磨损表是按块号排序的
//      排序后的块号表:
//      磨损次数   0~127的空闲块起始位置----free_region[0],balance_region[0]
//      磨损次数   0~127的空闲块结束位置
//      磨损次数 128~255的空闲块起始位置----free_region[1],balance_region[1]
//      磨损次数 128~255的空闲块结束位置
//      磨损次数 256~383的空闲块起始位置----free_region[2],balance_region[2]
//      磨损次数 256~383的空闲块结束位置
//      磨损次数 384~511的空闲块起始位置----free_region[3],balance_region[3]
//      磨损次数 384~511的空闲块结束位置
//      磨损次数 512~639的空闲块起始位置----free_region[4],balance_region[4]
//      磨损次数 512~639的空闲块结束位置
//      磨损次数 640~767的空闲块起始位置----free_region[5],balance_region[5]
//      磨损次数 640~767的空闲块结束位置
//      磨损次数 768~895的空闲块起始位置----free_region[6],balance_region[6]
//      磨损次数 768~895的空闲块结束位置
//      磨损次数896~1023的空闲块起始位置----free_region[7],balance_region[7]
//      磨损次数896~1023的空闲块结束位置
//      磨损次数   >1023的空闲块起始位置----free_region[8],balance_region[8]
//      磨损次数   >1023的空闲块结束位置
//      磨损次数   0~127的忙块起始位置------busy_region[0],balance_region[9]
//      磨损次数   0~127的忙块结束位置
//      磨损次数 128~255的忙块起始位置------busy_region[1],balance_region[10]
//      磨损次数 128~255的忙块结束位置
//      磨损次数 256~383的忙块起始位置------busy_region[2],balance_region[11]
//      磨损次数 256~383的忙块结束位置
//      磨损次数 384~511的忙块起始位置------busy_region[3],balance_region[12]
//      磨损次数 384~511的忙块结束位置
//      磨损次数 512~639的忙块起始位置------busy_region[4],balance_region[13]
//      磨损次数 512~639的忙块结束位置
//      磨损次数 640~767的忙块起始位置------busy_region[5],balance_region[14]
//      磨损次数 640~767的忙块结束位置
//      磨损次数 768~895的忙块起始位置------busy_region[6],balance_region[15]
//      磨损次数 768~895的忙块结束位置
//      磨损次数896~1023的忙块起始位置------busy_region[7],balance_region[16]
//      磨损次数896~1023的忙块结束位置
//      磨损次数   >1023的忙块起始位置------busy_region[8],balance_region[17]
//      磨损次数   >1023的忙块结束位置
//      坏块起始位置------------------------balance_region[18]
//      坏块结束位置
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//返回: 无
//------------
//更新记录:
// 1.日期: 2015/3/10
//   说明: 删除ART_offset无用赋值。
//   作者: 季兆林
//-----------------------------------------------------------------------------
void __DFFSD_SortART(struct tagStPTT_Flash *PTT_flash_tag)
{
    uint16_t *times_tab;
    uint32_t loop1,temp,invalid_offset,loop;
    uint32_t ART_offset,free_offset,block_no;
    uint32_t *pos_tab;
    uint32_t *busy_region,*free_region;
    pos_tab = PTT_flash_tag->ART_position;      //磨损位置表
    times_tab = PTT_flash_tag->ART_times;       //磨损次数表
    invalid_offset = PTT_flash_tag->block_sum;  //坏块位置指向磨损表底部
    loop = invalid_offset;
    do  //第一遍扫描，把全部坏块移到磨损表的末端
    {
        loop--;
        block_no = pos_tab[loop];
        if(PTT_flash_tag->DDR_MAT[block_no].attr == cn_invalid_block) //坏块
        {
            invalid_offset--;
            __DFFSD_ExchangeART_Item(PTT_flash_tag,loop,invalid_offset);
        }
    }while (loop != 0);
    PTT_flash_tag->balance_region[18] = invalid_offset;// todo jzl 这个赋值应该删除,待验证
    PTT_flash_tag->valid_sum = invalid_offset;
    free_offset = 0;            //闲块表偏移
    do  //把闲块放到balance_times表起端，忙块放到末端，先不排序
    {
        block_no = pos_tab[loop];
        if(PTT_flash_tag->DDR_MAT[loop].attr == cn_free_block)
        {
            __DFFSD_ExchangeART_Item(PTT_flash_tag,loop,free_offset);
            free_offset++;
        }
        loop++;
    }while (loop < invalid_offset);

    //至此，free_offset=闲块总数,invalid_offset=坏块区首，free_offset=忙块区首
    //下面对free表排序
    PTT_flash_tag->free_sum = free_offset;
    ART_offset = 0;
    free_region = PTT_flash_tag->balance_region;
    for(loop1 = 0; loop1 < 8; loop1++)  //扫描8个闲区
    {
        temp = 128*(loop1+1);
        free_region[loop1] = ART_offset;
        for(loop= ART_offset; loop < free_offset; loop++)
        {
            if(times_tab[loop] < temp)
            {
                //交换block_no_tab表中第loop第ART_offset条目
                __DFFSD_ExchangeART_Item(PTT_flash_tag,loop,ART_offset);
                ART_offset++;
            }else
            {
                ;   //什么都不用做，ART_offset维持原位
            }
        }
    }
    free_region[8] = ART_offset; //闲超限区指向超过1024的第一个块
    //下面对busy表排序，由于有DDR存在，必定有一块以上的忙块，故无需处理忙块数量
    //为0的特殊情况
    ART_offset = free_offset;
    busy_region = &PTT_flash_tag->balance_region[9];
    for(loop1 = 0; loop1 < 8; loop1++)      //扫描8个忙区
    {
        temp = 128*(loop1+1);
        busy_region[loop1] = ART_offset;
        for(loop= ART_offset; loop < PTT_flash_tag->valid_sum; loop++)
        {
            if(PTT_flash_tag->ART_times[loop] < temp)
            {
                //交换block_no_tab表中第loop第ART_offset条目
                __DFFSD_ExchangeART_Item(PTT_flash_tag,loop,ART_offset);
                ART_offset++;
            }else
            {
                ;   //什么都不用做，ART_offset维持原位
            }
        }
    }
    busy_region[8] = ART_offset; //忙超限区指向超过1024的第一个块
    return ;
}

//----计算DDR表尺寸------------------------------------------------------------
//功能：计算在内存中做1份DDR表镜像需要的字节数.比flash中的1份DDR表多一个DBL表。
//参数: PTT_flash_tag，被操作的分区，flash分区的专有数据结构指针
//返回: 内存镜像需要的字节数
//----------------------------------------------------------------------------
uint32_t __DFFSD_CalculateDDR_Size(struct tagStPTT_Flash *PTT_flash_tag)
{
    uint32_t size,blocks,fill,flash_MAT;
    uint64_t u64i;
    struct tagFlashChip *chip = PTT_flash_tag->chip;
    //下面计算1份DDR表尺寸,方法参见本文件开头部分的说明
    //粗算DDR表尺寸，可能是小数
    size =    4                                     //ECCF 使能ecc校验标志
            + 4                                     //ADB磨损次数基数
            + 2*PTT_flash_tag->block_sum            //ART磨损次数表
            + 4                                     //FDSB,fdt表首块块号
            + 4*2*PTT_flash_tag->block_sum;         //mat表
    //下式乘法可能造成32位整数溢出，故用64位整数
    u64i = chip->block_size * size + 4 * (chip->block_size-1);
    size = (uint32_t)(u64i / (chip->block_size-4));
    //上式得到的result可能会比实际需要多(0~3),下面调整之
    size = align_down(4,size);
    if(size > chip->block_size)       //超过1块，需考虑单个条目越块边界问题
    {//DDR表任意一个单项的任意一个条目不能跨块边界，下面调整之。
     //调整的过程中可能引起DDR_size变化，并连带引起DBL表长度变化
     //1、DBL表的起始偏移为0，表项固定为4字节，故无论DBL表占多少块，
     //   肯定不会有跨边界的项。
     //2、DBL表结束于4字节对齐地址，FDSB不可能跨边界。
     //2、FDSB结束于4字节对齐地址，磨损次数基数不可能跨边界。
     //3、磨损次数基数结束于4字节对齐地址，磨损次数表不可能跨界
     //4、磨损次数表结束于字节对齐地址，而MAT要求8字节对齐，故需要调整。
        blocks = (size +chip->block_size -1 ) / chip->block_size;   //块数
        flash_MAT = 4*blocks                        //DBL
                    + 4                             //ECCF
                    + 4 + 4                         //ARB+FDSB
                    + 2*PTT_flash_tag->block_sum;   //ART
        fill = align_up_sys(flash_MAT) - flash_MAT;  //算对齐损耗
        if((fill + size)/chip->block_size != size/chip->block_size)
        {//因调整增加了块数
            size += fill;
            if(fill < 4)   //对齐留下的空位不够一个DBL项
                size = size + 8;
        }else
        {
            size += fill;
        }
    }
    return size;
}

