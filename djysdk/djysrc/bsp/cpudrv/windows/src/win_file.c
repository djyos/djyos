//----------------------------------------------------
//Copyright (C), 2004-2009,  lst.
//版权所有 (C), 2004-2009,   lst.
//所属模块:flash文件系统
//作者：lst
//版本：V1.0.0
//文件描述:djyfs的windows文件系统驱动接口，
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

#include <windows.h>
#include <tchar.h>
#include <ntsecapi.h>
#include "stdint.h"
#include "stdlib.h"
#include "time.h"
#include "file.h"
#include "lock.h"

#define From1601To1970US    10000000000
//#pragma comment (lib,"ntdll_32.lib")  // Copy From DDK  64位系统改为ntdll_64.lib
//#pragma comment(linker, "/subsystem:windows")
//#pragma comment(linker, "/entry:start")
//#pragma comment(linker, "/filealign:0x200")


#define cn_winpath_limit    260
static char gc_windows_workpath[cn_winpath_limit + 2];
//----移动文件写指针-----------------------------------------------------------
//功能: 把一个文件的写指针移动到一个新位置
//参数: PTT_flash_tag，被操作的flash分区的存储媒体标签
//      fp，目标文件指针
//      new_ptr，新的写指针
//返回: true=成功，false=有错误
//-----------------------------------------------------------------------------
bool_t __winfs_move_file_write_ptr(FILE *fp,sint64_t new_ptr)
{
    switch (fp->open_mode)
    {
        case EN_R_RB:
        {
            return false;                       //只读文件，写位置无效
        }break;
        case EN_W_WB:
        {
            fp->write_ptr = new_ptr;            //写位置
        }break;
        case EN_A_AB:
        {
            fp->write_ptr = new_ptr;            //写位置
        }break;
        case EN_R_RB_PLUS:
        {
            fp->read_ptr = new_ptr;             //读位置
            fp->write_ptr = new_ptr;            //写位置
        }break;
        case EN_W_WB_PLUS:
        {
            fp->read_ptr = new_ptr;             //读位置
            fp->write_ptr = new_ptr;            //写位置
        }break;
        case EN_A_AB_PLUS:
        {
            fp->write_ptr = new_ptr;            //写位置
        }break;
        default: return false;
    }
    if(new_ptr > fp->file_size)       //写指针超出文件长度
    {
        fp->file_size = fp->write_ptr;  //用写指针覆盖文件长度
    }
    return true;
}


//本函数功能就是GetFinalPathNameByHandle的返回值减去当前目录，可惜
//GetFinalPathNameByHandle函数在xp及以下版本的windows中不支持。
//如果hnd是通过look_for_item得到的，怎么办?该hnd不在资源链表中!!    lst
void __winfs_GetFileNameFromRsc(char *name,struct FileRsc *parent,char *buf)
{
//    char name_temp[cn_winpath_limit+1];
    char *pwn;
    pwn = &parent->name[strlen(parent->name)+1];
    strcpy(buf,pwn);
    if(name != NULL)
    {
//        mbstowcs(name_temp,name,cn_winpath_limit+1);
        strcat(buf,_T("\\"));
        strcat(buf,name);
    }
    return ;
}

//----写入数据到文件------------------------------------------------------------
//功能：把数据写入文件，实际上调用windows的WriteFile函数执行写入
//参数：buf，输入缓冲区
//      len，本次写入的数据量
//      fp，被操作的文件指针
//返回：本次写入的数据量
//-----------------------------------------------------------------------------
uint32_t winfs_write_file(struct FileRsc *fp,uint8_t *buf,uint32_t len)
{
    uint32_t completed = 0;
    if((fp==NULL)||(len==0)||(buf==NULL))
        return 0;
    if(! WriteFile((HANDLE)(fp->file_medium_tag),buf,len,(PDWORD)&completed,NULL))
        completed = 0;
    __winfs_move_file_write_ptr(fp,fp->write_ptr + completed);
    return completed;
}

//----读出数据-----------------------------------------------------------------
//功能；把文件中的数据读入到缓冲区中，实际上调用windows的ReadFile函数读取
//参数：buf，输入缓冲区
//      len，读出的数据量
//      fp，被操作的文件指针
//返回：实际读取的字节数
//-----------------------------------------------------------------------------
uint32_t winfs_read_file(struct FileRsc *fp,uint8_t *buf,uint32_t len)
{
    uint32_t completed = 0;
    if((fp==NULL)||(len==0)||(buf==NULL))
        return 0;
    if(! ReadFile((HANDLE)(fp->file_medium_tag),buf,len,(PDWORD)&completed,NULL))
        completed = 0;
    fp->read_ptr += completed;
    return completed;
}

//----刷写缓冲区---------------------------------------------------------------
//功能: 把文件写缓冲区的数据写入flash
//参数：fp，被操作的文件指针
//返回：实际写入flash的字节数
//-----------------------------------------------------------------------------
uint32_t winfs_flush_file(struct FileRsc *fp)
{
    return FlushFileBuffers((HANDLE)(fp->file_medium_tag));
}

