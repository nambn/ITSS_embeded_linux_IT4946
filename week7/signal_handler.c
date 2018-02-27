/**
 * @Author: nam
 * @Date:   2018-02-27T10:41:46+07:00
 * @Email:  pegasis2012@gmail.com
 * @Last modified by:   nam
 * @Last modified time: 2018-02-27T10:48:30+07:00
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define SIG_OK 0
#define SIG_NG -1
#define LOOP_ON 0
#define LOOP_OFF 1

int loopFlag = LOOP_ON;

// fuction to handle each signal
void sigHandleSIGALRM (int sigNo);
void sigHandleSIGINT (int sigNo);

int main(int argc, char const *argv[]) {
	if (argc != 2) {
		printf("Argument should be 1 number\n");
		return -1;
	}

	signal(SIGALRM, sigHandleSIGALRM);
	signal(SIGINT, sigHandleSIGINT);

	printf("Cat is attacking! Ctrl - C quick!!!\n");

	// call alarm signal SIGALRM
	alarm(atoi(argv[1]));

	// infinity loop
	while (loopFlag == LOOP_ON) {
		sleep(1);
		printf(".\n");
	}
}

void sigHandleSIGALRM (int sigNo)
{
	printf("CAT ATTACK!!!\n");
	usleep(500000);
	printf("You died.\n");
	exit(SIG_OK);
}

void sigHandleSIGINT (int sigNo)
{
	printf("Cat died.\n");
	exit(SIG_OK);
}
