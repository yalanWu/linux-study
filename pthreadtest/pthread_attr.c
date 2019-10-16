#include <pthread.h>
#include <string.h>
#include <stdio.h>

void* threadWork(void* arg)
{
	printf("the child thread:%lu", pthread_self());
	return NULL;
}

int main(int argc, char const *argv[])
{
	pthread_t thread;
	//分离属性
	pthread_attr_t attr;
	//初始化
	pthread_attr_init(&attr);
	//设置分离
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	//创建线程的时候分离
	int revno = pthread_create(&thread, &attr, threadWork, NULL);
	if (revno != 0)
	{
		printf("error nuber:%d, %s\n", revno, strerror(revno));
	}

	sleep(2);

	//释放资源
	pthread_attr_destroy(&attr);


	//不需要主线程调用pthread_join回收子线程
	return 0;
}