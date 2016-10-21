//-----------------------------------------------------------------------------
// \file    test_ram.c
// \brief   implementation of DDR ram test.
//
//-----------------------------------------------------------------------------
#include "stdio.h"


#include "test_ram.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static Variable Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------
uint32_t verifyAddrDataBus(uint32_t in_begin_addr);
uint32_t verifyPattern(uint32_t in_begin_addr, uint32_t in_num_bytes);

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   tests the external ddr. this test assumes the code is not being
//          executed from external ddr, so makes not attempt to maintain data.
//
// \param   none.
//
// \return  uint32_t
//-----------------------------------------------------------------------------
uint32_t TEST_ram(void)
{
   uint32_t rtn = 0;
#if 0
   printf("------------------------------------------------------------\r\n");
   printf("                     DDR RAM Test\r\n\r\n");

   printf("Test Description\r\n");
   printf("----------------\r\n");
   printf("this code verifies the mDDR address bus and writes/verifies \r\n");
   printf("data patterns to the mDDR. \r\n");
   printf("------------------------------------------------------------\r\n\r\n");

   //-------------------------------------
   // initialize the required bsl modules.
   //-------------------------------------
   // nothing to init for this test. ram was taken care of in gel or main.
   
   //--------------
   // execute test.
   //--------------
   printf("\r\nExecute Test\r\n");
   printf("------------\r\n\r\n");
   
   printf("--- Test address and data bus ---\r\n");
#endif
   printf("--- Test address/data bus pattern ---\r\n");
   rtn = verifyAddrDataBus(DDR_MEM_BASE);
   if (rtn != ERR_NO_ERROR)
   {
      return (rtn);
   }
   else
   {
      printf("\taddress/data bus test passed\r\n");
   }

   printf("--- Test data pattern ---\r\n");
   rtn = verifyPattern(DDR_MEM_BASE, DDR_MEM_SIZE / 2);
   if (rtn != ERR_NO_ERROR)
   {
      return (rtn);
   }
   else
   {
      printf("\tdata pattern test passed\r\n");
   }

   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// tests the address and data lines.
//-----------------------------------------------------------------------------
uint32_t verifyAddrDataBus(uint32_t in_begin_addr)
{
   uint32_t offset;
   uint32_t test_data = 0;
   uint32_t *test_addr = (uint32_t *)in_begin_addr;

   for (offset = 1; offset <= 0x8000; offset <<= 1)
   {
      // write test data to the test address.
      *test_addr = test_data;
      *(test_addr + offset) = ~test_data;

      // verify the data is correct.
      if ((*test_addr != test_data) ||
         (*(test_addr + offset) != ~test_data))
      {
         printf("addr/data bus test failed at address: %08X\r\n", test_addr);
         printf("test_data: %08X\r\n", test_data);
		 printf("*test_addr: %08X\r\n", *test_addr);
         return (ERR_FAIL);
      }
      
      // bump test data for next loop.
      test_data++;
   }

   return (ERR_NO_ERROR);
}

uint32_t verifyPattern(uint32_t in_begin_addr, uint32_t in_num_bytes)
{
   uint32_t rtn = ERR_NO_ERROR;
   uint32_t offset;
   uint32_t *test_addr = (uint32_t *)in_begin_addr;
   
   // write ram under test to all 5's.
   for (offset = 0; offset < in_num_bytes; offset += sizeof(uint32_t))
   {
      *test_addr++ = 0x55555555;
   }

   // verify ram under test is 5's.
   test_addr = (uint32_t *)in_begin_addr;
   for (offset = 0; offset < in_num_bytes; offset += sizeof(uint32_t))
   {
      if (*test_addr++ != 0x55555555)
      {
         printf("data pattern (5) test failed at address: %08X\r\n", test_addr);
         rtn = ERR_FAIL;
      }
   }
   
   // write ram under test to all A's.
   test_addr = (uint32_t *)in_begin_addr;
   for (offset = 0; offset < in_num_bytes; offset += sizeof(uint32_t))
   {
      *test_addr++ = 0xAAAAAAAA;
   }

   // verify ram under test is A's.
   test_addr = (uint32_t *)in_begin_addr;
   for (offset = 0; offset < in_num_bytes; offset += sizeof(uint32_t))
   {
      if (*test_addr++ != 0xAAAAAAAA)
      {
         printf("data pattern (A) test failed at address: %08X\r\n", test_addr);
         rtn = ERR_FAIL;
      }
   }

   return (rtn);
}
