#include <stdio.h> /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define PORT 5550 /* Port that will be opened */
#define BACKLOG 2 /* Number of allowed connections */
#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
    if (argc != 2)
	{
		printf("Wrong arguments!\n");
		exit(-1);
    }
    
    int listen_sock, conn_sock; /* file descriptors */
    char recv_data[BUFF_SIZE];
    int bytes_sent, bytes_received;
    struct sockaddr_in server; /* server's address information */
    struct sockaddr_in client; /* client's address information */
    int sin_size;

    //Step 1: Construct a TCP socket to listen connection request
    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    { /* calls socket() */
        perror("\nError: ");
        return 0;
    }

    //Step 2: Bind address to socket
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));              /* Remember htons() from "Conversions" section? =) */
    server.sin_addr.s_addr = htonl(INADDR_ANY); /* INADDR_ANY puts your IP address automatically */
    if (bind(listen_sock, (struct sockaddr *)&server, sizeof(server)) == -1)
    { /* calls bind() */
        perror("\nError: ");
        return 0;
    }

    //Step 3: Listen request from client
    if (listen(listen_sock, BACKLOG) == -1)
    { /* calls listen() */
        perror("\nError: ");
        return 0;
    }

    //Step 4: Communicate with client
    while (1)
    {
        //accept request
        sin_size = sizeof(struct sockaddr_in);
        if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size)) == -1)
            perror("\nError: ");

        printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */

        //start conversation
        while (1)
        {
            //receives message from client
            bytes_received = recv(conn_sock, recv_data, BUFF_SIZE - 1, 0); //blocking
            if (bytes_received <= 0)
            {
                printf("\nConnection closed");
                break;
            }
            else
            {
                recv_data[bytes_received] = '\0';
                printf("\nReceive: %s \n", recv_data);
            }


            char num[1001],alp[1001];
            memset(num, '\0', (strlen(num) + 1));
            memset(alp, '\0', (strlen(alp) + 1));
            int i,inum=0,ialp=0;
            for(i=0;i<strlen(recv_data)-1;i++)
            {
                if(isdigit(recv_data[i])!=0){
                    num[inum++]=recv_data[i];
                } else if (isalpha(recv_data[i])){
                    alp[ialp++]=recv_data[i];
                } else {
                    char err[]="Error";
                    bytes_sent = send(conn_sock, err, strlen(err)+1, 0); /* send to the client welcome message */
                    if (bytes_sent <= 0)
                    {
                        printf("\nConnection closed");
                        break;
                    }
                }
            }
            char re[1001];
            sprintf(re,"%s\n%s",num,alp);

            //echo to client
            bytes_sent = send(conn_sock, re, strlen(re)+1, 0); /* send to the client welcome message */
            if (bytes_sent <= 0)
            {
                printf("\nConnection closed");
                break;
            }
        } //end conversation
        close(conn_sock);
    }

    close(listen_sock);
    return 0;
}