/*
 * websdownload.c
 *
 *  Created on: 2016Äê1ÔÂ11ÈÕ
 *      Author: zqf
 */

//this file is used to download the webs to the mem
//uptils now, we has not got a file system yet,this is very hard for the test of
//the webserver, we could not program flash each time just because we has just change
//the webs, this makes me upset!my god, so use the net tftp to dowload all the webs
//to the ram.
//first, we should download an webslist
//second, we download all the webs listed in the weblist file
//the third, we should connect all the webs to the webroms

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CN_GDATABUF_SIZE   0x10000   //64KB
static u32     gDataSize = 0;
static u8     *pDataBuf;
//we allocat an 1MB buf to hold the websdata, then we cpy it to the spicified buf
static u8     *gWebsListBuf;
static u32     gWebsListBufSize = 0;
static u32     gWebsListNum = 0;  //this vars storage how many file name in the webslist

//this function is used to write the tftp data to the pDataBuf
static bool_t __fnWebsDownload(char *buf, int count)
{
    u8   *dst;
    u8   *src;

    if((NULL == buf)||(0 >= count))
    {
        return false;
    }
    if(CN_GDATABUF_SIZE < (gDataSize + count))
    {
        return false;
    }

    dst = (u8 *)(pDataBuf + gDataSize);
    src = buf;
    memcpy(dst,src,count);
    gDataSize += count;
}

#define CN_SERVERIP_LEN 0x20
static u8   gWebsServerIp[CN_SERVERIP_LEN];

static bool_t WebsServerIpSet(char *param)
{
    bool_t result = false;
    int len;
    if(NULL != param)
    {
        len = strlen(param);
        if(len < CN_SERVERIP_LEN)
        {
            memcpy((void *)gWebsServerIp,param,len);
            result = true;
        }
    }
    return result;
}

//counter the file name number
static bool_t WebsListNum(void)
{

    u8         *name;
    u32         namelen;
    u8          bak;
    //show each file name saved in the weblist file

    bak = '\0';
    namelen = 0;
    name = gWebsListBuf;

    while((name)<( gWebsListBuf+ gWebsListBufSize))
    {
        if((*name != '\0')&&(bak =='\0'))
        {
            //OK,got an name here
            //skip the file name
            namelen = strlen(name);
            name+=namelen;
            gWebsListNum++;
            bak = *name;
            name++;
        }
        else
        {
            bak = *name;
            name++;
        }
    }
    return true;
}

//param is the file name
static  bool_t WebsDownLoadList(char *param)
{
    u8 *src;
    u32 i;
    __TftpRcvFile(gWebsServerIp, param,(void *) __fnWebsDownload);
    printf("Listfile:%s  Size:%d\n\r",param,gDataSize);
    if(NULL != gWebsListBuf)
    {
        gWebsListBufSize = 0;
        free(gWebsListBuf);
    }
    gWebsListBuf = malloc(gDataSize);
    if(NULL == gWebsListBuf)
    {
        return false;
    }

    memcpy((void *)gWebsListBuf,(void *)pDataBuf,gDataSize);
    gWebsListBufSize = gDataSize;

    //makes all the space/tab/enter key to the '\0'
    for(i =0;i <gWebsListBufSize;i++ )
    {
        src = (u8 *)(gWebsListBuf + i);
        if(((*src)==' ')||((*src)=='\n')||((*src)=='\r'))
        {
            *src ='\0';
        }
    }

    //counter the num here
    WebsListNum();

    return true;
}
//show the filename show in the file
#define CN_FILENAME_PREFIX  "      "
static bool_t WebsListShow(char *param)
{

    u8         *name;
    u32         namelen;
    u8          bak;
    u32         filenum;
    //show each file name saved in the weblist file

    bak = '\0';
    namelen = 0;
    name = gWebsListBuf;
    filenum = 0;

    while((name)<( gWebsListBuf+ gWebsListBufSize))
    {
        if((*name != '\0')&&(bak =='\0'))
        {
            //OK,got an name here
            printf("%s:%d:%s\n\r",CN_FILENAME_PREFIX,filenum,name);
            //skip the file name
            namelen = strlen(name);
            name+=namelen;
            bak = *name;
            name++;
            filenum++;
        }
        else
        {
            bak = *name;
            name++;
        }
    }
    return true;
}

