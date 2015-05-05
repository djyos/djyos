#include    "os_inc.h"

#include    "GDD_Private.h"
#include    "GDD.h"
#include    <wchar.h>

#include    "lauxlib.h"
#include    "lua.h"

/*============================================================================*/

void _exit(void)
{
    djy_printf("err _exit.\r\n");
    while(1);

}

int _open(const char *path)
{
    djy_printf("err _open.\r\n");
    while(1);
    return 0;
}

int _read(int fd)
{
    djy_printf("err _read.\r\n");
    while(1);

    return 0;
}

int _write(int fd)
{
    djy_printf("err _write.\r\n");
    while(1);

    return 0;
}

int _fstat(int fd)
{
    djy_printf("err _fstat.\r\n");
    while(1);

    return 0;
}

int _close(int fd)
{
    djy_printf("err _close.\r\n");
    while(1);

    return 0;
}

int _fini(int fd)
{
    djy_printf("err _fini.\r\n");
    while(1);

    return 0;
}

int _fseek(int fd)
{
    djy_printf("err _fseek.\r\n");
    while(1);

    return 0;
}

int _lseek(int fd)
{
    djy_printf("err _lseek.\r\n");
    while(1);

    return 0;
}
/*============================================================================*/

void    lux_x_free(void *p)
{
    M_Free(p);
}

void*   lua_x_realloc(void *p,int size)
{
    void *new;

    new =M_Malloc(size,1000*mS);

    if(p!=NULL)
    {
        memcpy(new,p,size);
        M_Free(p);
    }

    return new;

}


/*============================================================================*/


void *_sbrk(int size)
{
    djy_printf("err _sbrk.\r\n");
    while(1);

    return M_Malloc(size,1000*mS);
}

int _isatty(int fd)
{
    djy_printf("err _isatty.\r\n");
    while(1);

    return 0;
}

/*============================================================================*/

double floor(double a)
{
    volatile int i;
    volatile double f;

    f=a;
    if(f<0)
    {
        f =f-0.5;
    }
    i=(int)f;

    f=i;
    return f;

    //djy_printf("err floor.\r\n");
    //while(1);

}

int pow(void)
{
    djy_printf("err pow.\r\n");
    while(1);

    return 0;
}
/*============================================================================*/
