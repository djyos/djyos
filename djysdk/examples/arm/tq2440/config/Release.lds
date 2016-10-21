ENTRY(isr_vector)

MEMORY
{
	ROM1(R)		    : ORIGIN  = 0x00000000, LENGTH  = 8M
	RAM_nocache(RW)	: ORIGIN  = 0x30800000, LENGTH  = 8M

	RAM1(RXW)		: ORIGIN  = 0x31000000, LENGTH  = 16M
	RAM2(RXW)       : ORIGIN  = 0x32000000, LENGTH  = 16M

}
  

/*不同的make程序，文件路径表示方式，是不同的，输入段中文件名写法要相应修改*/
/*正确的写法，可参看console中输出的命令行*/
SECTIONS
{
	  
	text_startup_rROM1_lROM1 :
	{
	    . = ALIGN(0x08);   
	    KEEP(*(.isr_vector .isr_vector.*))
		KEEP(*liblibOS-release.a:initcpu.o (.text .text.* .rodata .rodata.*))
		KEEP(*liblibOS-release.a:initcpuc.o (.text .text.* .rodata .rodata.*))
		KEEP(*liblibOS-release.a:hard-exps.o (.text .text.* .rodata .rodata.*))
		KEEP(*liblibOS-release.a:hard-exp.o (.text .text.* .rodata .rodata.*))
		KEEP(*liblibOS-release.a:mmu.o (.text .text.* .rodata .rodata.*))
		KEEP(*liblibOS-release.a:cache.o (.text .text.* .rodata .rodata.*))
		KEEP(*liblibOS-release.a:pre_loader.o (.text .text.* .rodata .rodata.*))
		
		
	} >ROM1 

	
	text_preload_rROM1_lROM1 :
	{
	    . = ALIGN(0x08);
	    
		KEEP(src/user/critical/critical.o (.text .text.* .rodata .rodata.*))
		*liblibOS-release.a:loader.o (.text .text.* .rodata .rodata.*)
		*liblibOS-release.a:int_hard.o (.text .text.* .rodata .rodata.*)
		*liblibOS-release.a:int.o (.text .text.* .rodata .rodata.*)
		*liblibOS-release.a:ints.o (.text .text.* .rodata .rodata.*)
		*liblibOS-release.a:cpu_peri_gpio.o (.text .text.* .rodata .rodata.*)
		*liblibOS-release.a:cpu_peri.o (.text .text.* .rodata .rodata.*)
		*liblibOS-release.a:heap-static.o (.text .text.* .rodata .rodata.*)
	}>RAM1 AT>ROM1
	
	rw_preload_rRAM1_lROM1 :
	{
	    . = ALIGN(0x08);
		KEEP(* (.table.exceptions .table.exceptions.*))
		KEEP(* (.table.vectors .table.vectors.*))
		KEEP(src/user/critical/critical.o (.data .data.*))
		*liblibOS-release.a:loader.o (.data .data.*)
		*liblibOS-release.a:int_hard.o (.data .data.*)
		*liblibOS-release.a:int.o (.data .data.*)
		*liblibOS-release.a:ints.o (.data .data.*)
		*liblibOS-release.a:hard-exp.o (.data .data.*)
		*liblibOS-release.a:cpu_peri_gpio.o (.data .data.*)
		*liblibOS-release.a:cpu_peri.o (.data .data.*)
		*liblibOS-release.a:heap-static.o (.data .data.*)
	}>RAM1 AT>ROM1
	
	
	zi_preload_rRAM1_lRAM1 (NOLOAD) :
	{
	    . = ALIGN(0x08);
		KEEP(src/user/critical/critical.o (.bss .bss.* COMMON))
		*liblibOS-release.a:loader.o (.bss .bss.* COMMON)
		*liblibOS-release.a:int_hard.o (.bss .bss.* COMMON)
		*liblibOS-release.a:int.o (.bss .bss.* COMMON)
		*liblibOS-release.a:ints.o (.bss .bss.* COMMON)
		*liblibOS-release.a:hard-exp.o (.bss .bss.* COMMON)
		*liblibOS-release.a:cpu_peri_gpio.o (.bss .bss.* COMMON)
		*liblibOS-release.a:cpu_peri.o (.bss .bss.* COMMON)
		*liblibOS-release.a:heap-static.o (.bss .bss.* COMMON)
	}>RAM1 
	
	
	text_sysload_rROM1_lROM1 :
	{
	    . = ALIGN(0x08);
		* (.text .text.* .rodata .rodata.*)
	}>RAM1 AT>ROM1 
	
	
	rw_sysload_rRAM1_lROM1 :
	{
	    . = ALIGN(0x08);
		* (.data .data.*)
	}>RAM1 AT>ROM1
	
	
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
	}>ROM1 
	
	
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
	}>ROM1 
		
	
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
	    . += 16M;
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
	}>ROM1 
	
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
