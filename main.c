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
    PROC_FILES_LIST * all_porc_files;
    FILE_LIST * file;

    if ( filter == NULL )
        exit ( EXIT_FAILURE );

    all_pids = get_all_pids ( );

    all_porc_files = get_all_proc_files ( all_pids, filter );

    while ( all_porc_files != NULL )
    {
        file = all_porc_files->head;

        while ( file != NULL )
        {
            file->command[9] = '\0';

            printf ( "%10s %7d %20s %8s %8d %8ld %s\n", file->command, file->pid, file->user_name, file->file_descriptior, file->type, file->inode_number, file->file_name );

            file = file->next;
        }

        all_porc_files = all_porc_files->next;
    }

    return 0;
}