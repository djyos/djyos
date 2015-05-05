/*
** 2004 May 22
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
** This file contains code that is specific to Windows.
*/
#include "sqliteInt.h"
#include "stdint.h"
#include "file.h"
#include "stat.h"
#include "time.h"
#include "djyos.h"
#if SQLITE_OS_DJYOS               /* This file is used for djyos only */

/*
** Include code that is common to all os_*.c files
*/
#include "os_common.h"

/*
** Compiling and using WAL mode requires several APIs that are only
** available in Windows platforms based on the NT kernel.
*/
#ifndef SQLITE_OMIT_WAL
# error "unsupport WAL modefrom please define SQLITE_OMIT_WAL."
#endif


/*
** The djyFile structure is a subclass of sqlite3_file* specific to the djyos
** portability layer.
*/
typedef struct djyFile djyFile;
struct djyFile {
  const sqlite3_io_methods *pMethod; /*** Must be first ***/
  sqlite3_vfs *pVfs;      /* The VFS used to open this file */
  FILE *djyFp;               /* Handle for accessing the file */
  u8 locktype;            /* Type of lock currently held on this file */
  short sharedLockByte;   /* Randomly chosen byte used as a shared lock */
  u8 ctrlFlags;           /* Flags.  See WINFILE_* below */
  u32 lastErrno;        /* The djyos errno from the last I/O error */
#ifndef SQLITE_OMIT_WAL
  djyShm *pShm;           /* Instance of shared memory on this file */
#endif
  const char *zPath;      /* Full pathname of this file */
  int szChunk;            /* Chunk size configured by SQLITE_FCNTL_CHUNK_SIZE */
};

/*
** Allowed values for djyFile.ctrlFlags
*/
#define WINFILE_PERSIST_WAL     0x04   /* Persistent WAL mode */
#define WINFILE_PSOW            0x10   /* SQLITE_IOCAP_POWERSAFE_OVERWRITE */

/*
 * The value used with sqlite3_djyos_set_directory() to specify that
 * the data directory should be changed.
 */
#ifndef SQLITE_WIN32_DATA_DIRECTORY_TYPE
#  define SQLITE_WIN32_DATA_DIRECTORY_TYPE (1)
#endif

/*
 * The value used with sqlite3_djyos_set_directory() to specify that
 * the temporary directory should be changed.
 */
#ifndef SQLITE_WIN32_TEMP_DIRECTORY_TYPE
#  define SQLITE_WIN32_TEMP_DIRECTORY_TYPE (2)
#endif

/*
 * If compiled with SQLITE_DJYOS_MALLOC on djyos, we will use the
 * various djyos API heap functions instead of our own.
 */
#ifdef SQLITE_DJYOS_MALLOC

/*
 * If this is non-zero, an isolated heap will be created by the native Win32
 * allocator subsystem; otherwise, the default process heap will be used.  This
 * setting has no effect when compiling for WinRT.  By default, this is enabled
 * and an isolated heap will be created to store all allocated data.
 *
 ******************************************************************************
 * WARNING: It is important to note that when this setting is non-zero and the
 *          djyMemShutdown function is called (e.g. by the sqlite3_shutdown
 *          function), all data that was allocated using the isolated heap will
 *          be freed immediately and any attempt to access any of that freed
 *          data will almost certainly result in an immediate access violation.
 ******************************************************************************
 */
#ifndef SQLITE_WIN32_HEAP_CREATE
#  define SQLITE_WIN32_HEAP_CREATE    (TRUE)
#endif

/*
 * The initial size of the Win32-specific heap.  This value may be zero.
 */
#ifndef SQLITE_WIN32_HEAP_INIT_SIZE
#  define SQLITE_WIN32_HEAP_INIT_SIZE ((SQLITE_DEFAULT_CACHE_SIZE) * \
                                       (SQLITE_DEFAULT_PAGE_SIZE) + 4194304)
#endif

/*
 * The maximum size of the Win32-specific heap.  This value may be zero.
 */
#ifndef SQLITE_WIN32_HEAP_MAX_SIZE
#  define SQLITE_WIN32_HEAP_MAX_SIZE  (0)
#endif

/*
 * The extra flags to use in calls to the Win32 heap APIs.  This value may be
 * zero for the default behavior.
 */
#ifndef SQLITE_WIN32_HEAP_FLAGS
#  define SQLITE_WIN32_HEAP_FLAGS     (0)
#endif

/*
** The djyMemData structure stores information required by the Win32-specific
** sqlite3_mem_methods implementation.
*/
typedef struct djyMemData djyMemData;
struct djyMemData {
#ifndef NDEBUG
  u32 magic;    /* Magic number to detect structure corruption. */
#endif
  void * hHeap; /* The handle to our heap. */
  bool_t bOwned;  /* Do we own the heap (i.e. destroy it on shutdown)? */
};

#ifndef NDEBUG
#define WINMEM_MAGIC     0x42b2830b
#endif

static struct djyMemData djy_mem_data = {
#ifndef NDEBUG
  WINMEM_MAGIC,
#endif
  NULL, false
};

#ifndef NDEBUG
#define djyMemAssertMagic() assert( djy_mem_data.magic==WINMEM_MAGIC )
#else
#define djyMemAssertMagic()
#endif

#define djyMemGetHeap() djy_mem_data.hHeap

static void *djyMemMalloc(int nBytes);
static void djyMemFree(void *pPrior);
static void *djyMemRealloc(void *pPrior, int nBytes);
static int djyMemSize(void *p);
static int djyMemRoundup(int n);
static int djyMemInit(void *pAppData);
static void djyMemShutdown(void *pAppData);

const sqlite3_mem_methods *sqlite3MemGetDjyos(void);
#endif /* SQLITE_DJYOS_MALLOC */

#ifndef SYSCALL
#  define SYSCALL sqlite3_syscall_ptr
#endif

sqlite_uint64 sqlite3Hwtime(void)
{
    return ((sqlite_uint64)DjyGetTime());
}

/*
** This is the xSetSystemCall() method of sqlite3_vfs for all of the
** "djyos" VFSes.  Return SQLITE_OK opon successfully updating the
** system call pointer, or SQLITE_NOTFOUND if there is no configurable
** system call named zName.
*/
static int djySetSystemCall(
  sqlite3_vfs *pNotUsed,        /* The VFS pointer.  Not used */
  const char *zName,            /* Name of system call to override */
  sqlite3_syscall_ptr pNewFunc  /* Pointer to new system call value */
){
  return 0;
}

/*
** Return the value of a system call.  Return NULL if zName is not a
** recognized system call name.  NULL is also returned if the system call
** is currently undefined.
*/
static sqlite3_syscall_ptr djyGetSystemCall(
  sqlite3_vfs *pNotUsed,
  const char *zName
){
  return 0;
}

/*
** Return the name of the first system call after zName.  If zName==NULL
** then return the name of the first system call.  Return NULL if zName
** is the last system call or if zName is not the name of a valid
** system call.
*/
static const char *djyNextSystemCall(sqlite3_vfs *p, const char *zName){
  return 0;
}

void sqlite3_djyos_sleep(u32 milliseconds){
  Djy_EventDelay(milliseconds*mS);
}



#ifdef SQLITE_DJYOS_MALLOC
/*
** Allocate nBytes of memory.
*/
static void *djyMemMalloc(int nBytes){
  return M_MallocLc(nBytes,CN_TIMEOUT_FOREVER);
}

/*
** Free memory.
*/
static void djyMemFree(void *pPrior){
  free(pPrior);
}

/*
** Change the size of an existing memory allocation
*/
static void *djyMemRealloc(void *pPrior, int nBytes){
  free(pPrior);
  return M_MallocLc(nBytes,CN_TIMEOUT_FOREVER);
}

/*
** Return the size of an outstanding allocation, in bytes.
*/
static int djyMemSize(void *p){
  return (int)M_CheckSize(p);
}

/*
** Round up a request size to the next valid allocation size.
*/
static int djyMemRoundup(int n){
    return (int)M_FormatSize(n);
}

/*
** Initialize this module.
*/
static int djyMemInit(void *pAppData){
  return SQLITE_OK;
}

/*
** Deinitialize this module.
*/
static void djyMemShutdown(void *pAppData){
  return;
}

/*
** Populate the low-level memory allocation function pointers in
** sqlite3GlobalConfig.m with pointers to the routines in this file. The
** arguments specify the block of memory to manage.
**
** This routine is only called by sqlite3_config(), and therefore
** is not required to be threadsafe (it is not).
*/
const sqlite3_mem_methods *sqlite3MemGetDjyos(void){
  static const sqlite3_mem_methods djyMemMethods = {
    djyMemMalloc,
    djyMemFree,
    djyMemRealloc,
    djyMemSize,
    djyMemRoundup,
    djyMemInit,
    djyMemShutdown,
    &djy_mem_data
  };
  return &djyMemMethods;
}

void sqlite3MemSetDefault(void){
  sqlite3_config(SQLITE_CONFIG_MALLOC, sqlite3MemGetDjyos());
}
#endif /* SQLITE_DJYOS_MALLOC */

/*
** This function sets the data directory or the temporary directory based on
** the provided arguments.  The type argument must be 1 in order to set the
** data directory or 2 in order to set the temporary directory.  The zValue
** argument is the name of the directory to use.  The return value will be
** SQLITE_OK if successful.
*/
int sqlite3_djyos_set_directory(u32 type, char* zValue){
#ifndef SQLITE_OMIT_AUTOINIT
  int rc = sqlite3_initialize();
  if( rc ) return rc;
#endif
  if( type==SQLITE_WIN32_DATA_DIRECTORY_TYPE ){
    if(sqlite3_data_directory != NULL)
    {
      sqlite3_data_directory = zValue;
    }
    return SQLITE_OK;
  }else if( type==SQLITE_WIN32_TEMP_DIRECTORY_TYPE ){
    if(sqlite3_temp_directory != NULL)
    {
      sqlite3_temp_directory = zValue;
    }
    return SQLITE_OK;
  }else
    return SQLITE_ERROR;
}

/*
** The return value of getLastErrorMsg
** is zero if the error message fits in the buffer, or non-zero
** otherwise (if the message was truncated).
*/
static int getLastErrorMsg(u32 lastErrno, int nBuf, char *zBuf){
  return 0;
}

