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
// 这份授权条款，在使用者符合以下二条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、此三条件表列，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、此三条件表列，以及下述
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
#ifndef _FILE_H_
#define _FILE_H_
#include "stdint.h"
#include "stddef.h"
#include "stdio.h"
#include "errno.h"
#include "rsc.h"
#include "ring.h"
#ifdef __cplusplus
extern "C" {
#endif


#define F_OK        0   // does file exist
#define X_OK        1   // is it executable by caller
#define W_OK        2   // is it writable by caller
#define R_OK        4   // is it readable by caller
#define S_IREAD     0x0000400 // read permission, owner
#define S_IWRITE    0x0000200 // write permission, owner
#define S_IEXEC     0x0000100 // execute/search permission, owner

//这么长的单个文件名对嵌入式系统并无多大意义，为兼容大多数长文件名系统。
#define     CN_FILE_NAME_LIMIT    255
#define     CN_PTT_NAME_LIMIT     255      //分区（相当于盘符）名字长度
//我也不想限定文件系统的路径深度，每办法，无限深的路径会导致无限长的执行时间，
//在嵌入式系统中应该是要避免的，20级也应该够用了吧!
#define     CN_PATH_DEPTH_LIMIT   20

//因设备驱动函数的参数个数有限，定义以下几个结构是用于传递参数
 struct tagRwPara    //传递读写参数
{
    size_t  size;       //读写的每单元尺寸
    size_t nmemb;       //单元数
    struct tagFileRsc  *fp;       //被写入的目标文件。
};
 struct tagSeekPara         //传递seek偏移量的参数
{
    sint64_t offset;    //seek的偏移量
    u8  whence;    //seek的参考位置 SEEK_CUR、SEEK_END、SEEK_SET
};

enum _FILE_OPEN_MODE_                         //seek
{                   //操作 创建 清空 seek     截尾 写位置   读位置  初始 seek填充
    EN_R_RB,      //只读 不创 不清 读位置   无效 无效     seek位置   0   无效
    EN_W_WB,      //只写 创建 清空 写位置   不截 seek位置 无效       0   不填
    EN_A_AB,      //追加 创建 不清 无效     无效 尾部     无效     尾部  无效
    EN_R_RB_PLUS, //读写 不创 不清 读写位置 不截 seek位置 seek位置   0   不填
    EN_W_WB_PLUS, //读写 创建 清空 读写位置 不截 seek位置 seek位置   0   不填
    EN_A_AB_PLUS, //读写 创建 不清 读位置   不截 尾部     seek位置   0   无效
} ;

//文件系统出错代码，本enum常量从cn_fs_error开始依序增1.
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
    EN_FS_PTT_NO_FORMATED               //分区未格式化
};

#define CN_FS_OPEN_SUCCESS  0       //成功打开文件(目录)
#define CN_FS_ITEM_EXIST    1       //文件(目录)存在但不能打开(一般是模式不兼容)
#define CN_FS_ITEM_INEXIST  2       //文件(目录)不存在

struct tagFileRsc;
union file_attrs;
//分区设备的私有数据结构
//删掉了file_sum和folder_sum成员，因为该成员需要扫描整个FDT表才能获得，扫描的
//时间可能会很长且不确定，不符合嵌入式应用。
struct tagPTTDevice
{
    struct tagRscNode PTT_node;      //分区资源结点，由系统维护
    u32    opened_sum;         //打开的文件(目录)总数
    bool_t      formatted;          //分区格式化标志
    char        name[CN_PTT_NAME_LIMIT+1];    //分区名，相当于dos的卷标
    struct tagFileRsc *opened_root;   //打开的根目录项，名字=分区名
    ptu32_t PTT_medium_tag;     //特定分区的特征数据结构指针，由存储模块使用
    struct tagMutexLCB *chip_mutex_tag;   //保护分区的互斥量指针

    //分区功能函数
    //格式化分区，在PTT_device_tag参数中返回格式化后分区的参数，成功则true，
    //否则false
    bool_t (*format)(u32 fmt_para1,u32 fmt_para2,
                            struct tagPTTDevice *PTT_device_tag);

