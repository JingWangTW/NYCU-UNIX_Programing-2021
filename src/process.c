#include "process.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <linux/limits.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "file.h"
#include "util.h"

int get_cmd_username ( char * command, char * username, pid_t pid );
int check_command_pass ( const char * command, PROC_FILTER * filter );
int check_name_pass ( const char * name, const PROC_FILTER * filter );

FILE_LIST * get_cwd ( const pid_t pid, const PROC_FILTER * filter, const FILE_LIST template );
FILE_LIST * get_root ( const pid_t pid, const PROC_FILTER * filter, const FILE_LIST template );
FILE_LIST * get_exe ( const pid_t pid, const PROC_FILTER * filter, const FILE_LIST template );
FILE_LIST * get_mem ( const pid_t pid, const PROC_FILTER * filter, const FILE_LIST template );
FILE_LIST * get_all_fd_files ( const pid_t pid, const PROC_FILTER * filter, const FILE_LIST template );

PID_LIST get_all_pids ( )
{
    int pid_count = 0;
    char test_name_buf[NAME_MAX + 1];

    pid_t temp_pid;
    PID_LIST res;
    DIR * proc_dir;
    struct dirent * proc_dir_entry;

    res.size = 100;
    res.list = (pid_t *) check_malloc ( sizeof ( pid_t ) * res.size );
    proc_dir = opendir ( "/proc" );

    // read all entry in the directory
    while ( ( proc_dir_entry = readdir ( proc_dir ) ) != NULL )
    {
        memset ( test_name_buf, 0, sizeof ( char ) * ( NAME_MAX + 1 ) );
        // only find directory
        if ( proc_dir_entry->d_type == DT_DIR )
        {
            // skip this two special dir
            if ( strcmp ( proc_dir_entry->d_name, "." ) != 0 && strcmp ( proc_dir_entry->d_name, ".." ) != 0 )
            {
                // get the entry that its name only contains numbers
                sscanf ( proc_dir_entry->d_name, "%[0-9]", test_name_buf );

                if ( strcmp ( proc_dir_entry->d_name, test_name_buf ) == 0 )
                {
                    sscanf ( proc_dir_entry->d_name, "%d", &temp_pid );

                    if ( pid_count == res.size )
                    {
                        res.size += 100;
                        res.list = check_realloc ( res.list, sizeof ( pid_t ) * res.size );
                    }

                    res.list[pid_count] = temp_pid;

                    pid_count++;
                }
            }
        }
    }

    closedir ( proc_dir );

    res.list = check_realloc ( res.list, sizeof ( pid_t ) * pid_count );
    res.size = pid_count;

    return res;
}

FILE_LIST * get_all_proc_files ( PID_LIST pid_list, PROC_FILTER * filter )
{
    int check_filter;
    int proc_cnt;
    char command[NAME_MAX + 1];
    char username[LOGIN_NAME_MAX + 1];
    pid_t current_pid;

    FILE_LIST template;
    // FILE_LIST * res    = NULL;
    FILE_LIST * f_list = NULL;

    for ( proc_cnt = 0; proc_cnt < pid_list.size; proc_cnt++ )
    {
        /* reset buf */
        memset ( command, 0, sizeof ( char ) * ( NAME_MAX + 1 ) );
        memset ( username, 0, sizeof ( char ) * ( LOGIN_NAME_MAX + 1 ) );

        /* The current progress pid */
        current_pid = pid_list.list[proc_cnt];

        /* get command by pid */
        get_cmd_username ( command, username, current_pid );

        /* Check if command pass the filter */
        check_filter = check_command_pass ( command, filter );
        if ( !check_filter )
            continue;

        /* generate a template file node for current process */
        strcpy ( template.command, command );
        strcpy ( template.user_name, username );
        template.pid = current_pid;

        f_list = get_mem ( current_pid, filter, template );

        while ( f_list != NULL )
        {
            f_list->command[9] = '\0';
            printf ( "%15s %7d %20s %8s %8d %8ld %s\n", f_list->command, f_list->pid, f_list->user_name, f_list->file_descriptior, f_list->type, f_list->inode_number, f_list->file_name );

            f_list = f_list->next;
        }
    }

    return f_list;
}

int get_cmd_username ( char * command, char * username, pid_t pid )
{
    char temp_path[PATH_MAX];
    char tmp_cmd_name[NAME_MAX + 2]        = { 0 };
    char tmp_user_name[LOGIN_NAME_MAX + 2] = { 0 };
    char line_str[1000];
    char * find_ptr;
    int find_name, find_user;

    uid_t user_id;
    struct passwd * password;

    FILE * st_file;

    /* status file contains info we need */
    sprintf ( temp_path, "/proc/%d/status", pid );

    /* open file to read */
    st_file = fopen ( temp_path, "r" );
    if ( st_file == NULL )
        return -1;

    /* find each line in this file */
    find_user = 0;
    find_name = 0;
    while ( fscanf ( st_file, "%1000[^\n] ", line_str ) != EOF )
    {
        /* find "Name: " Row */
        find_ptr = strstr ( line_str, "Name:" );

        if ( find_ptr == line_str )
        {
            for ( find_ptr = find_ptr + 5; *find_ptr != '\0' && isspace ( (unsigned char) *find_ptr ); )
                find_ptr++;
            strcpy ( tmp_cmd_name, find_ptr );

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

            strcpy ( tmp_user_name, password->pw_name );

            find_user = 1;
        }

        if ( find_name && find_user )
            break;
    }

    if ( tmp_cmd_name[strlen ( tmp_cmd_name ) - 1] == '\n' )
        tmp_cmd_name[strlen ( tmp_cmd_name ) - 1] = '\0';

    if ( tmp_user_name[strlen ( tmp_user_name ) - 1] == '\n' )
        tmp_user_name[strlen ( tmp_user_name ) - 1] = '\0';

    fclose ( st_file );

    strcpy ( command, tmp_cmd_name );
    strcpy ( username, tmp_user_name );

    return 1;
}

