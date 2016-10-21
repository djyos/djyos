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

#include "stdint.h"
#include "board-cfg.h"
#include "cpld.h"


#define cn_cpld_led_addr                 (cn_sys_cpld_addr_base + 0x08)
#define cn_cpld_boardno_addr             (cn_sys_cpld_addr_base + 0x02) //从CPLD读板件号地址
#define cn_cpld_info_addr                (cn_sys_cpld_addr_base + 0x20) //从CPLD读板件信息基址


void  CPLD_LedOn(u8 num)
{
    if(num > 2)
    {
        return;
    }
    else
    {
//      *(char *)(cn_cpld_led_addr + num) = 0x00;
    }
}

void  CPLD_LedOff(u8 num)
{
    if(num > 2)
    {
        return;
    }
    else
    {
//      *(char *)(cn_cpld_led_addr + num) = 0xff;
    }
}
//----板件在装置中的编号----------------------------------------------------
//功能：从CPLD中读取板件在装置中的编号
//参数: 无
//返回: 编号
//--------------------------------------------------------------------------
uint8_t Hard_BoardNo_Read( void )
{
    uint8_t   byBoard;
//    __asm__ __volatile__("sync; isync; msync");
    byBoard = (uint8_t)((*(u8*)cn_cpld_boardno_addr) & 0x00FF);
//    __asm__ __volatile__("sync; isync; msync");
//    byBoard = 10;
    return(byBoard);
}

// -----获取CPLD版本和时间------------------------------------------------
//功能：获取CPLD的版本信息和版本时间，读FPGA相应地址
//参数：tgfpgaver：版本信息的结构体
//返回：无
// -----------------------------------------------------------------------
void Cpld_GetInfo(struct CpldVer *tgfpgaver)
{
    tgfpgaver->dwCpldYearHi    = *(uint8_t *)(cn_cpld_info_addr + 0x00);    //年高
    tgfpgaver->dwCpldYearLo    = *(uint8_t *)(cn_cpld_info_addr + 0x01);  //年低
    tgfpgaver->dwCpldMonth     = *(uint8_t *)(cn_cpld_info_addr + 0x02);  //月
    tgfpgaver->dwCpldDay       = *(uint8_t *)(cn_cpld_info_addr + 0x03);  //日
    tgfpgaver->dwBoardTypeHi   = *(uint8_t *)(cn_cpld_info_addr + 0x04);  //版件型号高
    tgfpgaver->dwBoardTypeLo   = *(uint8_t *)(cn_cpld_info_addr + 0x05);  //版件型号低
    tgfpgaver->dwProgVerHi     = *(uint8_t *)(cn_cpld_info_addr + 0x06);  //CPLD程序版本高
    tgfpgaver->dwProgVerLo     = *(uint8_t *)(cn_cpld_info_addr + 0x07);  //CPLD程序版本低
}
