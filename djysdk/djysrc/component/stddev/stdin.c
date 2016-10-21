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
//所属模块：标准IO模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 标准输入输出管理
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2015-09-11
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 创建文件
//------------------------------------------------------

#include "stdint.h"
#include "stddef.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "ctype.h"
#include "object.h"
#include "driver.h"
#include <djyfs/file.h>
#include "systime.h"
#include "ring.h"

#define SCANF_FROM_STRING     0   //从string中读取字符
#define SCANF_FROM_FILE       1   //从文件中输入，用于scanf（stdin是文件），或者fscanf
//#define SCANF_FROM_DIRECT     2   //直接从原始硬件中输入字符。
#define SCANF_FROM_DEV        3   //从设备中输入，用于fscanf（fp是设备）
#define SCANF_FROM_STDIN_DEV  4   //从stdin设备中输入，用于scanf（stdin是设备）
s32 skip_atoi(const char **s);
extern struct RingBuf *g_ptStdinDevBuf;
#define CN_SCANF_BUF_LEN        32

//----从非文件stdin中读取一个字符---------------------------------------------
//功能：如果stdin是设备，get系列函数从stdin输入时，将调用本
//      函数，IO系统为非文件形式的stdin设立了一个环形缓冲区，如果缓冲区里有字符，
//      便直接读，该字符必定是 ugetc函数产生的。否则从stdin读取，读到的字符，
//      返回给用户并存放在环形缓冲区中。
//参数：Source，设备指针
//      Offset：返回累计读入的字符数
//      ch：
//返回：读入的字符。
//-----------------------------------------------------------------------------
//bool_t __ReadCharToStdinDev(char *ch)
//{
//    bool_t result = true;
//
//    if (Ring_Check(g_ptStdinDevBuf) != 0)       //检查缓冲区中是否有数据
//        Ring_Read(g_ptStdinDevBuf,(u8*)ch,1);   //有数据，直接读取
//    else
//    {
//        if (stdin != StdNotInit)   //如果stdin已经初始化，则其必定是设备，调用方确保
//        {
//            if(Driver_ReadDevice((struct DjyDevice *)stdin,(u8*)ch,1,0,CN_TIMEOUT_FOREVER) == 1)
//            {
//                Ring_Write(g_ptStdinDevBuf,(u8*)(&ch),1);   //数据同时写入缓冲区中
//                Ring_PseudoRead(g_ptStdinDevBuf,1);           //哑读一次数据
//            }
//            else
//                result = false;
//        }
//   }
//    return result;
//}

//----从文件输入一个字符-------------------------------------------------------
//功能：从文件输入一个字符，getc和fgetc是等价的，搞笑的c标准。fp可以是数据文件，
//      也可以是stdin。如果是stdin，则允许NULL，或者设备。
//      若stdin是设备或者NULL，需要回显。
//参数：fp，输出目标文件，如果是stdin，则要判断是否设备。
//返回：读取的字符，错误则返回EOF
//-----------------------------------------------------------------------------
s32 getc(FILE *fp)     //getc = fgetc
{
    char ch;
    if (fp != NULL)
    {
        //是文件，从文件读，ungetc由文件系统提供支持，不需要回显
        if (IS_FILE(fp))
        {
            if(fread(&ch,1,1,fp) != 1)
            {
                ch = EOF;
            }
        }
        else if(fp == stdin)    //非文件，且是stdin。
        {
            if (fp == StdNotInit)   //stdin未初始化，不支持ungetc，需要回显
            {
                if (GetCharDirect)
                {
                    ch = GetCharDirect( );
                    putchar(ch);
                }
                {
                    ch = EOF;
                }
            }
            else                   //stdin已初始化，支持ungetc，需要回显
                if(fread(&ch,1,1,fp) != 1)
                {
                    Ring_Write(g_ptStdinDevBuf,(u8*)(&ch),1);   //数据同时写入缓冲区中
                    Ring_PseudoRead(g_ptStdinDevBuf,1);           //哑读一次数据
                    putchar(ch);
                }
                else
                {
                    ch = EOF;
                }
        }
        else        //是设备，且不是stdin，不支持ungetc，不需要回显
        {
            if(fread(&ch,1,1,fp) != 1)
            {
                ch = EOF;
            }
        }
    }
    else
    {
       ch = EOF;
    }
    if (ch == EOF)
        Djy_SaveLastError(EN_FS_READFILE_EOF);
    return ch;
}

