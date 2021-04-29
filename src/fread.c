#include <stdio.h>

#include "linux_cmd.h"
#include "utility.h"

size_t fread ( void * ptr, size_t size, size_t nmemb, FILE * stream )
{
    size_t ret_value;

    ret_value = linux_fread ( ptr, size, nmemb, stream );

    logger_output ( "fread", 5, INT_LONG, ret_value, STRING, ptr, INT_LONG, size, INT_LONG, nmemb, FILE_PTR, stream );

    return ret_value;
}