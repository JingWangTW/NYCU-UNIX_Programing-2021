# HW2 - Monitor File Activities of Dynamically Linked Programs

## Description
In this homework, we are going to practice library injection and API hijacking. Please implement a simple logger program that is able to show *file-access-related* activities of an arbitrary binary running on a Linux operating system. You have to implement your logger in two parts. One is a *logger* program that prepares the runtime environment to inject, load, and execute a monitored binary program. The other is a *shared object* that can be injected into a program by the *logger* using **`LD_PRELOAD`**. You have to dump the library calls as well as the passed parameters and the returned values. Please check the "Requirements" section for more details.

The output of your homework is required to strictly follow the spec. The TAs will use **`diff`** tool to compare the output of your program against the correct answer. Spaces and tabs in the output are compressed into a single space character when comparing the outputs.

## Requirements
### Program Arguments
Your program should work with the following arguments:
```
usage: ./logger [-o file] [-p sopath] [--] cmd [cmd args ...]
    -p: set the path to logger.so, default = ./logger.so
    -o: print output to file, print to "stderr" if no file specified
    --: separate the arguments for logger and for the command
```
The message should be displayed if an invalid argument is passed to the logger.

### Monitored file access activities
The list of monitored library calls is shown below. It covers several functions we have introduced in the class.
<b>
```
chmod       chown       close       creat       fclose      fopen       fread       fwrite 
open        read        remove      rename      tmpfile     write
```
</b>

