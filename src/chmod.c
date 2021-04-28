#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <sys/stat.h>

#include "utility.h"

int chmod ( const char * pathname, mode_t mode )
{
    static int ( *linux_chmod ) ( const char *, mode_t ) = NULL;
    int ret_value;

    FILE * output_file = get_output_file ( );

    if ( linux_chmod == NULL )
    {
        linux_chmod = dlsym ( RTLD_NEXT, "chmod" );
    }

    ret_value = linux_chmod ( pathname, mode );

    fprintf ( output_file, "[logger] chomd(\"%s\", %o) = %d\n", get_realpath ( pathname ), mode, ret_value );

    close_output_file ( output_file );

    return ret_value;
}