//----从文件输入一行-----------------------------------------------------------
//功能：从文件输入一行字符串，如果是stdin，则还可能是设备。持续输入直到回车，
//      包含空格。
//参数buf，输入字符串的缓冲区
//      size，buf的长度
//      fp，输出源文件，如果是stdin，则具备简单的编辑功能,允许是设备或者NULL。
//返回：1、成功，则返回第一个参数buf；
//      2、在读字符时遇到end-of-file，则eof指示器被设置，如果还没读入任何字符就
//         遇到这种情况，则buf保持原来的内容，返回NULL；
//      3、如果发生读入错误，error指示器被设置，返回NULL，buf的值可能被改变。
//-----------------------------------------------------------------------------
#define EditReview          0   //支持行编辑和回显
#define EditNoReview        1   //支持行编辑，不支持回显。
#define NoEditreview        2   //不支持行编辑，支持回显
#define NoEditNoReview      3   //不支持行编辑，不支持回显
#define InputError          4
char * fgets(char *buf, s32 size,FILE *fp)
{
    s32 result;
    s32 position = 0;
    char ch = 0;
    if (size == 0)
        return NULL;
    if (size == 1)
    {
        buf[0] = '\0';
        return buf;
    }
    if (fp == NULL)
    {
        Djy_SaveLastError(EN_FS_READFILE_EOF);
        return NULL;
    }
    //是文件，从文件读，ungetc由文件系统提供支持，不支持行编辑，不需要回显
    //是设备且非stdin，从设备读，不支持ungetc，不支持行编辑，不需要回显
    if (IS_FILE(fp) || (fp != stdin)) 
    {
        for (position = 0; position < size; position++)
        {
            if(fread(&ch,1,1,fp) != 1)
            {
                buf[position] = '\0';
                Djy_SaveLastError(EN_FS_READFILE_EOF);
                return NULL;
            }
            //忽略回车，以换行符结束，能兼容Windows和Linux格式文件。
            if(ch == '\r')
            {
                if (((struct Object*)fp)->Type == RSC_FILE)
                    continue;      //Windows格式文本文件有 '\r'，跳过
                else
                {
                    buf[position] = '\0';   //非文件，一般以回车结束一行。
                    break;
                }
            }
            else if(ch == '\n') //遇换行符，输入结束
            {
                buf[position] = '\0';
                puts("\n\r");
                break;
            }
            else
            {
                buf[position] = ch;
                continue;       //直接下一循环，绕过下面的字符编辑过程
            }
        }
        buf[size-1] = '\0';
        return buf;
    }
    if (fp == StdNotInit)   //原始硬件输入，不支持ungetc，不支持行编辑，需要回显
    {
        for (position = 0; position < size; position++)
        {
            if (GetCharDirect)
            {
                ch = GetCharDirect( );
            }
            else
                return NULL;
            if((ch == '\n') || (ch == '\r')) //遇换行符，输入结束
            {
                buf[position] = '\0';
                break;
            }
            else
            {
                buf[position] = ch;
                putchar(ch);
                continue;       //直接下一循环，绕过下面的字符编辑过程
            }
        }
        buf[size-1] = '\0';
        return buf;
    }
    else            //stdin设备输入，支持ungetc，支持行编辑，需要回显
    {
        if ( ! Ring_IsEmpty(g_ptStdinDevBuf))
        {
            position = Ring_Read(g_ptStdinDevBuf,(u8*)buf,size-1);
            buf[position] = '\0';
        }
        else
        {
            while ( 1 )
            {
                if(fread(&ch,1,1,fp) != 1)
                {
                    position = Ring_Read(g_ptStdinDevBuf,(u8*)buf,size-1);
                    buf[position] = '\0';
                    Djy_SaveLastError(EN_FS_READFILE_EOF);
                    return NULL;
                }
                switch (ch)
                {
                    case 37:        //left
                    case 38:        //up
                    case 39:        //right
                    case 40:        //down
                    case '\t':      //tab
                        break;
                    case '\r':      //回车键
                    case '\n':      //换行键
                    {
                        position = Ring_Read(g_ptStdinDevBuf,(u8*)buf,size-1);
                        buf[position] = '\0';
                        puts("\n\r");
                        return buf;
                    }break;
                    case '\b':      //退格键
                    {
                        if ( ! Ring_IsEmpty(g_ptStdinDevBuf))
                        {
                            Ring_SkipTail(g_ptStdinDevBuf,1);
                            puts("\b \b");
                        }
                    }break;
                    case 0x1B:      //esc键，已经输入的字符全部取消
                    {
                        position = Ring_Check(g_ptStdinDevBuf);
                        Ring_Clean(g_ptStdinDevBuf);
                        for (result = 0;result < position;result++)
                            puts("\b \b");
                    }break;
                    default:
                    {
                        //如果行缓冲区已满，就不再接受输入。
                        if( ! Ring_IsFull(g_ptStdinDevBuf))
                        {
                            Ring_Write(g_ptStdinDevBuf,(u8*)&ch,1);
                            putchar(ch);
                        }
                    }break;
                }
            }
        }
    }
    return buf;
}

