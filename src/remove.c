#include "linux_cmd.h"
#include "utility.h"

int remove ( const char * pathname )
{
    int ret_value;

    ret_value = linux_remove ( pathname );

    logger_output ( "remove", 2, INT_DEC, ret_value, PATH, pathname );

    return ret_value;
}