#include "stdint.h"
#include "stddef.h"
#include "stdio.h"

#include "os.h"

void djy_main(void)
{
#ifdef DEBUG
	printk("P1020rdb_debug\n\r");
#else
	printk("P1020rdb_release\n\r");
#endif
	while(1)
    { 
//		extern int DDRTETS_TI(void);
//		DDRTETS_TI();
		
		extern int memtester_main(int argc, char **argv) ;
#define CN_MEMTESTER_PARA    3
		int argc = CN_MEMTESTER_PARA;
		char *argv[CN_MEMTESTER_PARA] = {__FUNCTION__,"0x0000","0x40000000"};
		memtester_main(argc,argv);
		
    	Djy_EventDelay(1000*1000);
    }
}







