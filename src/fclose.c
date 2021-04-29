#include <sys/param.h>

#include "linux_cmd.h"
#include "utility.h"

int fclose ( FILE * stream )
{
    int ret_value;
    char file_name[MAXPATHLEN + 1];

    get_FILE_file_name_r ( stream, file_name );

    ret_value = linux_fclose ( stream );

    logger_output ( "fclose", 2, INT_DEC, ret_value, LONG_STR, file_name );

    return ret_value;
}