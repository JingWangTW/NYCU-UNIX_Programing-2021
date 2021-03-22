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
