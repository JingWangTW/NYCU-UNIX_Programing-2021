#include "utility.h"

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#include "linux_cmd.h"

#define OUTPUT_MAX_LEN ( 1024 )

typedef struct tOUTPUT_LIST
{
    char output_str[OUTPUT_MAX_LEN];
    struct tOUTPUT_LIST * next;
} OUTPUT_LIST;

static int get_output_fd ( );
static void close_output_fd ( int fd );

static char * get_realpath ( const char * path );
static char * get_fd_file_name ( int fd );
static char * get_FILE_file_name ( FILE * file );
static char * get_output_str ( const char * str );
static char * get_output_long_str ( const char * str );

void logger_output ( const char * func_name, int param_cnt, ... )
{
    int output_fd;
    va_list parm_list;

    OUTPUT_LIST * list_head;
    OUTPUT_LIST * list_tail;
    OUTPUT_LIST * ret_str;
    OUTPUT_LIST * list_tmp;
    OUTPUT_TYPE type;

    int output_cnt;
    int output_cnt_tmp;
    char output_line[OUTPUT_MAX_LEN];
    char output_tmp[OUTPUT_MAX_LEN];

    unsigned long ulong_parm;
    long long_parm;
    int int_parm;
    void * ptr_parm;
    char * str_parm;
    FILE * file_parm;

    ret_str   = NULL;
    list_tmp  = NULL;
    list_head = NULL;
    list_tail = NULL;
    output_fd = get_output_fd ( );

    va_start ( parm_list, param_cnt );

    while ( param_cnt-- )
    {
        type = va_arg ( parm_list, OUTPUT_TYPE );

        switch ( type )
        {
            case UINT_LONG_DEC:
            {
                ulong_parm = va_arg ( parm_list, unsigned long );
                snprintf ( output_tmp, OUTPUT_MAX_LEN - 1, "%lu", ulong_parm );
                break;
            }
            case INT_LONG_DEC:
            {
                long_parm = va_arg ( parm_list, long );
                snprintf ( output_tmp, OUTPUT_MAX_LEN - 1, "%ld", long_parm );
                break;
            }
            case INT_OCT:
            {
                int_parm = va_arg ( parm_list, unsigned int );
                snprintf ( output_tmp, OUTPUT_MAX_LEN - 1, "%o", int_parm );
                break;
            }
            case INT_DEC:
            {
                int_parm = va_arg ( parm_list, int );
                snprintf ( output_tmp, OUTPUT_MAX_LEN - 1, "%d", int_parm );
                break;
            }
            case POINTER:
            {
                ptr_parm = va_arg ( parm_list, void * );
                snprintf ( output_tmp, OUTPUT_MAX_LEN - 1, "%p", ptr_parm );
                break;
            }
            case FILE_PTR:
            {
                file_parm = va_arg ( parm_list, void * );
                snprintf ( output_tmp, OUTPUT_MAX_LEN - 1, "\"%s\"", get_FILE_file_name ( file_parm ) );
                break;
            }
            case FD_NO:
            {
                int_parm = va_arg ( parm_list, int );
                snprintf ( output_tmp, OUTPUT_MAX_LEN - 1, "\"%s\"", get_fd_file_name ( int_parm ) );
                break;
            }
            case STRING:
            {
                str_parm = va_arg ( parm_list, char * );
                snprintf ( output_tmp, OUTPUT_MAX_LEN - 1, "\"%s\"", get_output_str ( str_parm ) );
                break;
            }
            case LONG_STR:
            {
                str_parm = va_arg ( parm_list, char * );
                snprintf ( output_tmp, OUTPUT_MAX_LEN - 1, "\"%s\"", get_output_long_str ( str_parm ) );
                break;
            }
            case PATH:
            {
                str_parm = va_arg ( parm_list, char * );
                snprintf ( output_tmp, OUTPUT_MAX_LEN - 1, "\"%s\"", get_realpath ( str_parm ) );
                break;
            }
            case VOID:
            {
                snprintf ( output_tmp, OUTPUT_MAX_LEN - 1, "void" );
                break;
            }
        }

        if ( ret_str == NULL )
        {
            ret_str = (OUTPUT_LIST *) malloc ( sizeof ( OUTPUT_LIST ) );

            list_tmp = ret_str;
        }
        else if ( list_head == NULL )
        {
            list_head = (OUTPUT_LIST *) malloc ( sizeof ( OUTPUT_LIST ) );
            list_tail = list_head;

            list_tmp = list_head;
        }
        else
        {
            list_tail->next = (OUTPUT_LIST *) malloc ( sizeof ( OUTPUT_LIST ) );
            list_tail       = list_tail->next;

            list_tmp = list_tail;
        }

        strncpy ( list_tmp->output_str, output_tmp, OUTPUT_MAX_LEN - 1 );
        list_tmp->next = NULL;
    }

    list_tmp       = list_head;
    output_cnt     = 0;
    output_cnt_tmp = 0;
    snprintf ( output_line, OUTPUT_MAX_LEN - 1, "[logger] %s\t( %n", func_name, &output_cnt_tmp );
    output_cnt = output_cnt_tmp;

    while ( list_tmp != NULL )
    {
        snprintf ( output_line + output_cnt, OUTPUT_MAX_LEN - output_cnt - 1, "%s%s %n", list_tmp->output_str, list_tmp == list_tail ? "" : ",", &output_cnt_tmp );

        list_tmp = list_tmp->next;

        free ( list_head );
        list_head = list_tmp;
        output_cnt += output_cnt_tmp;
    }

    snprintf ( output_line + output_cnt, OUTPUT_MAX_LEN - output_cnt - 1, ") = %s\n%n", ret_str == NULL ? "void" : ret_str->output_str, &output_cnt_tmp );

    if ( ret_str != NULL )
        free ( ret_str );

    output_cnt += output_cnt_tmp;

    linux_write ( output_fd, output_line, output_cnt );

    close_output_fd ( output_fd );
}

