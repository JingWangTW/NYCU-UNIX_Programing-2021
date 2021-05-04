#include <stdio.h>

#include "linux_cmd.h"
#include "utility.h"

FILE * tmpfile ( )
{
    FILE * ret_value;

    ret_value = linux_tmpfile ( );

    logger_output ( "tmpfile", 1, POINTER, ret_value );

    return ret_value;
}

FILE * tmpfile64 ( )
{
    FILE * ret_value;

    ret_value = linux_tmpfile64 ( );

    logger_output ( "tmpfile", 1, POINTER, ret_value );

    return ret_value;
}