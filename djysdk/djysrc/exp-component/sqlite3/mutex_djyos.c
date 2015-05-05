/*
** 2007 August 14
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains the C functions that implement mutexes for djyos
*/
#include "stdint.h"
#include "lock.h"
#include "sqliteInt.h"

/*
** The code in this file is only used if we are compiling multithreaded
** on a djyos system.
*/
#ifdef SQLITE_MUTEX_DJYOS

/*
** Each recursive mutex is an instance of the following structure.
*/
struct sqlite3_mutex {
  struct tagMutexLCB *sql_mutex;    /* Mutex controlling the lock */
  int id;                    /* Mutex type */
#ifdef SQLITE_DEBUG
  volatile int nRef;         /* Number of enterances */
  volatile DWORD owner;      /* Thread holding this mutex */
  int trace;                 /* True to trace changes */
#endif
};
//#define SQLITE_W32_MUTEX_INITIALIZER { 0 }
#ifdef SQLITE_DEBUG
#define SQLITE3_MUTEX_INITIALIZER { 0, 0, 0L, (DWORD)0, 0 }
#else
#define SQLITE3_MUTEX_INITIALIZER { 0, 0 }
#endif

/*
** Return true (non-zero) if we are running under WinNT, Win2K, WinXP,
** or WinCE.  Return false (zero) for Win95, Win98, or WinME.
**
** Here is an interesting observation:  Win95, Win98, and WinME lack
** the LockFileEx() API.  But we can still statically link against that
** API as long as we don't call it win running Win95/98/ME.  A call to
** this routine is used to determine if the host is Win95/98/ME or
** WinNT/2K/XP so that we will know whether or not we can safely call
** the LockFileEx() API.
**
** mutexIsNT() is only used for the TryEnterCriticalSection() API call,
** which is only available if your application was compiled with
** _WIN32_WINNT defined to a value >= 0x0400.  Currently, the only
** call to TryEnterCriticalSection() is #ifdef'ed out, so #ifdef
** this out as well.
*/

#ifdef SQLITE_DEBUG
/*
** The sqlite3_mutex_held() and sqlite3_mutex_notheld() routine are
** intended for use only inside assert() statements.
*/
static int djyMutexHeld(sqlite3_mutex *p){
  return p->nRef!=0 && p->owner==(u32)Djy_MyEventId();
}
static int djyMutexNotheld2(sqlite3_mutex *p, DWORD tid){
  return p->nRef==0 || p->owner!=tid;
}
static int djyMutexNotheld(sqlite3_mutex *p){
  DWORD tid = (u32)Djy_MyEventId();
  return djyMutexNotheld2(p, tid);
}
#endif


/*
** Initialize and deinitialize the mutex subsystem.
*/
static sqlite3_mutex djyMutex_staticMutexes[6] = {
  SQLITE3_MUTEX_INITIALIZER,
  SQLITE3_MUTEX_INITIALIZER,
  SQLITE3_MUTEX_INITIALIZER,
  SQLITE3_MUTEX_INITIALIZER,
  SQLITE3_MUTEX_INITIALIZER,
  SQLITE3_MUTEX_INITIALIZER
};
static int djyMutex_isInit = 0;
/* As djyMutexInit() and djyMutexEnd() are called as part
** of the sqlite3_initialize and sqlite3_shutdown()
** processing, the "interlocked" magic is probably not
** strictly necessary.
*/
//static long djyMutex_lock = 0;

//void sqlite3_djyos_sleep(DWORD milliseconds); /* os_djyos.c */

static int djyMutexInit(void){
  u32 i;
  for(i=0; i<ArraySize(djyMutex_staticMutexes); i++)
  {
    djyMutex_staticMutexes[i].sql_mutex = Lock_MutexCreate(NULL);
    if(djyMutex_staticMutexes[i].sql_mutex == NULL)
    {
        for(i=0; i<ArraySize(djyMutex_staticMutexes); i++)
        {
            Lock_MutexDelete(djyMutex_staticMutexes[i].sql_mutex);
        }
        return SQLITE_ERROR;
    }
  }
  djyMutex_isInit = 1;
  return SQLITE_OK;
}

