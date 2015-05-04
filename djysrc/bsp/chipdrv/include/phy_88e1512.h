//----------------------------------------------------
//Copyright (C), 2013-2020,  luoshitian.
//版权所有 (C), 2013-2020,   罗侍田.
//所属模块: 88E1512 phy芯片函数
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 88E1512 phy芯片驱动函数
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-11-14
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

#ifndef __PHY_88E1512_H__
#define __PHY_88E1512_H__



struct phy_parameter
{
    u32 speed;
    u32 duplex;
};

s32 genphy_update_link_88E1512(u32 addr);
struct phy_parameter genphy_parse_link__88E1512(u32 addr);
s32 phy_config_88e1512(s32 addr,s32 supported);
void phy_reset_88e1512(s32 addr);

#endif









