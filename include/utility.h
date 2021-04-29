#ifndef __UTILITY_H__
#define __UTILITY_H__

typedef enum
{
    UINT_LONG_DEC,
    INT_LONG_DEC,
    INT_OCT,
    INT_DEC,
    POINTER,
    FILE_PTR,
    FD_NO,
    STRING,
    PATH,
    VOID,
} OUTPUT_TYPE;

void logger_output ( const char * func_name, int param_cnt, ... );

#endif