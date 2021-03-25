#include "file.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "process.h"
#include "util.h"

/* Get file stat and realpath from a provided file_path */
int get_file_stat ( char * realpath, struct stat * file_stat, const char * file_path );

/* According to the provided pid and fd_num to set fd string in parm dest */
void set_fd_str_fdinfo ( FILE_LIST * dest, const pid_t pid, const int fd_num );

/* According template, set the common field in dest */
/* Set value of command, username, pid */
void set_common_file_stat ( FILE_LIST * dest, const FILE_LIST template );

/* According to file_mode get from stat(), get the respective filetype */
FILE_TYPE get_file_type ( mode_t file_mode );

//

FILE_LIST * read_file_stat_fd ( const pid_t pid, const int fd_num, const FILE_LIST template )
{
    char realpath[PATH_MAX + 1]     = { '\0' };
    char fd_file_name[PATH_MAX + 1] = { '\0' };

    struct stat file_stat;

    FILE_LIST * res;
    res = (FILE_LIST *) check_malloc ( sizeof ( FILE_LIST ) );

    sprintf ( fd_file_name, "/proc/%d/fd/%d", pid, fd_num );

    set_common_file_stat ( res, template );
    if ( get_file_stat ( realpath, &file_stat, fd_file_name ) == -1 )
    {
        check_free ( res );
        return NULL;
    }

    strcpy ( res->file_name, realpath );
    res->type         = get_file_type ( file_stat.st_mode );
    res->inode_number = file_stat.st_ino;

    set_fd_str_fdinfo ( res, pid, fd_num );

    return res;
}

FILE_LIST * read_file_stat_path ( const char * file_path, const FILE_LIST template )
{
    char realpath[PATH_MAX + 1] = { '\0' };

    struct stat file_stat;

    FILE_LIST * res;
    res = (FILE_LIST *) check_malloc ( sizeof ( FILE_LIST ) );

    set_common_file_stat ( res, template );
    if ( get_file_stat ( realpath, &file_stat, file_path ) == -1 )
    {
        check_free ( res );
        return NULL;
    }

    strcpy ( res->file_name, realpath );
    res->type         = get_file_type ( file_stat.st_mode );
    res->inode_number = file_stat.st_ino;

    return res;
}

int get_file_stat ( char * realpath, struct stat * file_stat, const char * file_path )
{
    int fd_num;
    char error_str[64];

    char * str_search_str;

    // the file path is not exist
    // just skip it
    if ( lstat ( file_path, file_stat ) == -1 )
    {
        return -1;
    }

    // path is a symbolic link
    if ( S_ISLNK ( file_stat->st_mode ) )
    {
        // try to read the symbolic link
        if ( readlink ( file_path, realpath, PATH_MAX ) == -1 )
        {
            get_error_message ( errno, "readlink", error_str );
            sprintf ( realpath, "%s %s", file_path, error_str );

            return 0;
        }

        // A readble softlink, but can't stat it
        if ( stat ( realpath, file_stat ) == -1 )
        {
            // => It's a dangling symbolic link, the linked file has been deleted
            // => If readlink() read a symbolic link under /porc, it will append a "deleted" message at the tail
            str_search_str = strrchr ( realpath, ' ' );

            if ( str_search_str != NULL && strcmp ( str_search_str + 1, "(deleted)" ) == 0 )
            {
                fd_num = open ( file_path, O_RDONLY );

                // I can't figure out when this will happen
                // I just leave it as a failed request
                if ( fd_num == -1 )
                {
                    get_error_message ( errno, "open", error_str );
                    sprintf ( realpath, "%s %s", file_path, error_str );

                    close ( fd_num );

                    return -1;
                }

                // I can't figure out when this will happen
                // I just leave it as a failed request
                if ( fstat ( fd_num, file_stat ) == -1 )
                {
                    get_error_message ( errno, "fstat", error_str );
                    sprintf ( realpath, "%s %s", file_path, error_str );

                    close ( fd_num );

                    return -1;
                }

                close ( fd_num );
            }
            // I can't figure out when this will happen
            // I just leave it as a failed request
            else
            {
                get_error_message ( errno, "stat", error_str );
                sprintf ( realpath, "%s %s", file_path, error_str );

                return -1;
            }
        }
    }
    else
    {
        strcpy ( realpath, file_path );
    }

    return 0;
}

void set_fd_str_fdinfo ( FILE_LIST * dest, const pid_t pid, const int fd_num )
{
    int flag;
    char input_str[100];
    char * search_ptr;
    char fd_info_path[PATH_MAX];

    FILE * fd_info;

    sprintf ( fd_info_path, "/proc/%d/fdinfo/%d", pid, fd_num );

    if ( access ( fd_info_path, R_OK ) == -1 )
    {
        get_error_message ( errno, "access", dest->file_name );
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
        sprintf ( dest->file_descriptior, "ERR" );
    }
    else
    {
        if ( ( flag & O_ACCMODE ) == O_RDONLY )
            sprintf ( dest->file_descriptior, "%dr", fd_num );
        else if ( ( flag & O_ACCMODE ) == O_WRONLY )
            sprintf ( dest->file_descriptior, "%dw", fd_num );
        else if ( ( flag & O_ACCMODE ) == O_RDWR )
            sprintf ( dest->file_descriptior, "%du", fd_num );
        else
            sprintf ( dest->file_descriptior, "ERR" );
    }

    // check deleted file
    search_ptr = strrchr ( dest->file_name, ' ' );

    if ( search_ptr != NULL && strcmp ( search_ptr + 1, "(deleted)" ) == 0 )
    {
        sprintf ( dest->file_descriptior, "del" );
    }

    fclose ( fd_info );
}

void set_common_file_stat ( FILE_LIST * dest, const FILE_LIST template )
{
    /* common field */
    strcpy ( dest->command, template.command );
    strcpy ( dest->user_name, template.user_name );

    dest->pid  = template.pid;
    dest->next = NULL;
}

FILE_TYPE get_file_type ( mode_t file_mode )
{
    switch ( file_mode & S_IFMT )
    {
        case S_IFDIR:
            return DIRECTOR;

        case S_IFREG:
            return REGULAR;

        case S_IFCHR:
            return CHARACTER;

        case S_IFIFO:
            return FIFO_FILE;

        case S_IFSOCK:
            return SOCKET_FILE;

        default:
            return UNKNOWN_FILE;
    }
}