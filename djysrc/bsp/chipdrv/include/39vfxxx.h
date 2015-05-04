//----------------------------------------------------
//Copyright (C), 2004-2009,  都江堰操作系统开发团队
//版权所有 (C), 2004-2009,   都江堰操作系统开发团队
//所属模块:sst39vf160驱动
//作者：lst
//版本：V1.0.0
//文件描述:sst39vf160 flash驱动程序，提供flash的原始操作函数
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __flash_H__
#define __flash_H__
#ifdef __cplusplus
extern "C" {
#endif

#define flash_size              0x800000
#define flash_sector_length     4096
#define flash_sector_sum        (flash_size/flash_sector_length)
#define flash_block_length      65536
#define flash_block_sum         (flash_size/flash_block_length)
#define flash_base_addr         0x60000000
struct flash_info
{
    u16 mft_id;
    u16 device_id;
    u32 sector_size;
    u32 block_size;
    u32 block_sum;
};
#ifdef __cplusplus
}
#endif
#endif  //   __flash_H__
