//----------------------------------------------------
//Copyright (C), 2013-2020,  lst.
//版权所有 (C), 2013-2020,   lst.
//所属模块:板件FPGA驱动
//作者：lst
//版本：V1.0.0
//文件描述:铁电读写timer 驱动程序
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-09-28
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

#include "stdint.h"
#include "cpu_peri.h"
#include "fram_alm_timer.h"
#include "chips.h"

#define CN_AlmTime_Fram_Addr (0x1FF0)//实时数据存放在铁电中的位置

//------写时钟到铁电----------------------------------------
//功能：将时钟存放到FRAM
//参数：数据源
//返回：数据个数
//----------------------------------------------------------
STATUS FRAM_Set_Alm_Time(tagAlmTime *pTime)
{
//    return (FRAM_Write_Data(1,CN_AlmTime_Fram_Addr,
//                    (u8 *)pTime,sizeof(tagAlmTime)));
}

//------从铁电读时钟----------------------------------------
//功能：从FRAM获取时钟
//参数：数据源
//返回值  ：数据个数
//----------------------------------------------------------
STATUS FRAM_Get_Alm_Time(tagAlmTime *pTime)
{
//    return (FRAM_Read_Data(1,CN_AlmTime_Fram_Addr,
//                    (u8 *)pTime,sizeof(tagAlmTime)));
}

