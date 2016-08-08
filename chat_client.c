#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <error.h>
#include <string.h>
#define LOGIN 100				//message type of logining or out of login
#define SEND 90					//message type of sending a message
#define USER_LOG 1 				//the flag of logining
#define USER_OLOG 0 			//the flag of out of logining
#define KEY ((key_t)100) 		//id of message box
typedef struct _log{
	long mtype;
	int pid;
	char name[10];
	int flag;
}Log;
typedef struct _chat{
	long mtype;
	int pid_from;
	char name_from[10];
	int pid_to;
	char name_to[10];
	char str[100];
}Chat;
char name[11];
int mid;
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
		message.str[strlen(message.str)-1]='\0';
		if(!strcmp(message.str,"quit")){
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
		return 1;
	}
	printf("Login Success!\n");
	printf("My name is :%s\n", name);

	pthread_create(&thIDr, NULL,(void *)Write_to, NULL);
	pthread_create(&thIDw, NULL,(void *)Read_from, NULL);
	pthread_join(thIDr, NULL);
	pthread_join(thIDw, NULL);
	return 0;
}
