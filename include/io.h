#ifndef __IO_H__
#define __IO_H__

#include "process.h"

/* Parsing input from command line */
PROC_FILTER * parse_input ( const int argc, char * const * argv );

#endif