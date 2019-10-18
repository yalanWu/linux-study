#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/shm.h>
#include <sys/sem.h>

/*
*利用共享内存和信号量 实现进程间的读者和写者问题
* gcc -o sem_read_write sem_read_write.c
*
*
*/

typedef struct
{
	//读写的真正的值
	int value;
	//信号集id 绑定在共享内存上
	int semid;
}Storage;

union Semun
{
	int val;
	struct semid_ds* buf;
	unsigned short* array;
};

//创建并初始化信号量
//失败返回-1
int sem_init(Storage* s)
{
	assert(s != NULL);
	//创建信号量集
	int semid = semget(IPC_PRIVATE, 2, IPC_CREAT | IPC_EXCL | 0777);
	if (semid < 0)
	{
		perror("semget error");
		return -1;
	}
	//绑定在共享内存上
	s->semid = semid;
	//准备数组
	//创建2个信号灯 0号表示读 1号表示写
	//0号初始化为0 1号初始化为1
	unsigned short arr[2] = {0, 1};
	
	union Semun un;
	un.array = arr;
	//初始化
	semctl(semid, 0, SETALL, un);
	return 0;
}

//索引为semnum的信号量做p操作
void sem_p(const Storage* s, int semnum, int value)
{
	assert( s != NULL);
	assert( value > 0);

	struct  sembuf ops[] = {
		{semnum, -value, SEM_UNDO}
	};
	semop(s->semid, ops, sizeof(ops)/sizeof(struct sembuf));
}

//索引为semnum的信号量做v操作
void sem_v(const Storage* s, int semnum, int value)
{
	assert( s != NULL);
	assert( value > 0);
	struct  sembuf ops[] = {
		{semnum, value, SEM_UNDO}
	};
	semop(s->semid, ops, sizeof(ops)/sizeof(struct sembuf));
}

//销毁信号量集
void sem_destroy(const Storage* s)
{
	assert(s != NULL);
	semctl(s->semid, 0, IPC_RMID);
}


//写者
void do_write(Storage* s)
{
	assert(s != NULL);
	for (int i = 1; i <= 100; ++i)
	{
		//1号P操作
		sem_p(s, 1, 1);
		s->value = i;
		printf("write:%d\n", s->value);
		//0号V操作
		sem_v(s, 0, 1);
	}
}

//读者
void do_read(const Storage* s)
{
	assert(s != NULL);
	for (int i = 1; i <= 100; ++i)
	{
		//0号P操作
		sem_p(s, 0, 1);
		printf("read:%d\n", s->value);
		//1号V操作
		sem_v(s, 1, 1);
	}
}

int main(int argc, char const *argv[])
{
	//创建共享内存
	int shmid = shmget(IPC_PRIVATE, sizeof(Storage), IPC_CREAT | IPC_EXCL | 0777);
	if (shmid < 0)
	{
		perror("shmget error");
		exit(1);
	}
	//映射共享内存
	Storage* storage = (Storage*)shmat(shmid, 0, 0);
	if (storage == (Storage*)-1)
	{
		perror("shmat error");
		//删除共享内存
		shmctl(shmid, IPC_RMID, NULL);
		exit(1);
	}
	//创建并初始化信号量集
	sem_init(storage);

	//fork子进程
	pid_t pid = fork();
	if (pid < 0)
	{
		perror("fork error");
		//删除共享内存
		shmctl(shmid, IPC_RMID, NULL);
		//删除信号量集
		sem_destroy(storage);
		exit(1);
	}

	if (pid > 0) //父进程（写者）
	{
		do_write(storage);
		//阻塞回收子进程
		wait(0);
		//删除信号量集
		sem_destroy(storage);
		//解除映射
		shmdt(storage);
		//删除共享内存
		shmctl(shmid, IPC_RMID, NULL);
	}
	else//子进程（读者） 继承了共享内存映射的虚拟地址 共享内存只有一份
	{
		do_read(storage);
		//解除映射
		shmdt(storage);
	}


	return 0;
}