//----输入流中回退一个字符-----------------------------------------------------
//功能：把一个字符退回给输入流，输入流的指针从当前位置回退一个字符，并用传入的
//      字符 c 覆盖该字符。如果stream是stdin且是设备，利用stdio模块内建的环形
//      缓冲区回退。
//参数：c，待返回给输入流的字符
//      stream，目标输入流
//返回：成功执行返回参数c，否则返回EOF
//-----------------------------------------------------------------------------
s32 ungetc(s32 c, FILE *stream)
{
    s32 result = c;
    if(stream == stdin)
    {
        //如果如果stdin==StdNotInit，则不能执行ungetc
        if (stdin != StdNotInit)
        {
            if(((struct Object*)stdin)->Type == RSC_DEVICE)
            {
                if ( ! Ring_IsFull(g_ptStdinDevBuf))
                    Ring_RecedeRead(g_ptStdinDevBuf,1,(u8*)(&c));
                else
                    result = EOF;       //缓冲区没有空间可以回退
            }
        }
        else
            return EOF;
    }
    else
    {
        if( ! fseek(stream,-1,SEEK_CUR))
            result = EOF;
    }
    return result;

}
//----从stdin读入一个字符------------------------------------------------------
//功能：从stdin读入一个字符，stdin可以是设备，也可以是文件
//参数：无
//返回：输入的字符，错误则返回EOF
//-----------------------------------------------------------------------------
s32 getchar( void )
{
    return getc(stdin);//todo:stdio.h已经宏实现
}


//----从stdin输入一个字符串----------------------------------------------------
//功能：从stdin输入一个字符串，stdin可以是设备，也可以是文件，甚至可以NULL（原始
//      硬件）。狗血的c标准，fgets函数有buf长度限定，但gets却没有，程序员应该自
//      己保证缓冲区足够长。
//参数：buf ，接收字符串的缓冲区
//返回：正确则返回buf指针，错误则返回NULL，并设置错误标志
//特注：1、gets会读出回车键，并替换成'\0'，scanf则把回车键留在输入缓冲区。
//      2、本函数对buf没有长度限制，强烈建议不要使用，用fgets替代。
//-----------------------------------------------------------------------------
char * gets(char *buf)
{
    fgets(buf,CN_LIMIT_SINT32,stdin);
    return buf;
}

u32 __sc_strtoul(ptu32_t Source,char (*__GetChar)(ptu32_t Source,s32 *Offset),s32 *Offset,s32 base,char *LastCh)
{
    u32 result = 0,value;
    char ch = *LastCh;
    switch (base)
    {
        case 8:
            while ((ch <= '7') && (ch >= '0'))
            {
                result = (result<<3) + (ch-'0');
                ch = __GetChar(Source,Offset);
            }
            break;
        case 10:
            while (isdigit((s32)ch))
            {
                result = (result * 10) + (ch-'0');
                ch = __GetChar(Source,Offset);
            }
            break;
        case 16:
            while (isxdigit((s32)ch))
            {
                value = isdigit((s32)ch) ? ch-'0' : toupper((s32)ch)-'A'+10;
                result = (result<<4) + value;
                ch = __GetChar(Source,Offset);
            }
            break;
    }
    *LastCh = ch;
    return result;
}

