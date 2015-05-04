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



#include "def21469.h"
#include "cpu_peri_int_line.h"
#include "exception.h"


.message/suppress 1130;
.message/suppress 2024;

.global _load_application_parallel;
#define reset 0x8C003
.extern final_init_loop;
.extern ___lib_RSTI;

.SECTION/PM     seg_ldr;    // the kernel must reside in a single input_section

_load_application_parallel:
    
	call USER_INIT;        //0x8c005 of 48-bit word

    LIRPTL = 0;
    IMASK = 0;             // mask all interrupts since this code executes from within
    IRPTL = 0;             // clear any latched interrupts.
    BIT SET MODE1 IRPTEN;  //Enable global interrupts.
    
    ustat1 = dm(DMAC0);
    bit clr ustat1 DEN;
    dm(DMAC0) = ustat1;    // disable DMA for external port 0


    L0=0;   L4=0;          //Zero L's so I-registers can be used without wrap
    L8=0;   L12=0;  L15=0;
    M5=0;   M6=1;   M13=0;  M14=1;

    R11=DM(SYSCTL);        // Read current SYSCTL setting
    R12=PASS R11;          // Hold Initial SYSCTL setting => restored in final init
    I4=SYSCTL;
    
     
    
    BIT SET LIRPTL EP0IMSK ;  // Enables external port0 interrupt
 
// ==========================  READ_BOOT_INFO  ================================
// Read in section header (3 words that contain Data TAG, ADDRESS and COUNT)
// Places TAG in R8, Count in R2, and Destination Address in R3.  This code also
// analyzes the TAG and branches to the appropriate initialization code.
//
// Possible TAGS (the 'L' in each tag name signifying that the address and
// count are LOGICAL values, not physical):
//
//              0x0 FINAL_INIT
//              0x1 ZERO_LDATA
//              0x2 ZERO_L48
//              0x3 INIT_L16
//              0x4 INIT_L32
//              0x5 INIT_L48
//              0x6 INIT_L64
//              0x7 ZERO_EXT8
//              0x8 ZERO_EXT16
//              0x9 INIT_EXT8
//              0xA INIT_EXT16
//
//------------------------------------------------------------------------------
READ_BOOT_INFO:

    call READ_THREEx32;     // fetch three word header via DMA

    R8=dm(0x92003);         //r8 passes data-type tag
    R2=dm(0x92004);         //r2 passes count
    R3=dm(0x92005);         //r3 passes destination address

    R0=PASS R8;             // check TAG

    IF EQ JUMP final_init;  r0=r0-1;    // jump if fetched tag was 0, else tag--
    IF EQ JUMP ZERO_LDATA;  r0=r0-1;    // jump if fetched tag was 1, else tag--
    IF EQ JUMP ZERO_L48;    r0=r0-1;    // jump if fetched tag was 2, else tag--
    IF EQ JUMP INIT_L16;    r0=r0-1;    // jump if fetched tag was 3, else tag--
    IF EQ JUMP INIT_L32;    r0=r0-1;    // jump if fetched tag was 4, else tag--
    IF EQ JUMP INIT_L48;    r0=r0-1;    // jump if fetched tag was 5, else tag--
    IF EQ JUMP INIT_L64;    r0=r0-1;    // jump if fetched tag was 6, else tag--
    IF EQ JUMP ZERO_EXT8;   r0=r0-1;    // jump if fetched tag was 7, else tag--
    IF EQ JUMP ZERO_EXT16;  r0=r0-1;    // jump if fetched tag was 8, else tag--
    IF EQ JUMP INIT_EXT8;   r0=r0-1;    // jump if fetched tag was 9, else tag--
    IF EQ JUMP INIT_EXT16;  r0=r0-1;    // jump if fetched tag was A, else tag--
    IF EQ JUMP MULTI_PROC;  jump (pc,0); // jump if fetched tag was B, else tag--
                                         // was an invalid TAG, so trap for debug
//==============================================================================


