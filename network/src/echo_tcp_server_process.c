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
#include <errno.h>
#include "msg.h"

#define SERVER_PORT 8888
#define LISTEN_QUEUE_SISE 10

int socketfd;

void signal_handler(int signo)
{
	if (signo == SIGINT)
	{
		printf("server close by ctrl c\n");
		close(socketfd);
		exit(1);
	}
	else if (signo == SIGCHLD)
	{
		//子进程结束 向父进程发送 SIGCHLD 信号
		//父进程区 回收僵尸子进程
		printf("child process dead....\n");
		wait(NULL);//阻塞回收一个子进程
	}
	
}

void out_clientinfo(const struct sockaddr_in* outsockaddr)
{
	char ipstr[16];
	memset(ipstr, 0, sizeof(ipstr));
	// 将地址从网络字节序转换为点分十进制
	inet_ntop(AF_INET, &outsockaddr->sin_addr.s_addr, ipstr, sizeof(ipstr));

	printf("Connected by %s(%d)\n", ipstr, ntohs(outsockaddr->sin_port));
}

// 和客户端双向通信 收到客户端发来的数据马上返回
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
			/* 客户端断开了连接 挂掉了 跳出while 回到main往下走，会结束子进程
			 * 结束子进程 向父进程发送SIGCHLD信号
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
				 * 跳出while 回到main往下走，会结束子进程，结束子进程 向父进程发送SIGCHLD信号
				 **/
				break;
			}
			

			perror("write_msg error");
		}
	}
}


int main(int argc, char const *argv[])
{
	//Ctrl+C
	if (signal(SIGINT, signal_handler) == SIG_ERR)
	{
		perror("signal SIGINT error");
		exit(1);
	}

	//子进程结束 向父进程发送SIGCHLD
	if (signal(SIGCHLD, signal_handler) == SIG_ERR)
	{
		perror("signal SIGCHLD error");
		exit(1);
	}

	/* 1 sokect
	 * AF_INET ipv4
	 * SOCK_STREAM tcp
	 **/
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

	struct sockaddr_in clientaddr;
	socklen_t clientaddr_len = sizeof(clientaddr);
	while(1)
	{	
		// 4 accept 从队列拿出第一个
		// clientaddr获取客户端的地址信息，是传出参数
		int clientfd = accept(socketfd, (struct sockaddr*)&clientaddr, &clientaddr_len);
		if (clientfd  < 0)
		{
			perror("accept error");
			continue;
		}
		// 5 创建子进程，在子进程中做读写操作 所以注意进程回收
		pid_t pid = fork();
		if(pid < 0)
		{
			//创建进程失败 继续下一次循环
			perror("fork error");
			close(clientfd);
			continue;
		}
		else if(pid == 0)
		{
			//子进程
			/* 父进程中监听的套接字不再使需要 要关闭
			 * 子进程退出后 父进程在运行 那么子进程会变成僵尸进程 父进程需要监听SIGCHLD
			 * 子进程不再fork 必须break
			 **/
			close(socketfd);
			out_clientinfo(&clientaddr);
			do_something(clientfd);
			close(clientfd);
			break;
		}
		else
		{
			//父进程
			close(clientfd);//不使用 关闭
		}
	}
	
	// 6 close
	return 0;
}