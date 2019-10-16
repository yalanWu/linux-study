#include <signal.h>
#include <stdio.h>
#include <sys/time.h>


void catchalarm(int signo)
{
	sigset_t pendset;
	sigpending(&pendset);
	for (int i = 0; i < 32; ++i)
	{
		if (sigismember(&pendset,i) == 1)
		{
			printf("1");
		}
		else
		{
			printf("0");
		}
	}
	printf("\n");
}

int main(int argc, char const *argv[])
{
	//创建自定义信号集
	sigset_t set;
	//清空集合
	sigemptyset(&set);
	//Ctrl+c被屏蔽了
	sigaddset(&set, SIGINT);

	//自定义信号集设置给阻塞信号集
	sigprocmask(SIG_BLOCK, &set, NULL);

	//得到常规信号的未决信号集

	//每隔1秒读一次内存的未决信号集
	//信号捕捉
	struct sigaction act;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	act.sa_handler = catchalarm;
	sigaction(SIGALRM, &act, NULL);
	//定时器
	struct itimerval value;
	// 循环周期
	value.it_interval.tv_sec = 1;
	value.it_interval.tv_usec = 0;
	//第一次触发延迟
	value.it_value.tv_sec = 1;
	value.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &value, NULL);

	while(1);

	return 0;
}