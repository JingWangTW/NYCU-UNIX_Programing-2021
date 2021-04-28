#include <stdio.h>
#include <unistd.h>

#include "utility.h"

int close ( int fd )
{
    int ( *linux_close ) ( int ) = NULL;
    int ret_value;

    FILE * output_file = get_output_file ( );

    linux_close = get_linux_func ( "close" );

    ret_value = linux_close ( fd );

    fprintf ( output_file, "[logger] close(\"%s\") = %d\n", get_fd_file_name ( fd ), ret_value );

    close_output_file ( output_file );

    return ret_value;
}