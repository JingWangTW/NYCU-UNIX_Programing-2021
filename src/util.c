#include <stdio.h>
#include <stdlib.h>

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