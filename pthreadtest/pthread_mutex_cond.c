#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

/*
* ondition实现生产者消费者问题
* 条件变量并不是锁 只是一组函数，需要和mutex配合使用 实现同步
*
**/

//互斥体 抢资源
pthread_mutex_t mutex;
// 条件变量 阻塞线程
pthread_cond_t cond;
int g_num = 0;

void* producer(void* arg)
{
	while(1)
	{
		pthread_mutex_lock(&mutex);
		g_num++;
		printf("producer:%d\n",g_num);
		pthread_mutex_unlock(&mutex);
		//唤醒至少一个阻塞在条件变量上的线程
		pthread_cond_signal(&cond);
		sleep(rand()%3);
	}

	return NULL;
}

void* consumer(void* arg)
{
	while(1)
	{
		pthread_mutex_lock(&mutex);
		if (g_num <= 0)
		{
			//阻塞，节省CPU资源了呢
			//释放mutex锁，阻塞线程，等待一个条件变量，
			pthread_cond_wait(&cond, &mutex);
			//接到条件变量信号时，解除线程阻塞，将mutex加锁，继续向下执行
		}
		g_num--;
		printf("consumer:%d\n",g_num);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

int main(int argc, char const *argv[])
{
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	pthread_t p1;
	pthread_create(&p1, NULL, producer, NULL);

	pthread_t p2;
	pthread_create(&p2, NULL, consumer, NULL);

	//阻塞回收线程
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);

	//销毁互斥体
	pthread_mutex_destroy(&mutex);
	//销毁条件变量
	pthread_cond_destroy(&cond);
	return 0;
}