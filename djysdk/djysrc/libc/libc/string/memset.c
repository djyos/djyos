/*
FUNCTION
	<<memset>>---set an area of memory

INDEX
	memset

ANSI_SYNOPSIS
	#include <string.h>
	void *memset(void *<[dst]>, int <[c]>, size_t <[length]>);

TRAD_SYNOPSIS
	#include <string.h>
	void *memset(<[dst]>, <[c]>, <[length]>)
	void *<[dst]>;
	int <[c]>;
	size_t <[length]>;

DESCRIPTION
	This function converts the argument <[c]> into an unsigned
	char and fills the first <[length]> characters of the array
	pointed to by <[dst]> to the value.

RETURNS
	<<memset>> returns the value of <[dst]>.

PORTABILITY
<<memset>> is ANSI C.

    <<memset>> requires no supporting OS subroutines.

QUICKREF
	memset ansi pure
*/

#include <string.h>
#include "local.h"


#define LBLOCKSIZE (sizeof(long))
#define UNALIGNED(X)   ((long)X & (LBLOCKSIZE - 1))
#define TOO_SMALL(LEN) ((LEN) < LBLOCKSIZE)
#define UNALIGNED_64(X) ((u64)X&7)
#define UNALIGNED_32(X) ((u32)X&3)

_PTR
__inhibit_loop_to_libcall
_DEFUN (memset, (m, c, n),
	_PTR m _AND
	int c _AND
	size_t n)
{
  char *s = (char *) m;

#if !defined(PREFER_SIZE_OVER_SPEED) && !defined(__OPTIMIZE_SIZE__)
  int i;
  unsigned long buffer;
  unsigned long *aligned_addr;
  unsigned int d = c & 0xff;	/* To avoid sign extension, copy C to an
				   unsigned variable.  */

  while (UNALIGNED (s))
    {
      if (n--)
        *s++ = (char) c;
      else
        return m;
    }

  if (!TOO_SMALL (n))
    {
      /* If we get this far, we know that n is large and s is word-aligned. */
      aligned_addr = (unsigned long *) s;

      /* Store D into each char sized location in BUFFER so that
         we can set large blocks quickly.  */
      buffer = (d << 8) | d;
      buffer |= (buffer << 16);
      for (i = 32; i < (s32)LBLOCKSIZE * 8; i <<= 1)
        buffer = (buffer << i) | buffer;

      /* Unroll the loop.  */
      while (n >= LBLOCKSIZE*4)
        {
          *aligned_addr++ = buffer;
          *aligned_addr++ = buffer;
          *aligned_addr++ = buffer;
          *aligned_addr++ = buffer;
          n -= 4*LBLOCKSIZE;
        }

      while (n >= LBLOCKSIZE)
        {
          *aligned_addr++ = buffer;
          n -= LBLOCKSIZE;
        }
      /* Pick up the remainder with a bytewise loop.  */
      s = (char*)aligned_addr;
    }

#endif /* not PREFER_SIZE_OVER_SPEED */

  while (n--)
    *s++ = (char) c;

  return m;
}


u16 Exchange16(u16 m)
{
	u8 i=0;
	u16 n=m;
	u16 k,j=0x0000;
	unsigned char temp[2];
	for(i=0;i<=1;i++)
	{
		temp[i]=(unsigned char)n;
		n=n>>8;
		k=temp[i];
		j|=k<<(8*(1-i));
	}
	return j;
}

_PTR
__inhibit_loop_to_libcall
_DEFUN (memset16, (m, c, n),
	_PTR m _AND
	u16 c _AND
	size_t n)
{
  char *s = (char *) m;
  u8 i;
  char ByteOrderFlag=0;
  char temp[2];
  u16 d = c & 0xffff;	/* To avoid sign extension, copy C to an
  				   unsigned variable.  */
  u16 b=d;
  if(CN_CFG_BYTE_ORDER==CN_CFG_BIG_ENDIAN)    //大端系统
  	  ByteOrderFlag=1;
  if(ByteOrderFlag==1)   //如果是小端系统，将16bit数据高8位和低8位数据互换位置。
    {
     	b=Exchange16(d);
     	d=b;
    }
   for(i=0;i<=1;i++)
   {
     temp[i]=(char)b;
     b=b>>8;
   }
    b=d;
#if !defined(PREFER_SIZE_OVER_SPEED) && !defined(__OPTIMIZE_SIZE__)
  u32 buffer;
  u32 *aligned_addr;
  u16 e=0x0000;
  char count=0;
  //判断起始地址是否为4字节对齐，如果不对齐先填不对齐的地址直至地址4字节对齐。 在这里
  //填写的字节数可能为0~3.
  while (UNALIGNED_32(s))
    {
	  if(count==0)
	  {
		  if(n)
		 {
			*s++=temp[1];
			count=1;
			n--;
		 }
		  else
			  return m;
	  }
	  else
	  {
            *s++=temp[0];
			 count=0;
	  }
    }

  if(count==1)
  {
	  e=temp[0];
	  e=e<<8;
	  e|=temp[1];

	   b=e;    //改变字节序后数据
       d=b;
   }

  if (n>=2)
    {
      /* If we get this far, we know that n is large and s is word-aligned. */
      aligned_addr = (u32*) s;
      /* Store D into each char sized location in BUFFER so that
         we can set large blocks quickly.  */
      buffer = (b << 16) | d;
      b=d;
     do
     {
    	 *aligned_addr++=buffer;
    	  n=n-2;
     }while(n>1);
    }

      /* Pick up the remainder with a bytewise loop.  */
      s = (char*)aligned_addr;
      if(n==1)
      {
    	  *s++=(char)(b>>8);
    	   b=d;
    	  *s++=(char)b;
      }
      else
      {
    	  if(count==1)
    		  *s++=temp[0];
      }

return m;
#endif /* not PREFER_SIZE_OVER_SPEED */
while(n--)
 {
	 for(i=0;i<=1;i++)
	  {
	     *s++=(char)temp[1-i];
	  }
 }

  return m;
}


