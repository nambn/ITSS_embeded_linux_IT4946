//////////////////////////////////////////////////////////
//  \    /\   Simple power supply program - server side //
//   )  ( ')                                            //
//  (  /  )                                             //
//   \(__)|                                          N. //
//////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#define POWER_THRESHOLD 5000
#define WARNING_THRESHOLD 4500
#define BACKLOG 10 /* Number of allowed connections */
#define BUFF_SIZE 8192

////////////////////
// Variables list //
////////////////////
int server_port;
pid_t connectMng, powerSupply, elePowerCtrl, powSupplyInfoAccess, logWrite;
int powerSupply_count = 0;
int listen_sock, conn_sock;
char recv_data[BUFF_SIZE];
int bytes_sent, bytes_received;
struct sockaddr_in server;
struct sockaddr_in client;
int sin_size;

int tprintf(const char* fmt, ...){
	va_list args;
	struct tm * tstruct;
	time_t tsec = time(NULL);
	tstruct = localtime (&tsec);
	printf("%02d:%02d:%02d: %5d| ", tstruct->tm_hour, tstruct->tm_min, tstruct->tm_sec, getpid());
	va_start(args, fmt);
	return vprintf(fmt, args);
}

void powerSupply_handle(int conn_sock) {
	int is_first_message = 1;
	char name[50];
	int mode_2;
	int mode_3;
	char use_mode[3][10] = {"off", "normal", "limited"};

	while(1) {
		bytes_received = recv(conn_sock, recv_data, BUFF_SIZE-1, 0);
		if (bytes_received <= 0) {
			tprintf("Device [%s] disconnected\n", name);
			kill(getpid(), SIGKILL);
			break;
		} else {
			recv_data[bytes_received] = '\0';
		}
		
		if (is_first_message) {
			is_first_message = 0;
			sscanf(recv_data, "%[^|]|%d|%d", name, &mode_2, &mode_3);
			tprintf("--- Connected device info ---\n");
			tprintf("       name: %s\n", name);
			tprintf("     normal: %dW\n", mode_2);
			tprintf("      limit: %dW\n", mode_3);
			tprintf("   use mode: %s\n", use_mode[0]);
			tprintf("-----------------------------\n\n");
		}

	} // endwhile
}

void connectMng_handle() {
    //Step 1: Construct a TCP socket to listen connection request
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		tprintf("socket() failed\n");
		exit(1);
	}

    //Step 2: Bind address to socket
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(server_port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(listen_sock, (struct sockaddr *)&server, sizeof(server)) == -1) {
		tprintf("bind() failed\n");
		exit(1);
	}

    //Step 3: Listen request from client
	if (listen(listen_sock, BACKLOG) == -1) {
		tprintf("listen() failed\n");
		exit(1);
	}

	//Step 4: Communicate with client
	while (1) {
        //accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size)) == -1) {
			tprintf("accept() failed\n");
			continue;
		}


		if ((powerSupply = fork()) < 0){
			tprintf("powerSupply fork() failed\n");
			continue;
		} 

		if (powerSupply == 0){
			// powerSupply ps
			close(listen_sock);
			powerSupply_handle(conn_sock);
			close(conn_sock);
		} else {
			// connectMng ps
			close(conn_sock);
			powerSupply_count++;
			tprintf("\nA device connected, connectMng forked process powerSupply(%d) --- pid: %d.\n", powerSupply_count, powerSupply);
		} // end parent/child branch
	} //end communication

	close(listen_sock);
} //end function

void elePowerCtrl_handle() {
}

void powSupplyInfoAccess_handle() {
}

void logWrite_handle() {
}

int main(int argc, char const *argv[])
{
	if (argc != 2) {    
		fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
		exit(1);
	}
	server_port = atoi(argv[1]);
	printf("SERVER start, PID is %d.\n", getpid());

	///////////////////////////////////
	// start child process in SERVER //
	///////////////////////////////////
	
	connectMng = fork();

	if (connectMng > 0) {
		tprintf("SERVER forked process connectMng ------------------------------- pid: %d.\n", connectMng);
		waitpid(connectMng, NULL, 0);
		tprintf("SERVER exited\n\n");
	} else if (connectMng == 0) {
		connectMng_handle();
	}

	return 0;
}