//----------------------------------------------------
//Copyright (C), 2005-2009,  lst.
//版权所有 (C), 2005-2009,   lst.
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

//定义IO模式
#define cn_gpio_mode_od_pullup          0x00    //开漏，上拉
#define cn_gpio_mode_od_repeater        0x01    //开漏，中继(自适应)
#define cn_gpio_mode_od_nopull          0x02    //开漏，无上下拉
#define cn_gpio_mode_od_pulldown        0x03    //开漏，下拉
#define cn_gpio_mode_nood_pullup        0x04    //无开漏，上拉
#define cn_gpio_mode_nood_repeater      0x05    //无开漏，中继(自适应)
#define cn_gpio_mode_nood_nopull        0x06    //无开漏，无上下拉
#define cn_gpio_mode_nood_pulldown      0x07    //无开漏，下拉

//以下定义全部gpio的功能
#define	cn_p0_0_gpio             0x00      // gpio
#define cn_p0_0_rd1              0x01      // can-1控制器接收引脚
#define cn_p0_0_txd3             0x02      // uart3发送引脚
#define cn_p0_0_sda1             0x03      // i2c-1数据线（开漏）

#define cn_p0_1_gpio             0x00      // gpio
#define cn_p0_1_td1              0x01      // can-1控制器发送引脚
#define cn_p0_1_rxd3             0x02      // uart3接收引脚
#define cn_p0_1_scl1             0x03      // i2c-1时钟线（开漏）

#define cn_p0_2_gpio             0x00      // gpio
#define cn_p0_2_txd0             0x01      // uart0发送引脚
#define cn_p0_2_ad07             0x02      // ad0.7引脚
//...   reserved              0x03

#define cn_p0_3_gpio             0x00      // gpio
#define cn_p0_3_rxd0             0x01      // uart0接收引脚
#define cn_p0_3_ad06             0x02      // ad0.6引脚
//...   reserved              0x03

#define cn_p0_4_gpio             0x00      // gpio
#define cn_p0_4_i2srx_clk        0x01      // i2s总线接收时钟sck
#define cn_p0_4_rd2              0x02      // can-2接收线
#define cn_p0_4_cap20            0x03      // 定时器2捕获输入，通道0

#define cn_p0_5_gpio             0x00      // gpio
#define cn_p0_5_i2srx_ws         0x01      // i2s总线接收字选择ws
#define cn_p0_5_td2              0x02      // can-2发送线
#define cn_p0_5_cap21            0x03      // 定时器2捕获输入，通道1

#define cn_p0_6_gpio             0x00      // gpio
#define cn_p0_6_i2srx_sda        0x01      // i2s总线接收数据线sd
#define cn_p0_6_ssel1            0x02      // ssp-1总线从机选择
#define cn_p0_6_mat20            0x03      // 定时器2匹配输出，通道0

#define cn_p0_7_gpio             0x00      // gpio
#define cn_p0_7_i2stx_clk        0x01      // i2s总线发送时钟线sck
#define cn_p0_7_ssck1            0x02      // ssp-1时钟线
#define cn_p0_7_mat21            0x03      // 定时器2匹配输出，通道1

#define cn_p0_8_gpio             0x00      // gpio
#define cn_p0_8_i2stx_ws         0x01      // i2s总线发送字选择ws
#define cn_p0_8_smiso1           0x02      // ssp-1主输入从输出数据线
#define cn_p0_8_mat22            0x03      // 定时器2匹配输出，通道2

#define cn_p0_9_gpio             0x00      // gpio
#define cn_p0_9_i2stx_sda        0x01      // i2s总线发送数据线sd
#define cn_p0_9_smosi1           0x02      // ssp-1主输出从输入数据线
#define cn_p0_9_mat23            0x03      // 定时器2匹配输出，通道3

#define cn_p0_10_gpio            0x00      // gpio
#define cn_p0_10_txd2            0x01      // uart2发送引脚
#define cn_p0_10_sda2            0x02      // i2c-2数据线(非开漏引脚)
#define cn_p0_10_mat30           0x03      // 定时器3匹配输出，通道0

#define cn_p0_11_gpio            0x00      // gpio
#define cn_p0_11_rxd2            0x01      // uart2接收引脚
#define cn_p0_11_scl2            0x02      // i2c-2时钟线(非开漏引脚)
#define cn_p0_11_mat31           0x03      // 定时器3匹配输出，通道1

