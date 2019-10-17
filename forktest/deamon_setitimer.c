#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void catchalarm(int signo)
{
	//获取当前时间
	time_t curtime;
	time (&curtime);
	//格式化时间
	char* buf = ctime(&curtime);


	int fd = open("tmp/temp.txt", O_RDWR | O_CREAT | O_APPEND, 0777);
	write(fd, buf, strlen(buf)+1 );
	close(fd);
}


int main(int argc, char* argv[])
{

	pid_t pid = fork();
	if(pid >0)
	{
		//父进程杀死
		raise(SIGINT);
	}
	else if (pid == 0)
	{
		//变成会长 脱离终端控制 守护进程
		setsid();

		//修改工作目录 chdir
		//chdir 防止是U盘启动，拔掉U盘出错
		chdir("~");

		//关闭不需要的文件描述符节省资源
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);

		//重新设置从父进程继承来的文件掩码
		umask(0);

		//核心工作


		/* 方法一 sleep实现循环
		int fd = open("deamonfile", O_RDWR|O_CREAT,0777);
		if (fd < 0)
		{
			perror("open");
			exit(1);
		}
		
		while(1)
		{
			time_t timep;
			time (&timep);

			char* buf = asctime(gmtime(&timep));
			write(fd,buf,sizeof(buf));

			sleep(2);
		}
		close(fd);
		*/

		//方法二 使用setitimer定时器

		//信号捕捉
		struct sigaction act;
		act.sa_flags = 0;
		sigemptyset(&act.sa_mask);
		act.sa_handler = catchalarm;
		sigaction(SIGALRM, &act, NULL);

		//定时器
		struct itimerval value;
		// struct itimerval {
		//     struct timeval it_interval; /* next value */
		//     struct timeval it_value;    /* current value */
		// };
		 
		// struct timeval {
		//     time_t      tv_sec;         /* seconds */
		//     suseconds_t tv_usec;        /* microseconds */
		// }
		// 循环周期
		value.it_interval.tv_sec = 2;
		value.it_interval.tv_usec = 0;
		//第一次触发延迟
		value.it_value.tv_sec = 1;
		value.it_value.tv_usec = 0;
		setitimer(ITIMER_REAL, &value, NULL);


		//保证子进程处于运行状态
		while(1);
	}



	return 0;
}
