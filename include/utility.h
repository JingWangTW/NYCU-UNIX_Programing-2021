#ifndef __UTILITY_H__
#define __UTILITY_H__

typedef enum
{
    INT_LONG,
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