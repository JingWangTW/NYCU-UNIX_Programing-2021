#include <stdarg.h>
#include <sys/stat.h>

#include "linux_cmd.h"
#include "utility.h"

int open ( const char * pathname, int flags, ... )
{
    // the real open(2) can accept two or three arguments
    int ret_value;
    mode_t mode;

    va_list input_args;
    va_start ( input_args, flags );
    mode = va_arg ( input_args, mode_t );

    ret_value = linux_open ( pathname, flags, mode );

    logger_output ( "open", 4, INT_DEC, ret_value, PATH, pathname, INT_OCT, flags, INT_OCT, mode );

    va_end ( input_args );

    return ret_value;
}

int open64 ( const char * pathname, int flags, ... )
{
    // the real open(2) can accept two or three arguments
    int ret_value;
    mode_t mode;

    va_list input_args;
    va_start ( input_args, flags );
    mode = va_arg ( input_args, mode_t );

    ret_value = linux_open64 ( pathname, flags, mode );

    logger_output ( "open", 4, INT_DEC, ret_value, PATH, pathname, INT_OCT, flags, INT_OCT, mode );

    va_end ( input_args );

    return ret_value;
}