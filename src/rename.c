#include <stdio.h>

#include "linux_cmd.h"
#include "utility.h"

int rename ( const char * oldpath, const char * newpath )
{
    int ret_value;

    FILE * output_file = get_output_file ( );

    ret_value = linux_rename ( oldpath, newpath );

    fprintf ( output_file, "[logger] rename(\"%s\", \"%s\") = %d\n", get_realpath ( oldpath ), get_realpath ( newpath ), ret_value );

    close_output_file ( output_file );

    return ret_value;
}