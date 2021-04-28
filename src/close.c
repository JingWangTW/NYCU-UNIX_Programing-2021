#include <stdio.h>

#include "linux_cmd.h"
#include "utility.h"

int close ( int fd )
{
    int ret_value;

    FILE * output_file = get_output_file ( );

    ret_value = linux_close ( fd );

    fprintf ( output_file, "[logger] close(\"%s\") = %d\n", get_fd_file_name ( fd ), ret_value );

    close_output_file ( output_file );

    return ret_value;
}