    //分区文件读写函数，这些函数由left_read left_write函数使用
    //写文件，把buf中长度为len的数据写入到file的当前指针处
    u32 (*write)(struct tagFileRsc *fp,u8 *buf,u32 len);
    //读文件，从file当前指针处读取len长度的数据到buf中
    u32 (*read)(struct tagFileRsc *fp,u8 *buf,u32 len);
    //把写缓冲区的数据写入flash
    u32 (*flush)(struct tagFileRsc *fp);
    //查询有多少可读数据，对于实质文件来说，就是(文件长度-当前指针)，但流的标准
    //定义并非一定如此，比如通信端子。
    sint64_t (*query_file_stocks)(struct tagFileRsc *fp);
    //查询目标文件还可以写入多少数据，一般等于分区的剩余空间，但流的标准
    //定义并非一定如此。sum_size,valid_size,free_size三个指针都允许是空指针，
    //若是空指针，则对应的参数不返回。
    sint64_t (*query_file_cubage)(struct tagFileRsc *fp);
    void (*check_PTT)(struct tagPTTDevice *PTT_device_tag,
                     sint64_t *sum_size,sint64_t *valid_size,sint64_t *free_size);
    //以下是目录(文件)操作函数,这些函数由left_ctrl函数调用
    //设置文件长度，短于实际长度将截尾，长于当前长度将追加空间。
    sint64_t (*set_file_size)(struct tagFileRsc *fp,sint64_t new_len);
    //设置文件指针,0=成功，非0=失败，如果新位置超出文件长度且文件可写，将导致延
    //长文件尺寸并分配新的存储空间
    u32 (*seek_file)(struct tagFileRsc *fp,struct tagSeekPara *pos);
    //建立文件(目录)，attr参数将表明建立目录还是文件。
    bool_t (*create_item)(char *name,struct tagFileRsc *parent,
                         struct tagFileRsc *result,union file_attrs attr,
                         enum _FILE_OPEN_MODE_ mode);
    //debug bool_t (*move_file)(struct tagFileRsc *src_fp,struct tagFileRsc *dest_fp);
    //删除一个文件(目录)，只能删除空目录
    bool_t (*remove_item)(struct tagFileRsc *fp);
    //打开文件(目录)，result返回item信息，函数返回结果:
    //CN_FS_OPEN_SUCCESS,成功打开文件(目录)
    //CN_FS_ITEM_EXIST,文件(目录)存在但不能打开(一般是模式不兼容)
    //CN_FS_ITEM_INEXIST,文件(目录)不存在
    u32 (*open_item)(char *name,struct tagFileRsc *parent,
                        struct tagFileRsc *result,enum _FILE_OPEN_MODE_ mode);
    //关闭目录(文件)，true = 成功，false = 失败(因无需分配资源，一般不会失败)
    bool_t (*close_item)(struct tagFileRsc *fp);
    //查找文件(目录)，result非空则返回item信息，但不分配读写，不支持通配符
    //缓冲区，成功找到item返回true，否则返回false
    bool_t (*lookfor_item)(char *name,struct tagFileRsc *parent,
                           struct tagFileRsc *result);
    bool_t (*rename_item)(struct tagFileRsc *fp,char *newname);
    struct tagFileRsc *(*item_traversal_son)(struct tagFileRsc * parent,
                                    struct tagFileRsc * current_file);
    //目录表操作函数
    //查询目录表尺寸
    u32 (*check_fdt_size)(struct tagPTTDevice *PTT_device_tag);
    //读目录表,本函数是为一些专用工具准备的，一般不要用
    void (*read_fdt)(struct tagPTTDevice *PTT_device_tag,u8 *buf);
    //检查文件夹下子目录和文件的数量，不包含子目录下的文件。
    u32 (*check_folder)(struct tagFileRsc *folder);
};
enum _FS_LEFT_CMD_
{
    enum_fs_work_path,    //设置并打开当前工作路径
};
enum _FS_RIGHT_CMD_
{
    enum_fs_add_PTT,  //增加一个分区
    enum_fs_del_PTT,  //删除一个分区
};
enum _PTT_LEFT_CMD_
{
    EN_PTT_FORMAT,        //格式化分区
    EN_CHK_PTT_FORMAT,    //查询分区是否格式化
    EN_PTT_LOOKFOR,       //查找一个项目（文件或目录）是否存在，只返回一个布尔量
    EN_PTT_OPEN,          //打开一个文件，只能是文件
    EN_PTT_REMOVE,        //删除一个文件或空目录，不提供删除一棵树的功能
    EN_PTT_RENAME,        //文件（目录）改名
    EN_PTT_SEEK,          //设置一个打开的文件的读写位置
    EN_PTT_TRUNCATE,      //设置文件长度，可能导致截短或延长文件
    EN_PTT_CLOSE,         //关闭一个文件
    EN_PTT_CREAT,        //创建一个文件,ansi c里并没有对应的creat函数。
    EN_PTT_FLUSH,         //把缓冲区刷到物理设备上去
    EN_PTT_TRAVERSAL_SON, //遍历子目录
    EN_QUERY_FILE_CUBAGE, //查询一个文件还能写入多少数据
};
enum _PTT_RIGHT_CMD_
{
    CN_PTT_CLR_READBUF,     //清空某文件读缓冲区
    CN_PTT_CLR_WRITEBUF,    //清空某文件写缓冲区
    CN_PTT_FLUSH_ONE,       //把某文件缓冲区写入存储设备
    CN_PTT_FLUSH_ALL,       //把该分区下所有打开文件缓冲区写入存储设备
    CN_PTT_SEARCH_MEDIUM_TAG,  //修改文件的媒体标记
    CN_PTT_UPDATE_MEDIUM_TAG,  //修改文件的媒体标记
};

