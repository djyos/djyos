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
//所属模块: 锁模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 管道模块
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2010-10-01
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __PIPE_H__
#define __PIPE_H__

#ifdef __cplusplus
extern "C" {
#endif

//管道控制块
//当读管道时，如果管道中没有足够的数据量，将阻塞在信号量中，写入数据时使管道中的
//数据量超过level，则立即释放信号量。
//写管道时，如果写满了，将阻塞在信号量中，读管道使得管道中的数据量低于level时，
//将释放信号量。
//管道是单线程组件，

//特别注意:管道可能死锁，程序员应该自己注意不要发生死锁。死锁条件为:
//1、请求从管道读的数据量超过管道长度，管道写满后将阻塞，因为读数据量一直得不到
//   满足，故阻塞将持续下去。
//2、整体模式下，请求的数据量超过(管道长度-最后一次整体写入数据量)，将出现和1一
//   样的效果。
 struct tagPipePCB
{
    struct tagRingBuf pipe_buf;               //环形发送缓冲区.
    struct tagSemaphoreLCB *pipe_write_semp;  //写缓冲区锁
    struct tagSemaphoreLCB *pipe_read_semp;   //读缓冲区锁
    u32    write_level;                     //写触发水平
    u32    read_level;                      //读触发水平
    bool_t whole_mode;          //整体模式，true=确保每次写入完整记录，比如要写
                                //入10个字节，但pipe_buf中只有9个空位时，将阻塞
                                //false=先写入9个，然后阻塞
};

#define pipe_open_write(name,timeout)       Driver_DevOpenLeft(name,timeout)
#define pipe_open_read(name,timeout)        Driver_DevOpenRight(name,timeout)
#define pipe_open_write_fast(pipe,timeout)  Driver_DevOpenLeftFast(pipe,timeout)
#define pipe_open_read_fast(pipe,timeout)   Driver_DevOpenRightFast(pipe,timeout)
#define pipe_close_write(pipe)              Driver_DevCloseLeft(pipe)
#define pipe_close_read(pipe)               Driver_DevCloseRight(pipe)
#define pipe_write(pipe,buf,len,timeout)    Driver_DevWriteLeft(pipe,buf,len,timeout)
#define pipe_read(pipe,buf,len,timeout)     Driver_DevReadRight(pipe,buf,len,timeout)

ptu32_t Pipe_ModuleInit(ptu32_t para);
tagDevHandle Pipe_Create(u8* buf,u32 size,bool_t whole,
                             u32 write_level,u32 read_level,char *name);
ptu32_t Pipe_DevRead(tagDevHandle pipe_dev,ptu32_t res,
                                        ptu32_t dst_buf,ptu32_t len);
ptu32_t Pipe_DevWrite(tagDevHandle pipe_dev,ptu32_t src_buf,
                            ptu32_t res,ptu32_t len);
ptu32_t Pipe_DevCtrl(tagDevHandle pipe_dev,u32 cmd,
                        u32 data1,u32 data2);
bool_t Pipe_Delete(tagDevHandle pipe_dev);
u32 Pipe_Capacity(tagDevHandle pipe_dev);
u32 Pipe_Check(tagDevHandle pipe_dev);

#ifdef __cplusplus
}
#endif

#endif //__PIPE_H__

