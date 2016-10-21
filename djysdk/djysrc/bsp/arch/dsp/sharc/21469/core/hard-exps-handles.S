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

.file   "exception_handles.S";

#include <platform_include.h>
#include <sys/anomaly_macros_rtl.h>

#include "def21469.h"
#include "lib_glob.h"
#include "sig_glob.h"
#include "exception.h"

#include "cpu_peri_int_line.h"
#include "cpu_peri_address.h"


.file_attr libGroup="startup";
.file_attr libName="libc";
.file_attr prefersMem="internal";
.file_attr prefersMemNum="30";


.extern ___Int_EngineReal;
.extern ___Int_EngineAsynSignal;
.extern ___asm_switch_context_int;


.extern __Exp_General;

.extern int_stack_space;	// The base of the int stack
.extern int_stack_length;	// The size of the int stack  
.extern int_stack_end;	    // The end of the int stack 
//.extern Exception_StartAddress;

.extern exp_stack_space;	// The base of the exception stack
.extern exp_stack_length;	// The size of the exception stack  
.extern exp_stack_end;	    // The end of the exception stack 


.extern ldf_stack_space;
.extern ldf_stack_length;
.extern ldf_stack_end;

#define reg_backup_offset  0	//for ___backup_all_regs use only
#define reg_restore_offset 0	//for ___restore_all_regs use only


/*
@----异步信号跳转---------------------------------------------------------------
@功能:  
@参数:  
@返回:  
@函数原型:___nop_interrupt；___push_interrupt；
@-------------------------------------------------------------------------------
*/

#if defined (__SHORT_WORD_CODE__)
// The seg_rth output section only gets normal word code mapped in the LDFs
.SECTION/PM/NW seg_rth;
#else
.SECTION/PM    seg_rth;
#endif

.global ___nop_interrupt;
  /* Suppress the assembler warning for 02000065. We are safe here
  ** because I7 and M7 are not defined in the second delay slot of
  ** a jump to "___field_interrupt".
  */
.MESSAGE/SUPPRESS 2507 FOR 2 LINES;
___nop_interrupt:
	  bit clr MODE1 0x04fc;
	  nop;nop;	  
	  pop sts;				  //硬件推STS，软件给你推出来，以免你害人
	  nop;nop;
      BIT CLR MODE1 0x1000;   //Disable interrupts
      nop;nop;
	  push sts;
	  bit set MODE1 0x04fc;
	  nop;nop;	  
	  push loop;
      B7 = int_stack_space;
      I7 = (  (int_stack_space + int_stack_length - 1)
      		- ((int_stack_space + int_stack_length - 1) % 2) );
      L7 = (  int_stack_length
            - ((int_stack_space + int_stack_length - 1) % 2) );
      B6 = B7;
      I6 = I7;
      L6 = L7;
	  modify(i7, -6);
      DM(6,I7)=px1;
      DM(5,I7)=px2;
	  DM(4,I7)=R2;				  // Save IRQ number 
	  R1=IMASK;DM(3,I7)=R1;         // Save IMASK 
	  R1=LIRPTL;DM(2,I7)=R1;        // Save LIRPTL
//	  DM(1,I7)=R1;				  // align

//asyn_mask_imask为F04FFC10(cpu_peri_int_line.h中定义)
	  BIT CLR IMASK asyn_mask_imask;	  // MASK all the asyn ints
	  nop;nop;
	  
	  BIT CLR LIRPTL asyn_mask_lirptl;  // MASK all the asyn ints
	  nop;nop;
	  
      JUMP (PC, ___asyn_interrupt_handler) (DB); 
      NOP;
	  NOP;                        /* in place of freeze cache         */
.___nop_interrupt.end:


.global ___push_interrupt;
.MESSAGE/SUPPRESS 2507 FOR 2 LINES;
___push_interrupt:
	  bit clr MODE1 0x04fc;
	  nop;nop;	  
	  push sts;
	  bit set MODE1 0x04fc;
	  nop;nop;	  
      BIT CLR MODE1 0x1000;   //Disable interrupts
      nop;nop;
	  push loop;
      B7 = int_stack_space;
      I7 = (  (int_stack_space + int_stack_length - 1)
      		- ((int_stack_space + int_stack_length - 1) % 2) );
      L7 = (  int_stack_length
            - ((int_stack_space + int_stack_length - 1) % 2) );
      B6 = B7;
      I6 = I7;
      L6 = L7;
	  modify(i7, -6);
      DM(6,I7)=px1;
      DM(5,I7)=px2;
	  DM(4,I7)=R2;				  // Save IRQ number 
	  R1=IMASK;DM(3,I7)=R1;         // Save IMASK 
	  R1=LIRPTL;DM(2,I7)=R1;        // Save LIRPTL
//	  DM(6,I7)=R1;				  // align
	  BIT CLR IMASK asyn_mask_imask;	  // MASK all the asyn ints
	  nop;nop;
	  
	  BIT CLR LIRPTL asyn_mask_lirptl;  // MASK all the asyn ints
	  nop;nop;
	  
      JUMP (PC, ___asyn_interrupt_handler) (DB); 
      NOP;
	  NOP;                        /* in place of freeze cache         */
.___push_interrupt.end:


/*
@----异步信号中断---------------------------------------------------------------
@功能:  
@参数:  
@返回:  
@函数原型:___asyn_interrupt_handler;
@-------------------------------------------------------------------------------
*/

/* asyn interrupt handler */

.SECTION/PM    seg_pmco;

.GLOBAL    ___asyn_interrupt_handler;

___asyn_interrupt_handler:

		BIT CLR MODE1 0x1000;   //Disable interrupts
		
        
// Here is the section of code that saves the loop stacks to memory
// before calling the handler.  The technique is as follows:  R12 points
// to the address of the place to go if there were NO loop stack slots
// used.  The various registers are saved, and if the Loop Stack Empty
// bit is NOT set int STKYx, R12 is updated with the address of the code
// that performs one additional loop stack restore.
// This means that instead of just setting LCNTR, the first loop stack
// spot will be restored.
// The maximum number of loop stack spots will be saved to the C runtime
// stack, but the R12 pointer will only be changed if the stack is
// not empty.  The critical step is the stack restore.  See below for
// more details.

        R12=asyn_empty_stack;         // Go here for no stack
        
        modify(i7, -14);

        DM(14,I7)=LCNTR;              // Save loop counter
        DM(13,I7)=LADDR;              // Save loop address
        DM(12,I7)=CURLCNTR;           // Save current counter

        BIT TST STKYx 0x04000000; // Test for empty stack
            
        IF TF JUMP (PC, .asyn_save_loop_address1);
                                 // Loop stack is empty
        POP LOOP;                // Pop down loop stack
        R12=___asyn_lib_DO_NOT_EMU_STEP6;  // Performs 1 loop stack restore

.asyn_save_loop_address1:
        DM(11,I7)=LADDR;              // Save loop address
        DM(10,I7)=CURLCNTR;           // Save current counter
        BIT TST STKYx 0x04000000; // Test for empty stack
        IF TF JUMP (PC, .asyn_save_loop_address2);
                                 // Loop stack is empty
        POP LOOP;                // Pop down loop stack
        R12=___asyn_lib_DO_NOT_EMU_STEP5;  // Performs 2 loop stack restores

.asyn_save_loop_address2:
        DM(9,I7)=LADDR;              // Save loop address
        DM(8,I7)=CURLCNTR;           // Save current counter
        BIT TST STKYx 0x04000000; // Test for empty stack
        IF TF JUMP (PC, .asyn_save_loop_address3);
                                 // Set address
        POP LOOP;                // Pop down loop stack
        R12=___asyn_lib_DO_NOT_EMU_STEP4;  // Performs 3 loop stack restores

.asyn_save_loop_address3:
        DM(7,I7)=LADDR;              // Save loop address
        DM(6,I7)=CURLCNTR;           // Save current counter
        BIT TST STKYx 0x04000000; // Test for empty stack
        IF TF JUMP (PC, .asyn_save_loop_address4);
                                 // Set address
        POP LOOP;                // Pop down loop stack
        R12=___asyn_lib_DO_NOT_EMU_STEP3;  // Performs 4 loop stack restores

.asyn_save_loop_address4:
        DM(5,I7)=LADDR;              // Save loop address
        DM(4,I7)=CURLCNTR;           // Save current counter
        BIT TST STKYx 0x04000000; // Test for empty stack
        IF TF JUMP (PC, .asyn_save_loop_address5);
                                 // Set address
        POP LOOP;                // Pop down loop stack
        R12=___asyn_lib_DO_NOT_EMU_STEP2;  // Performs 5 loop stack restores

.asyn_save_loop_address5:
        DM(3,I7)=LADDR;              // Save loop address
        DM(2,I7)=CURLCNTR;           // Save current counter
        BIT TST STKYx 0x04000000; // Test for empty stack
        IF TF JUMP (PC, .asyn_determine_int_num);
                                 // Set address
        POP LOOP;                // Pop down loop stack
        R12=___asyn_lib_DO_NOT_EMU_STEP1;  // Performs 6 loop stack restores

.asyn_determine_int_num:

        /*
        ** Determine interrupt number.
        */
        /* Work out the interrupt from the address of the interrupt
        ** table index currently held in I2.
        */
        
        DM(1,I7)=R12;        	// save number of loop stack restores
           
        R2 = DM(18,I7);				// get the IRQ num back
        R4 = R2;					// R4 = ufl_line_hard NOW

        BIT SET MODE1 0x1000;       // Enable all interrupts
        
        
        I13=asyn_int_cont;          // People may need this still      
        pm_ptr=___Int_EngineAsynSignal;	// Interrupt engine all
        R2=frame_ptr;				// R2=frame_ptr;
        frame_ptr=stack_ptr;		// frame_ptr=stack_ptr;
        JUMP(pm_0, pm_ptr) (DB); 	// Call the handler JUMP(pm_0, pm_ptr) (DB);
           DM(I7,M7)=R2;			// DM(I7,M7)=R2;
           DM(I7,M7)=PC;			// DM(I7,M7)=PC;

           
           

           
                      
// Here, we calculate the new value of IMASK and assign it.
// For 21020 and 2106x, we also need to save the current FLAGS bits,
// restore the old value of ASTAT, and then mask in the saved FLAGS bits. 

asyn_int_cont:
		
        bit clr mode1 PEYEN;       // Clear SIMD mode, in case the user didn't
        nop;nop;
        
        I3=MODE1;
        JUMP (PC, asyn_stop_ints) (DB); // Discourage interrupts
        BIT CLR MODE1 0x1000;      // Disable all interrupts
        NOP;
        
asyn_stop_ints:
        pm_ptr=DM(1,I7);             // get loop stk restore

// At this point, we are going to restore the loop stack. Remember
// that we created a pointer that would point to the address to jump to.
// Well, the JUMP is next, and it will jump to one of the restore stack
// locations below.  When the JUMP occurs, the values will be read from 
// the C runtime stack, and restored to the loop stack.  If some dummy
// saves were made above (the loop stack was not full), then they are
// ignored.  Only valid loop stack information is restored

        JUMP (pm_0, pm_ptr) (DB);
          MODE1=I3;       // Restore state after handler
          ASTAT=R0;        // Write out new ASTAT on 21020, 2106x


// The emulator cannot step over a PUSH LOOP instruction.  If
// the user attempts to step through this code in the emulator, the
// code will NOT function correctly.  To remind users of this problem
// the label `___lib_DO_NOT_EMU_STEPn' where `n' is a unique number so
// that the labels do not clash.

___asyn_lib_DO_NOT_EMU_STEP1:PUSH LOOP;

#if WA_09000023
            R0 = DM(2,I7);
            CURLCNTR = R0;
            R0 = DM(3,I7);
            LADDR = R0;
#else
            CURLCNTR=DM(2,I7);
            LADDR=DM(3,I7);
#endif

___asyn_lib_DO_NOT_EMU_STEP2:PUSH LOOP;


#if WA_09000023
            R0 = DM(4,I7);
            CURLCNTR = R0;
            R0 = DM(5,I7);
            LADDR = R0;
#else
            CURLCNTR=DM(4,I7);
            LADDR=DM(5,I7);
#endif

___asyn_lib_DO_NOT_EMU_STEP3:PUSH LOOP;

#if WA_09000023
            R0 = DM(6,I7);
            CURLCNTR = R0;
            R0 = DM(7,I7);
            LADDR = R0;
#else
            CURLCNTR=DM(6,I7);
            LADDR=DM(7,I7);
#endif

___asyn_lib_DO_NOT_EMU_STEP4:PUSH LOOP;

#if WA_09000023
            R0 = DM(8,I7);
            CURLCNTR = R0;
            R0 = DM(9,I7);
            LADDR = R0;
#else
            CURLCNTR=DM(8,I7);
            LADDR=DM(9,I7);
#endif

___asyn_lib_DO_NOT_EMU_STEP5:PUSH LOOP;

#if WA_09000023
            R0 = DM(10,I7);
            CURLCNTR = R0;
            R0 = DM(11,I7);
            LADDR = R0;
#else
            CURLCNTR=DM(10,I7);
            LADDR=DM(11,I7);
#endif

___asyn_lib_DO_NOT_EMU_STEP6:PUSH LOOP;

#if WA_09000023
            R0 = DM(12,I7);
            CURLCNTR = R0;
            R0 = DM(13,I7);
            LADDR = R0;
#else
            CURLCNTR=DM(12,I7);
            LADDR=DM(13,I7);
#endif

asyn_empty_stack:
#if WA_09000023
            R0 = DM(14,I7);
            LCNTR = R0;
#else
            LCNTR=DM(14,I7);
#endif
			
            modify(i7, 14);		//before ___restore_all_regs , need to alter() the regs saved after ___backup_all_regs

// px1, px2 were saved as part of the Vector Code
		        
		bit clr MODE1 0x1000;	// Disable int
        nop;
        R1=DM(2,I7);		// Read LIRPTL old
        R2 = LIRPTL;	// R2 = LIRPTL current
        R2 = R2 or R1;	// OR current and old IRPTL
        LIRPTL = R2;	// Update LIRPTL
        nop;nop;
        
        R1=DM(3,I7);		// Read IMASK old
        R2 = IMASK;		// R2 = IMASK current
        R2 = R2 or R1;	// OR current and old IMASK
        IMASK = R2;		// Update IMASK
        nop;nop;
        
        px2=DM(5,I7);
      	px1=DM(6,I7);
        modify(i7, 6);
        
        bit clr MODE1 0x04fc;	// Disable second register
        nop;nop;
        pop loop;				// pop loop stack
        nop;
        pop sts;				// pop sts stack
        bit clr MODE1 0x1000;	// Disable int
        nop;
//
// Till now, Interrupt handler end.
//
        JUMP (PC, .asyn_return_from_int);

// Then we need to judge...
// 1. need to switch context in int, jump to ___asm_switch_context_int
// we need not to save the return PC, because we never come back
// we need to use R4, R8, I12, so we have to restore them 
		
//		JUMP ___backup_stks_regs;
           
//
// 2. normal Interrupt, return_from_int 
//		     
.asyn_return_from_int:   
	  	DM(-1,I7)=I12;			// Save I12
        I12 = PCSTK;			// get return PC counter
        pop pcstk;				// pop the return PC counter
        
        BIT CLR IMASKP 0xffffffff;
        nop;nop;
        BIT CLR LIRPTL 0xfff00000;
        nop;nop;
        
        
// End of the Interrupt,RTI       
// Notice: BIT CLR IMASKP must done after JUMP!!!
//
        JUMP (pm_0, pm_ptr) (DB);
        I12 = DM(-1,I7);		// restore I12
        bit set MODE1 0x1000;	// Enable int
        
.___asyn_interrupt_handler.end:


/*
@----实时中断-------------------------------------------------------------------
@功能:  
@参数:  
@返回:  
@函数原型:___real_interrupt_handler;
@-------------------------------------------------------------------------------
*/
/* real interrupt handler */

.GLOBAL    ___real_interrupt_handler;

___real_interrupt_handler:
		
	  modify(i7, -8);
      DM(7,I7)=px1;
      DM(6,I7)=px2;
	  DM(5,I7)=R3;				  // Save R3
	  DM(4,I7)=R1;				  // Save R1
	  R1=IMASK;DM(3,I7)=R1;		  // Save IMASK
	  R1=LIRPTL;DM(2,I7)=R1;     	  // Save LIRPTL
//	  DM(1,I7)=R1;				  // align
	  BIT CLR IMASK real_mask_imask;	  // MASK all the asyn ints and IRQ2 int
	  nop;nop;
	  BIT CLR LIRPTL real_mask_lirptl;  // MASK all the asyn ints
	  nop;nop;
	  
// Start to restore the registers that we saved.

//		call ___backup_all_regs;

		call ___backup_real_int_regs;
        
// Here is the section of code that saves the loop stacks to memory
// before calling the handler.  The technique is as follows:  R12 points
// to the address of the place to go if there were NO loop stack slots
// used.  The various registers are saved, and if the Loop Stack Empty
// bit is NOT set int STKYx, R12 is updated with the address of the code
// that performs one additional loop stack restore.
// This means that instead of just setting LCNTR, the first loop stack
// spot will be restored.
// The maximum number of loop stack spots will be saved to the C runtime
// stack, but the R12 pointer will only be changed if the stack is
// not empty.  The critical step is the stack restore.  See below for
// more details.

        R12=real_empty_stack;         // Go here for no stack

        modify(i7, -14);
        
        DM(14,I7)=LCNTR;              // Save loop counter
        DM(13,I7)=LADDR;              // Save loop address
        DM(12,I7)=CURLCNTR;           // Save current counter

        BIT TST STKYx 0x04000000; // Test for empty stack
            
        IF TF JUMP (PC, .real_save_loop_address1);
                                 // Loop stack is empty
        POP LOOP;                // Pop down loop stack
        R12=___real_lib_DO_NOT_EMU_STEP6;  // Performs 1 loop stack restore

.real_save_loop_address1:
        DM(11,I7)=LADDR;              // Save loop address
        DM(10,I7)=CURLCNTR;           // Save current counter
        BIT TST STKYx 0x04000000; // Test for empty stack
        IF TF JUMP (PC, .real_save_loop_address2);
                                 // Loop stack is empty
        POP LOOP;                // Pop down loop stack
        R12=___real_lib_DO_NOT_EMU_STEP5;  // Performs 2 loop stack restores

.real_save_loop_address2:
        DM(9,I7)=LADDR;              // Save loop address
        DM(8,I7)=CURLCNTR;           // Save current counter
        BIT TST STKYx 0x04000000; // Test for empty stack
        IF TF JUMP (PC, .real_save_loop_address3);
                                 // Set address
        POP LOOP;                // Pop down loop stack
        R12=___real_lib_DO_NOT_EMU_STEP4;  // Performs 3 loop stack restores

