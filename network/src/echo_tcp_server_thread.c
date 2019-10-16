#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "msg.h"
#include <errno.h>
#include <pthread.h>

#define SERVER_PORT 8888
#define LISTEN_QUEUE_SISE 10

int socketfd;

void signal_handler(int signo)
{
	if (signo == SIGINT)
	{
		printf("this serveice close\n");
		close(socketfd);
		exit(1);
	}
}

void print_clientinfo(int fd, const char* type)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	socklen_t len = sizeof(addr);

	if(getpeername(fd, (struct sockaddr*)&addr, &len) < 0)
	{
		perror("getpeername error");
		return;
	}

	char ipstr[16];
	memset(ipstr, 0, sizeof(ipstr));
	// 将地址从网络字节序转换为点分十进制
	inet_ntop(AF_INET, &addr.sin_addr.s_addr, ipstr, sizeof(ipstr));

	printf("%s(%d) %s \n", ipstr, ntohs(addr.sin_port), type);
}

void do_something(int fd)
{
	char buff[1024];
	while(1)
	{
		//读取客户端发送过来的内容
		memset(buff, 0, sizeof(buff));
		size_t size = read_msg(fd, buff, sizeof(buff));
		if (size < 0)
		{
			// 协议出错 跳出while 回到main 结束子进程 
			perror("read_msg error");
			break;
		}
		else if (size == 0)
		{
			/* 客户端断开了连接 挂掉了 跳出while 会结束子线程
			 **/
			break;
		}
		
		printf("recev from client:%s\n",buff);

		//将数据写回
		if (write_msg(fd, buff, sizeof(buff)) < 0)
		{
			if(errno == EPIPE)
			{
				/* 客户端断开了连接  
				 * 产生SIGPIPE信号 
				 * 将error设置为EPIPE
				 * 所以可在此判断errno 也可捕捉SIGPIPE信号
				 * 俗称管道爆裂
				 * 跳出while 会结束子线程
				 **/
				break;
			}
			

			perror("write_msg error");
		}
	}
}

void* threadWork(void* arg)
{
	int fd = (int)arg;
	print_clientinfo(fd, "connected");
	do_something(fd);
	print_clientinfo(fd, "closed");
	close(fd);
	return NULL;
}


int main(int argc, char const *argv[])
{
	if (signal(SIGINT, signal_handler) == SIG_ERR)
	{
		perror("signal error");
		exit(1);
	}

	// 1 sokect
	// AF_INET ipv4
	// SOCK_STREAM tcp
	if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error");
		exit(1);
	}

	// 2 bind 绑定本地地址和端口
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;//ipv4
	serveraddr.sin_port = htons(SERVER_PORT); //端口
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);//响应任意网卡的请求
	if(bind(socketfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
	{
		perror("bind error");
		exit(1);
	}

	// 3 listen 启动监听 通知系统接受来自客户端的连接 准备好连接队列
	if(listen(socketfd, LISTEN_QUEUE_SISE) < 0)
	{
		perror("listen error");
		exit(1);
	}

	pthread_t thread;
	//设置分离属性相关 不需要主线程调用pthread_join回收子线程
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	int retno;
	while(1)
	{	
		// 4 accept 从队列拿出第一个
		int clientfd = accept(socketfd, NULL, NULL);
		if (clientfd  < 0)
		{
			perror("accept error");
			continue;
		}
		// 5 创建分离属性的子线程 在子线程中做read/write
		if( (retno = pthread_create(&thread, &attr, 
			threadWork, (void*)clientfd)) != 0 )
		{
			printf("error nuber:%d, %s\n", retno, strerror(retno));
			close(clientfd);
			continue;
		}

	}
	
	// 销毁线程属性
	pthread_attr_destroy(&attr);
	// 6 close
	close(socketfd);
	return 0;
}