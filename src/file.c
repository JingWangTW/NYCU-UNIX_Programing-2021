#include "file.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "process.h"
#include "util.h"

FILE_TYPE get_file_type ( mode_t file_mode );

FILE_LIST * read_file_stat ( const char * file_path, const FILE_LIST template )
{
    int fd_num;

    char real_path[PATH_MAX + 1] = { '\0' };
    struct stat file_stat;

    FILE_LIST * res = (FILE_LIST *) check_malloc ( sizeof ( FILE_LIST ) );

    /* common field */
    strcpy ( res->command, template.command );
    res->pid = template.pid;
    strcpy ( res->user_name, template.user_name );
    res->next = NULL;

    if ( lstat ( file_path, &file_stat ) == -1 )
    {
        check_free ( res );
        return NULL;
    }

    if ( S_ISLNK ( file_stat.st_mode ) )
    {
        if ( readlink ( file_path, real_path, PATH_MAX ) == -1 )
        {
            if ( errno == EACCES )
            {
                sprintf ( res->file_name, "%s (readlink: Permission denied)", file_path );
                res->type         = -1;
                res->inode_number = -1;

                return res;
            }
            else if ( errno == ENOENT )
            {
                sprintf ( res->file_name, "%s (readlink: File Not Found)", file_path );
                res->type         = -1;
                res->inode_number = -1;

                return res;
            }
            else
            {
                check_free ( res );
                return NULL;
            }
        }

        // a readble softlink
        // can't not stat it
        // A dangling symbolic link
        // The file has been deleted
        if ( stat ( real_path, &file_stat ) == -1 )
        {
            strcpy ( res->file_name, real_path );
            res->type = -1;

            fd_num = open ( file_path, O_RDONLY );
            fstat ( fd_num, &file_stat );

            res->inode_number = file_stat.st_ino;

            close ( fd_num );

            return res;
        }
    }
    else
    {
        strcpy ( real_path, file_path );
    }

    strcpy ( res->file_name, real_path );
    res->type         = get_file_type ( file_stat.st_mode );
    res->inode_number = file_stat.st_ino;

    return res;
}

void get_fd_str ( pid_t pid, int fd_num, char * buf )
{
    int flag;
    char input_str[100];
    char * search_ptr;
    char fd_path[PATH_MAX];
    char fd_info_path[PATH_MAX];

    FILE * fd_info;

    sprintf ( fd_path, "/proc/%d/fd/%d", pid, fd_num );
    sprintf ( fd_info_path, "/proc/%d/fdinfo/%d", pid, fd_num );

    if ( access ( fd_info_path, R_OK ) == -1 || access ( fd_path, R_OK ) == -1 )
    {
        get_error_message ( errno, "access", buf );
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
        return;

    if ( ( flag & O_ACCMODE ) == O_RDONLY )
        sprintf ( buf, "%dr", fd_num );
    else if ( ( flag & O_ACCMODE ) == O_WRONLY )
        sprintf ( buf, "%dw", fd_num );
    else if ( ( flag & O_ACCMODE ) == O_RDWR )
        sprintf ( buf, "%du", fd_num );
    else
        sprintf ( buf, "del" );
}

const char * get_error_message ( int err_no, const char * func_name, char * buffer )
{
    sprintf ( buffer, "(%s: %s)", func_name, strerror ( err_no ) );

    return buffer;
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