# HW2 - Monitor File Activities of Dynamically Linked Programs

## How to build
```
make
```

## How to run
```
./hw2
```

## Reference
### Programing Relative
* Shell Script
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