s32 __sc_strtol(ptu32_t Source,char (*__GetChar)(ptu32_t Source,s32 *Offset),s32 *Offset,s32 base,char *LastCh)
{
    s32 result = 0,value;
    char ch = *LastCh;
    switch (base)
    {
        case 8:
            while ((ch <= '7') && (ch >= '0'))
            {
                result = (result<<3) + (ch-'0');
                ch = __GetChar(Source,Offset);
            }
            break;
        case 10:
            while (isdigit((s32)ch))
            {
                result = (result * 10) + (ch-'0');
                ch = __GetChar(Source,Offset);
            }
            break;
        case 16:
            while (isxdigit((s32)ch))
            {
                value = isdigit((s32)ch) ? ch-'0' : toupper((s32)ch)-'A'+10;
                result = (result<<4) + value;
                ch = __GetChar(Source,Offset);
            }
            break;
    }
    *LastCh = ch;
    return result;
}

u64 __sc_strtoull(ptu32_t Source,char (*__GetChar)(ptu32_t Source,s32 *Offset),s32 *Offset,s32 base,char *LastCh)
{
    u64 result = 0,value;
    char ch = *LastCh;
    switch (base)
    {
        case 8:
            while ((ch <= '7') && (ch >= '0'))
            {
                result = (result<<3) + (ch-'0');
                ch = __GetChar(Source,Offset);
            }
            break;
        case 10:
            while (isdigit((s32)ch))
            {
                result = (result * 10) + (ch-'0');
                ch = __GetChar(Source,Offset);
            }
            break;
        case 16:
            while (isxdigit((s32)ch))
            {
                value = isdigit((s32)ch) ? ch-'0' : toupper((s32)ch)-'A'+10;
                result = (result<<4) + value;
                ch = __GetChar(Source,Offset);
            }
            break;
    }
    *LastCh = ch;
    return result;
}

s64 __sc_strtoll(ptu32_t Source,char (*__GetChar)(ptu32_t Source,s32 *Offset),s32 *Offset,s32 base,char *LastCh)
{
    s64 result = 0,value;
    char ch = *LastCh;
    switch (base)
    {
        case 8:
            while ((ch <= '7') && (ch >= '0'))
            {
                result = (result<<3) + (ch-'0');
                ch = __GetChar(Source,Offset);
            }
            break;
        case 10:
            while (isdigit((s32)ch))
            {
                result = (result * 10) + (ch-'0');
                ch = __GetChar(Source,Offset);
            }
            break;
        case 16:
            while (isxdigit((s32)ch))
            {
                value = isdigit((s32)ch) ? ch-'0' : toupper((s32)ch)-'A'+10;
                result = (result<<4) + value;
                ch = __GetChar(Source,Offset);
            }
            break;
    }
    *LastCh = ch;
    return result;
}

//----直接读取一个字符---------------------------------------------------------
//功能：直接从原始硬件中读入一个字符
//参数：Source，无意义
//      Offset：返回累计读入的字符数
//返回：读入的字符。
//-----------------------------------------------------------------------------
//char __GetCharDirect(ptu32_t Source, s32 *Offset)
//{
//    char ch;
//    ch = GetCharDirect( );
//    (*Offset)++;
//    PutStrDirect(&ch,1);
//    return ch;
//}

//----从字符串中读取一个字符---------------------------------------------------
//功能：从字符串中读入一个字符
//参数：Source，字符串指针
//      Offset：读入的偏移量，返回累计读入的字符数
//返回：读入的字符。
//-----------------------------------------------------------------------------
char __GetCharFromStr(ptu32_t Source,s32 *Offset)
{
    char ch;
    ch = ((char*)Source)[*Offset];
    (*Offset)++;
    return ch;
}

