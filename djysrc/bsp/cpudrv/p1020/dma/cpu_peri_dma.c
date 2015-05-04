//----------------------------------------------------
//Copyright (C), 2013-2020,  luoshitian.
//版权所有 (C), 2013-2020,   罗侍田.
//所属模块: dma驱动模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: dma驱动文件，使用dma直接搬运
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-11-14
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include <cpu_peri_dma.h>
#include <asm/io.h>
#include <stdio.h>
/* Controller can only transfer 2^26 - 1 bytes at a time */
#define FSL_DMA_MAX_SIZE    (0x3ffffff)


#define FSL_DMA_MR_DEFAULT (FSL_DMA_MR_BWC_DIS | FSL_DMA_MR_CTM_DIRECT)

ccsr_dma_t *dma_base = (void *)(CONFIG_SYS_MPC85xx_DMA_ADDR);


#define max(x, y)    (((x) < (y)) ? (y) : (x))
#define min(x, y)    (((x) < (y)) ? (x) : (y))

static void dma_sync(void)
{
    __asm__("sync; isync; msync");
}

static void out_dma32(volatile unsigned *addr, int val)
{
    out_be32(addr, val);
}

static unsigned int in_dma32(volatile unsigned *addr)
{
    return in_be32(addr);
}
//----检查dma传输状态-----------------------------------------------------------------
//功能：检查dma传输状态，检查DMA的通道忙寄存器，等到其变为0，则表示传输完成，然后把start DMA cs清零。
//参数：无
//返回：返回dma sr状态寄存器值
//-----------------------------------------------------------------------------
static unsigned int dma_check(void) {
    volatile fsl_dma_t *dma = &dma_base->dma[3];
    unsigned int status;

    /* While the channel is busy, spin */
    do {
        status = in_dma32(&dma->sr);
    } while (status & FSL_DMA_SR_CB);

    /* clear MR[CS] channel start bit */
    out_dma32(&dma->mr, in_dma32(&dma->mr) & ~FSL_DMA_MR_CS);
    dma_sync();

    if (status != 0)
        printf ("DMA Error: status = %x\r\n", status);

    return status;
}
//----dma初始化-----------------------------------------------------------------
//功能：初始化dma寄存器，清除error状态
//参数：无
//返回：返回dma sr状态寄存器值
//-----------------------------------------------------------------------------
void dma_init(void) {
    volatile fsl_dma_t *dma = &dma_base->dma[3];

    out_dma32(&dma->satr, FSL_DMA_SATR_SREAD_SNOOP);
    out_dma32(&dma->datr, FSL_DMA_DATR_DWRITE_SNOOP);
    out_dma32(&dma->sr, 0xffffffff); /* clear any errors */
    dma_sync();
}
//----dma搬运函数-----------------------------------------------------------------
//功能：使用dma搬运数据，检测dma数据是否搬运完成。
//参数：phys_addr_t dest, 搬运的目标地址，phys_addr_t src, 搬运的源地址，phys_size_t count，搬运的数据长度
//返回：0表示搬运成功，-1表示搬运失败
//-----------------------------------------------------------------------------
int dmacpy(unsigned long dest, unsigned long src, unsigned long count) {
    volatile fsl_dma_t *dma = &dma_base->dma[3];
    unsigned int xfer_size;
    //printf("dest = %8x,src = %8x\r\n",dest,src);
    while (count) {
        xfer_size = min(FSL_DMA_MAX_SIZE, count);

        out_dma32(&dma->dar, (u32) (dest & 0xFFFFFFFF));
        out_dma32(&dma->sar, (u32) (src & 0xFFFFFFFF));

        out_dma32(&dma->satr,
            in_dma32(&dma->satr) | (u32)((u64)src >> 32));
        out_dma32(&dma->datr,
            in_dma32(&dma->datr) | (u32)((u64)dest >> 32));

        out_dma32(&dma->bcr, xfer_size);
        dma_sync();

        /* Prepare mode register */
        out_dma32(&dma->mr, FSL_DMA_MR_DEFAULT);
        dma_sync();

        /* Start the transfer */
        out_dma32(&dma->mr, FSL_DMA_MR_DEFAULT | FSL_DMA_MR_CS);

        count -= xfer_size;
        src += xfer_size;
        dest += xfer_size;

        dma_sync();

        if (dma_check())
            return -1;
    }

    return 0;
}
//----dma搬运函数（不带检测）-----------------------------------------------------------------
//功能：使用dma搬运数据，不检测dma数据是否搬运完成。
//参数：phys_addr_t dest, 搬运的目标地址，phys_addr_t src, 搬运的源地址，phys_size_t count，搬运的数据长度
//返回：0表示搬运成功，-1表示搬运失败
//-----------------------------------------------------------------------------
int dmacpy_nocheck(unsigned long dest, unsigned long src, unsigned long count) {
    volatile fsl_dma_t *dma = &dma_base->dma[3];
    unsigned int xfer_size;
    //printf("dest = %8x,src = %8x\r\n",dest,src);
    while (count) {
        xfer_size = min(FSL_DMA_MAX_SIZE, count);

        out_dma32(&dma->dar, (u32) (dest & 0xFFFFFFFF));
        out_dma32(&dma->sar, (u32) (src & 0xFFFFFFFF));

        out_dma32(&dma->satr,
            in_dma32(&dma->satr) | (u32)((u64)src >> 32));
        out_dma32(&dma->datr,
            in_dma32(&dma->datr) | (u32)((u64)dest >> 32));

        out_dma32(&dma->bcr, xfer_size);
        dma_sync();

        /* Prepare mode register */
        out_dma32(&dma->mr, FSL_DMA_MR_DEFAULT);
        dma_sync();

        /* Start the transfer */
        out_dma32(&dma->mr, FSL_DMA_MR_DEFAULT | FSL_DMA_MR_CS);

        count -= xfer_size;
        src += xfer_size;
        dest += xfer_size;

        dma_sync();

        //if (dma_check())
            //return -1;
    }

    return 0;
}
/*
 * 85xx/86xx use dma to initialize SDRAM when !CONFIG_ECC_INIT_VIA_DDRCONTROLLER
 * while 83xx uses dma to initialize SDRAM when CONFIG_DDR_ECC_INIT_VIA_DMA
 */
