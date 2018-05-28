#include <stdio.h>      
#include <sys/socket.h> 
#include <arpa/inet.h>  
#include <stdlib.h>     
#include <string.h>     
#include <unistd.h>     
#include <time.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#define MAXPENDING 10    /* Maximum outstanding connection requests */
#define MAXBUFSIZE 1024
#define MAXQUEST 10
#define PROC_OK  0
#define PROC_NG  -1


typedef struct {
	int num;
	char question[1024];
	char a[512];
	char b[512];
	char c[512];
	char d[512];
	char answer;
}question;


typedef struct {
	char user[20];
	char pass[20];
}acc;

void doIt(int clntSocket);
void salesMng(int clntSocket,char buffer[MAXBUFSIZE]);
void equipInfoAccess(char buffer[MAXBUFSIZE]);

int main(int argc, char *argv[])
{
	int servSock;                    
	int clntSock;
	struct sockaddr_in servAddr; 
	struct sockaddr_in clntAddr; 
	unsigned short servPort;     
	unsigned int clntLen;        
	char sendBuffer[MAXBUFSIZE];
	pid_t processID;                 
	unsigned int childProcCount = 0; 
	acc account[100];

	if (argc != 2)
	{    
		fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
		exit(1);
	}

	servPort = atoi(argv[1]);  

	if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		perror("socket() failed");
		exit(1);
	}
	memset(&servAddr, 0, sizeof(servAddr));   
	servAddr.sin_family = AF_INET;               
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servAddr.sin_port =htons(servPort);      
	
	if (bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
	{
		perror("bind() failed");
		exit(1);
	}
	
	if (listen(servSock, MAXPENDING) < 0)
	{
		perror("listen() failed");
		exit(1);
	}
	
	while(1)
	{    
		clntLen = sizeof(clntAddr);
		if ((clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntLen)) < 0){	
			perror("accept() failed");
			exit(1);
		}    
		if ((processID = fork()) < 0){
			perror("accept() failed");
			exit(1);
		} 
		else if (processID == 0){  
			/* If this is the child process */      
			close(servSock);   
			/* Child closes parent socket */
			printf("Handling client %s\n", inet_ntoa(clntAddr.sin_addr));
			doIt(clntSock);
			exit(0);           
		}

		printf("with child process: %d\n", (int) processID);
		close(clntSock);       /* Parent closes child socket descriptor */
		childProcCount++;      /* Increment number of outstanding child processes */

		while (childProcCount) { 
			/* Clean up all zombies */        
			processID = waitpid((pid_t) -1, NULL, WNOHANG);  /* Non-blocking wait */
			if (processID < 0) {  
				/* waitpid() error? */
				perror("waitpid() failed");
				exit(1);
			}
			else if (processID == 0)  
				/* No zombie to wait on */
				break;
			else
				/* Cleaned up after a child */
				childProcCount--; 
		}
	}
}




void doIt(int clntSocket)
{   
	char buffer[MAXBUFSIZE];        
	int recvMsgSize;                    

	if ((recvMsgSize = recv(clntSocket, buffer, MAXBUFSIZE, 0)) < 0)
	{
		perror("recv() failed");
		exit(1);
	}

	while (recvMsgSize > 0)
	{      

		buffer[recvMsgSize]='\0';
		if(strcmp(buffer,"quit")==0||strcmp(buffer,"Quit")==0){      
			close(clntSocket);            
		}

		char *args[] = {"ls", "-l", NULL};
		int iRet = 0;
		pid_t pid = 0;
		int  i, status = 0;

		// Creating child process
		pid = fork();
		if(pid == 0) pid = fork();
		switch (pid){
			case -1:
			// Process creating error processing
			perror("processGenerate fork");
			exit(1);

			case 0:
			// Child process processing
			salesMng(clntSocket,buffer);
			iRet = execv("/bin/ls", args);

			// It doesn't pass when execv succeeds. 
			printf("execv ret = %d\n", iRet);
			if(iRet == PROC_NG) {
				exit(1);
			}

			default:
			// Parent process processing
			// Wait for the state transition of the child process. 
			pid = wait(&status);
			printf("pid=%d,status=%d\n", pid, status);
			break;
		}
		
		if ((recvMsgSize = recv(clntSocket, buffer, MAXBUFSIZE, 0)) < 0){
			perror("recv() failed");      
			exit(1);
		}
	}  
}



void salesMng(int clntSocket,char buffer[MAXBUFSIZE])
{
	char *temp,user[20];
	acc accId;  
	question quest[MAXQUEST];
	int i;
	printf("%s\n", buffer);
	temp = strtok(buffer," ");

	printf("ad %s\n", temp);

	if(strcmp(temp,"choice")==0)
	{
		temp=strtok(NULL," ");
		printf(" %s\n", temp);

		equipInfoAccess(temp);
	} else if (strcmp(temp, "over") == 0)
	{
		temp= strtok(NULL, " ");

	}

}

void equipInfoAccess(char buffer[MAXBUFSIZE])
{
	time_t rawtime;
	struct tm *info;

	time( &rawtime );

	info = localtime( &rawtime );

	FILE *fout;
	fout = fopen("result.txt","a");
	if(fout==NULL)
	{
		printf("Cannot open file result.txt\n");
		return;
	}
	//printf("%s \n", buffer);
	fprintf(fout,"%s %s \n",asctime(info),buffer);    
	fclose(fout);

}


int sharedMemory() {
	int *shm, shmid, k;
	shmid = shmget(IPC_PRIVATE,128,IPC_CREAT|0666);
	shm = (int*) shmat(shmid,0,0);
	if(fork()==0) { 
		/*child*/
		shm[0]=111;
		shm[1]=999;
		sleep(3);
		printf("Process %d reads: Sum = %d", getpid(),shm[2]);
		shmdt((void *)shm);
		shmctl(shmid, IPC_RMID, (struct shmid_ds *)0);

	}
	else {    
		/*parent*/
		sleep(1);
		printf("Process %d writes to shared memory ...\n", getpid());
		shm[2]=shm[0]+shm[1];
		shmdt((void *)shm);
	}
	return(0);
} 