//----从文件中读取一个字符-----------------------------------------------------
//功能：直接从原始硬件中读入一个字符
//参数：Source，文件指针
//      Offset：返回累计读入的字符数
//返回：读入的字符。
//-----------------------------------------------------------------------------
char __GetCharFromFile(ptu32_t Source,s32 *Offset)
{
    char ch;
    fread(&ch,1,1,(FILE*)Source);
    (*Offset)++;
    return ch;
}

//----从设备中读取一个字符-----------------------------------------------------
//功能：直接从原始硬件中读入一个字符，如果缓冲区里有字符，便直接读，该字符必定
//      是 ugetc函数产生的。否则从设备读取。
//参数：Source，设备指针
//      Offset：返回累计读入的字符数
//返回：读入的字符。
//-----------------------------------------------------------------------------
char __GetCharFromDev(ptu32_t Source,s32 *Offset)
{
    char ch;
    fread(&ch,1,1,(FILE*)Source);
    (*Offset)++;
    return ch;
}

//----从Stdin设备中读取一个字符------------------------------------------------
//功能：直接从原始硬件中读入一个字符，如果缓冲区里有字符，便直接读，该字符必定
//      是 ugetc函数产生的。否则从设备读取。
//参数：Source，设备指针
//      Offset：返回累计读入的字符数
//返回：读入的字符。
//-----------------------------------------------------------------------------
static s32 s_StdinDevBufBytes = 0;
static s32 s_StdinDevposition = 0;
static char s_StdinDevBuf[CN_SCANF_BUF_LEN];
char __GetCharFromStdinDev(ptu32_t Source,s32 *Offset)
{
    char ch;
    if (s_StdinDevposition == s_StdinDevBufBytes)        //buf中没有数据
    {
        fgets(s_StdinDevBuf,CN_SCANF_BUF_LEN,stdin);
        s_StdinDevBufBytes = strlen(s_StdinDevBuf);
        s_StdinDevposition = 0;
        if(Ring_IsEmpty(g_ptStdinDevBuf))
            s_StdinDevBuf[s_StdinDevBufBytes++] = '\r';
    }
    ch = s_StdinDevBuf[s_StdinDevposition++];
    (*Offset)++;
    return ch;
}

