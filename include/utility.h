#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <stdio.h>

FILE * get_output_file ( );
void close_output_file ( FILE * output_file );

char * get_realpath ( const char * path );
char * get_fd_file_name ( int fd );
char * get_FILE_file_name ( FILE * file );
char * get_output_str ( const char * str );

#endif