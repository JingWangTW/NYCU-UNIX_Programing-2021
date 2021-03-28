#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void check_free ( void * ptr )
{
    if ( ptr != NULL )
        free ( ptr );
}

void * check_malloc ( const size_t size )
{
    void * ptr = malloc ( size );

    if ( ptr == NULL )
    {
        fprintf ( stderr, "Outof Memory." );
        exit ( EXIT_FAILURE );
    }

    return ptr;
}

void * check_realloc ( void * ptr, const size_t size )
{
    void * ptr_new = realloc ( ptr, size );

    if ( ptr_new == NULL )
    {
        fprintf ( stderr, "Outof Memory." );
        exit ( EXIT_FAILURE );
    }

    return ptr_new;
}

const char * get_error_message ( int err_no, const char * func_name, char * buffer )
{
    sprintf ( buffer, "(%s: %s)", func_name, strerror ( err_no ) );

    return buffer;
}

char * strncpy_append ( char * destination, const char * source, size_t num )
{
    strncpy ( destination, source, num );
    destination[num] = '\0';
    return destination;
}