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

//所属模块:设备管理模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述:提供设备管理功能
//其他说明:
//修订历史:
//3. 日起:2014-06-14
//   作者:  罗侍田.
//   新版本号：2.0.0
//   修改说明: 对设备驱动模块做了全面修改，不再提供左右手两套接口。
//2. 日期:2009-03-03
//   作者:  罗侍田.
//   新版本号：1.0.1
//   修改说明: 修正了dev_close_left函数和dev_close_right函数的错误，该bug由
//             网友sniper提交
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"

#include "stdio.h"
#include "stddef.h"
#include "stdlib.h"
#include "errno.h"
#include "string.h"
#include "atomic.h"
#include "object.h"
#include "pool.h"
#include "systime.h"
#include "multiplex.h"
#include "lock.h"
#include "driver.h"
#include "djyos.h"
#include "core_config.h"

static struct  Object *s_ptDeviceRscTree;

static struct DjyDevice *s_ptMenOfDevice;//设备控制块内存池
static struct MemCellPool *s_ptDevicePool;     //设备控制块内存池头指针
static struct MutexLCB s_tDevMutex;       //保护设备资源树的并发访问安全

struct VCommon g_tDevCom =
{
    .Context = NULL,
    .Name = "dev",
};
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: 非NULL -- 成功; NULL -- 失败;
//备注:
//-----------------------------------------------------------------------------
struct Object *__DevObjInit(void)
{
    u16 Size, Type;

    Size = sizeof(g_tDevCom) + strlen(g_tDevCom.Name);
    Type = RSC_RSCNODE;

    return (OBJ_AddChild(__RootObj(), &(g_tDevCom.Obj), Size, Type, g_tDevCom.Name));
}

//----初始化设备驱动---------------------------------------------------------
//功能：1.在资源管理链中增加名为"dev"的根结点.
//      2.初始化设备控制块内存池
//参数：无
//返回：1
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_Driver(ptu32_t para)
{
    s_ptMenOfDevice = M_Malloc(gc_u32CfgDeviceLimit * sizeof(struct DjyDevice),0x1);
    if(s_ptMenOfDevice == NULL)
        return 0;

    s_ptDeviceRscTree = __DevObjInit();

    //初始化设备控制块内存池
    s_ptDevicePool = Mb_CreatePool((void*)s_ptMenOfDevice,
                                    gc_u32CfgDeviceLimit,
                                    sizeof(struct DjyDevice ),
                                    0,0,
                                    "设备控制块池");
    Lock_MutexCreate_s(&s_tDevMutex,"device tree");
    return 1;
}

////----获取设备指针-------------------------------------------------------------
////功能: 通过设备handle逆向查找设备的设备指针。
////参数: handle,设备句柄
////返回: 设备指针
////-----------------------------------------------------------------------------
//struct DjyDevice *__dev_Hdl2Dev(struct DjyDevice * Handle)
//{
//    return (struct DjyDevice *)Handle;
//};
//
////----获取设备句柄-------------------------------------------------------------
////功能: 通过设备指针查找设备的设备handle。
////参数: Dev,设备指针
////返回: 设备handle
////-----------------------------------------------------------------------------
//struct DjyDevice * __dev_Dev2Hdl(struct DjyDevice * Dev)
//{
//    return (struct DjyDevice *)Dev;
//};

//----获取设备指针-------------------------------------------------------------
//功能: 通过设备别名查找设备的设备指针。
//参数: DevAlias,设备别名
//返回: 设备指针
//-----------------------------------------------------------------------------
struct DjyDevice *__dev_Alias2Dev(u32 DevAlias)
{
    return (struct DjyDevice *)(DevAlias-1);
};

//----获取设备别名-------------------------------------------------------------
//功能: 通过设备指针查找设备的设备别名
//参数: Dev,设备指针
//返回: 设备handle
//-----------------------------------------------------------------------------
u32 __dev_Dev2Alias(struct DjyDevice * Dev)
{
    return (u32)Dev+1;
};
//
////----获取设备句柄-------------------------------------------------------------
////功能: 通过设备别名查找设备的设备句柄。
////参数: DevAlias,设备别名
////返回: 设备句柄
////-----------------------------------------------------------------------------
//struct DjyDevice * __dev_Alias2Hdl(u32 DevAlias)
//{
//    return (struct DjyDevice *)(DevAlias-1);
//};
//
////----获取设备别名-------------------------------------------------------------
////功能: 通过设备句柄查找设备的设备别名
////参数: Handle,设备句柄
////返回: 设备handle
////-----------------------------------------------------------------------------
//u32 __dev_Hdl2Alias(struct DjyDevice * Handle)
//{
//    return (u32)Handle+1;
//};

