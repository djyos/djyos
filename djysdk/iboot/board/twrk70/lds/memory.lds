

MEMORY
{
    IbootRom(RX)    : org = 0x00000000,                 len = 512k
    
    AppCtlRom(R)    : org = 0x00000000+512k,            len = 256
    AppVerfyRom(R)  : org = 0x00000000+512k+256,        len = 256
    AppRom(RX)      : org = 0x00000000+512k+512,        len = 1024k-512k-512
    
    RAM1(RXW)       : org = 0x1FFF0000,                 len = 128k-16
    BootFlagRAM1(RW): org = 0x1FFF0000+128k-16,         len = 16
    ExtRAM(RXW)     : org = 0x08100000,                 len = 127M
}