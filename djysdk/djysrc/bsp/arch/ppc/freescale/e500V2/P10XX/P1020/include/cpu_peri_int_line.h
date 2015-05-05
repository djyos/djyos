//----------------------------------------------------
//Copyright (C), 2004-2009,  lst.
//版权所有 (C), 2004-2009,   lst.
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

//ther interrupt sequence is
//external---msg--smsg--ipi--gbtimer---internal

//external interrupt
#define cn_int_line_extbase                   (0x00)
#define cn_int_line_irq0                      (cn_int_line_extbase + 0x00)
#define cn_int_line_irq1                      (cn_int_line_extbase + 0x01)
#define cn_int_line_irq2                      (cn_int_line_extbase + 0x02)
#define cn_int_line_irq3                      (cn_int_line_extbase + 0x03)
#define cn_int_line_irq4                      (cn_int_line_extbase + 0x04)
#define cn_int_line_irq5                      (cn_int_line_extbase + 0x05)
#define cn_int_line_irq6                      (cn_int_line_extbase + 0x06)
#define cn_int_line_irq7                      (cn_int_line_extbase + 0x07)
#define cn_int_line_irq8                      (cn_int_line_extbase + 0x08)
#define cn_int_line_irq9                      (cn_int_line_extbase + 0x09)
#define cn_int_line_irqa                      (cn_int_line_extbase + 0x0a)
#define cn_int_line_irqb                      (cn_int_line_extbase + 0x0b)

//msg interrupt
#define cn_int_line_msgbase                   (0x0c)
//#define cn_int_line_b0msg_base                   (0x0c)
#define cn_int_line_msg0                      (cn_int_line_msgbase + 0x00)
#define cn_int_line_msg1                      (cn_int_line_msgbase + 0x01)
#define cn_int_line_msg2                      (cn_int_line_msgbase + 0x02)
#define cn_int_line_msg3                      (cn_int_line_msgbase + 0x03)
//#define cn_int_line_b1msg_base                   (0x10)
#define cn_int_line_msg4                      (cn_int_line_msgbase + 0x04)
#define cn_int_line_msg5                      (cn_int_line_msgbase + 0x05)
#define cn_int_line_msg6                      (cn_int_line_msgbase + 0x06)
#define cn_int_line_msg7                      (cn_int_line_msgbase + 0x07)

//shared msg interrupt
#define cn_int_line_smsgbase                   (0x14)
#define cn_int_line_smsg0                      (cn_int_line_smsgbase + 0x00)
#define cn_int_line_smsg1                      (cn_int_line_smsgbase + 0x01)
#define cn_int_line_smsg2                      (cn_int_line_smsgbase + 0x02)
#define cn_int_line_smsg3                      (cn_int_line_smsgbase + 0x03)
#define cn_int_line_smsg4                      (cn_int_line_smsgbase + 0x04)
#define cn_int_line_smsg5                      (cn_int_line_smsgbase + 0x05)
#define cn_int_line_smsg6                      (cn_int_line_smsgbase + 0x06)
#define cn_int_line_smsg7                      (cn_int_line_smsgbase + 0x07)

//inter-processor interrupt,core0
#define cn_int_line_ipibase0                   (0x1c) //core0
#define cn_int_line_c0_ipi0                    (cn_int_line_ipibase0 + 0x00)
#define cn_int_line_c0_ipi1                    (cn_int_line_ipibase0 + 0x01)
#define cn_int_line_c0_ipi2                    (cn_int_line_ipibase0 + 0x02)
#define cn_int_line_c0_ipi3                    (cn_int_line_ipibase0 + 0x03)
#define cn_int_line_c0_ipi4                    (cn_int_line_ipibase0 + 0x04)
//inter-processor interrupt,core1
#define cn_int_line_ipibase1                   (0x20) //core0
#define cn_int_line_c1_ipi0                    (cn_int_line_ipibase1 + 0x00)
#define cn_int_line_c1_ipi1                    (cn_int_line_ipibase1 + 0x01)
#define cn_int_line_c1_ipi2                    (cn_int_line_ipibase1 + 0x02)
#define cn_int_line_c1_ipi3                    (cn_int_line_ipibase1 + 0x03)
#define cn_int_line_c1_ipi4                    (cn_int_line_ipibase1 + 0x04)