//----添加设备-----------------------------------------------------------------
//功能: 新设备加入到兄弟设备的尾部,
//参数: ParentDevice,待添加设备的父设备，NULL表示设备树的根结点为父设备结点
//      name，新设备的名字，所指向的字符串内存区不能是局部变量，也不能是空
//      dReadMutex，读设备互斥量指针
//      dWriteMutex，写设备互斥量指针
//      WriteFunc，写函数指针
//      ReadFunc，读函数指针
//      Ctrl，控制函数指针
//      MultiplexAdd，支持多路复用的函数指针
//      tag,相应设备的驱动给定的一个数，常指向该设备特征数据结构
//返回:新加入的设备的资源结点指针.资源不足则返回NULL
//本函数由设备driver作者调用，不建议应用程序使用，故不使用djy_handle_t
//而是用struct  DjyDevice类型参数
//-----------------------------------------------------------------------------
struct DjyDevice * Driver_DeviceCreate(  struct DjyDevice *ParentDevice,
                                            const char              *name,
                                            struct MutexLCB      *dReadMutex,
                                            struct MutexLCB      *dWriteMutex,
                                            fntDevWrite            WriteFunc ,
                                            fntDevRead             ReadFunc,
                                            fntDevCtrl             Ctrl ,
                                            fntDevMultiplexAdd     MultiplexAdd,
                                            ptu32_t                 tag)
{
    struct DjyDevice *new_device,*result;
    if(name == NULL)
        return NULL;        //设备不能没有名字
    if(strchr(name,'\\'))   //名字中不能包含字符 \.
        return NULL;
    if(ParentDevice == NULL)
        ParentDevice = (struct DjyDevice *)s_ptDeviceRscTree;
    Lock_MutexPend(&s_tDevMutex,CN_TIMEOUT_FOREVER);
    if(OBJ_SearchChild(&ParentDevice->Node,name) != NULL)
    {
        Djy_SaveLastError(EN_DRV_HOMONYMY);
        printf("设备重名\n\r");
        result = NULL;
    }else
    {
        //分配设备控制块给新设备
        new_device = Mb_Malloc(s_ptDevicePool,0);
        if(new_device != NULL)
        {
            OBJ_AddChild(&ParentDevice->Node,&new_device->Node,
                                    sizeof(struct DjyDevice),RSC_DEVICE,name);
            new_device->MutexFlag = 0;
            if(dReadMutex != NULL)
            {
                new_device->dReadMutex = dReadMutex;
            }else
            {
                new_device->dReadMutex = Lock_MutexCreate(name);
                new_device->MutexFlag |= DEV_READ_MUTEX_SYS;
            }
            if(dWriteMutex != NULL)
            {
                new_device->dWriteMutex = dWriteMutex;
            }else
            {
                new_device->dWriteMutex = Lock_MutexCreate(name);
                new_device->MutexFlag |= DEV_WRITE_MUTEX_SYS;
            }
            if( (new_device->dReadMutex != NULL)
                    && (new_device->dWriteMutex != NULL))
            {
                if(WriteFunc != NULL)
                    new_device->dWrite = WriteFunc;
                else
                    new_device->dWrite =(fntDevWrite)NULL_func;
                if(ReadFunc != NULL)
                    new_device->dRead = ReadFunc;
                else
                    new_device->dRead = (fntDevRead)NULL_func;
                if(Ctrl != NULL)
                    new_device->dCtrl = Ctrl;
                else
                    new_device->dCtrl = (fntDevCtrl)NULL_func;
                if(MultiplexAdd != NULL)
                    new_device->dMultiplexAdd = MultiplexAdd;
                else
                    new_device->dMultiplexAdd = (fntDevMultiplexAdd)NULL;
                new_device->PrivateTag = tag;
                result = new_device;
            }
            else
            {
                if(new_device->MutexFlag & DEV_READ_MUTEX_SYS)
                    Lock_MutexDelete(new_device->dReadMutex);
                if(new_device->MutexFlag & DEV_WRITE_MUTEX_SYS)
                    Lock_MutexDelete(new_device->dWriteMutex);
                OBJ_Del(&new_device->Node);
                Mb_Free(s_ptDevicePool,new_device);
                Djy_SaveLastError(EN_MEM_TRIED);
                printf("内存不足\n\r");
                result = NULL;
            }
        }else
        {
            Djy_SaveLastError(EN_MEM_TRIED);
            printf("内存不足\n\r");
            result = NULL;
        }
    }
    Lock_MutexPost(&s_tDevMutex);
    return result;
}

