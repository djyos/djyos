//----------------------------------------------------
//Copyright (C), 2013-2020,  lst.
//版权所有 (C), 2013-2020,   lst.
//所属模块:
//作者：lst
//版本：V1.0.0
//文件描述:从flash或FPGA获取板件IP、FPGA日期、版本等信息
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-09-28
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef _BOARDIP_FPGA_FLASH__H
#define _BOARDIP_FPGA_FLASH__H

#define CN_FLASH_MAC_LEN                0x06
#define CN_FLASH_IP_LEN                    0x04
#include "sunri_types.h"
//读取FPGA版本信息的版本时间,改到cpld
/*struct FpgaVer
{
    UINT32 dwBoardTypeLo;           //FPGA版本信息低
    UINT32 dwBoardTypeHi;           //FPGA版本信息高
    UINT32 dwFpgaDay;               //FPGA版本时间天
    UINT32 dwFpgaMonth;             //FPGA版本时间月
    UINT32 dwFpgaYear;                //FPGA版本时间年
};*/


// 写入flash值
extern         void        WriteFcc0Mac  ( UINT8  *pbybuf );//条件调用未实现
extern         void        WriteFcc0IP   ( UINT8  *pbybuf );
extern         void        WriteFcc1Mac  ( UINT8  *pbybuf );//条件调用未实现
extern         void        WriteFcc1IP   ( UINT8  *pbybuf );
extern         void        WriteFXMac    ( UINT8  *pbybuf );
extern         void        WriteFXIP     ( UINT8  *pbybuf );

// 获取flash中保存值
extern         void        GetFcc0Mac    ( UINT8  *pbybuf );
extern         void        GetFcc0IP     ( UINT8  *pbybuf );
extern         void        GetFcc1Mac    ( UINT8  *pbybuf );
extern         void        GetFcc1IP     ( UINT8  *pbybuf );
extern         void        GetFXMac      ( UINT8  *pbybuf );
extern         void        GetFXIP       ( UINT8  *pbybuf ,UINT8 *fcc0_ip);

// 获取当前IP、MAC
extern         void        GetNetFcc0Mac ( UINT8  *pbybuf );   //for pub
extern         void        GetNetFcc0IP  ( UINT8  *pbybuf );   //for pub
extern         void        GetNetFcc1Mac ( UINT8  *pbybuf );   //for pub
extern         void        GetNetFcc1IP  ( UINT8  *pbybuf );   //for pub
extern         void        GetNetFXMac   ( UINT8  *pbybuf );   //for 827
extern         void        GetNetFXIP    ( UINT8  *pbybuf ,UINT8 *fcc0_ip);   //for 827 is used


//void FpgaVer_GetInfo(struct FpgaVer *tgfpgaver);
void FpgaTmr_Tmr_ClrFlag(void);
BOOLEAN FpgaTmr_Tmr_GetPPSFlg(void);
UINT32 FpgaTmr_TrigB_GetDate(void);
UINT32 FpgaTmr_TrigB_GetTime(void);

void GetNetFXIP( UINT8  *pbybuf ,UINT8 *fcc0_ip);
//UINT8 Hard_BoardNo_Read( void );

#endif
