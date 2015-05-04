/*******************************************************************************
// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: debug.lds //调试版本的链接脚本
// 模块版本: V1.00
// 创建人员: zhangqf_cyg
// 创建时间: 2:29:36 PM/Dec 20, 2013
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 备注---release版本程序的LDS文件，生产可执行文件的重要依据
//        关系着整个代码的布局
 ******************************************************************************/
MEMORY
{
    DDR(RXW)    : org = 0x00000000, len = 1024M
}

SECTIONS
{

    /*loader txt*/
    text_loader :
    {
        . = ALIGN(0x04);
        *liblibos_debug.a:cpustarts.o(.text .text.* .rodata .rodata.* ) 
        *liblibos_debug.a:cpustart.o(.text .text.* .rodata .rodata.* ) 
        *liblibos_debug.a:elbc.o(.text .text.* .rodata .rodata.* ) 
        *liblibos_debug.a:cpus.o(.text .text.* .rodata .rodata.* ) 
         
         *liblibos_debug.a:pre_loader.o (.text .text.* .rodata .rodata.*) 
           
    }>DDR
    

    /*prepare the loader envirionment--code to preload*/
    rw_preload_rDDR3_lnorflash_text :
    {
         . = ALIGN(0x04);
         *liblibos_debug.a:loader.o (.text .text.* .rodata .rodata.*)
         *liblibos_debug.a:int.o (.text .text.* .rodata .rodata.*)
         *liblibos_debug.a:wdt_hal.o(.text .text.* .rodata .rodata.* )
         *liblibos_debug.a:timer_hard.o(.text .text.* .rodata .rodata.* )
         *liblibos_debug.a:cpu_peri_gpio.o(.text .text.* .rodata .rodata.*)
         *liblibos_debug.a:exp_decoder.o(.text .text.* .rodata .rodata.*) 
         *liblibos_debug.a:heap-static.o(.text .text.* .rodata .rodata.*) 
          
         
         *liblibos_debug.a:exp_asm.o(.text .text.* .rodata .rodata.*)  
         *liblibos_debug.a:exp_c.o(.text .text.* .rodata .rodata.*)  
         *liblibos_debug.a:exp_vec.o(.text .text.* .rodata .rodata.*)  
         *liblibos_debug.a:cpu.o(.text .text.* .rodata .rodata.*)  
         *liblibos_debug.a:int_hard.o(.text .text.* .rodata .rodata.*)  
         *liblibos_debug.a:pic_timer.o(.text .text.* .rodata .rodata.*)  
                
         ./src/OS_prjcfg/config/int_config.o(.text .text.* .rodata .rodata.*)  
         ./src/user/critical/*(.text .text.* .rodata .rodata.* )     
    }>DDR

    rw_preload_rDDR3_lnorflash_data :
    {
         . = ALIGN(0x04);
         *liblibos_debug.a:loader.o (.data .data.* .sdata  .sdata.*)
         *liblibos_debug.a:int.o (.data .data.* .sdata  .sdata.*)
         *liblibos_debug.a:wdt_hal.o(.data .data.* .sdata  .sdata.*)
         *liblibos_debug.a:timer_hard.o(.data .data.* .sdata  .sdata.*)
         *liblibos_debug.a:cpu_peri_gpio.o(.data .data.* .sdata  .sdata.*)
         *liblibos_debug.a:exp_decoder.o(.data .data.* .sdata  .sdata.*)
         *liblibos_debug.a:heap-static.o(.data .data.* .sdata  .sdata.*)  
           
         
         *liblibos_debug.a:exp_asm.o(.data .data.* .sdata  .sdata.*)  
         *liblibos_debug.a:exp_c.o(.data .data.* .sdata  .sdata.*)  
         *liblibos_debug.a:exp_vec.o(.data .data.* .sdata  .sdata.*) 
         *liblibos_debug.a:cpu.o(.data .data.* .sdata  .sdata.*)   
         *liblibos_debug.a:int_hard.o(.data .data.* .sdata  .sdata.*)  
         *liblibos_debug.a:pic_timer.o(.data .data.* .sdata  .sdata.*)  
                
         ./src/OS_prjcfg/config/int_config.o(.data .data.* .sdata  .sdata.*)  
         ./src/user/critical/*(.data .data.* .sdata  .sdata.*)
         
    }>DDR

    zi_preload_rDDR3_lDDR3 (NOLOAD) :
    {
         . = ALIGN(0x04);
         *liblibos_debug.a:loader.o (.bss .bss.* .sbss .sbss.* SCOMMON COMMON)
         *liblibos_debug.a:int.o (.bss .bss.* .sbss .sbss.* SCOMMON COMMON)
         *liblibos_debug.a:wdt_hal.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)
         *liblibos_debug.a:timer_hard.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)
         *liblibos_debug.a:cpu_peri_gpio.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)
         *liblibos_debug.a:exp_decoder.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)  
         *liblibos_debug.a:heap-static.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON) 
                  
         *liblibos_debug.a:exp_asm.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)  
         *liblibos_debug.a:exp_c.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)  
         *liblibos_debug.a:exp_vec.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)  
         *liblibos_debug.a:cpu.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)  
         *liblibos_debug.a:int_hard.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)  
         *liblibos_debug.a:pic_timer.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)  
                
         ./src/OS_prjcfg/config/int_config.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)  
         ./src/user/critical/*(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)

    }>DDR


    /*the other sys code  except the sys loader*/
    rw_sysload_rDDR3_lnorflash_text :
    {
        . = ALIGN(0x20);
         cached_textsec_start = .;
        * (.Cached_textsec)
         cached_textsec_end = .;
        * (.text .text.* .rodata .rodata.*)
    }>DDR

    rw_sysload_rDDR3_lnorflash_data :
    {
         . = ALIGN(0x20);
         cached_datasec_start = .;
         * (.Cached_datasec)
         cached_datasec_end = .;
         * (.data .data.* .sdata  .sdata.*)
    }>DDR


    zi_sysload_rDDR3_lDDR3 (NOLOAD) :
    {
        . = ALIGN(0x20);
        cached_bsssec_start = .;
        * (.Cached_bsssec)
        cached_bsssec_end = .;
        * (.bss .bss.* .sbss .sbss.* SCOMMON COMMON)
    }>DDR
    
    stack_rDDR3_lDDR3 (NOLOAD) :
    {
        . = ALIGN(0x04);
    
        irq_bottom = .;
        KEEP(* (.stack.irq .stack.irq.*))
        irq_top = ALIGN(0x10);

        usr_bottom = .;
        KEEP(* (.stack.usr .stack.usr.*))
        usr_top = ALIGN(0x10);

        prg_exp_bottom = .;
        KEEP(* (.stack.prg_exp .stack.prg_exp.*))
        prg_exp_top = ALIGN(0x10);

        cint_exp_bottom = .;
        KEEP(* (.stack.cint_exp .stack.cint_exp.*))
        cint_exp_top = ALIGN(0x10);

        dbg_exp_bottom = .;
        KEEP(* (.stack.dbg_exp .stack.dbg_exp.*))
        dbg_exp_top = ALIGN(0x10);

        mc_exp_bottom = .;
        KEEP(* (.stack.mc_exp .stack.mc_exp.*))
        mc_exp_top = ALIGN(0x10);

        sys_exp_bottom = .;
        KEEP(* (.stack.sys_exp .stack.sys_exp.*))
        sys_exp_top = ALIGN(0x10);
        
        sys_core1_stackbottom = .;
        KEEP(* (.stack.core1_stack .stack.core1_stack.*))
        sys_core1_stacktop = ALIGN(0x10);
        
    }>DDR
    
    preload_cp_table :
    {
        . = ALIGN(0x04);
    
        preload_copy_table = .;
        LONG(16)
        LONG(3)

        LONG(LOADADDR(rw_preload_rDDR3_lnorflash_text))
        LONG(ADDR(rw_preload_rDDR3_lnorflash_text))
        LONG(SIZEOF(rw_preload_rDDR3_lnorflash_text))
        LONG(1)

        LONG(LOADADDR(rw_preload_rDDR3_lnorflash_data))
        LONG(ADDR(rw_preload_rDDR3_lnorflash_data))
        LONG(SIZEOF(rw_preload_rDDR3_lnorflash_data))
        LONG(1)

        LONG(LOADADDR(zi_preload_rDDR3_lDDR3))
        LONG(ADDR(zi_preload_rDDR3_lDDR3))
        LONG(SIZEOF(zi_preload_rDDR3_lDDR3))
        LONG(0)
    }>DDR

    sysload_cp_table :
    {
        . = ALIGN(0x04);
    
        sysload_copy_table = .;
        LONG(16)
        LONG(3)

        LONG(LOADADDR(rw_sysload_rDDR3_lnorflash_text))
        LONG(ADDR(rw_sysload_rDDR3_lnorflash_text))
        LONG(SIZEOF(rw_sysload_rDDR3_lnorflash_text))
        LONG(1)

        LONG(LOADADDR(rw_sysload_rDDR3_lnorflash_data))
        LONG(ADDR(rw_sysload_rDDR3_lnorflash_data))
        LONG(SIZEOF(rw_sysload_rDDR3_lnorflash_data))
        LONG(1)

        LONG(LOADADDR(zi_sysload_rDDR3_lDDR3))
        LONG(ADDR(zi_sysload_rDDR3_lDDR3))
        LONG(SIZEOF(zi_sysload_rDDR3_lDDR3))
        LONG(0)
    }>DDR
    


    heap_cp_table :
    {    
        . = ALIGN(0x04);
        pHeapList = .;
        LONG(1)                     /*表示该堆由1段内存组成*/
        LONG(0)                     /*该堆上分配的内存的对齐尺寸，0表示使用系统对齐*/
        LONG(sys_heap_bottom)   /*第一段基址，须符合对齐要求*/
        LONG(sys_heap_top)      /*第一段地址上限（不含），须符合对齐要求*/
        LONG(128)                   /*第一段页尺寸=128bytes*/
        BYTE(0x73)                  /*'s'*/
        BYTE(0x79)                  /*'y'*/
        BYTE(0x73)                  /*'s'*/
        BYTE(0)                     /*串结束，"sys"是堆的名字*/
        . = ALIGN(0x04);
        LONG(0)                     /*结束标志*/
        . = ALIGN(0x20);
        sys_heap_bottom = .;
        
    }>DDR
    
    sys_heap_top = 1024M;
}
/**** The End ****/