//----锁定设备-----------------------------------------------------------------
//功能: 锁定设备，使该设备不能被删除，但不影响对设备的其他操作。锁定设备是
//      Driver_OpenDeviceAlias函数安全运行的保证，可以确保Driver_OpenDeviceAlias前，
//      设备不会被卸载
//参数: DevAlias，目标设备别名，由Driver_FindDevice返回
//返回: 锁定状态，true=锁定，false=未锁定
//-----------------------------------------------------------------------------
bool_t Driver_LockDevice(u32 DevAlias)
{
    struct DjyDevice *Dev;
    if(DevAlias == 0)
        return false;
    Dev = __dev_Alias2Dev(DevAlias);
    atom_uadd32(&(Dev->delete_lock),1);
    return (Dev->delete_lock > 0);
}

//----解除锁定设备-------------------------------------------------------------
//功能: Driver_LockDevice函数的逆操作
//参数: DevAlias，目标设备别名，由Driver_FindDevice返回
//返回: 锁定状态，false=锁定，true=未锁定
//-----------------------------------------------------------------------------
bool_t Driver_UnLockDevice(u32 DevAlias)
{
    struct DjyDevice *Dev;
    if(DevAlias == 0)
        return false;
    Dev = __dev_Alias2Dev(DevAlias);
    atom_usub32(&(Dev->delete_lock),1);
    return (Dev->delete_lock == 0);
}

//----卸载设备------------------------------------------------------------------
//功能: 把设备从设备链表中删除,并释放其占用的内存.该设备需符合以下条件:
//      1.该设备没有子设备.
//      2.正在使用该设备的用户数为0.
//参数: handle,待释放的设备
//返回: 成功释放返回true,否则返回false
//与对称函数dev_add_device一样，本函数由设备driver作者调用，不建议应用程序
//使用，故不使用struct  dev_handle而是用struct  DjyDevice类型参数
//-----------------------------------------------------------------------------
bool_t Driver_DeleteDevice(struct DjyDevice * device)
{
    struct  Object *node;
    bool_t en_del = true;
    if(device == NULL)
        return false;
    if(device->delete_lock > 0)
        return false;
    Lock_MutexPend(&s_tDevMutex,CN_TIMEOUT_FOREVER);
    if( Lock_MutexQuery(device->dReadMutex)
            || Lock_MutexQuery(device->dWriteMutex))     //检查设备是否空闲
    {
        node = &device->Node;
        //如果互斥量是driver模块分配的,删除之
        if(device->MutexFlag & DEV_READ_MUTEX_SYS)
            Lock_MutexDelete(device->dReadMutex);
        if(device->MutexFlag & DEV_WRITE_MUTEX_SYS)
            Lock_MutexDelete(device->dWriteMutex);
        if(OBJ_Del(node) != NULL)
        {
            Mb_Free(s_ptDevicePool,device);
        }
        else
        {
            en_del = false;
        }
    }
    else
    {
        en_del = false;
    }

    Lock_MutexPost(&s_tDevMutex);
    return en_del;
}


//----查找设备-----------------------------------------------------------------
//功能: 查询设备，若存在返回设备的别名，但并不打开设备，不存在则返回0
//参数: name，设备名字，包含路径名，但不必包含'dev\'这4个字符
//返回: 设备存在则返回设备别名，不存在则返回0
//-----------------------------------------------------------------------------
u32 Driver_FindDevice(const char * name)
{
    struct DjyDevice     *Dev;
    if( ! Lock_MutexPend(&s_tDevMutex,0))    //这是保护设备树的互斥量
        return 0;
    //在设备树中搜索设备
    Dev = (struct DjyDevice *)OBJ_Search(s_ptDeviceRscTree,name);
    Lock_MutexPost(&s_tDevMutex);
    if(Dev != NULL)
        return __dev_Dev2Alias(Dev);
    else
        return 0;
}

