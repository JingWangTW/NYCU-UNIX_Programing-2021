#ifndef __UTIL_H__
#define __UTIL_H__

#include <stddef.h>

// MAX macro with type safe
#define MAX( a, b )                  \
    ( {                              \
        __typeof__ ( a ) _a = ( a ); \
        __typeof__ ( b ) _b = ( b ); \
        _a > _b ? _a : _b;           \
    } )

// MIN macro with type safe
#define MIN( a, b )                  \
    ( {                              \
        __typeof__ ( a ) _a = ( a ); \
        __typeof__ ( b ) _b = ( b ); \
        _a < _b ? _a : _b;           \
    } )

/* Checking if pointer is NULL before release it.*/
void check_free ( void * ptr );

/* Checking memory allocation is done successfully */
void * check_malloc ( const size_t size );

/* Checking memory reallocation is done successfully */
void * check_realloc ( void * ptr, const size_t size );

/* Get human readable error string */
const char * get_error_message ( int err_no, const char * func_name, char * buffer, size_t buflen );

/* A wrap function to wrap standard strncp */
/* Will append '\0' at the end of string */
char * strncpy_append ( char * destination, const char * source, size_t num );

#endif
