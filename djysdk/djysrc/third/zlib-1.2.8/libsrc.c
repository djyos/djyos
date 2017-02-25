/*
 * libsrc.c
 *
 *  Created on: 2015Äê4ÔÂ22ÈÕ
 *      Author: zhangqf
 */
#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"


void * calloc(size_t n, size_t size)
{
	void *result;

	result = malloc(n*size);
	if(NULL != result)
	{
		memset(result,0,n*size);
	}

	return result;
}
