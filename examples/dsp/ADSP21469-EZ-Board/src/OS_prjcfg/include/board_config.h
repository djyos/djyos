//----------------------------------------------------
//Copyright (C), 2013-2020,  都江堰操作系统开发团队
//版权所有 (C), 2013-2020,   都江堰操作系统开发团队
//所属模块:s29glxxx驱动
//作者：hm
//版本：V1.0.1
//文件描述:与具体板件相关的配置
//其他说明:
//修订历史:
//1. 日期: 2014-04-25
//   作者: hm
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

#ifndef _BOARD_CONFIG_H
#define _BOARD_CONFIG_H


/*____与硬件相关的配置____*/
#define Mhz 1000000
#define cfg_mclk (400*Mhz)  //主频
//#define cfg_fclk cfg_mclk  //cm3自由运行外设时钟
//#define cfg_ahbclk cfg_mclk  //总线时钟
//#define cfg_pclk2 cfg_mclk  //高速外设时钟
#define cfg_pclk1 (cfg_mclk/2)  //低速外设时钟
#define cfg_extclk 25000000  //外部时钟=8M
//#define cfg_core_ccb_clk (400*Mhz)
//#define cfg_core_implete_clk (cfg_core_ccb_clk/8)  //tbclk
//#define cfg_rtc_clk 800*Mhz

/*____板级外设设置____*/
#define easynorflash_start_sector    	0 	//定义简易文件系统起始块,范围：0~39(重新映射后),0=hw block 31,39=hw block 70
#define easynorflash_length_sector		40  //定义文件系统占用sector个数
#define cn_norflash_bus_width		   	8
//SPI_FLASH_M25P16
#define CFG_FLASH_SPI_BUS		g_ptSpi0Reg        //FLASH使用的SPIx
#define CFG_FLASH_SPI_CS		0			//SS0
#define CFG_FLASH_SPI_SPEED		1000000     //FLASH传递的SPI速度 600kHz


#endif