//----查询文件可读数据量-------------------------------------------------------
//功能：查询一个文件内有多少可读数据，对于固态文件（如flash文件）来说，就是文件
//      长度-当前指针，但流的标准定义并非一定如此，比如通信端子。而且，文件读写
//      指针如何表示与具体文件系统驱动的实现有关，文件系统的核心程序不应该做任何
//      假设。
//参数：fp，被操作的文件指针
//返回：文件可读数据的字节数
//-----------------------------------------------------------------------------
sint64_t winfs_query_file_stocks(struct FileRsc *fp)
{
    return fp->file_size - fp->read_ptr;
}

//----查询文件可写数据量-------------------------------------------------------
//功能：查询一个文件还能写入多少数据，对于固态文件（如flash文件），可能就是文件
//      柜的剩余空间，对于windows模拟器来说，就是模拟器所在的磁盘剩余空间
//参数：fp，被操作的文件指针
//返回：文件还能写入的字节数
//-----------------------------------------------------------------------------
s64 winfs_query_file_cubage(struct FileRsc *fp)
{
    ULARGE_INTEGER size;
    if(GetDiskFreeSpaceEx(NULL,&size,NULL,NULL))
        return (s64)size.LowPart + ((s64)size.HighPart<<32);
    else
        return 0;
}

//----设置文件长度-------------------------------------------------------------
//功能：设置文件长度，短于实际长度将截尾，长于实际长度将追加空间。
//参数：fp，被操作的文件指针
//      new_size，新的文件长度。
//返回：新的文件长度，若与new_size参数不等，一般是因为分区没有足够的容量导致
//-----------------------------------------------------------------------------
sint64_t winfs_set_file_size(struct FileRsc *fp,s64 new_size)
{
    LARGE_INTEGER liDistanceToMove;
    liDistanceToMove.QuadPart = new_size;
    SetFilePointerEx((HANDLE)(fp->file_medium_tag),liDistanceToMove,NULL,FILE_BEGIN);
    SetEndOfFile((HANDLE)(fp->file_medium_tag));
    //允许修改文件尺寸的所有模式，读写指针都是相同的，只需判断读指针即可
    if(fp->read_ptr < new_size)
    {
        liDistanceToMove.QuadPart = fp->read_ptr;
        //djyfs的定义，修改文件尺寸并不改变文件指针。
        SetFilePointerEx((HANDLE)(fp->file_medium_tag),liDistanceToMove,NULL,FILE_BEGIN);
    }
    else    //如果原指针超出文件的新长度，则改为新长度
    {
        fp->read_ptr = new_size;
        fp->write_ptr = new_size;
    }
    fp->file_size = new_size;
    return new_size;
}

//----设置文件指针-------------------------------------------------------------
//功能：把文件指针设置到指定的位置，并相应地同步读写缓冲区。
//参数：fp，被操作的文件指针
//      position的成员:
//          offset，移动的位置
//          whence，从何处开始计算移动长度，参见SEEK_CUR等的定义
//返回：0=成功，1=失败
//-----------------------------------------------------------------------------
uint32_t winfs_seek_file(struct FileRsc *fp,struct SeekPara *position)
{
    sint64_t new_position;
//    uint32_t buffed_size;
    uint8_t  whence;
    LARGE_INTEGER liDistanceToMove;
    if((position == NULL) ||(fp == NULL))
        return 1;
    whence = position->whence;
    switch (fp->open_mode)
    {
        case EN_R_RB:           //只读方式，需移动读指针和读缓冲区
        case EN_A_AB_PLUS:      //写追加方式，写指针不能变，读指针的方式与
                                  //只读文件一致
        {
            if(whence == SEEK_SET)
                new_position = position->offset;
            else if(whence == SEEK_CUR)
                new_position = position->offset + fp->read_ptr;
            else if(whence == SEEK_END)
                new_position = fp->file_size + position->offset;
            else
                return 1;
            if(fp->read_ptr == new_position)
                return 0;
            if(new_position > fp->file_size)
                fp->read_ptr = fp->file_size;
            else if(new_position < 0)
                fp->read_ptr = 0;
            else
                fp->read_ptr = new_position;
            liDistanceToMove.QuadPart = fp->read_ptr;
            SetFilePointerEx((HANDLE)(fp->file_medium_tag),
                                liDistanceToMove,NULL,FILE_BEGIN);
        }break;
        case EN_W_WB:   //只写文件，需移动写指针
        {
            if(whence == SEEK_SET)
                new_position = position->offset;
            else if(whence == SEEK_CUR)
                new_position = position->offset + fp->write_ptr;
            else if(whence == SEEK_END)
                new_position = fp->file_size - position->offset;
            else
                return 1;
            if(fp->write_ptr == new_position)
                return 0;
            if(new_position < 0)
                new_position = 0;
            liDistanceToMove.QuadPart = new_position;
            SetFilePointerEx((HANDLE)(fp->file_medium_tag),
                                liDistanceToMove,NULL,FILE_BEGIN);
            fp->write_ptr = new_position;   //设置写指针
            if(new_position > fp->file_size)
            {
                SetEndOfFile((HANDLE)(fp->file_medium_tag));
            }
        }break;
        case EN_A_AB:   //追加写方式，seek操作无效
        {
        }break;
        case EN_R_RB_PLUS:  //这两者都是可读写方式，读写指针重合，处理方式相同
        case EN_W_WB_PLUS:
        {
            if(whence == SEEK_SET)
                new_position = position->offset;
            else if(whence == SEEK_CUR)
                new_position = position->offset + fp->read_ptr;
            else if(whence == SEEK_END)
                new_position = fp->file_size + position->offset;
            else
                return 1;

            if(fp->read_ptr == new_position)
                return 0;

            if(new_position < 0)
                new_position = 0;

            if(new_position > fp->file_size)    //新位置超出文件长度
            {
                liDistanceToMove.QuadPart = new_position;
                if(SetFilePointerEx((HANDLE)(fp->file_medium_tag),
                                liDistanceToMove,NULL,FILE_BEGIN))
                {
                    if(SetEndOfFile((HANDLE)(fp->file_medium_tag)))
                    {
                        fp->write_ptr = new_position;   //设置写指针
                        fp->read_ptr = new_position;    //设置读指针
                    }
                    else
                    {
                        liDistanceToMove.QuadPart = fp->write_ptr;
                        SetFilePointerEx((HANDLE)(fp->file_medium_tag),
                                            liDistanceToMove,NULL,FILE_BEGIN);
                    }
                }
            }
            else
            {
                liDistanceToMove.QuadPart = new_position;
                if(SetFilePointerEx((HANDLE)(fp->file_medium_tag),
                                liDistanceToMove,NULL,FILE_BEGIN))
                {
                    fp->write_ptr = new_position;   //设置写指针
                    fp->read_ptr = new_position;    //设置读指针
                }
            }
        }break;
        default: return 1;
    }
    return 0;
}

