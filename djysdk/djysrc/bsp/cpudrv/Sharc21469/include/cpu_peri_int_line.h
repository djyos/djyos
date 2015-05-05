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
//所属模块:中断模块
//作者：lst
//版本：V1.0.0
//文件描述: 定义中断号
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009--9-28
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __CPU_PERI_INT_LINE_H__
#define __CPU_PERI_INT_LINE_H__

#define cn_int_line_base      (0x00)
#define cn_int_line_IICDI     (cn_int_line_base+0x00)      /* Exception: Illegal Input Condition Detected */
#define cn_int_line_SOVFI     (cn_int_line_base+0x01)      /* Exception: Stack overflow */
#define cn_int_line_TMZHI     (cn_int_line_base+0x02)      /* Timer = 0 (high priority) */
#define cn_int_line_SPERRI    (cn_int_line_base+0x03)      /* Exception: SPORT Error interrupt */
#define cn_int_line_BKPI      (cn_int_line_base+0x04)      /* Exception: Hardware Breakpoint interrupt */
#define cn_int_line_IRQ2I     (cn_int_line_base+0x05)      /* IRQ2- asserted */
#define cn_int_line_IRQ1I     (cn_int_line_base+0x06)      /* IRQ1- asserted */
#define cn_int_line_IRQ0I     (cn_int_line_base+0x07)      /* IRQ0- asserted */
#define cn_int_line_DAII      (cn_int_line_base+0x08)      /* Programmable Interrupt 0  (DAII) */
#define cn_int_line_SPII     (cn_int_line_base+0x09)      /* Programmable Interrupt 1  (default: SPI Interrupt) */
#define cn_int_line_GPTMR0I   (cn_int_line_base+0x0a)      /* Programmable Interrupt 2  (default: General purpose IOP Timer0 Interrupt) */
#define cn_int_line_SP1I      (cn_int_line_base+0x0b)      /* Programmable Interrupt 3  (default: SPORT 1 Interrupt) */
#define cn_int_line_SP3I      (cn_int_line_base+0x0c)      /* Programmable Interrupt 4  (default: SPORT 3 Interrupt) */
#define cn_int_line_SP5I      (cn_int_line_base+0x0d)      /* Programmable Interrupt 5  (default: SPORT 5 Interrupt) */
#define cn_int_line_SP0I      (cn_int_line_base+0x0e)      /* Programmable Interrupt 6  (default: SPORT 0 Interrupt) */
#define cn_int_line_SP2I      (cn_int_line_base+0x0f)      /* Programmable Interrupt 7  (default: SPORT 2 Interrupt) */
#define cn_int_line_SP4I      (cn_int_line_base+0x10)      /* Programmable Interrupt 8  (default: SPORT 4 Interrupt) */
#define cn_int_line_EPDM0I    (cn_int_line_base+0x11)      /* Programmable Interrupt 9  (default: External port DMA0 Interrupt) */
#define cn_int_line_GPTMR1I   (cn_int_line_base+0x12)      /* Programmable Interrupt 10 (default: General purpose IOP Timer1 Interrupt) */
#define cn_int_line_LP0I      (cn_int_line_base+0x13)      /* Programmable Interrupt 11 (default: LinkPort 0 Interrupt) */
#define cn_int_line_UARTI      (cn_int_line_base+0x14)      /* Programmable Interrupt 12 () */
#define cn_int_line_EPDM1I    (cn_int_line_base+0x15)      /* Programmable Interrupt 13 (default: External port DMA1 Interrupt) */
#define cn_int_line_DPII      (cn_int_line_base+0x16)      /* Programmable Interrupt 14 (DPII) */
#define cn_int_line_MTMI      (cn_int_line_base+0x17)      /* Programmable Interrupt 15 (default: MTMDMA Interrupt) */
#define cn_int_line_LP1I      (cn_int_line_base+0x18)      /* Programmable Interrupt 16 (default: LinkPort 1 Interrupt) */
#define cn_int_line_P17I      (cn_int_line_base+0x19)      /* Programmable Interrupt 17 () */
#define cn_int_line_TWII     (cn_int_line_base+0x1a)      /* Programmable Interrupt 18 (default: I2C Interrupt) */
#define cn_int_line_CB7I      (cn_int_line_base+0x1b)      /* Exception: Circ. buffer 7 overflow */
#define cn_int_line_CB15I     (cn_int_line_base+0x1c)      /* Exception: Circ. buffer 15 overflow */
#define cn_int_line_TMZLI     (cn_int_line_base+0x1d)      /* Timer = 0 (low priority) */
#define cn_int_line_FIXI      (cn_int_line_base+0x1e)      /* Exception: Fixed-pt. overflow */
#define cn_int_line_FLTOI     (cn_int_line_base+0x1f)      /* Exception: fltg-pt. overflow */
#define cn_int_line_FLTUI     (cn_int_line_base+0x20)      /* Exception: fltg-pt. underflow */
#define cn_int_line_FLTII     (cn_int_line_base+0x21)      /* Exception: fltg-pt. invalid */
#define cn_int_line_EMULI     (cn_int_line_base+0x22)      /* Exception: emulator interrupt low priority */
#define cn_int_line_SFT0I     (cn_int_line_base+0x23)      /* user software int 0 */
#define cn_int_line_SFT1I     (cn_int_line_base+0x24)      /* user software int 1 */
#define cn_int_line_SFT2I     (cn_int_line_base+0x25)      /* user software int 2 */
#define cn_int_line_SFT3I     (cn_int_line_base+0x26)      /* user software int 3 */
#define CN_INT_LINE_LAST      (0x26)