#define cn_p0_15_gpio            0x00      // gpio
#define cn_p0_15_txd1            0x01      // uart1发送引脚
#define cn_p0_15_sck0            0x02      // ssp0时钟线
#define cn_p0_15_sck             0x03      // spi时钟线

#define cn_p0_16_gpio            0x00      // gpio
#define cn_p0_16_rxd1            0x01      // uart1接收引脚
#define cn_p0_16_ssel0           0x02      // ssp0从机选择
#define cn_p0_16_ssel            0x03      // spi从机选择

#define cn_p0_17_gpio            0x00      // gpio
#define cn_p0_17_cts1            0x01      // uart1清零发送信号cts
#define cn_p0_17_miso0           0x02      // ssp0主输入从输出数据线
#define cn_p0_17_miso            0x03      // spi主入从出数据线

#define cn_p0_18_gpio            0x00      // gpio
#define cn_p0_18_dcd1            0x01      // uart1数据载波检测输入dcd
#define cn_p0_18_mosi0           0x02      // ssp-0主输出从输入数据线
#define cn_p0_18_mosi            0x03      // spi主出从入数据线

#define cn_p0_19_gpio            0x00      // gpio
#define cn_p0_19_dsr1            0x01      // uart1数据设置就绪dsr
//...   reserved              0x02      // 保留
#define cn_p0_19_sda1            0x03      // i2c-1数据线（非开漏）

#define cn_p0_20_gpio            0x00      // gpio
#define cn_p0_20_dtr1            0x01      // uart1终端就绪dtr
//...   reserved              0x02                                  	/* 保留
#define cn_p0_20_scl1            0x03      // i2c-1时钟线（非开漏）

#define cn_p0_21_gpio            0x00      // gpio
#define cn_p0_21_ri1             0x01      // uart1振铃信号
//...   reserved              0x02      // 保留
#define cn_p0_21_rd1             0x03      // can-1接收引脚

#define cn_p0_22_gpio            0x00      // gpio
#define cn_p0_22_rts1            0x01      // uart1请求发送引脚
//...   reserved              0x02      // 保留
#define cn_p0_22_td1             0x03      // can-1发送线

#define cn_p0_23_gpio            0x00      // gpio
#define cn_p0_23_ad00            0x01      // adc-0，通道0
#define cn_p0_23_i2srx_clk       0x02      // i2s接收时钟sck
#define cn_p0_23_cap30           0x03      // 定时器3捕获输入，通道0

#define cn_p0_24_gpio            0x00      // gpio
#define cn_p0_24_ad01            0x01      // adc-0，通道1
#define cn_p0_24_i2srx_ws        0x02      // i2s接收字选择ws
#define cn_p0_24_cap31           0x03      // 定时器3捕获输入，通道1

#define cn_p0_25_gpio            0x00      // gpio
#define cn_p0_25_ad02            0x01      // adc-0，通道2
#define cn_p0_25_i2srx_sda       0x02      // i2s接收数据sd
#define cn_p0_25_txd3            0x03      // uart3发送引脚

#define cn_p0_26_gpio            0x00      // gpio
#define cn_p0_26_ad03            0x01      // adc-0，通道3
#define cn_p0_26_aout            0x02      // dac输出
#define cn_p0_26_rxd3            0x03      // uart-3接收引脚

#define cn_p0_27_gpio            0x00      // gpio
#define cn_p0_27_sda0            0x01      // i2c0数据输入/输出，开漏
#define cn_p0_27_usb_sda         0x02      // usb接口i2c串行数据otg收发器
//..    reserved              0x03      // 保留

#define cn_p0_28_gpio            0x00      // gpio
#define cn_p0_28_scl0            0x01      // i2c0时钟输入，开漏
#define cn_p0_28_usb_scl         0x02      // usb接口i2c时钟线，otg收发器
//..    reserved              0x03                                   /* 保留

#define cn_p0_29_gpio            0x00      // gpio
#define cn_p0_29_usb_dd          0x01      // usb双向数据d+线
//..    reserved              0x02      // 保留
//..    reserved              0x03      // 保留

#define cn_p0_30_gpio            0x00      // gpio
#define cn_p0_30_usb_ds          0x01      // usb双向数据d-线
//..    reserved              0x02      // 保留
//..    reserved              0x03      // 保留

