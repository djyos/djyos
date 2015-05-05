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
//	extern void djyip_main(void);
//	djyip_main();	
//	extern void timertest(void);
//	timertest();
//	extern int VncServer_Main();
//	VncServer_Main();	
	while(1)
    { 
    	Djy_EventDelay(1000*1000);
    }
}







