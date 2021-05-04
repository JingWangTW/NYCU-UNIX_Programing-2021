#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main ( )
{
    int fd;
    char buf[16] = { 0 };
    FILE * tmp_file;
    FILE * test_file;

    fclose ( stderr );

    fd = creat ( "./test_creat", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
    close ( fd );
    fd = open ( "./test_open", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
    write ( fd, "12345", 5 );
    close ( fd );
    fd = open ( "./test_open", O_RDWR );
    read ( fd, buf, 5 );
    if ( strcmp ( buf, "12345" ) != 0 )
        exit ( -1 );
    close ( fd );

    chmod ( "./test_creat", S_ISVTX | S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH );
    chown ( "./test_creat", geteuid ( ), getegid ( ) );

    tmp_file = tmpfile ( );
    fclose ( tmp_file );

    test_file = fopen ( "./test_fopen", "w" );
    fwrite ( "12345", 5, 1, test_file );
    fclose ( test_file );

    test_file = fopen ( "./test_fopen", "r" );
    fread ( buf, 5, 1, test_file );
    if ( strcmp ( buf, "12345" ) != 0 )
        exit ( -1 );
    fclose ( test_file );

    rename ( "./test_fopen", "./test_rename" );

    remove ( "./test_rename" );
    remove ( "./test_creat" );
    remove ( "./test_open" );

    return 0;
}