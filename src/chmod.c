#include <sys/stat.h>

#include "linux_cmd.h"
#include "utility.h"

int chmod ( const char * pathname, mode_t mode )
{
    int ret_value;

    ret_value = linux_chmod ( pathname, mode );

    logger_output ( "chmod", 3, INT_DEC, ret_value, PATH, pathname, INT_OCT, mode );

    return ret_value;
}