#include <stdio.h>

#include "linux_cmd.h"
#include "utility.h"

int remove ( const char * pathname )
{
    int ret_value;

    FILE * output_file = get_output_file ( );

    ret_value = linux_remove ( pathname );

    fprintf ( output_file, "[logger] remove(\"%s\") = %d\n", get_realpath ( pathname ), ret_value );

    close_output_file ( output_file );

    return ret_value;
}