.real_save_loop_address3:
        DM(7,I7)=LADDR;              // Save loop address
        DM(6,I7)=CURLCNTR;           // Save current counter
        BIT TST STKYx 0x04000000; // Test for empty stack
        IF TF JUMP (PC, .real_save_loop_address4);
                                 // Set address
        POP LOOP;                // Pop down loop stack
        R12=___real_lib_DO_NOT_EMU_STEP3;  // Performs 4 loop stack restores

.real_save_loop_address4:
        DM(5,I7)=LADDR;              // Save loop address
        DM(4,I7)=CURLCNTR;           // Save current counter
        BIT TST STKYx 0x04000000; // Test for empty stack
        IF TF JUMP (PC, .real_save_loop_address5);
                                 // Set address
        POP LOOP;                // Pop down loop stack
        R12=___real_lib_DO_NOT_EMU_STEP2;  // Performs 5 loop stack restores

.real_save_loop_address5:
        DM(3,I7)=LADDR;              // Save loop address
        DM(2,I7)=CURLCNTR;           // Save current counter
        BIT TST STKYx 0x04000000; // Test for empty stack
        IF TF JUMP (PC, .real_determine_int_num);
                                 // Set address
        POP LOOP;                // Pop down loop stack
        R12=___real_lib_DO_NOT_EMU_STEP1;  // Performs 6 loop stack restores

.real_determine_int_num:

        /*
        ** Determine interrupt number.
        */
        /* Work out the interrupt from the address of the interrupt
        ** table index currently held in I2.
        */
        
        DM(1,I7)=R12;        			// save number of loop stack restores
        
        R4 = R2;					// R4 = ufl_line_hard NOW


        I13=real_int_cont;               // People may need this still     
        pm_ptr=___Int_EngineReal;	// Interrupt engine all 
        R2=frame_ptr;				// R2=frame_ptr;
        frame_ptr=stack_ptr;		// frame_ptr=stack_ptr;
        JUMP(pm_0, pm_ptr) (DB); 	// Call the handler JUMP(pm_0, pm_ptr) (DB);
           DM(I7,M7)=R2;			// DM(I7,M7)=R2;
           DM(I7,M7)=PC;			// DM(I7,M7)=PC;

           
           

           
                      
// Here, we calculate the new value of IMASK and assign it.
// For 21020 and 2106x, we also need to save the current FLAGS bits,
// restore the old value of ASTAT, and then mask in the saved FLAGS bits. 

real_int_cont:
		
        bit clr mode1 PEYEN;       // Clear SIMD mode, in case the user didn't
        nop;
        nop;
        I3=MODE1;
        JUMP (PC, real_stop_ints) (DB); // Discourage interrupts
        BIT CLR MODE1 0x1000;      // Disable all interrupts
        NOP;
        
real_stop_ints:
        pm_ptr=DM(1,I7);             // get loop stk restore



// At this point, we are going to restore the loop stack. Remember
// that we created a pointer that would point to the address to jump to.
// Well, the JUMP is next, and it will jump to one of the restore stack
// locations below.  When the JUMP occurs, the values will be read from 
// the C runtime stack, and restored to the loop stack.  If some dummy
// saves were made above (the loop stack was not full), then they are
// ignored.  Only valid loop stack information is restored

        JUMP (pm_0, pm_ptr) (DB);
          MODE1=I3;       // Restore state after handler

          ASTAT=R0;        // Write out new ASTAT on 21020, 2106x


// The emulator cannot step over a PUSH LOOP instruction.  If
// the user attempts to step through this code in the emulator, the
// code will NOT function correctly.  To remind users of this problem
// the label `___lib_DO_NOT_EMU_STEPn' where `n' is a unique number so
// that the labels do not clash.

___real_lib_DO_NOT_EMU_STEP1:PUSH LOOP;

#if WA_09000023
            R0 = DM(2,I7);
            CURLCNTR = R0;
            R0 = DM(3,I7);
            LADDR = R0;
#else
            CURLCNTR=DM(2,I7);
            LADDR=DM(3,I7);
#endif

___real_lib_DO_NOT_EMU_STEP2:PUSH LOOP;


#if WA_09000023
            R0 = DM(4,I7);
            CURLCNTR = R0;
            R0 = DM(5,I7);
            LADDR = R0;
#else
            CURLCNTR=DM(4,I7);
            LADDR=DM(5,I7);
#endif

___real_lib_DO_NOT_EMU_STEP3:PUSH LOOP;

#if WA_09000023
            R0 = DM(6,I7);
            CURLCNTR = R0;
            R0 = DM(7,I7);
            LADDR = R0;
#else
            CURLCNTR=DM(6,I7);
            LADDR=DM(7,I7);
#endif

___real_lib_DO_NOT_EMU_STEP4:PUSH LOOP;

#if WA_09000023
            R0 = DM(8,I7);
            CURLCNTR = R0;
            R0 = DM(9,I7);
            LADDR = R0;
#else
            CURLCNTR=DM(8,I7);
            LADDR=DM(9,I7);
#endif

___real_lib_DO_NOT_EMU_STEP5:PUSH LOOP;

#if WA_09000023
            R0 = DM(10,I7);
            CURLCNTR = R0;
            R0 = DM(11,I7);
            LADDR = R0;
#else
            CURLCNTR=DM(10,I7);
            LADDR=DM(11,I7);
#endif

___real_lib_DO_NOT_EMU_STEP6:PUSH LOOP;

#if WA_09000023
            R0 = DM(12,I7);
            CURLCNTR = R0;
            R0 = DM(13,I7);
            LADDR = R0;
#else
            CURLCNTR=DM(12,I7);
            LADDR=DM(13,I7);
#endif

real_empty_stack:
#if WA_09000023
            R0 = DM(14,I7);
            LCNTR = R0;
#else
            LCNTR=DM(14,I7);
#endif
			
            modify(i7, 14);		//before ___restore_all_regs , need to alter() the regs saved after ___backup_all_regs



// Start to restore the registers that we saved.
       
//		call ___restore_all_regs;
		
		call ___restore_real_int_regs;

// r2,  px1, px2, r1 were saved as part of the Vector Code
		
        R1=DM(2,I7);		// Read LIRPTL old
        R2 = LIRPTL;		// R2 = LIRPTL current
        R2 = R2 or R1;	// OR current and old LIRPTL
        LIRPTL = R2;		// Update LIRPTL
        nop;nop;
        
        R1=DM(3,I7);		// Read IMASK old
        R2 = IMASK;		// R2 = IMASK current
        R2 = R2 or R1;	// OR current and old IMASK
        IMASK = R2;		// Update IMASK
        nop;nop;
        
        R1=DM(4,I7);		// restore R1
        R3=DM(5,I7);		// restore R3
        px2=DM(6,I7);
      	px1=DM(7,I7);
        R2=DM(8,I7);		// restore R2
        modify(i7, 8);
        
        pop loop;		// pop loop stack
        nop;
        
        RTI ;			// go back to where it comes from
        
.___real_interrupt_handler.end:

/*
@----硬件栈+寄存器保存-----------------------------------------------------------------
@功能:  
@参数:  
@返回:  
@函数原型:___backup_stks_regs;
@-------------------------------------------------------------------------------
*/
// save all the registers and staks needed. 
// JUMP to this during Thread switch in asyn Interrupt

.GLOBAL    ___backup_stks_regs;

___backup_stks_regs:        		// pc stk, sts stk, loop stk , all_regs

	bit clr MODE1 0x1000;	// 

//初始化栈指针
//	no need here

//将PC返回值压栈
	DM(I7,M7)=pcstk;					//
	
//通用寄存器压栈(含MODE2,USTAT1-4)
	call ___backup_all_regs;    
	
//开始压硬件栈
	pop pcstk;					//清理掉进中断时push的一次pc值
	R0 = pcstkp;
	BIT TST STKYx 0x00400000;
	IF TF JUMP (PC, .save_pc_end);
.save_pc_start:					//若pcstkp！=0，则依次读pc，并入栈
	DM(I7,M7)=pcstk;
	pop pcstk;
	BIT TST STKYx 0x00400000;
	IF NOT TF JUMP (PC, .save_pc_start);
.save_pc_end:					
	DM(I7,M7)=R0;					//将R0=pcstkp入栈，在出栈时需根据此值判定循环次数
	//pc入栈结束，status入栈开始
	R0 = 1;
	R2 = MODE1;DM(I7,M7)=R2;			//存当前MODE1、ASTATx、ASTATy
	R2 = ASTAT;DM(I7,M7)=R2;
	R2 = 0x00000000;DM(I7,M7)=R2;
	BIT TST STKYx 0x01000000;
	IF TF JUMP (PC, .save_sts_end);
.save_sts_start:
	pop sts;					//出硬件栈
	R2 = MODE1;DM(I7,M7)=R2;			//存MODE1、ASTATx、ASTATy
	BIT CLR MODE1 0x1000;		//注意，未免发生意外，此处将全局中断关闭，在退出中断线程切换函数时打开
	R2 = ASTAT;DM(I7,M7)=R2;
	R2 = 0x00000000;DM(I7,M7)=R2;
	R0 = R0 + 1;
	BIT TST STKYx 0x01000000;
	IF NOT TF JUMP (PC, .save_sts_start);
.save_sts_end:					
	DM(I7,M7)=R0;					//将R0入栈，在出栈时需根据此值判定循环次数
	//status入栈结束，loop入栈开始
	R0 = 1;
	DM(I7,M7)=LCNTR;              	// Save loop counter
	DM(I7,M7)=LADDR;             	// Save loop address
	DM(I7,M7)=CURLCNTR;          	// Save current counter
	BIT TST STKYx 0x04000000;
	IF TF JUMP (PC, .save_loop_end);
.save_loop_start:
	pop loop;
	DM(I7,M7)=LCNTR;              	// Save loop counter
	DM(I7,M7)=LADDR;             	// Save loop address
	DM(I7,M7)=CURLCNTR;          	// Save current counter
	R0 = R0 + 1;
	BIT TST STKYx 0x04000000;
	IF NOT TF JUMP (PC, .save_loop_start);
.save_loop_end:					//loop入栈结束
	DM(I7,M7)=R0;					//将R0入栈，在出栈时需根据此值判定循环次数
//至此，存硬件栈结束,所有压栈操作完成

		bit clr MODE1 0x1000;	// must Disable int
	  	DM(-1,I7)=I12;			// Save I12
		I12 = ___asm_switch_context_int;
		R4 = 0xf1;		// new_vm
		R8 = 0xf2;		// old_vm
        JUMP (pm_0, pm_ptr) (DB);
        I12 = DM(-1,I7);		// restore I12
        nop;

.___backup_stks_regs.end:

/*
@----寄存器操作-----------------------------------------------------------------
@功能:  
@参数:  
@返回:  
@函数原型:
@-------------------------------------------------------------------------------
*/
.SECTION/PM    seg_pmco;

// RETURN imask
.GLOBAL    ___read_imask_seg;
___read_imask_seg:
	R0=IMASK;
	I12=dm(M7,I6);
	JUMP(M14,I12) (DB);
	rframe;
	nop;
.___read_imask_seg.end:

// RETURN lirptl
.GLOBAL    ___read_lirptl_seg;
___read_lirptl_seg:
	R0=LIRPTL;
	I12=dm(M7,I6);
	JUMP(M14,I12) (DB);
	rframe;
	nop;
.___read_lirptl_seg.end:

// RETURN irptl
.GLOBAL    ___read_irptl_seg;
___read_irptl_seg:
	R0=IRPTL;
	I12=dm(M7,I6);
	JUMP(M14,I12) (DB);
	rframe;
	nop;
.___read_irptl_seg.end:

// OR old imask and R4
.GLOBAL    ___set_imask_seg;
___set_imask_seg:
	R0=IMASK;
	R4=R0 or R4;
	IMASK=R4;
	nop;nop;
	I12=dm(M7,I6);
	JUMP(M14,I12) (DB);
	rframe;
	nop;
.___set_imask_seg.end:

// OR old lirptl and R4
.GLOBAL    ___set_lirptl_seg;
___set_lirptl_seg:
	R0=LIRPTL;
	R4=R0 or R4;
	LIRPTL=R4;
	nop;nop;
	I12=dm(M7,I6);
	JUMP(M14,I12) (DB);
	rframe;
	nop;
.___set_lirptl_seg.end:

// OR old irptl and R4
.GLOBAL    ___set_irptl_seg;
___set_irptl_seg:
	R0=IRPTL;
	R4=R0 or R4;
	IRPTL=R4;
	nop;nop;
	I12=dm(M7,I6);
	JUMP(M14,I12) (DB);
	rframe;
	nop;
.___set_irptl_seg.end:

// AND old imask and ~R4
.GLOBAL    ___clr_imask_seg;
___clr_imask_seg:
	R0=IMASK;
	R4=NOT R4;
	R4=R0 and R4;
	IMASK=R4;
	I12=dm(M7,I6);
	JUMP(M14,I12) (DB);
	rframe;
	nop;
.___clr_imask_seg.end:

// AND old lirptl and ~R4
.GLOBAL    ___clr_lirptl_seg;
___clr_lirptl_seg:
	R0=LIRPTL;
	R4=NOT R4;
	R4=R0 and R4;
	LIRPTL=R4;
	nop;nop;
	I12=dm(M7,I6);
	JUMP(M14,I12) (DB);
	rframe;
	nop;
.___clr_lirptl_seg.end:

// AND old irptl and ~R4
.GLOBAL    ___clr_irptl_seg;
___clr_irptl_seg:
	R0=IRPTL;
	R4=NOT R4;
	R4=R0 and R4;
	IRPTL=R4;
	nop;nop;
	I12=dm(M7,I6);
	JUMP(M14,I12) (DB);
	rframe;
	nop;
.___clr_irptl_seg.end:


/*
@----除硬件栈以外的寄存器保存---------------------------------------------------
@功能:  
@参数:  
@返回:  
@函数原型:___backup_all_regs;
@-------------------------------------------------------------------------------
*/

// save all the registers needed. 
// Call this during Interrupt and Thread switch
// Can not be interrupt by any interrupt

.GLOBAL    ___backup_all_regs;

___backup_all_regs:        		// 117 regs in total

        modify(i7, -120);		// for align
        // save r1, r2, i2, i13 at first
        DM(reg_backup_offset+120,I7)=R1;
        DM(reg_backup_offset+119,I7)=R2;
        R1=I2;
        DM(reg_backup_offset+118,I7)=R1;
        DM(reg_backup_offset+117,I7)=I13;
        R1=I6;
        DM(reg_backup_offset+116,I7)=R1;
        
        // Save MODE2/MMASK/STKYx/STKYy USTAT1-4/TPERIOD/TCOUNT
        DM(reg_backup_offset+115,I7)=MMASK;
        I13=MODE2;
        DM(reg_backup_offset+114,I7)=I13;
        I13 = STKYx;
        DM(reg_backup_offset+113,I7)=I13;
        DM(reg_backup_offset+112,I7)=STKYy;
        R1 = USTAT1;
        DM(reg_backup_offset+111,I7)=R1;
        R1 = USTAT2;
        DM(reg_backup_offset+110,I7)=R1;
        R1 = USTAT3;
        DM(reg_backup_offset+109,I7)=R1;
        R1 = USTAT4;
        DM(reg_backup_offset+108,I7)=R1;
    
        // Save and zero all the relevant L registers
        R1=L0; 
        DM(reg_backup_offset+107,I7)=R1;
        R1=L1; 
        DM(reg_backup_offset+106,I7)=R1;
        R1=L2; 
        DM(reg_backup_offset+105,I7)=R1;
        R1=L3; 
        DM(reg_backup_offset+104,I7)=R1;
        R1=L5; 
        DM(reg_backup_offset+103,I7)=R1;
        DM(reg_backup_offset+102,I7)=L8;
        DM(reg_backup_offset+101,I7)=L9;
        DM(reg_backup_offset+100,I7)=L10;
        DM(reg_backup_offset+99,I7)=L11;
        DM(reg_backup_offset+98,I7)=L13;
        DM(reg_backup_offset+97,I7)=L14;
        DM(reg_backup_offset+96,I7)=L15;
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
        DM(reg_backup_offset+95,I7)=px1;
        DM(reg_backup_offset+94,I7)=px2;

// All 40 bits of these registers must be saved. They are used in
// the integer divide routine and loss of accuracy in these registers
// will result in the wrong answer being generated.
        PX=R0; 
        DM(reg_backup_offset+93,I7)=px1; 
        DM(reg_backup_offset+92,I7)=px2;
        PX=R3; 
        DM(reg_backup_offset+91,I7)=px1;
        DM(reg_backup_offset+90,I7)=px2;
        PX=R4; 
        DM(reg_backup_offset+89,I7)=px1; 
        DM(reg_backup_offset+88,I7)=px2;
        PX=R7; 
        DM(reg_backup_offset+87,I7)=px1; 
        DM(reg_backup_offset+86,I7)=px2;
        PX=R8; 
        DM(reg_backup_offset+85,I7)=px1; 
        DM(reg_backup_offset+84,I7)=px2;
        PX=R9; 
        DM(reg_backup_offset+83,I7)=px1; 
        DM(reg_backup_offset+82,I7)=px2;
        PX=R11; 
        DM(reg_backup_offset+81,I7)=px1; 
        DM(reg_backup_offset+80,I7)=px2;
        PX=R12; 
        DM(reg_backup_offset+79,I7)=px1; 
        DM(reg_backup_offset+78,I7)=px2;
	// R1\R2 saved below        
        DM(reg_backup_offset+77,I7)=R5;
        DM(reg_backup_offset+76,I7)=R6;
        DM(reg_backup_offset+75,I7)=R10;
        DM(reg_backup_offset+74,I7)=R13;
        DM(reg_backup_offset+73,I7)=R14;
        DM(reg_backup_offset+72,I7)=R15;

// Save all of the Sregs. We can't do these saves and restores in
// SIMD mode in case the stack is in external memory.
// Note that on 2116x and above, we no longer save MODE1 and ASTAT
// - they are saved by the 'push sts' instruction in the vector table.
        PX=S0; 
        DM(reg_backup_offset+71,I7)=px1; 
        DM(reg_backup_offset+70,I7)=px2;
        PX=S4;
        DM(reg_backup_offset+69,I7)=px1; 
        DM(reg_backup_offset+68,I7)=px2;
        PX=S9; 
        DM(reg_backup_offset+67,I7)=px1; 
        DM(reg_backup_offset+66,I7)=px2;
        PX=S11; 
        DM(reg_backup_offset+65,I7)=px1; 
        DM(reg_backup_offset+64,I7)=px2;
        PX=S12; 
        DM(reg_backup_offset+63,I7)=px1; 
        DM(reg_backup_offset+62,I7)=px2;
        DM(reg_backup_offset+61,I7)=S1;
        DM(reg_backup_offset+60,I7)=S2;
        DM(reg_backup_offset+59,I7)=S3;
        DM(reg_backup_offset+58,I7)=S5;
        DM(reg_backup_offset+57,I7)=S6;
        DM(reg_backup_offset+56,I7)=S7;
        DM(reg_backup_offset+55,I7)=S8;
        DM(reg_backup_offset+54,I7)=S10;
        DM(reg_backup_offset+53,I7)=S13;
        DM(reg_backup_offset+52,I7)=S14;
        DM(reg_backup_offset+51,I7)=S15;
        BIT SET MODE1 RND32;
        nop;nop;

