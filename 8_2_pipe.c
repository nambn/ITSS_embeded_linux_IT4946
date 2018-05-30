#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(void)
{
	int pfds[2];
	char s[50];
	pipe(pfds);
	int status;

	if (fork()) {
		printf("--- PARENT ---\n");
		printf("Option: ");
		fgets(s, 50, stdin);
		s[strcspn(s,"\n")] = '\0';
		write(pfds[1], s, 50);
		close(pfds[1]);
		wait(&status);
		printf("--- PARENT ---\n");
		close(0);
		dup(pfds[0]);
		execlp("wc", "wc", "-l", NULL);
	} else {
		read(pfds[0], s, 50);
		close(pfds[0]);
		printf("--- CHILD ---\n");
		close(1);
		dup(pfds[1]);
		execlp("ls", "ls", s, NULL);
	}

	return 0;
}