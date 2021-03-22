#ifndef __IO_H__
#define __IO_H__

#include <regex.h>

struct command_arg
{
    regex_t command_reg;
    regex_t filename_reg;
    char * type_filter;
};

typedef struct command_arg ARG;

/* Parsing input from command line */
ARG * parse_input ( const int argc, char * const * argv );

#endif