/*
** ADSP-21469 startup code generated on Oct 11, 2013 at 14:45:59.
*/
/*
** Copyright (C) 2000-2013 Analog Devices Inc., All Rights Reserved.
**
** This file is generated automatically based upon the options selected
** in the System Configuration utility. Changes to the Startup Code configuration
** should be made by modifying the appropriate options rather than editing
** this file. To access the System Configuration utility, double-click the
** system.svc file from a navigation view.
**
** Custom additions can be inserted within the user-modifiable sections,
** these are bounded by comments that start with "$VDSG". Only changes
** placed within these sections are preserved when this file is re-generated.
**
** Product      : CrossCore Embedded Studio 1.0.2.0
** Tool Version : 6.0.2.32
*/

.FILE_ATTR libGroup="startup";
.FILE_ATTR libName="libc";

.FILE_ATTR libFunc="___lib_prog_term";
.FILE_ATTR FuncName="___lib_prog_term";
.FILE_ATTR libFunc="start";
.FILE_ATTR FuncName="start";
.FILE_ATTR prefersMem="any";
.FILE_ATTR prefersMemNum="50";

#include <sys/anomaly_macros_rtl.h> // defines silicon anomaly macros
#include <interrupt.h>              // defines interrupt support
#include <platform_include.h>       // defines MMR macros
#include <adi_osal.h>               // OSAL support
#include <sys/fatal_error_code.h>   // defines fatal error support

/* End marker for the list of static constructors.
** seg_ctdml hast to be mapped right after seg_ctdm.
*/

.SECTION/DATA/DOUBLEANY seg_ctdml;
.RETAIN_NAME __ctor_NULL_marker;
.GLOBAL __ctor_NULL_marker;
.VAR __ctor_NULL_marker = 0;

/* .gdt section start marker.
** This symbol is used by the exceptions run-time to locate the start
** of the .gdt memory section. This symbol must be first item mapped
** to the .gdt memory section.
*/

.SECTION/DATA/DOUBLEANY .gdt;
.GLOBAL ___eh_gdt;
.VAR ___eh_gdt[1];

/* Suppress the assemblers informational message regarding the automatic
** call sequence change it does when building -swc.
*/
.MESSAGE/SUPPRESS 2536;


.EXTERN _main, ___lib_setup_c;
.GLOBAL start;

.SECTION/CODE/DOUBLEANY seg_pmco;


start:


/*$VDSG<insert-code-very-beginning>                             */
.start_of_user_code_very_beginning:
      /*
      ** Insert any additional code to be executed before any other
      ** startup code here.
      ** Code inserted here is preserved when this file is re-generated.
      ** Note that the C runtime environment hasn't been setup yet
      ** so you cannot call C or C++ functions.
      */
.end_of_user_code_very_beginning:
/*$VDSG<insert-code-very-beginning>                             */

      /*
      ** Initializes the processor, memory, C runtime and heaps.
      */
      CALL ___lib_setup_c;


/*$VDSG<insert-code-after-setup>                                */
.start_of_user_code_after_setup:
      /*
      ** Insert any additional code to be executed at the start after
      ** the C runtime and hardware has been initialized.
      ** Code inserted here is preserved when this file is re-generated.
      */
.end_of_user_code_after_setup:
/*$VDSG<insert-code-after-setup>                                */

      /*
      ** Call the OSAL init function.
      */
      .EXTERN _adi_osal_Init;    // ADI_OSAL_STATUS adi_osal_Init(void);
      CJUMP _adi_osal_Init (DB);
         DM(I7, M7) = R2;
         DM(I7, M7) = PC;

      R1 = E_ADI_OSAL_SUCCESS;
      COMPU(R0, R1);
      IF NE JUMP .osal_Init_failed;

      /*
      ** Call all the C++ static class instance constructors.
      */
      .EXTERN  __ctors;
      I0 = __ctors;              // load pointer to the list of constructors
                                 // into call preserved I0
      R5 = I6;                   // hold current I6 in preserved register R5
      R0 = DM(I0, M6);           // get the address of the first constructor
      R0 = PASS R0, I13 = R0;    // check if it's the NULL list terminator
_lib_call_ctors:
      IF EQ JUMP _lib_call_ctors_exit; // if it is NULL we have completed

      I6 = I7;
      JUMP (M13, I13) (DB);      // call the constructor
         DM(I7, M7) = R5;
         DM(I7, M7) = PC;
      JUMP _lib_call_ctors (DB);
         R0 = DM(I0, M6);        // get the address of the next constructor
         R0 = PASS R0, I13 = R0; // check if it's the NULL list terminator
_lib_call_ctors_exit:

      R4 = R4 - R4,              // argc = 0
         R8 = M5;                // argv = NULL


/*$VDSG<insert-code-before-call-to-main>                        */
.start_of_user_code1:
      /*
      ** Insert any additional code to be executed before calling main() here.
      ** Code inserted here is preserved when this file is re-generated.
      ** Avoid clobbering the values of registers R4 and R8 as they hold
      ** the argc and argv parameters for main().
      */
.end_of_start_of_user_code1:
/*$VDSG<insert-code-before-call-to-main>                        */

      JUMP _main (DB);           // call main()
         DM(I7, M7) = 0;         // NULL FP to terminate call stack unwinding
         DM(I7, M7) = PC;

      .GLOBAL ___lib_prog_term;
___lib_prog_term:
      IDLE;
      JUMP ___lib_prog_term;
.___lib_prog_term.end:

      /*
      ** The call to _adi_osal_Init returned an error so call _adi_fatal_error.
      */
      .EXTERN _adi_fatal_error;
.osal_Init_failed:
      R12 = R0;                  // pass adi_osal_Init result value
      JUMP _adi_fatal_error (DB); // doesn't return
         R4 = _AFE_G_LibraryError;
         R8 = _AFE_S_adi_osal_Init_failure;
.osal_Init_failed.end:

.start.end:

