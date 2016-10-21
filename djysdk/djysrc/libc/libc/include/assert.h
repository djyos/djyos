/*
    assert.h
*/
#include "stdint.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDEBUG           /* required by ANSI standard */
# define assert(__e) ((void)0)
#else
# define assert(__e) ((__e) ?  (void)0 : __assert (__FILE__, __LINE__) )
#endif
void __assert(const char *, s32 line);

#ifdef __cplusplus
}
#endif
