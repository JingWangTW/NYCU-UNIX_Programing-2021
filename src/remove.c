#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>

#include "utility.h"

int remove ( const char * pathname )
{
    static int ( *linux_remove ) ( const char * ) = NULL;
    int ret_value;

    FILE * output_file = get_output_file ( );

    if ( linux_remove == NULL )
    {
        linux_remove = dlsym ( RTLD_NEXT, "remove" );
    }

    ret_value = linux_remove ( pathname );

    fprintf ( output_file, "[logger] remove(\"%s\") = %d\n", get_realpath ( pathname ), ret_value );

    close_output_file ( output_file );

    return ret_value;
}