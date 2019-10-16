#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
/* 读时共享 写时独占
* 特性： A加读锁成功，又来三个线程做读操作，可加锁成功。----读共享
*		A加写锁成功，又来三个线程做写操作，三个线程会阻塞。 -----写独占
*		A加读锁成功，B线程加写锁会阻塞，C线程加读锁会阻塞。------读写不能同时，写的优先级高。

* 场景： A持写锁，B请求读锁，C请求写锁。
*		BC都会阻塞->A解锁，C会加写锁，B阻塞->C解锁，B加读锁。
*
* 选择：当读多于写的时候，选择读写锁。否则互斥锁/读写锁都可以使用。
*
* pthread_rwlock_t rwlock
* pthread_rwlock_init(&rwlock, NULL);
* pthread_rwlock_wrlock(&rwlock);
* pthread_rwlock_rdlock(&rwlock);
* pthread_rwlock_unlock(&rwlock);
* pthread_rwlock_destroy(&rwlock);
*/

int g_num = 0;
pthread_rwlock_t rwlock;


void* writeFun(void* arg)
{
	int idx = (int)arg;
	while(1)
	{
		//加写锁
		pthread_rwlock_wrlock(&rwlock);
		g_num++;
		printf("[%d] write : %d\n", idx, g_num);
		//解锁
		pthread_rwlock_unlock(&rwlock);
		usleep(500);
	}

	return NULL;
}	

void* readFun(void* arg)
{
	int idx = (int)arg;
	while(1)
	{
		//加读锁
		pthread_rwlock_rdlock(&rwlock);
		printf("[%d] read : %d\n", idx, g_num);
		//解锁
		pthread_rwlock_unlock(&rwlock);
		usleep(500);
	}
	return NULL;
}

int main(int argc, char const *argv[])
{
	//初始化读写锁
	pthread_rwlock_init(&rwlock, NULL);

	pthread_t thread[8];

	//3个写线程
	for (int i = 0; i < 3; ++i)
	{
		pthread_create(&thread[i], NULL, writeFun, (void*)(i+1));
	}

	// 5个读线程
	for (int i = 3; i < 8; ++i)
	{
		pthread_create(&thread[i], NULL, readFun, (void*)(i+1));
	}

	//回收子线程
	for (int i = 0; i < 8; ++i)
	{
		pthread_join(thread[i], NULL);
	}

	//销毁读写锁
	pthread_rwlock_destroy(&rwlock);

	return 0;
}