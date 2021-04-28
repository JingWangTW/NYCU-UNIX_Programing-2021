#define _GNU_SOURCE

#include <dlfcn.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>

#include "utility.h"

int open ( const char * pathname, int flags, ... )
{
    // the real open(2) can accept two or three arguments
    static int ( *linux_open ) ( const char *, int, ... ) = NULL;
    int ret_value;
    mode_t mode;

    va_list input_args;
    va_start ( input_args, flags );
    mode = va_arg ( input_args, mode_t );

    FILE * output_file = get_output_file ( );

    if ( linux_open == NULL )
    {
        linux_open = dlsym ( RTLD_NEXT, "open" );
    }

    ret_value = linux_open ( pathname, flags, mode );

    fprintf ( output_file, "[logger] open(%s, %d, %d) = %d\n", pathname, flags, mode, ret_value );

    close_output_file ( output_file );

    va_end ( input_args );

    return ret_value;
}