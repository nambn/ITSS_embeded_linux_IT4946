#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROC_OK  0
#define PROC_NG  -1

/**
 * The main processing
 */
int main(void)
{
  printf("main           : start\n");

  char *args[] = {"ls", "-l", NULL};
  int iRet = 0;
  pid_t pid = 0;
  int  i, status = 0;

  // Creating child process
  pid = fork();
  if(pid == 0) pid = fork();
  switch (pid){
    case -1:
      // Process creating error processing
      perror("processGenerate fork");
      exit(1);
    case 0:
      // Child process processing
      printf("Do you want to kill child process %d? (y/n)", getpid());
      char c = getchar();
      if(c == 'y' || c == 'Y') {
        printf("Kill process pid=%d\n", getpid());
        char *cmd;
        sprintf(cmd, "kill %d", getpid());
        system(cmd);
        //break;
      }
      iRet = execv("/bin/ls", args);

      // It doesn't pass when execv succeeds. 
      printf("execv ret = %d\n", iRet);
      if(iRet == PROC_NG)
      {
        exit(1);
      }

    default:
      // Parent process processing
      printf("Parent Process ChildProcPid = %d\n", pid);

      // Wait for the state transition of the child process. 
      pid = wait(&status);
      printf("pid=%d,status=%d\n", pid, status);
      break;
  }

  printf("main           : end\n");
  return 0;
}
