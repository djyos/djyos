/*========================================================*
 * 文件模块说明:                                           *
 * omapl138的spi口驱动文件，提供对spi口初始化和输入输出驱动      *
 * 文件版本: v1.00                                         *
 * 开发人员: hm,liq                                        *
 * 创建时间: 2014.04.26                                    *
 * Copyright(c) 2011-2011  深圳鹏瑞软件有限公司                                  *
 *========================================================*
 * 程序修改记录(最新的放在最前面):                        *
 *  <版本号> <修改日期>, <修改人员>: <修改功能概述>       *
 *========================================================*
* 例如：
*  V1.02 2004-07-14, 张三: 接收函数增加对报文长度有效性校核
*
*/
#ifndef __CPU_PERI_SPI_H__
#define __CPU_PERI_SPI_H__
#include "stdint.h"


#ifdef __cplusplus
extern "C" {
#endif

//spi0 base address 0x01C41000
typedef struct
{
    u32 mode;       /* eSPI mode */
    u32 event;      /* eSPI event */
    u32 mask;       /* eSPI mask */
    u32 com;        /* eSPI command */
    u32 tx;     /* eSPI transmit FIFO access */
    u32 rx;     /* eSPI receive FIFO access */
    u8  res1[8];    /* reserved */
    u32 csmode[4];  /* 0x2c: sSPI CS0/1/2/3 mode */
    u8  res2[4048]; /* fill up to 0x1000 */
}tagSpiReg;


/* Controller-specific definitions: */

/* SPI mode flags */
#define SPI_CS_HIGH 0x04            /* CS active high */
#define SPI_LSB_FIRST   0x08            /* per-word bits-on-wire */
#define SPI_3WIRE   0x10            /* SI/SO signals shared */
#define SPI_LOOP    0x20            /* loopback mode */


/* SPI transfer flags */
#define SPI_XFER_BEGIN  0x01            /* Assert CS before transfer */
#define SPI_XFER_END    0x02            /* Deassert CS after transfer */


#ifdef __cplusplus
}
#endif

#endif /*__CPU_PERI_SPI_H__*/

