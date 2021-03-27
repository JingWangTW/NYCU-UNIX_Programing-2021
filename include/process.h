#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <regex.h>
#include <unistd.h>

#include "file.h"

struct process_filter
{
    regex_t * command_reg;
    regex_t * filename_reg;
    char * type_filter;
};

struct pid_list
{
    pid_t * list;
    int size;
};

typedef struct process_filter PROC_FILTER;
typedef struct pid_list PID_LIST;

/* get all pid number under /proc */
PID_LIST get_all_pids ( );
FILE_LIST ** get_all_proc_files ( PID_LIST pid_list, PROC_FILTER * filter );

#endif