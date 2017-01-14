
/*由于MEMORY命令不能使用符号，这些常量的定义，必须与MEMORY命令处一致 */

IbootSize = 256k;               /*此处定义应与相应板件的APP的memory.lds中定义一致 */
InnerFlashOffset = 0x00400000;
InnerFlashRange = 2048k - 128k;
InnerRamStart = 0x20400000;
InnerRamSize = 384k - 32k;                      /*32K用作nocache用*/

/*将384k内存分成三段，前两段可cache，后面一段非cache*/
/*配置的size = 2^(N+1) ， 4 < N < 31,且须保证MPU起始地址按Size对齐*/
CacheRam1Size    = 256k;        /*第一段cache大小，须保证0x20400000按该size大小对齐*/
CacheRam2Size    = 96k;
NoCacheRamSize   = 32k;         /*非cache区大小*/

RevSectors = 1;                                 /*保留块数，保留位于flash末尾块*/

MEMORY
{
    InnerFlash(RX)   : org = 0x00400000,len = 2048k-128k/*128k用作应用程序存储数据*/    
    
    RAM1(RXW)        : org = 0x20400000,                 len = 384k-32k
    RAM1NOCACHE(RXW) : org = 0x20400000+384k-32k,        len = 32k 
}