/*
**
** This function - djyLogErrorAtLine() - is only ever called via the macro
** djyLogError().
**
** This routine is invoked after an error occurs in an OS function.
** It logs a message using sqlite3_log() containing the current value of
** error code and, if possible, the human-readable equivalent from
** FormatMessage.
**
** The first argument passed to the macro should be the error code that
** will be returned to SQLite (e.g. SQLITE_IOERR_DELETE, SQLITE_CANTOPEN).
** The two subsequent arguments should be the name of the OS function that
** failed and the associated file-system path, if any.
*/
//#define djyLogError(a,b,c,d)   djyLogErrorAtLine(a,b,c,d,__LINE__)
//static int djyLogErrorAtLine(
//  int errcode,                    /* SQLite error code */
//  u32 lastErrno,                /* djyos last error */
//  const char *zFunc,              /* Name of OS function that failed */
//  const char *zPath,              /* File path associated with error */
//  int iLine                       /* Source line number where error occurred */
//){
//  Djy_SaveLastError(errcode);
//  printf("%s\n\r",zFunc);
//  return errcode;
//}

/*
** The number of times that a ReadFile(), WriteFile(), and DeleteFile()
** will be retried following a locking error - probably caused by
** antivirus software.  Also the initial delay before the first retry.
** The delay increases linearly with each retry.
*/
#ifndef SQLITE_WIN32_IOERR_RETRY
# define SQLITE_WIN32_IOERR_RETRY 10
#endif
#ifndef SQLITE_WIN32_IOERR_RETRY_DELAY
# define SQLITE_WIN32_IOERR_RETRY_DELAY 25
#endif
static int djy32IoerrRetry = SQLITE_WIN32_IOERR_RETRY;
static int djy32IoerrRetryDelay = SQLITE_WIN32_IOERR_RETRY_DELAY;

/*
** Log a I/O error retry episode.
*/
static void logIoerr(int nRetry){
  if( nRetry ){
    sqlite3_log(SQLITE_IOERR,
      "delayed %dms for lock/sharing conflict",
      djy32IoerrRetryDelay*nRetry*(nRetry+1)/2
    );
  }
}

/*****************************************************************************
** The next group of routines implement the I/O methods specified
** by the sqlite3_io_methods object.
******************************************************************************/

/*
** Close a file.
**
** It is reported that an attempt to close a handle might sometimes
** fail.  This is a very unreasonable result, but Windows is notorious
** for being unreasonable so I do not doubt that it might happen.  If
** the close fails, we pause for 100 milliseconds and try again.  As
** many as MX_CLOSE_ATTEMPT attempts to close the handle are made before
** giving up and returning an error.
*/
#define MX_CLOSE_ATTEMPT 3
static int djyClose(sqlite3_file *id){
    int rc, cnt = 0;
    djyFile *pFile = (djyFile*)id;

    assert( id!=0 );
#ifndef SQLITE_OMIT_WAL
    assert( pFile->pShm==0 );
#endif
    OSTRACE(("CLOSE %d\n", pFile->djyFp));
    do{
      rc = fclose(pFile->djyFp);
        /* SimulateIOError( rc=0; cnt=MX_CLOSE_ATTEMPT; ); */
    }while( rc!=0 && ++cnt < MX_CLOSE_ATTEMPT&& (sqlite3_djyos_sleep(100), 1) );
    OSTRACE(("CLOSE %d %s\n", pFile->djyFp, rc ? "ok" : "failed"));
    if( rc ){
      pFile->djyFp = NULL;
    }
    OpenCounter(-1);
    if(!rc)
        return SQLITE_OK;
    else
    {
        Djy_SaveLastError(SQLITE_IOERR_CLOSE);
        printf("djyClose error\n\r");
        return SQLITE_IOERR_CLOSE;
    }
}

/*
** Read data from a file into a buffer.  Return SQLITE_OK if all
** bytes were read successfully and SQLITE_IOERR if anything goes
** wrong.
*/
static int djyRead(
  sqlite3_file *id,          /* File to read from */
  void *pBuf,                /* Write content into this buffer */
  int amt,                   /* Number of bytes to read */
  sqlite3_int64 offset       /* Begin reading at this offset */
){
  djyFile *pFile = (djyFile*)id;  /* file handle */

  assert( id!=0 );
  SimulateIOError(return SQLITE_IOERR_READ);
  OSTRACE(("READ %d lock=%d\n", pFile->djyFp, pFile->locktype));
  if(amt != (int)fread_r(pFile->djyFp,pBuf,amt,offset))
  {
    Djy_SaveLastError(SQLITE_IOERR_SHORT_READ);
    printf("djyRead EOF\n\r");
    return SQLITE_IOERR_SHORT_READ;
  }
  return SQLITE_OK;
}

/*
** Write data from a buffer into a file.  Return SQLITE_OK on success
** or some other error code on failure.
*/
static int djyWrite(
  sqlite3_file *id,               /* File to write into */
  const void *pBuf,               /* The bytes to be written */
  int amt,                        /* Number of bytes to write */
  sqlite3_int64 offset            /* Offset into the file to begin writing at */
){
    u32 rc;
  djyFile *pFile = (djyFile*)id;  /* File handle */
  rc = strlen(pFile->zPath);
  if(rc < 0x1b)
      rc = 0;
  else
      rc = 0;

  assert( amt>0 );
  assert( pFile );
  SimulateIOError(return SQLITE_IOERR_WRITE);
  SimulateDiskfullError(return SQLITE_FULL);

  OSTRACE(("WRITE %d lock=%d\n", pFile->h, pFile->locktype));

  if(amt != (int)fwrite_r(pFile->djyFp,pBuf,amt,offset))
  {
    memset((void*)pBuf, 0, amt);
    Djy_SaveLastError(SQLITE_IOERR_WRITE);
    printf("djyWrite Error\n\r");
    return SQLITE_IOERR_WRITE;
  }

  return SQLITE_OK;
}

