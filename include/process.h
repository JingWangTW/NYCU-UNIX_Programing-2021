#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <regex.h>

#include "file.h"

struct pid_vector
{
    pid_t * pids;
    int size;
};

struct process_filter
{
    regex_t * command_regex;
    regex_t * filename_regex;
    char * type_filter;
};

typedef struct pid_vector PID_VECTOR;
typedef struct process_filter PROC_FILTER;

/* get all pid number under /proc */
PID_VECTOR get_all_pids ( );

/* Get all openned files by all procs */
FILE_LIST ** get_all_proc_files ( PID_VECTOR all_pid, PROC_FILTER * filter );

#endif