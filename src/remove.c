#include <sys/param.h>

#include "linux_cmd.h"
#include "utility.h"

int remove ( const char * pathname )
{
    int ret_value;
    char file_name[MAXPATHLEN + 1];

    get_realpath_r ( pathname, file_name );

    ret_value = linux_remove ( pathname );

    logger_output ( "remove", 2, INT_DEC, ret_value, LONG_STR, file_name );

    return ret_value;
}