/*
** Truncate an open file to a specified size
*/
static int djyTruncate(sqlite3_file *id, sqlite3_int64 nByte){
  djyFile *pFile = (djyFile*)id;  /* File handle object */

  assert( pFile );

  OSTRACE(("TRUNCATE %d %lld\n", pFile->h, nByte));
  SimulateIOError(return SQLITE_IOERR_TRUNCATE);

  /* If the user has configured a chunk-size for this file, truncate the
  ** file so that it consists of an integer number of chunks (i.e. the
  ** actual file size after the operation may be larger than the requested
  ** size).
  */
  if( pFile->szChunk>0 ){
    nByte = ((nByte + pFile->szChunk - 1)/pFile->szChunk) * pFile->szChunk;
  }

  if( Djyfs_Ftruncate(pFile->djyFp, nByte) ){
    Djy_SaveLastError(SQLITE_IOERR_TRUNCATE);
    printf("djyTruncate2 error\n\r");
    return SQLITE_IOERR_TRUNCATE;
  }

  OSTRACE(("TRUNCATE %d %lld %s\n", pFile->djyFp, nByte, SQLITE_OK ? "failed" : "ok"));
  return SQLITE_OK;
}

#ifdef SQLITE_TEST
/*
** Count the number of fullsyncs and normal syncs.  This is used to test
** that syncs and fullsyncs are occuring at the right times.
*/
int sqlite3_sync_count = 0;
int sqlite3_fullsync_count = 0;
#endif

/*
** Make sure all writes to a particular file are committed to disk.
*/
static int djySync(sqlite3_file *id, int flags){
  djyFile *pFile = (djyFile*)id;
#ifndef SQLITE_NO_SYNC
  /*
  ** Used only when SQLITE_NO_SYNC is not defined.
   */
#endif

  assert( pFile );
  /* Check that one of SQLITE_SYNC_NORMAL or FULL was passed */
  assert((flags&0x0F)==SQLITE_SYNC_NORMAL
      || (flags&0x0F)==SQLITE_SYNC_FULL
  );

  OSTRACE(("SYNC %d lock=%d\n", pFile->djyFp, pFile->locktype));


#ifndef SQLITE_TEST
  UNUSED_PARAMETER(flags);
#else
  if( (flags&0x0F)==SQLITE_SYNC_FULL ){
    sqlite3_fullsync_count++;
  }
  sqlite3_sync_count++;
#endif

  /* If we compiled with the SQLITE_NO_SYNC flag, then syncing is a
  ** no-op
  */
#ifdef SQLITE_NO_SYNC
  return SQLITE_OK;
#else
  /* fflush() returns non-zero when successful, or zero when it fails. */
  if(fflush(pFile->djyFp))
  {
    Djy_SaveLastError(SQLITE_IOERR_FSYNC);
    printf("djySync error\n\r");
    return SQLITE_IOERR_FSYNC;
  }
  else
  {
      return SQLITE_OK;
  }
#endif
}

/*
** Determine the current size of a file in bytes
*/
static int djyFileSize(sqlite3_file *id, sqlite3_int64 *pSize){
  djyFile *pFile = (djyFile*)id;
  struct stat info;
  assert( id!=0 );
  SimulateIOError(return SQLITE_IOERR_FSTAT);

  if(Djyfs_Fstat(pFile->djyFp,&info))
  {
    *pSize = info.file_size;
    return SQLITE_OK;
  }
  else
  {
    *pSize = 0;
    return SQLITE_ERROR;
  }

}

/*
** LOCKFILE_FAIL_IMMEDIATELY is undefined on some Windows systems.
*/
#ifndef LOCKFILE_FAIL_IMMEDIATELY
# define LOCKFILE_FAIL_IMMEDIATELY 1
#endif

#ifndef LOCKFILE_EXCLUSIVE_LOCK
# define LOCKFILE_EXCLUSIVE_LOCK 2
#endif

/*
** Historically, SQLite has used both the LockFile and LockFileEx functions.
** When the LockFile function was used, it was always expected to fail
** immediately if the lock could not be obtained.  Also, it always expected to
** obtain an exclusive lock.  These flags are used with the LockFileEx function
** and reflect those expectations; therefore, they should not be changed.
*/
#ifndef SQLITE_LOCKFILE_FLAGS
# define SQLITE_LOCKFILE_FLAGS   (LOCKFILE_FAIL_IMMEDIATELY | \
                                  LOCKFILE_EXCLUSIVE_LOCK)
#endif

/*
** Currently, SQLite never calls the LockFileEx function without wanting the
** call to fail immediately if the lock cannot be obtained.
*/
#ifndef SQLITE_LOCKFILEEX_FLAGS
# define SQLITE_LOCKFILEEX_FLAGS (LOCKFILE_FAIL_IMMEDIATELY)
#endif

