# HW1 - Implement a `lsof`-like program

## How to build
```
make
```

## How to run
```
./hw1
```

## Directory Structure
```
.
├── include         # all header files
│   ├── file.h      # define some limitation of string length
│   ├── io.h
│   ├── process.h
│   └── util.h
├── main.c          # main file
├── Makefile
├── README.md
├── Spec.md         # homework spec 
└── src             # all source file
    ├── file.c      # some functions about "file", to get all information of "file"
    ├── io.c        # some function to parse input and format output
    ├── process.c   # some functions about "process", to get all information of "process"
    └── util.c      # some utility wrap functions.
```

## Implementation Issue

### `COMMAND`
* In the real [`lsof(8)`](https://linux.die.net/man/8/lsof), the `COMMAND` field will only show the first nine characters of the UNIX command associated with the process.
* There has no relative restriction in the homework spec. (Maybe just print it all. 🤔)
* I chose to follow `lsof(8)`, just print the first nine characters.

### `FD` - `exe`
* The real [`lsof(8)`](https://linux.die.net/man/8/lsof) has no FD type with `exe`.
* The homework spec [said](Spec.md#L43) the FD type `exe` is gotten from `/proc/{pid}/exe`.
* In the real `lsof(8)`, the FD of executable of the process is `txt`.
* In the real `lsof(8)`, if the real `/proc/{pid}/exe` does not existed, it will not be shown in the result.
* In this implementation, I chose to show it with error text behind the file name.

### `FD` - `mem`
#### Pseudo path
* There are some pseudo-paths record in file `/proc/{pid}/maps`.
* In the real [`lsof(8)`](https://linux.die.net/man/8/lsof), these files would not be printed.
* There has no instruction in homework spec for this case.
* In this implementation, I chose to not show this infos either.
#### `exe` file
* The `exe` file of the process will also show in `/proc/{pid}/maps` file.
* In the real [`lsof(8)`](https://linux.die.net/man/8/lsof), it will not be shown again.
* There is no clear instruction of this situation in the homework spec. You can only observe it by the example in the homework spec. `exe` file shows twice in the example in the homework spec.
* In this implementation, I chose to follow the real `lsof(8)` to not show it again.

### `TYPE`
* In some cases, the symbolic file under `/proc/{pid}/fd` may link to a FD that has no inode.
* These FDs are produced by [`bpf(2)`](https://man7.org/linux/man-pages/man2/bpf.2.html), [`epoll_create(2)`](https://man7.org/linux/man-pages/man2/epoll_create.2.html), etc.
* The result of [`readlink(2)`](https://man7.org/linux/man-pages/man2/readlinkat.2.html) for these files would be formatted as  
    `anon_inode:<file-type>`
* In this implementation
    * I chose to show `unknown` in the `TYPE` field.
    * In `NAME` field, I chose to follow the real [`lsof(8)`](https://linux.die.net/man/8/lsof) to show the `<file-type>` only.

### Sorting Output
* There are no restricted rules about the sorting of results.
* In this implementation, it was sorted by the following order:
    * `PID`
    * `TYPE`
        * `cwd`
        * `root`
        * `exe`
        * `mem`
            * Follow by the order in `/proc/{pid}/maps`.
        * `del`
        * `[0-9]+[rwu]`
            * Follow by the order get from [`readdir(2)`](https://man7.org/linux/man-pages/man2/readdir.2.html) with `/proc/{pid}/fd`.
        * `NOFD`

## Reference
### Work Relative
* `/proc`:
    * In short, in this homework, we only need the number-named directory under `/proc`.
    * Each numbered directory `/proc` matches an alive(may run, sleep, etc.) process and the name of the directory is the `pid` of the process.
    * The purpose of other files/dirs under `/proc` you can see [here](https://tldp.org/LDP/Linux-Filesystem-Hierarchy/html/proc.html).
    * Most below info about files/dirs under `/proc` is capture from [proc(5)](https://man7.org/linux/man-pages/man5/procfs.5.html).
* `COMMAND`:
    * `/proc/{pid}/comm`
    * You can get the `COMMAND` by reading `/proc/{pid}/comm` file.
    * Other approaches: 
        * `Name` field in `/proc/{pid}/status` file.
        * Second field in `/proc/{pid}/stat` file. Show in parentheses. 
* `USER`:
    * I got this in two steps:
        * First: Get (real) uid that ran the process from `/proc/{pid}/status`.
        * Second: Use uid to get the respective user name.
    * `Uid`:
        * There are four numbers in Uid field in `/proc/{pid}/status`.
        * They are: real, effective, saved set, and filesystem UIDs.
* `FD`:
    * `cwd`: 
        * `/proc/{pid}/cwd`
        * The current working directory (cwd) of the process.
        * `/proc/{pid}/cwd` is a symbolic link to the cwd of the process.
        * Need to get the file info that the symbolic link to.
        * **Warn**: Need to check the permission to dereference the symbilic.
    * `root`: 
        * `/proc/{pid}/root`
        * `/proc/{pid}/root` is a symbolic link to the process's root directory.
        * UNIX and Linux support the idea of a per-process root of the filesystem, set by the [`chroot(2)`](https://man7.org/linux/man-pages/man2/chroot.2.html) system call.
        * Need to get the file info that the symbolic link to.
        * **Warn**: Need to check the permission to dereference the symbolic.
    * `exe`: 
        * `/proc/{pid}/exe`
        * Under Linux 2.2 and later
            * `/proc/{pid}/exe` is a symbolic link containing the actual pathname of the executed command.
            * If the pathname has been unlinked, the symbolic link will contain the string `'(deleted)'` appended to the original pathname.
            * In a multithreaded process, the contents of this symbolic link are not available if the main thread has already terminated.
            * **Warn**: The real `exe` file may not exist though you can get it by listed `/proc/{pid}` and see its status from `stat()`. Need to check the error code return from [`readlink(2)`](https://man7.org/linux/man-pages/man2/readlink.2.html).
        * Under Linux 2.0 and earlier
            * `/proc/{pid}/exe` is a pointer to the binary which was executed and appears as a symbolic link.
            * A `readlink(2)` call on this file under Linux 2.0 returns a string in the format: 
                `[device]:inode`
    * `[0-9]+[rwu]`, `del`:
        * `/proc/{pid}/fd/{fd_no}`, `/proc/{pid}/fdinfo/{fd_no}`, 
        * `/proc/{pid}/fd/{fd_no}`
            * You can get what file has been opened by a process in `/proc/{pid}/fd/{fd_no}`. The name of the file under `/proc/{pid}/fd` is the respective file descriptor.
            * Each file under `/proc/{pid}/fd` is a symbolic link to the actual file.
        * `/proc/{pid}/fdinfo/{fd_no}`
            * You can get the file access mode and status flags from the `flags` field in `/proc/{pid}/fdinfo/{fd_no}`.
            * **Warn**: `flags` field is an octal number.
        * **Hint**: In some cases, though the file is being opened by a process, it still can be deleted.
            * Yes, it's possible. In this case, the file still can be readable by the process. And the file would be actually deleted after the process terminated. However, you are not able to see it listed in the disk file system.
            * In this case, the string `(deleted)` would be appended in the result of [`readlink(2)`](https://man7.org/linux/man-pages/man2/readlink.2.html).
            * You can get the inode of deleted file (not the link itself) by following two steps:
                * Get a local fd from the result of [`open(2)`](https://man7.org/linux/man-pages/man2/open.2.html) with `/proc/{pid}/fd/{fd_no}` file. (Just open it. Treat `/proc/{pid}/fd/{fd_no}` as a normal file.)
                * Feed the local fd to the [`fstat(2)`](https://man7.org/linux/man-pages/man2/fstat.2.html) and you will get the inode of the deleted file.
        * **Hint**: 
            * If the file descriptor is for pipes or sockets, the result of `readlink()` will be 
                `type:[inode]`.
            * You should check the result to determine the `TYPE` is `FIFO` or `SOCK`.
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
                * `[stack]`, `[stack:<tid>]`, `[vdso]`, `[heap]`, etc.
        * **Warn**: Need to check the permission to read `/proc/{pid}/maps`.
        * **Hint**: `pathname` may be appended `(deleted)` to indicate the file has been deleted. Need to change `FD` field from `mem` to `del`.

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
    * `REG_NOTBOL`: Need to use this flag to make `$` and `^` work in reqular expression.
* Uid to username
    * [`getpwuid()`](https://linux.die.net/man/3/getpwuid)
    * Get a pointer to a structure containing the broken-out fields of the record in the password database that matches the user ID `uid`.
* File Access Mode
    * [`fcntl.h(0P)`](https://man7.org/linux/man-pages/man0/fcntl.h.0p.html)
    * A flag pass to [`open(2)`](https://man7.org/linux/man-pages/man2/open.2.html) or a flag get from `/proc/[pid]/fdinfo/{fd_no}`
    * `O_ACCMODE`: Since the flag mentioned above contain both access mode and status flags. You can use this mask to leave access mode only.
    * `O_RDONLY`,`O_RDWR`, `O_WRONLY`: Test with these macros, then you can get the access mode of the flag.
* GCC Diagnostic Pragmas
    * [Diagnostic Pragmas](https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html)
    * Use follow block to ignore some warning when debugging
        ```c
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wuninitialized"
        // C Program
        #pragma GCC diagnostic pop
        ```
* Limits on File System Capacity
    * [limits.h(0p)](https://man7.org/linux/man-pages/man0/limits.h.0p.html)
    * `NAME_MAX`: The limit a file name component, **not including** the terminating null character.
    * `PATH_MAX`: The limit for the length of an entire file name (that is, the argument given to system calls such as open), **including** the terminating null character.
* Limits on Names
    * [sysconf(3)](https://man7.org/linux/man-pages/man3/sysconf.3.html)
    * `LOGIN_NAME_MAX`: Maximum length of a login name, **including** the terminating null byte.