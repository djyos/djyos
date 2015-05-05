#include "os_inc.h"
#include "cpu_peri.h"

ptu32_t djy_main(void);

ptu32_t __djy_main(void)
{
    djy_main();
    return 0;
}

//----组件初始化2-----------------------------------------------------------
//功能：可选组件初始化函数共有3个:
//      1、sys_module_init，在sys_init.c中，参见该函数注释
//      2、prj_module_init，在这里初始化的模块，需要调用跟具体工程相关的代码，例
//         如键盘模块，除了调用module_init_keyboard函数外，还要调用跟硬件相关的
//         module_init_keyboard_hard函数。
//      前两步初始化时，块相联分配方式还没有初始化，驱动中如果用到动态分配，使用
//      的是准静态分配，关于准静态分配的说明，参见mems.c文件。
//      3、run_module_init，参见该函数注释。
//参数：无
//返回：无
//-----------------------------------------------------------------------------
void prj_module_init(void)
{
    uint16_t evtt_main;

#if CN_CFG_KEYBOARD == 1
    Keyboard_ModuleInit(0);
#endif

#if CN_CFG_DJYFS == 1
    Djyfs_ModuleInit(0);
#if CN_CFG_DJYFS_FLASH == 1
    DFFSD_ModuleInit();
#endif
#endif

#if(CN_CFG_UART0_USED == 1)
    module_init_uart0(0);
#endif
#if(CN_CFG_UART1_USED == 1)
    module_init_uart1(0);
#endif
#if(CN_CFG_UART2_USED == 1)
    module_init_uart2(0);
#endif
#if(CN_CFG_UART3_USED == 1)
    module_init_uart3(0);
#endif
    evtt_main = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,2,
                                __djy_main,CN_MAIN_STACK,"main function");
    //事件的两个参数暂设为0，如果用shell启动，可用来采集shell命令行参数
    Djy_EventPop(evtt_main,NULL,0,NULL,0,0,0);
    return ;
}

//----组件初始化3-----------------------------------------------------------
//功能：可选组件初始化函数共有3个:
//      1、sys_module_init，在sys_init.c中，参见该函数注释
//      2、prj_module_init，参见该函数注释。
//      3、run_module_init，本函数在djy_main函数开始处调用，需要操作系统的事件
//          调度开始后，或者需要块相联内存分配初始化后才能初始化的模块。
//参数：无
//返回：无
//-----------------------------------------------------------------------------
void run_module_init(void)
{

}

