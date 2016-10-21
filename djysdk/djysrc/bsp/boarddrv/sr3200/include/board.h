// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名    ：board.h
// 模块描述: 板件的一些函数声明
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 11/11.2015
// =============================================================================

#ifndef __BOARD__H__
#define __BOARD__H__


void BrdWdt_FeedDog(void);
void BrdWdt_ModuleInit(void);

void Board_IOInit(void);
void RUN_Status(u8 Run);
void LOCK1_Lock(void);
void LOCK1_UnLock(void);
void LOCK2_Lock(void);
void LOCK2_UnLock(void);
u8 KI_GetValue(void);

void FPGA_ModuleInit(void);


void L138COM_ModuleInit(void);
u32	L138COM1_Send(u8 *buf,u32 len);
u32	L138COM1_Receive(u8 *buf,u32 len,u32 wait_ms);
u32	L138COM2_Send(u8 *buf,u32 len);
u32	L138COM2_Receive(u8 *buf,u32 len,u32 wait_ms);





#endif






