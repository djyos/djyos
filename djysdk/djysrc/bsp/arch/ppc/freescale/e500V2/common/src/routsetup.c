#include "stdint.h"
#include "DDRC.h"
#include "LAW.h"
#include "TLB.h"

extern const tagTlbConfig tgTlbConfig[];
extern const int gTlbCfgItem;
/*
@-------------------------------------------------------------------------------
@function:setup the tlb for space0 in space1
@para:void
@return:void
@-------------------------------------------------------------------------------
*/
void TlbSetUp()
{
	int i =0;
	//create the tlb for the system
	for (i = 0; i < gTlbCfgItem; i++)
	{
		write_tlb(tgTlbConfig[i].mas0,
				  tgTlbConfig[i].mas1,
				  tgTlbConfig[i].mas2,
				  tgTlbConfig[i].mas3,
				  tgTlbConfig[i].mas7);
	}
}

extern void DdrcInit(void);
/*
@-------------------------------------------------------------------------------
@function:RoutSetUp
@para:void
@return:void
@-------------------------------------------------------------------------------
*/
void RoutSetUp()
{
	//LAW SETUP
	LAW_Setup();
	//DDR INIT
	DdrcInit();
	//TLB SETUP
	TlbSetUp();
	
	//SET THE L2CACHE AS THE SRAM
	extern void Cache_InitL2(void);
	Cache_InitL2();
	
	return;
}