// Save the relevant DAG1 M registers - M0-3. M4 is saved later, M5-7 are
// constant.
        R15=M0;
        DM(reg_backup_offset+50,I7)=R15;
        R15=M1;
        DM(reg_backup_offset+49,I7)=R15;
        R15=M2;
        DM(reg_backup_offset+48,I7)=R15;
        R15=M3;
        DM(reg_backup_offset+47,I7)=R15;

// Save the relevant DAG2 M registers - M8-11. M12 is saved later, M13-15
// are constant.

        DM(reg_backup_offset+46,I7)=M8;
        DM(reg_backup_offset+45,I7)=M9;
        DM(reg_backup_offset+44,I7)=M10;
        DM(reg_backup_offset+43,I7)=M11;


// Save the Iregs
        R15=I0;
        DM(reg_backup_offset+42,I7)=R15;
        R15=I1;
        DM(reg_backup_offset+41,I7)=R15;
        R15=I3;
        DM(reg_backup_offset+40,I7)=R15;
        R15=I5;
        DM(reg_backup_offset+39,I7)=R15;


        DM(reg_backup_offset+38,I7)=I8;
        DM(reg_backup_offset+37,I7)=I9;
        DM(reg_backup_offset+36,I7)=I10;
        DM(reg_backup_offset+35,I7)=I11;
        DM(reg_backup_offset+34,I7)=I14;
        DM(reg_backup_offset+33,I7)=I15;

 

// Save the scratch DAG registers on the stack.
        DM(reg_backup_offset+32,I7)=pm_ptr;   // save i12
        DM(reg_backup_offset+31,I7)=pm_bse;   // save b12
        DM(reg_backup_offset+30,I7)=pm_mdf;   // save m12
        DM(reg_backup_offset+29,I7)=pm_lnt;   // save l12
        R0=dm_ptr;
        DM(reg_backup_offset+28,I7)=R0;       // save i4
        R0=dm_bse;
        DM(reg_backup_offset+27,I7)=R0;       // save b4
        R0=dm_mdf; 
        DM(reg_backup_offset+26,I7)=R0;       // save m4
        R0=dm_lnt;
        DM(reg_backup_offset+25,I7)=R0;       // save l4
        
        //save B registers
        R1=B0;   DM(reg_backup_offset+24,I7)=R1;   
        R1=B1;   DM(reg_backup_offset+23,I7)=R1;
        R1=B2;   DM(reg_backup_offset+22,I7)=R1;
        R1=B3;   DM(reg_backup_offset+21,I7)=R1;
        R1=B5;   DM(reg_backup_offset+20,I7)=R1;
        
        DM(reg_backup_offset+19,I7)=B8;
        DM(reg_backup_offset+18,I7)=B9;
        DM(reg_backup_offset+17,I7)=B10;
        DM(reg_backup_offset+16,I7)=B11;
        DM(reg_backup_offset+15,I7)=B13;
        DM(reg_backup_offset+14,I7)=B14;
        DM(reg_backup_offset+13,I7)=B15;

// Save the MRF registers. We save them here so that the offset of their
// locations is within 32 words of the final stack pointer. This allows us,
// on 2106x at least, to  combine the get() instructions with another
// instruction when we restore them. Note that we can't directly access the
// shadow MRF registers on 2116x so we have to use S regs to get the values.
// This adds to the cycle count 

        bit set mode1 PEYEN;
        nop;nop;
        R10=MR0F;
        R11=MR1F;
        R12=MR2F;
        R13=MR0B;
        R14=MR1B;
        R15=MR2B;
        bit clr mode1 PEYEN;
        nop;nop;

        DM(reg_backup_offset+12,I7)=R10;
        DM(reg_backup_offset+11,I7)=R11;
        DM(reg_backup_offset+10,I7)=R12;
        DM(reg_backup_offset+9,I7)=R13;
        DM(reg_backup_offset+8,I7)=R14;
        DM(reg_backup_offset+7,I7)=R15;
        DM(reg_backup_offset+6,I7)=S10;
        DM(reg_backup_offset+5,I7)=S11;
        DM(reg_backup_offset+4,I7)=S12;
        DM(reg_backup_offset+3,I7)=S13;
        DM(reg_backup_offset+2,I7)=S14;
        DM(reg_backup_offset+1,I7)=S15;

        RTS;
.___backup_all_regs.end:	// 116 regs in total
        

/*
@----除硬件栈以外的寄存器恢复---------------------------------------------------
@功能:  
@参数:  
@返回:  
@函数原型:___restore_all_regs;
@-------------------------------------------------------------------------------
*/
// restore all the registers saved. 
// Call this during Interrupt and Thread switch

.GLOBAL    ___restore_all_regs;

___restore_all_regs:		// 116 regs in total
// We can't access the shadow mrf registers directly, so we have to go
// through this long-winded process.
      
        bit set mode1 PEYEN;
        nop;nop;
        
        S15=DM(reg_restore_offset+1,I7);
        S14=DM(reg_restore_offset+2,I7);
        S13=DM(reg_restore_offset+3,I7);
        S12=DM(reg_restore_offset+4,I7);
        S11=DM(reg_restore_offset+5,I7);
        S10=DM(reg_restore_offset+6,I7);
        R15=DM(reg_restore_offset+7,I7); 
        R14=DM(reg_restore_offset+8,I7);
        R13=DM(reg_restore_offset+9,I7);
        R12=DM(reg_restore_offset+10,I7);
        R11=DM(reg_restore_offset+11,I7);
        R10=DM(reg_restore_offset+12,I7);
         
        MR0B=r13;
        MR1B=r14;
        MR2B=r15;
        MR0F=r10;
        MR1F=r11;
        MR2F=r12;
        bit clr mode1 PEYEN;
        nop;nop;
        
        
        //restore B registers
         B15=DM(reg_restore_offset+13,I7);
         B14=DM(reg_restore_offset+14,I7);
         B13=DM(reg_restore_offset+15,I7);
         B11=DM(reg_restore_offset+16,I7);
         B10=DM(reg_restore_offset+17,I7);
         B9=DM(reg_restore_offset+18,I7);
         B8=DM(reg_restore_offset+19,I7);
         R1=DM(reg_restore_offset+20,I7);B5=R1;
         R1=DM(reg_restore_offset+21,I7);B3=R1;
         R1=DM(reg_restore_offset+22,I7);B2=R1;
         R1=DM(reg_restore_offset+23,I7);B1=R1;
         R1=DM(reg_restore_offset+24,I7);B0=R1;   
  
        dm_lnt=DM(reg_restore_offset+25,I7);      // Restore DM DAG scratch
        dm_mdf=DM(reg_restore_offset+26,I7);
        dm_bse=DM(reg_restore_offset+27,I7);      // Be sure to write base before writing ptr
        dm_ptr=DM(reg_restore_offset+28,I7);

        pm_lnt=DM(reg_restore_offset+29,I7);      // Restore PM DAG scratch
        pm_mdf=DM(reg_restore_offset+30,I7);
        pm_bse=DM(reg_restore_offset+31,I7);
        pm_ptr=DM(reg_restore_offset+32,I7);

        I15=DM(reg_restore_offset+33,I7);
        I14=DM(reg_restore_offset+34,I7);
        I11=DM(reg_restore_offset+35,I7);
        I10=DM(reg_restore_offset+36,I7);
        I9=DM(reg_restore_offset+37,I7);
        I8=DM(reg_restore_offset+38,I7);
        I5=DM(reg_restore_offset+39,I7);
        I3=DM(reg_restore_offset+40,I7);
        I1=DM(reg_restore_offset+41,I7);
        I0=DM(reg_restore_offset+42,I7);

        M11=DM(reg_restore_offset+43,I7);
        M10=DM(reg_restore_offset+44,I7);
        M9=DM(reg_restore_offset+45,I7);
        M8=DM(reg_restore_offset+46,I7);
        M3=DM(reg_restore_offset+47,I7);
        M2=DM(reg_restore_offset+48,I7);
        M1=DM(reg_restore_offset+49,I7);
        M0=DM(reg_restore_offset+50,I7);

        S15=DM(reg_restore_offset+51,I7);
        S14=DM(reg_restore_offset+52,I7);
        S13=DM(reg_restore_offset+53,I7);
        S10=DM(reg_restore_offset+54,I7);
        S8=DM(reg_restore_offset+55,I7);
        S7=DM(reg_restore_offset+56,I7);
        S6=DM(reg_restore_offset+57,I7);
        S5=DM(reg_restore_offset+58,I7);
        S3=DM(reg_restore_offset+59,I7);
        S2=DM(reg_restore_offset+60,I7);
        S1=DM(reg_restore_offset+61,I7);

        // Restore the 40-bit S registers
        PX2=DM(reg_restore_offset+62,I7); PX1=DM(reg_restore_offset+63,I7); S12=PX;
        PX2=DM(reg_restore_offset+64,I7); PX1=DM(reg_restore_offset+65,I7); S11=PX;
        PX2=DM(reg_restore_offset+66,I7); PX1=DM(reg_restore_offset+67,I7); S9=PX;
        PX2=DM(reg_restore_offset+68,I7); PX1=DM(reg_restore_offset+69,I7); S4=PX;
        PX2=DM(reg_restore_offset+70,I7); PX1=DM(reg_restore_offset+71,I7); S0=PX;

        R15=DM(reg_restore_offset+72,I7);
        R14=DM(reg_restore_offset+73,I7);
        R13=DM(reg_restore_offset+74,I7);
        R10=DM(reg_restore_offset+75,I7);
        R6=DM(reg_restore_offset+76,I7);
        R5=DM(reg_restore_offset+77,I7);

        // Restore the 40-bit registers and the PX registers
        PX2=DM(reg_restore_offset+78,I7); PX1=DM(reg_restore_offset+79,I7); R12=PX;
        PX2=DM(reg_restore_offset+80,I7); PX1=DM(reg_restore_offset+81,I7); R11=PX;
        PX2=DM(reg_restore_offset+82,I7); PX1=DM(reg_restore_offset+83,I7); R9=PX;
        PX2=DM(reg_restore_offset+84,I7); PX1=DM(reg_restore_offset+85,I7); R8=PX;
        PX2=DM(reg_restore_offset+86,I7); PX1=DM(reg_restore_offset+87,I7); R7=PX;
        PX2=DM(reg_restore_offset+88,I7); PX1=DM(reg_restore_offset+89,I7); R4=PX;
        PX2=DM(reg_restore_offset+90,I7); PX1=DM(reg_restore_offset+91,I7); R3=PX;
        PX2=DM(reg_restore_offset+92,I7); PX1=DM(reg_restore_offset+93,I7); R0=PX;
        PX2=DM(reg_restore_offset+94,I7);
        PX1=DM(reg_restore_offset+95,I7);
        
        // Restore the L registers
        L15=DM(reg_restore_offset+96,I7);
        L14=DM(reg_restore_offset+97,I7);
        L13=DM(reg_restore_offset+98,I7);
        L11=DM(reg_restore_offset+99,I7);
        L10=DM(reg_restore_offset+100,I7);
        L9=DM(reg_restore_offset+101,I7);
        L8=DM(reg_restore_offset+102,I7);
        L5=DM(reg_restore_offset+103,I7);
        L3=DM(reg_restore_offset+104,I7);
        L2=DM(reg_restore_offset+105,I7);
        L1=DM(reg_restore_offset+106,I7);
        L0=DM(reg_restore_offset+107,I7);

        R1=DM(reg_restore_offset+108,I7);
        USTAT4 = R1;
        nop;
        R1=DM(reg_restore_offset+109,I7);
        USTAT3 = R1;
        nop;
        R1=DM(reg_restore_offset+110,I7);
        USTAT2 = R1;
        nop;
        R1=DM(reg_restore_offset+111,I7);
        USTAT1 = R1;
        nop;
        I13=DM(reg_restore_offset+112,I7);
        STKYy = I13;
        nop;
        nop;
        I13=DM(reg_restore_offset+113,I7);
        STKYx = I13;
        nop;
        nop;
       I13=DM(reg_restore_offset+114,I7);
       MODE2=I13;
       nop;
       nop;
        MMASK=DM(reg_restore_offset+115,I7);
        nop;
        I6=DM(reg_restore_offset+116,I7);
        I13=DM(reg_restore_offset+117,I7);
        I2=DM(reg_restore_offset+118,I7);
        R2=DM(reg_restore_offset+119,I7);
        R1=DM(reg_restore_offset+120,I7);
        
        modify(i7, 120);		// for align
        
        RTS;
        
.___restore_all_regs.end:		// 116 regs in total
 
/*
@----实时中断过程中的寄存器保存-------------------------------------------------
@功能:  
@参数:  
@返回:  
@函数原型:___backup_real_int_regs;
@-------------------------------------------------------------------------------
*/
// save registers needed to save during real Interrupt 
// Call this during real Interrupt
// Can not be interrupt by any interrupt

.GLOBAL    ___backup_real_int_regs;

___backup_real_int_regs:        		// 26 regs in total

        modify(i7, -26);		// for align

		// save r1, r2, i2, i13 at first
        DM(reg_backup_offset+26,I7)=R1;
        DM(reg_backup_offset+25,I7)=R2;
        
        R1=I2; DM(reg_backup_offset+24,I7)=R1;
        DM(reg_backup_offset+23,I7)=I13;
        
        // Save STKYx USTAT1-4
        R1 = USTAT1;DM(reg_backup_offset+22,I7)=R1;
        R1 = USTAT2;DM(reg_backup_offset+21,I7)=R1;

        // Save PX registers
        DM(reg_backup_offset+20,I7)=px1;
        DM(reg_backup_offset+19,I7)=px2;

// All 40 bits of these registers must be saved. They are used in
// the integer divide routine and loss of accuracy in these registers
// will result in the wrong answer being generated.
        PX=R0; DM(reg_backup_offset+18,I7)=px1; DM(reg_backup_offset+17,I7)=px2;
        PX=R4; DM(reg_backup_offset+16,I7)=px1; DM(reg_backup_offset+15,I7)=px2;
        PX=R8; DM(reg_backup_offset+14,I7)=px1; DM(reg_backup_offset+13,I7)=px2;
        PX=R12; DM(reg_backup_offset+12,I7)=px1; DM(reg_backup_offset+11,I7)=px2;

// Save the Iregs
        R0=I3;
        DM(reg_backup_offset+10,I7)=R0;

// Save the scratch DAG registers on the stack.
        DM(reg_backup_offset+9,I7)=pm_ptr;   // save i12
        DM(reg_backup_offset+8,I7)=pm_bse;   // save b12
        DM(reg_backup_offset+7,I7)=pm_mdf;   // save m12
        DM(reg_backup_offset+6,I7)=pm_lnt;   // save l12
        R0=dm_ptr;
        DM(reg_backup_offset+5,I7)=R0;       // save i4
        R0=dm_bse;
        DM(reg_backup_offset+4,I7)=R0;       // save b4
        R0=dm_mdf; 
        DM(reg_backup_offset+3,I7)=R0;       // save m4
        R0=dm_lnt;
        DM(reg_backup_offset+2,I7)=R0;       // save l4
        
        DM(reg_backup_offset+1,I7)=B13;

        RTS;
.___backup_real_int_regs.end:	// 26 regs in total
        

/*
@----实时中断过程中的寄存器恢复-------------------------------------------------
@功能:  
@参数:  
@返回:  
@函数原型:___restore_real_int_regs;
@-------------------------------------------------------------------------------
*/
// restore registers needed to save during real Interrupt 
// Call this during real Interrupt
// Can not be interrupt by any interrupt

.GLOBAL    ___restore_real_int_regs;

___restore_real_int_regs:		// 26 regs in total
// We can't access the shadow mrf registers directly, so we have to go
// through this long-winded process.

        B13=DM(reg_restore_offset+1,I7);


        dm_lnt=DM(reg_restore_offset+2,I7);      // Restore DM DAG scratch  l4
        dm_mdf=DM(reg_restore_offset+3,I7);		// m4
        dm_bse=DM(reg_restore_offset+4,I7);      // Be sure to write base before writing ptr b4
        dm_ptr=DM(reg_restore_offset+5,I7);		// i4

        pm_lnt=DM(reg_restore_offset+6,I7);      // Restore PM DAG scratch l12
        pm_mdf=DM(reg_restore_offset+7,I7);		// m12
        pm_bse=DM(reg_restore_offset+8,I7);		// b12
        pm_ptr=DM(reg_restore_offset+9,I7);		// i12


        I3=DM(reg_restore_offset+10,I7);


        // Restore the 40-bit registers and the PX registers
        PX2=DM(reg_restore_offset+11,I7); PX1=DM(reg_restore_offset+12,I7); R12=PX;

        PX2=DM(reg_restore_offset+13,I7); PX1=DM(reg_restore_offset+14,I7); R8=PX;

        PX2=DM(reg_restore_offset+15,I7); PX1=DM(reg_restore_offset+16,I7); R4=PX;

        PX2=DM(reg_restore_offset+17,I7); PX1=DM(reg_restore_offset+18,I7); R0=PX;
        PX2=DM(reg_restore_offset+19,I7);
        PX1=DM(reg_restore_offset+20,I7);
        
        R1=DM(reg_restore_offset+21,I7);USTAT2 = R1;
        nop;
        R1=DM(reg_restore_offset+22,I7);USTAT1 = R1;
        nop;

        
        I13=DM(reg_restore_offset+23,I7);
        I2=DM(reg_restore_offset+24,I7);
        R2=DM(reg_restore_offset+25,I7);
        R1=DM(reg_restore_offset+26,I7);
        
        modify(i7, 26);		// for align
        
        RTS;
        
.___restore_real_int_regs.end:		// 26 regs in total





/*
@----------------------------------------------------
@功能: 异常处理(IICDI/CB7I/CB15I)
@参数:  
@返回:  无
@函数原型:void ___sys_general_exception(void)
@-------------------------------------------------------------------------------
*/

.GLOBAL ___sys_general_exception;

___sys_general_exception:
    	
//先将R1/R2存入内存中，接下来要使用到他们作为中间寄存器
     DM(Exception_StartAddress)=R2;    //存储异常向量编号
     DM(Exception_StartAddress+1)=R1;
  //Because CB7I/CB15I priority is lower than the real-interrupt, in order 
  //to avoiding interrupt by the real-interrupt,disable the global interrupt firstly. 
     R1=MODE1;
     DM(Exception_StartAddress +2)=R1;
     BIT CLR MODE1 0x1000;   //Disable interrupts  
     nop;nop; 
     
     R1=MODE2;  DM(Exception_StartAddress +3)=R1;  
     R1=USTAT1; DM(Exception_StartAddress +4)=R1;
     R1=USTAT2; DM(Exception_StartAddress +5)=R1;
     R1=USTAT3; DM(Exception_StartAddress +6)=R1;
     R1=USTAT4; DM(Exception_StartAddress +7)=R1;
     
     
     R1=0x1b;  //接下来要判断主异常类型及分支异常类型，IICDI/CB7I/CB15I中断向量号分别为0x0,0x1b,0x1c. 
     R1=R2-R1;
     IF EQ jump (PC,.CB7I_Overflow_exp);  //如果是CB7I则跳到I7_Overflow中处理CB7I异常,否则，继续查询
     R1=0x1c;
     R1=R2-R1;
     IF EQ jump (PC,.CB15I_Overflow_exp);//同上
     
     jump (PC,.IICDI_exp);
     
