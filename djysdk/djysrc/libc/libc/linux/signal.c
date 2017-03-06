/*
 * signal.c
 *
 *  Created on: 2015Äê9ÔÂ6ÈÕ
 *      Author: zqf
 */


#include <signal.h>


sighandler_t signal(int signum,sighandler_t handler)
{
	return SIG_IGN;
}

void alarm(int signum)
{
	return;
}
