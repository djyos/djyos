// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：brdexp.c
// 模块描述: 异常模块存储与解读，将AT45的最好1M配置为异常存储
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 12/03.2016
// =============================================================================
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "os.h"
#include "at45db321.h"
#include "exp.h"

//异常存储方案说明：
//1、使用AT45最后1M的空间存储异常信息；
//2、使用存储空间的超始RECORD_SIZE作为存储索引信息；
//3、从地址偏移为RECORD_SIZE顺序存入每条异常信息，每条异常信息包括：
//   16字节异常信息长度（四部分，分别为headinfolen,osstateinfolen,hookinfolen,throwinfolen）
//   和异常信息（分为四部分，分别为headinfo,osstateinfo,hookinfo,throwinfo)

#define RECPAGE_SIZE        (512)
#define RECORD_SIZE         (512*8)     //定义异常存储信息长度
#define RECORD_START_ADDR   (0x300000)  //存储的起始位置
#define RECORD_LENGTH       (0x100000)  //1M字节的存储空间
//static u8 sRecMsgBuf[RECORD_SIZE];

typedef struct _BrdExp_R
{
    u32 RecordNo;           //异常存储的序号
    u32 RecordAddr;         //该异常存储信息首地址（偏移首地址）
    u32 RecordLen;          //该异常存储信息总长度
}tagExpRecMsg;

static tagExpRecMsg sLastRecMsg;
// =============================================================================
// 函数功能：查询buf中最后一条异常记录条目
// 输入参数：无
// 输出参数：无
// 返回值  ：空
// =============================================================================
static tagExpRecMsg *__ExpRec_Traves(u8 *buf,u32 buflen)
{
    tagExpRecMsg *ExpRecMsg = NULL;
    u32 loop;

    for(loop = 0; loop < buflen -sizeof(tagExpRecMsg); )
    {
        if((u32)buf[loop] + 1 != (u32)buf[loop + sizeof(tagExpRecMsg)])
        {
            break;
        }
        loop += sizeof(tagExpRecMsg);
    }

    ExpRecMsg = (tagExpRecMsg *)&buf[loop];

    if(ExpRecMsg->RecordNo == 0xFFFFFFFF)
    {
        return NULL;
    }
    return ExpRecMsg;
}
// =============================================================================
// 函数功能：保存一帧异常信息到非易失存储器。存储或者输出，或者两者兼顾，由异常存储方案设计者决定
// 输入参数：recordpara,一帧异常信息
// 输出参数：
// 返回值  ：见enum EN_ExpDealResult
// =============================================================================
enum EN_ExpDealResult BrdExp_RecordSave(struct ExpRecordPara  *recordpara)
{
    tagExpRecMsg CurRecMsg;
    u32 addr;
    u8 buffer[16];

    if(NULL == recordpara)
        return EN_EXP_RESULT_PARAERR;

    //第一条异常信息
    if(sLastRecMsg.RecordLen == 0)
    {
        CurRecMsg.RecordNo = 1;
        CurRecMsg.RecordAddr = RECORD_START_ADDR + RECORD_SIZE;
    }
    else
    {
        CurRecMsg.RecordNo = sLastRecMsg.RecordNo + 1;
        CurRecMsg.RecordAddr = sLastRecMsg.RecordAddr + sLastRecMsg.RecordLen;
    }

    CurRecMsg.RecordLen = recordpara->headinfolen
                            + recordpara->hookinfolen
                            + recordpara->osstateinfolen
                            + recordpara->throwinfolen + 16;


    if( (sLastRecMsg.RecordNo > RECORD_LENGTH/sizeof(tagExpRecMsg)) ||
            (CurRecMsg.RecordAddr + CurRecMsg.RecordLen > RECORD_LENGTH +RECORD_START_ADDR))
    {
        return EN_EXP_RESULT_RECORD_NOSPACE;
    }

    //写入存储信息条目
    addr = RECORD_START_ADDR + (CurRecMsg.RecordNo - 1) * sizeof(tagExpRecMsg);
    AT45_FLASH_Write(addr,(u8*)&CurRecMsg,sizeof(tagExpRecMsg));