.CB7I_Overflow_exp:
    R1=0x3;
    DM(Exception_StartAddress +8)=R1;  //写主异常原因编号0x5  
    R1=0x1e;
    DM(Exception_StartAddress +9)=R1;  //写分支异常原因编号0x2e
    jump (PC,.Save_Commom_Message);
    
.CB15I_Overflow_exp:
    R1=0x4;
    DM(Exception_StartAddress +8)=R1;  //写主异常原因编号0x6  
    R1=0x1f;
    DM(Exception_StartAddress +9)=R1;  //写分支异常原因编号0x2f
    jump (PC,.Save_Commom_Message);   
.IICDI_exp:
    R1=0x0;
    DM(Exception_StartAddress +8)=R1;  //写主异常原因编号0x0
    bit tst STKYx IIRA;               //IIRA为Illegal Access Occurred标志位
    IF TF jump (PC,.IICDI_IIRA);
    R1=0x2;                          //unaligned long word access分支异常编号为0x2.
    DM(Exception_StartAddress +9)=R1;
   jump (PC,.Save_Commom_Message);  
.IICDI_IIRA:
   R1=0x1;
   DM(Exception_StartAddress +9)=R1;
   jump (PC,.Save_Commom_Message);  
   
   //接下来依次保存，发生异常时PC指针，栈指针I7,ASTATx,STKYx,IRPTL,IMASK等寄存器，R/S/I/M/B/L等通用寄存器值等。
.Save_Commom_Message:     
    R1=pcstk;
    DM(Exception_StartAddress +0xa)=R1;
    //DM(Exception_StartAddress +0xb)=I7;
    DM(Exception_StartAddress +0xb)=ASTATx;
    DM(Exception_StartAddress +0xc)=ASTATy;
    
    DM(Exception_StartAddress +0xd)=STKYx;
    DM(Exception_StartAddress +0xe)=STKYy;
    DM(Exception_StartAddress +0xf)=IRPTL;
    DM(Exception_StartAddress +0x10)=LIRPTL;
    DM(Exception_StartAddress +0x11)=IMASK;

    //开始保存通用寄存器值,首先是R registers
    DM(Exception_StartAddress +0x12)=PX1;
    DM(Exception_StartAddress +0x13)=PX2;
    PX=R0;
    DM(Exception_StartAddress +0x14)=PX1;DM(Exception_StartAddress +0x15)=PX2;
    PX=R3;
    DM(Exception_StartAddress +0x16)=PX1;DM(Exception_StartAddress +0x17)=PX2;
    PX=R4;
    DM(Exception_StartAddress +0x18)=PX1;DM(Exception_StartAddress +0x19)=PX2;
    DM(Exception_StartAddress +0x1a)=R5;
    DM(Exception_StartAddress +0x1b)=R6;
    PX=R7;
    DM(Exception_StartAddress +0x1c)=PX1;DM(Exception_StartAddress +0x1d)=PX2;
    PX=R8;
    DM(Exception_StartAddress +0x1e)=PX1;DM(Exception_StartAddress +0x1f)=PX2;
    PX=R9;
    DM(Exception_StartAddress +0x20)=PX1;DM(Exception_StartAddress +0x21)=PX2;
    DM(Exception_StartAddress +0x22)=R10;
    PX=R11;
    DM(Exception_StartAddress +0x23)=PX1;DM(Exception_StartAddress +0x24)=PX2;
    PX=R12;
    DM(Exception_StartAddress +0x25)=PX1;DM(Exception_StartAddress +0x26)=PX2;
    DM(Exception_StartAddress +0x27)=R13;
    DM(Exception_StartAddress +0x28)=R14;
    DM(Exception_StartAddress +0x29)=R15;
    //S registers
    PX=S0;
    DM(Exception_StartAddress +0x2a)=PX1;DM(Exception_StartAddress +0x2b)=PX2;
    DM(Exception_StartAddress +0x2c)=S1;
    DM(Exception_StartAddress +0x2d)=S2;
    DM(Exception_StartAddress +0x2e)=S3;
    PX=S4;
    DM(Exception_StartAddress +0x2f)=PX1;DM(Exception_StartAddress +0x30)=PX2;
    DM(Exception_StartAddress +0x31)=S5;
    DM(Exception_StartAddress+0x32 )=S6;
    DM(Exception_StartAddress +0x33)=S7;
    DM(Exception_StartAddress +0x34)=S8;
    PX=S9;
    DM(Exception_StartAddress +0x35)=PX1;DM(Exception_StartAddress +0x36)=PX2;
    DM(Exception_StartAddress +0x37)=S10;
    PX=S11;
    DM(Exception_StartAddress +0x38)=PX1;DM(Exception_StartAddress +0x39)=PX2;
    PX=S12;
    DM(Exception_StartAddress +0x3a)=PX1;DM(Exception_StartAddress +0x3b)=PX2;
    DM(Exception_StartAddress +0x3c)=S13;
    DM(Exception_StartAddress +0x3d)=S14;
    DM(Exception_StartAddress +0x3e)=S15;
   //I register
    DM(Exception_StartAddress +0x3f)=I0;
    DM(Exception_StartAddress +0x40)=I1;
    DM(Exception_StartAddress +0x41)=I2;
    DM(Exception_StartAddress +0x42)=I3;
    DM(Exception_StartAddress +0x43)=I4;
    DM(Exception_StartAddress +0x44)=I5;
    DM(Exception_StartAddress +0x45)=I6;
    DM(Exception_StartAddress +0x46)=I7;
    DM(Exception_StartAddress +0x47)=I8;
    DM(Exception_StartAddress +0x48)=I9;
    DM(Exception_StartAddress +0x49)=I10;
    DM(Exception_StartAddress +0x4a)=I11;
    DM(Exception_StartAddress +0x4b)=I12;
    DM(Exception_StartAddress +0x4c)=I13;
    DM(Exception_StartAddress +0x4d)=I14;
    DM(Exception_StartAddress +0x4e)=I15;
    //M registers
    DM(Exception_StartAddress +0x4f)=M0;
    DM(Exception_StartAddress +0x50)=M1;
    DM(Exception_StartAddress +0x51)=M2;
    DM(Exception_StartAddress +0x52)=M3;
    DM(Exception_StartAddress +0x53)=M4;
    DM(Exception_StartAddress +0x54)=M5;
    DM(Exception_StartAddress +0x55)=M6;
    DM(Exception_StartAddress +0x56)=M7;
    DM(Exception_StartAddress +0x57)=M8;
    DM(Exception_StartAddress +0x58)=M9;
    DM(Exception_StartAddress +0x59)=M10;
    DM(Exception_StartAddress +0x5a)=M11;
    DM(Exception_StartAddress +0x5b)=M12;
    DM(Exception_StartAddress +0x5c)=M13;
    DM(Exception_StartAddress +0x5d)=M14;
    DM(Exception_StartAddress +0x5e)=M15;
    //B registers
    DM(Exception_StartAddress +0x5f)=B0;
    DM(Exception_StartAddress +0x60)=B1;
    DM(Exception_StartAddress +0x61)=B2;
    DM(Exception_StartAddress +0x62)=B3;
    DM(Exception_StartAddress +0x63)=B4;
    DM(Exception_StartAddress +0x64)=B5;
    DM(Exception_StartAddress +0x65)=B6;
    DM(Exception_StartAddress +0x66)=B7;
    DM(Exception_StartAddress +0x67)=B8;
    DM(Exception_StartAddress +0x68)=B9;
    DM(Exception_StartAddress +0x69)=B10;
    DM(Exception_StartAddress +0x6a)=B11;
    DM(Exception_StartAddress +0x6b)=B12;
    DM(Exception_StartAddress +0x6c)=B13;
    DM(Exception_StartAddress +0x6d)=B14;
    DM(Exception_StartAddress +0x6e)=B15;
    // L registers
    DM(Exception_StartAddress +0x6f)=L0;
    DM(Exception_StartAddress +0x70)=L1;
    DM(Exception_StartAddress +0x71)=L2;
    DM(Exception_StartAddress +0x72)=L3;
    DM(Exception_StartAddress +0x73)=L4;
    DM(Exception_StartAddress +0x74)=L5;
    DM(Exception_StartAddress +0x75)=L6;
    DM(Exception_StartAddress +0x76)=L7;
    DM(Exception_StartAddress +0x77)=L8;
    DM(Exception_StartAddress +0x78)=L9;
    DM(Exception_StartAddress +0x79)=L10;
    DM(Exception_StartAddress +0x7a)=L11;
    DM(Exception_StartAddress +0x7b)=L12;
    DM(Exception_StartAddress +0x7c)=L13;
    DM(Exception_StartAddress +0x7d)=L14;
    DM(Exception_StartAddress +0x7e)=L15;
    // Save the MRF registers. We save them here so that the offset of their
// locations is within 32 words of the final stack pointer. This allows us,
// on 2106x at least, to  combine the get() instructions with another
// instruction when we restore them. Note that we can't directly access the
// shadow MRF registers on 2116x so we have to use S regs to get the values.
// This adds to the cycle count 
    bit set mode1 PEYEN;
    nop;nop;
    R10=MR0F;
    R11=MR1F;
    R12=MR2F;
    R13=MR0B;
    R14=MR1B;
    R15=MR2B;
    bit clr mode1 PEYEN;
    nop;nop;
    DM(Exception_StartAddress +0x7f)=R10;
    DM(Exception_StartAddress +0x80)=R11;
    DM(Exception_StartAddress +0x81)=R12;
    DM(Exception_StartAddress +0x82)=R13;
    DM(Exception_StartAddress +0x83)=R14;
    DM(Exception_StartAddress +0x84)=R15;
    DM(Exception_StartAddress +0x85)=S10;
    DM(Exception_StartAddress +0x86)=S11;
    DM(Exception_StartAddress +0x87)=S12;
    DM(Exception_StartAddress +0x88)=S13;
    DM(Exception_StartAddress +0x89)=S14;
    DM(Exception_StartAddress +0x8a)=S15;
    //上面保存的是各个异常都要保存的通用信息，对于IICDI/CB7I/CB15I等异常还需要保存发生异常执行的指令前后50条指令。
    R1=DM(Exception_StartAddress +0xa);  //取发生异常时PC指针值
    I2=R1;
    R1=Exception_StartAddress;
    R2=0x8b;
    R2=R1+R2;
    I3=R2;
    R3=0x32;
    R5=0;
.Save_general_Other:
    R2=0x0;
    R1=DM(I2,M7);     //取PC指令
    DM(I3,M5)=R1;     //存PC指令
    R2=R2+1;
    R5=R3-R2;        //判断是否存完50条指令
    IF EQ jump(PC,.Save_general_Other);
    //写异常信息头信息
    
    
    R1=0x87654321;
    DM(Exception_StartAddress-5)=R1;
    R1=DM(Exception_StartAddress);
    DM(Exception_StartAddress-4)=R1;
    R1=DM(Exception_StartAddress+9);
    DM(Exception_StartAddress-3)=R1;
    R1=Exception_StartAddress-5;
    DM(Exception_StartAddress-2)=R1;
    R1=0xc0;
    DM(Exception_StartAddress-1)=R1;
    
    
    
     B7 = exp_stack_space;
     I7 = (  (exp_stack_space + exp_stack_length - 1)
      		- ((exp_stack_space + exp_stack_length - 1) % 2) );
     L7 = (  exp_stack_length
            - ((exp_stack_space + exp_stack_length - 1) % 2) );
     B6 = B7;
     I6 = I7;
     L6 = L7;
    
   
    
    
    
    
    
    
    
    
    
//下面分别将主异常编号，分支异常编号及保存信息深度(sizes)作为参数传递给sys_SOVFI_exp_dealer，转入C模式处理.
    R4=DM(Exception_StartAddress+8);
    R8=DM(Exception_StartAddress +9);
    R12=0xc2;
    pm_ptr=_Exp_General;
    JUMP(M13,pm_ptr);
.___sys_general_exception.end:

/*
@----------------------------------------------------
@功能: 异常处理(硬件栈溢出异常处理)
@参数:  
@返回:  无
@函数原型:void ___sys_HardStack_exception(void)
@-------------------------------------------------------------------------------
*/

.GLOBAL ___sys_HardStack_exception;

___sys_HardStack_exception:
     	
//首先存储异常中断向量号R2及主异常编号，由于12个异常中断向量号非连续，所以不能直接将R2的值作为主异常编号。
     DM(Exception_StartAddress)=R2;    //存储异常向量编号
//首先保存R1寄存器值，接下来要使用R1作为中间寄存器传递参数
     DM(Exception_StartAddress+1)=R1;
//由于CB7I/CB15I等异常中断优先级较低，有可能会被实时信号打断，这是不允许的，故首先保存MODEI，然后关闭全局中断。
     R1=MODE1;
     DM(Exception_StartAddress +2)=R1;
     
     
     BIT CLR MODE1 0x1000;   //Disable interrupts 
     nop;nop;
     R1=MODE2;  DM(Exception_StartAddress +3)=R1;  
     R1=USTAT1; DM(Exception_StartAddress +4)=R1;
     R1=USTAT2; DM(Exception_StartAddress +5)=R1;
     R1=USTAT3; DM(Exception_StartAddress +6)=R1;
     R1=USTAT4; DM(Exception_StartAddress +7)=R1;
     R1=0x1; 
     DM(Exception_StartAddress +8)=R1; //保存主异常编号(major exp type)
 //每个主异常可能由多个分支异常造成，必须要标记分支异常标号(minor exp type) 
//对于IICDI/SOVFI/FIXI/FLTOI/FLTUI/FLTII六个异常在STKYx寄存器中相应标志位有标记,通过查询
//STKYx寄存器标志位即可查明异常具体原因，将异常具体原因编号(minor exp type)作为现场信息保存。
   bit tst STKYx PCFL;          //PCFL bit为PC Stack Full标志位，Full则为1，否则为0。
   IF TF jump (PC,.SOVFI_PC); 
   bit tst STKYx SSOV;         //SSOV bit为Status Stack Overflow标志位，Full则为1，否则为0。
   IF TF jump (PC,.SOVFI_Status);
   R1=0x5;                    //SOVFI异常的Loop Stack Overflow minor exp type编号为0x5
   DM(Exception_StartAddress +9)=R1; 
   jump (PC,.Save_HardStack_Commom_Message); 
.SOVFI_PC:
   R1=0x3;                    //SOVFI异常的PC Stack Full minor exp type编号为0x3
   DM(Exception_StartAddress +9)=R1;   
   jump (PC,.Save_HardStack_Commom_Message); 
.SOVFI_Status:
   R1=0x4;                   //SOVFI异常的Status Stack Overflow minor exp type编号为0x4
   DM(Exception_StartAddress +9)=R1;
        
.Save_HardStack_Commom_Message:     
//接下来依次保存发生异常时PC指针，栈指针I7,ASTATx,STKYx,IRPTL,IMASK等寄存器，R/S/I/M/B/L等通用寄存器值等。
    R1=pcstk;
    DM(Exception_StartAddress +0xa)=R1;
     //DM(Exception_StartAddress +0xb)=I7;
    DM(Exception_StartAddress +0xb)=ASTATx;
    DM(Exception_StartAddress +0xc)=ASTATy;
    
    DM(Exception_StartAddress +0xd)=STKYx;
    DM(Exception_StartAddress +0xe)=STKYy;
    DM(Exception_StartAddress +0xf)=IRPTL;
    DM(Exception_StartAddress +0x10)=LIRPTL;
    DM(Exception_StartAddress +0x11)=IMASK;

    //开始保存通用寄存器值,首先是R registers
    DM(Exception_StartAddress +0x12)=PX1;
    DM(Exception_StartAddress +0x13)=PX2;
    PX=R0;
    DM(Exception_StartAddress +0x14)=PX1;DM(Exception_StartAddress +0x15)=PX2;
    PX=R3;
    DM(Exception_StartAddress +0x16)=PX1;DM(Exception_StartAddress +0x17)=PX2;
    PX=R4;
    DM(Exception_StartAddress +0x18)=PX1;DM(Exception_StartAddress +0x19)=PX2;
    DM(Exception_StartAddress +0x1a)=R5;
    DM(Exception_StartAddress +0x1b)=R6;
    PX=R7;
    DM(Exception_StartAddress +0x1c)=PX1;DM(Exception_StartAddress +0x1d)=PX2;
    PX=R8;
    DM(Exception_StartAddress +0x1e)=PX1;DM(Exception_StartAddress +0x1f)=PX2;
    PX=R9;
    DM(Exception_StartAddress +0x20)=PX1;DM(Exception_StartAddress +0x21)=PX2;
    DM(Exception_StartAddress +0x22)=R10;
    PX=R11;
    DM(Exception_StartAddress +0x23)=PX1;DM(Exception_StartAddress +0x24)=PX2;
    PX=R12;
    DM(Exception_StartAddress +0x25)=PX1;DM(Exception_StartAddress +0x26)=PX2;
    DM(Exception_StartAddress +0x27)=R13;
    DM(Exception_StartAddress +0x28)=R14;
    DM(Exception_StartAddress +0x29)=R15;
    //S registers
    PX=S0;
    DM(Exception_StartAddress +0x2a)=PX1;DM(Exception_StartAddress +0x2b)=PX2;
    DM(Exception_StartAddress +0x2c)=S1;
    DM(Exception_StartAddress +0x2d)=S2;
    DM(Exception_StartAddress +0x2e)=S3;
    PX=S4;
    DM(Exception_StartAddress +0x2f)=PX1;DM(Exception_StartAddress +0x30)=PX2;
    DM(Exception_StartAddress +0x31)=S5;
    DM(Exception_StartAddress+0x32 )=S6;
    DM(Exception_StartAddress +0x33)=S7;
    DM(Exception_StartAddress +0x34)=S8;
    PX=S9;
    DM(Exception_StartAddress +0x35)=PX1;DM(Exception_StartAddress +0x36)=PX2;
    DM(Exception_StartAddress +0x37)=S10;
    PX=S11;
    DM(Exception_StartAddress +0x38)=PX1;DM(Exception_StartAddress +0x39)=PX2;
    PX=S12;
    DM(Exception_StartAddress +0x3a)=PX1;DM(Exception_StartAddress +0x3b)=PX2;
    DM(Exception_StartAddress +0x3c)=S13;
    DM(Exception_StartAddress +0x3d)=S14;
    DM(Exception_StartAddress +0x3e)=S15;
   //I register
    DM(Exception_StartAddress +0x3f)=I0;
    DM(Exception_StartAddress +0x40)=I1;
    DM(Exception_StartAddress +0x41)=I2;
    DM(Exception_StartAddress +0x42)=I3;
    DM(Exception_StartAddress +0x43)=I4;
    DM(Exception_StartAddress +0x44)=I5;
    DM(Exception_StartAddress +0x45)=I6;
    DM(Exception_StartAddress +0x46)=I7;
    DM(Exception_StartAddress +0x47)=I8;
    DM(Exception_StartAddress +0x48)=I9;
    DM(Exception_StartAddress +0x49)=I10;
    DM(Exception_StartAddress +0x4a)=I11;
    DM(Exception_StartAddress +0x4b)=I12;
    DM(Exception_StartAddress +0x4c)=I13;
    DM(Exception_StartAddress +0x4d)=I14;
    DM(Exception_StartAddress +0x4e)=I15;
    //M registers
    DM(Exception_StartAddress +0x4f)=M0;
    DM(Exception_StartAddress +0x50)=M1;
    DM(Exception_StartAddress +0x51)=M2;
    DM(Exception_StartAddress +0x52)=M3;
    DM(Exception_StartAddress +0x53)=M4;
    DM(Exception_StartAddress +0x54)=M5;
    DM(Exception_StartAddress +0x55)=M6;
    DM(Exception_StartAddress +0x56)=M7;
    DM(Exception_StartAddress +0x57)=M8;
    DM(Exception_StartAddress +0x58)=M9;
    DM(Exception_StartAddress +0x59)=M10;
    DM(Exception_StartAddress +0x5a)=M11;
    DM(Exception_StartAddress +0x5b)=M12;
    DM(Exception_StartAddress +0x5c)=M13;
    DM(Exception_StartAddress +0x5d)=M14;
    DM(Exception_StartAddress +0x5e)=M15;
    //B registers
    DM(Exception_StartAddress +0x5f)=B0;
    DM(Exception_StartAddress +0x60)=B1;
    DM(Exception_StartAddress +0x61)=B2;
    DM(Exception_StartAddress +0x62)=B3;
    DM(Exception_StartAddress +0x63)=B4;
    DM(Exception_StartAddress +0x64)=B5;
    DM(Exception_StartAddress +0x65)=B6;
    DM(Exception_StartAddress +0x66)=B7;
    DM(Exception_StartAddress +0x67)=B8;
    DM(Exception_StartAddress +0x68)=B9;
    DM(Exception_StartAddress +0x69)=B10;
    DM(Exception_StartAddress +0x6a)=B11;
    DM(Exception_StartAddress +0x6b)=B12;
    DM(Exception_StartAddress +0x6c)=B13;
    DM(Exception_StartAddress +0x6d)=B14;
    DM(Exception_StartAddress +0x6e)=B15;
    // L registers
    DM(Exception_StartAddress +0x6f)=L0;
    DM(Exception_StartAddress +0x70)=L1;
    DM(Exception_StartAddress +0x71)=L2;
    DM(Exception_StartAddress +0x72)=L3;
    DM(Exception_StartAddress +0x73)=L4;
    DM(Exception_StartAddress +0x74)=L5;
    DM(Exception_StartAddress +0x75)=L6;
    DM(Exception_StartAddress +0x76)=L7;
    DM(Exception_StartAddress +0x77)=L8;
    DM(Exception_StartAddress +0x78)=L9;
    DM(Exception_StartAddress +0x79)=L10;
    DM(Exception_StartAddress +0x7a)=L11;
    DM(Exception_StartAddress +0x7b)=L12;
    DM(Exception_StartAddress +0x7c)=L13;
    DM(Exception_StartAddress +0x7d)=L14;
    DM(Exception_StartAddress +0x7e)=L15;
    // Save the MRF registers. We save them here so that the offset of their
