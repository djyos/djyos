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
/*
@所属模块:调度器
@作者：lst
@版本：V1.0.0
@文件描述:调度器中与CPU直接相关的汇编代码,主要是异常表和线程切换。
@其他说明:
@修订历史:
@2. ...
@1. 日期: 2009-01-04
@   作者: lst
@   新版本号: V1.0.0
@   修改说明: 原始版本
@------------------------------------------------------
@ 注: psp并不保存在栈中，而是在vm->stack中!!
*/


.file   "cpus.S";

//#include "cpu_peri_int_line.h"
//#include "cpu_peri_address.h"
#include "def21469.h"
#include "lib_glob.h"
#include "sig_glob.h"

.extern  ___Djy_VmEngine;
.extern  _Int_ContactAsynSignal;
.extern  _Int_HalfEnableAsynSignal;

.extern ___backup_all_regs;
.extern ___restore_all_regs;


/*
@----计算内核时钟当前TCOUNT值与加载值之差-------------------------------------------------------------------
@功能:  计算内核时钟当前TCOUNT值与加载值之差
@参数:  无
@返回:  当前TCOUNT与TPERIOD加载值差值
@函数原型:u32 _asm_get_tick()
@-------------------------------------------------------------------------------
*/
.section/pm seg_pmco;
.global _asm_get_tick;
.extern _core_time;
_asm_get_tick:
  r1=TPERIOD;
  r2=TCOUNT;
  r0=r1-r2;
  FETCH_RETURN
      RETURN (DB);
      rframe;
      nop;
  _asm_get_tick.end:




/*
@----重置线程-------------------------------------------------------------------
@功能:  初始化线程的栈，并挂上线程执行函数，新创建线程时将会调用本函数
@参数:  thread_routine，被重置的线程函数
@       vm，线程指针
@返回:  初始化结束后的当前栈指针
@函数原型:void * __asm_reset_thread(void (*thread_routine)(struct event_script *),
@                                        struct  tagThreadVm  *vm)@
@-------------------------------------------------------------------------------
*/
.section/pm seg_pmco;
.global ___asm_reset_thread;
___asm_reset_thread:
   //首先将本汇编函数使用到的寄存器入栈保护
    modify(I7,-2);
    R1=I2; 
    DM(1,I7)=R1;
    R1=I13;
    DM(0,I7)=R1;
    I4=R8;     //根据ADSP21469参数传递规则，R8=stack                     	
    R2=DM(M6,I4);I2=R2;		//取虚拟机栈顶指针  根据虚拟机结构，第二个元素为存储栈顶指针
    //取得虚拟机栈顶指针后，即按照线程栈结构依次对栈中各个位置初始化
    R2=___Djy_VmEngine;	//取虚拟机引擎指针
    modify(I2,-130);			//先申请栈空间
    DM(130,I2)=R2;          //_djy_vm_engine入口作为PC地址，将PC地址存入软件栈，线程栈第一个位置存储PC
   // save r1, r2, i2, i13 at first
    DM(129,I2)=R1;
    DM(128,I2)=R2;
    R1=I2;
    DM(127,I2)=R1;
    DM(126,I2)=I13;
    R1=I6; 
    DM(125,I2)=R1;
  // Save STKYx USTAT1-4
   MODE2=0x42000027;
   nop;
   nop;
   R1=MODE2;
   DM(123,I2)=R1;
   STKYx=0x05400000;     //处理器上电后MODE2及STKYx的默认值
   nop;
   nop;
    R1=STKYx; 
    DM(122,I2)=R1;
    STKYy=0x5400000;
    nop;
    nop;
    R1=STKYy;
    DM(121,I2)=R1;
    R1 = USTAT1; 
    DM(120,I2)=R1;
    R1 = USTAT2; 
    DM(119,I2)=R1;
    R1 = USTAT3; 
    DM(118,I2)=R1;
    R1 = USTAT4; 
    DM(117,I2)=R1;
    
       // Save and zero all the relevant L registers
        R1=L0; DM(116,I2)=R1;
        R1=L1; DM(115,I2)=R1;
        R1=L2; DM(114,I2)=R1;
        R1=L3; DM(113,I2)=R1;
        R1=L5; DM(112,I2)=R1;
        
        DM(111,I2)=L8;
        DM(110,I2)=L9;
        DM(109,I2)=L10;
        DM(108,I2)=L11;
        DM(107,I2)=L13;
        DM(106,I2)=L14;
        DM(105,I2)=L15;
      
        L0=0;
        L1=0;
        L2=0;
        L3=0;
        // L4 saved below
        L5=0;
        L8=0;
        L9=0;
        L10=0;
        L11=0;
        // L12 saved below
        L13=0;
        L14=0;
        L15=0;
        
       // Save PX registers
       DM(104,I2)=PX1;
       DM(103,I2)=PX2;
// All 40 bits of these registers must be saved. They are used in
// the integer divide routine and loss of accuracy in these registers
// will result in the wrong answer being generated.
        PX=R0; DM(102,I2)=PX1;DM(101,I2)=PX2;
        PX=R3; DM(100,I2)=PX1;DM(99,I2)=PX2;
        PX=R4; DM(98,I2)=PX1;DM(97,I2)=PX2;
        PX=R7; DM(96,I2)=PX1;DM(95,I2)=PX2;
        PX=R8; DM(94,I2)=PX1;DM(93,I2)=PX2;
        PX=R9; DM(92,I2)=PX1;DM(91,I2)=PX2;
        PX=R11;DM(90,I2)=PX1;DM(89,I2)=PX2;
        PX=R12;DM(88,I2)=PX1;DM(87,I2)=PX2;
		// R1\R2 saved below        
       DM(86,I2)=R5;
       DM(85,I2)=R6;
	   DM(84,I2)=R10;
       DM(83,I2)=R13;
       DM(82,I2)=R14;
       DM(81,I2)=R15;	 
