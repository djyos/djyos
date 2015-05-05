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
//所属模块:泛设备管理模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述:提供泛设备管理功能
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __DRIVER_H__
#define __DRIVER_H__

#include "errno.h"
#include "rsc.h"
#include "multiplex.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CN_DEV_NAME_LIMIT   255     //设备名长度不能超过255字符
typedef struct tagDjyDevice * tagDevHandle;
struct  tagDjyDevice;
struct  tagRscNode;

#define O_RDONLY    1   //只读模式
#define O_WRONLY    2   //只写模式
#define O_RDWR      3   //读写模式

//泛设备模块出错代码，本enum常量从enum_drv_no_error开始依序增1.
enum _DRV_ERROR_CODE_
{
    EN_DRV_NO_ERROR = CN_DRV_NO_ERROR,  //没有错误
    EN_DRV_HANDLE_ERROR,                //句柄错误
    EN_DRV_HOMONYMY,                    //设备重名错误
    EN_DRV_DEV_DEL,                   //删除设备失败
};

//常用设备控制命令
#define CN_DRV_CTRL_START           0   //启动设备，有些能控制电源的设备需要
#define CN_DRV_CTRL_SHUTDOWN        1   //关闭电源,断电前需要特定处理的设备需要
#define CN_DRV_CTRL_SLEEP           2   //设备进入低功耗状态,用于电源管理
#define CN_DRV_CTRL_RESUME          3   //设备恢复,从低功耗状态唤醒
#define CN_DRV_CTRL_CHECK           4   //检查设备状态
#define CN_DRV_CTRL_SET_FUNC        5   //设置有输入/输出/错误时回调函数
#define CN_DRV_CTRL_SET_LEVEL       6   //设置设备事件读和写触发水平

#define CN_DRV_CTRL_USER            0x80   //自定义的设备控制命令，从0x100开始
//用户定义控制命令常数，请从0x80开始，例如:
//#define CN_XXX_CTRL_MYCMD1        (CN_DRV_CTRL_USER+0)
//#define CN_XXX_CTRL_MYCMD2        (CN_DRV_CTRL_USER+1)

//定义dev_Write的完成条件(block_option参数)，达到条件后，函数返回。
#define CN_BLOCK_BUFFER         0       //dev_Write的完成条件是发送到缓冲区
#define CN_BLOCK_COMPLETE       1       //dev_Write的完成条件是传输完成

//返回值:成功写入的字节数，如果设备有缓冲区，则写到缓冲区就算。例如一个串口设备，
//返回值表示函数返回时成功写入到设备缓冲区的数据量，并不确定是否已经从物理串口
//传输出去了。
typedef u32 (*devWriteFunc)(ptu32_t PrivateTag,u8 *buf,
                         u32 len,u32 offset,bool_t BlockOption,u32 timeout);
//返回值:成功读取的字节数
typedef u32 (*devReadFunc) (ptu32_t PrivateTag,u8 *buf,
                                     u32 len,u32 offset,u32 timeout);
//返回值:收到不支持的命令，返回-1，0表示成功执行，其他返回值的含义自定
typedef u32 (*devCtrlFunc) (ptu32_t PrivateTag,u32 cmd,
                                     ptu32_t data1,ptu32_t data2);
//返回值:true=成功执行，false=失败。
//SendsingBit设为0表示从MultiplexSets中删除该设备
typedef bool_t (*devMultiplexAddFunc) (ptu32_t PrivateTag,
                                       struct tagMultiplexSetsCB *MultiplexSets,
                                       u32 DevAlias,
                                       u32 SensingBit);
#define DEV_READ_MUTEX_SYS      1       //bit0,1=读设备的互斥量是系统分配的。
#define DEV_WRITE_MUTEX_SYS     2       //bit1,1=写设备的互斥量是系统分配的。
//注意，不提供dev_open、dev_close两个函数，如果有些设备需要在open时调用dev_open
//完成一些设置工作的，可用dev_Ctrl函数的CN_DRV_CTRL_START命令。同理，也不提供
//dev_close接口
struct tagDjyDevice
{
    struct  tagRscNode Node;
    devWriteFunc  dWrite;
    devReadFunc   dRead;
    devCtrlFunc   dCtrl;
    devMultiplexAddFunc dMultiplexAdd;  //若设备driver不支持多路复用，请置空。
    struct tagMutexLCB *dReadMutex;     //互斥量,控制设备独占式读访问
    struct tagMutexLCB *dWriteMutex;    //互斥量,控制设备独占式写访问
    u32    MutexFlag;                   //标志互斥量是用户提供的,还是系统分配的。
    u32 delete_lock;                    //删除锁，大于 0 表示该设备不能删除
    ptu32_t PrivateTag;                 //本设备特有的数据
};

ptu32_t ModuleInstall_Driver(ptu32_t para);
tagDevHandle Driver_DeviceCreate(  tagDevHandle         ParentDevice,
                                char                    *name,
                                struct tagMutexLCB      *dReadMutex,
                                struct tagMutexLCB      *dWriteMutex,
                                devWriteFunc            WriteFunc ,
                                devReadFunc             ReadFunc,
                                devCtrlFunc             Ctrl ,
                                devMultiplexAddFunc     MultiplexAdd,
                                ptu32_t                 tag);
bool_t Driver_LockDevice(u32 DevAlias);
bool_t Driver_UnLockDevice(u32 DevAlias);
bool_t Driver_DeleteDevice(tagDevHandle handle);
u32 Driver_FindDevice(char * name);
u32 Driver_FindScionDevice(tagDevHandle ancestor,
                                    char * scion_name);
tagDevHandle Driver_OpenDevice(char *name,u32 flags,u32 timeout);
tagDevHandle Driver_OpenScionDevice(tagDevHandle ancestor,
                                 char *scion_name,u32 flags, u32 timeout);
tagDevHandle Driver_OpenDeviceAlias(u32 DevAlias,u32 flags,u32 timeout);
bool_t Driver_CloseDevice(tagDevHandle handle);
u32 Driver_ReadDevice(tagDevHandle handle,u8 *buf,u32 len,u32 offset,u32 timeout);
u32 Driver_WriteDevice(tagDevHandle handle,u8 *buf,
                  u32 len,u32 offset,bool_t BlockOption,u32 timeout);
u32 Driver_CtrlDevice(tagDevHandle handle,u32 cmd,ptu32_t data1,ptu32_t data2);
u32 Driver_MultiplexCtrl(u32 DevAlias,u32 *ReadLevel,u32 *WriteLevel);
u32 Driver_MultiplexAdd(struct tagMultiplexSetsCB *MultiplexSets,
                        u32 *DevAliases,u32 num,u32 SensingBit);
void Driver_MultiplexDel(struct tagMultiplexSetsCB *MultiplexSets,
                         u32 *DevAliases,u32 num);

#ifdef __cplusplus
}
#endif
#endif //__DRIVER_H___