// locations is within 32 words of the final stack pointer. This allows us,
// on 2106x at least, to  combine the get() instructions with another
// instruction when we restore them. Note that we can't directly access the
// shadow MRF registers on 2116x so we have to use S regs to get the values.
// This adds to the cycle count 
    bit set mode1 PEYEN;
    nop;nop;
    R10=MR0F;
    R11=MR1F;
    R12=MR2F;
    R13=MR0B;
    R14=MR1B;
    R15=MR2B;
    bit clr mode1 PEYEN;
    nop;nop;
    DM(Exception_StartAddress +0x7f)=R10;
    DM(Exception_StartAddress +0x80)=R11;
    DM(Exception_StartAddress +0x81)=R12;
    DM(Exception_StartAddress +0x82)=R13;
    DM(Exception_StartAddress +0x83)=R14;
    DM(Exception_StartAddress +0x84)=R15;
    DM(Exception_StartAddress +0x85)=S10;
    DM(Exception_StartAddress +0x86)=S11;
    DM(Exception_StartAddress +0x87)=S12;
    DM(Exception_StartAddress +0x88)=S13;
    DM(Exception_StartAddress +0x89)=S14;
    DM(Exception_StartAddress +0x8a)=S15;
//对于SOVFI异常除了通用信息外，还需要将四个硬件栈中所有信息保存,由于当某个硬件栈出现异常时，
//再对该硬件栈出栈时，STKYx相应标志位不会自动改变，所以需要判断具体是哪个硬件栈出现异常，然后针对其单独出栈。  
    R7=0;
    R1=Exception_StartAddress;
    R2=0x8b;
    R2=R1+R2;
    I2=R2;
    R1=DM(Exception_StartAddress +9);  //标记造成异常具体原因的编号1/2/3
    R2=0;      //标记栈使用深度
    R5=0x3;    
    R6=R1-R5;
    IF EQ JUMP(PC,.save_PC_exp);
  //开始将硬件栈内容写进指定内存中
	R2 = pcstkp;          //如果不是PC Stack Full异常，则按常规将PC Stack 出栈并保存。
	BIT TST STKYx PCEM;
	IF TF JUMP (PC, .save_pc_end_exp);
.save_pc_start_exp:				       	//若pcstkp！=0，则依次读pc，并入栈
    R0=pcstk;
    DM(I2,M6)=R0;
    pop pcstk;
	BIT TST STKYx PCEM;
	IF NOT TF JUMP (PC, .save_pc_start_exp);
    JUMP(PC,.save_pc_end_exp);
.save_PC_exp:
    R2=R2+1;
    pop pcstk;
    R3=pcstk;
	DM(I2,M6)=R3;	// Save loop counter
	R3=0x1e;        //PC Stack满栈深度为30.
	R3=R2-R3;
	IF EQ JUMP(PC,.save_pc_end_exp);
	JUMP(PC,.save_PC_exp);	
			
.save_pc_end_exp:			
	DM(I2,M6)=R2;	//将R0=pcstkp写入内存区，直到PC栈使用的层数			
	//PC栈写进内存区结束，开始写Status Stack
	R7=R7+R2;
	R7=R7+1;   //切记除了R2个PC stack深度，表示栈深的数也压栈了。
	R2=0;
	R5=0x4;
    R1=R1-R5;
    IF EQ JUMP(PC,.save_sts_exp);
	bit tst STKYx SSEM;
	IF TF JUMP(PC,.save_sts_end_exp);
.save_sts_start_exp:
    pop sts;					//出Status栈,存MODE1、ASTATx、ASTATy
	R3 = MODE1;	
	DM(I2,M6)=R3;		
	R3 = ASTAT;
	DM(I2,M6)=R3;		
	R3 = 0x00000000;
	DM(I2,M6)=R3;	
	R2 = R2 + 1;
	BIT TST STKYx SSEM;
	IF NOT TF JUMP (PC, .save_sts_start_exp);
	JUMP(PC,.save_sts_end_exp);	
.save_sts_exp:
    R2=R2+1;
    pop sts;
    R3=MODE1;
	DM(I2,M6)=R3;	// Save loop counter
	R3=ASTAT;	
	DM(I2,M6)=R3;	// Save loop address
	R3 = 0x00000000;
	DM(I2,M6)=R3;	// Save current counter	
	R3=0xF;
	R3=R2-R3;
	IF EQ JUMP(PC,.save_sts_end_exp);
	JUMP(PC,.save_sts_exp);
		
.save_sts_end_exp:					
	DM(I2,M6)=R2;//将R2入栈，标示Loop栈使用的深度
	R1=R2;
	R2=R2+R1;
	R2=R2+R1;   //切记，一层sts stack 压3个寄存器
	R7=R7+R2;
	R7=R7+1;
	R2=0;
	R5=0x5;
    R1=DM(Exception_StartAddress +9);  //标记造成异常具体原因的编号1/2/3
    R1=R1-R5;
    IF EQ JUMP(PC,.save_loop_exp);
	bit tst STKYx LSEM;
	IF TF JUMP(PC,.save_loop_end_exp);
.save_loop_start_exp:
    pop loop;
    R3=LCNTR;
	DM(I2,M6)=R3;	// Save loop counter
	R3=LADDR;	
	DM(I2,M6)=R3;	// Save loop address
	R3=CURLCNTR;
	DM(I2,M6)=R3;	// Save current counter
	R2 = R2 + 1;
	BIT TST STKYx LSEM;
	IF NOT TF JUMP (PC, .save_loop_start_exp);
    JUMP(PC,.save_loop_end_exp);
.save_loop_exp:
    R2=R2+1;
    pop loop;
    R3=LCNTR;
	DM(I2,M6)=R3;	// Save loop counter
	R3=LADDR;	
	DM(I2,M6)=R3;	// Save loop address
	R3=CURLCNTR;
	DM(I2,M6)=R3;	// Save current counter	
	R3=0x6;
	R3=R2-R3;
	IF EQ JUMP(PC,.save_loop_end_exp);
	JUMP(PC,.save_loop_exp);
.save_loop_end_exp:					//loop入栈结束				
	DM(I2,M6)=R2;//将R2入栈，标示Loop栈使用的深度
	R1=R2;
	R2=R2+R1;
	R2=R2+R1;   //切记，一层sts stack 压3个寄存器
	R7=R7+R2;    //R7为硬件栈深度
	R7=R7+1;
	R1=0x8b;     //通用寄存器已保存0x89
	R7=R7+R1;    //SOVFI异常保存的现场信息总深度,至此，存硬件栈结束。
	R1=0x5;
	R7=R7+R1;
	
	//写异常信息头信息
	R1=0x87654321;
    DM(Exception_StartAddress-5)=R1;
    R1=DM(Exception_StartAddress);
    DM(Exception_StartAddress-4)=R1;
    R1=DM(Exception_StartAddress+9);
    DM(Exception_StartAddress-3)=R1;
    R1=Exception_StartAddress-5;
    DM(Exception_StartAddress-2)=R1;
    R1=R7;
    DM(Exception_StartAddress-1)=R1;
	
	
    
    
     B7 = exp_stack_space;
     I7 = (  (exp_stack_space + exp_stack_length - 1)
      		- ((exp_stack_space + exp_stack_length - 1) % 2) );
     L7 = (  exp_stack_length
            - ((exp_stack_space + exp_stack_length - 1) % 2) );
     B6 = B7;
     I6 = I7;
     L6 = L7;
    
    
    
    
    
 //下面分别将主异常编号，分支异常编号及保存信息深度(sizes)作为参数传递给sys_SOVFI_exp_dealer，转入C模式处理.   
	R4=DM(Exception_StartAddress+8);   
    R8=DM(Exception_StartAddress +9);
    R12=R7;
    pm_ptr=_Exp_General;
    JUMP(M13,pm_ptr);

.___sys_HardStack_exception.end:


/*
@----------------------------------------------------
@功能: 异常处理(计算单元异常处理FIXI/FLTUI/FLTOI/FLTII)
@参数:  
@返回:  无
@函数原型:void ___sys_COMP_exception(void)
@-------------------------------------------------------------------------------
*/

.GLOBAL ___sys_COMP_exception;

___sys_COMP_exception:
     	
//首先存储异常中断向量号R2及主异常编号，由于12个异常中断向量号非连续，所以不能直接将R2的值作为主异常编号。
     DM(Exception_StartAddress)=R2;    //存储异常向量编号
//首先保存R1寄存器值，接下来要使用R1作为中间寄存器传递参数
     DM(Exception_StartAddress+1)=R1;
//由于CB7I/CB15I等异常中断优先级较低，有可能会被实时信号打断，这是不允许的，故首先保存MODEI，然后关闭全局中断。
     R1=MODE1;
     DM(Exception_StartAddress +2)=R1;
     BIT CLR MODE1 0x1000;   //Disable interrupts 
     nop;nop;
     R1=MODE2;  DM(Exception_StartAddress +3)=R1;  
     R1=USTAT1; DM(Exception_StartAddress +4)=R1;
     R1=USTAT2; DM(Exception_StartAddress +5)=R1;
     R1=USTAT3; DM(Exception_StartAddress +6)=R1;
     R1=USTAT4; DM(Exception_StartAddress +7)=R1;
     R1=0x1e;  //接下来要判断主异常类型及分支异常类型，FIXI/FLTUI/FLTOI/FLTII中断向量号分别为0x1e,0x1f,0x20,0x21. 
     R1=R2-R1;
     IF EQ jump (PC,.Fixed_Overflow);  //如果是FIXI则跳到Fixed_Overflow中处理FIXI异常,否则，继续查询
     R1=0x1f;
     R1=R2-R1;
     IF EQ jump (PC,.Floating_Overflow);//同上
     R1=0x20;
     R1=R2-R1;
     IF EQ jump (PC,.Floating_Underflow);//同上
     jump (PC,.Floating_Invalid);
// 通过查询STKYx寄存器标志位即可查明异常具体原因，将异常具体原因编号(minor exp type)作为现场信息保存。 
.Fixed_Overflow:
    R1=0x5; 
    DM(Exception_StartAddress +8)=R1; //保存主异常编号(major exp type)
    bit tst STKYx AOS;   //AOS为FIXI ALU标志位，当异常由ALU造成，则AOS=1
    IF TF jump (PC,.ALU_Fixed_Overflow); //如果AOS=1，则跳到ALU_Fixed_Overflow中写标志位及清零
    R1=0x21;
    DM(Exception_StartAddress +9)=R1;
    bit clr STKYx MOS;   //MOS为FIXI Multplier标志位.
    nop;nop;
    jump (PC,.Save_COMP_Commom_Message);   
.ALU_Fixed_Overflow:
    R1=0x20;
    DM(Exception_StartAddress +9)=R1;  //写分支异常原因编号，FIXI异常 ALU分支异常编号为32.
    bit clr STKYx AOS;     //清标志位
    nop;nop;
    jump (PC,.Save_COMP_Commom_Message);  //开始保存其他通用信息 
  
.Floating_Overflow:
    R1=0x6; 
    DM(Exception_StartAddress +8)=R1; //保存主异常编号(major exp type)
    bit tst STKYx AVS;   //AVS为FLTOI ALU标志位  
    IF TF jump (PC,.ALU_Floating_Overflow);
    R1=0x23;
    DM(Exception_StartAddress +9)=R1;
    bit clr STKYx MVS;   //MVS为FLTOI Multplier标志位
    nop;nop;
    jump (PC,.Save_COMP_Commom_Message);  
.ALU_Floating_Overflow:
    R1=0x22;
    DM(Exception_StartAddress +9)=R1;
    bit clr STKYx AVS;
    nop;nop;
    jump (PC,.Save_COMP_Commom_Message);  
    
.Floating_Underflow:
    R1=0x7; 
    DM(Exception_StartAddress +8)=R1; //保存主异常编号(major exp type)
    bit tst STKYx AUS;
    IF TF jump (PC,.ALU_Floating_Underflow);
    R1=0x25;
    DM(Exception_StartAddress +9)=R1;   
    bit clr STKYx MUS;
    nop;nop;
   jump (PC,.Save_COMP_Commom_Message);  
.ALU_Floating_Underflow:
    R1=0x24;
    DM(Exception_StartAddress +9)=R1;
    bit clr STKYx AUS;
    nop;nop;
    jump (PC,.Save_COMP_Commom_Message);  
    
.Floating_Invalid:
    R1=0x8; 
    DM(Exception_StartAddress +8)=R1; //保存主异常编号(major exp type)
    bit tst STKYx AIS;
    IF TF jump (PC,.ALU_Invalid);
    R1=0x27;
    DM(Exception_StartAddress +9)=R1; 
    bit clr STKYx MIS;
    jump (PC,.Save_COMP_Commom_Message);  
.ALU_Invalid:
    R1=0x26;
    DM(Exception_StartAddress +9)=R1;
    bit clr STKYx AIS; 
    nop;nop;
    jump (PC,.Save_COMP_Commom_Message);  
          
