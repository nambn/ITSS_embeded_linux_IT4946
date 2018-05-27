CC = gcc
CFLAG = 

all: server client

server: server.c single-linked-list-lib.c
	$(CC) $(CFLAG) -o server server.c single-linked-list-lib.c

client: client.c
	$(CC) $(CFLAG) -o client client.c