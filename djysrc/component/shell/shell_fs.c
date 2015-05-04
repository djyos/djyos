//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

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
// 这份授权条款，在使用者符合下列条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------
//所属模块: djysh模块
//作者：wjz
//版本：djysh1.0.0
//文件描述: djysh的文件系统shell命令。
//修订历史:
// 1. 日期:2010-6-15
//   作者:wjz
//   新版本号：djysh1.0.0
//------------------------------------------------------

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stat.h"
#include "char_term.h"
#include "shell.h"
#include "shell_fs.h"
#include "file.h"
#include "flashfile.h"

extern struct tagFileRsc *g_ptWorkPath;
extern uint16_t __Djyfs_IsContainPTT_Name(const char *fullname);
extern bool_t __Djyfs_CheckFullName(const char *fullname);
extern bool_t __Djyfs_PickFileNameWord(const char *synname,char *name);
extern bool_t Sh_PrintWorkPath(void);
//----检查通配符-----------------------------------------------
//功能: 检查字符串的路径部分是否有星号或者问号,查找长度以命令行最大长度为限。
//参数: str,被测字符串
//返回: true=路径部分有'*'或者'?'，false=没'*'或者'?'
//----------------------------------------------------------------------------
bool_t __Shfs_CheckWildCardPath(char *str)
{
    u32 loop,name_len;

    if(str == NULL)
        return false;
    name_len = strlen(str);
    for(loop = 0; loop <= name_len; loop++)
    {
        if((str[loop]=='*')||(str[loop]=='?'))
        {
            break;
        }
    }
    for( ; loop <= name_len; loop++)
    {
        if(str[loop]=='\\')
        {
            return true;
        }
    }
    return false;
}

//----检查通配符-----------------------------------------------
//功能: 检查字符串的文件名部分是否有星号或者问号,查找长度以命令行最大长度为限。
//参数: str,被测字符串
//返回: true=文件名部分有'*'或者'?'，false=没'*'或者'?'
//----------------------------------------------------------------------------
bool_t __Shfs_CheckWildCardName(char *str)
{
    u32 name_len;
    s32 loop;
    if(str == NULL)
        return false;
    name_len = (u32)strlen(str);
    if((str[name_len-1] == '\\') || (str[name_len-1] == ':'))
        return false;
    for(loop = name_len-1 ; loop >= 0; loop--)
    {
        if((str[loop]=='*')||(str[loop]=='?'))
        {
            return true;
        }
        if(str[loop]=='\\')
        {
            return false;
        }
    }
    return false;
}

//----检查通配符-----------------------------------------------
//功能: 检查字符串是否有星号或者问号,查找长度以命令行最大长度为限。
//参数: str,被测字符串
//返回: true=文件名部分有'*'或者'?'，false=没'*'或者'?'
//----------------------------------------------------------------------------
bool_t __Shfs_CheckWildCard(char *str)
{
    u32 loop,name_len;

    if(str == NULL)
        return false;
    name_len = strlen(str);
    for( loop = 0; loop <= name_len; loop++)
    {
        if((str[loop]=='*')||(str[loop]=='?'))
        {
            return true;
        }
    }
    return false;
}

