#include "file.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "util.h"

/* Get file stat and realpath from a provided file_path */
int get_file_stat ( char * realpath, struct stat * file_stat, const char * file_path );

/* According to the provided pid and fd_num to set fd string in parm dest */
void set_fd_str_fdinfo ( FILE_LIST * dest, const pid_t pid, const int fd_num );

/* According template, set the common field in dest */
/* Set value of command, username, pid */
void set_common_file_stat ( FILE_LIST * dest, const FILE_LIST template );

/* Check if file has in the list */
int find_duplicate_file ( const FILE_LIST * head, const ino_t inode_num, const char * file_path );

/* According to file_mode get from stat(), get the respective filetype */
FILE_TYPE get_file_type ( const struct stat * file_status, const char * file_path );

/* Check if provided file_path has been appended "(deleted)" */
int check_filename_append_deleted ( const char * file_path );

//

FILE_LIST * read_file_stat_fd ( const pid_t pid, const int fd_num, const FILE_LIST template )
{
    int get_stat_res;
    char realpath[FILE_PATH_MAX]     = { '\0' };
    char fd_file_name[FILE_PATH_MAX] = { '\0' };

    struct stat file_stat;
    FILE_LIST * res;

    snprintf ( fd_file_name, FILE_PATH_MAX - 1, "/proc/%d/fd/%d", pid, fd_num );

    get_stat_res = get_file_stat ( realpath, &file_stat, fd_file_name );

    res = (FILE_LIST *) check_malloc ( sizeof ( FILE_LIST ) );

    set_common_file_stat ( res, template );
    strncpy_append ( res->file_name, realpath, FILE_PATH_MAX - 1 );

    res->type         = get_file_type ( get_stat_res == -1 ? NULL : &file_stat, realpath );
    res->inode_number = file_stat.st_ino;

    set_fd_str_fdinfo ( res, pid, fd_num );

    return res;
}

FILE_LIST * read_file_stat_path ( const char * file_path, const FILE_LIST template )
{
    int get_stat_res;
    char realpath[FILE_PATH_MAX] = { '\0' };

    struct stat file_stat;

    FILE_LIST * res;

    get_stat_res = get_file_stat ( realpath, &file_stat, file_path );

    res = (FILE_LIST *) check_malloc ( sizeof ( FILE_LIST ) );

    set_common_file_stat ( res, template );
    strncpy_append ( res->file_name, realpath, FILE_PATH_MAX - 1 );

    res->type         = get_file_type ( get_stat_res == -1 ? NULL : &file_stat, realpath );
    res->inode_number = file_stat.st_ino;

    return res;
}

FILE_LIST * read_maps_file ( const pid_t pid, const FILE_LIST template )
{
    int get_stat_res;
    int parse_result;

    char input_str[1000];
    char input_path[FILE_PATH_MAX];
    char real_path[FILE_PATH_MAX];
    char maps_file_path[FILE_PATH_MAX];

    ino_t inode_num;

    FILE * maps_file;

    FILE_LIST * head = NULL;
    FILE_LIST * tail = NULL;
    FILE_LIST * res  = NULL;

    struct stat file_status;

    snprintf ( maps_file_path, FILE_PATH_MAX - 1, "/proc/%d/maps", pid );

    maps_file = fopen ( maps_file_path, "r" );

    // failed to open maps file
    // may have some permission problems
    if ( maps_file == NULL )
    {
        return NULL;
    }

    // read file line by line
    while ( fscanf ( maps_file, "%999[^\n] ", input_str ) != EOF )
    {
        // parse required field only
        parse_result = sscanf ( input_str, "%*x-%*x %*4[-rwxsp]%*x%*x:%*x%lu %[^\n] ", &inode_num, input_path );

        // check if parsed success
        if ( parse_result != 2 )
            continue;

        // skip pseudo-path
        if ( strnlen ( input_path, FILE_PATH_MAX - 1 ) == 0 || input_path[0] == '[' )
            continue;

        // get file stat and its real path
        get_stat_res = get_file_stat ( real_path, &file_status, input_path );

        // A file may have multiple record in the maps file due to it the file may be seperated to different memory region
        if ( find_duplicate_file ( head, inode_num, real_path ) )
            continue;

        // a new availale file found
        // record it
        res               = (FILE_LIST *) check_malloc ( sizeof ( FILE_LIST ) );
        res->inode_number = inode_num;
        res->next         = NULL;

        set_common_file_stat ( res, template );

        // check whether is a delete file
        if ( check_filename_append_deleted ( real_path ) )
            strncpy_append ( res->file_descriptior, "del", FILE_DESCRIPTOR_MAX - 1 );
        else
            strncpy_append ( res->file_descriptior, "mem", FILE_DESCRIPTOR_MAX - 1 );

        strncpy_append ( res->file_name, real_path, FILE_PATH_MAX - 1 );
        res->type = get_file_type ( get_stat_res == -1 ? NULL : &file_status, input_path );

        // append the record to the list
        if ( head == NULL )
            head = tail = res;
        else
        {
            tail->next = res;
            tail       = res;
        }
    }

    fclose ( maps_file );

    return head;
}

