/* Copyright 2007-2010 ShenZhen Nari, Inc.*/
/*modification history
struct for app...

Writen By zhangji, 2007,10,19
*/
//****************************************************************************************
// fpga timer
//****************************************************************************************

#ifndef _FPGATIMER_82X_H
#define _FPGATIMER_82X_H

#include "sunri_types.h"

#ifdef __cplusplus
extern "C" {
#endif


extern  UINT32      FpgaTmr_GetFrq(void);
extern  void        FpgaTmr_SetCps(UINT8 byCps10m,    UINT8 byCps100m, UINT8 byCpss);
extern  void        FpgaTmr_InitReg(UINT32 dwcintfrq, UINT8 byCps10m,UINT8 byCps100m, UINT8 byCpss);
extern    void        FpgaTmr_SetSamCmp(UINT16 wSamCmp);
extern    void        FpgaTmr_ManuTimeAdd(SINT16 nSamCmp);
extern    UINT16         FpgaTmr_GetSamCmp(void);
extern  void        FpgaTmr_GetCmpReg(UINT8 * pbyGetPtr);
extern  void        FpgaTmr_SetMain(BOOL bSetMain);
extern  BOOL        FpgaTmr_GetMainSta(void);
extern  void        FpgaTmr_SetAutoLoad(BOOL bSetAutoLoad);
extern  BOOL        FpgaTmr_GetAutoReloadSta(void);
extern  BOOL        FpgaTmr_GetLvdsSta(void);
//extern  void        FpgaTmr_ManuLoad(UINT16 wSetCnt,UINT8 bySet10ms, UINT8 bySet100ms, UINT8 bySetSec, UINT8 bySetMin, UINT8 bySetHour);
extern  void        FpgaTmr_ManuLoad(UINT16 wSetCnt,UINT8 bySet10ms, UINT8 bySet100ms);
extern  void         FpgaTmr_ManuTimeLoad(UINT8 bySetSec, UINT8 bySetMilSecL, UINT8 bySetMilSecH);
extern  void        FpgaTmr_GetSetReg(UINT8 * pbyGetPtr);
extern  void        FpgaTmr_GetCntReg(UINT8 * pbyGetPtr);
extern  void        FpgaTmr_GetTime(UINT8 * pbyGetPtr);
extern  UINT16      FpgaTmr_GetCintCnt(void);
extern  BOOL        FpgaTmr_GetCintFlg(void);
extern  void        FpgaTmr_SetCintFlg(void);
extern    BOOL         FpgaTmr_Get1588Flg(void);
extern    void         FpgaTmr_Set1588Flg(void);
extern    UINT16         FpgaTmr_Get1588Reg(UINT8 * pbyCintCntLatch,UINT8 * pbyMsCntLatch);
extern    void         FpgaTmr_Set1588Mac(UINT8 * pbyMacAddr);
extern    BOOL         FpgaTmr_Get1588FlgAB(UINT32 dwPortNum);
extern    void         FpgaTmr_Set1588FlgAB(UINT32 dwPortNum);
extern    UINT16         FpgaTmr_Get1588RegAB(UINT32 dwPortNum,UINT8 * pbyCintCntLatch,UINT8 * pbyMsCntLatch);
extern    void         FpgaTmr_Set1588MacAB(UINT32 dwPortNum,UINT8 * pbyMacAddr);
extern    void         Fpga_SetMFun(void);
extern  UINT32      IEEE1588_GETFLAG(UINT8 macip);
extern  void        IEEE1588_SETFLAG(UINT32 data,UINT8 macip);
extern  UINT32      IEEE1588_GETTIME(UINT8 data,UINT8 macip);

#ifdef __cplusplus
}
#endif

#endif /* _FPGATIMER_H */

