#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#define SHIMSIZE 256

#define MSGSIZE 100
#define KEYFILE_PATH  "keyfilepath"
#define ID 'M'
#define MSGQ_OK 0
#define MSGQ_NG -1

int main(void)
{
  struct msgbuff{
    long mtype;
    int num;
  }message;

  int       i;
  int       shmsize;
  int       shmid;
  int       *ptr;  
  int       *head;
  int       value;

  int           msqid;
  key_t         keyx;
  struct msqid_ds msq;

  pthread_mutex_t *pm = malloc(sizeof(pthread_mutex_t));
  pthread_mutexattr_t pmattr;

  keyx = ftok(KEYFILE_PATH, (int)ID);

  msqid = msgget(keyx, 0666 | IPC_CREAT);

  if (msqid == -1) {
    perror("msgget");
    exit(1);
  }

  shmsize = SHIMSIZE;

  //Get the shared memory ID
  if ((shmid = shmget(keyx,shmsize*sizeof(int),IPC_CREAT|0660)) == -1){
    perror("shmget");
    exit(1);
  }

  printf("shmid: %d\n",shmid);
  
  //Attach the shared memory
  head = ptr = (int*)shmat(shmid,0,0);
  if (head == (int*)-1)
  {
    perror("shmat");
    exit(1);
  }

  *ptr = 0;

  printf("Address of shared memory: %p\n", ptr);

  // Initialize the mutex attribute object
  pthread_mutexattr_init(&pmattr);

  // Set mutex attribute(Setting for using mutex to inter process)
  if (pthread_mutexattr_setpshared(&pmattr, PTHREAD_PROCESS_SHARED) != 0) {
    perror("pthread_mutexattr_setpshared");
    exit(1);
  }
  // Initialize mutex
  pthread_mutex_init(pm, &pmattr);

  //fork();

  //Receive message
  while(1) {
    if((msgrcv(msqid, &message, sizeof(int), 1, 0)) ==
       MSGQ_NG){
      perror("msgrcv");
      exit(1);
    }
    printf("input value = %d\n", message.num);

    // Lock mutex
    if (pthread_mutex_lock(pm) != 0) {
      perror("pthread_mutex_lock");
      exit(1);
    }

    printf("mutex lock\n");

    printf("Value of shared memory :(pid=%d) %d\n", getpid(),*ptr);

    sleep(5);

    *ptr = *ptr + message.num;
  
    printf("Value of shared memory :(pid=%d) %d\n", getpid(),*ptr);

    // Unlock mutex
    if (pthread_mutex_unlock(pm) != 0) {
      perror("pthread_mutex_unlock");
      exit(1);
    }
    printf("mutex unlock\n");

    message.mtype = 1;
    message.num = *ptr;
    if((msgsnd(msqid, (void*)&message, sizeof(int), 0)) ==MSGQ_NG){
      perror("msgsnd");
      exit(1);
    }
  }

  //Control of message queue (message queue ID deletion)
  if(msgctl(msqid, IPC_RMID, NULL) == MSGQ_NG){
    perror("msgctl");
  }

  return 0;
}
