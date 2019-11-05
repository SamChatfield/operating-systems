# Exercise 1

## Tasks

There are three different tasks in this exercise.

### sort_simple (5 points)

Write a program that reads string from the standard input. All strings will contain no 0-byte characters and will be terminated by a `\n` newline. The last line of input might not be terminated by a newline character. After reading all strings, these strings should be sorted according of the order of the `strcmp` function as provided by `libc`. After all strings have been sorted, these string should be printed to `STDOUT` in the new order. Every string should be terminated by a `\n` newline character when writing it to `STDOUT`.

In this task, there will be at most 20 strings in the input, each one of them at most 20 characters long including the terminating `\n` newline.

Your solution must be saved in `sort.c`. You are not allowed to add additional files or change the `Makefile`.

There is a test in `test.sh`, that checks your compiled code against a few test cases. Feel free to add more tests. When the tests are passed with success, then `test.sh` will print "OK".

### sort_advanced (5 points)

This is the same task as in sort_simple, but your implementation must be able to handle an arbitrary number of input lines of arbitrary length. Should your implementation fail to allocate sufficient memory, then your program should exit with exit code 1 and should not generate any output.

### linked_list (5 points)

Your task is to implement a linked list for integers (int). In `linkedlist.h`, there is already a header file that defines the interface of that list. You will define the type of your list there with `typedef struct list` and you may define additional data type in this file. You must implement your list in `linkedlist.c`. In `test_list.c` is a simple test for your code that tests your code.

Again, there is a script in `test.sh`, that runs `test_list` and compares the output with a reference output.

## Submission

You will do the project in the gitlab server of the school. The first step is to set up the project in the virtual machine. Start the virtual machine and clone your OS-project with `git clone https://git-teaching.cs.bham.ac.uk/mod-os/<SoCS-username>.git <directory>`. This will create a copy of the OS-project in your virtual machine. Initially this project is empty. Download the exercise from [here](https://canvas.bham.ac.uk/courses/38480/files/8063885/download), copy the downloaded file `exercise1.tgz` to the top-level directory of the OS-project and extract the files with the command `tar zxfv exercise1.tgz`. Submission consists of pushing your solution to that project; the last submission before the submission deadline has ended counts as your submission. It is worth trying out submission well before the deadline even if the solution is not correct yet. It is important for the marking scripts to work that the directory exercise1 is in the top-level directory of the OS-project.

In Canvas we had to select "Submission on Paper", since the "Submission in external tool" option is broken. However, this is just a workaround: please do not submit any paper solutions.

## Scoring

In total, there are 15 points for this assignment. For every task that is perfectly done, you get 5 points. You get less points when your solution leaks memory, handles errors incorrectly or crashes for some inputs.

## Questions

Ask questions about this assignment in the Discussions page of canvas, but do not post parts of the solution there.

## Remarks

* Only return an error when malloc or a similar function fails. Do not set a hard limit for the memory in your code.
* Do not put object files or binaries into the repository, just the C code. Only create new files to add more tests.
* When printing the list, add a trailing space to the last element of the list and a newline character. As an example, when the integers 1, 2 and 3 are in the list, print "1 2 3 " followed by a newline character. When the list is empty, print "empty list" followed by a newline character.
* Any code which does not compile on the virtual machine using the Makefile provided will be awarded 0 marks and not be reviewed.
* You must not change the compiler options in the Makefile. These options imply that any warnings will be treated as errors, resulting in code that doesn't compile and thus will not be marked.
* For marking we will use additional, more advanced, test scripts which check whether your program satisfies the specification. If the provided test script fails, all the more advanced test scripts are likely to fail as well.
* There must be no memory leaks in your linked list implementation.