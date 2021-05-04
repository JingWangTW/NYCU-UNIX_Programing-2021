#ifndef __LINUX_CMD_H__
#define __LINUX_CMD_H__

#include <stdio.h>
#include <sys/stat.h>

int linux_chmod ( const char * pathname, mode_t mode );
int linux_chown ( const char * pathname, uid_t owner, gid_t group );

int linux_creat ( const char * pathname, mode_t mode );
int linux_creat64 ( const char * pathname, mode_t mode );
int linux_remove ( const char * pathname );
int linux_rename ( const char * oldpath, const char * newpath );

int linux_open ( const char * pathname, int flags, mode_t mode );
int linux_open64 ( const char * pathname, int flags, mode_t mode );
int linux_close ( int fd );
ssize_t linux_read ( int fd, void * buf, size_t count );
ssize_t linux_write ( int fd, const void * buf, size_t count );

FILE * linux_fopen ( const char * pathname, const char * mode );
FILE * linux_fopen64 ( const char * pathname, const char * mode );
int linux_fclose ( FILE * stream );
size_t linux_fread ( void * ptr, size_t size, size_t nmemb, FILE * stream );
size_t linux_fwrite ( const void * ptr, size_t size, size_t nmemb, FILE * stream );

FILE * linux_tmpfile ( );
FILE * linux_tmpfile64 ( );

#endif