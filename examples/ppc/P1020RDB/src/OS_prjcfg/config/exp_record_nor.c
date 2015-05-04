//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

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
// 这份授权条款，在使用者符合下列条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------
// 模块描述: NORFLASH异常存储方案
// 模块版本: V1.00
// 创建人员: Administrator
// 创建时间: 1:59:10 PM/May 9, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 备注:由于异常的操作都是禁止调度的，因此该方案不会再进行资源竞争的保护
#include "stdint.h"
#include "stddef.h"
#include "stdio.h"
#include "endian.h"

#include "os.h"

#include "exp_api.h"

#define CN_RECORD_ADDRBASE   0XFFB00000
#define CN_RECORD_SIZE       0X100000


//本文件使用的一些宏定义
#define CN_EXP_RECORD_INVALID_ADDR 0xFFFFFFFF
#define CN_EXP_RECORD_BLANKFLAG    0xFFFFFFFF
#define CN_EXP_RECORD_MAGICNUMBER  0x33333333

//该文件特有的数据结构
//存储异常信息表头，存储的方式：存储头+信息的方式，链表式
 struct tagExpNorRecordItemHead
{
    u32 recordendian;   //存储的大小端
    u32 magicnumber;    //自己的幸运数字
    u32 expinfoaddr;    //异常信息地址
    u32 expinfolen;     //异常信息长度
    u32 itemnextaddr;   //下一条地址
    //记录各个部分的信息,方便以后的信息过滤，地址均指的是存储地址
    u32 exp_headinfoaddr;       //头信息地址
    u32 exp_headinfolen;        //头信息长度
    u32 exp_osstateinfoaddr;    //OS状态信息地址
    u32 exp_osstateinfolen;     //OS状态信息长度
    u32 exp_hookinfoaddr;       //HOOK信息地址
    u32 exp_hookinfolen;        //HOOK信息长度
    u32 Exp_Throwinfoaddr;      //抛出信息地址
    u32 Exp_Throwinfolen;       //抛出信息长度
};
static u32  s_u32ExpRecordValidAddrNor = 0;//空闲存储区地址
static u32  s_u32ExpRecordValidLenNor = 0; //空闲存储区长度
static u32  s_u32ExpRecordItemNumNor = 0;  //已经存储的异常条目数

//flash读写
extern uint8_t Flash_EraseAddr(u32 addr, u32 len);
extern uint32_t Flash_ReadData(uint32_t addr, uint8_t *buf, uint32_t len);
extern uint32_t Flash_WriteData(uint32_t addr, uint8_t *buf, uint32_t len);

