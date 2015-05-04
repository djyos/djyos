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
#include "config-prj.h"
/*____与硬件相关的配置____*/
#define Mhz 1000000
#define CN_CFG_MCLK (400*Mhz)  //主频
//#define CN_CFG_FCLK CN_CFG_MCLK  //cm3自由运行外设时钟
//#define CN_CFG_AHBCLK CN_CFG_MCLK  //总线时钟
//#define CN_CFG_PCLK2 CN_CFG_MCLK  //高速外设时钟
#define CN_CFG_PCLK1 (CN_CFG_MCLK/2)  //低速外设时钟
#define CN_CFG_EXTCLK 25000000  //外部时钟=8M
//#define cfg_core_ccb_clk (400*Mhz)
//#define cfg_core_implete_clk (cfg_core_ccb_clk/8)  //tbclk
//#define cfg_rtc_clk 800*Mhz

#if(cn_cfg_board == CN_BOARD_P1020RDB)
#define CN_CFG_EASYNOR_START_SECTOR       0 //定义简易文件系统起始块
#define CN_CFG_EASYNOR_LENGTH_SECTOR      50  //定义文件系统占用sector个数
#define cn_norflash_bus_width		   16
#elif(cn_cfg_board == CN_BOARD_WB8298)

/*定义文件系统起始和结束块号*/
#define CN_CFG_EASYNOR_START_SECTOR       0   //定义简易文件系统起始块
#define CN_CFG_EASYNOR_LENGTH_SECTOR      50  //定义文件系统占用sector个数

#define cn_norflash_bus_width		   16 /*定义flash总线宽度*/
#define cn_norflash_buf_size           512//定义norflash缓冲区大小

//RTC_DS1390
#define CFG_RTC_SPI_BUS			g_ptSpi_Reg //RTC使用的SPI
#define CFG_RTC_SPI_CS			0			//CS0
#define CFG_RTC_SPI_SPEED		600000      //RTC传递的SPI速度 600kHz

#elif(cn_cfg_board == CN_BOARD_WB9260)
/*____板级外设设置____*/
#define CN_CFG_EASYNOR_START_SECTOR       40 //定义简易文件系统起始块
#define CN_CFG_EASYNOR_LENGTH_SECTOR      192  //定义文件系统占用sector个数
#define cn_norflash_bus_width		   8
#define cn_norflash_buf_size            64//定义norflash缓冲区大小
//RTC_DS1390
#define CFG_RTC_SPI_BUS			g_ptSpi0Reg //RTC使用的SPIx
#define CFG_RTC_SPI_CS			2			//SS2
#define CFG_RTC_SPI_SPEED		600000      //RTC传递的SPI速度 600kHz
//SPI_FLASH_AT45DB321D
#define AT45_Page_Size			512
#define CFG_FLASH_SPI_BUS		g_ptSpi0Reg //FLASH使用的SPIx
#define CFG_FLASH_SPI_CS		0			//SS0
#define CFG_FLASH_SPI_SPEED		1000000     //FLASH传递的SPI速度 600kHz

#elif (cn_cfg_board == ADSP21469_EZ_Board)
/*____板级外设设置____*/
#define easynorflash_start_sector    	0 	//定义简易文件系统起始块,范围：0~39(重新映射后),0=hw block 31,39=hw block 70
#define easynorflash_length_sector		40  //定义文件系统占用sector个数
#define cn_norflash_bus_width		   	8
//SPI_FLASH_M25P16
#define CFG_FLASH_SPI_BUS		g_ptSpi0Reg        //FLASH使用的SPIx
#define CFG_FLASH_SPI_CS		0			//SS0
#define CFG_FLASH_SPI_SPEED		1000000     //FLASH传递的SPI速度 600kHz
#endif

#endif









