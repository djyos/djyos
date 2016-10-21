// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：LED.H
// 模块描述: led驱动头文件
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 07/03.2016
// =============================================================================

#ifndef __LED_H__
#define __LED_H__

#define LEDRUN     16

void Led_Init(void);
void Led_RunOn(void);
void Led_RunOff(void);
void Led_Out(u8 (*p)[4]);
#endif

