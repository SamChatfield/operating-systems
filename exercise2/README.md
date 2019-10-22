Your task in exercise 2 is to implement a logging server and client.

# Client

The client is a program that takes 2 arguments:

* A hostname of a logging server, for example log.cs.bham.ac.uk
* A port number of the logging server, for example 5555

After it started, it will connect to the server and read messages from
STDIN. Every message is a string without 0 bytes. Messages are
terminated by a newline character \n. When there is nothing to read
anymore, the client will close the
connection and terminate with return code 0.

When there is an internal problem, such as the connection to the server breaks, an input cannot be parsed or a similar problem, the return code should be 1. Also you should print an error message that describes the internal error to STDERR.

# Server

The server is a program that takes 2 arguments:

* The port number to bind to, for example 5555
* A filename of a file to write the logs to, for example: /var/log/messages.log

After the server is started, it will bind to the port and listen for
incoming connections. For every connection, the server will read
messages and write them to the logfile. The format of the logfile is
always a line number in decimal followed by a space followed by the
message itself with a \n newline character at the end. When the server
cannot bind to the port, it will return with return code 1. When a
certain file cannot be opened or writing to a file fails, you should
write an error message to STDERR but continue to operate. When the
server starts it creates a new logfile if it does not exist, and
appends the data from the client to the logfile if it already exists.

The server should be able to handle inputs that are not properly formatted and should never crash. The client should also be able to handle any kind of unexpected responses from the server or invalid lines in the input or command line argument.

# Language and project format

You should implement your project in C and compile it with gcc from
the course VM. You are required to use `-D_POSIX_SOURCE -Wall -Werror
-pedantic -std=c99 -D_GNU_SOURCE -pthread` as command line
arguments. You will write a Makefile that will compile the project to
three binaries, namely "serverSingle", "serverThreaded" and "client"
as the default target. Do not use any files or directory with the
prefix "test", since we will use such files for running tests. Your
solution must be contained in a folder exercise2 in your project on
the School's git server. The folder exercise2 must be at the top level
your project for the marking scripts to work.
We will run the command make in the folder exercise2 in order to obtain
all required binaries.

# Parallelism

This tasks consists of two sub-tasks.

* In the first task, you just need to implement a server that handles a single connection at a time. For a perfect implementation, you will receive at most 10 points. Your result must compile to serverSingle.
* In the second task, you will implement a multi-threaded server that handles in general an arbitrary number connections in parallel, as long as system resources are available. For a perfect implementation, you will receive at most 10 points. Your result must compile to serverThreaded.

Of course, every correct solution of serverThreaded is also a correct solution for serverSingle. So when you are confident, feel free to start with serverThreaded directly and just copy the result to serverSingle.

# Remarks

* The critical section in serverThreaded should be as short as possible.
* The server must not leak memory.
* For marking we will use additional, more advanced, test scripts which check whether your program satisfies the specification. If the provided test scripts fail, all the more advanced test scripts are likely to fail as well.
* Any code which does not compile on the virtual machine provided will
  be awarded 0 marks and not be reviewed.


# Submission

Submission consists of pushing your solution to your project as
specified above; the last
submission before the submission deadline has ended counts as your
submission. It is worth trying out submission well before the deadline
even if the solution is not correct yet. 

In Canvas we had to select "Submission on Paper", since the "Submission in external tool" option is broken. However, this is just a workaround: please do not submit any paper solutions.
