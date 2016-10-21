/*
 * setjmp.h
 *
 *  Created on: 2015Äê9ÔÂ6ÈÕ
 *      Author: zqf
 */

#ifndef SETJMP_H_
#define SETJMP_H_

typedef struct
{
	volatile unsigned int reg1;
}tagJmpBuf;

typedef tagJmpBuf*   jmp_buf;

int setjmp(jmp_buf env);
void longjmp(jmp_buf env,int val);

#endif /* SETJMP_H_ */
