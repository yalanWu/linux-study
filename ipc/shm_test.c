/*
*
*父子进程利用共享内存通信
*用管道实现父子进程的互斥
* gcc -o shm_test shm_tell.c shm_test.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include "shm_tell.h"

int main(int argc, char const *argv[])
{

	//创建共享内存 1024是大小 可随意设置
	int shmid;
	if((shmid = shmget(IPC_PRIVATE, 1024, IPC_CREAT|IPC_EXCL|0777)) < 0) 
	{
		perror("shmget error");
		exit(1);
	}

	//初始化用来互斥的管道
	init_pipe();

	pid_t pid;
	if ( (pid = fork()) < 0)
	{
		perror("fork error");
		shmctl(shmid, IPC_RMID, NULL);
		exit(1);
	}

	//这里父子进程分别映射共享内存，可在父进程中映射一次，子进程继承了映射过来的虚拟内存
	if (pid > 0)//父进程往共享内存中写数据
	{
		//关闭管道读端
		close_pipe(0);
		//进行共享内存的映射 返回的是映射在自己的虚拟内存中的地址
		//返回的地址可强转成需要的类型
		int* ptr = (int*)shmat(shmid, 0, 0);
		if (ptr == (int*)-1)
		{
			perror("parent process shmat error");
			//关闭管道写端
			close_pipe(1);
			exit(1);
		}
		//共享内存中写入数据
		*ptr = 222;
		*(ptr + 1) = 223;
		//解除共享内存的映射
		shmdt(ptr);
		//通知子进程
		notify_pipe();
		//关闭管道写端
		close_pipe(1);
		//回收子进程 防止成为僵尸进程
		wait(0);
		//ipc对象是需要手动删除的 只有一份 所以删除一次即可
		//如果不删除 用ipcs -m 查看 此共享内存一直存在 除非重启系统
		shmctl(shmid, IPC_RMID, NULL);
	}
	else//子进程从共享内存中读数据
	{
		//关闭管道写端
		close_pipe(1);
		//进行共享内存的映射
		int* ptr = (int*)shmat(shmid, 0, 0);
		if (ptr == (int*)-1)
		{
			perror("child process shmat error");
			//关闭管道读端
			close_pipe(0);
			exit(1);
		}
		//阻塞 等待父进程通知
		wait_pipe();
		//从共享内存中读取数据
		printf("first : %d, second :%d \n", *ptr, *(ptr + 1));
		//解除共享内存的映射
		shmdt(ptr);
		//关闭管道读端
		close_pipe(0);
	}

	return 0;
}