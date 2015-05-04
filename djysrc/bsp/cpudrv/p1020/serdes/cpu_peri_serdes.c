//----------------------------------------------------
//Copyright (C), 2013-2020,  luoshitian.
//版权所有 (C), 2013-2020,   罗侍田.
//所属模块: serdes驱动模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: serdes串口驱动文件
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-11-14
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include <cpu_peri.h>
#include <asm/io.h>
#include <asm/bitops.h>
#include "stdio.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
typedef struct serdes_85xx
{
    u32    srdscr0;    /* 0x00 - SRDS Control Register 0 */
    u32    srdscr1;    /* 0x04 - SRDS Control Register 1 */
    u32    srdscr2;    /* 0x08 - SRDS Control Register 2 */
    u32    srdscr3;    /* 0x0C - SRDS Control Register 3 */
    u32    srdscr4;    /* 0x10 - SRDS Control Register 4 */
} serdes_85xx_t;
#define FSL_SRDSCR3_EIC0(x)    (((x) & 0x1f) << 8)
#define FSL_SRDSCR3_EIC0_MASK    FSL_SRDSCR3_EIC0(0x1f)
#define FSL_SRDSCR3_EIC1(x)    (((x) & 0x1f) << 0)
#define FSL_SRDSCR3_EIC1_MASK    FSL_SRDSCR3_EIC1(0x1f)
#define FSL_SRDSCR4_EIC2(x)    (((x) & 0x1f) << 8)
#define FSL_SRDSCR4_EIC2_MASK    FSL_SRDSCR4_EIC2(0x1f)
#define FSL_SRDSCR4_EIC3(x)    (((x) & 0x1f) << 0)
#define FSL_SRDSCR4_EIC3_MASK    FSL_SRDSCR4_EIC3(0x1f)
#define EIC_PCIE    0x13
#define EIC_SGMII    0x04

#define SRDS1_MAX_LANES        4

static u32 serdes1_prtcl_map;

static u8 serdes1_cfg_tbl[][SRDS1_MAX_LANES] =
{
    [0x0] = {PCIE1, NONE, NONE, NONE},
    [0x6] = {PCIE1, PCIE1, PCIE1, PCIE1},
    [0xe] = {PCIE1, PCIE2, SGMII_TSEC2, SGMII_TSEC3},
    [0xf] = {PCIE1, PCIE1, SGMII_TSEC2, SGMII_TSEC3},
};
//---接口是否配置serdes-----------------------------------------------------------------
//功能：判断该接口是否配置serdes
//参数：enum srds_prtcl prtcl，PCIE、SATA等接口是否配置serdes
//返回：不为0表示该接口有配置serdes。
//-----------------------------------------------------------------------------
s32 is_serdes_configured(enum srds_prtcl prtcl)
{
    return (1 << prtcl) & serdes1_prtcl_map;
}
//---serdes初始化-----------------------------------------------------------------
//功能：初始化serdes
//参数：无
//返回：无
//-----------------------------------------------------------------------------
void fsl_serdes_init(void)
{
    ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);
    serdes_85xx_t *serdes = (void *)CONFIG_SYS_MPC85xx_SERDES1_ADDR;

    u32 pordevsr = in_be32(&gur->pordevsr);
    u32 srds_cfg = (pordevsr & MPC85xx_PORDEVSR_IO_SEL) >>
                   MPC85xx_PORDEVSR_IO_SEL_SHIFT;
    s32 lane;
    u32 mask, val;

    printf("PORDEVSR[IO_SEL_SRDS] = %x\r\n", srds_cfg);

    if (srds_cfg > ARRAY_SIZE(serdes1_cfg_tbl))
    {
        printf("Invalid PORDEVSR[IO_SEL_SRDS] = %d\r\n", srds_cfg);
        return;
    }

    for (lane = 0; lane < SRDS1_MAX_LANES; lane++)
    {
        enum srds_prtcl lane_prtcl = serdes1_cfg_tbl[srds_cfg][lane];
        serdes1_prtcl_map |= (1 << lane_prtcl);
    }

    /* Init SERDES Receiver electrical idle detection control for PCIe */
#if 1
    /* Lane 0 is always PCIe 1 */
    mask = FSL_SRDSCR3_EIC0_MASK;
    val = FSL_SRDSCR3_EIC0(EIC_PCIE);

    /* Lane 1 */
    if ((serdes1_cfg_tbl[srds_cfg][1] == PCIE1) ||
            (serdes1_cfg_tbl[srds_cfg][1] == PCIE2))
    {
        mask |= FSL_SRDSCR3_EIC1_MASK;
        val |= FSL_SRDSCR3_EIC1(EIC_PCIE);
    }

    /* Handle lanes 0 & 1 */
    clrsetbits_be32(&serdes->srdscr3, mask, val);

    /* Handle lanes 2 & 3 */
    if (srds_cfg == 0x6)
    {
        mask = FSL_SRDSCR4_EIC2_MASK | FSL_SRDSCR4_EIC3_MASK;
        val = FSL_SRDSCR4_EIC2(EIC_PCIE) | FSL_SRDSCR4_EIC3(EIC_PCIE);
        clrsetbits_be32(&serdes->srdscr4, mask, val);
    }
#endif
    /* 100 ms delay */
    //djy_delay_10us(10000);
    //Djy_EventDelay(100000);
}
