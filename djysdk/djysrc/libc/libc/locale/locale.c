/*
FUNCTION
<<setlocale>>, <<localeconv>>---select or query locale

INDEX
    setlocale
INDEX
    localeconv
INDEX
    _setlocale_r
INDEX
    localeconv

ANSI_SYNOPSIS
    #include <locale.h>
    char *setlocale(int <[category]>, const char *<[locale]>);
    lconv *localeconv(void);

    char *_setlocale_r(void *<[reent]>,
                        int <[category]>, const char *<[locale]>);
    lconv *localeconv(void *<[reent]>);

TRAD_SYNOPSIS
    #include <locale.h>
    char *setlocale(<[category]>, <[locale]>)
    int <[category]>;
    char *<[locale]>;

    lconv *localeconv();

    char *_setlocale_r(<[reent]>, <[category]>, <[locale]>)
    char *<[reent]>;
    int <[category]>;
    char *<[locale]>;

    lconv *localeconv(<[reent]>);
    char *<[reent]>;

DESCRIPTION
<<setlocale>> is the facility defined by ANSI C to condition the
execution environment for international collating and formatting
information; <<localeconv>> reports on the settings of the current
locale.

This is a minimal implementation, supporting only the required <<"POSIX">>
and <<"C">> values for <[locale]>; strings representing other locales are not
honored unless _MB_CAPABLE is defined.

If _MB_CAPABLE is defined, POSIX locale strings are allowed, following
the form

  language[_TERRITORY][.charset][@@modifier]

<<"language">> is a two character string per ISO 639, or, if not available
for a given language, a three character string per ISO 639-3.
<<"TERRITORY">> is a country code per ISO 3166.  For <<"charset">> and
<<"modifier">> see below.

Additionally to the POSIX specifier, the following extension is supported
for backward compatibility with older implementations using newlib:
<<"C-charset">>.
Instead of <<"C-">>, you can also specify <<"C.">>.  Both variations allow
to specify language neutral locales while using other charsets than ASCII,
for instance <<"C.UTF-8">>, which keeps all settings as in the C locale,
but uses the UTF-8 charset.

The following charsets are recognized:
<<"UTF-8">>, <<"JIS">>, <<"EUCJP">>, <<"SJIS">>, <<"KOI8-R">>, <<"KOI8-U">>,
<<"GEORGIAN-PS">>, <<"PT154">>, <<"TIS-620">>, <<"ISO-8859-x">> with
1 <= x <= 16, or <<"CPxxx">> with xxx in [437, 720, 737, 775, 850, 852, 855,
857, 858, 862, 866, 874, 932, 1125, 1250, 1251, 1252, 1253, 1254, 1255, 1256,
1257, 1258].

Charsets are case insensitive.  For instance, <<"EUCJP">> and <<"eucJP">>
are equivalent.  Charset names with dashes can also be written without
dashes, as in <<"UTF8">>, <<"iso88591">> or <<"koi8r">>.  <<"EUCJP">> and
<<"EUCKR">> are also recognized with dash, <<"EUC-JP">> and <<"EUC-KR">>.

Full support for all of the above charsets requires that newlib has been
build with multibyte support and support for all ISO and Windows Codepage.
Otherwise all singlebyte charsets are simply mapped to ASCII.  Right now,
only newlib for Cygwin is built with full charset support by default.
Under Cygwin, this implementation additionally supports the charsets
<<"GBK">>, <<"GB2312">>, <<"eucCN">>, <<"eucKR">>, and <<"Big5">>.  Cygwin
does not support <<"JIS">>.

Cygwin additionally supports locales from the file
/usr/share/locale/locale.alias.

(<<"">> is also accepted; if given, the settings are read from the
corresponding LC_* environment variables and $LANG according to POSIX rules.

This implementation also supports the modifier <<"cjknarrow">>, which
affects how the functions <<wcwidth>> and <<wcswidth>> handle characters
from the "CJK Ambiguous Width" category of characters described at
http://www.unicode.org/reports/tr11/#Ambiguous. These characters have a width
of 1 for singlebyte charsets and a width of 2 for multibyte charsets
other than UTF-8. For UTF-8, their width depends on the language specifier:
it is 2 for <<"zh">> (Chinese), <<"ja">> (Japanese), and <<"ko">> (Korean),
and 1 for everything else. Specifying <<"cjknarrow">> forces a width of 1,
independent of charset and language.

If you use <<NULL>> as the <[locale]> argument, <<setlocale>> returns a
pointer to the string representing the current locale.  The acceptable
values for <[category]> are defined in `<<locale.h>>' as macros
beginning with <<"LC_">>.

<<localeconv>> returns a pointer to a structure (also defined in
`<<locale.h>>') describing the locale-specific conventions currently
in effect.

<<localeconv>> and <<_setlocale_r>> are reentrant versions of
<<localeconv>> and <<setlocale>> respectively.  The extra argument
<[reent]> is a pointer to a reentrancy structure.

RETURNS
A successful call to <<setlocale>> returns a pointer to a string
associated with the specified category for the new locale.  The string
returned by <<setlocale>> is such that a subsequent call using that
string will restore that category (or all categories in case of LC_ALL),
to that state.  The application shall not modify the string returned
which may be overwritten by a subsequent call to <<setlocale>>.
On error, <<setlocale>> returns <<NULL>>.

<<localeconv>> returns a pointer to a structure of type <<lconv>>,
which describes the formatting and collating conventions in effect (in
this implementation, always those of the C locale).

PORTABILITY
ANSI C requires <<setlocale>>, but the only locale required across all
implementations is the C locale.

NOTES
There is no ISO-8859-12 codepage.  It's also refused by this implementation.

No supporting OS subroutines are required.
*/

