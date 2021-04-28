#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>

#include "utility.h"

ssize_t read ( int fd, void * buf, size_t count )
{
    static int ( *linux_read ) ( int, void *, size_t ) = NULL;
    int ret_value;

    FILE * output_file = get_output_file ( );

    if ( linux_read == NULL )
    {
        linux_read = dlsym ( RTLD_NEXT, "read" );
    }

    ret_value = linux_read ( fd, buf, count );

    fprintf ( output_file, "[logger] read(%d, %p, %ld) = %d\n", fd, buf, count, ret_value );

    close_output_file ( output_file );

    return ret_value;
}