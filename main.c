#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "io.h"
#include "process.h"

int main ( const int argc, char * const * argv )
{
    PROC_FILTER * arguments = parse_input ( argc, argv );
    /* PID_LIST all_pids; */

    if ( arguments == NULL )
        exit ( EXIT_FAILURE );

    get_all_pids ( );

    return 0;
}