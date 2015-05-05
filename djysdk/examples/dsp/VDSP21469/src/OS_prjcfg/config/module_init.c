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
#include "rtc_ds1390.h"
#include "at45db321.h"
#include "endian.h"
//#include "stdarg.h"
//#include "char_term.h"

//for test use only
//#include "m29w320.h"
//#include "m25p16.h"`
#include "adm1032.h"
#include "led_key.h"
//#include "VDSP21469_uart.h"

#ifdef Monitor_version
		int delay_cnt1=1000000;
		int delay_cnt2=500000;
#endif
#ifdef Release_version
		int delay_cnt1=10000000;
		int delay_cnt2=5000000;
#endif

//section("seg_test") u32 num32_test=0x12345678;

struct tagSemaphoreLCB *thread_change_test1;
struct tagSemaphoreLCB *thread_change_test2;
struct tagSemaphoreLCB *thread_change_test3;

struct tagDjyTm gtime;

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

		Blink_LED(delay_cnt1,6,8);
#endif
#ifdef Release_version

		Blink_LED(delay_cnt1,6,8);
#endif

	}
	
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

		Blink_LED(delay_cnt1,1,1);
		//LED_Bar_BacknForth(delay_cnt1);
#endif
#ifdef Release_version

		Blink_LED(delay_cnt1,1,3);
#endif

	}
	
	/* Begin adding your custom code here */
	return 0;
 	
}

//简易文件系统不支持目录
#define szRecFileDIR
#define szRecFileACT       (szRecFileDIR "Act.rcd"  )      // 动作记录文件名(显示区)
static struct tagFileRsc * fs_testFileA;

int t_write_buf[5]={1,3,5,7,9},t_read_buf[3]={0,0,0};
int return_result=true;
int temp_data=0;

ptu32_t djy_main(void)
{   
	Djy_SetRRS_Slice(10);
/*	
	return_result=Easynor_CreateFile(szRecFileACT,"easy nor flash",0x10000);     //文件最大尺寸64K
    fs_testFileA = Djyfs_Fopen(szRecFileACT,"r+b");
    
    return_result=Djyfs_Fwrite_r(fs_testFileA,t_write_buf,5, 0);
    return_result=Djyfs_Fread_r(fs_testFileA,t_read_buf,3, 2);
*/
	while(1)
	{
		Lock_SempPend(thread_change_test1,CN_TIMEOUT_FOREVER);
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
	

#if(CN_CFG_UART1_USED == 1)
    Uart_ModuleInit(1);
#endif


#if (CN_CFG_SHELL == 1)
    Sh_MoudleInit(0);
    module_exp_shell_init(0);
#if (CN_CFG_XMODEM == 1)
extern ptu32_t Ymodem_ModuleInit(ptu32_t para);
   	Ymodem_ModuleInit(0);
#endif
    char_term_lhdl = Driver_DevOpenLeft(CN_CFG_STDIN_DEVICE,0);
    if(char_term_lhdl != NULL)
    {
        //设置串口波特率为115200，
        Driver_DevCtrlLeft(char_term_lhdl,CN_UART_SET_BAUD,115200,0);
        CharTerm_ModuleInit((ptu32_t)char_term_lhdl);    //执行这条命令会报FIXI异常
        Driver_DevCtrlLeft(char_term_lhdl,CN_UART_START,0,0);
    }
#endif

    reset_norflash(); //复位Flash
   	
    evtt_main = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,2,
                                __djy_main,CN_CFG_MAIN_STACK,"main function");
    //事件的两个参数暂设为0，如果用shell启动，可用来采集shell命令行参数
    Djy_EventPop(evtt_main,NULL,0,NULL,0,0,190);

    evtt_main = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,2,
                               __djy_second_thread,CN_CFG_MAIN_STACK,"second thread");
                               
    //事件的两个参数暂设为0，如果用shell启动，可用来采集shell命令行参数
    Djy_EventPop(evtt_main,NULL,0,NULL,0,0,200);

    evtt_main = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,2,
                               __djy_third_thread,CN_CFG_MAIN_STACK,"third thread");
                               
    //事件的两个参数暂设为0，如果用shell启动，可用来采集shell命令行参数
    Djy_EventPop(evtt_main,NULL,0,NULL,0,0,200);
    
	thread_change_test1 = Lock_SempCreate(1,1,NULL);
	thread_change_test2 = Lock_SempCreate(1,1,NULL);
	thread_change_test3 = Lock_SempCreate(1,1,NULL);
    
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
/*	
	// configure SRU 
	SRU(LOW,DPI_PB01_I);
	SRU(LOW,DPI_PB02_I);
	SRU(LOW,DPI_PB07_I);
	SRU(TWI_DATA_PBEN_O,DPI_PBEN07_I);
	SRU(DPI_PB07_O, TWI_DATA_I);
	SRU(LOW,DPI_PB08_I);
	SRU2(TWI_CLK_PBEN_O,DPI_PBEN08_I);
*/
/**************LinkPort  SET****************/
	
	*pPICR1 &= ~(0x1F<<25); // assign LINK PORT 0 interrupt to Programmable Interrupt 11
	*pPICR1 |= (0x19<<25); 
	
	*pPICR2 &= ~(0x1F<<20); // assign LINK PORT 1 interrupt to Programmable Interrupt 16
	*pPICR2 |= (0x1A<<20); 
	
