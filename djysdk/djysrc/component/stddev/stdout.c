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
//作者:  张前福.
//版本：V1.0.0
//文件描述: 标准输出函数
//其他说明:
//修订历史:
//2. 日期：2015-10-16
//   作者:  罗侍田.
//   新版本号: V1.1.0
//   修改说明: 全面修订，跟标准IO设计匹配
//1. 日期: 2013-11-06
//   作者:  张前福.
//   新版本号: V1.0.0
//   修改说明: 创建文件
//------------------------------------------------------
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "ctype.h"
#include "stdint.h"
#include "stddef.h"
#include "stdio.h"
#include "systime.h"
#include "driver.h"
#include "djyos.h"
#include <djyfs/file.h>
#define CN_BUF_LENGTH   0x40

#define ZEROPAD     1           /* pad with zero */
#define SIGN        2           /* unsigned/s32 */
#define PLUS        4           /* show plus */
#define SPACE       8           /* space if plus */
#define LEFT        16          /* left justified */
#define SMALL       32          /* Must be 32 == 0x20 */
#define SPECIAL     64          /* 0x */

#define PRINT_TO_STRING         0   //输出到string，用于sprintf等。
#define PRINT_TO_DIRECT         1   //直接输出到原始硬件，用于printk等。
#define PRINT_TO_FILE_OR_DEV    2   //输出到文件/设备，例如stdout/stderr，file
s32 skip_atoi(const char **s);

//----输出一个字符到文件-------------------------------------------------------
//功能：输出一个字符到文件，putc和fputc是等价的，搞笑的c标准
//参数：ch，待输出的字符
//      fp，输出目标文件或设备。
//返回：正确输出则返回被输出的字符，错误则返回EOF
//-----------------------------------------------------------------------------
s32 putc(s32 ch_para, FILE *fp)     //putc = fputc
{
    char  ch;
    
    ch = (char)ch_para;
    if (fp != NULL)
    {
        if (fp == StdNotInit)
        {
            if (PutStrDirect)
            {
                PutStrDirect((const char *)&ch,1);
                return ch;
            }
            else
                return EOF;

        }
        else
        {
            if(fwrite(&ch,1,1,fp) == 1)
                return ch;
            else
                return EOF;
        }
    }
    else
        return EOF;
}

//----输出一个字符串到文件-------------------------------------------------------
//功能：输出一个字符串到文件
//参数：str，待输出的字符串
//      fp，输出目标文件，如果是stdout或者stderr，则要判断是否设备。
//返回：正确输出则返回一个非负值，错误则返回EOF
//-----------------------------------------------------------------------------
s32 fputs(const char *str, FILE *fp)
{
    s32 len;
    if (fp != NULL)
    {
        len = strlen(str);
        if (fp == StdNotInit)
        {
            if (PutStrDirect)
            {
                PutStrDirect(str,len);
                return 1;
            }
            else
                return EOF;
        } 
        else
        {
            if(fwrite(str,len,1,fp) == len)
                return 1;
            else
                return EOF;
        }
    }
    else
        return EOF;
}

//----输出一个字符到stdout-----------------------------------------------------
//功能：输出一个字符到stdout，stdout可以是设备，也可以是文件
//参数：ch ，待输出的字符
//返回：正确输出则返回被输出的字符，错误则返回EOF
//-----------------------------------------------------------------------------
s32 putchar (s32 ch)
{
    return putc(ch,stdout);
}


//----输出一个字符串到stdout-----------------------------------------------------
//功能：输出一个字符串到stdout，stdout可以是设备，也可以是文件
//参数：str ，待输出的字符串
//返回：正确输出则返回一个非负值，错误则返回EOF
//-----------------------------------------------------------------------------
s32 puts (const char *str)
{
    return fputs(str,stdout);
}

u32 __div64_32(u64 *n, u32 base)
{
    u64 rem = *n;
    u64 b = base;
    u64 res, d = 1;
    u32  high = rem >> 32;

    /* Reduce the thing a bit first */
    res = 0;
    if (high >= base) {
        high /= base;
        res = (u64) high << 32;
        rem -= (u64) (high*base) << 32;
    }

    while ((u64 )b > 0 && b < rem) {
        b = b+b;
        d = d+d;
    }

    do {
        if (rem >= b) {
            rem -= b;
            res += d;
        }
        b >>= 1;
        d >>= 1;
    } while (d);

    *n = res;
    return rem;
}

