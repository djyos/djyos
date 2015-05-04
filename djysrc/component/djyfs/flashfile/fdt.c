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
//文件描述:flash文件系统中FDR表部分
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "endian.h"
#include "time.h"
#include "file.h"
#include "flashfile.h"
#include "fdt.h"
#include "ddr.h"

#include <string.h>

//----初始化FDT表--------------------------------------------------------------
//功能：初始化FDT表，把存储块划分为一个个FDT_item，并用双向链表连接起来。FDT
//      表的第一个FDT_item始终是根目录项。本函数只会在分区格式化时调用
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//返回：true=成功，false=失败
//------------
//更新记录:
// 1.日期: 2014/3/25
//   说明: 1. 更改了FDT结构体大小的计算方式
//         2. 时间属性结构
//   作者: 季兆林
//-----------------------------------------------------------------------------
bool_t __DFFSD_InitFDT(struct tagStPTT_Flash *PTT_flash_tag)
{
    uint8_t *FDT_buf;
    struct tagFdtInfo *pl_FDT_item;
    struct tagFlashChip *chip;
    uint32_t true_block,FDT_block_no;
    uint32_t block_capacity,loop;

    chip = PTT_flash_tag->chip;
    FDT_block_no = PTT_flash_tag->DDR_FDSB;
    block_capacity = chip->block_size / sizeof(struct tagFdtInfo);//这里的容量只计算了一块空间的
    PTT_flash_tag->FDT_capacity = block_capacity;
    PTT_flash_tag->FDT_free_num= block_capacity - 1;
    FDT_buf = chip->block_buf;

    for(loop=0; loop < block_capacity; loop++)
    {
        pl_FDT_item = (struct tagFdtInfo*)
                      (FDT_buf + (loop * sizeof(struct tagFdtInfo)));
        pl_FDT_item->name_buf_len = CN_FLASH_FILE_NAME_LIMIT +1;
        pl_FDT_item->mattr = 0xff;  //表示空闲FDT_item
        //以下把所有FDT_item的修改和创建日期时间均改为2009-01-04-12-00-00，
        //那是djyos的生日。
        /* 这里初始时间设定没有考虑生日 */
        pl_FDT_item->CreateTime = 0x12345678;
        pl_FDT_item->ModTime = 0x87654321;
        strcpy(pl_FDT_item->name,"free item");
        fill_little_32bit(pl_FDT_item->file_size,0,0);
        fill_little_32bit(pl_FDT_item->file_size,1,0);

        if(loop == 0)   //这是根目录项
        {
            //根目录项的next和previous指针用于把空闲块串起来。
            //根目录项的previous指针指向FDT末项
            fill_little_32bit(pl_FDT_item->previous,0,block_capacity-1);
            //根目录项的next指针指向第一个空闲项
            fill_little_32bit(pl_FDT_item->next,0,1);
            //根目录项的file_size指针用途特殊，前4字节记录着空闲FDT表项数
            fill_little_32bit(pl_FDT_item->file_size,0,block_capacity-1);
            //根目录项的file_size指针用途特殊，后4字节记录着总FDT表项数
            fill_little_32bit(pl_FDT_item->file_size,1,block_capacity);
            //根目录项的父目录指针parent空着
            fill_little_32bit(pl_FDT_item->parent,0,CN_LIMIT_UINT32);
            //根目录项的子目录指针fstart_dson暂时空着
            fill_little_32bit(pl_FDT_item->fstart_dson,0,CN_LIMIT_UINT32);
            //根目录是个只读目录
            pl_FDT_item->mattr = CN_FS_ATTR_DIRECTORY + CN_FS_ATTR_READ_ONLY;
            strcpy(pl_FDT_item->name,"root folder");
        }else if(loop == block_capacity-1)  //空闲的最后一项
        {
            //previous指针前一项
            fill_little_32bit(pl_FDT_item->previous,0,block_capacity-2);
            //最后一项的next指针指向根目录项
            fill_little_32bit(pl_FDT_item->next,0,0);
            fill_little_32bit(pl_FDT_item->parent,0,CN_LIMIT_UINT32);
            fill_little_32bit(pl_FDT_item->fstart_dson,0,CN_LIMIT_UINT32);
        }else
        {
            //previous指针指向前一项
            fill_little_32bit(pl_FDT_item->previous,0,loop - 1);
            //next指针指向后一项
            fill_little_32bit(pl_FDT_item->next,0,loop + 1);
            fill_little_32bit(pl_FDT_item->parent,0,CN_LIMIT_UINT32);
            fill_little_32bit(pl_FDT_item->fstart_dson,0,CN_LIMIT_UINT32);
        }
    }
    //把缓冲区写入flash块内
    true_block = __DFFSD_WriteNewBlock(PTT_flash_tag,
                                            FDT_block_no,0,chip->block_size);
    if(true_block == CN_LIMIT_UINT32)   //如果写入错误
        return false;
    if(true_block != FDT_block_no)      //如果发生了块交换
        __DFFSD_UpdateMAT_Item(PTT_flash_tag,FDT_block_no,true_block);
    return true;
}
//----FDT表增加1块-------------------------------------------------------------
//功能：当FDT表项用完后，调用本函数增加1块存储器，并把新增加的存储器划分为一个
//      个FDT_item块，然后用双向链表连接起来，并连接到空闲表中去。
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//返回：true=成功，false=失败，一般是因为没有空闲块
//-----------------------------------------------------------------------------
bool_t __DFFSD_AddFDTBlock(struct tagStPTT_Flash *PTT_flash_tag)
{
    uint8_t *FDT_buf;
    struct tagFdtInfo *pl_FDT_item,FDT_item;
    struct tagFlashChip *chip;
    uint32_t FDT_item_new_start,free_item,new_block_no,true_block;
    uint32_t block_capacity,last_block_no;
    uint32_t loop;

    if(PTT_flash_tag->valid_sum <= 2)
        return false;   //剩余可用块数小于等于2时，分配一块给FDT表是毫无意义的
    chip = PTT_flash_tag->chip;
    FDT_item_new_start = PTT_flash_tag->FDT_capacity;
    block_capacity = chip->block_size / 256;
    __DFFSD_ReadFDT_Item(PTT_flash_tag,0,&FDT_item);
    new_block_no = __DFFSD_AllocateBlock(PTT_flash_tag); //申请一个flash块
    if(new_block_no == CN_LIMIT_UINT32)
        return false;
    FDT_buf = chip->block_buf;
    pl_FDT_item = (struct tagFdtInfo*)FDT_buf;
    //新申请块的第一项previous指针指向根目录项
    fill_little_32bit(pl_FDT_item->previous,0,0);
    //新申请块的第一项next指针指向新申请块的第二项
    fill_little_32bit(pl_FDT_item->next,0,(FDT_item_new_start + 1));

    pl_FDT_item = (struct tagFdtInfo*)(FDT_buf + (block_capacity-1) * 256);
    //新申请块的最后一项的previous指针指向倒数第二项，-2是因为条目号从0起计。
    fill_little_32bit(pl_FDT_item->previous,0,
                        (FDT_item_new_start + block_capacity -2));
    if(0 == PTT_flash_tag->FDT_free_num)    //已经没有空闲FDT_item
    {
        //根目录项的previous指针指向新申请块的末项
        fill_little_32bit(FDT_item.previous,0,
                            FDT_item_new_start + block_capacity-1);
        //根目录项的next指针指向新申请块的首项
        fill_little_32bit(FDT_item.next,0,FDT_item_new_start);
        //根目录项写入flash中

        //新申请块的最后一项的next指针指向根目录项
        fill_little_32bit(pl_FDT_item->next,0,0);
    }else       //还剩余少许空闲FDT_item
    {
/* todo jzl 这段代码逻辑有问题，待验证*/
        free_item = pick_little_32bit(FDT_item.next, 0);//读第一个空闲FDT_item的条目号
        fill_little_32bit(FDT_item.next, 0, FDT_item_new_start);//根目录项的next指针指向新申请块的首项
        //debug FDT_item 参数必须换掉，因为上面的还没写入。注意啊！
        //读第一个空闲FDT项到缓冲区中
        __DFFSD_ReadFDT_Item(PTT_flash_tag,free_item,&FDT_item);
        //原第一个空闲项的向前指针指向新申请块的最后一项
        fill_little_32bit(FDT_item.previous,0,
                            FDT_item_new_start + block_capacity-1);
        //debug
        //这个写入，也要放在下面写入吧！不然影响0x40、0x7f项的调试
        //原第一个空闲项写入flash
        if(! __DFFSD_WriteFDT_Item(PTT_flash_tag,free_item,&FDT_item))
            return false;
        //新申请块的最后一项的next指针指向原第一个空闲项
        fill_little_32bit(pl_FDT_item->next,0,free_item);
    }
    for(loop=1; loop < block_capacity-1; loop++)
    {//把新申请块除首末FDT条目以外的其他FDT条目连接起来
        pl_FDT_item = (struct tagFdtInfo*)(FDT_buf + loop * 256);
        fill_little_32bit(pl_FDT_item->previous,0,
                            FDT_item_new_start + loop - 1);
        fill_little_32bit(pl_FDT_item->next,0,
                            FDT_item_new_start + loop + 1);
    }
    true_block = __DFFSD_WriteNewBlock(PTT_flash_tag,
                                            new_block_no,0,chip->block_size);
    if(true_block == CN_LIMIT_UINT32)
        return false;  //此时，FDT根目录项的指针已经改变并写入flash，但新块缓冲
                       //区却没有写入flash，将造成FDT的free项链断裂，错误可以在
                       //启动时查出，那时将会看到根目录项中第一个空闲项的号码大
                       //于MAT中FDT块的容量。
    PTT_flash_tag->FDT_capacity += block_capacity;
    PTT_flash_tag->FDT_free_num += block_capacity;
    //根目录项的file_size指针用途特殊，前4字节记录着空闲FDT表项数
    fill_little_32bit(FDT_item.file_size,0,PTT_flash_tag->FDT_free_num);
    //根目录项的file_size指针用途特殊，后4字节记录着总FDT表项数
    fill_little_32bit(FDT_item.file_size,1,PTT_flash_tag->FDT_capacity);
    if(! __DFFSD_WriteFDT_Item(PTT_flash_tag,0,&FDT_item))
         return false;
    //loop保存FDT首块的块号，用于定位MAT表中的FDT链
    loop = PTT_flash_tag->DDR_FDSB;
    do  //本循环用于找到MAT表中FDT链的最后一块
    {
        last_block_no = loop;
        loop = PTT_flash_tag->DDR_MAT[last_block_no].next;
    }while(loop != last_block_no);
    //把新申请块的实际写入的块号加入到MAT表中的FDT块号链中
    __DFFSD_AddMAT_Item(PTT_flash_tag,last_block_no,true_block);
    if(!__DFFSD_WriteDDR(PTT_flash_tag))
        return false;
    chip->erase_block(PTT_flash_tag->PCRB_no + PTT_flash_tag->start_block);
    return true;
}
//----把一个FDT_item添加到空闲-------------------------------------------------
//功能: 把一个已经被孤立的FDT_item加入到空闲项链表中，成为第一个结点。
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      FDT_item_no，被释放的条目号
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_AddFDTFreeItem(struct tagStPTT_Flash *PTT_flash_tag,
                         uint32_t FDT_item_no)
{
    uint32_t free_first;
    struct tagFdtInfo FDT_item;

    __DFFSD_ReadFDT_Item(PTT_flash_tag,0,&FDT_item);   //读出根目录项
    PTT_flash_tag->FDT_free_num++;
    //FDT表根目录项的file_size指针特殊用途:前4字节记录空闲FDT表项数
    fill_little_32bit(FDT_item.file_size,0,PTT_flash_tag->FDT_free_num);
    free_first = pick_little_32bit(FDT_item.next,0);
    if(PTT_flash_tag->FDT_free_num == 1)      //原空闲链表是空的
    {
        //根目录项的向前向后指针均指向目标项
        fill_little_32bit(FDT_item.next,0,FDT_item_no);
        fill_little_32bit(FDT_item.previous,0,FDT_item_no);
        __DFFSD_WriteFDT_Item(PTT_flash_tag,0,&FDT_item);  //写入根目录项
        //读出被释放项
        __DFFSD_ReadFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);
        //目标块的向前向后指针均指向根目录项
        fill_little_32bit(FDT_item.next,0,0);
        fill_little_32bit(FDT_item.previous,0,0);
        FDT_item.mattr |= CN_FS_ATTR_DELETED;

        fill_little_32bit(FDT_item.parent,0,CN_LIMIT_UINT32);
        fill_little_32bit(FDT_item.fstart_dson,0,CN_LIMIT_UINT32);
        //被修改项写入flash。
        __DFFSD_WriteFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);
        //为最大限度保留数据恢复信息，不修改除链表以外的其他数据
    }else               //空闲链表非空
    {
        //根目录项的向前指针均指向目标项
        fill_little_32bit(FDT_item.next,0,FDT_item_no);
        __DFFSD_WriteFDT_Item(PTT_flash_tag,0,&FDT_item);  //写入根目录项
        //读出被释放项
        __DFFSD_ReadFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);

        fill_little_32bit(FDT_item.parent,0,CN_LIMIT_UINT32);
        fill_little_32bit(FDT_item.fstart_dson,0,CN_LIMIT_UINT32);
        //被释放项前指针指向原最后一个空闲项
        fill_little_32bit(FDT_item.next,0,free_first);

        fill_little_32bit(FDT_item.previous,0,0);

        FDT_item.mattr |= CN_FS_ATTR_DELETED;

        strcpy(FDT_item.name,"free item");
        //被释放项写入flash中
        __DFFSD_WriteFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);
        //读出原第一个空闲项
        __DFFSD_ReadFDT_Item(PTT_flash_tag,free_first,&FDT_item);
        //原第一个空闲项的后指针指向被释放项
        fill_little_32bit(FDT_item.previous,0,FDT_item_no);
        //原最后一个空闲项写回flash
        __DFFSD_WriteFDT_Item(PTT_flash_tag,free_first,&FDT_item);
    }
}

