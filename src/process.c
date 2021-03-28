#include "process.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "file.h"
#include "util.h"

int get_cmd_and_username ( char * command, char * username, pid_t pid );
int check_command_pass ( const char * command, const PROC_FILTER * filter );
int check_file_path_pass ( const char * name, const PROC_FILTER * filter );
int check_file_type_pass ( FILE_TYPE type, const PROC_FILTER * filter );
void append_list ( FILE_LIST * target, FILE_LIST ** head, FILE_LIST ** tail );
FILE_LIST * remove_dup_files_in_mems ( const FILE_LIST * all, FILE_LIST * mems );
FILE_LIST * filter_apply ( const PROC_FILTER * filter, FILE_LIST * all );
int proc_compare ( const void * a, const void * b );

FILE_LIST * get_cwd ( const pid_t pid, const FILE_LIST template );
FILE_LIST * get_root ( const pid_t pid, const FILE_LIST template );
FILE_LIST * get_exe ( const pid_t pid, const FILE_LIST template );
FILE_LIST * get_all_fd_files ( const pid_t pid, const FILE_LIST template );

PID_VECTOR * get_all_pids ( )
{
    int pid_count = 0;
    char test_name_buf[FILE_NAME_MAX];

    pid_t temp_pid;
    DIR * proc_dir;
    struct dirent * proc_dir_entry;

    PID_VECTOR * res;

    proc_dir = opendir ( "/proc" );

    if ( proc_dir == NULL )
        return NULL;

    res = (PID_VECTOR *) check_malloc ( sizeof ( PID_VECTOR ) );
    /* init the size, it may grow latter */
    res->size = 100;
    res->pids = (pid_t *) check_malloc ( sizeof ( pid_t ) * res->size );

    // read all entry in the directory
    while ( ( proc_dir_entry = readdir ( proc_dir ) ) != NULL )
    {
        memset ( test_name_buf, 0, sizeof ( char ) * ( FILE_NAME_MAX ) );

        // only find directory
        if ( proc_dir_entry->d_type == DT_DIR )
        {
            // skip this two special dir
            if ( strcmp ( proc_dir_entry->d_name, "." ) != 0 && strcmp ( proc_dir_entry->d_name, ".." ) != 0 )
            {
                // get the entry that its name contains numbers only
                sscanf ( proc_dir_entry->d_name, "%[0-9]", test_name_buf );

                if ( strncmp ( proc_dir_entry->d_name, test_name_buf, FILE_NAME_MAX ) == 0 )
                {
                    sscanf ( proc_dir_entry->d_name, "%d", &temp_pid );

                    if ( pid_count == res->size )
                    {
                        res->size += 100;
                        res->pids = check_realloc ( res->pids, sizeof ( pid_t ) * res->size );
                    }

                    res->pids[pid_count] = temp_pid;

                    pid_count++;
                }
            }
        }
    }

    closedir ( proc_dir );

    res->pids = check_realloc ( res->pids, sizeof ( pid_t ) * pid_count );
    res->size = pid_count;

    return res;
}