//----建立项目（文件/目录）----------------------------------------------------
//功能：建立文件(目录)，attr参数将表明建立目录还是文件。如果建立文件，则同时分配
//      1块存储器。删除文件内容使文件长度为0时，也不会把最后一块释放。总之，任何
//      情况下，文件至少占用1块存储器
//参数：name，目录（文件）名
//      parent，父目录指针。
//      attr，新文件（目录）的属性
//返回：true=成功，false=失败，一般是因为分区没有足够的容量导致
//特注：本函数不检查重名，由调用者保证。作为存储介质，专守存储数据的职责，是否
//      允许重名是上一层的责任。
//-----------------------------------------------------------------------------
bool_t winfs_create_item(char *name,struct FileRsc *parent,
                         struct FileRsc *result,union file_attrs attr,
                         enum _FILE_OPEN_MODE_ mode)
{
    s64 s64Time;
    char name_buf[cn_winpath_limit+1];
    u32 FlagsAndAttributes = 0;
    HANDLE fhandle;
//    struct tm *file_time;
    char *pchar,*pwn;

    printf("%s    %s \n\r",__FUNCTION__ ,name);
    if(parent == NULL)
        return false;
    if(parent->file_attr.bits.folder == 0)
    {
        //父目录不能是文件
        return false;
    }
    __winfs_GetFileNameFromRsc(name,parent,name_buf);
    if( attr.bits.folder) //创建目录
    {
        if(CreateDirectoryA(name_buf,NULL))
        {
            fhandle = CreateFileA( name_buf,
                        GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL,
                        OPEN_EXISTING,
                        FILE_FLAG_BACKUP_SEMANTICS,
                        NULL
                        );
        }
        else
        {
            return false;
        }
    }else
    {
        if(attr.bits.archive)
            FlagsAndAttributes |= FILE_ATTRIBUTE_ARCHIVE;
        if(attr.bits.read_only)
            FlagsAndAttributes |= FILE_ATTRIBUTE_READONLY;
        if(attr.bits.hidden)
            FlagsAndAttributes |= FILE_ATTRIBUTE_HIDDEN ;
        //关闭时删除临时文件的操作，在file.c的djyfs_fclose函数中完成。
//        if(attr.bits.close_delete)
//            FlagsAndAttributes |= FILE_FLAG_DELETE_ON_CLOSE  ;
        fhandle = CreateFileA( name_buf,
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL,
                    CREATE_NEW,
                    FlagsAndAttributes,
                    NULL
                    );
    }
    s64Time = Tm_Time(NULL);
#if 0
    file_time = Tm_LocalTime(&s64Time);      //取系统时间
    result->second_create = (u8)file_time->tm_sec;
    result->second_modify = (u8)file_time->tm_sec;
    result->minute_create = (u8)file_time->tm_min;
    result->minute_modify = (u8)file_time->tm_min;
    result->hour_create = (u8)file_time->tm_hour;
    result->hour_modify = (u8)file_time->tm_hour;
    result->date_create = (u8)file_time->tm_mday;
    result->date_modify = (u8)file_time->tm_mday;
    result->month_create = (u8)file_time->tm_mon;
    result->minute_modify = (u8)file_time->tm_mon;
    result->year_low_create = (u8)file_time->tm_year;
    result->year_high_create = (u8)file_time->tm_year>>8;
    result->year_low_modify = (u8)file_time->tm_year;
    result->year_high_modify = (u8)file_time->tm_year>>8;
#endif
    result->CreateTime = s64Time;
    result->ModTime = s64Time;
    result->file_attr = attr;

