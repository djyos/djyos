########################################################################################
# Initialization file for P1020RDB board 
# Clock Configuration:
#       CPU0: 800 MHz, CPU1:800 MHz, CCB: 400 MHz,
#       DDR: 667 MHz, SYSCLK: 66 MHz
########################################################################################

variable CCSRBAR 0xff700000

proc CCSR {reg_off} {
	global CCSRBAR
	
	return p:0x[format %x [expr {$CCSRBAR + $reg_off}]]
}

proc apply_e500v2_workaround {} {
	# After reset, e500 cores need to run before being able to enter debug mode.
	# Work-around: set a HW BP at reset address and run the core; after the core hits the BP, it enters debug mode
	# e500 cores need to have valid opcode at the interrupt vector

	variable SPR_GROUP "e500 Special Purpose Registers/"	

	#######################################################################
	# Set a breakpoint at the reset address 
	reg ${SPR_GROUP}IAC1 = 0xfffffffc
	reg ${SPR_GROUP}DBCR0 = 0x40800000
	reg ${SPR_GROUP}DBCR1 = 0x00000000

	# Run the core
	config runcontrolsync off
	go
	wait 50   
	config runcontrolsync on
	stop

	# Clear affected registers 	
	reg ${SPR_GROUP}DBSR  = 0x01CF0000
	reg ${SPR_GROUP}DBCR0 = 0x41000000
	reg ${SPR_GROUP}IAC1  = 0x00000000
	reg ${SPR_GROUP}CSRR0 = 0x00000000
	reg ${SPR_GROUP}CSRR1 = 0x00000000
}