FILE_LIST ** get_all_proc_files ( const PID_VECTOR * all_pid, const PROC_FILTER * filter )
{
    int proc_cnt;
    int check_filter;

    char command[COMMAND_NAME_MAX];
    char username[USERNAME_MAX];
    pid_t current_pid;

    FILE_LIST template;

    FILE_LIST * res       = NULL;
    FILE_LIST * map_res   = NULL;
    FILE_LIST * proc_head = NULL;
    FILE_LIST * proc_tail = NULL;

    FILE_LIST ** all_proc_file_list = NULL;

    all_proc_file_list = (FILE_LIST **) check_malloc ( sizeof ( FILE_LIST * ) * all_pid->size );
    memset ( all_proc_file_list, 0, sizeof ( FILE_LIST * ) * all_pid->size );

    for ( proc_cnt = 0; proc_cnt < all_pid->size; proc_cnt++ )
    {
        /* reset list */
        proc_head = NULL;
        proc_tail = NULL;

        /* reset buf */
        memset ( command, 0, sizeof ( char ) * ( COMMAND_NAME_MAX ) );
        memset ( username, 0, sizeof ( char ) * ( USERNAME_MAX ) );

        /* The current progress pid */
        current_pid = all_pid->pids[proc_cnt];

        /* Get command by pid */
        if ( get_cmd_and_username ( command, username, current_pid ) == -1 )
            continue;

        /* Check if command pass the filter */
        check_filter = check_command_pass ( command, filter );
        if ( !check_filter )
            continue;

        /* Generate a template file node for current process */
        strncpy_append ( template.command, command, COMMAND_NAME_MAX - 1 );
        strncpy_append ( template.username, username, USERNAME_MAX - 1 );
        template.pid = current_pid;

        /* Get files from different source */
        res = get_cwd ( current_pid, template );
        append_list ( res, &proc_head, &proc_tail );

        res = get_root ( current_pid, template );
        append_list ( res, &proc_head, &proc_tail );

        res = get_exe ( current_pid, template );
        append_list ( res, &proc_head, &proc_tail );

        /* get file in fd directory */
        res = get_all_fd_files ( current_pid, template );

        /* get file in mems file */
        map_res = read_maps_file ( current_pid, template );

        /* remove duplicated find in all other source*/
        map_res = remove_dup_files_in_mems ( proc_head, map_res );
        map_res = remove_dup_files_in_mems ( res, map_res );

        /* append mems first */
        append_list ( map_res, &proc_head, &proc_tail );

        /* then the /fd */
        append_list ( res, &proc_head, &proc_tail );

        proc_head = filter_apply ( filter, proc_head );

        all_proc_file_list[proc_cnt] = proc_head;
    }

    qsort ( all_proc_file_list, all_pid->size, sizeof ( FILE_LIST * ), proc_compare );

    return all_proc_file_list;
}

int get_cmd_and_username ( char * command, char * username, pid_t pid )
{
    char temp_path[FILE_PATH_MAX];
    char tmp_cmd[COMMAND_NAME_MAX]  = { '\0' };
    char tmp_username[USERNAME_MAX] = { '\0' };
    char line_str[1024];
    char * find_ptr;
    int find_name, find_user;

    uid_t user_id;
    struct passwd * password;

    FILE * st_file;

    /* status file contains info we need */
    snprintf ( temp_path, FILE_PATH_MAX - 1, "/proc/%d/status", pid );

    /* open file to read */
    st_file = fopen ( temp_path, "r" );
    if ( st_file == NULL )
        return -1;

    /* find each line in this file */
    find_user = 0;
    find_name = 0;
    while ( fscanf ( st_file, "%1023[^\n] ", line_str ) != EOF )
    {
        /* find "Name: " Row */
        find_ptr = strstr ( line_str, "Name:" );

        if ( find_ptr == line_str )
        {
            for ( find_ptr = find_ptr + 5; *find_ptr != '\0' && isspace ( (unsigned char) *find_ptr ); )
                find_ptr++;
            strncpy_append ( tmp_cmd, find_ptr, COMMAND_NAME_MAX - 1 );

            find_name = 1;
        }

        /* find "Uid: " Row */
        find_ptr = strstr ( line_str, "Uid:" );
        if ( find_ptr == line_str )
        {
            sscanf ( find_ptr, "Uid: %d", &user_id );

            /* Get real user name from uid */
            password = getpwuid ( user_id );

            if ( password == NULL )
            {
                fclose ( st_file );
                return -1;
            }

            strncpy_append ( tmp_username, password->pw_name, USERNAME_MAX - 1 );

            find_user = 1;
        }

        if ( find_name && find_user )
            break;
    }

    if ( tmp_cmd[strnlen ( tmp_cmd, COMMAND_NAME_MAX - 1 ) - 1] == '\n' )
        tmp_cmd[strnlen ( tmp_cmd, COMMAND_NAME_MAX - 1 ) - 1] = '\0';

    if ( tmp_username[strnlen ( tmp_username, USERNAME_MAX - 1 ) - 1] == '\n' )
        tmp_username[strnlen ( tmp_username, USERNAME_MAX - 1 ) - 1] = '\0';

    fclose ( st_file );

    strncpy_append ( command, tmp_cmd, COMMAND_NAME_MAX - 1 );
    strncpy_append ( username, tmp_username, COMMAND_NAME_MAX - 1 );

    return 1;
}

FILE_LIST * get_cwd ( const pid_t pid, const FILE_LIST template )
{
    char cwd_path[FILE_PATH_MAX];

    FILE_LIST * res;

    snprintf ( cwd_path, FILE_PATH_MAX - 1, "/proc/%d/cwd", pid );

    res = read_file_stat_path ( cwd_path, template );

    if ( res )
    {
        strncpy_append ( res->file_descriptior, "cwd", FILE_DESCRIPTOR_MAX - 1 );
    }

    return res;
}

