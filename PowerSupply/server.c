#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "multi-process.h"

#define BACKLOG 10 /* Number of allowed connections */
#define BUFF_SIZE 1024
int _server_port;

void connectMng_handle() {
	tprintf("SERVER forked process connectMng ------------ pid: %d.\n", getpid());

	int listen_sock, conn_sock;
	char recv_data[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server;
	struct sockaddr_in client;
	int sin_size;

    //Step 1: Construct a TCP socket to listen connection request
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket() failed\n");
		exit(1);
	}

    //Step 2: Bind address to socket
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(_server_port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(listen_sock, (struct sockaddr *)&server, sizeof(server)) == -1) {
		perror("bind() failed\n");
		exit(1);
	}

    //Step 3: Listen request from client
	if (listen(listen_sock, BACKLOG) == -1) {
		perror("listen() failed\n");
		exit(1);
	}

	//Step 4: Communicate with client
	while (1) {
        //accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size)) == -1) {
			perror("accept() failed\n");
			exit(1);
		}

        printf("A device connected\n");

        close(conn_sock);
    }
}

void powerSupply_handle() {
	tprintf("SERVER forked process powerSupply ----------- pid: %d.\n", getpid());
}

void elePowerCtrl_handle() {
	tprintf("SERVER forked process elePowerCtrl ---------- pid: %d.\n", getpid());
}

void powSupplyInfoAccess_handle() {
	tprintf("SERVER forked process powSupplyInfoAccess --- pid: %d.\n", getpid());
}

void logWrite_handle() {
	tprintf("SERVER forked process logWrite -------------- pid: %d.\n", getpid());
}

int main(int argc, char const *argv[])
{
	if (argc != 2) {    
		fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
		exit(1);
	}
	printf("SERVER start, PID is %d.\n", getpid());

	_server_port = atoi(argv[1]);

	///////////////////////////////////
	// start child process in SERVER //
	///////////////////////////////////
	pid_t connectMng, elePowerCtrl, powSupplyInfoAccess, logWrite;

	connectMng = createProcess(connectMng_handle);
	elePowerCtrl = createProcess(elePowerCtrl_handle);
	powSupplyInfoAccess = createProcess(powSupplyInfoAccess_handle);
	logWrite = createProcess(logWrite_handle);

	if (isOutsideAllProcess()) {
		waitAllProcess();
		tprintf("SERVER exited\n\n");
	}
	else if(!processError()) {
		tprintf("Everything was gone without error.\n");
	}

	return 0;
}