/*
** Lock the file with the lock specified by parameter locktype - one
** of the following:
**
**     (1) SHARED_LOCK
**     (2) RESERVED_LOCK
**     (3) PENDING_LOCK
**     (4) EXCLUSIVE_LOCK
**
** Sometimes when requesting one lock state, additional lock states
** are inserted in between.  The locking might fail on one of the later
** transitions leaving the lock state different from what it started but
** still short of its goal.  The following chart shows the allowed
** transitions and the inserted intermediate states:
**
**    UNLOCKED -> SHARED
**    SHARED -> RESERVED
**    SHARED -> (PENDING) -> EXCLUSIVE
**    RESERVED -> (PENDING) -> EXCLUSIVE
**    PENDING -> EXCLUSIVE
**
** This routine will only increase a lock.  The djyUnlock() routine
** erases all locks at once and returns us immediately to locking level 0.
** It is not possible to lower the locking level one step at a time.  You
** must go straight to locking level 0.
*/
static int djyLock(sqlite3_file *id, int locktype){

  assert( id!=0 );
  OSTRACE(("LOCK %d %d was %d(%d)\n",
           pFile->djyFp, locktype, pFile->locktype, pFile->sharedLockByte));

  return SQLITE_OK;
}

/*
** This routine checks if there is a RESERVED lock held on the specified
** file by this or any other process. If such a lock is held, return
** non-zero, otherwise zero.
*/
static int djyCheckReservedLock(sqlite3_file *id, int *pResOut){

  SimulateIOError( return SQLITE_IOERR_CHECKRESERVEDLOCK; );

  assert( id!=0 );
  *pResOut = 0;
  return SQLITE_OK;
}

/*
** Lower the locking level on file descriptor id to locktype.  locktype
** must be either NO_LOCK or SHARED_LOCK.
**
** If the locking level of the file descriptor is already at or below
** the requested locking level, this routine is a no-op.
**
** It is not possible for this routine to fail if the second argument
** is NO_LOCK.  If the second argument is SHARED_LOCK then this routine
** might return SQLITE_IOERR;
*/
static int djyUnlock(sqlite3_file *id, int locktype){
  assert( pFile!=0 );
  assert( locktype<=SHARED_LOCK );
  OSTRACE(("UNLOCK %d to %d was %d(%d)\n", pFile->djyFp, locktype,
          pFile->locktype, pFile->sharedLockByte));
  return SQLITE_OK;
}

/*
** If *pArg is inititially negative then this is a query.  Set *pArg to
** 1 or 0 depending on whether or not bit mask of pFile->ctrlFlags is set.
**
** If *pArg is 0 or 1, then clear or set the mask bit of pFile->ctrlFlags.
*/
static void djyModeBit(djyFile *pFile, unsigned char mask, int *pArg){
  if( *pArg<0 ){
    *pArg = (pFile->ctrlFlags & mask)!=0;
  }else if( (*pArg)==0 ){
    pFile->ctrlFlags &= ~mask;
  }else{
    pFile->ctrlFlags |= mask;
  }
}

/* Forward declaration */
static int getTempname(int nBuf, char *zBuf);

/*
** Control and query of the open file handle.
*/
static int djyFileControl(sqlite3_file *id, int op, void *pArg){
  djyFile *pFile = (djyFile*)id;
  switch( op ){
    case SQLITE_FCNTL_LOCKSTATE: {
      *(int*)pArg = pFile->locktype;
      return SQLITE_OK;
    }
    case SQLITE_LAST_ERRNO: {
      *(int*)pArg = (int)pFile->lastErrno;
      return SQLITE_OK;
    }
    case SQLITE_FCNTL_CHUNK_SIZE: {
      pFile->szChunk = *(int *)pArg;
      return SQLITE_OK;
    }
    case SQLITE_FCNTL_SIZE_HINT: {
      if( pFile->szChunk>0 ){
        sqlite3_int64 oldSz;
        int rc = djyFileSize(id, &oldSz);
        if( rc==SQLITE_OK ){
          sqlite3_int64 newSz = *(sqlite3_int64*)pArg;
          if( newSz>oldSz ){
            SimulateIOErrorBenign(1);
            rc = djyTruncate(id, newSz);
            SimulateIOErrorBenign(0);
          }
        }
        return rc;
      }
      return SQLITE_OK;
    }
    case SQLITE_FCNTL_PERSIST_WAL: {
      djyModeBit(pFile, WINFILE_PERSIST_WAL, (int*)pArg);
      return SQLITE_OK;
    }
    case SQLITE_FCNTL_POWERSAFE_OVERWRITE: {
      djyModeBit(pFile, WINFILE_PSOW, (int*)pArg);
      return SQLITE_OK;
    }
    case SQLITE_FCNTL_VFSNAME: {
      *(char**)pArg = sqlite3_mprintf("djyos");
      return SQLITE_OK;
    }
    case SQLITE_FCNTL_WIN32_AV_RETRY: {
      int *a = (int*)pArg;
      if( a[0]>0 ){
        djy32IoerrRetry = a[0];
      }else{
        a[0] = djy32IoerrRetry;
      }
      if( a[1]>0 ){
        djy32IoerrRetryDelay = a[1];
      }else{
        a[1] = djy32IoerrRetryDelay;
      }
      return SQLITE_OK;
    }
    case SQLITE_FCNTL_TEMPFILENAME: {
      char *zTFile = sqlite3_malloc( pFile->pVfs->mxPathname );
      if( zTFile ){
        getTempname(pFile->pVfs->mxPathname, zTFile);
        *(char**)pArg = zTFile;
      }
      return SQLITE_OK;
    }
  }
  return SQLITE_NOTFOUND;
}

