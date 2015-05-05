// =============================================================================
// Copyright (C) 2012-2020 都江堰操作系统研发团队  All Rights Reserved
// 文件名     ：cpu_peri_iic.c
// 模块描述: FRAM模块的器件驱动程序，由器件驱动程序员完成
// 模块版本: V1.00
// 创建人员: hm
// 创建时间: 20/06.2014
// =============================================================================

#ifndef FRAM_FM24CL64B_DRIVER_H_
#define FRAM_FM24CL64B_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

#define CN_FRAM0_NO    0
#define CN_FRAM1_NO    1

u32 FRAM_Read_Data(u8 fram_no, u32 Fram_DstAddr,u8 *pDstBuf,u32 DstLen);
u32 FRAM_Write_Data(u8 fram_no ,u32 Fram_DstAddr,u8 *pSrcBuf,u32 SrcLen);
u32 FRAM_Read_DWord(u8 fram_no,u32 Fram_DstAddr);
u32 FRAM_Write_DWord(u8 fram_no,u32 Fram_DstAddr,u32 u32data);
u16 FRAM_Read_Word(u8 fram_no,u32 Fram_DstAddr);
u32 FRAM_Write_Word(u8 fram_no,u32 Fram_DstAddr,u16 u16data);

// 带互斥IIC操作函数
u16   FRAM_Read_Word_r   (u8 fram_no, u16 wAddress );
s32   FRAM_Write_Word_r  (u8 fram_no, u16 wAddress, u16 wData );
u32   FRAM_Read_Data_r   (u8 fram_no, u16 wAddress, u8 *pbyBuf, u32 dwLen );
u32   FRAM_Write_Data_r  (u8 fram_no, u16 wAddress, u8 *pbyBuf, u32 dwLen );

ptu32_t ModuleInit_Fram(ptu32_t para);

#ifdef __cplusplus
}
#endif


#endif
