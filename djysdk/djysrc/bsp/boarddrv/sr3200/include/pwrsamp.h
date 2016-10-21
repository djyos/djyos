// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名    ：pwrsamp.h
// 模块描述: SR3200板上+24V电压采集模块
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 1/11.2016
// =============================================================================


#ifndef __PWR_SAMPLE__
#define __PWR_SAMPLE__

void PwrSamp_Init(void);
bool_t PwrSamp_GetValue(u32 *Buf);

#endif
