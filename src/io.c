#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "process.h"
#include "util.h"

void copy_from_optarg ( char ** dest );
PROC_FILTER * check_input ( const char * command, const char * type, const char * filename );

PROC_FILTER * parse_input ( const int argc, char * const * argv )
{
    int opt;
    char * command_regex_str   = NULL;
    char * file_type_input_str = NULL;
    char * filename_regex_str  = NULL;

    PROC_FILTER * res;

    while ( ( opt = getopt ( argc, argv, "c:t:f:" ) ) != -1 )
    {
        switch ( opt )
        {
            case 'c':
                copy_from_optarg ( &command_regex_str );
                break;
            case 't':
                copy_from_optarg ( &file_type_input_str );
                break;
            case 'f':
                copy_from_optarg ( &filename_regex_str );
                break;
            default:
                fprintf ( stderr, "Unknown option: %c\n", opt );
                exit ( EXIT_FAILURE );
        }
    }

    res = check_input ( command_regex_str, file_type_input_str, filename_regex_str );

    check_free ( command_regex_str );
    check_free ( file_type_input_str );
    check_free ( filename_regex_str );

    return res;
}

void print_result ( FILE_LIST ** file_list, int size )
{
    int proc_cnt;
    int is_first_line = 1;
    char format_string[128];
    char field_buffer[MAX ( PID_STR_LEN_MAX, INODE_STR_LEN_MAX )];
    char type_buffer[FILE_TYPE_STR_LEN_MAX];
    char node_buffer[INODE_STR_LEN_MAX];

    // the initial value is the length of col title
    size_t max_command = 7;
    size_t max_pid     = 3;
    size_t max_user    = 4;
    size_t max_fd      = 2;
    const int max_type = 7;  // length of "unknown"
    size_t max_node    = 4;

    FILE_LIST * head;

    if ( file_list == NULL )
        return;

    for ( proc_cnt = 0; proc_cnt < size; proc_cnt++ )
    {
        head = file_list[proc_cnt];

        while ( head != NULL )
        {
            max_command = MIN ( MAX ( max_command, strnlen ( head->file_path, FILE_PATH_MAX - 1 ) ), (size_t) 9 );

            snprintf ( field_buffer, PID_STR_LEN_MAX, "%d", head->pid );
            max_pid = MAX ( max_pid, strnlen ( field_buffer, PID_STR_LEN_MAX - 1 ) );

            max_user = MAX ( max_user, strnlen ( head->username, USERNAME_MAX - 1 ) );

            max_fd = MAX ( max_fd, strnlen ( head->file_descriptior, FILE_DESCRIPTOR_MAX - 1 ) );

            snprintf ( field_buffer, INODE_STR_LEN_MAX, "%ld", head->inode_number );
            max_node = MAX ( max_node, strnlen ( field_buffer, INODE_STR_LEN_MAX - 1 ) );

            head = head->next;
        }
    }

    snprintf ( format_string, 127, "%%-%lds %%-%ldd %%-%lds %%-%lds %%-%ds %%-%lds %%s\n", max_command, max_pid, max_user, max_fd, max_type, max_node );

    is_first_line = 1;
    for ( proc_cnt = 0; proc_cnt < size; proc_cnt++ )
    {
        head = file_list[proc_cnt];

        while ( head != NULL )
        {
            get_type_str ( head, type_buffer );
            get_node_str ( head, node_buffer );

            head->command[9] = '\0';

            // Print first titile
            if ( is_first_line )
            {
                printf ( "%-*s %-*s %-*s %-*s %-*s %-*s %s\n", (int) max_command, "COMMAND", (int) max_pid, "PID", (int) max_user, "USER", (int) max_fd, "FD", max_type, "TYPE", (int) max_node, "NODE",
                         "NAME" );

                is_first_line = 0;
            }

            printf ( format_string, head->command, head->pid, head->username, head->file_descriptior, type_buffer, node_buffer, head->file_path );
            head = head->next;
        }
    }
}

void copy_from_optarg ( char ** dest )
{
    if ( *dest )
        check_free ( *dest );

    *dest = check_malloc ( sizeof ( char ) * ( strlen ( optarg ) + 1 ) );

    strncpy_append ( *dest, optarg, strlen ( optarg ) );
}

PROC_FILTER * check_input ( const char * command, const char * type, const char * filename )
{
    /* Flags to accept ^ and $ */
    const int c_flags = REG_NOTBOL;

    regex_t temp;
    PROC_FILTER * res = (PROC_FILTER *) check_malloc ( sizeof ( PROC_FILTER ) );
    memset ( res, 0, sizeof ( PROC_FILTER ) );

    if ( command )
    {
        if ( regcomp ( &temp, command, c_flags ) != 0 )
        {
            fprintf ( stderr, "Command Regex: \"%s\" is not a valid regex\n", command );
            check_free ( res );
            return NULL;
        }
        else
        {
            res->command_regex = (regex_t *) check_malloc ( sizeof ( regex_t ) );
            regcomp ( res->command_regex, command, c_flags );
            regfree ( &temp );
        }
    }

    if ( filename )
    {
        if ( regcomp ( &temp, filename, c_flags ) != 0 )
        {
            fprintf ( stderr, "Name Regex: \"%s\" is not a valid regex\n", filename );
            check_free ( res );
            return NULL;
        }
        else
        {
            res->filename_regex = (regex_t *) check_malloc ( sizeof ( regex_t ) );
            regcomp ( res->filename_regex, filename, c_flags );
            regfree ( &temp );
        }
    }

    if ( type )
    {
        if ( strcmp ( type, "REG" ) == 0 || strcmp ( type, "CHR" ) == 0 || strcmp ( type, "DIR" ) == 0 || strcmp ( type, "FIFO" ) == 0 || strcmp ( type, "SOCK" ) == 0 ||
             strcmp ( type, "unknown" ) == 0 )
        {
            res->type_filter = (char *) check_malloc ( sizeof ( char ) * ( strnlen ( type, FILE_TYPE_STR_LEN_MAX ) + 1 ) );
            strncpy_append ( res->type_filter, type, FILE_TYPE_STR_LEN_MAX );
        }
        else
        {
            fprintf ( stderr, "Invalid TYPE option.\n" );
            check_free ( res );
            return NULL;
        }
    }

    return res;
}