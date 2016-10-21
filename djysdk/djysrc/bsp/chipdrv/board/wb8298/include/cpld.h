//----------------------------------------------------
//Copyright (C), 2013-2020,  lst.
//版权所有 (C), 2013-2020,   lst.
//所属模块:
//作者：lst
//版本：V1.0.0
//文件描述:WB8298板级cpld中功能模块，如获取板件号、板件信息等
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2014-02-12
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

#ifndef __CPLD_H__
#define __CPLD_H__

//读取FPGA版本信息的版本时间
struct CpldVer
{
    uint8_t dwCpldYearHi;               //CPLD版本时间年H
    uint8_t dwCpldYearLo;               //CPLD版本时间年L
    uint8_t dwCpldMonth;                //CPLD版本时间月
    uint8_t dwCpldDay;                  //CPLD版本时间日
    uint8_t dwBoardTypeHi;              //板件类型高
    uint8_t dwBoardTypeLo;              //板件类型低
    uint8_t dwProgVerHi;                //CPLD程序版本高
    uint8_t dwProgVerLo;                //CPLD程序版本低
};


uint8_t Hard_BoardNo_Read( void );
void CpldVer_GetInfo(struct CpldVer *tgfpgaver);




#endif