    strcpy(result->name,name);
    pchar = &result->name[strlen(result->name)+1];
    pwn = &parent->name[strlen(parent->name)+1];
    strcpy(pchar,pwn);
    strcat(pchar,"\\");
    strcat(pchar,name);
    result->file_size = 0;
    result->read_ptr = 0;
    result->write_ptr = 0;
    result->p_read_buf = NULL;
    result->p_write_buf = NULL;
    result->file_medium_tag = (ptu32_t)fhandle;
    result->file_semp = NULL;
    return true;
}

//----移动文件-----------------------------------------------------------------
//功能：把一个文件从一个目录移动到另一个目录
//参数：name，目录（文件）名
//      parent，父目录指针。
//      attr，新文件（目录）的属性
//返回：true=成功，false=失败，
//-----------------------------------------------------------------------------
bool_t winfs_move_file(struct FileRsc *src_fp,   struct FileRsc *dest_fp)
{
    return false;
}

//----检查文件夹的子文件（目录）数---------------------------------------------
//功能：检查文件夹的子文件（目录）数，不包含子目录的子条目
//参数：PTT_device_tag，被操作的分区指针，通用结构，非flash专用。
//      fp，目标文件夹的文件指针
//返回：子文件（目录）数
//-----------------------------------------------------------------------------
uint32_t winfs_check_folder(struct FileRsc *parent)
{
    char name_buf[cn_winpath_limit+1];
    u32 items_num = 0;
    HANDLE hWnd = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA ffd;

    __winfs_GetFileNameFromRsc("*",parent,name_buf);
    hWnd = FindFirstFileA(name_buf,&ffd);
    if (INVALID_HANDLE_VALUE != hWnd)
    {
        do
        {
#ifdef _MSC_VER     //VC的ffd.cFileName是宽字符
                if( (wcscmp(ffd.cFileName,_T(".")) == 0)
                    || (wcscmp(ffd.cFileName,_T("..")) == 0) )
                    continue;
#else               //minGW的ffd.cFileName是本地码字符
                if( (strcmp(ffd.cFileName,(".")) == 0)
                    || (strcmp(ffd.cFileName,("..")) == 0) )
                    continue;
#endif
            items_num++;
        }while (FindNextFile(hWnd, &ffd) != 0);

        FindClose(hWnd);
    }

    return items_num;
}

//----删除项目（文件/目录）---------------------------------------------------
//功能：删除一个文件(目录)，只能删除空目录或已经关闭的文件。
//参数：fp，被删除的目录（文件）指针
//返回：true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t winfs_remove_file(struct FileRsc *fp)
{
    char name_buf[cn_winpath_limit+1];

    if(fp == NULL)
        return false;
    if(( ! fp->file_attr.bits.folder) && (fp->open_counter != 0))
        return false;   //fp是一个已经打开的文件
    if(fp->file_attr.bits.folder)
    {
        if(winfs_check_folder(fp) != 0)
            return false;   //是一个非空目录
    }
    __winfs_GetFileNameFromRsc(NULL,fp,name_buf);
    if( ! fp->file_attr.bits.folder) //是个文件，需要释放文件占用的存储器
    {
        DeleteFileA(name_buf);
    }
    else
    {
        RemoveDirectoryA(name_buf);
    }
    return true;
}

//----遍历一个目录的子项-------------------------------------------------------
//功能: 从current_file开始,获取下parent的下一个文件的指针,直到parent
//      的子项搜索完毕.
//参数: parent,需要搜索的树枝的祖先结点
//      current_file,当前搜索位置,空则表示第一次查找
//返回: 搜索到的子项指针(可能是文件或目录)
//备注: 这个函数，可以作为windows下 FindFirstFile等三个函数的配合遍历整
//      个目录下的文件
//      第一次遍历时(以current==NULL)为标志，系统将为struct file_rsc结构分配内存，
//      最后一次，将释放内存。如果用户正确地调用winfs_item_traversal_son遍历完
//      整个目录，内存管理将完全正常，否则，为struct file_rsc结构分配的内存将需
//      要用户手动释放
//---------------------------------------------------------------------
struct FileRsc * winfs_item_traversal_son(struct FileRsc *parent,
                                            struct FileRsc *current)
{
    char name_buf[cn_winpath_limit+1];
    HANDLE *hWnd;
//    SYSTEMTIME syst;
    WIN32_FIND_DATA ffd;
    bool_t found = false;
    char *pchar,*pwn;