// Save all of the Sregs. We can't do these saves and restores in
// SIMD mode in case the stack is in external memory.
// Note that on 2116x and above, we no longer save MODE1 and ASTAT
// - they are saved by the 'push sts' instruction in the vector table.
        PX=S0; DM(80,I2)=PX1;DM(79,I2)=PX2;
        PX=S4; DM(78,I2)=PX1;DM(77,I2)=PX2;
        PX=S9; DM(76,I2)=PX1;DM(75,I2)=PX2;
        PX=S11;DM(74,I2)=PX1;DM(73,I2)=PX2;
        PX=S12;DM(72,I2)=PX1;DM(71,I2)=PX2;
        
        DM(70,I2)=S1;
        DM(69,I2)=S2;
        DM(68,I2)=S3;
        DM(67,I2)=S5;
        DM(66,I2)=S6;
        DM(65,I2)=S7;
        DM(64,I2)=S8;
        DM(63,I2)=S10;
        DM(62,I2)=S13;
        DM(61,I2)=S14;
        DM(60,I2)=S15;
        
        BIT SET MODE1 RND32;
        nop;nop;

// Save the relevant DAG1 M registers - M0-3. M4 is saved later, M5-7 are
// constant.
        R1=M0;
        DM(59,I2)=R1;
        R1=M1;
        DM(58,I2)=R1;
        R1=M2;
        DM(57,I2)=R1;
        R1=M3;
        DM(56,I2)=R1;
       
// Save the relevant DAG2 M registers - M8-11. M12 is saved later, M13-15
// are constant.
        DM(55,I2)=M8;
        DM(54,I2)=M9;
        DM(53,I2)=M10;
        DM(52,I2)=M11;

// Save the Iregs
        R1=I0;
        DM(51,I2)=R1;
        R1=I1;
        DM(50,I2)=R1;
        R1=I3;
        DM(49,I2)=R1;
        R1=I5;
        DM(48,I2)=R1;
        
        DM(47,I2)=I8;
        DM(46,I2)=I9;
        DM(45,I2)=I10;
        DM(44,I2)=I11;
        DM(43,I2)=I14;
        DM(42,I2)=I15; 
       
// Save the scratch DAG registers on the stack.
        DM(41,I2)=pm_ptr;   // save i12
        DM(40,I2)=pm_bse;   // save b12
        DM(39,I2)=pm_mdf;   // save m12
        DM(38,I2)=pm_lnt;   // save l12

        R0=dm_ptr;
        DM(37,I2)=R0;        // save i4
        R0=dm_bse;
        DM(36,I2)=R0;        // save b4
        R0=dm_mdf; 
        DM(35,I2)=R0;        // save m4
        R0=dm_lnt;
        DM(34,I2)=R0;        // save l4
            
        //save B registers
        R1=B0;   DM(33,I2)=R0;   
        R1=B1;   DM(32,I2)=R0;   
        R1=B2;   DM(31,I2)=R0;   
        R1=B3;   DM(30,I2)=R0;   
        R1=B5;   DM(29,I2)=R0;   
        
        DM(28,I2)=B8; 
        DM(27,I2)=B9; 
        DM(26,I2)=B10;
        DM(25,I2)=B11;
        DM(24,I2)=B13;
        DM(23,I2)=B14;
        DM(22,I2)=B15;
            
      
 // 至此上下文中通用寄存器区复位完毕，接下来将硬件栈中ASTAT寄存器复位到上电状态
       
 		//初始化PC STACK在线程栈中值
        R2=0;
        DM(9,I2)=R2;   //PC stack此时为空
 		//初始化STATUS STACK在线程栈中值
        R1=MODE1;      
        R2=0x00001000;  //全局中断使能位置0
        R2=NOT R2;
        R2=R1 and R2;
	R1=0x00000800;  //中断嵌套使能位置1
	R2=R1 or R2;
        R1=0x04fc;		//以免第二套寄存器是使能的，引发BUG
        R1=NOT R1;
        R2=R1 and R2;
        DM(8,I2)=R2;
        ASTATx=0x0;
        nop;
        nop;
        R2=ASTATx;
        DM(7,I2)=R2;
        ASTATy=0x0;
        nop;
        nop;
        R2=ASTATy;
        DM(6,I2)=R2;
        R2=1;
        DM(5,I2)=R2;   //status stack 此时压入初始化值并将status number置1
        //初始化LOOP STACK在线程栈中值
        R2 = 0x00000001;
        DM(4,I2)=R2;    //LCNTR
        R2 = 0xFFFFFFFF;
        DM(3,I2)=R2;   //Loop Address  LADDR
        R2 = 0xFFFFFFFF;
        DM(2,I2)=R2;   //Loop Counter  CURLCNTR
        R2=1;
        DM(1,I2)=R2;   //loop stack 此时压入初始化值并将status number置1
       
       I4=R8;
       R2=I2;
       DM(I4,M5)=R2;   //将i7当前的位置也即vm的当前指针保存到R8所在的地址处，也即vm的栈底被更新
    
    //恢复进入本函数后压栈的寄存器
    R1=DM(0,I7);  I13=R1;
    R1=DM(1,I7); I2=R1;    
    alter(2);  
      FETCH_RETURN
      RETURN (DB);
      rframe;
      nop;
