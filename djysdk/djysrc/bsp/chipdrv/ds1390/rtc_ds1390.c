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
//所属模块：RTC时钟模块
//作者：hm
//版本：V0.1.0
//文件描述：硬件RTC连接及RTC底层驱动
//其他说明：
//修订历史：
//1. 日期: 2014-04-12
//   作者：
//   新版本号：V0.1.0
//   修改说明：原始版本
//------------------------------------------------------
#include "stdint.h"
#include "cpu_peri.h"
#include "stdio.h"
#include "rtc_ds1390.h"
#include "board_config.h"

#define HexToBcd(x) ((((x) / 10) <<4) + ((x) % 10))            //将16进制转换成BCD码
#define BcdToHex(x) ((((x) & 0xF0) >>4) * 10 + ((x) & 0x0F))   //将BCD码转换成16进制

#define RTC_CMD_SECONDS         (0x01)
#define RTC_CMD_MINUTES         (0x02)
#define RTC_CMD_HOURS           (0x03)
#define RTC_CMD_DAY_OF_WEEK     (0x04)
#define RTC_CMD_DATE_OF_MONTH   (0x05)
#define RTC_CMD_MONTH           (0x06)
#define RTC_CMD_YEAR            (0x07)

tagSpiConfig *rtc_spi_Config=NULL;

//-----读RTC时钟---------------------------------------------------------------
//功能：读RTC时钟模块的相应寄存器，如读秒、分、时、年、月、日等
//参数：reg,相应的寄存器号，根据ds1390pdf文档可知，读命令与写命令相差0x80，即，
//          例如，读秒命令为0x01时，写秒命令为0x81,此参数只需传递0x01即可
//返回：获取的数值，为BCD码
//-----------------------------------------------------------------------------
static unsigned char __rtc_read (unsigned char reg)
{
    unsigned char ret;

    Spi_ActiveCS(CFG_RTC_SPI_BUS,CFG_RTC_SPI_CS);
    Spi_TxRx(CFG_RTC_SPI_BUS,&reg,1,&ret,1,1);
    Spi_InActiveCS(CFG_RTC_SPI_BUS,CFG_RTC_SPI_CS);
    return (int)ret< 0 ? 0 : ret;
}

//-----写RTC时钟---------------------------------------------------------------
//功能：写RTC时钟模块的相应寄存器
//参数：reg,相应的寄存器号，根据ds1390pdf文档可知，读命令与写命令相差0x80，即，
//          例如，读秒命令为0x01时，写秒命令为0x81,此参数只需传递0x01即可
//      val,写到寄存器中的数值，写入的数据为BCD码
//返回：无
//-----------------------------------------------------------------------------
static void __rtc_write (unsigned char reg, unsigned char val)
{
    unsigned char dout[2];  /* SPI Output Data Bytes */

    dout[0] = 0x80 | reg;
    dout[1] = val;

    Spi_ActiveCS(CFG_RTC_SPI_BUS,CFG_RTC_SPI_CS);
    Spi_TxRx(CFG_RTC_SPI_BUS,dout,2,NULL,0,0);
    Spi_InActiveCS(CFG_RTC_SPI_BUS,CFG_RTC_SPI_CS);
}

