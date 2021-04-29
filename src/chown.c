#include <sys/stat.h>

#include "linux_cmd.h"
#include "utility.h"

int chown ( const char * pathname, uid_t owner, gid_t group )
{
    int ret_value;

    ret_value = linux_chown ( pathname, owner, group );

    logger_output ( "chown", 4, INT_DEC, ret_value, PATH, pathname, INT_DEC, owner, INT_DEC, group );

    return ret_value;
}