#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/fcntl.h>

#define FIFO_FILE_PATH "fifofilepath"
#define READ_FILE_PATH "readfilepath"
#define BUFF_SIZE 256

int func()
{
	int fifoFd, readFd, size;
	char buf[BUFF_SIZE];

	if (mkfifo(FIFO_FILE_PATH, 0666) == -1) {
		perror("mkfifo");
	}

	if ((fifoFd = open(FIFO_FILE_PATH, O_WRONLY)) == -1) {
		perror("fifofile open");
		exit(1);
	}

	if ((readFd = open(READ_FILE_PATH, O_RDONLY)) == -1) {
		perror("readfile open");
		exit(1);
	}

	while ((size = read(readFd, buf, BUFF_SIZE)) != 0) {
		write(fifoFd, buf, BUFF_SIZE);
	}

	close(readFd);
	close(fifoFd);

	unlink(FIFO_FILE_PATH);

	return 0;
}
