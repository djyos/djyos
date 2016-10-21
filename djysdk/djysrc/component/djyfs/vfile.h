//-----------------------------------------------------------------------------
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
#ifndef __VFILE_H__
#define __VFILE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>   //added by zqf
#include <object.h>
//
// 外部定义,来自于fs_module.c
//
extern struct VFilePool *g_ptVFilePool;
extern struct MutexLCB *g_ptVFilePoolMutex;
extern struct MutexLCB *g_ptVFMutex;
extern struct MutexLCB *g_ptFSTypeMutex;
extern struct SysEnv g_tSysRoot, g_tSysPWD;
extern struct VOperations g_tFOps;

//
// 文件系统出错代码，本enum常量从cn_fs_error开始依序增1.
//
enum _FS_ERROR_CODE_
{
    EN_FS_NO_ERROR = CN_FS_NO_ERROR,    //没有错误
    EN_FS_NAME_INVALID_CHAR,            //名字含有非法字符
    EN_FS_NAME_LEN_ERROR,               //名字长度错误
    EN_FS_OBJECT_OPENED,                //对象已经打开
    EN_FS_OBJECT_NONENTITY,             //对象不存在
    EN_FS_OBJECT_READONLY,              //只读对象
    EN_FS_FOLDER_UNBLANK,               //目录非空
    EN_FS_CREAT_PTT_ERROR,              //建立分区错误
    EN_FS_CREAT_ROOT_FOLDER_ERROR,      //建立根目录错误
    EN_FS_FORMAT_PTT_ERROR,             //格式化错误
    EN_FS_REMOVE_ERROR,                 //删除对象错误
    EN_FS_PTT_ERROR,                    //删除分区错误
    EN_FS_RENAME_ERROR,                 //对象改名错误
    EN_FS_PTT_NAME_REPEAT,              //分区重名错误
    EN_FS_PTT_NO_FORMATED,              //分区未格式化
    EN_FS_READFILE_ERROR,               //读文件错误
    EN_FS_READFILE_EOF                  //读文件到达文件结束
};

//
//
//
struct Stat
{
    long long       Size;// 文件大小
    unsigned int    Property;// 文件属性
};

//
// 文件操作模式Mode
//
#define M_TYPE              (u32)(0xF0000000)// MASK
#define M_REG               (u32)(0x10000000)// 文件
#define M_LINK              (u32)(0x20000000)// 链接
#define M_DIR               (u32)(0x40000000)// 目录

        
#define M_READ              (u32)(0x1)// 读
#define M_WRITE             (u32)(0x2)// 写
#define M_OPEN              (u32)(0x10)// 打开(备注:对于c库而言,打开与创建是可以共存的,而io却不能共存。因此M_OPEN位表示文件是可以已存在的)  
#define M_CREATE            (u32)(0x100)// 创建
#define M_TRUNCATE          (u32)(0x200)// 截断（从头开始）
#define M_APPEND            (u32)(0x800)// 追加（从尾开始）
#define M_TEXT              (u32)(0x4000)// 文本格式
#define M_BIN               (u32)(0x8000)// 二进制格式

//
//
//
#define V_SEEK_SET          (0)
#define V_SEEK_CUR          (1)
#define V_SEEK_END          (2)
 
//
//
//
struct PathInfo; 
struct VFile;
struct VOperations;

//
//
//
struct VFile
{
    struct VOperations *VOps;
    void *Private;
};
//
//
//
#define VFILE_NUM           (128) 
#define VFILE_SIZE          (sizeof(struct VFile))
#define VFILE_POOL_SIZE     (VFILE_NUM * VFILE_SIZE) 

struct VFilePool
{
    struct VFile *Space; // VFile池空间
    u16 Used; // 已使用数量
    u16 ID; // VFile池ID
    struct MemCellPool *Management; // VFile池管理结构
    struct VFilePool *Next; // VFile池链表,以NULL表示链表结束
};
 