//defines for DAI and DPI
#define cn_int_line_dai_base  (0x00)                        /* Programmable Interrupt 0  (DAII) */
#define cn_int_line_VALID     (cn_int_line_dai_base+0x00)   /* DAII: SPDIF RX Validity interrupt */
#define cn_int_line_LOCK      (cn_int_line_dai_base+0x01)   /* DAII: SPDIF RX Lock interrupt */
#define cn_int_line_NOSTREAM  (cn_int_line_dai_base+0x02)   /* DAII: SPDIF RX No Stream interrupt */
#define cn_int_line_CRCERROR  (cn_int_line_dai_base+0x03)   /* DAII: SPDIF RX CRC Error interrupt */
#define cn_int_line_NOAUDIO   (cn_int_line_dai_base+0x04)   /* DAII: SPDIF RX NO AUDIO interrupt */
#define cn_int_line_EMPHASIS  (cn_int_line_dai_base+0x05)   /* DAII: SPDIF RX EMPHASIS interrupt */
#define cn_int_line_SPDIFERR  (cn_int_line_dai_base+0x06)   /* DAII: SPDIF RX Parity OR Biphase Error interrupt */
#define cn_int_line_STATCNG   (cn_int_line_dai_base+0x07)   /* DAII: SPDIF RX Status Change interrupt */
#define cn_int_line_FIFOGTN   (cn_int_line_dai_base+0x08)   /* DAII: IDP FIFO Samples Exceeded Interrupt (Not FE) */
#define cn_int_line_FIFOOVR   (cn_int_line_dai_base+0x09)   /* DAII: IDP FIFO Overflow Interrupt (Not FE) */
#define cn_int_line_DMA0      (cn_int_line_dai_base+0x0a)   /* DAII: IDP DMA0 Interrupt (Not FE) */
#define cn_int_line_DMA1      (cn_int_line_dai_base+0x0b)   /* DAII: IDP DMA1 Interrupt (Not FE) */
#define cn_int_line_DMA2      (cn_int_line_dai_base+0x0c)   /* DAII: IDP DMA2 Interrupt (Not FE) */
#define cn_int_line_DMA3      (cn_int_line_dai_base+0x0d)   /* DAII: IDP DMA3 Interrupt (Not FE) */
#define cn_int_line_DMA4      (cn_int_line_dai_base+0x0e)   /* DAII: IDP DMA4 Interrupt (Not FE) */
#define cn_int_line_DMA5      (cn_int_line_dai_base+0x0f)   /* DAII: IDP DMA5 Interrupt (Not FE) */
#define cn_int_line_DMA6      (cn_int_line_dai_base+0x10)   /* DAII: IDP DMA6 Interrupt (Not FE) */
#define cn_int_line_DMA7      (cn_int_line_dai_base+0x11)   /* DAII: IDP DMA7 Interrupt (Not FE) */
#define cn_int_line_SRC0      (cn_int_line_dai_base+0x12)   /* DAII: SRC0 MUTE OUT interrupt */
#define cn_int_line_SRC1      (cn_int_line_dai_base+0x13)   /* DAII: SRC1 MUTE OUT interrupt */
#define cn_int_line_SRC2      (cn_int_line_dai_base+0x14)   /* DAII: SRC2 MUTE OUT interrupt */
#define cn_int_line_SRC3      (cn_int_line_dai_base+0x15)   /* DAII: SRC3 MUTE OUT interrupt */
#define cn_int_line_EXTMISCB0 (cn_int_line_dai_base+0x16)   /* DAII: EXT_MISCB_0 Interrupt */
#define cn_int_line_EXTMISCB1 (cn_int_line_dai_base+0x17)   /* DAII: EXT_MISCB_1 Interrupt */
#define cn_int_line_EXTMISCB2 (cn_int_line_dai_base+0x18)   /* DAII: EXT_MISCB_2 Interrupt */
#define cn_int_line_EXTMISCB3 (cn_int_line_dai_base+0x19)   /* DAII: EXT_MISCB_3 Interrupt */
#define cn_int_line_EXTMISCB4 (cn_int_line_dai_base+0x1a)   /* DAII: EXT_MISCB_4 Interrupt */
#define cn_int_line_EXTMISCB5 (cn_int_line_dai_base+0x1b)   /* DAII: EXT_MISCB_5 Interrupt */
#define cn_int_line_EXTMISCA0 (cn_int_line_dai_base+0x1c)   /* DAII: EXT_MISCA_0 Interrupt */
#define cn_int_line_EXTMISCA1 (cn_int_line_dai_base+0x1d)   /* DAII: EXT_MISCA_1 Interrupt */
#define cn_int_line_EXTMISCA2 (cn_int_line_dai_base+0x1e)   /* DAII: EXT_MISCA_2 Interrupt */
#define cn_int_line_EXTMISCA3 (cn_int_line_dai_base+0x1f)   /* DAII: EXT_MISCA_3 Interrupt */

