/**
** 客户端的循环输入 
** 对应服务器端 trs_tcp_server.c
**/

#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

#define SERVER_PORT 8888
#define SERVER_IP 127.0.0.1

int main(int argc, char const *argv[])
{
	//step 1 create socket ipv4 tcp
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

	char buf[50];
	char prochar = '>';
	char prochar2 = '\n';
	size_t n;
	while(1)
	{
		memset(buf, 0, sizeof(buf));
		//控制台显示提示符
		write(STDOUT_FILENO, &prochar, 1);
		//读取控制台输入 read是阻塞函数
		n = read(STDIN_FILENO, buf, sizeof(buf));
		if (n < 0)
		{
			perror("std read error");
			continue;
		}

		//写入socket
		buf[n - 1] = '\0';
		n = write(socketfd, buf, n);
		if(n==0)
		{
			printf("server may closed\n");
			break;
		}
		else if(n < 0)
		{
			perror("write error");
			continue;
		}

		//等待服务器返回
		memset(buf, 0, sizeof(buf));
		n = read(socketfd, buf, sizeof(buf));
		if (n == 0)
		{
			printf("server may closed\n");
			break;
		}
		else if(n < 0)
		{
			perror("read error");
			continue;
		}
		write(STDOUT_FILENO, buf, n);
		write(STDOUT_FILENO, &prochar2, 1);
	}
	close(socketfd);
	return 0;
}