/****************SYS INIT*******************/

	Init_LEDs();
	Init_PushButtons();
	
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
	
    Int_IsrConnect(cn_int_line_IRQ2I,SW9_IRQ2_handler);
    Int_RestoreRealLine(cn_int_line_IRQ2I);
        
    Int_IsrConnect(cn_int_line_DAII,__int_engine_dai);
    Int_RestoreAsynLine(cn_int_line_DAII);
        
///    Int_CutTrunk();
///    Int_ContactTrunk();
    	
	sysreg_bit_clr(sysreg_IRPTL, (IRQ1I | IRQ2I) );
	NOP;NOP;
	sysreg_bit_set(sysreg_MODE1, NESTM ); /* enable interrupts nest */
	sysreg_bit_set(sysreg_MODE1, IRPTEN ); /* enable global interrupts */
	
// 测试通过	
//	atom_high_test=Int_HighAtomStart();
//	Int_HighAtomEnd(atom_high_test);
//	atom_low_test=Int_LowAtomStart();
//	Int_LowAtomEnd(atom_low_test);
// 测试通过	
//	Int_TapLine(cn_int_line_IRQ2I);
//	Int_TapLine(cn_int_line_DAII);
//	Int_TapLine(cn_int_line_P12I);
// 测试通过	
//  bool_value=Int_QueryLine(cn_int_line_IRQ2I);
//  bool_value=Int_QueryLine(cn_int_line_DAII);
//  bool_value=Int_QueryLine(cn_int_line_P12I);
// 测试通过	
//  Int_CutLine(cn_int_line_SFT3I);
//  Int_CutLine(cn_int_line_P17I);
//  Int_CutLine(cn_int_line_IRQ2I);
    
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
	/* setup SPI signals */	
	SRU(SPI_MOSI_O,DPI_PB01_I);
	SRU(SPI_MOSI_PBEN_O, DPI_PBEN01_I);

	SRU(DPI_PB02_O, SPI_MISO_I);     //Connect DPI PB2 to MISO.
	SRU(SPI_MISO_PBEN_O, DPI_PBEN02_I);
	
	SRU(SPI_CLK_O,DPI_PB03_I);
	SRU(SPI_CLK_PBEN_O,DPI_PBEN03_I);

	/* for the flag pins to act as chip select:SS0 */
	SRU(FLAG4_O, DPI_PB05_I);
	SRU(HIGH, DPI_PBEN05_I);

	/* for the flag pins to act as chip select:SS1 */
	SRU(FLAG5_O, DPI_PB04_I);
	SRU(HIGH, DPI_PBEN04_I);
	
	/* for the flag pins to act as chip select:SS2 */
	SRU(FLAG6_O, DPI_PB06_I);
	SRU(HIGH, DPI_PBEN06_I);
	
	/* first set flag 4 as an output */
	sysreg_bit_set( sysreg_FLAGS, FLG4O );
	sysreg_bit_set( sysreg_FLAGS, FLG4 );
	/* first set flag 5 as an output */
	sysreg_bit_set( sysreg_FLAGS, FLG5O );
	sysreg_bit_set( sysreg_FLAGS, FLG5 );
	/* first set flag 6 as an output */
	sysreg_bit_set( sysreg_FLAGS, FLG6O );
	sysreg_bit_set( sysreg_FLAGS, FLG6 );
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