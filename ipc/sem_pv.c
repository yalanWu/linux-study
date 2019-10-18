#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/sem.h>

union Semun{
	int val;
	struct semid_ds* buf;
	unsigned short* array;
};


//创建并初始化信号量集
//nums 信号量的个数
//value 初始话的值
//成功：返回信号集id 失败：返回-1
int i_sem(int nsems, int value)
{
	assert(nsems > 0);
	//创建有nsems个信号量的信号量集
	int semid = semget(IPC_PRIVATE, nsems, IPC_CREAT|IPC_EXCL|0777);
	if(semid < 0 )
	{
		perror("semget error");
		return -1;
	}

	//创建数据给Semun使用
	unsigned short* array = (unsigned short *)calloc(nsems, sizeof(unsigned short));

	for (int i = 0; i < nsems; ++i )
	{
		array[i] = value;
	}

	union Semun un;
	un.array = array;

	//设置所有的信号量的值为value
	//参数二：0表示对所有信号量操作
	//参数三：操作类型 IPC_STAT IPC_SET GETVAL SETVAL GETALL SETALL
	semctl(semid, 0, SETALL, un);

	free(array);

	return semid;
}

//对semid信号量集中的索引为semnum的信号量，做取得操作 取得数量为value
int p_sem(int semid, int semnum, int value)
{
	assert(value >= 0);

	//SEM_UNDO 表示进程未归还资源就退出时，由内核归还资源
	//当前数组中只有一个成员 可定义若干个
	struct sembuf ops[] = { {semnum, -value, SEM_UNDO}};

	if( semop(semid, ops, sizeof(ops)/sizeof(struct sembuf)) < 0 )
	{
		perror("sem p error");
		return -1;
	}

	return 0;
}

//对semid信号量集中的索引为semnum的信号量，做归还操作 归还数量为value
int v_sem(int semid, int semnum, int value)
{
	assert(value >= 0);

	//SEM_UNDO 表示进程未归还资源就退出时，由内核归还资源
	struct sembuf ops[] = { {semnum, value, SEM_UNDO}};

	if( semop(semid, ops, sizeof(ops)/sizeof(struct sembuf)) < 0 )
	{
		perror("sem p error");
		return -1;
	}

	return 0;
}

//销毁创建的信号量 否则会一直存在系统中 使用ipcs可查看
void d_sem(int semid)
{
	semctl(semid, 0, IPC_RMID);
}