/*
** Return the sector size in bytes of the underlying block device for
** the specified file. This is almost always 512 bytes, but may be
** larger for some devices.
**
** SQLite code assumes this function cannot fail. It also assumes that
** if two files are created in the same file-system directory (i.e.
** a database and its journal file) that the sector size will be the
** same for both.
*/
static int djySectorSize(sqlite3_file *id){
  (void)id;
  return 1;
}

/*
** Return a vector of device characteristics.
*/
static int djyDeviceCharacteristics(sqlite3_file *id){
  djyFile *p = (djyFile*)id;
  return SQLITE_IOCAP_UNDELETABLE_WHEN_OPEN |
         ((p->ctrlFlags & WINFILE_PSOW)?SQLITE_IOCAP_POWERSAFE_OVERWRITE:0);
}

#ifndef SQLITE_OMIT_WAL
# error "请定义 SQLITE_OMIT_WAL."
#else
# define djyShmMap     0
# define djyShmLock    0
# define djyShmBarrier 0
# define djyShmUnmap   0
#endif /* #ifndef SQLITE_OMIT_WAL */


/*
** Here ends the implementation of all sqlite3_file methods.
**
********************** End sqlite3_file Methods *******************************
******************************************************************************/

/*
** This vector defines all the methods that can operate on an
** sqlite3_file for djyos.
*/
static const sqlite3_io_methods djyIoMethod = {
  2,                              /* iVersion */
  djyClose,                       /* xClose */
  djyRead,                        /* xRead */
  djyWrite,                       /* xWrite */
  djyTruncate,                    /* xTruncate */
  djySync,                        /* xSync */
  djyFileSize,                    /* xFileSize */
  djyLock,                        /* xLock */
  djyUnlock,                      /* xUnlock */
  djyCheckReservedLock,           /* xCheckReservedLock */
  djyFileControl,                 /* xFileControl */
  djySectorSize,                  /* xSectorSize */
  djyDeviceCharacteristics,       /* xDeviceCharacteristics */
  djyShmMap,                      /* xShmMap */
  djyShmLock,                     /* xShmLock */
  djyShmBarrier,                  /* xShmBarrier */
  djyShmUnmap                     /* xShmUnmap */
};

/****************************************************************************
**************************** sqlite3_vfs methods ****************************
**
** This division contains the implementation of methods on the
** sqlite3_vfs object.
*/

/*
** Create a temporary file name in zBuf.  zBuf must be big enough to
** hold at pVfs->mxPathname characters.
*/
static int getTempname(int nBuf, char *zBuf){
  static char zChars[] =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789";
  size_t i, j;
  int nTempPath;
  char zTempPath[MAX_PATH+2];

  /* It's odd to simulate an io-error here, but really this is just
  ** using the io-error infrastructure to test that SQLite handles this
  ** function failing.
  */
  SimulateIOError( return SQLITE_IOERR );

  memset(zTempPath, 0, MAX_PATH+2);

  if( sqlite3_temp_directory ){
    sqlite3_snprintf(MAX_PATH-30, zTempPath, "%s", sqlite3_temp_directory);
  }

  /* Check that the output buffer is large enough for the temporary file
  ** name. If it is not, return SQLITE_ERROR.
  */
  nTempPath = sqlite3Strlen30(zTempPath);

  if( (nTempPath + sqlite3Strlen30(SQLITE_TEMP_FILE_PREFIX) + 18) >= nBuf ){
    return SQLITE_ERROR;
  }

  for(i=nTempPath; i>0 && zTempPath[i-1]=='\\'; i--){}
  zTempPath[i] = 0;

  sqlite3_snprintf(nBuf-18, zBuf, (nTempPath > 0) ?
                       "%s\\"SQLITE_TEMP_FILE_PREFIX : SQLITE_TEMP_FILE_PREFIX,
                   zTempPath);
  j = sqlite3Strlen30(zBuf);
  sqlite3_randomness(15, &zBuf[j]);
  for(i=0; i<15; i++, j++){
    zBuf[j] = (char)zChars[ ((unsigned char)zBuf[j])%(sizeof(zChars)-1) ];
  }
  zBuf[j] = 0;
  zBuf[j+1] = 0;

  OSTRACE(("TEMP FILENAME: %s\n", zBuf));
  return SQLITE_OK;
}

/*
** Return TRUE if the named file is really a directory.  Return false if
** it is something other than a directory, or if there is any kind of memory
** allocation failure.
*/
static int djyIsDir(const char *zConverted){
  struct stat fp_info;
  bool_t result;
  result = stat(zConverted,&fp_info);
  if(result != -1)
  {
    if( fp_info.st_mode.bits.folder)
        return true;
    else
        return false;
  }
  else
    return false;
}