void get_type_str ( const FILE_LIST * file, char * buf )
{
    switch ( file->type )
    {
        case TYPE_DIR:
            strncpy_append ( buf, "DIR", FILE_TYPE_STR_LEN_MAX - 1 );
            break;
        case TYPE_CHR:
            strncpy_append ( buf, "CHR", FILE_TYPE_STR_LEN_MAX - 1 );
            break;
        case TYPE_REG:
            strncpy_append ( buf, "REG", FILE_TYPE_STR_LEN_MAX - 1 );
            break;
        case TYPE_FIFO:
            strncpy_append ( buf, "FIFO", FILE_TYPE_STR_LEN_MAX - 1 );
            break;
        case TYPE_SOCK:
            strncpy_append ( buf, "SOCK", FILE_TYPE_STR_LEN_MAX - 1 );
            break;
        case TYPE_UNKNOWN:
            strncpy_append ( buf, "unknown", FILE_TYPE_STR_LEN_MAX - 1 );
            break;
        default:
            buf[0] = '\0';
            break;
    }
}

void get_node_str ( const FILE_LIST * file, char * buf )
{
    if ( file->type == TYPE_UNKNOWN || file->type == (FILE_TYPE) -1 )
        buf[0] = '\0';
    else
        snprintf ( buf, INODE_STR_LEN_MAX - 1, "%ld", file->inode_number );
}

int get_file_stat ( char * realpath, struct stat * file_stat, const char * file_path )
{
    int fd_num;

    char error_str[ERROR_STR_LEN_MAX];

    char special_type[64];
    ino_t special_inode;

    // the file path is not exist
    // just skip it
    if ( lstat ( file_path, file_stat ) == -1 )
    {
        strncpy_append ( realpath, file_path, FILE_PATH_MAX - 1 );
        return -1;
    }

    // path is a symbolic link
    if ( S_ISLNK ( file_stat->st_mode ) )
    {
        // try to read the symbolic link
        // the file is exist, but wrong with its link
        if ( readlink ( file_path, realpath, FILE_PATH_MAX ) == -1 )
        {
            get_error_message ( errno, "readlink", error_str, ERROR_STR_LEN_MAX - 1 );
            snprintf ( realpath, FILE_PATH_MAX - 1, "%s %s", file_path, error_str );

            return -1;
        }

        // A readble softlink, but can't stat it
        if ( stat ( realpath, file_stat ) == -1 )
        {
            // => It's a dangling symbolic link, the linked file has been deleted
            // => If readlink() read a symbolic link under /porc, it will append a "deleted" message at the tail
            if ( check_filename_append_deleted ( realpath ) )
            {
                fd_num = open ( file_path, O_RDONLY );

                // May have some permission problem
                // I just leave it as a failed request
                if ( fd_num == -1 )
                {
                    get_error_message ( errno, "open", error_str, ERROR_STR_LEN_MAX - 1 );
                    snprintf ( realpath, FILE_PATH_MAX - 1, "%s %s", file_path, error_str );

                    return -1;
                }
                // I can't figure out when this will happen, I can open a file, but I can't stat it ???
                // Just leave this error check, more works is better than segment fault happened
                else if ( fstat ( fd_num, file_stat ) == -1 )
                {
                    get_error_message ( errno, "fstat", error_str, ERROR_STR_LEN_MAX - 1 );
                    snprintf ( realpath, FILE_PATH_MAX - 1, "%s %s", file_path, error_str );

                    return -1;
                }
                else
                {
                    // success find correct file and it's stat
                    // I have nothing to do
                    // I just want to leave this message to have correct view of this flow
                }

                close ( fd_num );
            }
            // The file may be something like [pipe:inode]
            else
            {
                memset ( file_stat, 0, sizeof ( struct stat ) );

                // something lke: [pipe:inode] [socket:inode]
                if ( sscanf ( realpath, "%[a-z]:[%ld]", special_type, &special_inode ) == 2 )
                {
                    strncpy_append ( realpath, special_type, FILE_PATH_MAX - 1 );
                    file_stat->st_ino = special_inode;
                }
                // something like: "anon_inode:<file-type>"
                else if ( sscanf ( realpath, "anon_inode:%s", special_type ) == 1 )
                {
                    strncpy_append ( realpath, special_type, FILE_PATH_MAX - 1 );
                }

                return -1;
            }
        }
    }
    else
    {
        // It's a normal file, the real path is just the same as file_path
        strncpy_append ( realpath, file_path, FILE_PATH_MAX - 1 );
    }

    return 0;
}