//p1口引脚功能设置
#define cn_p1_0_gpio             0x00      // gpio
#define cn_p1_0_enet_txd0        0x01      // 以太网发送数据脚0
//..    reserved              0x02      // 保留
//..    reserved              0x03      // 保留

#define cn_p1_1_gpio             0x00      // gpio
#define cn_p1_1_enet_tx_en       0x01      // 以太网发送数据脚1
//..    reserved              0x02      // 保留
//..    reserved              0x03      // 保留

#define cn_p1_4_gpio             0x00      // gpio
#define cn_p1_4_enet_tx_en       0x01      // 以太网发送使能
//..    reserved              0x02      // 保留
//..    reserved              0x03      // 保留

#define cn_p1_8_gpio             0x00      // gpio
#define cn_p1_8_enet_crs         0x01      // 以太网载波侦测脚
//..    reserved              0x02      // 保留
//..    reserved              0x03      // 保留

#define cn_p1_9_gpio             0x00      // gpio
#define cn_p1_9_enet_rxd0        0x01      // 以太网接收数据脚0
//..    reserved              0x02      // 保留
//..    reserved              0x03      // 保留

#define cn_p1_10_gpio            0x00      // gpio
#define cn_p1_10_enet_rxd1       0x01      // 以太网接收数据脚1
//..    reserved              0x02      // 保留
//..    reserved              0x03      // 保留

#define cn_p1_14_gpio            0x00      //  gpio
#define cn_p1_14_enet_rx_er      0x01      // 以太网接收错误
//..    reserved              0x02      // 保留
//..    reserved              0x03      // 保留

#define cn_p1_15_gpio            0x00      // gpio
#define cn_p1_15_enet_ref_clk    0x01      // 以太网参拷时钟输入
//..    reserved              0x02      // 保留
//..    reserved              0x03      // 保留

#define cn_p1_16_gpio            0x00      // gpio
#define cn_p1_16_enet_mdc        0x01      // 以太网miim时钟
//..    reserved              0x02      // 保留
//..    reserved              0x03      // 保留

#define cn_p1_17_gpio            0x00      // gpio
#define cn_p1_17_enet_mdio       0x01      // 以太网mi数据输入\输出
//..    reserved              0x02      // 保留
//..    reserved              0x03      // 保留

#define cn_p1_18_gpio            0x00      //  gpio
#define cn_p1_18_usb_up_led      0x01      // usb goodlink指示脚
#define cn_p1_18_pwm11           0x02      // pwm1输出，通道1
#define cn_p1_18_cap10           0x03      // timer1捕获输入，通道0

#define cn_p1_19_gpio            0x00      //  gpio
#define cn_p1_19_mco0a           0x01      // 电机控制pwm通道0，输出a
#define cn_p1_19_usb_ppwr        0x02      // usb端口电源使能信号
#define cn_p1_19_cap11           0x03      // timer1捕获输入，通道1

#define cn_p1_20_gpio            0x00      //  gpio
#define cn_p1_20_mcfb0           0x01      // 电机控制pwm通道0，回环输入
#define cn_p1_20_pwm12           0x02      // pwm-2输出，通道2
#define cn_p1_20_sck0            0x03      // ssp-0时钟输入

#define cn_p1_21_gpio            0x00      // gpio
#define cn_p1_21_mcabort         0x01      // 电机控制pwm，紧急中止
#define cn_p1_21_pwm13           0x02      // pwm-1输出，通道3
#define cn_p1_21_ssel0           0x03      // ssp-0从机选择

#define cn_p1_22_gpio            0x00      // gpio
#define cn_p1_22_mc0b            0x01      // 电机控制pwm通道0，输出b
#define cn_p1_22_usb_pwrd        0x02      // usb端口的电源状态
#define cn_p1_22_mat10           0x03      // 定时器1匹配输出，通道0

#define cn_p1_23_gpio            0x00      // gpio
#define cn_p1_23_mcfb1           0x01      // 电机控制pwm通道1，反馈输入
#define cn_p1_23_pwm14           0x02      // pwm-1输出，通道4
#define cn_p1_23_miso0           0x03      // ssp-0主机输入，从机输出

#define cn_p1_24_gpio            0x00      // gpio
#define cn_p1_24_mcfb2           0x01      // 电机控制pwm通道2，反馈输入
#define cn_p1_24_pwm15           0x02      // pwm-1输出，通道5
#define cn_p1_24_mosi0           0x03      // ssp-0主机输出，从机输入

