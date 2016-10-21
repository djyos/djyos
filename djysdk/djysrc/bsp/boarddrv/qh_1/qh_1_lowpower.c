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
//所属模块: CPU相关低功耗功能
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: cpu相关的低功耗管理,实现lowpower.h文件中规定bsp需要提供的函数,这里
//          实现跟板件相关的,跟CPU相关的函数,到cpudrv目录下找.
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "stddef.h"
#include "cpu_peri.h"
#include "stm32f10x.h"
#include "stm32f10x_flash.h"

//stm32上的低功耗Level定义(参考lowpower.h文件中的定义)
//L0:即执行wfe指令,无其他操作.与L1一样,进入的是处理器的睡眠模式.
//L1:wfe指令,内核执行的操作与L0一致,但用户的回调函数可能停掉更多时钟
//L2:即进入stm32的停止模式,保留SRAM和寄存器值,退出后继续运行
//L3/L4:进入stm32的待机模式,SRAM和寄存器值都丢失只有备份寄存器维持供电.区别在于
//退出后,L3进入时要保存SRAM和寄存器,退出时要恢复并从退出处继续执行.

extern   uint32_t   msp_top[ ];
#define  CN_LP_STACK_SIZE  0x100

//64K的片内RAM保存在flash主存储区的64k范围内
//主存储区为2K/Page
static void LP_RamFlashErase(void)
{
    u8 page;
    u32 addr;

    for(page = 255; page > 255-32; page--)
    {
        addr = page*0x800 + 0x08000000;
        FLASH_Unlock();
        FLASH_ErasePage(addr);
    }
}

static u32 LP_RamFlashProgram(u32 addr,u32 *buf,u32 len)
{
    u32 i;
    FLASH_Unlock();
    for(i = 0; i < len; i++)
    {
        FLASH_ProgramWord(addr+4*i,buf[i]);
    }

    return len;
}

static u32 LP_RamFlashRead(u32 addr,u32 *buf,u32 len)
{
    u32 i,temp;

    for(i = 0; i < len; )
    {
        temp = *(u32*)(addr+4*i);
        buf[i] = temp;
        i++;
    }
    return len;
}
//----恢复RAM------------------------------------------------------------------
//功能: 从非易失存储器中恢复RAM的内容,以便从进入低功耗处继续运行程序,仅用于L3级
//      低功耗.如果板件不打算支持L3级低功耗,本函数可直接返回false.
//参数: 无
//返回: true=成功恢复,false=失败
//-----------------------------------------------------------------------------
bool_t __LP_BSP_RestoreRamL3(void)
{
    u32 FlashAddr,len;
    u32 *RamAddr;
//  ModuleInit_K9f1208();       //init
//  for(i = 0; i < 256; i++)
//  {
//     NandFlash_Read(4096-300+i,0,0x60000000+0x4000*i,0x4000);
//  }
//
//  //内部SRAM分成三部分
//  //1.0x20000000 ~ msp_top-100 ,需恢复
//  //2.msp_top-100 ~ msp_top ,不能恢复，因为此时读flash程序正在使用
//  //3.msp_top - 0x20004000, 需恢复
//  temp = ((uint32_t)msp_top-CN_LP_STACK_SIZE - 0x20000000)/(uint32_t)0x4000;
//  for(i = 0; i < temp; i++)
//  {
//     NandFlash_Read(4096-4+i,0,0x20000000+0x4000*i,0x4000);
//  }
//  temp = ((uint32_t)msp_top-CN_LP_STACK_SIZE - 0x20000000)%0x4000;
//  NandFlash_Read(4096-4+i,0,0x20000000+0x4000*i,temp);
//
//  //两种情况：1.msp~msp-100处于一个block; 2.处于两个block
//  if((uint32_t)msp_top/0x4000 == ((uint32_t)msp_top-CN_LP_STACK_SIZE)/0x4000)
//      NandFlash_Read(4096-4+i,temp+CN_LP_STACK_SIZE,
//              temp+0x20000000+0x4000*i+CN_LP_STACK_SIZE,0x4000-CN_LP_STACK_SIZE-temp);
//  else
//  {
//      i++;
//      temp = ((uint32_t)msp_top- 0x20000000)%0x4000;
//      NandFlash_Read(4096-4+i,temp,0x20000000+0x4000*i+temp,0x4000-temp);
//  }
//
//  i++;
//
//  //读出剩余部分
//  for(; i <4 ; i++)
//  {
//      NandFlash_Read(4096-4+i,temp,0x20000000+0x4000*i+CN_LP_STACK_SIZE,0x4000);
//  }

    //内部SRAM分成三部分
    //1.0x20000000 ~ msp_top-100 ,需恢复
    //2.msp_top-100 ~ msp_top ,不能恢复，因为此时读flash程序正在使用
    //3.msp_top - 0x20004000, 需恢复
    RamAddr = (u32*)0x20000000;
    FlashAddr =  224*0x800 + 0x08000000;
    len = ((uint32_t)msp_top-CN_LP_STACK_SIZE - 0x20000000)/4;

    LP_RamFlashRead(FlashAddr,RamAddr,len);

    RamAddr = (u32 *)msp_top;
    FlashAddr = FlashAddr +len*4 + CN_LP_STACK_SIZE;
    len = (64*1024 - ((uint32_t)msp_top - 0x20000000))/4;

    LP_RamFlashRead(FlashAddr,RamAddr,len);

    return true;
}

//----备份RAM------------------------------------------------------------------
//功能: 进入L3级低功耗前,把RAM的内容保存到非易失存储器中,以便退出低功耗状态时,
//      恢复内存并从程序中断处继续运行.如果板件不打算支持L3级低功耗,本函数可
//      直接返回false.
//参数: 无
//返回: true=成功恢复,false=失败
//-----------------------------------------------------------------------------
bool_t __LP_BSP_SaveRamL3(void)
{
    u32 i,RamAddr,FlashAddr,len;
   //将现场缓存中的数据保存到flash
//   if(ModuleInit_K9f1208())
//   {
//     i = 300;
//     while(i--)
//     {
//         NandFlash_Erase(4096-i);
//     }
//     for(i = 0; i < 256; i++)
//     {
//         NandFlash_Write(4096-300+i,0,0x60000000+0x4000*i,0x4000);
//     }
//     for(i = 0; i < 4; i++)
//     {
//         NandFlash_Write(4096-4+i,0,0x20000000+0x4000*i,0x4000);
//     }
//   }
//   else
//     return false;
    RamAddr = 0x20000000;
    FlashAddr =  224*0x800 + 0x08000000;
    len = 64*1024/4;

    LP_RamFlashErase();

    LP_RamFlashProgram(FlashAddr,(u32*)RamAddr,len);

   return true;
}


