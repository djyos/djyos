//----------------------------------------------------
//Copyright (C), 2004-2011,  djyos team.
//版权所有 (C), 2004-2011,   djyos团队.
//作者：djyos团队
//版本：V1.0.0
//文件描述: s3c2440看门狗模块驱动
//其他说明:
//修订历史:
// 1. 日期: 2011-11-29
//   作者: djyos团队
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __WDT_HARD_H__
#define __WDT_HARD_H__

#ifdef __cplusplus
extern "C" {
#endif

void wdt_star(void);
void wdt_reboot(void);
void wdt_hard_reset(void);
void wdt_clear(void);
void wdt_init_hard(void);
#ifdef __cplusplus
}
#endif

#endif //__WDT_HARD_H__

