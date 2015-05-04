/************************************************************************
 *
 * irptl.h
 *
 * (c) Copyright 2001-2008 Analog Devices, Inc.  All rights reserved.
 * $Revision: 2 $
 ************************************************************************/

/*  Provide some macro relief for programmers that need
    to set IRPTL or other registers that require BIT SET
    or BIT CLR instructions that don't take a mask that 
    can be computed at runtime.  In order to get around
    this, we use instruction modification.  We limit the 
    place that the instructions are modified, and add some
    structure to the setting and clearing by using these
    macros.

    NOTE: unlike all other macros in the CRT, these macros
    expand to more than one instruction.  Routines that use
    these macros can't get a timing estimate by counting lines
    of assembly.

    These macros require that the file irptl.doj is placed in 
    writeable memory.

    It is recommended that these macros are used with interrupts
    disabled.  The single location that is modified should be 
    protected from modification during interrupt processing (and
    all of the standard interrupt handlers will use this method).

    There are three macros:

        write_instr_and_call
            This will modify a single instruction, and then 
            execute only that instruction.

        write_first_instr
            Intended to set up the first of two instructions
            that will be modified.  This should only be used 
            directly ahead of write_second_and_call.

        write_second_and_call
            Intended to be used directly after write_first_instr,
            this macro will set up a second modified instruction
            and then cause both instructions to be executed

    Each of the three macros takes three arguments:
        instr   -- a REGISTER that holds the 16 MSB of the instr
        mask    -- a REGISTER that holds a mask for the instr
        scratch -- a REGISTER that can be used for computing instr

*/

#include "lib_glob.h"

.EXTERN modify_instr, modify_two_instrs;

#if defined(__SIMDSHARC__)
#define MSB_16_set_irptl  0x140a0000;
#define MSB_16_clr_irptl  0x142a0000;
#define MSB_16_set_lirptl 0x140d0000;
#define MSB_16_clr_lirptl 0x142d0000;
#define MSB_16_set_imask  0x140b0000;
#define MSB_16_clr_imask  0x142b0000;
#else
#define MSB_16_set_irptl  0x14090000;
#define MSB_16_clr_irptl  0x14290000;
#define MSB_16_set_lirptl 0x00000000;
#define MSB_16_clr_lirptl 0x00000000;
#define MSB_16_set_imask  0x140d0000;
#define MSB_16_clr_imask  0x142d0000;
#endif

#if defined(__SIMDSHARC__)
# ifdef _ADI_SWFA
#  define write_instr_and_call(instr, mask, scratch) \
    scratch = LSHIFT mask BY -16; \
    scratch = instr OR scratch; \
    PX2 = scratch; \
    scratch = LSHIFT mask BY 16; \
    PX1 = scratch; \
    pm(modify_instr)=PX; \
    pm(__21160_anomaly_write_location)=r0; \
    pm(__21160_anomaly_write_location)=r1; \
    CALLER_HOLD(scratch) \
    CALLER_SWAP \
    RTLCALL(PC, modify_instr) (DB);  /* Call function */ \
    SAVE_OLD_FRAME(scratch) \
    SAVE_RET_ADDR
# else
#  define write_instr_and_call(instr, mask, scratch) \
    scratch = LSHIFT mask BY -16; \
    scratch = instr OR scratch; \
    PX2 = scratch; \
    scratch = LSHIFT mask BY 16; \
    PX1 = scratch; \
    pm(modify_instr)=PX; \
    CALLER_HOLD(scratch) \
    CALLER_SWAP \
    RTLCALL(PC, modify_instr) (DB);  /* Call function */ \
    SAVE_OLD_FRAME(scratch) \
    SAVE_RET_ADDR
# endif

# ifdef _ADI_SWFA
#  define write_first_instr(instr, mask, scratch) \
    scratch = LSHIFT mask BY -16; \
    scratch = instr OR scratch; \
    PX2 = scratch; \
    scratch = LSHIFT mask BY 16; \
    PX1 = scratch; \
    pm(modify_two_instrs)=PX; \
    pm(__21160_anomaly_write_location)=r0; \
    pm(__21160_anomaly_write_location)=r1
# else
#  define write_first_instr(instr, mask, scratch) \
    scratch = LSHIFT mask BY -16; \
    scratch = instr OR scratch; \
    PX2 = scratch; \
    scratch = LSHIFT mask BY 16; \
    PX1 = scratch; \
    pm(modify_two_instrs)=PX
#  endif


# ifdef _ADI_SWFA
#  define write_second_and_call(instr, mask, scratch) \
    scratch = LSHIFT mask BY -16; \
    scratch = instr OR scratch; \
    PX2 = scratch; \
    scratch = LSHIFT mask BY 16; \
    PX1 = scratch; \
    pm(modify_instr)=PX; \
    pm(__21160_anomaly_write_location)=r0; \
    pm(__21160_anomaly_write_location)=r1; \
    CALLER_HOLD(scratch) \
    CALLER_SWAP \
    RTLCALL(PC, modify_two_instrs) (DB);  /* Call function */ \
    SAVE_OLD_FRAME(scratch) \
    SAVE_RET_ADDR
# else
#  define write_second_and_call(instr, mask, scratch) \
    scratch = LSHIFT mask BY -16; \
    scratch = instr OR scratch; \
    PX2 = scratch; \
    scratch = LSHIFT mask BY 16; \
    PX1 = scratch; \
    pm(modify_instr)=PX; \
    CALLER_HOLD(scratch) \
    CALLER_SWAP \
    RTLCALL(PC, modify_two_instrs) (DB);  /* Call function */ \
    SAVE_OLD_FRAME(scratch) \
    SAVE_RET_ADDR
# endif

#else
# define write_instr_and_call(instr, mask, scratch) \
    scratch = LSHIFT mask BY -16; \
    scratch = instr OR scratch; \
    PX2 = scratch; \
    scratch = 0x0000FFFF; \
    scratch = scratch AND mask; \
    PX1 = scratch; \
    pm(modify_instr)=PX; \
    CALLER_HOLD(scratch) \
    CALLER_SWAP \
    RTLCALL(PC, modify_instr) (DB);  /* Call function */ \
    SAVE_OLD_FRAME(scratch) \
    SAVE_RET_ADDR

# define write_first_instr(instr, mask, scratch) \
    scratch = LSHIFT mask BY -16; \
    scratch = instr OR scratch; \
    PX2 = scratch; \
    scratch = 0x0000FFFF; \
    scratch = scratch AND mask; \
    PX1 = scratch; \
    pm(modify_two_instrs)=PX


# define write_second_and_call(instr, mask, scratch) \
    scratch = LSHIFT mask BY -16; \
    scratch = instr OR scratch; \
    PX2 = scratch; \
    scratch = 0x0000FFFF; \
    scratch = scratch AND mask; \
    PX1 = scratch; \
    pm(modify_instr)=PX; \
    CALLER_HOLD(scratch) \
    CALLER_SWAP \
    RTLCALL(PC, modify_two_instrs) (DB);  /* Call function */ \
    SAVE_OLD_FRAME(scratch) \
    SAVE_RET_ADDR
#endif


