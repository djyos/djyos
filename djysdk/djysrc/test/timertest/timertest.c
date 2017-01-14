#include "stdint.h"
#include "stddef.h"
#include "stdio.h"

#include "os.h"

#include "timer.h"
#include "timer_hard.h"


void timerisr(struct Timer *timer)
{
    printk("Timername = %s\n\r", Timer_GetName(timer));
}


void timertest(void)
{
    struct Timer *timer;
    timer = Timer_Create("Timer1",1000*mS,timerisr);
    if(NULL != timer)
    {
        Timer_Ctrl(timer,EN_TIMER_STARTCOUNT,0);
    }

    timer = Timer_Create("Timer2",2000*mS,timerisr);
    if(NULL != timer)
    {
        Timer_Ctrl(timer,EN_TIMER_STARTCOUNT,0);
    }
    timer = Timer_Create("Timer3",3000*mS,timerisr);
    if(NULL != timer)
    {
        Timer_Ctrl(timer,EN_TIMER_STARTCOUNT,0);
    }
}