//----查找后代设备-------------------------------------------------------------
//功能: 某设备的后代设备中，查询名字为scion_name的设备是否存在，存在则返回其设
//      备别名，但并不打开，不存在则返回0.
//参数: ancestor，被查询设备的祖先设备。
//      scion_name,设备名字符串,包含路径名,
//返回: 设备存在则返回设备别名，不存在则返回0
//-----------------------------------------------------------------------------
u32 Driver_FindScionDevice(struct DjyDevice * ancestor,const char * scion_name)
{
    struct DjyDevice *Dev_scion;
    if(ancestor == NULL)
        return 0;
    if( ! Lock_MutexPend(&s_tDevMutex,0))    //这是保护设备树的互斥量
        return 0;
    //在设备树中搜索设备
    Dev_scion = (struct DjyDevice*)OBJ_Search(&(ancestor->Node),
                                                scion_name);
    Lock_MutexPost(&s_tDevMutex);
    if(Dev_scion != NULL)
        return __dev_Dev2Alias(Dev_scion);
    else
        return 0;
}

//----打开设备-----------------------------------------------------------------
//功能: 根据设备名打开设备，搜索整个设备资源树，找到名称与name匹配的资源结点，
//      返回该句柄指针。多线程互斥方式如下:
//      A线程以D_RDWR模式打开,------其他线程不能再打开
//      A线程以D_RDONLY模式打开,----任意线程(包含A)还可以用D_WRONLY模式打开
//      D_WRONLY同上
//参数: name,设备名字符串,包含路径名，但不必包含'dev\'这4个字符
//      flags,设备打开模式,D_RDONLY,D_WRONLY,D_RDWR中的一个
//      timeout，超时设置,单位是微秒，CN_TIMEOUT_FOREVER=无限等待，0则立即按
//          超时返回。非0值将被向上调整为CN_CFG_TICK_US的整数倍
//返回: 成功打开设备(含经过等待后打开)返回设备句柄,否则返回NULL.
//-----------------------------------------------------------------------------
struct DjyDevice * Driver_OpenDevice(const char *name,u32 flags,u32 timeout)
{
    struct DjyDevice     *Dev;
    if( ! Lock_MutexPend(&s_tDevMutex,timeout))    //这是保护设备树的互斥量
        return NULL;
    //在设备树中搜索设备
    Dev = (struct DjyDevice *)OBJ_Search(s_ptDeviceRscTree,name);
    if(Dev != NULL)     //如果没有找到name设备,返回空
    {
        if(flags == O_RDONLY)
        {
            if(Lock_MutexPend(Dev->dReadMutex,timeout)==false)//获取互斥量,这是保护设备的
                Dev = NULL;
        }
        else if(flags == O_WRONLY)
        {
            if(Lock_MutexPend(Dev->dWriteMutex,timeout)==false)//获取互斥量,这是保护设备的
                Dev = NULL;
        }
        else if (flags == O_RDWR)
        {
            if(Lock_MutexPend(Dev->dReadMutex,timeout)!=false)//获取互斥量,这是保护设备的
            {
                if(Lock_MutexPend(Dev->dWriteMutex,timeout)==false)//获取互斥量,这是保护设备的
                {
                    Lock_MutexPost(Dev->dReadMutex);
                    Dev = NULL;
                }
            }
        }
    }
    Lock_MutexPost(&s_tDevMutex);
    return Dev;
}

