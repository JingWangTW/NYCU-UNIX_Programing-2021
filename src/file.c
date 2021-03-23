#include "file.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "process.h"
#include "util.h"

FILE_TYPE get_file_type ( mode_t file_mode );

FILE_LIST * read_file_stat ( const char * file_path, const FILE_LIST template )
{
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
            else
            {
                check_free ( res );
                return NULL;
            }
        }

        if ( stat ( real_path, &file_stat ) == -1 )
        {
            check_free ( res );
            return NULL;
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