/* Decimal conversion is by far the most typical, and is used
 * for /proc and /sys data. This directly impacts e.g. top performance
 * with many processes running. We optimize it for speed
 * using code from
 * http://www.cs.uiowa.edu/~jones/bcd/decimal.html
 * (with permission from the author, Douglas W. Jones). */

/* Formats correctly any integer in [0,99999].
 * Outputs from one to five digits depending on input.
 * On i386 gcc 4.1.2 -O2: ~250 bytes of code. */
static char *put_dec_trunc(char *buf, unsigned q)
{
    unsigned d3, d2, d1, d0;
    d1 = (q>>4) & 0xf;
    d2 = (q>>8) & 0xf;
    d3 = (q>>12);

    d0 = 6*(d3 + d2 + d1) + (q & 0xf);
    q = (d0 * 0xcd) >> 11;
    d0 = d0 - 10*q;
    *buf++ = d0 + '0'; /* least significant digit */
    d1 = q + 9*d3 + 5*d2 + d1;
    if (d1 != 0) {
        q = (d1 * 0xcd) >> 11;
        d1 = d1 - 10*q;
        *buf++ = d1 + '0'; /* next digit */

        d2 = q + 2*d2;
        if ((d2 != 0) || (d3 != 0)) {
            q = (d2 * 0xd) >> 7;
            d2 = d2 - 10*q;
            *buf++ = d2 + '0'; /* next digit */

            d3 = q + 4*d3;
            if (d3 != 0) {
                q = (d3 * 0xcd) >> 11;
                d3 = d3 - 10*q;
                *buf++ = d3 + '0';  /* next digit */
                if (q != 0)
                    *buf++ = q + '0'; /* most sign. digit */
            }
        }
    }
    return buf;
}
/* Same with if's removed. Always emits five digits */
static char *put_dec_full(char *buf, unsigned q)
{
    /* BTW, if q is in [0,9999], 8-bit ints will be enough, */
    /* but anyway, gcc produces better code with full-sized ints */
    unsigned d3, d2, d1, d0;
    d1 = (q>>4) & 0xf;
    d2 = (q>>8) & 0xf;
    d3 = (q>>12);

    /*
     * Possible ways to approx. divide by 10
     * gcc -O2 replaces multiply with shifts and adds
     * (x * 0xcd) >> 11: 11001101 - shorter code than * 0x67 (on i386)
     * (x * 0x67) >> 10:  1100111
     * (x * 0x34) >> 9:    110100 - same
     * (x * 0x1a) >> 8:     11010 - same
     * (x * 0x0d) >> 7:      1101 - same, shortest code (on i386)
     */

    d0 = 6*(d3 + d2 + d1) + (q & 0xf);
    q = (d0 * 0xcd) >> 11;
    d0 = d0 - 10*q;
    *buf++ = d0 + '0';
    d1 = q + 9*d3 + 5*d2 + d1;
        q = (d1 * 0xcd) >> 11;
        d1 = d1 - 10*q;
        *buf++ = d1 + '0';

        d2 = q + 2*d2;
            q = (d2 * 0xd) >> 7;
            d2 = d2 - 10*q;
            *buf++ = d2 + '0';

            d3 = q + 4*d3;
                q = (d3 * 0xcd) >> 11; /* - shorter code */
                /* q = (d3 * 0x67) >> 10; - would also work */
                d3 = d3 - 10*q;
                *buf++ = d3 + '0';
                    *buf++ = q + '0';
    return buf;
}
/* No inlining helps gcc to use registers better */
static  char *put_dec(char *buf, u64  num)
{
    while (1) {
        unsigned rem;
        if (num < 100000)
            return put_dec_trunc(buf, num);
//        rem = do_div(num, 100000);
        rem = num % 100000;
        num = num / 100000;
        buf = put_dec_full(buf, rem);
    }
}

// =============================================================================

