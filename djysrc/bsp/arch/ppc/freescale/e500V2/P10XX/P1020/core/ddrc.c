#include "stdint.h"
#include "DDRC.h"

// =============================================================================
// 函数功能:DdrcInit
//          DDRC的初始化
// 输入参数:
// 输出参数:无
// 返回值  :
// 说明    :667
// =============================================================================
void DdrcInit(void)
{
   /* Chip select 0 registers initialization */
   /* CS0_BNDS */
   *(u32 *)DDRmc1_CS0_BNDS_ADDR =  DDRmc1_CS0_BNDS_VAL;
   /* CS0_CONFIG */
   *(u32 *)DDRmc1_CS0_CONFIG_ADDR =  DDRmc1_CS0_CONFIG_VAL;
   /* CS0_CONFIG_2 */
   *(u32 *)DDRmc1_CS0_CONFIG_2_ADDR =  DDRmc1_CS0_CONFIG_2_VAL;

   /* Chip select 1 registers initialization */
   /* CS1_BNDS */
   *(u32 *)DDRmc1_CS1_BNDS_ADDR =  DDRmc1_CS1_BNDS_VAL;
   /* CS1_CONFIG */
   *(u32 *)DDRmc1_CS1_CONFIG_ADDR =  DDRmc1_CS1_CONFIG_VAL;
   /* CS1_CONFIG_2 */
   *(u32 *)DDRmc1_CS1_CONFIG_2_ADDR =  DDRmc1_CS1_CONFIG_2_VAL;

   /* TIMING_CFG_3 */
   *(u32 *)DDRmc1_TIMING_CFG_3_ADDR =  DDRmc1_TIMING_CFG_3_VAL;

   /* TIMING_CFG_0 */
   *(u32 *)DDRmc1_TIMING_CFG_0_ADDR =  DDRmc1_TIMING_CFG_0_VAL;

   /* TIMING_CFG_1 */
   *(u32 *)DDRmc1_TIMING_CFG_1_ADDR =  DDRmc1_TIMING_CFG_1_VAL;

   /* TIMING_CFG_2 */
   *(u32 *)DDRmc1_TIMING_CFG_2_ADDR =  DDRmc1_TIMING_CFG_2_VAL;

   /* DDR_SDRAM_CFG_2 */
   *(u32 *)DDRmc1_SDRAM_CFG_2_ADDR = DDRmc1_SDRAM_CFG_2_VAL;

   /* DDR_SDRAM_MODE */
   *(u32 *)DDRmc1_SDRAM_MODE_ADDR =  DDRmc1_SDRAM_MODE_VAL;

   /* DDR_SDRAM_MODE_2 */
   *(u32 *)DDRmc1_SDRAM_MODE_2_ADDR =  DDRmc1_SDRAM_MODE_2_VAL;


   /* DDR_SDRAM_CNTL */
   *(u32 *)DDRmc1_SDRAM_MD_CNTL_ADDR =  DDRmc1_SDRAM_MD_CNTL_VAL;

   /* DDR_SDRAM_INTERVAL */
   *(u32 *)DDRmc1_SDRAM_INTERVAL_ADDR =  DDRmc1_SDRAM_INTERVAL_VAL;

   /* DDR_SDRAM_DATA_INIT */
   *(u32 *)DDRmc1_DATA_INIT_ADDR =  DDRmc1_DATA_INIT_VAL;

   /* DDR_SDRAM_CLK_CNTL */
   *(u32 *)DDRmc1_SDRAM_CLK_CNTL_ADDR =  DDRmc1_SDRAM_CLK_CNTL_VAL;

   /* DDR_DATA_INIT */
   *(u32 *)DDRmc1_INIT_ADDR_ADDR =  DDRmc1_INIT_ADDR_VAL;

   /* DDR_INIT_EXT_ADDR */
   *(u32 *)DDRmc1_INIT_EXT_ADDR_ADDR =  DDRmc1_INIT_EXT_ADDR_VAL;

   /* TIMING_CFG_4 */
   *(u32 *)DDRmc1_TIMING_CFG_4_ADDR =  DDRmc1_TIMING_CFG_4_VAL;

   /* TIMING_CFG_5 */
   *(u32 *)DDRmc1_TIMING_CFG_5_ADDR =  DDRmc1_TIMING_CFG_5_VAL;

   /* DDR_ZQ_CNTL */
   *(u32 *)DDRmc1_ZQ_CNTL_ADDR =  DDRmc1_ZQ_CNTL_VAL;

   /* DDR_WRLVL_CNTL */
   *(u32 *)DDRmc1_WRLVL_CNTL_ADDR =  DDRmc1_WRLVL_CNTL_VAL;

   /* DDR_WRLVL_CNTL_2 */
   *(u32 *)DDRmc1_WRLVL_CNTL_2_ADDR =  DDRmc1_WRLVL_CNTL_2_VAL;

   /* DDR_WRLVL_CNTL_3 */
   *(u32 *)DDRmc1_WRLVL_CNTL_3_ADDR =  DDRmc1_WRLVL_CNTL_3_VAL;

   /* DDR_DDRCDR_1 */
   *(u32 *)DDRmc1_DDRCDR_1_ADDR =  DDRmc1_DDRCDR_1_VAL;

   /* DDR_DDRCDR_2 */
   *(u32 *)DDRmc1_DDRCDR_2_ADDR =  DDRmc1_DDRCDR_2_VAL;

   /* DDR_SDRAM_RCW_1 */
   *(u32 *)DDRmc1_SDRAM_RCW_1_ADDR =  DDRmc1_SDRAM_RCW_1_VAL;

   /* DDR_SDRAM_RCW_2 */
   *(u32 *)DDRmc1_SDRAM_RCW_2_ADDR =  DDRmc1_SDRAM_RCW_2_VAL;

   /* DDR_SDRAM_CFG */
   /* Set, but do not enable the memory */
   *(u32 *)DDRmc1_SDRAM_CFG_ADDR = (DDRmc1_SDRAM_CFG_VAL & (~(SDRAM_CFG_MEM_EN_MASK)));
   /* Sleep at least 500 microseconds */
   volatile int i = 0;
   for(i =0; i < 0x1000; i++)
   {
	   
   }
   *(u32 *)DDRmc1_DDR_MEMORY_MAPPED_SPACE =  DDRmc1_DDR_MEMORY_MAPPED_SPACE_VAL;
   /* Enable the memory after all configuration is entered in registers */
   *(u32 *)DDRmc1_SDRAM_CFG_ADDR = (DDRmc1_SDRAM_CFG_VAL | SDRAM_CFG_MEM_EN_MASK);

   /* Wait for the memory controller initialization to finish */
   while (*(u32 *)DDRmc1_SDRAM_CFG_2_ADDR & SDRAM_CFG2_D_INIT_MASK)
   {
       i++;
   }
   //do some delay
   for(i =0; i < 0x1000; i++)
   {
	   
   }
}
