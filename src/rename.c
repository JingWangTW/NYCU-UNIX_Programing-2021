#include "linux_cmd.h"
#include "utility.h"

int rename ( const char * oldpath, const char * newpath )
{
    int ret_value;

    ret_value = linux_rename ( oldpath, newpath );

    logger_output ( "rename", 3, INT_DEC, ret_value, PATH, oldpath, PATH, newpath );

    return ret_value;
}