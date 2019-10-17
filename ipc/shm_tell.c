#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "shm_tell.h"

int fd[2];

//管道初始化
int init_pipe()
{
	if (pipe(fd) < 0)
	{
		perror("pipe error");
		return -1;
	}
	return 0;
}

//利用管道进行等待 就是读取管道 如果写端不写入 读端默认阻塞一直到写端写入数据
int wait_pipe()
{
	char buf;
	if(read(fd[0], &buf, 1) < 0)
	{
		perror("pipe read error");
		return -1;
	}
	return 0;
}

//利用管道通知
//向管道写入数据 那么读端就解除阻塞 相当于通知了读端
int notify_pipe()
{
	char buf = 't';
	if(write(fd[1], &buf, 1) < 0)
	{
		perror("pipe write error");
		return -1;
	}
	return 0;
}

//销毁管道
void destory_pipe()
{
	close(fd[0]);
	close(fd[1]);
}

//关闭管道的一端
void close_pipe(int flag)
{
	if (flag < 0 || flag > 1)
	{
		return;
	}

	close(fd[flag]);
}
