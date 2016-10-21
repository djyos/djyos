

typedef struct{
    unsigned long *laddr;       //rom address
    unsigned long *raddr;       //mem address
    unsigned long size;        //long
}tagCpyRecord;
extern tagCpyRecord tgLoaderRecord;

void LoadLoader2Mem(void)
{
	unsigned long i, j ,k;
	unsigned long  *src;
	unsigned long  *dst;
	
	src = tgLoaderRecord.laddr;
	dst = tgLoaderRecord.raddr;
	j = tgLoaderRecord.size/4;
	if(src != dst)
	{
		for(i =0; i < j; i++)
		{
			k = *src;
			*dst = k;
			src++;
			dst++;
		}
	}
}
