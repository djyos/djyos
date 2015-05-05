/* errno is not a global variable, because that would make using it
   non-reentrant.  Instead, its address is returned by the function
   __errno.  */

#ifndef _SYS_ERRNO_H_
#define _SYS_ERRNO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
//出错enum常量的定义，所有的出错常量被定义为enum，enum的成员是一个整型常量，该
//常量的高8位用来区别出错模块名称，高8位中1~127由系统使用，128~255应用程序使用。
//如下表定义:
#define CN_KNL_NO_ERROR     (1<<24)         //系统调度起始错误号
#define CN_FS_NO_ERROR      (2<<24)         //文件系统起始错误号
#define CN_MEM_NO_ERROR     (3<<24)         //内存模块起始错误号
#define CN_DRV_NO_ERROR     (4<<24)         //pan driver模块起始错误号
#define CN_WDT_NO_ERROR     (5<<24)         //wdt模块起始错误号
#define CN_GK_NO_ERROR      (6<<24)         //gui kernel模块起始错误号
#define CN_LOCK_NO_ERROR    (7<<24)         //锁模块起始错误号
#define CN_CLOCK_NO_ERROR   (8<<24)         //时钟模块起始错误号
#define CN_NEWLIB_NO_ERROR  (255<<24)       //newlib库起始错误号。