//---------------------------------------------------------------------------
//功能: 按照格式字符串，解析输入字符串，Method参数设定输入字符串的来源
//参数: TempBuf，字符串转换使用的临时缓冲区，长度是CN_BUF_LENGTH，如果
//          Method==SCANF_FROM_STRING则忽略本参数，可传入NULL
//      Source，这是一个多用途的变量，设定被解析的字符串的来源属性，
//          依据Method不同，含义不同。
//          Method==SCANF_FROM_STRING，Source是字符串指针
//          Method==SCANF_FROM_DIRECT，Source无意义
//          Method==SCANF_FROM_FILE_OR_DEV，Source是文件或设备指针
//      Size，如果Source是字符串指针，Size表示数据尺寸，否则没有意义
//      Method，输出方法，SCANF_FROM_STRING等常量之一
//      fmt，格式字符串
//      args，可变参数的参数列表
//返回: 转换结果数量
//说明：目前没有加入浮点数解析--TODO
//-----------------------------------------------------------------------------
static s32 __vsnscanf(ptu32_t Source,u32 Method, const char *fmt,va_list args)
{
    char (*__GetChar)(ptu32_t Source,s32 *Offset);
    s32 Offset = 0;
    char ch,sign;
    s32 num = 0;
    s32 qualifier;
    s32 base;
    s32 field_width = -1;
    s32 is_sign = 0;
    switch (Method)
    {
        case SCANF_FROM_STRING:
            __GetChar = __GetCharFromStr;
            break;

//        case SCANF_FROM_DIRECT:
//            __GetChar = __GetCharDirect;
//            break;
        case SCANF_FROM_FILE:
            __GetChar = __GetCharFromFile;
            break;
        case SCANF_FROM_DEV:
            __GetChar = __GetCharFromDev;
            break;
        case SCANF_FROM_STDIN_DEV:
            __GetChar = __GetCharFromStdinDev;
            break;
        default:
            __GetChar = __GetCharFromStr;
            break;
    }
    ch = __GetChar(Source,&Offset);
    while(*fmt && ch)
    {
        /* skip any white space in format */
        /* white space in format matchs any amount of
         * white space, including none, in the input.
         */
        if (isspace((s32)*fmt))
        {
            while (isspace((s32)*fmt))
                ++fmt;
            while (isspace((s32)ch))
                ch = __GetChar(Source,&Offset);
        }

        /* anything that is not a conversion must match exactly */
        if (*fmt != '%' && *fmt)
        {
            if (*fmt++ != ch)
                break;
            ch = __GetChar(Source,&Offset);
            continue;
        }

        if (!*fmt)
            break;
        ++fmt;

        /* skip this conversion.
         * advance both strings to next white space
         */
        if (*fmt == '*') {
            while (!isspace((s32)*fmt) && *fmt)
                fmt++;
            while (!isspace((s32)ch) && ch)
                ch = __GetChar(Source,&Offset);
            continue;
        }

        /* get field width */
        if (isdigit((s32)*fmt))
            field_width = skip_atoi(&fmt);

        /* get conversion qualifier */
        qualifier = -1;
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt == 'Z') {
            qualifier = *fmt;
            fmt++;
        }

        if (!*fmt || !ch)
            break;

        switch(*fmt++)
        {
            case 'c':
            {
                char *s = (char *) va_arg(args,char*);
                if (field_width == -1)
                    field_width = 1;
                do
                {
                    *s++ = ch;
                    ch = __GetChar(Source,&Offset);
                } while(field_width-- > 0 && ch);
                num++;
            }
            continue;
            case 's':
            {
                char *s = (char *) va_arg(args, char *);
                if(field_width == -1)
                    field_width = INT_MAX;
                /* first, skip leading white space in buffer */
                while (isspace((s32)ch))
                    ch = __GetChar(Source,&Offset);

                /* now copy until next white space */
                while (ch && !isspace((s32)ch) && field_width--)
                {
                    *s++ = ch;
                    ch = __GetChar(Source,&Offset);
                }
                *s = '\0';
                num++;
            }
            continue;
            case 'n':
                /* return number of characters read so far */
            {
                s32 *i = (s32 *)va_arg(args,s32*);
                *i = Offset;
            }
            continue;
            is_sign = 0;
            case 'o':
                base = 8;
                is_sign = 1;
                break;
            case 'x':
            case 'X':
                base = 16;
                is_sign = 1;
                break;
            case 'd':
                base = 10;
                is_sign = 1;
                break;
            case 'i':
                base = 0;
                is_sign = 1;
                break;
            case 'u':
                base = 10;
                is_sign = 0;
                break;
                break;
            case '%':
                if (ch != '%')      //查找 %
                    goto GotoErrorExit;         //非法格式，结束
                ch = __GetChar(Source,&Offset);
                continue;
            default:
                goto GotoErrorExit;         //非法格式，结束
        }

        //各种数值转换
        while (isspace((s32)ch))       //首先删除前导空格
            ch = __GetChar(Source,&Offset);

        if (!ch)                  //串结束
            break;

        sign = ch;
        if ((sign == '-') || (sign == '+'))
            ch = __GetChar(Source,&Offset);

        //base = 0表示由输入流表示数据格式（8/10/16进制）。
        if (base == 0)
        {
            base = 10;
            if (ch == '0') {
                base = 8;
                ch = __GetChar(Source,&Offset);
                if ((ch == 'x') || (ch == 'X'))
                {
                    ch = __GetChar(Source,&Offset);
                    base = 16;
                }
            }
        } else if (base == 16)
        {
            if (ch == '0')
            {
                ch = __GetChar(Source,&Offset);
                if ((ch == 'x') || (ch == 'X'))
                    ch = __GetChar(Source,&Offset);
            }
        }

        switch(qualifier)
        {
        case 'h':
            if (is_sign) {
                s16 *s = (s16 *) va_arg(args,s16 *);
                *s = (s16) __sc_strtol(Source,__GetChar,&Offset,base,&ch);
                if (sign == '-')
                    *s = -*s;
            } else {
                u16 *s = (u16 *) va_arg(args, u16 *);
                *s = (u16) __sc_strtoul(Source,__GetChar,&Offset,base,&ch);
            }
            break;
        case 'l':
            if (is_sign) {
                s32 *l = (s32 *) va_arg(args,s32 *);
                *l = __sc_strtol(Source,__GetChar,&Offset,base,&ch);
                if (sign == '-')
                    *l = -*l;
            } else {
                u32 *l = (u32 *) va_arg(args,u32*);
                *l = __sc_strtoul(Source,__GetChar,&Offset,base,&ch);
            }
            break;
        case 'L':
            if (is_sign) {
                s64 *l = (s64 *) va_arg(args,s64 *);
                *l = __sc_strtoll(Source,__GetChar,&Offset,base,&ch);
                if (sign == '-')
                    *l = -*l;
            } else {
                u64 *l = (u64 *) va_arg(args,u64*);
                *l = __sc_strtoull(Source,__GetChar,&Offset,base,&ch);
            }
            break;
        case 'Z':
        {
            u32 *s = (u32*) va_arg(args,u32*);
            *s = (u32) __sc_strtoul(Source,__GetChar,&Offset,base,&ch);
        }
        break;
        default:
            if (is_sign) {
                s32 *i = (s32 *) va_arg(args, s32*);
                *i = (s32) __sc_strtol(Source,__GetChar,&Offset,base,&ch);
                if (sign == '-')
                    *i = -*i;
            } else {
                u32 *i = (u32*) va_arg(args, u32*);
                *i = (u32) __sc_strtoul(Source,__GetChar,&Offset,base,&ch);
            }
            break;
        }
        num++;

        if (ch == '\0')
            break;
    }
    if((*fmt == '%') && (*(fmt+1) == 'n'))
    {
        s32 *i = (s32 *)va_arg(args,s32*);
        *i = Offset;
    }