FILE_LIST * get_root ( const pid_t pid, const FILE_LIST template )
{
    char root_path[FILE_PATH_MAX];

    FILE_LIST * res;

    snprintf ( root_path, FILE_PATH_MAX - 1, "/proc/%d/root", pid );

    res = read_file_stat_path ( root_path, template );

    if ( res )
    {
        strncpy_append ( res->file_descriptior, "root", FILE_DESCRIPTOR_MAX - 1 );
    }

    return res;
}

FILE_LIST * get_exe ( const pid_t pid, const FILE_LIST template )
{
    char exe_path[FILE_PATH_MAX];

    FILE_LIST * res;

    snprintf ( exe_path, FILE_PATH_MAX - 1, "/proc/%d/exe", pid );

    res = read_file_stat_path ( exe_path, template );

    if ( res )
    {
        strncpy_append ( res->file_descriptior, "exe", FILE_DESCRIPTOR_MAX - 1 );
    }

    return res;
}

FILE_LIST * get_all_fd_files ( const pid_t pid, const FILE_LIST template )
{
    int fd_num;
    int str_parse;
    char fd_dir_path[FILE_PATH_MAX];

    DIR * fd_dir;
    struct dirent * fd_file_dirent;

    FILE_LIST * res  = NULL;
    FILE_LIST * head = NULL;
    FILE_LIST * tail = NULL;

    snprintf ( fd_dir_path, FILE_PATH_MAX - 1, "/proc/%d/fd", pid );

    fd_dir = opendir ( fd_dir_path );

    // failed to open dir
    if ( fd_dir == NULL )
    {
        res = (FILE_LIST *) check_malloc ( sizeof ( FILE_LIST ) );
        strncpy_append ( res->command, template.command, COMMAND_NAME_MAX - 1 );
        res->pid = template.pid;
        strncpy_append ( res->username, template.username, USERNAME_MAX - 1 );
        strncpy_append ( res->file_descriptior, "NOFD", FILE_DESCRIPTOR_MAX - 1 );
        strncpy_append ( res->file_path, fd_dir_path, FILE_PATH_MAX - 1 );
        get_error_message ( errno, "opendir", res->file_path + strlen ( res->file_path ), ERROR_STR_LEN_MAX - 1 );

        res->type         = -1;
        res->inode_number = -1;
        res->next         = NULL;

        return res;
    }

    while ( ( fd_file_dirent = readdir ( fd_dir ) ) != NULL )
    {
        str_parse = sscanf ( fd_file_dirent->d_name, "%d", &fd_num );

        // The d_name contains something else but number
        if ( str_parse != 1 )
            continue;

        res = read_file_stat_fd ( pid, fd_num, template );

        if ( res )
        {
            res->next = NULL;

            if ( head == NULL )
            {
                head = tail = res;
            }
            else
            {
                tail->next = res;
                tail       = res;
            }
        }
    }

    closedir ( fd_dir );

    return head;
}

int check_command_pass ( const char * command, const PROC_FILTER * filter )
{
    regmatch_t match;
    const int nmatch = 1;
    const int eflags = 0;
    int match_res;

    if ( strnlen ( command, COMMAND_NAME_MAX - 1 ) == 0 )
        return 0;

    if ( filter->command_regex )
    {
        match_res = regexec ( filter->command_regex, command, nmatch, &match, eflags );

        if ( match_res == 0 )
            return 1;
        else
            return 0;
    }

    return 1;
}

int check_file_path_pass ( const char * name, const PROC_FILTER * filter )
{
    regmatch_t match;
    const int nmatch = 1;
    const int eflags = 0;
    int match_res;

    if ( strnlen ( name, FILE_PATH_MAX - 1 ) == 0 )
        return 0;

    if ( filter->filename_regex )
    {
        match_res = regexec ( filter->filename_regex, name, nmatch, &match, eflags );

        if ( match_res == 0 )
            return 1;
        else
            return 0;
    }

    return 1;
}