void set_fd_str_fdinfo ( FILE_LIST * dest, const pid_t pid, const int fd_num )
{
    int flag;
    char input_str[100];
    char * search_ptr;
    char fd_info_path[FILE_PATH_MAX];

    FILE * fd_info;

    snprintf ( fd_info_path, FILE_PATH_MAX - 1, "/proc/%d/fdinfo/%d", pid, fd_num );

    if ( access ( fd_info_path, R_OK ) == -1 )
    {
        get_error_message ( errno, "access", dest->file_name, FILE_PATH_MAX - 1 );
        return;
    }

    fd_info = fopen ( fd_info_path, "r" );
    flag    = -1;

    while ( fscanf ( fd_info, "%99[^\n] ", input_str ) != EOF )
    {
        search_ptr = strstr ( input_str, "flags:" );

        if ( search_ptr == input_str )
        {
            sscanf ( input_str, "flags: %o", &flag );
            break;
        }
    }

    if ( flag == -1 )
    {
        snprintf ( dest->file_descriptior, FILE_DESCRIPTOR_MAX - 1, "ERR" );
    }
    else
    {
        if ( ( flag & O_ACCMODE ) == O_RDONLY )
            snprintf ( dest->file_descriptior, FILE_DESCRIPTOR_MAX - 1, "%dr", fd_num );
        else if ( ( flag & O_ACCMODE ) == O_WRONLY )
            snprintf ( dest->file_descriptior, FILE_DESCRIPTOR_MAX - 1, "%dw", fd_num );
        else if ( ( flag & O_ACCMODE ) == O_RDWR )
            snprintf ( dest->file_descriptior, FILE_DESCRIPTOR_MAX - 1, "%du", fd_num );
        else
            snprintf ( dest->file_descriptior, FILE_DESCRIPTOR_MAX - 1, "ERR" );
    }

    // check deleted file
    search_ptr = strrchr ( dest->file_name, ' ' );

    if ( search_ptr != NULL && strcmp ( search_ptr + 1, "(deleted)" ) == 0 )
    {
        snprintf ( dest->file_descriptior, FILE_DESCRIPTOR_MAX - 1, "del" );
    }

    fclose ( fd_info );
}

void set_common_file_stat ( FILE_LIST * dest, const FILE_LIST template )
{
    /* common field */
    strncpy_append ( dest->command, template.command, COMMAND_NAME_MAX - 1 );
    strncpy_append ( dest->user_name, template.user_name, COMMAND_NAME_MAX - 1 );

    dest->pid  = template.pid;
    dest->next = NULL;
}

int find_duplicate_file ( const FILE_LIST * head, const ino_t inode_num, const char * file_path )
{
    if ( head == NULL )
        return 0;

    while ( head != NULL )
    {
        if ( head->inode_number == inode_num && strcmp ( head->file_name, file_path ) == 0 )
            return 1;

        head = head->next;
    }

    return 0;
}

FILE_TYPE get_file_type ( const struct stat * file_status, const char * file_path )
{
    if ( strstr ( file_path, "pipe" ) != NULL )
    {
        return TYPE_FIFO;
    }
    else if ( strstr ( file_path, "socket" ) != NULL )
    {
        return TYPE_SOCK;
    }
    else
    {
        if ( file_status == NULL )
            return TYPE_UNKNOWN;

        switch ( file_status->st_mode & S_IFMT )
        {
            case S_IFDIR:
                return TYPE_DIR;

            case S_IFREG:
                return TYPE_REG;

            case S_IFCHR:
                return TYPE_CHR;

            case S_IFIFO:
                return TYPE_FIFO;

            case S_IFSOCK:
                return TYPE_SOCK;

            default:
                return TYPE_UNKNOWN;
        }
    }
}

int check_filename_append_deleted ( const char * file_path )
{
    char * search_ptr;

    search_ptr = strrchr ( file_path, ' ' );

    if ( search_ptr != NULL && strcmp ( search_ptr + 1, "(deleted)" ) == 0 )
    {
        return 1;
    }

    return 0;
}