.___asm_reset_thread.end:
/*
@----复位老线程，切换到新线程-------------------------------------------------
@功能:  把old_vm的上下文重新初始化到新创建的状态，然后切换到新线程的
@       上下文中继续执行。
@       当一个在常驻内存的线程中处理的事件完成后，不能删除该线程，必须复位
@       该线程，然后切换到其他就绪线程，这时候就需要调用本函数。因此本函数在
@       被复位的线程上下文里面执行。
@参数:  thread_routine，被重置的线程函数
@       new_vm，目标线程的线程
@       old_vm，被复位的线程
@返回:  无
@函数原型:void __asm_reset_switch(void (*thread_routine)(void),
@                           struct  tagThreadVm *new_vm,struct  tagThreadVm *old_vm)@
@-----------------------------------------------------------------------------
*/
.section/pm seg_pmco;
.global ___asm_reset_switch;
___asm_reset_switch:

    bit clr MODE1 0x1000;		// Disable int
       I4=R12;                   //根据ADSP21469参数传递规则，R12=old_vm
       R2=DM(M6,I4);             //取老虚拟机栈顶指针
       I7=R2;
       R2=___Djy_VmEngine;

    modify(I7,-130);			//先申请栈空间
    DM(130,I7)=R2;          //_djy_vm_engine入口作为PC地址，将PC地址存入软件栈，线程栈第一个位置存储PC
    
   // save r1, r2, i2, i13 at first
    DM(129,I7)=R1;
    DM(128,I7)=R2;
    R1=I2;
    DM(127,I7)=R1;
    DM(126,I7)=I13;
    R1=I6; 
    DM(125,I7)=R1;
  // Save STKYx USTAT1-4
   MODE2=0x42000027;
   nop;
   nop;
   R1=MODE2;
   DM(123,I7)=R1;
   STKYx=0x05400000;     //处理器上电后MODE2及STKYx的默认值
   nop;
   nop;
    R1=STKYx; 
    DM(122,I7)=R1;
    STKYy=0x5400000;
    nop;
    nop;
    R1=STKYy;
    DM(121,I7)=R1;
    R1 = USTAT1; 
    DM(122,I7)=R1;
    R1 = USTAT2; 
    DM(120,I7)=R1;
    R1 = USTAT3; 
    DM(118,I7)=R1;
    R1 = USTAT4; 
    DM(117,I7)=R1;
    
       // Save and zero all the relevant L registers
        R1=L0; DM(116,I7)=R1;
        R1=L1; DM(115,I7)=R1;
        R1=L2; DM(114,I7)=R1;
        R1=L3; DM(113,I7)=R1;
        R1=L5; DM(112,I7)=R1;
        DM(111,I7)=L8;
        DM(110,I7)=L9;
        DM(109,I7)=L10;
        DM(108,I7)=L11;
        DM(107,I7)=L13;
        DM(106,I7)=L14;
        DM(105,I7)=L15;
        L0=0;
        L1=0;
        L2=0;
        L3=0;
        // L4 saved below
        L5=0;
        L8=0;
        L9=0;
        L10=0;
        L11=0;
        // L12 saved below
        L13=0;
        L14=0;
        L15=0;
        
       // Save PX registers
       DM(104,I7)=PX1;
       DM(103,I7)=PX2;
// All 40 bits of these registers must be saved. They are used in
// the integer divide routine and loss of accuracy in these registers
// will result in the wrong answer being generated.
        PX=R0; DM(102,I7)=PX1;DM(101,I7)=PX2;
        PX=R3; DM(100,I7)=PX1;DM(99,I7)=PX2;
        PX=R4; DM(98,I7)=PX1;DM(97,I7)=PX2;
        PX=R7; DM(96,I7)=PX1;DM(95,I7)=PX2;
        PX=R8; DM(94,I7)=PX1;DM(93,I7)=PX2;
        PX=R9; DM(92,I7)=PX1;DM(91,I7)=PX2;
        PX=R11;DM(90,I7)=PX1;DM(89,I7)=PX2;
        PX=R12;DM(88,I7)=PX1;DM(87,I7)=PX2;
		// R1\R2 saved below        
       DM(86,I7)=R5;
       DM(85,I7)=R6;
	   DM(84,I7)=R10;
       DM(83,I7)=R13;
       DM(82,I7)=R14;
       DM(81,I7)=R15;	 
// Save all of the Sregs. We can't do these saves and restores in
// SIMD mode in case the stack is in external memory.
// Note that on 2116x and above, we no longer save MODE1 and ASTAT
// - they are saved by the 'push sts' instruction in the vector table.
        PX=S0; DM(80,I7)=PX1;DM(79,I7)=PX2;
        PX=S4; DM(78,I7)=PX1;DM(77,I7)=PX2;
        PX=S9; DM(76,I7)=PX1;DM(75,I7)=PX2;
        PX=S11;DM(74,I7)=PX1;DM(73,I7)=PX2;
        PX=S12;DM(72,I7)=PX1;DM(71,I7)=PX2;
        
        DM(70,I7)=S1;
        DM(69,I7)=S2;
        DM(68,I7)=S3;
        DM(67,I7)=S5;
        DM(66,I7)=S6;
        DM(65,I7)=S7;
        DM(64,I7)=S8;
        DM(63,I7)=S10;
        DM(62,I7)=S13;
        DM(61,I7)=S14;
        DM(60,I7)=S15;
        
        BIT SET MODE1 RND32;
        nop;nop;

// Save the relevant DAG1 M registers - M0-3. M4 is saved later, M5-7 are
// constant.
        R1=M0;
        DM(59,I7)=R1;
        R1=M1;
        DM(58,I7)=R1;
        R1=M2;
        DM(57,I7)=R1;
        R1=M3;
        DM(56,I7)=R1;
       
// Save the relevant DAG2 M registers - M8-11. M12 is saved later, M13-15
// are constant.
        DM(55,I7)=M8;
        DM(54,I7)=M9;
        DM(53,I7)=M10;
        DM(52,I7)=M11;

// Save the Iregs
        R1=I0;
        DM(51,I7)=R1;
        R1=I1;
        DM(50,I7)=R1;
        R1=I3;
        DM(49,I7)=R1;
        R1=I5;
        DM(48,I7)=R1;
        
        DM(47,I7)=I8;
        DM(46,I7)=I9;
        DM(45,I7)=I10;
        DM(44,I7)=I11;
        DM(43,I7)=I14;
        DM(42,I7)=I15; 
       
