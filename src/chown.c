#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utility.h"

int chown ( const char * pathname, uid_t owner, gid_t group )
{
    static int ( *linux_chown ) ( const char *, uid_t, gid_t ) = NULL;
    int ret_value;

    FILE * output_file = get_output_file ( );

    if ( linux_chown == NULL )
    {
        linux_chown = dlsym ( RTLD_NEXT, "chown" );
    }

    ret_value = linux_chown ( pathname, owner, group );

    fprintf ( output_file, "[logger] chown(%s, %d, %d) = %d", pathname, owner, group, ret_value );

    close_output_file ( output_file );

    return ret_value;
}