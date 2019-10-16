#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>


//定义自己的消息结构体
typedef struct 
{
	long mtype;//消息类型
	int start;//下面都是消息内容 包括start和end
	int end;
}MyTestMSG;

int main(int argc, char const *argv[])
{
	if (argc < 2)
	{
		printf("usage: %s key\n", argv[0]);
		exit(1);
	}

	//获取命令行第二个参数做为key
	key_t key = atoi(argv[1]);
	//创建消息队列 如果队列已经存在 则返回-1 程序退出
	int msq_id;
	if((msq_id = msgget(key, IPC_CREAT|IPC_EXCL|0777))< 0 )
	{
		perror("msgget error");
		exit(1);
	}

	printf("key: %d\n", key);
	printf("msg qid:%d\n", msq_id);
	//定义好要发送的消息
	MyTestMSG m1 = {5,50, 500};
	MyTestMSG m2 = {1,10, 100};
	MyTestMSG m3 = {3,30, 300};
	MyTestMSG m4 = {1,10, 101};
	MyTestMSG m5 = {9,90, 900};
	MyTestMSG m6 = {2,20, 200};
	MyTestMSG msgArr[6];
	msgArr[0] = m1;
	msgArr[1] = m2;
	msgArr[2] = m3;
	msgArr[3] = m4;
	msgArr[4] = m5;
	msgArr[5] = m6;

	//发送消息
	for (int i = 0; i < 6; ++i)
	{
		//第三个长度为结构体的长度减去第一个long型的type长度 
		//最后一个参数 设置非阻塞
		if (msgsnd(msq_id, &msgArr[i], sizeof(MyTestMSG) - sizeof(long), IPC_NOWAIT) < 0)
		{
			perror("msg send error");
		}
	}

	//获得队列中消息总数
	struct msqid_ds ds;
	if(msgctl(msq_id, IPC_STAT, &ds) < 0 )
	{
		perror("msgctl error");
	}
	
	printf("msg total: %ld\n", ds.msg_qnum);

	return 0;
}