

MEMORY
{
    IbootRom(RX)    : org = 0x00000000,                 len = 128k
    
    AppCtlRom(R)    : org = 0x00000000+128k,            len = 256
    AppVerfyRom(R)  : org = 0x00000000+128k+256,        len = 256
    AppRom(RX)      : org = 0x00000000+128k+512,        len = 512k-128k-512
    
    RAM1(RXW)       : org = 0x10000000,                 len = 32k-16
    BootFlagRAM1(RW): org = 0x10000000+32k-16,          len = 16
    RAM2(RXW)       : org = 0x2007C000,                 len = 32k

}