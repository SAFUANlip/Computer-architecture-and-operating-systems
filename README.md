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