// =============================================================================
// 函数功能:_exp_norrecordheadswap
//         存储信息头的大小端转换
// 输入参数:itemhead,存储信息头
// 输出参数:itemhead
// 返回值  :无
// 说明    :
// =============================================================================
void _exp_norrecordheadswap(struct tagExpNorRecordItemHead *itemhead)
{
    itemhead->expinfoaddr = swapl(itemhead->expinfoaddr);
    itemhead->expinfolen = swapl(itemhead->expinfolen);
    itemhead->itemnextaddr = swapl(itemhead->itemnextaddr);
    itemhead->magicnumber = swapl(itemhead->magicnumber);
    itemhead->recordendian = swapl(itemhead->recordendian);
}
// =============================================================================
// 函数功能:Exp_Recordscan_nor
//         扫描异常存储区，更新可用地址以及已经存在的信息条目
// 输入参数:无
// 输出参数:无
// 返回值  :无
// 说明    :CN_RECORD_SIZE，配置的存储区长度，
//        CN_RECORD_ADDRBASE，配置的存储区基地址（相对于NORFLASH的偏移地址）
// =============================================================================
void Exp_Recordscan_nor(void)
{
    bool_t continuescan;
    u32 readaddr;
    u32 readlen;
    u32 hasread;
    u32 itemnum;
    struct tagExpNorRecordItemHead  itemhead;

    readaddr = CN_RECORD_ADDRBASE;
    continuescan = true;
    readlen = sizeof(itemhead);
    itemnum = 0;
    while(continuescan)  //扫描直到出现空闲区域或者到达存储区域结尾
    {
        if((readaddr)>= (CN_RECORD_ADDRBASE + CN_RECORD_SIZE - readlen))
        {
           //存储区扫描完毕
            s_u32ExpRecordValidAddrNor = readaddr;
            s_u32ExpRecordValidLenNor = CN_RECORD_ADDRBASE + CN_RECORD_SIZE -readaddr;
            s_u32ExpRecordItemNumNor =itemnum ;
            continuescan = false;
        }
        else
        {
            hasread = Flash_ReadData(readaddr, (u8 *)&itemhead,readlen);
            if(readlen != hasread)
            {
                printk("__exp_norrecordscan:scanerr:readaddr = 0x%08x\n\r",readaddr);
                printk("__exp_norrecordscan:readlen = 0x%08x,hasread = 0x%08x\n\r",\
                            readlen, hasread);
                //扫描失败
                s_u32ExpRecordValidAddrNor = CN_RECORD_ADDRBASE + CN_RECORD_SIZE;
                s_u32ExpRecordValidLenNor = 0;
                s_u32ExpRecordItemNumNor = 0xFFFFFFFF;
                continuescan = false;
            }
            else
            {
                //用小端作为标准主要是因为小端是0，大小端都一样
                if(((itemhead.recordendian == CN_CFG_LITTLE_ENDIAN)&&(CN_CFG_BYTE_ORDER != CN_CFG_LITTLE_ENDIAN))||\
                   ((itemhead.recordendian != CN_CFG_LITTLE_ENDIAN)&&(CN_CFG_BYTE_ORDER == CN_CFG_LITTLE_ENDIAN)))
                {
                    //需要大小端转换
                    _exp_norrecordheadswap(&itemhead);
                }
                if(CN_EXP_RECORD_MAGICNUMBER != itemhead.magicnumber)//空闲的
                {
                    s_u32ExpRecordValidAddrNor = readaddr;
                    s_u32ExpRecordValidLenNor = CN_RECORD_ADDRBASE + CN_RECORD_SIZE -readaddr;
                    s_u32ExpRecordItemNumNor =itemnum ;
                    continuescan = false;
                }
                else
                {
                    itemnum ++;
                    readaddr = itemhead.itemnextaddr;
                    continuescan = true;
                }
            }
        }
    }
}
// =============================================================================
// 函数功能：__exp_checkrecordpara_nor
//           检查存储参数
// 输入参数：recordpara，存储参数
// 输出参数：
// 返回值  ：true,合法参数，false，不合法参数
// =============================================================================
bool_t  __exp_checkrecordpara_nor(struct tagExpRecordPara *recordpara)
{
    bool_t result;

    result = true;
    if((NULL == recordpara) ||\
       (recordpara->headinfolen > s_u32ExpRecordValidLenNor)||\
       (recordpara->hookinfolen > s_u32ExpRecordValidLenNor)||\
       (recordpara->osstateinfolen > s_u32ExpRecordValidLenNor)||\
       (recordpara->throwinfolen > s_u32ExpRecordValidLenNor))
    {
        result = false;
    }
    else
    {
        if((recordpara->headinfoaddr == 0) &&(recordpara->headinfolen != 0))//不一致
        {
            result = false;
        }
        if((recordpara->hookinfoaddr == 0) &&(recordpara->hookinfolen != 0))//不一致
        {
            result = false;
        }
        if((recordpara->osstateinfoaddr== 0) &&(recordpara->osstateinfolen != 0))//不一致
        {
            result = false;
        }
        if((recordpara->throwinfoaddr == 0) &&(recordpara->throwinfolen!= 0))//不一致
        {
            result = false;
        }
    }
    return result;
}
// =============================================================================
// 函数功能：Exp_Record_nor
//           记录一帧异常信息
// 输入参数：recordpara，需要记录的异常信息
// 输出参数：
// 返回值  ：见en_sysExp_Record_result定义
// =============================================================================
u32 Exp_Record_nor(struct tagExpRecordPara *recordpara)
{
    u32 infolen = 0;
    u32 itemlen = 0;
    u32 recordaddr = 0;
    u32 recordlen = 0;
    u32 haswrite = 0;
    u8* infobuf;
    u32 result;
    struct tagExpNorRecordItemHead  itemhead;
    if(false == __exp_checkrecordpara_nor(recordpara))
    {
        printk("exp_norrecord:invalid recordpara\n\r");
        result =EN_EXP_DEAL_RECORD_PARAERR;
    }
    else
    {
        infolen = recordpara->headinfolen + recordpara->hookinfolen + \
                  recordpara->osstateinfolen + recordpara->throwinfolen;
        itemlen = sizeof(itemhead) + infolen;
        if(itemlen > s_u32ExpRecordValidLenNor)
        {
            result = EN_EXP_DEAL_RECORD_NOSPACE;
        }
        else
        {
            result = EN_EXP_DEAL_RECORD_SUCCESS;

            infobuf = (u8*)&itemhead;
            recordaddr = s_u32ExpRecordValidAddrNor;
            recordlen = sizeof(itemhead);
            //存储链表结构(做存储链表头)
            itemhead.recordendian = CN_CFG_BYTE_ORDER;
            itemhead.magicnumber = CN_EXP_RECORD_MAGICNUMBER;
            itemhead.expinfolen = infolen;
            itemhead.expinfoaddr = recordaddr+sizeof(itemhead);
            itemhead.itemnextaddr = recordaddr + itemlen;
            itemhead.exp_headinfoaddr = itemhead.expinfoaddr;
            itemhead.exp_headinfolen = recordpara->headinfolen;
            itemhead.exp_osstateinfoaddr = itemhead.exp_headinfoaddr+ itemhead.exp_headinfolen;
            itemhead.exp_osstateinfolen = recordpara->osstateinfolen;
            itemhead.exp_hookinfoaddr = itemhead.exp_osstateinfoaddr + itemhead.exp_osstateinfolen ;
            itemhead.exp_hookinfolen = recordpara->hookinfolen;
            itemhead.Exp_Throwinfoaddr = itemhead.exp_hookinfoaddr+ itemhead.exp_hookinfolen;
            itemhead.Exp_Throwinfolen= recordpara->throwinfolen;

            haswrite = Flash_WriteData(recordaddr, infobuf, recordlen);
            if(haswrite != recordlen)
            {
                result = EN_EXP_DEAL_RECORD_HARDERR;
            }
            recordaddr = recordaddr + recordlen;

            //更新静态变量
            s_u32ExpRecordValidAddrNor += itemlen;
            s_u32ExpRecordValidLenNor  -= itemlen;
            s_u32ExpRecordItemNumNor++;

            //开始记录INFOHEAD
            infobuf = (u8*)recordpara->headinfoaddr;
            recordlen = recordpara->headinfolen;
            haswrite = Flash_WriteData(recordaddr, infobuf, recordlen);
            if(haswrite != recordlen)
            {
                result = EN_EXP_DEAL_RECORD_HARDERR;
            }
            recordaddr = recordaddr + recordlen;
            //记录OSSTATEINFO
            infobuf = (u8*)recordpara->osstateinfoaddr;
            recordlen = recordpara->osstateinfolen;
            haswrite = Flash_WriteData(recordaddr, infobuf, recordlen);
            if(haswrite != recordlen)
            {
                result = EN_EXP_DEAL_RECORD_HARDERR;
            }
            recordaddr = recordaddr + recordlen;

            //记录HOOKINFO
            infobuf = (u8*)recordpara->hookinfoaddr;
            recordlen = recordpara->hookinfolen;
            haswrite = Flash_WriteData(recordaddr, infobuf, recordlen);
            if(haswrite != recordlen)
            {
                result = EN_EXP_DEAL_RECORD_HARDERR;
            }
            recordaddr = recordaddr + recordlen;
            //记录THROWINFO
            infobuf = (u8*)recordpara->throwinfoaddr;
            recordlen = recordpara->throwinfolen;
            haswrite = Flash_WriteData(recordaddr, infobuf, recordlen);
            extern bool_t Flash_OpetionIsOK(void);
            Flash_OpetionIsOK();
            if(haswrite != recordlen)
            {
                result = EN_EXP_DEAL_RECORD_HARDERR;
            }
            recordaddr = recordaddr + recordlen;

            if(recordaddr != s_u32ExpRecordValidAddrNor)
            {
                printk("exp_norrecord:logic err\n\r");
            }
        }
    }
    return result;
}
// =============================================================================
// 函数功能：Exp_RecordClear_nor
//          清除所有的异常信息，清除异常信息存储区域
// 输入参数：无
// 输出参数：无
// 返回值  ：true 成功， false失败
// =============================================================================
bool_t Exp_RecordClear_nor(void)
{
    bool_t result;
    Int_SaveAsynSignal();
    result = Flash_EraseAddr(CN_RECORD_ADDRBASE,CN_RECORD_SIZE);
    Exp_Recordscan_nor();
    Int_RestoreAsynSignal();
    return result;
}
// =============================================================================
// 函数功能：Exp_RecordCheckNum_nor
//          查看一共存储了多少条异常信息
// 输入参数：无
// 输出参数：recordnum,返回的异常信息条目数
// 返回值     ：false,失败 true成功
// =============================================================================
bool_t Exp_RecordCheckNum_nor(u32 *recordnum)
{
    *recordnum = s_u32ExpRecordItemNumNor;
    return true;
}