#include    "wsIntrn.h"

static websRomPageIndexType  *pWebsPtr;
//download all the webs listed in the listfile
static bool_t WebsDownLoad(char *param)
{

    u8         *name;
    u32         namelen;
    u8          bak;
    //show each file name saved in the weblist file

    //malloc an webs here
    websRomPageIndexType  *tmp;

    if(NULL != pWebsPtr)
    {
        free(pWebsPtr);
        pWebsPtr = NULL;
    }
    pWebsPtr = malloc(sizeof(websRomPageIndexType) * (gWebsListNum+1));
    if(NULL == pWebsPtr)
    {
        return false;
    }
    memset((void *)pWebsPtr,0,sizeof(websRomPageIndexType) * gWebsListNum);
    //now we download each web listed in the list file
    bak = '\0';
    namelen = 0;
    name = gWebsListBuf;
    tmp = pWebsPtr;

    while((name)<( gWebsListBuf+ gWebsListBufSize))
    {
        if((*name != '\0')&&(bak =='\0'))
        {
            //OK,got an name here
            namelen = strlen(name);
            tmp->path = malloc(namelen +2);
            memset(tmp->path,0,namelen+2);
            tmp->path[0] = '/';
            memcpy(tmp->path +1,name, namelen);

            //download the file
            __TftpRcvFile(gWebsServerIp, name,(void *) __fnWebsDownload);
            //cpy it from the buf
            tmp->page = malloc(gDataSize);
            if(NULL != tmp->page)
            {
                tmp->size = gDataSize;

                memcpy((void *)tmp->page,(void *)pDataBuf,gDataSize);
            }
            else
            {
                tmp->size = 0;
            }
            tmp++;
            //skip the file name
            name+=namelen;
            bak = *name;
            name++;
        }
        else
        {
            bak = *name;
            name++;
        }
    }

    return true;
}

static bool_t WebsStat(char *param)
{

    websRomPageIndexType  *tmp;
    tmp = pWebsPtr;
    while(tmp < (pWebsPtr+gWebsListNum))
    {
        if(NULL != tmp->path)
        {
            printf("webspath:%s:size:0x%08x  pos:0x%08x\n\r",tmp->path,tmp->size,tmp->pos);
            tmp++;
        }
        else
        {
            break;
        }
    }

    return true;
}

static bool_t WebsUpdate(char *param)
{
    extern int websRomOpen();
    extern void websRomClose();

    if(NULL != pWebsPtr)
    {
        websRomClose();
        websRomPageIndex = pWebsPtr;
        websRomOpen();
    }

    return true;
}


#include "shell.h"
static struct ShellCmdTab  gWebsDownload[] =
{
    {
        "setserver",
        WebsServerIpSet,
        "usage:setserver + serverip",
        "usage:this cmd used to set the webs storage server"
    },
    {
        "webslistdownload",
        WebsDownLoadList,
        "usage:websdownloadlst + listfilename",
        "usage:this cmd used to download the webs file list name file"
    },
    {
        "webslist",
        WebsListShow,
        "usage:webslist",
        "usage:this cmd used to show what file to be download or has been downloaded"
    },

    {
        "websdownload",
        WebsDownLoad,
        "usage:websdownload",
        "usage:download all the webs listed in the listfile"
    },
    {
        "webstat",
        WebsStat,
        "usage:webstat",
        "usage:show all the webs stat"
    },
    {
        "websupdate",
        WebsUpdate,
        "usage:websupdate",
        "usage:update all the webs"
    }
};

#define CN_WEBSDOWNLOAD_CMDNUM  ((sizeof(gWebsDownload))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc gWebsDownloadRsc[CN_WEBSDOWNLOAD_CMDNUM];
bool_t ModuleInstall_WebsDownload(void)
{
    bool_t result = false;

    pDataBuf = malloc(CN_GDATABUF_SIZE);
    if(NULL == pDataBuf)
    {
        return result;
    }

    result = Sh_InstallCmd(gWebsDownload,gWebsDownloadRsc,CN_WEBSDOWNLOAD_CMDNUM);

    return result;
}



