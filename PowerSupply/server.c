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
#include <sys/shm.h>
#include <sys/ipc.h>

#define POWER_THRESHOLD 5000
#define WARNING_THRESHOLD 4500
#define BACKLOG 10 /* Number of allowed connections */
#define BUFF_SIZE 8192
#define MAX_DEVICE 10

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
char use_mode[][10] = {"off", "normal", "limited"};
key_t key = 1234;
int shmid;

typedef struct {
	int pid;
	char name[50];
	int normal;
	int limit;
	int mode;
} device_t;

device_t *devices;

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
	int no;

	// get memory to store device infomation
	if ((devices = (device_t*) shmat(shmid, (void*) 0, 0)) == (void*)-1) {
		tprintf("shmat() failed\n");
		exit(1);
	}
	for (no = 0; no < MAX_DEVICE; no++) {
		if (devices[no].pid == 0)
			break;
	}
	while(1) {

		///////////////////
		// listen on tcp //
		///////////////////
		bytes_received = recv(conn_sock, recv_data, BUFF_SIZE-1, 0);
		if (bytes_received <= 0) {
			// if DISCONNECT
			tprintf("Device [%s] disconnected\n\n", devices[no].name);
			devices[no].pid = 0;
			strcpy(devices[no].name, "");
			devices[no].normal = 0;
			devices[no].limit = 0;
			devices[no].mode = 0;
			powerSupply_count--;
			kill(getpid(), SIGKILL);
			break;
		} else {
			recv_data[bytes_received] = '\0';
		}
		
		// Create device instance for first time receive
		if (is_first_message) {
			is_first_message = 0;
			sscanf(recv_data, "%[^|]|%d|%d", devices[no].name, &devices[no].normal, &devices[no].limit);
			devices[no].pid = getpid();
			devices[no].mode = 0;
			tprintf("--- Connected device info ---\n");
			tprintf("       name: %s\n", devices[no].name);
			tprintf("     normal: %dW\n", devices[no].normal);
			tprintf("      limit: %dW\n", devices[no].limit);
			tprintf("   use mode: %s\n", use_mode[devices[no].mode]);
			tprintf("-----------------------------\n\n");
			continue;
		}

		devices[no].mode = atoi(recv_data);
		tprintf("Device [%s] change mode to [%s]\n", devices[no].name, use_mode[devices[no].mode]);

	} // endwhile
} // end function

void connectMng_handle() {

	///////////////////////
	// Connect to client //
	///////////////////////

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

		if (powerSupply == 0) { 
			//child
			close(listen_sock);
			powerSupply_handle(conn_sock);
			close(conn_sock);
		} else { 
			//parent
			close(conn_sock);
			powerSupply_count++;
			tprintf("A device connected, connectMng forked process powerSupply --- pid: %d.\n", powerSupply);
		}
	} //end communication

	close(listen_sock);
} //end function

void elePowerCtrl_handle() {
	while(1) {

	} // endwhile
}

void powSupplyInfoAccess_handle() {
	while(1) {
		
	} // endwhile
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
	// Create shared memory for data //
	///////////////////////////////////

	if ((shmid = shmget(key, sizeof(device_t) * MAX_DEVICE, 0644 | IPC_CREAT)) < 0) {
		tprintf("shmge() failed\n");
		exit(1);
	}

	///////////////////////////////////
	// start child process in SERVER //
	///////////////////////////////////
	
	if ((connectMng = fork()) == 0) {
		connectMng_handle();
	} else if ((elePowerCtrl = fork()) == 0) {
		elePowerCtrl_handle();
	} else if ((powSupplyInfoAccess = fork()) == 0) {
		powSupplyInfoAccess_handle();
	} else {
		tprintf("SERVER forked process connectMng ------------------ pid: %d.\n", connectMng);
		tprintf("SERVER forked process elePowerCtrl ---------------- pid: %d.\n", elePowerCtrl);
		tprintf("SERVER forked process powSupplyInfoAccess --------- pid: %d.\n\n", powSupplyInfoAccess);
		waitpid(connectMng, NULL, 0);
		waitpid(elePowerCtrl, NULL, 0);
		waitpid(powSupplyInfoAccess, NULL, 0);
		tprintf("SERVER exited\n\n");
	}
	
	return 0;
}
