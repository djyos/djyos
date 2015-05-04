/************************************************************************
 *
 * linkports.h
 *
 * (c) Copyright 2001-2004 Analog Devices, Inc.  All rights reserved.
 * $Revision: 2 $
 ************************************************************************/

/* Define the first and last link port interrupts for each machine with
 * an LIRPTL register. Makes this code easier to read and maintain.
 */

#if defined(__213xx__) || defined(__214xx__)
# define FIRST_START_LINK_PORT SIG_P6
# define FIRST_END_LINK_PORT SIG_P13
# define LAST_START_LINK_PORT SIG_P17
# define LAST_END_LINK_PORT SIG_P18
#elif defined(__2126x__)
# define FIRST_START_LINK_PORT SIG_SP0
# define FIRST_END_LINK_PORT (SIG_DAIL+1)
# define LAST_START_LINK_PORT SIG_GPTMR2
# define LAST_END_LINK_PORT SIG_SPIL
#elif defined(__ADSP21160__)
# define FIRST_LINK_PORT SIG_LP0I
# define LAST_LINK_PORT SIG_LP5I
#elif defined(__ADSP21161__)
# define FIRST_LINK_PORT SIG_LP0I
# define LAST_LINK_PORT SIG_SPITI
#endif
# define MID_START_IRPTL (FIRST_END_LINK_PORT+1)
# define MID_END_IRPTL (LAST_START_LINK_PORT-1)