// Save the scratch DAG registers on the stack.
        DM(41,I7)=pm_ptr;   // save i12
        DM(40,I7)=pm_bse;   // save b12
        DM(39,I7)=pm_mdf;   // save m12
        DM(38,I7)=pm_lnt;   // save l12

        R0=dm_ptr;
        DM(37,I7)=R0;        // save i4
        R0=dm_bse;
        DM(36,I7)=R0;        // save b4
        R0=dm_mdf; 
        DM(35,I7)=R0;        // save m4
        R0=dm_lnt;
        DM(34,I7)=R0;        // save l4
            
        //save B registers
        R1=B0;   DM(33,I7)=R0;   
        R1=B1;   DM(32,I7)=R0;   
        R1=B2;   DM(31,I7)=R0;   
        R1=B3;   DM(30,I7)=R0;   
        R1=B5;   DM(29,I7)=R0;   
        
        DM(28,I7)=B8; 
        DM(27,I7)=B9; 
        DM(26,I7)=B10;
        DM(25,I7)=B11;
        DM(24,I7)=B13;
        DM(23,I7)=B14;
        DM(22,I7)=B15;
            
      
 // 至此上下文中通用寄存器区复位完毕，接下来将硬件栈中ASTAT寄存器复位到上电状态
       
 		//初始化PC STACK在线程栈中值
        R2=0;
        DM(9,I7)=R2;   //PC stack此时为空
 		//初始化STATUS STACK在线程栈中值
        R1=MODE1;      
        R2=0x00001000;  //全局中断使能位置0
        R2=NOT R2;
        R2=R1 and R2;
	R1=0x00000800;  //中断嵌套使能位置1
	R2=R1 or R2;
        R1=0x04fc;		//以免第二套寄存器是使能的，引发BUG
        R1=NOT R1;
        R2=R1 and R2;
        DM(8,I7)=R2;
        ASTATx=0x0;
        nop;
        nop;
        R2=ASTATx;
        DM(7,I7)=R2;
        ASTATy=0x0;
        nop;
        nop;
        R2=ASTATy;
        DM(6,I7)=R2;
        R2=1;
        DM(5,I7)=R2;   //status stack 此时压入初始化值并将status number置1
        //初始化LOOP STACK在线程栈中值
        R2 = 0x00000001;
        DM(4,I7)=R2;    //LCNTR
        R2 = 0xFFFFFFFF;
        DM(3,I7)=R2;   //Loop Address  LADDR
        R2 = 0xFFFFFFFF;
        DM(2,I7)=R2;   //Loop Counter  CURLCNTR
        R2=1;
        DM(1,I7)=R2;   //loop stack 此时压入初始化值并将status number置1

        //至此旧线程复位完成    
       I4=R12;
       R2=I7;
       DM(I4,M5)=R2;          //将I7当前的位置也即vm的当前指针保存到R12所在的地址处，也即vm的栈底被更新 

      //开始切换新线程，将新线程上下文恢复
       I4=R8;                  //R8传递 new vm结构体首地址，也即new vm->stack
       I12=I4;        
       R2=DM(M6,I4);     //取虚拟机栈顶指针  根据虚拟机结构，第二个元素为存储栈顶指针
    //接下来要取vm结构体中stack_size这个变量 vm->stack_size这个成员在vm结构体中第4个位置
      R0=I4;
      R1=2;
      R0=R1+R0;
      I4=R0;
      R1=DM(M6,I4); //取得vm->stack_size值，接下来将vm->stack_size值赋给L7，同时根据栈顶及栈深L7求出栈底并赋值给B7
      L7=R1;
      R0=R2-R1;
      R0=R0+1;
      B7=R0;
      I4=I12;
      R2=DM(M5,I4);
      I7=R2;
      B6=B7;		//一定不要忘记了
      L6=L7;
       
    //开始调用C函数int_half_enable_asyn_signal,在调用C函数之前需完成两步预备动作
      R2=I6;
      I6=I7;
     I12=_Int_HalfEnableAsynSignal;
     JUMP(M13,I12)(DB);
     DM(I7,M7)=R2;
     DM(I7,M7)=PC;
 //首先恢复硬件栈中内容,开始弹硬件栈	
	R0=DM(1,I7);
	modify(I7,1);					//取R0，计算LOOP栈长度
	R0 = R0 - 1;
	IF EQ JUMP (.restore_loop_end_reset_switch);			//if R0==0,jump
.restore_loop_start_reset_switch:
    R2=DM(1,I7); CURLCNTR = R2;
	R2=DM(2,I7); LADDR = R2;
	R2=DM(3,I7); LCNTR = R2;
	modify(I7,3);
	push loop;
	R0 = R0 - 1;
	IF NE JUMP (.restore_loop_start_reset_switch);
.restore_loop_end_reset_switch:
    R2=DM(1,I7); CURLCNTR = R2;
	R2=DM(2,I7); LADDR = R2;
	R2=DM(3,I7); LCNTR = R2;
	modify(I7,3);        //loop出栈结束，status出栈开始
	R0=DM(1,I7);         //取R0，计算STATUS栈长度
	modify(I7,1); 				
	R0 = R0 - 1;
	IF EQ JUMP (.restore_sts_end_reset_switch);			//if R0==0,jump
.restore_sts_start_reset_switch:
///	R2=DM(1,I7);
    R2=DM(2,I7);  ASTAT = R2;
    nop;nop;
    R2=DM(3,I7);  MODE1 = R2;
    nop;nop;
    modify(I7,3); 	
	push sts;
	R0 = R0 - 1;
	IF NE JUMP (.restore_sts_start_reset_switch);
.restore_sts_end_reset_switch:
///	R2=DM(1,I7);
    R2=DM(2,I7);  ASTAT = R2;
    nop;nop;
    R2=DM(3,I7);  MODE1 = R2;
    nop;nop;
    modify(I7,3); 	
	//status出栈结束，pc出栈开始
	R0=DM(1,I7);  //取R0，计算PC栈长度
	modify(I7,1); 				
	R0 = PASS R0;
	IF EQ JUMP (.restore_pc_end_reset_switch);			//if R0==0,jump
.restore_pc_start_reset_switch:
	push pcstk;
	R2=DM(1,I7); PCSTK = R2;
    modify(I7,1); 
	R0 = R0 - 1;
	IF NE JUMP (.restore_pc_start_reset_switch);
.restore_pc_end_reset_switch:
	nop;
//至此，弹硬件栈结束,下面开始恢复寄存器值
    call ___restore_all_regs;  //通用寄存器出栈
    I12=DM(1,I7);
    modify(I7,1); 
    JUMP(M13,I12)(db);
     nop;
     bit set MODE1 0x1000;		// Enable int
.___asm_reset_switch.end:

/*
@----启动多事件调度-----------------------------------------------------------
@功能: 系统初始化完成后，启动多事件调度并切入第一个事件.
@参数: new_vm，首次启动的线程控制块指针
@返回: 无
@函数原型: void __asm_start_thread(struct  tagThreadVm  *new_vm)@
@-----------------------------------------------------------------------------
*/
.section/pm seg_pmco;
.global __asm_start_thread;
__asm_start_thread:
    nop;
.__asm_start_thread.end:

/*
@----切入上下文---------------------------------------------------------------
@功能:  不保存原上下文，直接切入新的上下文执行
@参数:  new_vm，待切入的线程控制块指针
@返回:  无
@函数原型: void __asm_turnto_context(struct  tagThreadVm  *new_vm)@
@说明:  当事件完成,就没有必要保存旧事件的上下文,直接切换到新事件即可.
@-----------------------------------------------------------------------------
*/
.section/pm seg_pmco;
.global ___asm_turnto_context;
___asm_turnto_context:

    bit clr MODE1 0x1000;		// Disable int
    
    I4=R4;            //R4传递 new vm结构体首地址，也即new vm->stack
    I12=I4;        
    R2=DM(M6,I4);     //取虚拟机栈顶指针  根据虚拟机结构，第二个元素为存储栈顶指针
    //接下来要取vm结构体中stack_size这个变量 vm->stack_size这个成员在vm结构体中第4个位置
    R0=I4;
    R1=2;
    R0=R1+R0;
    I4=R0;
    R1=DM(M6,I4); //取得vm->stack_size值，接下来将vm->stack_size值赋给L7，同时根据栈顶及栈深L7求出栈底并赋值给B7
    L7=R1;
    R0=R2-R1;
    R0=R0+1;
    B7=R0;
    I4=I12;
    R2=DM(M5,I4);
    I7=R2;
      B6=B7;		//一定不要忘记了
      L6=L7;
    //开始调用C函数int_half_enable_asyn_signal,在调用C函数之前需完成两步预备动作
    R2=I6;
    I6=I7;
    I12=_Int_HalfEnableAsynSignal;
    JUMP(M13,I12)(DB);
     DM(I7,M7)=R2;
     DM(I7,M7)=PC;
   //首先恢复硬件栈中内容,开始弹硬件栈：注意，此时的硬件栈都已经为空了	
	R0=DM(1,I7);
	modify(I7,1);					//取R0，计算LOOP栈长度
	R0 = R0 - 1;
	IF EQ JUMP (.restore_loop_end_turnto);			//if R0==0,jump
.restore_loop_start_turnto:
	R2=DM(1,I7); CURLCNTR = R2;
	R2=DM(2,I7); LADDR = R2;
	R2=DM(3,I7); LCNTR = R2;
	modify(I7,3);
	push loop;
	R0 = R0 - 1;
	IF NE JUMP(.restore_loop_start_turnto);
.restore_loop_end_turnto:
	R2=DM(1,I7); CURLCNTR = R2;
	R2=DM(2,I7); LADDR = R2;
	R2=DM(3,I7); LCNTR = R2;
	modify(I7,3);        //loop出栈结束，status出栈开始
	R0=DM(1,I7);         //取R0，计算STATUS栈长度
	modify(I7,1); 				
	R0 = R0 - 1;
	IF EQ JUMP (.restore_sts_end_turnto);			//if R0==0,jump
.restore_sts_start_turnto:
	R2=DM(1,I7);
    R2=DM(2,I7);  ASTAT = R2;
    nop;nop;
    R2=DM(3,I7);  MODE1 = R2;
    nop;nop;
    modify(I7,3); 	
	push sts;
	R0 = R0 - 1;
	IF NE JUMP  (.restore_sts_start_turnto);
.restore_sts_end_turnto:
    R2=DM(1,I7);
    R2=DM(2,I7);  ASTAT = R2;
    nop;nop;
	R2=DM(3,I7);
	R1=0x00001000;R1=NOT R1;R2=R2 and R1;//注意，未免发生意外，此处将全局中断关闭，在退出中断时打开
//	R1=0x000004fc;R1=NOT R1;R2=R2 and R1;//当程序在asyn ISR中注册时，会发生异常，所以这里清理一下
	MODE1 = R2;					
	nop;nop;
	modify(I7,3); 	
	R0=DM(1,I7);  //status出栈结束，pc出栈开始取R0，计算PC栈长度
	modify(I7,1); 				
	R0 = PASS R0;
	IF EQ JUMP (.restore_pc_end_turnto);			//if R0==0,jump
.restore_pc_start_turnto:
	push pcstk;
	R2=DM(1,I7); PCSTK = R2;
    modify(I7,1); 
	R0 = R0 - 1;
	IF NE JUMP (.restore_pc_start_turnto);
.restore_pc_end_turnto:
	nop;
//至此，弹硬件栈结束,下面开始恢复寄存器值
    call ___restore_all_regs;  //通用寄存器出栈
      I12=DM(1,I7);
      modify(I7,1); 
      
      
  
      //测试SOVFI异常对系统影响
      /* push loop;
       push loop;
       push loop;
       push loop;
       push loop;
       push loop;
       //loop stack已满，再压即会溢出
       push loop;*/
       
       
       
       
       
       
       
       
       
      JUMP(M13,I12)(db);
      nop;
      bit set MODE1 0x1000;		// Enable int
   
.___asm_turnto_context.end:

/*
@----上下文切换---------------------------------------------------------------
@功能:  保存当前线程的上下文，切换到新线程的上下文中继续执行。
@参数:  new_sp，新上下文的栈指针
@参数:  old_sp，旧上下文的栈指针的指针，即&vm->stack。无需提供旧上下文栈指针，
@               sp寄存器的当前值就是
@返回:  无
@函数原型: 
void __asm_switch_context(struct  tagThreadVm *new_vm,struct  tagThreadVm *old_vm)
@-----------------------------------------------------------------------------
*/
.section/pm seg_pmco;
.global ___asm_switch_context;
___asm_switch_context:

    bit clr MODE1 0x1000;		// Disable int
      
	R0=DM(M7,I6);
	DM(I7,M7)=R0;              //首先将旧线程返回地址保存到上下文的PC位置处
    call ___backup_all_regs;    //通用寄存器压栈
    //开始将硬件栈内容压入线程虚拟机
	R0 = pcstkp;
	BIT TST STKYx 0x00400000;
	IF TF JUMP (PC, .save_pc_end_switch_content);
.save_pc_start_switch_content:					//若pcstkp！=0，则依次读pc，并入栈
	DM(I7,M7)=pcstk;
	pop pcstk;
	BIT TST STKYx 0x00400000;
	IF NOT TF JUMP (PC, .save_pc_start_switch_content);
.save_pc_end_switch_content:					
	DM(I7,M7)=R0;    //将R0=pcstkp入栈，在出栈时需根据此值判定循环次数					
	//pc入栈结束，status入栈开始
	R0 = 1;
	R2 = MODE1;  DM(I7,M7)=R2;			//存当前MODE1、ASTATx、ASTATy
	R2 = ASTAT;  DM(I7,M7)=R2; 
	R2 = 0x00000000; DM(I7,M7)=R2;
	BIT TST STKYx 0x01000000;
	IF TF JUMP (PC, .save_sts_end_switch_content);
.save_sts_start_switch_content:
	pop sts;					//出硬件栈
	R2 = MODE1;DM(I7,M7)=R2;			//存MODE1、ASTATx、ASTATy
//	BIT CLR MODE1 0x1000;		//注意，未免发生意外，此处将全局中断关闭，在退出本函数时打开
	R2 = ASTAT;DM(I7,M7)=R2;
	R2 = 0x00000000; DM(I7,M7)=R2;
	R0 = R0 + 1;
	BIT TST STKYx 0x01000000;
	IF NOT TF JUMP (PC, .save_sts_start_switch_content);
.save_sts_end_switch_content:					
    DM(I7,M7)=R0;   //将R0入栈，在出栈时需根据此值判定循环次数				
	//status入栈结束，loop入栈开始
	R0 = 1;
	DM(I7,M7)=LCNTR;    // Save loop counter
	DM(I7,M7)=LADDR;    // Save loop address
	DM(I7,M7)=CURLCNTR; // Save current counter       	
	BIT TST STKYx 0x04000000;
	IF TF JUMP (PC, .save_loop_end_switch_content);
.save_loop_start_switch_content:
	pop loop;
	DM(I7,M7)=LCNTR;    // Save loop counter
	DM(I7,M7)=LADDR;    // Save loop address
	DM(I7,M7)=CURLCNTR; // Save current counter  
	R0 = R0 + 1;
	BIT TST STKYx 0x04000000;
	IF NOT TF JUMP (PC, .save_loop_start_switch_content);
.save_loop_end_switch_content:					//loop入栈结束
	put(R0);					//将R0入栈，在出栈时需根据此值判定循环次数
//至此，存硬件栈结束,所有压栈操作完成
    R1=I7;
    I4=R8;
    DM(M5,I4)=R1;      //将old_vm的当前栈指针I7存入到R8所指的地址中
    //至此，旧线程上下文已经被保存，接下来开始切入到新线程
    I4=R4;            //R4传递 new vm结构体首地址，也即new vm->stack
    I12=I4;        
    R2=DM(M6,I4);     //取虚拟机栈顶指针  根据虚拟机结构，第二个元素为存储栈顶指针
    //接下来要取vm结构体中stack_size这个变量 vm->stack_size这个成员在vm结构体中第4个位置
    R0=I4;
    R1=2;
    R0=R1+R0;
    I4=R0;
    R1=DM(M6,I4); //取得vm->stack_size值，接下来将vm->stack_size值赋给L7，同时根据栈顶及栈深L7求出栈底并赋值给B7
    L7=R1;
    R0=R2-R1;
    R0=R0+1;
    B7=R0;
    I4=I12;
    R2=DM(M5,I4);
    I7=R2;
    B6=B7;		//一定不要忘记了
    L6=L7;
    //开始调用C函数int_half_enable_asyn_signal,在调用C函数之前需完成两步预备动作
    R2=I6;
    I6=I7;
    I12=_Int_HalfEnableAsynSignal;
    JUMP(M13,I12)(DB);
    DM(I7,M7)=R2;
    DM(I7,M7)=PC;
   //首先恢复硬件栈中内容,开始弹硬件栈：注意，此时的硬件栈都已经为空了	
    R0=DM(1,I7);
	modify(I7,1);					//取R0，计算LOOP栈长度
	R0 = R0 - 1;
	IF EQ JUMP  (.restore_loop_end_switch_content);			//if R0==0,jump
.restore_loop_start_switch_content:
    R2=DM(1,I7); CURLCNTR = R2;
	R2=DM(2,I7); LADDR = R2;
	R2=DM(3,I7); LCNTR = R2;
	modify(I7,3);
	push loop;
	R0 = R0 - 1;
	IF NE JUMP (.restore_loop_start_switch_content);
.restore_loop_end_switch_content:
	R2=DM(1,I7); CURLCNTR = R2;
	R2=DM(2,I7); LADDR = R2;
	R2=DM(3,I7); LCNTR = R2;
	modify(I7,3);        //loop出栈结束，status出栈开始
	R0=DM(1,I7);         //取R0，计算STATUS栈长度
	modify(I7,1); 				
	R0 = R0 - 1;
	IF EQ JUMP (.restore_sts_end_switch_content);			//if R0==0,jump