//----孤立一个忙FDT_item-------------------------------------------------------
//功能: 把一个FDT_item从其原来的FDT链中孤立出来，这是删除文件/目录的第一步。
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      FDT_item_no，被孤立的条目号
//返回: true = 成功，false = 失败，一般是应为FDT_item_no有子目录/文件
//-----------------------------------------------------------------------------
bool_t __DFFSD_AcnodeFDTItem(struct tagStPTT_Flash *PTT_flash_tag,
                         uint32_t FDT_item_no)
{
    uint32_t parent,elder,less,eldest;     //被释放项的父、兄、弟、长兄项目号
    struct tagFdtInfo FDT_item;
    __DFFSD_ReadFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item); //读出目标项
    if((FDT_item.mattr & CN_FS_ATTR_DELETED) != 0)
        return false;   //目标FDT_item是一个空闲项/被删除项
    if((((FDT_item.mattr & CN_FS_ATTR_DIRECTORY) == 1) &&
          (pick_little_32bit(FDT_item.fstart_dson,0)) != CN_LIMIT_UINT32))
        return false;   //目标FDT_item有子文件/目录的目录
    parent = pick_little_32bit(FDT_item.parent,0);    //取父FDT项目号
    elder = pick_little_32bit(FDT_item.previous,0);   //取兄FDT项目号
    less = pick_little_32bit(FDT_item.next,0);        //取弟FDT项目号
    if(elder == less)       //兄、弟FDT项目号相等
    {
        if(elder == FDT_item_no)    //FDT_item_no是父目录下唯一的文件/目录
        {
            __DFFSD_ReadFDT_Item(PTT_flash_tag,parent,&FDT_item);//读父项
            //把子项号设置为没有子项
            fill_little_32bit(FDT_item.fstart_dson,0,CN_LIMIT_UINT32);
            __DFFSD_WriteFDT_Item(PTT_flash_tag,parent,&FDT_item); //写父项
        }else                   //父目录下有两个文件/目录
        {
            //读出兄(弟)FDT项
            __DFFSD_ReadFDT_Item(PTT_flash_tag,elder,&FDT_item);
            fill_little_32bit(FDT_item.previous,0,elder); //前指针指向自己
            fill_little_32bit(FDT_item.next,0,elder);     //后指针指向自己
            __DFFSD_WriteFDT_Item(PTT_flash_tag,elder,&FDT_item);  //写回flash
            __DFFSD_ReadFDT_Item(PTT_flash_tag,parent,&FDT_item);  //读父项
            eldest = pick_little_32bit(FDT_item.fstart_dson,0); //取长兄项目号
            if(eldest != elder)     //被孤立的恰好就是长兄
            {
                //设置父项的子项号
                fill_little_32bit(FDT_item.fstart_dson,0,elder);
                __DFFSD_WriteFDT_Item(PTT_flash_tag,parent,&FDT_item);//写父项
            }
        }
    }else       //兄、弟项目号不相等，说明被孤立项的同级目录项至少有3个
    {
        __DFFSD_ReadFDT_Item(PTT_flash_tag,parent,&FDT_item);  //读父项
        eldest = pick_little_32bit(FDT_item.fstart_dson,0);   //取长兄项号
        if(eldest == FDT_item_no)   //被孤立的恰好就是长兄
        {
            //设置父项的子项号
            fill_little_32bit(FDT_item.fstart_dson,0,less);
            __DFFSD_WriteFDT_Item(PTT_flash_tag,parent,&FDT_item); //写父项
        }
        __DFFSD_ReadFDT_Item(PTT_flash_tag,elder,&FDT_item);   //读兄结点
        fill_little_32bit(FDT_item.next,0,less); //兄结点的向后指针指向弟结点
        __DFFSD_WriteFDT_Item(PTT_flash_tag,elder,&FDT_item);  //写兄结点
        __DFFSD_ReadFDT_Item(PTT_flash_tag,less,&FDT_item);    //读弟结点
        //弟结点的向前指针指向弟结点
        fill_little_32bit(FDT_item.previous,0,elder);
        __DFFSD_WriteFDT_Item(PTT_flash_tag,less,&FDT_item);   //写弟结点
    }
    //注意，为最大限度保留数据恢复信息，不对被孤立项做任何修改
    return true;
}

