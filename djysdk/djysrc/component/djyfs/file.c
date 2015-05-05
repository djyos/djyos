//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. As a constituent part of djyos,do not transplant it to other software
//    without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
// Copyright (c) 2014 著作权由深圳鹏瑞软件有限公司所有。著作权人保留一切权利。
//
// 这份授权条款，在使用者符合以下三条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。
// 3. 本软件作为都江堰操作系统的组成部分，未获事前取得的书面许可，不允许移植到非
//    都江堰操作系统环境下运行。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------
//所属模块:文件系统
//作者：lst
//版本：V1.0.0
//文件描述:文件系统与存储介质无关的部分
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "config-prj.h"
#include "ctype.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "rsc.h"
#include "pool.h"
#include "lock.h"
#include "systime.h"
#include "djyos.h"
#include "file.h"
#include "shell.h"
#include "shell_fs.h"
//ansi c standard 支持的文件输入输出操作

//— The wide-character input functions — those functions described in 7.24 that perform
//input into wide characters and wide strings: fgetwc, fgetws, getwc, getwchar,
//fwscanf, wscanf, vfwscanf, and vwscanf.

//— The wide-character output functions — those functions described in 7.24 that
//perform output from wide characters and wide strings: fputwc, fputws, putwc,
//putwchar, fwprintf, wprintf, vfwprintf, and vwprintf.

//— The wide-character input/output functions — the union of the ungetwc function,
//the wide-character input functions, and the wide-character output functions.

//— The byte input/output functions — those functions described in this subclause that
//perform input/output: fgetc, fgets, fprintf, fputc, fputs, fread,
//fscanf, fwrite, getc, getchar, gets, printf, putc, putchar, puts,
//scanf, ungetc, vfprintf, vfscanf, vprintf, and vscanf.

//Operations on files，文件操作函数
//int remove(const char *filename);
//int rename(const char *old, const char *new);
//FILE *tmpfile(void);
//char *tmpnam(char *s);
//
//File access functions，文件访问函数
//int fclose(FILE *stream);
//int fflush(FILE *stream);
//FILE *fopen(const char * filename,const char * mode);
//FILE *freopen(const char * filename,const char * mode,FILE * restrict stream);
//void setbuf(FILE * restrict stream,char * restrict buf);
//int setvbuf(FILE * restrict stream,char * restrict buf,int mode, size_t size);
//
//Formatted input/output functions，格式化输入输出函数
//int fprintf(FILE * restrict stream,const char * restrict format, ...);
//int fscanf(FILE * restrict stream,const char * restrict format, ...);
//int printf(const char * restrict format, ...);
//int scanf(const char * restrict format, ...);
//int snprintf(char * restrict s, size_t n,const char * restrict format, ...);
//int sprintf(char * restrict s,const char * restrict format, ...);
//int sscanf(const char * restrict s,const char * restrict format, ...);
//int vfprintf(FILE * restrict stream,const char * restrict format,va_list arg);#include <stdarg.h>
//int vfscanf(FILE * restrict stream,const char * restrict format,va_list arg);
//int vprintf(const char * restrict format,va_list arg);
//int vscanf(const char * restrict format,va_list arg);
//int vsnprintf(char * restrict s, size_t n,const char * restrict format,va_list arg);
//int vsprintf(char * restrict s,const char * restrict format,va_list arg);
//int vsscanf(const char * restrict s,const char * restrict format,va_list arg);
//
//Character input/output functions，字符输入输出函数
//int fgetc(FILE *stream);
//char *fgets(char * restrict s, int n,FILE * restrict stream);
//int fputc(int c, FILE *stream);
//int fputs(const char * restrict s,FILE * restrict stream);
//int getc(FILE *stream);
//int getchar(void);
//char *gets(char *s);
//int putc(int c, FILE *stream);
//int putchar(int c);
//int puts(const char *s);
//int ungetc(int c, FILE *stream);
//
//Direct input/output functions，直接输入输出函数
//size_t fread(void * restrict ptr,size_t size, size_t nmemb,FILE * restrict stream);
//size_t fwrite(const void * restrict ptr,size_t size, size_t nmemb,FILE * restrict stream);
//
//File positioning functions，文件读写位置函数
//int fgetpos(FILE * restrict stream,fpos_t * restrict pos);
//int fseek(FILE *stream, long int offset, int whence);
//int fsetpos(FILE *stream, const fpos_t *pos);
//long int ftell(FILE *stream);
//void rewind(FILE *stream);
//
//Error-handling functions，出错处理函数
//void clearerr(FILE *stream);
//int feof(FILE *stream);
//int ferror(FILE *stream);
//void perror(const char *s);

//文件(目录)变量命名规范:
//在"a:\aaa\bbb\ccc"中：
// 1."a"被命名为PTT_name
// 2."aaa"、"bbb"为spathname
// 3."ccc"被命名为filename
//4."aaa"、"bbb"、"ccc"统称为word
//5.整串或可能是整串时，命名为fullname
//6."a:\aaa\bbb\"被命名为fullpath
//7."\aaa\bbb\"被命名为abspath
//8."aaa\bbb\"称为relativepath
//9."\aaa\bbb\ccc"命名为synname
//10."aaa\bbb\ccc"命名为relativename(相对于当前路径)

static struct tagFileRsc *s_ptContentBuf;//定义目录项内存池
static struct tagMemCellPool *s_ptContentPool;              //内存池头指针。
//设定一个工作路径,相当于windows/dos的当前路径
struct tagFileRsc *g_ptWorkPath = NULL;    //工作路径指针
static struct tagRscNode s_tFileSystemRoot; //打开的文件资源的根节点。
struct tagSemaphoreLCB *Fs_Semaphore; //保护fs树所用的信号量

bool_t __Djyfs_PickWord(const char *synname,u32 curent_offset,char *name);
bool_t __Djyfs_PickPathWord(const char *synname,u32 curent_offset,char *name);
bool_t __Djyfs_PickFileNameWord(const char *synname,char *name);
bool_t __Djyfs_IsAbsPath(const char *fullname);

struct tagShellCmdTab const ShellFsCmdTab[] =
{
    {
        "rmdir",
        ShFs_RD,
        "删除一个目录",
        NULL
    },
    {
        "rd",
        ShFs_RD,
        "删除一个目录",
        NULL
    },
    {
        "dir",
        ShFs_Dir,
        "显示指定目录下的文件和子目录列表",
        "dir [分区：]路径  [分区：]路径... ...\r\n\
            显示指定目录下的文件和子目录列表\r\n\
dir [分区：][含有通配符路径][分区：][含有通配符路径]... ...\r\n\
            显示指定目录下匹配的文件和子目录列表\r\n\
dir [\\][\\]... ... 显示根目录下一级的文件和子目录列表\r\n\
dir [..][..]... ... 显示当前目录上一级的文件和子目录列表\r\n\
dir [](空字符)      显示当前目录下的文件和子目录列表"
    },
    {
        "rename",
        ShFs_Ren,
        "重命名一个文件",
        "Ren/rename [分区:][路径]文件名1 文件名2"
    },
    {
        "ren",
        ShFs_Ren,
        "重命名一个文件",
        "Ren/rename [分区:][路径]文件名1 文件名2"
    },
    {
        "del",
        ShFs_Del,
        "删除至少一个文件",
        NULL
    },
    {
        "copy",
        ShFs_Copy,
        "复制一个文件",
        NULL
    },
    {
        "move",
        ShFs_Move,
        "将一个或多个文件移动到指定目录下",
        NULL
    },
    {
        "chdir",
        ShFs_CD,
        "显示当前目录的名称或将其更改",
        NULL
    },
    {
        "cd",
        ShFs_CD,
        "显示当前目录的名称或将其更改",
        "PTT_name[:]         进入指定分区名根目录\r\n\
                   例如：D： 进入D分区根目录\r\n\
cd/chdir[分区：]路径 显示当前目录的名称或改变当前目录\r\n\
                   例如: 1)cd C:\\folder 进入folder目录\r\n\
                          2)cd folder 进入folder目录\r\n\
cd/chdir[](空字符) 显示当前目录名称\r\n\
cd/chdir[\\]       更换到根目录\r\n\
cd/chdir[..]       更换到上一级目录\r\n\
                   例如: 1)cd ..\\.. 进入上两级的目录 \r\n\
                         2)cd ..\\folder 进入上一级目录中含有folder的目录"
    },
    {
        "mkdir",
        ShFs_MD,
        "创建一个目录",
        NULL
    },
    {
        "md",
        ShFs_MD,
        "创建一个目录",
        NULL
    },
    {
        "create-ptt",
        ShFs_CreatePTT,
        "创建分区",
        "命令格式: create-ptt 分区大小 分区名"
    },
    {
        "test-w",
        ShFs_TestWrite,
        "测试写文件",
        NULL
    },
    {
        "test-r",
        ShFs_TestRead,
        "测试读文件",
        NULL
    },
   {
        "format",
        ShFs_Format,
        "格式化文件系统",
        NULL
    },
    {
        "chkptt",
        ShFs_Chkptt,
        "显示分区空间的参数",
        NULL
    },
};

static struct tagShellCmdRsc tg_FileCmdRsc
                        [sizeof(ShellFsCmdTab)/sizeof(struct tagShellCmdTab)];
void Sh_PrintWorkPathSet(void);

ptu32_t ModuleInstall_Djyfs(ptu32_t para)
{
    //在资源链表中建立一个根结点fs，所有分区、根部亩以及打开的文件和目录都建立在此结点下。
    //初始化分区时，每个立即添加的分区为该节点的子节点，该分区的根目录结点为分区结点的子结点。
    Fs_Semaphore = Lock_SempCreate(1,1,CN_SEMP_BLOCK_FIFO,"fs tree");
    if(Fs_Semaphore == NULL)
        return 0;  //fs树信号量申请失败
    s_ptContentBuf = M_Malloc(gc_u32CfgOpenedfileLimit * sizeof(struct tagFileRsc),0);
    if(s_ptContentBuf == NULL)
    {
        Lock_SempDelete(Fs_Semaphore);
        return 0;
    }
    Rsc_AddTree(&s_tFileSystemRoot,sizeof(struct  tagRscNode),RSC_RSCNODE,"fs");
    //建立目录项内存池，每个打开的文件算一项，每个目录也算一项，如果是多级目录，
    //则每级单独计算，如果打开的多个文件的路径有重合，重合部分不单独占用目录项。
    s_ptContentPool = Mb_CreatePool(s_ptContentBuf,
                                gc_u32CfgOpenedfileLimit,
                                sizeof(struct tagFileRsc),0,0,
                                "打开的目录项内存池");
    if(s_ptContentPool == NULL)
    {
        Lock_SempDelete(Fs_Semaphore);
        free(s_ptContentBuf);
        return 0;
    }
    if(Sh_InstallCmd(ShellFsCmdTab,tg_FileCmdRsc,
            sizeof(ShellFsCmdTab)/sizeof(struct tagShellCmdTab)))
    {
        Sh_PrintWorkPathSet();
        return 1;
    }
    return 0;
}

//----设置当前工作路径---------------------------------------------------
//功能: 当前工作路径。默认情况下，设置为第一个
//      分区第一个根目录。cfg_work_path，在cofig-prj.h里配置
//参数: path，新的工作路径
//返回: 1，设置成功
//      0, 设置失败
//-------------------------------------------------------------------------
u32 Djyfs_SetWorkPath(char *path)
{
    struct tagPTTDevice *PTT_device_tag;

    if(path != NULL)
    {
        if(strcmp(path,"")!=0)
        {
            if( Djyfs_AppCtrl(enum_fs_work_path,
                                    (ptu32_t)path,0)
                                    == 0)
            {
                printf("用户设置工作路径失败\r\n");
                return 0;
            }
            else
                return 1;
        }
    }
    //指向第一个分区的根目录
    PTT_device_tag = (struct tagPTTDevice*)Rsc_GetSon(&s_tFileSystemRoot);
    if(PTT_device_tag->opened_root == NULL)
    {
        return 0;
    }
    else
    {
        g_ptWorkPath = PTT_device_tag->opened_root;
        return 1;
    }
}

//----获取当前分区名-----------------------------------------------------------
//功能: 获取当前工作目录所在的分区名(对应windows和dos的盘符)
//参数: PTT_name，保存字符串的缓冲区
//      namebuf_len,名字缓冲区的长度
//返回: true，获取成功
//      false,获取失败
//------------
//更新记录:
// 1.日期: 2015/3/25
//   说明: 修改打印信息
//   作者: 季兆林
//-------------------------------------------------------------------------
bool_t Djyfs_GetPTT_Name(char *PTT_name,u32 namebuf_len)
{
    struct tagRscNode *parent,*fs_tree;

    if(g_ptWorkPath == NULL)// 说明路径未初始化或者分区未建立
    {
        printf("未指定当前工作目录\r\n");
        return false;
    }
    if(PTT_name == NULL)
        return false;
    Lock_SempPend(Fs_Semaphore,CN_TIMEOUT_FOREVER);
    fs_tree = Rsc_GetTree((struct tagRscNode*)g_ptWorkPath);
    parent = (struct tagRscNode*)g_ptWorkPath;
    while(Rsc_GetParent(parent) != fs_tree)//将parent指针指向分区资源节点
    {
        parent = Rsc_GetParent(parent);
    }
    if(strlen(parent->name) >= namebuf_len)
    {
        Lock_SempPost(Fs_Semaphore);
        return false;
    }
    else
    {
        strcpy(PTT_name,parent->name);
        Lock_SempPost(Fs_Semaphore);
        return true;
    }

}