static void forcdecpt(char *buffer)
{
  while (*buffer)
  {
    if (*buffer == '.') return;
    if (*buffer == 'e' || *buffer == 'E') break;
    buffer++;
  }
  if (*buffer)
  {
    int n = strlen(buffer);
    while (n > 0)
    {
      buffer[n + 1] = buffer[n];
      n--;
    }
    *buffer = '.';
  }
  else
  {
    *buffer++ = '.';
    *buffer = '/0';
  }
}
static void cropzeros(char *buffer)
{
  char *stop;
  while (*buffer && *buffer != '.') buffer++;
  if (*buffer++)
  {
    while (*buffer && *buffer != 'e' && *buffer != 'E') buffer++;
    stop = buffer--;
    while (*buffer == '0') buffer--;
    if (*buffer == '.') buffer--;
    while (*++buffer = *stop++);
  }
}
#include <stdlib.h>
char *ecvtbuf(double arg, int ndigits, int *decpt, int *sign, char *buf);
char *fcvtbuf(double arg, int ndigits, int *decpt, int *sign, char *buf);
static void cfltcvt(double value, char *buffer, char fmt, int precision)
{
  int decpt, sign, exp, pos;
  char *digits = NULL;
  char cvtbuf[80];
  int capexp = 0;
  int magnitude;
  if (fmt == 'G' || fmt == 'E')
  {
    capexp = 1;
    fmt += 'a' - 'A';
  }
  if (fmt == 'g')
  {
    digits = ecvtbuf(value, precision, &decpt, &sign, cvtbuf);
    magnitude = decpt - 1;
    if (magnitude < -4  ||  magnitude > precision - 1)
    {
      fmt = 'e';
      precision -= 1;
    }
    else
    {
      fmt = 'f';
      precision -= decpt;
    }
  }
  if (fmt == 'e')
  {
    digits = ecvtbuf(value, precision + 1, &decpt, &sign, cvtbuf);
    if (sign) *buffer++ = '-';
    *buffer++ = *digits;
    if (precision > 0) *buffer++ = '.';
    memcpy(buffer, digits + 1, precision);
    buffer += precision;
    *buffer++ = capexp ? 'E' : 'e';
    if (decpt == 0)
    {
      if (value == 0.0)
        exp = 0;
      else
        exp = -1;
    }
    else
      exp = decpt - 1;
    if (exp < 0)
    {
      *buffer++ = '-';
      exp = -exp;
    }
    else
      *buffer++ = '+';
//    buffer[2] = (exp % 10) + '0';
//    exp = exp / 10;
    buffer[1] = (exp % 10) + '0';
    exp = exp / 10;
    buffer[0] = (exp % 10) + '0';
    buffer += 2;
  }
  else if (fmt == 'f')
  {
    digits = fcvtbuf(value, precision, &decpt, &sign, cvtbuf);
    if (sign) *buffer++ = '-';
    if (*digits)
    {
      if (decpt <= 0)
      {
        *buffer++ = '0';
        *buffer++ = '.';
        for (pos = 0; pos < -decpt; pos++) *buffer++ = '0';
        while (*digits) *buffer++ = *digits++;
      }
      else
      {
        pos = 0;
        while (*digits)
        {
          if (pos++ == decpt) *buffer++ = '.';
          *buffer++ = *digits++;
        }
      }
    }
    else
    {
      *buffer++ = '0';
      if (precision > 0)
      {
        *buffer++ = '.';
        for (pos = 0; pos < precision; pos++) *buffer++ = '0';
      }
    }
  }
  *buffer = '\0';
}




static u32 flt(char *TempBuf, ptu32_t Target, s32 Size, double num,
        int size, int precision, char fmt,int flags,u32 position,
        u32 (*PushChar)(char *TempBuf,ptu32_t Target, s32 Size, char ch,u32 position))
{
  char tmp[80];
  char c, sign;
  int n, i;

  memset(tmp,0x00,80);
  // Left align means no zero padding
  if (flags & LEFT) flags &= ~ZEROPAD;
  // Determine padding and sign char
  c = (flags & ZEROPAD) ? '0' : ' ';
  sign = 0;
  if (flags & SIGN)
  {
    if (num < 0.0)
    {
      sign = '-';
      num = -num;
      size--;
    }
    else if (flags & PLUS)
    {
      sign = '+';
      size--;
    }
    else if (flags & SPACE)
    {
      sign = ' ';
      size--;
    }
  }
  // Compute the precision value
  if (precision < 0)
    precision = 6; // Default precision: 6
  else if (precision == 0 && fmt == 'g')
    precision = 1; // ANSI specified
  // Convert floating point number to text
  cfltcvt(num, tmp, fmt, precision);
  // '#' and precision == 0 means force a decimal point
  if ((flags & SPECIAL) && precision == 0) forcdecpt(tmp);
  // 'g' format means crop zero unless '#' given
  if (fmt == 'g' && !(flags & SPECIAL)) cropzeros(tmp);
  n = strlen(tmp);
  // Output number with alignment and padding
  size -= n;
  if (!(flags & (ZEROPAD | LEFT)))
  {
      while(size-- > 0)
      {
          position = PushChar(TempBuf,Target, Size,' ',position);
      }
  }
  if (sign)
  {
      position = PushChar(TempBuf,Target, Size,sign,position);
  }
  if (!(flags & LEFT))
  {
      while (size-- > 0)
      {
          position = PushChar(TempBuf,Target, Size,c,position);
      }
  }
  for (i = 0; i < n; i++)
  {
      position = PushChar(TempBuf,Target, Size,tmp[i],position);
  }
  while (size-- > 0)
  {
      position = PushChar(TempBuf,Target, Size,' ',position);
  }
  return position;
}

//==============================================================================

static u32 number(char *TempBuf,ptu32_t Target, s32 Size, u64  num,\
        s32 base, s32 size, s32 precision, s32 type, u32 position,\
        u32 (*PushChar)(char *TempBuf,ptu32_t Target, s32 Size, char ch,u32 position))
{
    /* we are called with base 8, 10 or 16, only, thus don't need "G..."  */
    static const char digits[ ] = "0123456789ABCDEF";

    char tmp[66];
    char sign;
    char locase;
    s32 need_pfx = ((type & SPECIAL) && base != 10);
    s32 i;

    /* locase = 0 or 0x20. ORing digits or letters with 'locase'
     * produces same digits or (maybe lowercased) letters */
    locase = (type & SMALL);
    if (type & LEFT)
        type &= ~ZEROPAD;
    sign = 0;
    if (type & SIGN) {
        if ((s64) num < 0) {
            sign = '-';
            num = -(s64) num;
            size--;
        } else if (type & PLUS) {
            sign = '+';
            size--;
        } else if (type & SPACE) {
            sign = ' ';
            size--;
        }
    }
    if (need_pfx) {
        size--;
        if (base == 16)
            size--;
    }

    /* generate full string in tmp[], in reverse order */
    i = 0;
    if (num == 0)
        tmp[i++] = '0';
    /* Generic code, for any base:
    else do {
        tmp[i++] = (digits[do_div(num,base)] | locase);
    } while (num != 0);
    */
    else if (base != 10) { /* 8 or 16 */
        s32 mask = base - 1;
        s32 shift = 3;

        if (base == 16)
            shift = 4;

        do {
            tmp[i++] = (digits[((unsigned char)num) & mask]
                    | locase);
            num >>= shift;
        } while (num);
    } else { /* base 10 */
        i = put_dec(tmp, num) - (char *)tmp;
    }

    /* printing 100 using %2d gives "100", not "00" */
    if (i > precision)
        precision = i;
    /* leading space padding */
    size -= precision;
    if (!(type & (ZEROPAD + LEFT))) {
        while (--size >= 0)
            position = PushChar(TempBuf,Target, Size,' ',position);
//          position = ADDDCH_With_push(buf, ' ', position, strputfunc);
    }
    /* sign */
    if (sign)
        position = PushChar(TempBuf,Target, Size,sign,position);
//      position = ADDDCH_With_push(buf, sign, position, strputfunc);
    /* "0x" / "0" prefix */
    if (need_pfx) {
        position = PushChar(TempBuf,Target, Size,'0',position);
//      position = ADDDCH_With_push(buf, '0', position, strputfunc);
        if (base == 16)
            position = PushChar(TempBuf,Target, Size, ' ',position);
//          position = ADDDCH_With_push(buf, 'X' | locase, position, strputfunc);
    }
    /* zero or space padding */
    if (!(type & LEFT)) {
        char c = (type & ZEROPAD) ? '0' : ' ';

        while (--size >= 0)
            position = PushChar(TempBuf,Target, Size, c,position);
//          position = ADDDCH_With_push(buf, c, position, strputfunc);
    }
    /* hmm even more zero padding? */
    while (i <= --precision)
        position = PushChar(TempBuf,Target, Size, '0',position);
//      position = ADDDCH_With_push(buf, '0', position, strputfunc);
    /* actual digits of result */
    while (--i >= 0)
        position = PushChar(TempBuf,Target, Size, tmp[i],position);
//      position = ADDDCH_With_push(buf, tmp[i], position, strputfunc);
    /* trailing space padding */
    while (--size >= 0)
        position = PushChar(TempBuf,Target, Size, ' ',position);
//      position = ADDDCH_With_push(buf, ' ', position, strputfunc);
    return position;
}

static u32 string(char *TempBuf,ptu32_t Target, s32 Size, char *s, s32 field_width,
        s32 precision, s32 flags, u32 position,
        u32 (*PushChar)(char *TempBuf,ptu32_t Target, s32 Size, char ch,u32 position))
{
    s32 len, i;

    if (s == NULL)
        s = "<NULL>";

    len = strnlen(s, precision);

    if (!(flags & LEFT))
        while (len < field_width--)
            position = PushChar(TempBuf,Target, Size, ' ',position);
//          position = ADDDCH_With_push(buf, ' ', position, strputfunc);
    for (i = 0; i < len; ++i)
        position = PushChar(TempBuf,Target, Size, *s++,position);
//      position = ADDDCH_With_push(buf, *s++, position, strputfunc);
    while (len < field_width--)
        position = PushChar(TempBuf,Target, Size, ' ',position);
//      position = ADDDCH_With_push(buf, ' ', position, strputfunc);
    return position;
}
static u32 pointer(char *TempBuf,ptu32_t Target, s32 Size, void *ptr,
        s32 field_width, s32 precision, s32 flags, u32 position,
        u32 (*PushChar)(char *TempBuf,ptu32_t Target, s32 Size, char ch,u32 position))
{
    flags |= SMALL;
    if (field_width == -1) {
        field_width = 2*sizeof(void *);
        flags |= ZEROPAD;
    }
    return number(TempBuf,Target, Size, (u32)ptr, 16, field_width,
              precision, flags,position, PushChar);
}

//----把字符压入字符串中-------------------------------------------------------
//功能：把一个字符推入字符串中，若超出长度，则忽略。
//参数：TempBuf，为函数原型一致而填充的参数，无意义
//      Target,接收字符串的缓冲区地址。
//      Size，缓冲区尺寸，含末端'\0'
//      ch，压入的字符
//      Position，Buf的偏移量
//返回：Position的新位置。
//-----------------------------------------------------------------------------
u32 __PushCharToString(char *TempBuf,ptu32_t Target,s32 Size,const char ch,u32 Position)
{
    if(Position < (Size-1))
    {
        *((char*)Target + Position) = ch;
        Position ++;
    }
    return Position;
}

//----把字缓冲后直接输出------------------------------------------------------
//功能：把一个字符先缓冲到字符串TempBuf中，缓冲满了后，直接输出到硬件，例如串口。
//参数：TempBuf，暂存缓冲区，长度是CN_BUF_LENGTH
//      Target,无意义。
//      Size，无意义
//      ch，压入的字符
//      Position，Buf的偏移量
//返回：Position的新位置。
//-----------------------------------------------------------------------------
u32 __PushCharDirect(char *TempBuf,ptu32_t Target,s32 Size,const char ch,u32 Position)
{
    if(Position >= CN_BUF_LENGTH)
    {
        PutStrDirect((const char *)TempBuf,CN_BUF_LENGTH);
        Position = 1;
        *TempBuf = ch;
    }
    else
    {
        *(TempBuf + Position) = ch;
        Position ++;
    }
    return Position;
}

//----把字符压入文件/设备中--------------------------------------------------
//功能：把一个字符文件/设备对象中，先缓冲到字符串TempBuf中，缓冲满了后，输出
//      到文件/设备，本函数用于printf/fprintf/vprintf/vfprintf等。
//      djyos对标准c的stdout/stdin/stderr做了扩展，标准c中，它们只能是FILE *
//      类型，而djyos中，它们也允许是tagDevHandle，这样，即使在文件系统被裁掉
//      的场合，也可以正常使用printf等函数。
//参数：TempBuf，为函数原型一致而填充的参数，无意义
//      Target,接收字符串的文件/设备指针。
//      Size，缓冲区尺寸，含末端'\0'
//      ch，压入的字符
//      Position，Buf的偏移量
//返回：Position的新位置。
//-----------------------------------------------------------------------------
u32 __PushCharToFileDev(char *TempBuf,ptu32_t Target,s32 Res2,const char ch,u32 Position)
{
    if(Position >= CN_BUF_LENGTH)
    {
        fwrite(TempBuf,CN_BUF_LENGTH,1,(FILE*)Target);
        Position = 1;
        *TempBuf = ch;
    }
    else
    {
        *(TempBuf + Position) = ch;
        Position ++;
    }
    return Position;
}

//---------------------------------------------------------------------------
//功能: 按照格式字符串，生成输出字符串，并按照Method参数指定的方式，输出字符串。
//参数: TempBuf，字符串转换使用的临时缓冲区，长度是CN_BUF_LENGTH，如果
//          Method==PRINT_TO_STRING则忽略本参数，可传入NULL
//      Target，这是一个多用途的变量，设定字符串输出目的地的属性，依据Method
//          不同，其含义不同。
//          Method==PRINT_TO_STRING，Target是字符串指针
//          Method==PRINT_TO_DIRECT，Target无意义
//          Method==PRINT_TO_FILE_OR_DEV，Target是文件或设备指针
//      Size，如果Target是字符串指针，Size限定其长度，-1表示不限定长度
//      Method，输出方法，PRINT_TO_STRING等常量之一
//      fmt，格式字符串
//      args，可变参数的参数列表
//返回: 转换结果字符串长度
//说明：目前没有加入浮点打印--TODO
//-----------------------------------------------------------------------------
static s32 __vsnprintf(char *TempBuf,ptu32_t Target, s32 Size,
                       u32 Method, const char *fmt,va_list args)
{
    u32 (*PushChar)(char *TempBuf,ptu32_t Target, s32 Size, char ch,u32 position);
    u64 num;
    u32 position;
    s32 base;
    s32 flags;          //显示数字时的标志

    s32 field_width;    //显示的最小宽度
    s32 precision;      //显示精度，即数字的最大位数和字符串的最大长度，超出部分截掉。
    s32 qualifier;      //控制数字的长度
                        // 'h', 'l', or 'L' for integer fields
                        // 'z' support added 23/7/1999 S.H.
                        // 'z' changed to 'Z' --davidm 1/25/99
                        // 't' added for ptrdiff_t
    position = 0;
    if(Djy_IsMultiEventStarted() == false)//如果调度并未开始,采用直接发送方式
    {
        if (Method == PRINT_TO_FILE_OR_DEV)
            Method = PRINT_TO_DIRECT;
    }
    switch (Method)
    {
        case PRINT_TO_STRING:
            PushChar = __PushCharToString;
            break;
        case PRINT_TO_DIRECT:
            PushChar = __PushCharDirect;
            break;
        case PRINT_TO_FILE_OR_DEV:
            PushChar = __PushCharToFileDev;
            break;
        default:
            PushChar = __PushCharDirect;
            break;
    }

    for (; *fmt ; ++fmt)
    {
        if (*fmt != '%') {
            position = PushChar(TempBuf,Target, Size,*fmt,position);
//            position = ADDDCH_With_push(Target, *fmt, position,Method);
            continue;
        }

        /* process flags */
        flags = 0;
repeat:
            ++fmt;        /* this also skips first '%' */
            switch (*fmt)
            {
            case '-':
                flags |= LEFT;
                goto repeat;
            case '+':
                flags |= PLUS;
                goto repeat;
            case ' ':
                flags |= SPACE;
                goto repeat;
            case '#':
                flags |= SPECIAL;
                goto repeat;
            case '0':
                flags |= ZEROPAD;
                goto repeat;
            }

        /* get field width */
        field_width = -1;
        if (isdigit((s32)*fmt))
            field_width = skip_atoi(&fmt);
        else if (*fmt == '*')
        {
            ++fmt;
            /* it's the next argument */
            field_width = va_arg(args, s32);
            if (field_width < 0) {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        /* get the precision */
        precision = -1;
        if (*fmt == '.')
        {
            ++fmt;
            if (isdigit((s32)*fmt))
                precision = skip_atoi(&fmt);
            else if (*fmt == '*') {
                ++fmt;
                /* it's the next argument */
                precision = va_arg(args, s32);
            }
            if (precision < 0)
                precision = 0;
        }

        /* get the conversion qualifier */
        qualifier = -1;
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' ||
            *fmt == 'Z' || *fmt == 'z' || *fmt == 't')
        {
            qualifier = *fmt;
            ++fmt;
            if (qualifier == 'l' && *fmt == 'l') {
                qualifier = 'L';
                ++fmt;
            }
        }

        /* default base */
        base = 10;

        switch (*fmt)
        {
        case 'c':
            if (!(flags & LEFT))
            {
                while (--field_width > 0)
                    position = PushChar(TempBuf,Target, Size, ' ',position);
//                  position = ADDDCH_With_push(Target, ' ', position, Method);
            }
            position = PushChar(TempBuf,Target, Size, (unsigned char) va_arg(args, s32),position);
//          position = ADDDCH_With_push(Target, (unsigned char) va_arg(args, s32),position, Method);
            while (--field_width > 0)
                position = PushChar(TempBuf,Target, Size, ' ',position);
//              position = ADDDCH_With_push(Target, ' ', position, Method);
            continue;

        case 's':
            position = string(TempBuf,Target, Size, va_arg(args, char *),
                     field_width, precision, flags, position, PushChar);
            continue;

        case 'p':
            position = pointer(TempBuf,Target, Size,va_arg(args, void *),
                    field_width, precision, flags, position, PushChar);
            /* Skip all alphanumeric pointer suffixes */
            while (isalnum((s32)fmt[1]))
                fmt++;
            continue;

        case 'n':
            if (qualifier == 'l')
            {
                s32 *ip = va_arg(args, s32 *);
                *ip = position;
            } else {
                s32 *ip = va_arg(args, s32 *);
                *ip = position;
            }
            continue;

        case '%':
            position = PushChar(TempBuf,Target, Size,'%',position);
//          position = ADDDCH_With_push(Target, '%', position, Method);
            continue;

        /* integer number formats - set up the flags and "break" */
        case 'o':
            base = 8;
            break;

        case 'x':
            flags |= SMALL;
        case 'X':
            base = 16;
            break;

        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':
            break;

        case 'E':
        case 'G':
        case 'e':
        case 'f':
        case 'g':
        position = flt(TempBuf,Target, Size, va_arg(args, double), field_width, precision,
                         *fmt, flags | SIGN, position, PushChar);
          continue;

        default:
            position = PushChar(TempBuf,Target, Size,'%',position);
//          position = ADDDCH_With_push(Target, '%', position, Method);
            if (*fmt)
                position = PushChar(TempBuf,Target, Size,*fmt,position);
//              position = ADDDCH_With_push(Target, *fmt, position, Method);
            else
                --fmt;
            continue;
        }
        if (qualifier == 'L')  /* "quad" for 64 bit variables */
            num = va_arg(args, u64);
        else if (qualifier == 'l')
        {
            num = va_arg(args, u32);
            if (flags & SIGN)
                num = (s32) num;
        } else if (qualifier == 'Z' || qualifier == 'z')
        {
            num = va_arg(args, s32);
        } else if (qualifier == 't')
        {
            num = va_arg(args, s32);
        } else if (qualifier == 'h')
        {
            num = (u16) va_arg(args, s32);
            if (flags & SIGN)
                num = (s16) num;
        } else
        {
            num = va_arg(args, u32);
            if (flags & SIGN)
                num = (s32) num;
        }
        position = number(TempBuf,Target, Size, num, base, field_width, precision,
                 flags, position, PushChar);
    }

    if (position != 0)
    {
        switch (Method)
        {
            case PRINT_TO_STRING:
                *(char*)(Target + position) = '\0';
                break;
            case PRINT_TO_DIRECT:
                PutStrDirect((const char *)TempBuf,position);
                break;
            case PRINT_TO_FILE_OR_DEV:
                fwrite((const void *)TempBuf,position,1,(FILE*)Target);
                break;
            default:
                PutStrDirect((const char *)TempBuf,position);
                break;
        }
    }
    return  position;
}
s32 vsprintf(char *buf, const char *fmt, va_list args)
{
    return __vsnprintf(NULL,(ptu32_t)buf, CN_LIMIT_SINT32,PRINT_TO_STRING,fmt, args);
}
s32 vsnprintf(char *buf, size_t n, const char *fmt, va_list args)
{

    return __vsnprintf(NULL,(ptu32_t)buf, n,PRINT_TO_STRING,fmt, args);
}

s32 vprintf (const char *fmt, va_list args)
{
    s32 i;
    char TempBuf[CN_BUF_LENGTH];

    if (stdout == StdNotInit)
        i = __vsnprintf (TempBuf,(ptu32_t)NULL,0,PRINT_TO_DIRECT, fmt, args);
    else
        i =  __vsnprintf (TempBuf,(ptu32_t)stdout, 0,PRINT_TO_FILE_OR_DEV,fmt, args);
    return i;
}

//----格式打印-----------------------------------------------------------------
//功能: 按格式打印字符串，与C库函数类似，不使用动态分配，一次打印的长度限制为255
//      个单字节字符，或等效多字节字符。
//参数: const char *fmt, ...
//返回: 转换的字符串长度
//说明：该打印函数经过终端设备获得服务，较为复杂，打印有规律整齐。因此代码成熟
//      阶段请使用该打印函数，谢谢合作，在特殊情况下，希望直接输出，可使用
//      djy_printk函数。
//-----------------------------------------------------------------------------
s32 printf (const char *fmt, ...)
{
    va_list args;
    u32  i;
    char TempBuf[CN_BUF_LENGTH];

    va_start (args, fmt);

    if (stdout == StdNotInit)
        i = __vsnprintf (TempBuf,(ptu32_t)NULL,0,PRINT_TO_DIRECT, fmt, args);
    else
        i =  __vsnprintf (TempBuf,(ptu32_t)stdout, 0,PRINT_TO_FILE_OR_DEV,fmt, args);
    va_end (args);

    return i;
}

//----格式化字符串--------------------------------------------------------------
//功能: 格式化字符串，和C库一样，按照一定的格式来格式化字符串
//参数: const char *fmt, ...
//返回: 转换的字符串长度
//说明：
//-----------------------------------------------------------------------------
s32 sprintf(char *buf, const char *fmt, ...)
{
    va_list args;
    u32  i;

    va_start (args, fmt);

    i = __vsnprintf (NULL,(ptu32_t)buf, CN_LIMIT_SINT32,PRINT_TO_STRING,fmt, args);
    va_end (args);
    return i;
}
#if 0
s32 snprintf(char *buf,int n, const char *fmt, ...)//todo: 参数类型不对(与libc库不一致)
#else
s32 snprintf(char *buf,u32 n, const char *fmt, ...)
#endif
{
    va_list args;
    u32  i;

    va_start (args, fmt);
    i = __vsnprintf (NULL,(ptu32_t)buf, n, PRINT_TO_STRING,fmt, args);
    va_end (args);
    return i;
}

//----格式化字符串--------------------------------------------------------------
//功能: 格式化字符串，和C库一样，按照一定的格式来格式化字符串
//参数: const char *fmt, ...
//返回: 写入的字符串长度
//说明：
//-----------------------------------------------------------------------------
s32 fprintf(FILE *fp, const char *fmt, ...)
{
    va_list args;
    u32  i;
    char TempBuf[CN_BUF_LENGTH];
    if (fp == NULL)
    {
        Djy_SaveLastError(EN_FS_READFILE_EOF);
        return -1;
    }
    va_start (args, fmt);
    if (fp == StdNotInit)
        i = __vsnprintf (TempBuf,(ptu32_t)NULL,0,PRINT_TO_DIRECT, fmt, args);
    else
        i = __vsnprintf (TempBuf,(ptu32_t)fp, 0, PRINT_TO_FILE_OR_DEV, fmt, args);
    va_end (args);

    return i;
}
//----格式打印-----------------------------------------------------------------
//功能: 按格式打印字符串，与C库函数类似，不使用动态分配，一次打印的长度限制为
//      cn_buf_length个单字节字符，或等效多字节字符。
//参数: const char *fmt, ...
//返回: 转换的字符串长度
//说明：和printf不一样，它直接输出到终端设备上。
//-----------------------------------------------------------------------------
s32 printk (const char *fmt, ...)
{
    va_list args;
    u32  i;
    char TempBuf[CN_BUF_LENGTH];
    va_start (args, fmt);

    i = __vsnprintf (TempBuf,(ptu32_t)NULL,0,PRINT_TO_DIRECT, fmt, args);
    va_end (args);

    return i;
}