/*
** Open a file.
*/
static int djyOpen(
  sqlite3_vfs *pVfs,        /* Not used */
  const char *zName,        /* Name of the file */
  sqlite3_file *id,         /* Write the SQLite file handle here */
  int flags,                /* Open mode flags */
  int *pOutFlags            /* Status return flags */
){
  char *open_mode;
  djyFile *pFile = (djyFile*)id;
  const char *zFileName = zName;

  /* If argument zFileName is a NULL pointer, this function is required to open
  ** a temporary file. Use this buffer to store the file name in.
  */
  char zTmpname[MAX_PATH+2];     /* Buffer used to create temp filename */

  int rc = SQLITE_OK;            /* Function Return Code */

  assert( id!=0 );
  /* If the second argument to this function is NULL, generate a
  ** temporary file name to use
  */
  if( !zFileName ){
    rc = getTempname(MAX_PATH+2, zTmpname);
    open_mode = "cd";
    zFileName = zTmpname;
    if( rc!=SQLITE_OK ){
      return rc;
    }
  }
  else if(flags & SQLITE_OPEN_READONLY)
    open_mode = "r";
  else
  {
    if(stat(zFileName,NULL) != -1)
      open_mode = "r+";
    else
      open_mode = "w+";
  }
  memset(pFile, 0, sizeof(*pFile));
  pFile->djyFp = 0;

  /* Database filenames are double-zero terminated if they are not
  ** URIs with parameters.  Hence, they can always be passed into
  ** sqlite3_uri_parameter().
  */
      assert( (flags & SQLITE_OPEN_URI) || zName[strlen(zName)+1]==0 );


  if( djyIsDir(zFileName) ){
    sqlite3_free((void*)zFileName);
    return SQLITE_CANTOPEN_ISDIR;
  }
  pFile->djyFp = fopen(zFileName,open_mode);
  if(pOutFlags)
  {
      if( pFile->djyFp )
      {
        if(flags & SQLITE_OPEN_READWRITE){
          *pOutFlags = SQLITE_OPEN_READWRITE;
        }else{
          *pOutFlags = SQLITE_OPEN_READONLY;
        }
      }
      else
      {
        *pOutFlags = 0;
      }
  }

  pFile->pMethod = &djyIoMethod;
  pFile->lastErrno = 0;
  pFile->pVfs = pVfs;
#ifndef SQLITE_OMIT_WAL
  pFile->pShm = 0;
#endif
  pFile->zPath = zFileName;
  if( sqlite3_uri_boolean(zFileName, "psow", SQLITE_POWERSAFE_OVERWRITE) ){
    pFile->ctrlFlags |= WINFILE_PSOW;
  }


  OpenCounter(+1);
  return rc;
}

/*
** Delete the named file.
**
** Note that Windows does not allow a file to be deleted if some other
** process has it open.  Sometimes a virus scanner or indexing program
** will open a journal file shortly after it is created in order to do
** whatever it does.  While this other process is holding the
** file open, we will be unable to delete it.  To work around this
** problem, we delay 100 milliseconds and try to delete again.  Up
** to MX_DELETION_ATTEMPTs deletion attempts are run before giving
** up and returning an error.
*/
static int djyDelete(
  sqlite3_vfs *pVfs,          /* Not used on djyos */
  const char *zFilename,      /* Name of file to delete */
  int syncDir                 /* Not used on djyos */
){
  int rc;

  SimulateIOError(return SQLITE_IOERR_DELETE);
  rc = remove(zFilename);
  if(rc == EN_FS_OBJECT_NONENTITY)
    rc = SQLITE_IOERR_DELETE_NOENT;
  else if(rc == 0)
    rc = SQLITE_OK;
  else
    rc = SQLITE_ERROR;
  OSTRACE(("DELETE \"%s\" %s\n", zFilename, (rc ? "failed" : "ok" )));
  return rc;
}

/*
** Check the existance and status of a file.
** 经阅读sqlite3源码，返回值的含义是djyAccess函数是否成功调用。pResOut返回的含义
** 是:如果文件能按照flags指定的方式访问，返回非0，否则返回0
*/
static int djyAccess(
  sqlite3_vfs *pVfs,         /* Not used on djyos */
  const char *zFilename,     /* Name of file to check */
  int flags,                 /* Type of test to make on this file */
  int *pResOut               /* OUT: Result */
){
  struct stat fp_info;
  bool_t fp_exist;
  UNUSED_PARAMETER(pVfs);

  SimulateIOError( return SQLITE_IOERR_ACCESS; );
  switch( flags ){
    case SQLITE_ACCESS_READ:
    case SQLITE_ACCESS_EXISTS:
        if(stat(zFilename,NULL) == -1)
            *pResOut = 0;
        else
            *pResOut = 1;
        break;
    case SQLITE_ACCESS_READWRITE:
      fp_exist = stat(zFilename,&fp_info);
      if(fp_exist != -1)
      {
        if(fp_info.st_mode.bits.read_only)
          *pResOut = 0;
        else
          *pResOut = 1;
      }
      else
          *pResOut = 0;
      break;
    default:
        *pResOut = 0;
        assert(!"Invalid flags argument");
  }
  return SQLITE_OK;
}

