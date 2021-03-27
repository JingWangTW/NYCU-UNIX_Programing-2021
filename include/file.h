#ifndef __FILE_H__
#define __FILE_H__

#include <limits.h>
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
    char file_name[PATH_MAX + 32];  // may concat with permission denied string

    struct file_list * next;
};

typedef enum file_type FILE_TYPE;
typedef struct file_list FILE_LIST;

/* Read file stat from fd and pid number */
FILE_LIST * read_file_stat_fd ( const pid_t pid, const int fd_num, const FILE_LIST template );

/* Read file stat from path, for cwd, exe, root */
/* Leave FD blank */
FILE_LIST * read_file_stat_path ( const char * file_path, const FILE_LIST template );

/* According pid, read /proc/{pid}/maps */
FILE_LIST * read_maps_file ( const pid_t pid, const FILE_LIST template );

/* Get corresponding "type" string from the file structure */
void get_type_str ( const FILE_LIST * file, char * buf );

/* Get corresponding "node" string from the file structure */
void get_node_str ( const FILE_LIST * file, char * buf );

#endif