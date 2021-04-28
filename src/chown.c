#include <stdio.h>
#include <sys/stat.h>

#include "linux_cmd.h"
#include "utility.h"

int chown ( const char * pathname, uid_t owner, gid_t group )
{
    int ret_value;

    FILE * output_file = get_output_file ( );

    ret_value = linux_chown ( pathname, owner, group );

    fprintf ( output_file, "[logger] chown(\"%s\", %d, %d) = %d\n", get_realpath ( pathname ), owner, group, ret_value );

    close_output_file ( output_file );

    return ret_value;
}