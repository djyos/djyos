/*=========================================================*
 * 文件模块说明: 将系统的符号表加载到内存的3.5M-4M的地址空间
  *                                :这部分代码参考symShow.c文件
 * 文件版本: v1.0.0                             *
 * 开发人员:  xp                            *
 * 创建时间:  2012-01-15                            *
 * Copyright(c)  Shenzhen NARI Limited Co.        *
 *****************================================================*
 * 程序修改记录(最新的放在最前面):                          *
 * <版本号><修改日期>, <修改人员>: <修改功能概述>           *
 *==========================================================*/

#include "vxWorks.h"
#include "string.h"
#include "stdio.h"
#include "a_out.h"
#include "sysSymTbl.h"
#include "symLib.h"
#include "errno.h"
#include "moduleLib.h"
#include "fioLib.h"
#include "types.h"
#include "private/cplusLibP.h"
#include "iolib.h"

#define WR_SYMBOL_FILE 

LOCAL char *typeName [] =		/* system symbol table types */
    {
    "????",
    "abs",
    "text",
    "data",
    "bss",
    };

#ifdef  WR_SYMBOL_FILE 
 int symFd=-1;
#endif

INT8 * pSymTblWr;
LOCAL char *strMatch
    (
    FAST char *str1,		/* where to look for match */
    FAST char *str2		/* string to find a match for */
    )
{
    FAST int str2Length = strlen (str2);
    FAST int ntries	= strlen (str1) - str2Length;

    for (; ntries >= 0; str1++, --ntries)
	{
	if (strncmp (str1, str2, str2Length) == 0)
	    return (str1);	/* we've found a match */
	}

    return (NULL);
  }

LOCAL BOOL symTblPrint
    (
    char *	name,
    int		val,
    INT8	type,
    char *	substr,
    UINT16      group
    )
    {
    char 	moduleName [NAME_MAX];
    MODULE_ID	moduleId;
    char	demangled [MAX_SYS_SYM_LEN + 1];
    char *	nameToPrint;
    char	quitChar;		/* q to quit displaying symbols */
    char   buf[150];
    /*
     * If group is the system group default, don't print anything.  If
     * it's not the default, try to get a corresponding module name.
     * If you can't find a module, just print the group number.
     */

    if (group == symGroupDefault)
        moduleName [0] = EOS;
    else if ((moduleId = moduleFindByGroup (group)) != NULL)
        sprintf (moduleName, "(%s)", moduleId->name);
    else
        sprintf (moduleName, "(%d)", group);
    
    if (substr == NULL || strMatch (name, substr) != NULL)
    {
	nameToPrint = cplusDemangle (name, demangled, MAX_SYS_SYM_LEN + 1);
	{

	       sprintf(buf,"%-25s 0x%08x %-8s %s \n", nameToPrint, val,
		typeName [(type >> 1) & 7], moduleName);
		/*不需要模块名的时候使用下面的拼接方式
	       sprintf(buf,"%-25s 0x%08x %-8s\n", nameToPrint, val,
		typeName [(type >> 1) & 7]);*/
#ifndef WR_SYMBOL_FILE
		memcpy(pSymTblWr,buf,strlen(buf));
		pSymTblWr+=strlen(buf);
#else
           if(symFd)
             write(symFd,buf,strlen(buf));

#endif
 

	}
       }

//	if ((type & N_EXT) == 0)
	//    printf (" (local)");
    return (TRUE);
}

/*write symbol table to file or memory 0xc0380000~0xc0400000 space*/
STATUS symTblWr( )
    {
    SYMTAB *	pSymTbl=sysSymTbl;
    char *	substr="";
    if (OBJ_VERIFY (pSymTbl, symTblClassId) != OK)
	return (ERROR);				/* invalid symbol table ID */
#ifdef WR_SYMBOL_FILE
    symFd=open("/ram/symTbl",O_CREAT|O_RDWR,0664);
    if(symFd<0)
    {
        printf("open file failed\n");
	 return;
    }
#endif
    if (substr == NULL)
	{
	printf ("%-20s: %-10d\n", "Number of Symbols", pSymTbl->nsymbols);
	printf ("%-20s: 0x%-10x\n", "Symbol Mutex Id", &pSymTbl->symMutex);
	printf ("%-20s: 0x%-10x\n", "Symbol Hash Id", pSymTbl->nameHashId);
	printf ("%-20s: 0x%-10x\n", "Symbol memPartId", pSymTbl->symPartId);
	printf ("%-20s: %-10s\n", "Name Clash Policy", 
		(pSymTbl->sameNameOk) ? "Allowed" : "Disallowed");
	}
    else
	{
	if (pSymTbl == sysSymTbl)
	    {
#ifndef WR_SYMBOL_FILE
          pSymTblWr=(char *)0xc0380000;
#endif
	    symEach (pSymTbl, (FUNCPTR) symTblPrint, (int) substr);
	    }
	else
	   ;//symEach (pSymTbl, (FUNCPTR) symPrint, (int) substr);
	}
#ifdef WR_SYMBOL_FILE

    if(symFd)
    {
       close(symFd);
	 symFd=-1;
    }
#endif
    return (OK);
    }