//----释放FDT_item-------------------------------------------------------------
//功能: 释放一个FDT_item到FDT表中，把它加入空闲项链表
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      FDT_item_no，被释放的条目号
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_FreeFDTItem(struct tagStPTT_Flash *PTT_flash_tag,
                         uint32_t FDT_item_no)
{
    struct tagFlashChip *chip = PTT_flash_tag->chip;
    //从目录链表中取出被操作的FDT项
    if(__DFFSD_AcnodeFDTItem(PTT_flash_tag,FDT_item_no))
    {
        //把刚才取出的FDT项加入到空闲FDT链表中去
        __DFFSD_AddFDTFreeItem(PTT_flash_tag,FDT_item_no);
    }
    chip->erase_block(PTT_flash_tag->PCRB_no + PTT_flash_tag->start_block);
}


//----分配FDT_item-------------------------------------------------------------
//功能：从FDT表中分配一个FDT_item，把它从空闲链表中取出。如果没有空闲FDT_item，
//      则调用DFFSD_add_FDT_block函数分配一块。
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//返回：新分配的FDT_item项号，出错则返回cn_limit_uint32
//-----------------------------------------------------------------------------
uint32_t __DFFSD_AllocateFDTItem(struct tagStPTT_Flash *PTT_flash_tag)
{
    uint32_t next_free,next_next_free;
    struct tagFdtInfo FDT_root,FDT_free_item;
    struct tagFlashChip *chip = PTT_flash_tag->chip;

    __DFFSD_ReadFDT_Item(PTT_flash_tag,0,&FDT_root);
    //所有空闲FDT_item都链接在以根目录为核心的双向链表上
    next_free = pick_little_32bit(FDT_root.next,0);

    if(next_free >= PTT_flash_tag->FDT_capacity)
        return CN_LIMIT_UINT32;   //条目号大于FDT容量，肯定出错
    if(PTT_flash_tag->FDT_free_num == 0)
    {
        if(!__DFFSD_AddFDTBlock(PTT_flash_tag))     //延长FDT表失败
            return CN_LIMIT_UINT32;
        __DFFSD_ReadFDT_Item(PTT_flash_tag,0,&FDT_root);   //重新读根目录项
        next_free = pick_little_32bit(FDT_root.next,0); //重读根目录项next指针
    }
    //读取下一个空闲项，也就是即将返回给调用者的项
    __DFFSD_ReadFDT_Item(PTT_flash_tag,next_free,&FDT_free_item);
    //读取下下一个空闲项的编号
    next_next_free = pick_little_32bit(FDT_free_item.next,0);
    PTT_flash_tag->FDT_free_num--;
    //FDT表根目录项的file_size指针特殊用途:前4字节记录空闲FDT表项数
    fill_little_32bit(FDT_root.file_size,0,PTT_flash_tag->FDT_free_num);
    if(next_next_free == 0) //next_next_free指针指向第0项，这是最后一个空闲项
    {
        fill_little_32bit(FDT_root.next,0,0);
        fill_little_32bit(FDT_root.previous,0,0);
        if(! __DFFSD_WriteFDT_Item(PTT_flash_tag,0,&FDT_root))
            return CN_LIMIT_UINT32;
    }else
    {
        //读取下下一个空闲项的内容
        __DFFSD_ReadFDT_Item(PTT_flash_tag,next_next_free,&FDT_free_item);
        //根目录项的next指针指向下下一个空闲项的项号
        fill_little_32bit(FDT_root.next,0,next_next_free);
        //下下一个空闲项的previous指针指向根目录项
        fill_little_32bit(FDT_free_item.previous,0,0);
        //根目录项写入flash
        if(! __DFFSD_WriteFDT_Item(PTT_flash_tag,0,&FDT_root))
            return CN_LIMIT_UINT32;
        //下下一个空闲项写入flash
        if(!__DFFSD_WriteFDT_Item(PTT_flash_tag,next_next_free,&FDT_free_item))
            return CN_LIMIT_UINT32;
    }
    //如果写入FDT表时发生坏块替换，就需要刷新DDR表
    if(!__DFFSD_WriteDDR(PTT_flash_tag))
        return false;
    chip->erase_block(PTT_flash_tag->PCRB_no + PTT_flash_tag->start_block);
    return next_free;
}
//----压缩FDT表----------------------------------------------------------------
//功能：当FDT表中空闲表项所占的存储器容量超过1个block时，可以调用本函数压缩之。
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//返回：无
// db 待增加
//-----------------------------------------------------------------------------
void __DFFSD_CompressFDT(struct tagStPTT_Flash *PTT_flash_tag)
{
}