static int djyMutexEnd(void){
  if( djyMutex_isInit==1 ){
    int i;
    for(i=0; i<ArraySize(djyMutex_staticMutexes); i++)
    {
        Lock_MutexDelete(djyMutex_staticMutexes[i].sql_mutex);
    }
    djyMutex_isInit = 0;
  }
  return SQLITE_OK;
}

/*
** The sqlite3_mutex_alloc() routine allocates a new
** mutex and returns a pointer to it.  If it returns NULL
** that means that a mutex could not be allocated.  SQLite
** will unwind its stack and return an error.  The argument
** to sqlite3_mutex_alloc() is one of these integer constants:
**
** <ul>
** <li>  SQLITE_MUTEX_FAST
** <li>  SQLITE_MUTEX_RECURSIVE
** <li>  SQLITE_MUTEX_STATIC_MASTER
** <li>  SQLITE_MUTEX_STATIC_MEM
** <li>  SQLITE_MUTEX_STATIC_MEM2
** <li>  SQLITE_MUTEX_STATIC_PRNG
** <li>  SQLITE_MUTEX_STATIC_LRU
** <li>  SQLITE_MUTEX_STATIC_PMEM
** </ul>
**
** The first two constants cause sqlite3_mutex_alloc() to create
** a new mutex.  The new mutex is recursive when SQLITE_MUTEX_RECURSIVE
** is used but not necessarily so when SQLITE_MUTEX_FAST is used.
** The mutex implementation does not need to make a distinction
** between SQLITE_MUTEX_RECURSIVE and SQLITE_MUTEX_FAST if it does
** not want to.  But SQLite will only request a recursive mutex in
** cases where it really needs one.  If a faster non-recursive mutex
** implementation is available on the host platform, the mutex subsystem
** might return such a mutex in response to SQLITE_MUTEX_FAST.
**
** The other allowed parameters to sqlite3_mutex_alloc() each return
** a pointer to a static preexisting mutex.  Six static mutexes are
** used by the current version of SQLite.  Future versions of SQLite
** may add additional static mutexes.  Static mutexes are for internal
** use by SQLite only.  Applications that use SQLite mutexes should
** use only the dynamic mutexes returned by SQLITE_MUTEX_FAST or
** SQLITE_MUTEX_RECURSIVE.
**
** Note that if one of the dynamic mutex parameters (SQLITE_MUTEX_FAST
** or SQLITE_MUTEX_RECURSIVE) is used then sqlite3_mutex_alloc()
** returns a different mutex on every call.  But for the static
** mutex types, the same mutex is returned on every call that has
** the same type number.
*/
static sqlite3_mutex *djyMutexAlloc(int iType){
  sqlite3_mutex *p;

  switch( iType ){
    case SQLITE_MUTEX_FAST:
    case SQLITE_MUTEX_RECURSIVE: {
      p = sqlite3MallocZero( sizeof(*p) );
      if( p ){
#ifdef SQLITE_DEBUG
        p->id = iType;
#endif
        p->sql_mutex = Lock_MutexCreate(NULL);
        if(p->sql_mutex == NULL)
        {
            sqlite3_free(p);
            return NULL;
        }
      }
      break;
    }
    default: {
      assert( djyMutex_isInit==1 );
      assert( iType-2 >= 0 );
      assert( iType-2 < ArraySize(djyMutex_staticMutexes) );
      p = &djyMutex_staticMutexes[iType-2];
#ifdef SQLITE_DEBUG
      p->id = iType;
#endif
      break;
    }
  }
  return p;
}