//============================== READ_Nx32 =====================================
// Load either one or three 32-bit words (via DMA) into scratch locations listed
// below for core access. For 32-bit accesses, the three words will be at
// 0x92003, 0x92004, and 0x92005.  For 48-bit accesses two words will be
// available at 0x8c002 and 0x8c003.
//------------------------------------------------------------------------------
READ_THREEx32:
    USTAT1 = 3;
    DM(ICEP0)=USTAT1;     //Internal DMA count = three 32 bit words (two 48-bit words)
    USTAT1 = 3;
    DM(ECEP0)=USTAT1;     //External DMA count = three 32 bit words (two 48-bit words)
    jump (pc,3);          //skip over next three instructions

READ_ONEx32:
    DM(ICEP0)=M14;        //Internal DMA count = one 32-bit word
   
    DM(ECEP0)=M14;        //External DMA count = one 32-bit words

    DM(IMEP0)=M14;        // Internal DMA modify = 1
    DM(EMEP0)=M14;        // External DMA modify = 1
    USTAT1=0x92003;       // 0x92003, 0x92004, 0x92005 2-col = 0x8c002, 0x8c003 3-col
    DM(IIEP0)=USTAT1;     // Setup DMA destination address.

        
    ustat1 = dm(DMAC0);
    bit set ustat1 DEN;
    dm(DMAC0) = ustat1;        // enable DMA for external port 0


    //-------- Wait for external port DMA to complete using polling------
    idle;
    //---------------------------------------------------------

    ustat1 = 0;
    
    dm(DMAC0) = ustat1;        // disables DMA for external port 0



    r8=dm(0x92003);           //place 32-bit word in R8
    rts;
//==============================================================================

// ============================== ZERO_LDATA ===================================
// Use core to init 16/32/64-bit internal buffers to zero (saves space in LDR)
//------------------------------------------------------------------------------
ZERO_LDATA:
    I0=R3;  //R3 passes destination address (LW, NW, or SW)
    R0=0;   //Value used to write zero's to memory
    R1=0;   //neighbor register used to write upper 32 bits if LW accesses

    LCNTR=R2, DO ZERO_WRITE_END UNTIL LCE;  // R2 passes logical word count
ZERO_WRITE_END: DM(I0,M6)=R0;               // write zero to x16, x32, or x64 memory
    JUMP READ_BOOT_INFO;                    // fetch next section-header
//==============================================================================

//============================== EP0I_ISR =====================================
// This is the first instruction run after the kernel is booted. It returns to
// 0x8c005 and starts the kernel. Instructions added or removed may require
// adding "NOP" instructions to ensure this instruction resolves to 0x8c050.
// Check map file (kernel.map) to verify location.
//------------------------------------------------------------------------------

x50_EP0I_ISR: rti;
//==============================================================================


//================================  ZERO_L48 ===================================
// User core to initialize 48/40-bit internal memory to zero
//------------------------------------------------------------------------------
ZERO_L48:
    I0=R3;  //R3 passes destination address
    PX=0;
    LCNTR=R2, do SAVE_PX_END until lce;
SAVE_PX_END: dm(i0,m6)=PX;
    JUMP READ_BOOT_INFO;            // fetch next section-header
//==============================================================================


//===============================  INIT_L16 ====================================
// Initialize Internal Short-word (16-bit) Memory
//------------------------------------------------------------------------------
INIT_L16:

    I0=R3;                  //R3 passes destination address
    ustat2=r2;              //R2 passes 16-bit word count
    r2= lshift r2 by -1;    //divide count by 2 since each pass through the loop
                            //      initializes two 16-bit words

    if sz jump LAST_WORD_16; // Detect if there is only one word to initialize

    lcntr = r2, do READ_WRITE_END until lce;
        call READ_ONEx32;
        r6=dm(0x124006);    //SW equivalent of NW address 0x92003 (two LSBs)
        r7=dm(0x124007);    //SW equivalent of NW address 0x92003 (two MSBs)
        dm(i0,m6)=r6;
READ_WRITE_END: dm(i0,m6)=r7;

    bit tst ustat2 1;       // count odd or even? (0x1 = mask, not bit number)
    if NOT TF JUMP READ_BOOT_INFO;  //if bit0 of count was NOT 0x1 then the
                                    //count's even, so fetch next section-header

    LAST_WORD_16:
    call READ_ONEx32;       //else fetch and init last x16 word of this section
    r6=dm(0x124006);
    dm(i0,m0)=r6;
    JUMP READ_BOOT_INFO;    // fetch next section-header
