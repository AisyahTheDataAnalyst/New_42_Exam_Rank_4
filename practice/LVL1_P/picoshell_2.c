/* 
Assignment name:       picoshell
Expected files:        picoshell.c
Allowed functions:     close, fork, wait, exit, execvp, dup2, pipe


Write the following function:

int    picoshell(char **cmds[]);

The goal of this function is to execute a pipeline. It must execute each
commands [sic] of cmds and connect the output of one to the input of the
next command (just like a shell).

Cmds contains a null-terminated list of valid commands. Each rows [sic]
of cmds are an argv array directly usable for a call to execvp. The first
arguments [sic] of each command is the command name or path and can be passed
directly as the first argument of execvp.

If any error occur [sic], The function must return 1 (you must of course
close all the open fds before). otherwise the function must wait all child
processes and return 0. You will find in this directory a file main.c which
contain [sic] something to help you test your function.


Examples: 
./picoshell /bin/ls "|" /usr/bin/grep picoshell
picoshell
./picoshell echo 'squalala' "|" cat "|" sed 's/a/b/g'
squblblb/
*/

// Breakdown of sed 's/a/b/g':
// s → substitute
// a → the pattern to match (literal a here)
// b → the replacement text
// g → global flag, meaning replace all matches on a line, not just the first one

#include <unistd.h> //close, pipe, dup2, execve,
#include <sys/wait.h> //wait, fork, pid_t
#include <stdlib.h> //exit

int    picoshell(char **cmds[])
{
    int fd[2];
    int fd_in = 0;
    int i = 0;
    int ret = 0;
    int status;
    pid_t pid;

    while (cmds[i])
    {
        if (cmds[i + 1])
        {
            if (pipe(fd) == -1)
                return (1);
        }
        else
        {
            fd[0] = -1;
            fd[1] = -1;
        }
        pid = fork();
        if (pid < 0)
        {
            if (fd[0] != -1)
                close(fd[0]);
            if (fd[1] != -1)
                close (fd[1]);
            if (fd_in != 0)
                close (fd_in);
            return (1);
        }
        if (pid == 0)
        {
            if (fd[1] != -1)
            {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
                close(fd[0]);
            }
            if (fd_in != 0)
            {
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            execvp(cmds[i][0], cmds[i]);
            exit(EXIT_FAILURE);
        }
        if (pid > 0)
        {
            if (fd[1] != -1)
                close (fd[1]);
            if (fd_in != 0)
                close (fd_in);
            fd_in = fd[0];
        }
        i++;
    }
    while (wait(&status) > 0)
    {
        if (WIFEXITED(status))
        {
            if (WEXITSTATUS(status) != 0)
                ret = 1;
        }
        else
            ret = 1;
    }
    return (ret);
}

#include <stdio.h> //printf
#include <string.h> //strcmp

int main(int ac, char **av)
{
    if (ac < 2)
    {
        printf("Insufficient commands\n");
        return (1);
    }

    char **cmds[ac];
    int cmd_count = 0;
    int i = 1;

    while (i < ac)
    {
        cmds[cmd_count++] = &av[i];
        while (i < ac && strcmp(av[i], "|") != 0)
            i++;
        if (i < ac && strcmp(av[i], "|") == 0)
        {
            av[i] = NULL;
            i++;
        }
    }
    cmds[cmd_count] = NULL;
    return(picoshell(cmds));
}