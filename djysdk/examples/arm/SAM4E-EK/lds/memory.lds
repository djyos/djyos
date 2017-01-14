
/*由于MEMORY命令不能使用符号，这些常量的定义，必须与MEMORY命令处一致 */

IbootSize = 256k;   /*此处定义应与相应板件的APP的memory.lds中定义一致 */
InnerFlashOffset = 0x00400000;
InnerFlashRange = 1024k;
InnerRamStart = 0x20000000;
InnerRamSize = 128k;

MEMORY
{
    IbootRom(RX)    : org = 0x00400000,                 len = 1024k
    
    RAM1(RXW)       : org = 0x20000000,                 len = 128k
}
