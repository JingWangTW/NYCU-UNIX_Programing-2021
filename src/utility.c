#include <stdio.h>
#include <stdlib.h>

FILE * get_output_file ( )
{
    FILE * output_file = NULL;
    char * file_name   = NULL;

    file_name = getenv ( "LOGGER_OUTPUT_FILE" );

    if ( file_name == NULL )
    {
        output_file = stderr;
    }
    else
    {
        output_file = fopen ( file_name, "a" );
    }

    if ( output_file == NULL )
    {
        fprintf ( stderr, "Failed to open file %s.\n", file_name );
    }

    return output_file;
}

void close_output_file ( FILE * output_file )
{
    if ( output_file == stderr )
    {
        fclose ( output_file );
    }
}