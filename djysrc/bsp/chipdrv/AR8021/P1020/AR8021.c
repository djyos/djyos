//----------------------------------------------------
//Copyright (C), 2013-2020,  luoshitian.
//版权所有 (C), 2013-2020,   罗侍田.
//所属模块: AR8021 phy芯片函数
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: AR8021 phy芯片驱动函数
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-11-14
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
extern s32 tsec_phy_write(s32 addr, s32 dev_addr, s32 regnum,
                   s16 value);
/**
 * genphy_config_aneg - restart auto-negotiation or write BMCR
 * @phydev: target phy_device struct
 *
 * Description: If auto-negotiation is enabled, we configure the
 *   advertising, and then restart auto-negotiation.  If it is not
 *   enabled, then we write the BMCR.
 */
//----phy初始化函数-----------------------------------------------------------------
//功能：配置Atheros PHY寄存器，只针对p1020rdb中etsec3口的ar8201 phy芯片
//参数：int addr，phy芯片地址，int supported，phy芯片支持的数据传输模式设置
//返回：返回0
//-----------------------------------------------------------------------------
s32 phy_config_ar8021(s32 addr,s32 supported)
{
    tsec_phy_write(addr, -1, 0x1d, 0x05);
    tsec_phy_write(addr, -1, 0x1e, 0x3D47);

    return 0;
}