// =============================================================================
// 函数功能：Exp_RecordCheckLen_nor
//          查看指定条目的异常信息的长度
// 输入参数：assignedno,指定的异常帧条目(从1开始)
// 输出参数：recordlen,用于存储指定异常条目的长度
// 返回值     ：false,失败 true成功
// =============================================================================
bool_t Exp_RecordCheckLen_nor(u32 assignedno, u32 *recordlen)
{
    bool_t result;
    bool_t continuescan;
    u32 readaddr;
    u32 readlen;
    u32 hasread;
    u32 itemnum;
    struct tagExpNorRecordItemHead  itemhead;

    if((assignedno > s_u32ExpRecordItemNumNor)||(NULL == recordlen))
    {
        result = false;
        printk("checklen:assignedno = %d  recorditemnum = %d lenrecord = 0x%08x\n\r",\
                assignedno, s_u32ExpRecordItemNumNor, (u32)recordlen);
    }
    else//开始扫描
    {
        readaddr = CN_RECORD_ADDRBASE;
        continuescan = true;
        readlen = sizeof(itemhead);
        itemnum = 0;
        result = false;

        while(continuescan)  //扫描直到出现空闲区域或者到达存储区域结尾
        {
            if((readaddr)>= (CN_RECORD_ADDRBASE + CN_RECORD_SIZE - readlen))
            {
               //存储区扫描完毕
                s_u32ExpRecordValidAddrNor = readaddr;
                s_u32ExpRecordValidLenNor = CN_RECORD_ADDRBASE + CN_RECORD_SIZE -readaddr;
                s_u32ExpRecordItemNumNor =itemnum ;
                result = false;
                continuescan = false;
            }
            else
            {
                hasread = Flash_ReadData(readaddr, (u8 *)&itemhead,readlen);
                if(readlen != hasread)
                {
                    printk("__exp_norrecordscan:scanerr:readaddr = 0x%08x\n\r",readaddr);
                    printk("__exp_norrecordscan:readlen = 0x%08x,hasread = 0x%08x\n\r",\
                                readlen, hasread);
                    //扫描失败
                    result = false;
                    continuescan = false;
                }
                else
                {
                    //用小端作为标准主要是因为小端是0，大小端都一样
                    if(((itemhead.recordendian == CN_CFG_LITTLE_ENDIAN)&&(CN_CFG_BYTE_ORDER != CN_CFG_LITTLE_ENDIAN))||\
                       ((itemhead.recordendian != CN_CFG_LITTLE_ENDIAN)&&(CN_CFG_BYTE_ORDER == CN_CFG_LITTLE_ENDIAN)))
                    {
                        //需要大小端转换
                        _exp_norrecordheadswap(&itemhead);
                    }
                    if(CN_EXP_RECORD_MAGICNUMBER != itemhead.magicnumber)//空闲的
                    {
                        result = false;
                        continuescan = false;
                    }
                    else
                    {
                        itemnum ++;
                        if(itemnum == assignedno)
                        {
                            continuescan = false;
                            result = true;
                        }
                        else
                        {
                            readaddr = itemhead.itemnextaddr;
                            continuescan = true;
                            result = false;
                        }
                    }
                }
            }
        }
        if(true == result)
        {
            *recordlen = itemhead.expinfolen;
        }
    }
    return result;
}
// =============================================================================
// 函数功能：Exp_RecordGet_nor
//          从存储介质中获取指定条目的异常帧信息
// 输入参数：assignedno,指定的异常帧条目
//          buflenlimit,系统异常帧的长度，限制长度是因为避免拷贝越界
// 输出参数：buf,用于存储获取指定条目的异常信息
//          recordpara,异常信息存储时的参数，在此是对buf的各个部分的定义
// 返回值     ：true成功 false失败
// =============================================================================
bool_t Exp_RecordGet_nor(u32 assignedno, u32 buflenlimit, u8 *buf, \
                         struct tagExpRecordPara *recordpara)
{
    bool_t result_bool = false;
    bool_t continuescan;
    u32 readaddr;
    u32 readlen;
    u32 hasread;
    u32 itemnum;
    struct tagExpNorRecordItemHead  itemhead;

