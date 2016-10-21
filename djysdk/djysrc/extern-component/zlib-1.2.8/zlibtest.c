/*
 * zlibtest.c
 *
 *  Created on: 2015年4月22日
 *      Author: zhangqf
 */
#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#include "os.h"

#include "zlib.h"

#define CN_TEST_LEN   0x1000               //4KB

void ZlibCmpandUncmpTestMain(void)
{
	int result;
	char *src;
	char *tmp1;
	char *tmp2;
	char *dst;
	char *uncmp;
	u32  dstlen;
	u32  srclen;
	u32  uncmplen;
	u32 i;          //循环临时变量

	src = malloc(CN_TEST_LEN);

	if(NULL == src)
	{
		printk("%s:No mem for src\n\r",__FUNCTION__);
		return;
	}

	dstlen = compressBound(CN_TEST_LEN);
	dst = malloc(dstlen);

	if(NULL == dst)
	{
		printk("%s:No mem for dst\n\r",__FUNCTION__);
		free(src);
		return;
	}

	//ok,now initialize the src
	srclen = CN_TEST_LEN;
	tmp1 = src;
	for(i =0; i< srclen ; i++)
	{
		*(u8 *)(tmp1+i) = i%256;
	}
    printk("%s:src = 0x%08x\n\r", __FUNCTION__,(u32)src);
	//ok , now compress the src data
	if((result = compress(dst,&dstlen, src, srclen))==Z_OK)
	{
		printk("%s:Compress success!--srclen = %d dstlen = %d\n\r",\
				__FUNCTION__,srclen,dstlen);
		//then uncompress the data to compare weather right
		uncmplen = CN_TEST_LEN*2;
		uncmp = malloc(uncmplen);
		if(NULL != uncmp)
		{
			printk("%s:src = 0x%08x dst = 0x%08x  uncmp =0x%08x--END\n\r",\
				   __FUNCTION__,(u32)src, (u32)dst,(u32)uncmp);
			if((result=uncompress(uncmp,&uncmplen,dst, dstlen))==Z_OK)
			{
				printk("%s:Uncompress success!--srclen = %d dstlen = %d\n\r",\
						__FUNCTION__,dstlen,uncmplen);
				//then cmp the source and uncompressed result,check if any error

				//the src mem has been changed, maybe
				tmp1 = src;
				tmp2 = uncmp;
				for(i =0; i <CN_TEST_LEN; i++)
				{
					if(*tmp1 != *tmp2)
					{
						Djy_EventDelay(1000*mS);
						printk("%s:error: %d: src = %02d, uncmp = %02d\n\r",\
								__FUNCTION__,i,*(u8 *)tmp1, *(u8*)tmp2);
					}
					tmp1++;
					tmp2++;
				}
			}
			else
			{
				printk("%s:Unompress the data failed--code = %d\n\r",__FUNCTION__,result);
			}
			free(uncmp);
		}
		else
		{
			printk("%s:No mem for uncmp\n\r");
		}
	}
	else
	{
		printk("%s:Compress the data failed--code = %d\n\r",__FUNCTION__,result);
	}
	printk("%s:TEST END\n\r",__FUNCTION__);
	free(src);
	free(dst);

	while(1)
	{
		Djy_EventDelay(1000*mS);
	}
	return;
}


void ZlibCmpandUncmpTest(void)
{
	u16 evtt;
	evtt = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,\
					      ZlibCmpandUncmpTestMain,NULL,0x1000,\
                          "ZlibCmpandUncmpTestMain");
    Djy_EventPop(evtt,NULL,0,NULL,0,0);
}