struct FDT_test
{
    u8 res[16];
    u32 free,sum;
    u32 parent,son,pre,next;
    u8 nane[216];
};

//----写一个FDT表项------------------------------------------------------------
//功能: 把一个FDT表项写入flash的FDT表中
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      FDT_item_no，条目号
//      FDT_item，FDT表项指针
//返回: true=成功写入，false=失败，一般是遇到坏块且没有空闲块可替换。
//------------
//更新记录:
// 1.日期: 2015/3/23
//   说明: 更改了FDT大小的计算方式
//   作者: 季兆林
//-----------------------------------------------------------------------------
bool_t __DFFSD_WriteFDT_Item(struct tagStPTT_Flash *PTT_flash_tag,
                           uint32_t FDT_item_no,struct tagFdtInfo *FDT_item)
{
    uint32_t true_block;
    struct tagFlashChip *chip;
    uint32_t block_offset,FDT_block_no,loop,FDT_block_serial;
    uint32_t FDT_Size = sizeof(struct tagFdtInfo);

    chip = PTT_flash_tag->chip;
    //计算FDT_item_no在fdt表中的块序号

    FDT_block_serial = FDT_item_no * FDT_Size / chip->block_size;
    block_offset = FDT_item_no * FDT_Size % chip->block_size;

    //读取FDT首块的块号
    FDT_block_no = PTT_flash_tag->DDR_FDSB;
    //从首块开始，沿MAT表搜索，找到第FDT_block_serial个FDT块的块号
    for(loop = 0; loop < FDT_block_serial; loop++)
        FDT_block_no = PTT_flash_tag->DDR_MAT[FDT_block_no].next;

    memcpy((chip->block_buf + block_offset), FDT_item, FDT_Size);
    //写入目标块
    true_block = __DFFSD_WriteUpdateBlock(PTT_flash_tag, FDT_block_no,
                                          block_offset, FDT_Size);

    if(true_block != CN_LIMIT_UINT32)
    {//正确写入
        if(true_block != FDT_block_no)   //写入时发生了块替换
        {
            __DFFSD_UpdateMAT_Item(PTT_flash_tag,FDT_block_no,true_block);
            if(! __DFFSD_WriteDDR(PTT_flash_tag))
                return false;
        }
    }else       //写入错误
        return false;
    chip->erase_block(PTT_flash_tag->PCRB_no + PTT_flash_tag->start_block);
    return true;
}

