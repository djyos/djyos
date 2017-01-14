/*由于MEMORY命令不能使用符号，这些常量的定义，必须与MEMORY命令处一致 */

IbootSize = 128k;   /*此处定义应与相应板件的APP的memory.lds中定义一致 */
InnerFlashOffset = 0x08000000;
InnerFlashRange = 512k;
InnerRamStart = 0x20000000;
InnerRamSize = 0x10000;

ExtRamStart = 0x60000000;
ExtRamSize = 0x400000;

MEMORY
{
    InnerFlash(RX)   : org = 0x08000000,           len = 512k    
    
    RAM1(RXW)        : org = 0x20000000,           len = 64k
    extram(RXW)      : org = 0x60000000,           len = 0x400000  
}
