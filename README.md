# Computer-architecture-and-operating-systems tasks


## T1 

Write a useless program (Unix System Extremely Late Execution Software System) that reads the file and runs the programs specified in it
with the specified delay from the start time of the program is useless.
The format of the record in the file: "delay time in seconds" "program to execute".

For example:

5 Is Equal to

1 ./test

3 ps

---

## T2

In order to prevent the loss of information in the event of a disk failure, usually use file backup (backup). The simplest form of backup'a is copying files from one catalog to another. This method requires a lot of time and disk space. Write a program that implements a more intelligent approach. The program should take two parameters from the command line: the names of the source directory and the destination directory. It should recursively scan the source directory, make copies of all files for which it has not previously copies were made or which have been modified since the last backup by placing them in the appropriate places of the destination directory. After copying each file, the compression command should be called gzip. This will reduce the required disk space, and the file will be renamed with the addition of an extension.gz. All errors that occur (there is no source directory, the file is not readable, etc.) must be correctly processed with the output of the corresponding message.

---

## T3

Write a runsim program that monitors the number
of simultaneously running Unix applications running with its
help. The program reads the name of a Unix command from standard input and
runs it for execution. The number of simultaneously running
commands should not exceed N, where N is a command-line parameter when
running runsim. If you try to run more than N applications, issue
an error message and continue waiting for commands
to be executed. The runsim program should stop working when there is an end-
of-file sign on the standard input.

---

## T4

Two people work at the dish washing in the restaurant. One of them washes the dishes, the second wipes the already washed. The times of washing and wiping dishes vary depending on what is being washed. The table for washed, but not wiped dishes has limited dimensions. Model the staff work process as follows: each employee has their own process. The operation execution times are contained in two files. Each file has the format of entries: "type of dishes" : "operation time"

The table holds N items, regardless of their name. The value of N it is set as an environment variable TABLE LIMIT before the start of the process. Dirty dishes coming to the sink are described by a file with the record format:

"type of dishes" : "number of items"

Records with the same type of dishes can occur repeatedly.

Organize the transfer of dishes from process to process:

a) through a file, using semaphores for synchronization;

b) via pipe;

b) through messages;

d) via shared memory;

e) via sockets.
