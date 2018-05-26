#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFF_SIZE 8192

int main(int argc, char const *argv[])
{
	if(argc != 3) {
		fprintf(stderr,"Usage: %s <Server IP> <Echo Port>\n",argv[0]);
		exit(1);
	}

	int client_sock;
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr;
	int msg_len, bytes_sent, bytes_received;

    // Step 1: Construct socket
	client_sock = socket(AF_INET, SOCK_STREAM, 0);

    // Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // Step 3: Request to connect server
	if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
	{
		perror("accept() failed\n");
		return 0;
	}

	// Step 4: Communicate with server
	while(1) {
		
	}
	
    // Step 5: Close socket
	close(client_sock);
	return 0;
}