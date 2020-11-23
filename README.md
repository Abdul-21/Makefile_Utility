# Makefile-Utility
It parses a makefile, and can print out all the recipes in the correct order. It can also execute the recipes in the right order. It also print out a easy to read dependencies, and targets of the Makefile.

**How to compile the program.**

~~~
$ make
~~~

You will need use GNU makefile to be able to executed the project

* Usage
`./makeutil [options] [target]` 

* Options
    1. `[-p] filenamee`: This is print flag that will target, dependencies, and recipes in the makefile. 
    
    2. `[-r] filename`: The flag will print all the recipes in order of execution.
    
    3. `filename [target]`: This will fork and exec the given makefile up to the target given


* Targets
 You will use able to use a single target which was defined in Makefile.
