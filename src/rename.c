#include <sys/param.h>

#include "linux_cmd.h"
#include "utility.h"

int rename ( const char * oldpath, const char * newpath )
{
    int ret_value;
    char file_name[MAXPATHLEN + 1];

    get_realpath_r ( oldpath, file_name );

    ret_value = linux_rename ( oldpath, newpath );

    logger_output ( "rename", 3, INT_DEC, ret_value, LONG_STR, oldpath, PATH, newpath );

    return ret_value;
}