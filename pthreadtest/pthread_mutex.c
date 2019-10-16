#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/*
* 线程的互斥，并不能实现线程同步
* mutex 多个线程访问同一资源是串行的，效率低
*
* pthread_mutex_t mutex;
* pthread_mutex_init(&mutex, NULL);// lock = 1
* pthread_mutex_lock(&mutex);//如果已锁，则阻塞。等到锁被打开，获得后，解除阻塞。 lock =0
*	pthread_mutex_trylock(&mutex) 如果没锁，会加锁，返回0。如果已锁，不会阻塞，会马上返回。
* pthread_mutex_unlock(&mutex); lock = 1
* pthread_mutex_destroy(&mutex);
*
**/

int g_num;
pthread_mutex_t mutex;

void* threadWork1(void* arg)
{
	int idx = (int)arg;
	for (int i = 0; i < 20; ++i)
	{
		pthread_mutex_lock(&mutex);
		g_num++;
		printf("thread[%d] == %d\n", idx, g_num);
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
	
	return NULL;
}

void* threadWork2(void* arg)
{
	int idx = (int)arg;
	for (int i = 0; i < 20; ++i)
	{
		pthread_mutex_lock(&mutex);
		g_num++;
		printf("thread[%d] == %d\n", idx, g_num);
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
	return NULL;
}

int main(int argc, char const *argv[])
{
	pthread_mutex_init(&mutex, NULL);
	pthread_t thread[2];
	int revno;
	revno = pthread_create(&thread[0], NULL, threadWork1, (void*)1) ;
	if (revno != 0 )
	{
		printf("the [2] thread create: error nuber:%d, %s\n", revno, strerror(revno));
		exit(1);
	}

	revno = pthread_create(&thread[1], NULL, threadWork2, (void*)2);
	if( revno != 0)
	{
		printf("the [2] thread create: error nuber:%d, %s\n", revno, strerror(revno));
		exit(1);
	}

	for (int i = 0; i < sizeof(thread); ++i)
	{
		pthread_join(thread[i], NULL);
	}


	printf("g_num == %d\n", g_num);

	pthread_mutex_destroy(&mutex);

	return 0;
}