proc init_board {} {

	global CCSRBAR

	# bits 4-7 - ROM_LOC
	# 1000 Local bus FCM 8-bit NAND flash small page
	# 1010 Local bus FCM 8-bit NAND flash large page
	# 1101 Local bus GPCM 8-bit ROM
	# 1110 Local bus GPCM 16-bit ROM
	# 1111 Local bus GPCM 16-bit ROM (default)
	# using PORBMSR registers from Global Utilities
	variable ROM_LOC					0x[format %x [expr {[mem [CCSR 0xe0004] -np] & 0x0f000000}]]
	variable FCM_SMALL_PAGE_BOOT_LOC 	"0x08000000"
	variable FCM_LARGE_PAGE_BOOT_LOC 	"0x0A000000"
	variable GPCM_8BIT_BOOT_LOC	 		"0x0D000000"
	variable GPCM_16BIT_BOOT_LOC	 	"0x0E000000"

	variable BOOT_DEVICE		0x[format %x [expr {$ROM_LOC & 0x0c000000 }]]
	variable NAND_BOOT_DEVICE 	"0x08000000"
	variable NOR_BOOT_DEVICE 	"0x0c000000"

	# configure local access windows

	# LAWBAR0 - Local Bus NOR
	# bit 8 - 31 = 0xFF000000 - base addr
	mem [CCSR 0xc08] = 0x000FF000

	# LAWAR0
	# bit 0 = 1 - enable window
	# bit 7-11 = 00100 - Local Bus
	# bit 26 - 31 =  011000 16M - size
	mem [CCSR 0xc10] = 0x80400017

	# LAWBAR1 - PEX1
	# bit 8 - 31 = 0x80000000 - base addr
	mem [CCSR 0xc28] = 0x00080000

	# LAWAR1
	# bit 0 = 1 - enable window
	# bit 7-11 = 00010 - PEX1
	# bit 26 - 31 =  011100 512M - size
	mem [CCSR 0xc30] = 0x8020001c

	# LAWBAR2 - PEX 1
	# bit 8 - 31 = 0xEFC20000 - base addr
	mem [CCSR 0xc48] = 0x000efc00

	# LAWAR2
	# bit 0 = 1 - enable window
	# bit 7-11 = 00010 - PEX1
	# bit 26-31 = 001111 64k - size
	mem [CCSR 0xc50] = 0x8020000f


	# LAWBAR3 - PEX2
	# bit 8 - 31 = 0xA0000000 - base addr
	mem [CCSR 0xc68] = 0x000a0000

	# LAWAR3
	# bit 0 = 1 - enable window
	# bit 7-11 = 00001 - PEX2
	# bit 26 - 31 =  011100 512M - size
	mem [CCSR 0xc70] = 0x8010001c

	# LAWBAR4 - PEX 2
	# bit 8 - 31 = 0xEFC10000 - base addr
	mem [CCSR 0xc88] = 0x000efc10

	# LAWAR4
	# bit 0 = 1 - enable window
	# bit 7-11 = 00001 - PEX1
	# bit 26-31 = 001111 64k - size
	mem [CCSR 0xc90] = 0x8010000f

	# LAWBAR5 - CPLD
	# bit 8 - 31 = 0xEFA00000 - base addr
	mem [CCSR 0xca8] = 0x000efa00
	
	# LAWAR5
	# bit 0 = 1 - enable window
	# bit 7-11 = 00100 - Local Bus
	# bit 26-31 = 010000 128k - size
	mem [CCSR 0xcb0] = 0x80400010

	# LAWBAR6 - Local Bus NAND
	# bit 8 - 31 = 0xEFF00000 - base addr
	mem [CCSR 0xcc8] = 0x000eff00
	
	# LAWAR6
	# bit 0 = 1 - enable window
	# bit 7-11 = 00100 - IFC
	# bit 26-31 = 001011 16k - size
	mem [CCSR 0xcd0] = 0x8040000d

	# LAWBAR11 - DDR
	# bit 8 - 31 = 0x00000000 - base addr
	mem [CCSR 0xd68] = 0x00000000

	# LAWAR11
	# bit 0 = 1 - enable window
	# bit 7-11 = 01111 - DDR
	# bit 26 - 31 =  011101 1G - size
	mem [CCSR 0xd70] = 0x80f0001d
	
	##################################################################################
	# DDR Controllers Setup

	# using PORPLLSR registers from Global Utilities to choose between different DDR Controllers Setups Freqs
	variable DDR_ratio_800 0x1800
	variable DDR_ratio_677 0x1400

	variable DDR_ratio 0x[format %x [expr {[mem [CCSR 0xe0000] -np] & 0x00003E00}]]

	if { $DDR_ratio == $DDR_ratio_677 } {

		# DDR_SDRAM_CFG
		mem [CCSR 0x2110] = 0x470c0008

		#CS0_BNDS
		mem [CCSR 0x2000] = 0x0000003f

		#CS1_BNDS
		mem [CCSR 0x2008] = 0x00000000

		#CS0_CNFG
		mem [CCSR 0x2080] = 0x80014302

		#CS1_CNFG
		mem [CCSR 0x2084] = 0x00000000

		# TIMING_CFG_0
		mem [CCSR 0x2104] = 0x00110104
	
		# TIMING_CFG_1
		mem [CCSR 0x2108] = 0x5d59e544

		# TIMING_CFG_2
		mem [CCSR 0x210c] = 0x0fa888cd

		# TIMING_CFG_3
		mem [CCSR 0x2100] = 0x00020000

		# DDR_SDRAM_CFG_2
		mem [CCSR 0x2114] = 0x24401010

		# DDR_SDRAM_MODE
		mem [CCSR 0x2118] = 0x00441210

		# DDR_SDRAM_MODE_2
		mem [CCSR 0x211c] = 0x00000000

		# DDR_SDRAM_MD_CNTL
		mem [CCSR 0x2120] = 0x00000000

		# DDR_SDRAM_INTERVAL
		mem [CCSR 0x2124] = 0x0a280100

		# DDR_DATA_INIT
		mem [CCSR 0x2128] = 0xdeadbeef	
	
		# DDR_SDRAM_CLK_CNTL
		mem [CCSR 0x2130] = 0x03000000

		# TIMING_CFG_4
		mem [CCSR 0x2160] = 0x00000001

		# TIMING_CFG_5
		mem [CCSR 0x2164] = 0x01401400

		# DDR_ZQ_CNTL
		mem [CCSR 0x2170] = 0x89080600

		# DDR_WRLVL_CNTL
		mem [CCSR 0x2174] = 0x8675f608

		# DDR_CDR_1 
		mem [CCSR 0x2b28] = 0x00000000
	
		# DDR_CDR_2
		mem [CCSR 0x2b2c] = 0x00000000
	}

	if { $DDR_ratio == $DDR_ratio_800 } {

		# DDR_SDRAM_CFG
		mem [CCSR 0x2110] = 0x470c0008

		#CS0_BNDS
		mem [CCSR 0x2000] = 0x0000003f

		#CS1_BNDS
		mem [CCSR 0x2008] = 0x00000000

		#CS0_CNFG
		mem [CCSR 0x2080] = 0x80014302

		#CS1_CNFG
		mem [CCSR 0x2084] = 0x00000000

		# TIMING_CFG_0
		mem [CCSR 0x2104] = 0x00110104
	
		# TIMING_CFG_1
		mem [CCSR 0x2108] = 0x6f6b8644

		# TIMING_CFG_2
		mem [CCSR 0x210c] = 0x0fa888cf

		# TIMING_CFG_3
		mem [CCSR 0x2100] = 0x00030000

		# DDR_SDRAM_CFG_2
		mem [CCSR 0x2114] = 0x24401010

		# DDR_SDRAM_MODE
		mem [CCSR 0x2118] = 0x00441420

		# DDR_SDRAM_MODE_2
		mem [CCSR 0x211c] = 0x00000000

		# DDR_SDRAM_MD_CNTL
		mem [CCSR 0x2120] = 0x00000000

		# DDR_SDRAM_INTERVAL
		mem [CCSR 0x2124] = 0x0c300100

		# DDR_DATA_INIT
		mem [CCSR 0x2128] = 0xdeadbeef	
	
		# DDR_SDRAM_CLK_CNTL
		mem [CCSR 0x2130] = 0x03000000

		# TIMING_CFG_4
		mem [CCSR 0x2160] = 0x00000001

		# TIMING_CFG_5
		mem [CCSR 0x2164] = 0x02401400

		# DDR_ZQ_CNTL
		mem [CCSR 0x2170] = 0x89080600

		# DDR_WRLVL_CNTL
		mem [CCSR 0x2174] = 0x8675f608

		# DDR_CDR_1 
		mem [CCSR 0x2b28] = 0x00000000
	
		# DDR_CDR_2
		mem [CCSR 0x2b2c] = 0x00000000
	}
	
	#delay before enable
	wait 100

	# DDR_SDRAM_CFG
	mem [CCSR 0x2110] = 0xc70c0008
	
	#wait for DRAM data initialization
	wait 100

	#SPI init
	# SPMODE 
	mem [CCSR 0x7000] = 0x80000404 
	# SPIM - catch all events
	mem [CCSR 0x7008] = 0x0000FB00
	# SPMODE0
	mem [CCSR 0x7020] = 0x25170008

	##################################################################################
	# configure Local Bus memory controller 

	if {$BOOT_DEVICE == $NAND_BOOT_DEVICE} {
		# CS0 - NAND Flash 	
		# BR0 base address at 0xEFF00000, port size 8 bit, FCM, ECC checking and generation are enabled for FCM on full-page transfers
		mem [CCSR 0x5000] = 0xEFF00C21

		if {$ROM_LOC == $FCM_SMALL_PAGE_BOOT_LOC} {
			# boot from fcm small page boot location
			# OR0 64KB flash size, FCM (small page NAND Flash)
			mem [CCSR 0x5004] = 0xFFFF03AE
		} else {
			# boot from fcm large page boot location
			# OR0 64KB flash size, FCM (large page NAND Flash)
			mem [CCSR 0x5004] = 0xFFFF07AE
		}
		# CS1 - NOR Flash 
		# BR1 base address at 0xFF000000, port size 16 bit, GPCM, DECC disabled
		mem [CCSR 0x5008] = 0xFF001001 
		# OR1 16MB flash size
		mem [CCSR 0x500c] = 0xFF000FF7
	} elseif {$BOOT_DEVICE == $NOR_BOOT_DEVICE} {

		if {$ROM_LOC == $GPCM_8BIT_BOOT_LOC} {
			# boot from nor gpcm 8 bit boot location
			# CS0 - NOR Flash 
			# BR0 base address at 0xFF000000, port size 8 bit, GPCM, DECC disabled
			mem [CCSR 0x5000] = 0xFF000801 
		} else {
			# boot from nor gpcm 16 bit boot location (default)
			# CS0 - NOR Flash 
			# BR0 base address at 0xFF000000, port size 16 bit, GPCM, DECC disabled
			mem [CCSR 0x5000] = 0xFF001001 
		}
		# OR0 16MB flash size
		mem [CCSR 0x5004] = 0xFF000FF7
		
		# CS1 - NAND Flash 	
		# BR1 base address at 0xF8000000, port size 8 bit, FCM
		mem [CCSR 0x5008] = 0xEFF00C21
		# OR1 64KB flash size, FCM (small page NAND Flash)
		mem [CCSR 0x500c] = 0xFFF003AE
	} else {
		# boot from nor gpcm 16 bit boot location (default)
		# CS0 - NOR Flash 
		# BR0 base address at 0xFF000000, port size 16 bit, GPCM, DECC disabled
		mem [CCSR 0x5000] = 0xFF001001 

		# OR0 16MB flash size
		mem [CCSR 0x5004] = 0xFF000FF7
		
		# CS1 - NAND Flash 	
		# BR1 base address at 0xEFF00000, port size 8 bit, FCM
		mem [CCSR 0x5008] = 0xEFF00C21
		# OR1 64KB flash size, FCM (small page NAND Flash)
		mem [CCSR 0x500c] = 0xFFF003AE
	}

	mem [CCSR 0x50e0] = 0x0000F020

	# CS3 - CPLD 
	# BR3 base address at 0xEFA00000, port size 8 bit, GCPM 
	mem [CCSR 0x5018] = 0xEFA00801
	# OR3 128KB flash size
	mem [CCSR 0x501c] = 0xFFFE09F7

	# LBCR
	mem [CCSR 0x50d0] = 0x4000000f
	# LCRR
	mem [CCSR 0x50d4] = 0x80000008
	
	# L2CTL
	# bit 0 = 0 		- L2E: L2 disabled
	# bit 1 = 1			- L2I: L2 flash invalidate	
	mem [CCSR 0x20000] = 0x[format %x [expr {[mem [CCSR 0x20000] -np] & 0x7FFFFFFF | 0x40000000}]]
}

