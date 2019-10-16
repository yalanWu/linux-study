#include <unistd.h>
#include <stdio.h> 

int main(int argc, char* argv[])
{
	pid_t pid = fork();
	if(pid>0)
{
	printf("this is parent process, pid=%d\n",getpid());
}
else
{
	printf("this is child process ,pid=%d\n",getpid());
}


	return 0;
}
