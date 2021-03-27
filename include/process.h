#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <regex.h>

#include "file.h"

struct pid_list
{
    pid_t * list;
    int size;
};

struct process_filter
{
    regex_t * command_reg;
    regex_t * filename_reg;
    char * type_filter;
};

typedef struct pid_list PID_LIST;
typedef struct process_filter PROC_FILTER;

/* get all pid number under /proc */
PID_LIST get_all_pids ( );

/* Get all openned files by all procs */
FILE_LIST ** get_all_proc_files ( PID_LIST pid_list, PROC_FILTER * filter );

#endif