#define cn_int_line_dpi_base  (0x00)                        /* Programmable Interrupt 14 (DPII) */
#define cn_int_line_UART0TX   (cn_int_line_dpi_base+0x00)   /* DPII: UART0 TX interrupt */
#define cn_int_line_UART0RX   (cn_int_line_dpi_base+0x01)   /* DPII: UART0 RX interrupt */
#define cn_int_line_TWI       (cn_int_line_dpi_base+0x02)   /* DPII: TWI(I2C) interrupt */
#define cn_int_line_EXTMISC0  (cn_int_line_dpi_base+0x03)   /* DPII: EXT_MISC_0 Interrupt */
#define cn_int_line_EXTMISC1  (cn_int_line_dpi_base+0x04)   /* DPII: EXT_MISC_1 Interrupt */
#define cn_int_line_EXTMISC2  (cn_int_line_dpi_base+0x05)   /* DPII: EXT_MISC_2 Interrupt */
#define cn_int_line_EXTMISC3  (cn_int_line_dpi_base+0x06)   /* DPII: EXT_MISC_3 Interrupt */
#define cn_int_line_EXTMISC4  (cn_int_line_dpi_base+0x07)   /* DPII: EXT_MISC_4 Interrupt */
#define cn_int_line_EXTMISC5  (cn_int_line_dpi_base+0x08)   /* DPII: EXT_MISC_5 Interrupt */
#define cn_int_line_EXTMISC6  (cn_int_line_dpi_base+0x09)   /* DPII: EXT_MISC_6 Interrupt */
#define cn_int_line_EXTMISC7  (cn_int_line_dpi_base+0x0a)   /* DPII: EXT_MISC_7 Interrupt */
#define cn_int_line_EXTMISC8  (cn_int_line_dpi_base+0x0b)   /* DPII: EXT_MISC_8 Interrupt */



//real/sync mask
#define real_mask_imask     0xF04FFD10
#define real_mask_lirptl    0x000FFC00
#define asyn_mask_imask     0xF04FFC10
#define asyn_mask_lirptl    0x000FFC00


#endif //__CPU_PERI_INT_LINE_H__

