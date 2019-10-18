#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <assert.h>
#include "shm_account.h"
#include "sem_pv.h"

/*
*共享内存操作了一个结构体 信号量做的互斥
*gcc -o sem_pv_test shm_account.c sem_pv.c sem_pv_test.c

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

	//创建信号量 一个资源所以需要一个信号灯就可以了 初始值为1
	int semid = i_sem(1, 1);
	if (semid < 0)
	{
		//删除创建的共享内存
		shmctl(shmid, IPC_RMID, NULL);
		exit(-1);
	}

	//v_sem(semid, 0, 1);
	pid_t pid = fork();
	if( pid < 0)
	{
		perror("fork error");
		//删除创建的共享内存
		shmctl(shmid, IPC_RMID, NULL);
		//删除创建的信号量集
		d_sem(semid);
		exit(1);
	}
	else if( pid > 0)//父进程
	{
		//取款之前先对信号量集中的索引为0的信号量做P操作
		printf("===============>>>>>parent try get sem:%d\n",semid);
		p_sem(semid, 0, 1);

		//取款
		double amt = withdraw(a, 10000);
		printf("===============>>>>>>parent process withdraw %f from code %d\n", amt, a->code);
		//对信号量集中的索引为0的信号量做V操作
		v_sem(semid, 0, 1);
		//回收子进程
		wait(0);
		printf("final code: %d balance :%f\n", a->code, get_balance(a));
		//解除映射
		shmdt(a);
		//删除创建的共享内存
		shmctl(shmid, IPC_RMID, NULL);
		//删除创建的信号量集
		d_sem(semid);
	}
	else //子进程
	{
		//继承了共享内存的映射关系
		//取款之前先对信号量集做P操作 继承了父进程的信号量集关系
		printf("===============>>>>>child try get sem:%d\n",semid);
		p_sem(semid, 0, 1);
		//取款
		double amt = withdraw(a, 10000);
		printf("===============>>>>>child process withdraw %f from code %d\n", amt, a->code);
		//对信号量做V操作
		v_sem(semid, 0, 1);
		//解除映射
		shmdt(a);
	}



	return 0;
}