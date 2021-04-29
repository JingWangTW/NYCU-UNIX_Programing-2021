#include "linux_cmd.h"
#include "utility.h"

int fclose ( FILE * stream )
{
    int ret_value;

    ret_value = linux_fclose ( stream );

    logger_output ( "fclose", 2, INT_DEC, ret_value, FILE_PTR, stream );

    return ret_value;
}