//----打开后代设备-------------------------------------------------------------
//功能: 打开后代设备，搜索ancestor设备的整个子设备树，找到名称与scion_name匹配
//      的资源结点,返回设备句柄。多线程互斥方式如下:
//      A线程以D_RDWR模式打开,------其他线程不能再打开
//      A线程以D_RDONLY模式打开,----任意线程(包含A)还可以用D_WRONLY模式打开
//      D_WRONLY同上
//参数: ancestor，被打开设备的祖先设备。
//      scion_name,设备名字符串,包含路径名,
//      flags,设备打开模式,D_RDONLY,D_WRONLY,D_RDWR中的一个
//      timeout，超时设置,单位是微秒，CN_TIMEOUT_FOREVER=无限等待，0则立即按
//          超时返回。非0值将被向上调整为CN_CFG_TICK_US的整数倍
//返回: 成功打开设备(含经过阻塞后打开)返回设备句柄,否则返回NULL.
//-----------------------------------------------------------------------------
struct DjyDevice * Driver_OpenScionDevice(struct DjyDevice * ancestor,
                                const char *scion_name,u32 flags, u32 timeout)
{
    struct DjyDevice     *Dev;
    if(ancestor == NULL)
        return NULL;
    if( ! Lock_MutexPend(&s_tDevMutex,timeout))    //这是保护设备树的互斥量
        return NULL;
    //在设备树中搜索设备
    Dev = (struct DjyDevice*)OBJ_Search(&(ancestor->Node),
                                                scion_name);
    if(Dev != NULL)     //如果没有找到name设备,返回空
    {
        if(flags == O_RDONLY)
        {
            if(Lock_MutexPend(Dev->dReadMutex,timeout)==false)//获取互斥量,这是保护设备的
                Dev = NULL;
        }
        else if(flags == D_WRONLY)
        {
            if(Lock_MutexPend(Dev->dWriteMutex,timeout)==false)//获取互斥量,这是保护设备的
                Dev = NULL;
        }
        else if (flags == O_RDWR)
        {
            if(Lock_MutexPend(Dev->dReadMutex,timeout)!=false)//获取互斥量,这是保护设备的
            {
                if(Lock_MutexPend(Dev->dWriteMutex,timeout)==false)//获取互斥量,这是保护设备的
                {
                    Lock_MutexPost(Dev->dReadMutex);
                    Dev = NULL;
                }
            }
        }
    }

    Lock_MutexPost(&s_tDevMutex);
    return Dev;
}

//----快速打开设备-------------------------------------------------------------
//功能: 在设备别名已知的情况下，Driver_OpenDeviceAlias函数用字符串名遍历设备树
//      查找设备，执行速度很快。本函数与Driver_FindDevice函数Driver_CloseDevice
//      函数配合使用，可实现快速打开设备，及时关闭，不长期占据设备。
//      多线程互斥方式如下:
//      A线程以D_RDWR模式打开,------其他线程不能再打开
//      A线程以D_RDONLY模式打开,----任意线程(包含A)还可以用D_WRONLY模式打开
//      D_WRONLY同上
//参数: DevAlias,待打开的设备别名
//      flags,设备打开模式,D_RDONLY,D_WRONLY,D_RDWR中的一个
//      timeout，超时设置,单位是微秒，CN_TIMEOUT_FOREVER=无限等待，0则立即按
//          超时返回。非0值将被向上调整为CN_CFG_TICK_US的整数倍
//返回: 成功打开设备则返回句柄,否则返回NULL.
//-----------------------------------------------------------------------------
struct DjyDevice * Driver_OpenDeviceAlias(u32 DevAlias,u32 flags,u32 timeout)
{
    struct DjyDevice     *Dev;
    if (DevAlias == 0)
        return NULL;
    Dev = __dev_Alias2Dev(DevAlias);
    if(Dev != NULL)     //如果没有找到name设备,返回空
    {
        if(flags == O_RDONLY)
        {
            if(Lock_MutexPend(Dev->dReadMutex,timeout)==false)//获取互斥量,这是保护设备的
                Dev = NULL;
        }
        else if(flags == O_WRONLY)
        {
            if(Lock_MutexPend(Dev->dWriteMutex,timeout)==false)//获取互斥量,这是保护设备的
                Dev = NULL;
        }
        else if (flags == O_RDWR)
        {
            if(Lock_MutexPend(Dev->dReadMutex,timeout)!=false)//获取互斥量,这是保护设备的
            {
                if(Lock_MutexPend(Dev->dWriteMutex,timeout)==false)//获取互斥量,这是保护设备的
                {
                    Lock_MutexPost(Dev->dReadMutex);
                    Dev = NULL;
                }
            }
            else
            {
            	Dev = NULL;		
            }
        }
    }
    return Dev;
}

