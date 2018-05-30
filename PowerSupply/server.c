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
#define MAX_LOG_DEVICE 100
#define MESSAGE_MAX 255

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
key_t key_d = 1234, key_m = 5678; //device, message
int shmid_d, shmid_m; //device, message
FILE *log_server;
FILE *log_client[MAX_LOG_DEVICE];
int log_client_count = 0;
char *message; //ipc

// device struct
typedef struct {
	int pid;
	char name[50];
	int normal;
	int limit;
	int mode;
} device_t;
device_t *devices;

// typedef struct {
// 	int 
// } system_info_t;

int tprintf(const char* fmt, ...){
	va_list args;
	struct tm * tstruct;
	time_t tsec = time(NULL);
	tstruct = localtime (&tsec);
	printf("%02d:%02d:%02d: %5d| ", tstruct->tm_hour, tstruct->tm_min, tstruct->tm_sec, getpid());
	va_start(args, fmt);
	return vprintf(fmt, args);
}

void sigHandleSIGINT () {
	fclose(log_server);
	kill(0, SIGKILL);
	exit(0);
}

void powerSupply_handle(int conn_sock) {
	int is_first_message = 1;
	int no;

	//////////////////////////////
	// connect to shared memory //
	//////////////////////////////
	// for devices
	if ((devices = (device_t*) shmat(shmid_d, (void*) 0, 0)) == (void*)-1) {
		tprintf("shmat() failed\n");
		exit(1);
	}
	// search for avaible slot, assign it to no.
	for (no = 0; no < MAX_DEVICE; no++) {
		if (devices[no].pid == 0)
			break;
	}

	// for ipc
	char *mess;
	if ((mess = (char*) shmat(shmid_m, (void*) 0, 0)) == (void*)-1) {
		tprintf("shmat() failed\n");
		exit(1);
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
			sprintf(mess, "1 | A device connected\n");
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

		// if 11-th device connect to SERVER
		if (powerSupply_count == MAX_DEVICE) {
			char re = '9';
			if ((bytes_sent = send(conn_sock, &re, 1, 0)) <= 0)
				tprintf("send() failed\n");
			close(conn_sock);
			break;
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
			tprintf("A device connected, connectMng forked new process powerSupply --- pid: %d.\n", powerSupply);
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
	// header == 1 -> write server log

	//////////////////////
	// Create sever log //
	//////////////////////
	char file_name[255];
	time_t t = time(NULL);
	struct tm * now = localtime(&t);
	strftime(file_name, sizeof(file_name), "log/server_%Y-%m-%d_%H:%M:%S.txt", now);
	log_server = fopen(file_name, "w");
	tprintf("Log server started, file is %s\n", file_name);

	//////////////////////////////
	// Connect to shared memory //
	//////////////////////////////
	// for ipc
	char *mess;
	if ((mess = (char*) shmat(shmid_m, (void*) 0, 0)) == (void*)-1) {
		tprintf("shmat() failed\n");
		exit(1);
	}

	// Write log if requested
	while (1) {
		if (strlen(mess) && (mess[0] == '1')){
			char buff[MESSAGE_MAX];
			//extract from mess
			memcpy(buff, &mess[4], strlen(mess) - 4);
			// get time now
			char log_time[20];
			strftime(log_time, sizeof(log_time), "%Y-%m-%d_%H:%M:%S", now);
			// write log
			fprintf(log_server, "%s | %s", log_time, buff);
			// delete mess in shared memory
			memset(mess, '\0', MESSAGE_MAX);
		}
	}
}

int main(int argc, char const *argv[])
{
	if (argc != 2) {    
		fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
		exit(1);
	}
	server_port = atoi(argv[1]);
	printf("SERVER start, PID is %d.\n", getpid());

	//////////////////////////////////////
	// Create shared memory for devices //
	//////////////////////////////////////
	if ((shmid_d = shmget(key_d, sizeof(device_t) * MAX_DEVICE, 0644 | IPC_CREAT)) < 0) {
		tprintf("shmge() failed\n");
		exit(1);
	}

	if ((devices = (device_t*) shmat(shmid_d, (void*) 0, 0)) == (void*)-1) {
		tprintf("shmat() failed\n");
		exit(1);
	}

	// Init data for shared memory
	int i;
	for (i = 0; i < MAX_DEVICE; i++) {
		devices[i].pid = 0;
		strcpy(devices[i].name, "");
		devices[i].normal = 0;
		devices[i].limit = 0;
		devices[i].mode = 0;
	}

	//////////////////////////////////
	// Create shared memory for IPC //
	//////////////////////////////////
	// convention:
	// <header (int)> | <body (string)>
	// header = 0 -> no read
	// header = 1 -> logWrite_handle
	if ((shmid_m = shmget(key_m, MESSAGE_MAX, 0644 | IPC_CREAT)) < 0) {
		tprintf("shmge() failed\n");
		exit(1);
	}

	// get da memory
	char *mess;
	if ((mess = (char*) shmat(shmid_m, (void*) 0, 0)) == (void*)-1) {
		tprintf("shmat() failed\n");
		exit(1);
	}

	memset(mess, '\0', MESSAGE_MAX);

	///////////////////
	// Handle Ctrl-C //
	///////////////////
	signal(SIGINT, sigHandleSIGINT);

	///////////////////////////////////
	// start child process in SERVER //
	///////////////////////////////////
	if ((connectMng = fork()) == 0) {
		connectMng_handle();
	} else if ((elePowerCtrl = fork()) == 0) {
		elePowerCtrl_handle();
	} else if ((powSupplyInfoAccess = fork()) == 0) {
		powSupplyInfoAccess_handle();
	} else if ((logWrite = fork()) == 0) {
		logWrite_handle();
	} else {
		tprintf("SERVER forked new process connectMng ------------------ pid: %d.\n", connectMng);
		tprintf("SERVER forked new process elePowerCtrl ---------------- pid: %d.\n", elePowerCtrl);
		tprintf("SERVER forked new process powSupplyInfoAccess --------- pid: %d.\n", powSupplyInfoAccess);
		tprintf("SERVER forked new process logWrite -------------------- pid: %d.\n\n", logWrite);
		waitpid(connectMng, NULL, 0);
		waitpid(elePowerCtrl, NULL, 0);
		waitpid(powSupplyInfoAccess, NULL, 0);
		waitpid(logWrite, NULL, 0);
		tprintf("SERVER exited\n\n");
	}

	return 0;
}
