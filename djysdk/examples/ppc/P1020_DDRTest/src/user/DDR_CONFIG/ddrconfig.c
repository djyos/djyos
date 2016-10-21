/*
 * ddrconfig.c
 *
 *  Created on: Jun 8, 2015
 *      Author: zhangqf
 */

#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"

#include "os.h"
#include "shell.h"

#include "SysSpace.h"
#include "cpu.h"

#define CN_DDRC_BASE_ADDR           (CN_CCSR_NEWADDR_V+0x2000)

#define DEBUG_3_ADDR                 (0xF08)
#define DEBUG_13_ADDR                (0xF30)
#define DEBUG_22_ADDR                (0xF54)
#define DEBUG_2_ADDR                 (0xF04)

#define CS0_BNDS_REG                 (0x000)
#define CS1_BNDS_REG                 (0x008)
#define CS0_CFG_REG                  (0x080)
#define CS1_CFG_REG                  (0x084)
#define CS0_CFG2_REG                 (0x0C0)
#define CS1_CFG2_REG                 (0x0C4)
#define TIMING_CFG0_REG              (0x104)
#define TIMING_CFG1_REG              (0x108)
#define TIMING_CFG2_REG              (0x10C)
#define TIMING_CFG3_REG              (0x100)
#define TIMING_CFG4_REG              (0x160)
#define TIMING_CFG5_REG              (0x164)
#define SDRAM_CFG_REG                (0x110)
#define SDRAM_CFG2_REG               (0x114)
#define SDRAM_INTERVAL_REG           (0x124)
#define SDRAM_CLK_CNTL_REG           (0x130)
#define ZQ_CNTL_REG                  (0x170)
#define DDRmc1_WRLVL_CNTL_ADDR       (0x174)
#define WRLVL_CNTL2_REG              (0x190)
#define WRLVL_CNTL3_REG              (0x194)
#define DDRmc1_SDRAM_MODE_ADDR       (0x118)
#define DDRmc1_SDRAM_MODE_2_ADDR     (0x11C)

#define INIT_ADDR_REG                (0x148)

#define INIT_EXT_ADDR_REG            (0x14C)
#define SDRAM_RCW1_REG               (0x180)
#define SDRAM_RCW2_ADDR              (0x184)
#define DATA_INIT_REG                (0x128)
#define SDRAM_MD_CNTL_REG            (0x120)
#define DDRCDR1_REG                  (0xB28)
#define DDRCDR2_REG                  (0xB2C)

/* Errata: B4, T4 for v1.0
Before enabling the DDR controller, set the DDR memory mapped register space at offset 0xF70 to a value of 0x30003000."*/
#define DDR_MEMORYMAPPED_SPACE       (0xF70)
#define DDR_MEMORYMAPPED_SPACE_VALUE 0x30003000

#define SDRAM_CFG_MEM_EN_MASK           0x80000000
#define SDRAM_CFG2_D_INIT_MASK          0x00000010
#define TIMING_CFG2_CPO_MASK            0xF07FFFFF
#define CS0_BNDS_DEBUG3_MASK            0x00000400
#define CS0_BNDS_DEBUG13_MASK           0x00000015
#define CS0_BNDS_DEBUG22_MASK           0x24000000
#define ZQ_CNTL_ZQ_EN_DISABLE_MASK      0x7FFFFFFF
#define SDRAM_INTERVAL_REFINT_MASK      0x0000FFFF
#define SDRAM_MD_CNTL_MASK              0x84080000
#define SDRAM_MD_CNTL_MD_EN_MASK        0x80000000
#define SDRAM_MD_CNTL_CKE_CNTL_MASK     0x00200000
#define WRLVL_CNTL_WRLVL_EN_DISABLE_MASK        0x7FFFFFFF
#define SDRAM_CFG2_RCW_D_INT_DISABLE_MASK       0xFFFFFFEB

#define CN_DDR_REG_NUM   400
static u32 gDdrRegValueWrite[CN_DDR_REG_NUM];
static u32 gDdrRegValueRead[CN_DDR_REG_NUM];

