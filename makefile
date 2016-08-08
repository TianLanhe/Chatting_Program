program:client server

client:chat_client.c
	gcc -o client chat_client.c -lpthread
server:chat_server.c
	gcc -o server chat_client.c -lpthread
clean:
	rm server client