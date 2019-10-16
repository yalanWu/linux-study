#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	//设置定时器
	alarm(1);
	//开始计时
	int i = 0;
	while(1)
	{
		printf("%d\n",++i);
	}
	return 0;
}