int get_output_fd ( )
{
    int output_fd;
    char * file_name = NULL;

    file_name = getenv ( "LOGGER_OUTPUT_FILE" );

    if ( file_name == NULL )
    {
        output_fd = fileno ( stderr );
    }
    else
    {
        output_fd = linux_open ( file_name, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
    }

    if ( output_fd == -1 )
    {
        fprintf ( stderr, "[Logger] Failed to open file %s. %s\n", file_name, strerror ( errno ) );
    }

    return output_fd;
}

void close_output_fd ( int fd )
{
    if ( fd != fileno ( stderr ) )
    {
        linux_close ( fd );
    }
}

char * get_realpath ( const char * path )
{
    static char resolve_path[MAXPATHLEN + 1];

    realpath ( path, resolve_path );

    return resolve_path;
}

char * get_realpath_r ( const char * path, char * resolved_path )
{
    return realpath ( path, resolved_path );
}

char * get_fd_file_name ( int fd )
{
    /* /proc/self/fd/{fd_num} */
    const int max_fd_filepath_len = 14 + 30;
    static char fd_filepath[14 + 30 + 1];

    snprintf ( fd_filepath, max_fd_filepath_len, "/proc/self/fd/%d", fd );

    return get_realpath ( fd_filepath );
}

char * get_fd_file_name_r ( int fd, char * file_name )
{
    /* /proc/self/fd/{fd_num} */
    const int max_fd_filepath_len = 14 + 30;
    static char fd_filepath[14 + 30 + 1];

    snprintf ( fd_filepath, max_fd_filepath_len, "/proc/self/fd/%d", fd );

    return get_realpath_r ( fd_filepath, file_name );
}

char * get_FILE_file_name ( FILE * file )
{
    return get_fd_file_name ( fileno ( file ) );
}

char * get_FILE_file_name_r ( FILE * file, char * file_name )
{
    return get_fd_file_name_r ( fileno ( file ), file_name );
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

static char * get_output_long_str ( const char * str )
{
    static char output_str[1024];
    size_t i;

    for ( i = 0; i < strlen ( str ) && i < 1023; i++ )
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
