//----------------------------------------------------
//Copyright (C), 2004-2009,  luoshitian.
//版权所有 (C), 2004-2009,   罗侍田.
//所属模块: CPU外设定义
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 包含CPU所有外设的定义
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

//管脚配置外设寄存器
struct st_pin_config_reg volatile * const pg_pin_config_reg
                            = (struct st_pin_config_reg *)0x4002c000;

//gpio相关寄存器
struct tagGPIO_StReg volatile * const pg_gpio0_reg
                            = (struct tagGPIO_StReg *)0x2009c000;
struct tagGPIO_StReg volatile * const pg_gpio1_reg
                            = (struct tagGPIO_StReg *)0x2009c020;
struct tagGPIO_StReg volatile * const pg_gpio2_reg
                            = (struct tagGPIO_StReg *)0x2009c040;
struct tagGPIO_StReg volatile * const pg_gpio4_reg
                            = (struct tagGPIO_StReg *)0x2009c080;
//系统控制寄存器
struct st_sysctrl_reg volatile * const pg_sysctrl_reg
                            = (struct st_sysctrl_reg *)0x400fc000;
//看门狗寄存器
struct st_wdt_reg volatile * const pg_wdt_reg
                            = (struct st_wdt_reg *)0x40000000;
//串口寄存器
struct st_nxp_uart_reg volatile * const pg_uart0_reg
                        = (struct st_nxp_uart_reg *)0x4000c000;
struct st_nxp_uart_reg volatile * const pg_uart1_reg
                        = (struct st_nxp_uart_reg *)0x40010000;
struct st_nxp_uart_reg volatile * const pg_uart2_reg
                        = (struct st_nxp_uart_reg *)0x40098000;
struct st_nxp_uart_reg volatile * const pg_uart3_reg
                        = (struct st_nxp_uart_reg *)0x4009c000;