#define cn_p1_25_gpio            0x00      // gpio
#define cn_p1_25_mc1a            0x01      // 电机控制pwm通道1，输出a
#define cn_p1_25_clkout          0x02      // 时钟输出引脚
#define cn_p1_25_mat11           0x03      // 定时器1匹配输出，通道1

#define cn_p1_26_gpio            0x00      // gpio
#define cn_p1_26_mc1b            0x01      // 电机控制pwm通道1，输出b
#define cn_p1_26_pwm16           0x02      // pwm-1输出，通道6
#define cn_p1_26_cap00           0x03      // 定时器0捕获输入，通道0

#define cn_p1_27_gpio            0x00      // gpio
#define cn_p1_27_clkout          0x01      // 时钟输出引脚
#define cn_p1_27_usb_ovrcr       0x02      // usb端口过载电流状态
#define cn_p1_27_cap01           0x03      // 定时器0捕获输入，通道1

#define cn_p1_28_gpio            0x00      // gpio
#define cn_p1_28_mc2a            0x01      // 电机控制pwm通道2，输出a
#define cn_p1_28_pcap10          0x02      // pwm-1捕获输入，通道0
#define cn_p1_28_mat00           0x03      // 定时器0匹配输出，通道0

#define cn_p1_29_gpio            0x00      // gpio
#define cn_p1_29_mc2b            0x01      // 电机控制pwm通道2，输出b
#define cn_p1_29_pcap11          0x02      // pwm-1捕获输入，通道1
#define cn_p1_29_mat01           0x03      // 定时器0匹配输出，通道1

#define cn_p1_30_gpio            0x00      // gpio
//...   reserved              0x01      // 保留
#define cn_p1_30_vbus            0x02      // 存在的usb总线电源
#define cn_p1_30_ad04            0x03      // adc转换器0.通道4输入

#define cn_p1_31_gpio            0x00      // gpio
//...   reserved              0x01      // 保留
#define cn_p1_31_ssck1           0x02      // ssp-1时钟线
#define cn_p1_31_ad05            0x03      // adc-0输入，通道5

//p2口引脚功能设置
#define cn_p2_0_gpio             0x00      // gpio
#define cn_p2_0_pwm11            0x01      // pwm-1，通道1
#define cn_p2_0_txd1             0x02      // uart1数据发送引脚
//...   reserved              0x03      // 保留

#define cn_p2_1_gpio             0x00      // gpio
#define cn_p2_1_pwm12            0x01      // pwm-1，通道2
#define cn_p2_1_rxd1             0x02      // uart1数据接收引脚
//...   reserved              0x03      // 保留

#define cn_p2_2_gpio             0x00      // gpio
#define cn_p2_2_pwm13            0x01      // pwm-1，通道3
#define cn_p2_2_cts1             0x02      // uart1清零发送线
//...   reserved              0x03      // 保留

#define cn_p2_3_gpio             0x00      // gpio
#define cn_p2_3_pwm14            0x01      // pwm-1，通道4
#define cn_p2_3_dcd1             0x02      // uart1数据载波检测线
//...   reserved              0x03      // 保留

#define cn_p2_4_gpio             0x00      // gpio
#define cn_p2_4_pwm15            0x01      // pwm-1，通道5
#define cn_p2_4_dsr1             0x02      // uart1数据设置就绪线
//...   reserved              0x03      // 保留

#define cn_p2_5_gpio             0x00      // gpio
#define cn_p2_5_pwm16            0x01      // pwm-1，通道6
#define cn_p2_5_dtr1             0x02      // uart1数据终端就绪线
//...   reserved              0x03      // 保留

#define cn_p2_6_gpio             0x00      // gpio
#define cn_p2_6_pcap10           0x01      // pwm-1捕获输入，通道0
#define cn_p2_6_ri1              0x02      // uart1振铃输入
//...   reserved              0x03      // 保留

#define cn_p2_7_gpio             0x00      // gpio
#define cn_p2_7_rd2              0x01      // can-2接收线
#define cn_p2_7_rts1             0x02      // uart1请求发送输出
                                        // rs-485/eia-485输出使能信号
//...   reserved              0x03      // 保留