//-----更新RTC时钟---------------------------------------------------------------
//功能：将年月日时分秒写入到RTC时钟，更新RTC，由于RTC芯片存放的时间信息是基于BCD
//      编码格式，所以在写入时间前，需要将时间信息转化为相应的BCD格式
//参数：DateTime,rtc_tm结构类型的变量，里面存放着需更新RTC的时间信息
//返回：无
//-----------------------------------------------------------------------------
uint32_t rtc_time_get(struct rtc_tm *DateTime)
{
    uint32_t sec, min, hour, mday, wday, mon, year;

    //判断是否需要初始化SPI，以防module_init_rtc未被调用
    if(!rtc_spi_Config)
    {
        rtc_spi_Config = &pg_spi_Config;
        rtc_spi_Config->freq=CFG_RTC_SPI_SPEED;
        Spi_Init(CFG_RTC_SPI_BUS,rtc_spi_Config);
    }

    //从RTC读时间
    sec  = __rtc_read (RTC_CMD_SECONDS);
    min  = __rtc_read (RTC_CMD_MINUTES);
    hour = __rtc_read (RTC_CMD_HOURS);
    mday = __rtc_read (RTC_CMD_DATE_OF_MONTH);
    wday = __rtc_read (RTC_CMD_DAY_OF_WEEK);
    mon  = __rtc_read (RTC_CMD_MONTH);
    year = __rtc_read (RTC_CMD_YEAR);

    //将BCD格式转化为正常模式
    DateTime->tm_sec    = BcdToHex(sec & 0x7F);
    DateTime->tm_min    = BcdToHex(min & 0x7F);
    DateTime->tm_hour   = BcdToHex(hour);
    DateTime->tm_mday   = BcdToHex(mday & 0x3F);
    DateTime->tm_wday   = BcdToHex(wday & 0x07) - 1;
    DateTime->tm_mon    = BcdToHex(mon & 0x1F);
    DateTime->tm_year   = BcdToHex(year) + 2000;

/*---------------------test use only----------------------*/
    printf("Get RTC year: %04d mon: %02d mday: %02d wday: %02d "
           "hr: %02d min: %02d sec: %02d\r\n",
           DateTime->tm_year, DateTime->tm_mon, DateTime->tm_mday,
           DateTime->tm_wday, DateTime->tm_hour, DateTime->tm_min,
           DateTime->tm_sec);
/*---------------------test use only----------------------*/

    return 0;
}
//-----更新RTC时钟---------------------------------------------------------------
//功能：将年月日时分秒写入到RTC时钟，更新RTC，由于RTC芯片存放的时间信息是基于BCD
//      编码格式，所以在写入时间前，需要将时间信息转化为相应的BCD格式
//参数：DateTime,rtc_tm结构类型的变量，里面存放着需更新RTC的时间信息
//返回：无
//-----------------------------------------------------------------------------
bool_t rtc_update_time(struct rtc_tm *DateTime)
{
    //判断是否需要初始化SPI，以防module_init_rtc未被调用
    if(!rtc_spi_Config)
    {
        rtc_spi_Config = &pg_spi_Config;
        rtc_spi_Config->freq=CFG_RTC_SPI_SPEED;
        Spi_Init(CFG_RTC_SPI_BUS,rtc_spi_Config);
    }

    __rtc_write (RTC_CMD_SECONDS,     HexToBcd (DateTime->tm_sec));
    __rtc_write (RTC_CMD_MINUTES,     HexToBcd (DateTime->tm_min));
    __rtc_write (RTC_CMD_HOURS,       HexToBcd (DateTime->tm_hour));
    __rtc_write (RTC_CMD_DAY_OF_WEEK, HexToBcd (DateTime->tm_wday + 1));//星期几
    __rtc_write (RTC_CMD_DATE_OF_MONTH, HexToBcd (DateTime->tm_mday));
    __rtc_write (RTC_CMD_MONTH,       HexToBcd (DateTime->tm_mon));
    __rtc_write (RTC_CMD_YEAR,        HexToBcd (DateTime->tm_year- 2000));

    return 1;
}

//----初始化rtc实时时钟模块------------------------------------------------------
//功能：初始化RTC模块，主要进行外部中断1的初始化
//参数：模块初始化函数没有参数
//返回：true = 成功初始化，false = 初始化失败
//-----------------------------------------------------------------------------
u32 rtc_init(void)
{
    if(!rtc_spi_Config)
    {
        rtc_spi_Config = &pg_spi_Config;
        rtc_spi_Config->freq=CFG_RTC_SPI_SPEED;
        Spi_Init(CFG_RTC_SPI_BUS,rtc_spi_Config);
    }
/*---------------------test use only----------------------*/
    struct rtc_tm time,gtime;
    time.tm_sec    = 00;
    time.tm_min    = 12;
    time.tm_hour   = 16;
    time.tm_mday   = 15;
    time.tm_wday   = 2;
    time.tm_mon    = 4;
    time.tm_year   = 2014;
    //rtc_update_time(&time);
    rtc_time_get(&gtime);
/*---------------------test use only----------------------*/
    return 1;
}

#ifdef  CN_CFG_DJYOS_USED
ptu32_t ModuleInstall_RTC(ptu32_t para)
{
    Tm_ConnectRtc(rtc_update_time,NULL);
    rtc_init();
    return 1;
}
#endif