//==============================================================================


//==============================  INIT_L32 =====================================
// initialize 32-bit internal memory
//------------------------------------------------------------------------------
INIT_L32:
    I0=R3;              //R3 passes destination address
    LCNTR=R2, DO CALL_LOOP_END UNTIL LCE;
        CALL READ_ONEx32(DB);
        NOP;            // Branches such as CALLs cannot be in the last three
        NOP;            // instructions of a loop
CALL_LOOP_END: DM(I0,M6)=R8;
    JUMP READ_BOOT_INFO;// fetch next section-header
//==============================================================================


//==============================  INIT_L48 =====================================
// Initialize instructions and 40-bit data
//------------------------------------------------------------------------------
INIT_L48:
    I0=R3;                  //R3 passes destination address
    ustat2=r2;              //R2 passes 48-bit word count
    r2= lshift r2 by -1;    //divide count by 2 since each pass through the loop
                            //      initializes two 48-bit words

    if sz jump LAST_WORD_48; // Detect if there is only one word to initialize

    LCNTR=R2, DO L48_CALL_LOOP_END UNTIL LCE;
        CALL READ_THREEx32;         // fetch two 48-bit words (3x32)
        PX=dm(0x8c002);             // fetch the first 48-bit word
        dm(i0,M6)=PX;
        PX=dm(0x8c003);
L48_CALL_LOOP_END: dm(i0,M6)=PX;

    bit tst ustat2 1;               // count odd or even? (0x1 = mask, not bit number)
    if NOT TF JUMP READ_BOOT_INFO;  //if bit0 of count was NOT 0x1 then the
                                    //count's even, so fetch next section-header

    LAST_WORD_48:
    CALL READ_THREEx32;             //Else fetch and init last 48-bit word of section.

    //NOTE: Since DMA moves 32-bit words, the minimum number of instructions
    //accessible by DMA is 2 (3x32=2x48).  If there are an odd number of 48-bit
    //words, the loader inserts a dummy 48-bit word into the stream.  As one can
    //see below, only the first of the last 2 instructions is initialized: the
    //dummy word is ignored. (Specifically, it's left at the scratch location
    //0x8c003 and is overwritten by subsequent data.

    PX=dm(0x8c002);
    dm(i0,M6)=PX;
    JUMP READ_BOOT_INFO;            // fetch next section-header
//==============================================================================



//================================  INIT_L64 ===================================
INIT_L64:
    I0=R3;  //R3 passes destination address (LW, NW, or SW)
    LCNTR=R2, DO L64_CALL_LOOP_END UNTIL LCE;
        CALL READ_ONEx32;   // read 32 lsbs of 64-bit word
        PX1=R8;
        CALL READ_ONEx32;   // read 32 msbs of 64-bit word
        PX2=R8;
        nop;                // Call prohibited from last 3 loop instr's
L64_CALL_LOOP_END: DM(I0,M6)=PX (lw);  // write all 64-bits to longword address location
    JUMP READ_BOOT_INFO;            // fetch next section-header
//==============================================================================

// ============================== ZERO_EXT ===================================
// Use core to init external buffers to zero (saves space in LDR)
//------------------------------------------------------------------------------
ZERO_EXT8:
ZERO_EXT16:
      JUMP ZERO_LDATA;       // Initilalizing the external memory with zero is same as initializing the internal memory with zero for external port

 

   
//==============================================================================

//==============================  INIT_EXT =====================================
// initialize external memory
//------------------------------------------------------------------------------
INIT_EXT:
INIT_EXT8:
INIT_EXT16:
     JUMP INIT_L32;         // initializing external memory is same as initializing internal memory for external port

//==============================  INIT_EXT =====================================
// Multiprocessor memory tag
//------------------------------------------------------------------------------
MULTI_PROC:
// R0 = dm(SYSTAT);     //  reads the system status register to get the ID pin status
// R1 = fext R0 by 8:3; //  Extracts the id no defined by the ID pins
// btst R2 by R1;       // tests the multiprocessor header value with extracted ID value 
// if not sz JUMP READ_BOOT_INFO;
 nop;nop;nop;nop;
 R0 = dm(EIEP0);
