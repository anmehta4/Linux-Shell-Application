
# Unix Shell

## Overview

In this assignment, I implemented a *command line interpreter (CLI)* or,
as it is more commonly known, a *shell*. The shell should operate in this
basic way: when you type in a command (in response to its prompt), the shell
creates a child process that executes the command you entered and then prompts
for more user input when it has finished.

## Program Specifications

### Basic Shell: `wish`

Your basic shell, called `wish` (short for Wisconsin Shell, naturally), is
basically an interactive loop: it repeatedly prints a prompt `wish> ` (note
the space after the greater-than sign), parses the input, executes the command
specified on that line of input, and waits for the command to finish. This is
repeated until the user types `exit`.  The name of your final executable
should be `wish`.

The shell can be invoked with either no arguments or a single argument;
anything else is an error. Here is the no-argument way:

```
prompt> ./wish
wish> 
```

At this point, `wish` is running, and ready to accept commands. Type away!

The mode above is called *interactive* mode, and allows the user to type
commands directly. The shell also supports a *batch mode*, which instead reads
input from a batch file and executes commands from therein. Here is how you
run the shell with a batch file named `batch.txt`:

```
prompt> ./wish batch.txt
```

One difference between batch and interactive modes: in interactive mode, a
prompt is printed (`wish> `). In batch mode, no prompt should be printed.

You should structure your shell such that it creates a process for each new
command (the exception are *built-in commands*, discussed below).  Your basic
shell should be able to parse a command and run the program corresponding to
the command.  For example, if the user types `ls -la /tmp`, your shell should
run the program `/bin/ls` with the given arguments `-la` and `/tmp` (how does
the shell know to run `/bin/ls`? It's something called the shell **path**;
more on this below).

## Structure

### Basic Shell

The shell is very simple (conceptually): it runs in a while loop, repeatedly
asking for input to tell it what command to execute. It then executes that
command. The loop continues indefinitely, until the user types the built-in
command `exit`, at which point it exits. That's it!

**Important:** Note that the shell itself does not *implement* `ls` or other
commands (except built-ins). All it does is find those executables in one of
the directories specified by `path` and create a new process to run them.

### Built-in Commands

Whenever your shell accepts a command, it should check whether the command is
a **built-in command** or not. If it is, it should not be executed like other
programs. Instead, your shell will invoke your implementation of the built-in
command. For example, to implement the `exit` built-in command, you simply
call `exit(0);` in your wish source code, which then will exit the shell.

In this project, you should implement `exit`, `cd`, and `path` as built-in
commands.

* `exit`: When the user types `exit`, your shell should simply call the `exit`
  system call with 0 as a parameter. It is an error to pass any arguments to
  `exit`. 

* `cd`: `cd` always take one argument (0 or >1 args should be signaled as an
error). To change directories, use the `chdir()` system call with the argument
supplied by the user; if `chdir` fails, that is also an error.

* `path`: The `path` command takes 0 or more arguments, with each argument
  separated by whitespace from the others. A typical usage would be like this:
  `wish> path /bin /usr/bin`, which would add `/bin` and `/usr/bin` to the
  search path of the shell. If the user sets path to be empty, then the shell
  should not be able to run any programs (except built-in commands). The
  `path` command always overwrites the old path with the newly specified
  path. 

### Redirection

Many times, a shell user prefers to send the output of a program to a file
rather than to the screen. Usually, a shell provides this nice feature with
the `>` character. Formally this is named as redirection of standard
output. To make your shell users happy, your shell should also include this
feature, but with a slight twist (explained below).

For example, if a user types `ls -la /tmp > output`, nothing should be printed
on the screen. Instead, the standard output of the `ls` program should be
rerouted to the file `output`. In addition, the standard error output of
the program should be rerouted to the file `output` (the twist is that this
is a little different than standard redirection).

If the `output` file exists before you run your program, you should simple
overwrite it (after truncating it).  

The exact format of redirection is a command (and possibly some arguments)
followed by the redirection symbol followed by a filename. Multiple
redirection operators or multiple files to the right of the redirection sign
are errors.

### Loop command

There is a special built-in command you need to add, which is called loop. When the user types:
prompt> loop 10 somecommand
the wish shell should run somecommand ten times.
For example, if the user types echo hello as the command (to run the program /bin/echo , which just prints out whatever the arguments to it are), the shell should do this:

prompt> loop 5 echo hello
hello
hello
hello
hello
hello
prompt>

To make this slightly more useful, there is one more feature you'll need to add, a loop variable. This variable is referred to by $loop . Thus, the user should then be able to do type:

prompt> loop 5 echo hello $loop
hello 1
hello 2
hello 3
hello 4
hello 5
prompt>