int check_file_type_pass ( FILE_TYPE type, const PROC_FILTER * filter )
{
    if ( filter->type_filter )
    {
        if ( strcmp ( filter->type_filter, "REG" ) == 0 )
        {
            if ( type == TYPE_REG )
                return 1;
        }
        else if ( strcmp ( filter->type_filter, "CHR" ) == 0 )
        {
            if ( type == TYPE_CHR )
                return 1;
        }
        else if ( strcmp ( filter->type_filter, "DIR" ) == 0 )
        {
            if ( type == TYPE_DIR )
                return 1;
        }
        else if ( strcmp ( filter->type_filter, "FIFO" ) == 0 )
        {
            if ( type == TYPE_FIFO )
                return 1;
        }
        else if ( strcmp ( filter->type_filter, "SOCK" ) == 0 )
        {
            if ( type == TYPE_SOCK )
                return 1;
        }
        else if ( strcmp ( filter->type_filter, "unknown" ) == 0 )
        {
            if ( type == TYPE_UNKNOWN )
                return 1;
        }

        return 0;
    }

    return 1;
}

void append_list ( FILE_LIST * target, FILE_LIST ** head, FILE_LIST ** tail )
{
    if ( target == NULL )
        return;

    if ( *head == NULL )
    {
        *head = target;
        *tail = target;
    }
    else if ( *tail == NULL )
    {
        *tail = *head;
        while ( ( *tail )->next != NULL )
            *tail = ( *tail )->next;

        ( *tail )->next = target;
        *tail           = target;
    }
    else
    {
        ( *tail )->next = target;
        *tail           = target;
    }

    while ( ( *tail )->next != NULL )
        ( *tail ) = ( *tail )->next;
}

FILE_LIST * remove_dup_files_in_mems ( const FILE_LIST * all, FILE_LIST * mems )
{
    FILE_LIST * mems_head = mems;
    FILE_LIST * mems_tail = mems;
    FILE_LIST * mems_prev = NULL;

    const FILE_LIST * all_tail = all;
    ino_t current_cmp_inode;
    const char * current_cmp_name;

    while ( all_tail != NULL && mems_head != NULL )
    {
        mems_tail = mems_head;
        mems_prev = NULL;

        current_cmp_inode = all_tail->inode_number;
        current_cmp_name  = all_tail->file_path;

        /* There are no duplicated file is mems, only check once */
        if ( mems_head != NULL && mems_head->inode_number == current_cmp_inode && strncmp ( mems_head->file_path, current_cmp_name, FILE_PATH_MAX - 1 ) == 0 )
        {
            mems_head = mems_head->next;

            check_free ( mems_tail );

            all_tail = all_tail->next;
            continue;
        }

        mems_tail = mems_head->next;
        mems_prev = mems_head;

        while ( mems_tail != NULL )
        {
            if ( mems_tail->inode_number == current_cmp_inode && strncmp ( mems_tail->file_path, current_cmp_name, FILE_PATH_MAX - 1 ) == 0 )
            {
                mems_tail = mems_tail->next;
                check_free ( mems_prev->next );
                mems_prev->next = mems_tail;

                break;
            }

            if ( mems_tail != NULL )
            {
                mems_prev = mems_tail;
                mems_tail = mems_tail->next;
            }
        }

        all_tail = all_tail->next;
    }

    return mems_head;
}

FILE_LIST * filter_apply ( const PROC_FILTER * filter, FILE_LIST * all )
{
    FILE_LIST * head = all;
    FILE_LIST * tail = all;
    FILE_LIST * prev = NULL;

    if ( filter->type_filter || filter->filename_regex )
    {
        /* apply on head first */
        while ( head != NULL && ( check_file_type_pass ( head->type, filter ) == 0 || check_file_path_pass ( head->file_path, filter ) == 0 ) )
        {
            head = head->next;

            check_free ( tail );

            tail = head;
        }

        if ( head == NULL )
            return NULL;

        prev = head;
        tail = head->next;

        while ( tail != NULL )
        {
            if ( check_file_type_pass ( tail->type, filter ) == 0 || check_file_path_pass ( tail->file_path, filter ) == 0 )
            {
                tail = tail->next;

                check_free ( prev->next );

                prev->next = tail;
            }
            else
            {
                prev = tail;
                tail = tail->next;
            }
        }
    }

    return head;
}

int proc_compare ( const void * a, const void * b )
{
    FILE_LIST * _a = *( (FILE_LIST **) a );
    FILE_LIST * _b = *( (FILE_LIST **) b );

    if ( _a != NULL && _b != NULL )
    {
        return _a->pid - _b->pid;
    }
    else if ( _a != NULL && _b == NULL )
    {
        return 1;
    }
    else if ( _a == NULL && _b != NULL )
    {
        return -1;
    }
    else
    {
        return 0;
    }
}