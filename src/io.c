#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "process.h"
#include "util.h"

void copy_from_optarg ( char ** dest );
PROC_FILTER * check_input ( const char * command, const char * type, const char * filename );

PROC_FILTER * parse_input ( const int argc, char * const * argv )
{
    int opt;
    char * command_reg = NULL;
    char * type        = NULL;
    char * file_reg    = NULL;

    PROC_FILTER * res;

    while ( ( opt = getopt ( argc, argv, "c:t:f:" ) ) != -1 )
    {
        switch ( opt )
        {
            case 'c':
                copy_from_optarg ( &command_reg );
                break;
            case 't':
                copy_from_optarg ( &type );
                break;
            case 'f':
                copy_from_optarg ( &file_reg );
                break;
            default:
                fprintf ( stderr, "Unknown option: %c\n", opt );
                exit ( EXIT_FAILURE );
        }
    }

    res = check_input ( command_reg, type, file_reg );

    check_free ( command_reg );
    check_free ( type );
    check_free ( file_reg );

    return res;
}

void copy_from_optarg ( char ** dest )
{
    if ( *dest )
        free ( *dest );

    *dest = check_malloc ( sizeof ( char ) * ( strlen ( optarg ) + 1 ) );

    strcpy ( *dest, optarg );
}

PROC_FILTER * check_input ( const char * command, const char * type, const char * filename )
{
    const int c_flags = 0;

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
            res->command_reg = (regex_t *) check_malloc ( sizeof ( regex_t ) );
            regcomp ( res->command_reg, command, c_flags );
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
            res->filename_reg = (regex_t *) check_malloc ( sizeof ( regex_t ) );
            regcomp ( res->filename_reg, command, c_flags );
            regfree ( &temp );
        }
    }

    if ( type )
    {
        if ( strcmp ( type, "REG" ) == 0 || strcmp ( type, "CHR" ) == 0 || strcmp ( type, "DIR" ) == 0 || strcmp ( type, "FIFO" ) == 0 || strcmp ( type, "SOCK" ) == 0 ||
             strcmp ( type, "unknown" ) == 0 )
        {
            res->type_filter = (char *) check_malloc ( sizeof ( char ) * ( strlen ( type ) + 1 ) );
            strcpy ( res->type_filter, type );
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