
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#define CN_SECTOR_SIZE  0x1000    //4KB
#define CN_BLOCK_SIZE   0X10000   //64KB
#define CN_POLL_TIMEOUT 0x2000000

#define SETUP_ERASE		0x80
#define SETUP_WRITE		0xa0
#define READ_ID 		0x90
#define SECTOR_ERASE    0x50
#define BLOCK_ERASE		0x30		/*块擦除*/
#define READ_ARRAY		0xf0        /*软件ID读模式退出*/
#define UNLOCK_1		0xaa        /*解锁写入值*/
#define	UNLOCK_2		0x55
#define UNLOCK_ADDR1	0x5555		/*解锁偏移地址[字]*/
#define	UNLOCK_ADDR2	0x2aaa

u32    sst39DrvIdentify(u32 baseaddr);
bool_t sst39DrvErase(u32 baseaddr, u32 offset, u32 len);
bool_t sst39DrvWrite(u32 baseaddr,u32 offset, void *buffer, int length);
/*------------------------------------------------------------------------
 Procedure:     sst39DrvOverDetect ID:1
 Purpose:       探测write,erase操作是否结束,超时错误
 Input:
 Output:
 Errors:
------------------------------------------------------------------------*/
int  sst39DrvOverDetect(void * ptr,  int timeCounter)
{
	volatile u16 *pFlash = ptr;
	u16 buf1,buf2;

	buf1 = *pFlash & 0x40;
	while(1)
	{
		    buf2  = *pFlash & 0x40;
		    if(buf1 == buf2)
				break;
		    else
			    buf1 = buf2;
		    if(timeCounter-- <= 0)
		    {
				return -1;
		    }
	}

	return 0;
}



/*------------------------------------------------------------------------
 Procedure:     sst39MTDWrite ID:1
 Purpose:       MTD写Flash函数
 Input:
 Output:
 Errors:
------------------------------------------------------------------------*/

bool_t sst39DrvWrite(u32 baseaddr,u32 offset, void *buffer, int length)
{
	volatile int cLength;
	volatile u16* flashPtr;
	volatile u16* pTemp;
	volatile u16 *gBuffer;

	flashPtr = (u16 *)(baseaddr + offset);
	cLength = length/2;
	gBuffer = (u16 *)buffer;
    pTemp=(u16 *)baseaddr;

	while (cLength >= 1)
	{
		pTemp[UNLOCK_ADDR1]=UNLOCK_1;
		pTemp[UNLOCK_ADDR2]=UNLOCK_2;
		pTemp[UNLOCK_ADDR1]=SETUP_WRITE;
		*flashPtr = *gBuffer;

		if(sst39DrvOverDetect((void *)flashPtr, CN_POLL_TIMEOUT))
		{
			return false;
		}

		if(*flashPtr != *gBuffer)
		{
			*flashPtr = READ_ARRAY;
			return false;
		}
		cLength--;
		flashPtr++;
		gBuffer++;
	}

	return true;
}

bool_t sst39DrvErase(u32 baseaddr, u32 offset, u32 len)
{
	volatile u16* flashPtr;
	volatile u16 *pTemp;
	u32 elen;

	pTemp=(u16 *)baseaddr;
	elen = 0;
	while (elen < len)
	{
		flashPtr =(u16 *)(baseaddr+ offset + elen);

		sst39DrvOverDetect((void *)flashPtr, CN_POLL_TIMEOUT);
		pTemp[UNLOCK_ADDR1] = UNLOCK_1;
		pTemp[UNLOCK_ADDR2] = UNLOCK_2;
		pTemp[UNLOCK_ADDR1] = SETUP_ERASE;
		pTemp[UNLOCK_ADDR1] = UNLOCK_1;
		pTemp[UNLOCK_ADDR2] = UNLOCK_2;
		*flashPtr = SECTOR_ERASE;
		if(sst39DrvOverDetect((void *)flashPtr, CN_POLL_TIMEOUT))
		{
			return false;
		}
		elen +=CN_SECTOR_SIZE;
	}

	return true;
}

/*------------------------------------------------------------------------
 Procedure:     fllv160Identify ID:1
 Purpose:       MTD读Flash标识
 Input:
 Output:
 Errors:
------------------------------------------------------------------------*/
u32 sst39DrvIdentify(u32 baseaddr)
{
	volatile u16* pTemp;
	int  i,type;

	pTemp=(u16 *)baseaddr;
    pTemp[UNLOCK_ADDR1]=(UNLOCK_1);
    pTemp[UNLOCK_ADDR2]=(UNLOCK_2);
    pTemp[UNLOCK_ADDR1]=(READ_ID);

	type = pTemp[1];

	pTemp[0] = READ_ARRAY;    /*回到读状态*/

	for(i=0;i<10;i++);

	switch(type)
	{
		case 0x2782:          //160
			break;
		case 0x235b:          //3201
			break;
		case 0x234b:          //160
			break;
	    case 0x236d:          //6401
			break;
		default:
			break;
	}
  	return type;
}



