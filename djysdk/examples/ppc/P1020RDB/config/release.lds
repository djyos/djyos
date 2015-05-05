/*******************************************************************************
// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: release.lds  //烧录版本的链接脚本
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
ENTRY(__E500V2_RESET)

MEMORY
{
    SYSCODE(RXW)    : org = 0x00000000, len = 64M
    SYSDATA(RXW)    : org = 0x04000000, len = 64M
    DDR3(RXW)       : org = 0x08000000, len = 128M
    FLASHROM(RX)    : org = 0xfff00000, len = 1024K-4K
    RESETVEC(RX)    : org = 0xfffffffc, len = 4
    ROMSTART(RX)    : org = 0xfffff000, len = 4092
}

SECTIONS
{
    /*first command**/
    text_resetvec :
    { 
        . = ALIGN(0x04);
        *liblibos_release.a:resetvec.o(.text .text.*)
    }>RESETVEC

    /*4K code**/
    text_romstart :
    {
        . = ALIGN(0x04);
        *liblibos_release.a:start.o(.text .text.* .rodata .rodata.* )    
        *liblibos_release.a:routsetups.o(.text .text.* .rodata .rodata.* )    
        *liblibos_release.a:routsetup.o(.text .text.* .rodata .rodata.* )    
        *liblibos_release.a:ddrc.o(.text .text.* .rodata .rodata.* )    
        *liblibos_release.a:law.o(.text .text.* .rodata .rodata.* )            
        *liblibos_release.a:loadloader2mem.o(.text .text.* .rodata .rodata.*)  
        *liblibos_release.a:cache.o(.text .text.* .rodata .rodata.* )         
        
    }>ROMSTART
    data_romstart :
    {
        *liblibos_release.a:boardroutcfg.o(.rodata .rodata.*  .data .data.* .sdata  .sdata.*)
    }>ROMSTART
    
    /*loader txt*/
    text_loader :
    {
        . = ALIGN(0x04);
        *liblibos_release.a:cpustarts.o(.text .text.* .rodata .rodata.* ) 
        *liblibos_release.a:cpustart.o(.text .text.* .rodata .rodata.* ) 
        *liblibos_release.a:elbc.o(.text .text.* .rodata .rodata.* ) 
        *liblibos_release.a:cpus.o(.text .text.* .rodata .rodata.* ) 
         
         *liblibos_release.a:pre_loader.o (.text .text.* .rodata .rodata.*) 
           
    }>SYSCODE AT>FLASHROM
    

    /*prepare the loader envirionment--code to preload*/
    rw_preload_rDDR3_lnorflash_text :
    {
         . = ALIGN(0x04);
         *liblibos_release.a:loader.o (.text .text.* .rodata .rodata.*)
         *liblibos_release.a:int.o (.text .text.* .rodata .rodata.*)
         *liblibos_release.a:wdt_hal.o(.text .text.* .rodata .rodata.* )
         *liblibos_release.a:timer_hard.o(.text .text.* .rodata .rodata.* )
         *liblibos_release.a:cpu_peri_gpio.o(.text .text.* .rodata .rodata.*)
         *liblibos_release.a:exp_decoder.o(.text .text.* .rodata .rodata.*) 
         *liblibos_release.a:heap-static.o(.text .text.* .rodata .rodata.*) 
          
         
         *liblibos_release.a:exp_asm.o(.text .text.* .rodata .rodata.*)  
         *liblibos_release.a:exp_c.o(.text .text.* .rodata .rodata.*)  
         *liblibos_release.a:exp_vec.o(.text .text.* .rodata .rodata.*)  
         *liblibos_release.a:cpu.o(.text .text.* .rodata .rodata.*)  
         *liblibos_release.a:int_hard.o(.text .text.* .rodata .rodata.*)  
         *liblibos_release.a:pic_timer.o(.text .text.* .rodata .rodata.*)  
                
         ./src/OS_prjcfg/config/int_config.o(.text .text.* .rodata .rodata.*)  
         ./src/user/critical/*(.text .text.* .rodata .rodata.* )     
    }>SYSCODE AT>FLASHROM

    rw_preload_rDDR3_lnorflash_data :
    {
         . = ALIGN(0x04);
         *liblibos_release.a:loader.o (.data .data.* .sdata  .sdata.*)
         *liblibos_release.a:int.o (.data .data.* .sdata  .sdata.*)
         *liblibos_release.a:wdt_hal.o(.data .data.* .sdata  .sdata.*)
         *liblibos_release.a:timer_hard.o(.data .data.* .sdata  .sdata.*)
         *liblibos_release.a:cpu_peri_gpio.o(.data .data.* .sdata  .sdata.*)
         *liblibos_release.a:exp_decoder.o(.data .data.* .sdata  .sdata.*)
         *liblibos_release.a:heap-static.o(.data .data.* .sdata  .sdata.*)  
           
         
         *liblibos_release.a:exp_asm.o(.data .data.* .sdata  .sdata.*)  
         *liblibos_release.a:exp_c.o(.data .data.* .sdata  .sdata.*)  
         *liblibos_release.a:exp_vec.o(.data .data.* .sdata  .sdata.*) 
         *liblibos_release.a:cpu.o(.data .data.* .sdata  .sdata.*)   
         *liblibos_release.a:int_hard.o(.data .data.* .sdata  .sdata.*)  
         *liblibos_release.a:pic_timer.o(.data .data.* .sdata  .sdata.*)  
                
         ./src/OS_prjcfg/config/int_config.o(.data .data.* .sdata  .sdata.*)  
         ./src/user/critical/*(.data .data.* .sdata  .sdata.*)
         
    }>SYSDATA AT>FLASHROM

    zi_preload_rDDR3_lDDR3 (NOLOAD) :
    {
         . = ALIGN(0x04);
         *liblibos_release.a:loader.o (.bss .bss.* .sbss .sbss.* SCOMMON COMMON)
         *liblibos_release.a:int.o (.bss .bss.* .sbss .sbss.* SCOMMON COMMON)
         *liblibos_release.a:wdt_hal.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)
         *liblibos_release.a:timer_hard.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)
         *liblibos_release.a:cpu_peri_gpio.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)
         *liblibos_release.a:exp_decoder.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)  
         *liblibos_release.a:heap-static.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON) 
                  
         *liblibos_release.a:exp_asm.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)  
         *liblibos_release.a:exp_c.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)  
         *liblibos_release.a:exp_vec.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)  
         *liblibos_release.a:cpu.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)  
         *liblibos_release.a:int_hard.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)  
         *liblibos_release.a:pic_timer.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)  
                
         ./src/OS_prjcfg/config/int_config.o(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)  
         ./src/user/critical/*(.bss .bss.* .sbss .sbss.* SCOMMON COMMON)

    }>SYSDATA

    /*the other sys code  except the sys loader*/
    rw_sysload_rDDR3_lnorflash_text :
    {
            . = ALIGN(0x20);
         Cached_textsec_start = .;
        * (.Cached_textsec)
         Cached_textsec_end = .;
        * (.text .text.* .rodata .rodata.*)
    }>SYSCODE AT>FLASHROM

    rw_sysload_rDDR3_lnorflash_data :
    {
         . = ALIGN(0x20);
         Cached_datasec_start = .;
         * (.Cached_datasec)
         Cached_datasec_end = .;
         * (.data .data.* .sdata  .sdata.*)
    }>SYSDATA AT>FLASHROM


    zi_sysload_rDDR3_lDDR3 (NOLOAD) :
    {
        . = ALIGN(0x20);
        Cached_bsssec_start = .;
        * (.Cached_bsssec)
        Cached_bsssec_end = .;
        * (.bss .bss.* .sbss .sbss.* SCOMMON COMMON)
    }>SYSDATA
    
    stack_rDDR3_lDDR3 (NOLOAD) :
    {
        . = ALIGN(0x04);
        irq_bottom = .;
        KEEP(* (.stack.irq .stack.irq.*))
        irq_top = ALIGN(0x10);

        usr_bottom = .;
        KEEP(* (.stack.usr .stack.usr.*))
        usr_top = ALIGN(0x10);

        cint_exp_bottom = .;
        KEEP(* (.stack.cint_exp .stack.cint_exp.*))
        cint_exp_top = ALIGN(0x10);
        
        sys_exp_bottom = .;
        KEEP(* (.stack.prg_exp .stack.prg_exp.*))
        sys_exp_top = ALIGN(0x10);
        
        sys_heap_bottom = .;
    }>SYSDATA
    
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
    }>FLASHROM

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
    }>FLASHROM
    
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
    }>FLASHROM
    sys_heap_top = sys_heap_bottom + 128M;
    
    tab_CpyForLoader :
    {
       . = ALIGN(0x04);
       tgLoaderRecord = .;
       LONG(LOADADDR(text_loader))
       LONG(ADDR(text_loader))
       LONG(SIZEOF(text_loader))
    }>ROMSTART
}
/**** The End ****/


