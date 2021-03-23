# HW1 - Implement a `lsof`-like program

## How to build
```
make
```

## How to run
```
./hw1
```

## Some Reference
### Work Relative
* `/proc`.
    * In short, in this homework, we only need the number-named directory under `/proc`.
    * Each numbered directory `/proc` match alive(may run, sleep, etc.) process and this number is the `pid` of the process.
    * The purpose of other files/dirs under `/proc` you can see [here](https://tldp.org/LDP/Linux-Filesystem-Hierarchy/html/proc.html).
    * Most below info about files/dirs under `/proc` is capture from [proc(5)](https://man7.org/linux/man-pages/man5/procfs.5.html).
* `COMMAND`:
    * `/proc/{pid}/comm`.
    * You can get the `COMMAND` by reading `/proc/{pid}/comm` file.
    * Another approaches: 
        * `Name` filed in `/proc/{pid}/status` file.
        * Second filed in `/proc/{pid}/stat` file. Show in parenthesses. 
* `USER`:
    * I got this info in two steps:
        * First: Get (real) uid that ran the process from `/proc/{pid}/status`.
        * Second: Use `getpwuid()` to get respective user name.
    * `Uid`:
        * There are four number in Uid field in `/proc/{pid}/status`.
        * They are: Real, effective, saved set, and filesystem UIDs.
    * [`getpwuid()`](https://linux.die.net/man/3/getpwuid):
        * Get password file entry.

* `FD`:
    * `cwd`: 
        * `/proc/{pid}/cwd`
        * The current working directory (cwd) of the process.
        * `/proc/{pid}/cwd` is a symbolic link to the cwd of the process.
        * Need to get the file info that the link point to.
        * **Warn**: Need to check the permission to dereference symbilic link.

### Programing Relative
* Parsing Arguments
    * [`getopt()`](https://man7.org/linux/man-pages/man3/getopt.3.html)
    * [Example usage](https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html) of `getopt()`.
* Regular Expression
    * [`regcomp(), regexec(), regerror(), regfree()`](https://man7.org/linux/man-pages/man3/regex.3.html)
    * `regcomp()`: Compile the regular expressiont, e.g. Test if the regex is valid.
    * `regexec()`: Used to test if the input string is matched with the regex.
    * `regerror()`: Get human read string from the error code output from `regcomp()` or `regexec()`.
    * `regfree()`: Release regex obj got from `regcomp()`.
* Limits on File System Capacity
    * [Limits For File](https://www.gnu.org/software/libc/manual/html_node/Limits-for-Files.html)
    * `MAX_INPUT`
    * `NAME_MAX`
    * `PATH_MAX`
* Limits on Names
    * [sysconf(3)](https://man7.org/linux/man-pages/man3/sysconf.3.html)
    * `LOGIN_NAME_MAX`