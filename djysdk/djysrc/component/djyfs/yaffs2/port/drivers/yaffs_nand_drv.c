/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2011 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 * This is an interface module for handling NAND in yaffs2 mode.
 */

/* This code calls a driver that accesses "generic" NAND. In the simulator
 * this is direceted at a file-backed NAND simulator. The NAND access functions
 * should also work with real NAND.
 *
 * This driver is designed for use in yaffs2 mode with a 2k page size and
 * 64 bytes of spare.
 *
 * The spare ares is used as follows:
 * offset 0: 2 bytes bad block marker.
 * offset 2: 8x3 bytes of ECC over the data.
 * offset 26: rest available to store Yaffs tags etc.
 */

#include <lock.h>
#include "yaffs_drv_generic.h"
#include "../../yaffs2-583dbd9/yaffs2-583dbd9/direct/yportenv.h"

//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static int yaffs_nand_drv_WriteChunk(struct yaffs_dev *dev, int nand_chunk,
                   const u8 *data, int data_len,
                   const u8 *oob, int oob_len)
{
    //逻辑：
    //   操作上带ECC的写，每页只能写一次的。

    struct FlashChip *Chip = Dev2Chip(dev);
    struct NandDescr *NandInfo = &Chip->Descr.Nand;
    struct FlashOps *NandOps = &Chip->Ops;
    u8 *Buf = ChipBuf(dev);
    u32 Flags = 0;
    s32 Ret;

#if 0// todo: yaffs调用时,存在data==NULL,data_len不为零的情况。
    if (((!data) && (data_len > 0)) || ((!oob) && (oob_len > 0)))
        return (YAFFS_FAIL);
#endif

    Lock_MutexPend((struct MutexLCB *)(Chip->Lock), CN_TIMEOUT_FOREVER);

    if (dev->param.use_nand_ecc)
    {
    	//  硬件ECC优先使用
    	if(NandInfo->Controller & HW_ECC_SUPPORTED)
    		Flags |= HW_ECC;
    	else if(NandInfo->Controller & SW_ECC_SUPPORTED)
    		Flags |= SW_ECC;
    	else
    		Flags |= NO_ECC;
    }
    else
    	Flags |= NO_ECC;

    if (data)
        memcpy(Buf, data, data_len);

    if (oob)
    {
        Flags |= SPARE_REQ;
        memcpy((Buf + NandInfo->BytesPerPage), oob, oob_len);
    }

    Ret = NandOps->WrPage(nand_chunk, Buf, Flags);

    Lock_MutexPost((struct MutexLCB *)(Chip->Lock));

    if (Ret < 0)
    {
        //出错。
        return (YAFFS_FAIL);
    }

    return (YAFFS_OK);

}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static int yaffs_nand_drv_ReadChunk(struct yaffs_dev *dev, int nand_chunk,
                   u8 *data, int data_len,
                   u8 *oob, int oob_len,
                   enum yaffs_ecc_result *ecc_result_out)
{
    struct FlashChip *Chip = Dev2Chip(dev);
    struct NandDescr *NandInfo = &Chip->Descr.Nand;
    struct FlashOps *NandOps = &Chip->Ops;
    u8 *Buf = ChipBuf(dev);
    u32 Flags = 0;
    s32 Ret;
    *ecc_result_out = YAFFS_ECC_RESULT_NO_ERROR;

#if 0// todo: 存在data==NULL,data_len不为零的情况。
    if (((!data) && (data_len > 0)) || ((!oob) && (oob_len > 0)))
        return (YAFFS_FAIL);
#endif

    Lock_MutexPend((struct MutexLCB *)(Chip->Lock), CN_TIMEOUT_FOREVER);

    if (dev->param.use_nand_ecc)
    {
		//  硬件ECC优先使用
		if(NandInfo->Controller & HW_ECC_SUPPORTED)
			Flags |= HW_ECC;
		else if(NandInfo->Controller & SW_ECC_SUPPORTED)
			Flags |= SW_ECC;
		else
			Flags |= NO_ECC;
	}
    else
    	Flags |= NO_ECC;