#if 0
#if ((!defined CONFIG_MPC83xx && defined(CONFIG_DDR_ECC) &&    \
    !defined(CONFIG_ECC_INIT_VIA_DDRCONTROLLER)) ||        \
    (defined(CONFIG_MPC83xx) && defined(CONFIG_DDR_ECC_INIT_VIA_DMA)))
void dma_meminit(uint val, uint size)
{
    uint *p = 0;
    uint i = 0;

    for (*p = 0; p < (uint *)(8 * 1024); p++) {
        if (((uint)p & 0x1f) == 0)
            ppcDcbz((ulong)p);

        *p = (uint)CONFIG_MEM_INIT_VALUE;

        if (((uint)p & 0x1c) == 0x1c)
            ppcDcbf((ulong)p);
    }

    dmacpy(0x002000, 0, 0x002000); /* 8K */
    dmacpy(0x004000, 0, 0x004000); /* 16K */
    dmacpy(0x008000, 0, 0x008000); /* 32K */
    dmacpy(0x010000, 0, 0x010000); /* 64K */
    dmacpy(0x020000, 0, 0x020000); /* 128K */
    dmacpy(0x040000, 0, 0x040000); /* 256K */
    dmacpy(0x080000, 0, 0x080000); /* 512K */
    dmacpy(0x100000, 0, 0x100000); /* 1M */
    dmacpy(0x200000, 0, 0x200000); /* 2M */
    dmacpy(0x400000, 0, 0x400000); /* 4M */

    for (i = 1; i < size / 0x800000; i++)
        dmacpy((0x800000 * i), 0, 0x800000);
}
#endif
#endif
