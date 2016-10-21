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

#include "yaffs_guts.h"
#include "yaffs_trace.h"
#include "yaffs_packedtags2.h"
/******************************************************************************
*  yaffs2专用.
******************************************************************************/
static int yaffs_tags_marshall_write(struct yaffs_dev *dev,
                    int nand_chunk, const u8 *data,
                    const struct yaffs_ext_tags *tags)
{
    struct yaffs_packed_tags2 pt;
    int retval;

    int packed_tags_size =
        dev->param.no_tags_ecc ? sizeof(pt.t) : sizeof(pt);
    void *packed_tags_ptr =
        dev->param.no_tags_ecc ? (void *)&pt.t : (void *)&pt;

    yaffs_trace(YAFFS_TRACE_MTD,
        "yaffs_tags_marshall_write chunk %d data %p tags %p",
        nand_chunk, data, tags);

    /* For yaffs2 writing there must be both data and tags.
     * If we're using inband tags, then the tags are stuffed into
     * the end of the data buffer.
     */
    if (!data || !tags)
        BUG();/* 出错,data或tags有一个没有数据 */
    else if (dev->param.inband_tags) {/* tags放于页内 */
        struct yaffs_packed_tags2_tags_only *pt2tp;
        pt2tp =
            (struct yaffs_packed_tags2_tags_only *)(data +
                            dev->
                            data_bytes_per_chunk);
        yaffs_pack_tags2_tags_only(pt2tp, tags);
    } else {/* tags放于OOB区 */
        yaffs_pack_tags2(&pt, tags, !dev->param.no_tags_ecc);/*  */
    }

    retval = dev->drv.drv_write_chunk_fn(dev, nand_chunk,
            data, dev->param.total_bytes_per_chunk,
            (dev->param.inband_tags) ? NULL : packed_tags_ptr,
            (dev->param.inband_tags) ? 0 : packed_tags_size);

    return retval;
}
/******************************************************************************
*  yaffs2专用.
* 参数: nand_chunk --
*      data -- 数据空间,可以为NULL。
*      tags --
******************************************************************************/
static int yaffs_tags_marshall_read(struct yaffs_dev *dev,
                   int nand_chunk, u8 *data,
                   struct yaffs_ext_tags *tags)
{
    int retval = 0;
    int local_data = 0;
    u8 spare_buffer[100];
    enum yaffs_ecc_result ecc_result;

    struct yaffs_packed_tags2 pt;

    int packed_tags_size =
        dev->param.no_tags_ecc ? sizeof(pt.t) : sizeof(pt);
    void *packed_tags_ptr =
        dev->param.no_tags_ecc ? (void *)&pt.t : (void *)&pt;

    yaffs_trace(YAFFS_TRACE_MTD,
        "yaffs_tags_marshall_read chunk %d data %p tags %p",
        nand_chunk, data, tags);

    if (dev->param.inband_tags) {
        if (!data) {
            local_data = 1;
            data = yaffs_get_temp_buffer(dev);
        }
    }

    if (dev->param.inband_tags || (data && !tags))/* tags在页内或者只读页数据(不读OOB区) */
        retval = dev->drv.drv_read_chunk_fn(dev, nand_chunk,
                    data, dev->param.total_bytes_per_chunk,
                    NULL, 0,
                    &ecc_result);
    else if (tags)/* Tags在OOB区 */
        retval = dev->drv.drv_read_chunk_fn(dev, nand_chunk,
                    data, dev->param.total_bytes_per_chunk,
                    spare_buffer, packed_tags_size,
                    &ecc_result);/* todo:读失败的怎么办？这是只读spare_buffer */
    else
        BUG();


