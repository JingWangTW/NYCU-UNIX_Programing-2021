#include "process.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "util.h"

PID_LIST get_all_pids ( )
{
    int pid_count = 0;
    char test_name_buf[NAME_MAX + 1];

    pid_t temp_pid;
    PID_LIST res;
    DIR * proc_dir;
    struct dirent * proc_dir_entry;

    res.size = 100;
    res.list = (pid_t *) check_malloc ( sizeof ( pid_t ) * res.size );
    proc_dir = opendir ( "/proc" );

    // read all entry in the directory
    while ( ( proc_dir_entry = readdir ( proc_dir ) ) != NULL )
    {
        memset ( test_name_buf, 0, sizeof ( char ) * ( NAME_MAX + 1 ) );
        // only find directory
        if ( proc_dir_entry->d_type == DT_DIR )
        {
            // skip this two special dir
            if ( strcmp ( proc_dir_entry->d_name, "." ) != 0 && strcmp ( proc_dir_entry->d_name, ".." ) != 0 )
            {
                // get the entry that its name only contains numbers
                sscanf ( proc_dir_entry->d_name, "%[0-9]", test_name_buf );

                if ( strcmp ( proc_dir_entry->d_name, test_name_buf ) == 0 )
                {
                    sscanf ( proc_dir_entry->d_name, "%d", &temp_pid );

                    if ( pid_count == res.size )
                    {
                        res.size += 100;
                        res.list = check_realloc ( res.list, sizeof ( pid_t ) * res.size );
                    }

                    res.list[pid_count] = temp_pid;

                    pid_count++;
                }
            }
        }
    }

    res.list = check_realloc ( res.list, sizeof ( pid_t ) * pid_count );
    res.size = pid_count;

    return res;
}