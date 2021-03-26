# HW1 - Implement a `lsof`-like program

## How to build
```
make
```

## How to run
```
./hw1
```

## About the implementation
### `FD - exe`
* The real [`lsof(8)`](https://linux.die.net/man/8/lsof) has no fd type `exe`.
* The homework spec [said](Spec.md#L43) the fd type `exe` is gotten from `/proc/{pid}/exe`.
* In the real `lsof(8)`, the FD of executable is `txt`.
* In the real `lsof(8)`, if the real `/proc/{pid}/exe` is not exist, it will not be shown in the result. In this homework implementation, I chose to show it with error text ,` (readlink: File Not Found)`, behind.

## Reference
### Work Relative
* `/proc`:
    * In short, in this homework, we only need the number-named directory under `/proc`.
    * Each numbered directory `/proc` match an alive(may run, sleep, etc.) process and the name of the directory is the `pid` of the process.
    * The purpose of other files/dirs under `/proc` you can see [here](https://tldp.org/LDP/Linux-Filesystem-Hierarchy/html/proc.html).
    * Most below info about files/dirs under `/proc` is capture from [proc(5)](https://man7.org/linux/man-pages/man5/procfs.5.html).
* `COMMAND`:
    * `/proc/{pid}/comm`
    * You can get the `COMMAND` by reading `/proc/{pid}/comm` file.
    * Another approaches: 
        * `Name` filed in `/proc/{pid}/status` file.
        * Second filed in `/proc/{pid}/stat` file. Show in parenthesses. 
* `USER`:
    * I got this in two steps:
        * First: Get (real) uid that ran the process from `/proc/{pid}/status`.
        * Second: Use uid to get respective user name.
    * `Uid`:
        * There are four numbers in Uid field in `/proc/{pid}/status`.
        * They are: Real, effective, saved set, and filesystem UIDs.
* `FD`:
    * `cwd`: 
        * `/proc/{pid}/cwd`
        * The current working directory (cwd) of the process.
        * `/proc/{pid}/cwd` is a symbolic link to the cwd of the process.
        * Need to get the file info that the link point to.
        * **Warn**: Need to check the permission to dereference symbilic link.
    * `root`: 
        * `/proc/{pid}/root`
        * `/proc/{pid}/root` is a symbolic link to that points to the process's root directory.
        * UNIX and Linux support the idea of a per-process root of the filesystem, set by the `chroot(2)` system call.
        * Need to get the file info that the link point to.
        * **Warn**: Need to check the permission to dereference symbilic link.
    * `exe`: 
        * `/proc/{pid}/exe`
        * Under Linux 2.2 and later
            * `/proc/{pid}/exe` is a symbolic link containing the actual pathname of the executed command.
            * If the pathname has been unlinked, the symbolic link will contain the string '(deleted)' appended to the original pathname.
            * In a multithreaded process, the contents of this symbolic link are not available if the main thread has already terminated.
            * **Warn**: The real `exe` file may not exist though you can get it by listed `/proc/{pid}` and see its status from `stat()`. Need to check the error code return from `readlink()`.
        * Under Linux 2.0 and earlier
            * `/proc/{pid}/exe` is a pointer to the binary which was executed, and appears as a symbolic link.
            * A `readlink(2)` call on this file under Linux 2.0 returns a string in the format: 
                `[device]:inode`
    * `[0-9]+[rwu]`, `del`:
        * `/proc/{pid}/fd/{fd_no}`, `/proc/{pid}/fdinfo/{fd_no}`, 
        * `/proc/{pid}/fd/{fd_no}`
            * You can get what file has been opened by process in `/proc/{pid}/fd/{fd_no}`. The name of the file under `/proc/{pid}/fd` is the respective file descriptor.
            * Each file under `/proc/{pid}/fd` is a symbolic link to the actual file.
        * `/proc/{pid}/fdinfo/{fd_no}`
            * You can get the file access mode and status flags from the `flags` field in `/proc/{pid}/fdinfo/{fd_no}`.
            * **Warn**: `flags` field is an octal number.
        * **Hint**: In some cases, though the file is being opened by a process, it still can be deleted.
            * Yes, it's possible. In this case,the file still can be readable by the process. And the file would be actually deleted after process terminated. However, you are not able to see it listed in the disk file system.
            * In this case, the string `(deleted)` would be appended in the result of `readlink()`.
            * You can get the inode of deleted file (not the link itself) by following two steps:
                * Get a local fd from the result of `open()` `/proc/{pid}/fd/{fd_no}`. (Just open it.)
                * Feed the local fd to the `fstat()` and you will get the inode of the deleted file.

    * `maps`, `del`:
        * `/proc/{pid}/maps`
            * Containing the currently mapped memory regions and their access permissions.
            * Format: 
                ```
                address           perms offset  dev   inode       pathname
                00400000-00452000 r-xp 00000000 08:02 173521      /usr/bin/dbus-daemon
                00651000-00652000 r--p 00051000 08:02 173521      /usr/bin/dbus-daemon
                00652000-00655000 rw-p 00052000 08:02 173521      /usr/bin/dbus-daemon
                ```
            * The value in `perms` field would be `r`(read), `w`(write), `x`(execute), `s`(shared), `p`(private).
            * There are some pseudo-paths:
                * `[stack]`, `[stack:<tid>]`, `[vsdo]`, `[heap]`
        * `pathname` may be appended `(deleted)` indicate the file has been deleted. Need to change `FD` field from `mem` to `del`.

### Programing Relative
* Parsing Arguments
    * [`getopt(3)`](https://man7.org/linux/man-pages/man3/getopt.3.html)
    * [Example usage](https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html) of `getopt()`.
* Regular Expression
    * [`regex(3)`](https://man7.org/linux/man-pages/man3/regex.3.html)
    * `regcomp()`: Compile the regular expressiont, e.g. test if the regex is valid.
    * `regexec()`: Used to test if the input string is matched with the regex.
    * `regerror()`: Get human read string from the error code output from `regcomp()` or `regexec()`.
    * `regfree()`: Release regex obj got from `regcomp()`.
* Uid to username
    * [`getpwuid()`](https://linux.die.net/man/3/getpwuid)
    * Get a pointer to a structure containing the broken-out fields of the record in the password database that matches the user ID `uid`.
* File Access Mode
    * [`fcntl.h(0P)`](https://man7.org/linux/man-pages/man0/fcntl.h.0p.html)
    * A flag pass to [`open(2)`](https://man7.org/linux/man-pages/man2/open.2.html) or a flag get from `/proc/[pid]/fdinfo/{fd_no}`
    * `O_ACCMODE`: Since the flag mentioned above contain both access mode and status flags. You can use this mask to leave access mode only.
    * `O_RDONLY`,`O_RDWR`, `O_WRONLY`: Test with these macros, then you can get the access mode of the flag.
* Limits on File System Capacity
    * [limits.h(0p)](https://man7.org/linux/man-pages/man0/limits.h.0p.html)
    * `NAME_MAX`: The limit a file name component, **not including** the terminating null character.
    * `PATH_MAX`: The limit for the length of an entire file name (that is, the argument given to system calls such as open), **including** the terminating null character.
* Limits on Names
    * [sysconf(3)](https://man7.org/linux/man-pages/man3/sysconf.3.html)
    * `LOGIN_NAME_MAX`: Maximum length of a login name, **including** the terminating null byte.