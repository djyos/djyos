
/*由于MEMORY命令不能使用符号，这些常量的定义，必须与MEMORY命令处一致 */

IbootSize = 128k;   /*此处定义应与相应板件的APP的memory.lds中定义一致 */
InnerFlashOffset = 0x00000000;
InnerFlashRange = 512k;
InnerRam1Start = 0x10000000;
InnerRam1Size = 32k;  

InnerRam2Start = 0x2007C000;
InnerRam2Size = 32k; 

MEMORY
{
    InnerFlash(RX)    : org = 0x00000000,               len = 512k
    
    RAM1(RXW)       : org = 0x10000000,                 len = 32k
    RAM2(RXW)       : org = 0x2007C000,                 len = 32k

}
