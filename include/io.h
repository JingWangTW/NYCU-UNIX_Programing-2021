#ifndef __IO_H__
#define __IO_H__

#include "file.h"
#include "process.h"

/* Parsing input from command line */
PROC_FILTER * parse_input ( const int argc, char * const * argv );
void print_result ( FILE_LIST ** file_list, int size );

#endif