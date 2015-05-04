//----------------------------------------------------
//Copyright (C), 2004-2009,  luoshitian.
//版权所有 (C), 2004-2009,   罗侍田.
//所属模块: CPU外设定义
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 包含CPU所有外设的定义，
//      1、不包含内核相关的固定的设备，比如m3的nvic等，所有m3内核均一致。
//      2、不包含内核相关的可选配但固定的定义，比如m3的mpu部件，不一定所有m3内核
//         的芯片都有，但只有有无的区别，不存在变化。
//         以上两部分在相应的内核目录下定义。
//      3、包含内核相关的可变部件定义
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __CPU_PERI_H__
#define __CPU_PERI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "cpu_peri_gpio.h"
#include "cpu_peri_int_line.h"
#include "cpu_peri_sysctrl.h"
#include "cpu_peri_wdt.h"
#include "cpu_peri_uart.h"


extern struct st_pin_config_reg volatile * const pg_pin_config_reg;
extern struct tagGPIO_StReg volatile * const pg_gpio0_reg;
extern struct tagGPIO_StReg volatile * const pg_gpio1_reg;
extern struct tagGPIO_StReg volatile * const pg_gpio2_reg;
extern struct tagGPIO_StReg volatile * const pg_gpio4_reg;
extern struct st_sysctrl_reg volatile * const pg_sysctrl_reg;
extern struct st_wdt_reg volatile * const pg_wdt_reg;
extern struct st_nxp_uart_reg volatile * const pg_uart0_reg;
extern struct st_nxp_uart_reg volatile * const pg_uart1_reg;
extern struct st_nxp_uart_reg volatile * const pg_uart2_reg;
extern struct st_nxp_uart_reg volatile * const pg_uart3_reg;

#ifdef __cplusplus
}
#endif

#endif //__CPU_PERI_H__

