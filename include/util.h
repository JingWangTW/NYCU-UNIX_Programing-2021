#ifndef __UTIL_H__
#define __UTIL_H__

#include <stddef.h>

/* Checking if pointer is NULL before release it.*/
void check_free ( void * ptr );

/* Checking memory allocation is done successfully */
void * check_malloc ( const size_t size );

/* Checking memory reallocation is done successfully */
void * check_realloc ( void * ptr, const size_t size );

/* Get human readable error string */
const char * get_error_message ( int err_no, const char * func_name, char * buffer );

#endif
