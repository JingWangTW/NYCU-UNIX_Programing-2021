#include <stdio.h>

#include "utility.h"

int rename ( const char * oldpath, const char * newpath )
{
    int ( *linux_rename ) ( const char *, const char * );
    int ret_value;

    FILE * output_file = get_output_file ( );

    linux_rename = get_linux_func ( "rename" );

    ret_value = linux_rename ( oldpath, newpath );

    fprintf ( output_file, "[logger] rename(\"%s\", \"%s\") = %d\n", get_realpath ( oldpath ), get_realpath ( newpath ), ret_value );

    close_output_file ( output_file );

    return ret_value;
}