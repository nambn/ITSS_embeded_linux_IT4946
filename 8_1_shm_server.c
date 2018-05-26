/*
* @Author: Nam
* @Date:   2018-05-15 04:55:02
* @Last Modified by:   Nam
* @Last Modified time: 2018-05-15 04:55:25
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMSIZE 20
#define KEYPATH "."
#define KEYCHAR 'a'

int main() {

	int shmid;
	char *shm, *ptr;
	char c;
	key_t key;

	key = ftok(KEYPATH, (int)KEYCHAR);

	if ((shmid = shmget(key, SHMSIZE*sizeof(char), IPC_CREAT|0666)) < 0) {
		perror("shmget");
		exit(1);
	}

	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
		perror("shmat");
		exit(1);
	}

	printf("Server has attached the shared memory...\n");
	sleep(1);
	printf("Data put into shared memory: ");

	ptr = shm;
	for (c = 'a'; c <= 'f'; c++) {
		*ptr++ = c;
		putchar(c);
	}
	*ptr = NULL;

	sleep(1);
	printf("\nPress anykey to delete data and release memory...\n");
	getchar();

	if (shmdt((void*)shm) == -1) {
		perror("shmdt");
		exit(1);
	}

	if (shmctl(shmid, IPC_RMID, 0) == -1) {
		perror("shmctl");
		exit(1);
	}
}