//----查找文件----------------------------------------------------------------
//功能: 由file.c内部调用的一个函数，用于在文件系统中查找一个文件（目录）
//      是否存在。函数调用分区的lookfor_item函数完成任务，只返回查找
//      结果，并不返回被查找目标的信息。
//参数: PTT_device_tag，被操作的分区数据结构指针
//      synname,文件名或者目录名字符串，
//返回: 找到则返回true，找不到返回false
//------------
//更新记录:
// 1.日期: 2015/3/19
//   说明: 将对"\"单独判断的逻辑合并到了__Djyfs_PickWord中
//   作者: 季兆林
//-----------------------------------------------------------------------------
bool_t __Djyfs_LookforItem(struct tagPTTDevice *PTT_device_tag,char *synname,
                                          struct stat *result)
{
    struct tagFileRsc *parent,*son=NULL,fp_buf;
    uint16_t next_char_off;
    char name[CN_FILE_NAME_LIMIT + 1];
    if(__Djyfs_IsAbsPath(synname)) //synname中已经去掉了分区名，无需再判断
    {   //路径名以"\"开始,则从根目录开始操作
        parent = PTT_device_tag->opened_root;
        next_char_off = 1;
    }
    else
    {   //从当前路径开始操作
        parent = g_ptWorkPath;
        next_char_off = 0;
    }

    Lock_SempPend(Fs_Semaphore, CN_TIMEOUT_FOREVER);
    //沿路径逐级搜索目录和文件
    while(__Djyfs_PickWord(synname, next_char_off, name))
    {
        //name是模块内部提供的字符串指针，已经经过字符串长度合法性检查
        next_char_off += strlen(name) + 1; //+1是为跳过字符'\'
        son = (struct tagFileRsc *)Rsc_SearchSon(&parent->file_node, name);
        if(NULL != son)
        {   //目标已经打开
            parent = son; //以当前打开的目录为下次使用的父目录
        }
        else
        {   //目标文件(目录)尚未打开
            Rsc_CleanNode(&fp_buf.file_node);
            //查找目标并初始化文件数据结构
            if(PTT_device_tag->lookfor_item(name, parent, &fp_buf))
            {   //目标存在，并且已经初始化目录信息
                fp_buf.home_PTT = parent->home_PTT;
                son = &fp_buf;   //以当前打开的目标为下次使用的父目录
                Rsc_CleanNode(&son->file_node);
                parent = son;
            }
            else
            {   //目录不存在，返回false
                Lock_SempPost(Fs_Semaphore);
                return false;
            }
        }
        if(next_char_off >= strlen(synname))
            break;
    }
    Lock_SempPost(Fs_Semaphore);

     Djyfs_Fstat(parent, result);
    return true;
}

//----打开分区中的文件-------------------------------------------------
//功能: 由file.c内部调用的一个函数，打开该分区中的一个文件。函数首先
//      调用分区的lookfor_item确定目标文件是否存在，然后沿路径逐个打开目录，
//      为每一个目录分配 struct tagFileRsc 结构内存，并把他挂到资源链表上。最后打
//      开文件，也挂到资源链表上。
//参数: PTT_device_tag，被操作的分区数据结构指针
//      synname,文件名或者目录名字符串，文件和目录统称item
//      mode，文件打开模式
//返回: 打开的文件及打开的目录资源，错误则返回NULL
//-----------------------------------------------------------------------------
FILE *__Djyfs_OpenFile(struct tagPTTDevice *PTT_device_tag,
                                const char *synname,char *mode)
{
    struct tagFileRsc *parent,*son;
    struct tagFileRsc *opened = NULL;   //保存最后一个已经打开的目录项
    u32 open_result;
    uint16_t next_char_off;
    char    name[256];
    union file_attrs file_attr;
    enum _FILE_OPEN_MODE_ my_mode;
    bool_t need_to_creat = true;    //表示当文件不存在时，是否需要创建
    file_attr.all = 0;
    if((strcmp(mode,"r")==0) || (strcmp(mode,"rb")==0))
    {
        need_to_creat = false;
        my_mode = EN_R_RB;
    }else if((strcmp(mode,"w")==0) || (strcmp(mode,"wb")==0))
        my_mode = EN_W_WB;
    else if((strcmp(mode,"a")==0) || (strcmp(mode,"ab")==0))
        my_mode = EN_A_AB;
    else if((strcmp(mode,"r+")==0)||(strcmp(mode,"rb+")==0)
                            ||(strcmp(mode,"r+b")==0))
    {
        need_to_creat = false;
        my_mode = EN_R_RB_PLUS;
    }else if((strcmp(mode,"w+")==0) || (strcmp(mode,"wb+")==0)
                         || (strcmp(mode,"w+b")==0))
        my_mode = EN_W_WB_PLUS;
    else if((strcmp(mode,"a+")==0) || (strcmp(mode,"ab+")==0)
                            || (strcmp(mode,"a+b")==0))
        my_mode = EN_A_AB_PLUS;
    else if(strcmp(mode,"cd")==0)
    {
        my_mode = EN_A_AB_PLUS;
        file_attr.bits.close_delete = 1;
    }
    else
        return NULL;
    if(__Djyfs_IsAbsPath(synname))  //synname中已经去掉了分区名，无需再判断
    {
        parent = PTT_device_tag->opened_root;  //从根目录开始操作
        next_char_off = 1;
    }else
    {
        parent = g_ptWorkPath;  //从当前路径开始操作
        next_char_off = 0;
    }
    //在djyfs_fclose里，也必须弄清对根目录的操作
    if(strcmp(synname,"\\")==0)
        return PTT_device_tag->opened_root;//返回的是根目录

    //消除编译告警(son使用前未初始化),因下面的while和if语句至少会有一个条件成立，
    //所以不会出现son未初始化的情况。
    son = parent;

    Lock_SempPend(Fs_Semaphore,CN_TIMEOUT_FOREVER);
    while(__Djyfs_PickPathWord(synname,next_char_off,name))
    {   //沿路径逐级打开目录，但不会打开文件
        //name是模块内部提供的字符串指针，已经经过字符串长度合法性检查
        next_char_off += strlen(name) +1;    //+1是为跳过字符'\'
        if((son = (struct tagFileRsc *)Rsc_SearchSon(&parent->file_node,name))
                    != NULL)
        {//目标已经打开
            opened = son;
            if(son->open_counter != CN_LIMIT_UINT32)
                son->open_counter ++;
        }
        else
        {//目标文件(目录)尚未打开
            son = (struct tagFileRsc *)Mb_Malloc(s_ptContentPool,0);
            if(son == NULL)
                goto exit_open_err;    //分配内存失败
            son->home_PTT = parent->home_PTT;
            //查找目录并初始化文件数据结构
            open_result = PTT_device_tag->open_item(name,parent,son,my_mode);
            if(open_result == CN_FS_OPEN_SUCCESS)
            {//目录存在，并且已经初始化目录信息
                //打开目录，实际上就是把目录结点挂到打开的文件资源树上。
                Rsc_AddHeadSon(&parent->file_node,&son->file_node,
                                sizeof(struct tagFileRsc),RSC_FILE,son->name);
                if(PTT_device_tag->opened_sum != CN_LIMIT_UINT32)
                    PTT_device_tag->opened_sum ++;
                son->open_counter = 1;
                son->eno = EN_FS_NO_ERROR;
                son->open_mode = my_mode;
            }
            else if(open_result == CN_FS_ITEM_EXIST)   //目录存在，但模式不匹配
            {
                goto exit_open_err;
            }
            else
            {//目录不存在，看是否需要创建。
                if(need_to_creat)
                {//需要创建
                    file_attr.bits.folder = 1;  //创建的是目录
                    if(PTT_device_tag->create_item(name,parent,son,
                                                file_attr,my_mode) == false)
                    {//创建目录失败
                        Mb_Free(s_ptContentPool,son);
                        goto exit_open_err;
                    }
                    else
                    {
                        //打开目录实际上就是把目录结点挂到打开的文件资源树。
                        Rsc_AddHeadSon(&parent->file_node,
                                           &son->file_node,
                                           sizeof(struct tagFileRsc),
                                           RSC_FILE,son->name);
                        if(PTT_device_tag->opened_sum != CN_LIMIT_UINT32)
                            PTT_device_tag->opened_sum ++;
                        son->open_counter = 1;
                        son->open_mode = my_mode;
                        son->eno = EN_FS_NO_ERROR;
                    }
                }
                else
                {//不需要创建，当以r或r+方式打开文件时，不存在也不创建。
                    Mb_Free(s_ptContentPool,son);
                    goto exit_open_err;
                }
            }
            //被打开的项目(目录)所属分区
            son->home_PTT = PTT_device_tag;
        }
        parent = son;   //以当前打开的目录为下次使用的父目录
    }
    //至此，目录已经全部打开(或创建)，下面打开文件
    if(__Djyfs_PickFileNameWord(synname,name))  //synname串中包含文件名吗?
    {//打开文件
        son = (struct tagFileRsc *)Rsc_SearchSon(&parent->file_node,name);
        if(son == NULL)
        {//文件尚未打开
            son = (struct tagFileRsc *)Mb_Malloc(s_ptContentPool,0);
            if(son == NULL)
                goto exit_open_err;    //分配内存失败
            son->home_PTT = parent->home_PTT;
            //查找并打开文件
            open_result = PTT_device_tag->open_item(name,parent,son,my_mode);
            if(open_result == CN_FS_OPEN_SUCCESS)
            {//文件存在并且可以按my_mode模式打开，已经初始化文件信息
                Rsc_AddHeadSon(&parent->file_node,&son->file_node,
                                sizeof(struct tagFileRsc),RSC_FILE,son->name);
                if(PTT_device_tag->opened_sum != CN_LIMIT_UINT32)
                    PTT_device_tag->opened_sum ++;
                son->open_counter = 1;
                son->eno = EN_FS_NO_ERROR;
                son->open_mode = my_mode;
            }
            else if(open_result == CN_FS_ITEM_EXIST)   //文件存在，但模式不匹配
            {
                goto exit_open_err;
            }
            else
            {//文件不存在，看是否需要创建。
                if(need_to_creat)
                {//需要创建
                    file_attr.bits.folder = 0;      //创建的是文件
                    file_attr.bits.archive = 1;     //是文件
                    if(PTT_device_tag->create_item(name,parent,son,
                                                    file_attr,my_mode)==false)
                    {//创建文件失败
                        Mb_Free(s_ptContentPool,son);
                        goto exit_open_err;
                    }
                    else
                    {
                        Rsc_AddHeadSon(&parent->file_node,
                                           &son->file_node,
                                           sizeof(struct tagFileRsc),
                                           RSC_FILE,son->name);
                        son->open_counter = 1;
                        son->eno = EN_FS_NO_ERROR;
                        son->open_mode = my_mode;
                        if(PTT_device_tag->opened_sum != CN_LIMIT_UINT32)
                            PTT_device_tag->opened_sum ++;
                    }
                }
                else
                {//r或r+方式，不需要创建。
                    Mb_Free(s_ptContentPool,son);
                    goto exit_open_err;
                }
            }
        }
        else
        {   //文件已经打开
            //已经打开的，由于误操作，或者路径后面有没有反斜杠等等，在这里
            //再判断一下，有好处。
            if(son->file_attr.bits.folder)
            {
                if(son->open_counter != CN_LIMIT_UINT32)
                son->open_counter ++;
            }
            else
            {
                if(my_mode == son->open_mode)
                {//已经打开的文件，只能以相同的方式再次打开。并且读写指针不改变
                    if(son->open_counter != CN_LIMIT_UINT32)
                        son->open_counter ++;
                    goto exit_open_son;
                }else
                {
                    Lock_SempPost(Fs_Semaphore);
                    return NULL;
                }
            }
        }
    }
    else
    {
        //打开目录，目录在if(__Djyfs_PickFileNameWord(synname,name))前已经打开，
        //且无需计打开次数
        goto exit_open_son;//返回打开的目录
    }
    son->home_PTT = PTT_device_tag;
exit_open_son:
    Lock_SempPost(Fs_Semaphore);
    return son;
exit_open_err:
    //删除已经添加的资源节点和释放分配的内存,opened保存的是本次增加的
    //第一个资源节点的上一级节点。
    if(opened == NULL)
    {
        Lock_SempPost(Fs_Semaphore);
        return NULL;
    }
    son = (struct tagFileRsc *)Rsc_GetTwig(&opened->file_node);
    while(son != NULL)
    {
        Rsc_DelNode(&son->file_node);
        Mb_Free(s_ptContentPool,son);
        son = (struct tagFileRsc *)Rsc_GetTwig(&opened->file_node);
    }
    Rsc_DelNode(&opened->file_node);
    Mb_Free(s_ptContentPool,opened);
    Lock_SempPost(Fs_Semaphore);
    return NULL;
}
//----文件系统设备左手控制函数-------------------------------------------------
//功能：顾名思义，执行一系列上层发下来的文件系统控制命令
//参数：AppCmd，左手命令字，使用时转换成enum fs_left_cmd类型
//      data1、data2，跟命令字相关的数据
//返回：与命令字相关
//-----------------------------------------------------------------------------
ptu32_t Djyfs_AppCtrl(u32 AppCmd,ptu32_t data1,ptu32_t data2)
{
    switch(AppCmd)
    {
        case enum_fs_work_path:
        {
            //data1,字符串指针
            //返回: 1，设置成功
            //      0, 设置失败
            if((char*)data1 == NULL)
                return 0;
            g_ptWorkPath = fopen((char*)data1,"w+");
            if(g_ptWorkPath == NULL)
            {
                printf("用户设置工作路径失败\r\n");
                return 0;
            }
            else
                return 1;

        }break; //for enum_fs_work_path
        default:break;
    }
    return 0;
}

