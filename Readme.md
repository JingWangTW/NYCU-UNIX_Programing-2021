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
* Limits on File System Capacity
    * [limits.h(0p)](https://man7.org/linux/man-pages/man0/limits.h.0p.html)
    * `NAME_MAX`: The limit a file name component, **not including** the terminating null character.
    * `PATH_MAX`: The limit for the length of an entire file name (that is, the argument given to system calls such as open), **including** the terminating null character.
* Limits on Names
    * [sysconf(3)](https://man7.org/linux/man-pages/man3/sysconf.3.html)
    * `LOGIN_NAME_MAX`: Maximum length of a login name, **including** the terminating null byte.