u32 Exchange32(u32 m)
{
	u8 i=0;
	u32 n=m;
	u32 k,j=0x00000000;
	unsigned char temp[4];
	for(i=0;i<=3;i++)
	{
		temp[i]=(unsigned char)n;
		n=n>>8;
		k=temp[i];
		j|=k<<(8*(3-i));
	}
	return j;
}


_PTR
__inhibit_loop_to_libcall
_DEFUN (memset32, (m, c, n),
	_PTR m _AND
	u32 c _AND
	size_t n)
{
  char *s = (char *) m;
  char ByteOrderFlag=0;
  u8 i=0;
  char temp[4];
  u32 d = c & 0xffffffff;	/* To avoid sign extension, copy C to an
  				             unsigned variable.  */
  u32 b=d;
  if(CN_CFG_BYTE_ORDER==CN_CFG_BIG_ENDIAN)
  	  ByteOrderFlag=1;
  if(ByteOrderFlag==1)
  	{
  		  b=Exchange32(d);
  		  d=b;
  	}
  	for(i=0;i<=3;i++)
  	{
  		temp[i]=(char)b;
  		b=b>>8;
  	}
    b=d;

#if !defined(PREFER_SIZE_OVER_SPEED) && !defined(__OPTIMIZE_SIZE__)
  u32 buffer;
  u32 *aligned_addr;
  u32 e=0x00000000;
  u8 j;
  char count=0;
  if(UNALIGNED_32(s))
   {
      do
        {
          	*s++=(char)(b>>8*(3-count));
          	count++;
          	b=d;
         }while(UNALIGNED(s));
    }
  if(count!=0)
	  n--;

  if (n>=1)
    {
      /* If we get this far, we know that n is large and s is word-aligned. */
      aligned_addr = (u32 *) s;
      for(i=0,j=count;i<count;i++,j--)
      	{
      	   e|=temp[3-i];
      	   if(j!=1)
      	   {
      	      e=e<<8;
      	   }
      	}
      buffer=(b<<(8*count))|e;
      do
      {
    	 *aligned_addr++=buffer;
    	 n--;
      }while(n);
      /* Pick up the remainder with a bytewise loop.  */
      s = (char*)aligned_addr;
  }
      for(i=0;i<count;i++)
      {
      	*s++=temp[3-i];
      }
      return m;

#endif /* not PREFER_SIZE_OVER_SPEED */
 while(n--)
 {
	 for(i=0;i<=3;i++)
	  {
	     *s++=(char)temp[3-i];
	 }
 }

  return m;
}


u64 Exchange64(u64 m)
{
	u8 i=0;
	u64 n=m;
	u64 k,j=0x0000000000000000;
	unsigned char temp[8];
	for(i=0;i<=7;i++)
	{
		temp[i]=(unsigned char)n;
		n=n>>8;
		k=temp[i];
		j|=k<<(8*(7-i));
	}
	return j;
}


_PTR
__inhibit_loop_to_libcall
_DEFUN (memset64, (m, c, n),
	_PTR m _AND
	u64 c _AND
	size_t n)
{
	u8 i=0;
	char ByteOrderFlag=0;
    char temp[8];
    u64 d = c & 0xffffffffffffffff;	/* To avoid sign extension, copy C to an
   					   unsigned variable.  */
    u64 b=d;
	char *s = (char *) m;
	if(CN_CFG_BYTE_ORDER==CN_CFG_BIG_ENDIAN)
		  	  ByteOrderFlag=1;
	if(ByteOrderFlag==1)
	{
		  b=Exchange64(d);
		  d=b;
	}
	for(i=0;i<8;i++)
	{
		temp[i]=(char)b;
		b=b>>8;
	}
	  b=d;
	#if !defined(PREFER_SIZE_OVER_SPEED) && !defined(__OPTIMIZE_SIZE__)
	  u8 j;
	  u64 buffer;
	  u64 *aligned_addr;
	  u64 e=0x0000000000000000;
	  char count=0;
        if(UNALIGNED_64(s))
        {
        	do
        	{
        	   *s++=(char)(b>>8*(7-count));
        		count++;
        		b=d;
        	}while(UNALIGNED_64(s));
        }
     if(count!=0)
	   n--;
	  if (n>=1)
	    {
	      /* If we get this far, we know that n is large and s is word-aligned. */
	      aligned_addr = (u64 *) s;
	      /* Store D into each char sized location in BUFFER so that
	         we can set large blocks quickly.  */
	    for(i=0,j=count;i<count;i++,j--)
	    {
	    	e|=temp[7-i];
	    	if(j!=1)
	    	{
	    		 e=e<<8;
	    	}
	    }
	    buffer=(b<<(8*count))|e;
	    do
	    {
	    	*aligned_addr++=buffer;
	    	n--;
	    }while(n);
	      /* Pick up the remainder with a bytewise loop.  */
	      s = (char*)aligned_addr;
	  }
	  for(i=0;i<count;i++)
	  {
		  *s++=temp[7-i];
	  }
	  return m;
#endif /* not PREFER_SIZE_OVER_SPEED */
    while(n--)
    {
    	for(i=0;i<=7;i++)
    	{
    		*s++=(char)temp[7-i];
    	}
    }
	  return m;
}