//----文件系统设备右手控制函数-------------------------------------------------
//功能：顾名思义，执行一系列存储设备driver发出的文件系统控制命令
//参数：right_cmd，右手命令字，使用时转换成enum fs_right_cmd类型
//      data1、data2，跟命令字相关的数据
//返回：与命令字相关，参见源程序注释
//------------
//更新记录:
// 1.日期: 2015/3/20
//   说明: 1.添加分区指令中增加对Rsc_AddSon失败的判断。
//   作者: 季兆林
//-----------------------------------------------------------------------------
ptu32_t Djyfs_FsDriverCtrl(u32 right_cmd,
                                ptu32_t data1,ptu32_t data2)
{
    struct tagFileRsc *root_folder;
    struct tagPTTDevice *PTT_device_tag;
    switch(right_cmd)
    {
        case enum_fs_add_PTT:
        {//成功添加返回enum_fs_no_error，否则返回enum_fs_creat_PTT_error
         //data1为分区专有数据结构指针
         //data1是 struct tagPTTDevice* 类型，指向分区专有数据结构，
            struct tagRscNode *NewNode;
            //提取分区专有数据结构
            PTT_device_tag = (struct tagPTTDevice*)data1;
            Lock_SempPend(Fs_Semaphore,CN_TIMEOUT_FOREVER);
            //把新分区的分区资源节点加入到文件根资源结点下，排在队列尾
            NewNode = Rsc_AddSon(&s_tFileSystemRoot, &PTT_device_tag->PTT_node,
                                 sizeof(struct tagPTTDevice), RSC_PTT,
                                 PTT_device_tag->name);
            if(NULL == NewNode)
                return (EN_FS_CREAT_PTT_ERROR);

            if(PTT_device_tag->formatted)   //这是已经格式化好的分区
            {
                //申请根目录资源结点内存
                root_folder = Mb_Malloc(s_ptContentPool,0);
                if(root_folder == NULL)
                {   //申请不到内存，释放信号量，返回EN_FS_CREAT_PTT_ERROR
                    Lock_SempPost(Fs_Semaphore);
                    return EN_FS_CREAT_PTT_ERROR;
                }
                memset(root_folder,0,sizeof(struct tagFileRsc));
                //把新分区的根目录资源节点加入到对应的分区资源节点下，排在队列尾
                Rsc_AddSon(&PTT_device_tag->PTT_node,&root_folder->file_node,
                                  sizeof(struct tagFileRsc),RSC_FILE,PTT_device_tag->name);
                root_folder->home_PTT = PTT_device_tag;
                root_folder->file_attr.bits.folder = 1;
                root_folder->file_attr.bits.read_only = 1;
                //分区设备的打开文件的根结点指向该根资源结点。
                PTT_device_tag->opened_root = root_folder;
                PTT_device_tag->opened_sum = 0;
                strcpy(root_folder->name,PTT_device_tag->name);
//                strcat(PTT_device_tag->opened_root->name,":");
            }else                           //这是未格式化的分区
                PTT_device_tag->opened_root = NULL;
            Lock_SempPost(Fs_Semaphore);
            return EN_FS_NO_ERROR;
        }break; //for enum_fs_add_PTT
        case enum_fs_del_PTT:
        {
            //提取分区专有数据结构
            PTT_device_tag = (struct tagPTTDevice*)data1;
            Lock_SempPend(Fs_Semaphore,CN_TIMEOUT_FOREVER);
            if(Rsc_DelNode(&PTT_device_tag->opened_root->file_node))
            {
                Lock_SempPost(Fs_Semaphore);
                    return EN_FS_NO_ERROR;
            }else
                Lock_SempPost(Fs_Semaphore);
                return EN_FS_REMOVE_ERROR;
        }break; //for enum_fs_del_PTT
        default:break;
    }
    return EN_FS_NO_ERROR;
}

//----APP写分区------------------------------------------------------------
//功能: 从分区APP接口把数据写入文件,本函数由file.c内部调用，
//参数: PTT_device_tag，被操作的分区数据结构指针
//      buf，写入数据缓冲区指针
//      write_para，写入参数
//      file，被写的文件指针。
//----------------------------------------------------------------------------
u32 __Djyfs_PTT_AppWrite(struct tagPTTDevice *PTT_device_tag,ptu32_t buf,
                        ptu32_t write_para,u32 res)
{
    struct tagRwPara *pl_write_para = (struct tagRwPara *)write_para;
    u32 write_len;  //需要写入的数据长度
    sint64_t rest_len;
    u32 result;
    struct tagFileRsc *fp = (struct tagFileRsc *)pl_write_para->fp;  //取得文件指针
    res = res;        //消除编译器告警
    write_len = pl_write_para->nmemb * pl_write_para->size;  //计算需写入的长度
    //查询物理设备空闲空间
    rest_len = PTT_device_tag->query_file_cubage(fp);
    if(rest_len >= write_len)
    {//物理设备上有足够的空间
        result=PTT_device_tag->write(fp,(uint8_t*)buf,write_len);
        result = result / pl_write_para->nmemb;
    }else
    {//物理设备上没有足够的空间，写入整数个完整记录
        write_len = (u32)((rest_len / pl_write_para->nmemb) * pl_write_para->nmemb);
        result=PTT_device_tag->write(fp,(uint8_t*)buf,write_len);
        result = result / pl_write_para->nmemb;
    }
    return result;
}
//----APP读分区------------------------------------------------------------
//功能: 从分区APP接口读出文件,本函数由file.c内部调用
//参数: PTT_device_tag，被操作的分区数据结构指针
//      buf，保存数据的缓冲区指针
//      read_para，读出参数
//      file，被读的文件指针。
//----------------------------------------------------------------------------
ptu32_t __Djyfs_PTT_AppRead(struct tagPTTDevice *PTT_device_tag,ptu32_t buf,
                        ptu32_t read_para,u32 res)
{
    struct tagRwPara *pl_read_para = (struct tagRwPara *)read_para;
    u32 read_len;  //需要读取的数据长度
    sint64_t stock_size;
    u32 result;

    struct tagFileRsc *fp = (struct tagFileRsc *)pl_read_para->fp;  //取得文件指针
    res = res;        //消除编译器告警
    read_len = pl_read_para->nmemb * pl_read_para->size;
    //查询物理设备有多少数据可读，文件尺寸并不可靠，不能用
    stock_size = PTT_device_tag->query_file_stocks(fp);
    if(stock_size >= read_len)
    {//物理设备上有足够的数据
        //读取物理设备
        result = PTT_device_tag->read(fp,(uint8_t*)buf,read_len);
        result = result / pl_read_para->nmemb;
    }else
    {//物理设备上没有足够的数据，需要调整读取数量，以确保读到整数个单元
        read_len = (u32)((stock_size / pl_read_para->nmemb) * pl_read_para->nmemb);
        //需要从物理设备读取的数据字节数
        read_len = read_len * pl_read_para->size;
        //读取物理设备
        result = PTT_device_tag->read(fp,(uint8_t*)buf,read_len);
        result = result / pl_read_para->nmemb;
    }

    return result;
}
//----测试字符串是否包含分区名-----------------------------------------------
//功能: 测试给定的字符串是否包含分区名
//参数: fullname，待检查的字符串
//返回: 含分区名，则返回分区名的长度，否则返回0
//备注: 内部函数，调用前先确保fullname是合法的字符串。
//----------------------------------------------------------------------------
uint16_t __Djyfs_IsContainPTT_Name(const char *fullname)
{
    uint16_t offset;
    for(offset = 0; offset < CN_PTT_NAME_LIMIT+1; offset++)
    {
        if(fullname[offset] == ':')
            return offset;
        if(fullname[offset] == '\0')
            return 0;
    }
        return 0;
}

//----测试目录串是否绝对路径--------------------------------------------------
//功能: 测试目录串是否绝对路径
//参数: fullname，待检查的字符串
//返回: true = fullname是绝对路径，false = 不是绝对路径
//备注: 内部函数，调用前先确保fullname是合法的字符串。
//----------------------------------------------------------------------------
bool_t __Djyfs_IsAbsPath(const char *fullname)
{
    if(fullname[0] == '\\')
    {
        return true;
    }else
    {
        if(__Djyfs_IsContainPTT_Name(fullname) != 0)
            return true;
        else
            return false;
    }
}

//----测试目录串是否包含文件名--------------------------------------------------
//功能: 测试目录串是否包含文件名
//参数: fullname，待检查的字符串
//返回: true = fullname包含文件名，false = 不包含文件名
//备注: 内部函数，调用前先确保fullname是合法的字符串。
//----------------------------------------------------------------------------
bool_t __Djyfs_IsContainFileName(const char *fullname)
{
    //fullname是已经经过字符串长度合法性检查的指针
    if(fullname[strlen(fullname)-1] != '\\')
        return true;
    else
        return false;
}

//----查看文件(目录)是否已经打开----------------------------------------------
//功能: 查看一个文件(目录)是否已经打开，即看它是否在被打开的文件资源中
//参数: PTT_device_tag，被操作的分区数据结构指针
//      synname，待查的文件(目录)名
//返回: true = 已经打开，false = 未打开(或其他错误)
//备注: 调用前保证fullname是合法的字符串
//----------------------------------------------------------------------------
bool_t __Djyfs_ItemIsOpened(struct tagPTTDevice *PTT_device_tag,
                              char *synname)
{
    struct tagFileRsc *parent,*son;
    u32 offset,name_len,index=0;
    char    name[256];

    if(__Djyfs_IsAbsPath(synname))
    {
        parent = PTT_device_tag->opened_root;  //从根目录开始操作
        offset = 1;
    }else
    {
        parent = g_ptWorkPath;  //从当前路径开始操作
        offset = 0;
    }
    //synname是已经经过字符串长度合法性检查的指针
    name_len = strlen(synname);
    for( ; offset< name_len; offset++)
    {//沿路径逐级查看目录是否已经打开
        if(synname[offset] == '\\')    //遇到字符'\'
        {
            name[index] = '\0';
            index = 0;
            Lock_SempPend(Fs_Semaphore,CN_TIMEOUT_FOREVER);
            if((son=(struct tagFileRsc*)Rsc_SearchSon(&parent->file_node,name))
                                            != NULL)
            {//目标已经打开
                parent = son;   //以当前打开的目录为下次使用的父目录
                Lock_SempPost(Fs_Semaphore);
            }else
            {
                Lock_SempPost(Fs_Semaphore);
                return false;
            }
        }else   //未遇到字符'\'。
        {
            name[index] = synname[offset];     //复制当前字符到文件名串
            index++;
        }
        if(index >= CN_FILE_NAME_LIMIT)    //目录名长度超限。
            return false;
    }
    //至此，如果filename传入的是文件名(末字符不是'\')，name保存的是文件名，
    //index是串长度，串结束符'\0'未赋值，整个路径都已经打开。
    //如果filename出入是目录名，则name保存最后一个目录，且已经打开，index=0。
    if(index != 0)
    {
        name[index] = '\0';
        Lock_SempPend(Fs_Semaphore,CN_TIMEOUT_FOREVER);
        if(Rsc_SearchSon(&parent->file_node,name) != NULL)
        {
            Lock_SempPost(Fs_Semaphore);
            return true;        //文件已经打开
        }else
        {
            Lock_SempPost(Fs_Semaphore);
            return false;       //文件尚未打开
        }
    }else
        return true;
}