    if(parent == NULL)
        return NULL;
    if(current == NULL)
    {
        current = M_MallocLc(sizeof(struct FileRsc),CN_TIMEOUT_FOREVER + sizeof(HANDLE*));
        if(current == NULL)
            return NULL;
        hWnd = (HANDLE*)(current+1);
        __winfs_GetFileNameFromRsc("*",parent,name_buf);
        *hWnd = FindFirstFileA(name_buf,&ffd);
        if (INVALID_HANDLE_VALUE != *hWnd)
        {
            do
            {
#ifdef _MSC_VER     //VC的ffd.cFileName是宽字符
                if( (wcscmp(ffd.cFileName,_T(".")) == 0)
                    || (wcscmp(ffd.cFileName,_T("..")) == 0) )
                    continue;
#else               //minGW的ffd.cFileName是本地码字符
                if( (strcmp(ffd.cFileName,(".")) == 0)
                    || (strcmp(ffd.cFileName,("..")) == 0) )
                    continue;
#endif
                else
                {
                    found = true;
                    break;
                }
            }while (FindNextFile(*hWnd, &ffd) != 0);
            if(!found)
            {
                FindClose(*hWnd);
                free(current);
                return NULL;
            }

        }
    }
    else
    {
        hWnd = (HANDLE*)(current+1);    //+1是首次给current分配内存时多分配的
        if(FindNextFile(*hWnd, &ffd) != 0)
            found = true;
        else
            FindClose(*hWnd);
    }

    if(found)
    {

        current->file_attr.all = 0;
        if(ffd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
        {
            current->file_attr.bits.read_only = 1;
        }
        if(ffd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
        {
            current->file_attr.bits.hidden = 1;
        }

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            current->file_attr.bits.folder =1;
        }
        else
        {
            current->file_attr.bits.archive =1;
        }
        current->file_size = (s64)(ffd.nFileSizeLow + (((u64)ffd.nFileSizeHigh)<<32));
        current->read_ptr = 0;
        current->write_ptr = 0;
        current->p_read_buf = NULL;
        current->p_write_buf = NULL;
        current->file_medium_tag = (ptu32_t)(*hWnd);
        current->file_semp = NULL;
        current->open_mode = 0;

#ifdef _MSC_VER     //VC的ffd.cFileName是宽字符
        wcstombs(current->name,ffd.cFileName,cn_winpath_limit);
#else               //minGW的ffd.cFileName是宽字符
        strcpy(current->name,ffd.cFileName);
#endif
        pchar = &current->name[strlen(current->name)+1];
        pwn = &parent->name[strlen(parent->name)+1];
        strcpy(pchar,pwn);
        strcat(pchar,"\\");
        strcat(pchar,current->name);

        current->CreateTime = (s64)ffd.ftCreationTime.dwLowDateTime
                              +((s64)ffd.ftCreationTime.dwLowDateTime<<32)
                              - (s64)From1601To1970US;
        current->ModTime    = (s64)ffd.ftLastWriteTime.dwLowDateTime
                              +((s64)ffd.ftLastWriteTime.dwLowDateTime<<32)
                              - (s64)From1601To1970US;
#if 0
        FileTimeToSystemTime(&ffd.ftCreationTime,&syst);

        current->second_create = syst.wSecond;
        current->minute_create = syst.wMinute;
        current->hour_create = syst.wHour;
        current->date_create = syst.wDay;
        current->month_create = syst.wMonth ;
        current->year_low_create = syst.wYear/100;
        current->year_high_create = syst.wYear%100;

        FileTimeToSystemTime(&ffd.ftLastWriteTime,&syst);
        current->second_modify = syst.wSecond;
        current->minute_modify = syst.wMinute;
        current->hour_modify = syst.wHour;
        current->date_modify = syst.wDay;
        current->month_modify = syst.wMonth ;
        current->year_low_modify = syst.wYear/100;
        current->year_high_modify = syst.wYear%100;
#endif
        return current;
    }
    else
    {
        free(current);
        return NULL;
    }
}


//----打开（文件/目录）--------------------------------------------------------
//功能：在分区中查找并打开文件(目录)，如果是文件，还需要为它分配读写缓冲区。只
//      查父目录的子项，不查孙项及以下
//参数：name，文件（目录）名
//      parent，文件/目录的父目录，
//      result，返回找到的文件的信息
//      mode，文件（目录）打开模式
//返回：cn_fs_open_success,成功打开文件(目录)
//      CN_FS_ITEM_EXIST,文件(目录)存在但不能打开(一般是模式不兼容)
//      CN_FS_ITEM_INEXIST,文件(目录)不存在
//      cn_limit_uint32，其他错误
//----------------------------------------------------------------------------
uint32_t winfs_open_item(char *name,struct FileRsc *parent,
                       struct FileRsc *result,enum _FILE_OPEN_MODE_ access_mode)
{
    char name_buf[cn_winpath_limit+1];
    u32 dwCreationDisposition = 0;
    u32 dwDesiredAccess = 0;
    LARGE_INTEGER liDistanceToMove;
    HANDLE fhandle;
    WIN32_FIND_DATA ffd;
//    SYSTEMTIME syst;
    char *pchar,*pwn;

