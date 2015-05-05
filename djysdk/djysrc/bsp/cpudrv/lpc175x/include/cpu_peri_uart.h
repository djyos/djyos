#ifndef __CPU_PERI_UART__
#define __CPU_PERI_UART__

#ifdef __cplusplus
extern "C" {
#endif

//寄存器位偏移定义
//中断使能寄存器位ier偏移定义
#define bo_uart_ier_rbr     0   //1=允许接收中断和接收超时中断
#define bo_uart_ier_thre    1   //1=允许发送中断
#define bo_uart_ier_rxl     2   //1=允许接收线状态中断，状态可从lsr4:1读出

//中断标识寄存器iir位偏移定义
#define bo_uart_iir_all     0   //0=至少有一个中断挂起,1=无中断挂起
#define bm_uart_iir_id      0xe0//1=发送中断，2=接收中断，3=线状态中断，6=超时
#define bo_uart_iir_id      1   //标识挂起的中断

//fifo控制寄存器fcr位偏移定义
#define bo_uart_fcr_ffen    0   //1=使能fifo，并使本寄存器其他位可访问。
#define bo_uart_fcr_rffrs   1   //1=复位rx fifo，复位后由硬件清零本位
#define bo_uart_fcr_tffrs   2   //1=复位tx fifo，复位后由硬件清零本位
#define bm_uart_fcr_fflevel 0xc0//fifo触发水平，0=1字符，1=4字符，2=8字符，3=14
#define bo_uart_fcr_fflevel 6   //收发fifo触发水平选择

//线状态控制寄存器lcr位偏移定义
#define bm_uart_lcr_bw      0x3 //字符长度，0~3=5~8位
#define bo_uart_lcr_bw      0   //字符长度位偏移
#define bo_uart_lcr_st      2   //0=1位停止位，1=2位停止位，若5位字符则1.5位
#define bo_uart_lcr_pe      3   //0=禁止奇偶校验，1=使能奇偶校验
#define bm_uart_lcr_p       0x30//奇偶校验，0=奇，1=偶，2=强制1，3=强制0
#define bo_uart_lcr_p       4   //奇偶校验选择
#define bo_uart_lcr_lbc     6   //间断发送控制，0=正常发送，1=强制发送0
#define bo_uart_lcr_dlab    7   //1=选择baud寄存器(baud与其他寄存器地址相同)

//线状态寄存器lsr寄存器位偏移定义
#define bo_uart_lsr_rdr     0   //1=接收就绪
#define bo_uart_lsr_oe      1   //1=接收溢出，读lsr则清除
#define bo_uart_lsr_pe      2   //1=产生奇偶错误
#define bo_uart_lsr_fe      3   //1=发生帧错误
#define bo_uart_lsr_bi      4   //1=检测到间断发送信号
#define bo_uart_lsr_thre    5   //1=发送fifo空
#define bo_uart_lsr_temt    6   //1=发送移位器空
#define bo_uart_lsr_rsfe    7   //1=rx fifo中有一个接收错误字符

//发送使能寄存器ter位偏移定义
#define bo_uart_ter_txen    7   //1=允许发送，0=停止发送

//注：1.不同的CPU，uart寄存器基址不同。
//    2.不是所有cpu都支持所有寄存器，参看所用的器件。
struct st_nxp_uart_reg
{
  union {                   //0x00
      vu32  RBR;                //dlab=0 接收fifo地址，thr是，
      vu32  THR;                //dlab=0 发送fifo地址
      vu32  DLL;                //dlab=1 baud除数低字节
  }RTD;
  union {                   //0x04
      vu32  DLM;                //dlab=1 baud除数高字节
      vu32  IER;                //dlab=0 中断使能寄存器
  }DI;
  union {                   //offset = 0x08
      vu32  IIR;                //中断id寄存器
      vu32  FCR;                //fifo控制寄存器
  }IF;
  vu32  LCR;                //0x0c，线控制寄存器
  vu32  MCR;                //0x010 Modem control Register (R/W)
  vu32  LSR;                //0x014 线状态寄存器
  vu32  MSR;                //0x018 Modem status Register (R/ )
  vu32  SCR;                //0x01C 高速缓存
  vu32  ACR;                //0x020 Auto-baud Control Register (R/W)
  vu32  ICR;                //0x024 IrDA Control Register (R/W) */      
  vu32  FDR;                //0x028 Fractional Divider Register (R/W)
  vu32  RESERVED0;
  vu32  TER;                //0x030 发送使能
  vu32  RESERVED1[6];
  vu32  RS485CTRL;          //0x04C RS-485/EIA-485 Control Register (R/W)
  vu32  ADRMATCH;           //0x050 RS-485/EIA-485 address match Register (R/W)
  vu32  RS485DLY;           //0x054 RS-485/EIA-485 direction control delay Register (R/W)
  vu32  FIFOLVL;            //0x058 FIFO Level Register (R/ )
};

ptu32_t module_init_uart0(ptu32_t para);
ptu32_t module_init_uart1(ptu32_t para);
ptu32_t module_init_uart2(ptu32_t para);
ptu32_t module_init_uart3(ptu32_t para);


#ifdef __cplusplus
}
#endif
#endif /* __CPU_PERI_UART__ */