struct tagFileAttrBits
{
    u8 read_only:1;    //只读文件
    u8 hidden:1;       //隐藏文件
    u8 reserve:2;
    u8 folder:1;       //目录
    u8 archive:1;      //档案文件
    u8 close_delete:1; //1=关闭时删除，用于临时文件
    u8 deleted:1;      //文件被删除
};

//r open text file for reading
//    打开文本文件用于读
//w truncate to zero length or create text file for writing
//    重置或建立一个文本文件用于写
//a append; open or create text file for writing at end-of-file
//    打开一个文本文件用于追加
//rb open binary file for reading
//    打开一个二进制文件用于读
//wb truncate to zero length or create binary file for writing
//    重置或建立一个二进制文件用于写
//ab append; open or create binary file for writing at end-of-file
//    打开或建立一个二进制文件用于追加
//r+ open text file for update (reading and writing)
//    打开一个文本文件用于读写
//w+ truncate to zero length or create text file for update
//    重置或建立一个文本文件用于读写
//a+ append; open or create text file for update, writing at end-of-file
//    打开或建立一个文本文件用于读和追加写
//r+b or rb+ open binary file for update (reading and writing)
//    打开一个二进制文件用于读写
//w+b or wb+ truncate to zero length or create binary file for update
//    重置或建立一个二进制文件用于读写
//a+b or ab+ append; open or create binary file for update, writing at end-of-file
//    打开或建立一个二进制文件用于读和追加写
//      与unix一样，djyos的文件系统只支持二进制模式，理由：文件系统只是用来帮助
//      访问文件的助手，不应该插手文件格式，只支持二进制，既简单又可以获得更好
//      的模块化。因此，djyos中mode的含义如下：
//      r，rb   只读方式打开一个文件
//      w，wb   只写方式创建一个文件，若原文件存在则覆盖之
//      a，ab   追加方式打开一个文件，若原文件不存在则创建之
//      r+，r+b，rb+ 可读可写方式打开一个文件
//      w+，w+b，wb+ 可读可写方式创建一个文件，若原文件存在则覆盖之
//      a+，a+b，ab+ 可读写追加方式打开一个文件，若原文件不存在则创建之
//
//  以下为djyfs扩展标志
//dc 创建文件，并在关闭时删除，特别适合临时文件。


union file_attrs
{
    u8 all;
    struct tagFileAttrBits bits;
};

struct tagDjyfsStat
{
    s64 st_atime;           //最后访问时间，暂时无用(2012-08-23)
    s64 st_ctime;           //文件创建时间
    s64 st_mtime;           //最后修改时间
    union file_attrs  st_mode;    //文件模式，bit mask
    s64 st_size;            //文件尺寸
};

