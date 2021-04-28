#include <stdio.h>

#include "linux_cmd.h"
#include "utility.h"

FILE * fopen ( const char * pathname, const char * mode )
{
    FILE * ret_value;

    FILE * output_file = get_output_file ( );

    ret_value = linux_fopen ( pathname, mode );

    fprintf ( output_file, "[logger] fopen(\"%s\", \"%s\") = \"%s\"\n", get_output_str ( pathname ), get_output_str ( mode ), get_FILE_file_name ( ret_value ) );

    close_output_file ( output_file );

    return ret_value;
}