//----关闭设备-----------------------------------------------------------------
//功能: 关闭设备，由于设备受互斥量保护，故只有设备拥有者(即打开设备的事件)才能关
//      闭设备，这是由互斥量的特点决定的。
//      设备打开和关闭过程如下:
//
//      1.A线程打开,D_RDWR模式
//      2.A线程关闭,----全部关闭
//
//      1.A线程打开,D_WRONLY模式
//      2.A线程打开,D_RDONLY模式
//      3.A线程关闭,----全部关闭
//
//      1.A线程打开,D_WRONLY模式
//      2.B线程打开,D_RDONLY模式
//      3.A线程关闭,----只关闭了"写"模式,此时C线程可用写模式打开设备,
//      4.B线程关闭,----继续关闭了"读"模式,此时C线程可用读写模式打开设备,
//
//参数: Dev,被关闭的设备指针
//返回: true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t Driver_CloseDevice(struct DjyDevice * Dev)
{
    if (Dev == NULL)
        return false;
    if(Lock_MutexGetOwner(Dev->dReadMutex) == Djy_MyEventId())
    {
        Lock_MutexPost(Dev->dReadMutex );
    }
    if(Lock_MutexGetOwner(Dev->dWriteMutex) == Djy_MyEventId())
    {
        Lock_MutexPost(Dev->dWriteMutex );
    }

    return true;
}

//----读设备函数---------------------------------------------------------------
//功能: 从设备读取数据.
//参数: Dev,设备指针
//      buf,用于接收数据的缓冲区，其容量必须不小于len。buf的容量无法检查。
//      len,读取的数据量，须不大于buf的容量
//      offset,读取位置在设备中的偏移，对于流设备(例如串口)来说，通常是0.
//      timeout，超时设置,单位是微秒，CN_TIMEOUT_FOREVER=无限等待，0则立即按
//          超时返回。非0值将被向上调整为CN_CFG_TICK_US的整数倍
//      以上参数，driver模型只是如实传送给具体设备，设备开发者也可以不遵守上述
//      规定。
//返回: -1表示参数检查出错，其他值为实际读取的数据长度.
//-----------------------------------------------------------------------------
u32 Driver_ReadDevice(struct DjyDevice * Dev,u8 *buf,u32 len,u32 offset,u32 timeout)
{
    if (Dev == NULL)
        return -1;
    else
    {
        //只有设备的持有人才能操作设备
//        if(Lock_MutexGetOwner(Dev->dReadMutex) == Djy_MyEventId() )
            return (Dev->dRead(Dev->PrivateTag,buf,len,offset,timeout));
//        else
//            return -1;
    }
}

//----写设备函数---------------------------------------------------------------
//功能: 把数据写入设备.
//参数: Dev,设备指针
//      buf,待发送数据的缓冲区，其容量必须不小于len。buf的容量无法检查。
//      len,发送的数据量，须不大于buf的容量
//      offset,写入位置在设备中的偏移，对于流设备(例如串口)来说，通常是0.
//      timeout，超时设置,单位是微秒，CN_TIMEOUT_FOREVER=无限等待，0则立即按
//          超时返回。非0值将被向上调整为CN_CFG_TICK_US的整数倍
//      BlockOption，阻塞选项，取值为CN_BLOCK_BUFFER或CN_BLOCK_COMPLETE。含义见
//          此二常量定义处。
//      以上参数，driver模型只是如实传送给具体设备，设备开发者也可以不遵守上述
//      规定。
//返回: -1表示参数检查出错，其他值为实际写的数据长度.
//-----------------------------------------------------------------------------
u32 Driver_WriteDevice(struct DjyDevice * Dev,u8 *buf,
                  u32 len,u32 offset,bool_t BlockOption,u32 timeout)
{
    if (Dev == NULL)
        return -1;
    else
    {
//        if(Lock_MutexGetOwner(Dev->dWriteMutex) == Djy_MyEventId())
            return (Dev->dWrite(Dev->PrivateTag,buf,len,offset,
                                                BlockOption,timeout));
//        else
//            return -1;
    }
}

//----控制设备函数-------------------------------------------------------------
//功能: 用于控制设备.
//参数: left_handle,设备句柄
//      cmd,命令号，driver.h中预定义了一些命令，以及用户命令的使用规则，可参阅
//          常量CN_DRV_CTRL_START定义附近代码。
//      data1，data2,控制命令所需要的参数，由设备开发者定义
//返回: -1表示参数检查出错，其他值由设备开发者定义
//-----------------------------------------------------------------------------
u32 Driver_CtrlDevice(struct DjyDevice * Dev,u32 cmd,ptu32_t data1,ptu32_t data2)
{
    if (Dev == NULL)
        return -1;
    else
    {
//        if( (Lock_MutexGetOwner(Dev->dReadMutex) == Djy_MyEventId())
//           || (Lock_MutexGetOwner(Dev->dWriteMutex) == Djy_MyEventId()) )
            return (Dev->dCtrl(Dev->PrivateTag,cmd,data1,data2));
//        else
//            return -1;
    }
}

