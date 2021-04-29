
#include "linux_cmd.h"
#include "utility.h"

int close ( int fd )
{
    int ret_value;

    ret_value = linux_close ( fd );

    logger_output ( "close", 2, INT_DEC, ret_value, FD_NO, fd );

    return ret_value;
}