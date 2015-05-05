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
//作者：lst
//版本：V1.0.0
//文件描述:stm32 cpu的dma寄存器定义
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-09-10
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __CPU_PERI_DMA_H__
#define __CPU_PERI_DMA_H__

#ifdef __cplusplus
extern "C" {
#endif

//dma位域定义，bo_为位偏移，1位的用位带地址，bb_前缀，多位用掩码，bm_前缀
#define bb_dma1_ch1_ccr_base   (0x42000000 + 0x20008*32)            //位带基址
#define bb_dma1_ch1_ccr_en     (*(vu32*)(bb_dma1_ch1_ccr_base+4*0))
#define bb_dma1_ch1_ccr_tcie   (*(vu32*)(bb_dma1_ch1_ccr_base+4*1))
#define bb_dma1_ch1_ccr_htie   (*(vu32*)(bb_dma1_ch1_ccr_base+4*2))
#define bb_dma1_ch1_ccr_teie   (*(vu32*)(bb_dma1_ch1_ccr_base+4*3))
#define bb_dma1_ch1_ccr_dir    (*(vu32*)(bb_dma1_ch1_ccr_base+4*4))
#define bb_dma1_ch1_ccr_circ   (*(vu32*)(bb_dma1_ch1_ccr_base+4*5))
#define bb_dma1_ch1_ccr_pinc   (*(vu32*)(bb_dma1_ch1_ccr_base+4*6))
#define bb_dma1_ch1_ccr_minc   (*(vu32*)(bb_dma1_ch1_ccr_base+4*7))
#define bb_dma1_ch1_ccr_m2m    (*(vu32*)(bb_dma1_ch1_ccr_base+4*14))
#define bm_dma1_ch1_ccr_psize  0x00000300
#define bm_dma1_ch1_ccr_msize  0x00000c00
#define bm_dma1_ch1_ccr_pl     0x00003000
#define bo_dma1_ch1_ccr_en     0    //1=通道启用
#define bo_dma1_ch1_ccr_tcie   1    //1=允许传输完成中断
#define bo_dma1_ch1_ccr_htie   2    //1=允许半传输中断
#define bo_dma1_ch1_ccr_teie   3    //1=允许传输错误中断
#define bo_dma1_ch1_ccr_dir    4    //0=从外设读，1=从存储器读
#define bo_dma1_ch1_ccr_circ   5    //0=执行循环操作，1反之
#define bo_dma1_ch1_ccr_pinc   6    //0=外设地址不增量，1反之
#define bo_dma1_ch1_ccr_minc   7    //0=存储器地址不增量，1反之
#define bo_dma1_ch1_ccr_m2m    14   //0=非存储器到存储器模式，1=启用存储器到存储器模式
#define bo_dma1_ch1_ccr_psize  8    //外设数据宽度，00=8，01=16，10=32
#define bo_dma1_ch1_ccr_msize  10   //存储器数据宽度，00=8，01=16，10=32
#define bo_dma1_ch1_ccr_pl     12   //通道优先级，00最低，11最高

#define bb_dma1_ch2_ccr_base   (0x42000000 + 0x2001c*32)            //位带基址
#define bb_dma1_ch2_ccr_en     (*(vu32*)(bb_dma1_ch2_ccr_base+4*0))
#define bb_dma1_ch2_ccr_tcie   (*(vu32*)(bb_dma1_ch2_ccr_base+4*1))
#define bb_dma1_ch2_ccr_htie   (*(vu32*)(bb_dma1_ch2_ccr_base+4*2))
#define bb_dma1_ch2_ccr_teie   (*(vu32*)(bb_dma1_ch2_ccr_base+4*3))
#define bb_dma1_ch2_ccr_dir    (*(vu32*)(bb_dma1_ch2_ccr_base+4*4))
#define bb_dma1_ch2_ccr_circ   (*(vu32*)(bb_dma1_ch2_ccr_base+4*5))
#define bb_dma1_ch2_ccr_pinc   (*(vu32*)(bb_dma1_ch2_ccr_base+4*6))
#define bb_dma1_ch2_ccr_minc   (*(vu32*)(bb_dma1_ch2_ccr_base+4*7))
#define bb_dma1_ch2_ccr_m2m    (*(vu32*)(bb_dma1_ch2_ccr_base+4*14))
#define bm_dma1_ch2_ccr_psize  0x00000300
#define bm_dma1_ch2_ccr_msize  0x00000c00
#define bm_dma1_ch2_ccr_pl     0x00003000
#define bo_dma1_ch2_ccr_en     0    //1=通道启用
#define bo_dma1_ch2_ccr_tcie   1    //1=允许传输完成中断
#define bo_dma1_ch2_ccr_htie   2    //1=允许半传输中断
#define bo_dma1_ch2_ccr_teie   3    //1=允许传输错误中断
#define bo_dma1_ch2_ccr_dir    4    //0=从外设读，1=从存储器读
#define bo_dma1_ch2_ccr_circ   5    //0=执行循环操作，1反之
#define bo_dma1_ch2_ccr_pinc   6    //0=外设地址不增量，1反之
#define bo_dma1_ch2_ccr_minc   7    //0=存储器地址不增量，1反之
#define bo_dma1_ch2_ccr_m2m    14   //0=非存储器到存储器模式，1=启用存储器到存储器模式
#define bo_dma1_ch2_ccr_psize  8    //外设数据宽度，00=8，01=16，10=32
#define bo_dma1_ch2_ccr_msize  10   //存储器数据宽度，00=8，01=16，10=32
#define bo_dma1_ch2_ccr_pl     12   //通道优先级，00最低，11最高

#define bb_dma1_ch3_ccr_base   (0x42000000 + 0x20030*32)            //位带基址
#define bb_dma1_ch3_ccr_en     (*(vu32*)(bb_dma1_ch3_ccr_base+4*0))
#define bb_dma1_ch3_ccr_tcie   (*(vu32*)(bb_dma1_ch3_ccr_base+4*1))
#define bb_dma1_ch3_ccr_htie   (*(vu32*)(bb_dma1_ch3_ccr_base+4*2))
#define bb_dma1_ch3_ccr_teie   (*(vu32*)(bb_dma1_ch3_ccr_base+4*3))
#define bb_dma1_ch3_ccr_dir    (*(vu32*)(bb_dma1_ch3_ccr_base+4*4))
#define bb_dma1_ch3_ccr_circ   (*(vu32*)(bb_dma1_ch3_ccr_base+4*5))
#define bb_dma1_ch3_ccr_pinc   (*(vu32*)(bb_dma1_ch3_ccr_base+4*6))
#define bb_dma1_ch3_ccr_minc   (*(vu32*)(bb_dma1_ch3_ccr_base+4*7))
#define bb_dma1_ch3_ccr_m2m    (*(vu32*)(bb_dma1_ch3_ccr_base+4*14))
#define bm_dma1_ch3_ccr_psize  0x00000300  //外设数据宽度，00=8，01=16，10=32
#define bm_dma1_ch3_ccr_msize  0x00000c00  //存储器数据宽度，00=8，01=16，10=32
#define bm_dma1_ch3_ccr_pl     0x00003000  //通道优先级，00最低，11最高
#define bo_dma1_ch3_ccr_en     0    //1=通道启用
#define bo_dma1_ch3_ccr_tcie   1    //1=允许传输完成中断
#define bo_dma1_ch3_ccr_htie   2    //1=允许半传输中断
#define bo_dma1_ch3_ccr_teie   3    //1=允许传输错误中断
#define bo_dma1_ch3_ccr_dir    4    //0=从外设读，1=从存储器读
#define bo_dma1_ch3_ccr_circ   5    //0=执行循环操作，1反之
#define bo_dma1_ch3_ccr_pinc   6    //0=外设地址不增量，1反之
#define bo_dma1_ch3_ccr_minc   7    //0=存储器地址不增量，1反之
#define bo_dma1_ch3_ccr_m2m    14   //0=非存储器到存储器模式，1=启用存储器到存储器模式
#define bo_dma1_ch3_ccr_psize  8    //外设数据宽度，00=8，01=16，10=32
#define bo_dma1_ch3_ccr_msize  10   //存储器数据宽度，00=8，01=16，10=32
#define bo_dma1_ch3_ccr_pl     12   //通道优先级，00最低，11最高

#define bb_dma1_ch4_ccr_base   (0x42000000 + 0x20044*32)            //位带基址
#define bb_dma1_ch4_ccr_en     (*(vu32*)(bb_dma1_ch4_ccr_base+4*0))
#define bb_dma1_ch4_ccr_tcie   (*(vu32*)(bb_dma1_ch4_ccr_base+4*1))
#define bb_dma1_ch4_ccr_htie   (*(vu32*)(bb_dma1_ch4_ccr_base+4*2))
#define bb_dma1_ch4_ccr_teie   (*(vu32*)(bb_dma1_ch4_ccr_base+4*3))
#define bb_dma1_ch4_ccr_dir    (*(vu32*)(bb_dma1_ch4_ccr_base+4*4))
#define bb_dma1_ch4_ccr_circ   (*(vu32*)(bb_dma1_ch4_ccr_base+4*5))
#define bb_dma1_ch4_ccr_pinc   (*(vu32*)(bb_dma1_ch4_ccr_base+4*6))
#define bb_dma1_ch4_ccr_minc   (*(vu32*)(bb_dma1_ch4_ccr_base+4*7))
#define bb_dma1_ch4_ccr_m2m    (*(vu32*)(bb_dma1_ch4_ccr_base+4*14))
#define bm_dma1_ch4_ccr_psize  0x00000300  //外设数据宽度，00=8，01=16，10=32
#define bm_dma1_ch4_ccr_msize  0x00000c00  //存储器数据宽度，00=8，01=16，10=32
#define bm_dma1_ch4_ccr_pl     0x00003000  //通道优先级，00最低，11最高
#define bo_dma1_ch4_ccr_en     0    //1=通道启用
#define bo_dma1_ch4_ccr_tcie   1    //1=允许传输完成中断
#define bo_dma1_ch4_ccr_htie   2    //1=允许半传输中断
#define bo_dma1_ch4_ccr_teie   3    //1=允许传输错误中断
#define bo_dma1_ch4_ccr_dir    4    //0=从外设读，1=从存储器读
#define bo_dma1_ch4_ccr_circ   5    //0=执行循环操作，1反之
#define bo_dma1_ch4_ccr_pinc   6    //0=外设地址不增量，1反之
#define bo_dma1_ch4_ccr_minc   7    //0=存储器地址不增量，1反之
#define bo_dma1_ch4_ccr_m2m    14   //0=非存储器到存储器模式，1=启用存储器到存储器模式
#define bo_dma1_ch4_ccr_psize  8    //外设数据宽度，00=8，01=16，10=32
#define bo_dma1_ch4_ccr_msize  10   //存储器数据宽度，00=8，01=16，10=32
#define bo_dma1_ch4_ccr_pl     12   //通道优先级，00最低，11最高

#define bb_dma1_ch5_ccr_base   (0x42000000 + 0x20058*32)            //位带基址
#define bb_dma1_ch5_ccr_en     (*(vu32*)(bb_dma1_ch5_ccr_base+4*0))
#define bb_dma1_ch5_ccr_tcie   (*(vu32*)(bb_dma1_ch5_ccr_base+4*1))
#define bb_dma1_ch5_ccr_htie   (*(vu32*)(bb_dma1_ch5_ccr_base+4*2))
#define bb_dma1_ch5_ccr_teie   (*(vu32*)(bb_dma1_ch5_ccr_base+4*3))
#define bb_dma1_ch5_ccr_dir    (*(vu32*)(bb_dma1_ch5_ccr_base+4*4))
#define bb_dma1_ch5_ccr_circ   (*(vu32*)(bb_dma1_ch5_ccr_base+4*5))
#define bb_dma1_ch5_ccr_pinc   (*(vu32*)(bb_dma1_ch5_ccr_base+4*6))
#define bb_dma1_ch5_ccr_minc   (*(vu32*)(bb_dma1_ch5_ccr_base+4*7))
#define bb_dma1_ch5_ccr_m2m    (*(vu32*)(bb_dma1_ch5_ccr_base+4*14))
#define bm_dma1_ch5_ccr_psize  0x00000300  //外设数据宽度，00=8，01=16，10=32
#define bm_dma1_ch5_ccr_msize  0x00000c00  //存储器数据宽度，00=8，01=16，10=32
#define bm_dma1_ch5_ccr_pl     0x00003000  //通道优先级，00最低，11最高
#define bo_dma1_ch5_ccr_en     0    //1=通道启用
#define bo_dma1_ch5_ccr_tcie   1    //1=允许传输完成中断
#define bo_dma1_ch5_ccr_htie   2    //1=允许半传输中断
#define bo_dma1_ch5_ccr_teie   3    //1=允许传输错误中断
#define bo_dma1_ch5_ccr_dir    4    //0=从外设读，1=从存储器读
#define bo_dma1_ch5_ccr_circ   5    //0=执行循环操作，1反之
#define bo_dma1_ch5_ccr_pinc   6    //0=外设地址不增量，1反之
#define bo_dma1_ch5_ccr_minc   7    //0=存储器地址不增量，1反之
#define bo_dma1_ch5_ccr_m2m    14   //0=非存储器到存储器模式，1=启用存储器到存储器模式
#define bo_dma1_ch5_ccr_psize  8    //外设数据宽度，00=8，01=16，10=32
#define bo_dma1_ch5_ccr_msize  10   //存储器数据宽度，00=8，01=16，10=32
#define bo_dma1_ch5_ccr_pl     12   //通道优先级，00最低，11最高

#define bb_dma1_ch6_ccr_base   (0x42000000 + 0x2006c*32)            //位带基址
#define bb_dma1_ch6_ccr_en     (*(vu32*)(bb_dma1_ch6_ccr_base+4*0))
#define bb_dma1_ch6_ccr_tcie   (*(vu32*)(bb_dma1_ch6_ccr_base+4*1))
#define bb_dma1_ch6_ccr_htie   (*(vu32*)(bb_dma1_ch6_ccr_base+4*2))
#define bb_dma1_ch6_ccr_teie   (*(vu32*)(bb_dma1_ch6_ccr_base+4*3))
#define bb_dma1_ch6_ccr_dir    (*(vu32*)(bb_dma1_ch6_ccr_base+4*4))
#define bb_dma1_ch6_ccr_circ   (*(vu32*)(bb_dma1_ch6_ccr_base+4*5))
#define bb_dma1_ch6_ccr_pinc   (*(vu32*)(bb_dma1_ch6_ccr_base+4*6))
#define bb_dma1_ch6_ccr_minc   (*(vu32*)(bb_dma1_ch6_ccr_base+4*7))
#define bb_dma1_ch6_ccr_m2m    (*(vu32*)(bb_dma1_ch6_ccr_base+4*14))
#define bm_dma1_ch6_ccr_psize  0x00000300  //外设数据宽度，00=8，01=16，10=32
#define bm_dma1_ch6_ccr_msize  0x00000c00  //存储器数据宽度，00=8，01=16，10=32
#define bm_dma1_ch6_ccr_pl     0x00003000  //通道优先级，00最低，11最高
#define bo_dma1_ch6_ccr_en     0    //1=通道启用
#define bo_dma1_ch6_ccr_tcie   1    //1=允许传输完成中断
#define bo_dma1_ch6_ccr_htie   2    //1=允许半传输中断
#define bo_dma1_ch6_ccr_teie   3    //1=允许传输错误中断
#define bo_dma1_ch6_ccr_dir    4    //0=从外设读，1=从存储器读
#define bo_dma1_ch6_ccr_circ   5    //0=执行循环操作，1反之
#define bo_dma1_ch6_ccr_pinc   6    //0=外设地址不增量，1反之
#define bo_dma1_ch6_ccr_minc   7    //0=存储器地址不增量，1反之
#define bo_dma1_ch6_ccr_m2m    14   //0=非存储器到存储器模式，1=启用存储器到存储器模式
#define bo_dma1_ch6_ccr_psize  8    //外设数据宽度，00=8，01=16，10=32
#define bo_dma1_ch6_ccr_msize  10   //存储器数据宽度，00=8，01=16，10=32
#define bo_dma1_ch6_ccr_pl     12   //通道优先级，00最低，11最高

#define bb_dma1_ch7_ccr_base   (0x42000000 + 0x20080*32)            //位带基址
#define bb_dma1_ch7_ccr_en     (*(vu32*)(bb_dma1_ch7_ccr_base+4*0))
#define bb_dma1_ch7_ccr_tcie   (*(vu32*)(bb_dma1_ch7_ccr_base+4*1))
#define bb_dma1_ch7_ccr_htie   (*(vu32*)(bb_dma1_ch7_ccr_base+4*2))
#define bb_dma1_ch7_ccr_teie   (*(vu32*)(bb_dma1_ch7_ccr_base+4*3))
#define bb_dma1_ch7_ccr_dir    (*(vu32*)(bb_dma1_ch7_ccr_base+4*4))
#define bb_dma1_ch7_ccr_circ   (*(vu32*)(bb_dma1_ch7_ccr_base+4*5))
#define bb_dma1_ch7_ccr_pinc   (*(vu32*)(bb_dma1_ch7_ccr_base+4*6))
#define bb_dma1_ch7_ccr_minc   (*(vu32*)(bb_dma1_ch7_ccr_base+4*7))
#define bb_dma1_ch7_ccr_m2m    (*(vu32*)(bb_dma1_ch7_ccr_base+4*14))
#define bm_dma1_ch7_ccr_psize  0x00000300  //外设数据宽度，00=8，01=16，10=32
#define bm_dma1_ch7_ccr_msize  0x00000c00  //存储器数据宽度，00=8，01=16，10=32
#define bm_dma1_ch7_ccr_pl     0x00003000  //通道优先级，00最低，11最高
#define bo_dma1_ch7_ccr_en     0    //1=通道启用
#define bo_dma1_ch7_ccr_tcie   1    //1=允许传输完成中断
#define bo_dma1_ch7_ccr_htie   2    //1=允许半传输中断
#define bo_dma1_ch7_ccr_teie   3    //1=允许传输错误中断
#define bo_dma1_ch7_ccr_dir    4    //0=从外设读，1=从存储器读
#define bo_dma1_ch7_ccr_circ   5    //0=执行循环操作，1反之
#define bo_dma1_ch7_ccr_pinc   6    //0=外设地址不增量，1反之
#define bo_dma1_ch7_ccr_minc   7    //0=存储器地址不增量，1反之
#define bo_dma1_ch7_ccr_m2m    14   //0=非存储器到存储器模式，1=启用存储器到存储器模式
#define bo_dma1_ch7_ccr_psize  8    //外设数据宽度，00=8，01=16，10=32
#define bo_dma1_ch7_ccr_msize  10   //存储器数据宽度，00=8，01=16，10=32
#define bo_dma1_ch7_ccr_pl     12   //通道优先级，00最低，11最高


#define bb_dma2_ch1_ccr_base   (0x42000000 + 0x20408*32)            //位带基址
#define bb_dma2_ch1_ccr_en     (*(vu32*)(bb_dma2_ch1_ccr_base+4*0))
#define bb_dma2_ch1_ccr_tcie   (*(vu32*)(bb_dma2_ch1_ccr_base+4*1))
#define bb_dma2_ch1_ccr_htie   (*(vu32*)(bb_dma2_ch1_ccr_base+4*2))
#define bb_dma2_ch1_ccr_teie   (*(vu32*)(bb_dma2_ch1_ccr_base+4*3))
#define bb_dma2_ch1_ccr_dir    (*(vu32*)(bb_dma2_ch1_ccr_base+4*4))
#define bb_dma2_ch1_ccr_circ   (*(vu32*)(bb_dma2_ch1_ccr_base+4*5))
#define bb_dma2_ch1_ccr_pinc   (*(vu32*)(bb_dma2_ch1_ccr_base+4*6))
#define bb_dma2_ch1_ccr_minc   (*(vu32*)(bb_dma2_ch1_ccr_base+4*7))
#define bb_dma2_ch1_ccr_m2m    (*(vu32*)(bb_dma2_ch1_ccr_base+4*14))
#define bm_dma2_ch1_ccr_psize  0x00000300  //外设数据宽度，00=8，01=16，10=32
#define bm_dma2_ch1_ccr_msize  0x00000c00  //存储器数据宽度，00=8，01=16，10=32
#define bm_dma2_ch1_ccr_pl     0x00003000  //通道优先级，00最低，11最高
#define bo_dma2_ch1_ccr_en     0    //1=通道启用
#define bo_dma2_ch1_ccr_tcie   1    //1=允许传输完成中断
#define bo_dma2_ch1_ccr_htie   2    //1=允许半传输中断
#define bo_dma2_ch1_ccr_teie   3    //1=允许传输错误中断
#define bo_dma2_ch1_ccr_dir    4    //0=从外设读，1=从存储器读
#define bo_dma2_ch1_ccr_circ   5    //0=执行循环操作，1反之
#define bo_dma2_ch1_ccr_pinc   6    //0=外设地址不增量，1反之
#define bo_dma2_ch1_ccr_minc   7    //0=存储器地址不增量，1反之
#define bo_dma2_ch1_ccr_m2m    14   //0=非存储器到存储器模式，1=启用存储器到存储器模式
#define bo_dma2_ch1_ccr_psize  8    //外设数据宽度，00=8，01=16，10=32
#define bo_dma2_ch1_ccr_msize  10   //存储器数据宽度，00=8，01=16，10=32
#define bo_dma2_ch1_ccr_pl     12   //通道优先级，00最低，11最高

#define bb_dma2_ch2_ccr_base   (0x42000000 + 0x2041c*32)           //位带基址
#define bb_dma2_ch2_ccr_en     (*(vu32*)(bb_dma2_ch2_ccr_base+4*0))
#define bb_dma2_ch2_ccr_tcie   (*(vu32*)(bb_dma2_ch2_ccr_base+4*1))
#define bb_dma2_ch2_ccr_htie   (*(vu32*)(bb_dma2_ch2_ccr_base+4*2))
#define bb_dma2_ch2_ccr_teie   (*(vu32*)(bb_dma2_ch2_ccr_base+4*3))
#define bb_dma2_ch2_ccr_dir    (*(vu32*)(bb_dma2_ch2_ccr_base+4*4))
#define bb_dma2_ch2_ccr_circ   (*(vu32*)(bb_dma2_ch2_ccr_base+4*5))
#define bb_dma2_ch2_ccr_pinc   (*(vu32*)(bb_dma2_ch2_ccr_base+4*6))
#define bb_dma2_ch2_ccr_minc   (*(vu32*)(bb_dma2_ch2_ccr_base+4*7))
#define bb_dma2_ch2_ccr_m2m    (*(vu32*)(bb_dma2_ch2_ccr_base+4*14))
#define bm_dma2_ch2_ccr_psize  0x00000300  //外设数据宽度，00=8，01=16，10=32
#define bm_dma2_ch2_ccr_msize  0x00000c00  //存储器数据宽度，00=8，01=16，10=32
#define bm_dma2_ch2_ccr_pl     0x00003000  //通道优先级，00最低，11最高
#define bo_dma2_ch2_ccr_en     0    //1=通道启用
#define bo_dma2_ch2_ccr_tcie   1    //1=允许传输完成中断
#define bo_dma2_ch2_ccr_htie   2    //1=允许半传输中断
#define bo_dma2_ch2_ccr_teie   3    //1=允许传输错误中断
#define bo_dma2_ch2_ccr_dir    4    //0=从外设读，1=从存储器读
#define bo_dma2_ch2_ccr_circ   5    //0=执行循环操作，1反之
#define bo_dma2_ch2_ccr_pinc   6    //0=外设地址不增量，1反之
#define bo_dma2_ch2_ccr_minc   7    //0=存储器地址不增量，1反之
#define bo_dma2_ch2_ccr_m2m    14   //0=非存储器到存储器模式，1=启用存储器到存储器模式
#define bo_dma2_ch2_ccr_psize  8    //外设数据宽度，00=8，01=16，10=32
#define bo_dma2_ch2_ccr_msize  10   //存储器数据宽度，00=8，01=16，10=32
#define bo_dma2_ch2_ccr_pl     12   //通道优先级，00最低，11最高

#define bb_dma2_ch3_ccr_base   (0x42000000 + 0x20430*32)            //位带基址
#define bb_dma2_ch3_ccr_en     (*(vu32*)(bb_dma2_ch3_ccr_base+4*0))
#define bb_dma2_ch3_ccr_tcie   (*(vu32*)(bb_dma2_ch3_ccr_base+4*1))
#define bb_dma2_ch3_ccr_htie   (*(vu32*)(bb_dma2_ch3_ccr_base+4*2))
#define bb_dma2_ch3_ccr_teie   (*(vu32*)(bb_dma2_ch3_ccr_base+4*3))
#define bb_dma2_ch3_ccr_dir    (*(vu32*)(bb_dma2_ch3_ccr_base+4*4))
#define bb_dma2_ch3_ccr_circ   (*(vu32*)(bb_dma2_ch3_ccr_base+4*5))
#define bb_dma2_ch3_ccr_pinc   (*(vu32*)(bb_dma2_ch3_ccr_base+4*6))
#define bb_dma2_ch3_ccr_minc   (*(vu32*)(bb_dma2_ch3_ccr_base+4*7))
#define bb_dma2_ch3_ccr_m2m    (*(vu32*)(bb_dma2_ch3_ccr_base+4*14))
#define bm_dma2_ch3_ccr_psize  0x00000300  //外设数据宽度，00=8，01=16，10=32
#define bm_dma2_ch3_ccr_msize  0x00000c00  //存储器数据宽度，00=8，01=16，10=32
#define bm_dma2_ch3_ccr_pl     0x00003000  //通道优先级，00最低，11最高
#define bo_dma2_ch3_ccr_en     0    //1=通道启用
#define bo_dma2_ch3_ccr_tcie   1    //1=允许传输完成中断
#define bo_dma2_ch3_ccr_htie   2    //1=允许半传输中断
#define bo_dma2_ch3_ccr_teie   3    //1=允许传输错误中断
#define bo_dma2_ch3_ccr_dir    4    //0=从外设读，1=从存储器读
#define bo_dma2_ch3_ccr_circ   5    //0=执行循环操作，1反之
#define bo_dma2_ch3_ccr_pinc   6    //0=外设地址不增量，1反之
#define bo_dma2_ch3_ccr_minc   7    //0=存储器地址不增量，1反之
#define bo_dma2_ch3_ccr_m2m    14   //0=非存储器到存储器模式，1=启用存储器到存储器模式
#define bo_dma2_ch3_ccr_psize  8    //外设数据宽度，00=8，01=16，10=32
#define bo_dma2_ch3_ccr_msize  10   //存储器数据宽度，00=8，01=16，10=32
#define bo_dma2_ch3_ccr_pl     12   //通道优先级，00最低，11最高

#define bb_dma2_ch4_ccr_base   (0x42000000 + 0x20444*32)            //位带基址
#define bb_dma2_ch4_ccr_en     (*(vu32*)(bb_dma2_ch4_ccr_base+4*0))
#define bb_dma2_ch4_ccr_tcie   (*(vu32*)(bb_dma2_ch4_ccr_base+4*1))
#define bb_dma2_ch4_ccr_htie   (*(vu32*)(bb_dma2_ch4_ccr_base+4*2))
#define bb_dma2_ch4_ccr_teie   (*(vu32*)(bb_dma2_ch4_ccr_base+4*3))
#define bb_dma2_ch4_ccr_dir    (*(vu32*)(bb_dma2_ch4_ccr_base+4*4))
#define bb_dma2_ch4_ccr_circ   (*(vu32*)(bb_dma2_ch4_ccr_base+4*5))
#define bb_dma2_ch4_ccr_pinc   (*(vu32*)(bb_dma2_ch4_ccr_base+4*6))
#define bb_dma2_ch4_ccr_minc   (*(vu32*)(bb_dma2_ch4_ccr_base+4*7))
#define bb_dma2_ch4_ccr_m2m    (*(vu32*)(bb_dma2_ch4_ccr_base+4*14))
#define bm_dma2_ch4_ccr_psize  0x00000300  //外设数据宽度，00=8，01=16，10=32
#define bm_dma2_ch4_ccr_msize  0x00000c00  //存储器数据宽度，00=8，01=16，10=32
#define bm_dma2_ch4_ccr_pl     0x00003000  //通道优先级，00最低，11最高
#define bo_dma2_ch4_ccr_en     0    //1=通道启用
#define bo_dma2_ch4_ccr_tcie   1    //1=允许传输完成中断
#define bo_dma2_ch4_ccr_htie   2    //1=允许半传输中断
#define bo_dma2_ch4_ccr_teie   3    //1=允许传输错误中断
#define bo_dma2_ch4_ccr_dir    4    //0=从外设读，1=从存储器读
#define bo_dma2_ch4_ccr_circ   5    //0=执行循环操作，1反之
#define bo_dma2_ch4_ccr_pinc   6    //0=外设地址不增量，1反之
#define bo_dma2_ch4_ccr_minc   7    //0=存储器地址不增量，1反之
#define bo_dma2_ch4_ccr_m2m    14   //0=非存储器到存储器模式，1=启用存储器到存储器模式
#define bo_dma2_ch4_ccr_psize  8    //外设数据宽度，00=8，01=16，10=32
#define bo_dma2_ch4_ccr_msize  10   //存储器数据宽度，00=8，01=16，10=32
#define bo_dma2_ch4_ccr_pl     12   //通道优先级，00最低，11最高

#define bb_dma2_ch5_ccr_base   (0x42000000 + 0x20458*32)           //位带基址
#define bb_dma2_ch5_ccr_en     (*(vu32*)(bb_dma2_ch5_ccr_base+4*0))
#define bb_dma2_ch5_ccr_tcie   (*(vu32*)(bb_dma2_ch5_ccr_base+4*1))
#define bb_dma2_ch5_ccr_htie   (*(vu32*)(bb_dma2_ch5_ccr_base+4*2))
#define bb_dma2_ch5_ccr_teie   (*(vu32*)(bb_dma2_ch5_ccr_base+4*3))
#define bb_dma2_ch5_ccr_dir    (*(vu32*)(bb_dma2_ch5_ccr_base+4*4))
#define bb_dma2_ch5_ccr_circ   (*(vu32*)(bb_dma2_ch5_ccr_base+4*5))
#define bb_dma2_ch5_ccr_pinc   (*(vu32*)(bb_dma2_ch5_ccr_base+4*6))
#define bb_dma2_ch5_ccr_minc   (*(vu32*)(bb_dma2_ch5_ccr_base+4*7))
#define bb_dma2_ch5_ccr_m2m    (*(vu32*)(bb_dma2_ch5_ccr_base+4*14))
#define bm_dma2_ch5_ccr_psize  0x00000300  //外设数据宽度，00=8，01=16，10=32
#define bm_dma2_ch5_ccr_msize  0x00000c00  //存储器数据宽度，00=8，01=16，10=32
#define bm_dma2_ch5_ccr_pl     0x00003000  //通道优先级，00最低，11最高
#define bo_dma2_ch5_ccr_en     0    //1=通道启用
#define bo_dma2_ch5_ccr_tcie   1    //1=允许传输完成中断
#define bo_dma2_ch5_ccr_htie   2    //1=允许半传输中断
#define bo_dma2_ch5_ccr_teie   3    //1=允许传输错误中断
#define bo_dma2_ch5_ccr_dir    4    //0=从外设读，1=从存储器读
#define bo_dma2_ch5_ccr_circ   5    //0=执行循环操作，1反之
#define bo_dma2_ch5_ccr_pinc   6    //0=外设地址不增量，1反之
#define bo_dma2_ch5_ccr_minc   7    //0=存储器地址不增量，1反之
#define bo_dma2_ch5_ccr_m2m    14   //0=非存储器到存储器模式，1=启用存储器到存储器模式
#define bo_dma2_ch5_ccr_psize  8    //外设数据宽度，00=8，01=16，10=32
#define bo_dma2_ch5_ccr_msize  10   //存储器数据宽度，00=8，01=16，10=32
#define bo_dma2_ch5_ccr_pl     12   //通道优先级，00最低，11最高
//dma1 channel1:0x40020008
//dma1 channel2:0x4002001c
//dma1 channel3:0x40020030
//dma1 channel4:0x40020044
//dma1 channel5:0x40020058
//dma1 channel6:0x4002006c
//dma1 channel7:0x40020080
//dma2 channel1:0x40020408
//dma2 channel2:0x4002041c
//dma2 channel3:0x40020430
//dma2 channel4:0x40020444
//dma2 channel5:0x40020458
struct tagDMA_StChannelReg
{
  vu32 CCR;
  vu32 CNDTR;
  vu32 CPAR;
  vu32 CMAR;
};

#define bb_dma1_isr_base    (0x42000000 + 0x20000*32)           //位带基址
#define bb_dma1_isr_gif1    (*(vu32*)(bb_dma1_isr_base+4*0))    //全局中断标志
#define bb_dma1_isr_tcif1   (*(vu32*)(bb_dma1_isr_base+4*1))    //传输完成标志
#define bb_dma1_isr_htif1   (*(vu32*)(bb_dma1_isr_base+4*2))    //半传输完成标志
#define bb_dma1_isr_teif1   (*(vu32*)(bb_dma1_isr_base+4*3))    //传输错误标志
#define bb_dma1_isr_gif2    (*(vu32*)(bb_dma1_isr_base+4*4))    //全局中断标志
#define bb_dma1_isr_tcif2   (*(vu32*)(bb_dma1_isr_base+4*5))    //传输完成标志
#define bb_dma1_isr_htif2   (*(vu32*)(bb_dma1_isr_base+4*6))    //半传输完成标志
#define bb_dma1_isr_teif2   (*(vu32*)(bb_dma1_isr_base+4*7))    //传输错误标志
#define bb_dma1_isr_gif3    (*(vu32*)(bb_dma1_isr_base+4*8))    //全局中断标志
#define bb_dma1_isr_tcif3   (*(vu32*)(bb_dma1_isr_base+4*9))    //传输完成标志
#define bb_dma1_isr_htif3   (*(vu32*)(bb_dma1_isr_base+4*10))   //半传输完成标志
#define bb_dma1_isr_teif3   (*(vu32*)(bb_dma1_isr_base+4*11))   //传输错误标志
#define bb_dma1_isr_gif4    (*(vu32*)(bb_dma1_isr_base+4*12))   //全局中断标志
#define bb_dma1_isr_tcif4   (*(vu32*)(bb_dma1_isr_base+4*13))   //传输完成标志
#define bb_dma1_isr_htif4   (*(vu32*)(bb_dma1_isr_base+4*14))   //半传输完成标志
#define bb_dma1_isr_teif4   (*(vu32*)(bb_dma1_isr_base+4*15))   //传输错误标志
#define bb_dma1_isr_gif5    (*(vu32*)(bb_dma1_isr_base+4*16))   //全局中断标志
#define bb_dma1_isr_tcif5   (*(vu32*)(bb_dma1_isr_base+4*17))   //传输完成标志
#define bb_dma1_isr_htif5   (*(vu32*)(bb_dma1_isr_base+4*18))   //半传输完成标志
#define bb_dma1_isr_teif5   (*(vu32*)(bb_dma1_isr_base+4*19))   //传输错误标志
#define bb_dma1_isr_gif6    (*(vu32*)(bb_dma1_isr_base+4*10))   //全局中断标志
#define bb_dma1_isr_tcif6   (*(vu32*)(bb_dma1_isr_base+4*11))   //传输完成标志
#define bb_dma1_isr_htif6   (*(vu32*)(bb_dma1_isr_base+4*22))   //半传输完成标志
#define bb_dma1_isr_teif6   (*(vu32*)(bb_dma1_isr_base+4*23))   //传输错误标志
#define bb_dma1_isr_gif7    (*(vu32*)(bb_dma1_isr_base+4*24))   //全局中断标志
#define bb_dma1_isr_tcif7   (*(vu32*)(bb_dma1_isr_base+4*25))   //传输完成标志
#define bb_dma1_isr_htif7   (*(vu32*)(bb_dma1_isr_base+4*26))   //半传输完成标志
#define bb_dma1_isr_teif7   (*(vu32*)(bb_dma1_isr_base+4*27))   //传输错误标志

#define bb_dma1_ifcr_base   (0x42000000 + 0x20004*32)           //位带基址
#define bb_dma1_ifcr_cgif1  (*(vu32*)(bb_dma1_ifcr_base+4*0))   //清全局中断标志
#define bb_dma1_ifcr_ctcif1 (*(vu32*)(bb_dma1_ifcr_base+4*1))   //清传输完成标志
#define bb_dma1_ifcr_chtif1 (*(vu32*)(bb_dma1_ifcr_base+4*2))   //清半传输完成标志
#define bb_dma1_ifcr_cteif1 (*(vu32*)(bb_dma1_ifcr_base+4*3))   //清传输错误标志
#define bb_dma1_ifcr_cgif2  (*(vu32*)(bb_dma1_ifcr_base+4*4))   //清全局中断标志
#define bb_dma1_ifcr_ctcif2 (*(vu32*)(bb_dma1_ifcr_base+4*5))   //清传输完成标志
#define bb_dma1_ifcr_chtif2 (*(vu32*)(bb_dma1_ifcr_base+4*6))   //清半传输完成标志
#define bb_dma1_ifcr_cteif2 (*(vu32*)(bb_dma1_ifcr_base+4*7))   //清传输错误标志
#define bb_dma1_ifcr_cgif3  (*(vu32*)(bb_dma1_ifcr_base+4*8))   //清全局中断标志
#define bb_dma1_ifcr_ctcif3 (*(vu32*)(bb_dma1_ifcr_base+4*9))   //清传输完成标志
#define bb_dma1_ifcr_chtif3 (*(vu32*)(bb_dma1_ifcr_base+4*10))  //清半传输完成标志
#define bb_dma1_ifcr_cteif3 (*(vu32*)(bb_dma1_ifcr_base+4*11))  //清传输错误标志
#define bb_dma1_ifcr_cgif4  (*(vu32*)(bb_dma1_ifcr_base+4*12))  //清全局中断标志
#define bb_dma1_ifcr_ctcif4 (*(vu32*)(bb_dma1_ifcr_base+4*13))  //清传输完成标志
#define bb_dma1_ifcr_chtif4 (*(vu32*)(bb_dma1_ifcr_base+4*14))  //清半传输完成标志
#define bb_dma1_ifcr_cteif4 (*(vu32*)(bb_dma1_ifcr_base+4*15))  //清传输错误标志
#define bb_dma1_ifcr_cgif5  (*(vu32*)(bb_dma1_ifcr_base+4*16))  //清全局中断标志
#define bb_dma1_ifcr_ctcif5 (*(vu32*)(bb_dma1_ifcr_base+4*17))  //清传输完成标志
#define bb_dma1_ifcr_chtif5 (*(vu32*)(bb_dma1_ifcr_base+4*18))  //清半传输完成标志
#define bb_dma1_ifcr_cteif5 (*(vu32*)(bb_dma1_ifcr_base+4*19))  //清传输错误标志
#define bb_dma1_ifcr_cgif6  (*(vu32*)(bb_dma1_ifcr_base+4*10))  //清全局中断标志
#define bb_dma1_ifcr_ctcif6 (*(vu32*)(bb_dma1_ifcr_base+4*11))  //清传输完成标志
#define bb_dma1_ifcr_chtif6 (*(vu32*)(bb_dma1_ifcr_base+4*22))  //清半传输完成标志
#define bb_dma1_ifcr_cteif6 (*(vu32*)(bb_dma1_ifcr_base+4*23))  //清传输错误标志
#define bb_dma1_ifcr_cgif7  (*(vu32*)(bb_dma1_ifcr_base+4*24))  //清全局中断标志
#define bb_dma1_ifcr_ctcif7 (*(vu32*)(bb_dma1_ifcr_base+4*25))  //清传输完成标志
#define bb_dma1_ifcr_chtif7 (*(vu32*)(bb_dma1_ifcr_base+4*26))  //清半传输完成标志
#define bb_dma1_ifcr_cteif7 (*(vu32*)(bb_dma1_ifcr_base+4*27))  //清传输错误标志

#define bb_dma2_isr_base    (0x42000000 + 0x20400*32)           //位带基址
#define bb_dma2_isr_gif1    (*(vu32*)(bb_dma2_isr_base+4*0))    //全局中断标志
#define bb_dma2_isr_tcif1   (*(vu32*)(bb_dma2_isr_base+4*1))    //传输完成标志
#define bb_dma2_isr_htif1   (*(vu32*)(bb_dma2_isr_base+4*2))    //半传输完成标志
#define bb_dma2_isr_teif1   (*(vu32*)(bb_dma2_isr_base+4*3))    //传输错误标志
#define bb_dma2_isr_gif2    (*(vu32*)(bb_dma2_isr_base+4*4))    //全局中断标志
#define bb_dma2_isr_tcif2   (*(vu32*)(bb_dma2_isr_base+4*5))    //传输完成标志
#define bb_dma2_isr_htif2   (*(vu32*)(bb_dma2_isr_base+4*6))    //半传输完成标志
#define bb_dma2_isr_teif2   (*(vu32*)(bb_dma2_isr_base+4*7))    //传输错误标志
#define bb_dma2_isr_gif3    (*(vu32*)(bb_dma2_isr_base+4*8))    //全局中断标志
#define bb_dma2_isr_tcif3   (*(vu32*)(bb_dma2_isr_base+4*9))    //传输完成标志
#define bb_dma2_isr_htif3   (*(vu32*)(bb_dma2_isr_base+4*10))   //半传输完成标志
#define bb_dma2_isr_teif3   (*(vu32*)(bb_dma2_isr_base+4*11))   //传输错误标志
#define bb_dma2_isr_gif4    (*(vu32*)(bb_dma2_isr_base+4*12))   //全局中断标志
#define bb_dma2_isr_tcif4   (*(vu32*)(bb_dma2_isr_base+4*13))   //传输完成标志
#define bb_dma2_isr_htif4   (*(vu32*)(bb_dma2_isr_base+4*14))   //半传输完成标志
#define bb_dma2_isr_teif4   (*(vu32*)(bb_dma2_isr_base+4*15))   //传输错误标志
#define bb_dma2_isr_gif5    (*(vu32*)(bb_dma2_isr_base+4*16))   //全局中断标志
#define bb_dma2_isr_tcif5   (*(vu32*)(bb_dma2_isr_base+4*17))   //传输完成标志
#define bb_dma2_isr_htif5   (*(vu32*)(bb_dma2_isr_base+4*18))   //半传输完成标志
#define bb_dma2_isr_teif5   (*(vu32*)(bb_dma2_isr_base+4*19))   //传输错误标志

#define bb_dma2_ifcr_base   (0x42000000 + 0x20404*32)           //位带基址
#define bb_dma2_ifcr_cgif1  (*(vu32*)(bb_dma2_ifcr_base+4*0))   //清全局中断标志
#define bb_dma2_ifcr_ctcif1 (*(vu32*)(bb_dma2_ifcr_base+4*1))   //清传输完成标志
#define bb_dma2_ifcr_chtif1 (*(vu32*)(bb_dma2_ifcr_base+4*2))   //清半传输完成标志
#define bb_dma2_ifcr_cteif1 (*(vu32*)(bb_dma2_ifcr_base+4*3))   //清传输错误标志
#define bb_dma2_ifcr_cgif2  (*(vu32*)(bb_dma2_ifcr_base+4*4))   //清全局中断标志
#define bb_dma2_ifcr_ctcif2 (*(vu32*)(bb_dma2_ifcr_base+4*5))   //清传输完成标志
#define bb_dma2_ifcr_chtif2 (*(vu32*)(bb_dma2_ifcr_base+4*6))   //清半传输完成标志
#define bb_dma2_ifcr_cteif2 (*(vu32*)(bb_dma2_ifcr_base+4*7))   //清传输错误标志
#define bb_dma2_ifcr_cgif3  (*(vu32*)(bb_dma2_ifcr_base+4*8))   //清全局中断标志
#define bb_dma2_ifcr_ctcif3 (*(vu32*)(bb_dma2_ifcr_base+4*9))   //清传输完成标志
#define bb_dma2_ifcr_chtif3 (*(vu32*)(bb_dma2_ifcr_base+4*10))  //清半传输完成标志
#define bb_dma2_ifcr_cteif3 (*(vu32*)(bb_dma2_ifcr_base+4*11))  //清传输错误标志
#define bb_dma2_ifcr_cgif4  (*(vu32*)(bb_dma2_ifcr_base+4*12))  //清全局中断标志
#define bb_dma2_ifcr_ctcif4 (*(vu32*)(bb_dma2_ifcr_base+4*13))  //清传输完成标志
#define bb_dma2_ifcr_chtif4 (*(vu32*)(bb_dma2_ifcr_base+4*14))  //清半传输完成标志
#define bb_dma2_ifcr_cteif4 (*(vu32*)(bb_dma2_ifcr_base+4*15))  //清传输错误标志
#define bb_dma2_ifcr_cgif5  (*(vu32*)(bb_dma2_ifcr_base+4*16))  //清全局中断标志
#define bb_dma2_ifcr_ctcif5 (*(vu32*)(bb_dma2_ifcr_base+4*17))  //清传输完成标志
#define bb_dma2_ifcr_chtif5 (*(vu32*)(bb_dma2_ifcr_base+4*18))  //清半传输完成标志
#define bb_dma2_ifcr_cteif5 (*(vu32*)(bb_dma2_ifcr_base+4*19))  //清传输错误标志

struct tagDMA_StReg      //dma1 base 0x40020000,dma2 base 0x40020400
{
  vu32 ISR;
  vu32 IFCR;
};

extern struct tagDMA_StReg volatile * const pg_dma1_reg;
extern struct tagDMA_StReg volatile * const pg_dma2_reg;

extern struct tagDMA_StChannelReg volatile * const pg_dma1_channel1_reg;
extern struct tagDMA_StChannelReg volatile * const pg_dma1_channel2_reg;
extern struct tagDMA_StChannelReg volatile * const pg_dma1_channel3_reg;
extern struct tagDMA_StChannelReg volatile * const pg_dma1_channel4_reg;
extern struct tagDMA_StChannelReg volatile * const pg_dma1_channel5_reg;
extern struct tagDMA_StChannelReg volatile * const pg_dma1_channel6_reg;
extern struct tagDMA_StChannelReg volatile * const pg_dma1_channel7_reg;

extern struct tagDMA_StChannelReg volatile * const pg_dma2_channel1_reg;
extern struct tagDMA_StChannelReg volatile * const pg_dma2_channel2_reg;
extern struct tagDMA_StChannelReg volatile * const pg_dma2_channel3_reg;
extern struct tagDMA_StChannelReg volatile * const pg_dma2_channel4_reg;
extern struct tagDMA_StChannelReg volatile * const pg_dma2_channel5_reg;

#ifdef __cplusplus
}
#endif
#endif //__CPU_PERI_DMA_H__

