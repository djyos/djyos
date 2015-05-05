// =============================================================================
// Copyright (C) 2012-2020 	 All Rights Reserved
// FileDescription:
// FileVersion    :
// Author         :
// Create Time    :2014/12/12
// =============================================================================
// Instructions:
// this file defines the whole space in the system, including the tmp space used 
// in the startup, some important para such as the CCB clock
#ifndef __SYSSPACE_H
#define __SYSSPACE_H
#define MB                       0x100000

//**************TLB MEM MAP***************//
//address for ccsr
#define CN_CCSR_RESETADDR         0XFF700000
#define CN_CCSR_TESTADDR          (CN_CCSR_RESETADDR+1*MB)
#define CN_CCSR_NEWADDR_V         0XE0000000
#define CN_CCSR_NEWADDR_PL        CN_CCSR_NEWADDR_V
#define CN_CCSR_NEWADDR_PH        0x0

//stack for the boot--4K
#define CN_SRAM_STACK             0xEFEC0000  
#define CN_SRAM_STACK_SIZE        0X1000

//DDR---1GB
#define CN_DDR_ADDR_V             0x00
#define CN_DDR_ADDR_P             CN_DDR_ADDR_V
#define CN_DDR_CODEADDR           CN_DDR_ADDR_V 

#define CN_DDR_ADDR_64MB_V0        (CN_DDR_ADDR_V)          //用作代码段，用作代码保护
#define CN_DDR_ADDR_64MB_V1        (CN_DDR_ADDR_V + 64*MB)
#define CN_DDR_ADDR_64MB_V2        (CN_DDR_ADDR_V + 128*MB)
#define CN_DDR_ADDR_64MB_V3        (CN_DDR_ADDR_V + 192*MB)
#define CN_DDR_ADDR_256MB_V1       (CN_DDR_ADDR_V + 256*MB)
#define CN_DDR_ADDR_256MB_V2       (CN_DDR_ADDR_V + 512*MB)
#define CN_DDR_ADDR_256MB_V3       (CN_DDR_ADDR_V + 768*MB)

#define CN_LBUS_ADDR              0xf00000000   //256MB
//NAND FLASH--256MB
#define CN_NANDFLASH_ADDR_V       0xEFA00000
#define CN_NANDFLASH_ADDR_P       CN_NANDFLASH_ADDR_V

//NORFLASH--64MB
#define CN_NORFLASH_CODEADDR      0xFFF00000                //storage the code
#define CN_NORFLASH_ADDR_V        0xFF000000
#define CN_NORFLASH_ADDR_P        CN_NORFLASH_ADDR_V
#define CN_NORFLASH_BASE          0xFF000000                //16MB


#define CN_BOOT_ADDR4K            0XFFFFF000                //THE BOOT 4K ADDR




#endif
