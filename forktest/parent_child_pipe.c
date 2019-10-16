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
	if (pid == -1)
	{
		perror("fork");
		exit(0);
	}


	if (pid > 0)
	{
		// 父进程 写 关闭读
		close(fd[0]);
		char* buf = "hello,world";
		write(fd[1], buf, strlen(buf)+1);
		close(fd[1]);
	}
	else if (pid == 0)
	{
		//子进程 读 关闭写
		close(fd[1]);
		char buf[512];
		read(fd[0], buf, sizeof(buf));
		printf("read from pip:%s\n",buf);
		close(fd[0]);
	}
	return 0;
}