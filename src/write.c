#include <unistd.h>

#include "linux_cmd.h"
#include "utility.h"

ssize_t write ( int fd, const void * buf, size_t count )
{
    ssize_t ret_value;

    ret_value = linux_write ( fd, buf, count );

    logger_output ( "write", 4, INT_LONG, ret_value, FD_NO, fd, STRING, buf, INT_DEC, count );

    return ret_value;
}