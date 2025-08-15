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

#include <unistd.h> //close, execvp, dup2, pipe
#include <sys/wait.h> //wait, pid_t (fork), 
#include <stdlib.h> // exit, 

int picoshell(char *cmds[])
{
    int i = 0;
    int fd[2];
    int in_fd = 0;   // default standard input file descriptor [0]
    int ret = 0;
    pid_t pid;
    int status;

    while (cmds[i])
    {
        if (cmds[i + 1]) // If it’s not the last command////////////////////////////////////
        {
            if (pipe(fd) == -1)
                return 1;
        }
        else
        {
            fd[0] = -1;
            fd[1] = -1;
        }

        pid = fork();
        if (pid < 0) // Process fail //////////////////////////////////////////////////
        {
            if (fd[0] != -1)
                close(fd[0]);
            if (fd[1] != -1)
                close(fd[1]);
            if (in_fd != 0)
                close(in_fd);
            return 1;
        }
        if (pid == 0) // Child ////////////////////////////////////////////////////////
        {
            if (in_fd != 0)
            {
                if (dup2(in_fd, STDIN_FILENO) == -1)
                    exit(1);
                close(in_fd);
            }
            if (fd[1] != -1) // If it’s not the last one
            {
                if (dup2(fd[1], STDOUT_FILENO) == -1)
                    exit(1);
                close(fd[1]);
                close(fd[0]);
            }
            execvp(cmds[i][0], cmds[i]);
            exit(EXIT_FAILURE); // execvp failed
        }
        else if (pid > 0) // Parent //////////////////////////////////////////////////////
        {
            if (in_fd != 0)
                close(in_fd);
            if (fd[1] != -1)
                close(fd[1]);
			// if (fd[0] != -1)
				// close(fd[0]);
            in_fd = fd[0];
            i++;
        }////////////////////////////////////////////////////////////////////////////////////
    }
	i = 0;
    while (cmds[i])
	{
		if (wait(&status) > 0)
		{
			if (WIFEXITED(status) != 0 && WEXITSTATUS(status) != 0) // == 0 (false) // 
				ret = 1;
			else if (WIFEXITED(status) == 0)
				ret = 1;
		}
		i++;
	}
    return ret;
}


int main(int argc, char **argv)
{
    // Example: ./picoshell /bin/ls "|" /usr/bin/grep picoshell
    //          ./picoshell echo squalala "|" cat "|" sed s/a/b/g

    // We'll build the cmds array: each element is a NULL-terminated argv array for one command.
    // "cmds" will be terminated by a final NULL.

    // This is just an example for:
    // ls -l | grep main
    char *cmd1[] = { "/bin/ls", "-l", NULL };
    char *cmd2[] = { "/usr/bin/grep", "open", NULL };
    char **cmds[] = { cmd1, cmd2, NULL };

    // Call your pipeline executor
    return picoshell(cmds);
}

// can only run if we can make the main like this and just run ./picoshell alone, without argv 
// but if we have to accept argv after ./picoshell, the code need to edit more 
// - to take the whole char ** (/bin/ls -l) as our arg in execvp 
// and to continue + do pipe if we came upon "|"