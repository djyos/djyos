// =============================================================================
// Copyright (C) 2012-2020 	 All Rights Reserved
// FileDescription:This file is the implement of the physic route
// FileVersion    :
// Author         :
// Create Time    :2014/12/12
// =============================================================================
// Instructions:
// This file create the physic route to the interface, such as the IFC DDR PCIE
#include "LAW.h"
#include "SysSpace.h"
//local access window defines
#define   CN_LAW_ADDR          (CN_CCSR_NEWADDR_V + 0x0C08)
#define   CN_LAW_NUM           12


typedef struct
{
	volatile u32 lawbarn;
	volatile u32 reserved_0;
	volatile u32 lawwarn;
	volatile u32 reserved_1[5];
}tagLawMem;

extern tagLawConfig tgLawConfig[];
extern const int gLawCfgItem;
/*
@-------------------------------------------------------------------------------
@function:setup the law
@para:void
@return:void
@-------------------------------------------------------------------------------
*/
void LAW_Setup(void)
{
	int i;
	//local access window configuration
	tagLawMem *lawitem;
	lawitem = (tagLawMem *)CN_LAW_ADDR;
	for (i = 0; i <  gLawCfgItem; i++)
	{
		lawitem->lawbarn = tgLawConfig[i].law_lawbarn;
		lawitem->lawwarn = tgLawConfig[i].law_lawarn;
		lawitem++;
	}
}