//
// 路径检索时的上下文
//
struct PathWalk
{
    char *Path; // 文件系统的绝对路径，但不包含根目录
    char *UncoveredPath; // 未缓存的文件路径
    char *Root; // 文件系统的根名
    struct Object *End; // 遍历路径的结束文件
    struct VOperations *VOps; // 返回值，提供给VFile
    void *FileOps; // 子系统的操作方法，例如struct FileOperations
    void *Private; // 返回值，提供给VFile,当前为struct FileContext
    struct VFile **PrivateVHost; // 建立Private与VFile之间的映射关系
};
//
// 目录项
//
struct Dirent{
	char Name[257];
	u32 Property;
};
//
//
//
struct VOperations
{
    s32 (*VOpen)(struct PathWalk *Path, u32 Mode, u8 IsFound);
    s32 (*VClose)(void *Private);
    s32 (*VWrite)(const void *Buf, u32 Size, u32 Nmemb, void *Private);
    s32 (*VRead)(void *Buf, u32 Size, u32 Nmemb, void *Private);
    s32 (*VCntrl)(u32 Command, void *Param0, void *Param1, void *Private);// todo
};
//
//
//
struct FileOperations
{
    struct FileContext *(*FileOpen)(const char *Path, u32 Mode, const char *Root);
    s32 (*FileClose)(struct FileContext *FileCt);
    s32 (*FileWrite)(const void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt);
    s32 (*FileRead)(void *Buf, u32 Size, u32 Nmemb, struct FileContext *FileCt);
    s32 (*FileSeek)(struct FileContext *FileCt, s64 Offset, s32 whence);
    s32 (*FileDelete)(const char *Path, const char *Root);
    s32 (*FileSync)(struct FileContext *FileCt);
    s32 (*FileStat)(struct FileContext *FileCt, const char *Path, struct Stat *Buf, const char *Root);
    s32 (*FileTruncate)(struct FileContext *FileCt, off_t NewSize);//todo: off_t牵涉到C库声明，实际应该是long型，现在与实际有冲突
	s32 (*DirRead)(struct FileContext *FileCt, struct Dirent *Content);
};
//
// 文件的上下文结构
//
#define P_READ              (0x1)
#define P_WRITE             (0x2)
#define P_TYPE              (0xF0)// MASK
#define P_REG               (0x10)// 文件
#define P_DIR               (0x20)// 目录
#define P_LINK              (0x40)// 链接
#define P_DEV               (0X80)// 设备
struct FileContextList
{
    struct FileContextList *Pre;
    struct FileContextList *Next;
};
struct FileContext
{
    struct VCommon *CHost;// 指向缓冲中的VCommon
    struct VFile *FHost;// 指向句柄
    struct FileBuf *Buf;// NULL表示具体文件系统处理
    void *Private; // 不同文件系统上下文
    void *FileOps; // 文件的操作函数集,例如struct FileOperations
    u32 Property;// 文件属性,由具体文件系统管理
    struct FileContextList List; // 为NULL表示头和尾,文件的多次打开的链表
};
//
//
//
#define FS_BUFFERED        	(0x1)
#define BUF_DIRTY       	(0x00000001)
#define BUF_CLEAN       	(0x00000002)
#define BUF_SIZE            (512)
struct FileBuf
{
    u8 *Start, *Current, *End;
    u32 Status;
};

//
//
//
struct SysEnv
{
    struct VOperations *VOps;
    struct Object *Obj;
    char *Path; // 当进入某个文件系统内部时,指向全路径(考虑到第三方可能不支持当前路径等的设置);
    struct FileOpertions *FileOps; // 当进入某个文件系统内部时,指向文件的操作方式(考虑到第三方可能不支持当前路径等的设置);
};


//
//
//
s32 FOpen(struct PathWalk *PathContext, u32 Mode, u8 IsFound);
s32 FClose(void *File);
s32 FWrite(const void *Buf, u32 Size, u32 Nmemb, void *Private);
s32 FRead(void *Buf, u32 Size, u32 Nmemb, void *Private);
//
//
//
void __PathInit(struct PathWalk *PathContext, const char *Path);
struct VFile *AllocVFile(void);
void FreeVFile(struct VFile *File);
struct VFilePool *__ExtendVFilePool(void);
struct VCommon *__CoverPath(const char *Path, struct Object *Obj);
u8 __VPathLookup(struct PathWalk *PathContext, u8 Open);
struct VFilePool *PoolOf(struct VFile *Fp);
struct VFile *__Open(const char *Path, u32 Mode);
struct VFile *__OpenUnlock(const char *Path, u32 Mode);
char *PathCopy(const char *Root, const char *RelPath, char(*Rebuild)(char *Path));
void FreeFileBuf(struct FileBuf *Buf);
s32 AllocFileBuf(struct FileBuf **BufRet, u8 Force);
struct FileContext *AllocContext(void);
void FreeContext(struct FileContext * FileCt);
void __FlushDir(struct VFile *Fp);
struct Object *__FileObj(struct VFile *Fp);
s32 __CacheDir(struct VFile *Fp);
void *__FileOps(struct PathWalk *PathCt);
s32 ReadDir(struct VFile *Dir, struct Dirent *DirentInfo);
bool_t IS_FILE(struct VFile *File);
bool_t IS_DEV(struct VFile *File);
struct DjyDevice *ToDev(struct VFile *Fp); 

#ifdef __cplusplus
}
#endif

#endif /* __VFILE_H__ */
