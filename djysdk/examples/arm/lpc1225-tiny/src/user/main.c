#include "os_inc.h"
#include "uart_tiny.h"
#include "cpu_peri.h"
void run_module_init(void);

ptu32_t djy_main(void)
{
    run_module_init();

}
