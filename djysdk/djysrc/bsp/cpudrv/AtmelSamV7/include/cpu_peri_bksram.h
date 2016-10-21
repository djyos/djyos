//-----------------------------------------------------------------------------
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：cpu_peri_BkSram.h
// 模块描述: BackUp Sram头文件
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 09/12.2014
// =============================================================================

#ifndef CPU_PERI_BACKUP_SRAM_H_
#define CPU_PERI_BACKUP_SRAM_H_

u32 BackUpSram_Read(u8 *rBuffer,u32 offset,u32 len);
u32 BackUpSram_Write(u8 *wBuffer,u32 offset,u32 len);
#endif /* CPU_PERI_BACKUP_SRAM_H_ */
