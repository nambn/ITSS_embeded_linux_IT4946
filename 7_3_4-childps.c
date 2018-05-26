/*
* @Author: Nam
* @Date:   2018-04-28 18:39:17
* @Last Modified by:   Nam
* @Last Modified time: 2018-05-15 04:56:57
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define PROC_OK 0
#define PROC_NG -1

int main(int argc, char const *argv[]) {

	printf("main            :start\n");

	char *args[] = {"ls", "-l", NULL};
	int iRet = 0;
	pid_t pid = 0;
	int status = 0;

	switch (pid = fork()) {
		case -1: {
			perror("processGenerate fork");
			return 1;
		}

		case 0: {
			iRet = execv("/bin/ls", args);

			printf("execv ret = %d\n", iRet);

			if (iRet == PROC_NG) {
				return 1;
			}
		}

		default: {
			printf("Parent Process ChildProcPid = %d\n", pid);

			pid = wait(&status);
			printf("pid = %d, status = %d\n", pid, status);
			break;
		}
	}

	printf("main            :end\n");
	return 0;
}
