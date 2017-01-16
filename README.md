simple-c-shell

Simple and thoroughly commented shell written in C, just for educative purposes.

Created by Aneel Balda  as an University exercise.

Features

Basic commands: exit, pwd, clear and cd
Environment management with setenv and unsetenv
Program invocation with forking and child processes
Piping implemented (<cmd1> | <cmd2>) via pipe and dup2 syscalls. Multiple piping is allowed.
SIGINT signal when Ctrl-C is pressed (shell is not exited)
