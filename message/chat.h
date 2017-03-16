#ifndef chat_h
#define chat_h

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <error.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define LOGIN 100				//消息类型，表示登录或登出
#define SEND 90					//消息类型，表示发送信息
#define LENGTH 10 			//最大用户数
#define USER_LOG 1 			//登录类型：表示登录
#define USER_OLOG 0 		//登录类型：表示登出
#define KEY ((key_t)100)//消息队列KEY

typedef struct _log{
	long mtype;
	int pid;					//发送请求的进程PID
	char name[10];		//分配给客户端的用户名称
	int flag;					//表示登录还是登出的标志
}Log;
typedef struct _chat{
	long mtype;
	int pid_from;				//源进程PID
	char name_from[10];	//源进程用户姓名
	int pid_to;					//目的进程PID
	char name_to[10];		//目的进程用户姓名
	char str[100];			//聊天内容
}Chat;
int mid;
#endif
