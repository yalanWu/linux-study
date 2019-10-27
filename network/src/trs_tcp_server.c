/**
** 可接受来自单个客户端的循环输入 
** 对应客户端 trs_tcp_client.c
**/

#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SERVER_PORT 8888
#define LISTEN_QUEUE_SISE 10

int socketid;

void signal_handler(int signo)
{
	printf("this serveice close\n");
	close(socketid);
	exit(1);
}

void out_clientinfo(const struct sockaddr_in* outsockaddr)
{
	char ipstr[16];
	memset(ipstr, 0, sizeof(ipstr));
	// 将地址从网络字节序转换为点分十进制
	inet_ntop(AF_INET, &outsockaddr->sin_addr.s_addr, ipstr, sizeof(ipstr));

	printf("Connected by %s(%d)\n", ipstr, ntohs(outsockaddr->sin_port));
}

void dosomething(int fd)
{
	char buf[512];
	int size ;
	
	while(1)
	{
		memset(buf, 0, sizeof(buf));
		size = read(fd, buf, sizeof(buf));
		if (size == 0 )
		{
			printf("client may closed\n");
			break;
		}
		else if( size < 0 )
		{
			perror("read client error ");
			continue;
		}
		else
		{
			for(int i = 0; i<size;++i)
				buf[i] = toupper(buf[i]);

			size = write(fd, buf, size);
			if (size < 0 )
			{
				perror("write to client error");
				continue;
			}
			else if (size == 0)
			{
				printf("client may closed\n");
				break;
			}
			else
			{
				//printf("write to %s\n", buf);
			}

		}
	}
}

int main(int argc, char const *argv[])
{
	if (signal(SIGINT, signal_handler) == SIG_ERR)
	{
		perror("signal error");
		exit(1);
	}

	//获得socket
	socketid = socket(AF_INET, SOCK_STREAM, 0);
	if (socketid < 0)
	{
		perror("socke error");
		exit(1);
	}
	//获得addr
	struct sockaddr_in seradder;
	bzero(&seradder, sizeof(seradder));
	seradder.sin_family = AF_INET;
	seradder.sin_port = htons(SERVER_PORT); //端口
	seradder.sin_addr.s_addr = htonl(INADDR_ANY);//响应任意网卡的请求
	//bind
	if (bind(socketid, (struct sockaddr*)&seradder, sizeof(seradder)) < 0)
	{
		perror("bind error");
		exit(1);
	}
	//listen
	listen(socketid, LISTEN_QUEUE_SISE);

	//accepte
	struct sockaddr_in clientaddr;
	socklen_t clientaddr_len = sizeof(clientaddr);
	int clientfd;
	while(1)
	{
		clientfd = accept(socketid, (struct sockaddr*)&clientaddr, &clientaddr_len);
		if (clientfd  < 0)
		{
			perror("accept error");
			continue;
		}
		// read/write
		out_clientinfo(&clientaddr);
		dosomething(clientfd);
		//close
		printf("close socket of client \n");
		close(clientfd);
	}

	//close
	close(socketid);
	return 0;
}