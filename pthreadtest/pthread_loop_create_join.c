#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void* threadWorkd(void* arg)
{
	//printf("the %d of child thread :%lu\n",0, pthread_self());
	int param = *(int*)arg;
	int term = (param + 1)*(param + 1);
	*(int*)(arg) = term;
	printf("the thread [%d] :term = %d \n", param, term);
	return NULL;
}


int main(int argc, char const *argv[])
{
	const int nthread = 5;
	pthread_t thread[nthread];
	int pthread_data[nthread];

	for (int i = 0; i < nthread; ++i)
	{
		pthread_data[i] = i;
     	int revno = pthread_create(&thread[i], NULL, threadWorkd, &pthread_data[i]);
     	if (revno != 0)
		{
			// 0时表示创建成功，注意获取错误信息的方法 strerror
			printf("the [%d]: error nuber:%d, %s\n", i, revno, strerror(revno));
			pthread_data[i] = 0;
		}
	}

	int sum = 0;
	for (int i = 0; i < nthread; ++i)
	{
		pthread_join(thread[i], NULL);
		sum += pthread_data[i];
	}
	
	printf("Sum is = %d\n", sum);

	return 0;
}