/*
** Turn a relative pathname into a full pathname.  Write the full
** pathname into zOut[].  zOut[] will be at least pVfs->mxPathname
** bytes in size.
*/
static int djyFullPathname(
  sqlite3_vfs *pVfs,            /* Pointer to vfs object */
  const char *zRelative,        /* Possibly relative input path */
  int nFull,                    /* Size of output buffer in bytes */
  char *zFull                   /* Output buffer */
){

    SimulateIOError( return SQLITE_ERROR );
    //zRelative已经是全路径名了
    if( (strchr(zRelative,':') || zRelative[0] == '\\') )
    {
        if(nFull > (int)strlen(zRelative))
        {
          strcpy(zFull,zRelative);
          return SQLITE_OK;
        }
        else
        {
          return SQLITE_ERROR;
        }
    }
    else
    {
        if ( sqlite3_data_directory )
        {
            if(nFull > (int)(strlen(zRelative) + strlen(sqlite3_data_directory) ) )
            {
              strcpy(zFull,sqlite3_data_directory);
              strcat(zFull,zRelative);
              return SQLITE_OK;
            }
            else
            {
              return SQLITE_ERROR;
            }
        }
        else
        {
            if(Djyfs_GetCwd(zFull,nFull))
            {
                if(nFull > (int)(strlen(zRelative) + strlen(zFull) ) )
                {
                  strcat(zFull,zRelative);
                  return SQLITE_OK;
                }
                else
                {
                  return SQLITE_ERROR;
                }
            }
        }
    }
    return SQLITE_OK;
}

#ifndef SQLITE_OMIT_LOAD_EXTENSION
/*
** Interfaces for opening a shared library, finding entry points
** within the shared library, and closing the shared library.
*/
/*
** Interfaces for opening a shared library, finding entry points
** within the shared library, and closing the shared library.
*/
# error "请定义 SQLITE_OMIT_LOAD_EXTENSION."

#else /* if SQLITE_OMIT_LOAD_EXTENSION is defined: */
  #define djyDlOpen  0
  #define djyDlError 0
  #define djyDlSym   0
  #define djyDlClose 0
#endif


/*
** Write up to nBuf bytes of randomness into zBuf.
*/
static int djyRandomness(sqlite3_vfs *pVfs, int nBuf, char *zBuf){
    memset(zBuf,0,nBuf);
    return nBuf;
}


/*
** Sleep for a little while.  Return the amount of time slept.
*/
static int djySleep(sqlite3_vfs *pVfs, int microsec){
  Djy_EventDelay(microsec);
  return microsec;
}

/*
** The following variable, if set to a non-zero value, is interpreted as
** the number of seconds since 1970 and is used to set the result of
** sqlite3OsCurrentTime() during testing.
*/
#ifdef SQLITE_TEST
int sqlite3_current_time = 0;  /* Fake system time in seconds since 1970. */
#endif

/*
** Find the current time (in Universal Coordinated Time).  Write into *piNow
** the current time and date as a Julian Day number times 86_400_000.  In
** other words, write into *piNow the number of milliseconds since the Julian
** epoch of noon in Greenwich on November 24, 4714 B.C according to the
** proleptic Gregorian calendar.
**
** On success, return SQLITE_OK.  Return SQLITE_ERROR if the time and date
** cannot be found.
*/
static int djyCurrentTimeInt64(sqlite3_vfs *pVfs, sqlite3_int64 *piNow){
  sqlite3_int64 i;
  TM_Time(&i);
  i += ((sqlite3_int64)2440587*24 +12)*3600;     //2490588是19700101的儒略日。
  i *= mS;

  return SQLITE_OK;
}

/*
** Find the current time (in Universal Coordinated Time).  Write the
** current time and date as a Julian Day number into *prNow and
** return 0.  Return 1 if the time and date cannot be found.
*/
static int djyCurrentTime(sqlite3_vfs *pVfs, double *prNow){
  *prNow = 0;       //本函数已经废弃
  return 0;
}

/*
** djy_error_login功能尚未把错误号与出错信息对应起来，本函数暂时空
*/
static int djyGetLastError(sqlite3_vfs *pVfs, int nBuf, char *zBuf){
  return 0;
}

/*
** Initialize and deinitialize the operating system interface.
*/
int sqlite3_os_init(void){
  static sqlite3_vfs djyVfs = {
    3,                   /* iVersion */
    sizeof(djyFile),     /* szOsFile */
    MAX_PATH,            /* mxPathname */
    0,                   /* pNext */
    "djyos",             /* zName */
    0,                   /* pAppData */
    djyOpen,             /* xOpen */
    djyDelete,           /* xDelete */
    djyAccess,           /* xAccess */
    djyFullPathname,     /* xFullPathname */
    djyDlOpen,           /* xDlOpen */
    djyDlError,          /* xDlError */
    djyDlSym,            /* xDlSym */
    djyDlClose,          /* xDlClose */
    djyRandomness,       /* xRandomness */
    djySleep,            /* xSleep */
    djyCurrentTime,      /* xCurrentTime */
    djyGetLastError,     /* xGetLastError */
    djyCurrentTimeInt64, /* xCurrentTimeInt64 */
    djySetSystemCall,    /* xSetSystemCall */
    djyGetSystemCall,    /* xGetSystemCall */
    djyNextSystemCall,   /* xNextSystemCall */
  };

  sqlite3_vfs_register(&djyVfs, 1);
  return SQLITE_OK;
}

int sqlite3_os_end(void){
  return SQLITE_OK;
}

#endif /* SQLITE_OS_DJYOS */
