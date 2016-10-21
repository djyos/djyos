


MEMORY
{
    IbootRom(RX)    : ORIGIN = 0x30000000,              LENGTH = 1M   
    AppCtlRom(R)    : ORIGIN = 0x30000000+1M,           LENGTH = 256
    AppVerfyRom(R)  : ORIGIN = 0x30000000+1M+256,       LENGTH = 256
    AppRom(RX)      : ORIGIN = 0x30000000+1M+512,       LENGTH = 8M-1M-512
    
    RAM_nocache(RW) : ORIGIN = 0x30800000,              LENGTH  = 8M  
    RAM1(RXW)       : ORIGIN = 0x31000000,              LENGTH  = 16M
    RAM2(RXW)       : ORIGIN = 0x32000000,              LENGTH  = 16M-16
    
    BootFlagRAM1(RW): ORIGIN = 0x32000000+16M-16,       LENGTH = 16
}