//----删除文件(目录)----------------------------------------------------------
//功能：删除一个文件（目录），本函数不能删除非空目录，以及只读文件（目录）。要
//      删除只读文件，只有先把它改为非只读文件才能进行。本函数由file.c内部调用
//参数: PTT_device_tag，被操作的分区数据结构指针
//      synname,文件名或者目录名字符串，文件和目录统称item
//返回：参考文件系统错误代码列表
//----------------------------------------------------------------------------
u32 __Djyfs_RemoveItem(struct tagPTTDevice *PTT_device_tag,char *synname)
{
    struct tagFileRsc item;
    struct tagFileRsc *parent,*son=NULL;
    uint16_t next_char_off;
    char    name[256];

    if(__Djyfs_ItemIsOpened(PTT_device_tag,synname) == true)
    {
        return (u32)EN_FS_OBJECT_OPENED;       //不能删除已经打开的对象
    }
    if(__Djyfs_IsAbsPath(synname))  //synname中已经去掉了分区名，无需再判断
    {
        parent = PTT_device_tag->opened_root;  //从根目录开始操作
        next_char_off = 1;
    }
    else
    {
        parent = g_ptWorkPath;  //从当前路径开始操作
        next_char_off = 0;
    }
    //是根目录，无需删除
       if(strcmp(synname,"\\")==0)
        return EN_FS_PTT_ERROR;

    Lock_SempPend(Fs_Semaphore,CN_TIMEOUT_FOREVER);
    while(__Djyfs_PickWord(synname,next_char_off,name))
    {//沿路径逐级打开目录，但不会打开文件
        //name是模块内部提供的字符串指针，已经经过字符串长度合法性检查
        next_char_off += strlen(name) +1;    //+1是为跳过字符'\'
        if((son = (struct tagFileRsc *)Rsc_SearchSon(&parent->file_node,name))
                    != NULL)
        {//目标已经打开
            parent = son;   //以当前打开的目录为下次使用的父目录
        }
        else
        {//目标文件(目录)尚未打开
            //查找目标并初始化文件数据结构
            if(PTT_device_tag->lookfor_item(name,parent,&item))
            {//目标存在，并且已经初始化目录信息
                item.home_PTT = parent->home_PTT;
                son = &item;   //以当前打开的目标为下次使用的父目录
                Rsc_CleanNode(&son->file_node);
                parent = son;
            }else       //目录不存在，返回false
            {
                Lock_SempPost(Fs_Semaphore);
                return (u32)EN_FS_OBJECT_NONENTITY;   //被删除的对象不存在
            }
        }
        if(next_char_off >= strlen(synname))
            break;
    }
    Lock_SempPost(Fs_Semaphore);
    item.open_counter = 0;     //打开次数清零
    item.home_PTT = PTT_device_tag;
    if(item.file_attr.bits.read_only == true)
        return (u32)EN_FS_OBJECT_READONLY;       //不能删除只读对象
    if(item.file_attr.bits.folder)
    {//是个目录，需要判断是否空
        if(PTT_device_tag->check_folder(&item) != 0)
            return (u32)EN_FS_FOLDER_UNBLANK;
    }
    if(PTT_device_tag->remove_item(&item))
        return EN_FS_NO_ERROR;
    else
        return EN_FS_REMOVE_ERROR;
}

//----目录(文件)改名-----------------------------------------------------------
//功能：把一个目录（文件）改名，不允许修改只读目录（文件）的名字，要修改只读文
//      件，必须先修改只读文件的属性，本函数由file.c内部调用，
//参数: PTT_device_tag，被操作的分区数据结构指针
//      old_synname,文件或目录原名字符串，
//      newname，文件或目录的新名字字符串
//返回：参考文件系统错误代码列表
//----------------------------------------------------------------------------
u32 __Djyfs_PTT_RenameItem(struct tagPTTDevice *PTT_device_tag,
                            char *old_synname,char *newname)
{
    struct tagFileRsc item;
    struct tagFileRsc *parent,*son=NULL;
    uint16_t next_char_off;
    char    name[256];

    if(__Djyfs_ItemIsOpened(PTT_device_tag,old_synname) == true)
    {
        return (u32)EN_FS_OBJECT_OPENED;       //不能改名已经打开的对象
    }
    if(__Djyfs_IsAbsPath(old_synname)) //synname中已经去掉了分区名，无需再判断
    {
        parent = PTT_device_tag->opened_root;  //从根目录开始操作
        next_char_off = 1;
    }else
    {
        parent = g_ptWorkPath;  //从当前路径开始操作
        next_char_off = 0;
    }
    //是根目录，无需修改
    if(strcmp(old_synname,"\\")==0)
        return EN_FS_RENAME_ERROR;
    Lock_SempPend(Fs_Semaphore,CN_TIMEOUT_FOREVER);
    while(__Djyfs_PickWord(old_synname,next_char_off,name))
    {//沿路径逐级打开目录，但不会打开文件
        //name是模块内部提供的字符串指针，已经经过字符串长度合法性检查
    //    printf(name);
        next_char_off += strlen(name) +1;    //+1是为跳过字符'\'
        if((son = (struct tagFileRsc *)Rsc_SearchSon(&parent->file_node,name))
                   != NULL)
        {//目标已经打开
           parent = son;   //以当前打开的目录为下次使用的父目录
        }else
        {//目标文件(目录)尚未打开
            //查找目标并初始化文件数据结构
            if(PTT_device_tag->lookfor_item(name,parent,&item))
            {//目标存在，并且已经初始化目录信息
                item.home_PTT = parent->home_PTT;
                son = &item;   //以当前打开的目标为下次使用的父目录
                Rsc_CleanNode(&son->file_node);
                parent = son;
            }else       //目录不存在，返回false
            {
                Lock_SempPost(Fs_Semaphore);
                return (u32)EN_FS_OBJECT_NONENTITY;   //被改名的对象不存在
            }
        }
        if(next_char_off >= strlen(old_synname))
            break;
    }
    Lock_SempPost(Fs_Semaphore);
    if(item.file_attr.bits.read_only == true)
    {
           return EN_FS_OBJECT_READONLY;   //只读文件不允许改名
    }

    if( PTT_device_tag->rename_item(son,newname))
        return EN_FS_NO_ERROR;
    else
        return EN_FS_RENAME_ERROR;

}


//----关闭文件(目录)----------------------------------------------------------
//功能：关闭一个文件（目录），本函数由file.c内部调用
//参数：PTT_device_tag，被关闭文件（目录）所属分区的数据结构指针
//      file，被关闭的文件（目录）指针
//返回：true=成功关闭则，false=失败
//----------------------------------------------------------------------------
bool_t __Djyfs_PTT_CloseItem(struct tagPTTDevice *PTT_device_tag,struct tagFileRsc *fp)
{
    struct tagFileRsc *parent,*son=NULL;
    Lock_SempPend(Fs_Semaphore,CN_TIMEOUT_FOREVER);
    if(fp->file_attr.bits.folder)
    {
        if(Rsc_GetSon(&fp->file_node) != NULL)
        {
            Lock_SempPost(Fs_Semaphore);
            return false;       //被关闭的目录仍有已经被打开的子项，不能关闭
        }
    }

    //下面把文件从资源链表中删除，包括已经空的目录
    son = fp;
    parent = (struct tagFileRsc *)Rsc_GetParent(&son->file_node);
    do
    {//从被删除结点开始，逐级向上删除空目录。空目录是指资源链表上的空目录结点，
     //而物理存储器上该目录可能不是空的
        if(son == g_ptWorkPath)  //当前工作路径不能被删除
            break;

        //如果是根目录的话，是不能释放的。直接返回，不做处理
        if(parent == PTT_device_tag->opened_root)
        {
            PTT_device_tag->close_item(son);
            if(Rsc_DelNode(&son->file_node) != NULL) //从打开文件资源链表中删除结点
                Mb_Free(s_ptContentPool,son);               //释放结点内存
            if(PTT_device_tag->opened_sum != 0)
                PTT_device_tag->opened_sum --;
            break;
        }
        //当被删除的文件，还有兄弟辈，那么不能释放父节点
        if(Rsc_GetNext(&son->file_node) != &son->file_node)
        {
            PTT_device_tag->close_item(son);
            if(Rsc_DelNode(&son->file_node) != NULL) //从打开文件资源链表中删除结点
                Mb_Free(s_ptContentPool,son);
            if(PTT_device_tag->opened_sum != 0)
                PTT_device_tag->opened_sum --;
            break;
        }
        if(son->open_counter>1)     //lst 20120722,调换本句和下一句if的顺序
        {//该目录多次被打开，故只需要打开次数减1，就达到关闭一次的目的
            son->open_counter--;
            break;
        }
        else
        {
            PTT_device_tag->close_item(son);
            if(son->file_attr.bits.close_delete)
                PTT_device_tag->remove_item(son);
        }
        if(Rsc_DelNode(&son->file_node) != NULL) //从打开文件资源链表中删除结点
            Mb_Free(s_ptContentPool,son);               //释放结点内存

        son = parent;               //下一循环释放父结点
        parent=(struct tagFileRsc*)Rsc_GetParent(&parent->file_node);//父结点上移
        if(PTT_device_tag->opened_sum != 0)
            PTT_device_tag->opened_sum --;
    }while(son != PTT_device_tag->opened_root);    //直到根目录
    Lock_SempPost(Fs_Semaphore);
    return true;
}

//----把文件写入到存储器------------------------------------------------------
//功能: 把文件写缓冲区的数据和buf中的数据一起写入到存储器中，并清空写缓冲区。
//      该函数由file.c内部调用
//参数：PTT_device_tag，被操作文件所属分区的数据结构指针
//      buf，数据缓冲区指针
//      fp，被关闭的文件（目录）指针
//返回：写入存储介质的数据量
//----------------------------------------------------------------------------
u32 __Djyfs_PTT_FlushFile(struct tagPTTDevice *PTT_device_tag,struct tagFileRsc *fp)
{
    u32 result;
    result = PTT_device_tag->flush(fp);
    return result;
}

//----flush分区的所有文件----------------------------------------------------
//功能: 把分区中所有文件的写缓冲区的数据写入到存储器中，并清空写缓冲区。
//      本函数由file.c内部调用
//参数：PTT_device_tag，被操作文件所属分区的数据结构指针
//返回：0
//----------------------------------------------------------------------------
u32 __Djyfs_PTT_FlushAll(struct tagPTTDevice *PTT_device_tag)
{
    struct  tagRscNode  *current,*start,*temp;
    struct  tagFileRsc *item;
    start = &PTT_device_tag->opened_root->file_node;
    current = &PTT_device_tag->opened_root->file_node;
    Lock_SempPend(Fs_Semaphore,CN_TIMEOUT_FOREVER);
    while((temp = Rsc_TraveScion(start,current)) != NULL)
    {//此循环遍历整个分区所有打开的文件和目录
        item = (struct tagFileRsc *)temp;
        if( ! item->file_attr.bits.folder)    //如果是文件，则刷到存储器
            __Djyfs_PTT_FlushFile(PTT_device_tag,item);
        current = temp; //rsc_search_tree函数下次调用的起点
    }
    Lock_SempPost(Fs_Semaphore);
    return 0;
}
//----分区设备左手控制函数-------------------------------------------------
//功能：顾名思义，执行一系列上层发下来的文件系统控制命令
//参数：PTT_device_tag，被操作的分区数据结构指针
//      left_cmd，左手命令字，使用时转换成enum PTT_left_cmd类型
//      data1、data2，跟命令字相关的数据
//返回：与命令字相关，参见源程序注释
//-----------------------------------------------------------------------------
ptu32_t __Djyfs_PTT_AppCtrl(struct tagPTTDevice *PTT_device_tag,u32 left_cmd,
                                ptu32_t data1,ptu32_t data2)
{
    struct tagFileRsc *root_folder;
    switch((enum _PTT_LEFT_CMD_)left_cmd)
    {
        case EN_PTT_FORMAT:
        {//格式化分区，data1、data2是格式化参数
            if(PTT_device_tag->opened_root == NULL)
            {
                //申请根目录资源结点内存
                root_folder = Mb_Malloc(s_ptContentPool,0);
                if(root_folder == NULL)
                {
                    PTT_device_tag->formatted = false;
                    return EN_FS_CREAT_ROOT_FOLDER_ERROR;
                }
                memset(root_folder,0,sizeof(struct tagFileRsc));
                //把新分区的根目录资源节点加入到文件根资源结点下，
                Lock_SempPend(Fs_Semaphore,CN_TIMEOUT_FOREVER);
                Rsc_AddSon(&PTT_device_tag->PTT_node,&root_folder->file_node,
                                  sizeof(struct tagFileRsc),RSC_FILE,PTT_device_tag->name);
                Lock_SempPost(Fs_Semaphore);
                root_folder->home_PTT = PTT_device_tag;
                //分区设备的打开文件的根结点指向该根资源结点。
                PTT_device_tag->opened_root = root_folder;

            }
            if(PTT_device_tag->format(data1,data2,PTT_device_tag))
            {
                PTT_device_tag->opened_root->file_attr.bits.folder = 1;
                PTT_device_tag->opened_root->file_attr.bits.read_only = 1;
                strcpy(PTT_device_tag->opened_root->name,PTT_device_tag->name);
 //               strcat(PTT_device_tag->opened_root->name,":");
                PTT_device_tag->opened_root->home_PTT = PTT_device_tag;
                PTT_device_tag->formatted = true;
                return EN_FS_NO_ERROR;
            }else
            {
                Mb_Free(s_ptContentPool, PTT_device_tag->opened_root);
                PTT_device_tag->formatted = false;
                PTT_device_tag->opened_root = NULL;
                return EN_FS_FORMAT_PTT_ERROR;
            }
        }break; //for EN_PTT_FORMAT
        case EN_CHK_PTT_FORMAT:
        {//查找文件，data1是文件名(目录名)
            return PTT_device_tag->formatted;
        }break; //for EN_CHK_PTT_FORMAT
        case EN_PTT_LOOKFOR:
        {//查找文件，data1是文件名(目录名)
            return (ptu32_t)__Djyfs_LookforItem(PTT_device_tag,
                                                    (char*)data1,
                                                    (struct stat*)data2);
        }break; //for EN_PTT_LOOKFOR
        case EN_PTT_OPEN:
        {//打开文件，data1是文件名，data2是模式字符串，返回文件指针
            return (ptu32_t)__Djyfs_OpenFile(PTT_device_tag,(char*)data1,(char*)data2);
        }break; //for EN_PTT_OPEN
        case EN_PTT_REMOVE:
        {//data1是文件（目录）名，data2不用
            return (ptu32_t)__Djyfs_RemoveItem(PTT_device_tag,(char*)data1);
        }break;     //for EN_PTT_REMOVE
        case EN_PTT_RENAME:
        {//data1是原文件（目录）名，data2是新文件名
            return (ptu32_t)__Djyfs_PTT_RenameItem(PTT_device_tag,(char*)data1,(char*)data2);
        }break; //for EN_PTT_RENAME
        case EN_PTT_CLOSE:
        {
            return (ptu32_t)__Djyfs_PTT_CloseItem(PTT_device_tag,(struct tagFileRsc*)data1);
        }break; //for EN_PTT_CLOSE
        case EN_PTT_FLUSH:
        {
            if(__Djyfs_PTT_FlushFile(PTT_device_tag,(struct tagFileRsc *)data1)==0)
                return (ptu32_t)EOF;
            else
                return 0;
        } break; //for EN_PTT_FLUSH
        case EN_PTT_SEEK:
        {
            return (ptu32_t)PTT_device_tag->seek_file((struct tagFileRsc *)data2,
                                                     (struct tagSeekPara *)data1);
        } break; //for EN_PTT_SEEK
        case EN_PTT_TRUNCATE:
        {
            return (ptu32_t)PTT_device_tag->set_file_size((struct tagFileRsc *)data2,
                                                     *(s64*)data1);
        } break; //for EN_PTT_TRUNCATE
        case EN_PTT_TRAVERSAL_SON:
        {
            return (ptu32_t)PTT_device_tag->item_traversal_son(
                            (struct tagFileRsc *)data1,(struct tagFileRsc *)data2);
        } break; //for EN_PTT_TRAVERSAL_SON
        case EN_QUERY_FILE_CUBAGE:
        {
            return PTT_device_tag->query_file_cubage((struct tagFileRsc *)data2);
        }
        default : break;
    }
    return 0;
}

