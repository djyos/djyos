
MEMORY
{
    IbootRom(RX)    : org = 0x00400000, 			    len = 256k
    
    AppCtlRom(R)    : org = 0x00400000+256k,			len = 256
    AppVerfyRom(R)  : org = 0x00400000+256k+256,        len = 256
    AppRom(RX)      : org = 0x00400000+256k+512,		len = 2048k-256k-512
    
    RAM1(RXW)       : org = 0x20400000, 			    len = 384k-16
    BootFlagRAM1(RW): org = 0x20400000+384k-16, 		len = 16
}

