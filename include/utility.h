#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <stdio.h>

#ifndef O_TMPFILE
#define O_TMPFILE ( __O_TMPFILE | O_DIRECTORY )
#endif

typedef enum
{
    INT_OCT,
    INT_DEC,
    INT_LONG_DEC,
    UINT_LONG_DEC,

    POINTER,

    FILE_PTR,
    FD_NO,

    STRING,
    LONG_STR,

    PATH,

    VOID,
} OUTPUT_TYPE;

void logger_output ( const char * func_name, int param_cnt, ... );

char * get_realpath_r ( const char * path, char * resolved_path );
char * get_fd_file_name_r ( int fd, char * file_name );
char * get_FILE_file_name_r ( FILE * file, char * file_name );

#endif