//----分区设备右手控制函数-------------------------------------------------
//功能：顾名思义，执行一系列存储设备driver发出的文件系统控制命令
//参数：PTT_device_tag，被操作的分区数据结构指针
//      right_cmd，右手命令字，使用时转换成enum PTT_right_cmd类型
//      data1、data2，跟命令字相关的数据
//返回：与命令字相关，参见源程序注释
//-----------------------------------------------------------------------------
ptu32_t __Djyfs_PTT_MemoryCtrl(struct tagPTTDevice *PTT_device_tag,u32 right_cmd,
                                ptu32_t data1,ptu32_t data2)
{
    data2 = data2;        //消除编译器告警
    switch((enum _PTT_RIGHT_CMD_)right_cmd)
    {
        case CN_PTT_CLR_READBUF:
        {//清除文件的读缓冲区，data1是文件指针
            if(((struct tagFileRsc*)data1)->p_read_buf != NULL)
               Ring_Flush(((struct tagFileRsc*)data1)->p_read_buf);
            return 0;
        }break; //for CN_PTT_CLR_READBUF
        case CN_PTT_CLR_WRITEBUF:
        {//清除文件的写缓冲区，data1是文件指针
            if(((struct tagFileRsc*)data1)->p_write_buf != NULL)
               Ring_Flush(((struct tagFileRsc*)data1)->p_write_buf);
            return 0;
        }break; //for CN_PTT_CLR_WRITEBUF
        case CN_PTT_FLUSH_ONE:
        {//
            return (ptu32_t)__Djyfs_PTT_FlushFile(PTT_device_tag,(struct tagFileRsc *)data1);
        }break; //for CN_PTT_FLUSH_ONE
        case CN_PTT_FLUSH_ALL:
        {//
            return (ptu32_t)__Djyfs_PTT_FlushAll(PTT_device_tag);
        }break; //for CN_PTT_FLUSH_ALL
        case CN_PTT_UPDATE_MEDIUM_TAG:
        {//修改(设置)一个文件的媒体标记，data1为文件指针，data2为新标记
            ;
        }break; //for CN_PTT_UPDATE_MEDIUM_TAG
        case CN_PTT_SEARCH_MEDIUM_TAG:
        {//搜索一个文件的媒体标记，data1为该文件的媒体标记，返回文件指针
            ;
        }break; //for CN_PTT_SEARCH_MEDIUM_TAG
        default:break;
    }
    return 0;
}

//----打开分区接口------------------------------------------------------
//功能: 根据文件(目录)名，打开与之对应的分区接口。
//参数: fullname，文件(目录)名指针
//返回：设备句柄，出现下列情况返回NULL:
//      1.分区不存在  2.fullname和pg_work_path_name均没有指定分区名
//----------------------------------------------------------------------------
struct tagPTTDevice *__Djyfs_OpenPTT_Left(const char *fullname)
{
    struct tagPTTDevice *PTT_device_tag;
    char PTT_name[256];
    uint16_t name_len;

    name_len = __Djyfs_IsContainPTT_Name(fullname);
    if(name_len != 0)
    {//fullname是包括分区名在内的全路径(也可以是当前分区)
        memcpy(PTT_name,fullname,name_len); //从参数中copy分区名
        PTT_name[name_len] = '\0';          //写入串结束符
    }else
    {
        //fullname中没有指定分区名，应从当前工作路径中取
        if(!Djyfs_GetPTT_Name(PTT_name,CN_FILE_NAME_LIMIT))
            return NULL;
    }
    //获取分区数据结构指针
    Lock_SempPend(Fs_Semaphore,CN_TIMEOUT_FOREVER);
    PTT_device_tag = (struct tagPTTDevice *)Rsc_SearchSon(&s_tFileSystemRoot,PTT_name);
    Lock_SempPost(Fs_Semaphore);
    return PTT_device_tag;
}

//----从目录中取一字-----------------------------------------------------------
//功能: 从一个可能包含多级路径名和文件名的字符串curent_offset位置开始取出一个
//      由'\'分隔的word
//参数: synname，输入字符串，格式如"\aaa\bbb\ccc.txt"
//      start_offset，搜索起始位置指针
//      name，保存取得的字符串的缓冲区指针
//返回: true=成功
//      false=失败，1、已经完成最后一个目录字
//                  2、synname中根本就不包含目录字
//                  3、字长超过255字符
//      在*name中返回取得的字符串
//备注: synname应该是经过合法性检查的字符串，不会出现word超长的问题
//------------
//更新记录:
// 1.日期: 2015/3/19
//   说明: 1.路径名段(从current_offset算起)"\",返回false。
//         2.路径名段(从current_offset算起)为空字符,返回false;
//   作者: 季兆林
//-----------------------------------------------------------------------------
bool_t __Djyfs_PickWord(const char *synname,u32 curent_offset,char *name)
{
    u32 index = 0;
    //synname是已经经过字符串长度合法性检查的指针
    u32 name_len = strlen(synname);
    u32 offset = curent_offset;

    if (('\\' == synname[curent_offset]) && ('\0' == synname[curent_offset+1]))
        return (false);//路径名段为"\"
    if ('\0' == synname[curent_offset])
        return (false);//路径名段为空字符

    for( ; offset< name_len; offset++)
    {
        if(synname[offset] == '\\')
        {   //遇到字符'\\'
            name[index] = '\0';
            return true;
        }
        else
        {   //未遇到字符'\\'
            if(index == CN_FILE_NAME_LIMIT)
                return false;
            name[index] = synname[offset]; //复制当前字符到文件名串
            index++;
        }
    }

    name[index] = '\0';//文件(非目录)的截止符号。
    return true; //若正确提取目录字，程序不会运行至此

}

//----取一目录名字-------------------------------------------------------------
//功能: 从一个可能包含多级路径名和文件名的字符串中curent_offset位置开始取出一个
//      目录名
//参数: synname，输入字符串，格式如"\aaa\bbb\ccc.txt"
//      start_offset，搜索起始位置指针
//      name，保存取得的字符串的缓冲区指针
//返回: true=成功
//      false=失败，1、已经完成最后一个目录字
//                  2、synname中根本就不包含目录字
//                  3、字长超过255字符
//      在*name中返回取得的字符串
//备注: synname应该是经过合法性检查的字符串，不会出现word超长的问题
//-----------------------------------------------------------------------------
bool_t __Djyfs_PickPathWord(const char *synname,u32 curent_offset,char *name)
{
    u32 index = 0;
    //synname是已经经过字符串长度合法性检查的指针
    u32 name_len = strlen(synname);
    u32 offset = curent_offset;
    for( ; offset< name_len; offset++)
    {
        if(synname[offset] == '\\')    //遇到字符'\'
        {
            name[index] = '\0';
            return true;
        }else   //未遇到字符'\'。
        {
            if(index == CN_FILE_NAME_LIMIT)
                return false;
            name[index] = synname[offset];     //复制当前字符到文件名串
            index++;
        }
    }
    return false;   //若正确提取目录字，程序不会运行至此
}

//----取文件名串---------------------------------------------------------------
//功能: 从一个可能包含多级路径名和文件名的字符串中取出文件名
//参数: synname，输入字符串，格式如"\aaa\bbb\ccc.txt"
//      start_offset，搜索起始位置指针
//      name，保存取得的字符串的缓冲区指针
//返回: true=成功
//      false=失败，synname中不包含文件名
//      在*name中返回取得的字符串
//备注: synname应该是经过合法性检查的字符串，不会出现word超长的问题
//-----------------------------------------------------------------------------
bool_t __Djyfs_PickFileNameWord(const char *synname,char *name)
{
    u32 index = 0;
    //synname是已经经过字符串长度合法性检查的指针
    u32 name_len = strlen(synname);
    u32 offset;
    for(offset = name_len; offset >0; offset--)
    {//从最后一个字符搜索字符'\'
        if(synname[offset-1] == '\\')    //遇到字符'\'
            break;
    }
    if(offset == name_len)      //最后一个字符就是'\'，串中没有文件名
        return false;
    else if((name_len -offset) > CN_FILE_NAME_LIMIT)    //文件名超长
        return false;
    for( ; offset < name_len; offset++)
    {
        name[index] = synname[offset];
        index++;
    }
    name[index] = '\0';
    return true;
}

//----忽略路径中的分区部分---------------------------------------------------
//功能: 忽略文件(目录)名字符串中关于分区的部分，提取synname，比如，
//      输入"c:\aaa\bbb\ccc.txt"，将返回"\aaa\bbb\ccc.txt"
//参数: fullname,输入字符串
//返回: 取得的路径串
//----------------------------------------------------------------------------
char *__Djyfs_PickSynName(const char *fullname)
{
    uint16_t name_len;

    name_len = __Djyfs_IsContainPTT_Name(fullname);
    if(name_len != 0)//绝对路径
           return (char *)fullname + name_len +1;
    else
        return (char *)fullname;//相对路径，返回原来的字符串
}


