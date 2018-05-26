/*
* @Author: Nam
* @Date:   2018-04-28 18:39:17
* @Last Modified by:   Nam
* @Last Modified time: 2018-05-15 04:56:42
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

typedef struct timeval wallclock_t;

void time_mark (wallclock_t *const tp)
{
	gettimeofday(tp, NULL);
}

double time_dur (wallclock_t *const tp)
{
	struct timeval now;
	gettimeofday (&now, NULL);

	return difftime(now.tv_sec, tp->tv_sec) + ((double)now.tv_usec - (double)tp->tv_usec) / 1000000.0;
}

int main(int argc, char const *argv[]) {
	wallclock_t t;

	printf("Sleep %d second(s) using sleep\n", atoi(argv[1]));
	time_mark(&t);
	sleep(atoi(argv[1]));
	printf("Actual time: %f\n\n", time_dur(&t));

	printf("Sleep %d second(s) using usleep\n", atoi(argv[1]));
	time_mark(&t);
	usleep(atoi(argv[1])*1000000);
	printf("Actual time: %f\n\n", time_dur(&t));

	printf("Sleep %d second(s) using nanosleep\n", atoi(argv[1]));
	struct timespec tim, tim2;
	tim.tv_sec = atoi(argv[1]);
	tim.tv_nsec = 0L;
	time_mark(&t);
	nanosleep(&tim, &tim2);
	printf("Actual time: %f\n\n", time_dur(&t));

	return 0;
}