.restore_sts_start_switch_content:
    R2=DM(1,I7);
    R2=DM(2,I7);  ASTAT = R2;
    nop;nop;
    R2=DM(3,I7);  MODE1 = R2;
    nop;nop;
    modify(I7,3); 	
	push sts;
	R0 = R0 - 1;
	IF NE JUMP  (.restore_sts_start_switch_content);
.restore_sts_end_switch_content:
    R2=DM(1,I7);
    R2=DM(2,I7);  ASTAT = R2;
    nop;nop;
	R2=DM(3,I7);
	R1=0x00001000;R1=NOT R1;R2=R2 and R1;//注意，未免发生意外，此处将全局中断关闭，在退出中断时打开
//	R1=0x000004fc;R1=NOT R1;R2=R2 and R1;//当程序在asyn ISR中注册时，会发生异常，所以这里清理一下
	MODE1 = R2;					
	nop;nop;
	modify(I7,3); 	
	R0=DM(1,I7);  //status出栈结束，pc出栈开始取R0，计算PC栈长度
	modify(I7,1); 				
	R0 = PASS R0;
	IF EQ JUMP (.restore_pc_end_switch_content);			//if R0==0,jump
.restore_pc_start_switch_content:
	push pcstk;
	R2=DM(1,I7); PCSTK = R2;
    modify(I7,1); 
	R0 = R0 - 1;
	IF NE JUMP (.restore_pc_start_switch_content);
.restore_pc_end_switch_content:
	nop;
//至此，弹硬件栈结束,下面开始恢复寄存器值
      //在这里，我们需要将之前save的asyn恢复
      //注意，总中断必须在此关闭，在退出本函数时打开
      bit clr MODE1 0x1000;		// Disable int
      CJUMP _Int_ContactAsynSignal(DB);
      DM(I7,M7)=R2;
      DM(I7,M7)=PC;
      call ___restore_all_regs;  //通用寄存器出栈
      DM(-1,I7)=I12;            // Save I12
      I12=DM(1,I7);			
      modify(I7,1);
      pop sts;
      JUMP(M13,I12)(db);
      //I12=DM(-1,I7);
      I12=DM(-2,I7);			// Restore I12   //20140114改动
      bit set MODE1 0x1000;		// Enable int
.___asm_switch_context.end:





/*
@----启动多事件调度-----------------------------------------------------------
@功能: 系统初始化完成后，启动多事件调度并切入第一个事件.
@参数: new_vm，首次启动的线程控制块指针
@返回: 无
@函数原型: void __asm_start_thread(struct  tagThreadVm  *new_vm)@
@-----------------------------------------------------------------------------
*/
.section/pm seg_pmco;
.global ___asm_start_thread;
___asm_start_thread:
    NOP;
.___asm_start_thread.end:

/*
@----中断中的上下文切换-------------------------------------------------------
@功能:  保存被中断线程的上下文，切换到新线程的上下文中继续执行。本函数虽然在中
@       断服务函数(非用户用int_isr_connect函数连接的中断服务函数)中，但在ARM
@       中，却运行在svc态
@参数:  new_sp，切换目标线程
@参数:  old_sp，被中断线程
@返回:  无
@函数原型: void __asm_switch_context_int(struct tagThreadVm *new_vm,struct tagThreadVm *old_vm)@
@-----------------------------------------------------------------------------
*/
.section/pm seg_pmco;
.global ___asm_switch_context_int;
___asm_switch_context_int:
	
	bit clr MODE1 0x1000;	// 
    nop;nop;
	bit clr MODE1 0x04fc;	// Disable second register
    nop;nop;

//初始化栈指针
//	no need here

//将PC返回值压栈
   DM(I7,M7)=pcstk;				//
//通用寄存器压栈(含MODE2,USTAT1-4)
	call ___backup_all_regs;    
//开始压硬件栈
	pop pcstk;					//清理掉进中断时push的一次pc值
	R0 = pcstkp;
	BIT TST STKYx 0x00400000;
	IF TF JUMP (PC, .save_pc_end);
.save_pc_start:
    DM(I7,M7)=pcstk;	//若pcstkp！=0，则依次读pc，并入栈
	pop pcstk;
	BIT TST STKYx 0x00400000;
	IF NOT TF JUMP (PC, .save_pc_start);
.save_pc_end:					
	DM(I7,M7)=R0;   //将R0=pcstkp入栈，在出栈时需根据此值判定循环次数				
	R0 = 1;       //pc入栈结束，status入栈开始
//	pop sts;
	R2 = MODE1;					//存当前MODE1、ASTATx、ASTATy
	R1=0x00001000;R2=R2 or R1;
	DM(I7,M7)=R2;
	R2 = ASTAT;  DM(I7,M7)=R2; 
	R2 = 0x00000000; DM(I7,M7)=R2; 
	BIT TST STKYx 0x01000000;
	IF TF JUMP (PC, .save_sts_end);
.save_sts_start:
	pop sts;					//出硬件栈
	R2 = MODE1;   DM(I7,M7)=R2; 			//存MODE1、ASTATx、ASTATy
	BIT CLR MODE1 0x1000;		//注意，未免发生意外，此处将全局中断关闭，在退出中断时打开
	R2 = ASTAT;DM(I7,M7)=R2; 
	R2 = 0x00000000; DM(I7,M7)=R2; 
	R0 = R0 + 1;
	BIT TST STKYx 0x01000000;
	IF NOT TF JUMP (PC, .save_sts_start);
.save_sts_end:					
	DM(I7,M7)=R0;  	//将R0入栈，在出栈时需根据此值判定循环次数				
	//status入栈结束，loop入栈开始
	R0 = 1;
	DM(I7,M7)=LCNTR;
	DM(I7,M7)=LADDR;
	DM(I7,M7)=CURLCNTR;
	BIT TST STKYx 0x04000000;
	IF TF JUMP (PC, .save_loop_end);
