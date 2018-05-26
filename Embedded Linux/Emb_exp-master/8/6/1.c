#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSGSIZE 100
#define KEYFILE_PATH  "keyfilepath"
#define ID 'M'
#define MSGQ_OK 0
#define MSGQ_NG -1

int main(void)
{
  struct msgbuff{
    long mtype;
    char mtext[MSGSIZE];
  }message;

  int           msqid;
  key_t         keyx;
  struct msqid_ds msq;
  
  printf("processA(MessageReceive)  : start\n");

  keyx = ftok(KEYFILE_PATH, (int)ID);

  msqid = msgget(keyx, 0666 | IPC_CREAT);

  if (msqid == -1) {
    fprintf(stderr, "msgget failed with error: %d\n", errno);
    exit(EXIT_FAILURE);
  }

  printf("processB(MessageSend)   : msqID = %d\n", msqid);

  //Receive message
  if((msgrcv(msqid, &message, sizeof(message.mtext), 1, 0)) ==
     MSGQ_NG){
    perror("msgrcv:processA(MessageReceive)");
    exit(1);
  }
  printf("processA(MessageReceive)  : received message = %s\n", message.mtext);

  //Control of message queue (message queue ID deletion)
  if(msgctl(msqid, IPC_RMID, NULL) == MSGQ_NG){
    perror("msgctl:processA(MessageReceive)");
  }

  printf("processA(MessageReceive)  : end\n");
  return 0;
}
