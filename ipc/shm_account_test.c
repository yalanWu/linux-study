#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <assert.h>
#include "shm_account.h"

/*
*共享内存操作了一个结构体 没有用互斥 所以产生问题 两个线程都取了10000块
*gcc -o shm_account_test shm_account.c shm_account_test.c

*/

int main(int argc, char const *argv[])
{
	//创建共享内存
	int shmid;
	if( (shmid = shmget(IPC_PRIVATE, sizeof(SHMACOUNT), IPC_CREAT|IPC_EXCL|0777)) < 0)
	{
		perror("shmget error");
		exit(1);
	}

	//在父进程进行映射
	SHMACOUNT* a =  (SHMACOUNT* )shmat(shmid, 0, 0);
	if (a == (SHMACOUNT* )-1)
	{
		perror("shmat error");
		//删除创建的共享内存
		shmctl(shmid, IPC_RMID, NULL);
		exit(1);
	}

	//共享内存中存入数据
	a->code = 88888888;
	deposit(a, 10000);
	printf("init code: %d balance :%f\n", a->code, get_balance(a));

	pid_t pid = fork();
	if( pid < 0)
	{
		perror("fork error");
		//删除创建的共享内存
		shmctl(shmid, IPC_RMID, NULL);
		exit(1);
	}
	else if( pid > 0)//父进程
	{
		//取款
		double amt = withdraw(a, 10000);
		printf("parent process withdraw %f from code %d, balance :%f\n", amt, a->code, get_balance(a));
		//回收子进程
		wait(0);
		printf("final code: %d balance :%f\n", a->code, get_balance(a));
		//解除映射
		shmdt(a);
		//删除创建的共享内存
		shmctl(shmid, IPC_RMID, NULL);
		printf("没有加互斥对象，肯定会有问题\n");
	}
	else //子进程
	{
		//继承了共享内存的映射关系
		//取款
		double amt = withdraw(a, 10000);
		printf("child process withdraw %f from code %d, balance :%f\n", amt, a->code, get_balance(a));
		//解除映射
		shmdt(a);
	}



	return 0;
}