.Save_COMP_Commom_Message:     
//接下来依次保存发生异常时PC指针，栈指针I7,ASTATx,STKYx,IRPTL,IMASK等寄存器，R/S/I/M/B/L等通用寄存器值等。
    R1=pcstk;
    DM(Exception_StartAddress +0xa)=R1;
 //DM(Exception_StartAddress +0xb)=I7;
    DM(Exception_StartAddress +0xb)=ASTATx;
    DM(Exception_StartAddress +0xc)=ASTATy;
    
    DM(Exception_StartAddress +0xd)=STKYx;
    DM(Exception_StartAddress +0xe)=STKYy;
    DM(Exception_StartAddress +0xf)=IRPTL;
    DM(Exception_StartAddress +0x10)=LIRPTL;
    DM(Exception_StartAddress +0x11)=IMASK;

    //开始保存通用寄存器值,首先是R registers
    DM(Exception_StartAddress +0x12)=PX1;
    DM(Exception_StartAddress +0x13)=PX2;
    PX=R0;
    DM(Exception_StartAddress +0x14)=PX1;DM(Exception_StartAddress +0x15)=PX2;
    PX=R3;
    DM(Exception_StartAddress +0x16)=PX1;DM(Exception_StartAddress +0x17)=PX2;
    PX=R4;
    DM(Exception_StartAddress +0x18)=PX1;DM(Exception_StartAddress +0x19)=PX2;
    DM(Exception_StartAddress +0x1a)=R5;
    DM(Exception_StartAddress +0x1b)=R6;
    PX=R7;
    DM(Exception_StartAddress +0x1c)=PX1;DM(Exception_StartAddress +0x1d)=PX2;
    PX=R8;
    DM(Exception_StartAddress +0x1e)=PX1;DM(Exception_StartAddress +0x1f)=PX2;
    PX=R9;
    DM(Exception_StartAddress +0x20)=PX1;DM(Exception_StartAddress +0x21)=PX2;
    DM(Exception_StartAddress +0x22)=R10;
    PX=R11;
    DM(Exception_StartAddress +0x23)=PX1;DM(Exception_StartAddress +0x24)=PX2;
    PX=R12;
    DM(Exception_StartAddress +0x25)=PX1;DM(Exception_StartAddress +0x26)=PX2;
    DM(Exception_StartAddress +0x27)=R13;
    DM(Exception_StartAddress +0x28)=R14;
    DM(Exception_StartAddress +0x29)=R15;
    //S registers
    PX=S0;
    DM(Exception_StartAddress +0x2a)=PX1;DM(Exception_StartAddress +0x2b)=PX2;
    DM(Exception_StartAddress +0x2c)=S1;
    DM(Exception_StartAddress +0x2d)=S2;
    DM(Exception_StartAddress +0x2e)=S3;
    PX=S4;
    DM(Exception_StartAddress +0x2f)=PX1;DM(Exception_StartAddress +0x30)=PX2;
    DM(Exception_StartAddress +0x31)=S5;
    DM(Exception_StartAddress+0x32 )=S6;
    DM(Exception_StartAddress +0x33)=S7;
    DM(Exception_StartAddress +0x34)=S8;
    PX=S9;
    DM(Exception_StartAddress +0x35)=PX1;DM(Exception_StartAddress +0x36)=PX2;
    DM(Exception_StartAddress +0x37)=S10;
    PX=S11;
    DM(Exception_StartAddress +0x38)=PX1;DM(Exception_StartAddress +0x39)=PX2;
    PX=S12;
    DM(Exception_StartAddress +0x3a)=PX1;DM(Exception_StartAddress +0x3b)=PX2;
    DM(Exception_StartAddress +0x3c)=S13;
    DM(Exception_StartAddress +0x3d)=S14;
    DM(Exception_StartAddress +0x3e)=S15;
   //I register
    DM(Exception_StartAddress +0x3f)=I0;
    DM(Exception_StartAddress +0x40)=I1;
    DM(Exception_StartAddress +0x41)=I2;
    DM(Exception_StartAddress +0x42)=I3;
    DM(Exception_StartAddress +0x43)=I4;
    DM(Exception_StartAddress +0x44)=I5;
    DM(Exception_StartAddress +0x45)=I6;
    DM(Exception_StartAddress +0x46)=I7;
    DM(Exception_StartAddress +0x47)=I8;
    DM(Exception_StartAddress +0x48)=I9;
    DM(Exception_StartAddress +0x49)=I10;
    DM(Exception_StartAddress +0x4a)=I11;
    DM(Exception_StartAddress +0x4b)=I12;
    DM(Exception_StartAddress +0x4c)=I13;
    DM(Exception_StartAddress +0x4d)=I14;
    DM(Exception_StartAddress +0x4e)=I15;
    //M registers
    DM(Exception_StartAddress +0x4f)=M0;
    DM(Exception_StartAddress +0x50)=M1;
    DM(Exception_StartAddress +0x51)=M2;
    DM(Exception_StartAddress +0x52)=M3;
    DM(Exception_StartAddress +0x53)=M4;
    DM(Exception_StartAddress +0x54)=M5;
    DM(Exception_StartAddress +0x55)=M6;
    DM(Exception_StartAddress +0x56)=M7;
    DM(Exception_StartAddress +0x57)=M8;
    DM(Exception_StartAddress +0x58)=M9;
    DM(Exception_StartAddress +0x59)=M10;
    DM(Exception_StartAddress +0x5a)=M11;
    DM(Exception_StartAddress +0x5b)=M12;
    DM(Exception_StartAddress +0x5c)=M13;
    DM(Exception_StartAddress +0x5d)=M14;
    DM(Exception_StartAddress +0x5e)=M15;
    //B registers
    DM(Exception_StartAddress +0x5f)=B0;
    DM(Exception_StartAddress +0x60)=B1;
    DM(Exception_StartAddress +0x61)=B2;
    DM(Exception_StartAddress +0x62)=B3;
    DM(Exception_StartAddress +0x63)=B4;
    DM(Exception_StartAddress +0x64)=B5;
    DM(Exception_StartAddress +0x65)=B6;
    DM(Exception_StartAddress +0x66)=B7;
    DM(Exception_StartAddress +0x67)=B8;
    DM(Exception_StartAddress +0x68)=B9;
    DM(Exception_StartAddress +0x69)=B10;
    DM(Exception_StartAddress +0x6a)=B11;
    DM(Exception_StartAddress +0x6b)=B12;
    DM(Exception_StartAddress +0x6c)=B13;
    DM(Exception_StartAddress +0x6d)=B14;
    DM(Exception_StartAddress +0x6e)=B15;
    // L registers
    DM(Exception_StartAddress +0x6f)=L0;
    DM(Exception_StartAddress +0x70)=L1;
    DM(Exception_StartAddress +0x71)=L2;
    DM(Exception_StartAddress +0x72)=L3;
    DM(Exception_StartAddress +0x73)=L4;
    DM(Exception_StartAddress +0x74)=L5;
    DM(Exception_StartAddress +0x75)=L6;
    DM(Exception_StartAddress +0x76)=L7;
    DM(Exception_StartAddress +0x77)=L8;
    DM(Exception_StartAddress +0x78)=L9;
    DM(Exception_StartAddress +0x79)=L10;
    DM(Exception_StartAddress +0x7a)=L11;
    DM(Exception_StartAddress +0x7b)=L12;
    DM(Exception_StartAddress +0x7c)=L13;
    DM(Exception_StartAddress +0x7d)=L14;
    DM(Exception_StartAddress +0x7e)=L15;
    // Save the MRF registers. We save them here so that the offset of their
// locations is within 32 words of the final stack pointer. This allows us,
// on 2106x at least, to  combine the get() instructions with another
// instruction when we restore them. Note that we can't directly access the
// shadow MRF registers on 2116x so we have to use S regs to get the values.
// This adds to the cycle count 
    bit set mode1 PEYEN;
    nop;nop;
    R10=MR0F;
    R11=MR1F;
    R12=MR2F;
    R13=MR0B;
    R14=MR1B;
    R15=MR2B;
    bit clr mode1 PEYEN;
    nop;nop;
    DM(Exception_StartAddress +0x7f)=R10;
    DM(Exception_StartAddress +0x80)=R11;
    DM(Exception_StartAddress +0x81)=R12;
    DM(Exception_StartAddress +0x82)=R13;
    DM(Exception_StartAddress +0x83)=R14;
    DM(Exception_StartAddress +0x84)=R15;
    DM(Exception_StartAddress +0x85)=S10;
    DM(Exception_StartAddress +0x86)=S11;
    DM(Exception_StartAddress +0x87)=S12;
    DM(Exception_StartAddress +0x88)=S13;
    DM(Exception_StartAddress +0x89)=S14;
    DM(Exception_StartAddress +0x8a)=S15;
    
    
  //对于FIXI/FLTUI/FLTOI/FLTII四个异常，除了保存常用的寄存器信息外，同时将发生异常处
     //前50处指令记录下来。   
    
    R1=DM(Exception_StartAddress +0xa);  //取发生异常时PC指针值
    I2=R1;
    R1=Exception_StartAddress;
    R2=0x8b;
    R2=R1+R2;
    I3=R2;
    R3=0x32;
    R5=0;
.Save_general_Other_Fexp:
    R2=0x0;
    R1=DM(I2,M7);     //取PC指令
    DM(I3,M5)=R1;     //存PC指令
    R2=R2+1;
    R5=R3-R2;        //判断是否存完50条指令
    IF EQ jump(PC,.Save_general_Other_Fexp);
    
    
    
    	//写异常信息头信息
	R1=0x87654321;
    DM(Exception_StartAddress-5)=R1;
    R1=DM(Exception_StartAddress);
    DM(Exception_StartAddress-4)=R1;
    R1=DM(Exception_StartAddress+9);
    DM(Exception_StartAddress-3)=R1;
    R1=Exception_StartAddress-5;
    DM(Exception_StartAddress-2)=R1;
    R1=0x90;
    DM(Exception_StartAddress-1)=R1;
    
    
     B7 = exp_stack_space;
     I7 = (  (exp_stack_space + exp_stack_length - 1)
      		- ((exp_stack_space + exp_stack_length - 1) % 2) );
     L7 = (  exp_stack_length
            - ((exp_stack_space + exp_stack_length - 1) % 2) );
     B6 = B7;
     I6 = I7;
     L6 = L7;
    
    
    
    
//通信异常信息保存完毕，对于FIXI/FLTUI/FLTOI/FLTII四个异常其他处理信息暂时不清楚，暂时保留。
//下面分别将主异常编号，分支异常编号及保存信息深度(sizes)作为参数传递给sys_SOVFI_exp_dealer，转入C模式处理.   
	
    R4=DM(Exception_StartAddress+8);   
    R8=DM(Exception_StartAddress +9);
    R12=0xc2;
    R2=I6;      //由于FIXI/FLTUI/FLTOI/FLTII四个异常有可能不用复位经过处理后可以继续运行，所以这里必须可以从C函数中返回.
    I6=I7;
    pm_ptr=_Exp_General;
    JUMP(M13,pm_ptr)(DB);
    
    DM(I7,M7)=R2;
    DM(I7,M7)=PC;
    //如果是FLTUI异常，只记录不复位而是记录完成后返回至原处继续执行。
    //在这里需要先把现场的寄存器恢复
   
   /*  bit set mode1 PEYEN;
     nop;nop;
     
     S15=DM(Exception_StartAddress +0x8a);
     S14=DM(Exception_StartAddress +0x89);
     S13=DM(Exception_StartAddress +0x88);
     S12=DM(Exception_StartAddress +0x87);
     S11=DM(Exception_StartAddress +0x86);
     S10=DM(Exception_StartAddress +0x85);
     R15=DM(Exception_StartAddress +0x84);
     R14=DM(Exception_StartAddress +0x83);
     R13=DM(Exception_StartAddress +0x82);
     R12=DM(Exception_StartAddress +0x81);
     R11=DM(Exception_StartAddress +0x80);
     R10=DM(Exception_StartAddress +0x7f);
          
     MR0F=R10;
     MR1F=R11;
     MR2F=R12;
     MR0B=R13;
     MR1B=R14;
     MR2B=R15;
     bit clr mode1 PEYEN;
     nop;nop;
    
       // L registers
    L15=DM(Exception_StartAddress +0x7e);
    L14=DM(Exception_StartAddress +0x7d);
    L13=DM(Exception_StartAddress +0x7c);
    L12=DM(Exception_StartAddress +0x7b);
    L11=DM(Exception_StartAddress +0x7a);
    L10=DM(Exception_StartAddress +0x79);
    L9=DM(Exception_StartAddress +0x78);
    L8= DM(Exception_StartAddress +0x77);
    L7=DM(Exception_StartAddress +0x76);
    L6=DM(Exception_StartAddress +0x75);
    L5=DM(Exception_StartAddress +0x74);
    L4=DM(Exception_StartAddress +0x73);
    L3=DM(Exception_StartAddress +0x72);
    L2=DM(Exception_StartAddress +0x71);
    L1= DM(Exception_StartAddress +0x70);
    L0=DM(Exception_StartAddress +0x6f);
     
      //B registers
  
    B15=DM(Exception_StartAddress +0x6e);
    B14=DM(Exception_StartAddress +0x6d);
    B13=DM(Exception_StartAddress +0x6c);
    B12=DM(Exception_StartAddress +0x6b);
    B11=DM(Exception_StartAddress +0x6a);
    B10=DM(Exception_StartAddress +0x69);
    B9=DM(Exception_StartAddress +0x68);
    B8=DM(Exception_StartAddress +0x67);
    B7=DM(Exception_StartAddress +0x66);
    B6=DM(Exception_StartAddress +0x65);
    B5=DM(Exception_StartAddress +0x64);
    B4=DM(Exception_StartAddress +0x63);
    B3=DM(Exception_StartAddress +0x62);
    B2=DM(Exception_StartAddress +0x61);
    B1=DM(Exception_StartAddress +0x60);
    B0=DM(Exception_StartAddress +0x5f);
     
      //M registers
    
    M15=DM(Exception_StartAddress +0x5e);
    M14=DM(Exception_StartAddress +0x5d);
    M13=DM(Exception_StartAddress +0x5c);
    M12=DM(Exception_StartAddress +0x5b);
    M11=DM(Exception_StartAddress +0x5a);
    M10=DM(Exception_StartAddress +0x59);
    M9=DM(Exception_StartAddress +0x58);
    M8=DM(Exception_StartAddress +0x57);
    M7=DM(Exception_StartAddress +0x56);
    M6=DM(Exception_StartAddress +0x55);
    M5=DM(Exception_StartAddress +0x54);
    M4=DM(Exception_StartAddress +0x53);
    M3=DM(Exception_StartAddress +0x52);
    M2=DM(Exception_StartAddress +0x51);
    M1= DM(Exception_StartAddress +0x50);
    M0= DM(Exception_StartAddress +0x4f);
    
    
     //I register
   I15= DM(Exception_StartAddress +0x4e);
   I14=  DM(Exception_StartAddress +0x4d);
   I15=  DM(Exception_StartAddress +0x4c);
   I12=  DM(Exception_StartAddress +0x4b);
   I11=  DM(Exception_StartAddress +0x4a);
   I10=  DM(Exception_StartAddress +0x49);
   I9=  DM(Exception_StartAddress +0x48);
   I8=  DM(Exception_StartAddress +0x47);
   I7=  DM(Exception_StartAddress +0x46);
   I6=  DM(Exception_StartAddress +0x45);
   I5=  DM(Exception_StartAddress +0x44);
   I4=  DM(Exception_StartAddress +0x43);
   I3=  DM(Exception_StartAddress +0x42);
   I2= DM(Exception_StartAddress +0x41);
   I1=  DM(Exception_StartAddress +0x40);
   I0=  DM(Exception_StartAddress +0x3f);
      //S registers
    S15=DM(Exception_StartAddress +0x3e);
    S14=DM(Exception_StartAddress +0x3d);
    S13=DM(Exception_StartAddress +0x3c);
    PX2= DM(Exception_StartAddress +0x3b);PX1=DM(Exception_StartAddress +0x3a);
    S12=PX;
    PX2= DM(Exception_StartAddress +0x39);PX1=DM(Exception_StartAddress +0x38);
    S11=PX;
    S10=DM(Exception_StartAddress +0x37);
    PX2=DM(Exception_StartAddress +0x36);PX1=DM(Exception_StartAddress +0x35);
    S9=PX;
    S8=DM(Exception_StartAddress +0x34);
    S7=DM(Exception_StartAddress +0x33);
    S6=DM(Exception_StartAddress +0x32);
    S5=DM(Exception_StartAddress +0x31);
    PX2=DM(Exception_StartAddress +0x30);PX1=DM(Exception_StartAddress +0x2f);
    S4=PX;
    S3=DM(Exception_StartAddress +0x2e);
    S2=DM(Exception_StartAddress +0x2d);
    S1=DM(Exception_StartAddress +0x2c);
    PX2=DM(Exception_StartAddress +0x2b);PX1=DM(Exception_StartAddress +0x2a);
    S0=PX;
    
    
    //R Registers
   
   
  
    R15=DM(Exception_StartAddress +0x29);
    R14=DM(Exception_StartAddress +0x28);
    R13=DM(Exception_StartAddress +0x27);
    PX2=DM(Exception_StartAddress +0x26);PX1=DM(Exception_StartAddress +0x25);
    R12=PX;
    
    PX2=DM(Exception_StartAddress +0x24);PX1=DM(Exception_StartAddress +0x23);
    R11=PX;
    R10=DM(Exception_StartAddress +0x22);
    
    PX2=DM(Exception_StartAddress +0x21);PX1=DM(Exception_StartAddress +0x20);
    R9=PX;
    PX2=DM(Exception_StartAddress +0x1f);PX1=DM(Exception_StartAddress +0x1e);
    R8=PX;
    
    PX2=DM(Exception_StartAddress +0x1d);PX1=DM(Exception_StartAddress +0x1c);
    R7=PX;
    R6=DM(Exception_StartAddress +0x1b);
    R5=DM(Exception_StartAddress +0x1a);
    PX2=DM(Exception_StartAddress +0x19);PX1=DM(Exception_StartAddress +0x18);
    R4=PX;
    
    PX2=DM(Exception_StartAddress +0x17);PX1=DM(Exception_StartAddress +0x16);
    R3=PX;
    
    PX2=DM(Exception_StartAddress +0x15);PX1=DM(Exception_StartAddress +0x14);
    R0=PX;
    
    PX2=DM(Exception_StartAddress +0x13);
    PX1=DM(Exception_StartAddress +0x12);
    
    IMASK=DM(Exception_StartAddress +0x11);
    LIRPTL= DM(Exception_StartAddress +0x10);
    IRPTL= DM(Exception_StartAddress +0xf);
    STKYy=DM(Exception_StartAddress +0xe);
    STKYx=DM(Exception_StartAddress +0xd);
    ASTATy=DM(Exception_StartAddress +0xc);
    ASTATx=DM(Exception_StartAddress +0xb);
    R1=DM(Exception_StartAddress +0xa);
    pcstk=R1;
    
    R1=DM(Exception_StartAddress +7); USTAT4=R1;
    R1=DM(Exception_StartAddress +6); USTAT3=R1;
    R1=DM(Exception_StartAddress +5); USTAT2=R1;
    R1=DM(Exception_StartAddress +4); USTAT1=R1;
    R1=DM(Exception_StartAddress +3); MODE2=R1;  
    R1=DM(Exception_StartAddress +2); MODE1=R1;  
    R1=DM(Exception_StartAddress +1);
    R2=DM(Exception_StartAddress);
    
   
    pm_ptr=DM(Exception_StartAddress+0xa);
    JUMP(m14,pm_ptr);*/
    
.___sys_COMP_exception.end:    


/*
@----------------------------------------------------
@功能: 异常处理(SPORT Error异常处理)
@参数:  
@返回:  无
@函数原型:void ___sys_SPORT_exception(void)
@-------------------------------------------------------------------------------
*/
//对于SPORT异常，首先判断具体是哪一个SPORT口异常，然后判断是哪一个异常即可，接着将现场信息保存写进Flash
//最后清除相应位，即可不用复位系统继续使用.

.GLOBAL ___sys_SPORT_exception;

___sys_SPORT_exception:
     	
//首先存储异常中断向量号R2及主异常编号，由于12个异常中断向量号非连续，所以不能直接将R2的值作为主异常编号。
     DM(Exception_StartAddress)=R2;    //存储异常向量编号
//首先保存R1寄存器值，接下来要使用R1作为中间寄存器传递参数
     DM(Exception_StartAddress+1)=R1;
