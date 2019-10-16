/*
*封装自定义的协议
*
*/

#ifndef _MSG_H_
#define _MSG_H_

#include <sys/types.h>

typedef struct msg
{
	//协议头
	char head[9];
	//校验码
	char checkcode;
	//实际的存入数据
	char data[1024];
}Msg;

//发送一个基于自定义协议的Msg
//从fd中读取内容，写入到buff
extern int read_msg(int sockfd, char* buff, size_t buffsize);

////读取一个基于自定义协议的Msg
//将buff中的内容写入到fd
extern int write_msg(int sockfd, char* buff, size_t buffsize);

#endif