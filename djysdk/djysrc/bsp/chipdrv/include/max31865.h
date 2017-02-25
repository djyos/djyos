// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：max31865.h
// 模块描述: 温度检测芯片max31865的驱动
// 模块版本: V1.00
// 创建人员:
// 创建时间: 2/9.2017
// =============================================================================

#ifndef _CHIP_MAX31865_H_
#define _CHIP_MAX31865_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MAX31865_CONTINOUS_MODE      0		//连续采样模式
#define MAX31865_SINGLE_MODE	 	 1		//单次触发采样模式

bool_t ModuleInstall_Max31865(char *BusName,u8 SampleMode);
float Max31865_TemperatureGet(void);

#ifdef __cplusplus
}
#endif

#endif