    if(parent == NULL)
        return CN_LIMIT_UINT32;
    if(parent->file_attr.bits.folder == 0)
    {
        //父目录不能是文件
        return CN_LIMIT_UINT32;
    }
    __winfs_GetFileNameFromRsc(name,parent,name_buf);
    fhandle = FindFirstFileA(name_buf,&ffd);
    FindClose(fhandle);
    if (INVALID_HANDLE_VALUE != fhandle)
    {

        result->file_attr.all = 0;
        if(ffd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
        {
            result->file_attr.bits.read_only = 1;
        }
        if(ffd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
        {
            result->file_attr.bits.hidden = 1;
        }

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            fhandle = CreateFileA( name_buf,
                        GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL,
                        OPEN_EXISTING,
                        FILE_FLAG_BACKUP_SEMANTICS,
                        NULL
                        );
            if (INVALID_HANDLE_VALUE == fhandle)
                return CN_FS_ITEM_EXIST;
            result->file_attr.bits.folder =1;
        }
        else
        {
            if(access_mode== EN_R_RB)
                dwDesiredAccess = GENERIC_READ;
            else
            {
                if(ffd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
                {
                    return CN_FS_ITEM_EXIST;
                }
                else
                    dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
            }

            if( (access_mode == EN_W_WB) || (access_mode == EN_W_WB_PLUS))
                dwCreationDisposition = TRUNCATE_EXISTING;
            else
                dwCreationDisposition = OPEN_EXISTING;
            fhandle = CreateFileA( name_buf,
                        dwDesiredAccess,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL,
                        dwCreationDisposition,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL
                        );
            if (INVALID_HANDLE_VALUE == fhandle)
                return CN_FS_ITEM_EXIST;

            result->file_attr.bits.archive =1;
        }
        result->file_size = (s64)(ffd.nFileSizeLow + (((u64)ffd.nFileSizeHigh)<<32));
        if(access_mode == EN_A_AB)
        {
            result->read_ptr = result->file_size;
            result->write_ptr = result->file_size;
            liDistanceToMove.QuadPart = result->file_size;
            SetFilePointerEx((HANDLE)(result->file_medium_tag),
                                            liDistanceToMove,NULL,FILE_BEGIN);
        }
        else
        {
            result->p_read_buf = NULL;
            result->p_write_buf = NULL;
        }
        result->file_medium_tag = (ptu32_t)fhandle;
        result->file_semp = NULL;

        strcpy(result->name,name);
        pchar = &result->name[strlen(result->name)+1];
        pwn = &parent->name[strlen(parent->name)+1];
        strcpy(pchar,pwn);
        strcat(pchar,"\\");
        strcat(pchar,name);

        result->CreateTime = (s64)ffd.ftCreationTime.dwLowDateTime
                              +((s64)ffd.ftCreationTime.dwLowDateTime<<32)
                              - (s64)From1601To1970US;
        result->ModTime    = (s64)ffd.ftLastWriteTime.dwLowDateTime
                              +((s64)ffd.ftLastWriteTime.dwLowDateTime<<32)
                              - (s64)From1601To1970US;
#if 0
        FileTimeToSystemTime(&ffd.ftCreationTime,&syst);

        result->second_create = syst.wSecond;
        result->minute_create = syst.wMinute;
        result->hour_create = syst.wHour;
        result->date_create = syst.wDay;
        result->month_create = syst.wMonth ;
        result->year_low_create = syst.wYear/100;
        result->year_high_create = syst.wYear%100;

        FileTimeToSystemTime(&ffd.ftLastWriteTime,&syst);
        result->second_modify = syst.wSecond;
        result->minute_modify = syst.wMinute;
        result->hour_modify = syst.wHour;
        result->date_modify = syst.wDay;
        result->month_modify = syst.wMonth ;
        result->year_low_modify = syst.wYear/100;
        result->year_high_modify = syst.wYear%100;
#endif
    }
    else
        return CN_FS_ITEM_INEXIST;
    return CN_FS_OPEN_SUCCESS;
}



//----查找（文件/目录）--------------------------------------------------------
//功能：在分区中查找文件(目录)
//参数：name，文件（目录）名
//      parent，文件/目录的父目录，
//      result，返回找到的文件的信息
//返回：true=文件/目录存在，并在result中返回文件信息(不分配文件缓冲区)。
//      false = 文件/目录不存在，不修改result。
//----------------------------------------------------------------------------
bool_t winfs_lookfor_item(char *name,struct FileRsc *parent,
                         struct FileRsc *result)
{
    char name_buf[cn_winpath_limit+1];
    HANDLE fhandle;
    WIN32_FIND_DATA ffd;
//    SYSTEMTIME syst;
    char *pchar,*pwn;

    if(parent == NULL)
        return false;
    if(parent->file_attr.bits.folder == 0)
    {
        //父目录不能是文件
        return false;
    }
    __winfs_GetFileNameFromRsc(name,parent,name_buf);

    fhandle = FindFirstFileA(name_buf,&ffd);
    FindClose(fhandle);
    if (INVALID_HANDLE_VALUE != fhandle)
    {
        result->file_attr.all = 0;
        if(ffd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
        {
            result->file_attr.bits.read_only = 1;
        }
        if(ffd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
        {
            result->file_attr.bits.hidden = 1;
        }

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            result->file_attr.bits.folder =1;
        }
        else
        {
            result->file_attr.bits.archive =1;
        }
        result->file_size = (s64)(ffd.nFileSizeLow + (((u64)ffd.nFileSizeHigh)<<32));
        result->read_ptr = 0;
        result->write_ptr = 0;
        result->p_read_buf = NULL;
        result->p_write_buf = NULL;
        result->file_medium_tag = (ptu32_t)fhandle;
        result->file_semp = NULL;
        result->open_mode = 0;

        strcpy(result->name,name);
        pchar = &result->name[strlen(result->name)+1];
        pwn = &parent->name[strlen(parent->name)+1];
        strcpy(pchar,pwn);
        strcat(pchar,"\\");
        strcat(pchar,name);

        result->CreateTime = (s64)ffd.ftCreationTime.dwLowDateTime
                              +((s64)ffd.ftCreationTime.dwLowDateTime<<32)
                              - (s64)From1601To1970US;
        result->ModTime    = (s64)ffd.ftLastWriteTime.dwLowDateTime
                              +((s64)ffd.ftLastWriteTime.dwLowDateTime<<32)
                              - (s64)From1601To1970US;
#if 0
        FileTimeToSystemTime(&ffd.ftCreationTime,&syst);

        result->second_create = syst.wSecond;
        result->minute_create = syst.wMinute;
        result->hour_create = syst.wHour;
        result->date_create = syst.wDay;
        result->month_create = syst.wMonth ;
        result->year_low_create = syst.wYear/100;
        result->year_high_create = syst.wYear%100;

        FileTimeToSystemTime(&ffd.ftLastWriteTime,&syst);
        result->second_modify = syst.wSecond;
        result->minute_modify = syst.wMinute;
        result->hour_modify = syst.wHour;
        result->date_modify = syst.wDay;
        result->month_modify = syst.wMonth ;
        result->year_low_modify = syst.wYear/100;
        result->year_high_modify = syst.wYear%100;
#endif
        return true;
    }
    else
        return false;
}



//----关闭文件(目录)-----------------------------------------------------------
//功能: 关闭一个打开的文件(目录)，如果是文件，则释放读写缓冲区，并使分区中
//      opened_sum减量，如果是目录，则只是opened_sum减量。本函数只是处理关闭过程
//      与存储介质相关的操作，系统资源链表由file.c模块处理。
//参数：fp，被操作的文件指针
//返回：true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t winfs_close_item(struct FileRsc *fp)
{
    if(fp == NULL)
        return false;
    if(CloseHandle((HANDLE)fp->file_medium_tag))
        return true;
    else
        return false;
}

//----文件（目录）改名---------------------------------------------------------
//功能：修改一个文件（目录）的名字，新名字保存在fp中。fp中的名字可以达到255字
//      符，但只有前cn_flash_file_name_limit字符有效。
//参数：fp，被操作的文件指针
//返回：true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t winfs_rename_item(struct FileRsc *fp,char *newname)
{
    return true;
}

//----查询分区参数-----------------------------------------------------------
//功能: 返回分区总空间，有效空间，空闲空间的字节数
//参数:
//返回: 无
//-----------------------------------------------------------------------------
void winfs_check_PTT(struct PTTDevice *PTT_device_tag,
                     sint64_t *sum_size,sint64_t *valid_size,sint64_t *free_size)
{
    u32 SectorsPerCluster;
    u32 BytesPerSector;
    u32 NumberOfFreeClusters;
    u32 TotalNumberOfClusters;
    printf("%s     \n\r",__FUNCTION__);
    if( GetDiskFreeSpace(NULL,(PDWORD)&SectorsPerCluster,(PDWORD)&BytesPerSector,
                        (PDWORD)&NumberOfFreeClusters,(PDWORD)&TotalNumberOfClusters))
    {
        if(sum_size != NULL)
            *sum_size =     (s64)SectorsPerCluster
                          * (s64)BytesPerSector
                          * (s64)TotalNumberOfClusters;
        if(valid_size != NULL)
            *valid_size = *sum_size;
        if(free_size != NULL)
            *free_size =     (s64)SectorsPerCluster
                          * (s64)BytesPerSector
                          * (s64)NumberOfFreeClusters;
    }
    else
    {
        if(sum_size != NULL)
            *sum_size = 0;
        if(valid_size != NULL)
            *valid_size = 0;
        if(free_size != NULL)
            *free_size = 0;
    }
}

//----查询目录表尺寸-----------------------------------------------------------
//功能：查询一个分区的目录表尺寸，按字节计算。当应用程序需要读取整个目录表时，
//      必须先用本函数计算目录表尺寸，然后才能分配内存。
//参数：PTT_device_tag，被操作的分区指针，通用结构，非flash专用。
//返回：目录表尺寸
//-----------------------------------------------------------------------------
uint32_t winfs_check_FDT_size(struct PTTDevice *PTT_device_tag)
{
    printf("%s    \n\r",__FUNCTION__);
    return 0;
}
bool_t winfs_Format(u32 fmt_para1,u32 fmt_para2,struct PTTDevice *PTT_device_tag)
{
    return true;
}

//----winfs模块初始化----------------------------------------------------------
//功能: 就是在系统资源链表中添加flash芯片根结点
//参数: 无
//返回: 1
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_WinFs(ptu32_t para)
{
    static struct PTTDevice PTT_device_tag[3];
    struct FileRsc *root_file;
    char *pchar;
    char buf[256];
    u32 dbx_num = 0;
    HANDLE hWnd = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA ffd;
    SYSTEMTIME syst;
    syst.wSecond = 0;
    syst.wMinute = 0;
    syst.wHour = 0;
    syst.wDay = 1;
    syst.wMonth = 1;
    syst.wYear = 1970;
    SystemTimeToFileTime(&syst,&ffd.ftCreationTime);

    if( ! SetCurrentDirectory( _T("djyfs") ))
    {
        printf("初始化文件系统失败，检查运行目录下是否有\"djyfs\"目录");
        return 0;
    }

    GetCurrentDirectoryA(cn_winpath_limit+2,gc_windows_workpath);
    hWnd = FindFirstFileA("*",&ffd);
    if (INVALID_HANDLE_VALUE != hWnd)
    {
        do
        {
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
#ifdef _MSC_VER     //VC的ffd.cFileName是宽字符
                if( (wcscmp(ffd.cFileName,_T(".")) == 0)
                    || (wcscmp(ffd.cFileName,_T("..")) == 0) )
                    continue;
#else               //minGW的ffd.cFileName是本地码字符
                if( (strcmp(ffd.cFileName,(".")) == 0)
                    || (strcmp(ffd.cFileName,("..")) == 0) )
                    continue;
#endif
                PTT_device_tag[dbx_num].chip_mutex_tag = Lock_MutexCreate("winfile");
                if(PTT_device_tag->chip_mutex_tag == NULL)
                {
                    break;
                }
                PTT_device_tag[dbx_num].format = winfs_Format;
                PTT_device_tag[dbx_num].write = winfs_write_file;
                PTT_device_tag[dbx_num].read = winfs_read_file;
                PTT_device_tag[dbx_num].flush = winfs_flush_file;
                PTT_device_tag[dbx_num].query_file_stocks = winfs_query_file_stocks;
                PTT_device_tag[dbx_num].query_file_cubage = winfs_query_file_cubage;
                PTT_device_tag[dbx_num].check_PTT = winfs_check_PTT;
                PTT_device_tag[dbx_num].set_file_size = winfs_set_file_size;
                PTT_device_tag[dbx_num].seek_file = winfs_seek_file;
                PTT_device_tag[dbx_num].create_item = winfs_create_item;
            //    PTT_device_tag[dbx_num].move_file= winfs_move_file;
                PTT_device_tag[dbx_num].remove_item = winfs_remove_file;
                PTT_device_tag[dbx_num].open_item = winfs_open_item;
                PTT_device_tag[dbx_num].close_item = winfs_close_item;
                PTT_device_tag[dbx_num].lookfor_item = winfs_lookfor_item;
                PTT_device_tag[dbx_num].rename_item = winfs_rename_item;
                PTT_device_tag[dbx_num].item_traversal_son = winfs_item_traversal_son;
                PTT_device_tag[dbx_num].check_fdt_size = winfs_check_FDT_size;
                PTT_device_tag[dbx_num].read_fdt = (void (*)(struct PTTDevice *,u8 *))NULL_func;
                PTT_device_tag[dbx_num].check_folder = winfs_check_folder;
                PTT_device_tag[dbx_num].opened_sum = 0;

#ifdef _MSC_VER     //VC的ffd.cFileName是宽字符
                wcstombs(PTT_device_tag[dbx_num].name,
                                ffd.cFileName,cn_winpath_limit);
#else               //minGW的ffd.cFileName是本地码字符
                strcpy(PTT_device_tag[dbx_num].name,ffd.cFileName);
#endif
                PTT_device_tag[dbx_num].PTT_medium_tag = (ptu32_t)0;
                PTT_device_tag[dbx_num].formatted = true;
                Djyfs_FsDriverCtrl(enum_fs_add_PTT,
                                    (ptu32_t)&(PTT_device_tag[dbx_num]),0);

                buf[0] = '\0';
                strcat(buf,PTT_device_tag[dbx_num].name);
                strcat(buf,":\\");
                root_file = fopen(buf,"r");
                pchar = &root_file->name[strlen(root_file->name)+1];
                *pchar = '\0';
                strcat(pchar,PTT_device_tag[dbx_num].name);
                dbx_num++;
            }
            if(dbx_num >=3)
                break;
        }while (FindNextFile(hWnd, &ffd) != 0);

        FindClose(hWnd);
    }

    return 1;
}

