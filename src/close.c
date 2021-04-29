
#include <sys/param.h>

#include "linux_cmd.h"
#include "utility.h"

int close ( int fd )
{
    int ret_value;
    char file_name[MAXPATHLEN + 1];

    get_fd_file_name_r ( fd, file_name );

    ret_value = linux_close ( fd );

    logger_output ( "close", 2, INT_DEC, ret_value, LONG_STR, file_name );

    return ret_value;
}