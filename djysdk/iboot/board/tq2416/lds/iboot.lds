ENTRY(isr_vector)

INCLUDE  ..\board\tq2416\lds\memory.lds

/*不同的make程序，文件路径表示方式，是不同的，输入段中文件名写法要相应修改*/
/*正确的写法，可参看console中输出的命令行*/
SECTIONS
{
    Iboot_RevRam :
    {
        g_cIbootFlag = .;           /*字符串变量，c中可用，用于判断进入APP还是Iboot*/
    }>BootFlagRAM1
    
    Iboot_AppCtrl :
    {
        gc_ptIbootCtrl = .;   /*结构变量，c中可用，用于检查APP正确性*/
    }>AppCtlRom

    Iboot_AppInfo :
    {
        gc_ptAppInfo = .;         /*ROM区 存储APP的起始地址及大小 */ 
    }>AppVerfyRom
   
    Iboot_AppStart :
    {
        gc_AppAddr = .;           /*函数指针变量，指向APP的起始地址*/
        __AppPreLoad = .;           /*函数指针变量，指向APP的Preload函数*/
    }>AppRom
                
	text_startup_rROM1_lROM1 :
	{
	    . = ALIGN(0x08);   
	    KEEP(*(.isr_vector .isr_vector.*))
		KEEP(*liblibOS_tq2416_o0.a:initcpu.o (.text .text.* .rodata .rodata.*))
		KEEP(*liblibOS_tq2416_o0.a:initcpuc.o (.text .text.* .rodata .rodata.*))
		KEEP(*liblibOS_tq2416_o0.a:mmu.o (.text .text.* .rodata .rodata.*))
		KEEP(*liblibOS_tq2416_o0.a:set-cache.o (.text .text.* .rodata .rodata.*))
		KEEP(*liblibOS_tq2416_o0.a:pre_loader.o (.text .text.* .rodata .rodata.*))	
	} >IbootRom 

	
	text_preload_rROM1_lROM1 :
	{
	    . = ALIGN(0x08);
	    
		KEEP(*board/tq2416/src/critical/critical.o (.text .text.* .rodata .rodata.*))
        KEEP(*liblibOS_tq2416_o0.a:hard-exps.o (.text .text.* .rodata .rodata.*))
        KEEP(*liblibOS_tq2416_o0.a:hard-exp.o (.text .text.* .rodata .rodata.*))
		*liblibOS_tq2416_o0.a:loader.o (.text .text.* .rodata .rodata.*)
		*liblibOS_tq2416_o0.a:int_hard.o (.text .text.* .rodata .rodata.*)
		*liblibOS_tq2416_o0.a:int.o (.text .text.* .rodata .rodata.*)
		*liblibOS_tq2416_o0.a:ints.o (.text .text.* .rodata .rodata.*)
		*liblibOS_tq2416_o0.a:cpu_peri_gpio.o (.text .text.* .rodata .rodata.*)
		*liblibOS_tq2416_o0.a:cpu_peri.o (.text .text.* .rodata .rodata.*)
		*liblibOS_tq2416_o0.a:heap-static.o (.text .text.* .rodata .rodata.*)
		*liblibOS_tq2416_o0.a:list.o (.text .text.* .rodata .rodata.*)
	}>RAM1 AT>IbootRom
	
	rw_preload_rRAM1_lROM1 :
	{
	    . = ALIGN(0x08);
		KEEP(* (.table.exceptions .table.exceptions.*))
		KEEP(* (.table.vectors .table.vectors.*))
		KEEP(*board/tq2416/src/critical/critical.o (.data .data.*))
		KEEP(*liblibOS_tq2416_o0.a:hard-exps.o (.data .data.*))
        KEEP(*liblibOS_tq2416_o0.a:hard-exp.o (.data .data.*))
		*liblibOS_tq2416_o0.a:loader.o (.data .data.*)
		*liblibOS_tq2416_o0.a:int_hard.o (.data .data.*)
		*liblibOS_tq2416_o0.a:int.o (.data .data.*)
		*liblibOS_tq2416_o0.a:ints.o (.data .data.*)
		*liblibOS_tq2416_o0.a:hard-exp.o (.data .data.*)
		*liblibOS_tq2416_o0.a:cpu_peri_gpio.o (.data .data.*)
		*liblibOS_tq2416_o0.a:cpu_peri.o (.data .data.*)
		*liblibOS_tq2416_o0.a:heap-static.o (.data .data.*)
		*liblibOS_tq2416_o0.a:list.o (.data .data.*)
	}>RAM1 AT>IbootRom
	
	
	zi_preload_rRAM1_lRAM1 (NOLOAD) :
	{
	    . = ALIGN(0x08);
		KEEP(*board/tq2416/src/critical/critical.o (.bss .bss.* COMMON))
		KEEP(*liblibOS_tq2416_o0.a:hard-exps.o (.bss .bss.* COMMON))
        KEEP(*liblibOS_tq2416_o0.a:hard-exp.o (.bss .bss.* COMMON))
		*liblibOS_tq2416_o0.a:loader.o (.bss .bss.* COMMON)
		*liblibOS_tq2416_o0.a:int_hard.o (.bss .bss.* COMMON)
		*liblibOS_tq2416_o0.a:int.o (.bss .bss.* COMMON)
		*liblibOS_tq2416_o0.a:ints.o (.bss .bss.* COMMON)
		*liblibOS_tq2416_o0.a:hard-exp.o (.bss .bss.* COMMON)
		*liblibOS_tq2416_o0.a:cpu_peri_gpio.o (.bss .bss.* COMMON)
		*liblibOS_tq2416_o0.a:cpu_peri.o (.bss .bss.* COMMON)
		*liblibOS_tq2416_o0.a:heap-static.o (.bss .bss.* COMMON)
		*liblibOS_tq2416_o0.a:list.o (.bss .bss.* COMMON)
	}>RAM1 
	
	
	text_sysload_rROM1_lROM1 :
	{
	    . = ALIGN(0x08);
		* (.text .text.* .rodata .rodata.*)
	}>RAM1 AT>IbootRom 
	
    stack_rRAM1_lRAM1 (NOLOAD) :
    {
        . = ALIGN(0x08);
        msp_bottom = .;
        KEEP(* (.data.InitStack))
        msp_top = ALIGN(0x08);
    }>RAM1
    
	rw_sysload_rRAM1_lROM1 :
	{
	    . = ALIGN(0x08);
		* (.data .data.*)
	}>RAM1 AT>IbootRom
	
	
	zi_sysload_rRAM1_lRAM1 (NOLOAD) :
	{
	    . = ALIGN(0x08);
		* (.bss .bss.* COMMON)
	}>RAM1 
	
	stack_rRAM1_lRAM1 (NOLOAD) :
	{
	    . = ALIGN(0x08);
	    
		usr_bottom = .;
		/*KEEP(* (.stack.usr .stack.usr.*))*/
		. += 4096;
		usr_top = .;
		
		svc_bottom = .;
		/*KEEP(* (.stack.svc .stack.svc.*))*/
		. += 4096;
		svc_top = .;
		
		irq_bottom = .;
		/*KEEP(* (.stack.irq .stack.irq.*))*/
		. += 4096;
		irq_top = .;
		
		fiq_bottom = .;
		/*KEEP(* (.stack.fiq .stack.fiq.*))*/
		. += 4096;
		fiq_top = .;
		
		undef_bottom = .;
		/*KEEP(* (.stack.undef .stack.undef.*))*/
		. += 4096;
		undef_top = .;
		
		abort_bottom = .;
		/*KEEP(* (.stack.abort .stack.abort.*))*/
		. += 4096; 
		abort_top = .;
		
	}>RAM1 
	
 	
	preload_cp_table :
	{
	    . = ALIGN(0x08);
		preload_copy_table = .;
		
		LONG(16)
		LONG(3)
		
		LONG(LOADADDR(text_preload_rROM1_lROM1))
		LONG(ADDR(text_preload_rROM1_lROM1))
		LONG(SIZEOF(text_preload_rROM1_lROM1))
		LONG(1)
		
		LONG(LOADADDR(rw_preload_rRAM1_lROM1))
		LONG(ADDR(rw_preload_rRAM1_lROM1))
		LONG(SIZEOF(rw_preload_rRAM1_lROM1))
		LONG(1)
		
		LONG(LOADADDR(zi_preload_rRAM1_lRAM1))
		LONG(ADDR(zi_preload_rRAM1_lRAM1))
		LONG(SIZEOF(zi_preload_rRAM1_lRAM1))
		LONG(0)
	}>IbootRom 
	
	sysload_cp_table :
	{
	    . = ALIGN(0x08);
		sysload_copy_table = .;
		
		LONG(16)
		LONG(3)
		
		LONG(LOADADDR(text_sysload_rROM1_lROM1))
		LONG(ADDR(text_sysload_rROM1_lROM1))
		LONG(SIZEOF(text_sysload_rROM1_lROM1))
		LONG(1)
		
		LONG(LOADADDR(rw_sysload_rRAM1_lROM1))
		LONG(ADDR(rw_sysload_rRAM1_lROM1))
		LONG(SIZEOF(rw_sysload_rRAM1_lROM1))
		LONG(1)
				
		LONG(LOADADDR(zi_sysload_rRAM1_lRAM1))
		LONG(ADDR(zi_sysload_rRAM1_lRAM1))
		LONG(SIZEOF(zi_sysload_rRAM1_lRAM1))
		LONG(0)
	}>IbootRom 
			
	heap_rRAM1_lRAM1 (NOLOAD) :
	{
	    . = ALIGN(0x08);
	    sys_heap_bottom = .;
	    . += 8M;
		sys_heap_top = .;
	}>RAM1
	
	heap_rextram_lextram (NOLOAD) :
	{
	    . = ALIGN(0x08);
	    extram_heap_bottom = .;
	    . += 16M-16;
	    extram_heap_top = .;
		
	}>RAM2 
	
	heap_cp_table :
	{
	    . = ALIGN(0x08);
		pHeapList = .;
		
        /* 默认堆，必须排首位，名字必须是“sys” */
        LONG(1)                     /*表示该堆由1段内存组成*/
        LONG(0)                     /*该堆上分配的内存的对齐尺寸，0表示使用系统对齐*/
        LONG(0)                     /*0=通用堆,1=专用堆,如果系统只有唯一一个堆,则只能是通用堆*/
        LONG(sys_heap_bottom)       /*第一段基址，须符合对齐要求*/
        LONG(sys_heap_top)          /*第一段地址上限（不含），须符合对齐要求*/
        LONG(32)                    /*第一段页尺寸=32bytes*/
        BYTE(0x73)                  /*'s'*/
        BYTE(0x79)                  /*'y'*/
        BYTE(0x73)                  /*'s'*/
        BYTE(0)                     /*串结束，"sys"是堆的名字*/

        . = ALIGN(0x04);
        LONG(1)                     /*表示该堆由1段内存组成*/
        LONG(32)                    /*该堆上分配的内存的对齐尺寸，0表示使用系统对齐*/
        LONG(0)                     /*0=通用堆,1=专用堆,如果系统只有唯一一个堆,则只能是通用堆*/
        LONG(extram_heap_bottom)    /*第一段基址，须符合对齐要求*/
        LONG(extram_heap_top)       /*第一段地址上限（不含），须符合对齐要求*/
        LONG(128)                   /*第一段页尺寸=128bytes*/
        BYTE(0x65)                  /*'e'*/
        BYTE(0x78)                  /*'x'*/
        BYTE(0x74)                  /*'t'*/
        BYTE(0x72)                  /*'r'*/
        BYTE(0x61)                  /*'a'*/
        BYTE(0x6d)                  /*'m'*/
        BYTE(0)                     /*串结束，"extram"是堆的名字*/
        . = ALIGN(0x04);
        LONG(0)                     /*结束标志*/
	}>IbootRom 
	
    zi_startup_rRAM_nocache_lRAM_nocache (NOLOAD) :
	{
	    . = ALIGN(0x08);
		mmu_page_table = .;		
		. += 0x8000;
		
		engine_irq = .;
		. += 0x04;
		engine_fiq = .;
		. += 0x04;
		engine_undef = .;
		. += 0x04;
		engine_swi = .;
		. += 0x04;
		engine_pabort = .;
		. += 0x04;
		engine_dabort = .;
		. += 0x04;
		engine_reserved = .;
		. += 0x04;

	} >RAM_nocache
	
}
/**** The End ****/
