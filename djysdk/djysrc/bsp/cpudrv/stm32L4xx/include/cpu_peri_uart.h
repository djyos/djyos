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
//所属模块: uart驱动程序
//作者：lst
//版本：V1.0.1
//文件描述: 驱动44b0的uart
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-03-10
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __CPU_PERI_UART_H__
#define __CPU_PERI_UART_H__

#ifdef __cplusplus
extern "C" {
#endif


//同步/异步串口位域，bo_为位偏移，1位的用位带地址，bb_前缀，多位用掩码，bm_前缀
//uart1 sr寄存器位定义
#define bb_uart1_sr_base   (0x42000000 + 0x13800*32)           //位带基址
#define bb_uart1_sr_pe     (*(vu32*)(bb_uart1_sr_base+4*0))
#define bb_uart1_sr_fe     (*(vu32*)(bb_uart1_sr_base+4*1))
#define bb_uart1_sr_ne     (*(vu32*)(bb_uart1_sr_base+4*2))
#define bb_uart1_sr_ore    (*(vu32*)(bb_uart1_sr_base+4*3))
#define bb_uart1_sr_idle   (*(vu32*)(bb_uart1_sr_base+4*4))
#define bb_uart1_sr_rxne   (*(vu32*)(bb_uart1_sr_base+4*5))
#define bb_uart1_sr_tc     (*(vu32*)(bb_uart1_sr_base+4*6))
#define bb_uart1_sr_txe    (*(vu32*)(bb_uart1_sr_base+4*7))
#define bb_uart1_sr_lbd    (*(vu32*)(bb_uart1_sr_base+4*8))
#define bb_uart1_sr_cts    (*(vu32*)(bb_uart1_sr_base+4*9))

//uart2 sr寄存器位定义
#define bb_uart2_sr_base   (0x42000000 + 0x4400*32)           //位带基址
#define bb_uart2_sr_pe     (*(vu32*)(bb_uart2_sr_base+4*0))
#define bb_uart2_sr_fe     (*(vu32*)(bb_uart2_sr_base+4*1))
#define bb_uart2_sr_ne     (*(vu32*)(bb_uart2_sr_base+4*2))
#define bb_uart2_sr_ore    (*(vu32*)(bb_uart2_sr_base+4*3))
#define bb_uart2_sr_idle   (*(vu32*)(bb_uart2_sr_base+4*4))
#define bb_uart2_sr_rxne   (*(vu32*)(bb_uart2_sr_base+4*5))
#define bb_uart2_sr_tc     (*(vu32*)(bb_uart2_sr_base+4*6))
#define bb_uart2_sr_txe    (*(vu32*)(bb_uart2_sr_base+4*7))
#define bb_uart2_sr_lbd    (*(vu32*)(bb_uart2_sr_base+4*8))
#define bb_uart2_sr_cts    (*(vu32*)(bb_uart2_sr_base+4*9))

//uart3 sr寄存器位定义
#define bb_uart3_sr_base   (0x42000000 + 0x4800*32)           //位带基址
#define bb_uart3_sr_pe     (*(vu32*)(bb_uart3_sr_base+4*0))
#define bb_uart3_sr_fe     (*(vu32*)(bb_uart3_sr_base+4*1))
#define bb_uart3_sr_ne     (*(vu32*)(bb_uart3_sr_base+4*2))
#define bb_uart3_sr_ore    (*(vu32*)(bb_uart3_sr_base+4*3))
#define bb_uart3_sr_idle   (*(vu32*)(bb_uart3_sr_base+4*4))
#define bb_uart3_sr_rxne   (*(vu32*)(bb_uart3_sr_base+4*5))
#define bb_uart3_sr_tc     (*(vu32*)(bb_uart3_sr_base+4*6))
#define bb_uart3_sr_txe    (*(vu32*)(bb_uart3_sr_base+4*7))
#define bb_uart3_sr_lbd    (*(vu32*)(bb_uart3_sr_base+4*8))
#define bb_uart3_sr_cts    (*(vu32*)(bb_uart3_sr_base+4*9))

//uart4 sr寄存器位定义
#define bb_uart4_sr_base    (0x42000000 + 0x4c00*32)           //位带基址
#define bb_uart4_sr_pe      (*(vu32*)(bb_uart4_sr_base+4*0))
#define bb_uart4_sr_fe      (*(vu32*)(bb_uart4_sr_base+4*1))
#define bb_uart4_sr_ne      (*(vu32*)(bb_uart4_sr_base+4*2))
#define bb_uart4_sr_ore     (*(vu32*)(bb_uart4_sr_base+4*3))
#define bb_uart4_sr_idle    (*(vu32*)(bb_uart4_sr_base+4*4))
#define bb_uart4_sr_rxne    (*(vu32*)(bb_uart4_sr_base+4*5))
#define bb_uart4_sr_tc      (*(vu32*)(bb_uart4_sr_base+4*6))
#define bb_uart4_sr_txe     (*(vu32*)(bb_uart4_sr_base+4*7))
#define bb_uart4_sr_lbd     (*(vu32*)(bb_uart4_sr_base+4*8))
#define bb_uart4_sr_cts     (*(vu32*)(bb_uart4_sr_base+4*9))

//uart5 sr寄存器位定义
#define bb_uart5_sr_base    (0x42000000 + 0x5000*32)           //位带基址
#define bb_uart5_sr_pe      (*(vu32*)(bb_uart5_sr_base+4*0))
#define bb_uart5_sr_fe      (*(vu32*)(bb_uart5_sr_base+4*1))
#define bb_uart5_sr_ne      (*(vu32*)(bb_uart5_sr_base+4*2))
#define bb_uart5_sr_ore     (*(vu32*)(bb_uart5_sr_base+4*3))
#define bb_uart5_sr_idle    (*(vu32*)(bb_uart5_sr_base+4*4))
#define bb_uart5_sr_rxne    (*(vu32*)(bb_uart5_sr_base+4*5))
#define bb_uart5_sr_tc      (*(vu32*)(bb_uart5_sr_base+4*6))
#define bb_uart5_sr_txe     (*(vu32*)(bb_uart5_sr_base+4*7))
#define bb_uart5_sr_lbd     (*(vu32*)(bb_uart5_sr_base+4*8))
#define bb_uart5_sr_cts     (*(vu32*)(bb_uart5_sr_base+4*9))

//uart1 cr1寄存器位定义
#define bb_uart1_cr1_base      (0x42000000 + 0x1380c*32)           //位带基址
#define bb_uart1_cr1_sbk       (*(vu32*)(bb_uart1_cr1_base+4*0))
#define bb_uart1_cr1_rwu       (*(vu32*)(bb_uart1_cr1_base+4*1))
#define bb_uart1_cr1_re        (*(vu32*)(bb_uart1_cr1_base+4*2))
#define bb_uart1_cr1_te        (*(vu32*)(bb_uart1_cr1_base+4*3))
#define bb_uart1_cr1_idleie    (*(vu32*)(bb_uart1_cr1_base+4*4))
#define bb_uart1_cr1_rxneie    (*(vu32*)(bb_uart1_cr1_base+4*5))
#define bb_uart1_cr1_tcie      (*(vu32*)(bb_uart1_cr1_base+4*6))
#define bb_uart1_cr1_txeie     (*(vu32*)(bb_uart1_cr1_base+4*7))
#define bb_uart1_cr1_peie      (*(vu32*)(bb_uart1_cr1_base+4*8))
#define bb_uart1_cr1_ps        (*(vu32*)(bb_uart1_cr1_base+4*9))
#define bb_uart1_cr1_pce       (*(vu32*)(bb_uart1_cr1_base+4*10))
#define bb_uart1_cr1_wake      (*(vu32*)(bb_uart1_cr1_base+4*11))
#define bb_uart1_cr1_m         (*(vu32*)(bb_uart1_cr1_base+4*12))
#define bb_uart1_cr1_ue        (*(vu32*)(bb_uart1_cr1_base+4*13))

//uart2 cr1寄存器位定义
#define bb_uart2_cr1_base      (0x42000000 + 0x440c*32)           //位带基址
#define bb_uart2_cr1_sbk       (*(vu32*)(bb_uart2_cr1_base+4*0))
#define bb_uart2_cr1_rwu       (*(vu32*)(bb_uart2_cr1_base+4*1))
#define bb_uart2_cr1_re        (*(vu32*)(bb_uart2_cr1_base+4*2))
#define bb_uart2_cr1_te        (*(vu32*)(bb_uart2_cr1_base+4*3))
#define bb_uart2_cr1_idleie    (*(vu32*)(bb_uart2_cr1_base+4*4))
#define bb_uart2_cr1_rxneie    (*(vu32*)(bb_uart2_cr1_base+4*5))
#define bb_uart2_cr1_tcie      (*(vu32*)(bb_uart2_cr1_base+4*6))
#define bb_uart2_cr1_txeie     (*(vu32*)(bb_uart2_cr1_base+4*7))
#define bb_uart2_cr1_peie      (*(vu32*)(bb_uart2_cr1_base+4*8))
#define bb_uart2_cr1_ps        (*(vu32*)(bb_uart2_cr1_base+4*9))
#define bb_uart2_cr1_pce       (*(vu32*)(bb_uart2_cr1_base+4*10))
#define bb_uart2_cr1_wake      (*(vu32*)(bb_uart2_cr1_base+4*11))
#define bb_uart2_cr1_m         (*(vu32*)(bb_uart2_cr1_base+4*12))
#define bb_uart2_cr1_ue        (*(vu32*)(bb_uart2_cr1_base+4*13))

//uart3 cr1寄存器位定义
#define bb_uart3_cr1_base      (0x42000000 + 0x480c*32)           //位带基址
#define bb_uart3_cr1_sbk       (*(vu32*)(bb_uart3_cr1_base+4*0))
#define bb_uart3_cr1_rwu       (*(vu32*)(bb_uart3_cr1_base+4*1))
#define bb_uart3_cr1_re        (*(vu32*)(bb_uart3_cr1_base+4*2))
#define bb_uart3_cr1_te        (*(vu32*)(bb_uart3_cr1_base+4*3))
#define bb_uart3_cr1_idleie    (*(vu32*)(bb_uart3_cr1_base+4*4))
#define bb_uart3_cr1_rxneie    (*(vu32*)(bb_uart3_cr1_base+4*5))
#define bb_uart3_cr1_tcie      (*(vu32*)(bb_uart3_cr1_base+4*6))
#define bb_uart3_cr1_txeie     (*(vu32*)(bb_uart3_cr1_base+4*7))
#define bb_uart3_cr1_peie      (*(vu32*)(bb_uart3_cr1_base+4*8))
#define bb_uart3_cr1_ps        (*(vu32*)(bb_uart3_cr1_base+4*9))
#define bb_uart3_cr1_pce       (*(vu32*)(bb_uart3_cr1_base+4*10))
#define bb_uart3_cr1_wake      (*(vu32*)(bb_uart3_cr1_base+4*11))
#define bb_uart3_cr1_m         (*(vu32*)(bb_uart3_cr1_base+4*12))
#define bb_uart3_cr1_ue        (*(vu32*)(bb_uart3_cr1_base+4*13))

//uart4 cr1寄存器位定义
#define bb_uart4_cr1_base       (0x42000000 + 0x4c0c*32)           //位带基址
#define bb_uart4_cr1_sbk        (*(vu32*)(bb_uart4_cr1_base+4*0))
#define bb_uart4_cr1_rwu        (*(vu32*)(bb_uart4_cr1_base+4*1))
#define bb_uart4_cr1_re         (*(vu32*)(bb_uart4_cr1_base+4*2))
#define bb_uart4_cr1_te         (*(vu32*)(bb_uart4_cr1_base+4*3))
#define bb_uart4_cr1_idleie     (*(vu32*)(bb_uart4_cr1_base+4*4))
#define bb_uart4_cr1_rxneie     (*(vu32*)(bb_uart4_cr1_base+4*5))
#define bb_uart4_cr1_tcie       (*(vu32*)(bb_uart4_cr1_base+4*6))
#define bb_uart4_cr1_txeie      (*(vu32*)(bb_uart4_cr1_base+4*7))
#define bb_uart4_cr1_peie       (*(vu32*)(bb_uart4_cr1_base+4*8))
#define bb_uart4_cr1_ps         (*(vu32*)(bb_uart4_cr1_base+4*9))
#define bb_uart4_cr1_pce        (*(vu32*)(bb_uart4_cr1_base+4*10))
#define bb_uart4_cr1_wake       (*(vu32*)(bb_uart4_cr1_base+4*11))
#define bb_uart4_cr1_m          (*(vu32*)(bb_uart4_cr1_base+4*12))
#define bb_uart4_cr1_ue         (*(vu32*)(bb_uart4_cr1_base+4*13))

//uart5 cr1寄存器位定义
#define bb_uart5_cr1_base       (0x42000000 + 0x500c*32)           //位带基址
#define bb_uart5_cr1_sbk        (*(vu32*)(bb_uart5_cr1_base+4*0))
#define bb_uart5_cr1_rwu        (*(vu32*)(bb_uart5_cr1_base+4*1))
#define bb_uart5_cr1_re         (*(vu32*)(bb_uart5_cr1_base+4*2))
#define bb_uart5_cr1_te         (*(vu32*)(bb_uart5_cr1_base+4*3))
#define bb_uart5_cr1_idleie     (*(vu32*)(bb_uart5_cr1_base+4*4))
#define bb_uart5_cr1_rxneie     (*(vu32*)(bb_uart5_cr1_base+4*5))
#define bb_uart5_cr1_tcie       (*(vu32*)(bb_uart5_cr1_base+4*6))
#define bb_uart5_cr1_txeie      (*(vu32*)(bb_uart5_cr1_base+4*7))
#define bb_uart5_cr1_peie       (*(vu32*)(bb_uart5_cr1_base+4*8))
#define bb_uart5_cr1_ps         (*(vu32*)(bb_uart5_cr1_base+4*9))
#define bb_uart5_cr1_pce        (*(vu32*)(bb_uart5_cr1_base+4*10))
#define bb_uart5_cr1_wake       (*(vu32*)(bb_uart5_cr1_base+4*11))
#define bb_uart5_cr1_m          (*(vu32*)(bb_uart5_cr1_base+4*12))
#define bb_uart5_cr1_ue         (*(vu32*)(bb_uart5_cr1_base+4*13))


typedef struct
{
  vu16 SR;      //状态寄存器，b31：10，保留，硬件强制为0
                //rc，b9：CTS，cts标志，0=cts无变化，1=cts有变化
                //rc，b8：LBD，LIN break检测标志，0=没有检测到LIN break，1=反之
                //r，b7，TXE，发送数据寄存器空，0 = 非空，1=空
                //rc，TC，b6，发送完成标志，0=未完成，1=完成
                //r，RXNE，b5，接收ready，0=非ready，1=ready
                //r，IDLE，b4，总线空闲标志，0=非空闲，1=空闲
                //r，ORE，b3，接收溢出标志，0=无溢出，1=溢出
                //r，NE，b2，噪声错误标志，0=有噪声，1=无噪声，读SR再读DR可清零
                //           本标志无中断，因为它与RXNE标志一起出现。
                //r，FE，b1，帧错误，与NE一样。
                //r，PE，b0，校验错误，0=无错，1=有错，读SR再读DR可清零
  u16 RESERVED0;
  vu16 DR;      //收发数据寄存器，低9位有效，一个地址的两个寄存器。
  u16 RESERVED1;
  vu16 BRR;     //Baud寄存器，16位定点数，低4位是小数，Baud = pclk/(16*BRR)
  u16 RESERVED2;
  vu16 CR1;     //bit31~14,保留，硬件强制为0
                //rw，UE，b13，使能位，0=禁止uart，1=允许
                //rw，M，b12，字长，0=8数据位，n个停止位，1=9数据位，1停止位
                //rw，WAKE，b11，多机通信唤醒方法，0=空闲总线，1=地址标记
                //rw，PCE，b10，校验使能，0=禁止，1=使能
                //rw，PS，b9，校验选择，0=偶校验，1=奇校验
                //rw，PEIE，b8，PE中断使能，控制RS寄存器中的PE位是否产生中断
                //                0=禁止，1=允许
                //rw，TXEIE，b7，同上，监视SR的TXE位
                //rw，TCIE，b6，同上，监视SR的TC位
                //rw，RXNEIE，b5，同上，监视SR的RXNE位
                //rw，IDLEIE，b4，同上，监视SR的IDLE位
                //rw，TE，b3，发送使能，0=禁止，1=允许
                //rw，RE，b2，接收使能，0=禁止，1=允许
                //rw，RWU，b1，进入静默模式，0=正常模式，1=静默模式，
                //    用于多机通信，必须先收1字节后才能进入静默模式
                //rw，SBK，b0，0=无动作，1=将要发送断开帧，用于LIN模式
  u16 RESERVED3;
  vu16 CR2;     //bit31~15,保留，硬件强制为0
                //rw，LINEN，b14，LIN模式使能，0=禁止，1=使能
                //rw，STOP，b13~12，stop位数，CR1中的M位为0才有效。
                //          00=1bit，01=0.5bit，10=2bit，11=1.5bit
                //rw，CLKEN，b11，时钟使能，0=SCLK引脚禁止，1=允许，用于同步通信
                //rw，CPOL，b10，SCLK上的时钟极性，0=总线空闲时低，1=高
                //rw，CPHA，b9，SCLK时钟相位，0=SCLK第一个边沿采样，1=第二边沿
                //rw，LBCL，b8，同步模式下，是否发送最后一个字节的MSB时钟脉冲
                //          0=不输出，1=输出
                //rw，b7，保留，强制0
                //rw，LBDIE，b6，LIN break检测中断使能，0=禁止，1=使能
                //rw，LBDL，b5，LIN break检测长度，0=10位，1=11位
                //rw，b4，保留，强制0
                //rw，ADD[3:0]，b3~0，多机通信模式下该USART节点的地址
  u16 RESERVED4;
  vu16 CR3;     //bit31~11,保留，硬件强制为0
                //rw，CTSIE，b10，CTS中断使能，0=禁止，1=SR中CTS置位就中断
                //rw，CTSE，b9，CTS硬件流控使能，0=禁止，1=使能
                //rw，RTSE，b8，RTS硬件流控使能，0=禁止，1=使能
                //rw，DMAT，b7，DMA使能发送，0=禁止，1=使能
                //rw，DMAR，b6，DMA使能接收，0=禁止，1=使能
                //rw，SCEN，b5，智能卡模式使能，0=禁止，1=使能
                //rw，NACK，b4，智能卡NACK使能，0=校验错误是不发NACK，1=反之
                //rw，HDSEL，b3，0=选择半双工选择，1=不选
                //rw，IRLP，b2，0=正常模式，1=红外低功耗模式
                //rw，IREN，b1，红外模式使能，0=禁止，1=使能
                //rw，EIE，b0，错误中断使能，0=禁止，1=使能
                //rw，
                //rw，
  u16 RESERVED5;
  vu16 GTPR;    //保护时间和预分频寄存器，用于智能卡模式
  u16 RESERVED6;
}tagUartReg;


#define CN_UART1    0
#define CN_UART2    1
#define CN_UART3    2
#define CN_UART4    3
#define CN_UART5    4
#define CN_UART6    5
#define CN_UART_NUM 6

ptu32_t ModuleInstall_UART(ptu32_t para);

#ifdef __cplusplus
}
#endif

#endif // __CPU_PERI_UART_H__

