// =============================================================================
// Copyright (C) 2012-2020 	 All Rights Reserved
// FileDescription:This file define the tlb items data structure
// FileVersion    :
// Author         :
// Create Time    :2014/12/12
// =============================================================================
// Instructions:
#ifndef _TLB_H
#define _TLB_H
#include "stdint.h"
#include "E500V2RegBits.h"

extern void write_tlb(u32 _mas0, u32 _mas1, u32 _mas2, u32 _mas3, u32 _mas7);
#define SET_TLB_ENTRY(_tlb, _epn, _rpn, _perms, _wimge, _ts, _esel, _sz, _iprot) \
	{ .mas0 = FSL_BOOKE_MAS0(_tlb, _esel, 0), \
	  .mas1 = FSL_BOOKE_MAS1(1, _iprot, 0, _ts, _sz), \
	  .mas2 = FSL_BOOKE_MAS2(_epn, _wimge), \
	  .mas3 = FSL_BOOKE_MAS3(_rpn, 0, _perms), \
	  .mas7 = FSL_BOOKE_MAS7(_rpn), }

typedef struct{
	unsigned long	mas0;
	unsigned long	mas1;
	unsigned long	mas2;
	unsigned long	mas3;
	unsigned long	mas7;
}tagTlbConfig;

#endif /* _TLB_H */
