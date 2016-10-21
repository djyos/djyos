ENTRY(Load_Preload)

INCLUDE  ..\lds\memory.lds

/*不同的make程序，文件路径表示方式，是不同的，输入段中文件名写法要相应修改*/
/*正确的写法，可参看console中输出的命令行*/
SECTIONS
{
    Iboot_RevRam :
    {
        g_cIbootFlag = .;      /*字符串变量，c中可用，用于判断进入APP还是Iboot*/
    }>BootFlagRAM1
    
    Iboot_AppCtrl :
    {
        gc_ptIbootCtrl = .;   /*结构变量，c中可用，用于检查APP正确性*/
    }>AppCtlRom
    
    Iboot_AppInfo :
    {
        gc_ptAppInfo = .;   
        LONG( . + 256);
        gc_AppSize = .;          /*ROM区 存储APP的bin文件大小 */
        LONG(pProgRomEnd - gc_AppSize + 4); /* APP的bin文件大小 */
    }>AppVerfyRom
    
    Iboot_AppStart :
    {
        gc_AppAddr = .;         /*函数指针变量，指向APP的起始地址*/
    }>AppRom
    
    stack_rRAM1_lRAM1 (NOLOAD) :
    {
        . = ALIGN(0x08);
        msp_bottom = .;
        KEEP(* (.data.InitStack))
        msp_top = ALIGN(0x08);
    }>RAM1 

    text_startup_rAppRom_lAppRom :
    {
       . = ALIGN(0x08);
        KEEP(*liblibOS_mcb1700_o2.a:pre_loader.o (.text .text.Load_Preload))
        KEEP(*liblibOS_mcb1700_o2.a:pre_loader.o (.text .text.* .rodata .rodata.*))
    }>AppRom

    text_preload_rAppRom_lAppRom :
    {
       . = ALIGN(0x08);
        g_pAppCodeStartRamAddr = .;
        *liblibOS_mcb1700_o2.a:loader.o (.text .text.* .rodata .rodata.*)
        *liblibOS_mcb1700_o2.a:int_hard.o (.text .text.* .rodata .rodata.*) 
        KEEP(*liblibOS_mcb1700_o2.a:hard-exps.o (.text .text.* .rodata .rodata.*)) 
        *liblibOS_mcb1700_o2.a:hard-exp.o (.text .text.* .rodata .rodata.*)
        *liblibOS_mcb1700_o2.a:int.o (.text .text.* .rodata .rodata.*)
        *liblibOS_mcb1700_o2.a:ints.o (.text .text.* .rodata .rodata.*)
        *src/critical/*.o (.text .text.* .rodata .rodata.*)
        *liblibOS_mcb1700_o2.a:heap-static.o (.text .text.* .rodata .rodata.*)
        *liblibOS_mcb1700_o2.a:list.o (.text .text.* .rodata .rodata.*)
    }>AppRom
    
    text_sysload_rAppRom_lAppRom :
    {
       . = ALIGN(0x08);
        * (.text .text.* .rodata .rodata.*)  
        g_pAppCodeEndRamAddr = .;
    }>AppRom

    rw_preload_rRAM1_lAppRom :
    {
       . = ALIGN(0x08);
        *liblibOS_mcb1700_o2.a:loader.o (.data .data.*)
        *liblibOS_mcb1700_o2.a:int_hard.o (.data .data.*)
        KEEP(*liblibOS_mcb1700_o2.a:hard-exps.o (.data .data.*))
        *liblibOS_mcb1700_o2.a:hard-exp.o (.data .data.*)
        *liblibOS_mcb1700_o2.a:int.o (.data .data.*)
        *liblibOS_mcb1700_o2.a:ints.o (.data .data.*)
        *src/critical/*.o (.data .data.*)
        *liblibOS_mcb1700_o2.a:heap-static.o (.data .data.*)
        *liblibOS_mcb1700_o2.a:list.o (.data .data.*)
    }>RAM1 AT>AppRom

    zi_preload_rRAM1_lRAM1 (NOLOAD) :
    {
       . = ALIGN(0x100);
        KEEP(* (.table.exceptions .table.exceptions.*))
        KEEP(* (.table.vectors .table.vectors.*))
        *liblibOS_mcb1700_o2.a:loader.o (.bss .bss.* COMMON)
        *liblibOS_mcb1700_o2.a:int_hard.o (.bss .bss.* COMMON)
        KEEP(*liblibOS_mcb1700_o2.a:hard-exps.o (.bss .bss.* COMMON))
        *liblibOS_mcb1700_o2.a:hard-exp.o (.bss .bss.* COMMON)
        *liblibOS_mcb1700_o2.a:int.o (.bss .bss.* COMMON)
        *liblibOS_mcb1700_o2.a:ints.o (.bss .bss.* COMMON)
        *src/critical/*.o (.bss .bss.* COMMON)
        *liblibOS_mcb1700_o2.a:heap-static.o (.bss .bss.* COMMON)
        *liblibOS_mcb1700_o2.a:list.o (.bss .bss.* COMMON)
    }>RAM1

     rw_sysload_rRAM1_lAppRom :
    {
       . = ALIGN(0x08);
        * (.data .data.*)
    }>RAM1 AT>AppRom

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

        LONG(LOADADDR(text_preload_rAppRom_lAppRom))
        LONG(ADDR(text_preload_rAppRom_lAppRom))
        LONG(SIZEOF(text_preload_rAppRom_lAppRom))
        LONG(1)

        LONG(LOADADDR(rw_preload_rRAM1_lAppRom))
        LONG(ADDR(rw_preload_rRAM1_lAppRom))
        LONG(SIZEOF(rw_preload_rRAM1_lAppRom))
        LONG(1)

        LONG(LOADADDR(zi_preload_rRAM1_lRAM1))
        LONG(ADDR(zi_preload_rRAM1_lRAM1))
        LONG(SIZEOF(zi_preload_rRAM1_lRAM1))
        LONG(0)
    }>AppRom

    sysload_cp_table :
    {
       . = ALIGN(0x08);
        sysload_copy_table = .;
        LONG(16)
        LONG(3)

        LONG(LOADADDR(text_sysload_rAppRom_lAppRom))
        LONG(ADDR(text_sysload_rAppRom_lAppRom))
        LONG(SIZEOF(text_sysload_rAppRom_lAppRom))
        LONG(1)

        LONG(LOADADDR(rw_sysload_rRAM1_lAppRom))
        LONG(ADDR(rw_sysload_rRAM1_lAppRom))
        LONG(SIZEOF(rw_sysload_rRAM1_lAppRom))
        LONG(1)

        LONG(LOADADDR(zi_sysload_rRAM1_lRAM1))
        LONG(ADDR(zi_sysload_rRAM1_lRAM1))
        LONG(SIZEOF(zi_sysload_rRAM1_lRAM1))
        LONG(0)
    }>AppRom

    heap_rRAM1_lRAM1 (NOLOAD) :
    {
        . = ALIGN(0x08);
        sys_heap_bottom = .;
    }>RAM1
    sys_heap_top = 0x10000000+32k-16;

    heap_rRAM2_lRAM2 (NOLOAD) :
    {
        . = ALIGN(0x08);
        sys_heap_bottom_ext = .;
    }>RAM2
    sys_heap_top_ext = 0x2007C000+32k;

    heap_cp_table :
    {
       . = ALIGN(0x08);
        pHeapList = .;

/* 默认堆，必须排首位，名字必须是“sys” */
        LONG(3)                     /*表示该堆由两段内存组成*/
        LONG(0)                     /*该堆上分配的内存的对齐尺寸，0表示使用系统对齐*/
        LONG(0)                     /*0=通用堆,1=专用堆,如果系统只有唯一一个堆,则只能是通用堆*/
        LONG(sys_heap_bottom)       /*第一段基址，须符合对齐要求*/
        LONG(sys_heap_top)          /*第一段地址上限（不含），须符合对齐要求*/
        LONG(32)                    /*第一段页尺寸=32bytes*/
        LONG(msp_bottom)            /*第二段基址，回收初始化栈，须符合对齐要求*/
        LONG(msp_top - 1024)         /*第二段地址上限（不含），384是留给中断使用的，须符合对齐要求*/
        LONG(24)                    /*第二段页尺寸=24bytes*/
        LONG(sys_heap_bottom_ext)   /*第三段基址，须符合对齐要求*/
        LONG(sys_heap_top_ext)      /*第三段地址上限（不含），须符合对齐要求*/
        LONG(24)                    /*第三段页尺寸=32bytes*/
        BYTE(0x73)                  /*'s'*/
        BYTE(0x79)                  /*'y'*/
        BYTE(0x73)                  /*'s'*/
        BYTE(0)                     /*串结束，"sys"是堆的名字*/
        . = ALIGN(0x04);
        LONG(0)     /*结束标志*/
    }>AppRom
    
    rom_prog_end :
    {
       . = ALIGN(0x04);
        pProgRomEnd = .;
    }>AppRom 
}
/**** The End ****/
