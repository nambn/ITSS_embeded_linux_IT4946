#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5550
#define BUFF_SIZE 8192

int main(int argc, char *argv[])
{
    if (argc != 3)
	{
		printf("Wrong arguments!\n");
		exit(-1);
    }

    int client_sock;
    char buff[BUFF_SIZE];
    struct sockaddr_in server_addr; /* server's address information */
    int msg_len, bytes_sent, bytes_received;

    //Step 1: Construct socket
    client_sock = socket(AF_INET, SOCK_STREAM, 0);

    //Step 2: Specify server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    //Step 3: Request to connect server
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
    {
        printf("\nError!Can not connect to sever! Client exit imediately! \n");
        return 0;
    }

    //Step 4: Communicate with server
    while (1)
    {
        //send message
        printf("\nInsert string to send:");
        memset(buff, '\0', (strlen(buff) + 1));
        fgets(buff, BUFF_SIZE, stdin);
        msg_len = strlen(buff);
        if (msg_len == 0)
            break;

        bytes_sent = send(client_sock, buff, msg_len, 0);
        if (bytes_sent <= 0)
        {
            printf("\nConnection closed!\n");
            break;
        }

        //receive echo reply
        bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);
        if (bytes_received <= 0)
        {
            printf("\nError!Cannot receive data from sever!\n");
            break;
        }

        buff[bytes_received] = '\0';
        printf("Reply from server: %s", buff);
    }

    //Step 4: Close socket
    close(client_sock);
    return 0;
}