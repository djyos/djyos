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
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：board.c
// 模块描述: 板件相关部分初始化或配置等
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 5/11.2015
// =============================================================================

#include "stdint.h"
#include "stddef.h"
#include "os.h"
#include "cpu_peri.h"


// =============================================================================
// 功能：根据具体的板件配置串口的GPIO的引脚功能，这是与板件相关，所以该函数放在该文件，CPU
//      串口驱动直接调用该函数来初始化串口的GPIO引脚
//      主要包括GPIO端口和串口时钟使能、GPIO配置、重映射、时钟等
// 参数：无
// 返回：true
// =============================================================================
bool_t Board_UartGpioInit(u8 SerialNo)
{
    GPIO_PowerOn(1);
    switch(SerialNo)
    {
    case CN_UART0:
        GPIO_CfgPinFunc(0,2,1);
        GPIO_CfgPinFunc(0,3,1);
        break;
    case CN_UART1:
        GPIO_CfgPinFunc(2,0,2);
        GPIO_CfgPinFunc(2,1,2);
        break;
//    case CN_UART2:
//        GPIO_CfgPinFunc(0,10,1);
//        GPIO_CfgPinFunc(0,11,1);
//        break;
    case CN_UART3:
        GPIO_CfgPinFunc(4,28,3);
        GPIO_CfgPinFunc(4,29,3);
        break;
    default:
        return false;
    }
    return true;
}

// =============================================================================
// 功能：485通信时半双工设置为发送，将GPIO口设置，若非半双工，此函数可空着
// 参数：串口号
// 返回：无
// =============================================================================
void Board_UartHalfDuplexSend(u8 SerialNo)
{

}

// =============================================================================
// 功能：485通信时半双工设置为接收，将GPIO口设置，若非半双工，此函数可空着
// 参数：串口号
// 返回：无
// =============================================================================
void Board_UartHalfDuplexRecv(u8 SerialNo)
{

}

// =============================================================================
// 功能：Board_AdcGpioInit板上有关CPU自带ADC0采样的GPIO初始化
// 参数：串口号
// 返回：无
// =============================================================================
void Board_AdcGpioInit(void)
{
	GPIO_CfgPinFunc(0,25,1);
	GPIO_CfgPinFunc(0,26,1);
	GPIO_CfgPinFunc(1,30,3);

	GPIO_CfgPinMode(0,25,CN_GPIO_MODE_OD_NOPULL);
	GPIO_CfgPinMode(0,26,CN_GPIO_MODE_OD_NOPULL);
	GPIO_CfgPinMode(1,30,CN_GPIO_MODE_OD_NOPULL);
}