    //写入异常信息
    addr = CurRecMsg.RecordAddr;
    memcpy(buffer,      (u8*)&recordpara->headinfolen,4);
    memcpy(buffer + 4,  (u8*)&recordpara->osstateinfolen,4);
    memcpy(buffer + 8,  (u8*)&recordpara->hookinfolen,4);
    memcpy(buffer + 12, (u8*)&recordpara->throwinfolen,4);
    AT45_FLASH_Write(addr ,buffer,16);

    addr += 16;
    AT45_FLASH_Write(addr,(u8*)recordpara->headinfoaddr,recordpara->headinfolen);
    addr += recordpara->headinfolen;
    AT45_FLASH_Write(addr,(u8*)recordpara->osstateinfoaddr,recordpara->osstateinfolen);
    addr += recordpara->osstateinfolen;
    AT45_FLASH_Write(addr,(u8*)recordpara->hookinfoaddr,recordpara->hookinfolen);
    addr += recordpara->hookinfolen ;
    AT45_FLASH_Write(addr,(u8*)recordpara->throwinfoaddr,recordpara->throwinfolen);

    sLastRecMsg = CurRecMsg;
    return EN_EXP_RESULT_SUCCESS;
}

// =============================================================================
// 函数功能：fnExp_RecordClean
//          清除所有的异常信息，清除异常信息存储区域
// 输入参数：无
// 输出参数：无
// 返回值  ：TRUE 成功， FALSE失败
// =============================================================================
bool_t BrdExp_RecordClean(void)
{
    u32 StartAddr,EndAddr;
    u32 loop;

    StartAddr = RECORD_START_ADDR;
    EndAddr   = (RECORD_START_ADDR + RECORD_SIZE);
    for(loop = StartAddr;loop <= EndAddr;)
    {
        AT45_Page_Erase(loop);
        loop += RECPAGE_SIZE;
    }
    sLastRecMsg.RecordNo = 0;
    sLastRecMsg.RecordAddr = 0;
    sLastRecMsg.RecordLen = 0;
    return true;
}


// =============================================================================
// 函数功能：fnExp_RecordCheckNum
//          查看一共存储了多少条异常信息
// 输入参数：
// 输出参数：recordnum,存储的异常信息条目数
// 返回值  ：FALSE,失败 TRUE成功
// =============================================================================
bool_t BrdExp_RecordCheckNum(u32 *recordnum)
{
    if(NULL == recordnum)
        return false;

    *recordnum = sLastRecMsg.RecordNo;
    return true;
}


// =============================================================================
// 函数功能：fnBrdExp_RecordCheckLen
//          产看指定异常条目的长度
// 输入参数：assignedno,指定的条目
// 输出参数：recordlen,该条目的长度
// 返回值  ：FALSE,失败 TRUE成功
// =============================================================================
bool_t BrdExp_RecordCheckLen(u32 assignedno, u32 *recordlen)
{
    tagExpRecMsg *pRecMsg;
    u8 buffer[sizeof(tagExpRecMsg)];
    u32 addr;

    if( (NULL == recordlen) || (assignedno > sLastRecMsg.RecordNo) )
        return false;

    addr = RECORD_START_ADDR + (assignedno-1) * sizeof(tagExpRecMsg);
    //先读出存储条目信息
    AT45_FLASH_Read(addr,buffer,sizeof(tagExpRecMsg));
    //查询最后一条异常信息的位置和存储位置
    pRecMsg = (tagExpRecMsg *)buffer;

    *recordlen = pRecMsg->RecordLen - 4*4;

    return true;
}

// =============================================================================
// 函数功能：fnExp_RecordGet
//          从存储介质中获取指定条目的异常帧信息
// 输入参数：assignedno,指定的异常帧条目
//           buflen,缓冲区长度
// 输出参数：buf,用于存储获取指定条目的异常信息
//          recordpara,异常信息存储时的参数，在此是对buf的各个部分的定义
// 返回值  ：FALSE,失败 TRUE成功
// =============================================================================
bool_t BrdExp_RecordGet(u32 assignedno,u32 buflen,u8 *buf,\
                            struct ExpRecordPara *recordpara)
{
    tagExpRecMsg *pRecMsg;
    u32 addr;
    u8 buffer[16];

