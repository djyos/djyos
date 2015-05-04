########################################################################################
# Initialization file for P1020RDB board 
# Clock Configuration:
#       CPU0: 800 MHz, CPU1:800 MHz, CCB: 400 MHz,
#       DDR: 667/800 MHz, SYSCLK: 66 MHz
########################################################################################

proc envsetup {} {
	# Environment Setup
	radix x 
	config hexprefix 0x
	config MemIdentifier v
	config MemWidth 32 
	config MemAccess 32 
	config MemSwap off
}

variable CCSRBAR 0xff700000

proc CCSR {reg_off} {
	global CCSRBAR
	
	return p:0x[format %x [expr {$CCSRBAR + $reg_off}]]
}

#-------------------------------------------------------------------------------
# Main                                                                          
#-------------------------------------------------------------------------------

envsetup

# After reset, e500 cores need to run before being able to enter debug mode.
# Work-around: set a HW BP at reset address and run the core; after the core hits the BP, it enters debug mode
# e500 cores need to have valid opcode at the interrupt vector, so temporarily use SRAM for this purpose

variable SPR_GROUP "e500 Special Purpose Registers/"
variable SSP_GROUP "Standard Special Purpose Registers/"
variable GPR_GROUP "General Purpose Registers/"

# ABIST off - needed in order to use L2 SRAM reliably
# L2ERRDIS[MBECCDIS]=1 L2ERRDIS[SBECCDIS]=1
mem [CCSR 0x20e44] = 0x0000001D		

# Configure internal SRAM at highest memory address to cover the default MMU page

# L2CTL
# bit 0 = 0 		- L2E: L2 SRAM disabled
# bit 2-3 = 01 		- L2SIZ: = 256K
# bit 13-15 = 001 	- L2SRAM: Entire array is a single SRAM (256KB)
mem [CCSR 0x20000] = 0x10010000

# L2SRBAR0
# bit 0-17 = BASE addr: 0xFFFC0000
mem [CCSR 0x20100] = 0xFFFC0000

# L2SRBAREA0
# bit 28-31 = EXTENTED BASE addr: 0x00000000
mem [CCSR 0x20104] = 0x00000000

# L2CTL
# bit 0 = 1 		- L2E: L2 SRAM enable
mem [CCSR 0x20000] = 0x90010000
	
# Set interrupt vectors in SRAM
# IVPR 
reg ${SPR_GROUP}IVPR = 0xFFFF0000 	
# debug - (a valid instruction should exist to be fetched)
reg ${SPR_GROUP}IVOR15 = 0x0000F000	
# program
reg ${SPR_GROUP}IVOR6 = 0x0000F700	

# Put a valid opcode at debug and program exception vector addresses
mem v:0xFFFFF000 = 0x48000000
mem v:0xFFFFF700 = 0x48000000

# Set a breakpoint at the reset address 
reg ${SPR_GROUP}IAC1  = 0xfffffffc
reg ${SPR_GROUP}DBCR0 = 0x40800000
reg ${SPR_GROUP}DBCR1 = 0x00000000
reg	${SSP_GROUP}MSR   = 0x02000200

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


# L2 disable, invalidate and reset to cache mode (no SRAM)
mem [CCSR 0x20000] = 0x50000000


#  set the PC back to the reset address (for debug-->connect)
reg	${GPR_GROUP}PC = 0xfffffffc
# re-activate debug interrupt and enable SPU (hitting the HW bp altered it)
reg	${SSP_GROUP}MSR = 0x02000200
# for debugging starting at program entry point when stack is not initialized
reg	${GPR_GROUP}SP = 0x0000000F


