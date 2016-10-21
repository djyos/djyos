#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "os.h"

void djy_main(void)
{
    printf("%s:»¶Ó­Ê¹ÓÃSR1210°å¼þ\r\n",__FUNCTION__);
    for ( ; ; )
    {
        Djy_EventDelay(1000*mS);
    }
}