GotoErrorExit:
    if (SCANF_FROM_STDIN_DEV == Method) //把没有用完的数据还给缓冲区
    {
        if(s_StdinDevBufBytes != s_StdinDevposition)
        {
            if(s_StdinDevBuf[s_StdinDevBufBytes] == '\r')
                //最后一个'\r'是人为添加进去的，不需要还给设备缓冲区。
                Ring_RecedeRead(g_ptStdinDevBuf,s_StdinDevBufBytes-s_StdinDevposition-1,NULL);
            else
                Ring_RecedeRead(g_ptStdinDevBuf,s_StdinDevBufBytes-s_StdinDevposition,NULL);
            s_StdinDevBufBytes = 0;
            s_StdinDevposition = 0;
        }
    }

    return num;
}

s32 fscanf(FILE *fp,const char * fmt, ...)
{
    va_list args;
    s32 i;
    if (fp == NULL)
        return 0;
    else
    {
        va_start(args,fmt);
        if (((struct Object*)fp)->Type == RSC_FILE)
        {
            i = __vsnscanf((ptu32_t)fp,SCANF_FROM_FILE,fmt,args);
        }
        else if (stdin == fp)   //fgets函数将提供回显和行编辑功能
        {
            i = __vsnscanf(0,SCANF_FROM_STDIN_DEV,fmt,args);
        }
        else
        {
            i = __vsnscanf((ptu32_t)fp,SCANF_FROM_DEV,fmt,args);
        }
        va_end(args);
    }
    return i;
}

s32 sscanf(const char * buf, const char * fmt, ...)
{
    va_list args;
    s32 i;

    va_start(args,fmt);
    i = __vsnscanf((ptu32_t)buf,SCANF_FROM_STRING,fmt,args);
    va_end(args);

    return i;
}


s32 scanf(const char * fmt, ...)
{
    va_list args;
    s32 i;

    va_start(args,fmt);
    if (((struct Object*)stdin)->Type == RSC_FILE)
    {
        i = __vsnscanf((ptu32_t)stdin,SCANF_FROM_FILE,fmt,args);
    }
    else        //fgets函数将提供回显和行编辑功能，这里不管。
    {
        i = __vsnscanf(0,SCANF_FROM_STDIN_DEV,fmt,args);
    }
    va_end(args);

    return i;
}


