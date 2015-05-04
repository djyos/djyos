/************************************************************************
 *
 * sig_glob.h
 *
 * (c) Copyright 2001-2010 Analog Devices, Inc.  All rights reserved.
 * $Revision: 2620 $
 ************************************************************************/

/*  This file contains the global declarations for the
    signal.h header file and its associated routines */

#ifndef __SIG_GLOB_DEFINED
#define __SIG_GLOB_DEFINED

#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rule_19_7:"The macros defined are used in assembly so function replacements are not an option.")
#pragma diag(suppress:misra_rule_20_8:"Interrupt support requires signal.h to be included.")
#endif /* _MISRA_RULES */

#include <signal.h>

#define next_mask 0
#define next_func 1
#define curr_func 2
#define defl_mask 3
#define defl_func 4
#define defl_disp 5
#define siga_size 6	/* Size of signal entry */

/* Accessor macros for various fields of the interrupt table */
#define SIGMASK(I)	(___lib_int_table+((I)*siga_size))
#define SIGCFUNC(I)	(___lib_int_table+((I)*siga_size)+curr_func)
#define SIGDISP(I)	(___lib_int_table+((I)*siga_size)+defl_disp)

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif

#endif