/* Parts of this code are originally taken from FreeBSD. */
/*
 * Copyright (c) 1996 - 2002 FreeBSD Project
 * Copyright (c) 1991, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Paul Borman at Krystal Technologies.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <errno.h>
#include <locale.h>
#include <string.h>
#include <limits.h>

#include <stdlib.h>
#include <wchar.h>
#include "lmonetary.h"
#include "lnumeric.h"
#include "lctype.h"
#include "timelocal.h"
#include "djyos.h"

#define _LC_LAST      7
#define ENCODING_LEN 31

int __mb_cur_max = 6;

int __nlocale_changed = 0;
int __mlocale_changed = 0;
char *_PathLocale = NULL;

static
struct lconv lconv =
{
  ".", "", "", "", "", "", "", "", "", "",
  CHAR_MAX, CHAR_MAX, CHAR_MAX, CHAR_MAX,
  CHAR_MAX, CHAR_MAX, CHAR_MAX, CHAR_MAX,
  CHAR_MAX, CHAR_MAX, CHAR_MAX, CHAR_MAX,
  CHAR_MAX, CHAR_MAX
};

/*
 * Category names for getenv()
 */
static char *categories[_LC_LAST] = {
  "LC_ALL",
  "LC_COLLATE",
  "LC_CTYPE",
  "LC_MONETARY",
  "LC_NUMERIC",
  "LC_TIME",
  "LC_MESSAGES",
};

/*
 * Default locale per POSIX.  Can be overridden on a per-target base.
 */
#ifndef DEFAULT_LOCALE
#define DEFAULT_LOCALE  "C"
#endif
/*
 * This variable can be changed by any outside mechanism.  This allows,
 * for instance, to load the default locale from a file.
 */
char __default_locale[ENCODING_LEN + 1] = DEFAULT_LOCALE;

/*
 * Current locales for each category
 */
static char current_categories[_LC_LAST][ENCODING_LEN + 1] = {
    "C",
    "C",
#ifdef __CYGWIN__ /* Cygwin starts with LC_CTYPE set to "C.UTF-8". */
    "C.UTF-8",
#else
    "C",
#endif
    "C",
    "C",
    "C",
    "C",
};

/*
 * The locales we are going to try and load
 */
static char new_categories[_LC_LAST][ENCODING_LEN + 1];
static char saved_categories[_LC_LAST][ENCODING_LEN + 1];

static char current_locale_string[_LC_LAST * (ENCODING_LEN + 1/*"/"*/ + 1)];
static char *currentlocale(void);
static char *loadlocale(int);
static const char *__get_locale_env( int);


static char lc_ctype_charset[ENCODING_LEN + 1] = "ASCII";
static char lc_message_charset[ENCODING_LEN + 1] = "ASCII";
static int lc_ctype_cjk_lang = 0;

char * _setlocale_r(int category, _CONST char *locale)
{
  int i, j, len, saverr;
  const char *env, *r;

  if (category < LC_ALL || category >= _LC_LAST)
    {
      Djy_SaveLastError(EINVAL);
      return NULL;
    }

  if (locale == NULL)
    return category != LC_ALL ? current_categories[category] : currentlocale();

  /*
   * Default to the current locale for everything.
   */
  for (i = 1; i < _LC_LAST; ++i)
    strcpy (new_categories[i], current_categories[i]);

  /*
   * Now go fill up new_categories from the locale argument
   */
  if (!*locale)
  {
    if (category == LC_ALL)
    {
      for (i = 1; i < _LC_LAST; ++i)
        {
          env = __get_locale_env ( i);
          if (strlen (env) > ENCODING_LEN)
        {
          Djy_SaveLastError(EINVAL);
          return NULL;
        }
          strcpy (new_categories[i], env);
        }
    }
    else
    {
      env = __get_locale_env (category);
      if (strlen (env) > ENCODING_LEN)
        {
          Djy_SaveLastError(EINVAL);
          return NULL;
        }
      strcpy (new_categories[category], env);
    }
  }
  else if (category != LC_ALL)
  {
    if (strlen (locale) > ENCODING_LEN)
  {
    Djy_SaveLastError(EINVAL);
    return NULL;
  }
    strcpy (new_categories[category], locale);
  }
  else
    {
      if ((r = strchr (locale, '/')) == NULL)
    {
      if (strlen (locale) > ENCODING_LEN)
        {
          Djy_SaveLastError(EINVAL);
          return NULL;
        }
      for (i = 1; i < _LC_LAST; ++i)
        strcpy (new_categories[i], locale);
    }
      else
    {
      for (i = 1; r[1] == '/'; ++r)
        ;
      if (!r[1])
        {
          Djy_SaveLastError(EINVAL);
          return NULL;  /* Hmm, just slashes... */
        }
      do
        {
          if (i == _LC_LAST)
        break;  /* Too many slashes... */
          if ((len = r - locale) > ENCODING_LEN)
        {
          Djy_SaveLastError(EINVAL);
          return NULL;
        }
          strlcpy (new_categories[i], locale, len + 1);
          i++;
          while (*r == '/')
        r++;
          locale = r;
          while (*r && *r != '/')
        r++;
        }
      while (*locale);
      while (i < _LC_LAST)
        {
          strcpy (new_categories[i], new_categories[i-1]);
          i++;
        }
    }
    }

  if (category != LC_ALL)
    return loadlocale (category);

  for (i = 1; i < _LC_LAST; ++i)
    {
      strcpy (saved_categories[i], current_categories[i]);
      if (loadlocale (i) == NULL)
    {
      saverr = g_ptEventRunning->error_no;
      for (j = 1; j < i; j++)
        {
          strcpy (new_categories[j], saved_categories[j]);
          if (loadlocale (j) == NULL)
        {
          strcpy (new_categories[j], "C");
          loadlocale (j);
        }
        }
      Djy_SaveLastError(saverr);
      return NULL;
    }
    }
  return currentlocale ();
}

static char *currentlocale()
{
        int i;

        (void)strcpy(current_locale_string, current_categories[1]);

        for (i = 2; i < _LC_LAST; ++i)
                if (strcmp(current_categories[1], current_categories[i])) {
                        for (i = 2; i < _LC_LAST; ++i) {
                                (void)strcat(current_locale_string, "/");
                                (void)strcat(current_locale_string,
                                             current_categories[i]);
                        }
                        break;
                }
        return (current_locale_string);
}


extern void __set_ctype (const char *charset);

static char *loadlocale(int category)
{
   return NULL;
}

static const char *__get_locale_env(int category)
{
  return categories[category];
#if 0   //lst 待重新整理国际化设计。
  const char *env;

  /* 1. check LC_ALL. */
  env = _getenv_r (p, categories[0]);

  /* 2. check LC_* */
  if (env == NULL || !*env)
    env = _getenv_r (p, categories[category]);

  /* 3. check LANG */
  if (env == NULL || !*env)
    env = _getenv_r (p, "LANG");

  /* 4. if none is set, fall to default locale */
  if (env == NULL || !*env)
    env = __default_locale;

  return env;
#endif
}

char *
_DEFUN_VOID(__locale_charset)
{
#if 0//def __HAVE_LOCALE_INFO__
  return __get_current_ctype_locale ()->codeset;
#else
  return lc_ctype_charset;
#endif
}

int
_DEFUN_VOID(__locale_mb_cur_max)
{
#if 0//def __HAVE_LOCALE_INFO__
  return __get_current_ctype_locale ()->mb_cur_max[0];
#else
  return __mb_cur_max;
#endif
}


char *
_DEFUN_VOID(__locale_msgcharset)
{
#ifdef __HAVE_LOCALE_INFO__
  return (char *) __get_current_messages_locale ()->codeset;
#else
  return lc_message_charset;
#endif
}

int __locale_cjk_lang(void)
{
  return lc_ctype_cjk_lang;
}

struct lconv *localeconv(void)
{
#ifdef __HAVE_LOCALE_INFO__
  if (__nlocale_changed)
    {
      struct lc_numeric_T *n = __get_current_numeric_locale ();
      lconv.decimal_point = (char *) n->decimal_point;
      lconv.thousands_sep = (char *) n->thousands_sep;
      lconv.grouping = (char *) n->grouping;
      __nlocale_changed = 0;
    }
  if (__mlocale_changed)
    {
      struct lc_monetary_T *m = __get_current_monetary_locale ();
      lconv.int_curr_symbol = (char *) m->int_curr_symbol;
      lconv.currency_symbol = (char *) m->currency_symbol;
      lconv.mon_decimal_point = (char *) m->mon_decimal_point;
      lconv.mon_thousands_sep = (char *) m->mon_thousands_sep;
      lconv.mon_grouping = (char *) m->mon_grouping;
      lconv.positive_sign = (char *) m->positive_sign;
      lconv.negative_sign = (char *) m->negative_sign;
      lconv.int_frac_digits = m->int_frac_digits[0];
      lconv.frac_digits = m->frac_digits[0];
      lconv.p_cs_precedes = m->p_cs_precedes[0];
      lconv.p_sep_by_space = m->p_sep_by_space[0];
      lconv.n_cs_precedes = m->n_cs_precedes[0];
      lconv.n_sep_by_space = m->n_sep_by_space[0];
      lconv.p_sign_posn = m->p_sign_posn[0];
      lconv.n_sign_posn = m->n_sign_posn[0];
#ifdef __HAVE_LOCALE_INFO_EXTENDED__
      lconv.int_p_cs_precedes = m->int_p_cs_precedes[0];
      lconv.int_p_sep_by_space = m->int_p_sep_by_space[0];
      lconv.int_n_cs_precedes = m->int_n_cs_precedes[0];
      lconv.int_n_sep_by_space = m->int_n_sep_by_space[0];
      lconv.int_n_sign_posn = m->int_n_sign_posn[0];
      lconv.int_p_sign_posn = m->int_p_sign_posn[0];
#else /* !__HAVE_LOCALE_INFO_EXTENDED__ */
      lconv.int_p_cs_precedes = m->p_cs_precedes[0];
      lconv.int_p_sep_by_space = m->p_sep_by_space[0];
      lconv.int_n_cs_precedes = m->n_cs_precedes[0];
      lconv.int_n_sep_by_space = m->n_sep_by_space[0];
      lconv.int_n_sign_posn = m->n_sign_posn[0];
      lconv.int_p_sign_posn = m->p_sign_posn[0];
#endif /* !__HAVE_LOCALE_INFO_EXTENDED__ */
      __mlocale_changed = 0;
    }
#endif /* __HAVE_LOCALE_INFO__ */
  return (struct lconv *) &lconv;
}


char *setlocale(int category,const char *locale)
{
  return _setlocale_r (category, locale);
}

