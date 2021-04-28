#include <stdio.h>

#include "linux_cmd.h"
#include "utility.h"

int fclose ( FILE * stream )
{
    int ret_value;

    FILE * output_file = get_output_file ( );

    ret_value = linux_fclose ( stream );

    fprintf ( output_file, "[logger] fclose(\"%s\") = %d\n", get_FILE_file_name ( stream ), ret_value );

    close_output_file ( output_file );

    return ret_value;
}