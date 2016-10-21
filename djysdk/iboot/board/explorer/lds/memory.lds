

MEMORY
{
    IbootRom(RX)    : org = 0x08000000,                 len = 256k
    
    AppCtlRom(R)    : org = 0x08000000+256k,            len = 256
    AppVerfyRom(R)  : org = 0x08000000+256k+256,        len = 256
    AppRom(RX)      : org = 0x08000000+256k+512,        len = 1024k-256k-512
    
    RAM1(RXW)       : org = 0x20000000,                 len = 128k-16
    BootFlagRAM1(RW): org = 0x20000000+128k-16,         len = 16
    extram(RXW)     : org = 0x68000000,                 len = 0x100000
}