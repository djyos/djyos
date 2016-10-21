/*
This is a MODIFIED version of the Dhrystone 2.1 Benchmark program.

The only changes which have been made are:
1) the 'old-style' K&R function declarations have been replaced with
'ANSI-C-style' function declarations (in dhry_1.c and dhry_2,c), and
2) function prototypes have been added (in dhry.h)
3) dhry.h uses CLOCKS_PER_SEC instead of CLK_TCK (to build with -strict)

These changes allow an ANSI-C compiler to produce more efficient code, with
no warnings.
*/

/*
 ****************************************************************************
 *
 *                   "DHRYSTONE" Benchmark Program
 *                   -----------------------------
 *
 *  Version:    C, Version 2.1
 *
 *  File:       dhry_1.c (part 2 of 3)
 *
 *  Date:       May 25, 1988
 *
 *  Author:     Reinhold P. Weicker
 *
 ****************************************************************************
 */
#include <stdio.h>
#include "os.h"
#include "dhry.h"

/* Global Variables: */

Rec_Pointer     Ptr_Glob,
                Next_Ptr_Glob;
Rec_Type        rec,next_rec;        //by lst
int             Int_Glob;
Boolean         Bool_Glob;
char            Ch_1_Glob,
                Ch_2_Glob;
int             Arr_1_Glob [50];
int             Arr_2_Glob [50] [50];

#ifndef REG
        Boolean Reg = false;
#define REG
        /* REG becomes defined as empty */
        /* i.e. no register variables   */
#else
        Boolean Reg = true;
#endif

/* variables for time measurement: */
#define Too_Small_Time 5000     //ºÁÃë
                /* Measurements should last at least 2 seconds */
signed    long long    begin_time,end_time,User_Time;
volatile float  Microseconds,Dhrystones_Per_Second;

ptu32_t DhryTask(void)
{
		One_Fifty       Int_1_Loc;
	REG   One_Fifty       Int_2_Loc;
		One_Fifty       Int_3_Loc;
	REG   char            Ch_Index;
		Enumeration     Enum_Loc;
		Str_30          Str_1_Loc;
		Str_30          Str_2_Loc;
	REG   int             Run_Index;
	REG   int             Number_Of_Runs;
	REG   u64             NumberOfRuns;
	u64   TestTime;

	/* Initializations */

	//  Next_Ptr_Glob = (Rec_Pointer) malloc (sizeof (Rec_Type));   //comment by lst
	Next_Ptr_Glob = &next_rec;     //by lst
	//  Ptr_Glob = (Rec_Pointer) malloc (sizeof (Rec_Type));   //comment by lst
	Ptr_Glob = &rec;     //by lst

	Ptr_Glob->Ptr_Comp                    = Next_Ptr_Glob;
	Ptr_Glob->Discr                       = Ident_1;
	Ptr_Glob->variant.var_1.Enum_Comp     = Ident_3;
	Ptr_Glob->variant.var_1.Int_Comp      = 40;
	strcpy (Ptr_Glob->variant.var_1.Str_Comp,
		  "DHRYSTONE PROGRAM, SOME STRING");
	strcpy (Str_1_Loc, "DHRYSTONE PROGRAM, 1'ST STRING");

	Arr_2_Glob [8][7] = 10;
		/* Was missing in published program. Without this statement,    */
		/* Arr_2_Glob [8][7] would have an undefined value.             */
		/* Warning: With 16-Bit processors and Number_Of_Runs > 32000,  */
		/* overflow may occur for this array element.                   */

	Number_Of_Runs = 100;
	NumberOfRuns = 0;
	TestTime = Too_Small_Time * 1000;//us
	/***************/
	/* Start timer */
	/***************/
	printf("\r\n ****************** DHRYSTONE TEST START ******************\r\n ");

	begin_time = DjyGetSysTime();
	do
	{
	  for (Run_Index = 1; Run_Index <= Number_Of_Runs; ++Run_Index)
	  {

		Proc_5();
		Proc_4();
		  /* Ch_1_Glob == 'A', Ch_2_Glob == 'B', Bool_Glob == true */
		Int_1_Loc = 2;
		Int_2_Loc = 3;
		strcpy (Str_2_Loc, "DHRYSTONE PROGRAM, 2'ND STRING");
		Enum_Loc = Ident_2;
		Bool_Glob = ! Func_2 (Str_1_Loc, Str_2_Loc);
		  /* Bool_Glob == 1 */
		while (Int_1_Loc < Int_2_Loc)  /* loop body executed once */
		{
		  Int_3_Loc = 5 * Int_1_Loc - Int_2_Loc;
			/* Int_3_Loc == 7 */
		  Proc_7 (Int_1_Loc, Int_2_Loc, &Int_3_Loc);
			/* Int_3_Loc == 7 */
		  Int_1_Loc += 1;
		} /* while */
		  /* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
		Proc_8 (Arr_1_Glob, Arr_2_Glob, Int_1_Loc, Int_3_Loc);
		  /* Int_Glob == 5 */
		Proc_1 (Ptr_Glob);
		for (Ch_Index = 'A'; Ch_Index <= Ch_2_Glob; ++Ch_Index)
								 /* loop body executed twice */
		{
		  if (Enum_Loc == Func_1 (Ch_Index, 'C'))
			  /* then, not executed */
			{
			Proc_6 (Ident_1, &Enum_Loc);
			strcpy (Str_2_Loc, "DHRYSTONE PROGRAM, 3'RD STRING");
			Int_2_Loc = Run_Index;
			Int_Glob = Run_Index;
			}
		}
		  /* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
		Int_2_Loc = Int_2_Loc * Int_1_Loc;
		Int_1_Loc = Int_2_Loc / Int_3_Loc;
		Int_2_Loc = 7 * (Int_2_Loc - Int_3_Loc) - Int_1_Loc;
		  /* Int_1_Loc == 1, Int_2_Loc == 13, Int_3_Loc == 7 */
		Proc_2 (&Int_1_Loc);
		  /* Int_1_Loc == 5 */

	  } /* loop "for Run_Index" */

	  /**************/
	  /* Stop timer */
	  /**************/
	  end_time = DjyGetSysTime();
	  User_Time = (end_time - begin_time);
	  NumberOfRuns += Number_Of_Runs;
	}while(User_Time < TestTime);

	Microseconds =  User_Time / NumberOfRuns;
	Dhrystones_Per_Second = ( 1000000 *  NumberOfRuns) / User_Time;

	printf("\r\n Dhrystone Test Result : dhrystones per second = %d \r\n",
			(u32)Dhrystones_Per_Second);

	printf("\r\n ****************** DHRYSTONE TEST END   ******************\r\n ");
	while(1)
	{
		Djy_EventDelay(500*mS);
	}
}

int dhry_main (void)
/*****/

  /* main program, corresponds to procedures        */
  /* Main and Proc_0 in the Ada version             */
{
	u16 dhry_evtt;
    dhry_evtt = Djy_EvttRegist(EN_CORRELATIVE,1 ,0,0,
    							DhryTask,NULL,1024,
                                "Dhrystone task");
    if(dhry_evtt != CN_EVTT_ID_INVALID)
    {
    	Djy_EventPop(dhry_evtt,NULL,0,0,0,0);
    	printf("dhry_main() Regist Evtt Failed !\r\n");
    	return 0;
    }
    return 1;
}

void Proc_1 (REG Rec_Pointer Ptr_Val_Par)
/******************/
    /* executed once */
{
  REG Rec_Pointer Next_Record = Ptr_Val_Par->Ptr_Comp;
                                        /* == Ptr_Glob_Next */
  /* Local variable, initialized with Ptr_Val_Par->Ptr_Comp,    */
  /* corresponds to "rename" in Ada, "with" in Pascal           */

  structassign (*Ptr_Val_Par->Ptr_Comp, *Ptr_Glob);
  Ptr_Val_Par->variant.var_1.Int_Comp = 5;
  Next_Record->variant.var_1.Int_Comp
        = Ptr_Val_Par->variant.var_1.Int_Comp;
  Next_Record->Ptr_Comp = Ptr_Val_Par->Ptr_Comp;
  Proc_3 (&Next_Record->Ptr_Comp);
    /* Ptr_Val_Par->Ptr_Comp->Ptr_Comp
                        == Ptr_Glob->Ptr_Comp */
  if (Next_Record->Discr == Ident_1)
    /* then, executed */
  {
    Next_Record->variant.var_1.Int_Comp = 6;
    Proc_6 (Ptr_Val_Par->variant.var_1.Enum_Comp,
           &Next_Record->variant.var_1.Enum_Comp);
    Next_Record->Ptr_Comp = Ptr_Glob->Ptr_Comp;
    Proc_7 (Next_Record->variant.var_1.Int_Comp, 10,
           &Next_Record->variant.var_1.Int_Comp);
  }
  else /* not executed */
    structassign (*Ptr_Val_Par, *Ptr_Val_Par->Ptr_Comp);
} /* Proc_1 */


void Proc_2 (One_Fifty *Int_Par_Ref)
/******************/
    /* executed once */
    /* *Int_Par_Ref == 1, becomes 4 */
{
  One_Fifty  Int_Loc;
  Enumeration   Enum_Loc;

  Int_Loc = *Int_Par_Ref + 10;
  do /* executed once */
    if (Ch_1_Glob == 'A')
      /* then, executed */
    {
      Int_Loc -= 1;
      *Int_Par_Ref = Int_Loc - Int_Glob;
      Enum_Loc = Ident_1;
    } /* if */
  while (Enum_Loc != Ident_1); /* true */
} /* Proc_2 */


void Proc_3 (Rec_Pointer *Ptr_Ref_Par)
/******************/
    /* executed once */
    /* Ptr_Ref_Par becomes Ptr_Glob */
{
  if (Ptr_Glob != Null)
    /* then, executed */
    *Ptr_Ref_Par = Ptr_Glob->Ptr_Comp;
  Proc_7 (10, Int_Glob, &Ptr_Glob->variant.var_1.Int_Comp);
} /* Proc_3 */


void Proc_4 (void) /* without parameters */
/*******/
    /* executed once */
{
  Boolean Bool_Loc;

  Bool_Loc = Ch_1_Glob == 'A';
  Bool_Glob = Bool_Loc | Bool_Glob;
  Ch_2_Glob = 'B';
} /* Proc_4 */


void Proc_5 (void) /* without parameters */
/*******/
    /* executed once */
{
  Ch_1_Glob = 'A';
  Bool_Glob = false;
} /* Proc_5 */


        /* Procedure for the assignment of structures,          */
        /* if the C compiler doesn't support this feature       */
#ifdef  NOSTRUCTASSIGN
memcpy (d, s, l)
register char   *d;
register char   *s;
register int    l;
{
        while (l--) *d++ = *s++;
}
#endif