    if (dev->param.inband_tags) {/* tags在页内 */
        if (tags) {
            struct yaffs_packed_tags2_tags_only *pt2tp;
            pt2tp =/* 指向tags位置 */
                (struct yaffs_packed_tags2_tags_only *)
                &data[dev->data_bytes_per_chunk];
            yaffs_unpack_tags2_tags_only(tags, pt2tp);/* 解析tags,不带ECC */
        }
    } else if (tags) {/* tags在OOB区 */
        memcpy(packed_tags_ptr, spare_buffer, packed_tags_size);
        yaffs_unpack_tags2(tags, &pt, !dev->param.no_tags_ecc);/* 解析tags,带ECC */
    }

    if (local_data)
        yaffs_release_temp_buffer(dev, data);

    if (tags && ecc_result == YAFFS_ECC_RESULT_UNFIXED) {
        tags->ecc_result = YAFFS_ECC_RESULT_UNFIXED;
        dev->n_ecc_unfixed++;
    }

    if (tags && ecc_result == -YAFFS_ECC_RESULT_FIXED) {
        if (tags->ecc_result <= YAFFS_ECC_RESULT_NO_ERROR)/* tags2的校验结果错误 */
            tags->ecc_result = YAFFS_ECC_RESULT_FIXED;
        dev->n_ecc_fixed++;
    }

    if (ecc_result < YAFFS_ECC_RESULT_UNFIXED)
        return YAFFS_OK;
    else
        return YAFFS_FAIL;
}
/******************************************************************************
*    通过访问块的首页来判别块目前的状态。
* 参数: block_no -- 块号（设备范围）
*       state --
*       seq_number --
******************************************************************************/
static int yaffs_tags_marshall_query_block(struct yaffs_dev *dev, int block_no,
                   enum yaffs_block_state *state,
                   u32 *seq_number)
{
    int retval;

    yaffs_trace(YAFFS_TRACE_MTD, "yaffs_tags_marshall_query_block %d",
            block_no);

    retval = dev->drv.drv_check_bad_fn(dev, block_no);/* 块状态检查 */

    if (retval== YAFFS_FAIL) {/* 坏块 */
        yaffs_trace(YAFFS_TRACE_MTD, "block is bad");

        *state = YAFFS_BLOCK_STATE_DEAD;
        *seq_number = 0;
    } else {/* 好块 */
        struct yaffs_ext_tags t;

        yaffs_tags_marshall_read(dev,
                    block_no * dev->param.chunks_per_block,
                    NULL, &t);/* 只读tags,todo:出错怎么办？ */

        if (t.chunk_used) {/* 页已被使用 */
            *seq_number = t.seq_number;
            *state = YAFFS_BLOCK_STATE_NEEDS_SCAN;
        } else {/* 空闲页 */
            *seq_number = 0;
            *state = YAFFS_BLOCK_STATE_EMPTY;
        }
    }

    yaffs_trace(YAFFS_TRACE_MTD,
        "block query returns  seq %d state %d",
        *seq_number, *state);

    if (retval == 0)
        return YAFFS_OK;
    else
        return YAFFS_FAIL;
}

/******************************************************************************
*
******************************************************************************/
static int yaffs_tags_marshall_mark_bad(struct yaffs_dev *dev, int block_no)
{
    return dev->drv.drv_mark_bad_fn(dev, block_no);

}

/******************************************************************************
*
******************************************************************************/
void yaffs_tags_marshall_install(struct yaffs_dev *dev)
{
    if (!dev->param.is_yaffs2)/* 用于yaffs2 */
        return;

    if (!dev->tagger.write_chunk_tags_fn)
        dev->tagger.write_chunk_tags_fn = yaffs_tags_marshall_write;

    if (!dev->tagger.read_chunk_tags_fn)
        dev->tagger.read_chunk_tags_fn = yaffs_tags_marshall_read;

    if (!dev->tagger.query_block_fn)
        dev->tagger.query_block_fn = yaffs_tags_marshall_query_block;

    if (!dev->tagger.mark_bad_fn)
        dev->tagger.mark_bad_fn = yaffs_tags_marshall_mark_bad;

}
