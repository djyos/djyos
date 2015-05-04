#include <cdef21469.h>
#include <def21469.h>
#include <sru.h>
#include <signal.h>
#include <sysreg.h>
#include <string.h>
#include <math.h>
#include "os_inc.h"
#include "cpu_peri.h"
#include "key_hard.h"
#include "uart.h"
#include "printf.h"
#include "easynorflash\easynorflash.h"
//#include "rtc_ds1390.h"
//#include "at45db321.h"
#include "endian.h"

//for test use only
#include "m29w320.h"
#include "m25p16.h"`
#include "adm1032.h"
#include "led_key.h"

#ifdef Monitor_version
		int delay_cnt1=1000000;
		int delay_cnt2=500000;
#endif
#ifdef Release_version
		int delay_cnt1=10000000;
		int delay_cnt2=5000000;
#endif

//section("seg_test") u32 num32_test=0x12345678;

struct semaphore_LCB *thread_change_test1;
struct semaphore_LCB *thread_change_test2;
struct semaphore_LCB *thread_change_test3;

struct djy_tm gtime;

u32 LED_Status3=0x00,LED_Status2=0x00;
u32 LED_Note3=0x00,LED_Note2=0x00;
bool_t loop_samp=1;
u32 ALU_ERR_CNT=0;

u32 endian_test=0x12345678;
u64 endian_test64=0xfedcba9876543210;
u64 endian_result64=0xfedcba9876543210;
u8  test_data_buf[16]={0};

extern int g_LEDs[8];

ptu32_t __djy_third_thread(void)
{
	return djy_third_thread();
}


ptu32_t djy_third_thread(void)
{
	
#ifdef Monitor_version	
	djy_printf("\r\nT-3\r\n");
#endif
	while(1)
	{
#ifdef Monitor_version

		//Blink_LED(delay_cnt1,6,8);
		//ClearSet_LED(g_LEDs[5], LED_TOGGLE);
		(*pSRU_PIN3) ^= 0x00004000;
		LED_Status3 ^= 0x04;
		Delays(delay_cnt1);
		//ClearSet_LED(g_LEDs[6], LED_TOGGLE);
		(*pSRU_PIN3) ^= 0x00200000;
		LED_Status3 ^= 0x02;
		Delays(delay_cnt1);
		//ClearSet_LED(g_LEDs[7], LED_TOGGLE);
		(*pSRU_PIN4) ^= 0x00000001;
		LED_Status3 ^= 0x01;
		Delays(delay_cnt1);
		
		LED_Note3 ^= 0x07;
		if(LED_Note3 != LED_Status3)
		{
			ALU_ERR_CNT++;
			if(LED_Note3 != LED_Status3)
			{
				sysreg_bit_clr(sysreg_MODE1, IRPTEN );
				while(loop_samp);
			}
		}
		NOP;
				
#endif
#ifdef Release_version

		Blink_LED(delay_cnt1,6,8);
#endif

	}
	sysreg_bit_clr(sysreg_MODE1, IRPTEN );
	while(1);
	
	/* Begin adding your custom code here */
	return 0;
 	
}

ptu32_t __djy_second_thread(void)
{
	return djy_second_thread();
}


ptu32_t djy_second_thread(void)
{
#ifdef Monitor_version	
	djy_printf("\r\nT-2\r\n");
#endif
	while(1)
	{
#ifdef Monitor_version

		//Blink_LED(delay_cnt1,1,1);
		//ClearSet_LED(g_LEDs[1], LED_TOGGLE);
		sysreg_bit_tgl(sysreg_FLAGS, FLG7);
		LED_Status2 ^= 0x04;
		Delay(delay_cnt1);
		//ClearSet_LED(g_LEDs[2], LED_TOGGLE);
		sysreg_bit_tgl(sysreg_FLAGS, FLG4);
		LED_Status2 ^= 0x02;
		Delay(delay_cnt1);
		//ClearSet_LED(g_LEDs[3], LED_TOGGLE);
		(*pSRU_PIN0) ^= 0x00004000;
		LED_Status2 ^= 0x01;
		Delay(delay_cnt1);
		
		LED_Note2 ^= 0x07;
		if(LED_Note2 != LED_Status2)
		{
			ALU_ERR_CNT++;
			if(LED_Note2 != LED_Status2)
			{
				sysreg_bit_clr(sysreg_MODE1, IRPTEN );
				while(loop_samp);
			}
		}
		NOP;
#endif
#ifdef Release_version

		Blink_LED(delay_cnt1,1,3);
#endif

	}
	sysreg_bit_clr(sysreg_MODE1, IRPTEN );
	while(1);
	
	/* Begin adding your custom code here */
	return 0;
 	
}