struct tagFileRsc
{
    struct tagRscNode file_node;      //打开的文件资源结点，由系统维护
    u32 open_counter;               //文件打开次数计数,本参数由file.c模块维护
    struct tagPTTDevice *home_PTT;          //本文件所属的分区，由file.c维护
    enum _FILE_OPEN_MODE_ open_mode;  //文件打开模式，由file.c维护
    union file_attrs file_attr;     //文件属性，file.c和驱动模块共同维护
    struct tagSemaphoreLCB *file_semp;//保留备用，由驱动维护
    sint32_t CreateTime; //创建时间
    sint32_t ModTime; //最近的修改时间
    ptu32_t file_medium_tag;        //与媒体相关的标记，其含义由特定文件系统
                                    //driver解释，如flash file driver)中，保存
                                    //该文件(目录)的FDT项目号。
    //以下两种缓冲区，均有驱动模块使用，驱动模块也可选择不使用缓冲区，不使用的
    //话，把指针置NULL即可
    struct tagRingBuf read_rw_buf;    //独立读写文件的读缓冲区，或合并读写文件的读
                                    //写缓冲区，合并读写的，无写缓冲区write_buf
    struct tagRingBuf write_buf;      //文件写缓冲区
    struct tagRingBuf *p_read_buf;    //读缓冲区指针
    struct  tagRingBuf *p_write_buf;   //写缓冲区指针
    sint64_t file_size;             //文件尺寸,字节数，含仍在缓冲区的数据，由驱动
                                    //模块维护
    sint64_t read_ptr;              //当前读指针，由驱动维护和使用。
    sint64_t write_ptr;             //当前写指针，由驱动维护和使用。
    u32 eno;           //最近一次出错类型，EN_FS_NO_ERROR表示从来没有出错
    char     name[CN_FILE_NAME_LIMIT+1]; //文件名（目录名）,具体所支持的文件名长
                                    //度是驱动模块决定的，故由驱动模块维护。
};

struct stat
{
    union file_attrs st_mode;       //文件属性
    enum _FILE_OPEN_MODE_ open_mode;  //文件打开模式
    sint32_t CreateTime; //创建时间
    sint32_t ModTime; //最近的修改时间
    sint64_t file_size;             //文件尺寸,字节数，不含仍在缓冲区的数据
};

ptu32_t ModuleInstall_Djyfs(ptu32_t para);
u32 Djyfs_SetWorkPath(char *path);
bool_t Djyfs_GetCwd(char *user_work_path,u32 namebuf_len);
bool_t Djyfs_GetPTT_Name(char *PTT_name,u32 namebuf_len);
ptu32_t Djyfs_AppCtrl(u32 left_cmd,
                                ptu32_t data1,ptu32_t data2);
ptu32_t Djyfs_FsDriverCtrl(u32 right_cmd,
                                ptu32_t data1,ptu32_t data2);
bool_t Djyfs_Fstat(struct tagFileRsc *fp,struct stat *fp_info);
struct tagFileRsc *Djyfs_GetRootFolder(struct tagFileRsc *fp);
bool_t Djyfs_PTT_Stat(const char *fullname,s64 *sum,s64 *valid,s64 *free);
bool_t Djyfs_PTT_StatFp(struct tagFileRsc * fp,s64 *sum,s64 *valid,s64 *free);
u32  Djyfs_GetPathLen(struct tagFileRsc * fp);
bool_t Djyfs_GetPath(struct tagFileRsc * fp,char *buf,u32 maxlen);
u32  Djyfs_GetCwdLen(void);
bool_t Djyfs_GetCwd(char *buf,u32 maxlen);
FILE *Djyfs_FolderTraversalSon(struct tagFileRsc *ancestor_file,
                                            struct tagFileRsc *current_file);
u32 Djyfs_Ftruncate ( FILE *fp, sint64_t offset);
u32 Djyfs_Format(u32 format_para1,u32 format_para2,char *dbx_name);
//bool_t djyfs_move(char *src,char *dest);
size_t Djyfs_Fcubage(FILE *fp);
u32 fseekL(FILE *fp, s64 offset, s32 whence);


#ifdef __cplusplus
}
#endif

#endif // _FILE_H_

