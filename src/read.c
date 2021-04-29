#include <stdio.h>

#include "linux_cmd.h"
#include "utility.h"

ssize_t read ( int fd, void * buf, size_t count )
{
    ssize_t ret_value;

    ret_value = linux_read ( fd, buf, count );

    logger_output ( "read", 4, INT_LONG_DEC, ret_value, FD_NO, fd, STRING, buf, UINT_LONG_DEC, count );

    return ret_value;
}