//简易文件系统不支持目录
#define szRecFileDIR
#define szRecFileACT       (szRecFileDIR "Act.rcd"  )      // 动作记录文件名(显示区)
static struct file_rsc * fs_testFileA;

int t_write_buf[5]={1,4,6,7,9},t_read_buf[3]={0,0,0};
int return_result=true;
int temp_data=0;

ptu32_t djy_main(void)
{   
	djy_set_RRS_slice(10);
/**/	//easynorflash test
	return_result=easynor_create_file(szRecFileACT,"easy nor flash",0x10000);     //文件最大尺寸64K
	///	return_result=return_result+'0';
	///xmitUARTmessage(&return_result,1);
    fs_testFileA = djyfs_fopen(szRecFileACT,"r+b");
    
    return_result=djyfs_fwrite_r(fs_testFileA,t_write_buf,5, 0);
    return_result=djyfs_fread_r(fs_testFileA,t_read_buf,3, 0);
    return_result=djyfs_fread_r(fs_testFileA,t_read_buf,3, 2);


/*	//IIC temp_sensor test
	Delay(200000);
	ADM1032_Read_Data(SR_R,&temp_data,1);
	ADM1032_Read_Data(RCS_R,&temp_data,1);
	Delay(2000000);
	t_write_buf[0]=0x0A;
	ADM1032_Write_Data(CR_W,t_write_buf,1);
	ADM1032_Read_Data(CR_R,&temp_data,1);
	Delay(200000);
	t_write_buf[0]=-8;
	ADM1032_Write_Data(RTOHB_W,t_write_buf,1);
	ADM1032_Read_Data(RTOHB_R,&temp_data,1);
	Delay(200000);
	ADM1032_Read_Data(RCS_R,&temp_data,1);
	Delay(200000);
	Delay(200000);
	
	// use for LINKPORT DMA Test
	module_init_linkport(LinkPort2,RecvMode,16);
	Delay(10000000);
	///while(!LP2RecvFlag);
	///LP2RecvFlag=false;
	module_init_linkport(LinkPort2,SendMode,16);
	Delay(10000000);
	
	module_init_linkport(LinkPort2,RecvMode,16);
	while(loop_samp)
	{
		;
	}
*/
	while(1)
	{
		semp_pend(thread_change_test1,cn_timeout_forever);
		Blink_LED(delay_cnt2,4,5);
		Blink_LED(delay_cnt2,4,5);
#ifdef Release_version
		xmitUARTmessage("\r\n<<djyos:thread-Release_version-running>>\r\n",sizeof("\r\n<<djyos:thread-Release_version-running>>\r\n"));
#endif
#ifdef Monitor_version
		djy_printf("\r\ndjyos:thread-Monitor_version-running\r\n");
#endif
	}
	
	/* Begin adding your custom code here */
	return 0;
}

ptu32_t __djy_main(void)
{
	return djy_main();
}

