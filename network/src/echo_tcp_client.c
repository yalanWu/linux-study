#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include "msg.h"

#define SERVER_PORT 8888
#define SERVER_IP 127.0.0.1

int main(int argc, char const *argv[])
{
	//step 1 创建socket

	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0)
	{
		perror("socket error");
		exit(1);
	}

	//step 2 connect
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVER_PORT);

	if(connect(socketfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0 )
	{
		perror("connect error");
		exit(1);
	}

	//step 3 read write
	char buf[512];
	char* promt = ">";
	size_t size;
	while(1)
	{
		memset(buf, 0, sizeof(buf));
		//控制台显示提示符
		write(STDOUT_FILENO, promt, 1);
		//读取控制台输入 read是阻塞函数
		size = read(STDIN_FILENO, buf, sizeof(buf));
		if(size < 0) continue;
		buf[size - 1] = '\0';

		if (write_msg(socketfd, buf, size) < 0)
		{
			//写入错误 则忽略 进行下一次写入
			perror("write_msg error");
			continue;
		}

		//等待服务器返回
		memset(buf, 0, sizeof(buf));
		//read是阻塞函数 如果服务器没有下发消息，会一直阻塞在这里，直到收到消息。
		size = read_msg(socketfd, buf,sizeof(buf));
		if( size > 0)
		{
			printf("recev from server:%s\n",buf);
		}
		else if(size == 0)
		{
			//可能服务器关闭socket了
			printf("socket close from server\n");
			break;
		}
		else
		{
			perror("read_msg error");
			continue;
		}
	}

	//step 4 close
	close(socketfd);
	return 0;
}



