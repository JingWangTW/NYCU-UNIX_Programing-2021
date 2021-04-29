# HW2 - Monitor File Activities of Dynamically Linked Programs

## How to build
```
make
```

## How to run
```bash
./logger [-o file] [-p sopath] [--] cmd [cmd args ...]
    -o: print output to file, print to "stderr" if no file specified
    -p: set the path to logger.so, default = ./logger.so
    --: separate the arguments for logger and for the command
```

## Note
### `close`, `fclose`, `remove`, `rename`
* The real file in the disk represent by yhe filepath, `FILE` structure, fd passed to these functions are going to be deleted or disappeared.
* Need to resolve the real file path before launch the real linux function. 
* Otherwise, you can not get the right path of the file.

### Be careful
* For instance, if you are going to call `fopen` to open a file to write output log.
* Be careful to launch the right one, not the faked one, which is written by you.
* Otherwise, the faked function will be called recursivley.

## Implementation Issue
### `open(2)`
* There are two prototypes of [`open(2)`](https://linux.die.net/man/2/open) in manual pages.
* There are no clear instruction in the spec whether we should show different result in the logger output of the function call.
* And I can find it always show 3 parameters in the sample out section in the spec.
* As a result, I just print 3 parameters in the logger output.

### Path Output
* As spec mentioned, string output should be truncated to 32 bytes at most.
* But it doesn't cover the situation which the path is longer than 32 bytes.
* I decide to print it all. The full pathname will not be truncated.

### Output File
* Spec didn't mentioned the behaviour when the logger received an invalid output file.
* The given output file may be not able to open due to some reason like permission denied.
* I will check the writable permission have been granted before launch the application.

## Reference
### [`open(2)`](https://linux.die.net/man/2/open)
* [`open(2)`](https://linux.die.net/man/2/open)
* There are two function prototypes in man page.
* However, there should only be one function declaration for the same name in C.
* You can find how the real [`open(2)`](https://linux.die.net/man/2/open) declararion in `/usr/include/fcntl.h`.
    ```c
    int open (const char *__file, int __oflag, ...);
    ```
* Use `va_*` family macros to deal with the unnamed arguments.
### Shared Library Compilation
* Compile single file in project
    ```bash
    # -fPIC: Position Independent Code
    #    Generated machine code is not being located at a specific addr.
    #    Jumps would be generated as relative rather than absolute.
    gcc -fPIC -c src.c -o src.o
    ```
* Link all object codes and generate required shared library
    * [`ld(1)`](https://linux.die.net/man/1/ld)
    ```bash
    # -shared: compile to a shared library
    # -Wl,--no-as-needed
    #    -Wl: means the following command is provided for linker
    #    --no-as-needed: record the symbol  whether the library is needed
    gcc -shared -Wl,--no-as-needed -ldl src.o -o libsrc.so
    ```

### `ldd(1)`
* [`ldd(1)`](https://linux.die.net/man/1/ldd)
* Print shared library dependencies.
* Perform relocations for both data objects and functions, and report any missing objects or functions.
    ```bash
    ldd -r ./libsrc.so
    ```

### `dlsym(3)`
* [`dlsym(3)`](https://linux.die.net/man/3/dlsym)
* `void *dlsym(void *handle, const char *symbol);`
* Link with `-ldl`.
* `void * handlle`
    * Get from [`dlopen(3)`](https://linux.die.net/man/3/dlopen)
    * Or use two special pseudo-handles
        * `RELD_DEFAULT`: Find the first occurrence of the desired symbol using the default shared object search order.
        * `RELD_NEXT`: Find the next occurrence of the desired symbol in the search order after the current object.
        * If going to using this two pseudo-hanlders, need to define `_GNU_SOURCE` before include `dlfcn.h`.
            ```c
            #define _GNU_SOURCE
            #include <dlfcn.h>
            ```
* `const char * symbol`
    * The symbol we are going to find.
    * If want to find a symbol that was contained in a shared object which was written in `c++`, eg. the symbol may become mangled name, you can solve it in two situation.
        * If you can re-compile the c++ code:
            * Wrap c++ code with
                ```c
                #ifdef __cplusplus
                    extern "C"
                    {
                #endif

                    // your c++ code here

                #ifdef __cplusplus
                    }
                #endif
                ```
        * If you can't re-comile the c++ code:
            * [`readelf(1)`](https://linux.die.net/man/1/readelf)
            * Read the symbol in the shared object and get the correct mangled name.
                ```
                readelf --sym ./libsrc.so
                ```

### `stdarg.h(3)`
* [`stdarg(3)`](https://linux.die.net/man/3/stdarg)
* Define four macro functions and one type.
```c
void easy_printf (char *fmt, int last_args, ...)   /* '...' is C syntax for a variadic function */
{
    /*
        Declare the variable to store all the rest variables
    */
    va_list  ap;    
    
    /* 
        Initialize the var ap, 
        
        First argument for the va_start: variable to save the rest variable
        Second argument for the va_start: the last named variable of this foo() function.
    */
    va_start(ap, last_args);    

    while (*fmt) {
        switch (*fmt++) {
        case 's':            
            /* 
                Get the rest arguments
                
                First argument for the va_arg: variable for saving the rest variable
                Second argument for the va_arg: the type of this argumnet
            */  
            s = va_arg(ap, char *);     
            printf("string %s\n", s);
            break;
        case 'd':             
            d = va_arg(ap, int);
            printf("int %d\n", d);
            break;
        }
    }

    /* 
        Terminate to resolve the rest arguments
    */
    va_end(ap);
}
```

### `static`
* A file-static variables, functions is simlar to a private static member of a class.
* It can only be accessed by functions in that file, similar to how a private static member variable can only be accessed by functions in the class in which it is defined.
* There is only one copy of the variable.
* Its lifetime is the program lifetime.

### Shell Script
* [`bash(1)`](https://linux.die.net/man/1/bash)    
* Variable
    * [Bash Scripting Tutorial - Variables](https://ryanstutorials.net/bash-scripting-tutorial/bash-variables.php)
    ```bash
    # Get the value of a variable
    echo $variable  # remember the $ sign

    # Set value to a variable
    variable=value  # No space on both sides of =
                    # Leave out the $ sign

    # Command Arguments
    echo $1  # First command line arguments
    echo $2  # Second command line arguments

    # Save output of a command into a variable
    variable=$(command)

    # Quotes
    # ' : Single quotes will treat every character literally
    # " : Double quotes will allow to do substitution 
    variable=1
    newVar1='$variable'
    newVar2="$variable"
    echo $newVar1   # $variable
    echo $newVar2   # 1
    ```
* Array
    * [Array Basics in Shell Scripting](https://www.geeksforgeeks.org/array-basics-shell-scripting-set-1/)
    ```bash
    # Declaration
    ARRAYNAME[INDEXNR]=value
    declare -a ARRAYNAME
    ARRAYNAME=(value1 value2  .... valueN)

    # Print all
    echo ${arr[@]}
    echo ${arr[*]}

    # Print length
    echo ${#arr[@]}
    echo ${#arr[*@*]}
    ```
* `if/elif/else`:
    * [If Statements - Bash Scripting Tutorial](https://ryanstutorials.net/bash-scripting-tutorial/bash-if-statements.php)
    * [`test(1)`](https://linux.die.net/man/1/test)
    ```bash
    # [ ] are actually the "test" command
    if [ $1 -ge 18 ]
    then
        echo 'You may go to the party.'
    
    # = and -eq have different behaviour
    # = is compare with string, -eq is compare with integer
    elif [ $2 = 'yes' ]
    then
        echo 'You may go to the party but be back before midnight.'
    
    else

        # Nested if statement
        # If need to check an expression, use the double brackets
        if (( $1 % 2 == 0 ))
        then
            echo 'This is a nested if statement.'
        
        # or operator
        elif [ $USER == 'bob' ] || [ $USER == 'andy' ]
        then
            echo 'Hello bob or andy.'
        
        # and operator
        elif [ $USER == 'nick' ] && [ $2 == 'no' ]
        then
            echo 'Hello nick.'
        
        else
            echo 'You may not go to the party.'
        fi
    fi
    ```
* `case`
    * [If Statements - Bash Scripting Tutorial](https://ryanstutorials.net/bash-scripting-tutorial/bash-if-statements.php)
    ```bash
    # start of case
    case $1 in
        # different case
        start)
            # what do you want to do for this case just write it here
            echo starting
            # two ; to declare the end of this case
            ;;
        stop)
            echo stoping
            ;;
        restart)
            echo restarting
            ;;
        # default case
        *)  
            echo dont know
            ;;
    # end of case
    esac
    ```
* `for` Loop
    * [Loops - Bash Scripting Tutorial](https://ryanstutorials.net/bash-scripting-tutorial/bash-loops.php#for)
    ```bash
    names='Stan Kyle Cartman'
    
    # for var_name in <list>
    for name in $names;
    do
        echo $name
    done
    ```
* Function
    * [Functions - Bash Scripting Tutorial](https://ryanstutorials.net/bash-scripting-tutorial/bash-functions.php)
    ```bash
    # The first syntax to declare a function
    function print_something1 {
        echo "Hello I am the function1"
        echo "Here is my args: $@"  # aaa bbb ccc

        return "Return from function 1"
    }
    
    # The second syntax to declare a function
    # The () will NOT contain any argument inside like we do in other language.
    # We need () here is just because it's a syntax to declare a function
    # We always get argumnets from $@ and $1, $2, ...
    print_something2 () {
        echo "Hello I am the function1"
        echo "Here is my args: $@"  # aaa bbb ccc

        return "Return from function 2"
    }
    
    # Call the function like we are launching a coommand
    print_something1 aaa bbb ccc

    # Get the return value of the function
    echo $?         # Return from function 1

    # Save the return value of the function like we are launching a command
    variable=$(print_something2 aaa bbb ccc)

    echo $variable  # Return from function 2
    ```