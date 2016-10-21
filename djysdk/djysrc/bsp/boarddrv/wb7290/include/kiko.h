/*
 * key.h
 *
 *  Created on: 2016Äê3ÔÂ14ÈÕ
 *      Author: zhangqf
 */

#ifndef _BOARDDRV_INCLUDE_KIKO_H_
#define _BOARDDRV_INCLUDE_KIKO_H_

extern void KO_Init(void);
//void KO_Write(u32 dwKO);
extern void KO_Write(u32 *p);
extern void KO_StartRly(u8 flag);
extern void CpuLedCtrl(u8 flag,u8 Num);
extern void KI_Init(void);
extern void KI_Read(u8 *buf);
extern u8 	GPS_Read(void);
extern u8   ZZYC_Read(void);
extern u8   QDJ1_Read(void);


#endif
