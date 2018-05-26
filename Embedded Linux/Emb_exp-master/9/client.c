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

int main()
{
  int iSock = 0,n;
  struct sockaddr_in serverData = {0,0,0,""};
  struct hostent *hp;

  char buffer[256];
  
  // Creating socket
  if ( (iSock=socket(AF_INET,SOCK_STREAM,0)) == -1 ){
    perror("socket");
    exit(1);
  }

  // Getting the host of the server. 
  if( (hp=gethostbyname(SERVER_ADDR)) == NULL ){
    perror("Cannot get host\n");
    shutdown(iSock,SHUT_RDWR);
    close(iSock);
    exit(1);
  }
  
  // Making the server identifier. 
  memcpy( (char*)&serverData.sin_addr, (char*)hp->h_addr, hp->h_length );
  serverData.sin_family=AF_INET;
  serverData.sin_port=htons(SERVER_PORT);
  
  // Begin the connection
  if ( connect(iSock,(struct sockaddr*)&serverData, sizeof(serverData)) == -1 ){
    perror("connect");
    shutdown(iSock,SHUT_RDWR);
    close(iSock);
    exit(1);
  }

  while(1) {

    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
   
    /* Send message to the server */
    n = write(iSock,buffer,strlen(buffer));
   
    if (n < 0)
      {
	perror("ERROR writing to socket");
	exit(1);
      }

    if(strncmp(buffer,"quit",4)==0) break; 
   
    /* Now read server response */
    bzero(buffer,256);
    n = read(iSock,buffer,255);
  
    if (n < 0)
      {
	perror("ERROR reading from socket");
	exit(1);
      }
    printf("msg from server:%s\n",buffer);

  }

  // End of the connection
  shutdown(iSock,SHUT_RDWR);
  close(iSock);
  
  return 0;
}
