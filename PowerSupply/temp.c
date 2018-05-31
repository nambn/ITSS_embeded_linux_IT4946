/*
* @Author: Nam
* @Date:   2018-05-30 08:18:39
* @Last Modified by:   Nam
* @Last Modified time: 2018-05-31 23:06:09
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <signal.h>
#include <stddef.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(int argc, char const *argv[])
{
	key_t key = 1234;
	int shmid;
	char *num;

	if ((shmid = shmget(key, 100, 0644 | IPC_CREAT)) < 0) {
		printf("shmge() failed\n");
		exit(1);
	}
	if (fork() == 0) {
		if ((num = (char*) shmat(shmid, (void*) 0, 0)) == (void*)-1) {
			printf("shmat() failed\n");
			exit(1);
		}

		while(1) {
			printf("%s\n", num);
			sleep(5);
		}
	} else {
		if ((num = (char*) shmat(shmid, (void*) 0, 0)) == (void*)-1) {
			printf("shmat() failed\n");
			exit(1);
		}
		sprintf(num, "");
		while (1) {
			scanf("%s%*[\n]", num);
		}
	}
	return 0;
}