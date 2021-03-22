#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "util.h"

void copy_from_optarg ( char ** dest );
ARG * check_input ( const char * command, const char * type, const char * filename );

ARG * parse_input ( const int argc, char * const * argv )
{
    int opt;
    char * command_reg = NULL;
    char * type        = NULL;
    char * file_reg    = NULL;

    ARG * res;

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

    printf ( "%s\n%s\n%s\n", command_reg, type, file_reg );

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

ARG * check_input ( const char * command, const char * type, const char * filename )
{
    const int c_flags = 0;

    ARG * res = (ARG *) check_malloc ( sizeof ( ARG ) );
    memset ( res, 0, sizeof ( ARG ) );

    if ( command )
    {
        if ( regcomp ( &( res->command_reg ), command, c_flags ) != 0 )
        {
            fprintf ( stderr, "Command Regex: \"%s\" is not a valid regex\n", command );
            check_free ( res );
            return NULL;
        }
    }

    if ( filename )
    {
        if ( regcomp ( &( res->filename_reg ), filename, c_flags ) != 0 )
        {
            fprintf ( stderr, "Name Regex: \"%s\" is not a valid regex\n", filename );
            check_free ( res );
            return NULL;
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