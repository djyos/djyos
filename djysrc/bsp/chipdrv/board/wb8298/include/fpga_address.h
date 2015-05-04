//----------------------------------------------------
//Copyright (C), 2013-2020,  lst.
//版权所有 (C), 2013-2020,   lst.
//所属模块:板级外设驱动
//作者：lst
//版本：V1.0.0
//文件描述:FPGA的地址,该部分地址不属于某个特定模块
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-09-28
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __FPGA_ADDRESS_H_
#define __FPGA_ADDRESS_H_


#ifdef __cplusplus
extern "C" {
#endif

//-----todo
//由于p1020通过PCIE访问FPGA,因此本文件中的地址与PCIE分配的地址相关
//目前PCIE访问FPGA的地址为0xA0000000
#define CN_FPGA_BASE_ADDR                (0xA0000000)

//FPGA所带信息地址
#define CN_ADDR_BOARDTYPE_LO             (CN_FPGA_BASE_ADDR + 0x3FC08)        // FPGA所带的板件类型低位29(WB82901)‘WB8’被省去，末位1表示FPGA小版本
#define CN_ADDR_BOARDTYPE_HI             (CN_FPGA_BASE_ADDR + 0x3FC0C)        // FPGA所带的板件类型低位01(WB82901)‘WB8’被省去，末位1表示FPGA小版本
#define CN_ADDR_FPGADATE_DAY             (CN_FPGA_BASE_ADDR + 0x3FC10)        // FPGA版本日期-日
#define CN_ADDR_FPGADATE_MONTH           (CN_FPGA_BASE_ADDR + 0x3FC14)        // FPGA版本日期-月
#define CN_ADDR_FPGADATE_YEAR            (CN_FPGA_BASE_ADDR + 0x3FC18)        // FPGA版本日期-年
#define CN_ADDR_FPGAINFO_BASE            (CN_FPGA_BASE_ADDR + 0x3FC08)        // FPGA信息起始地址
#define CN_ADDR_FPGAINFO_UINT32LEN       (5)                                  // FPGA信息的UINT32个数

//FPGA读板件号地址
#define CN_HARD_HARD_BASE                (CN_FPGA_BASE_ADDR + 0x3FC1C)

//FPGA的timer模块基址
#define FPGA_TIMER_ADDRESS               (CN_FPGA_BASE_ADDR + 0x02000)

//主从FPGA配置地址
#define FPGA_MS_ADDRESS                  (CN_FPGA_BASE_ADDR + 0x0C000)

//LVDSIO基地
#define LVDS_IO_ADDRESS                  (CN_FPGA_BASE_ADDR + 0x00000)


//B码相关寄存器地址定义
#define  CN_FPGA_TIMER_CLR_FLAG          (CN_FPGA_BASE_ADDR + 0x23808)           // B码清锁存校时标志的地址
#define  CN_FPGA_TIMER_FLAG              (CN_FPGA_BASE_ADDR + 0x2380c)           // B码读锁存校时标志的地址
#define  CN_FPGA_TIMER_DATA_L            (CN_FPGA_BASE_ADDR + 0x23810)           // B码校时接收的时/分/秒
#define  CN_FPGA_TIMER_DATA_H            (CN_FPGA_BASE_ADDR + 0x23814)           // B码校时接收的年和天数


#ifdef __cplusplus
}
#endif


#endif
