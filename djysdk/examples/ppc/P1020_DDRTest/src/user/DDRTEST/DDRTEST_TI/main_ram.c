//-----------------------------------------------------------------------------
// \file    main.c
// \brief   implementation of main() to test bsl drivers.
//
//-----------------------------------------------------------------------------
#include "stdint.h"
#include "stddef.h"
#include "string.h"
#include "stdio.h"

#include "test_ram.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------
// uncomment this define if running without gel initialization.
// #define NO_GEL    (1)

//-----------------------------------------------------------------------------
// Static Variable Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   entry point for bsl test code.
//
// \param   none.
//
// \return  none.
//-----------------------------------------------------------------------------
bool_t DDRTEST_TI(char *param)
{
   uint32_t results = 0;
   uint32_t i;
   char *word,*next_param;
   u32  testtimes;
   char decision;
       
   printk("\n\r");
   word = Sh_GetWord(param,&next_param);
   testtimes = strtol(word, (char **)NULL, 0);
   
   printk("%s:TestTimesRequired:%d\n\r",__FUNCTION__,testtimes);
//   printk("%s:Press Y/y to Test and others Cancel\n\r",__FUNCTION__);
//   decision = getchar();
//   if((decision == 'Y')||(decision == 'y'))
//   {}
//   else
//   {
//	   printk("%s:Receive the order to cancel\n\r",__FUNCTION__);
//   }

   // TEST RAM
   for(i =0; i <testtimes;i++)
   {
	   results = TEST_ram();
	   if (results != ERR_NO_ERROR)
	   {
		   printf("%s:Times:%d: DDR TEST FAILED **********\n\r",__FUNCTION__,i);
		   break;
	   }
	   else
	   {
		   printf("%s:Times:%d: DDR TEST SUCCESS**********\n\r",__FUNCTION__,i);
	   }
	   printf("\n\r");
   }


   printk("%s:Operation End\n\r",__FUNCTION__);

   return true;
}
