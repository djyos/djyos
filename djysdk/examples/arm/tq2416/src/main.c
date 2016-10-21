


#include <stdint.h>
#include <stdio.h>
#include <djyos.h>
#include <IAP.h>
#include <cfg/Iboot_config.h>
#include <int.h>
#include <set-cache.h>


ptu32_t djy_main(void)
{
	printf("this is the tq2616 app !\r\n");
	while(1)
	{
		Djy_EventDelay(10000);

	}
}