//----读一个FDT表项------------------------------------------------------------
//功能: 从FDT表中读取一个表项
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      FDT_item_no，条目号
//      FDT_item，返回数据的指针
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_ReadFDT_Item(struct tagStPTT_Flash *PTT_flash_tag,
                           uint32_t FDT_item_no,struct tagFdtInfo *FDT_item)
{
    struct tagFlashChip *chip;
    uint32_t block_offset,FDT_block_abs,FDT_block_no,loop,FDT_block_serial;
    chip = PTT_flash_tag->chip;
    //计算FDT_item_no在fdt表中的块序号
    FDT_block_serial = FDT_item_no * sizeof(struct tagFdtInfo) / chip->block_size;
    block_offset = FDT_item_no * sizeof(struct tagFdtInfo) % chip->block_size;
    //读取FDT首块的块号
    FDT_block_no = PTT_flash_tag->DDR_FDSB;
    //从首块开始，沿MAT表搜索，找到第FDT_block_serial个FDT块的块号
    for(loop = 0; loop < FDT_block_serial; loop++)
        FDT_block_no = PTT_flash_tag->DDR_MAT[FDT_block_no].next;
    //计算FDT_block在芯片中的绝对块号
    FDT_block_abs = FDT_block_no + PTT_flash_tag->start_block;
    chip->read_data_with_ecc(FDT_block_abs,block_offset,
                            (uint8_t*)FDT_item,sizeof(struct tagFdtInfo));
}