/*
** This routine deallocates a previously
** allocated mutex.  SQLite is careful to deallocate every
** mutex that it allocates.
*/
static void djyMutexFree(sqlite3_mutex *p){
  assert( p );
  assert( p->nRef==0 && p->owner==0 );
  assert( p->id==SQLITE_MUTEX_FAST || p->id==SQLITE_MUTEX_RECURSIVE );
  Lock_MutexDelete(p->sql_mutex);
  sqlite3_free(p);
}

/*
** The sqlite3_mutex_enter() and sqlite3_mutex_try() routines attempt
** to enter a mutex.  If another thread is already within the mutex,
** sqlite3_mutex_enter() will block and sqlite3_mutex_try() will return
** SQLITE_BUSY.  The sqlite3_mutex_try() interface returns SQLITE_OK
** upon successful entry.  Mutexes created using SQLITE_MUTEX_RECURSIVE can
** be entered multiple times by the same thread.  In such cases the,
** mutex must be exited an equal number of times before another thread
** can enter.  If the same thread tries to enter any other kind of mutex
** more than once, the behavior is undefined.
*/
static void djyMutexEnter(sqlite3_mutex *p){
#ifdef SQLITE_DEBUG
  DWORD tid = (u32)Djy_MyEventId();
  assert( p->id==SQLITE_MUTEX_RECURSIVE || djyMutexNotheld2(p, tid) );
#endif
  Lock_MutexPend(p->sql_mutex,CN_TIMEOUT_FOREVER);
#ifdef SQLITE_DEBUG
  assert( p->nRef>0 || p->owner==0 );
  p->owner = tid;
  p->nRef++;
  if( p->trace ){
    printf("enter mutex %p (%d) with nRef=%d\n", p, p->trace, p->nRef);
  }
#endif
}
static int djyMutexTry(sqlite3_mutex *p){
#ifndef NDEBUG
  DWORD tid = (u32)Djy_MyEventId();
#endif
  int rc = SQLITE_BUSY;
  assert( p->id==SQLITE_MUTEX_RECURSIVE || djyMutexNotheld2(p, tid) );
  /*
  ** The sqlite3_mutex_try() routine is very rarely used, and when it
  ** is used it is merely an optimization.  So it is OK for it to always
  ** fail.
  */
#if 0
  if( mutexIsNT() && TryEnterCriticalSection(&p->mutex) ){
    p->owner = tid;
    p->nRef++;
    rc = SQLITE_OK;
  }
#else
  UNUSED_PARAMETER(p);
#endif
#ifdef SQLITE_DEBUG
  if( rc==SQLITE_OK && p->trace ){
    printf("try mutex %p (%d) with nRef=%d\n", p, p->trace, p->nRef);
  }
#endif
  return rc;
}

/*
** The sqlite3_mutex_leave() routine exits a mutex that was
** previously entered by the same thread.  The behavior
** is undefined if the mutex is not currently entered or
** is not currently allocated.  SQLite will never do either.
*/
static void djyMutexLeave(sqlite3_mutex *p){
#ifndef NDEBUG
  DWORD tid = (u32)Djy_MyEventId();
  assert( p->nRef>0 );
  assert( p->owner==tid );
  p->nRef--;
  if( p->nRef==0 ) p->owner = 0;
  assert( p->nRef==0 || p->id==SQLITE_MUTEX_RECURSIVE );
#endif
  Lock_MutexPost(p->sql_mutex);
#ifdef SQLITE_DEBUG
  if( p->trace ){
    printf("leave mutex %p (%d) with nRef=%d\n", p, p->trace, p->nRef);
  }
#endif
}

sqlite3_mutex_methods const *sqlite3DefaultMutex(void){
  static const sqlite3_mutex_methods sMutex = {
    djyMutexInit,
    djyMutexEnd,
    djyMutexAlloc,
    djyMutexFree,
    djyMutexEnter,
    djyMutexTry,
    djyMutexLeave,
#ifdef SQLITE_DEBUG
    djyMutexHeld,
    djyMutexNotheld
#else
    0,
    0
#endif
  };

  return &sMutex;
}
#endif /* SQLITE_MUTEX_W32 */
