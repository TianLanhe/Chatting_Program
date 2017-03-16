#include "chat.h"

int login();
//发送登录请求，并接收回应，若PID=-1，表示登录失败，否则登录登录成功，取出应答中的名字
int Write_to();
//负责读取屏幕输入，并将消息发送到服务器端（若屏幕输入quit QUIT exit EXIT，则退出程序）
int Read_from();
//负责从服务器接收消息，并将结果输出到屏幕

char name[11];
extern int mid;

int login(){
	int sign;
	Log message;
	message.mtype=LOGIN;
	message.pid=getpid();
	message.flag=USER_LOG;
	sign=msgsnd(mid,&message,sizeof(Log)-sizeof(long),0);
	if(sign == -1){
		perror("client send:");
		return -1;
	}
	sign=msgrcv(mid,&message,sizeof(Log)-sizeof(long),getpid(),0);
	if(sign == -1){
		perror("client receive:");
		return -1;
	}
	if(message.pid == -1)return -1;
	strcpy(name,message.name);
	return 0;
}
int Write_to(){
	int sign;
	Chat message;
	while(1){
		printf("I:");
		fgets(message.str,100,stdin);
		message.str[strlen(message.str)-1]='\0';  //最后一个本来是'\n'
		if(!strcmp(message.str,"quit")||!strcmp(message.str,"QUIT")||!strcmp(message.str,"EXIT")||!strcmp(message.str,"exit")){
			Log msg;
			msg.mtype = LOGIN;
			msg.pid = getpid();
			msg.flag = USER_OLOG;
			sign=msgsnd(mid,&msg,sizeof(Log)-sizeof(long),0);
			if(sign == -1){
				perror("client send:");
				return -1;
			}
			exit(0);
		}else{
			message.mtype=SEND;
			message.pid_from=getpid();
			strcpy(message.name_from,name);
			sign=msgsnd(mid,&message,sizeof(Chat)-sizeof(long),0);
			if(sign == -1){
				perror("client send:");
				return -1;
			}
		}
	}
}
int Read_from(){
	int sign;
	int i;
	Chat message;
	while(1){
		sign=msgrcv(mid,&message,sizeof(Chat)-sizeof(long),getpid(),0);
		if(sign == -1){
			perror("client receive:");
			return -1;
		}
		if(message.pid_to == -1)printf("对不起，该用户已离开！\nI:");
		else if(message.pid_to == -2)printf("对不起，格式错误！\nI:");
		else if(message.pid_to == getpid()){
			for(i = 0; i < strlen("I:"); i++)printf("\b");
			printf("%s:%s\nI:",message.name_from,message.str);
		}
		fflush(stdout);
	}
}
int main(){
	pthread_t thIDr, thIDw;

	mid=msgget(KEY,IPC_CREAT|0660);
	if(mid == -1){
		perror("client creat message:");
		return -1;
	}
	
	if(login() == -1){
		printf("Login Failed!\n");
		return -1;
	}
	printf("Login Success!\n");
	printf("My name is :%s\n", name);

	pthread_create(&thIDr, NULL,(void *)Write_to, NULL);
	pthread_create(&thIDw, NULL,(void *)Read_from, NULL);
	pthread_join(thIDr, NULL);
	pthread_join(thIDw, NULL);
	return 0;
}

