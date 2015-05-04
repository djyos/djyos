/* Copyright 2007-2010 ShenZhen Nari, Inc.*/
/*modification history
addrv.h

Writen By zhangji, 2007,10,19
*/
#ifndef _ADDRV_H
#define _ADDRV_H

#ifdef __cplusplus
extern "C" {
#endif
#include "sunri_types.h"

extern void         Ad_InitReg(UINT16 wchipnum);
extern UINT16 *     Ad_Read();
extern void         Ad_SampleHold(void);

#ifdef __cplusplus
}
#endif

#endif /* _ADDRV_H */

