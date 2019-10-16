#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>

/**
* type 传的负数 取小于等于abs(type)的消息中的类型最小的第一个 所以取到类型为1的第一条
* ./msg_rcv 2 -4
* msg qid:131072
* msg: type:1 start:10 end:100
*
*/

//定义自己的消息结构体
typedef struct 
{
	long mtype;//消息类型
	int start;//下面都是消息内容 包括start和end
	int end;
}MyTestMSG;

int main(int argc, char const *argv[])
{
	if (argc < 3)
	{
		printf("usage: %s key type\n", argv[0]);
		exit(1);
	}

	//获取key
	key_t key = atoi(argv[1]);
	long type = atoi(argv[2]);

	//根据key获得消息队列
	int msq_id;
	if( (msq_id = msgget(key, 0777)) < 0)
	{
		perror("msgget error");
		exit(0);
	}

	printf("msg qid:%d\n", msq_id);

	MyTestMSG msg;
	//根据类型获得消息
	if(msgrcv(msq_id, &msg, sizeof(MyTestMSG) - sizeof(long), type, IPC_NOWAIT) < 0 )
	{
		perror("msg rcv");
		exit(1);
	}		

	printf("msg: type:%ld start:%d end:%d\n", msg.mtype, msg.start, msg.end);

	return 0;
}