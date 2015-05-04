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
#define CN_RSC_NAME_LIMIT   255

struct tagRscNode
{
    struct tagRscNode *next,*previous,*parent,*child;
    u16  RscSize;       //包含node的数据结构的尺寸，用于调试
    u8  rscv;
    u8  RscType;        //起源类型
    char *name;         //资源名,当用于文件系统为文件名或目录名,用于设备是设备名
                        //用于gui则是窗口名.
};

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

ptu32_t Rsc1_ModuleInit(ptu32_t para);
ptu32_t Rsc2_ModuleInit(ptu32_t para);
void Rsc_CleanNode(struct tagRscNode *node);
struct  tagRscNode * Rsc_AddTree(struct  tagRscNode *node,u16 size,u8 RscType,char *name);
struct  tagRscNode * Rsc_AddToPrevious(struct  tagRscNode *node,struct  tagRscNode *new_node,
                                u16 size,u8 RscType,char *name);
struct  tagRscNode * Rsc_AddToNext(struct  tagRscNode *node,struct  tagRscNode *new_node,
                                u16 size,u8 RscType,char *name);
struct  tagRscNode * Rsc_AddSon(struct  tagRscNode *parent_node,
                               struct  tagRscNode *new_node,
                               u16 size,u8 RscType,char *name);
struct  tagRscNode * Rsc_AddHeadSon(struct  tagRscNode *parent_node,
                                    struct  tagRscNode *new_node,
                                    u16 size,u8 RscType,char *name);
bool_t Rsc_DisplaceNode(struct tagRscNode *oldnode,struct tagRscNode *newnode);
struct  tagRscNode * Rsc_DelBranch(struct  tagRscNode *branch);
struct  tagRscNode * Rsc_DelNode(struct  tagRscNode *node);
bool_t Rsc_MoveToTree(struct tagRscNode *parent,struct  tagRscNode *node);
bool_t Rsc_MoveToLast(struct  tagRscNode *node);
bool_t Rsc_MoveToHead(struct  tagRscNode *node);
bool_t Rsc_MoveToNext(struct  tagRscNode *elder,struct  tagRscNode *node);
bool_t Rsc_MoveToPrevious(struct  tagRscNode *lesser,struct  tagRscNode *node);
bool_t Rsc_RoundPrevious(struct  tagRscNode *parent);
bool_t Rsc_RoundNext(struct  tagRscNode *parent);
bool_t Rsc_RenameNode(struct tagRscNode *node,char *new_name);
struct  tagRscNode *Rsc_GetTree(struct  tagRscNode *scion_node);
struct  tagRscNode *Rsc_SearchTree(char *tree_name);
struct  tagRscNode *Rsc_GetRoot(void);
char *Rsc_GetName(struct  tagRscNode *node);
u16 Rsc_GetNodeSize(struct  tagRscNode *node);
u8 Rsc_GetNodeType(struct  tagRscNode *node);
struct  tagRscNode *Rsc_GetParent(struct  tagRscNode *son_node);
struct  tagRscNode *Rsc_GetSon(struct  tagRscNode *parent_node);
struct  tagRscNode *Rsc_GetPrevious(struct  tagRscNode *next_node);
struct  tagRscNode *Rsc_GetNext(struct  tagRscNode *previous_node);
struct  tagRscNode *Rsc_GetHead(struct  tagRscNode *rnode);
struct  tagRscNode *Rsc_GetTwig(struct  tagRscNode *parent_node);
u32 Rsc_GetClass(struct  tagRscNode *node);
struct  tagRscNode *Rsc_TraveSon(struct  tagRscNode *parent_node,
                                  struct  tagRscNode *current_son);
struct  tagRscNode *Rsc_TraveScion(struct  tagRscNode *parent_node,
                                 struct  tagRscNode *current_node);
struct  tagRscNode *Rsc_SearchSibling(struct  tagRscNode *layer,char *name);
struct  tagRscNode *Rsc_SearchSon(struct  tagRscNode *parent,char *name);
struct  tagRscNode *Rsc_SearchScion(struct  tagRscNode *parent_node,char *name);
struct  tagRscNode *Rsc_Search(struct  tagRscNode *parent_node,char *path);
u32 Rsc_NodeSequencing(struct tagRscNode *node);
bool_t Rsc_IsLast(struct tagRscNode *node);
bool_t Rsc_IsHead(struct tagRscNode *node);

#ifdef __cplusplus
}
#endif

#endif //__rsc_h__

