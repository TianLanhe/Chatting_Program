#include "chat.h"

#define LENGTH 10 				//最大用户数

typedef struct _user{
	int userid;					    //用户ID，也是进程PID，若为0表示未使用
	char username[10];			//用户名称，这里采用分配的方式，为A-J
}User;

void InitUser();
//初始化用户列表
int checklogin();
//处理登录登出请求，若是登录请求，则尝试分配用户。若用户已满
//则发送PID为-1；若是登出请求，则从用户列表中删除，置为0表示未使用
//返回值：若为-1，表示出错
//		  若大于或等于LENGTH，则表示用户已满登录失败或用户列表找不到用户登出失败
//		  若[0,LENGTH)，表示正常
int passmsg();
//处理客户端发送过来的消息。若格式错误(X:message_content)，则发送PID为-2
//若格式正确，但目的用户不存在，则发送PID为-1
//若目的用户存在，则填写name_to和pid_to以及str，发送给源用户(表示确认)和目的用户
int userlogin();
int usermsg();

User userlink[LENGTH];			//已连接的用户数组
extern int mid;

void InitUser(){
	int i;
	char *name[LENGTH]={"A","B","C","D","E",
				"F","G","H","I","J"};
	for(i=0;i<LENGTH;i++){		//初始化用户列表的id为0，表示无人登录
		userlink[i].userid=0;	//名字采用分配的方式，分别为A-J
		strcpy(userlink[i].username,name[i]);
	}
	mid=msgget(KEY,IPC_CREAT|0660);
	if(mid == -1){
		perror("server create message:");
	}
}
int checklogin(){
	int sign;
	int i;
	Log message;
	sign=msgrcv(mid,&message,sizeof(Log)-sizeof(long),LOGIN,0);
	if(sign == -1){
		perror("server receive:");
		return -1;
	}
	if(message.flag == USER_LOG){
		for(i=0;i<LENGTH;i++){
			if(!userlink[i].userid){
				userlink[i].userid=message.pid;
				break;
			}
		}
		message.mtype=message.pid;
		if(i >= LENGTH)			//若聊天室已满人，则发送-1给客户端
			message.pid=-1;
		else{
			strcpy(message.name,userlink[i].username);
			message.pid=getpid();
		}
		sign=msgsnd(mid,&message,sizeof(Log)-sizeof(long),0);
		if(sign == -1){
			perror("server send:");
			return -1;
		}
	}else if(message.flag == USER_OLOG){
		for(i=0;i<LENGTH;i++){
			if(userlink[i].userid == message.pid){
				userlink[i].userid=0;
				break;
			}
		}
	}
	return i;
}
int passmsg(){
	int sign;
	int i;
	char *ptr;
	Chat message;
	sign=msgrcv(mid,&message,sizeof(Chat)-sizeof(long),SEND,0);
	if(sign == -1){
		perror("server receive:");
		return -1;
	}
	if(message.str[1] != ':'){	//格式错误
		message.mtype=message.pid_from;
		message.pid_to=-2;
		sign=msgsnd(mid,&message,sizeof(Chat)-sizeof(long),0);
		if(sign == -1){
			perror("server receive:");
			return -1;
		}
	}else{
		message.str[1]='\0';
		ptr=&message.str[2];
		for(i=0;i<LENGTH;i++)
			if(userlink[i].userid && !strcmp(message.str,userlink[i].username))
				break;
		if(i >= LENGTH){		//目的用户不存在
			message.mtype=message.pid_from;
			message.pid_to=-1;
			sign=msgsnd(mid,&message,sizeof(Chat)-sizeof(long),0);
			if(sign == -1){
				perror("server receive:");
				return -1;
			}
		}else{
			message.mtype=message.pid_from;
			message.pid_to=userlink[i].userid;
			strcpy(message.name_to,userlink[i].username);
			strcpy(message.str,ptr);
			sign=msgsnd(mid,&message,sizeof(Chat)-sizeof(long),0);
			if(sign == -1){
				perror("server send:");
				return -1;
			}
			message.mtype=message.pid_to;
			sign=msgsnd(mid,&message,sizeof(Chat)-sizeof(long),0);
			if(sign == -1){
				perror("server send:");
				return -1;
			}
		}
	}
	return 1;
}
int userlogin(){
	int i;
	while(1){
		if((i=checklogin(userlink)) == -1)return 1;
		else if(i < LENGTH){
			if(userlink[i].userid)printf("%s has logined.\n",userlink[i].username);
			else printf("%s has quited!\n", userlink[i].username);
		}else if(i >=LENGTH)printf("User is full!\n");
	}
}
int usermsg(){
	while(1){
		if(passmsg())printf("发送成功！\n" );
		else printf("发送失败！\n" );
	}
}
int main(){
	pthread_t thIDr, thIDw;
	InitUser();
	pthread_create(&thIDr, NULL,(void *)userlogin, NULL);
	pthread_create(&thIDw, NULL,(void *)usermsg, NULL);
	pthread_join(thIDr, NULL);
	pthread_join(thIDw, NULL);
	return 0;
}