    if (oob)
        Flags |= SPARE_REQ;

    Ret = NandOps->RdPage(nand_chunk, Buf, Flags);
    if (Ret < 0)
    {
        // 出错。
        if ((-3 == Ret) && (!IsErasedChunk(Buf, NandInfo->BytesPerPage, oob_len)))
            *ecc_result_out = YAFFS_ECC_RESULT_UNFIXED;/* ECC校验失败(保证不是被擦除的块中的页) */
        else
            *ecc_result_out = YAFFS_ECC_RESULT_UNKNOWN;/* 读错误等,或者无错误（情景：ECC读一个刚被擦除的页） */
    }

    if (data)
        memcpy(data, Buf, data_len);

    if (oob)
        memcpy(oob, (Buf + NandInfo->BytesPerPage), oob_len);

    Lock_MutexPost((struct MutexLCB *)(Chip->Lock));

    if(Ret < 0)
    	return (YAFFS_FAIL);/* 就算发生错误，也将数据传递回去(启动时,新块的ECC读肯定是错的) */

    return (YAFFS_OK);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static int yaffs_nand_drv_EraseBlock(struct yaffs_dev *dev, int block_no)
{
    struct FlashChip *Chip = Dev2Chip(dev);
    struct FlashOps *NandOps = &Chip->Ops;

    if(0 == NandOps->ErsBlk(block_no))
        return YAFFS_OK;
    else
        return YAFFS_FAIL;
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static int yaffs_nand_drv_MarkBad(struct yaffs_dev *dev, int block_no)
{
    struct FlashChip *Chip = Dev2Chip(dev);

    if(0 == Chip->Ops.Special.Nand.MrkBad(block_no))
        return YAFFS_OK;
    else
        return YAFFS_FAIL;

}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static int yaffs_nand_drv_CheckBad(struct yaffs_dev *dev, int block_no)
{
    struct FlashChip *Chip = Dev2Chip(dev);

    if(0 == Chip->Ops.Special.Nand.ChkBlk(block_no))
        return YAFFS_OK;
    else
        return YAFFS_FAIL;

}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注: 建立驱动层互斥锁
//-----------------------------------------------------------------------------
static int yaffs_nand_drv_Initialise(struct yaffs_dev *dev)
{
    struct FlashChip *Chip = Dev2Chip(dev);

    if(NULL == Chip->Lock)
        Chip->Lock = (void*)Lock_MutexCreate("nand driver lock of yaffs");

    return YAFFS_OK;
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注: 卸载驱动层互斥锁
//-----------------------------------------------------------------------------
static int yaffs_nand_drv_Deinitialise(struct yaffs_dev *dev)
{
    struct FlashChip *Chip = Dev2Chip(dev);

    if(Chip->Lock)
        Lock_MutexDelete((struct MutexLCB *)(Chip->Lock));
    return YAFFS_OK;
}

//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
int yaffs_nand_install_drv(struct yaffs_dev *dev, struct FlashChip *chip)
{
    struct yaffs_driver *drv = &dev->drv;

    if (!dev || !chip)
         return YAFFS_FAIL;

    drv->drv_write_chunk_fn = yaffs_nand_drv_WriteChunk;
    drv->drv_read_chunk_fn = yaffs_nand_drv_ReadChunk;
    drv->drv_erase_fn = yaffs_nand_drv_EraseBlock;
    drv->drv_mark_bad_fn = yaffs_nand_drv_MarkBad;
    drv->drv_check_bad_fn = yaffs_nand_drv_CheckBad;
    drv->drv_initialise_fn = yaffs_nand_drv_Initialise;
    drv->drv_deinitialise_fn = yaffs_nand_drv_Deinitialise;

    dev->driver_context = (void *) chip;
    return YAFFS_OK;
}

