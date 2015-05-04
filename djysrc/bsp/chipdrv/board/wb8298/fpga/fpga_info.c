//----------------------------------------------------
//Copyright (C), 2013-2020,  lst.
//版权所有 (C), 2013-2020,   lst.
//所属模块:
//作者：lst
//版本：V1.0.0
//文件描述:从flash或FPGA获取板件IP和板号,获取FPGA版本号及时间，
//         获取B码校时等
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-09-28
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdlib.h"
#include "sunri_types.h"
#include "fpga_info.h"
#include "easynorflash\easynorflash.h"
#include "fpga_address.h"
#include "string.h"
#include "chips.h"
#include "cpld.h"

//----板件在装置中的编号----------------------------------------------------
//功能：从FPGA中读取板件在装置中的编号
//参数: 无
//返回: 编号
//--------------------------------------------------------------------------
/*UINT8 Hard_BoardNo_Read( void )
{
    UINT8   byBoard;
//    byBoard = (UINT8)((*(UINT32*)CN_HARD_HARD_BASE) & 0x00FF);

    byBoard = 10;
    return(byBoard);
}
*/

//----读取FCC0的IP地址-------------------------------------------------------
//功能：从flash中读取读取FCC0的IP地址
//参数: 无
//返回: 编号
//--------------------------------------------------------------------------
void GetFXIP(UINT8*  pbybuf,UINT8 *fcc0_ip)
{
//    extern u8 fcc0_ip[4];
    if((fcc0_ip[4] > 0) && (fcc0_ip[4] < 0xFF))
    {
        memcpy(pbybuf,fcc0_ip,CN_FLASH_IP_LEN);
    }
    else
        memset(pbybuf,0xFF,CN_FLASH_IP_LEN);
}

//----获取当前mac/ip值-------------------------------------------------------
//功能：从FPGA/flash中读取板件在装置中的编号
//参数: 无
//返回: 编号
//--------------------------------------------------------------------------
void GetNetFXIP( UINT8  *pbybuf,UINT8 *fcc0_ip )
{
    UINT8 byBoardNo = Hard_BoardNo_Read();

    if(0==byBoardNo)
        byBoardNo = 204;

    GetFXIP(pbybuf,fcc0_ip);
    if(0xff==pbybuf[0])
    {
        pbybuf[0] = 192;
        pbybuf[1] = 168;
        pbybuf[2] = 253;
        pbybuf[3] = byBoardNo;
    }
}

//------获取时间------------------------------------------------------------
//功能：获取B码校时接收的时/分/秒，读FPGA
//参数：无
//返回：返回时间
//--------------------------------------------------------------------------
UINT32 FpgaTmr_TrigB_GetTime(void)
{
    return (*(volatile UINT32*)(CN_FPGA_TIMER_DATA_L));
}
//-------获取校准时间-------------------------------------------------------
//功能：获取B码校时接收的年和天数，读FPGA
//参数：无
//返回： 返回年和天数
//--------------------------------------------------------------------------
UINT32 FpgaTmr_TrigB_GetDate(void)
{
    return (*(volatile UINT32*)(CN_FPGA_TIMER_DATA_H));
}

//-------B码1读取锁存校时标志-----------------------------------------------
// 功能：B码1读取锁存校时标志
// 参数：无
//返回值：  TRUE:PPS锁存成功
//             FALSE：PPS未锁存
//--------------------------------------------------------------------------
BOOLEAN FpgaTmr_Tmr_GetPPSFlg(void)
{
    register UINT32     *pTicksFlg;                     // 锁存标志临时变量
    register UINT8      iGetFlg;

    pTicksFlg = (UINT32 *)(CN_FPGA_TIMER_FLAG);         // 读取锁存标志
    iGetFlg   = (UINT8)(*pTicksFlg & 0x000000ff);
    if( iGetFlg !=0 )
    {
        return TRUE;                                    // PPS锁存成功
    }
    else
    {
        return FALSE;                                   // 未锁存
    }
}

// -----B码清除锁存校时标志-----------------------------------------------
//功能：B码清除锁存校时标志
//参数：无
//返回：无
// -----------------------------------------------------------------------
void FpgaTmr_Tmr_ClrFlag(void)
{
    (*(volatile UINT32*)(CN_FPGA_TIMER_CLR_FLAG)) = 0;      // 清锁存校时标志
}


