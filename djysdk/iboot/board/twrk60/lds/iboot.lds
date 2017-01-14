ENTRY(Init_Cpu)

INCLUDE  ..\board\TWRK60\lds\memory.lds

/*不同的make程序，文件路径表示方式，是不同的，输入段中文件名写法要相应修改*/
/*正确的写法，可参看console中输出的命令行*/
SECTIONS
{	
	Rom_Start :
	{
	    restore = .; 
	}>InnerFlash
    
    /*RevRam是Ram保留区域，用于存储preload前使用或保存的内在数据,只有IAP_pload.c*/
    /*的pg_IapVar变量从该段内分配，且该段在iboot.lds和debug/release.lds保持一致*/
    Iboot_RevRam (NOLOAD):
    {
        . = ALIGN(0x08);
        KEEP(*(.IapBootFlgRam .IapBootFlgRam.*))  
    }>RAM1	

    stack_rRAM1_lRAM1 (NOLOAD) :
    {
        . = ALIGN(0x08);
        msp_bottom = .;
        KEEP(* (.data.InitStack))
        msp_top = ALIGN(0x08);
    }>RAM1
		
    text_startup_rIbootRom_lIbootRom :
    {
       . = ALIGN(0x08);
        KEEP(*(.StartupExpTbl .StartupExpTbl.*))
        KEEP(*(.KtSecCode .KtSecCode.*))
        KEEP(*liblibOS_twrk60_o0.a:initcpuc.o (.text .text.* .rodata .rodata.*))
        KEEP(*liblibOS_twrk60_o0.a:sysinit.o (.text .text.* .rodata .rodata.*))
        KEEP(*liblibOS_twrk60_o0.a:pre_loader.o (.text .text.* .rodata .rodata.*))
        KEEP(*liblibOS_twrk60_o0.a:IAP_pLoad.o (.text .text.* .rodata .rodata.*))
        KEEP(*board/twrk60/src/OS_prjcfg/Iboot_config.o (.text .text.* .rodata .rodata.*))
    }>InnerFlash

    text_preload_rIbootRom_lIbootRom :
    {
       . = ALIGN(0x08);
        g_pAppCodeStartRamAddr = .;
        *liblibOS_twrk60_o0.a:loader.o (.text .text.* .rodata .rodata.*)
        *liblibOS_twrk60_o0.a:int_hard.o (.text .text.* .rodata .rodata.*)
         KEEP(*liblibOS_twrk60_o0.a:hard-exps.o (.text .text.* .rodata .rodata.*)) 
        *liblibOS_twrk60_o0.a:hard-exp.o (.text .text.* .rodata .rodata.*)
        *liblibOS_twrk60_o0.a:int.o (.text .text.* .rodata .rodata.*)
        *liblibOS_twrk60_o0.a:ints.o (.text .text.* .rodata .rodata.*)
        *board/twrk60/src/critical/*.o (.text .text.* .rodata .rodata.*)
        *liblibOS_twrk60_o0.a:heap-static.o (.text .text.* .rodata .rodata.*)
        *liblibOS_twrk60_o0.a:list.o (.text .text.* .rodata .rodata.*)
    }>InnerFlash
    
    text_sysload_rIbootRom_lIbootRom :
    {
       . = ALIGN(0x08);
        * (.text .text.* .rodata .rodata.*)
        g_pAppCodeEndRamAddr = .;
    }>InnerFlash

    rw_preload_rRAM1_lIbootRom :
    {
       . = ALIGN(0x08);
        *liblibOS_twrk60_o0.a:loader.o (.data .data.*)
        *liblibOS_twrk60_o0.a:int_hard.o (.data .data.*)
        KEEP(*liblibOS_twrk60_o0.a:hard-exps.o (.data .data.*))
        *liblibOS_twrk60_o0.a:hard-exp.o (.data .data.*)
        *liblibOS_twrk60_o0.a:int.o (.data .data.*)
        *liblibOS_twrk60_o0.a:ints.o (.data .data.*)
        *board/twrk60/src/critical/*.o (.data .data.*)
        *liblibOS_twrk60_o0.a:heap-static.o (.data .data.*)
        *liblibOS_twrk60_o0.a:list.o (.data .data.*)
    }>RAM1 AT>InnerFlash

    /*Cortex-M 系列的向量表可重定位，对起始地址的要求是：若有N个向量（包含异常），*/
    /*先增大到2的整数次幂，若为M，则Addr = ALIGN(4 * M) */
    /*举例：若cpu有64个中断，则N = 80, M = 128, Addr = ALIGN(0x200)*/
    zi_preload_rRAM1_lRAM1 (NOLOAD) :
    {
       . = ALIGN(0x200);
        KEEP(* (.table.exceptions .table.exceptions.*))
        KEEP(* (.table.vectors .table.vectors.*))
        *liblibOS_twrk60_o0.a:loader.o (.bss .bss.* COMMON)
        *liblibOS_twrk60_o0.a:int_hard.o (.bss .bss.* COMMON)
        KEEP(*liblibOS_twrk60_o0.a:hard-exps.o (.bss .bss.* COMMON))
        *liblibOS_twrk60_o0.a:hard-exp.o (.bss .bss.* COMMON)
        *liblibOS_twrk60_o0.a:int.o (.bss .bss.* COMMON)
        *liblibOS_twrk60_o0.a:ints.o (.bss .bss.* COMMON)
        *board/twrk60/src/critical/*.o (.bss .bss.* COMMON)
        *liblibOS_twrk60_o0.a:heap-static.o (.bss .bss.* COMMON)
        *liblibOS_twrk60_o0.a:list.o (.bss .bss.* COMMON)
    }>RAM1

    rw_sysload_rRAM1_lIbootRom :
    {
       . = ALIGN(0x08);
        * (.data .data.*)
    }>RAM1 AT>InnerFlash

    zi_sysload_rRAM1_lRAM1 (NOLOAD) :
    {
       . = ALIGN(0x08);
        * (.bss .bss.* COMMON)
    }>RAM1

    preload_cp_table :
    {
       . = ALIGN(0x08);
        preload_copy_table = .;
        LONG(16)
        LONG(3)

        LONG(LOADADDR(text_preload_rIbootRom_lIbootRom))
        LONG(ADDR(text_preload_rIbootRom_lIbootRom))
        LONG(SIZEOF(text_preload_rIbootRom_lIbootRom))
        LONG(1)

        LONG(LOADADDR(rw_preload_rRAM1_lIbootRom))
        LONG(ADDR(rw_preload_rRAM1_lIbootRom))
        LONG(SIZEOF(rw_preload_rRAM1_lIbootRom))
        LONG(1)

        LONG(LOADADDR(zi_preload_rRAM1_lRAM1))
        LONG(ADDR(zi_preload_rRAM1_lRAM1))
        LONG(SIZEOF(zi_preload_rRAM1_lRAM1))
        LONG(0)
    }>InnerFlash

    sysload_cp_table :
    {
       . = ALIGN(0x08);
        sysload_copy_table = .;
        LONG(16)
        LONG(3)

        LONG(LOADADDR(text_sysload_rIbootRom_lIbootRom))
        LONG(ADDR(text_sysload_rIbootRom_lIbootRom))
        LONG(SIZEOF(text_sysload_rIbootRom_lIbootRom))
        LONG(1)

        LONG(LOADADDR(rw_sysload_rRAM1_lIbootRom))
        LONG(ADDR(rw_sysload_rRAM1_lIbootRom))
        LONG(SIZEOF(rw_sysload_rRAM1_lIbootRom))
        LONG(1)

        LONG(LOADADDR(zi_sysload_rRAM1_lRAM1))
        LONG(ADDR(zi_sysload_rRAM1_lRAM1))
        LONG(SIZEOF(zi_sysload_rRAM1_lRAM1))
        LONG(0)
    }>InnerFlash

    heap_rRAM1_lRAM1 (NOLOAD) :
    {
       . = ALIGN(0x08);
        sys_heap_bottom = .;
    }>RAM1
    sys_heap_top = InnerRamStart + InnerRamSize;

    heap_cp_table :
    {
       . = ALIGN(0x08);
        pHeapList = .;

/* 默认堆，必须排首位，名字必须是“sys” */
        LONG(2)                     /*表示该堆由两段内存组成*/
        LONG(0)                     /*该堆上分配的内存的对齐尺寸，0表示使用系统对齐*/
        LONG(0)                     /*0=通用堆,1=专用堆,如果系统只有唯一一个堆,则只能是通用堆*/
        LONG(sys_heap_bottom)       /*第一段基址，须符合对齐要求*/
        LONG(sys_heap_top)          /*第一段地址上限（不含），须符合对齐要求*/
        LONG(32)                    /*第一段页尺寸=32bytes*/
        LONG(msp_bottom)            /*第二段基址，回收初始化栈，须符合对齐要求*/
        LONG(msp_top - 1024)        /*第二段地址上限（不含），1024是留给中断使用的，须符合对齐要求*/
        LONG(24)                    /*第二段页尺寸=24bytes*/
        BYTE(0x73)                  /*'s'*/
        BYTE(0x79)                  /*'y'*/
        BYTE(0x73)                  /*'s'*/
        BYTE(0)                     /*串结束，"sys"是堆的名字*/
        . = ALIGN(0x04);
        LONG(0)                     /*结束标志*/
    }>InnerFlash
    
    Iboot_Info :
    {
        gc_ptFlashOffset = .;
        LONG(InnerFlashOffset)
              
        gc_ptFlashRange = .;
        LONG(InnerFlashRange)
        
        gc_ptIbootSize = .;
        LONG(IbootSize)
    }>InnerFlash
    
    App_Info :
    {
        . = ALIGN(0x08);
        gc_pAppOffset = .;          /* APP空间偏置 */
        LONG(InnerFlashOffset + IbootSize)
        gc_pAppRange = .;           /* APP空间范围 */
        LONG(InnerFlashRange - IbootSize)       
    }>InnerFlash
    
    Iboot_AppInfo :
    {
        . = restore + IbootSize;
        gc_ptIbootCtrl = .;         /*结构变量，c中可用，用于检查APP正确性*/
        . += 256;
        gc_ptAppInfo = .;           /*ROM区 存储APP的起始地址及大小 */ 
        . += 256;
        gc_AppAddr = .;             /*函数指针变量，指向APP的起始地址*/
        __AppStart = .;           /*函数指针变量，指向APP的Preload函数*/
    }>InnerFlash
    
}
/**** The End ****/
