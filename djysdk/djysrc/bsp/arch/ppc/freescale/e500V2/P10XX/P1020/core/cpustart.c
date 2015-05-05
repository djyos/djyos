// =============================================================================
// Copyright (C) 2012-2020 	 All Rights Reserved
// FileDescription:This file define the cpu init,such as the IFC and some devices
// FileVersion    :
// Author         :
// Create Time    :2014/12/12
// =============================================================================
// Instructions:

// cpu basic init
void CpuInit(void)
{
	extern void P1020ElbcInit();
	P1020ElbcInit();	
	return;
}