//----构造一个FDT_item---------------------------------------------------------
//功能：构造一个FDT_item的内存影像，但是没有处理构成链表的4个指针。
//参数: FDT_item，待构造的FDT条目指针
//      attr，条目属性
//      name，条目名称
//返回: 无
//------------
//更新记录:
// 1.日期: 2015/3/19
//   说明: 更改对文件时间属性格式
//   作者: 季兆林
//-----------------------------------------------------------------------------
void __DFFSD_AsmFDT_Item(struct tagFdtInfo *FDT_item,union file_attrs file_attr,
                          char *name)
{
    uint8_t attr_mirror;
    s64 nowtime;
    struct tm *file_time;
    ufast_t i;
    //因存在可能的移植问题，不宜直接使用位联合结构的attr参数
    attr_mirror = 0;
    if(file_attr.bits.read_only)
        attr_mirror +=CN_FS_ATTR_READ_ONLY;
    if(file_attr.bits.hidden)
        attr_mirror +=CN_FS_ATTR_HIDDEN;
    if(file_attr.bits.folder)
        attr_mirror +=CN_FS_ATTR_DIRECTORY;
    if(file_attr.bits.archive)
        attr_mirror +=CN_FS_ATTR_ARCHIVE;
    if(file_attr.bits.close_delete)
        attr_mirror +=CN_FS_ATTR_CLOSEDELETE;
    FDT_item->mattr = attr_mirror;
    FDT_item->name_buf_len = CN_FLASH_FILE_NAME_LIMIT +1;
    for(i = 0; i < 8; i++)
        FDT_item->file_size[i] = 0;    //文件尺寸=0
    //name是模块内部提供的字符串指针，已经经过字符串长度合法性检查
    if(strlen(name) <= CN_FLASH_FILE_NAME_LIMIT)//读取分区名字
        strcpy(FDT_item->name,name);
    else
    {
        memcpy(FDT_item->name,name,CN_FLASH_FILE_NAME_LIMIT);
        FDT_item->name[CN_FLASH_FILE_NAME_LIMIT] = '\0';
    }

    __DFFSD_SetItemTime(FDT_item);
    __DFFSD_UpdateItemTime(FDT_item);

    return;
}