//----组件初始化2-----------------------------------------------------------
//功能：可选组件初始化函数共有3个:
//      1、sys_module_init，在sys_init.c中，参见该函数注释
//      2、prj_module_init，在这里初始化的模块，需要调用跟具体工程相关的代码，例
//          如键盘模块，除了调用module_init_keyboard函数外，还要调用跟硬件相关的
//          module_init_keyboard_hard函数。
//      前两步初始化时，块相联分配方式还没有初始化，驱动中如果用到动态分配，使用
//      的是准静态分配，关于准静态分配的说明，参见mems.c文件。
//      3、run_module_init，参见该函数注释。
//参数：无
//返回：无
//-----------------------------------------------------------------------------
void prj_module_init(void)
{
	
    djy_handle_t char_term_lhdl;
	uint16_t evtt_main;
	
	//for test use only
	our_project_init();
	

#if(cfg_uart1_used == 1)
    module_init_uart(1);
#endif


#if (cfg_shell == 1)
    module_init_shell(0);
    module_exp_shell_init(0);
#if (cfg_xmodem == 1)
extern ptu32_t module_int_ymodem(ptu32_t para);
   	module_int_ymodem(0);
#endif
    char_term_lhdl = dev_open_left(cfg_stdin_device,0);
    if(char_term_lhdl != NULL)
    {
        //设置串口波特率为115200，
        dev_ctrl_left(char_term_lhdl,cn_uart_set_baud,115200,0);
        module_init_char_term((ptu32_t)char_term_lhdl);    //执行这条命令会报FIXI异常
        dev_ctrl_left(char_term_lhdl,cn_uart_start,0,0);
    }
#endif

/*	// use for 8Bit ParaFlash Test
	reset_norflash();
	erase_sector(40);
	erase_sector(41);
	erase_sector(42);
	write_data(42,0x0005,t_write_buf,3);
	read_data(42,0x0005,t_read_buf,3);
	// use for 8Bit ParaFlash Test
		return_result=t_read_buf[0]+'0';
	xmitUARTmessage(&return_result,1);
		return_result=t_read_buf[1]+'0';
	xmitUARTmessage(&return_result,1);
		return_result=t_read_buf[2]+'0';
	xmitUARTmessage(&return_result,1);
	write_data(42,0x0000,t_write_buf,3);
	read_data(42,0x0000,t_read_buf,3);
	// use for 8Bit ParaFlash Test
		return_result=t_read_buf[0]+'0';
	xmitUARTmessage(&return_result,1);
		return_result=t_read_buf[1]+'0';
	xmitUARTmessage(&return_result,1);
		return_result=t_read_buf[2]+'0';
	xmitUARTmessage(&return_result,1);
	while(1);
*/	
//目前，Release版本的文件系统跑不起来，建立失败，因为片内heap空间不足.
//可以将文件系统所需要的内存空间转移至DDR2中的独立空间，需要修改
#if ((cfg_djyfs == 1) && (cfg_run_mode != cn_mode_tiny))
    module_init_djyfs(0);
#if (cfg_djyfs_easy_flash == 1)
extern ptu32_t module_init_fs_easynorflash(ptu32_t para);
    return_result=module_init_fs_easynorflash(0);
#endif
    djyfs_set_work_path(cfg_work_path);
#endif

/**/
	// use for 8Bit SPIFlash Test
	m25p16_Open();
	//EraseFlash(0);
	EraseBlock(0,0x00);
	m25p16_Read(t_read_buf,0x00,3);
	m25p16_Write(t_write_buf,0x00,3);
	m25p16_Read(t_read_buf,0x00,3);
	EraseBlock(0,0x00);
	// use for 8Bit SPIFlash Test


	// use for IIC Temp_sensor Test
/*	module_init_adm1032(0);
*/	

/*	//endian test
	//32bit test
	fill_big_32bit(test_data_buf,0,endian_test);
	endian_test=pick_big_32bit(test_data_buf,0);
	fill_little_32bit(test_data_buf,0,endian_test);
	endian_test=pick_little_32bit(test_data_buf,0);
	//16bit test
	fill_big_16bit(test_data_buf,0,endian_test);
	endian_test=pick_big_16bit(test_data_buf,0);
	fill_little_16bit(test_data_buf,0,endian_test);
	endian_test=pick_little_16bit(test_data_buf,0);
	//64bit test
	endian_test64=0x123456789abcdef0;
	fill_little_64bit(test_data_buf,0,endian_test64);
	endian_result64=pick_little_64bit(test_data_buf,0);
	fill_big_64bit(test_data_buf,0,endian_test64);
	endian_result64=pick_big_64bit(test_data_buf,0);
	
	itoa(0xffffffff, test_data_buf, 10);
	itoa(0xffffffff, test_data_buf, 16);
*/

    evtt_main = djy_evtt_regist(enum_correlative,cn_prio_RRS,0,0,2,
                                __djy_main,cfg_main_stack,"main function");
    //事件的两个参数暂设为0，如果用shell启动，可用来采集shell命令行参数
    djy_event_pop(evtt_main,NULL,0,NULL,0,0,190);

    evtt_main = djy_evtt_regist(enum_correlative,cn_prio_RRS,0,0,2,
                               __djy_second_thread,cfg_main_stack,"second thread");
                               
    //事件的两个参数暂设为0，如果用shell启动，可用来采集shell命令行参数
    djy_event_pop(evtt_main,NULL,0,NULL,0,0,200);

    evtt_main = djy_evtt_regist(enum_correlative,cn_prio_RRS,0,0,2,
                               __djy_third_thread,cfg_main_stack,"third thread");
                               
    //事件的两个参数暂设为0，如果用shell启动，可用来采集shell命令行参数
    djy_event_pop(evtt_main,NULL,0,NULL,0,0,200);
    
	thread_change_test1 = semp_create(1,1,NULL);
	thread_change_test2 = semp_create(1,1,NULL);
	thread_change_test3 = semp_create(1,1,NULL);
    
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


void our_project_init(void)
{
	
	InitSRU();
	Init_ParallelFlash();
	

/****************SYS INIT*******************/

	
	sysreg_bit_clr(sysreg_MODE1, IRPTEN ); /* disable global interrupts */
	/* unmask individual interrupts */
	(*pDAI_IRPTL_PRI) = (SRU_EXTMISCA1_INT | SRU_EXTMISCA2_INT);
	
	/* make sure interrupts latch on the rising edge */
    (*pDAI_IRPTL_RE) = (SRU_EXTMISCA1_INT | SRU_EXTMISCA2_INT);

  	/* setup the IRQ pins to use the pushbuttons */
	(*pSYSCTL) |= (IRQ1EN | IRQ2EN);
	
	sysreg_bit_set(sysreg_MODE2, (IRQ1E | IRQ2E) );
	
	/* enable IRQ interrupts and make DAI interrupts high priority */
	sysreg_bit_set(sysreg_IMASK, (IRQ1I | IRQ2I | DAIHI) );
	sysreg_bit_clr(sysreg_IMASKP, (IRQ1I | IRQ2I | DAIHI) );
	
	sysreg_bit_clr(sysreg_IRPTL, (IRQ1I | IRQ2I) );

	/* register the interrut handlers */
	
    int_isr_connect(cn_int_line_IRQ2I,SW9_IRQ2_handler);
    int_restore_real_line(cn_int_line_IRQ2I);
        
    int_isr_connect(cn_int_line_DAII,__int_engine_dai);
    int_restore_asyn_line(cn_int_line_DAII);
        
///    int_cut_trunk();
///    int_contact_trunk();
    	
	sysreg_bit_clr(sysreg_IRPTL, (IRQ1I | IRQ2I) );
	NOP;NOP;
	sysreg_bit_set(sysreg_MODE1, NESTM ); /* enable interrupts nest */
	sysreg_bit_set(sysreg_MODE1, IRPTEN ); /* enable global interrupts */
	
// 测试通过	
//	atom_high_test=int_high_atom_start();
//	int_high_atom_end(atom_high_test);
//	atom_low_test=int_low_atom_start();
//	int_low_atom_end(atom_low_test);
// 测试通过	
//	int_tap_line(cn_int_line_IRQ2I);
//	int_tap_line(cn_int_line_DAII);
//	int_tap_line(cn_int_line_P12I);
// 测试通过	
//  bool_value=int_query_line(cn_int_line_IRQ2I);
//  bool_value=int_query_line(cn_int_line_DAII);
//  bool_value=int_query_line(cn_int_line_P12I);
// 测试通过	
//  int_cut_line(cn_int_line_SFT3I);
//  int_cut_line(cn_int_line_P17I);
//  int_cut_line(cn_int_line_IRQ2I);
    
/****************UART TEST******************/

//	xmitUARTmessage("Djyos",sizeof("Djyos")); 
	
}

void Delays(const int n)
{	
	int nDelay,mDelay;
	for( nDelay = 0; nDelay < n; nDelay++)
	{
		NOP;
	}
}

// This function will setup the SRU Registers
void InitSRU(void)
{

/**************SPI  SET****************/
//SPI for EzBoard
	// setup SPI signals 
	SRU(SPI_MOSI_O,DPI_PB01_I);
	SRU(SPI_MOSI_PBEN_O, DPI_PBEN01_I);

	SRU(DPI_PB02_O, SPI_MISO_I);     //Connect DPI PB2 to MISO.
	SRU(SPI_MISO_PBEN_O, DPI_PBEN02_I);
	
	SRU(SPI_CLK_O,DPI_PB03_I);
	SRU(SPI_CLK_PBEN_O,DPI_PBEN03_I);

	// for the flag pins to act as chip select:SS0
	SRU(FLAG4_O, DPI_PB05_I);
	SRU(HIGH, DPI_PBEN05_I);

	// first set flag 4 as an output 
	sysreg_bit_set( sysreg_FLAGS, FLG4O );
	sysreg_bit_set( sysreg_FLAGS, FLG4 );

/****************UART SET*******************/

	*pPICR2 &= ~(0x1F); //Sets the UART0 receive&transmit interrupt to P12I

	*pPICR2 |= (0x13<<0); 
	
	SRU2(UART0_TX_O,DPI_PB09_I); // UART transmit signal is connected to DPI pin 9
	SRU2(HIGH,DPI_PBEN09_I);
	SRU2(DPI_PB10_O,UART0_RX_I); // connect the pin buffer output signal to the UART0 receive
	SRU2(LOW,DPI_PB10_I);
	SRU2(LOW,DPI_PBEN10_I);      // disables DPI pin10 as input
	
/****************IIC  SET*******************/

	*pPICR3 &= ~(0x1F); //Sets the TWI receive&transmit interrupt to P18I

	*pPICR3 |= (0x17<<0); 
	
	// configure SRU 
    SRU2( LOW, DPI_PB07_I);
    SRU2( TWI_DATA_PBEN_O, DPI_PBEN07_I);
    SRU2( DPI_PB07_O, TWI_DATA_I);
    SRU2( LOW, DPI_PB08_I);
    SRU2( TWI_CLK_PBEN_O, DPI_PBEN08_I);
    SRU2( DPI_PB08_O, TWI_CLK_I);

/**************LinkPort  SET****************/
	
	*pPICR1 &= ~(0x1F<<25); // assign LINK PORT 0 interrupt to Programmable Interrupt 11
	*pPICR1 |= (0x19<<25); 
	
	*pPICR2 &= ~(0x1F<<20); // assign LINK PORT 1 interrupt to Programmable Interrupt 16
	*pPICR2 |= (0x1A<<20); 
	
/**************Demon Board  SET****************/
	Init_LEDs();
	Init_PushButtons();
	
}
/*******************************************************************
*   Function:    Init_ParallelFlash
*   Description: This function initializes parallel flash.
*******************************************************************/
void Init_ParallelFlash(void)
{
	/* setup EPCTL to use bank 1 (MS1) core accesses */
	*pEPCTL = (((*pEPCTL) & (~B1SD)) | EPBRCORE);

	/* setup for max waitstates, note the PKDIS bit is set which makes a 1 to 1
		mapping, each 8 bit byte maps to an address */
	*pAMICTL1 = ( PKDIS | WS31 | HC1 | HC2 | RHC1 | RHC2 | IC7 | AMIEN | AMIFLSH);
}