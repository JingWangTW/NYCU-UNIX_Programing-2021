#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <unistd.h>

struct pid_list
{
    pid_t * list;
    int size;
};

typedef struct pid_list PID_LIST;

/* get all pid number under /proc */
PID_LIST get_all_pids ( );

#endif