#define cn_p2_8_gpio             0x00      // gpio
#define cn_p2_8_td2              0x01      // can-2发送线
#define cn_p2_8_txd2             0x02      // uart2发送引脚
#define cn_p2_8_enet_mdc         0x03      // 以太网miim时钟

#define cn_p2_9_gpio             0x00      // gpio
#define cn_p2_9_u1connect        0x01      // usb1软连接控制
#define cn_p2_9_rxd2             0x02      // uart2接收引脚
#define cn_p2_9_enet_mdio        0x03      // 以太网miim数据输入和输出

#define cn_p2_10_gpio            0x00      // gpio
#define cn_p2_10_eint0           0x01      // 外部中断0输入
#define cn_p2_10_nmi             0x02      // 非屏蔽中断输入
//...   reserved              0x03      // 保留

#define cn_p2_11_gpio            0x00      // gpio
#define cn_p2_11_eint1           0x01      // 外部中断1输入
//...   reserved              0x02      // 保留
#define cn_p2_11_i2stx_clk       0x03      // i2s发送时钟sck

#define cn_p2_12_gpio            0x00      // gpio
#define cn_p2_12_eint2           0x01      // 外部中断2输入
//...   reserved              0x02      // 保留
#define cn_p2_12_i2stx_ws        0x03      // i2s发送字选择ws

#define cn_p2_13_gpio            0x00      // gpio
#define cn_p2_13_eint3           0x01      // 外部中断3输入
//...   reserved              0x02      // 保留
#define cn_p2_13_i2stx_sda       0x03      // i2s发送数据sd

//p3口引脚功能设置
#define cn_p3_25_gpio            0x00      //  gpio
//...   reserved              0x01      //  保留
#define cn_p3_25_mat00           0x02      //  定时器0匹配输出，通道0
#define cn_p3_25_pwm12           0x03      //  pwm-1输出，通道2

#define cn_p3_26_gpio            0x00      // gpio
#define cn_p3_26_stclk           0x01      // 系统节拍定时器时钟输入
#define cn_p3_26_mat01           0x02      // 定时器0匹配输出，通道1
#define cn_p3_26_pwm13           0x03      // pwm-1输出，通道3

//p4口引脚功能设置
#define cn_p4_28_gpio            0x00      // gpio
#define cn_p4_28_rx_mclk         0x01      // —i2s接收主机时钟
#define cn_p4_28_mat20           0x02      // 定时器2匹配输出，通道0
#define cn_p4_28_txd3            0x03      // uart3发送引脚

#define cn_p4_29_gpio            0x00      // gpio
#define cn_p4_29_tx_mclk         0x01      // i2s发送主机时钟
#define cn_p4_29_mat21           0x02      // 定时器2匹配输出，通道1
#define cn_p4_29_rxd3            0x03      // uart3接收引脚

//p10  the trace port interface unit (tpiu)
#define cn_tpiu_disable          0x00      //tpiu interface 禁止
#define cn_tpiu_enable           0x01      //tpiu interface 使能

//引脚配置寄存器，用于选择引脚功能、上下拉等模式
//address=0x4002c000
struct st_pin_config_reg
{
    vu32 pinsel0;
    vu32 pinsel1;
    vu32 pinsel2;
    vu32 pinsel3;
    vu32 pinsel4;
    vu32 pinsel5;
    vu32 pinsel6;
    vu32 pinsel7;
    vu32 pinsel8;
    vu32 pinsel9;
    vu32 pinsel10;
    u32 res[5];
    vu32 pinmode0;
    vu32 pinmode1;
    vu32 pinmode2;
    vu32 pinmode3;
    vu32 pinmode4;
    vu32 pinmode5;
    vu32 pinmode6;
    vu32 pinmode7;
    vu32 pinmode8;
    vu32 pinmode9;
    vu32 pinmode_od0;
    vu32 pinmode_od1;
    vu32 pinmode_od2;
    vu32 pinmode_od3;
    vu32 pinmode_od4;
    vu32 i2cpadcfg;
};

//gpio模块控制寄存器
//address:  p0 = 0x2009c000
//          p1 = 0x2009c020
//          p2 = 0x2009c040
//          p3 = 0x2009c080
struct tagGPIO_StReg
{
    vu32    dir;
    u32     res[3];
    vu32    mask;
    vu32    pin;
    vu32    setpin;
    vu32    clrpin;
};

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
