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
//所属模块: 资源管理
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 创建、添加、移动、删除、搜索资源结点的服务
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __rsc_h__
#define __rsc_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define CN_RSC_NAME_LIMIT   255

typedef struct Object
{
    struct Object *Next,*Previous,*Parent,*Child;
    u16  Size;      //包含node的数据结构的尺寸，用于调试
    u16  Type;      //对象类型
    u32  Inuse;     //使用计数
    char *Name;     //资源名,当用于文件系统为文件名或目录名,用于设备是设备名
                    //用于gui则是窗口名.
}TagObject;

//
// 文件系统使用
//
struct VCommon
{
    struct Object Obj;
    void *Context; // 指向struct FileContext或struct VMount
    char Name[];
};

//文件系统的类型
#define OB_MOUNT                (0x0800) // 安装类型
#define OB_FILE                 (0x8000) // 文件类型
#define OB_REG                  (0x9000) // 文件
#define OB_DIR                  (0xA000) // 目录
#define OB_LINK                 (0xC000) // 链接

//定义资源类型
#define RSC_RSCNODE             0       //纯节点
#define RSC_RSCV_TYPE           1       //保留类型
#define RSC_DEVICE              2       //设备
#define RSC_SEMP                3       //信号量
#define RSC_MUTEX               4       //互斥量
#define RSC_MSGQ                5       //消息队列
#define RSC_MEMPOOL             6       //内存池
#define RSC_SOFT_TIMER          7       //软定时器
#define RSC_SOCKET              8       //socket
#define RSC_NET_CARD            9        //网卡
#define RSC_SPIBUS              10       //spi总线
#define RSC_SPI_DEVICE          11       //spi器件
#define RSC_IICBUS              12       //i2c总线
#define RSC_IIC_DEVICE          13       //i2c器件
#define RSC_STDIN_OUT           14       //标准输入或输出设备
#define RSC_SHELL_CMD           15       //shell 命令
#define RSC_DISPLAY             16       //显示器
#define RSC_GKWIN               17       //内核窗口(显示)
#define RSC_FILE                18       //文件或文件夹
#define RSC_FILE_SYS            19       //文件系统(指整个文件系统)
#define RSC_PTT                 20       //文件系统分区
#define RSC_FFS_FLASH_CHIP      21       //用于flash文件系统的flash芯片
#define RSC_EFS_NOR_CHIP        22       //用于easy nor文件系统的flash芯片
#define RSC_CHARSET             23       //字符集
#define RSC_FONT                24       //字体


#define RSCTYPE_USER_BASE       0x80
#define RSCTYPE_USER            (RSCTYPE_USER_BASE + 0)

ptu32_t ModuleInstall_OBJ(ptu32_t Para);
//ptu32_t Rsc2_ModuleInit(ptu32_t para);
void OBJ_Clean(struct Object *Obj);
struct Object *OBJ_AddTree(struct Object *Obj, u16 Size, u16 RscType, const char *Name);
struct Object *OBJ_AddToPrevious(struct Object *Obj,struct Object *NewObj,
                                u16 Size, u16 RscType, const char *Name);
struct Object *OBJ_AddToNext(struct Object *Obj,struct Object *NewObj,
                             u16 Size, u16 RscType, const char *Name);
struct Object *OBJ_AddChild(struct Object *Parent, struct Object *Child,
                            u16 Size, u16 RscType, const char *Name);
struct Object *OBJ_AddChildHead(struct Object *Parent, struct Object *Child,
                                u16 Size, u16 RscType, const char *Name);
bool_t OBJ_Displace(struct Object *Old, struct Object *New);
struct Object *OBJ_DelBranch(struct Object *Branch);
struct Object *OBJ_Del(struct  Object *Obj);
bool_t OBJ_MoveToTree(struct Object *Parent,struct Object *Node);
bool_t OBJ_MoveToLast(struct Object *Obj);
bool_t OBJ_MoveToHead(struct Object *Obj);
bool_t OBJ_MoveToNext(struct Object *Obj, struct Object *NewNext);
bool_t OBJ_MoveToPrevious(struct Object *Obj, struct Object *NewPre);
bool_t OBJ_RoundPrevious(struct Object *Parent);
bool_t OBJ_RoundNext(struct Object *Parent);
bool_t OBJ_Rename(struct Object *Obj, const char *NewName);
struct Object *OBJ_GetTree(struct Object *Obj);
struct Object *OBJ_SearchTree(const char *Name);
struct Object *OBJ_SysRoot(void);
char *OBJ_Name(struct Object *Obj);
u16 OBJ_Size(struct Object *Obj);
u16 OBJ_Type(struct Object *Obj);
struct Object *OBJ_Parent(struct Object *Obj);
struct Object *OBJ_Child(struct Object *Obj);
struct Object *OBJ_Previous(struct Object *Obj);
struct Object *OBJ_Next(struct Object *Obj);
struct Object *OBJ_GetHead(struct Object *Obj);
struct Object *OBJ_GetTwig(struct Object *Obj);
u32 OBJ_GetLevel(struct Object *Obj);
struct Object *OBJ_TraveChild(struct Object *Parent, struct Object *Child);
struct Object *OBJ_TraveScion(struct Object *Ancestor, struct Object *Current);
struct Object *OBJ_SearchSibling(struct Object *Brother, const char *Name);
struct Object *OBJ_SearchChild(struct Object *Parent, const char *Name);
struct Object *OBJ_SearchScion(struct Object *Ancestor, const char *Name);
struct Object *OBJ_Search(struct Object *Start, const char *Path);
u32 OBJ_Sequencing(struct Object *Obj);
bool_t OBJ_IsLast(struct Object *Obj);
bool_t OBJ_IsHead(struct Object *Obj);
struct Object *__RootObj(void);
#ifdef __cplusplus
}
#endif

#endif //__rsc_h__

