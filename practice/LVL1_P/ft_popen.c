/* Allowed functions: pipe, fork, dup2, execvp, close, exit

write the following function:

    int    ft_popen(const char *file, char const argv[], char type)

The function must launch the executable file with the arguments argv (using execvp).
If the type is 'r' the function must return a file descriptor connected to the output of the command.
If the type is 'w' the function must return a file descriptor connected to the input of the command.
In case of error or invalid parameter the function must return -1.

example:

int main() 
{
    int fd = ft_popen("ls", (char *const[]){"ls", NULL}, 'r');

    char	*line;
    while((line = get_next_line(fd)))
        ft_putendl_fd(line, 1);
}

Hint: Do not leak file descriptors! */

#include <sys/types.h> // pid_t , fork()
#include <unistd.h> //pipe(), dup2(), execvp(), close(), 
#include <stdlib.h> //exit()

// But if the parent doesn’t close the unused pipe end before reading,
// read() will never see EOF — it will just block
// and you might exit or stop early before the last chunk arrives.
int    ft_popen(const char *file, char *const argv[], char type)
{
	if (!file || !argv || (type != 'r' && type != 'w'))
		return (-1);
	int	fd[2];
	if (pipe(fd) == -1)
		return (-1);
	pid_t pid = fork();
	if (pid < 0)
	{
		close(fd[1]);
		close(fd[0]);
		return (-1);
	}
	if (pid == 0)
	{
		if (type == 'w')
		{
			// close(fd[0]);
			dup2(fd[1], STDOUT_FILENO);
			// close(fd[1]);
		}
		else if (type == 'r')
		{
			// close(fd[1]);
			dup2(fd[0], STDIN_FILENO);
			// close(fd[0]);
		}
		close(fd[0]);
		close(fd[1]);
		execvp(file, argv);
		exit (EXIT_FAILURE);
	}
	if (pid > 0)
	{
		if (type == 'r')
		{
			close(fd[1]);
			return (fd[0]);
		}
		else if (type == 'w')
		{
			close(fd[0]);
			return (fd[1]);
		}
	}
	return (-1);
}

int main() {
    int fd = ft_popen("cat", (char *const[]){"cat", "b", NULL}, 'r');

    char	*line;
    while((line = get_next_line(fd)) != NULL)
	{
		ft_putstr_fd("READ: ", 1);
        ft_putstr_fd(line, 1);
		free (line);
	}
	close(fd);
}