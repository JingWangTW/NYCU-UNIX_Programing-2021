#ifndef __FILE_H__
#define __FILE_H__

#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>

#define ERROR_STR_LEN_MAX ( 64 )

#define FILE_TYPE_STR_LEN_MAX ( 16 )
#define INODE_STR_LEN_MAX     ( 32 )
#define PID_STR_LEN_MAX       ( 32 )

#define COMMAND_NAME_MAX    ( NAME_MAX + 1 )
#define USER_NAME_MAX       ( LOGIN_NAME_MAX )
#define FILE_DESCRIPTOR_MAX ( 16 )
#define FILE_PATH_MAX       ( PATH_MAX + ERROR_STR_LEN_MAX )
#define FILE_NAME_MAX       ( NAME_MAX + 1 )

enum file_type
{
    TYPE_DIR,
    TYPE_REG,
    TYPE_CHR,
    TYPE_FIFO,
    TYPE_SOCK,
    TYPE_UNKNOWN,
};

struct file_list
{
    char command[COMMAND_NAME_MAX];
    pid_t pid;
    char user_name[USER_NAME_MAX];

    char file_descriptior[FILE_DESCRIPTOR_MAX];
    enum file_type type;
    ino_t inode_number;
    char file_name[FILE_PATH_MAX];  // may concat with permission denied string

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