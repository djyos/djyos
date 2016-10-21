MEMORY
{
    IbootRom(RX)         : org = 0x08000000,           len = 128k    
    
    AppCtlRom(RW)     : org = 0x08000000+128k,	   len = 256
    AppVerfyRom(RW)   : org = 0x08000000+128k+256, len = 256    
    AppRom(RXW)       : org = 0x08000000+128k+512, len = 512k-128k-512
        
    extram(RXW)      : org = 0x60000000,           len = 0x400000
        
    RAM1(RXW)        : org = 0x20000000,           len = 64k-16
    BootFlagRAM1(RW) : org = 0x20000000+64k-16,    len = 16
}