//----读取文件(目录)参数-------------------------------------------------------
//功能：从FDT表项的内存镜像中的中读取文件(目录)信息，并初始化文件结构指针fp。
//参数：PTT_flash_tag，被操作的分区指针，通用结构，非flash专用。
//      fp，保存文件(目录)属性的文件指针
//      FDT_item，从flash中读出的FDT表项
//返回：无
//------------
//更新记录:
// 1.日期: 2015/3/19
//   说明: 修改文件时间格式
//   作者: 季兆林
//-----------------------------------------------------------------------------
void __DFFSD_UasmFDT_Item(struct tagStPTT_Flash *PTT_flash_tag,
                              struct tagFileRsc *fp,
                              struct tagFdtInfo *FDT_item)
{
    uint8_t attr_mirror;
    uint32_t name_len;

    fp->file_attr.all = 0;
    attr_mirror = FDT_item->mattr;
    if(attr_mirror & CN_FS_ATTR_READ_ONLY)
        fp->file_attr.bits.read_only =1;
    if(attr_mirror & CN_FS_ATTR_DIRECTORY)
        fp->file_attr.bits.folder =1;
    if(attr_mirror & CN_FS_ATTR_HIDDEN)
        fp->file_attr.bits.hidden =1;
    if(attr_mirror & CN_FS_ATTR_ARCHIVE)
        fp->file_attr.bits.archive =1;
    if(attr_mirror & CN_FS_ATTR_CLOSEDELETE)
        fp->file_attr.bits.close_delete =1;

    fp->CreateTime = FDT_item->CreateTime;
    fp->ModTime = FDT_item->ModTime;
    //读文件尺寸，为防止符号为扩展导致不必要的麻烦，按无符号数移位。
    fp->file_size = pick_little_32bit(FDT_item->file_size,0)
            +((uint64_t)pick_little_32bit(FDT_item->file_size,1)<<32);
    name_len = strnlen(FDT_item->name,CN_FLASH_FILE_NAME_LIMIT+1);
    if(name_len <= CN_FLASH_FILE_NAME_LIMIT)//分区名字不超过cn_flash_file_name_limit
        strcpy(fp->name, FDT_item->name);
    else
    {
        memcpy(fp->name, FDT_item->name,CN_FLASH_FILE_NAME_LIMIT);
        fp->name[CN_FLASH_FILE_NAME_LIMIT] = '\0';
    }
    //fp->open_counter = 0;
    return ;
}

//----解析根目录条目-------------------------------------------------------
//功能：从根目录的FDT表项的内存镜像中读取根目录信息，写到PTT_flash_tag中
//参数：PTT_flash_tag，被操作的分区指针，通用结构，非flash专用。
//      FDT_item，从flash中读出的FDT表项
//返回：无
//-----------------------------------------------------------------------------
void __DFFSD_UasmRootFolder(struct tagStPTT_Flash *PTT_flash_tag,
                                struct tagFdtInfo *FDT_item)
{
    //根目录的FDT条目的file_size有特殊用途，前4字节为FDT表中空闲FDT条目数，后
    //4个字节是FDT表中总FDT条目数
    PTT_flash_tag->FDT_free_num = pick_little_32bit(FDT_item->file_size,0);
    PTT_flash_tag->FDT_capacity = pick_little_32bit(FDT_item->file_size,1);
    return ;
}

