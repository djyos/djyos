
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>


int __UpdateMode(char *servip,char *filename, void * hook)
{
    return 0;
}

u32  NULL_flashId(u32 baseaddr)
{
    return 0;
}
bool_t  NULL_flashErase(u32 baseaddr,u32 offset, u32 len)
{
    return false;
}
bool_t  NULL_flashWrite(u32 baseaddr,u32 offset, void *buf, u32 buflen)
{
    return false;
}

typedef u32    (*fnFlashID)(u32 baseaddr);
typedef bool_t (*fnFlashErase)(u32 baseaddr,u32 offset,u32 len);
typedef bool_t (*fnFlashWrite)(u32 baseaddr,u32 offset, void *buf, u32 buflen);
typedef struct
{
    fnFlashErase  flasherase;
    fnFlashID     flashid;
    fnFlashWrite  flashwrite;
}tagOsUpdateMedia;
static tagOsUpdateMedia gOsupdateMeida={
    .flashid = NULL_flashId,
    .flasherase = NULL_flashErase,
    .flashwrite = NULL_flashWrite,
};


#define CN_PARAM_NUMMAX   4
static u8     *bufDownload = NULL;
static u32     bufLen = 0;
static u32     bufDatalen = 0;

#include "osupdateconfig.h"
//this function is used to getthe flash ID
//useage:download +server+filename +length
bool_t __osflashID(char *param)
{
    u32    id;

    id = gOsupdateMeida.flashid(CN_OSFLASH_BASEADDR);
    printf("%s:NORID:0X%08X\r\n",__FUNCTION__,id);

    return true;

}
//this function is used to erase the nor;
//usage: erase + address + len
static bool_t  __osflasherase(char *param)
{
    u32 baseaddr;
    u32 offset;
    u32 len;

    baseaddr = CN_OSFLASH_BASEADDR;
    offset = CN_OSSPACE_OFFSET;
    len = CN_OSSPACE_SIZE;

    printf("%s:base:0x%08x off:0x%08x len:0x%08x\r\n",__FUNCTION__,baseaddr,offset,len);

    return gOsupdateMeida.flasherase(baseaddr,offset,len);
}

//this function is used to write the nor;
//which will write the download data to the flash
//usage: updateos
static bool_t  __oswriteflash(char *param)
{
    u32    baseaddress;
    u32    offset;
    baseaddress = CN_OSFLASH_BASEADDR;
    offset = CN_OSSPACE_OFFSET;
    //first we should check the buf is valid,we got the data
    if((NULL != bufDownload)&&(0 < bufDatalen))
    {
        //ok, now write the data
        printf("%s:BASEDst:0x%08x OFFSET:0X%08X Src:0x%08x len:0x%08x\r\n",\
                __FUNCTION__,(u32)baseaddress,offset,(u32)bufDownload,bufDatalen);
        gOsupdateMeida.flashwrite(baseaddress,offset,(void *)bufDownload,bufDatalen);
        return true;
    }
    else
    {
        return false;
    }
}

bool_t __oswrite(char *param)
{
    __osflasherase(NULL);
    __oswriteflash(NULL);
    return true;
}

static bool_t __download(char *buf, int count)
{
    if((bufDatalen + count) > bufLen)
    {
        printf("%s:memerr: buflen:%d datalen:%d writelen:%d!\n\r",\
                __FUNCTION__,bufLen,bufDatalen,count);
        return false;
    }

    memcpy((void *)(bufDownload + bufDatalen),(void *)buf, count);

    bufDatalen += count;

    return true;
}

static bool_t __osbootm(char *param)
{
    void (*gotoOs)(void);

    gotoOs = (void(*)(void))(CN_OSFLASH_BASEADDR + CN_OSSPACE_OFFSET);

    gotoOs();

    while(1);
    return true; //never comsback
}


//this function is used to download the file to the mem
//useage:download +server+filename +length
static bool_t __osdownload(char *param)
{
    u32 len;
    char *filename;
    int    argc = CN_PARAM_NUMMAX;
    char  *argv[CN_PARAM_NUMMAX];

    string2arg(&argc,argv,param);
    if(argc != 1)
    {
        printf("%s:parameter %d :SERVER NOT SPECIFIED\r\n",__FUNCTION__,argc);
        return false;
    }

    len = CN_OSSPACE_SIZE;
    filename = CN_OSFILE_NAME;

    if(NULL != bufDownload)
    {
        free(bufDownload);
    }

    bufDownload = malloc(len);
    if(NULL != bufDownload)
    {
        bufLen = len;
    }
    else
    {
        printf("%s:mem %d is not enough!",__FUNCTION__,len);
        bufLen = 0;
        return false;
    }

    bufDatalen = 0;
    __UpdateMode(argv[0], filename,(void *) __download);
    return true;
}


static bool_t __osversion(char *param)
{
    printf("DJYOSVERSION:DATE:%s TIME:%s\n\r",__DATE__,__TIME__);

    return true;
}

struct ShellCmdTab  gOsUpdateCmd[] =
{
    {
        "flashid",
        __osflashID,
        "usage:norid ",
        NULL
    },
    {
        "writeapp",
        __oswrite,
        "usage:writeapp",
        NULL
    },
    {
        "dlapp",
        __osdownload,
        "usage:dlapp+serverip",
        NULL
    },
    {
        "bootm",
        __osbootm,
        "usage:bootm",
        NULL
    },
    {
        "version",
        __osversion,
        "usage:version",
        NULL
    }
};

#define CN_OSUPDATE_CMDNUM  ((sizeof(gOsUpdateCmd))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc gOsUpdateCmdRsc[CN_OSUPDATE_CMDNUM];
bool_t ModuleInstall_OsUpdate(void *fnflashId, void *fnflasherase, void *fnflashwite)
{
    bool_t result;

    if(NULL != fnflashId)
    {
        gOsupdateMeida.flashid = (fnFlashID)fnflashId;
    }

    if(NULL != fnflasherase)
    {
        gOsupdateMeida.flasherase = (fnFlashErase)fnflasherase;
    }

    if(NULL != fnflashwite)
    {
        gOsupdateMeida.flashwrite = (fnFlashWrite)fnflashwite;
    }

    result = Sh_InstallCmd(gOsUpdateCmd,gOsUpdateCmdRsc,CN_OSUPDATE_CMDNUM);

    return result;
}