//----Multiplex控制函数--------------------------------------------------------
//功能: 设置设备的Multiplex参数，设置读写触发阈值，调用devMultiplexAdd前建议先调
//      用本函数，否则读写水平都将是设备driver设计时的默认值，具体默认值是多少，
//      请参考相应设备driver的说明。

//      设备是否支持设置阈值，也是具体设备driver的事，若不支持，将返回
//      CN_MULTIPLEX_ERROR，

//      如果dMultiplexAdd为空，表明该设备不支持Multiplex功能，本函数也将返回
//      CN_MULTIPLEX_INVALID
//参数: DevAlias,被操作的设备别名,由Driver_FindDevice函数返回。
//      ReadLevel,读触发水平，==NULL表示不设置ReadLevel，*ReadLevel=0表示只查询
//      WriteLevel,写触发水平，==NULL表示不设置WriteLevel，*WriteLevel=0表示只查询
//返回: CN_MULTIPLEX_SUCCESS = 成功设置，
//      CN_MULTIPLEX_INVALID = 设备不支持Multiplex功能。
//      CN_MULTIPLEX_ERROR   = 设备支持Multiplex，但拒绝了本次调用
//-----------------------------------------------------------------------------
u32 Driver_MultiplexCtrl(u32 DevAlias,u32 *ReadLevel,u32 *WriteLevel)
{
    struct DjyDevice *Dev;
    if (DevAlias == 0)
        return CN_MULTIPLEX_INVALID;
    else
    {
        Dev = __dev_Alias2Dev(DevAlias);
        if(Dev->dMultiplexAdd != NULL)
        {
            if(Dev->dCtrl(Dev->PrivateTag,CN_DRV_CTRL_SET_LEVEL,(ptu32_t)ReadLevel,
                               (ptu32_t)WriteLevel) == CN_LIMIT_UINT32)
                return CN_MULTIPLEX_ERROR;
            else
                return CN_MULTIPLEX_SUCCESS;
        }
        else
            return CN_MULTIPLEX_INVALID;
    }
}

//----Multiplex设置函数--------------------------------------------------------
//功能: 把一组设备加入到一个多路复用集中，使用设备Index。
//
//参数: MultiplexSets，目标多路复用集指针
//      DevAliases,一组被操作的设备Index数组地址,由Driver_FindDevice函数返回。
//      num,DevAliases数组中设备数量
//      SensingBit,感兴趣的操作，见Multiplex.h中CN_MULTIPLEX_SENSINGBIT_READ常量
//          及相关常量定义.特别注意，如果是0，则从MultiplexSets中删除对象(driver
//          将调用Multiplex_Del函数)。共31个感兴趣的bit，bit31用于定义对象触发类
//          型，取值 CN_MULTIPLEX_SENSINGBIT_AND 或 CN_MULTIPLEX_SENSINGBIT_OR
//返回: 操作成功的设备数量
//-----------------------------------------------------------------------------
u32 Driver_MultiplexAdd(struct MultiplexSetsCB *MultiplexSets,
                        u32 *DevAliases,u32 num,u32 SensingBit)
{
    struct DjyDevice *Dev;
    u32 n,result=0;
    if (DevAliases == NULL)
        return 0;
    else
    {
        for(n = 0; n < num; n++)
        {
            Dev = __dev_Alias2Dev(DevAliases[n]);
            if(Dev->dMultiplexAdd != NULL)
            {
                if( (Dev->dMultiplexAdd(Dev->PrivateTag,MultiplexSets,
                                        DevAliases[n],SensingBit)) )
                    result++;
            }
        }
        return result;
    }
}

//----Multiplex清除函数-----------------------------------------------------------
//功能: 把一组设备从一个多路复用集中删除，使用设备Index。
//参数: MultiplexSets，目标多路复用集指针
//      DevAliases,一组被操作的设备Index数组地址,由Driver_FindDevice函数返回。
//      num,DevAliases数组中设备数量
//返回: 无
//-----------------------------------------------------------------------------
void Driver_MultiplexDel(struct MultiplexSetsCB *MultiplexSets,
                         u32 *DevAliases,u32 num)
{
    Driver_MultiplexAdd(MultiplexSets,DevAliases,num,0);
}

