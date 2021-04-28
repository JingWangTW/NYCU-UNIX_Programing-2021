#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>

#include "linux_cmd.h"

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

void close_output_file ( FILE * output_file )
{
    if ( output_file != stderr )
    {
        linux_fclose ( output_file );
    }
}

char * get_realpath ( const char * path )
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