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
//所属模块:IO初始化
//作者：lst
//版本：V1.0.0
//文件描述:IO初始化
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-03-10
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __CPU_PERI_GPIO_REG__
#define __CPU_PERI_GPIO_REG__

#ifdef __cplusplus
extern "C" {
#endif

#define cn_p0_0_cn_p0_0_default         (u32)0
#define cn_p0_0_cn_p0_0_axr0_8          (u32)1
#define cn_p0_0_clks1                   (u32)2
#define cn_p0_0_ecap1                   (u32)4
#define cn_p0_0_gpio                    (u32)8

#define cn_p0_1_default                 (u32)0
#define cn_p0_1_axr0_9                  (u32)1
#define cn_p0_1_dx1                     (u32)2
#define cn_p0_1_observe0_phy_state2     (u32)4
#define cn_p0_1_gpio0_1                 (u32)8

#define cn_p0_2_default                 (u32)0
#define cn_p0_2_axr0_10                 (u32)1
#define cn_p0_2_dr1                     (u32)2
#define cn_p0_2_observe0_phy_state1     (u32)4
#define cn_p0_2_gpio0_2                 (u32)8

#define cn_p0_3_default                 (u32)0
#define cn_p0_3_axr0_11                 (u32)1
#define cn_p0_3_fsx1                    (u32)2
#define cn_p0_3_observe0_phy_state0     (u32)4
#define cn_p0_3_gpio0_3                 (u32)8

#define cn_p0_4_default                 (u32)0
#define cn_p0_4_axr0_12                 (u32)1
#define cn_p0_4_fsr1                    (u32)2
#define cn_p0_4_observe0_phy_ready      (u32)4
#define cn_p0_4_gpio0_4                 (u32)8

#define cn_p0_5_default                 (u32)0
#define cn_p0_5_axr0_13                 (u32)1
#define cn_p0_5_clkx1                   (u32)2
#define cn_p0_5_observe0_cominit        (u32)4
#define cn_p0_5_gpio0_5                 (u32)8

#define cn_p0_6_default                 (u32)0
#define cn_p0_6_axr0_14                 (u32)1
#define cn_p0_6_clkr1                   (u32)2
#define cn_p0_6_observe0_comwake        (u32)4
#define cn_p0_6_gpio0_6                 (u32)8

#define cn_p0_7_default                 (u32)0
#define cn_p0_7_axr0_15                 (u32)1
#define cn_p0_7_epwm0tz0                (u32)2
#define cn_p0_7_ecap2                   (u32)4
#define cn_p0_7_gpio0_7                 (u32)8

#define cn_p0_8_default                 (u32)0
#define cn_p0_8_reserved1               (u32)1
#define cn_p0_8_alarm                   (u32)2
#define cn_p0_8_uart2_cts               (u32)4
#define cn_p0_8_gpio0_8                 (u32)8

#define cn_p0_9_default                 (u32)0
#define cn_p0_9_amute0                  (u32)1
#define cn_p0_9_pru0_r30_16             (u32)2
#define cn_p0_9_uart2_rts               (u32)4
#define cn_p0_9_gpio0_9                 (u32)8

#define cn_p0_10_default                (u32)0
#define cn_p0_10_ahclkx0                (u32)1
#define cn_p0_10_usb_refclkin           (u32)2
#define cn_p0_10_uart1_cts              (u32)4
#define cn_p0_10_gpio0_10               (u32)8

#define cn_p0_11_default                (u32)0
#define cn_p0_11_ahclkr0                (u32)1
#define cn_p0_11_pru0_r30_18            (u32)2
#define cn_p0_11_uart1_rts              (u32)4
#define cn_p0_11_gpio0_11               (u32)8

#define cn_p0_12_default                (u32)0
#define cn_p0_12_afsx0                  (u32)1
#define cn_p0_12_reserved2              (u32)2
#define cn_p0_12_observe0_los           (u32)4
#define cn_p0_12_gpio0_12               (u32)8

#define cn_p0_13_default                (u32)0
#define cn_p0_13_afsr0                  (u32)1
#define cn_p0_13_reserved2              (u32)2
#define cn_p0_13_observe0_sync          (u32)4
#define cn_p0_13_gpio0_13               (u32)8

#define cn_p0_14_default                (u32)0
#define cn_p0_14_aclkx0                 (u32)1
#define cn_p0_14_reserved2              (u32)2
#define cn_p0_14_pru0_r30_19            (u32)4
#define cn_p0_14_gpio0_14               (u32)8

#define cn_p0_15_default                (u32)0
#define cn_p0_15_aclkr0                 (u32)1
#define cn_p0_15_reserved2              (u32)2
#define cn_p0_15_pru0_r30_20            (u32)4
#define cn_p0_15_gpio0_15               (u32)8

#define cn_p1_0_default                 (u32)0
#define cn_p1_0_nspi1_scs2              (u32)1
#define cn_p1_0_uart1_txd               (u32)2
#define cn_p1_0_cp_pod                  (u32)4
#define cn_p1_0_gpio1_0                 (u32)8

#define cn_p1_1_default                 (u32)0
#define cn_p1_1_nspi1_scs3              (u32)1
#define cn_p1_1_uart1_rxd               (u32)2
#define cn_p1_1_led                     (u32)4
#define cn_p1_1_gpio1_1                 (u32)8

#define cn_p1_2_default                 (u32)0
#define cn_p1_2_nspi1_scs4              (u32)1
#define cn_p1_2_uart2_txd               (u32)2
#define cn_p1_2_i2c1_sda                (u32)4
#define cn_p1_2_gpio1_2                 (u32)8

#define cn_p1_3_default                 (u32)0
#define cn_p1_3_nspi1_scs5              (u32)1
#define cn_p1_3_uart2_rxd               (u32)2
#define cn_p1_3_i2c1_scl                (u32)4
#define cn_p1_3_gpio1_3                 (u32)8

#define cn_p1_4_default                 (u32)0
#define cn_p1_4_nspi1_scs6              (u32)1
#define cn_p1_4_i2c0_sda                (u32)2
#define cn_p1_4_tm64p3_out12            (u32)4
#define cn_p1_4_gpio1_4                 (u32)8

#define cn_p1_5_default                 (u32)0
#define cn_p1_5_nspi1_scs7              (u32)1
#define cn_p1_5_i2c0_scl                (u32)2
#define cn_p1_5_tm64p2_out12            (u32)4
#define cn_p1_5_gpio1_5                 (u32)8

#define cn_p1_6_default                 (u32)0
#define cn_p1_6_nspi0_scs0              (u32)1
#define cn_p1_6_tm64p1_out12            (u32)2
#define cn_p1_6_gpio1_6                 (u32)4
#define cn_p1_6_mdio_d                  (u32)8

#define cn_p1_7_default                 (u32)0
#define cn_p1_7_nspi0_scs1              (u32)1
#define cn_p1_7_tm64p0_out12            (u32)2
#define cn_p1_7_gpio1_7                 (u32)4
#define cn_p1_7_mdio_clk                (u32)8

#define cn_p1_8_default                 (u32)0
#define cn_p1_8_spi0_clk                (u32)1
#define cn_p1_8_epwm0a                  (u32)2
#define cn_p1_8_gpio1_8                 (u32)4
#define cn_p1_8_mii_rxclk               (u32)8

#define cn_p1_9_default                 (u32)0
#define cn_p1_9_axr0_1                  (u32)1
#define cn_p1_9_dx0                     (u32)2
#define cn_p1_9_gpio1_9                 (u32)4
#define cn_p1_9_mii_txd1                (u32)8

#define cn_p1_10_default                (u32)0
#define cn_p1_10_axr0_2                 (u32)1
#define cn_p1_10_dr0                    (u32)2
#define cn_p1_10_gpio1_10               (u32)4
#define cn_p1_10_mii_txd2               (u32)8

#define cn_p1_11_default                (u32)0
#define cn_p1_11_axr0_3                 (u32)1
#define cn_p1_11_fsx0                   (u32)2
#define cn_p1_11_gpio1_11               (u32)4
#define cn_p1_11_mii_txd3               (u32)8

#define cn_p1_12_default                (u32)0
#define cn_p1_12_axr0_4                 (u32)1
#define cn_p1_12_fsr0                   (u32)2
#define cn_p1_12_gpio1_12               (u32)4
#define cn_p1_12_mii_col                (u32)8

#define cn_p1_13_default                (u32)0
#define cn_p1_13_axr0_5                 (u32)1
#define cn_p1_13_clkx0                  (u32)2
#define cn_p1_13_gpio1_13               (u32)4
#define cn_p1_13_mii_txclk              (u32)8

#define cn_p1_14_default                (u32)0
#define cn_p1_14_axr0_6                 (u32)1
#define cn_p1_14_clkr0                  (u32)2
#define cn_p1_14_gpio1_14               (u32)4
#define cn_p1_14_mii_txen               (u32)8

#define cn_p1_15_default                (u32)0
#define cn_p1_15_axr0_7                 (u32)1
#define cn_p1_15_epwm1tz0               (u32)2
#define cn_p1_15_pru0_r30_17            (u32)4
#define cn_p1_15_gpio1_15               (u32)8

#define cn_p2_0_default                 (u32)0
#define cn_p2_0_nema_cs0                (u32)1
#define cn_p2_0_reserved2               (u32)2
#define cn_p2_0_reserved4               (u32)4
#define cn_p2_0_gpio2_0                 (u32)8

#define cn_p2_1_default                 (u32)0
#define cn_p2_1_ema_wait1               (u32)1
#define cn_p2_1_reserved2               (u32)2
#define cn_p2_1_pru0_r30_1              (u32)4
#define cn_p2_1_gpio2_1                 (u32)8

#define cn_p2_2_default                 (u32)0
#define cn_p2_2_nema_we_dqm1            (u32)1
#define cn_p2_2_reserved2               (u32)2
#define cn_p2_2_reserved4               (u32)4
#define cn_p2_2_gpio2_2                 (u32)8

#define cn_p2_3_default                 (u32)0
#define cn_p2_3_nema_we_dqm0            (u32)1
#define cn_p2_3_reserved2               (u32)2
#define cn_p2_3_reserved4               (u32)4
#define cn_p2_3_gpio2_3                 (u32)8

#define cn_p2_4_default                 (u32)0
#define cn_p2_4_nema_cas                (u32)1
#define cn_p2_4_reserved2               (u32)2
#define cn_p2_4_pru0_r30_2              (u32)4
#define cn_p2_4_gpio2_4                 (u32)8

#define cn_p2_5_default                 (u32)0
#define cn_p2_5_nema_ras                (u32)1
#define cn_p2_5_reserved2               (u32)2
#define cn_p2_5_pru0_r30_3              (u32)4
#define cn_p2_5_gpio2_5                 (u32)8

#define cn_p2_6_default                 (u32)0
#define cn_p2_6_ema_sdcke               (u32)1
#define cn_p2_6_reserved2               (u32)2
#define cn_p2_6_pru0_r30_4              (u32)4
#define cn_p2_6_gpio2_6                 (u32)8

#define cn_p2_7_default                 (u32)0
#define cn_p2_7_ema_clk                 (u32)1
#define cn_p2_7_reserved2               (u32)2
#define cn_p2_7_pru0_r30_5              (u32)4
#define cn_p2_7_gpio2_7                 (u32)8

#define cn_p2_8_default                 (u32)0
#define cn_p2_8_ema_ba0                 (u32)1
#define cn_p2_8_reserved2               (u32)2
#define cn_p2_8_reserved4               (u32)4
#define cn_p2_8_gpio2_8                 (u32)8

#define cn_p2_9_default                 (u32)0
#define cn_p2_9_ema_ba1                 (u32)1
#define cn_p2_9_reserved2               (u32)2
#define cn_p2_9_reserved4               (u32)4
#define cn_p2_9_gpio2_9                 (u32)8

#define cn_p2_10_default                (u32)0
#define cn_p2_10_spi1_simo0             (u32)1
#define cn_p2_10_reserved2              (u32)2
#define cn_p2_10_reserved4              (u32)4
#define cn_p2_10_gpio2_10               (u32)8

#define cn_p2_11_default                (u32)0
#define cn_p2_11_spi1_somi0             (u32)1
#define cn_p2_11_reserved2              (u32)2
#define cn_p2_11_reserved4              (u32)4
#define cn_p2_11_gpio2_11               (u32)8

#define cn_p2_12_default                (u32)0
#define cn_p2_12_nspi1_ena              (u32)1
#define cn_p2_12_reserved2              (u32)2
#define cn_p2_12_reserved4              (u32)4
#define cn_p2_12_gpio2_12               (u32)8

#define cn_p2_13_default                (u32)0
#define cn_p2_13_spi1_clk               (u32)1
#define cn_p2_13_reserved2              (u32)2
#define cn_p2_13_reserved4              (u32)4
#define cn_p2_13_gpio2_13               (u32)8

#define cn_p2_14_default                (u32)0
#define cn_p2_14_nspi1_scs0             (u32)1
#define cn_p2_14_epwm1b                 (u32)2
#define cn_p2_14_pru0_r30_7             (u32)4
#define cn_p2_14_gpio2_14               (u32)8

#define cn_p2_15_default                (u32)0
#define cn_p2_15_nspi1_scs1             (u32)1
#define cn_p2_15_epwm1a                 (u32)2
#define cn_p2_15_pru0_r30_8             (u32)4
#define cn_p2_15_gpio2_15               (u32)8

#define cn_p3_0_default                 (u32)0
#define cn_p3_0_ema_d8                  (u32)1
#define cn_p3_0_reserved2               (u32)2
#define cn_p3_0_reserved4               (u32)4
#define cn_p3_0_gpio3_0                 (u32)8

#define cn_p3_1_default                 (u32)0
#define cn_p3_1_ema_d9                  (u32)1
#define cn_p3_1_reserved2               (u32)2
#define cn_p3_1_reserved4               (u32)4
#define cn_p3_1_gpio3_1                 (u32)8

#define cn_p3_2_default                 (u32)0
#define cn_p3_2_ema_d10                 (u32)1
#define cn_p3_2_reserved2               (u32)2
#define cn_p3_2_reserved4               (u32)4
#define cn_p3_2_gpio3_2                 (u32)8

#define cn_p3_3_default                 (u32)0
#define cn_p3_3_ema_d11                 (u32)1
#define cn_p3_3_reserved2               (u32)2
#define cn_p3_3_reserved4               (u32)4
#define cn_p3_3_gpio3_3                 (u32)8

#define cn_p3_4_default                 (u32)0
#define cn_p3_4_ema_d12                 (u32)1
#define cn_p3_4_reserved2               (u32)2
#define cn_p3_4_reserved4               (u32)4
#define cn_p3_4_gpio3_4                 (u32)8

#define cn_p3_5_default                 (u32)0
#define cn_p3_5_ema_d13                 (u32)1
#define cn_p3_5_reserved2               (u32)2
#define cn_p3_5_reserved4               (u32)4
#define cn_p3_5_gpio3_5                 (u32)8

#define cn_p3_6_default                 (u32)0
#define cn_p3_6_ema_d14                 (u32)1
#define cn_p3_6_reserved2               (u32)2
#define cn_p3_6_reserved4               (u32)4
#define cn_p3_6_gpio3_6                 (u32)8

#define cn_p3_7_default                 (u32)0
#define cn_p3_7_ema_d15                 (u32)1
#define cn_p3_7_reserved2               (u32)2
#define cn_p3_7_reserved4               (u32)4
#define cn_p3_7_gpio3_7                 (u32)8

#define cn_p3_8_default                 (u32)0
#define cn_p3_8_ema_wait0               (u32)1
#define cn_p3_8_reserved2               (u32)2
#define cn_p3_8_pru0_r30_0              (u32)4
#define cn_p3_8_gpio3_8                 (u32)8

#define cn_p3_9_default                 (u32)0
#define cn_p3_9_nema_rnw                (u32)1
#define cn_p3_9_reserved2               (u32)2
#define cn_p3_9_reserved4               (u32)4
#define cn_p3_9_gpio3_9                 (u32)8

#define cn_p3_10_default                (u32)0
#define cn_p3_10_nema_oe                (u32)1
#define cn_p3_10_reserved2              (u32)2
#define cn_p3_10_reserved4              (u32)4
#define cn_p3_10_gpio3_10               (u32)8

#define cn_p3_11_default                (u32)0
#define cn_p3_11_nema_we                (u32)1
#define cn_p3_11_reserved2              (u32)2
#define cn_p3_11_reserved4              (u32)4
#define cn_p3_11_gpio3_11               (u32)8

#define cn_p3_12_default                (u32)0
#define cn_p3_12_nema_cs5               (u32)1
#define cn_p3_12_reserved2              (u32)2
#define cn_p3_12_reserved4              (u32)4
#define cn_p3_12_gpio3_12               (u32)8

#define cn_p3_13_default                (u32)0
#define cn_p3_13_nema_cs4               (u32)1
#define cn_p3_13_reserved2              (u32)2
#define cn_p3_13_reserved4              (u32)4
#define cn_p3_13_gpio3_13               (u32)8

#define cn_p3_14_default                (u32)0
#define cn_p3_14_nema_cs3               (u32)1
#define cn_p3_14_reserved2              (u32)2
#define cn_p3_14_reserved4              (u32)4
#define cn_p3_14_gpio3_14               (u32)8

#define cn_p3_15_default                (u32)0
#define cn_p3_15_nema_cs2               (u32)1
#define cn_p3_15_reserved2              (u32)2
#define cn_p3_15_reserved4              (u32)4
#define cn_p3_15_gpio3_15               (u32)8

#define cn_p4_0_default                 (u32)0
#define cn_p4_0_ema_a16                 (u32)1
#define cn_p4_0_mmcsd0_dat5             (u32)2
#define cn_p4_0_pru1_r30_24             (u32)4
#define cn_p4_0_gpio4_0                 (u32)8

#define cn_p4_1_default                 (u32)0
#define cn_p4_1_ema_a17                 (u32)1
#define cn_p4_1_mmcsd0_dat4             (u32)2
#define cn_p4_1_pru1_r30_25             (u32)4
#define cn_p4_1_gpio4_1                 (u32)8

#define cn_p4_2_default                 (u32)0
#define cn_p4_2_ema_a18                 (u32)1
#define cn_p4_2_mmcsd0_dat3             (u32)2
#define cn_p4_2_pru1_r30_26             (u32)4
#define cn_p4_2_gpio4_2                 (u32)8

#define cn_p4_3_default                 (u32)0
#define cn_p4_3_ema_a19                 (u32)1
#define cn_p4_3_mmcsd0_dat2             (u32)2
#define cn_p4_3_pru1_r30_27             (u32)4
#define cn_p4_3_gpio4_3                 (u32)8

#define cn_p4_4_default                 (u32)0
#define cn_p4_4_ema_a20                 (u32)1
#define cn_p4_4_mmcsd0_dat1             (u32)2
#define cn_p4_4_pru1_r30_28             (u32)4
#define cn_p4_4_gpio4_4                 (u32)8

#define cn_p4_5_default                 (u32)0
#define cn_p4_5_ema_a21                 (u32)1
#define cn_p4_5_mmcsd0_dat0             (u32)2
#define cn_p4_5_pru1_r30_29             (u32)4
#define cn_p4_5_gpio4_5                 (u32)8

#define cn_p4_6_default                 (u32)0
#define cn_p4_6_ema_a22                 (u32)1
#define cn_p4_6_mmcsd0_cmd              (u32)2
#define cn_p4_6_pru1_r30_30             (u32)4
#define cn_p4_6_gpio4_6                 (u32)8

#define cn_p4_7_default                 (u32)0
#define cn_p4_7_ema_a23                 (u32)1
#define cn_p4_7_mmcsd0_clk              (u32)2
#define cn_p4_7_pru1_r30_31             (u32)4
#define cn_p4_7_gpio4_7                 (u32)8

#define cn_p4_8_default                 (u32)0
#define cn_p4_8_ema_d0                  (u32)1
#define cn_p4_8_reserved2               (u32)2
#define cn_p4_8_reserved4               (u32)4
#define cn_p4_8_gpio4_8                 (u32)8

#define cn_p4_9_default                 (u32)0
#define cn_p4_9_ema_d1                  (u32)1
#define cn_p4_9_reserved2               (u32)2
#define cn_p4_9_reserved4               (u32)4
#define cn_p4_9_gpio4_9                 (u32)8

#define cn_p4_10_default                (u32)0
#define cn_p4_10_ema_d2                 (u32)1
#define cn_p4_10_reserved2              (u32)2
#define cn_p4_10_reserved4              (u32)4
#define cn_p4_10_gpio4_10               (u32)8

#define cn_p4_11_default                (u32)0
#define cn_p4_11_ema_d3                 (u32)1
#define cn_p4_11_reserved2              (u32)2
#define cn_p4_11_reserved4              (u32)4
#define cn_p4_11_gpio4_11               (u32)8

#define cn_p4_12_default                (u32)0
#define cn_p4_12_ema_d4                 (u32)1
#define cn_p4_12_reserved2              (u32)2
#define cn_p4_12_reserved4              (u32)4
#define cn_p4_12_gpio4_12               (u32)8

#define cn_p4_13_default                (u32)0
#define cn_p4_13_ema_d5                 (u32)1
#define cn_p4_13_reserved2              (u32)2
#define cn_p4_13_reserved4              (u32)4
#define cn_p4_13_gpio4_13               (u32)8

#define cn_p4_14_default                (u32)0
#define cn_p4_14_ema_d6                 (u32)1
#define cn_p4_14_reserved2              (u32)2
#define cn_p4_14_reserved4              (u32)4
#define cn_p4_14_gpio4_14               (u32)8

#define cn_p4_15_default                (u32)0
#define cn_p4_15_ema_d7                 (u32)1
#define cn_p4_15_reserved2              (u32)2
#define cn_p4_15_reserved4              (u32)4
#define cn_p4_15_gpio4_15               (u32)8

#define cn_p5_0_default                 (u32)0
#define cn_p5_0_ema_a0                  (u32)1
#define cn_p5_0_reserved2               (u32)2
#define cn_p5_0_reserved4               (u32)4
#define cn_p5_0_gpio5_0                 (u32)8

#define cn_p5_1_default                 (u32)0
#define cn_p5_1_ema_a1                  (u32)1
#define cn_p5_1_reserved2               (u32)2
#define cn_p5_1_reserved4               (u32)4
#define cn_p5_1_gpio5_1                 (u32)8

#define cn_p5_2_default                 (u32)0
#define cn_p5_2_ema_a2                  (u32)1
#define cn_p5_2_reserved2               (u32)2
#define cn_p5_2_reserved4               (u32)4
#define cn_p5_2_gpio5_2                 (u32)8

#define cn_p5_3_default                 (u32)0
#define cn_p5_3_ema_a3                  (u32)1
#define cn_p5_3_reserved2               (u32)2
#define cn_p5_3_reserved4               (u32)4
#define cn_p5_3_gpio5_3                 (u32)8

#define cn_p5_4_default                 (u32)0
#define cn_p5_4_ema_a4                  (u32)1
#define cn_p5_4_reserved2               (u32)2
#define cn_p5_4_reserved4               (u32)4
#define cn_p5_4_gpio5_4                 (u32)8

#define cn_p5_5_default                 (u32)0
#define cn_p5_5_ema_a5                  (u32)1
#define cn_p5_5_reserved2               (u32)2
#define cn_p5_5_reserved4               (u32)4
#define cn_p5_5_gpio5_5                 (u32)8

#define cn_p5_6_default                 (u32)0
#define cn_p5_6_ema_a6                  (u32)1
#define cn_p5_6_reserved2               (u32)2
#define cn_p5_6_reserved4               (u32)4
#define cn_p5_6_gpio5_6                 (u32)8

#define cn_p5_7_default                 (u32)0
#define cn_p5_7_ema_a7                  (u32)1
#define cn_p5_7_reserved2               (u32)2
#define cn_p5_7_pru1_r30_15             (u32)4
#define cn_p5_7_gpio5_7                 (u32)8

#define cn_p5_8_default                 (u32)0
#define cn_p5_8_ema_a8                  (u32)1
#define cn_p5_8_reserved2               (u32)2
#define cn_p5_8_pru1_r30_16             (u32)4
#define cn_p5_8_gpio5_8                 (u32)8

#define cn_p5_9_default                 (u32)0
#define cn_p5_9_ema_a9                  (u32)1
#define cn_p5_9_reserved2               (u32)2
#define cn_p5_9_pru1_r30_17             (u32)4
#define cn_p5_9_gpio5_9                 (u32)8

#define cn_p5_10_default                (u32)0
#define cn_p5_10_ema_a10                (u32)1
#define cn_p5_10_reserved2              (u32)2
#define cn_p5_10_pru1_r30_18            (u32)4
#define cn_p5_10_gpio5_10               (u32)8

#define cn_p5_11_default                (u32)0
#define cn_p5_11_ema_a11                (u32)1
#define cn_p5_11_reserved2              (u32)2
#define cn_p5_11_pru1_r30_19            (u32)4
#define cn_p5_11_gpio5_11               (u32)8

#define cn_p5_12_default                (u32)0
#define cn_p5_12_ema_a12                (u32)1
#define cn_p5_12_reserved2              (u32)2
#define cn_p5_12_pru1_r30_20            (u32)4
#define cn_p5_12_gpio5_12               (u32)8

#define cn_p5_13_default                (u32)0
#define cn_p5_13_ema_a13                (u32)1
#define cn_p5_13_pru0_r30_21            (u32)2
#define cn_p5_13_pru1_r30_21            (u32)4
#define cn_p5_13_gpio5_13               (u32)8

#define cn_p5_14_default                (u32)0
#define cn_p5_14_ema_a14                (u32)1
#define cn_p5_14_mmcsd0_dat7            (u32)2
#define cn_p5_14_pru1_r30_22            (u32)4
#define cn_p5_14_gpio5_14               (u32)8

#define cn_p5_15_default                (u32)0
#define cn_p5_15_ema_a15                (u32)1
#define cn_p5_15_mmcsd0_dat6            (u32)2
#define cn_p5_15_pru1_r30_23            (u32)4
#define cn_p5_15_gpio5_15               (u32)8

#define cn_p6_0_default                 (u32)0
#define cn_p6_0_reserved1               (u32)1
#define cn_p6_0_nlcd_ac_enb_cs          (u32)2
#define cn_p6_0_reserved4               (u32)4
#define cn_p6_0_gpio6_0                 (u32)8

#define cn_p6_1_default                 (u32)0
#define cn_p6_1_clko3                   (u32)1
#define cn_p6_1_reserved2               (u32)2
#define cn_p6_1_pru1_r30_0              (u32)4
#define cn_p6_1_gpio6_1                 (u32)8

#define cn_p6_2_default                 (u32)0
#define cn_p6_2_clkin3                  (u32)1
#define cn_p6_2_mmcsd1_dat1             (u32)2
#define cn_p6_2_pru1_r30_1              (u32)4
#define cn_p6_2_gpio6_2                 (u32)8

#define cn_p6_3_default                 (u32)0
#define cn_p6_3_clko2                   (u32)1
#define cn_p6_3_mmcsd1_dat2             (u32)2
#define cn_p6_3_pru1_r30_2              (u32)4
#define cn_p6_3_gpio6_3                 (u32)8

#define cn_p6_4_default                 (u32)0
#define cn_p6_4_clkin2                  (u32)1
#define cn_p6_4_mmcsd1_dat3             (u32)2
#define cn_p6_4_pru1_r30_3              (u32)4
#define cn_p6_4_gpio6_4                 (u32)8

#define cn_p6_5_default                 (u32)0
#define cn_p6_5_din8                    (u32)1
#define cn_p6_5_uhpi_hd0                (u32)2
#define cn_p6_5_upp_d0                  (u32)4
#define cn_p6_5_gpio6_5                 (u32)8

#define cn_p6_6_default                 (u32)0
#define cn_p6_6_clkin1                  (u32)1
#define cn_p6_6_nuhpi_hds1              (u32)2
#define cn_p6_6_pru1_r30_9              (u32)4
#define cn_p6_6_gpio6_6                 (u32)8

#define cn_p6_7_default                 (u32)0
#define cn_p6_7_clkin0                  (u32)1
#define cn_p6_7_nuhpi_hcs               (u32)2
#define cn_p6_7_pru1_r30_10             (u32)4
#define cn_p6_7_gpio6_7                 (u32)8

#define cn_p6_8_default                 (u32)0
#define cn_p6_8_pru0_r30_26             (u32)1
#define cn_p6_8_uhpi_hrnw               (u32)2
#define cn_p6_8_ch1_wait                (u32)4
#define cn_p6_8_gpio6_8                 (u32)8

#define cn_p6_9_default                 (u32)0
#define cn_p6_9_pru0_r30_27             (u32)1
#define cn_p6_9_uhpi_hhwil              (u32)2
#define cn_p6_9_ch1_enable              (u32)4
#define cn_p6_9_gpio6_9                 (u32)8

#define cn_p6_10_default                (u32)0
#define cn_p6_10_pru0_r30_28            (u32)1
#define cn_p6_10_uhpi_hcntl1            (u32)2
#define cn_p6_10_ch1_start              (u32)4
#define cn_p6_10_gpio6_10               (u32)8

#define cn_p6_11_default                (u32)0
#define cn_p6_11_pru0_r30_29            (u32)1
#define cn_p6_11_uhpi_hcntl0            (u32)2
#define cn_p6_11_ch1_clk                (u32)4
#define cn_p6_11_gpio6_11               (u32)8

#define cn_p6_12_default                (u32)0
#define cn_p6_12_pru0_r30_30            (u32)1
#define cn_p6_12_nuhpi_hint             (u32)2
#define cn_p6_12_pru1_r30_11            (u32)4
#define cn_p6_12_gpio6_12               (u32)8

#define cn_p6_13_default                (u32)0
#define cn_p6_13_pru0_r30_31            (u32)1
#define cn_p6_13_nuhpi_hrdy             (u32)2
#define cn_p6_13_pru1_r30_12            (u32)4
#define cn_p6_13_gpio6_13               (u32)8

#define cn_p6_14_default                (u32)0
#define cn_p6_14_obsclk0                (u32)1
#define cn_p6_14_nuhpi_hds2             (u32)2
#define cn_p6_14_pru1_r30_13            (u32)4
#define cn_p6_14_gpio6_14               (u32)8

#define cn_p6_15_default                (u32)0
#define cn_p6_15_nresetout              (u32)1
#define cn_p6_15_nuhpi_has              (u32)2
#define cn_p6_15_pru1_r30_14            (u32)4
#define cn_p6_15_gpio6_15               (u32)8

#define cn_p7_0_default                 (u32)0
#define cn_p7_0_dout8                   (u32)1
#define cn_p7_0_lcd_d8                  (u32)2
#define cn_p7_0_upp_xd0                 (u32)4
#define cn_p7_0_gpio7_0                 (u32)8

#define cn_p7_1_default                 (u32)0
#define cn_p7_1_dout9                   (u32)1
#define cn_p7_1_lcd_d9                  (u32)2
#define cn_p7_1_upp_xd1                 (u32)4
#define cn_p7_1_gpio7_1                 (u32)8

#define cn_p7_2_default                 (u32)0
#define cn_p7_2_dout10                  (u32)1
#define cn_p7_2_lcd_d10                 (u32)2
#define cn_p7_2_upp_xd2                 (u32)4
#define cn_p7_2_gpio7_2                 (u32)8

#define cn_p7_3_default                 (u32)0
#define cn_p7_3_dout11                  (u32)1
#define cn_p7_3_lcd_d11                 (u32)2
#define cn_p7_3_upp_xd3                 (u32)4
#define cn_p7_3_gpio7_3                 (u32)8

#define cn_p7_4_default                 (u32)0
#define cn_p7_4_dout12                  (u32)1
#define cn_p7_4_lcd_d12                 (u32)2
#define cn_p7_4_upp_xd4                 (u32)4
#define cn_p7_4_gpio7_4                 (u32)8

#define cn_p7_5_default                 (u32)0
#define cn_p7_5_dout13                  (u32)1
#define cn_p7_5_lcd_d13                 (u32)2
#define cn_p7_5_upp_xd5                 (u32)4
#define cn_p7_5_gpio7_5                 (u32)8

#define cn_p7_6_default                 (u32)0
#define cn_p7_6_dout14                  (u32)1
#define cn_p7_6_lcd_d14                 (u32)2
#define cn_p7_6_upp_xd6                 (u32)4
#define cn_p7_6_gpio7_6                 (u32)8

#define cn_p7_7_default                 (u32)0
#define cn_p7_7_dout15                  (u32)1
#define cn_p7_7_lcd_d15                 (u32)2
#define cn_p7_7_upp_xd7                 (u32)4
#define cn_p7_7_gpio7_7                 (u32)8

#define cn_p7_8_default                 (u32)0
#define cn_p7_8_dout0                   (u32)1
#define cn_p7_8_lcd_d0                  (u32)2
#define cn_p7_8_upp_xd8                 (u32)4
#define cn_p7_8_gpio7_8                 (u32)8

#define cn_p7_9_default                 (u32)0
#define cn_p7_9_dout1                   (u32)1
#define cn_p7_9_lcd_d1                  (u32)2
#define cn_p7_9_upp_xd9                 (u32)4
#define cn_p7_9_gpio7_9                 (u32)8

#define cn_p7_10_default                (u32)0
#define cn_p7_10_dout2                  (u32)1
#define cn_p7_10_lcd_d2                 (u32)2
#define cn_p7_10_upp_xd10               (u32)4
#define cn_p7_10_gpio7_10               (u32)8

#define cn_p7_11_default                (u32)0
#define cn_p7_11_dout3                  (u32)1
#define cn_p7_11_lcd_d3                 (u32)2
#define cn_p7_11_upp_xd11               (u32)4
#define cn_p7_11_gpio7_11               (u32)8

#define cn_p7_12_default                (u32)0
#define cn_p7_12_dout4                  (u32)1
#define cn_p7_12_lcd_d4                 (u32)2
#define cn_p7_12_upp_xd12               (u32)4
#define cn_p7_12_gpio7_12               (u32)8

#define cn_p7_13_default                (u32)0
#define cn_p7_13_dout5                  (u32)1
#define cn_p7_13_lcd_d5                 (u32)2
#define cn_p7_13_upp_xd13               (u32)4
#define cn_p7_13_gpio7_13               (u32)8

#define cn_p7_14_default                (u32)0
#define cn_p7_14_dout6                  (u32)1
#define cn_p7_14_lcd_d6                 (u32)2
#define cn_p7_14_upp_xd14               (u32)4
#define cn_p7_14_gpio7_14               (u32)8

#define cn_p7_15_default                (u32)0
#define cn_p7_15_dout7                  (u32)1
#define cn_p7_15_lcd_d7                 (u32)2
#define cn_p7_15_upp_xd15               (u32)4
#define cn_p7_15_gpio7_15               (u32)8

#define cn_p8_0_default                 (u32)0
#define cn_p8_0_rtck                    (u32)1
#define cn_p8_0_reserved2               (u32)2
#define cn_p8_0_reserved4               (u32)4
#define cn_p8_0_gpio8_0                 (u32)8

#define cn_p8_1_default                 (u32)0
#define cn_p8_1_nspi0_scs2              (u32)1
#define cn_p8_1_uart0_rts               (u32)2
#define cn_p8_1_gpio8_1                 (u32)4
#define cn_p8_1_mii_rxd0                (u32)8

#define cn_p8_2_default                 (u32)0
#define cn_p8_2_nspi0_scs3              (u32)1
#define cn_p8_2_uart0_cts               (u32)2
#define cn_p8_2_gpio8_2                 (u32)4
#define cn_p8_2_mii_rxd1                (u32)8

#define cn_p8_3_default                 (u32)0
#define cn_p8_3_nspi0_scs4              (u32)1
#define cn_p8_3_uart0_txd               (u32)2
#define cn_p8_3_gpio8_3                 (u32)4
#define cn_p8_3_mii_rxd2                (u32)8

#define cn_p8_4_default                 (u32)0
#define cn_p8_4_nspi0_scs5              (u32)1
#define cn_p8_4_uart0_rxd               (u32)2
#define cn_p8_4_gpio8_4                 (u32)4
#define cn_p8_4_mii_rxd3                (u32)8

#define cn_p8_5_default                 (u32)0
#define cn_p8_5_spi0_simo0              (u32)1
#define cn_p8_5_epwmsynco               (u32)2
#define cn_p8_5_gpio8_5                 (u32)4
#define cn_p8_5_mii_crs                 (u32)8

#define cn_p8_6_default                 (u32)0
#define cn_p8_6_spi0_somi0              (u32)1
#define cn_p8_6_epwmsynci               (u32)2
#define cn_p8_6_gpio8_6                 (u32)4
#define cn_p8_6_mii_rxer                (u32)8

#define cn_p8_7_default                 (u32)0
#define cn_p8_7_axr0_0                  (u32)1
#define cn_p8_7_ecap0                   (u32)2
#define cn_p8_7_gpio8_7                 (u32)4
#define cn_p8_7_mii_txd0                (u32)8

#define cn_p8_8_default                 (u32)0
#define cn_p8_8_mmcsd1_dat4             (u32)1
#define cn_p8_8_lcd_vsync               (u32)2
#define cn_p8_8_pru1_r30_4              (u32)4
#define cn_p8_8_gpio8_8                 (u32)8

#define cn_p8_9_default                 (u32)0
#define cn_p8_9_mmcsd1_dat5             (u32)1
#define cn_p8_9_lcd_hsync               (u32)2
#define cn_p8_9_pru1_r30_5              (u32)4
#define cn_p8_9_gpio8_9                 (u32)8

#define cn_p8_10_default                (u32)0
#define cn_p8_10_mmcsd1_dat6            (u32)1
#define cn_p8_10_lcd_mclk               (u32)2
#define cn_p8_10_pru1_r30_6             (u32)4
#define cn_p8_10_gpio8_10               (u32)8

#define cn_p8_11_default                (u32)0
#define cn_p8_11_mmcsd1_dat7            (u32)1
#define cn_p8_11_lcd_pclk               (u32)2
#define cn_p8_11_pru1_r30_7             (u32)4
#define cn_p8_11_gpio8_11               (u32)8

#define cn_p8_12_default                (u32)0
#define cn_p8_12_pru0_r30_22            (u32)1
#define cn_p8_12_pru1_r30_8             (u32)2
#define cn_p8_12_ch0_wait               (u32)4
#define cn_p8_12_gpio8_12               (u32)8

#define cn_p8_13_default                (u32)0
#define cn_p8_13_pru0_r30_23            (u32)1
#define cn_p8_13_mmcsd1_cmd             (u32)2
#define cn_p8_13_ch0_enable             (u32)4
#define cn_p8_13_gpio8_13               (u32)8

#define cn_p8_14_default                (u32)0
#define cn_p8_14_pru0_r30_24            (u32)1
#define cn_p8_14_mmcsd1_clk             (u32)2
#define cn_p8_14_ch0_start              (u32)4
#define cn_p8_14_gpio8_14               (u32)8

#define cn_p8_15_default                (u32)0
#define cn_p8_15_pru0_r30_25            (u32)1
#define cn_p8_15_mmcsd1_dat0            (u32)2
#define cn_p8_15_ch0_clk                (u32)4
#define cn_p8_15_gpio8_15               (u32)8

#define cn_px_0_default                 (u32)0
#define cn_px_0_din9                    (u32)1
#define cn_px_0_uhpi_hd1                (u32)2
#define cn_px_0_upp_d1                  (u32)4
#define cn_px_0_pru0_r30_9              (u32)8

#define cn_px_1_default                 (u32)0
#define cn_px_1_din10                   (u32)1
#define cn_px_1_uhpi_hd2                (u32)2
#define cn_px_1_upp_d2                  (u32)4
#define cn_px_1_pru0_r30_10             (u32)8

#define cn_px_2_default                 (u32)0
#define cn_px_2_din11                   (u32)1
#define cn_px_2_uhpi_hd3                (u32)2
#define cn_px_2_upp_d3                  (u32)4
#define cn_px_2_pru0_r30_11             (u32)8

#define cn_px_3_default                 (u32)0
#define cn_px_3_din12                   (u32)1
#define cn_px_3_uhpi_hd4                (u32)2
#define cn_px_3_upp_d4                  (u32)4
#define cn_px_3_pru0_r30_12             (u32)8

#define cn_px_4_default                 (u32)0
#define cn_px_4_din13_field             (u32)1
#define cn_px_4_uhpi_hd5                (u32)2
#define cn_px_4_upp_d5                  (u32)4
#define cn_px_4_pru0_r30_13             (u32)8

#define cn_px_5_default                 (u32)0
#define cn_px_5_din14_hsync             (u32)1
#define cn_px_5_uhpi_hd6                (u32)2
#define cn_px_5_upp_d6                  (u32)4
#define cn_px_5_pru0_r30_14             (u32)8

#define cn_px_6_default                 (u32)0
#define cn_px_6_din15_vsync             (u32)1
#define cn_px_6_uhpi_hd7                (u32)2
#define cn_px_6_upp_d7                  (u32)4
#define cn_px_6_pru0_r30_15             (u32)8

#define cn_px_7_default                 (u32)0
#define cn_px_7_din0                    (u32)1
#define cn_px_7_uhpi_hd8                (u32)2
#define cn_px_7_upp_d8                  (u32)4
#define cn_px_7_rmii_crs_dv             (u32)8

#define cn_px_8_default                 (u32)0
#define cn_px_8_din1                    (u32)1
#define cn_px_8_uhpi_hd9                (u32)2
#define cn_px_8_upp_d9                  (u32)4
#define cn_px_8_rmii_mhz_50_clk         (u32)8

#define cn_px_9_default                 (u32)0
#define cn_px_9_din2                    (u32)1
#define cn_px_9_uhpi_hd10               (u32)2
#define cn_px_9_upp_d10                 (u32)4
#define cn_px_9_rmii_rxer               (u32)8

#define cn_px_10_default                (u32)0
#define cn_px_10_din3                   (u32)1
#define cn_px_10_uhpi_hd11              (u32)2
#define cn_px_10_upp_d11                (u32)4
#define cn_px_10_rmii_rxd0              (u32)8

#define cn_px_11_default                (u32)0
#define cn_px_11_din4                   (u32)1
#define cn_px_11_uhpi_hd12              (u32)2
#define cn_px_11_upp_d12                (u32)4
#define cn_px_11_rmii_rxd1              (u32)8

#define cn_px_12_default                (u32)0
#define cn_px_12_din5                   (u32)1
#define cn_px_12_uhpi_hd13              (u32)2
#define cn_px_12_upp_d13                (u32)4
#define cn_px_12_rmii_txen              (u32)8

#define cn_px_13_default                (u32)0
#define cn_px_13_din6                   (u32)1
#define cn_px_13_uhpi_hd14              (u32)2
#define cn_px_13_upp_d14                (u32)4
#define cn_px_13_rmii_txd0              (u32)8

#define cn_px_14_default                (u32)0
#define cn_px_14_din7                   (u32)1
#define cn_px_14_uhpi_hd15              (u32)2
#define cn_px_14_upp_d15                (u32)4
#define cn_px_14_rmii_txd1              (u32)8

#define cn_px_15_default                (u32)0
#define cn_px_15_nspi0_ena              (u32)1
#define cn_px_15_epwm0b                 (u32)2
#define cn_px_15_pru0_r30_6             (u32)4
#define cn_px_15_mii_rxdv               (u32)8

typedef struct
{
    vu32 REVID;
    vu32 RSVD0;
    vu32 BINTEN;
    vu32 RSVD1;
    struct
    {
        vu32 dir;         //0=output,1=input
        vu32 out_data;    //输出数据。
        vu32 set_data;    //对应位写1置高，写0无效
        vu32 clr_data;    //对应位写1置低，写0无效
        vu32 in_data;     //输入数据
        vu32 set_ris_int; //对应位写1使能上升沿中断，写0无效
        vu32 clr_ris_int; //对应位写1除能上升沿中断，写0无效
        vu32 set_fal_int; //对应位写1使能下降沿中断，写0无效
        vu32 clr_fal_int; //对应位写1除能下降沿中断，写0无效
        vu32 int_stat;    //1=对应引脚有中断挂起
    }bank[5];          //port0、1为一个bank，2、3为一个bank，类推之
}tagGpioReg;

void GPIO_CfgPinFunc(u32 port,u32 pinnum,u32 func_no);
void gpio_cfg_pin_mode(u32 port,u32 pinnum,u32 mode);
void gpio_setto_in(u32 port,u32 msk);
void gpio_setto_out(u32 port,u32 msk);
u32 GPIO_GetData(u32 port);
void GPIO_OutData(u32 port,u32 data);
void GPIO_SettoHigh(u32 port,u32 msk);
void GPIO_SettoLow(u32 port,u32 msk);
void GPIO_PowerOn(u32 port);
void GPIO_PowerOff(u32 port);

#ifdef __cplusplus
}
#endif

#endif /*__CPU_PERI_GPIO_REG__*/
