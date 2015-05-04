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
#ifndef __CPU_PERI_INT_LINE__
#define __CPU_PERI_INT_LINE__

#ifdef __cplusplus
extern "C" {
#endif

#define cn_int_line_commtx          (0)
#define cn_int_line_commrx          (1)
#define cn_int_line_nint            (2)
#define cn_int_line_pru_ev0         (3)
#define cn_int_line_pru_ev1         (4)
#define cn_int_line_pru_ev2         (5)
#define cn_int_line_pru_ev3         (6)
#define cn_int_line_pru_ev4         (7)
#define cn_int_line_pru_ev5         (8)
#define cn_int_line_pru_ev6         (9)
#define cn_int_line_pru_ev7         (10)
#define cn_int_line_edma3_0_cc0     (11)
#define cn_int_line_edma3_0_cc0_err (12)
#define cn_int_line_edma3_0_tc0_err (13)
#define cn_int_line_emifa           (14)
#define cn_int_line_iic0            (15)
#define cn_int_line_mmcsd0_int0     (16)
#define cn_int_line_mmcsd0_int1     (17)
#define cn_int_line_psc0_allint     (18)
#define cn_int_line_rtc_irqs10      (19)
#define cn_int_line_spi0            (20)
#define cn_int_line_t64p0_tint12    (21)
#define cn_int_line_t64p0_tint34    (22)
#define cn_int_line_t64p1_tint12    (23)
#define cn_int_line_t64p1_tint34    (24)
#define cn_int_line_uart0           (25)
#define cn_int_line_reserved1       (26)
#define cn_int_line_proterr         (27)
#define cn_int_line_sfg_chip0       (28)
#define cn_int_line_sfg_chip1       (29)
#define cn_int_line_sfg_chip2       (30)
#define cn_int_line_sfg_chip3       (31)
#define cn_int_line_edma3_0_tc1_err (32)
#define cn_int_line_emac_c0rxthresh (33)
#define cn_int_line_emac_c0rx       (34) 
#define cn_int_line_emac_c0tx       (35)
#define cn_int_line_emac_c0misc     (36)
#define cn_int_line_emac_c1rxthresh (37)
#define cn_int_line_emac_c1rx       (38)
#define cn_int_line_emac_c1tx       (39)
#define cn_int_line_c1misc          (40)
#define cn_int_line_memerr          (41)
#define cn_int_line_gpiob0          (42)
#define cn_int_line_gpiob1          (43)
#define cn_int_line_gpiob2          (44)
#define cn_int_line_gpiob3          (45)
#define cn_int_line_gpiob4          (46)
#define cn_int_line_gpiob5          (47)
#define cn_int_line_gpiob6          (48)
#define cn_int_line_gpiob7          (49)
#define cn_int_line_gpiob8          (50)
#define cn_int_line_iic1            (51)
#define cn_int_line_lcdc            (52)
#define cn_int_line_uart1           (53)
#define cn_int_line_mcasp           (54)
#define cn_int_line_psc1_allint     (55)
#define cn_int_line_spi1            (56)
#define cn_int_line_uhpi_arm        (57)
#define cn_int_line_usb0            (58)
#define cn_int_line_usb1_hc         (59)
#define cn_int_line_usb1_r_wakeup   (60)
#define cn_int_line_uart2           (61)
#define cn_int_line_reserved2       (62)
#define cn_int_line_ehrpwm0         (63)
#define cn_int_line_ehrpwm0tz       (64)
#define cn_int_line_ehrpwm1         (65)
#define cn_int_line_ehrpwm1tz       (66)
#define cn_int_line_sata            (67)
#define cn_int_line_t64p2_all       (68)
#define cn_int_line_ecap0           (69)
#define cn_int_line_ecap1           (70)
#define cn_int_line_ecap2           (71)
#define cn_int_line_mmcad1_int0     (72)
#define cn_int_line_mmcsd1_int1     (73)
#define cn_int_line_t64p0_cmp0      (74)
#define cn_int_line_t64p0_cmp1      (75)
#define cn_int_line_t64p0_cmp2      (76)
#define cn_int_line_t64p0_cmp3      (77)
#define cn_int_line_t64p0_cmp4      (78)
#define cn_int_line_t64p0_cmp5      (79)
#define cn_int_line_t64p0_cmp6      (80)
#define cn_int_line_t64p0_cmp7      (81)
#define cn_int_line_t64p1_cmp0      (82)
#define cn_int_line_t64p1_cmp1      (83)
#define cn_int_line_t64p1_cmp2      (84)
#define cn_int_line_t64p1_cmp3      (85)
#define cn_int_line_t64p1_cmp4      (86)
#define cn_int_line_t64p1_cmp5      (87)
#define cn_int_line_t64p1_cmp6      (88)
#define cn_int_line_t64p1_cmp7      (89)
#define cn_int_line_armclkstopreq   (90)
#define cn_int_line_upp_all         (91)
#define cn_int_line_vpif_all        (92)
#define cn_int_line_edma3_1_cc0     (93)
#define cn_int_line_edma3_1_cc0_err (94)
#define cn_int_line_edma3_1_tc0_err (95)
#define cn_int_line_t64p3_all       (96)
#define cn_int_line_mcbsp0_r        (97)
#define cn_int_line_mcbsp0_x        (98)
#define cn_int_line_mcbsp1_r        (99)
#define cn_int_line_mcbsp1_x        (100)
#define CN_INT_LINE_LAST            (100)

#define cn_int_msk_all_line         (0xffffffff)

#ifdef __cplusplus
}
#endif

#endif //__CPU_PERI_INT_LINE__