    if((assignedno > s_u32ExpRecordItemNumNor)||(NULL == buf) ||(NULL == recordpara))
    {
        readlen = 0;
        printk("checklen:assignedno = %d  recorditemnum = %d buf = 0x%08\n\r",\
                assignedno, s_u32ExpRecordItemNumNor, (u32)buf);
    }
    else//开始扫描
    {
        readaddr = CN_RECORD_ADDRBASE;
        continuescan = true;
        readlen = sizeof(itemhead);
        itemnum = 0;
        result_bool = false;

        while(continuescan)  //扫描直到出现空闲区域或者到达存储区域结尾
        {
            if((readaddr)>= (CN_RECORD_ADDRBASE + CN_RECORD_SIZE - readlen))
            {
               //存储区扫描完毕
                s_u32ExpRecordValidAddrNor = readaddr;
                s_u32ExpRecordValidLenNor = CN_RECORD_ADDRBASE + CN_RECORD_SIZE -readaddr;
                s_u32ExpRecordItemNumNor =itemnum ;
                result_bool = false;
                continuescan = false;
            }
            else
            {
                hasread = Flash_ReadData(readaddr, (u8 *)&itemhead,readlen);
                if(readlen != hasread)
                {
                    printk("__exp_norrecordscan:scanerr:readaddr = 0x%08x\n\r",readaddr);
                    printk("__exp_norrecordscan:readlen = 0x%08x,hasread = 0x%08x\n\r",\
                                readlen, hasread);
                    //扫描失败
                    result_bool = false;
                    continuescan = false;
                }
                else
                {
                    //用小端作为标准主要是因为小端是0，大小端都一样
                    if(((itemhead.recordendian == CN_CFG_LITTLE_ENDIAN)&&(CN_CFG_BYTE_ORDER != CN_CFG_LITTLE_ENDIAN))||\
                       ((itemhead.recordendian != CN_CFG_LITTLE_ENDIAN)&&(CN_CFG_BYTE_ORDER == CN_CFG_LITTLE_ENDIAN)))
                    {
                        //需要大小端转换
                        _exp_norrecordheadswap(&itemhead);
                    }
                    if(CN_EXP_RECORD_MAGICNUMBER != itemhead.magicnumber)//空闲的
                    {
                        result_bool = false;
                        continuescan = false;
                    }
                    else
                    {
                        itemnum ++;
                        if(itemnum == assignedno)
                        {
                            continuescan = false;
                            result_bool = true;
                        }
                        else
                        {
                            readaddr = itemhead.itemnextaddr;
                            continuescan = true;
                            result_bool = false;
                        }
                    }
                }
            }
        }
        if(true == result_bool)
        {
            readlen = itemhead.expinfolen;
            if(readlen > buflenlimit)
            {
                printk("readlen(%d) > buflenlimit(%d)",readlen, buflenlimit);
                result_bool = false;
            }
            else
            {
                readaddr = itemhead.expinfoaddr;
                readlen = Flash_ReadData(readaddr, buf, readlen);

                recordpara->headinfoaddr = (u32)buf;
                recordpara->headinfolen = itemhead.exp_headinfolen;
                recordpara->osstateinfoaddr = recordpara->headinfoaddr + \
                                             recordpara->headinfolen;
                recordpara->osstateinfolen = itemhead.exp_osstateinfolen;
                recordpara->hookinfoaddr = recordpara->osstateinfoaddr + \
                                             recordpara->osstateinfolen;
                recordpara->hookinfolen = itemhead.exp_hookinfolen;
                recordpara->throwinfoaddr = recordpara->hookinfoaddr + \
                                             recordpara->hookinfolen;
                recordpara->throwinfolen = itemhead.Exp_Throwinfolen;
            }
        }
    }
    return result_bool;
}

//norflash异常操作
struct tagExpRecordOperate sgexprecordopt_norflash = {
    .fnexprecord = Exp_Record_nor,
    .fnexprecordchecklen = Exp_RecordCheckLen_nor,
    .fnexprecordchecknum = Exp_RecordCheckNum_nor,
    .fnexprecordclear = Exp_RecordClear_nor,
    .fnexprecordget = Exp_RecordGet_nor,
    .fnexprecordscan = Exp_Recordscan_nor,
};

// =============================================================================
// 函数功能：注册NORFLASH作为异常存储方案
// 输入参数：无
// 返回值     ：true成功 false失败
// =============================================================================
bool_t Exp_RecordNorInit(void)
{
    bool_t result;
    result = Exp_RegisterRecordOpt(&sgexprecordopt_norflash);
    return result;
}

