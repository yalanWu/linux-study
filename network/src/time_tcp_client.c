#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SERVER_PORT 8888
#define SERVER_IP "127.0.0.1"

int main(int argc, char const *argv[])
{
	//1 创建socket

	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0)
	{
		perror("socket error");
		exit(1);
	}

	//2 connect
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVER_PORT);
	//将IP地址转换为网络字节序后填入serveraddr
	inet_pton(AF_INET, SERVER_IP, &serveraddr.sin_addr.s_addr);

	if(connect(socketfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0 )
	{
		perror("connect error");
		exit(1);
	}

	//3 read write
	char buf[1024];
	//read是阻塞函数 如果服务器没有下发消息，会一直阻塞在这里，知道收到消息。
	if (read(socketfd, buf, sizeof(buf)) > 0)
	{
		printf("%s",buf);
	}

	//4 close
	close(socketfd);
	return 0;
}



