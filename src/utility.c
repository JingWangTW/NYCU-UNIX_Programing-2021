#define _GNU_SOURCE

#include <ctype.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

static int ( *linux_chmod ) ( const char *, mode_t )       = NULL;
static int ( *linux_chown ) ( const char *, uid_t, gid_t ) = NULL;
static int ( *linux_close ) ( int )                        = NULL;
static int ( *linux_open ) ( const char *, int, ... )      = NULL;
static int ( *linux_read ) ( int, void *, size_t )         = NULL;
static int ( *linux_remove ) ( const char * )              = NULL;
static int ( *linux_creat ) ( const char *, mode_t )       = NULL;
static int ( *linux_rename ) ( const char *, const char * );

FILE * get_output_file ( )
{
    FILE * output_file = NULL;
    char * file_name   = NULL;

    file_name = getenv ( "LOGGER_OUTPUT_FILE" );

    if ( file_name == NULL )
    {
        output_file = stderr;
    }
    else
    {
        output_file = fopen ( file_name, "a" );
    }

    if ( output_file == NULL )
    {
        fprintf ( stderr, "Failed to open file %s.\n", file_name );
    }

    return output_file;
}

void * get_linux_func ( const char * func_name )
{
    void * ret = NULL;

    if ( strcmp ( func_name, "chmod" ) == 0 )
    {
        if ( linux_chmod == NULL )
        {
            linux_chmod = dlsym ( RTLD_NEXT, "chmod" );
        }
        ret = linux_chmod;
    }
    else if ( strcmp ( func_name, "chown" ) == 0 )
    {
        if ( linux_chown == NULL )
        {
            linux_chown = dlsym ( RTLD_NEXT, "chown" );
        }
        ret = linux_chown;
    }
    else if ( strcmp ( func_name, "close" ) == 0 )
    {
        if ( linux_close == NULL )
        {
            linux_close = dlsym ( RTLD_NEXT, "close" );
        }
        ret = linux_close;
    }
    else if ( strcmp ( func_name, "open" ) == 0 )
    {
        if ( linux_open == NULL )
        {
            linux_open = dlsym ( RTLD_NEXT, "open" );
        }
        ret = linux_open;
    }
    else if ( strcmp ( func_name, "read" ) == 0 )
    {
        if ( linux_read == NULL )
        {
            linux_read = dlsym ( RTLD_NEXT, "read" );
        }
        ret = linux_read;
    }
    else if ( strcmp ( func_name, "remove" ) == 0 )
    {
        if ( linux_remove == NULL )
        {
            linux_remove = dlsym ( RTLD_NEXT, "remove" );
        }
        ret = linux_remove;
    }
    else if ( strcmp ( func_name, "creat" ) == 0 )
    {
        if ( linux_creat == NULL )
        {
            linux_creat = dlsym ( RTLD_NEXT, "creat" );
        }
        ret = linux_creat;
    }
    else if ( strcmp ( func_name, "rename" ) == 0 )
    {
        if ( linux_rename == NULL )
        {
            linux_rename = dlsym ( RTLD_NEXT, "rename" );
        }
        ret = linux_rename;
    }

    if ( ret == NULL )
    {
        fprintf ( stderr, "Cannot find symbol %s\n", func_name );
        exit ( -1 );
    }

    return ret;
}

void close_output_file ( FILE * output_file )
{
    if ( output_file == stderr )
    {
        fclose ( output_file );
    }
}

char * get_realpath ( char * path )
{
    static char resolve_path[MAXPATHLEN + 1];

    realpath ( path, resolve_path );

    return resolve_path;
}

char * get_fd_file_name ( int fd )
{
    /* /proc/self/fd/{fd_num} */
    const int max_fd_filepath_len = 14 + 30;
    static char fd_filepath[14 + 30 + 1];

    snprintf ( fd_filepath, max_fd_filepath_len, "/proc/self/fd/%d", fd );

    return get_realpath ( fd_filepath );
}

char * get_FILE_file_name ( FILE * file )
{
    return get_fd_file_name ( fileno ( file ) );
}

char * get_output_str ( const char * str )
{
    static char output_str[32 + 1];
    size_t i;

    for ( i = 0; i < strlen ( str ) && i < 32; i++ )
    {
        if ( isprint ( str[i] ) )
        {
            output_str[i] = str[i];
        }
        else
        {
            output_str[i] = '.';
        }
    }

    output_str[i] = '\0';

    return output_str;
}