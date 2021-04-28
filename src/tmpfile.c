#include <stdio.h>

#include "linux_cmd.h"
#include "utility.h"

FILE * tmpfile ( )
{
    FILE * ret_value;

    FILE * output_file = get_output_file ( );

    ret_value = linux_tmpfile ( );

    fprintf ( output_file, "[logger] tmpfile() = \'%s\'\n", get_FILE_file_name ( ret_value ) );

    close_output_file ( output_file );

    return ret_value;
}