//THIS ARRAY DETERMINE THE SEQUENCE OF CONFIG THE DDR REGS
static u32 gDdrRegUsed[]=\
{
	//DDR CS0
	CS0_BNDS_REG,
	CS0_CFG_REG,
	CS0_CFG2_REG,
	//DDR CS1
	CS1_BNDS_REG,
	CS1_CFG_REG,
	CS1_CFG2_REG,
	
	//DDR TIMING
	TIMING_CFG3_REG,
	TIMING_CFG0_REG,
	TIMING_CFG1_REG,
	TIMING_CFG2_REG,
	
	//SDRAM
	SDRAM_MD_CNTL_REG,
	SDRAM_INTERVAL_REG,
	DATA_INIT_REG,
	SDRAM_CLK_CNTL_REG,
	INIT_ADDR_REG,
	INIT_EXT_ADDR_REG,
	
	TIMING_CFG4_REG,
	TIMING_CFG5_REG,
	
	ZQ_CNTL_REG,
	WRLVL_CNTL2_REG,
	WRLVL_CNTL3_REG,
	
	DDRCDR1_REG,
	DDRCDR2_REG,
	
	
	SDRAM_RCW1_REG,
	SDRAM_RCW2_ADDR,
	
	SDRAM_CFG_REG,
};

#define CN_USED_REGNUM    ((sizeof(gDdrRegUsed))/4)

void WriteDdrReg(u32 reg, u32 value)
{
	u32 *addr;
	addr = (u32 *)(CN_DDRC_BASE_ADDR + reg);
	write32(addr, value);
	return;
}

u32 ReadDdrReg(u32 reg)
{
	u32 result;
	u32 *addr;
	addr = (u32 *)(CN_DDRC_BASE_ADDR + reg);
	result = read32(addr);
	return result;
}

bool_t StartConfigDdr(char *param)
{
	volatile u32 i = 0;
	volatile u32 reg;
	volatile u32 value;
	
	printk("\n\r");
	printk("%s:Operation Begin..\n\r",__FUNCTION__);
	
	//disable the ddr mem
	reg = SDRAM_CFG_REG;
	value = ReadDdrReg(reg);
	value &=(~SDRAM_CFG_MEM_EN_MASK);
	write32(reg,value);
	
	for(i = 0; i < CN_USED_REGNUM; i++)
	{
		//config the ddrc sequencely
		reg = gDdrRegUsed[i];
		value = gDdrRegValueWrite[reg/4];
		WriteDdrReg(reg, value);
	}
	
	//do some delay here
	for(i =0; i <0x1000;i++)
	{
		
	}
	//before we enable the ddr, we must modified the errata
	reg = DDR_MEMORYMAPPED_SPACE;
	value = DDR_MEMORYMAPPED_SPACE_VALUE;
	WriteDdrReg(reg,value);
	
	//enable the ddr now
	reg = SDRAM_CFG_REG;
	value = ReadDdrReg(reg);
	value |= SDRAM_CFG_MEM_EN_MASK;
	WriteDdrReg(reg, value);
	
	//wait until the ddr controller init success
	reg = SDRAM_CFG2_REG;
	value = ReadDdrReg(reg);
	while(value&SDRAM_CFG2_D_INIT_MASK)
	{
		value = ReadDdrReg(reg);
	}
	printk("%s:Operation End\n\r",__FUNCTION__);
	return true;
}

bool_t ShowConfigWrite(char *param)
{
	volatile u32  i =0;
	volatile u32  reg = 0;
	volatile u32  value = 0;
	printk("\n\r");
	printk("%s:Operation Begin..\n\r",__FUNCTION__);
	for(i = 0; i< CN_USED_REGNUM;i++)
	{
		reg = gDdrRegUsed[i];
		value = gDdrRegValueWrite[reg/4];
		printk("Reg:0x%04x = 0x%08x \n\r",reg,value);
	}
	printk("%s:Operation End\n\r",__FUNCTION__);
	return true;
}

