//----------------------------------------------------
//Copyright (C), 2004-2009,  lst.
//版权所有 (C), 2004-2009,   lst.
//所属模块:中断模块
//作者：lst
//版本：V1.0.0
//文件描述: 定义中断号
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009--9-28
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef ____CPU_PERI_IWDG_H___
#define ____CPU_PERI_IWDG_H___

#ifdef __cplusplus
extern "C" {
#endif

struct st_wdt_reg
{
    vu32  wdmod;
    vu32 wdtc;
    vu32  wdfeed;
    vu32 wdtv;
    vu32 wdclksel;
 };

#ifdef __cplusplus
}
#endif

#endif //____CPU_PERI_IWDG_H___
