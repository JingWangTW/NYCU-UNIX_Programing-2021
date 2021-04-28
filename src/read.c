#include <stdio.h>
#include <unistd.h>

#include "linux_cmd.h"
#include "utility.h"

ssize_t read ( int fd, void * buf, size_t count )
{
    int ret_value;

    FILE * output_file = get_output_file ( );

    ret_value = linux_read ( fd, buf, count );

    fprintf ( output_file, "[logger] read(\"%s\", \"%s\", %ld) = %d\n", get_fd_file_name ( fd ), get_output_str ( buf ), count, ret_value );

    close_output_file ( output_file );

    return ret_value;
}