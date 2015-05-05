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
//========================================================  
// 文件模块说明:                                            
// omapl138定时器操作函数                         
// 文件版本: v1.00                                          
// 开发人员: lst                                            
// 创建时间: 2011.06.01                                     
// Copyright(c) 2011-2011  深圳鹏瑞软件有限公司            
//========================================================  
// 程序修改记录(最新的放在最前面):							
//  <版本号> <修改日期>, <修改人员>: <修改功能概述>			
//========================================================  
//例如：
// V1.02 2004-07-14, 张三: 接收函数增加对报文长度有效性校核

#ifndef __OMAPL138_TIMER_H__
#define __OMAPL138_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

//timer0:0x01c20000
//timer1:0x01c21000
//timer2:0x01f0c000
//timer3:0x01f0d000
//特别注意:L138的timer相关引脚名称中的out和in没有意义，引脚究竟是输入还是输出由
//GPDATA_GPDIR寄存器确定。比如TM64P_OUT34引脚和TM64P_IN34引脚，复位后都是输入脚，
//可由GPDATA_GPDIR控制这两引脚为输入或输出
typedef struct
{
    vu32 REV;             // 0x0000    0x44720211
    vu32 EMUMGT;          // 0x0004 bit0=0使能bit1控制仿真模式下
                                       // timer运行方式。bit1=0停止，=1到期停止
    vu32 GPINT_GPEN;      // 0x0008 timer外部引脚工作模式
    vu32 GPDATA_GPDIR;    // 0x000C timer外部引脚工作模式
    vu32 TIM12;           // 0x0010 加计数器
    vu32 TIM34;           // 0x0014 加计数器
    vu32 PRD12;           // 0x0018 周期寄存器
    vu32 PRD34;           // 0x001C 周期寄存器
    vu32 TCR;             // 0x0020 
    vu32 TGCR;            // 0x0024
    vu32 WDTCR;           // 0x0028
    vu32 RSVD0[2];        // 0x002C
    vu32 REL12;           // 0x0034 重载寄存器
    vu32 REL34;           // 0x0038 重载寄存器
    vu32 CAP12;           // 0x003C 捕获寄存器
    vu32 CAP34;           // 0x0040 捕获寄存器
    vu32 INTCTLSTAT;      // 0x0044
    vu32 RSVD1[6];        // 0x0048
    vu32 CMP0;            // 0x0060
    vu32 CMP1;            // 0x0064
    vu32 CMP2;            // 0x0068
    vu32 CMP3;            // 0x006C
    vu32 CMP4;            // 0x0070
    vu32 CMP5;            // 0x0074
    vu32 CMP6;            // 0x0078
    vu32 CMP7;            // 0x007C
}tagTimerReg;

// bitmask defines for GPINT_GPEN.
#define GPENO34         (0x02000000)   // bit 25
#define GPENI34         (0x01000000)   // bit 24
#define GPENO12         (0x00020000)   // bit 17
#define GPENI12         (0x00010000)   // bit 16
#define GPINT34INVO     (0x00002000)   // bit 13
#define GPINT34INVI     (0x00001000)   // bit 12
#define GPINT34ENO      (0x00000200)   // bit 9
#define GPINT34ENI      (0x00000100)   // bit 8
#define GPINT12INVO     (0x00000020)   // bit 5
#define GPINT12INVI     (0x00000010)   // bit 4
#define GPINT12ENO      (0x00000002)   // bit 1
#define GPINT12ENI      (0x00000001)   // bit 0

// bitmask defines for GPDATA_GPDIR.
#define GPDIRO34        (0x02000000)   // bit 25
#define GPDIRI34        (0x01000000)   // bit 24
#define GPDIRO12        (0x00020000)   // bit 17
#define GPDIRI12        (0x00010000)   // bit 16
#define GPDATAO34       (0x00000200)   // bit 9
#define GPDATAI34       (0x00000100)   // bit 8
#define GPDATAO12       (0x00000002)   // bit 1
#define GPDATAI12       (0x00000001)   // bit 0

// bitmask defines for TCR.
#define ENAMODE34_OFF   22          //t34的工作模式,2bit
#define ENAMODE12_OFF   6           //t12的工作模式,2bit

#define CLKSRC34_OFF    24          //t34的时钟源,1bit
#define CLKSRC12_64_OFF 8           //t12或64位timer模式的时钟源,1bit

#define EVTINTEN34      (1<<18)     //t34事件捕获中断
#define PRDINTEN34      (1<<16)     //t34周期匹配中断
#define EVTINTEN12      (1<<2)      //t12事件捕获中断
#define PRDINTEN12      (1<<0)      //t12周期匹配中断

void timer_counter_len_set(u32 t64_no,ufast_t mode);
void timer_int_ctrl(u32 t64_no,u32 msk,bool_t en);
void timer_set_clksrc(u32 t64_no,u32 sub,u32 src);
void timer_set_mode(u32 t64_no,u32 sub,u32 mode);
void timer_set_Period(u32 t64_no,u32 sub,u32 value);
void timer_set_reload(u32 t64_no,u32 sub,u32 value);
void Timer_Stop(u32 t64_no,u32 sub);
void Timer_Start(u32 t64_no,u32 sub);
void timer_clear(u32 t64_no,u32 sub);
u32 Timer_Read(u32 t64_no,u32 sub);

#ifdef __cplusplus
}
#endif

#endif //__OMAPL138_TIMER_H__
