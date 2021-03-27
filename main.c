#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "file.h"
#include "io.h"
#include "process.h"

int main ( const int argc, char * const * argv )
{
    PROC_FILTER * filter = parse_input ( argc, argv );
    PID_LIST all_pids;
    FILE_LIST ** all_porc_files;

    if ( filter == NULL )
        exit ( EXIT_FAILURE );

    all_pids = get_all_pids ( );

    all_porc_files = get_all_proc_files ( all_pids, filter );

    print_result ( all_porc_files, all_pids.size );

    return 0;
}