proc P1020RDB_init_core {} {

	global CCSRBAR
	
	variable proc_id [expr {[reg PIR %d -np]} ]
	variable CAM_GROUP "regPPCTLB1/"
	variable SPR_GROUP "e500 Special Purpose Registers/"
	variable GPR_GROUP "General Purpose Registers/"
	variable SSP_GROUP "Standard Special Purpose Registers/"

	if {$proc_id == 0} {
		##################################################################################
		# move CCSR at 0xE0000000
	
		# CCSRBAR
		# bit 8 - 23 - BASE_ADDR
		mem [CCSR 0x0] = 0x000e0000
		set CCSRBAR 0xe0000000
	
		#######################################################################
		# invalidate BR0
		mem [CCSR 0x5000] = 0x00001000	
	
		# ABIST off
		# L2ERRDIS[MBECCDIS]=1 L2ERRDIS[SBECCDIS]=1
		mem [CCSR 0x20e44] = 0x0000001D
	}

	# activate debug interrupt and enable SPU
	reg	${SSP_GROUP}MSR = 0x02000200
	
	
	##################################################################################
	#	
	#	Memory Map
	#
	#   0x00000000  0x3FFFFFFF  DDR3	  			1G
	#   0x80000000  0x9FFFFFFF  PEX1				512MB
	#   0xA0000000  0xBFFFFFFF  PEX2				512MB
	#   0xE0000000  0xE01FFFFF  CCSR   				2M
	#   0xEFA00000  0xEFA1FFFF  CPLD         		128k
	#   0xEFC00000  0xEFC1FFFF  PEX I/O     		128k
	#   0xEFF00000	0xEFF0FFFF  NAND			    64k
	#   0xFF000000	0xFFFFFFFF  LocalBus NOR FLASH	16M
	#
	##################################################################################
	
	##################################################################################
	# MMU initialization

	# define 32MB   TLB1 entry 0: 0xFF000000 - 0xFFFFFFFF; for Local Bus, cache inhibited, guarded
	reg ${CAM_GROUP}L2MMU_CAM0 = 0x70000FCAFC080000FF000000FF000001
	
	# define 1GB	TLB1 entry 1: 0x00000000 - 0x3FFFFFFF; DDR, cacheable, not guarded
	reg ${CAM_GROUP}L2MMU_CAM1 = 0xA0007FC41C0800000000000000000001
	
	# define 1GB    TLB entry 3: 0x80000000 - 0xBFFFFFFF; for PCI Express 1&2, cache inhibited, guarded
	reg ${CAM_GROUP}L2MMU_CAM3 = 0xA0007FCA1C0800008000000080000001

	# define 1MB	TLB1 entry 6: 0xE0000000 - 0xE00FFFFF; CCSR Space, non cacheable, guarded
	reg ${CAM_GROUP}L2MMU_CAM6 = 0x500003CAFC080000E0000000E0000001

	# define 1MB	TLB1 entry 7: 0xE0100000 - 0xE01FFFFF; CCSR Space, non cacheable, guarded
	reg ${CAM_GROUP}L2MMU_CAM7 = 0x500003CAFC080000E0100000E0100001

	# define 1MB    TLB entry 8: 0xEFC00000 - 0xEFCFFFFF; for PCI Express I/O, cache inhibited, guarded
	reg ${CAM_GROUP}L2MMU_CAM8 = 0x500003CAFC080000EFC00000EFC00001

	# define 64k   TLB1 entry 10: 0xEFF00000 - 0xEFF0FFFF
	reg ${CAM_GROUP}L2MMU_CAM10 = 0x300000CA1C080000EFF00000EFF00001

	# define 256k   TLB1 entry 11: 0xEFA00000 - 0xEFa3FFFF
	reg ${CAM_GROUP}L2MMU_CAM11 = 0x400001CA1C080000EFA00000EFA00001

	# disable & invalidate all core caches
	reg ${SPR_GROUP}L1CSR0 = 0x2
	reg ${SPR_GROUP}L1CSR1 = 0x0
	reg ${SPR_GROUP}L1CSR1 = 0x2
	
	# init board, only when the init is run for core 0
	if {$proc_id == 0} {
        init_board
    }
    
	##################################################################################
	# Interrupt vectors initialization 
	# interrupt vectors in RAM at 0x00000000
	# IVPR (default reset value) 
	reg ${SPR_GROUP}IVPR = 0x[expr {${proc_id} << 1}]0000000
	
	# interrupt vector offset registers 
	# IVOR0 - critical input
	reg ${SPR_GROUP}IVOR0 = 0x00000100	
	# IVOR1 - machine check
	reg ${SPR_GROUP}IVOR1 = 0x00000200	
	# IVOR2 - data storage
	reg ${SPR_GROUP}IVOR2 = 0x00000300	
	# IVOR3 - instruction storage
	reg ${SPR_GROUP}IVOR3 = 0x00000400	
	# IVOR4 - external input
	reg ${SPR_GROUP}IVOR4 = 0x00000500	
	# IVOR5 - alignment
	reg ${SPR_GROUP}IVOR5 = 0x00000600	
	# IVOR6 - program
	reg ${SPR_GROUP}IVOR6 = 0x00000700
	# IVOR8 - system call
	reg ${SPR_GROUP}IVOR8 = 0x00000c00	
	# IVOR10 - decrementer
	reg ${SPR_GROUP}IVOR10 = 0x00000900	
	# IVOR11 - fixed-interval timer interrupt
	reg ${SPR_GROUP}IVOR11 = 0x00000f00	
	# IVOR12 - watchdog timer interrupt
	reg ${SPR_GROUP}IVOR12 = 0x00000b00	
	# IVOR13 - data TLB errror
	reg ${SPR_GROUP}IVOR13 = 0x00001100	
	# IVOR14 - instruction TLB error
	reg ${SPR_GROUP}IVOR14 = 0x00001000	
	# IVOR15 - debug
	reg ${SPR_GROUP}IVOR15 = 0x00001500	
	# IVOR32 - SPE-APU unavailable
	reg ${SPR_GROUP}IVOR32 = 0x00001600	
	# IVOR33 - SPE-floating point data exception
	reg ${SPR_GROUP}IVOR33 = 0x00001700
	# IVOR34 - SPE-floating point round exception
	reg ${SPR_GROUP}IVOR34 = 0x00001800
	# IVOR35 - performance monitor
	reg ${SPR_GROUP}IVOR35 = 0x00001900	

    
   	##################################################################################
	# Debugger settings
	
	# infinite loop at program exception to prevent taking the exception again
    mem v:0x[expr {${proc_id} << 1}]0000700 = 0x48000000
 	mem v:0x[expr {${proc_id} << 1}]0001500 = 0x48000000
	
	##################################################################################
	apply_e500v2_workaround
	
	# enable floating point
	reg ${SSP_GROUP}MSR = 0x02001200
	
	# enable machine check
	reg ${SPR_GROUP}HID0 = 0x00004000
	
	
	#Trap debug event enable
	reg ${SPR_GROUP}DBCR0 = 0x41000000

	#  set the PC at the reset address (for debug-->connect)
	reg ${GPR_GROUP}PC = 0xFFFFFFFC
	# for debugging starting at program entry point when stack is not initialized
	reg ${GPR_GROUP}SP = 0x0000000F

	if {$proc_id == 0} {
		# BRRL - enable all cores
		mem [CCSR 0x1010] = 0x03000000
	}
}

proc envsetup {} {
	# Environment Setup
	radix x 
	config hexprefix 0x
	config MemIdentifier v
	config MemWidth 32 
	config MemAccess 32 
	config MemSwap off
}

#-------------------------------------------------------------------------------
# Main                                                                          
#-------------------------------------------------------------------------------

  envsetup
  
  P1020RDB_init_core
