#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
	int fd[2];
	int ret = pipe(fd);
	if (ret == -1)
	{
		perror("pipe error");
		exit(1);
	}

	pid_t pid = fork();
	if (pid > 0)
	{
		//父进程 写 ps aux
		close(fd[0]);
		//文件描述符重定向
		dup2(fd[1], STDOUT_FILENO);
		execlp("ps", "ps", "aux", NULL);
		perror("execlp");
		exit(1);
	}
	else if (pid == 0)
	{
		//子进程 grep bash
		close(fd[1]);

		dup2(fd[0], STDIN_FILENO);
		execlp("grep", "grep", "bash",NULL);
		close(fd[0]);
	}


	return 0;
}