//----检查路径名是否合法------------------------------------------------------
//功能: 检查路径名是否合法，与windows兼容，暂不考虑网络路径名，方法如下:
//      1.由字符':'分隔的分区名长度必须合法，':'后面必须是字符'\'
//      2.不允许出现连续两个'\'。
//      3.不允许出现'* ? / | < > "'这7个字符,这些字符可能用于特殊用途
//      4.不允许出现长度超过255字符的object
//参数: fullname,被测字符串
//返回: true=合法，false=非法
//----------------------------------------------------------------------------
bool_t __Djyfs_CheckFullName(const char *fullname)
{
    u32 colon_offset = CN_LIMIT_UINT32,syn_offset;
    u32 loop,temp;
    ufast_t depth = 0;
    if(fullname == NULL)
        return false;
    //无需使用strlen检查fullname长度，因为接下来的所有检查项目都在限定长度内
    //操作。
    for(loop = 0; loop <= CN_PTT_NAME_LIMIT; loop++)
    {//检查串中第一个':'出现在什么位置
        if((fullname[loop]=='*')||(fullname[loop]=='/')||(fullname[loop]=='?')
           ||(fullname[loop]=='<')||(fullname[loop]=='>')||(fullname[loop]=='|')
           ||(fullname[loop]=='"'))
            return false;   //名称串中不能出现这几个字符的
        if(fullname[loop] == ':')
        {
            colon_offset = loop; //第一个':'出现的位置，用于检查分区名长度
            break;
        }

        if(fullname[loop] == '\\')   //出现word分隔符。
            break;
        if(fullname[loop] == '\0')    //出现停止符号
            break;
    }
    if(colon_offset == 0)
        return false;       //首字符不能是':'
    if(colon_offset != CN_LIMIT_UINT32) //':'位置合法(含分区名且长度合法)
    {
        if(fullname[colon_offset+1] != '\\')
            return false;       //冒号后面必须是字符'\'
        syn_offset = colon_offset + 2;
    }else if(fullname[0] =='\\')
        syn_offset = 1;
    else
        syn_offset = 0;
    temp = 0;
    //检查有没有超过cn_file_name_limit字符的object，并检查有没有非法字符
    //不算分区名，全路径所包含的字符个数不会超过
    //CN_PATH_DEPTH_LIMIT * (CN_FILE_NAME_LIMIT+1),+1是包含word分隔符'\'
    for(loop = syn_offset;
               (loop < CN_PATH_DEPTH_LIMIT*(CN_FILE_NAME_LIMIT+1)+syn_offset);
               loop++)
    {
        //从分区名后面开始继续检查非法字符，此时连冒号':'也是非法字符
        if((fullname[loop]=='*')||(fullname[loop]=='/')||(fullname[loop]=='?')
           ||(fullname[loop]=='<')||(fullname[loop]=='>')||(fullname[loop]=='|')
           ||(fullname[loop]=='"')||(fullname[loop]==':'))
            return false;   //含有这几个非法字符，
        if(fullname[loop] == '\0')
            return true;
        if(fullname[loop] == '\\')
        {
            if(temp == 0)
                return false;       //说明遇到连续的word分隔符'\'
            temp = 0;//文件的长度清零，重新开始计算
            depth++;
        }else
        {
            if(depth >= CN_PATH_DEPTH_LIMIT)    //路径过深
                return false;
            temp++;
            if(temp > CN_FILE_NAME_LIMIT)       //word长度超限
                return false;
        }
    }
    return false;       //能跑到这里，肯定没有找到串结束符
}

//----检查单项名是否合法------------------------------------------------------
//功能: 检查一个单项的名字是否合法，即不能含'\/?:*'这几个字符
//参数: word,被测字符串，可能是单一路径名，也可能是文件名
//返回: true=合法，false=非法
//----------------------------------------------------------------------------
bool_t __check_word(const char *word)
{
    u32 loop;
    //word是已经经过字符串长度合法性检查的指针
    for(loop = 0; loop < strlen(word); loop++)
    {
        if((word[loop]=='*')||(word[loop]=='/')
                        ||(word[loop]=='?')||(word[loop] == ':'))
            return false;   //含有这几个非法字符，
    }
    return true;
}

//----获取文件信息-------------------------------------------------------------
//功能: 根据一个文件指针，取得该文件的一些信息
//参数: fp，文件指针
//      fp_info，用于返回信息的指针。
//返回: 根目录指针
//------------
//更新记录:
// 1.日期: 2015/3/25
//   说明: 更改fp_info结构体
//   作者: 季兆林
//-----------------------------------------------------------------------------
bool_t Djyfs_Fstat(struct tagFileRsc *fp,struct stat *fp_info)
{

    if((fp == NULL) || (fp_info == NULL))
        return false;
    fp_info->st_mode = fp->file_attr;
    fp_info->open_mode = fp->open_mode;

    fp_info->CreateTime = fp->CreateTime;
    fp_info->ModTime = fp->ModTime;
    fp_info->file_size = fp->file_size;

    return true;
}
//----获取根目录---------------------------------------------------------------
//功能: 根据一个文件指针，取得该文件指针所在的分区的根目录指针
//参数: fp，文件指针
//返回: 根目录指针
//-----------------------------------------------------------------------------
struct tagFileRsc *Djyfs_GetRootFolder(struct tagFileRsc *fp)
{
    struct tagPTTDevice *PTT_device_tag;
    //fullname不是一个合法的字符串
    if(fp == NULL)
        return false;
    PTT_device_tag = (struct tagPTTDevice*)(fp->home_PTT);
    if(PTT_device_tag->formatted)
    {
        return PTT_device_tag->opened_root;
    }else
        return NULL;
}
//----取分区信息-------------------------------------------------------------
//功能: 取一个分区的信息，总空间，空闲空间等
//参数: fullname，路径名。如fullname中包含了分区名，则取该分区的信息；如果没包含，
//          则取当前路径所在分区信息，若当前路径又没有设置，返回false。若只想获
//          取当前路径所在分区信息，可给NULL
//      sum,valid,free,用于返回数据，可以是NULL
//返回: true = 正确获取，false = 错误，可能参数错误，也可能没格式化
//------------
//更新记录:
// 1.日期: 2015/3/25
//   说明: 更新函数实现
//   作者: 季兆林
//-----------------------------------------------------------------------------
bool_t Djyfs_PTT_Stat(const char *fullname,s64 *sum,s64 *valid,s64 *free)
{

    struct tagRscNode *FsRoot, *Fs;

    //fullname不是一个合法的字符串
    if(__Djyfs_CheckFullName(fullname) == false)
    {
        printf("分区名参数不合法.\r\n");
        return (false);
    }

    FsRoot = Rsc_SearchTree("fs");
    if( NULL == FsRoot )
    {
        printf("系统中未安装文件系统模块\r\n");
        return (false);
    }

    /* 检索文件系统下各分区（具体文件系统） */
    Fs = FsRoot;
    do
    {
        Fs = Rsc_TraveSon(FsRoot, Fs);
        if(NULL != Fs) {
            if(0 == strcmp(Fs->name, fullname))
            {
            	((struct tagPTTDevice *)Fs)->check_PTT(
            			(struct tagPTTDevice *)Fs, sum, valid, free);

                return (true);
            }
        }
    }while(NULL != Fs);

    printf("分区不存在\r\n");
    return (false);

}

//----取分区信息-------------------------------------------------------------
//功能: 取fp所在的分区的信息，总空间，有效空间、空闲空间
//参数: fp，文件指针
//      sum,valid,free,用于返回数据，可以是NULL
//返回: true = 正确获取，false = 错误，可能参数错误，也可能没格式化
//-----------------------------------------------------------------------------
bool_t Djyfs_PTT_StatFp(struct tagFileRsc * fp,s64 *sum,s64 *valid,s64 *free)
{
    bool_t result;
    struct tagPTTDevice *PTT_device_tag;
    //fullname不是一个合法的字符串
    if(fp == NULL)
        return false;
    PTT_device_tag = (struct tagPTTDevice*)(fp->home_PTT);
    if(PTT_device_tag->formatted)
    {
        PTT_device_tag->check_PTT(PTT_device_tag,sum,valid,free);
        result = true;
    }else
        result = false;
    return result;
}

//----取绝对路径字符串长度-----------------------------------------------------
//功能: 如名。
//参数: fp，目标文件
//返回: 字符串长度，含串结束符'\0'
//-----------------------------------------------------------------------------
u32  Djyfs_GetPathLen(struct tagFileRsc * fp)
{
    struct tagFileRsc *current_fp;
    struct tagPTTDevice *PTT_device_tag;
    u32 len = 0,index;

    if(fp == NULL)
        return 0;

    PTT_device_tag = (struct tagPTTDevice *)(fp->home_PTT);

    current_fp = fp;
    Lock_SempPend(Fs_Semaphore,CN_TIMEOUT_FOREVER);
    for(index = 0; index<CN_PATH_DEPTH_LIMIT; index++)
    {
        len += strlen(current_fp->name);
        len+=2;      //字符'\'，以及最后一级的'\0'的位置
        if(current_fp == PTT_device_tag->opened_root)
            break;
        current_fp = (struct tagFileRsc *)Rsc_GetParent(
                                    (struct tagRscNode *)current_fp);
    }
    Lock_SempPost(Fs_Semaphore);
    return len+1;    //+1是分区名后的“:”
}

//----取绝对路径-----------------------------------------------------------
//功能: 取指定文件的绝对路径到buf中，最大长度为maxlen。
//      buf为NULL则动态分配buf所需内存，调用方须在使用完毕后释放内存。djyos强烈
//      不推荐这种方式，仅为了与传统函数保持兼容才保留这种方式.djyfs提供替代方案
//      是:先调用djyfs_getpath_len获取目录长度，用户自己根据长度分配内存，再调用
//      djyfs_getpath，就可以让"malloc--free"这对函数出现在同一个函数内。
//参数: fp，目标文件
//      buf，接收路径名的字符串缓冲区。
//      maxlen，长度限制，含串结束符'\0'。
//返回: true = 成功获取，false = 失败
//-----------------------------------------------------------------------------
bool_t Djyfs_GetPath(struct tagFileRsc * fp,char *buf,u32 maxlen)
{
    struct tagFileRsc *current_fp;
    struct tagPTTDevice *PTT_device_tag;
    u32 index=0,len = 0;
    struct tagRscNode *point_num[CN_PATH_DEPTH_LIMIT];

    if((fp == NULL) || (maxlen == 0))
        return false;
    if(buf == NULL)
    {
        maxlen = Djyfs_GetPathLen(fp);
        if((buf = M_MallocLc(maxlen,0)) == NULL)
            return false;
        buf[0] = '\0';
    }
    else
        buf[0] = '\0';
    PTT_device_tag = (struct tagPTTDevice *)(g_ptWorkPath->home_PTT);
    current_fp = fp;

    Lock_SempPend(Fs_Semaphore,CN_TIMEOUT_FOREVER);
    for(index = 0; index<CN_PATH_DEPTH_LIMIT; index++)
    {
        point_num[index] = (struct tagRscNode *)current_fp;
        if(current_fp == PTT_device_tag->opened_root)
            break;
        current_fp = (struct tagFileRsc *)Rsc_GetParent(
                                    (struct tagRscNode *)current_fp);
    }
    Lock_SempPost(Fs_Semaphore);
    len = strlen(point_num[0]->name) + 2;
    if(len < maxlen)
    {
        strcat(buf,point_num[0]->name);
        strcat(buf,":\\");
    }

    for( ; index>0; index--)
    {
        len += strlen(point_num[index]->name) +1;
        if(len < maxlen)
        {
            strcat(buf,point_num[index]->name);
            strcat(buf,"\\");
        }
        else
            break;
    }
    return true;
}

//----取当前绝对路径字符串长度-------------------------------------------------
//功能: 如名。
//参数: 无
//返回: 字符串长度，含串结束符'\0'
//-----------------------------------------------------------------------------
u32  Djyfs_GetCwdLen(void)
{
    return Djyfs_GetPathLen(g_ptWorkPath);
}

//----取当前绝对路径-----------------------------------------------------------
//功能: 取当前的绝对路径到buf中，最大长度为maxlen。
//      buf为NULL则动态分配buf所需内存，调用方须在使用完毕后释放内存。djyos强烈
//      不推荐这种方式，仅为了与传统函数保持兼容才保留这种方式.djyfs提供替代方案
//      是:先调用djyfs_getpath_len获取目录长度，用户自己根据长度分配内存，再调用
//      djyfs_getpath，就可以让"malloc--free"这对函数出现在同一个函数内。
//参数: buf，接收路径名的字符串缓冲区。
//      maxlen，长度限制，含串结束符'\0'。
//返回: true = 成功获取，false = 失败
//-----------------------------------------------------------------------------
bool_t Djyfs_GetCwd(char *buf,u32 maxlen)
{
    if(maxlen == 0)
        return false;
    return Djyfs_GetPath(g_ptWorkPath,buf,maxlen);
}

//----遍历一个目录下的子目录(文件)---------------------------------------------
//功能: 这是文件系统提供的一个API调用，用于获取文件（目录）是信息,仅获取信息，
//      文件并没有被打开。
//参数: parent,父目录的信息
//      current，当前查找位置，空则表示第一次查找。
//返回: current的下一个文件
//备注: 这个考虑是否要搞成windows类似的FindFirstFile等三种API。到时候增加
//      api的时候再说。
//-----------------------------------------------------------------------------
FILE *Djyfs_FolderTraversalSon(struct tagFileRsc *parent,
                                       struct tagFileRsc *current)
{
    struct tagFileRsc * result;
    struct tagPTTDevice *PTT_device_tag;

    if(parent == NULL)
        return NULL;
    PTT_device_tag = (struct tagPTTDevice *)parent->home_PTT;
    Lock_MutexPend(PTT_device_tag->chip_mutex_tag,CN_TIMEOUT_FOREVER);
    if(! parent->file_attr.bits.folder)
    {
        result = NULL;         //文件不能遍历
        goto goto_end;
    }
    result = (struct tagFileRsc *)__Djyfs_PTT_AppCtrl(PTT_device_tag,EN_PTT_TRAVERSAL_SON,
                                    (ptu32_t)parent,(ptu32_t)current);

goto_end:
    Lock_MutexPost(PTT_device_tag->chip_mutex_tag);
    return result;

}

