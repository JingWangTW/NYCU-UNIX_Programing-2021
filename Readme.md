# HW1 - Implement a `lsof`-like program

## How to build
```
make
```

## How to run
```
./hw1
```

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