// R0 = R0 + R3;       // add the offset from the multiprocessor tag to the register
 nop;
 dm(EIEP0) = R0;     // Modify the index register based on the offset
 JUMP READ_BOOT_INFO; 
//============================ FINAL_INIT ======================================
// This code is the very last piece run in the kernel.  It is self-modifying
// code.  It uses a DMA to overwrite itself; initializing the 256 instructions
// that reside in the interrupt vector table. (Additional explanation below)
//------------------------------------------------------------------------------
final_init:

    //---- Setup for IVT instruction patch -------------------------------------
    I8=0x8c050;             // Point to external port 0 vector to patch from PX
    R9=0xb16b0000;          // Load opcode for "PM(0,I8)=PX" into R9
    PX=pm(0x8c003);         // User-instruction destined for 0x90050 is passed
                            // in the section-header for FINAL_INIT. That instr.
                            // is initialized upon completion of this DMA (see
                            // comments below) using the PX register.
    R11=BSET R11 BY 9;      // Set IMDW to 1 for inst write
    DM(SYSCTL)=R11;         // Set IMDW to 1 for inst write

    //---- Setup loop for self-modifying instruction ---------------------------
    I12=0x8c004;            // Point to 0x090004 for self modifing code inserted
                            // by the loader at 0x90004 in bootstream
    R9=pass R12, R11=R9;    // Clear AZ, copy power-on value of SYSCTL to R11

    //---- Setup final DMA parameters ------------------------------------------
    R1=0x92000; DM(IIEP0)=R1; // Setup DMA to load over ldr
    R2=0x180;   DM(ICEP0)=R2; // Load internal count
    R2=0x180;   DM(ECEP0)=R2; // Load external count
                DM(EMEP0)=M6; // Set to increment external prt
                DM(IMEP0)=M6; // Set to increment internal ptr
    
    //---- Enable EP0I interrupt ------------------------------------------------
    FLUSH CACHE;            // Remove any kernel instr's from cache

    DO ___lib_RSTI UNTIL EQ;    // Set bottom-of-loop address (loopstack) to 0x90004
                            // and top-of-loop address (PC Stack) to the address
                            // of the next instruction.
    PCSTK=reset;            // Change top-of-loop value from the address of
                            // this instruction to 0x90003..


    //---- Begin final DMA to overwrite this code ------------------------------
     ustat1 = dm(DMAC0);
     bit set ustat1 DEN;
     dm(DMAC0) = ustat1;        // enable final DMA for external port 0

    //---- Initiate self-modifying sequence ------------------------------------
    JUMP reset (DB);        // Causes 0x8c003 to be the return address when this
                            // DMA completes and the RTI at 0x8c050 is executed.
        IDLE;               // After IDLE, patch then start
        LIRPTL=0;           // clear any interrupts on way to 0x8c003


