#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>

#include "utility.h"

int close ( int fd )
{
    static int ( *linux_close ) ( int ) = NULL;
    int ret_value;

    FILE * output_file = get_output_file ( );

    if ( linux_close == NULL )
    {
        linux_close = dlsym ( RTLD_NEXT, "close" );
    }

    ret_value = linux_close ( fd );

    fprintf ( output_file, "[logger] close(%d) = %d\n", fd, ret_value );

    close_output_file ( output_file );

    return ret_value;
}