bool_t ReadConfig(char *param)
{
	volatile u32  i =0;
	volatile u32  reg = 0;
	volatile u32  value = 0;
	printk("\n\r");
	printk("%s:Operation Begin..\n\r",__FUNCTION__);
	for(i = 0; i< CN_USED_REGNUM;i++)
	{
		reg = gDdrRegUsed[i];
		value = ReadDdrReg(reg);
		gDdrRegValueRead[reg/4] = value;
		printk("Reg:0x%04x = 0x%08x \n\r",reg,value);
	}
	printk("%s:Operation End\n\r",__FUNCTION__);
	return true;
}
bool_t SetReg(char *param)
{
    char *word,*next_param;
    u32  reg;
    u32  value;
        
	printk("\n\r");
	
    word = Sh_GetWord(param,&next_param);
    reg = strtol(word, (char **)NULL, 0);
    word = Sh_GetWord(next_param,&next_param);
    value = strtol(word, (char **)NULL, 0);
   
    if((reg >= 0)&&(reg <0x1000))
    {
    	gDdrRegValueWrite[reg/4] = value;
    	printk("%s:set:reg = 0x%04x, value = 0x%08x\n\r",__FUNCTION__,reg,value);
    }
    else
    {
    	printk("%s:para error:reg = 0x%04x, value = 0x%08x\n\r",__FUNCTION__,reg,value);
    }
    
	return true;
}


bool_t CpyRead2Write(char *param)
{
	volatile u32  i = 0;
	volatile int  len;
	volatile u32  reg;
	
	printk("\n\r");
	printk("%s:Operation Begin..\n\r",__FUNCTION__);
	

	len = (sizeof(gDdrRegUsed))/4;
	for(i = 0; i < len; i++)
	{
		reg = gDdrRegUsed[i];
		gDdrRegValueWrite[reg/4] = gDdrRegValueRead[reg/4];
	}
	
	printk("%s:Operation End\n\r",__FUNCTION__);

	return true;
}

extern bool_t DDRTEST_TI(char *param);
struct ShellCmdTab  g_DdrCfgDebug[] =
{
    {
        "ddrcfg",
        StartConfigDdr,
        "ÅäÖÃÄÚ´æ¿ØÖÆÆ÷",
        NULL
    },
    {
        "ddrcfgshow",
        ShowConfigWrite,
        "Õ¹Ê¾ÅäÖÃ»º´æÄÚÈÝ",
        NULL
    },
    {
        "ddrcfgread",
        ReadConfig,
        "¶ÁÈ¡DDR¿ØÖÆÆ÷ÄÚ´æ",
        NULL
    },
    {
        "setreg",
        SetReg,
        "ÉèÖÃÅäÖÃ¼Ä´æÆ÷»º´æ",
        NULL
    },
    {
        "cpyfromread",
        CpyRead2Write,
        "¿½±´¶ÁÈ¡ÅäÖÃµ½ÅäÖÃ»º´æ",
        NULL
    },    
    {
        "reset",
         reset,
        "ÖØÆô»úÆ÷",
        NULL
    },
    {
        "ddrtest_ti",
        DDRTEST_TI,
        "ÄÚ´æ²âÊÔTI",
        NULL
    }
};

#define CN_DDR_DEBUGCMD_NUM  ((sizeof(g_DdrCfgDebug))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc g_DdrCfgDebugCmdRsc[CN_DDR_DEBUGCMD_NUM];
void ModuleInstall_DdrDebug(void)
{
	printk("\n\r");
	printk("%s:Operation Begin..\n\r",__FUNCTION__);
	
	if(Sh_InstallCmd(g_DdrCfgDebug,g_DdrCfgDebugCmdRsc,CN_DDR_DEBUGCMD_NUM))
	{
		printk("%s:DDR CFG SHELL LOADED SUCCESS\n\r",__FUNCTION__);
	}
	else
	{
		printk("%s:DDR CFG SHELL LOADED FAILED\n\r",__FUNCTION__);
	}	
	printk("%s:Operation End\n\r",__FUNCTION__);
	
	return;
}



