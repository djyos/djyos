#ifndef __CONFIG_SQLITE3_H__
#define __CONFIG_SQLITE3_H__

#define SQLITE_CORE                     1
#define SQLITE_OS_DJYOS                 1
#define SQLITE_OS_OTHER                 1
#define _CRT_SECURE_NO_DEPRECATE
//#define _CRT_SECURE_NO_WARNINGS
#define SQLITE_THREADSAFE               1
#define SQLITE_THREAD_OVERRIDE_LOCK     (-1)
#define SQLITE_TEMP_STORE               1
#define SQLITE_ENABLE_FTS3              1
#define SQLITE_ENABLE_RTREE             1
#define SQLITE_ENABLE_COLUMN_METADATA   1
#define SQLITE_MAX_TRIGGER_DEPTH        100
#define SQLITE_DJYOS_MALLOC             1
#define SQLITE_OMIT_LOAD_EXTENSION
#define SQLITE_OMIT_AUTOINIT
#define SQLITE_OMIT_LOCALTIME
#if !defined(MAX_PATH)
    #define MAX_PATH                    256
#endif
#define SQLITE_OMIT_WAL                 1
#define HAVE_USLEEP                     1

#endif /* __CONFIG_SQLITE3_H__ */