    if( (NULL == recordpara) || (NULL == buf))
        return false;

    addr = RECORD_START_ADDR + (assignedno-1) * sizeof(tagExpRecMsg);
    //先读出存储条目信息
    AT45_FLASH_Read(addr,buffer,sizeof(tagExpRecMsg));
    //查询最后一条异常信息的位置和存储位置
    pRecMsg = (tagExpRecMsg *)buffer;

    addr = pRecMsg->RecordAddr;
    AT45_FLASH_Read(addr,buffer,16);
    recordpara->headinfolen = *((u32*)&buffer[0]);
    recordpara->osstateinfolen = *((u32*)&buffer[4]);
    recordpara->hookinfolen = *((u32*)&buffer[8]);
    recordpara->throwinfolen = *((u32*)&buffer[12]);

    recordpara->headinfoaddr = (ptu32_t)&buf[0];
    recordpara->osstateinfoaddr = (ptu32_t)(buf + recordpara->headinfolen);
    recordpara->hookinfoaddr = (ptu32_t)(buf
                                            + recordpara->headinfolen
                                            + recordpara->osstateinfolen);
    recordpara->throwinfoaddr = (ptu32_t)(buf
                                            + recordpara->headinfolen
                                            + recordpara->hookinfolen
                                            + recordpara->osstateinfolen);
    addr += 16;
    AT45_FLASH_Read(addr,buf,buflen);

    //--TODO modified by zqf,read order must be identical with the save order
    //       otherwise you could not decode the record message as you wish

    return true;
}

// =============================================================================
// 函数功能：fnExp_RecordScan
//           开机的时候扫描异常存储记录，获取关键信息方便以后存储
// 输入参数：无
// 输出参数：无
// 返回值  ：空
// =============================================================================
void BrdExp_RecordScan(void)
{
    u8 buffer[50*sizeof(tagExpRecMsg)];
    tagExpRecMsg *pLastRecMsg;
    u32 addr,length;

    addr = RECORD_START_ADDR;
    length = 50*sizeof(tagExpRecMsg);

    do{
        AT45_FLASH_Read(addr,buffer,length);
        pLastRecMsg = __ExpRec_Traves(buffer,length);
        if(NULL != pLastRecMsg)
        {
            sLastRecMsg.RecordNo = pLastRecMsg->RecordNo;
            sLastRecMsg.RecordAddr = pLastRecMsg->RecordAddr;
            sLastRecMsg.RecordLen = pLastRecMsg->RecordLen;

            if( (sLastRecMsg.RecordNo%50) )
                break;
        }
        else
        {
            break;
        }
        addr += length;
    }while(addr < (RECORD_START_ADDR + RECORD_SIZE));

}


// =============================================================================
// 函数功能：fnExp_RecordScan
//           开机的时候扫描异常存储记录，获取关键信息方便以后存储
// 输入参数：无
// 输出参数：无
// 返回值  ：空
// =============================================================================
bool_t ModuleInstall_BrdExp(ptu32_t para)
{
    struct ExpRecordOperate ExpOpt;

	if(AT45_FLASH_Ready())
	{
		ExpOpt.fnExpRecordScan      = BrdExp_RecordScan;
		ExpOpt.fnExpRecordSave      = BrdExp_RecordSave;
		ExpOpt.fnExpRecordClean     = BrdExp_RecordClean;
		ExpOpt.fnExpRecordGet       = BrdExp_RecordGet;
		ExpOpt.fnExpRecordCheckNum  = BrdExp_RecordCheckNum;
		ExpOpt.fnExpRecordCheckLen  = BrdExp_RecordCheckLen;

		if(!Exp_RegisterRecorder(&ExpOpt))
			return false;
		else
			return true;
	}
	else
        return false;
}