.save_loop_start:
	pop loop;
	DM(I7,M7)=LCNTR;
	DM(I7,M7)=LADDR;
	DM(I7,M7)=CURLCNTR;
	R0 = R0 + 1;
	BIT TST STKYx 0x04000000;
	IF NOT TF JUMP (PC, .save_loop_start);
.save_loop_end:					//loop入栈结束
	DM(I7,M7)=R0;					//将R0入栈，在出栈时需根据此值判定循环次数
//至此，存硬件栈结束,所有压栈操作完成
    R1=I7;
//注意，对于中断中的线程切换而言，因为参数是通过第二套寄存器传递进来的，
//因此，这里必须重新使能第二套寄存器的R0-R8，来获取new vm值，然后再还原
	bit set MODE1 0x0080;	// Enable second R0-R7 register
    nop;nop;
    I4=R8;            //R4传递 new vm结构体首地址，也即new vm->stack
	bit clr MODE1 0x0080;	// Disable second R0-R7 register
    nop;nop;
    DM(M5,I4)=R1;      //将old_vm的当前栈指针I7存入到R8所指的地址中
//至此，旧线程上下文已经被保存，接下来开始切入到新线程
    
//初始化栈指针
//注意，对于中断中的线程切换而言，因为参数是通过第二套寄存器传递进来的，
//因此，这里必须重新使能第二套寄存器的R8-R15，来获取new vm值，然后再还原
	bit set MODE1 0x0400;	// Enable second R8-R15 register
    nop;nop;
    I4=R4;            //R8传递 old vm结构体首地址，也即old vm->stack
	bit clr MODE1 0x0400;	// Disable second R8-R15 register
    nop;nop;
    
    I12=I4;        
    R2=DM(M6,I4);     //取虚拟机栈顶指针  根据虚拟机结构，第二个元素为存储栈顶指针
    //接下来要取vm结构体中stack_size这个变量 vm->stack_size这个成员在vm结构体中第4个位置
    R0=I4;
    R1=2;
    R0=R1+R0;
    I4=R0;
    R1=DM(M6,I4); //取得vm->stack_size值，接下来将vm->stack_size值赋给L7，同时根据栈顶及栈深L7求出栈底并赋值给B7
    L7=R1;
    R0=R2-R1;
    R0=R0+1;
    B7=R0;
    I4=I12;
    R2=DM(M5,I4);
    I7=R2;
      B6=B7;		//一定不要忘记了
      L6=L7;
  
/*    //开始调用C函数int_half_enable_asyn_signal,在调用C函数之前需完成两步预备动作
    R2=I6;
    I6=I7;
    I12=_Int_HalfEnableAsynSignal;
    JUMP(M13,I12)(DB);
    DM(I7,M7)=R2;
    DM(I7,M7)=PC;
	*/
//开始弹硬件栈：注意，此时的硬件栈都已经为空了	
	R0=DM(1,I7);    //取R0，计算LOOP栈长度
    modify(I7,1);
	R0 = R0 - 1;
	IF EQ JUMP (.restore_loop_end);			//if R0==0,jump
.restore_loop_start:
    R2=DM(1,I7); CURLCNTR = R2;
    R2=DM(2,I7); LADDR = R2;
    R2=DM(3,I7); LCNTR = R2;
	modify(I7,3);
	push loop;
	R0 = R0 - 1;
	IF NE JUMP (.restore_loop_start);
.restore_loop_end:
	R2=DM(1,I7); CURLCNTR = R2;
    R2=DM(2,I7); LADDR = R2;
    R2=DM(3,I7); LCNTR = R2;
	modify(I7,3);
	//loop出栈结束，status出栈开始
	R0=DM(1,I7);    //取R0，计算STATUS栈长度
	modify(I7,1);				
	R0 = R0 - 1;
	IF EQ JUMP (.restore_sts_end);			//if R0==0,jump
.restore_sts_start:
    R2=DM(2,I7); ASTAT = R2;
    nop;nop;
    R2=DM(3,I7); MODE1= R2;
    nop;nop;
	modify(I7,3);
	push sts;
	R0 = R0 - 1;
	IF NE JUMP (.restore_sts_start);
.restore_sts_end:
    R2=DM(2,I7); ASTAT = R2;
	nop;nop;
	R2=DM(3,I7);
	R1=0x00001000;R1=NOT R1;R2=R2 and R1;//注意，未免发生意外，此处将全局中断关闭，在退出中断时打开
	R1=0x000004fc;R1=NOT R1;R2=R2 and R1;//当程序在asyn ISR中注册时，会发生异常，所以这里清理一下
	MODE1 = R2;					
	nop;nop;
	modify(I7,3);    //ignore 1 . reserve				
	//status出栈结束，pc出栈开始
	R0=DM(1,I7);    //取R0，计算PC栈长度
    modify(I7,1);
	R0 = PASS R0;
	IF EQ JUMP (.restore_pc_end);			//if R0==0,jump
.restore_pc_start:
	push pcstk;
	R2=DM(1,I7); pcstk=R2;
	modify(I7,1);
	R0 = R0 - 1;
	IF NE JUMP (.restore_pc_start);
.restore_pc_end:
	nop;
//至此，弹硬件栈结束,下面开始恢复寄存器值
//在这里，我们不需要重新使能asyn
//通用寄存器出栈(含MODE2,USTAT1-4)
	call ___restore_all_regs;
	
	//伪造进中断时PCSTK的状态
	push pcstk;
	pcstk=DM(1,I7);
	modify(I7,1);
	//至此，所有偷梁换柱工作完成，可以返回了
	bit set MODE1 0x04fc;	// Enable second register
    nop;nop;
    
    I12=DM(M7, I6);
    JUMP (M14,I12)(DB);
    rframe;
    nop;
    

.___asm_switch_context_int.end:
