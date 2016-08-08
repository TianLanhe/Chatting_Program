#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <error.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#define LOGIN 100				//message type of logining or out of login
#define SEND 90					//message type of sending a message
#define LENGTH 10 				//max number of people to login
#define USER_LOG 1 				//the flag of logining
#define USER_OLOG 0 			//the flag of out of logining
#define KEY ((key_t)100) 		//id of message box
typedef struct _log{
	long mtype;
	int pid;
	char name[10];
	int flag;
}Log;
typedef struct _user{
	int userid;
	char username[10];
}User;
typedef struct _chat{
	long mtype;
	int pid_from;
	char name_from[10];
	int pid_to;
	char name_to[10];
	char str[100];
}Chat;
User userlink[LENGTH];
int mid;
void InitUser(User *userlink){
	int i;
	char *name[LENGTH]={"A","B","C","D","E",
				"F","G","H","I","J"};
	for(i=0;i<LENGTH;i++){
		userlink[i].userid=0;
		strcpy(userlink[i].username,name[i]);
	}
	mid=msgget(KEY,IPC_CREAT|0660);
	if(mid == -1){
		perror("server creat message:");
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
		if(i >= LENGTH){
			message.mtype=message.pid;
			message.pid=-1;
		}else{
			strcpy(message.name,userlink[i].username);
			message.mtype=message.pid;
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
	if(message.str[1] != ':'){
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
		for(i=0;i<LENGTH;i++){
			if(userlink[i].userid && !strcmp(message.str,userlink[i].username))break;
		}
		if(i < LENGTH){
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
			return strlen(ptr);
		}else{
			message.mtype=message.pid_from;
			message.pid_to=-1;
			sign=msgsnd(mid,&message,sizeof(Chat)-sizeof(long),0);
			if(sign == -1){
				perror("server receive:");
				return -1;
			}
		}
	}
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
	InitUser(userlink);
	pthread_create(&thIDr, NULL,(void *)userlogin, NULL);
	pthread_create(&thIDw, NULL,(void *)usermsg, NULL);
	pthread_join(thIDr, NULL);
	pthread_join(thIDw, NULL);
	return 0;
}
