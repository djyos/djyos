// =============================================================================
// Copyright (C) 2012-2020 	 All Rights Reserved
// FileDescription:This file used for the law REGISTER defines and configurations
// FileVersion    :
// Author         :
// Create Time    :2014/12/12
// =============================================================================
// Instructions:
#ifndef _LAW_H
#define _LAW_H

#include "stdint.h"

typedef struct {
	unsigned long law_lawbarn;
	unsigned long law_lawarn;
}tagLawConfig;

enum law_size {
	LAW_SIZE_4K = 0xc,
	LAW_SIZE_8K,
	LAW_SIZE_16K,
	LAW_SIZE_32K,
	LAW_SIZE_64K,
	LAW_SIZE_128K,
	LAW_SIZE_256K,
	LAW_SIZE_512K,
	LAW_SIZE_1M,
	LAW_SIZE_2M,
	LAW_SIZE_4M,
	LAW_SIZE_8M,
	LAW_SIZE_16M,
	LAW_SIZE_32M,
	LAW_SIZE_64M,
	LAW_SIZE_128M,
	LAW_SIZE_256M,
	LAW_SIZE_512M,
	LAW_SIZE_1G,
	LAW_SIZE_2G,
	LAW_SIZE_4G,
	LAW_SIZE_8G,
	LAW_SIZE_16G,
	LAW_SIZE_32G,
};
enum law_trgt_if {
	enum_law_target_interface_pcie1=0x02,
	enum_law_target_interface_pcie2 = 0x01,
	enum_law_target_interface_ddrmem_ctr = 0x0f,
	enum_law_target_interface_elbc = 0x04
};


#define  cn_lawbarn_msk      		0xffffffff
#define  cn_lawarn_en_msk    		0x01
#define  cn_lawarn_size_msk         0x3f
#define  cn_lawarn_target_msk       0x1f

//bit8---bit31  presents the high 24bits
//remember that we only use the 32bits
//so shift 12
#define FSL_SET_LAW_ENTRY(lawbarn, en_law, size, target_if) \
	{ .law_lawbarn = (lawbarn>>12) & cn_lawbarn_msk,\
	  .law_lawarn  = ((en_law & cn_lawarn_en_msk)<<31)|\
    				((target_if & cn_lawarn_target_msk)<<20)|\
    				((size & cn_lawarn_size_msk)<<0)}

void LAW_Setup(void);
#endif
