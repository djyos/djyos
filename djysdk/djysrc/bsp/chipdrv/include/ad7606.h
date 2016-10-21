// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：AD7606.h
// 模块描述: AD7606采集模块的底层驱动头文件
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 09/04.2015
// =============================================================================
#ifndef __AD7606_H__
#define __AD7606_H__

#include "stdint.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

void AD7606_Init(void);
bool_t AD7606_GetValue(uint16_t *AinBuf);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __AD7606_H__ */
