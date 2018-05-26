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
#include "pipe.h"

#define BUFF_SIZE 1024

void connectMng_handle() {
	tprintf("SERVER forked process connectMng ------------ pid: %d.\n", getpid());
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

	int svPort = atoi(argv[1]);
	int svSock;
	struct sockaddr_in svAddr; 
	struct sockaddr_in clAddr; 

	// Step 1: Construct a UDP socket
	if ((svSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket() failed\n");
		exit(1);
	} 

	// Step 2: Bind address to socket
	svAddr.sin_family = AF_INET;
	svAddr.sin_port = htons(servPort);
	svAddr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(svAddr.sin_zero), 8);

	if (bind(svSock, (struct sockaddr *) &svAddr, sizeof(svAddr)) < 0) {
		perror("bind() failed\n");
		exit(1);
	}

	// Step 3: Communicate with clients
	while(1) {
		
	} // endwhile


	printf("SERVER start, PID is %d.\n", getpid());

	///////////////////////////////////
	// start child process in SERVER //
	///////////////////////////////////
	pid_t connectMng, powerSupply, elePowerCtrl, powSupplyInfoAccess, logWrite;

	connectMng = createProcess(connectMng_handle);
	powerSupply = createProcess(powerSupply_handle);
	elePowerCtrl = createProcess(elePowerCtrl_handle);
	powSupplyInfoAccess = createProcess(powSupplyInfoAccess_handle);
	logWrite = createProcess(logWrite_handle);

	if (isOutsideAllProcess()) {
		waitAllProcess();
		tprintf("SERVER exited\n\n");
	}
	else if(!processError()) {
		tprintf("Everything was gone without error.\n");
    } // endif

    return 0;
}