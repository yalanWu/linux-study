#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
	int fd[2];
	int ret = pipe(fd);
	if (ret == -1)
	{
		perror("pipe error");
		exit(1);
	}

	int i = 0;
	for (i = 0; i < 2; ++i)
	{
		pid_t pid = fork();
		if (pid == 0)
		{
			break;
		}
	}

	if (i == 2)
	{
		//父进程 关闭读和写
		close(fd[0]);
		close(fd[1]);
		//回收子进程
		pid_t wpid;
		while((wpid = waitpid(-1, NULL, WNOHANG)) != -1)
		{
			if (wpid == 0)
			{
				continue;
			}
			printf("died child pid = %d\n", wpid);
		}
	}
	else if( i == 0)
	{
		// 子进程1  写 ps aux
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		execlp("ps", "ps", "aux", NULL);
		close(fd[1]);
	}
	else if (i == 1)
	{
		// 子进程2 读 grep bash
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		execlp("grep", "grep", "bash",NULL);
		close(fd[0]);
	}
	return 0;
}