
/*由于MEMORY命令不能使用符号，这些常量的定义，必须与MEMORY命令处一致 */

IbootSize = 256k;   /*此处定义应与相应板件的APP的memory.lds中定义一致 */
InnerFlashOffset = 0x08000000;
InnerFlashRange = 1024k;
InnerRamStart = 0x20000000;
InnerRamSize = 320K;
 
CacheRam1Size    = 256k;        /*第一段cache大小*/
CacheRam2Size    = 32K;
NoCacheRamSize   = 32k;         /*非cache区大小*/

RevSectors = 0;                                 /*保留块数，保留位于flash末尾块*/

MEMORY
{
    InnerFlash(RX)   : org = 0x08000000,         len = 1024k   
    RAM1(RXW)        : org = 0x20000000,         len = 320K-32K
    RAM1NOCACHE(RXW) : org = 0x20000000+256k+32k,len = 32k 
}