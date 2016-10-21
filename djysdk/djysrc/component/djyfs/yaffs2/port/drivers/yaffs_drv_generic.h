/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2011 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 */

#ifndef __YAFFS_NAND_DRV_H__
#define __YAFFS_NAND_DRV_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <driver/flash/flash.h>
#include "../../yaffs2-583dbd9/yaffs2-583dbd9/yaffs_guts.h"

struct FlashChip *Dev2Chip(struct yaffs_dev *Dev);
u8 *ChipBuf(struct yaffs_dev *Dev);
s32 IsErasedChunk(const u8 *Buf, s32 DataLen, s32 TagLen);
int yaffs_nand_install_drv(struct yaffs_dev *dev, struct FlashChip *chip);
int yaffs_nor_install_drv(struct yaffs_dev *dev, struct FlashChip *chip);

#ifdef __cplusplus
}
#endif

#endif /* __YAFFS_NAND_DRV_H__ */