//----查找文件----------------------------------------------------------------
//功能: 这是文件系统提供的一个API调用，用于检查文件（目录）是否存在,存在且buf非
//      空，则通过buf返回文件信息
//参数: fullname,文件（目录）名
//      buf,非空则返回文件信息
//返回: -1表示失败
//------------
//更新记录:
// 1.日期: 2015/3/25
//   说明: 更新函数实现
//   作者: 季兆林
//-----------------------------------------------------------------------------

s32 stat(const char *filename, struct stat *buf)
{
    bool_t result;
    struct tagPTTDevice *PTT_device_tag;
    char *synname;

    if(filename == NULL)
        return -1; //此处考虑增加errno = ENOENT,文件名不存在。
    if(!__Djyfs_CheckFullName(filename))
        return -1; //此处考虑增加errno = ENOENT,文件名不存在。

    PTT_device_tag = __Djyfs_OpenPTT_Left(filename);
    if(PTT_device_tag == NULL)
        return -1; //分区不存在，或fullname和当前路径均没有指定分区名
    Lock_MutexPend(PTT_device_tag->chip_mutex_tag,CN_TIMEOUT_FOREVER);
    if(PTT_device_tag->formatted)
    {
        synname = __Djyfs_PickSynName(filename);
       //调用分区控制函数
       result = (bool_t)__Djyfs_PTT_AppCtrl(PTT_device_tag, EN_PTT_LOOKFOR,
                                        (ptu32_t)synname,(ptu32_t)buf);
    }
    else
        result = false;
    Lock_MutexPost(PTT_device_tag->chip_mutex_tag);
    if(result)
        return 0;
    else
        return -1;
}
//-----------------------------------------------------------------------------
//功能: 测试文件是否存在，以及访问权限。如果被查找的项目是目录，则直接返回文件存在。
//参数: fullname，文件名
//     flags，被测试的访问权限，0则查找文件是否存在。
//         F_OK        0   // does file exist
//         X_OK        1   // is it executable by caller
//         W_OK        2   // is it writable by caller
//         R_OK        4   // is it readable by caller
//输出: 无
//返回: 0=文件存在且flags表示的访问权限为真。-1=文件不存在，或无访问权限。
//------------
//更新记录:
// 1.日期: 2014/12/08
//   说明: 创建
//   作者: 罗侍田
//-----------------------------------------------------------------------------
s32 access(const char *fullname, int flags)
{
  struct stat s;
  if (stat(fullname, &s))
    return -1;
  if (s.st_mode.bits.folder == 1)
    return 0;
  if (flags & W_OK)
  {
    if (s.st_mode.bits.read_only)
      return -1;
    return 0;
  }
  return 0;
}

//-----------------------------------------------------------------------------
//功能: 清除一个文件的出错状态。
//参数: fp,目标文件
//输出: 无
//返回: 无
//------------
//更新记录:
// 1.日期: 2014/12/09
//   说明: 创建
//   作者: 罗侍田
//-----------------------------------------------------------------------------
void clearerr(FILE *fp)
{
    if(fp == NULL)
        return;
    fp->eno = EN_FS_NO_ERROR;
}

//----查询文件可写大小---------------------------------------------------------
//功能: 这是文件系统提供的一个API调用，用于查询文件可写入的实际大小
//参数: fp，被写的文件
//返回: 可写入的大小
//-----------------------------------------------------------------------------
size_t Djyfs_Fcubage(FILE *fp)
{
    size_t result;
    struct tagPTTDevice *PTT_device_tag;
    if(fp == NULL)
        return 0;
    PTT_device_tag = (struct tagPTTDevice *)fp->home_PTT;
    Lock_MutexPend(PTT_device_tag->chip_mutex_tag,CN_TIMEOUT_FOREVER);
    if(fp->file_attr.bits.folder)
    { //目录不能写
        result = 0;
        goto goto_end;
    }
    if(fp->open_mode == EN_R_RB)
    {//只读文件，不能写
        result = 0;
        goto goto_end;
    }
    result = __Djyfs_PTT_AppCtrl(PTT_device_tag,EN_QUERY_FILE_CUBAGE,
                                (ptu32_t)0,(ptu32_t)fp);
goto_end:
    Lock_MutexPost(PTT_device_tag->chip_mutex_tag);
    return result;
}

//----改变文件长度-------------------------------------------------------------
//功能: 改变文件长度，如果文件可写方式打开，且新长度大于文件尺寸，
//      将导致文件长度增加。
//参数：fp，被操作的文件指针
//      offset，移动的位置
//      whence，从何处开始计算移动长度，参见SEEK_CUR等的定义
//返回：0=成功，-1=失败
//-----------------------------------------------------------------------------
u32 Djyfs_Ftruncate (FILE *fp, sint64_t offset)
{
    struct tagPTTDevice *PTT_device_tag;
    u32 result;

    if(fp == NULL)
        return CN_LIMIT_UINT32;
    PTT_device_tag = (struct tagPTTDevice *)fp->home_PTT;
    Lock_MutexPend(PTT_device_tag->chip_mutex_tag,CN_TIMEOUT_FOREVER);
    if( (fp->file_attr.bits.folder)
        ||(fp->open_mode == EN_R_RB)
        ||(fp->open_mode == EN_A_AB)
        ||(fp->open_mode == EN_A_AB_PLUS)
        ||(fp->file_size == offset) )
    {//目录没有不能设置长度
        result = CN_LIMIT_UINT32;
        goto goto_end;
    }

    result = __Djyfs_PTT_AppCtrl(PTT_device_tag,EN_PTT_TRUNCATE ,
                                (ptu32_t)&offset,(ptu32_t)fp);
goto_end:
    Lock_MutexPost(PTT_device_tag->chip_mutex_tag);
    return result;
}

//----格式化分区-------------------------------------------------------------
//功能: 格式化分区
//参数: format_para1，格式化参数
//      format_para1，格式化参数
//      ptt_name，被格式化的分区名字
//返回: true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t Djyfs_Format(u32 format_para1,u32 format_para2,char *PTT_name)
{
    u32 name_len,loop;
    bool_t result;
    struct tagPTTDevice *PTT_device_tag;
    if(PTT_name == NULL)
        return false;
    name_len = strnlen(PTT_name,CN_PTT_NAME_LIMIT+1);
    if((name_len == 0) || (name_len > CN_PTT_NAME_LIMIT))
        return false;
    for(loop = 0;loop < name_len; loop++)
    {
        if((PTT_name[loop]=='*')||(PTT_name[loop]=='/')||(PTT_name[loop]=='?')
           ||(PTT_name[loop]=='<')||(PTT_name[loop]=='>')||(PTT_name[loop]=='|')
           ||(PTT_name[loop]=='"')||(PTT_name[loop]==':'))
            return false;   //名称串中不能出现这几个字符的
    }
    Lock_SempPend(Fs_Semaphore,CN_TIMEOUT_FOREVER);
    PTT_device_tag = (struct tagPTTDevice *)Rsc_SearchSon(&s_tFileSystemRoot,PTT_name);
    Lock_SempPost(Fs_Semaphore);
    if(PTT_device_tag == NULL)
        return false;
    Lock_MutexPend(PTT_device_tag->chip_mutex_tag,CN_TIMEOUT_FOREVER);
    if(PTT_device_tag->opened_sum != 0)
        result = false;
    else
    {
        if(EN_FS_NO_ERROR == __Djyfs_PTT_AppCtrl(PTT_device_tag,EN_PTT_FORMAT,
                                       format_para1,format_para2))
        {
            result = true;
        }
        else
            result = false;
    }
    Lock_MutexPost(PTT_device_tag->chip_mutex_tag);
    return result;
}
//----关闭文件-----------------------------------------------------------------
//功能: 这是文件系统提供的一个API调用，用于关闭打来的文件
//参数: fp，被关闭的文件
//返回: 0=成功，cn_limit_uint32=失败。
//-----------------------------------------------------------------------------
u32 fclose(FILE *fp)
{
    bool_t result;
    struct tagPTTDevice *PTT_device_tag;
    if(fp == NULL)
        return CN_LIMIT_UINT32;
    PTT_device_tag = (struct tagPTTDevice *)fp->home_PTT;
    Lock_MutexPend(PTT_device_tag->chip_mutex_tag,CN_TIMEOUT_FOREVER);
     //根目录和当前工作路径，无需关闭
    if(fp == PTT_device_tag->opened_root || fp == g_ptWorkPath)
    {
        Lock_MutexPost(PTT_device_tag->chip_mutex_tag);
        return true;
    }

    if(fp->open_counter>1)
    {//该文件多次被打开，故只需要打开次数减1，就达到关闭一次的目的
        fp->open_counter--;
        Lock_MutexPost(PTT_device_tag->chip_mutex_tag);
        return 0;
    }
    result = (bool_t)__Djyfs_PTT_AppCtrl(PTT_device_tag,EN_PTT_CLOSE,(ptu32_t)fp,0);
    Lock_MutexPost(PTT_device_tag->chip_mutex_tag);
    if(result)
    {
        //防止多次关闭
        fp = NULL;
        return 0;
    }
    else
        return CN_LIMIT_UINT32;
}

//-----------------------------------------------------------------------------
//功能: 检查文件指针是否达到结尾。
//参数: fp,目标文件
//输出: 无
//返回: 0 = 文件未结束，-1 = 参数检查出错，1 = 文件未结束
//------------
//更新记录:
// 1.日期: 2014/12/09
//   说明: 创建
//   作者: 罗侍田
//-----------------------------------------------------------------------------
s32 feof(FILE *fp)
{
    if(fp == NULL)
        return -1;
    if(fp->file_size == fp->read_ptr)
        return 1;
    else
        return 0;
}

//-----------------------------------------------------------------------------
//功能: 获取文件的最后一次访问错误编码。
//参数: fp,目标文件
//输出: 无
//返回: 该文件最后一次访问错误编码。
//------------
//更新记录:
// 1.日期: 2014/12/09
//   说明: 创建
//   作者: 罗侍田
//-----------------------------------------------------------------------------
s32 ferror(FILE *fp)
{
    if(fp == NULL)
        return -1;
    return fp->eno;
}

//----刷新文件-----------------------------------------------------------------
//功能: 这是文件系统提供的一个API调用，用于从打来的文件读数据
//参数: fp，被刷新的文件
//返回: 0=成功，指定的流没有缓冲区或者只读打开时也返回0值。返回EOF指出一个错误
//-----------------------------------------------------------------------------
u32 fflush(FILE *fp)
{
    struct tagPTTDevice *PTT_device_tag;
    size_t result;
    if(fp == NULL)
        return EOF;
    PTT_device_tag = (struct tagPTTDevice *)fp->home_PTT;
    Lock_MutexPend(PTT_device_tag->chip_mutex_tag,CN_TIMEOUT_FOREVER);
    if(fp->file_attr.bits.folder)
    {   //目录不能写
        result = EOF;
        goto goto_end;
    }
    if(fp->open_mode == EN_R_RB)
    {//只读文件，无需刷新
        result = EOF;
        goto goto_end;
    }
    result = __Djyfs_PTT_AppCtrl(PTT_device_tag,EN_PTT_FLUSH,
                                (ptu32_t)fp,0);
goto_end:
    Lock_MutexPost(PTT_device_tag->chip_mutex_tag);
    return result;
}

//----打开文件----------------------------------------------------------------
//功能: 这是文件系统提供的一个API调用，用于打开文件系统中的一个文件
//参数: fullname,文件名
//      mode，文件打开模式
//返回: 成功打开则返回文件指针，否则返回NULL。
//-----------------------------------------------------------------------------
FILE *fopen(const char *fullname, char *mode)
{
    struct tagFileRsc *result;
    struct tagPTTDevice *PTT_device_tag;
    char *synname;
    //fullname不是一个合法的字符串
    if((__Djyfs_CheckFullName(fullname) == false)||(mode ==  NULL))
        return NULL;
    PTT_device_tag = __Djyfs_OpenPTT_Left(fullname);
    if(PTT_device_tag == NULL)
        return NULL;    //分区不存在，或fullname和当前路径均没有指定分区名
    Lock_MutexPend(PTT_device_tag->chip_mutex_tag,CN_TIMEOUT_FOREVER);
    if(PTT_device_tag->formatted)
    {
        synname = __Djyfs_PickSynName(fullname);
        //调用打开文件函数
        result = (struct tagFileRsc *)__Djyfs_PTT_AppCtrl(PTT_device_tag,EN_PTT_OPEN,
                          (ptu32_t)synname,(ptu32_t)mode);
    }else
        result = NULL;
    Lock_MutexPost(PTT_device_tag->chip_mutex_tag);
    return result;
}

