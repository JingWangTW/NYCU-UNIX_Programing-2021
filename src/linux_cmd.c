#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <sys/stat.h>

int linux_chmod ( const char * pathname, mode_t mode )
{
    static int ( *__chmod ) ( const char *, mode_t ) = NULL;

    if ( __chmod == NULL )
    {
        __chmod = dlsym ( RTLD_NEXT, "chmod" );
    }

    return __chmod ( pathname, mode );
}

int linux_chown ( const char * pathname, uid_t owner, gid_t group )
{
    static int ( *__chown ) ( const char *, uid_t, gid_t ) = NULL;

    if ( __chown == NULL )
    {
        __chown = dlsym ( RTLD_NEXT, "chown" );
    }

    return __chown ( pathname, owner, group );
}

int linux_creat ( const char * pathname, mode_t mode )
{
    static int ( *__creat ) ( const char *, mode_t ) = NULL;

    if ( __creat == NULL )
    {
        __creat = dlsym ( RTLD_NEXT, "creat" );
    }

    return __creat ( pathname, mode );
}

int linux_creat64 ( const char * pathname, mode_t mode )
{
    static int ( *__creat64 ) ( const char *, mode_t ) = NULL;

    if ( __creat64 == NULL )
    {
        __creat64 = dlsym ( RTLD_NEXT, "creat64" );
    }

    return __creat64 ( pathname, mode );
}

int linux_remove ( const char * pathname )
{
    static int ( *__remove ) ( const char * ) = NULL;

    if ( __remove == NULL )
    {
        __remove = dlsym ( RTLD_NEXT, "remove" );
    }

    return __remove ( pathname );
}

int linux_rename ( const char * oldpath, const char * newpath )
{
    static int ( *__rename ) ( const char *, const char * ) = NULL;

    if ( __rename == NULL )
    {
        __rename = dlsym ( RTLD_NEXT, "rename" );
    }

    return __rename ( oldpath, newpath );
}

int linux_open ( const char * pathname, int flags, mode_t mode )
{
    static int ( *__open ) ( const char *, int, mode_t ) = NULL;

    if ( __open == NULL )
    {
        __open = dlsym ( RTLD_NEXT, "open" );
    }

    return __open ( pathname, flags, mode );
}

int linux_open64 ( const char * pathname, int flags, mode_t mode )
{
    static int ( *__open64 ) ( const char *, int, mode_t ) = NULL;

    if ( __open64 == NULL )
    {
        __open64 = dlsym ( RTLD_NEXT, "open" );
    }

    return __open64 ( pathname, flags, mode );
}

int linux_close ( int fd )
{
    static int ( *__close ) ( int ) = NULL;

    if ( __close == NULL )
    {
        __close = dlsym ( RTLD_NEXT, "close" );
    }

    return __close ( fd );
}

ssize_t linux_read ( int fd, void * buf, size_t count )
{
    static int ( *__read ) ( int, void *, size_t ) = NULL;

    if ( __read == NULL )
    {
        __read = dlsym ( RTLD_NEXT, "read" );
    }

    return __read ( fd, buf, count );
}

ssize_t linux_write ( int fd, const void * buf, size_t count )
{
    static ssize_t ( *__write ) ( int, const void *, size_t ) = NULL;

    if ( __write == NULL )
    {
        __write = dlsym ( RTLD_NEXT, "write" );
    }

    return __write ( fd, buf, count );
}

FILE * linux_fopen ( const char * pathname, const char * mode )
{
    static FILE * ( *__fopen ) ( const char *, const char * ) = NULL;

    if ( __fopen == NULL )
    {
        __fopen = dlsym ( RTLD_NEXT, "fopen" );
    }

    return __fopen ( pathname, mode );
}

FILE * linux_fopen64 ( const char * pathname, const char * mode )
{
    static FILE * ( *__fopen64 ) ( const char *, const char * ) = NULL;

    if ( __fopen64 == NULL )
    {
        __fopen64 = dlsym ( RTLD_NEXT, "fopen64" );
    }

    return __fopen64 ( pathname, mode );
}

int linux_fclose ( FILE * stream )
{
    static int ( *__fclose ) ( FILE * ) = NULL;

    if ( __fclose == NULL )
    {
        __fclose = dlsym ( RTLD_NEXT, "fclose" );
    }

    return __fclose ( stream );
}

size_t linux_fread ( void * ptr, size_t size, size_t nmemb, FILE * stream )
{
    static size_t ( *__fread ) ( void *, size_t, size_t, FILE * );

    if ( __fread == NULL )
    {
        __fread = dlsym ( RTLD_NEXT, "fread" );
    }

    return __fread ( ptr, size, nmemb, stream );
}

size_t linux_fwrite ( const void * ptr, size_t size, size_t nmemb, FILE * stream )
{
    static size_t ( *__fwrite ) ( const void *, size_t, size_t, FILE * );

    if ( __fwrite == NULL )
    {
        __fwrite = dlsym ( RTLD_NEXT, "fwrite" );
    }

    return __fwrite ( ptr, size, nmemb, stream );
}

FILE * linux_tmpfile ( )
{
    static FILE * ( *__tmpfile ) ( ) = NULL;

    if ( __tmpfile == NULL )
    {
        __tmpfile = dlsym ( RTLD_NEXT, "tmpfile" );
    }

    return __tmpfile ( );
}

FILE * linux_tmpfile64 ( )
{
    static FILE * ( *__tmpfile64 ) ( ) = NULL;

    if ( __tmpfile64 == NULL )
    {
        __tmpfile64 = dlsym ( RTLD_NEXT, "tmpfile64" );
    }

    return __tmpfile64 ( );
}
