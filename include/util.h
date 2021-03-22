#ifndef __UTIL_H__
#define __UTIL_H__

#include <stddef.h>

/* Checking if pointer is NULL before release it.*/
void check_free ( void * ptr );

/* Checking memory allocation is done successfully */
void * check_malloc ( const size_t size );

#endif
