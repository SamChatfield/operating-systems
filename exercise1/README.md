# Tasks

There are three different tasks in this exercise.

## sort_simple (5 points)

Write a program that reads string from the standard input. All strings will contain no 0-byte characters and will be terminated by a `\n` newline. The last line of input might not be terminated by a newline character. After reading all strings, these strings should be sorted according of the order of the `strcmp` function as provided by `libc`. After all strings have been sorted, these string should be printed to `STDOUT` in the new order. Every string should be terminated by a `\n` newline character when writing it to `STDOUT`.

In this task, there will be at most 20 strings in the input, each one of them at most 20 characters long including the terminating `\n` newline.

Your solution must be saved in `sort.c`. You are not allowed to add additional files or change the `Makefile`.

There is a test in `test.sh`, that checks your compiled code against a few test cases. Feel free to add more tests. When the tests are passed with success, then `test.sh` will print "OK".

## sort_advanced (5 points)

This is the same task as in `sort_simple`, but your implementation must be able to handle an arbitrary number of input lines of arbitrary length. Should your implementation fail to allocate sufficient memory, then your program should exit with exit code 1 and should not generate any output.

## linked_list (5 points)

Your task is to implement a linked list for integers (int). In `linkedlist.h`, there is already a header file that defines the interface of that list. You will define the type of your list there with `typedef struct list` and you may define additional data type in this file. You must implement your list in `linkedlist.c`. In `test_list.c` is a simple test for your code that tests your code.

Again, there is a script in `test.sh`, that runs test_list and compares the output with a reference output.

# Remarks

* Only return an error when `malloc` or a similar function fails. Do not set a hard limit for the memory in your code.
* Do not put object files or binaries into the repository, just the C code. Only create new files to add more tests.
* When printing the list, add a trailing space to the last element of the list and a newline character. As an example, when the integers 1, 2 and 3 are in the list, print `"1 2 3 "` followed by a newline character. When the list is empty, print `"empty list"` followed by a newline character.
* Any code which does not compile on the virtual machine using the `Makefile` provided will be awarded 0 marks and not be reviewed.
* You must not change the compiler options in the `Makefile`. These options imply that any warnings will be treated as errors, resulting in code that doesn't compile and thus will not be marked.
* For marking we will use additional, more advanced, test scripts which check whether your program satisfies the specification. If the provided test script fails, all the more advanced test scripts are likely to fail as well.
* There must be no memory leaks in your linked list implementation.
