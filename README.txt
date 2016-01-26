Authors: Yang Liu(yliu17), Will Frick(wofrick)
Date: Jan 25, 2016

The source code for each of the phases is separated into folders. To compile them, navigate into this folder in the terminal and run "make". The resulting executables are named "runCommand", "shell" and "shell2", respectively.

All jobs executed are stored in a global struct called ts. ts contains two counters and a list. One counter tracks how many jobs have ever been executed; the other counts how many are still active. By comparing this value to 0, we can determine whether there are any tasks waiting to be completed.

ts' list, called tasks, is a list of task structs. Each task struct tracks its own tid, pid and name, and has a boolean value called "active" to track whether it is still executing.

To test the program, we primarily used the sleep command, as it made for an easy way to ensure a task would remain running for a while. By starting various numbers of instances of sleep with varied, but relatively long, wait times, we were able to easily test the ability of our program to correctly identify running tasks, and print information when tasks completed. For tests that did not require tasks to remain running (including testing for runCommand and shell), we usually used ls as a simple command.


Our program supports using double quotes to mark argument names with spaces. For example, consider the following command:

cmd "a b" c

This command would be broken into the following:

cmd
a b 
c

Rather than this:

cmd
"a
b"
c

We used the cd command to test this functionality on various folder structures.
