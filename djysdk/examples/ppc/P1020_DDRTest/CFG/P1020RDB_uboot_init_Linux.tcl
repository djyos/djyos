#
# Init file for Linux kernel debug using uboot initialization scenario. 
#

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

variable GPR_GROUP "General Purpose Registers/"
variable SPR_GROUP "e500 Special Purpose Registers/"
variable SSP_GROUP "Standard Special Purpose Registers/"

# let uboot initialize the board.
# reset & run
reset run		
# the uboot must be stopped before its prompt appear
wait 1000	
# the board is prepared to download and start the kernel
config runcontrolsync on
stop		

#############
#
# activate debug interrupt and enable SPU
reg	${SSP_GROUP}MSR = 0x02000200

#########################
#
# CW debugger settings
#
#Trap debug event enable


#disable the cache
reg ${SPR_GROUP}L1CSR0 = 0x0
reg ${SPR_GROUP}L1CSR1 = 0x0

reg ${GPR_GROUP}SP = 0x0000000F
