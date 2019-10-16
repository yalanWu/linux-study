#include <unistd.h>

int main(int argc, char* argv[])
{

	int num=0;
	pid_t pid;	
	for(;num<3;++num)
	{
		pid = fork();
		if(pid==0)
		{
			break;
		}
	}

	while(1);
	return 0;
}
