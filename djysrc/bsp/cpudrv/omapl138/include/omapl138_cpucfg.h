
//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
// Copyright (c) 2014 著作权由深圳鹏瑞软件有限公司所有。著作权人保留一切权利。
//
// 这份授权条款，在使用者符合下列条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------
/*========================================================       *
 * 文件模块说明:                                                 *
 * omapl138的cpu公共模块配置文件                                 *
 * 文件版本: v1.00                                               *
 * 开发人员: lst                                                 *
 * 创建时间: 2011.06.01                                          *
 * Copyright(c) 2011-2011  深圳鹏瑞软件有限公司                  *
 *========================================================       *
 * 程序修改记录(最新的放在最前面):							     *
 *  <版本号> <修改日期>, <修改人员>: <修改功能概述>				 *
 *========================================================       *
* 例如：
*  V1.02 2004-07-14, 张三: 接收函数增加对报文长度有效性校核
*
*/

#ifndef __CPUCFG_H__
#define __CPUCFG_H__

#ifdef __cplusplus
extern "C" {
#endif

//at 0x01c14000
typedef struct 
{
   vu32 REVID;            // 0x0000
   vu32 RSVD0;            // 0x0004
   vu32 DIEIDR[4];        // 0x0008
   vu32 DEVIDR0;
   vu32 RSVD1[1];         // 0x001c
   vu32 BOOTCFG;          // 0x0020
   vu32 RSVD2[5];         // 0x0024
   vu32 KICKR[2];         // 0x0038
   vu32 HOST0CFG;         // 0x0040
   vu32 HOST1CFG;         // 0x0044
   vu32 RSVD3[38];        // 0x0048
   vu32 IRAWSTAT;         // 0x00E0
   vu32 IENSTAT;          // 0x00E4
   vu32 IENSET;           // 0x00E8
   vu32 IENCLR;           // 0x00EC
   vu32 EOI;              // 0x00F0
   vu32 FLTADDRR;         // 0x00F4
   vu32 FLTSTAT;          // 0x00F8
   vu32 RSVD4[5];         // 0x00FC
   vu32 MSTPRI[3];        // 0x0110
   vu32 RSVD5;            // 0x011C
   vu32 PINMUX[20];       // 0x0120
   vu32 SUSPSRC;          // 0x0170
   vu32 CHIPSIG;          // 0x0174
   vu32 CHIPSIG_CLR;      // 0x0178
   vu32 CFGCHIP[5];       // 0x017C
}tagSysconfig0Reg;

//at 0x01e2c000
typedef struct 
{
    vu32 VTPIO_CTL;
    vu32 DDR_SLEW;
    vu32 DEEPSLEEP;
    vu32 PUPD_ENA;
    vu32 PUPD_SEL;
    vu32 RXACTIVE;
    vu32 PWRDN;
}tagSysconfig1Reg;

typedef struct
{
   vu32 REVID;         // 0x0000
   vu32 RSVD0[56];     // 0x0004
   vu32 RSTYPE;        // 0x00E4
   vu32 RSVD1[6];      // 0x00E8
   vu32 PLLCTL;        // 0x0100
   vu32 OCSEL;         // 0x0104
   vu32 RSVD2[2];      // 0x0108
   vu32 PLLM;          // 0x0110
   vu32 PREDIV;        // 0x0114
   vu32 PLLDIV1;       // 0x0118
   vu32 PLLDIV2;       // 0x011C
   vu32 PLLDIV3;       // 0x0120
   vu32 OSCDIV;        // 0x0124
   vu32 POSTDIV;       // 0x0128
   vu32 RSVD3[3];      // 0x012C
   vu32 PLLCMD;        // 0x0138
   vu32 PLLSTAT;       // 0x013C
   vu32 ALNCTL;        // 0x0140
   vu32 DCHANGE;       // 0x0144
   vu32 CKEN;          // 0x0148
   vu32 CKSTAT;        // 0x014C
   vu32 SYSTAT;        // 0x0150
   vu32 RSVD4[3];      // 0x0154
   vu32 PLLDIV4;       // 0x0160
   vu32 PLLDIV5;       // 0x0164
   vu32 PLLDIV6;       // 0x0168
   vu32 PLLDIV7;       // 0x016C
} tagPllReg;

// bitmask defines for PLLCTL.
#define EXTCLKSRC       (0x00000200)   // bit 9
#define CLKMODE         (0x00000100)   // bit 8
#define PLLENSRC        (0x00000020)   // bit 5
#define PLLDIS          (0x00000010)   // bit 4
#define PLLRST          (0x00000008)   // bit 3
#define PLLPWRDN        (0x00000002)   // bit 1
#define PLLEN           (0x00000001)   // bit 0
#define CLKMODE_SHIFT   (8)

// bitmask defines for PLLCMD and PLLSTAT.
#define GOSET        (0x00000001)
#define GOSTAT       (0x00000001)

// defines for divisors.
#define DIV_ENABLE         (0x00008000)
#define MULTIPLIER         (24)
#define PLL_LOCK_CYCLES          (2400)
#define PLL_STABILIZATION_TIME   (2000)
#define PLL_RESET_TIME_CNT       (200)

// unlock/lock kick registers defines.
#define KICK0R_UNLOCK      (0x83E70B13)
#define KICK1R_UNLOCK      (0x95A4F1E0)
#define KICK0R_LOCK        (0x00000000)
#define KICK1R_LOCK        (0x00000000)

// bitmask defines for cfgchip[0].
#define PLL0_MASTER_LOCK   (0x00000010)   // bit 4

// bitmask defines for cfgchip[3].
#define CLK2XSRC           (0x00000080)   // bit 7
#define PLL1_MASTER_LOCK   (0x00000020)   // bit 5
#define DIV4P5ENA          (0x00000004)   // bit 2
#define EMA_CLKSRC         (0x00000002)   // bit 1

// defines for hostcfg.
#define BOOTRDY            (0x00000001)   // bit 0

// defines for pinmux0.
#define MASK_EMB_WE        (0xF0000000) // bits 31-28
#define MASK_EMB_RAS       (0x0F000000) // bits 27-24

typedef struct
{
  vu32 REVID;          // 0x0000
  vu32 RSVD0[5];       // 0x0004
  vu32 INTEVAL;        // 0x0018
  vu32 RSVD1[9];       // 0x001C
  vu32 MERRPR0;        // 0x0040
  vu32 RSVD2[3];       // 0x0044
  vu32 MERRCR0;        // 0x0050
  vu32 RSVD3[3];       // 0x0054
  vu32 PERRPR;         // 0x0060
  vu32 RSVD4;          // 0x0064
  vu32 PERRCR;         // 0x0068
  vu32 RSVD5[45];      // 0x006C
  vu32 PTCMD;          // 0x0120
  vu32 RSVD6;          // 0x0124
  vu32 PTSTAT;         // 0x0128
  vu32 RSVD7[53];      // 0x012C
  vu32 PDSTAT0;        // 0x0200
  vu32 PDSTAT1;        // 0x0204
  vu32 RSVD8[62];      // 0x0208
  vu32 PDCTL0;         // 0x0300
  vu32 PDCTL1;         // 0x0304
  vu32 RSVD9[62];      // 0x0308
  vu32 PDCFG0;         // 0x0400
  vu32 PDCFG1;         // 0x0404
  vu32 RSVD10[254];    // 0x0408
  vu32 MDSTAT[32];     // 0x0800
  vu32 RSVD11[96];     // 0x0880
  vu32 MDCTL[32];      // 0x0A00
} tagLpscReg;

//psc0上各电源域定义
#define cn_PSC0_EMDA3_CC0   0
#define cn_PSC0_EMDA3_TC0   1
#define cn_PSC0_EMDA3_TC1   2
#define cn_PSC0_EMIFA       3
#define cn_PSC0_SPI0        4
#define cn_PSC0_MMCSD0      5
#define cn_PSC0_AINTC       6
#define cn_PSC0_ARM_RAMROM  7
#define cn_PSC0_UART0       9
#define cn_PSC0_SCR0_SS     10
#define cn_PSC0_SCR1_SS     11
#define cn_PSC0_SCR2_SS     12
#define cn_PSC0_PRU         13
#define cn_PSC0_ARM         14
#define cn_PSC0_DSP         15

#define cn_psc1_base        32
//psc1上各电源域定义
#define cn_PSC1_EMDA3_CC1   (cn_psc1_base+0 )
#define cn_PSC1_USB0        (cn_psc1_base+1 )
#define cn_PSC1_USB1        (cn_psc1_base+2 )
#define cn_PSC1_GPIO        (cn_psc1_base+3 )
#define cn_PSC1_UHPI        (cn_psc1_base+4 )
#define cn_PSC1_EMAC        (cn_psc1_base+5 )
#define cn_PSC1_DDR2_MDDR   (cn_psc1_base+6 )
#define cn_PSC1_MCASP0      (cn_psc1_base+7 )
#define cn_PSC1_SATA        (cn_psc1_base+8 )
#define cn_PSC1_VPIF        (cn_psc1_base+9 )
#define cn_PSC1_SPI1        (cn_psc1_base+10)
#define cn_PSC1_I2C1        (cn_psc1_base+11)
#define cn_PSC1_UART1       (cn_psc1_base+12)
#define cn_PSC1_UART2       (cn_psc1_base+13)
#define cn_PSC1_MCBSP0      (cn_psc1_base+14)
#define cn_PSC1_MCBSP1      (cn_psc1_base+15)
#define cn_PSC1_LCDC        (cn_psc1_base+16)
#define cn_PSC1_EHRPWM      (cn_psc1_base+17)
#define cn_PSC1_MMCSD1      (cn_psc1_base+18)
#define cn_PSC1_UPP         (cn_psc1_base+19)
#define cn_PSC1_ECAP0_1_2   (cn_psc1_base+20)
#define cn_PSC1_EMDA3_TC2   (cn_psc1_base+21)
#define cn_PSC1_SCRF0_SS    (cn_psc1_base+24)
#define cn_PSC1_SCRF1_SS    (cn_psc1_base+25)
#define cn_PSC1_SCRF2_SS    (cn_psc1_base+26)
#define cn_PSC1_SCRF6_SS    (cn_psc1_base+27)
#define cn_PSC1_SCRF7_SS    (cn_psc1_base+28)
#define cn_PSC1_SCRF8_SS    (cn_psc1_base+29)
#define cn_PSC1_BR_F7       (cn_psc1_base+30)
#define cn_PSC1_SHRAM       (cn_psc1_base+31)

// psc module status register defines.
#define MASK_STATE               (0x0000003F)

// psc module control register defines.
#define FORCE                    (0x80000000)   // bit 31.
#define EMUIHBIE                 (0x00000400)   // bit 10.
#define EMURSTIE                 (0x00000200)   // bit 9.
#define LRST                     (0x00000100)   // bit 8.
#define NEXT                     (0x00000007)   // bits 0-2.

// psc module states.
#define PSC_ENABLE               (0x00000003)
#define PSC_DISABLE              (0x00000002)
#define PSC_SYNCRESET            (0x00000001)
#define PSC_SWRSTDISABLE         (0x00000000)

// domain defines.
#define cn_Always_ON             (0x00000001)
#define cn_RAM_Pseudo            (0x00000002)

typedef struct
{
    vu32 REVID;
    vu32 SDRSTAT;
    vu32 SDCR;
    vu32 SDRCR;
    vu32 SDTIMR1;
    vu32 SDTIMR2;
    vu32 RSVD0;
    vu32 SDCR2;
    vu32 PBBPR;
    vu32 RSVD1[7];
    vu32 PC1;
    vu32 PC2;
    vu32 PCC;
    vu32 PCMRS;
    vu32 PCT;
    vu32 RSVD2[27];
    vu32 IRR;
    vu32 IMR;
    vu32 IMSR;
    vu32 IMCR;
    vu32 RSVD3[5];
    vu32 DRPYC1R;
} tagDdriiReg;


typedef struct
{
   vu32 REVID;               // 0x0000
   vu32 AWCC;                // 0x0004
   vu32 SDCR;                // 0x0008
   vu32 SDRCR;               // 0x000C
   vu32 CE2CFG;              // 0x0010
   vu32 CE3CFG;              // 0x0014
   vu32 CE4CFG;              // 0x0018
   vu32 CE5CFG;              // 0x001C
   vu32 SDTIMR;              // 0x0020
   vu32 RSVD0[6];            // 0x0024
   vu32 SDSRETR;             // 0x003C
   vu32 INTRAW;              // 0x0040
   vu32 INTMASK;             // 0x0044
   vu32 INTMASKSET;          // 0x0048
   vu32 INTMASKCLR;          // 0x004C
   vu32 RSVD1[4];            // 0x0050
   vu32 NANDFCR;             // 0x0060
   vu32 NANDFSR;             // 0x0064
   vu32 PMCR;                // 0x0068
   vu32 NANDF1ECC;           // 0x0070
   vu32 NANDF2ECC;           // 0x0074
   vu32 NANDF3ECC;           // 0x0078
   vu32 NANDF4ECC;           // 0x007C
   vu32 RSVD2[15];           // 0x0080
   vu32 NAND4BITECCLOAD;     // 0x00BC
   vu32 NAND4BITECC1;        // 0x00C0
   vu32 NAND4BITECC2;        // 0x00C4
   vu32 NAND4BITECC3;        // 0x00C8
   vu32 NAND4BITECC4;        // 0x00CC
   vu32 NANDERRADDR1;        // 0x00D0
   vu32 NANDERRADDR2;        // 0x00D4
   vu32 NANDERRVAL1;         // 0x00D8
   vu32 NANDERRVAL2;         // 0x00DC
} tagEmifaReg;

#define CN_ASIZE_MASK           (0x00000003)
#define CN_ASIZE_OFFSET         (0)
#define CN_TA_MASK              (0x0000000C)
#define CN_TA_OFFSET            (2)
#define CN_R_HOLD_MASK          (0x00000070)
#define CN_R_HOLD_OFFSET        (4)
#define CN_R_STROBE_MASK        (0x00001F80)
#define CN_R_STROBE_OFFSET      (7)
#define CN_R_SETUP_MASK         (0x0001E000)
#define CN_R_SETUP_OFFSET       (13)
#define CN_W_HOLD_MASK          (0x000E0000)
#define CN_W_HOLD_OFFSET        (17)
#define CN_W_STROBE_MASK        (0x03F00000)
#define CN_W_STROBE_OFFSET      (20)
#define CN_W_SETUP_MASK         (0x3C000000)
#define CN_W_SETUP_OFFSET       (26)
#define CN_EW_MASK       	    (0x40000000)
#define CN_EW_OFFSET            (30)
#define CN_SS_MASK       	    (0x80000000)
#define CN_SS_OFFSET            (31)
#define CN_MAX_EXT_WAIT_MASK    (0x000000FF)
#define CN_MAX_EXT_WAIT_OFFSET  (0)

enum ModulePower
{
    PlscPowerOn = 0,
    PlscPowerOff
};
void Cpucfg_EnableLPSC(u8 module);
void Cpucfg_DisableLPSC(u8 module);
void config_pll1(u32 pllm, u32 postdiv, u32 plldiv1, u32 plldiv2, u32 plldiv3);
void config_pll0(u32 clkmode, u32 prediv, u32 pllm, u32 postdiv, 
                u32 plldiv1, u32 plldiv3, u32 plldiv7);
void Cpucfg_EmifaInit(void);

#ifdef __cplusplus
}
#endif

#endif /*__CPUCFG_H__*/