//由于CB7I/CB15I等异常中断优先级较低，有可能会被实时信号打断，这是不允许的，故首先保存MODEI，然后关闭全局中断。
     R1=MODE1;
     DM(Exception_StartAddress +2)=R1;
     BIT CLR MODE1 0x1000;   //Disable interrupts 
     nop;nop;
     R1=MODE2;  DM(Exception_StartAddress +3)=R1;  
     R1=USTAT1; DM(Exception_StartAddress +4)=R1;
     R1=USTAT2; DM(Exception_StartAddress +5)=R1;
     R1=USTAT3; DM(Exception_StartAddress +6)=R1;
     R1=USTAT4; DM(Exception_StartAddress +7)=R1;
     R1=0x2; 
     DM(Exception_StartAddress +8)=R1; //保存主异常编号(major exp type)  
 //ADSP21469 SPOTR有8个SPORT，每个SPORT有出现异常有三种情形Unexpected_Frame_Syncs，A_Data_Buffer_Error
 //及B_Data_Buffer_Error.通过SPERRSTAT寄存器可判断是哪一个SPORT出异常，然后通过SPERRCTLx寄存器可查询具体是什么异常.
     USTAT1=DM(SPERRSTAT);
     //接下来依次查询SPERRSTAT各标志位，确认是哪个一个SPORT口出异常及分支异常
     bit tst USTAT1 SP0_DERRA;  //SP0_DERRA/SP0_DERRB等在exception.h文件通过宏定义定义。
     IF TF JUMP (PC,.SP0_exp);
     bit tst USTAT1 SP0_DERRB;
     IF TF JUMP (PC,.SP0_exp);
     bit tst USTAT1 SP0_FSERR;
     IF TF JUMP (PC,.SP0_exp);
     
     bit tst USTAT1 SP1_DERRA;
     IF TF JUMP (PC,.SP1_exp);
     bit tst USTAT1 SP1_DERRB;
     IF TF JUMP (PC,.SP1_exp);
     bit tst USTAT1 SP1_FSERR;
     IF TF JUMP (PC,.SP1_exp);
     
     bit tst USTAT1 SP2_DERRA;
     IF TF JUMP (PC,.SP2_exp);
     bit tst USTAT1 SP2_DERRB;
     IF TF JUMP (PC,.SP2_exp);
     bit tst USTAT1 SP2_FSERR;
     IF TF JUMP (PC,.SP2_exp);
     
     bit tst USTAT1 SP3_DERRA;
     IF TF JUMP (PC,.SP3_exp);
     bit tst USTAT1 SP3_DERRB;
     IF TF JUMP (PC,.SP3_exp);
     bit tst USTAT1 SP3_FSERR;
     IF TF JUMP (PC,.SP3_exp);
     
     bit tst USTAT1 SP4_DERRA;
     IF TF JUMP (PC,.SP4_exp);
     bit tst USTAT1 SP4_DERRB;
     IF TF JUMP (PC,.SP4_exp);
     IF TF JUMP (PC,.SP4_exp);
     bit tst USTAT1 SP4_FSERR;
     
     bit tst USTAT1 SP5_DERRA;
     IF TF JUMP (PC,.SP5_exp);
     bit tst USTAT1 SP5_DERRB;
     IF TF JUMP (PC,.SP5_exp);
     bit tst USTAT1 SP5_FSERR;
     IF TF JUMP (PC,.SP5_exp);
     
     bit tst USTAT1 SP6_DERRA;
     IF TF JUMP (PC,.SP6_exp);
     bit tst USTAT1 SP6_DERRB;
     IF TF JUMP (PC,.SP6_exp);
     bit tst USTAT1 SP6_FSERR;
     IF TF JUMP (PC,.SP6_exp);
     
     bit tst USTAT1 SP7_DERRA;
     IF TF JUMP (PC,.SP7_A_exp);
     bit tst USTAT1 SP7_DERRB;
     IF TF JUMP (PC,.SP7_B_exp);
     R1=0x1b;                     //如不是Channel A error/Channel B error，则只能是Frame sync error
     DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);
.SP7_A_exp:
    R1=0x1c;
    DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);
.SP7_B_exp:
    R1=0x1d;
    DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);  
   
//接下来判断SPORT0中具体是哪一个原因造成异常Channel A error/Channel B error/Frame sync error
.SP0_exp:
     bit tst USTAT1 SP0_DERRA;   //SP0_DERRA为Channel A error标志位
     IF TF JUMP (PC,.SP0_A_exp);
     bit tst USTAT1 SP0_DERRB;   //SP0_DERRB为Channel B error标志位
     IF TF JUMP (PC,.SP0_B_exp);
     R1=0x6;                     //如不是Channel A error/Channel B error，则只能是Frame sync error
     DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);
.SP0_A_exp:
    R1=0x7;
    DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);
.SP0_B_exp:
    R1=0x8;
    DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);
//接下来SP1~`SP7处理方式同SP0   
.SP1_exp:
     bit tst USTAT1 SP1_DERRA;   //SP1_DERRA为Channel A error标志位
     IF TF JUMP (PC,.SP1_A_exp);
     bit tst USTAT1 SP1_DERRB;   //SP1_DERRB为Channel B error标志位
     IF TF JUMP (PC,.SP1_B_exp);
     R1=0x9;                     //如不是Channel A error/Channel B error，则只能是Frame sync error
     DM(Exception_StartAddress+9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);
.SP1_A_exp:
    R1=0xa;
    DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);
.SP1_B_exp:
    R1=0xb;
    DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_Commom_Message);

.SP2_exp:
     bit tst USTAT1 SP2_DERRA;   //SP2_DERRA为Channel A error标志位
     IF TF JUMP (PC,.SP2_A_exp);
     bit tst USTAT1 SP2_DERRB;   //SP2_DERRB为Channel B error标志位
     IF TF JUMP (PC,.SP2_B_exp);
     R1=0xc;                     //如不是Channel A error/Channel B error，则只能是Frame sync error
     DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);
.SP2_A_exp:
    R1=0xd;
    DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);
.SP2_B_exp:
    R1=0xe;
    DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);  
 
.SP3_exp:
     bit tst USTAT1 SP3_DERRA;   //SP3_DERRA为Channel A error标志位
     IF TF JUMP (PC,.SP3_A_exp);
     bit tst USTAT1 SP3_DERRB;   //SP3_DERRB为Channel B error标志位
     IF TF JUMP (PC,.SP3_B_exp);
     R1=0xf;                     //如不是Channel A error/Channel B error，则只能是Frame sync error
     DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);
.SP3_A_exp:
    R1=0x10;
    DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_Commom_Message);
.SP3_B_exp:
    R1=0x11;
    DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message); 
    
.SP4_exp:
     bit tst USTAT1 SP4_DERRA;   //SP4_DERRA为Channel A error标志位
     IF TF JUMP (PC,.SP4_A_exp);
     bit tst USTAT1 SP4_DERRB;   //SP4_DERRB为Channel B error标志位
     IF TF JUMP (PC,.SP4_B_exp);
     R1=0x12;                     //如不是Channel A error/Channel B error，则只能是Frame sync error
     DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);
.SP4_A_exp:
    R1=0x13;
    DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);
.SP4_B_exp:
    R1=0x14;
    DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);   

.SP5_exp:
     bit tst USTAT1 SP5_DERRA;   //SP5_DERRA为Channel A error标志位
     IF TF JUMP (PC,.SP5_A_exp);
     bit tst USTAT1 SP5_DERRB;   //SP5_DERRB为Channel B error标志位
     IF TF JUMP (PC,.SP5_B_exp);
     R1=0x15;                     //如不是Channel A error/Channel B error，则只能是Frame sync error
     DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);
.SP5_A_exp:
    R1=0x16;
    DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);
.SP5_B_exp:
    R1=0x17;
    DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);
   
.SP6_exp:
     bit tst USTAT1 SP6_DERRA;   //SP6_DERRA为Channel A error标志位
     IF TF JUMP (PC,.SP6_A_exp);
     bit tst USTAT1 SP6_DERRB;   //SP6_DERRB为Channel B error标志位
     IF TF JUMP (PC,.SP6_B_exp);
     R1=0x18;                     //如不是Channel A error/Channel B error，则只能是Frame sync error
     DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);
.SP6_A_exp:
    R1=0x19;
    DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);
.SP6_B_exp:
    R1=0x1a;
    DM(Exception_StartAddress +9)=R1;
    jump(PC,.Save_SPORT_Commom_Message);     
    
.Save_SPORT_Commom_Message:     
//接下来依次保存发生异常时PC指针，栈指针I7,ASTATx,STKYx,IRPTL,IMASK等寄存器，R/S/I/M/B/L等通用寄存器值等。
     R1=pcstk;
    DM(Exception_StartAddress +0xa)=R1;
   //DM(Exception_StartAddress +0xb)=I7;
    DM(Exception_StartAddress +0xb)=ASTATx;
    DM(Exception_StartAddress +0xc)=ASTATy;
    DM(Exception_StartAddress +0xd)=STKYx;
    DM(Exception_StartAddress +0xe)=STKYy;
    DM(Exception_StartAddress +0xf)=IRPTL;
    DM(Exception_StartAddress +0x10)=LIRPTL;
    DM(Exception_StartAddress +0x11)=IMASK;

    //开始保存通用寄存器值,首先是R registers
    DM(Exception_StartAddress +0x12)=PX1;
    DM(Exception_StartAddress +0x13)=PX2;
    PX=R0;
    DM(Exception_StartAddress +0x14)=PX1;DM(Exception_StartAddress +0x15)=PX2;
    PX=R3;
    DM(Exception_StartAddress +0x16)=PX1;DM(Exception_StartAddress +0x17)=PX2;
    PX=R4;
    DM(Exception_StartAddress +0x18)=PX1;DM(Exception_StartAddress +0x19)=PX2;
    DM(Exception_StartAddress +0x1a)=R5;
    DM(Exception_StartAddress +0x1b)=R6;
    PX=R7;
    DM(Exception_StartAddress +0x1c)=PX1;DM(Exception_StartAddress +0x1d)=PX2;
    PX=R8;
    DM(Exception_StartAddress +0x1e)=PX1;DM(Exception_StartAddress +0x1f)=PX2;
    PX=R9;
    DM(Exception_StartAddress +0x20)=PX1;DM(Exception_StartAddress +0x21)=PX2;
    DM(Exception_StartAddress +0x22)=R10;
    PX=R11;
    DM(Exception_StartAddress +0x23)=PX1;DM(Exception_StartAddress +0x24)=PX2;
    PX=R12;
    DM(Exception_StartAddress +0x25)=PX1;DM(Exception_StartAddress +0x26)=PX2;
    DM(Exception_StartAddress +0x27)=R13;
    DM(Exception_StartAddress +0x28)=R14;
    DM(Exception_StartAddress +0x29)=R15;
    //S registers
    PX=S0;
    DM(Exception_StartAddress +0x2a)=PX1;DM(Exception_StartAddress +0x2b)=PX2;
    DM(Exception_StartAddress +0x2c)=S1;
    DM(Exception_StartAddress +0x2d)=S2;
    DM(Exception_StartAddress +0x2e)=S3;
    PX=S4;
    DM(Exception_StartAddress +0x2f)=PX1;DM(Exception_StartAddress +0x30)=PX2;
    DM(Exception_StartAddress +0x31)=S5;
    DM(Exception_StartAddress+0x32 )=S6;
    DM(Exception_StartAddress +0x33)=S7;
    DM(Exception_StartAddress +0x34)=S8;
    PX=S9;
    DM(Exception_StartAddress +0x35)=PX1;DM(Exception_StartAddress +0x36)=PX2;
    DM(Exception_StartAddress +0x37)=S10;
    PX=S11;
    DM(Exception_StartAddress +0x38)=PX1;DM(Exception_StartAddress +0x39)=PX2;
    PX=S12;
    DM(Exception_StartAddress +0x3a)=PX1;DM(Exception_StartAddress +0x3b)=PX2;
    DM(Exception_StartAddress +0x3c)=S13;
    DM(Exception_StartAddress +0x3d)=S14;
    DM(Exception_StartAddress +0x3e)=S15;
   //I register
    DM(Exception_StartAddress +0x3f)=I0;
    DM(Exception_StartAddress +0x40)=I1;
    DM(Exception_StartAddress +0x41)=I2;
    DM(Exception_StartAddress +0x42)=I3;
    DM(Exception_StartAddress +0x43)=I4;
    DM(Exception_StartAddress +0x44)=I5;
    DM(Exception_StartAddress +0x45)=I6;
    DM(Exception_StartAddress +0x46)=I7;
    DM(Exception_StartAddress +0x47)=I8;
    DM(Exception_StartAddress +0x48)=I9;
    DM(Exception_StartAddress +0x49)=I10;
    DM(Exception_StartAddress +0x4a)=I11;
    DM(Exception_StartAddress +0x4b)=I12;
    DM(Exception_StartAddress +0x4c)=I13;
    DM(Exception_StartAddress +0x4d)=I14;
    DM(Exception_StartAddress +0x4e)=I15;
    //M registers
    DM(Exception_StartAddress +0x4f)=M0;
    DM(Exception_StartAddress +0x50)=M1;
    DM(Exception_StartAddress +0x51)=M2;
    DM(Exception_StartAddress +0x52)=M3;
    DM(Exception_StartAddress +0x53)=M4;
    DM(Exception_StartAddress +0x54)=M5;
    DM(Exception_StartAddress +0x55)=M6;
    DM(Exception_StartAddress +0x56)=M7;
    DM(Exception_StartAddress +0x57)=M8;
    DM(Exception_StartAddress +0x58)=M9;
    DM(Exception_StartAddress +0x59)=M10;
    DM(Exception_StartAddress +0x5a)=M11;
    DM(Exception_StartAddress +0x5b)=M12;
    DM(Exception_StartAddress +0x5c)=M13;
    DM(Exception_StartAddress +0x5d)=M14;
    DM(Exception_StartAddress +0x5e)=M15;
    //B registers
    DM(Exception_StartAddress +0x5f)=B0;
    DM(Exception_StartAddress +0x60)=B1;
    DM(Exception_StartAddress +0x61)=B2;
    DM(Exception_StartAddress +0x62)=B3;
    DM(Exception_StartAddress +0x63)=B4;
    DM(Exception_StartAddress +0x64)=B5;
    DM(Exception_StartAddress +0x65)=B6;
    DM(Exception_StartAddress +0x66)=B7;
    DM(Exception_StartAddress +0x67)=B8;
    DM(Exception_StartAddress +0x68)=B9;
    DM(Exception_StartAddress +0x69)=B10;
    DM(Exception_StartAddress +0x6a)=B11;
    DM(Exception_StartAddress +0x6b)=B12;
    DM(Exception_StartAddress +0x6c)=B13;
    DM(Exception_StartAddress +0x6d)=B14;
    DM(Exception_StartAddress +0x6e)=B15;
    // L registers
    DM(Exception_StartAddress +0x6f)=L0;
    DM(Exception_StartAddress +0x70)=L1;
    DM(Exception_StartAddress +0x71)=L2;
    DM(Exception_StartAddress +0x72)=L3;
    DM(Exception_StartAddress +0x73)=L4;
    DM(Exception_StartAddress +0x74)=L5;
    DM(Exception_StartAddress +0x75)=L6;
    DM(Exception_StartAddress +0x76)=L7;
    DM(Exception_StartAddress +0x77)=L8;
    DM(Exception_StartAddress +0x78)=L9;
    DM(Exception_StartAddress +0x79)=L10;
    DM(Exception_StartAddress +0x7a)=L11;
    DM(Exception_StartAddress +0x7b)=L12;
    DM(Exception_StartAddress +0x7c)=L13;
    DM(Exception_StartAddress +0x7d)=L14;
    DM(Exception_StartAddress +0x7e)=L15;
    // Save the MRF registers. We save them here so that the offset of their
// locations is within 32 words of the final stack pointer. This allows us,
// on 2106x at least, to  combine the get() instructions with another
// instruction when we restore them. Note that we can't directly access the
// shadow MRF registers on 2116x so we have to use S regs to get the values.
// This adds to the cycle count 
    bit set mode1 PEYEN;
    nop;nop;
    R10=MR0F;
    R11=MR1F;
    R12=MR2F;
    R13=MR0B;
    R14=MR1B;
    R15=MR2B;
    bit clr mode1 PEYEN;
    nop;nop;
    DM(Exception_StartAddress +0x7f)=R10;
    DM(Exception_StartAddress +0x80)=R11;
    DM(Exception_StartAddress +0x81)=R12;
    DM(Exception_StartAddress +0x82)=R13;
    DM(Exception_StartAddress +0x83)=R14;
    DM(Exception_StartAddress +0x84)=R15;
    DM(Exception_StartAddress +0x85)=S10;
    DM(Exception_StartAddress +0x86)=S11;
    DM(Exception_StartAddress +0x87)=S12;
    DM(Exception_StartAddress +0x88)=S13;
    DM(Exception_StartAddress +0x89)=S14;
    DM(Exception_StartAddress +0x8a)=S15;
