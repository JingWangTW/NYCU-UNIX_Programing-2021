#include <regex.h>
#include <stdlib.h>

#include "file.h"
#include "io.h"
#include "process.h"
#include "util.h"

void release_resource ( PROC_FILTER * filter, PID_VECTOR * all_pids, FILE_LIST ** all_files );

int main ( const int argc, char * const * argv )
{
    PROC_FILTER * filter;
    PID_VECTOR * all_pids;
    FILE_LIST ** all_proc_files;

    filter = parse_input ( argc, argv );

    if ( filter == NULL )
        exit ( EXIT_FAILURE );

    all_pids = get_all_pids ( );

    all_proc_files = get_all_proc_files ( all_pids, filter );

    print_result ( all_proc_files, all_pids->size );

    release_resource ( filter, all_pids, all_proc_files );

    return 0;
}

void release_resource ( PROC_FILTER * filter, PID_VECTOR * all_pids, FILE_LIST ** all_files )
{
    int proc_cnt;
    FILE_LIST * temp;
    FILE_LIST * tail;

    for ( proc_cnt = 0; proc_cnt < all_pids->size; proc_cnt++ )
    {
        tail = all_files[proc_cnt];

        while ( tail != NULL )
        {
            temp = tail;
            tail = tail->next;
            check_free ( temp );
        }
    }

    check_free ( all_pids->pids );

    if ( filter->command_regex )
        regfree ( filter->command_regex );

    if ( filter->filename_regex )
        regfree ( filter->filename_regex );

    if ( filter->type_filter )
        check_free ( filter->type_filter );

    check_free ( filter );
    check_free ( all_pids );
    check_free ( all_files );
}