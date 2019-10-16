#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
* 信号量 信号灯 是锁 高级的互斥锁
* 互斥锁：串行 锁最大为1
* 信号量：并行 多把互斥锁 锁的最大值取决于初始设置
* 
* 
*/

////////////////编译不通过 macos下没有sem_init；

//声明信号量
//生产者信号量
sem_t sem_producer;
//消费者信号量
sem_t sem_consumer;
int g_num = 0;

void* producer(void* arg)
{
	while(1)
	{
		//加锁 value-- 不一定阻塞 value==0时 才会阻塞
		sem_wait(&sem_producer);
		++g_num;
		printf("producer: %d\n", g_num);
		//通知消费者线程
		sem_post(&sem_consumer);
		sleep(rand()%3);
	}
	
	return NULL;
}

void* consumer(void* arg)
{
	while(1)
	{
		sem_wait(&sem_consumer);//==0就阻塞
		--g_num;
		printf("consumer: %d\n", g_num);
		//将占有的资源释放 还给生产者
		sem_post(&sem_producer);
		sleep(rand()%3);
	}
	return NULL;
}

int main(int argc, char const *argv[])
{
	//初始化信号量
	int pshared = 0;// 0 线程同步 1 进程同步
	int producerValue = 1;//资源的个数，也就是同时允许访问的线程的个数
	sem_init(&sem_producer, pshared, producerValue);
	//初始化的时候不给消费者分配资源
	sem_init(&sem_consumer, pshared, 0);

	pthread_t p1, p2, p3, p4;
	pthread_create(&p1, NULL, producer, NULL);
	pthread_create(&p2, NULL, consumer, NULL);
	pthread_create(&p3, NULL, consumer, NULL);
	pthread_create(&p4, NULL, consumer, NULL);

	//回收线程
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	pthread_join(p3, NULL);
	pthread_join(p4, NULL);
	//销毁信号
	sem_destroy(&sem_producer);
	sem_destroy(&sem_consumer);
	return 0;
}