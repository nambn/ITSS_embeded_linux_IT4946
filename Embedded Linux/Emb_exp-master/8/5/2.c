#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#define SHIMSIZE 1024
#define KEYFILE_PATH "keyfilepath"
#define PROJ_CHAR    'S'

int main()
{
  int i;
  int shm_id;
  pthread_mutex_t *pm;

  printf ("writer started.\n");

  key_t shm_key = ftok( KEYFILE_PATH,(int)PROJ_CHAR);
  const int shm_size = SHIMSIZE;

  /* Allocate a shared memory segment. */
  shm_id = shmget (shm_key, shm_size, IPC_CREAT | S_IRUSR | S_IWUSR);

  /* Attach the shared memory segment. */
  pm = (pthread_mutex_t *) shmat (shm_id, 0, 0);

  //while(pthread_mutex_trylock(pm) == EBUSY);// Lock mutex
  if (pthread_mutex_lock(pm) != 0) {
    perror("pthread_mutex_lock");
    exit(1);
  }

  printf("mutex lock\n");

  // Unlock mutex
  if (pthread_mutex_unlock(pm) != 0) {
    perror("pthread_mutex_unlock");
    exit(1);
  }
  printf("mutex unlock\n");

  /* Detach the shared memory segment. */
  shmdt (pm);
  /* Deallocate the shared memory segment.*/
  shmctl (shm_id, IPC_RMID, 0);

  return 0;
}
