/*
*封装自定义的协议
*
*/
#include "msg.h"
#include <memory.h>
#include <unistd.h>

#define HEAD_DESC "elan2019"

//获得校验码
static unsigned char msg_checkcode(const Msg* message)
{
	unsigned char ret = 0;
	for (int i = 0; i < sizeof(message->head); ++i)
	{
		ret += message->head[i];
	}
	for (int i = 0; i < sizeof(message->data); ++i)
	{
		ret += message->data[i];
	}
	return ret;
}


// 从fd中读取内容，读取的数据存放到buff
int read_msg(int fd, char* buff, size_t buffsize)
{
	Msg message;
	memset(&message, 0, sizeof(message));

	size_t size = read(fd, &message, sizeof(message));
	if( size < 0 )
	{
		return -1;
	}
	else if (size == 0)
	{
		return 0;
	}

	//校验比对，判断接收到的数据完整性
	unsigned char cc = msg_checkcode(&message);
	if ((unsigned char)message.checkcode ==  cc
		&& (strcmp(HEAD_DESC, message.head) == 0))
	{
		memcpy(buff, &message.data, buffsize);
		return sizeof(message);
	}
	return -1;
}

//将buff中的内容写入到fd
int write_msg(int fd, char* buff, size_t buffsize)
{
	if (buffsize == 0)
	{
		//return -1;
	}
	// 构建message
	Msg message;
	memset(&message, 0, sizeof(message));
	//填充头
	strcpy(message.head, HEAD_DESC);
	//填充数据
	memcpy(&message.data, buff, buffsize);
	//填充校验码
	message.checkcode = msg_checkcode(&message);
	//开始写入
	size_t size;
	if((size = write(fd, &message, sizeof(message))) < 0)
	{
		return -1;
	}
	else if(size != sizeof(message))
	{
		return -1;
	}
	return sizeof(message);
}





