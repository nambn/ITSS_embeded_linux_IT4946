CC = gcc
CFLAG = -std=c99

server: server.c pipe.c
	$(CC) $(CFLAG) -o server server.c pipe.c

client: client.c
	$(CC) $(CFLAG) -o client client.c