FILE_LIST * get_cwd ( const pid_t pid, const PROC_FILTER * filter, const FILE_LIST template )
{
    char cwd_path[PATH_MAX];

    FILE_LIST * res;

    sprintf ( cwd_path, "/proc/%d/cwd", pid );

    res = read_file_stat_path ( cwd_path, template );

    if ( res )
    {
        strcpy ( res->file_descriptior, "cwd" );

        if ( check_name_pass ( res->file_name, filter ) == 0 )
        {
            check_free ( res );
            return NULL;
        }
    }

    return res;
}

FILE_LIST * get_root ( const pid_t pid, const PROC_FILTER * filter, const FILE_LIST template )
{
    char root_path[PATH_MAX];

    FILE_LIST * res;

    sprintf ( root_path, "/proc/%d/root", pid );

    res = read_file_stat_path ( root_path, template );

    if ( res )
    {
        strcpy ( res->file_descriptior, "root" );

        if ( check_name_pass ( res->file_name, filter ) == 0 )
        {
            check_free ( res );
            return NULL;
        }
    }

    return res;
}

FILE_LIST * get_exe ( const pid_t pid, const PROC_FILTER * filter, const FILE_LIST template )
{
    char exe_path[PATH_MAX];

    FILE_LIST * res;

    sprintf ( exe_path, "/proc/%d/exe", pid );

    res = read_file_stat_path ( exe_path, template );

    if ( res )
    {
        strcpy ( res->file_descriptior, "exe" );

        if ( check_name_pass ( res->file_name, filter ) == 0 )
        {
            check_free ( res );
            return NULL;
        }
    }

    return res;
}

FILE_LIST * get_mem ( const pid_t pid, const PROC_FILTER * filter, const FILE_LIST template )
{
    FILE_LIST * head;
    FILE_LIST * tail;
    FILE_LIST * temp;

    head = read_maps_file ( pid, template );
    tail = head;

    while ( tail != NULL )
    {
        if ( check_name_pass ( tail->file_name, filter ) == 0 )
        {
            temp = tail->next;

            if ( head == tail )
                head = temp;

            check_free ( tail );
            tail = temp;
        }
        else
        {
            tail = tail->next;
        }
    }

    return head;
}

FILE_LIST * get_all_fd_files ( const pid_t pid, const PROC_FILTER * filter, const FILE_LIST template )
{
    DIR * fd_dir;
    struct dirent * fd_file_dirent;
    int fd_num;
    int str_parse;

    char fd_dir_path[PATH_MAX];

    FILE_LIST * res  = NULL;
    FILE_LIST * head = NULL;
    FILE_LIST * tail = NULL;

    sprintf ( fd_dir_path, "/proc/%d/fd", pid );

    fd_dir = opendir ( fd_dir_path );

    // failed to open dir
    if ( fd_dir == NULL )
    {
        res = (FILE_LIST *) check_malloc ( sizeof ( FILE_LIST ) );
        strcpy ( res->command, template.command );
        res->pid = template.pid;
        strcpy ( res->user_name, template.user_name );

        strcpy ( res->file_name, fd_dir_path );
        get_error_message ( errno, "opendir", res->file_name + strlen ( res->file_name ) );

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
            if ( check_name_pass ( res->file_name, filter ) == 0 )
            {
                check_free ( res );
                continue;
            }

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

int check_command_pass ( const char * command, PROC_FILTER * filter )
{
    regmatch_t match;
    const int nmatch = 1;
    const int eflags = 0;
    int match_res;

    if ( strlen ( command ) == 0 )
        return 0;

    if ( filter->command_reg )
    {
        match_res = regexec ( filter->command_reg, command, nmatch, &match, eflags );

        if ( match_res == 0 )
            return 1;
        else
            return 0;
    }

    return 1;
}

int check_name_pass ( const char * name, const PROC_FILTER * filter )
{
    regmatch_t match;
    const int nmatch = 1;
    const int eflags = 0;
    int match_res;

    if ( strlen ( name ) == 0 )
        return 0;

    if ( filter->filename_reg )
    {
        match_res = regexec ( filter->filename_reg, name, nmatch, &match, eflags );

        if ( match_res == 0 )
            return 1;
        else
            return 0;
    }

    return 1;
}