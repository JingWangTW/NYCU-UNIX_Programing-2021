#ifndef __LINUX_CMD_H__
#define __LINUX_CMD_H__

#include <stdio.h>
#include <sys/stat.h>

int linux_chmod ( const char * pathname, mode_t mode );
int linux_chown ( const char * pathname, uid_t owner, gid_t group );
int linux_close ( int fd );
int linux_open ( const char * pathname, int flags, mode_t mode );
int linux_read ( int fd, void * buf, size_t count );
int linux_remove ( const char * pathname );
int linux_creat ( const char * pathname, mode_t mode );
int linux_rename ( const char * oldpath, const char * newpath );

#endif