//==============================================================================
// When this final DMA completes, the PP interrupt is latched,
// which triggers the following chain of events:
//
//  1) The IDLE in the delayed branch to completes
//  2) IMASK is cleared
//  3) The PC (now 0x8c003 due to the "JUMP RESET (db)") is pushed on the
//      PC-Stack and the DSP vectors to 0x8c050 to service the interrupt.
//
//      - Meanwhile, the loader (anticipating this sequence) has automatically
//      inserted an "RTI" instruction at 0x8c050. The user-instruction intended
//      for that address is instead placed in the FINAL_INIT section-header, and
//      was loaded into PX before the DMA was initiated.)
//
// 4) The DSP executes the RTI at 0x8c050, and vectors to the address stored
//      on the PC-stack (0x8c003).
//
//      - Again, the loader has inserted an instruction into the boot stream and
//      has placed it at 0x8c004:
//
//          R0=R0-R0, DM(I4,M5)=R9, PM(I12,M13)=R11;   //opcode: 0x39732D802000
//
//      The loader has also placed a NOP at location 0x8c003, opcode 0x000000000000
//      Locations 0x8c000 - 0x8c004 are reserved and should never be used in any user
//      application. Because of this, the two locations used in final init are never 
//      restored to the value set forth in the executable generated by the tools.
//
//
// 5) This instruction does three things:
//
//      A) Restores the power-up value of SYSCTL (held in R11).
//      B) It overwrites itself with the instruction "PM(0,I8)=PX;"
//          (The first instruction of FINAL_INIT places the opcode for this
//          new instruction, 0xB16B00000000, into R9.)
//      C) R0=R0-R0 causes the AZ flag to be set.
//
//      This satisfies the loop termination-condition of the loop set up
//      in FINAL_INIT ("DO final_init_loop UNTIL EQ;").  When a loop condition is
//      achieved within the last three instructions of a loop, the DSP branches
//      to the top-of-loop address (PCSTK) one final time.
//
// 6) We manually changed this top-of-loop address to 0x90003 and the bottom
//      of the loop to 8c004, and so to conclude the kernel, the DSP executes the instructions at 0x8c003 and
//      0x8c004  
//      *again*.
//
// 7) Now, there's a new instruction at 0x8c004: "PM(0,I8)=PX;".  This
//      initializes the user-intended instruction at 0x8c050 (the vector for
//      the External Port Interrupt). //
//
//  At this point, the kernel is finished, and execution continues at 0x8c005.
//==============================================================================



//==============================  USER_INIT  ===================================
// This space is provided for the user to insert code. This is only required
// when a system requires a custom configuration for the DSP.  
//------------------------------------------------------------------------------

                /**** INSERT CUSTOM CONFIGURATION CODE HERE****/

USER_INIT:

		ustat1 = 0xF1;
		dm(EPCTL)=ustat1;
		
		ustat1=0x5C1;
//		ustat1=0xDDFC9;
		dm(AMICTL1)=ustat1;
			
 		r0=0x00000;			//start addr of Monitor_version
		R1=release_reboot;
		COMP(R1,R4);
		IF NE JUMP _start_addr_load;
		r0=0x80000;			//start addr of Release_version
_start_addr_load:
 		r0=lshift r0 by -2; 
 		r1=0x4000180;
 		r0=r0 or r1;
 		dm(EIEP0)=r0;

	rts;
//==============================================================================

.global ___second_stage_reboot;

.SECTION/PM     seg_ldr;    // the kernel must reside in a single input_section

___second_stage_reboot:

	DM(-1,I7)=R1;
	R1=monitor_reboot;
	COMP(R1,R4);
	IF EQ JUMP _start_second_stage_reboot;
	nop;
	R1=release_reboot;
	COMP(R1,R4);
	IF EQ JUMP _start_second_stage_reboot;
	nop;
	R1=cn_int_line_IRQ1I;
	COMP(R1,R4);
	IF NE JUMP _abort_second_stage_reboot;
	R1=0x6000;
	lcntr=R1, do _irp1_reboot_delay until lce;	//消除杂波干扰
_irp1_reboot_delay: nop;
	If FLAG1_IN JUMP _start_second_stage_reboot;
	
_abort_second_stage_reboot:
	R1=DM(-1,I7);	//restore R1
	R4=DM(0,I7);	//restore R4
	rti;

_start_second_stage_reboot:
	BIT CLR IMASK real_mask_imask;	  // MASK all the real ints
	nop;nop;
	BIT CLR LIRPTL real_mask_lirptl;  // MASK all the real ints
	nop;nop;
    BIT CLR IMASKP 0xffffffff;
    nop;nop;
    BIT CLR LIRPTL 0xfff00000;
    nop;nop;
    R0=0x0B3E;					// 此处的3个DM指令，用于改写seg_rth段IVT表中P9I中断向量处的代码为RTI指令，
    dm(0x1240f2)=R0;			// 以使second_stage_reboot正常执行。新的代码会重新覆盖此处
    R0=0x0;
    dm(0x1240f1)=R0;
    R0=0x0;
    dm(0x1240f0)=R0;
    nop;nop;
	jump _load_application_parallel;

.___second_stage_reboot.end: