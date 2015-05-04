#include "stdint.h"
#include "stddef.h"
#include "stdio.h"

#include "os.h"

#include "timer.h"


void timerisr(tagTimerSoft *timer)
{
	printk("Timername = %s\n\r", timer->name);
}


void timertest(void)
{
	tagTimerSoft *timer;
	timer = TimerSoft_Create("Timer1",1000*mS,timerisr);
	if(NULL != timer)
	{
		TimerSoft_Ctrl(timer,EN_TIMER_STARTCOUNT,0);
	}

	timer = TimerSoft_Create("Timer2",2000*mS,timerisr);
	if(NULL != timer)
	{
		TimerSoft_Ctrl(timer,EN_TIMER_STARTCOUNT,0);
	}
	timer = TimerSoft_Create("Timer3",3000*mS,timerisr);
	if(NULL != timer)
	{
		TimerSoft_Ctrl(timer,EN_TIMER_STARTCOUNT,0);
	}
}
