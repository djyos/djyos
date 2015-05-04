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
#ifndef __CPU_PERI_INT_LINE__
#define __CPU_PERI_INT_LINE__

#ifdef __cplusplus
extern "C" {
#endif

#define cn_int_line_wwdg            0x00    // 16 wdt
#define cn_int_line_timer0          0x01    // 17 timer0
#define cn_int_line_timer1          0x02    // 18 timer1
#define cn_int_line_timer2          0x03    // 19 timer2
#define cn_int_line_timer3          0x04    // 20 timer3
#define cn_int_line_uart0           0x05    // 21 uart0
#define cn_int_line_uart1           0x06    // 22 uart1
#define cn_int_line_uart2           0x07    // 23 uart2
#define cn_int_line_uart3           0x08    // 24 uart3
#define cn_int_line_pwm1            0x09    // 25 pwm1
#define cn_int_line_i2c0            0x0A    // 26 i2c0
#define cn_int_line_i2c1            0x0B    // 27 i2c1
#define cn_int_line_i2c2            0x0C    // 28 i2c2
#define cn_int_line_spi             0x0D    // 29 spi
#define cn_int_line_ssp0            0x0E    // 30 ssp0
#define cn_int_line_ssp1            0x0F    // 31 ssp1
#define cn_int_line_pll0            0x10    // 32 pll0
#define cn_int_line_rtc             0x11    // 33 rtc
#define cn_int_line_eint0           0x12    // 34 eint0
#define cn_int_line_eint1           0x13    // 35 eint1
#define cn_int_line_eint2           0x14    // 36 eint2
#define cn_int_line_eint3_gpio      0x15    // 37 eint3
#define cn_int_line_adc             0x16    // 38 adc
#define cn_int_line_bod             0x17    // 39 bod
#define cn_int_line_usb             0x18    // 40 usb
#define cn_int_line_can             0x19    // 41 can
#define cn_int_line_gpdma           0x1A    // 42 gp dma
#define cn_int_line_i2s             0x1B    // 43 i2s
#define cn_int_line_ethernet        0x1C    // 44 ethernet
#define cn_int_line_rit             0x1D    // 45 repete interrupt timer
#define cn_int_line_mc              0x1E    // 46 motor control pwm
#define cn_int_line_qe              0x1F    // 47 quadrature encoder
#define cn_int_line_pll1            0x20    // 48 pll1
#define cn_int_line_usbact          0x21    // 49 usb_need_clk
#define cn_int_line_canact          0x22    // 50 can1wake, can2wake
#define CN_INT_LINE_LAST            0x22

#ifdef __cplusplus
}
#endif

#endif //__CPU_PERI_INT_LINE__

