#include <sys/stat.h>

#include "linux_cmd.h"
#include "utility.h"

int creat ( const char * pathname, mode_t mode )
{
    int ret_value;

    ret_value = linux_creat ( pathname, mode );

    logger_output ( "creat", 3, INT_DEC, ret_value, PATH, pathname, INT_OCT, mode );

    return ret_value;
}

int creat64 ( const char * pathname, mode_t mode )
{
    int ret_value;

    ret_value = linux_creat64 ( pathname, mode );

    logger_output ( "creat", 3, INT_DEC, ret_value, PATH, pathname, INT_OCT, mode );

    return ret_value;
}