### Output
You have to dump the library calls as well as the corresponding parameters and the return value. We have several special rules for printing out function arguments and return values:
* If a passed argument is a filename string, print the real absolute path of the file by using [`realpath(3)`](https://linux.die.net/man/3/realpath). If [`realpath(3)`](https://linux.die.net/man/3/realpath) cannot resolve the filename string, simply print out the string untouched.
* If a passed argument is a descriptor or a FILE * pointer, print the absolute path of the corresponding file. The filename for a corresponding descriptor can be found in [`/proc/{pid}/fd`](https://linux.die.net/man/5/proc) directory.
* If a passed argument is a mode or a flag, print out the value in octal.
* If a passed argument is an integer, simply print out the value in decimal.
* If a passed argument is a regular character buffer, print it out up to 32 bytes. Check each output character using [`isprint(3)`](https://linux.die.net/man/3/isprint) function and output a dot '.' if a character is not printable.
* If a return value is an integer, simply print out the value in decimal.
* If a return value is a pointer, print out it using `%p` format conversion specifier.
* Output strings should be quoted with double-quotes.
A sample output of the homework is given below. More examples can be found in the "Sample Output" section.

```
$ ./logger ./sample  
[logger] creat("/home/ta/hw2/tmp/aaaa", 600) = 3
[logger] chmod("/home/ta/hw2/tmp/aaaa", 666) = 0
[logger] chown("/home/ta/hw2/tmp/aaaa", 65534, 65534) = -1
[logger] rename("/home/ta/hw2/tmp/aaaa", "/home/ta/hw2/tmp/bbbb") = 0
[logger] open("/home/ta/hw2/tmp/bbbb", 1101, 666) = 4
[logger] write("/home/ta/hw2/tmp/bbbb", "cccc", 5) = 5
[logger] close("/home/ta/hw2/tmp/bbbb") = 0
[logger] open("/home/ta/hw2/tmp/bbbb", 0, 0) = 4
[logger] read("/home/ta/hw2/tmp/bbbb", "cccc", 100) = 5
[logger] close("/home/ta/hw2/tmp/bbbb") = 0
[logger] tmpfile() = "/tmp/#14027789 (deleted)"
[logger] fwrite("cccc", 1, 5, "/tmp/#14027789 (deleted)") = 5
[logger] fclose("/tmp/#14027789 (deleted)") = 0
[logger] fopen("/home/ta/hw2/tmp/bbbb", "r") = 0x558d89401260
[logger] fread("cccc", 1, 100, "/home/ta/hw2/tmp/bbbb") = 5
[logger] fclose("/home/ta/hw2/tmp/bbbb") = 0
[logger] remove("/home/ta/hw2/tmp/bbbb") = 0
sample done.
[logger] write("/dev/pts/7", "sample done.  
", 14) = 14
```

## Sample Output
Some basic usage.
```
$ ./logger
no command given.

$ ./logger -h                
./logger: invalid option -- 'h'
usage: ./logger [-o file] [-p sopath] [--] cmd [cmd args ...]
        -p: set the path to logger.so, default = ./logger.so
        -o: print output to file, print to "stderr" if no file specified
        --: separate the arguments for logger and for the command

$ ./logger ls                
[logger] fopen("/proc/filesystems", "re") = 0x55aa12cd6ad0
[logger] fclose("/proc/filesystems") = 0
hw2.c  logger  logger.so  Makefile  sample  sample.c  sample.txt
[logger] fclose("/dev/pts/7") = 0

$ ./logger ls 2>/dev/null 
hw2.c  logger  logger.so  Makefile  sample  sample.c  sample.txt

$ ./logger -o ls_al.txt -- ls -al
... [output of `ls -al`] ...

$ cat ls_al.txt       
[logger] fopen("/proc/filesystems", "re") = 0x558c872fcad0
[logger] fclose("/proc/filesystems") = 0
[logger] fopen("/etc/passwd", "rme") = 0x558c872fcd00
[logger] fclose("/etc/passwd") = 0
[logger] fopen("/etc/group", "rme") = 0x558c872fcf30
[logger] fclose("/etc/group") = 0
[logger] fclose("/dev/pts/7") = 0
[logger] fclose("/dev/pts/7") = 0
```

Sample output for `exec()` or `system()`.
```
$ ./logger -o bash.txt -- bash
ta@lab:~/hw2$ ls -al
... [output of `ls -al`] ...
ta@lab:~/hw2$ exit
exit

$ cat bash.txt
[logger] open("/dev/tty", 4002, 0) = 3
[logger] close("/dev/tty") = 0
[logger] fopen("/etc/passwd", "rme") = 0x55b89284a260
[logger] fclose("/etc/passwd") = 0
[logger] close("/dev/pts/7") = 0
...
[logger] read("/dev/pts/7", "l", 1) = 1
[logger] fwrite("l", 1, 1, "/dev/pts/7") = 1
[logger] read("/dev/pts/7", "s", 1) = 1
[logger] fwrite("s", 1, 1, "/dev/pts/7") = 1
...
[logger] fopen("/proc/filesystems", "re") = 0x55b89284af30
[logger] fclose("/proc/filesystems") = 0
[logger] fclose("/dev/pts/7") = 0
...
```

## Hints
When implementing your homework, you may inspect symbols used by an executable. We have mentioned that you are not able to see any symbol if the symbols were stripped using [`strip(1)`](https://linux.die.net/man/1/strip) command. However, you may consider working with [`readelf(1)`](https://linux.die.net/man/1/readelf) command. For example, we can check the symbols that are unknown to the binary:
```
$ nm /usr/bin/wget
nm: /usr/bin/wget: no symbols
$ readelf --syms /usr/bin/wget | grep open
    72: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND freopen64@GLIBC_2.2.5 (2)
    73: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND iconv_open@GLIBC_2.2.5 (2)
   103: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND gzdopen
   107: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND fdopen@GLIBC_2.2.5 (2)
   119: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND open64@GLIBC_2.2.5 (2)
   201: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND fopen64@GLIBC_2.2.5 (2)
```
Alternatively, you may consider using [`nm -D`](https://linux.die.net/man/1/nm) to read symbols. Basically, we have two different symbol tables. One is the regular symbol table, and the other is the dynamic symbol table. The one removed by [`strip(1)`](https://linux.die.net/man/1/strip) is the regular symbol table. So you will need to work with [`nm -D`](https://linux.die.net/man/1/nm) or [`readelf --syms`](https://linux.die.net/man/1/readelf) to read the dynamic symbol table.

## Homework Submission
We will compile your homework by simply typing 'make' in your homework directory. You have to ensure your Makefile produces the executable ***logger*** and the shared object ***logger.so***. Please make sure your Makefile works and the output executable name is correct before submitting your homework. The ***logger*** may be implemented as a shell script. In that case, you can simply pack the shell script in your submitted file.

Please pack your C/C++ code and Makefile into a **zip** archive. The directory structure should follow the below illustration. The id is your student id. Please note that you don't need to enclose your id with the braces.
```
{id}_hw2.zip
└── {id}_hw2/
	├── Makefile
	├── hw2.c (or hw2.cpp)
	└── (any other c/c++ files if needed)
```
You have to submit your homework via the E3 system. Scores will be graded based on the completeness of your implementation.


## Remarks
* Please implement your homework in C or C++.
* Using any non-standard libraries and any external binaries (e.g., via `system()`) are not allowed.
* No copycats. Please do not use codes from others (even open source projects).
* We will test your program in **Ubuntu 20.04 LTS** Linux with the default gcc version (9.3.0).
