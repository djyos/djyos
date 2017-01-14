
/*由于MEMORY命令不能使用符号，这些常量的定义，必须与MEMORY命令处一致 */

IbootSize = 1M;   /*此处定义应与相应板件的APP的memory.lds中定义一致 */
RAM1Start = 0x31000000;
RAM1Size  = 16M;
RAM2Start = 0x32000000;
RAM2Size  = 16M;

MEMORY
{
    ROM(RX)    	    : ORIGIN = 0x30000000,              LENGTH = 8M   
    
    RAM_nocache(RW) : ORIGIN = 0x30800000,              LENGTH  = 8M  
    RAM1(RXW)       : ORIGIN = 0x31000000,              LENGTH  = 16M
    RAM2(RXW)       : ORIGIN = 0x32000000,              LENGTH  = 16M
}