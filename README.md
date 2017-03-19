# Chatting_Program
聊天程序(消息队列与Socket)

## 说明
* message (消息队列私人聊天室)
	* chat.h
	* chat_clinet.c
	* chat_server.c
	* makefile
* socket (Socket多人聊天室)
	* chat.h
	* chat.c
	* client.c
	* server.c
	* makefile

## 私人聊天室
同一主机不同进程间通信，采用消息队列。服务器开启两个线程，一个处理消息，一个处理登录与登出请求。客户端向公共消息队列发送登录请求，服务器分配用户名称给客户端，并将其登记到用户列表中。客户端登录成功后，开启输入和读取两个线程，读取线程尝试向消息队列读取消息，输入线程读取屏幕输入并发送到公共消息队列。服务器接收到消息后解析其格式并搜索用户列表查找目标用户，若格式错误或不存在用户，则向源用户回送失败信息，否则将消息发给目标用户，并向源用户回送成功信息。

## 多人聊天室
利用TCP/IP协议跨主机通信，采用Socket。服务器采用fork子进程的方式为客户端服务。
