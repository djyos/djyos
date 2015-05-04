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

#ifndef SRAM_H_
#define SRAM_H_

#define SRAM_D0_GPIOD14        	GPIO_Pin_14
#define SRAM_D1_GPIOD15        	GPIO_Pin_15
#define SRAM_D2_GPIOD0         	GPIO_Pin_0
#define SRAM_D3_GPIOD1         	GPIO_Pin_1
#define SRAM_D4_GPIOE7        	GPIO_Pin_7
#define SRAM_D5_GPIOE8        	GPIO_Pin_8
#define SRAM_D6_GPIOE9        	GPIO_Pin_9
#define SRAM_D7_GPIOE10        	GPIO_Pin_10
#define SRAM_D8_GPIOE11        	GPIO_Pin_11
#define SRAM_D9_GPIOE12        	GPIO_Pin_12
#define SRAM_D10_GPIOE13       GPIO_Pin_13
#define SRAM_D11_GPIOE14       GPIO_Pin_14
#define SRAM_D12_GPIOE15       GPIO_Pin_15
#define SRAM_D13_GPIOD8        	GPIO_Pin_8
#define SRAM_D14_GPIOD9        	GPIO_Pin_9
#define SRAM_D15_GPIOD10       GPIO_Pin_10

#define SRAM_WE_GPIOD5        	GPIO_Pin_5
#define SRAM_RE_GPIOD4        	GPIO_Pin_4
#define SRAM_CS_GPIOD7        	GPIO_Pin_7
#define SRAM_LB_GPIOE0        	GPIO_Pin_0
#define SRAM_UB_GPIOE1        	GPIO_Pin_1

#define SRAM_A0_GPIOF0        	GPIO_Pin_0
#define SRAM_A1_GPIOF1        	GPIO_Pin_1
#define SRAM_A2_GPIOF2        	GPIO_Pin_2
#define SRAM_A3_GPIOF3       	GPIO_Pin_3
#define SRAM_A4_GPIOF4       	GPIO_Pin_4
#define SRAM_A5_GPIOF5        	GPIO_Pin_5
#define SRAM_A6_GPIOF12        	GPIO_Pin_12
#define SRAM_A7_GPIOF13        	GPIO_Pin_13
#define SRAM_A8_GPIOF14       	GPIO_Pin_14
#define SRAM_A9_GPIOF15        	GPIO_Pin_15
#define SRAM_A10_GPIOG0        	GPIO_Pin_0
#define SRAM_A11_GPIOG1        	GPIO_Pin_1
#define SRAM_A12_GPIOG2        	GPIO_Pin_2
#define SRAM_A13_GPIOG3        	GPIO_Pin_3
#define SRAM_A14_GPIOG4        	GPIO_Pin_4
#define SRAM_A15_GPIOG5        	GPIO_Pin_5
#define SRAM_A16_GPIOD11       GPIO_Pin_11
#define SRAM_A17_GPIOD12       GPIO_Pin_12
#define SRAM_A18_GPIOD13       GPIO_Pin_13
#define SRAM_A19_GPIOE3        	GPIO_Pin_3
#define SRAM_A20_GPIOE4        	GPIO_Pin_4
#define SRAM_A21_GPIOE5        	GPIO_Pin_5


#define PSRAM_BASE       (uint8_t*)(uint32_t)(0x60000000)

#if !defined (PSRAM)
#define PSRAM	                	1
//#define EM7324SU16SP		1	                    // 4M字节的PSRAM
#define EM7644SU16ANP		1	                    // 8M字节的PSRAM
#endif


#if( PSRAM == EM7644SU16ANP )
    #define PSRAM_32TEST_PAGE	    	0x800000

    #define PSRAM_16TEST_PAGE	    	0x10000
    #define PSRAM_16TEST_nPAGE		0x80

    #define PSRAM_8TEST_PAGE	    	0x100
    #define PSRAM_8TEST_nPAGE	    	0x8000
#elif( PSRAM == EM7324SU16SP )
    #define PSRAM_32TEST_PAGE	    	0x400000

    #define PSRAM_16TEST_PAGE	    	0x10000
    #define PSRAM_16TEST_nPAGE		0x40

    #define PSRAM_8TEST_PAGE	    	0x100
    #define PSRAM_8TEST_nPAGE	    	0x4000
#else
    #define PSRAM_32TEST_PAGE	    	0x0
    #define PSRAM_32TEST_nPAGE		0x0

    #define PSRAM_16TEST_PAGE	    	0x0
    #define PSRAM_16TEST_nPAGE		0x0

    #define PSRAM_8TEST_PAGE	    	0x0
    #define PSRAM_8TEST_nPAGE	    	0x0
#endif

#endif /* SRAM_H_ */
