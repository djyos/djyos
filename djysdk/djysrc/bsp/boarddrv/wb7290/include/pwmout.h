// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：LED.H
// 模块描述: led驱动头文件
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 10/19.2016
// =============================================================================

#ifndef __PWM_OUT_H__
#define __PWM_OUT_H__

#include <stdint.h>

#define PWM_OUT_0	1
#define PWM_OUT_1	3


void PWM_Config(u8 idx,uint32_t cycle,uint32_t duty);

#endif