//----线程安全读文件-----------------------------------------------------------
//功能: 这是文件系统提供的一个API调用，用于从打来的文件读数据。C标准的
//      djyfs_fread函数默认从当前文件指针处读，如果该指针被别的线程改变，读出的
//      数据将是错误位置的，故不是线程安全函数。
//      实际上，本函数就是一个在信号量保护下的fseek和fread组合操作。
//参数: fp，被读的文件
//      buf，保存读出的数据的缓冲区
//      size，读出的尺寸，nmemb块数
//      offset，文件偏移量(字节数)
//返回: 实际读出的数据量
//注意: 参数顺序跟fread不一样，而是跟win32api中ReadFile相似
//-----------------------------------------------------------------------------
size_t fread_r(FILE *fp,const void *buf,size_t size, size_t offset)
{
    size_t result;
    struct tagPTTDevice *PTT_device_tag;
    struct tagRwPara  param;
    struct tagSeekPara new_pos;
    if( (fp == NULL) || (buf == NULL))
        return false;
    if(size == 0)
        return 0;   //读取的数据量是0
    PTT_device_tag = (struct tagPTTDevice *)fp->home_PTT;
    Lock_MutexPend(PTT_device_tag->chip_mutex_tag,CN_TIMEOUT_FOREVER);
    if(fp->file_attr.bits.folder)
    { //目录不能读
        result = 0;
        goto goto_end;
    }
    if((fp->open_mode == EN_W_WB)||(fp->open_mode == EN_A_AB))
    { //只写模式，不能读
        result = 0;
        goto goto_end;
    }
    new_pos.offset = offset;
    new_pos.whence = SEEK_SET;
    result = __Djyfs_PTT_AppCtrl(PTT_device_tag,EN_PTT_SEEK,
                                (ptu32_t)&new_pos,(ptu32_t)fp);
    param.nmemb = 1;
    param.size = size;
    param.fp = fp;
    result = __Djyfs_PTT_AppRead(PTT_device_tag,(ptu32_t)buf,(ptu32_t)&param,0);

goto_end:
    Lock_MutexPost(PTT_device_tag->chip_mutex_tag);
    return result;
}

//----读文件-----------------------------------------------------------------
//功能: 这是文件系统提供的一个API调用，用于从打来的文件读数据，兼容C标准
//参数: buf，保存读出的数据的缓冲区
//      size，读出的尺寸，nmemb块数
//      nmemb，每块数据长度(字节数)
//      fp，被读的文件
//返回: 实际读出的数据量，nmemb块数
//-----------------------------------------------------------------------------
size_t fread(const void *buf,size_t size, size_t nmemb,FILE *fp)
{
    size_t result;
    struct tagPTTDevice *PTT_device_tag;
    struct tagRwPara  param;
    if( (fp == NULL) || (buf == NULL))
        return false;
    if((size == 0) || (nmemb == 0))
        return 0;   //读取的数据量是0
    PTT_device_tag = (struct tagPTTDevice *)fp->home_PTT;
    Lock_MutexPend(PTT_device_tag->chip_mutex_tag,CN_TIMEOUT_FOREVER);
    if(fp->file_attr.bits.folder)
    { //目录不能读
        result = 0;
        goto goto_end;
    }
    if((fp->open_mode == EN_W_WB)||(fp->open_mode == EN_A_AB))
    { //只写模式，不能读
        result = 0;
        goto goto_end;
    }
    param.nmemb = nmemb;
    param.size = size;
    param.fp = fp;
    result = __Djyfs_PTT_AppRead(PTT_device_tag,(ptu32_t)buf,(ptu32_t)&param,0);

goto_end:
    Lock_MutexPost(PTT_device_tag->chip_mutex_tag);
    return result;
}

//----设置文件指针-------------------------------------------------------------
//功能: 把文件指针设置到指定的位置，如果文件可写方式打开，且新位置大于文件尺寸，
//      将导致文件长度增加。
//参数：fp，被操作的文件指针
//      offset，移动的位置
//      whence，从何处开始计算移动长度，参见SEEK_CUR等的定义
//返回：0=成功，-1=失败
//-----------------------------------------------------------------------------
u32 fseekL(FILE *fp, s64 offset, s32 whence)
{
    struct tagPTTDevice *PTT_device_tag;
    u32 result;
    struct tagSeekPara new_pos;
    if(fp == NULL)
        return CN_LIMIT_UINT32;
    PTT_device_tag = (struct tagPTTDevice *)fp->home_PTT;
    Lock_MutexPend(PTT_device_tag->chip_mutex_tag,CN_TIMEOUT_FOREVER);
    if( (fp->file_attr.bits.folder)
        ||(fp->open_mode == EN_A_AB))
    {//目录没有位置指针,追加只写方式不能改变位置指针
        result = CN_LIMIT_UINT32;
        goto goto_end;
    }
    new_pos.offset = offset;
    new_pos.whence = whence;
    result = __Djyfs_PTT_AppCtrl(PTT_device_tag,EN_PTT_SEEK,
                                (ptu32_t)&new_pos,(ptu32_t)fp);
goto_end:
    Lock_MutexPost(PTT_device_tag->chip_mutex_tag);
    return result;
}

//----设置文件长指针----------------------------------------------------------
//功能: 把文件指针设置到指定的位置，如果文件可写方式打开，且新位置大于文件尺寸，
//      将导致文件长度增加。
//参数：fp，被操作的文件指针
//      offset，移动的位置
//      whence，从何处开始计算移动长度，参见SEEK_CUR等的定义
//返回：0=成功，-1=失败
//-----------------------------------------------------------------------------
u32 fseek(FILE *fp, long offset, s32 whence)
{
    return fseekL(fp,(s64)offset,whence);
}

//----线程安全写文件-----------------------------------------------------------
//功能: 这是文件系统提供的一个API调用，用于把数据写入从打来的文件。C标准的
//      djyfs_fwrite函数默认写入当前文件指针处，如果该指针被别的线程改变,
//      数据将写入错误位置的，故不是线程安全函数。
//      实际上，本函数就是一个在信号量保护下的fseek和fwrite组合操作。
//参数: fp，被读的文件
//      buf，保存读出的数据的缓冲区
//      size，读出的尺寸，nmemb块数
//      offset，文件偏移量(字节数)
//返回: 实际写入的数据量
//注意: 参数顺序跟fwrite不一样，而是跟win32api中WriteFile相似
//-----------------------------------------------------------------------------
size_t fwrite_r(FILE *fp,const void *buf,size_t size, size_t offset)
{
    size_t result;
    struct tagPTTDevice *PTT_device_tag;
    struct tagRwPara  param;
    struct tagSeekPara new_pos;
    if( (fp == NULL) || (buf == NULL))
        return 0;
    if(size == 0)
        return 0;   //写入的数据量是0
    PTT_device_tag = (struct tagPTTDevice *)fp->home_PTT;
    Lock_MutexPend(PTT_device_tag->chip_mutex_tag,CN_TIMEOUT_FOREVER);
    if(fp->file_attr.bits.folder)
    { //目录不能写
        result = 0;
        goto goto_end;
    }
    if(fp->open_mode == EN_R_RB)
    {//只读文件，不能写
        result = 0;
        goto goto_end;
    }
    new_pos.offset = offset;
    new_pos.whence = SEEK_SET;
    result = __Djyfs_PTT_AppCtrl(PTT_device_tag,EN_PTT_SEEK,
                                (ptu32_t)&new_pos,(ptu32_t)fp);
    param.nmemb = 1;
    param.size = size;
    param.fp = fp;
    result = __Djyfs_PTT_AppWrite(PTT_device_tag,(ptu32_t)buf,
                                (ptu32_t)&param,0);
goto_end:
    Lock_MutexPost(PTT_device_tag->chip_mutex_tag);
    return result;
}

//----写文件-------------------------------------------------------------------
//功能: 这是文件系统提供的一个API调用，用于把数据写入打来的文件
//参数: buf，提供写入的数据的缓冲区
//      size，写入的尺寸，nmemb块数
//      nmemb，每块数据长度(字节数)
//      fp，被写的文件
//返回: 实际写入的数据量，nmemb块数
//-----------------------------------------------------------------------------
size_t fwrite(const void *buf,size_t size, size_t nmemb,FILE *fp)
{
    size_t result;
    struct tagPTTDevice *PTT_device_tag;
    struct tagRwPara  param;
    if( (fp == NULL) || (buf == NULL))
        return 0;
    if((size == 0) || (nmemb == 0))
        return 0;   //写入的数据量是0
    PTT_device_tag = (struct tagPTTDevice *)fp->home_PTT;
    Lock_MutexPend(PTT_device_tag->chip_mutex_tag,CN_TIMEOUT_FOREVER);
    if(fp->file_attr.bits.folder)
    { //目录不能写
        result = 0;
        goto goto_end;
    }
    if(fp->open_mode == EN_R_RB)
    {//只读文件，不能写
        result = 0;
        goto goto_end;
    }
    param.nmemb = nmemb;
    param.size = size;
    param.fp = fp;
    result = __Djyfs_PTT_AppWrite(PTT_device_tag,(ptu32_t)buf,
                                (ptu32_t)&param,0);
goto_end:
    Lock_MutexPost(PTT_device_tag->chip_mutex_tag);
    return result;
}

//----删除文件----------------------------------------------------------------
//功能: 这是文件系统提供的一个API调用，用于删除一个文件（目录）
//参数: fullname,文件（目录）名
//返回: 0=成功，非0=删除失败，详情看文件系统出错代码表
//-----------------------------------------------------------------------------
u32 remove(const char *fullname)
{
    u32 result;
    struct tagPTTDevice *PTT_device_tag;
    char *synname;

    if(fullname == NULL)
        return 1;
    if(!__Djyfs_CheckFullName(fullname))
        return 1;
    PTT_device_tag = __Djyfs_OpenPTT_Left(fullname);
    if(PTT_device_tag == NULL)
        return 1;    //分区不存在，或fullname和当前路径均没有指定分区名
    Lock_MutexPend(PTT_device_tag->chip_mutex_tag,CN_TIMEOUT_FOREVER);
    if(PTT_device_tag->formatted)
    {
        synname = __Djyfs_PickSynName(fullname);
        result = __Djyfs_PTT_AppCtrl(PTT_device_tag,EN_PTT_REMOVE,(ptu32_t)synname,0);
        //调用分区的左手控制函数
        if(result == EN_FS_NO_ERROR)
            result = 0;
    }else
        result = EN_FS_PTT_NO_FORMATED;
    Lock_MutexPost(PTT_device_tag->chip_mutex_tag);
    return result;
}

//----修改文件(目录)名字-------------------------------------------------------
//功能: 这是文件系统提供的一个API调用，用于修改一个文件（目录）的名字
//参数: old_fullname,文件（目录）旧名
//      new_filename，新名字
//返回: 0=成功，非0 = 失败
//-----------------------------------------------------------------------------
u32 rename(const char *old_fullname,const char *new_filename)
{
    u32 result;
    struct tagPTTDevice *PTT_device_tag;
    char *synname;

    if(old_fullname == NULL)
        return 1;
    if(__check_word(new_filename) == false)
        return 1;
    if(!__Djyfs_CheckFullName(old_fullname))
        return 1;
    PTT_device_tag = __Djyfs_OpenPTT_Left(old_fullname);
    if(PTT_device_tag == NULL)
        return false;    //分区不存在，或fullname和当前路径均没有指定分区名
    Lock_MutexPend(PTT_device_tag->chip_mutex_tag,CN_TIMEOUT_FOREVER);
    if(PTT_device_tag->formatted)
    {
        synname = __Djyfs_PickSynName(old_fullname);
        //调用分区的左手控制函数
        if(__Djyfs_PTT_AppCtrl(PTT_device_tag,EN_PTT_RENAME,(ptu32_t)synname,
                             (ptu32_t)new_filename) == EN_FS_NO_ERROR)
            result = 0;
        else
            result = 1;
    }else
        result = 1;
    Lock_MutexPost(PTT_device_tag->chip_mutex_tag);
    return result;
}

s32 fputs(const char *__restrict buf, FILE *__restrict fp)
{
    return 0;
    #warning 待完成。
}

//-----------------------------------------------------------------------------
//功能: 从文件中读取一个字符（字节），若文件不可读或者达到文件为，返回EOF
//参数: fp，被读的文件
//输出: 无
//返回: 读取的字符，或者EOF。
//------------
//更新记录:
// 1.日期: 2014/12/09
//   说明: 创建
//   作者: 罗侍田
//-----------------------------------------------------------------------------
s32 fgetc(FILE *fp)
{
    if(fp == NULL)
        return EOF;
    #warning "fgetc还没有实现。"
    return 0;

}

//-----------------------------------------------------------------------------
//功能: 输出宽字符,不是按原始字符输出,而是转换成本地多字节字符输出
//参数: wc,待输出的宽字符
//    fp,接收字符的文件.
//输出: 无
//返回: WEOF或者输出的字符
//------------
//更新记录:
// 1.日期: 2014/12/31
//   说明: 创建
//   作者: 罗侍田
//-----------------------------------------------------------------------------
wint_t putwc(wchar_t wc, FILE *fp)
{
    char buf[8];    //最长的mb字符：utf8，只有7个字节。
    wctomb(buf,wc);
    fputs (buf, fp);
    return wc;
#warning "未处理完成,特别是WEOF"
}





