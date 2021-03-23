#ifndef __FILE_H__
#define __FILE_H__

#include <bits/local_lim.h>
#include <sys/stat.h>
#include <unistd.h>

enum file_type
{
    DIRECTOR,
    REGULAR,
    CHARACTER,
    FIFO_FILE,
    SOCKET_FILE,
    UNKNOWN_FILE,
};

struct file_list
{
    char command[NAME_MAX + 1];
    pid_t pid;
    char user_name[LOGIN_NAME_MAX + 1];
    char file_descriptior[64];
    enum file_type type;
    ino_t inode_number;
    char file_name[PATH_MAX];

    struct file_list * next;
};

typedef enum file_type FILE_TYPE;
typedef struct file_list FILE_LIST;

#endif