//----修改fdt条目中的起始块号--------------------------------------------------
//功能: 当一个文件的首块被修改时，该文件的fdt条目中保存的起始块号也将被修改。只
//      有在发生磨损平衡块交换时，才有可能调用本函数，故在调用本函数时产生的写
//      操作并不修改磨损次数，即使发生坏块交换，也不调用标准的块分配函数
//      __allocate_block分配空闲块，而是直接从free块中取一块，不修改磨损次数。
//      这样可以避免发生连环套。
//参数: PTT_flash_tag,被操作的flash分区的存储媒体标签
//      FDT_item_no,fdt条目编号。
//      new_block_no，替换块号
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_UpdateFDT_FstartDson(struct tagStPTT_Flash *PTT_flash_tag,
                       uint32_t FDT_item_no,uint32_t new_block_no)
{
    struct tagFdtInfo FDT_item;
    __DFFSD_ReadFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);
    fill_little_32bit(FDT_item.fstart_dson,0,new_block_no);
    __DFFSD_WriteFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);
    return ;
}

//----在FDT中修改文件尺寸------------------------------------------------------
//功能: 修改FDT表项中的文件尺寸域。
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      fp，被修改的文件指针，由调用方保证不是目录而是文件。
//      new_size，新的文件尺寸
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_UpdateFDT_FileSize(struct tagStPTT_Flash *PTT_flash_tag,
                                  struct tagFileRsc *fp,sint64_t new_size)
{
    uint32_t FDT_item_no;
    struct tagFdtInfo FDT_item;
    FDT_item_no = (uint32_t)fp->file_medium_tag;
    __DFFSD_ReadFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);
    fill_little_32bit(FDT_item.file_size,0,(u32)new_size);
    //为防止符号为扩展导致不必要的麻烦，按无符号数移位。
    fill_little_32bit(FDT_item.file_size,1,(uint64_t)new_size>>32);
    __DFFSD_WriteFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);
    return ;
}

//----在FDT中修改文件名------------------------------------------------------
//功能: 修改FDT表项中的文件(目录)名域。
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      fp，被修改的文件指针
//      name，新的文件(目录)名
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_UpdateFDT_ItemName(struct tagStPTT_Flash *PTT_flash_tag,
                                  struct tagFileRsc *fp,char *name)
{
    uint32_t FDT_item_no;
    struct tagFdtInfo FDT_item;
    FDT_item_no = (uint32_t)fp->file_medium_tag;
    __DFFSD_ReadFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);
    //name是模块内部提供的字符串指针，已经经过字符串长度合法性检查
    if(strlen(name) <= CN_FLASH_FILE_NAME_LIMIT)//读取分区名字
        strcpy(FDT_item.name,name);
    else
    {
        memcpy(FDT_item.name,name,CN_FLASH_FILE_NAME_LIMIT);
        FDT_item.name[CN_FLASH_FILE_NAME_LIMIT] = '\0';
    }
    __DFFSD_WriteFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);
    return ;
}

//----在FDT中修改文件属性------------------------------------------------------
//功能: 修改FDT表项中的文件(目录)属性域。
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      fp，被修改的文件指针
//      attr，新的文件(目录)属性
//返回: 无
//-----------------------------------------------------------------------------
void __DFFSD_UpdateFDT_ItemAttr(struct tagStPTT_Flash *PTT_flash_tag,
                                  struct tagFileRsc *fp,union file_attrs attr)
{
    uint32_t FDT_item_no;
    struct tagFdtInfo FDT_item;
    uint8_t attr_mirror;
    FDT_item_no = (uint32_t)fp->file_medium_tag;
    __DFFSD_ReadFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);
    //因存在可能的移植问题，不宜直接使用位联合结构的attr参数
    attr_mirror = FDT_item.mattr;
    if(attr.bits.read_only)
        attr_mirror +=CN_FS_ATTR_READ_ONLY;
    if(attr.bits.hidden)
        attr_mirror +=CN_FS_ATTR_HIDDEN;
    if(attr.bits.archive)
        attr_mirror +=CN_FS_ATTR_ARCHIVE;   //以上只初始化3个属性，还两个暂不用
                                            //不能修改目录/文件属性
    FDT_item.mattr = attr_mirror;
    __DFFSD_WriteFDT_Item(PTT_flash_tag,FDT_item_no,&FDT_item);
    return ;
}
//----------------------------------------------------------------------------
//功能:
//参数:
//
//返回:
//问题: 等待C库local部分完成,再解决32位时间问题
//----------------------------------------------------------------------------
inline void __DFFSD_UpdateItemTime(struct tagFdtInfo *item)
{
    s64 Time;
    Time = TM_Time(NULL);
    fill_little_32bit((u8*)&(item->ModTime), 0, (u32)Time);
}
//----------------------------------------------------------------------------
//功能: 更新FDT的创建时间属性
//参数:
//
//返回:
//问题: 等待C库local部分完成,再解决32位时间问题
//----------------------------------------------------------------------------
inline void __DFFSD_SetItemTime(struct tagFdtInfo *item)
{
    s64 Time;
    Time = TM_Time(NULL);
    fill_little_32bit((u8*)&(item->CreateTime), 0, (u32)Time);
}

