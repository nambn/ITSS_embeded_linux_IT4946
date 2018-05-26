#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHIMSIZE 256
#define KEYFILE_PATH "keyfilepath"
#define PROJ_CHAR    'S'

int main()
{
  int       i;
  int       shmsize;
  int       shmid;
  key_t     keyval;
  char      *ptr;  
  char      *head;

  keyval = ftok( KEYFILE_PATH,(int)PROJ_CHAR);
  shmsize = SHIMSIZE;

  //Get the shared memory ID
  if ((shmid = shmget(keyval,shmsize,IPC_CREAT|0660)) == -1){
    perror("shmget");
    exit(1);
  }

  printf("shmid: %d\n",shmid);
  
  //Attach the shared memory
  head = ptr = (char*)shmat(shmid,0,0);
  if (head == (char*)-1)
  {
    perror("shmat");
    exit(1);
  }

  printf("Address of shared memory: %p\n", ptr);

  //Write to the shared memory
  char c ;
  while(1){
    c = getchar();
    *ptr = c;
    if(*ptr++ == 'q') break;
  }

  //Detach the shared memory
  if( shmdt((void*)head ) == -1){
    perror("shmdt");
    exit(1);
  }
  //Delete the shared memory
  if( shmctl(shmid,IPC_RMID,0) == -1 ){
    perror("shmctl");
    exit(1);
  }

  return 0;
}