//-------带通配符的字符匹配-----------------------------------------------------
//功能: 带通配符的字符匹配最常见的通配符是'?'和'*'。其中,'?'可以代表一个字符
//        (不能没有),'*'可以代表任意多个字符(可以为空)。首先是'?'， 根据'?'
//        的功能,'?'表示任意字符,也就是说在匹配过程中,'?'永远匹配成功 。
//        本质上,'?'并没有修改算法，而仅仅修改了匹配规则 -- 遇到'?'则一定匹配 。
//        然而'*'与此不同,'*'的作用是匹配任意多个字符，显然我们不能简单的修改匹配
//        过程而满足要求。
//参数: src - 待匹配的字符串,不能含有'?'或'*'
//        wildcard - 通配符字符串
//返回: true=匹配成功,
//      false=匹配失败,
//-----------------------------------------------------------------------------
bool_t __Shfs_WildCardMatch(char *src, char *wildcard)
{
    u32 i = 0;
    u32 j = 0;

    if(wildcard == NULL)
        return true;

    while (wildcard[j] || src[i])
    {
        if (wildcard[j] == '*')
        {
            while (wildcard[j] == '*')    // 找到一连串*号的最后一个*号
            {
                j++;
                if (wildcard[j] == '\0')
                {
                    return true;    // 匹配
                }
            }
            while (src[i] != wildcard[j])    // 匹配src中与*后第一个字符
            {
                if (src[i])
                {
                    i++;
                }
                else
                {
                    return false;    // 不匹配
                }
            }
        }
        else
        {
            if ((src[i] == wildcard[j]) || (wildcard[j] == '?'))
            {
                i++;
                j++;
            }
            else
            {
                return false;
            }
        }
    }

    if ((src[i] == '\0') && (wildcard[j] == '\0'))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//----判断'\'后面是否有数据-----------------------------------------------------
//功能: 从一个可能包含多级路径名和文件名的字符串中判断第一次遇到'\'之后是否还有
//      数据
//参数: path，被测路径，格式如"C:\aaaa\bbbb\cccc"
//返回: true=成功, 说明'\'之后有数据,或者没有'\'
//      false=失败，说明'\'之后没有数据
//备注: path应该是经过合法性检查的字符串，不会出现word超长的问题
//-----------------------------------------------------------------------------
bool_t __Shfs_JudgeBackSlashIsData(char *path)
{
    uint32_t index;
    uint32_t name_len;

    name_len = strlen(path);
    for(index = 0; index< name_len; index++)
    {
        if(path[index] == '\\')    //遇到字符'\'
        {
            if(path[index+1] != '\0')
                return true;
            else
                return false;
        }
    }
    //字符串里没有反斜杠
    if(index == name_len)
        return true;
    else
        return false;
}

//----删除路径后的文件或者目录-------------------------------------------------
//功能: 从一个可能包含多级路径名和文件名的字符串中删除最后一项，如果path是一个根
//      目录名，则不操作
//参数: path，被测路径，格式如"C:\aaaa\bbbb\cccc"
//返回: true=成功删除，或无须删除
//      false=失败
//-----------------------------------------------------------------------------
bool_t __Shfs_CutPathEndItem(char *path)
{
    u32 name_len,loop;

    name_len = (u32)strlen(path);
    loop=name_len;
    if(path[name_len-1]=='\\')//有反斜杠说明是去除文件夹
    {
        if(path[name_len-2]==':')
            return true;//说明是根目录，不用操作
        else
        {
            for(loop=name_len-2;loop>0;loop--)
            {
                if(path[loop] == '\\')
                {
                    path[loop+1] = '\0';
                    break;
                }
            }
        }
        return true;
    }
    else//没有反斜杠，说明去除的是文件
    {
        for(loop=name_len-1;loop>0;loop--)
        {
            if(path[loop] == '\\')
            {
                path[loop+1] = '\0';
                break;
            }
        }
    }
    return true;
}

//----提取路径后的文件或者目录--------------------------------------------------
//功能: 从一个可能包含多级路径名和文件名的字符串中提取最后一项。如果path是根目录
//      则不操作。path串保持不变。
//参数: path，被测路径，格式如"C:\aaaa\bbbb\cccc"
//        name,保存提取的文件或者目录
//返回: true=成功, 提取成功
//      false=失败，提取失败
//-----------------------------------------------------------------------------
bool_t __Shfs_PickPathEndItem(char *path,char *name)
{
    u32 name_len,loop;
    char *str;

    name_len = strlen(path);
    loop=name_len;
    if(path[name_len-1]=='\\')//有反斜杠，说明提取的是文件夹名
    {
        if(path[name_len-2]==':')
            return false;   //说明是根目录，不用操作
        else
        {
            for(loop=name_len-2;loop>0;loop--)
            {
                if(path[loop] == '\\')
                {
                    str=&path[loop+1];
                    strcpy(name,str);
                    return true;
                }
            }
        }
    }
    else//没有反斜杠，说明提取的是文件名
    {
        for(loop=name_len-1;loop>0;loop--)
        {
            if(path[loop] == '\\')
            {
                str=&path[loop+1];
                strcpy(name,str);
                return true;
            }
        }

    }
    //执行到这步，说明只是一个文件或者目录，直接拷贝就ok
    strcpy(name,path);
    return true;
}

//----分离文件名---------------------------------------------------------------
//功能: 如果字符串包含文件名，则把最后一个'\\'后的第一个字符改成'\0'，并返回文件
//      名指针
//参数: path，被测试的串
//      filename，返回文件名地址,但首字符已经被作为返回值返回，串中被替换为'\0'
//      pathname，若path串中存在目录名，则返回路径名地址，否则返回NULL;
//返回: 被'\0'替换的字符，'\0'=不含文件名
//-----------------------------------------------------------------------------
char __Shfs_SeparateFileName(char *path,char **filename,char **pathname)
{
    u32 name_len,loop;
    char ch;
    name_len = strlen(path);
    if(name_len == 0)
        return 0;
    for(loop = 0; loop < name_len; loop++)
    {
        if((path[loop-1] == ':') || (path[loop-1] == '\\'))
            break;
    }
    if(loop == name_len)        //整个串是一个文件名
    {
        *pathname = NULL;
        *filename = path;
        return path[0];
    }
    else if(loop == name_len -1)    //不含文件名
    {
        *filename = NULL;
        *pathname = path;
        return 0;
    }
    else
    {
        *filename = path + loop;
        *pathname = path;
        ch = path[loop];
        path[loop] = '\0';
        return ch;
    }
}

//----判断字符串是绝对路径还是相对路径-----------------------------------------
//功能: 检查一个可能包含多级路径名和文件名的字符串是否包含分区名的绝对路径
//参数: path，被测路径，格式如"C:\aaaa\bbbb\cccc"
//返回: true=是绝对路径
//      false=不是绝对路径
//备注: path应该是经过合法性检查的字符串，不会出现word超长的问题
//-----------------------------------------------------------------------------
bool_t __Shfs_PathIsAbsolutePath(char *path)
{
    uint32_t index;
    //synname是已经经过字符串长度合法性检查的指针
    uint32_t name_len;

    name_len = strlen(path);
    if(path[0] == '\\')
        return true;
    for(index = 0; index< name_len; index++)
    {
        if(path[index] == ':')
        {
            return true;
        }
    }
        return false;
}

//----判断路径是否包含分区---------------------------------------------------
//功能: 判断一个可能包含多级路径名和文件名的字符串是否包含分区名
//参数: path，被测路径，格式如"C:\aaaa\bbbb\cccc"
//返回: true=含分区名
//      false=不含分区名
//-----------------------------------------------------------------------------
bool_t __Shfs_PathIncludeDbxName(char *path)
{
    uint32_t index;
    //synname是已经经过字符串长度合法性检查的指针
    uint32_t name_len;

    name_len = strlen(path);
    for(index = 0; index< name_len; index++)
    {
        if(path[index] == ':')
        {
            return true;
        }
    }
        return false;
}

//----目录下匹配的文件提取到一个字符串--------------------------------------
//功能: 在一个目录下扫描存在的文件，然后每个文件通过匹配函数和wildcard_param进行
//        一一匹配，把匹配的文件名提到到一个字符串里。
//参数: wildcard_param，含有通配符的参数
//        get_merge_str ，最后符合要求的文件名合并而成的字符串
//返回: true=成功, 提取成功
//      false=失败，提取失败
//-----------------------------------------------------------------------------
char *ShFs_GetFolderWildcardStr(char *wildcard_param,char *get_merge_str)
{
    struct tagFileRsc *fp = NULL,*current_fp = NULL;
    char work_path[256];

    if(__Djyfs_IsContainPTT_Name(wildcard_param) ==0)
        fp = g_ptWorkPath;//相对路径，直接使用工作路径
    else
    {
        strcpy(work_path,wildcard_param);
        __Djyfs_PickFileNameWord(work_path,wildcard_param);
        __Shfs_CutPathEndItem(work_path);
        fp = fopen(work_path, "r");
    }
    //是文件，没意义（同时fp2是非根目录,根目录下，文件属性不确定）
    if(fp->file_attr.bits.archive&&(fp->file_medium_tag!=0))
        return NULL;
    current_fp =(struct tagFileRsc *)Djyfs_FolderTraversalSon(fp,current_fp);
    if(current_fp == NULL)
    {
        fclose(fp);
        printf("该目录为空!\r\n");
        return NULL;
    }

        //全部置0，这样就可以准确知道本来merge_str已经没有数据了
    memset(get_merge_str,0,sizeof(get_merge_str));
    do//循环读目录表，直到读完
    {
        if(__Shfs_WildCardMatch(current_fp->name,wildcard_param))
        {//把匹配成功的目录串成一个新字符串
            strcat(get_merge_str,current_fp->name);
            strcat(get_merge_str," ");
        }
        //当fp下所有的文件都被删除了，就没文件，返回false
        current_fp = (struct tagFileRsc *)Djyfs_FolderTraversalSon(fp,current_fp);
        if(current_fp == NULL)
            break;
    }while(current_fp != NULL);
    fclose(fp);

    return get_merge_str;
}

//----带通配符删除文件---------------------------------------------------------
//功能: 带通配符删除文件，路径部分不能带通配符。空目录也一并删除。
//参数:
//返回: true=成功, 提取成功
//      false=失败，提取失败
//-----------------------------------------------------------------------------
bool_t ShFs_DelWildcardFiles(char *wildcard_param)
{
    char *filename,*pathname;
    char ch;
    struct tagFileRsc *current=NULL,*parent;

    if(wildcard_param == NULL)
    {
        printf("文件名不正确!\r\n");
        return false;
    }
    ch = __Shfs_SeparateFileName(wildcard_param,&filename,&pathname);
    if(filename == NULL)     //不含文件名
    {
        printf("文件名不正确!\r\n");
        return false;
    }

    if(pathname != NULL)
    {
        if(stat(pathname,NULL) != -1)
        {
            parent = fopen(pathname,"r");
        }
        else
        {
            printf("路径不存在\n\r");
            return false;
        }
    }
    else
    {
        parent = g_ptWorkPath;
    }
    if(filename != NULL)
        filename[0] = ch;       //恢复文件名的第一个字符
    while(1)                //循环读目录表，直到读完
    {
        current =(struct tagFileRsc *)Djyfs_FolderTraversalSon(parent,current);
        if(current == NULL)
            break;
        if(__Shfs_WildCardMatch(current->name,filename))
        {
            remove(current->name);
        }
    }

    fclose(parent);
    return true;
}

//----在字符串末尾无反斜杠加上反斜杠--------------------------------------------------------------
//功能：在字符串末尾无反斜杠加上反斜杠,有反斜杠就不要加了
//参数：str 字符串，任何形式
//返回：true 成功
//        false 遇到".."了
//------------------------------------------------------------------------------
bool_t ShFs_StrEndIncreaseBackSlash(char *str)
{
    s32 name_len;

    if(strcmp(str,"..") ==0)//如果是"..",那么不要操作
        return false;
    name_len = strnlen(str,CN_SHELL_CMD_LIMIT +1);
    if(name_len >CN_SHELL_CMD_LIMIT)
        return false;
    //如果字符串最后一位不是'\\'，那么加反斜杠
    if(str[name_len-1] != '\\')
    {
        str[name_len] = '\\';
        //如果str不是规定好的空间，很容易泄露，必须防止
           str[name_len+1] = '\0';
    }
    return true;
}

//----在字符串末尾有反斜杠去掉反斜杠--------------------------------------------
//功能：在字符串末尾无反斜杠加上反斜杠，没有反斜杠就不处理
//参数：str 字符串，任何形式
//返回：true 成功
//        false 字符串没有反斜杠
//------------------------------------------------------------------------------
bool_t ShFs_StrEndWipeBackSlash(char *str)
{
    s32 name_len;

    name_len = strnlen(str,CN_SHELL_CMD_LIMIT +1);
    if(name_len >CN_SHELL_CMD_LIMIT)
        return false;
    if(str[name_len-1] == '\\')
    {
        str[name_len-1] = '\0';
        return true;
    }
    else
    return false;

}

//----显示目录下的文件和文件夹------------------------------------------------
//功能: 扫描根目录下的文件和文件夹，然后按照一定个格式，全部打印出来
//参数: ancestor_fp，被显示的文件夹
//返回: true=成功, 列出成功
//      false=失败，列出失败
//------------
//更新记录:
// 1.日期: 2015/3/19
//   说明: 未完成
//   作者: 季兆林
//-----------------------------------------------------------------------------
bool_t ShFs_DirItemSon(struct tagFileRsc *parent_fp)
{
    struct tagFileRsc *current_fp;
    s64 files_size=0;
    u32 index=0;
    uint32_t files_items=0,folder_items=0;
    s64 available_PTT_size=0;
    char *buf;

    if(parent_fp == NULL)
        return false;//可能是分区未建立

    index = Djyfs_GetPathLen(parent_fp);
    buf = M_MallocLc(index,0);
    if(buf == NULL)
    {
        printf("内存不足，不能执行dir");
        return false;
    }
    buf[0] = '\0';
    Djyfs_GetPath(parent_fp,buf,index+1);
    printf("%s 的目录\r\n\r\n",buf);
    free(buf);
    Djyfs_PTT_StatFp(parent_fp,NULL,NULL,&available_PTT_size);

    current_fp = NULL;
    current_fp = (struct tagFileRsc*)Djyfs_FolderTraversalSon(parent_fp,current_fp);
    if(current_fp == NULL)
    {
        printf("该目录为空!\r\n");
        return false;
    }

    do //循环读目录表，直到读完
    {
#if 0
        printf("%d/%02d/%02d %02d:%02d",
                (current_fp->year_high_modify<<8) + current_fp->year_low_modify,
                current_fp->month_modify,
                current_fp->date_modify,
                current_fp->hour_modify,
                current_fp->minute_modify);
#endif
        if(current_fp->file_attr.bits.folder)
        {
            printf(" <DIR> ");//代表是目录，可以继续dir
            folder_items++;
        }
        else
        {
            printf("       ");
            files_items++;
        }
        printf("%9d %s\r\n",
                (s32)current_fp->file_size, current_fp->name);
        files_size+=current_fp->file_size;
        current_fp = (struct tagFileRsc*)Djyfs_FolderTraversalSon(parent_fp,current_fp);
    }while(current_fp != NULL);

    printf("%d个文件 %d字节\r\n", files_items, (s32)files_size);
    printf("%d个目录 %d可用字节\r\n", folder_items, (s32)available_PTT_size);
    return true;
}

//----显示含有通配符的文件或者文件夹------------------------------------------------
//功能: dir命令下，显示含有通配符的文件或者文件夹。扫描当前工作路径或者指定路径下
//        的文件或者目录。然后用匹配函数一一匹配。符合要求的，显示出来。
//参数: wildcard_param，含有通配符的字符串
//返回: true=成功, 列出成功
//      false=失败，列出失败
//------------
//更新记录:
// 1.日期: 2015/3/19
//   说明: 未完成
//   作者: 季兆林
//-----------------------------------------------------------------------------
bool_t ShFs_DirWildcardFiles(char *wildcard_param)
{
    char *filename,*pathname;
    uint32_t files_items=0,folder_items=0;
    s64 files_size=0,available_PTT_size=0;
    char ch;
    struct tagFileRsc *current=NULL,*parent;
//    struct tagFileRsc *bak_work_path = g_ptWorkPath;

    if(wildcard_param == NULL)
    {
        printf("文件名不正确!\r\n");
        return false;
    }
    ch = __Shfs_SeparateFileName(wildcard_param,&filename,&pathname);

    if(pathname != NULL)
    {
        if(stat(pathname,NULL) != -1)
        {
            parent = fopen(pathname,"r");
        }
        else
        {
            printf("路径不存在\n\r");
            return false;
        }
    }
    else
    {
        parent = g_ptWorkPath;
    }
    if(filename != NULL)
        filename[0] = ch;       //恢复文件名的第一个字符

    while(1)                //循环读目录表，直到读完
    {
        current =(struct tagFileRsc *)Djyfs_FolderTraversalSon(parent,current);
        if(current == NULL)
            break;
        if(__Shfs_WildCardMatch(current->name,filename))
        {
#if 0  //文件时间格式改变
            printf("%d/%02d/%02d %02d:%02d",
                    (current->year_high_modify<<8) + current->year_low_modify,
                    current->month_modify,
                    current->date_modify,
                    current->hour_modify,
                    current->minute_modify);
#endif
            if(current->file_attr.bits.folder)
            {
                printf(" <DIR> ");
                folder_items++;
            }
            else
            {
                printf("       ");
                files_items++;
            }
            printf("%9dB %s\r\n", (s32)current->file_size, current->name);
            files_size += current->file_size;
        }
    }

    Djyfs_PTT_StatFp(parent,NULL,NULL,&available_PTT_size);
    fclose(parent);
    printf("%d个文件 %d字节\r\n", files_items, (s32)files_size);
    printf("%d个目录 %d可用字节\r\n", folder_items, (s32)available_PTT_size);
    return true;
}



//----判断一个目录是否是文件夹------------------------------------------------
//功能: 通过访问fp的属性，判断是否是文件夹
//参数: fullname,路径 格式:"C:\aa\bb\cc"
//返回: CN_ITEM_IS_FOLDER=是文件夹,
//      CN_ITEM_IS_FILE=是文件，
//      CN_ITEM_NO_EXIST=不存在这样的文件或文件夹
//注:   调用时须确保fullname是合法的字符串
//-----------------------------------------------------------------------------
u32 __Shfs_JudgeItemAttr(char *fullname)
{
    struct stat FileStat;
    bool_t result;
    result = stat(fullname,&FileStat);
    if(result != -1)
    {
           if(FileStat.st_mode.bits.folder)
        {
            return CN_ITEM_IS_FOLDER;
        }
        else
        {
            return CN_ITEM_IS_FILE;
        }
    }
    else
        return CN_ITEM_NO_EXIST;
}

//----建立一个文件夹------------------------------------------------
//功能: 指定建立一个文件夹
//参数: md_param，输入字符
//返回: true=成功,
//      false=失败，
//-----------------------------------------------------------------------------
bool_t mkdir(char *md_param)
{
    struct tagFileRsc *fp;

    if(md_param == NULL)
    {
        printf("命令语法不正确!\r\n");
        return false;
    }
    //在创建的末尾加了'\'，无论如何，都是目录
    //这里不怕内存泄漏，因为md函数里，规定了长度
    ShFs_StrEndIncreaseBackSlash(md_param);
    if(!__Djyfs_CheckFullName(md_param))
         return false;
    //如果有存在文件或者目录，那么就不需要创建了
    if(stat(md_param,NULL) == -1)
    {
        fp = fopen(md_param,"w+");
        if(fp == NULL)
            return false;
        fclose(fp);
    }
    else
    {
        printf("\r\n子目录或者文件%d已经存在\r\n", md_param);

        return false;
    }
    return true;
}
//----删除文件夹--------------------------------------------------------------
//功能: 把指定的文件夹删除掉
//参数: rd_param，输入字符
//返回: true=成功,
//      false=失败，
//-----------------------------------------------------------------------------
bool_t ShFs_RemoveFolder(char *rd_param)
{
    struct tagFileRsc *fp;

    if(rd_param == NULL)
    {
        printf("命令语法不正确!\r\n");
        return false;
    }
    if(!__Djyfs_CheckFullName(rd_param))
         return false;
    fp = fopen(rd_param,"r");
    if(fp == NULL)
    {
        printf("系统找不到指定的文件\r\n");
        return false;
    }
    if(fp->file_attr.bits.folder)
    {
        fclose(fp);
        if(remove(rd_param) == EN_FS_FOLDER_UNBLANK)
        {
            printf("目录%s不是空！\r\n", rd_param);
        }
        return true;
    }
    else
    {
        fclose(fp);
        printf("目录名称无效!\r\n");
        return false;
    }

}

//----显示指定目录下的文件和子目录列表-------------------------------------------------------
//功能：显示指定目录下的文件和子目录列表
//参数：dir_param，dir输入的参数，可以是文件，路径或者不输入
//返回：true，显示成功
//        false,显示失败
//-----------------------------------------------------------------------------
bool_t Shfs_DirParent(char *next_param)
{
    char *foldername=NULL;
    struct tagFileRsc *fp;
    uint32_t lenth=0;
    char work_path[CN_SHELL_CMD_LIMIT+1];

    //这里，必须使用get_work_path了。下面，dir ..\folder，需要                                                        ->private_tag);
    Djyfs_GetCwd(work_path,CN_SHELL_CMD_LIMIT);
    do
    {
        foldername = (char *)Sh_GetItem(next_param,&next_param);
        if(foldername == NULL&&lenth==0)
            return false;
        //执行 如: cd ..\foder
        if(strcmp(foldername,"..") != 0)
        {
            ShFs_StrEndIncreaseBackSlash(work_path);
            strcat(work_path,foldername);
            fp = fopen(work_path,"r");
            if(fp == NULL)
            {
                printf("系统找不到指定的路径!\r\n");
                return false;
            }
            ShFs_DirItemSon(fp);
            fclose(fp);
            break;
        }
        __Shfs_CutPathEndItem(work_path);
        if(next_param == NULL)
        {
            fp = fopen(work_path,"r");
            ShFs_DirItemSon(fp);
            fclose(fp);
            break;
        }
        //路径深度大于cn_path_depth_limit，会跳出循环。在这里，
        //就为了防止死循环，多一个保护吧
        lenth++;
     }while((lenth!=CN_PATH_DEPTH_LIMIT)&&(foldername != NULL));
    return true;
}

//----更改当前工作路径到父目录-------------------------------------------------------
//功能：更改当前工作路径到原来当前工作路径的父目录
//参数：next_param，输入的参数
//返回：true，显示成功
//        false,显示失败
//-----------------------------------------------------------------------------

bool_t ShFs_CD_Parent(char *next_param)
{
    char *foldername=NULL;
    struct tagFileRsc *fp;
    struct tagFileRsc *root,*temp;
    uint32_t lenth=0;

    root = (struct tagFileRsc*)Rsc_GetTree(&g_ptWorkPath->file_node);
    do
    {
        foldername = Sh_GetItem(next_param,&next_param);
        //执行 如: cd ..\foder
        if(strcmp(foldername,"..") != 0)
        {
            //是目录才能打开，不用考虑根目录的情况。
            //假如"cd ..\foder",在根目录下执行，就没意义。
            //以只读方式打开，如果不存在，就返回NULL，
            //就不用去专门搜索该目录下这个目录是否存在。
            if(__Shfs_JudgeItemAttr(foldername) == CN_ITEM_IS_FOLDER)
            {
                fp = g_ptWorkPath;
                g_ptWorkPath = fopen(foldername,"r");
                fclose(fp);
                return true;
            }
            else
            {
                printf("系统找不到指定路径\r\n");
                return false;
            }
            lenth++;
        }
        temp = (struct tagFileRsc*)Rsc_GetParent(&g_ptWorkPath->file_node);
        if(temp != root)
        {
            fp=g_ptWorkPath;        //当前工作路径不能关闭,故转移
            g_ptWorkPath = temp;
            fclose(fp);
        }
        else
            break;
        //路径深度大于cn_path_depth_limit，会跳出循环。在这里，
        //就为了防止死循环，多一个保护吧
     }while((lenth <= CN_PATH_DEPTH_LIMIT)&&(next_param != NULL));
    return true;
}


//----显示指定目录下的文件和子目录列表-------------------------------------------------------
//功能：显示指定目录下的文件和子目录列表
//参数：dir_param，dir输入的参数，可以是文件，路径或者不输入
//返回：true，显示成功
//        false,显示失败
//-----------------------------------------------------------------------------

bool_t ShFs_CD_PTT(char *PTT_Name)
{
    char work_path[CN_SHELL_CMD_LIMIT+1];
    struct tagFileRsc *fp;

    strcpy(work_path,PTT_Name);
    strcat(work_path,"\\");
    if(stat(work_path,NULL) != -1)
    {
        fp = fopen(work_path,"r");
        if(fp != NULL)
        {
            g_ptWorkPath = fp;
            return true;
        }
        else
            return false;
    }
    else
    {
        printf("该分区不存在\r\n");
        return false;
    }
}

//复制次数太多，出的问题吧!可能是磨损平衡吧!总之，这算是一个大bug。

//----复制一个文件到目标------------------------------------------------
//功能: 把一个文件复制到需要被复制的地方
//参数:  src，当前的工作路径，不过区别work_path，因为work_patch是全局变量，
//        尽量少修改。
//        dest，这个工作路径是复制到目标的路径
//返回: true=成功,
//      false=失败，
//-----------------------------------------------------------------------------
bool_t Shfs_CopyFileToDest(char *src,char *dest)
{
    uint32_t file_size,loop,loop_times,buf_size;
    uint32_t buf_size_offset,size;
    struct tagFileRsc *src_fp;
    struct tagFileRsc *dest_fp;
    char* buf;

    src_fp = fopen(src,"r");
    if(src_fp == NULL)
        return false;
    file_size = src_fp->file_size;
    fclose(src_fp);
    ////申请16k内存,防止内存不足
    //申请的内存不能太小，否则文件大，操作次数多，影响copy的速度
    //太大，申请不了就完蛋了，暂时定为16k，大概一个flash块的大小
    //待频繁测试之后，再确定
    buf_size = 16384;
    loop_times =(file_size+buf_size-1)/buf_size;
    if(loop_times==0)
    {//loop_times==0时候，说明文件大小为0.只需要建立，就ok
        dest_fp = fopen(dest,"a+");
        fclose(dest_fp);
        return true;
    }
    buf = (char *)M_MallocLc(file_size,0);
    if(buf == NULL)
        return false;
    buf_size_offset = file_size%buf_size;
    if(buf_size_offset == 0)
        buf_size_offset = buf_size;
    src_fp = fopen(src,"r");
    dest_fp = fopen(dest,"a+");
    for(loop=0;loop<loop_times;loop++)
    {
        if(loop!=loop_times-1)
            size = buf_size;
        else//写入的数据小于申请的内存，不应该写入buf_size的大小
            size = buf_size_offset;
        fseek(src_fp, (sint64_t)(size*loop), SEEK_SET);
           fread(buf, (size_t)size, 1,src_fp);
        fwrite((void *)buf,(size_t)size,1,dest_fp);
    }
    fclose(src_fp);
    fclose(dest_fp);
    free(buf);
    return true;
}
//----移动一个目录下的文件到目标------------------------------------------------
//功能: 把一个目录的文件移动到需要被移动的地方
//参数: src，当前的工作路径，不过区别work_path，因为work_patch是全局变量，
//        尽量少修改。
//        dest，这个工作路径是复制到目标的路径
//返回: true=成功,
//      false=失败，
//-----------------------------------------------------------------------------
bool_t Shfs_CopyFileOfFolderToDest(char *src,char *dest)
{

    uint32_t file_nunmber=0;
    struct tagFileRsc *dest_fp,*src_fp,*current_fp=NULL;
    if(stat(dest,NULL) == -1)
    {
        printf("djyos不支持此操作,详情请看help!\r\n");
        return false;
    }
    dest_fp = fopen(dest,"r");
    if(dest_fp == NULL)
        return false;
    //djyfs不支持多个文件合并写入到一个文件里
    if(dest_fp->file_attr.bits.archive)
    {
        printf("djyos不支持此操作,详情请看help!\r\n");
        fclose(dest_fp);
        return false;
    }
    fclose(dest_fp);

    src_fp = fopen(src,"r");
    if(src_fp == NULL)
    {
        fclose(dest_fp);
        return false;
    }
    current_fp = (struct tagFileRsc *)Djyfs_FolderTraversalSon(src_fp,current_fp);
    //必须循环一次就关闭，否则数据不准确，有些文件可能被删除
       fclose(src_fp);
    if(current_fp == NULL)
    {
        printf("%s*\r\n", src);
        printf("系统找不到指定的文件\r\n");
        printf("已经复制了            0个文件\r\n");//暂时这样
        return false;  //parent是一个空目录
    }

    do
    {

        if(current_fp->file_attr.bits.archive)
        {
            strcat(src,current_fp->name);
            strcat(dest,current_fp->name);
            Shfs_CopyFileToDest(src,dest);
            printf("%s\r\n", src);
            __Shfs_CutPathEndItem(src);
            __Shfs_CutPathEndItem(dest);
            file_nunmber++;
        }
        src_fp = fopen(src,"r");
        if(src_fp == NULL)
        {
            fclose(dest_fp);
            return false;
        }
        //必须循环一次就关闭一次，否则数据不准确，有些文件可能被删除
           fclose(src_fp);
        current_fp = (struct tagFileRsc *)Djyfs_FolderTraversalSon(src_fp,current_fp);
    } while(current_fp != NULL);
        printf("已经复制了            %d个文件\r\n", file_nunmber);
        return true;
}

//复制次数太多，出的问题吧!可能是磨损平衡吧!总之，这算是一个大bug。

//----移动一个文件到目标------------------------------------------------
//功能: 把一个文件移动到需要被移动的地方
//参数:  src，当前的工作路径，不过区别work_path，因为work_patch是全局变量，
//        尽量少修改。
//        dest，这个工作路径是复制到目标的路径
//返回: true=成功,
//      false=失败，
//-----------------------------------------------------------------------------
bool_t Shfs_MoveFileToDest(char *src,char *dest)
{
    uint32_t file_size,loop,loop_times,buf_size;
    uint32_t buf_size_offset,size;
    struct tagFileRsc *src_fp;
    struct tagFileRsc *dest_fp;
    char* buf;

    src_fp = fopen(src,"r");
    if(src_fp == NULL)
        return false;
    file_size = src_fp->file_size;
    fclose(src_fp);
    ////申请16k内存,防止内存不足
    //申请的内存不能太小，否则文件大，操作次数多，影响copy的速度
    //太大，申请不了就完蛋了，暂时定为16k，大概一个flash块的大小
    //待频繁测试之后，再确定
    buf_size = 16384;
    loop_times =(file_size+buf_size-1)/buf_size;
    if(loop_times == 0)
    {
        dest_fp = fopen(dest,"a+");
        fclose(dest_fp);
        remove(src);
    }
    buf = (char *)M_MallocLc(file_size,0);
    if(buf == NULL)
        return false;
    buf_size_offset = file_size%buf_size;
    if(buf_size_offset == 0)
        buf_size_offset = buf_size;
    src_fp = fopen(src,"r");
    dest_fp = fopen(dest,"a+");
    for(loop=0;loop<loop_times;loop++)
    {
        if(loop!=loop_times-1)
            size = buf_size;
        else//写入的数据小于申请的内存，不应该写入buf_size的大小
            size = buf_size_offset;
        fseek(src_fp, (sint64_t)(size*loop), SEEK_SET);
           fread(buf, (size_t)size, 1,src_fp);
        fwrite((void *)buf,(size_t)size,1,dest_fp);
    }
    fclose(src_fp);
    fclose(dest_fp);
    remove(src);
    free(buf);

    return true;
}

//----移动一个目录下的文件到目标------------------------------------------------
//功能: 把一个目录的文件移动到需要被移动的地方
//参数: src，当前的工作路径，不过区别work_path，因为work_patch是全局变量，
//        尽量少修改。
//        dest，这个工作路径是复制到目标的路径
//返回: true=成功,
//      false=失败，
//-----------------------------------------------------------------------------
bool_t Shfs_MoveFileOfFolderToDest(char *src,char *dest)
{

    uint32_t file_nunmber=0,fat_block_no=0xffffffff;
    struct tagFileRsc *dest_fp,*src_fp,*current_fp=NULL;
    if(stat(dest,NULL) == -1)
    {
        printf("djyos不支持此操作,详情请看help!\r\n");
        return false;
    }
    dest_fp = fopen(dest,"r");
    if(dest_fp == NULL)
        return false;
    //djyfs不支持多个文件合并写入到一个文件里
    if(dest_fp->file_attr.bits.archive)
    {
        printf("djyos不支持此操作,详情请看help!\r\n");
        fclose(dest_fp);
        return false;
    }
    fclose(dest_fp);

    src_fp = fopen(src,"r");
    if(src_fp == NULL)
    {
        fclose(dest_fp);
        return false;
    }
    current_fp =(struct tagFileRsc *) Djyfs_FolderTraversalSon(src_fp,current_fp);
    //必须循环一次就关闭，否则数据不准确，有些文件可能被删除
       fclose(src_fp);
    if(current_fp == NULL)
    {
        printf("%s*\r\n", src);
        printf("系统找不到指定的文件\r\n");
        printf("已经移动了            0个文件\r\n");//暂时这样
        return false;  //parent是一个空目录
    }
    //strcpy(name,current_fp->name);
    do
    {

        if(current_fp->file_attr.bits.archive)
        {
            strcat(src,current_fp->name);
            strcat(dest,current_fp->name);
            Shfs_MoveFileToDest(src,dest);
            printf("%s\r\n", src);
            __Shfs_CutPathEndItem(src);
            __Shfs_CutPathEndItem(dest);
            file_nunmber++;
        }
        else if(!current_fp->file_attr.bits.archive)
        {
            if(fat_block_no==0xffffffff)//第一个的话，变成了child
                fat_block_no = current_fp->file_medium_tag;
        }
        src_fp = fopen(src,"r");
        if(src_fp == NULL)
        {
            fclose(dest_fp);
            return false;
        }
        //必须循环一次就关闭一次，否则数据不准确，有些文件可能被删除
           fclose(src_fp);
        current_fp = (struct tagFileRsc *)Djyfs_FolderTraversalSon(src_fp,current_fp);
        if(current_fp ==NULL)
            break;
        if(current_fp->file_attr.bits.archive)
        {
           continue;
        }
        if(fat_block_no==current_fp->file_medium_tag)
            break;
    } while(current_fp !=NULL);
        printf("已经移动了            %d个文件\r\n", file_nunmber);
        return true;
}

//----显示指定目录下的文件和子目录列表-----------------------------------------
//功能：显示指定目录下的文件和子目录列表
//参数：dir_param，dir输入的参数，可以是文件
//返回：true，显示成功
//        false,显示失败
//------------
//更新记录:
// 1.日期: 2015/3/19
//   说明: 未完成
//   作者: 季兆林
//-----------------------------------------------------------------------------

bool_t Shfs_Dir(char *dir_param)
{
    struct tagFileRsc *fp;
    s64 size;
    if(dir_param == NULL)
        return false;
    if(__Shfs_CheckWildCardPath(dir_param))     //路径部分不允许带通配符
    {
        printf("%s 文件名不正确!\r\n",dir_param);
        return true;
    }
    else if(__Shfs_CheckWildCardName(dir_param))   //文件名带通配符
    {
        ShFs_DirWildcardFiles(dir_param);
        return true;
    }

    if(strcmp(dir_param,"\\")==0)//执行 dir [\]
    {
        fp = Djyfs_GetRootFolder(g_ptWorkPath);
        ShFs_DirItemSon(fp);
        return true;
    }
    if(dir_param[0]=='.' && dir_param[1]=='.')//执行dir    [..]
    {
        if(Shfs_DirParent(dir_param))
            return true;
        else
            return false;
    }
    if((dir_param[0] == '.') && (dir_param[1] == '\0'))
    {
        ShFs_DirItemSon(g_ptWorkPath);
        return true;
    }

    if(!__Djyfs_CheckFullName(dir_param))
    {
        printf("路径格式不正确!\r\n");
            return false;
    }
    if(__Shfs_JudgeItemAttr(dir_param) != CN_ITEM_IS_FOLDER)
    {
        printf("系统找不到指定的路径\r\n");
        return false;
    }
    if(stat(dir_param,NULL) != -1)
    {
        fp = fopen(dir_param,"r");
        if(fp->file_attr.bits.folder)
            ShFs_DirItemSon(fp);
        else
        {
            Djyfs_PTT_StatFp(fp,NULL,NULL,&size);
            printf("%s 的目录\r\n\r\n",dir_param);
#if 0  //文件时间格式改变
            printf("%d/%02d/%02d %02d:%02d",
                    (fp->year_high_modify<<8) + fp->year_low_modify,
                    fp->month_modify,
                    fp->date_modify,
                    fp->hour_modify,
                    fp->minute_modify);
#endif
            printf("       ");
            printf("%9dB %s\r\n", (s32)fp->file_size, fp->name);
            printf("1个文件 %d字节\r\n", (s32)fp->file_size);
            printf("0个目录 %d可用字节\r\n", (s32)size);
        }
        fclose(fp);
        return true;
    }
    else
    {
        printf("找不到文件\r\n");
        return false;
    }
}
//----列出分区内的空间参数--------------------------------------------------
//功能：把分区的大小，有效空间，空闲大小都列出来
//参数：PTT_name 分区名
//返回：true 成功
//------------
//更新记录:
// 1.日期: 2015/3/25
//   说明: 更改函数实现
//   作者: 季兆林
//-----------------------------------------------------------------------------
bool_t ShFs_Chkptt(char *PTT_name)
{
    s64 sum_size;
    s64 valid_size;
    s64 free_size;

    if(PTT_name == NULL)
    {
        printf("错误: 参数不能为空!\r\n");
        return false;
    }
    if(Djyfs_PTT_Stat(PTT_name,&sum_size,&valid_size,&free_size))
    {
        printf("分区%s占用总空间：%d 字节\r\n", PTT_name, (s32)sum_size);
        printf("分区%s有效空间：%d 字节\r\n", PTT_name, (s32)valid_size);
        printf("分区%s空闲空间：%d 字节\r\n", PTT_name, (s32)free_size);
    }
    else
    {
        printf("分区名不正确或未格式化!\r\n");
    }
     return true;
}


//----将至少一个文件复制到另外一个位置--------------------------------------------------
//功能：将至少一个文件复制到另外一个位置
//参数：PTT_name 分区名
//返回：true 成功
//-----------------------------------------------------------------------------
bool_t ShFs_Copy(char *param)
{
    char *next_param,source[255],destination[255];
    struct tagFileRsc *fp,*second_fp;
    char s[255];
    if(param == NULL)
    {
        printf("命令语法不正确!\r\n");
        return false;
    }
    strcpy(source,Sh_GetWord(param,&next_param));

    if(NULL == next_param)
    {
        printf("djyfs不支持此操作，详情请看help\r\n");
        return false;
    }
    strcpy(destination,Sh_GetWord(next_param,&next_param));
    if((!__Djyfs_CheckFullName(source))||(!__Djyfs_CheckFullName(destination)))
        return false;
    if(stat(source,NULL) == -1)
    {
        printf("系统找不到指定的文件\r\n");
        return false;
    }
    //如果操作的文件一样，返回，不操作，因为效果一样
    if(strcmp(source,destination)==0)
        return true;
    if(stat(destination,NULL) != -1)
    {
        second_fp = fopen(destination,"r");
        if(second_fp == NULL)
        {
            return false;
        }
        if(second_fp->file_attr.bits.archive)
        {
            printf("是否覆盖%s？ <yes/no>", destination);
            fclose(second_fp);
            while(1)
            {
                EasyScanf(NULL,s);
                if(s[0] == 'y'||s[0] == 'Y')
                {
                    Djy_PutChar(s[0]);
                    printf("\r\n");
                    remove(destination);//为覆盖做准备
                    break;
                }
                else if(s[0] == 'n'||s[0] == 'N')
                {
                    Djy_PutChar(s[0]);
                    printf("\r\n");
                    return false;
                }
            }
        }
        else
        {
            //是目录，需要增加反斜杠，以便下面拷贝路径使用
            ShFs_StrEndIncreaseBackSlash(destination);
            fp = fopen(source,"r");
            if(fp == NULL)
            {
                fclose(second_fp);
                return false;
            }
            if(fp->file_attr.bits.archive)
            {//如果source_file是文件，把文件名插入到second_work_path末尾
            //使用这个函数，还得提供准确的路径。
                ShFs_StrEndWipeBackSlash(source);
                __Djyfs_PickFileNameWord(source,s);
                strcat(destination,s);
             }
                fclose(second_fp);
                fclose(fp);
        }
    }
    else//如果destination不存在，那么全部按照文件处理
        ShFs_StrEndWipeBackSlash(source);//可以不用加反斜杠吧?
    fp = fopen(source,"r");
    if(fp == NULL)
        return 0;
    if(!fp->file_attr.bits.folder)//source是一个文件
    {
        fclose(fp);
        Shfs_CopyFileToDest(source,destination);
        printf("已经复制了      1个文件\r\n");
    }
    else//source是一个目录
    {
        fclose(fp);
        ShFs_StrEndIncreaseBackSlash(source);
        Shfs_CopyFileOfFolderToDest(source,destination);
    }
    return true;
}


//----将至少一个文件移动到另外一个位置--------------------------------------------------
//功能：将至少一个文件移动到另外一个位置
//参数：PTT_name 分区名
//返回：true 成功
//-----------------------------------------------------------------------------
bool_t ShFs_Move(char *param)
{
    char *next_param,source[255],destination[255];
    struct tagFileRsc *fp,*second_fp;
    char s[255];
    if(param == NULL)
    {
        printf("命令语法不正确!\r\n");
        return false;
    }
    strcpy(source,Sh_GetWord(param,&next_param));

    if(NULL == next_param)
    {
        printf("djyfs不支持此操作，详情请看help\r\n");
        return false;
    }
    strcpy(destination,Sh_GetWord(next_param,&next_param));
    if((!__Djyfs_CheckFullName(source))||(!__Djyfs_CheckFullName(destination)))
        return false;
    if(stat(source,NULL) == -1)
    {
        printf("系统找不到指定的文件\r\n");
        return false;
    }
    //如果操作的文件一样，返回，不操作，因为效果一样
    if(strcmp(source,destination)==0)
        return true;
    if(stat(destination,NULL) != -1)
    {
        second_fp = fopen(destination,"r");
        if(second_fp == NULL)
        {
            return false;
        }
        if(second_fp->file_attr.bits.archive)
        {
            printf("是否覆盖%s？ <yes/no>", destination);
            fclose(second_fp);
            while(1)
            {
                EasyScanf(NULL,s);
                if(s[0] == 'y')
                {
                    Djy_PutChar(s[0]);
                    printf("\r\n");
                    remove(destination);//为覆盖做准备
                    break;
                }
                else if(s[0] == 'n')
                {
                    Djy_PutChar(s[0]);
                    printf("\r\n");
                    return false;
                }
            }
        }
        else
        {
            //是目录，需要增加反斜杠，以便下面拷贝路径使用
            ShFs_StrEndIncreaseBackSlash(destination);
            fp = fopen(source,"r");
            if(fp == NULL)
            {
                fclose(second_fp);
                return false;
            }
            if(fp->file_attr.bits.archive)
            {//如果source_file是文件，把文件名插入到second_work_path末尾
            //使用这个函数，还得提供准确的路径。
                ShFs_StrEndWipeBackSlash(source);
                __Djyfs_PickFileNameWord(source,s);
                strcat(destination,s);
             }
                fclose(second_fp);
                fclose(fp);
        }
    }
    else//如果destination不存在，那么全部按照文件处理
        ShFs_StrEndWipeBackSlash(source);
    fp = fopen(source,"r");
    if(fp == NULL)
        return 0;
    if(!fp->file_attr.bits.folder)//source是一个文件
    {
        fclose(fp);
        Shfs_MoveFileToDest(source,destination);
        printf("已经移动了      1个文件\r\n");
    }
    else//source是一个目录
    {
        fclose(fp);
        ShFs_StrEndIncreaseBackSlash(source);
        Shfs_MoveFileOfFolderToDest(source,destination);
    }
    return true;
}

//----修改指定文件或者文件夹的名字------------------------------------------------
//功能: 这是命令ren，修改指定的文件或者文件夹
//参数: param，输入参数
//返回: true=成功,
//      false=失败，
//-----------------------------------------------------------------------------
bool_t ShFs_Ren(char *param)
{
    char *next_param;
    char *fullname;//指定被修改的文件
    char *filename;//修改后的名字

    if(param == NULL)
    {
        printf("命令语法不正确!\r\n");
        return false;
    }
    fullname = Sh_GetWord(param,&next_param);
    filename = Sh_GetWord(next_param,&next_param);
    if(!__Djyfs_CheckFullName(fullname))
        return false;
    if(stat(fullname,NULL) == -1)
    {
        printf("系统找不到指定的文件\r\n");
         return false;
    }
    rename(fullname,filename);
    return true;

}
//----创建一个或者多个目录------------------------------------------------
//功能: 这是命令md，创建一个或者多个目录
//参数: param，输入参数
//返回: true=成功,
//      false=失败，
//-----------------------------------------------------------------------------
bool_t ShFs_MD(char *param)
{
    char *next_param;
    char *del_param;
    //限制255字符，因为fs_mkdir的参数，要进行增加字符串
    //如果不限制，会内存泄漏。造成不可控
    char str[CN_SHELL_CMD_LIMIT+1];

    del_param = Sh_GetWord(param,&next_param);
    if(del_param == NULL)
    {
        printf("命令语法不正确!\r\n");
        return false;
    }
    strcpy(str,del_param);
    do
    {
        mkdir(str);
        del_param = Sh_GetWord(next_param,&next_param);
        if(del_param != NULL)
            strcpy(str,del_param);
    }while(del_param!=NULL);
    return true;
}


//----删除一个或者多个文件夹里的文件-------------------------------------------
//功能: 把指定的一个或者多个文件夹删除掉，只能删空目录
//参数: rd_param，输入字符
//返回: true=成功,
//      false=失败，
//-----------------------------------------------------------------------------
bool_t ShFs_RD(char *param)
{
    char *next_param = param;
    char *del_param;

    if(param == NULL)
    {
        printf("命令语法不正确!\r\n");
        return false;
    }


    while(1)
    {
        del_param = Sh_GetWord(next_param,&next_param);
        if(del_param == NULL)
            break;
        if(__Shfs_CheckWildCard(del_param))     //不允许带通配符删除文件夹
        {
            printf("命令语法不正确!\r\n");
        }
        else
            ShFs_RemoveFolder(del_param);
    }

    return true;
}

//----删除一个或者多个文件-----------------------------------------------------
//功能: 删除一个或者多个文件
//参数: param，输入字符
//返回: true=成功,
//      false=失败，
//-----------------------------------------------------------------------------
bool_t ShFs_Del(char *param)
{
    char *next_param;
    char *del_param;

    //参数不能为空字符
    if(param == NULL)
    {
        printf("命令语法不正确!\r\n");
        return false;
    }
    next_param = param;
    while(1)
    {
        del_param = strsep(&next_param," ");
        if(del_param == NULL)
            break;
        if(del_param[0] == ' ')     //遇到连续空格
            break;
        if( ! __Shfs_JudgeBackSlashIsData(del_param))     //路径不包含文件名
        {
            printf("%s 文件名不正确!\r\n",del_param);
            break;
        }
        if(__Shfs_CheckWildCardPath(del_param))     //路径部分不允许带通配符
        {
            printf("%s 文件名不正确!\r\n",del_param);
        }
        else if(__Shfs_CheckWildCardName(del_param))   //文件名带通配符
        {
            ShFs_DelWildcardFiles(del_param);
        }
        else
        {
            if(__Shfs_JudgeItemAttr(del_param) == CN_ITEM_IS_FILE) //不删除目录
            {
                if(remove(del_param) == 0)
                    printf("已删除 %s \r\n",del_param);
                else
                    printf("删除 %s 错误\r\n",del_param);
            }
        }
    }

    return true;
}

//----显示当前目录的名称或改变当前目录-----------------------------------------
//功能: 显示当前目录的名称或改变当前目录
//参数: chdir_param，输入字符
//返回: true=成功,
//      false=失败，
//-----------------------------------------------------------------------------
bool_t ShFs_CD(char *chdir_param)
{
    bool_t result;
    struct tagFileRsc *fp;
    char work_path[CN_SHELL_CMD_LIMIT+1];

    //执行cd/chdir[](空字符) 显示当前目录名称
    if(chdir_param == NULL)
    {
        Sh_PrintWorkPath();
        printf("\r\n");
        return true;
    }
    //方便用户输入目录的时候不用输入反斜杠
    //如"cd folder\"变成"cd folder"
    //执行cd/chdir[\] 更换到根目录
    if(strcmp(chdir_param,"\\") ==0)
    {
        if(!Djyfs_GetPTT_Name(work_path,CN_SHELL_CMD_LIMIT))
            return false;
        strcat(work_path,":\\");
        fp=g_ptWorkPath;
        g_ptWorkPath = fopen(work_path,"r");
        fclose(fp);
        return true;
    }
    //执行cd/chdir[..] 更换到上一级目录
    else if(chdir_param[0]=='.' && chdir_param[1]=='.')//执行 "cd .."
    {
        if(ShFs_CD_Parent(chdir_param))
            return true;
        else
            return false;
    }
    else
    {
        if(!__Djyfs_CheckFullName(chdir_param))
             return false;
        result = stat(chdir_param,NULL);
        //执行 cd/chdir[分区：]路径 显示当前目录的名称或改变当前目录
        if(result != -1)
        {
            //是目录才能打开
            if(__Shfs_JudgeItemAttr(chdir_param) == CN_ITEM_IS_FOLDER)
            {
                fp = g_ptWorkPath;
                g_ptWorkPath = fopen(chdir_param,"r");
                fclose(fp);
                return true;
            }
            else if(!__Shfs_JudgeBackSlashIsData(chdir_param))
            {//如C:\,在judge_is_folder里无法成功判断
                fp = g_ptWorkPath;
                g_ptWorkPath = fopen(chdir_param,"r");
                fclose(fp);
                return true;
            }
            else
            {
                printf("系统找不到指定的路径\r\n");
                return false;
            }
        }
        else
                printf("系统找不到指定的路径\r\n");
    }
     return true;
}

//-------显示指定目录下的文件和子目录列表--------------------------------------
//功能: 显示指定目录下的文件和子目录列表
//参数: param，输入字符
//返回: true=成功,
//      false=失败，
//-----------------------------------------------------------------------------
bool_t ShFs_Dir(char *param)
{
    char *dir_param;
    char *next_param = param;
    if(param == NULL)
    {
        ShFs_DirItemSon(g_ptWorkPath);
        return true;
    }

    do
    {
        dir_param = strsep(&next_param," ");
        if(dir_param == NULL)
            break;
        if(dir_param[0] == ' ')     //遇到连续空格
            break;
        Shfs_Dir(dir_param);
    }while(dir_param != NULL);

    return true;
}

//-----------------------------------------------------------------------------
//功能: 创建分区
//参数: param: 分区尺寸(字节为单位) 分区名
//返回: true=成功,
//      false=失败，
//------------
//更新记录:
// 1.日期: 2015/3/11
//   说明: 修改至能够运行
//   作者: 季兆林
// 2.日期: 2015/3/19
//   说明: 修正chip未正确使用的问题
//   作者: 季兆林
// todo: 这是专门为flashfile写的shell么?
//-----------------------------------------------------------------------------
bool_t ShFs_CreatePTT(char *param)
{

    struct tagFlashChip *chip;
    struct tagRscNode *chip_tree;
    u64 dbx_size;//分区尺寸, 字节为单位
    u32 blocks;//分区尺寸转
    char *word, *next_param;
    bool_t DeviceFound = false;



    chip_tree = Rsc_SearchTree("flash chip"); //获取Flash资源根节点,用于查询设备结构体
    if(NULL != chip_tree){
        chip = (struct tagFlashChip *)Rsc_GetSon(chip_tree);
        if(NULL != chip)
            DeviceFound = true;
    }

    if(!DeviceFound)
    {
        printf("分区设备不存在.\r\n");
        return (false);
    }

    word = Sh_GetWord(param, &next_param);
    dbx_size = strtol(word, (char **)NULL, 0);
    blocks = dbx_size / chip->block_size + 1; //分区大小以块计
    if(blocks > chip->valid_sum)
    {
        printf("尺寸太大\r\n");
        return false;
    }
    word = Sh_GetWord(next_param, &next_param);
    if(word == NULL)
    {
        printf("分区名不能为空\r\n");
        return false;
    }


    if(0 != DFFSD_Create_PTT(chip, blocks, word))
    {
        printf("创建分区成功\r\n");
        return true;
    }
    else
    {
        printf("创建分区失败\r\n");
        return false;
    }
}

bool_t ShFs_Format(char *param)
{
    if(param == NULL)
    {
        printf("分区名不能为空");
        return true;
    }

    if(true == Djyfs_Format(1, 0, param))
    {
        printf("格式化成功");
    }
    else
    {
        printf("格式化失败");
    }
    printf("\r\n");
    return true;
}

//----打印当前目录-------------------------------------------------
//功能: 打印当前目录。
//参数: node，得到的资源节点
//返回: true，打印成功
//      false,打印失败
//-----------------------------------------------------------------------------
bool_t Sh_PrintWorkPath(void)
{
    u32 len;
    char *buf;
    len = Djyfs_GetCwdLen( );
    buf = M_MallocLc(len,0);
    if(buf == NULL)
        return false;
    Djyfs_GetCwd(buf,len);
    printf("\r\n%s>", buf);
    free(buf);
    return true;
}


bool_t ShFs_TestWrite(char *param)
{
    FILE *fp;
    u16 i;
    u8 write_buf[100];

    for(i=0;i<100;i++)
        write_buf[i]=i;

    fp = fopen("sys:\\Test_W_R.dbx","w+");
    if(fp == NULL)
    {
        printf("文件打开有误");
    }
    else
    {
        if(fwrite(write_buf,100,1,fp) != 100)
        {
            printf("文件写入有误");
        }
        else
        {
            printf("文件写入成功");
        }

    }
    printf("\r\n");
    fclose(fp);

    return true;
}

bool_t ShFs_TestRead(char *param)
{
    FILE *fp;
    u16 i;
    u8 read_buf[100];

    fp = fopen("sys:\\Test_W_R.dbx","r");
    if(fp == NULL)
    {
        printf("文件打开有误");
    }
    else
    {
        if(fread(read_buf,100,1,fp) != 100)
        {
            printf("文件读出有误");
        }
        else
        {
            for(i=0;i<100;i++)
            {
                if(read_buf[i]!=i)
                {
                    printf("文件校验有误");
                    i = 200;
                }
            }
            if(i<200)
                printf("文件校验成功");
        }
    }
    printf("\r\n");
    fclose(fp);

    return true;
}

