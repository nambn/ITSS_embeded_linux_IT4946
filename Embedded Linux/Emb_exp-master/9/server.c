#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#define SERVER_ADDR "localhost"
#define SERVER_PORT 5678

void doprocessing(int iClientSock);

int main()
{
  int iSock = 0,pid;
  int iClientSock = 0;
  struct sockaddr_in serverData = {0,0,0,""};
  struct sockaddr_in clientData = {0,0,0,""};
  int iClientSize = 0;
  int i,n;

  char buffer[256];
  
  // Creation of socket
  if( (iSock=socket(AF_INET, SOCK_STREAM,0)) == -1 ){
    perror("socket");
    exit(1);
  }

  /* Initialize socket structure */
  bzero((char *) &serverData, sizeof(serverData));
   
  serverData.sin_family = AF_INET;
  serverData.sin_addr.s_addr = INADDR_ANY;
  serverData.sin_port = htons(SERVER_PORT);

  // Identify the socket
  if ( bind(iSock, (struct sockaddr*)&serverData, sizeof(serverData)) == -1 ){
    perror("bind");
    shutdown(iSock,SHUT_RDWR);
    close(iSock);
    exit(1);
  }
  
  // Connection setup
  if ( listen(iSock,1) == -1 ){
    perror("listen");
    shutdown(iSock,SHUT_RDWR);
    close(iSock);
    exit(1);
  }
  
  iClientSize=sizeof(clientData);

  while(1) {
    // Connection standby
    if ( (iClientSock=accept(iSock, (struct sockaddr*)&clientData, &iClientSize)) == -1 ){
      perror("accept");
      shutdown(iSock,SHUT_RDWR);
      close(iSock);
      exit(1);
    }
    /* Create child process */
    pid = fork();
    if (pid < 0)
      {
	perror("ERROR on fork");
	exit(1);
      }
      
    if (pid == 0)
      {
	/* This is the client process */
	//shutdown(iSock,SHUT_RDWR);
	close(iSock);
	doprocessing(iClientSock);
	exit(0);
      }
    else
      {
	//shutdown(iClientSock,SHUT_RDWR);
	close(iClientSock);
      }
  
  }

  //end of connection
  //shutdown(iClientSock,SHUT_RDWR);
  //close(iClientSock);
  //shutdown(iSock,SHUT_RDWR);
  //close(iSock);
  return 0;
}

void doprocessing(int iClientSock) {
  int i,n;
  char buffer[256];

  while(1) {

    /* If connection is established then start communicating */
    bzero(buffer,256);
    n = read( iClientSock,buffer,255 );
   
    if (n < 0)
      {
	perror("ERROR reading from socket");
	exit(1);
      }
   
    printf("msg from client: %s\n",buffer);

    if(strncmp(buffer,"quit",4)==0) return;
   
    for(i = 0; i < strlen(buffer); i++) buffer[i] = tolower(buffer[i]);
 
    /* Write a response to the client */
    n = write(iClientSock,buffer,strlen(buffer));
   
    if (n < 0)
      {
	perror("ERROR writing to socket");
	exit(1);
      }

  }
}

  