//对于SPERRI异常除了通用信息外，还需要将与SPORT相关的寄存器保存  
   R1=SPERRSTAT;  DM(Exception_StartAddress+0x8b)=R1;
   R1=SPERRCTL0;  DM(Exception_StartAddress+0x8c)=R1;
   R1=SPERRCTL1;  DM(Exception_StartAddress+0x8d)=R1;
   R1=SPERRCTL2;  DM(Exception_StartAddress+0x8e)=R1;
   R1=SPERRCTL3;  DM(Exception_StartAddress+0x8f)=R1; 
   R1=SPERRCTL4;  DM(Exception_StartAddress+0x90)=R1;
   R1=SPERRCTL5;  DM(Exception_StartAddress+0x91)=R1;
   R1=SPERRCTL6;  DM(Exception_StartAddress+0x92)=R1;
   R1=SPERRCTL7;  DM(Exception_StartAddress+0x93)=R1;
   R1=DIV0;       DM(Exception_StartAddress+0x94)=R1;
   R1=DIV1;       DM(Exception_StartAddress+0x95)=R1;
   R1=DIV2;       DM(Exception_StartAddress+0x96)=R1;
   R1=DIV3;       DM(Exception_StartAddress+0x97)=R1;
   R1=DIV4;       DM(Exception_StartAddress+0x98)=R1;
   R1=DIV5;       DM(Exception_StartAddress+0x99)=R1;
   R1=DIV6;       DM(Exception_StartAddress+0x9a)=R1;
   R1=DIV7;       DM(Exception_StartAddress+0x9b)=R1;
   R1=SPCTL0;     DM(Exception_StartAddress+0x9c)=R1;
   R1=SPCTL1;     DM(Exception_StartAddress+0x9d)=R1;
   R1=SPCTL2;     DM(Exception_StartAddress+0x9e)=R1;
   R1=SPCTL3;     DM(Exception_StartAddress+0x9f)=R1;
   R1=SPCTL4;     DM(Exception_StartAddress+0xa0)=R1;
   R1=SPCTL5;     DM(Exception_StartAddress+0xa1)=R1;
   R1=SPCTL6;     DM(Exception_StartAddress+0xa2)=R1;
   R1=SPCTL7;     DM(Exception_StartAddress+0xa3)=R1;
   R1=SPCTLN0;    DM(Exception_StartAddress+0xa4)=R1;
   R1=SPCTLN1;    DM(Exception_StartAddress+0xa5)=R1;
   R1=SPCTLN2;    DM(Exception_StartAddress+0xa6)=R1;
   R1=SPCTLN3;    DM(Exception_StartAddress+0xa7)=R1;
   R1=SPCTLN4;    DM(Exception_StartAddress+0xa8)=R1;
   R1=SPCTLN5;    DM(Exception_StartAddress+0xa9)=R1;
   R1=SPCTLN6;    DM(Exception_StartAddress+0xaa)=R1;
   R1=SPCTLN7;    DM(Exception_StartAddress+0xab)=R1;
   R1=SPMCTL0;    DM(Exception_StartAddress+0xac)=R1;
   R1=SPMCTL1;    DM(Exception_StartAddress+0xad)=R1;
   R1=SPMCTL2;    DM(Exception_StartAddress+0xae)=R1;
   R1=SPMCTL3;    DM(Exception_StartAddress+0xaf)=R1;
   R1=SPMCTL4;    DM(Exception_StartAddress+0xb0)=R1;
   R1=SPMCTL5;    DM(Exception_StartAddress+0xb1)=R1;
   R1=SPMCTL6;    DM(Exception_StartAddress+0xb2)=R1;
   R1=SPMCTL7;    DM(Exception_StartAddress+0xb3)=R1;
   R1=SP0CS0;     DM(Exception_StartAddress+0xb4)=R1;
   R1=SP0CS1;     DM(Exception_StartAddress+0xb5)=R1;
   R1=SP0CS2;     DM(Exception_StartAddress+0xb6)=R1;
   R1=SP0CS3;     DM(Exception_StartAddress+0xb7)=R1;
   R1=SP1CS0;     DM(Exception_StartAddress+0xb8)=R1;
   R1=SP1CS1;     DM(Exception_StartAddress+0xb9)=R1;
   R1=SP1CS2;     DM(Exception_StartAddress+0xba)=R1;
   R1=SP1CS3;     DM(Exception_StartAddress+0xbb)=R1;
   R1=SP2CS0;     DM(Exception_StartAddress+0xbc)=R1;
   R1=SP2CS1;     DM(Exception_StartAddress+0xbd)=R1;
   R1=SP2CS2;     DM(Exception_StartAddress+0xbe)=R1;
   R1=SP2CS3;     DM(Exception_StartAddress+0xbf)=R1;
   R1=SP3CS0;     DM(Exception_StartAddress+0xc0)=R1;
   R1=SP3CS1;     DM(Exception_StartAddress+0xc1)=R1;
   R1=SP3CS2;     DM(Exception_StartAddress+0xc2)=R1;
   R1=SP3CS3;     DM(Exception_StartAddress+0xc3)=R1;
   R1=SP4CS0;     DM(Exception_StartAddress+0xc4)=R1;
   R1=SP4CS1;     DM(Exception_StartAddress+0xc5)=R1;
   R1=SP4CS2;     DM(Exception_StartAddress+0xc6)=R1;
   R1=SP4CS3;     DM(Exception_StartAddress+0xc7)=R1;
   R1=SP5CS0;     DM(Exception_StartAddress+0xc8)=R1;
   R1=SP5CS1;     DM(Exception_StartAddress+0xc9)=R1;
   R1=SP5CS2;     DM(Exception_StartAddress+0xca)=R1;
   R1=SP5CS3;     DM(Exception_StartAddress+0xcb)=R1;
   R1=SP6CS0;     DM(Exception_StartAddress+0xcc)=R1;
   R1=SP6CS1;     DM(Exception_StartAddress+0xcd)=R1;
   R1=SP6CS2;     DM(Exception_StartAddress+0xce)=R1;
   R1=SP6CS3;     DM(Exception_StartAddress+0xcf)=R1;
   R1=SP7CS0;     DM(Exception_StartAddress+0xd0)=R1;
   R1=SP7CS1;     DM(Exception_StartAddress+0xd1)=R1;
   R1=SP7CS2;     DM(Exception_StartAddress+0xd2)=R1;
   R1=SP7CS3;     DM(Exception_StartAddress+0xd3)=R1;  
   R1=SP0CCS0;    DM(Exception_StartAddress+0xd4)=R1;
   R1=SP0CCS1;    DM(Exception_StartAddress+0xd5)=R1;
   R1=SP0CCS2;    DM(Exception_StartAddress+0xd6)=R1;
   R1=SP0CCS3;    DM(Exception_StartAddress+0xd7)=R1;
   R1=SP1CCS0;    DM(Exception_StartAddress+0xd8)=R1;
   R1=SP1CCS1;    DM(Exception_StartAddress+0xd9)=R1;
   R1=SP1CCS2;    DM(Exception_StartAddress+0xda)=R1;
   R1=SP1CCS3;    DM(Exception_StartAddress+0xdb)=R1;
   R1=SP2CCS0;    DM(Exception_StartAddress+0xdc)=R1;
   R1=SP2CCS1;    DM(Exception_StartAddress+0xdd)=R1;
   R1=SP2CCS2;    DM(Exception_StartAddress+0xde)=R1;
   R1=SP2CCS3;    DM(Exception_StartAddress+0xdf)=R1;
   R1=SP3CCS0;    DM(Exception_StartAddress+0xe0)=R1;
   R1=SP3CCS1;    DM(Exception_StartAddress+0xe1)=R1;
   R1=SP3CCS2;    DM(Exception_StartAddress+0xe2)=R1;
   R1=SP3CCS3;    DM(Exception_StartAddress+0xe3)=R1;
   R1=SP4CCS0;    DM(Exception_StartAddress+0xe4)=R1;
   R1=SP4CCS1;    DM(Exception_StartAddress+0xe5)=R1;
   R1=SP4CCS2;    DM(Exception_StartAddress+0xe6)=R1;
   R1=SP4CCS3;    DM(Exception_StartAddress+0xe7)=R1;
   R1=SP5CCS0;    DM(Exception_StartAddress+0xe8)=R1;
   R1=SP5CCS1;    DM(Exception_StartAddress+0xe9)=R1;
   R1=SP5CCS2;    DM(Exception_StartAddress+0xea)=R1;
   R1=SP5CCS3;    DM(Exception_StartAddress+0xeb)=R1;
   R1=SP6CCS0;    DM(Exception_StartAddress+0xec)=R1;
   R1=SP6CCS1;    DM(Exception_StartAddress+0xed)=R1;
   R1=SP6CCS2;    DM(Exception_StartAddress+0xee)=R1;
   R1=SP6CCS3;    DM(Exception_StartAddress+0xef)=R1;
   R1=SP7CCS0;    DM(Exception_StartAddress+0xf0)=R1;
   R1=SP7CCS1;    DM(Exception_StartAddress+0xf1)=R1;
   R1=SP7CCS2;    DM(Exception_StartAddress+0xf2)=R1;
   R1=SP7CCS3;    DM(Exception_StartAddress+0xf3)=R1;  
  // DM(Exception_StartAddress+0xf4)=LIRPTL;  //SPORT异常需要保存LIRPTL寄存器，因为SPORT0,2,4,7中断通过LIRPTL寄存器unmask.
    
  	//写异常信息头信息
	R1=0x87654321;
    DM(Exception_StartAddress-5)=R1;
    R1=DM(Exception_StartAddress);
    DM(Exception_StartAddress-4)=R1;
    R1=DM(Exception_StartAddress+9);
    DM(Exception_StartAddress-3)=R1;
    R1=Exception_StartAddress-5;
    DM(Exception_StartAddress-2)=R1;
    R1=0xf9;
    DM(Exception_StartAddress-1)=R1;
  
  
     B7 = exp_stack_space;
     I7 = (  (exp_stack_space + exp_stack_length - 1)
      		- ((exp_stack_space + exp_stack_length - 1) % 2) );
     L7 = (  exp_stack_length
            - ((exp_stack_space + exp_stack_length - 1) % 2) );
     B6 = B7;
     I6 = I7;
     L6 = L7;
    
    
    
   
 //下面分别将主异常编号，分支异常编号及保存信息深度(sizes)作为参数传递给sys_SPORT_exp_dealer，转入C模式处理.   
	R4=DM(Exception_StartAddress+8);   
    R8=DM(Exception_StartAddress+9);
    R12=0xf9;
    R2=I6;      //由于SPERRI不用复位经过处理后可以继续运行，所以这里必须可以从C函数中返回.
    I6=I7;
    pm_ptr=_Exp_General;
    JUMP(M13,pm_ptr);
    DM(I7,M7)=R2;
    DM(I7,M7)=PC;
    
    R1=DM(Exception_StartAddress+4);
    R5=0x8;
    R6=R5-R1;
    IF GE JUMP(PC,.SPORT0_exp);   //标示符GE表示>=0.
    R5=0xb;
    R6=R5-R1;
    IF GE JUMP(PC,.SPORT1_exp);
    R5=0xe;
    R6=R5-R1;
    IF GE JUMP(PC,.SPORT2_exp);
    R5=0x11;
    R6=R5-R1;
    IF GE JUMP(PC,.SPORT3_exp);
    R5=0x14;
    R6=R5-R1;
    IF GE JUMP(PC,.SPORT4_exp);
    R5=0x17;
    R6=R5-R1;
    IF GE JUMP(PC,.SPORT5_exp);
    R5=0x1a;
    R6=R5-R1;
    IF GE JUMP(PC,.SPORT6_exp);
    
    R5=0x1c;
    R6=R5-R1;
    IF EQ JUMP(PC,.SPORT7_A_exp);
    R5=0x1b;
    R6=R5-R1;
    IF EQ JUMP(PC,.SPORT7_Frame_exp);
    USTAT3=DM(SPERRCTL7);
    bit set USTAT3 DERRB_STAT;
    DM(SPERRCTL7)=USTAT3;
    R5=DM(SPCTL7);
    RTI;   
.SPORT7_Frame_exp:
    USTAT3=DM(SPERRCTL7);
    bit set USTAT3 FSERR_STAT;
    DM(SPERRCTL7)=USTAT3;
    R5=DM(SPCTL7);
    RTI;
.SPORT7_A_exp:
    USTAT3=DM(SPERRCTL7);
    bit set USTAT3 DERRA_STAT;
    DM(SPERRCTL7)=USTAT3;
    R5=DM(SPCTL7);
    RTI;
    
 
.SPORT0_exp:
    IF EQ JUMP(PC,.SPORT0_Frame_exp);
    R5=0x7;
    R6=R5-R1;
    IF EQ JUMP(PC,.SPORT0_A_exp);
    USTAT3=DM(SPERRCTL0);
    bit set USTAT3 DERRB_STAT;
    DM(SPERRCTL0)=USTAT3;
    R5=DM(SPCTL0);
    RTI;   
.SPORT0_Frame_exp:
    USTAT3=DM(SPERRCTL0);
    bit set USTAT3 FSERR_STAT;
    DM(SPERRCTL0)=USTAT3;
    R5=DM(SPCTL0);
    RTI;
.SPORT0_A_exp:
    USTAT3=DM(SPERRCTL0);
    bit set USTAT3 DERRA_STAT;
    DM(SPERRCTL0)=USTAT3;
    R5=DM(SPCTL0);
    RTI;
    
 .SPORT1_exp:
    IF EQ JUMP(PC,.SPORT1_Frame_exp);
    R5=0xa;
    R6=R5-R1;
    IF EQ JUMP(PC,.SPORT1_A_exp);
    USTAT3=DM(SPERRCTL1);
    bit set USTAT3 DERRB_STAT;
    DM(SPERRCTL1)=USTAT3;
    R5=DM(SPCTL1);
    RTI;   
.SPORT1_Frame_exp:
    USTAT3=DM(SPERRCTL1);
    bit set USTAT3 FSERR_STAT;
    DM(SPERRCTL1)=USTAT3;
    R5=DM(SPCTL1);
    RTI;
.SPORT1_A_exp:
    USTAT3=DM(SPERRCTL1);
    bit set USTAT3 DERRA_STAT;
    DM(SPERRCTL1)=USTAT3;
    R5=DM(SPCTL1);
    RTI;
    
 .SPORT2_exp:
    IF EQ JUMP(PC,.SPORT2_Frame_exp);
    R5=0xd;
    R6=R5-R1;
    IF EQ JUMP(PC,.SPORT2_A_exp);
    USTAT3=DM(SPERRCTL2);
    bit set USTAT3 DERRB_STAT;
    DM(SPERRCTL2)=USTAT3;
    R5=DM(SPCTL2);
    RTI;   
.SPORT2_Frame_exp:
    USTAT3=DM(SPERRCTL2);
    bit set USTAT3 FSERR_STAT;
    DM(SPERRCTL2)=USTAT3;
    R5=DM(SPCTL2);
    RTI;
.SPORT2_A_exp:
    USTAT3=DM(SPERRCTL2);
    bit set USTAT3 DERRA_STAT;
    DM(SPERRCTL2)=USTAT3;
    R5=DM(SPCTL2);
    RTI;
    
.SPORT3_exp:
    IF EQ JUMP(PC,.SPORT3_Frame_exp);
    R5=0x10;
    R6=R5-R1;
    IF EQ JUMP(PC,.SPORT3_A_exp);
    USTAT3=DM(SPERRCTL3);
    bit set USTAT3 DERRB_STAT;
    DM(SPERRCTL3)=USTAT3;
    R5=DM(SPCTL3);
    RTI;   
.SPORT3_Frame_exp:
    USTAT3=DM(SPERRCTL3);
    bit set USTAT3 FSERR_STAT;
    DM(SPERRCTL3)=USTAT3;
    R5=DM(SPCTL3);
    RTI;
.SPORT3_A_exp:
    USTAT3=DM(SPERRCTL3);
    bit set USTAT3 DERRA_STAT;
    DM(SPERRCTL3)=USTAT3;
    R5=DM(SPCTL3);
    RTI;
    
.SPORT4_exp:
    IF EQ JUMP(PC,.SPORT4_Frame_exp);
    R5=0x13;
    R6=R5-R1;
    IF EQ JUMP(PC,.SPORT4_A_exp);
    USTAT3=DM(SPERRCTL4);
    bit set USTAT3 DERRB_STAT;
    DM(SPERRCTL4)=USTAT3;
    R5=DM(SPCTL4);
    RTI;   
.SPORT4_Frame_exp:
    USTAT3=DM(SPERRCTL4);
    bit set USTAT3 FSERR_STAT;
    DM(SPERRCTL4)=USTAT3;
    R5=DM(SPCTL4);
    RTI;
.SPORT4_A_exp:
    USTAT3=DM(SPERRCTL4);
    bit set USTAT3 DERRA_STAT;
    DM(SPERRCTL4)=USTAT3;
    R5=DM(SPCTL4);
    RTI;  
    
.SPORT5_exp:
    IF EQ JUMP(PC,.SPORT5_Frame_exp);
    R5=0x16;
    R6=R5-R1;
    IF EQ JUMP(PC,.SPORT5_A_exp);
    USTAT3=DM(SPERRCTL5);
    bit set USTAT3 DERRB_STAT;
    DM(SPERRCTL5)=USTAT3;
    R5=DM(SPCTL5);
    RTI;   
.SPORT5_Frame_exp:
    USTAT3=DM(SPERRCTL5);
    bit set USTAT3 FSERR_STAT;
    DM(SPERRCTL5)=USTAT3;
    R5=DM(SPCTL5);
    RTI;
.SPORT5_A_exp:
    USTAT3=DM(SPERRCTL5);
    bit set USTAT3 DERRA_STAT;
    DM(SPERRCTL5)=USTAT3;
    R5=DM(SPCTL5);
    RTI; 
     
.SPORT6_exp:
    IF EQ JUMP(PC,.SPORT6_Frame_exp);
    R5=0x19;
    R6=R5-R1;
    IF EQ JUMP(PC,.SPORT6_A_exp);
    USTAT3=DM(SPERRCTL6);
    bit set USTAT3 DERRB_STAT;
    DM(SPERRCTL6)=USTAT3;
    R5=DM(SPCTL6);
    RTI;   
.SPORT6_Frame_exp:
    USTAT3=DM(SPERRCTL6);
    bit set USTAT3 FSERR_STAT;
    DM(SPERRCTL6)=USTAT3;
    R5=DM(SPCTL6);
    RTI;
.SPORT6_A_exp:
    USTAT3=DM(SPERRCTL6);
    bit set USTAT3 DERRA_STAT;
    DM(SPERRCTL6)=USTAT3;
    R5=DM(SPCTL6);
    RTI;  

.___sys_SPORT_exception.end:


/*
@----------------------------------------------------
@功能: 调试异常处理(EMUI/BKPI/EMULI)
@参数:  
@返回:  无
@函数原型:void ___sys_Debug_exception(void)
@-------------------------------------------------------------------------------
*/

.GLOBAL ___sys_Debug_exception;

___sys_Debug_exception:

nop;
nop;

.___sys_Debug_exception.end:



/*
@----------------------------------------------------
@功能: 软件复位(software reset)
@参数:  
@返回:  无
@函数原型:void ___asm_software_reset(void)
@-------------------------------------------------------------------------------
*/
.section/pm seg_pmco;
.GLOBAL ___asm_software_reset;

___asm_software_reset:

 USTAT1=DM(SYSCTL);
 bit set USTAT1 0x1;
 DM(SYSCTL)=USTAT1;
 nop;
 nop;

.___asm_software_reset.end:


/*
@----------------------------------------------------
@功能: 运行复位(Running reset)
@参数:  无
@返回:  无
@函数原型:void ___asm_running_reset(void)
@-------------------------------------------------------------------------------
*/
.section/pm seg_pmco;
.GLOBAL ___asm_running_reset;

___asm_running_reset:
   USTAT1=DM(RUNRSTCTL);
   bit set USTAT1 0x3;
   DM(RUNRSTCTL)=USTAT1;
   nop;
   nop;
.___asm_running_reset.end:


/*
@----------------------------------------------------
@功能: 软件硬复位
@参数:  
@返回:  无
@函数原型:void ___asm_reset_cpu_by_guts()
@-------------------------------------------------------------------------------
*/
.section/pm seg_pmco;
.GLOBAL ___asm_reset_cpu_by_guts;

___asm_reset_cpu_by_guts:

 nop;

.___asm_reset_cpu_by_guts.end:


/*
@----------------------------------------------------
@功能: 硬件软复位
@参数:  
@返回:  无
@函数原型:void ___asm_reset_cpu_hard_srst()
@-------------------------------------------------------------------------------
*/
.section/pm seg_pmco;
.GLOBAL ___asm_reset_cpu_hard_srst;

___asm_reset_cpu_hard_srst:

 nop;

.___asm_reset_cpu_hard_srst.end:


/*
@----------------------------------------------------
@功能: 异常初始化
@参数:  无
@返回:  无
@函数原型:void ___asm_exp_init(void)
@-------------------------------------------------------------------------------
*/
.section/pm seg_pmco;
.GLOBAL ___asm_exp_init;
___asm_exp_init:
       R1=IMASK;
//IMASK寄存器中异常中断unmask标志位为IICDI(bit2),SOVFI(bit3)
//SPERRI(bit5), CB7I(bit20),CB15I(bit21),FIXI(bit23),FLTOI(bit24)
//FLTUI(bit25),FLTII(bit26)。    
       //R2=0x5b0002c;   //暂时不开放FLTUI
       R2=0x30002c;
       R1=R1 or R2;
       IMASK=R1;
       FETCH_RETURN
      RETURN (DB);
      rframe;
      nop;
.___asm_exp_init.end:



//***************************file end*************************//    
     
 