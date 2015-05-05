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
# define assert(__e) ((__e) ?  __assert (__FILE__, __LINE__) : (void)0)
#endif
void __assert(const char *, s32 line);

#ifdef __cplusplus
}
#endif