enum _NEWLIB_ERROR_CODE_
{
    EN_NEWLIB_NO_ERROR = CN_NEWLIB_NO_ERROR,  //没有错误
    EPERM,       /* Not super-user */
    ENOENT,      /* No such file or directory */
    ESRCH,       /* No such process */
    EINTR,       /* Interrupted system call */
    EIO,         /* I/O error */
    ENXIO,       /* No such device or address */
    E2BIG,       /* Arg list too long */
    ENOEXEC,     /* Exec format error */
    EBADF,       /* Bad file number */
    ECHILD,      /* No children */
    EAGAIN,      /* No more processes */
    ENOMEM,      /* Not enough core */
    EACCES,      /* Permission denied */
    EFAULT,      /* Bad address */
//#ifdef __LINUX_ERRNO_EXTENSIONS__
    ENOTBLK,     /* Block device required */
//#endif
    EBUSY,       /* Mount device busy */
    EEXIST,      /* File exists */
    EXDEV,       /* Cross-device link */
    ENODEV,      /* No such device */
    ENOTDIR,     /* Not a directory */
    EISDIR,      /* Is a directory */
    EINVAL,      /* Invalid argument */
    ENFILE,      /* Too many open files in system */
    EMFILE,      /* Too many open files */
    ENOTTY,      /* Not a typewriter */
    ETXTBSY,     /* Text file busy */
    EFBIG,       /* File too large */
    ENOSPC,      /* No space left on device */
    ESPIPE,      /* Illegal seek */
    EROFS,       /* Read only file system */
    EMLINK,      /* Too many links */
    EPIPE,       /* Broken pipe */
    EDOM,        /* Math arg out of domain of func */
    ERANGE,      /* Math result not representable */
    ENOMSG,      /* No message of desired type */
    EIDRM,       /* Identifier removed */
//#ifdef __LINUX_ERRNO_EXTENSIONS__
    ECHRNG,      /* Channel number out of range */
    EL2NSYNC,    /* Level   not synchronized */
    EL3HLT,      /* Level   halted */
    EL3RST,      /* Level   reset */
    ELNRNG,      /* Link number out of range */
    EUNATCH,     /* Protocol driver not attached */
    ENOCSI,      /* No CSI structure available */
    EL2HLT,      /* Level   halted */
//#endif
    EDEADLK,     /* Deadlock condition */
    ENOLCK,      /* No record locks available */
//#ifdef __LINUX_ERRNO_EXTENSIONS__
    EBADE,       /* Invalid exchange */
    EBADR,       /* Invalid request descriptor */
    EXFULL,      /* Exchange full */
    ENOANO,      /* No anode */
    EBADRQC,     /* Invalid request code */
    EBADSLT,     /* Invalid slot */
    EDEADLOCK,       /* File locking deadlock error */
    EBFONT,      /* Bad font file fmt */
//#endif
    ENOSTR,      /* Device not a stream */
    ENODATA,     /* No data (for no delay io) */
    ETIME,       /* Timer expired */
    ENOSR,       /* Out of streams resources */
//#ifdef __LINUX_ERRNO_EXTENSIONS__
    ENONET,      /* Machine is not on the network */
    ENOPKG,      /* Package not installed */
    EREMOTE,     /* The object is remote */
//#endif
    ENOLINK,     /* The link has been severed */
//#ifdef __LINUX_ERRNO_EXTENSIONS__
    EADV,        /* Advertise error */
    ESRMNT,      /* Srmount error */
    ECOMM,       /* Communication error on send */
//#endif
    EPROTO,      /* Protocol error */
    EMULTIHOP,       /* Multihop attempted */
//#ifdef __LINUX_ERRNO_EXTENSIONS__
    ELBIN,       /* Inode is remote (not really error) */
    EDOTDOT,     /* Cross mount point (not really error) */
//#endif
    EBADMSG,     /* Trying to read unreadable message */
    EFTYPE,      /* Inappropriate file type or format */
//#ifdef __LINUX_ERRNO_EXTENSIONS__
    ENOTUNIQ,    /* Given log. name not unique */
    EBADFD,      /* f.d. invalid for this operation */
    EREMCHG,     /* Remote address changed */
    ELIBACC,     /* Can't access a needed shared lib */
    ELIBBAD,     /* Accessing a corrupted shared lib */
    ELIBSCN,     /* .lib section in a.out corrupted */
    ELIBMAX,     /* Attempting to link in too many libs */
    ELIBEXEC,    /* Attempting to exec a shared library */
//#endif
    ENOSYS,      /* Function not implemented */
//#ifdef __CYGWIN__
    ENMFILE,         /* No more files */
//#endif
    ENOTEMPTY,       /* Directory not empty */
    ENAMETOOLONG,    /* File or path name too long */
    ELOOP,       /* Too many symbolic links */
    EOPNOTSUPP,      /* Operation not supported on transport endpoint */
    EPFNOSUPPORT,    /* Protocol family not supported */
    ECONNRESET,      /* Connection reset by peer */
    ENOBUFS,     /* No buffer space available */
    EAFNOSUPPORT,     /* Address family not supported by protocol family */
    EPROTOTYPE,      /* Protocol wrong type for socket */
    ENOTSOCK,        /* Socket operation on non-socket */
    ENOPROTOOPT,     /* Protocol not available */
//#ifdef __LINUX_ERRNO_EXTENSIONS__
    ESHUTDOWN,       /* Can't send after socket shutdown */
//#endif
    ECONNREFUSED,        /* Connection refused */
    EADDRINUSE,          /* Address already in use */
    ECONNABORTED,        /* Connection aborted */
    ENETUNREACH,         /* Network is unreachable */
    ENETDOWN,            /* Network interface is not configured */
    ETIMEDOUT,           /* Connection timed out */
    EHOSTDOWN,           /* Host is down */
    EHOSTUNREACH,        /* Host is unreachable */
    EINPROGRESS,         /* Connection already in progress */
    EALREADY,            /* Socket already connected */
    EDESTADDRREQ,        /* Destination address required */
    EMSGSIZE,            /* Message too long */
    EPROTONOSUPPORT,     /* Unknown protocol */
//#ifdef __LINUX_ERRNO_EXTENSIONS__
    ESOCKTNOSUPPORT,     /* Socket type not supported */
//#endif
    EADDRNOTAVAIL,       /* Address not available */
    ENETRESET,
    EISCONN,         /* Socket is already connected */
    ENOTCONN,            /* Socket is not connected */
    ETOOMANYREFS,
//#ifdef __LINUX_ERRNO_EXTENSIONS__
    EPROCLIM,
    EUSERS,
//#endif
    EDQUOT,
    ESTALE,
    ENOTSUP,         /* Not supported */
//#ifdef __LINUX_ERRNO_EXTENSIONS__
    ENOMEDIUM,       /* No medium (in tape drive) */
//#endif
//#ifdef __CYGWIN__
    ENOSHARE,        /* No such host or network path */
    ECASECLASH,      /* Filename exists with different case */
//#endif
    EILSEQ,
    EOVERFLOW,       /* Value too large for defined data type */
    ECANCELED,       /* Operation canceled */
    ENOTRECOVERABLE,     /* State not recoverable */
    EOWNERDEAD,      /* Previous owner died */
//#ifdef __LINUX_ERRNO_EXTENSIONS__
    ESTRPIPE,        /* Streams pipe error */
//#endif
    EWOULDBLOCK = EAGAIN,  /* Operation would block */

    __ELASTERROR,        /* Users can add values starting here */
};
void Djy_SaveLastError(u32 ErrorCode);
u32 Djy_GetLastError(void);
#define errno Djy_GetLastError()

#ifdef __cplusplus
}
#endif
#endif /* _SYS_ERRNO_H */