//pic global timer interrupt
#define cn_int_line_gbtimer_abase            (0x24)
#define cn_int_line_gbtimer_a0               (cn_int_line_gbtimer_abase + 0x00)
#define cn_int_line_gbtimer_a1               (cn_int_line_gbtimer_abase + 0x01)
#define cn_int_line_gbtimer_a2               (cn_int_line_gbtimer_abase + 0x02)
#define cn_int_line_gbtimer_a3               (cn_int_line_gbtimer_abase + 0x03)
#define cn_int_line_gbtimer_bbase            (0x28)
#define cn_int_line_gbtimer_b0               (cn_int_line_gbtimer_bbase + 0x00)
#define cn_int_line_gbtimer_b1               (cn_int_line_gbtimer_bbase + 0x01)
#define cn_int_line_gbtimer_b2               (cn_int_line_gbtimer_bbase + 0x02)
#define cn_int_line_gbtimer_b3               (cn_int_line_gbtimer_bbase + 0x03)

//internal chip interrupt---64,also means the interrupt number
#define cn_int_line_interbase                 (0x2c)
#define cn_int_line_error                     (cn_int_line_interbase + 0x00)  //
#define cn_int_line_etsec1_g1_transmit        (cn_int_line_interbase + 0x01)  //
#define cn_int_line_etsec1_g1_receive         (cn_int_line_interbase + 0x02)  //
#define cn_int_line_elbc_general_interrupt    (cn_int_line_interbase + 0x03)  //
#define cn_int_line_DMA_ch1                   (cn_int_line_interbase + 0x04)  //
#define cn_int_line_DMA_ch2                   (cn_int_line_interbase + 0x05)  //
#define cn_int_line_DMA_ch3                     (cn_int_line_interbase + 0x06)  //
#define cn_int_line_DMA_ch4                   (cn_int_line_interbase + 0x07)  //
#define cn_int_line_etsec1_g1_error           (cn_int_line_interbase + 0x08)  //
#define cn_int_line_etsec3_g3_transmit        (cn_int_line_interbase + 0x09)  //
#define cn_int_line_etsec3_g3_receive         (cn_int_line_interbase + 0x0A)  //
#define cn_int_line_etsec3_g1_error              (cn_int_line_interbase + 0x0B)  //
#define cn_int_line_usb1                      (cn_int_line_interbase + 0x0C)  //
#define cn_int_line_etsec1_g0_transmit        (cn_int_line_interbase + 0x0D)  //
#define cn_int_line_etsec1_g0_receive         (cn_int_line_interbase + 0x0E)  //
#define cn_int_line_etsec3_g0_transmit        (cn_int_line_interbase + 0x0F)  //
#define cn_int_line_etsec3_g0_receive         (cn_int_line_interbase + 0x10)  //
#define cn_int_line_etsec3_g0_error           (cn_int_line_interbase + 0x11)  //
#define cn_int_line_etsec1_g0_error           (cn_int_line_interbase + 0x12)  //
#define cn_int_line_etsec2_g0_transmit        (cn_int_line_interbase + 0x13)  //
#define cn_int_line_etsec2_g0_receive         (cn_int_line_interbase + 0x14)  //
#define cn_int_line_reserverd_21              (cn_int_line_interbase + 0x15)  //
#define cn_int_line_reserverd_22              (cn_int_line_interbase + 0x16)  //
#define cn_int_line_reserverd_23              (cn_int_line_interbase + 0x17)  //
#define cn_int_line_etsec2_g0_error           (cn_int_line_interbase + 0x18)  //
#define cn_int_line_reserverd_25              (cn_int_line_interbase + 0x19)  //
#define cn_int_line_duart                     (cn_int_line_interbase + 0x1A)  //
#define cn_int_line_iic_controllers           (cn_int_line_interbase + 0x1B)  //
#define cn_int_line_performance_monitor       (cn_int_line_interbase + 0x1C)  //
#define cn_int_line_security_interrupt1       (cn_int_line_interbase + 0x1D)  //
#define cn_int_line_usb2                      (cn_int_line_interbase + 0x1E)  //
#define cn_int_line_gpio                       (cn_int_line_interbase + 0x1F)  //
#define cn_int_line_reserverd_32               (cn_int_line_interbase + 0x20)  //
#define cn_int_line_reserverd_33              (cn_int_line_interbase + 0x21)  //
#define cn_int_line_reserverd_34               (cn_int_line_interbase + 0x22)  //
#define cn_int_line_etsec2_g1_transmit        (cn_int_line_interbase + 0x23)  //
#define cn_int_line_etsec2_g1_receive         (cn_int_line_interbase + 0x24)  //
#define cn_int_line_reserverd_37                (cn_int_line_interbase + 0x25)  //
#define cn_int_line_reserverd_38                (cn_int_line_interbase + 0x26)  //
#define cn_int_line_reserverd_39                (cn_int_line_interbase + 0x27)  //
#define cn_int_line_reserverd_40                 (cn_int_line_interbase + 0x28)  //
#define cn_int_line_reserverd_41              (cn_int_line_interbase + 0x29)  //
#define cn_int_line_security_interrupt2       (cn_int_line_interbase + 0x2A)  //
#define cn_int_line_espi                      (cn_int_line_interbase + 0x2B)  //
#define cn_int_line_reserverd_44               (cn_int_line_interbase + 0x2C)  //
#define cn_int_line_reserverd_45                 (cn_int_line_interbase + 0x2D)  //
#define cn_int_line_TDM                       (cn_int_line_interbase + 0x2E)  //
#define cn_int_line_TDM_error                  (cn_int_line_interbase + 0x2F)  //
#define cn_int_line_reserverd_48              (cn_int_line_interbase + 0x30)  //
#define cn_int_line_reserverd_49              (cn_int_line_interbase + 0x31)  //
#define cn_int_line_reserverd_50              (cn_int_line_interbase + 0x32)  //
#define cn_int_line_etsec2_g1_error           (cn_int_line_interbase + 0x33)  //
#define cn_int_line_etsec1_1588timer          (cn_int_line_interbase + 0x34)  //
#define cn_int_line_etsec2_1588timer          (cn_int_line_interbase + 0x35)  //
#define cn_int_line_etsec3_1588timer            (cn_int_line_interbase + 0x36)  //
#define cn_int_line_reserverd_55              (cn_int_line_interbase + 0x37)  //
#define cn_int_line_eSDHC                      (cn_int_line_interbase + 0x38)  //
#define cn_int_line_reserverd_57              (cn_int_line_interbase + 0x39)  //
#define cn_int_line_reserverd_58              (cn_int_line_interbase + 0x3A)  //
#define cn_int_line_reserverd_59                 (cn_int_line_interbase + 0x3B)  //
#define cn_int_line_reserverd_60              (cn_int_line_interbase + 0x3C)  //
#define cn_int_line_reserverd_61              (cn_int_line_interbase + 0x3D)  //
#define cn_int_line_reserverd_62              (cn_int_line_interbase + 0x3E)  //
#define cn_int_line_reserverd_63                (cn_int_line_interbase + 0x3F)  //
#define CN_INT_LINE_LAST                      (cn_int_line_interbase + 0x3F)

//list the int destination type,some int may be the combination
#define cn_enum_int2ep      (0x01<<31)
#define cn_enum_int2cint0   (0x01<<30)
#define cn_enum_int2cint1   (0x01<<29)
#define cn_enum_int2int0    (0x01<<0)
#define cn_enum_int2int1    (0x01<<1)





#endif //__CPU_PERI_INT_LINE_H__

