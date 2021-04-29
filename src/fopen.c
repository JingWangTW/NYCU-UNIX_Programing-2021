#include "linux_cmd.h"
#include "utility.h"

FILE * fopen ( const char * pathname, const char * mode )
{
    FILE * ret_value;

    ret_value = linux_fopen ( pathname, mode );

    logger_output ( "fopen", 3, POINTER, ret_value, PATH, pathname, STRING, mode );

    return ret_value;
}